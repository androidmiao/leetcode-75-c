# Google Code Review 面試準備：Semaphore 專題

> **時間：剩 1 天**
> **語言：C (POSIX `<semaphore.h>` + `<pthread.h>`)**
> **格式：Google 常見的 code review 面試 = reviewer 給你一段含 bug 的 code，你要在 30~45 分鐘內：(1) 找出所有問題 (2) 講清楚為什麼錯 (3) 提出可上線的修正版**

---

## 0. 心法：Google reviewer 在意的四個層次

每看一段 code，都要過一次這個 checklist：

1. **Correctness（正確性）**：race condition、deadlock、signal lost、starvation、ABA、memory ordering。
2. **Robustness（韌性）**：error path 有沒有 leak？`sem_wait` 被 signal 中斷（`EINTR`）有沒有處理？初始化失敗呢？
3. **Readability（可讀性）**：semaphore 名字說不說得出「在保護什麼」？`P/V` 配對是不是在同一層 scope 一眼看得出來？
4. **Performance / Scalability**：有沒有不必要的 contention？是不是該用 mutex 卻硬用 semaphore？counting semaphore 的上限對不對？

> **面試小技巧**：每個 bug 都用「症狀 → 根因 → 修法 → 為什麼這修法是對的」四段式講。Google L4/L5 reviewer 最在意的是你會不會 reason about concurrency，而不是會不會「猜到」答案。

---

## 1. Semaphore 概念速查

| 用途 | Semaphore 初值 | P (sem_wait) | V (sem_post) |
|---|---|---|---|
| Mutex（互斥）| 1 | 進臨界區 | 離臨界區 |
| Counting（資源池）| N | 取一個資源 | 還一個資源 |
| Signaling（事件通知）| 0 | 等事件 | 通知事件 |
| Bounded buffer 空格數 | N | 寫之前 | 讀之後 |
| Bounded buffer 已用格 | 0 | 讀之前 | 寫之後 |

**口訣**：counting semaphore 不是 mutex。**它沒有 owner 概念**——任何 thread 都能 `sem_post`，所以拿它當 mutex 用會錯過 priority inheritance、會被搞錯配對。

---

## 2. 六題 Code Review 模擬

### 題目 1：Connection Pool（系統設計延伸：Web Crawler / API Gateway）

**情境**：reviewer 說「這是我們新 service 的 DB connection pool，限制最多 10 條 connection。請你 review。」

```c
// connection_pool.c  -- VERSION UNDER REVIEW
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define POOL_SIZE 10

static Connection *pool[POOL_SIZE];
static int          next_free = 0;
static sem_t        slots;

void pool_init(void) {
    sem_init(&slots, 0, POOL_SIZE);
    for (int i = 0; i < POOL_SIZE; i++) {
        pool[i] = db_connect();
    }
}

Connection *pool_acquire(void) {
    sem_wait(&slots);
    Connection *c = pool[next_free];
    next_free = (next_free + 1) % POOL_SIZE;
    return c;
}

void pool_release(Connection *c) {
    next_free = (next_free - 1 + POOL_SIZE) % POOL_SIZE;
    pool[next_free] = c;
    sem_post(&slots);
}
```

<details>
<summary>👉 你要在 review 中指出什麼（先自己想 5 分鐘再展開）</summary>

**Bug 1：`next_free` 不是 atomic，多 thread 同時 acquire 會 race。**
- 症狀：兩個 thread 拿到同一條 connection，第三個 thread 拿到 NULL。
- 根因：semaphore 只控「能不能進來」，沒控「進來後操作的共享變數」。Counting semaphore ≠ mutex。
- 修法：再加一把 `pthread_mutex_t pool_lock;`，包住對 `pool[]` 與 `next_free` 的存取。

**Bug 2：`pool_release` 假設 caller 釋放順序 = acquire 順序，這是錯的。**
- 真實情況：thread A 先拿 conn[0]，thread B 後拿 conn[1]，A 慢、B 快，B 先 release。`next_free` 算錯，conn 會錯位甚至覆蓋。
- 修法：用 free list（linked list 或 stack），`pool_release` 把 connection 推回 free list 頂端，`pool_acquire` 從頂端取。

