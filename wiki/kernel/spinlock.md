# spinlock 自旋鎖

## 核心概念

`spinlock_t` 是 Linux kernel 中**最短、最輕、唯一能在 atomic context 使用**的鎖。取鎖時不讓出 CPU，而是以 busy-wait（自旋）等待；一旦拿到鎖，被保護的 critical section **絕對不可睡眠**。

### 一句話判斷

> 持鎖時間極短（< 數十個 cycles）、不能睡眠、需要在 atomic context（IRQ / softirq / preempt-off / RCU read side）使用時 → 選 spinlock。其他情況優先 mutex。

---

## 使用時機

### ✅ 必須用 spinlock 的場景

1. **Interrupt handler 內**（hard IRQ、softirq、tasklet、timer callback）
   - 中斷上下文不能睡眠，mutex 會睡，所以只剩 spinlock。

2. **保護非常短的 critical section**
   - 更新一個 counter、從 list 摘一個節點、改一個 flag。
   - 「忙等」比「schedule out」便宜。

3. **會同時被 process context 與 IRQ context 存取的資料**
   - process 側必須用 `spin_lock_irqsave()`，否則 IRQ 進來時搶同一把鎖會 self-deadlock。

4. **已經在 atomic context**（`in_atomic()` 為真）
   - spinlock 內、RCU read-side、`preempt_disable()`、`local_bh_disable()` 區段。
   - 沒得選，只能 spinlock；且被保護區同樣禁止睡眠。

5. **SMP 間保護，持鎖時間夠短**
   - UP kernel 上 spinlock 會編成 no-op（或僅關 preempt），幾乎零成本。

### ❌ 不要用 spinlock 的場景

- **critical section 可能睡眠** → 用 mutex。
  - `kmalloc(GFP_KERNEL)`、`copy_from_user`、`mutex_lock`、`wait_event`、`msleep`、任何 I/O。
  - 這正是 `[[interview/weekly-mock-bank]]` Mon-3 Bug #4 的考點。
- **critical section 很長**（> 幾微秒） → mutex 更便宜，其他 CPU 不必空轉。
- **讀多寫少** → 考慮 `rwlock_t`、`seqlock` 或 **RCU**。
- **只是一個計數器** → `atomic_t` / `refcount_t` / `percpu_counter` 通常更快。

---

## 三種變體選擇

| 變體 | 適用 | 關閉 | 成本 |
|------|------|------|------|
| `spin_lock()` / `spin_unlock()` | 只有 process context 競爭，資料不會在 IRQ/softirq 被動到 | preempt | 最低 |
| `spin_lock_bh()` / `spin_unlock_bh()` | 會與 softirq / tasklet 競爭（網路收包路徑常見） | preempt + softirq | 中 |
| `spin_lock_irqsave(lock, flags)` / `spin_unlock_irqrestore(lock, flags)` | 會與 hard IRQ 競爭，或不確定是否在 IRQ-off 狀態被呼叫 | preempt + local IRQ | 最高（但最保險） |

口訣：**看你要擋住誰**。
- 擋 process → `spin_lock()`
- 擋 softirq → `spin_lock_bh()`
- 擋 hard IRQ → `spin_lock_irqsave()`

另有 `spin_lock_irq()` —— 進入時直接關 local IRQ，退出時直接開；僅在「你確知進入時 IRQ 原本是開的」才用，否則會把呼叫者原本關掉的 IRQ 一併打開，造成 race。

---

## 標準使用範式

### 靜態初始化

```c
static DEFINE_SPINLOCK(my_lock);                // 編譯期初始化完成
```

### 動態初始化

```c
struct foo {
    spinlock_t lock;
    /* ... */
};

static int foo_init(struct foo *f)
{
    spin_lock_init(&f->lock);                   // 忘了這行 → lockdep 亂噴 / UP 下隨機 crash
    return 0;
}
```

### 一般用法（process context only）

```c
spin_lock(&my_lock);
/* 超短 critical section：不可 sleep、不可 kmalloc(GFP_KERNEL) */
list_add(&obj->node, &head);
spin_unlock(&my_lock);
```

### 與 IRQ handler 共享資料

```c
unsigned long flags;

spin_lock_irqsave(&my_lock, flags);             // 保存 IRQ 狀態並關本地 IRQ
/* critical section */
spin_unlock_irqrestore(&my_lock, flags);        // 還原 IRQ 狀態（不是無條件 enable）
```

### 與 softirq 共享資料（典型：網路路徑）

```c
spin_lock_bh(&my_lock);
/* critical section */
spin_unlock_bh(&my_lock);
```

---

## 常見地雷（面試高頻）

1. **持 spinlock 時呼叫會睡的函式**
   - `kmalloc(GFP_KERNEL)`、`copy_from_user`、`mutex_lock`、`msleep`、`schedule()`。
   - 在 `CONFIG_DEBUG_ATOMIC_SLEEP` 下會噴 `BUG: sleeping function called from invalid context`。
   - **修法**：移出 critical section，或改 `GFP_ATOMIC`（但 pool 有限、失敗率高）。