**Bug 3：`sem_wait` 沒處理 `EINTR`。**
- 症狀：被 signal 中斷時，`sem_wait` 返回 -1，code 卻當成成功，回傳髒資料。
- 修法：

```c
while (sem_wait(&slots) == -1 && errno == EINTR) { /* retry */ }
```

**Bug 4：`sem_init` / `db_connect` 回傳值沒檢查。**
- 初始化失敗時 pool 是 garbage，後面全部 UB。

**Bug 5：沒有 `pool_destroy`。** Long-running service OK，但短任務或 unit test 會 leak。

**Bug 6（架構級，加分題）：counting semaphore 的初值應該等於「實際成功建立的 connection 數」，不是 `POOL_SIZE`。** 如果有 3 條 `db_connect` 失敗，還允許 10 個 thread 進來，會拿到 NULL connection。

---

**修正版骨架（你可以白板上寫這個）：**

```c
typedef struct {
    Connection *conns[POOL_SIZE];
    int         top;          // index of next available
    sem_t       slots;        // counting: how many free
    pthread_mutex_t lock;     // protects conns[] and top
} Pool;

int pool_init(Pool *p) {
    int built = 0;
    for (int i = 0; i < POOL_SIZE; i++) {
        Connection *c = db_connect();
        if (c) p->conns[built++] = c;
    }
    if (built == 0) return -1;
    p->top = built;
    if (sem_init(&p->slots, 0, built) != 0) return -1;
    if (pthread_mutex_init(&p->lock, NULL) != 0) return -1;
    return 0;
}

Connection *pool_acquire(Pool *p) {
    while (sem_wait(&p->slots) == -1) {
        if (errno != EINTR) return NULL;
    }
    pthread_mutex_lock(&p->lock);
    Connection *c = p->conns[--p->top];
    pthread_mutex_unlock(&p->lock);
    return c;
}

void pool_release(Pool *p, Connection *c) {
    pthread_mutex_lock(&p->lock);
    p->conns[p->top++] = c;
    pthread_mutex_unlock(&p->lock);
    sem_post(&p->slots);     // 注意：post 在 unlock 後，避免 post 後立刻有人搶 lock
}
```

**Reviewer 想聽你解釋的順序**：先講「semaphore 管 capacity，mutex 管 data structure 一致性」這個分工，再講為什麼 `sem_post` 必須在 `unlock` 之後（否則 wake 起來的 thread 還要等 lock，浪費一輪 context switch）。

</details>

---

### 題目 2：Producer-Consumer Bounded Buffer

**情境**：「這是我們 log shipper 的 ring buffer，producer 是各個 worker thread，consumer 是 flusher thread。」

```c
#define N 1024
static Item   buf[N];
static int    in = 0, out = 0;
static sem_t  empty;     // init N
static sem_t  full;      // init 0
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void produce(Item x) {
    pthread_mutex_lock(&m);
    sem_wait(&empty);
    buf[in] = x;
    in = (in + 1) % N;
    sem_post(&full);
    pthread_mutex_unlock(&m);
}

Item consume(void) {
    pthread_mutex_lock(&m);
    sem_wait(&full);
    Item x = buf[out];
    out = (out + 1) % N;
    sem_post(&empty);
    pthread_mutex_unlock(&m);
    return x;
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（致命，deadlock）：`mutex_lock` 在 `sem_wait` 之前。**
- Buffer 滿時，producer 拿著 mutex 卡在 `sem_wait(&empty)`。Consumer 想拿 mutex 進來消費，拿不到 → 永遠 deadlock。
- **黃金法則：永遠先 `sem_wait`（資源類 semaphore）再 `mutex_lock`。**

**修法**：

```c
void produce(Item x) {
    while (sem_wait(&empty) == -1 && errno == EINTR) {}
    pthread_mutex_lock(&m);
    buf[in] = x;
    in = (in + 1) % N;
    pthread_mutex_unlock(&m);
    sem_post(&full);
}
```

**Bug 2：single producer / single consumer 其實不需要 mutex。** 多 producer / 多 consumer 才需要。Reviewer 會問你「這個 service 是幾對幾」，這時候你要反問需求，而不是直接砍 mutex——這是 Google 在意的「don't optimize without measuring」。

**Bug 3：沒有 shutdown 機制。** Service stop 時 consumer 卡在 `sem_wait(&full)` 永遠不會醒。修法：加一個 `atomic_bool stopping`，shutdown 時先 set flag 再 `sem_post(&full)` 把 consumer 喚醒，consumer 醒來檢查 flag 決定要不要結束。

</details>

---

### 題目 3：Rate Limiter（用 semaphore 做 token bucket）

**情境**：「我們要限制對外部 API 每秒最多 100 個 request。我寫了這個 rate limiter。」

```c
static sem_t tokens;

void *refiller(void *arg) {
    while (1) {
        sleep(1);
        for (int i = 0; i < 100; i++) sem_post(&tokens);
    }
    return NULL;
}

void rate_limiter_init(void) {
    sem_init(&tokens, 0, 100);
    pthread_t t;
    pthread_create(&t, NULL, refiller, NULL);
}

void api_call(void) {
    sem_wait(&tokens);
    do_actual_call();
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（最重要）：token 數會無限累積。**
- 如果 1 秒內沒人用，下一秒 refiller 又 post 100 個，semaphore 變成 200。再過一陣子變成 1000、10000。最後爆發流量時瞬間放出 10000 個 request，整個 rate limit 形同虛設。
- 根因：POSIX semaphore 沒有「上限」概念（不像 Java `Semaphore` 也沒有，但你要自己 cap）。
- **修法**：refiller 裡先讀目前值（`sem_getvalue`），只 post 到「補滿到 100」為止：

```c
void *refiller(void *arg) {
    while (1) {
        sleep(1);
        int cur;
        sem_getvalue(&tokens, &cur);
        for (int i = cur; i < 100; i++) sem_post(&tokens);
    }
    return NULL;
}
```

但 reviewer 會立刻問你：「`sem_getvalue` 跟 `sem_post` 之間不是 atomic，會不會 race？」答：會，可能 over-post 幾個。要嚴格的話用 mutex + condition variable + counter 自己實作，或在 refiller 旁邊加一把 mutex 把 getvalue/post 包住，並讓 `api_call` 的拿 token 也走同一把鎖（但這樣就退化成普通 mutex + counter，semaphore 沒優勢了）。

**Bug 2：`sleep(1)` 不準確且會 drift。** 應該用 `clock_gettime(CLOCK_MONOTONIC)` + 計算下一次喚醒時間。

**Bug 3：`pthread_create` 沒檢查 return value，refiller thread 沒 detach 也沒 join → resource leak。**

**Bug 4（架構級）：burst 很糟。** 這是 fixed window，不是 token bucket。真正的 token bucket 應該每 10ms refill 1 個（或用 lazy 計算：在每次 `api_call` 時根據時間差算可用 token），這樣 burst 行為平滑。

**反問 reviewer 的問題**：這 service 是 single process 嗎？如果是 multi-process / 跨機器，semaphore 根本不夠，要用 Redis 或 distributed token bucket。Google 面試很愛你問這種「scope」的問題。

</details>

---

### 題目 4：Reader-Writer 用 semaphore 實作

**情境**：「我用 semaphore 寫了個 read-write lock，多 reader 可同時讀，writer 互斥。」

```c
static int    reader_cnt = 0;
static sem_t  rw;          // init 1
static sem_t  cnt_lock;    // init 1, 保護 reader_cnt

void read_lock(void)  {
    sem_wait(&cnt_lock);
    reader_cnt++;
    if (reader_cnt == 1) sem_wait(&rw);
    sem_post(&cnt_lock);
}
void read_unlock(void) {
    sem_wait(&cnt_lock);
    reader_cnt--;
    if (reader_cnt == 0) sem_post(&rw);
    sem_post(&cnt_lock);
}
void write_lock(void)   { sem_wait(&rw); }
void write_unlock(void) { sem_post(&rw); }
```

<details>
<summary>👉 答案</summary>

**Bug 1（最關鍵）：writer starvation。**
- 只要 reader 一直來，`reader_cnt` 永遠 ≥ 1，writer 永遠拿不到 `rw`。
- 這是經典「reader-preference」實作，Google reviewer 一定會問你：「這個對我們的 workload 適合嗎？」如果是 metadata service（write-heavy），會被 starve 到死。
- 修法：加第三個 semaphore `turnstile`，writer 先把它鎖住，新進 reader 全卡住，等已在讀的 reader 排空後 writer 進入。這是 writer-preference 或 fair 版本。

**Bug 2：`read_lock` 裡面 `sem_wait(&rw)` 是在持有 `cnt_lock` 的情況下做的。**
- 如果 writer 持有 `rw`，第一個 reader 拿著 `cnt_lock` 卡在 `sem_wait(&rw)`，後續 reader 連 `cnt_lock` 都拿不到 → 不只是慢，是後面 reader 連「我想讀」這個動作都做不了。
- 嚴格來說功能對，但 contention 會放大。較好的寫法是用 condition variable（`pthread_cond_t`）配 mutex，因為 cv 可以在「條件不滿足」時 atomically 釋放 mutex 等待。
- **更好的回答：直接建議用 `pthread_rwlock_t`。** 自己造輪子在 production code review 是大紅旗。Reviewer 想聽你說「除非有測過 `pthread_rwlock_t` 不夠用，否則不要自己刻」。

**Bug 3：error handling 全缺。**

</details>

---

### 題目 5：Fork-Join Worker（signaling semaphore）

**情境**：「main thread 派發 N 個 task 給 worker pool，等所有 task 做完再繼續。」

```c
static sem_t done;

typedef struct { int id; } Task;

void *worker(void *arg) {
    Task *t = arg;
    do_work(t);
    sem_post(&done);
    free(t);
    return NULL;
}

void run_all(int n) {
    sem_init(&done, 0, 0);
    for (int i = 0; i < n; i++) {
        Task *t = malloc(sizeof(Task));
        t->id = i;
        pthread_t th;
        pthread_create(&th, NULL, worker, t);
        pthread_detach(th);
    }
    sem_wait(&done);   // 等完成
    sem_destroy(&done);
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（致命）：只 `sem_wait` 一次，但有 N 個 worker。**
- main thread 拿到第一個完成的 signal 就繼續了，剩下 N-1 個 worker 還在跑，可能 access 已經 destroyed 的 semaphore（UB / SIGSEGV）。
- 修法：

```c
for (int i = 0; i < n; i++) {
    while (sem_wait(&done) == -1 && errno == EINTR) {}
}
```

**Bug 2：`pthread_detach` + `sem_destroy` 是 race。**
- detached thread 在 `sem_post` 前可能還沒跑到。即使你迴圈 wait 完 N 次，`sem_post` 之後 worker 還有 `free(t); return NULL;`——理論上 OK，但若 worker 還碰 `done`（例如 logging），就 UAF。
- 更好的做法：不用 detach，main 收集 thread id，最後 `pthread_join` 每個。或者用 `pthread_barrier_t`，這 case 它更貼切。

**Bug 3：`malloc(sizeof(Task))` 沒檢查 NULL。**

**Bug 4：`pthread_create` 沒檢查 return value——Google style guide 對 syscall return 非常嚴格。**

**Reviewer 加分問題**：「你會用 semaphore 還是 `pthread_barrier_t` 還是 `atomic_int` + cv？」答：N 已知 → barrier；N 動態 → atomic counter + cv（worker 做完 `--remaining`，到 0 broadcast）；要極簡且對 init/destroy ordering 有把握 → semaphore。

</details>

---

### 題目 6：Signal-safety（系統程式設計題，Google L5+ 常考）

**情境**：「我在 SIGTERM handler 裡通知 main loop 該結束。」

```c
static sem_t shutdown_sig;

void on_sigterm(int sig) {
    sem_post(&shutdown_sig);
}

int main(void) {
    sem_init(&shutdown_sig, 0, 0);
    signal(SIGTERM, on_sigterm);
    main_loop_until(&shutdown_sig);
}
```

<details>
<summary>👉 答案</summary>

**重點 1（會考你「為什麼這段沒錯」）：`sem_post` 是 POSIX 規定的 async-signal-safe 函式之一。**
- 這跟 `pthread_cond_signal` / `pthread_mutex_lock` 形成對比——後兩者**不是** signal-safe，在 handler 裡用是 UB。
- 這也是少數應該用 semaphore 而不是 cv 的場合。Reviewer 會故意挑你這個——能講出「為什麼這裡 semaphore 比 cv 好」就是 senior signal。

**Bug 1：應該用 `sigaction` 而不是 `signal`。**
- `signal()` 在不同 OS 行為不一致（System V vs BSD），handler 可能被 reset、`EINTR` 行為不定。`sigaction` 可控（`SA_RESTART`、mask）。

**Bug 2：`sem_init` 失敗沒檢查；register handler 失敗沒檢查。**

**Bug 3（細節）：handler 應該存 `errno` 進入、出去 restore。** 因為 handler 可能在某個 syscall 中間打斷，handler 內部任何呼叫如果動到 `errno`，外面 caller 看到的 `errno` 就被污染。

```c
void on_sigterm(int sig) {
    int saved = errno;
    sem_post(&shutdown_sig);   // 失敗時會動 errno
    errno = saved;
}
```

**Bug 4（深水區）：multiple SIGTERM 怎辦？**
- semaphore counter 會疊加。main loop 只 `sem_wait` 一次後就結束了，看似沒事。但若你 logic 是「shutdown 收到 N 次強制 kill」，要小心 counter 語意。

</details>

---

## 3. Google Code Review 高頻檢查清單（背起來）

### Concurrency 紅旗
- [ ] semaphore 用作 mutex（沒有 owner、不能 priority inherit）
- [ ] mutex 在 semaphore wait 之前（容易 deadlock）
- [ ] `sem_wait` 沒處理 `EINTR`
- [ ] `sem_post` / `sem_wait` 對數不平衡（error path 漏 post）
- [ ] semaphore 沒 cap（rate limiter / token bucket 經典坑）
- [ ] cv 在 signal handler（UB；應該用 semaphore）
- [ ] reader-writer 自己刻而不用 `pthread_rwlock_t`
- [ ] detach 之後 destroy 同步原語（race）
- [ ] global state 只有 semaphore 保護 capacity，沒 mutex 保護 data 一致性
- [ ] busy wait 卻其實該 block

### 通用 C 紅旗
- [ ] `malloc` / `pthread_create` / `sem_init` return value 沒檢查
- [ ] 用 `signal()` 而非 `sigaction()`
- [ ] error path leak（acquire 後 `return -1` 沒釋放）
- [ ] 無 shutdown / cleanup
- [ ] 共用全域變數沒標 `static`
- [ ] `volatile` 被誤用為「同步」（要用 `_Atomic` 或 memory barrier）
- [ ] 註解寫「what」而不是「why」

### 架構級紅旗（拿到 senior 分數）
- [ ] 只在單機 work，沒考慮 multi-process / distributed
- [ ] 沒 metric / log，無法 debug production 卡死
- [ ] 沒 unit test for concurrency（建議用 `ThreadSanitizer`）
- [ ] 把 policy（rate、pool size）hardcode 而不是 config

---

## 4. 面試現場流程模板（30 分鐘版）

```
0:00 - 0:03  通讀整段 code，先別找 bug，先問 reviewer：
              - 這 code 跑在哪？single process？
              - producer 幾個、consumer 幾個？
              - 預期 throughput / latency？
              - shutdown 路徑長什麼樣？

0:03 - 0:15  逐行掃，每行問三個問題：
              (a) 哪些 thread 會走這行？
              (b) 共享狀態被誰保護？
              (c) error / signal 來怎辦？
              發現 bug 就口頭 narrate：「這裡我擔心 X，因為當 thread A 在
              這行時 thread B 可能在...」

0:15 - 0:25  把找到的 bug 排優先序：
              correctness > robustness > readability > perf
              然後白板寫修正版骨架（不一定要 compile，但要對）。

0:25 - 0:30  反問 reviewer：
              - 這 code 是不是抽自 production？背景是什麼？
              - 我有沒有遺漏什麼 constraint？
              - 我建議我們補哪些 unit test / load test？
```

---

## 5. 最後一晚要做的事

1. 把六題 buggy code 抄到一個 `.c` 檔，**手動編譯 + `clang -fsanitize=thread`** 跑一次，親眼看 race。
2. 用 `man sem_overview` / `man pthread_mutex_lock` 把 `EINTR`、`EAGAIN`、`EDEADLK` 三個 errno 看一遍。
3. 對著鏡子 narrate 題目 1（connection pool）的 review 過程，限時 8 分鐘。能不打草稿口頭講完，就過了。
4. 睡飽。Concurrency 題拼的是 working memory，沒睡飽全完蛋。

祝你明天順利。