2. **忘了用 `_irqsave`，而該鎖也會在 IRQ handler 取得**
   - 單 CPU 上 IRQ 重入立即 deadlock。

3. **`spin_unlock_irqrestore` 的 flags 用錯變數**
   - 每次 save/restore 必須配對同一個 `unsigned long flags`，不可跨函式共用。

4. **鎖順序不一致（AB–BA deadlock）**
   - lockdep 會抓；但 review 時也要目視檢查所有取鎖點的順序。

5. **critical section 過長**
   - 其他 CPU 在那邊燒電；整體延遲變差。寫 kernel 時永遠先問「能不能縮？能不能移出去？」

6. **動態鎖忘了 `spin_lock_init()`**
   - UP 下可能「剛好」能跑，SMP 或 lockdep 開起來就爆炸。

7. **spinlock 不可遞迴**
   - 同一把 lock 被同一 CPU / thread 連取兩次立即 self-deadlock。需要遞迴語義請另尋他法（重構 / 分層鎖 / mutex）。

---

## 與其他同步原語的對比

| 原語 | 可睡嗎 | 適用 | 備註 |
|------|-------|------|------|
| `spinlock_t` | **否** | 短 critical section、atomic context | 本篇 |
| `mutex` | 是 | 長 critical section、process context | kernel 預設選項 |
| `rwlock_t` | 否 | 讀多寫少（短） | 現代已多被 RCU 取代 |
| `rw_semaphore` | 是 | 讀多寫少（長） | 例：`mmap_lock` |
| `seqlock` | 讀者不睡、寫者短 | 寫罕見、讀要低延遲、允許 retry | 時間子系統、`jiffies` |
| **RCU** | 讀者零鎖 | 幾乎只讀的結構 | 路由表、dcache、VFS |
| `atomic_t` / `refcount_t` | — | 單一變數 | 比 spinlock 便宜非常多 |
| `percpu_counter` | — | 高頻計數 | 近零競爭 |

---

## 實戰：在 kernel 源碼中的定位

### 典型用例

- **`include/linux/list.h`** — `list_add` / `list_del` 本身不帶鎖，caller 需自行用 spinlock 或其他手段保護（`list_for_each_safe` 搭配 spinlock 是常見搭配）。
- **`kernel/sched/core.c`** — `rq->lock`（runqueue spinlock），持鎖必須 ≤ 數十奈秒。
- **`net/ipv4/tcp_input.c`** — socket 的 `bh_lock_sock()`，正是 `spin_lock_bh()` 的包裝。
- **`mm/slub.c`** — per-cpu cache 操作時用 `local_lock` / spinlock 保護 freelist。

### Lockdep：寫 kernel 的最佳朋友

開啟 `CONFIG_PROVE_LOCKING` 後，lockdep 會即時追蹤：
- 取鎖順序是否一致（AB–BA）
- 是否在 IRQ-unsafe 鎖裡呼叫 IRQ-safe 鎖（會 deadlock）
- 是否在 atomic context 睡眠
- spinlock 是否跨 schedule

Review 任何 spinlock 代碼時，**先問自己 lockdep 噴什麼**往往是最快找到 bug 的方式。

---

## 相關 wiki

- `[[interview/google_code_review_semaphore_prep_linux_primitives]]` — **5 大原語選擇邏輯總圖**（mutex / spinlock / semaphore / completion / rwsem）；spinlock 在「ISR 共享 list」「bitmap 池」場景的 圖解對照
- `[[interview/google_code_review_semaphore_prep_linux_solution]]` — AP-DSP IPC driver 6 題修正解答；題目 1 `list_lock`（spinlock_irqsave）、題目 2 channel bitmap（spinlock）的 idiomatic 用法
- `[[interview/weekly-mock-bank]]` — Mon-3 Code Review Round 的 Bug #4（spinlock + `GFP_KERNEL`）
- `[[interview/Mon-3-code-review-solution]]` — 上題逐行繁中註解 + 修正解答（`spin_lock_irqsave` 範式示範）
- `[[interview/code-review-questions]]` — 主管視角 code review 清單（§鎖與同步章節）
- `[[kernel/list-head]]` — 搭配 `list_for_each_safe` 的鎖使用慣例
- `[[kernel/memory-management]]` — `GFP_KERNEL` vs `GFP_ATOMIC` 的 context 差異

---

## 面試速記（背誦版）

1. spinlock = busy-wait + **禁止睡眠** + atomic context 可用。
2. 要擋 process 用 `spin_lock()`；擋 softirq 用 `_bh()`；擋 hard IRQ 用 `_irqsave()`。
3. 持鎖時若需分配記憶體：**移出 critical section** > 改 `GFP_ATOMIC`。
4. 分享出去的指標配 refcount；釋放路徑在鎖外。
5. lockdep / `CONFIG_DEBUG_ATOMIC_SLEEP` 是 debug 的第一站。
6. 長 critical section、讀多寫少、單變數 → 換 mutex / RCU / atomic_t。
