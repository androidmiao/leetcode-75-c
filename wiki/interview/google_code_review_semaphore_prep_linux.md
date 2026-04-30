# Google Code Review 面試準備：AP-DSP IPC Driver × Semaphore

> **時間：剩 1 天**
> **題目背景**：原系統設計題是 **AP ↔ DSP 通訊 driver**（Linux kernel driver / Android embedded SoC 場景）
> **架構**：AP 與 DSP 透過 GB 級 SDRAM 共享、TCM 做 DSP 本地低延遲存取、Mailbox + IPI 做事件通知，IPI manager 管 65 個 channel
> **要實作**：`dsp_map_memory_region`、`dsp_ipi_init`、`dsp_mbox_probe`、`PLT init`、`Time Sync`、message dispatching
> **語言**：C，Linux kernel（`<linux/semaphore.h>`、`completion`、`mutex`、`spinlock`、`wait_queue`）

---

## 0. 為什麼這題的 code review 一定圍繞 Semaphore

AP-DSP IPC driver 是 semaphore 教科書案例，因為它同時有四種典型 semaphore 用法：

| 場景 | 同步原語 | 用途 |
|---|---|---|
| **限制同時送出的 IPI 請求數** | counting semaphore | 防止 mailbox HW FIFO 溢位 |
| **AP 等 DSP 回 ack** | `struct completion`（binary semaphore 的 idiomatic 寫法）| 一次性事件通知 |
| **65 個 IPI channel 的 slot 配置** | counting semaphore | 資源池 |
| **送 IPI 時序列化對 mbox register 的存取** | mutex 或 spinlock | 互斥 |
| **driver probe 與 firmware ready 的握手** | completion | startup barrier |
| **suspend/resume 時等所有 in-flight 請求結束** | semaphore drain pattern | shutdown |

> **Reviewer 心法**：每段 code 都問自己「這裡真的需要 semaphore 嗎？mutex / completion / spinlock 哪個更合適？」Google 在 kernel 面試特別在意的是「為什麼選這個原語、不選那個」——這是 senior signal。

---

## 1. Linux Kernel Semaphore vs POSIX 速查

| 操作 | POSIX (user) | Linux kernel |
|---|---|---|
| 宣告 | `sem_t s` | `struct semaphore s` |
| 初始化 | `sem_init(&s, 0, n)` | `sema_init(&s, n)` |
| P (acquire) | `sem_wait(&s)` | `down(&s)` |
| P 可中斷 | `sem_wait` + `EINTR` | `down_interruptible(&s)` ← **driver 必用** |
| P 不阻塞 | `sem_trywait(&s)` | `down_trylock(&s)` |
| P 帶 timeout | `sem_timedwait` | `down_timeout(&s, jiffies)` |
| V (release) | `sem_post(&s)` | `up(&s)` |
| **完成事件** | 用 `sem_t` init=0 | `struct completion` ← idiomatic |

**重點**：
- 在 kernel 裡，**永遠用 `down_interruptible`**，不用 `down`。`down` 不可中斷會造成 unkillable D-state process，是 kernel review 一級紅旗。
- **不要在 interrupt context（IRQ handler、softirq）呼叫 `down*()`**，會 sleep。在 IRQ 裡只能用 `spinlock` 或 `down_trylock`。
- **single-shot 完成通知用 `struct completion`**，不要用 `sema_init(&s, 0)`，後者語意不清而且在 reviewer 眼裡像 anti-pattern。

---

## 2. 六題 Code Review 模擬（全部圍繞這個 AP-DSP 題）

---

### 題目 1：`dsp_send_ipi()` — 送 IPI 並等 DSP 回應

**情境**：reviewer 說「這是我寫的同步 IPI send，AP thread 呼叫後會 block 直到 DSP 處理完 ack。」

```c
// dsp_ipi.c -- VERSION UNDER REVIEW
#include <linux/semaphore.h>
#include <linux/io.h>

#define MAX_INFLIGHT 4   // mbox HW FIFO depth

struct dsp_ipi_ctx {
    struct semaphore inflight;        // init MAX_INFLIGHT
    struct semaphore ack;             // init 0, used as "DSP done"
    void __iomem    *mbox_base;
};

static struct dsp_ipi_ctx ctx;

int dsp_ipi_init(struct platform_device *pdev) {
    sema_init(&ctx.inflight, MAX_INFLIGHT);
    sema_init(&ctx.ack, 0);
    ctx.mbox_base = devm_ioremap_resource(&pdev->dev, ...);
    return 0;
}

int dsp_send_ipi(u32 channel, void *msg, size_t len) {
    down(&ctx.inflight);
    memcpy_toio(ctx.mbox_base + channel * 64, msg, len);
    writel(1, ctx.mbox_base + IPI_TRIGGER);
    down(&ctx.ack);                    // wait DSP
    up(&ctx.inflight);
    return 0;
}

// 在 IPI ISR 裡
irqreturn_t dsp_ipi_isr(int irq, void *dev) {
    up(&ctx.ack);
    return IRQ_HANDLED;
}
```

<details>
<summary>👉 Reviewer 想聽你指出的問題（先想 5 分鐘再展開）</summary>

**Bug 1（致命）：用一個全域 `ack` semaphore 串接所有 in-flight 請求 → ack 會被張冠李戴。**
- 場景：thread A 送 channel 3，thread B 同時送 channel 7。DSP 先回 channel 7 的 ack，但是 `up(&ctx.ack)` 把 thread A 喚醒了。A 拿到 B 的 ack 結果，B 永遠等不到。
- 根因：counting semaphore 沒有「身份」概念，post 起來的不一定是對應的那個 wait。
- **修法**：每個請求自帶 `struct completion`，放在 per-request struct 裡。ISR 根據 mbox header 找出對應請求，呼叫 `complete(&req->done)`。

```c
struct dsp_ipi_req {
    u32                 channel;
    u32                 seq;
    struct completion   done;
    int                 result;
    struct list_head    node;
};

// send
struct dsp_ipi_req req;
init_completion(&req.done);
req.seq = atomic_inc_return(&ctx.seq);
spin_lock_irqsave(&ctx.lock, flags);
list_add_tail(&req.node, &ctx.pending);
spin_unlock_irqrestore(&ctx.lock, flags);
trigger_ipi(...);
wait_for_completion_interruptible_timeout(&req.done, msecs_to_jiffies(1000));

// ISR
struct dsp_ipi_req *r = lookup_by_seq(seq_from_mbox);
complete(&r->done);
```

**Bug 2：用 `down()` 而不是 `down_interruptible()`。**
- AP thread 卡在 mailbox 等 DSP，DSP 死掉時，user kill 不掉這個 process（D-state、unkillable）。
- 修法：`down_interruptible(&ctx.inflight)`，且必須處理 `-ERESTARTSYS` 回傳。

**Bug 3（致命，error path leak）：`down(&ctx.ack)` 之前如果 `memcpy_toio` 或 trigger 失敗，沒有 `up(&ctx.inflight)`，semaphore 永遠 -1。**
- 修法：error path 一律用 `goto out_release;` pattern。

**Bug 4：在 IRQ handler 裡呼叫 `up()`。**
- 嚴格說 `up()` 是 **可以** 在 IRQ context 用的（kernel/locking/semaphore.c 會用 `raw_spin_lock_irqsave` 保護），但 **`down*()` 絕對不行**。如果你混用得不小心，下次有人加一行 `down_trylock` 在 ISR 旁邊就炸了。
- 比較好的做法：ISR 只 schedule tasklet/workqueue，真正 `complete()` / `up()` 在 bottom half 做。對 latency 敏感的 IPI 才容許 ISR 直接 complete。

**Bug 5：`devm_ioremap_resource` 回傳值沒檢查 `IS_ERR()`。**

**Bug 6：沒有 timeout，DSP hang 住的話 AP thread 永遠卡。**
- 修法：`down_timeout(&ctx.inflight, msecs_to_jiffies(500))`，超時回 `-ETIMEDOUT`，並把 hardware 強制 reset / log warning。

**Bug 7（架構級）：`MAX_INFLIGHT = 4` hardcode，沒對應 device tree 的 mbox FIFO depth。** 換 SoC 就破。

**Reviewer 加分問題**：「為什麼這裡你建議用 `completion` 而不是 `semaphore(0)`？」
> 答：(a) 語意明確——讀 code 的人一看 `wait_for_completion` 就知道是「等一次性事件」，不是「拿資源」。(b) `completion` 的內部實作對 single-waiter case 有最佳化，`down/up` 在 sleeper queue 上每次都要 enqueue/dequeue。(c) `completion` 提供 `complete_all()`，配合 multi-waiter（比如 firmware boot done 通知所有等的人）很乾淨。

</details>

---

### 題目 2：`dsp_ipi_channel_alloc()` — 65 channel 的 slot 配置

**情境**：「IPI manager 有 65 個 channel slot，service 動態跟我要 channel，用完歸還。」

```c
// dsp_ipi_table.c
#define DSP_IPI_NR_CH 65

static DECLARE_BITMAP(ch_busy, DSP_IPI_NR_CH);
static struct semaphore ch_avail;   // init 65

int dsp_ipi_channel_alloc(void) {
    int ch;
    down(&ch_avail);
    for (ch = 0; ch < DSP_IPI_NR_CH; ch++) {
        if (!test_bit(ch, ch_busy)) {
            set_bit(ch, ch_busy);
            return ch;
        }
    }
    return -ENOSPC;   // unreachable?
}

void dsp_ipi_channel_free(int ch) {
    clear_bit(ch, ch_busy);
    up(&ch_avail);
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（race）：`test_bit` + `set_bit` 不是 atomic 對。**
- 雖然 `set_bit` 本身 atomic，但 `test_bit` 和 `set_bit` 中間另一個 thread 可能也看到同一 bit 是 0 → 兩個 thread 都拿到 channel 5。
- 修法：用 `test_and_set_bit()`（atomic compare-and-swap 那一族）：

```c
for (ch = 0; ch < DSP_IPI_NR_CH; ch++) {
    if (!test_and_set_bit(ch, ch_busy))
        return ch;
}
```

**Bug 2（subtle）：「unreachable 的 `return -ENOSPC`」其實是 reachable 的，但只有在 race 下才會觸發。**
- 場景：semaphore 允許你進來，但你進來時所有 bit 都被別 thread 搶走。
- 雖然 `test_and_set_bit` 修好了正確性，但你 `down` 拿了一個 token 卻沒 channel 拿，這個 token 沒還回去 → semaphore 漏 -1。
- 修法：error path 一定要 `up(&ch_avail)`。

**Bug 3：用 counting semaphore 配 bitmap 是 over-engineering。**
- semaphore 的存在是為了 block 等待——但你的設計裡 caller 是「能拿就拿，拿不到就回錯」。這時候 semaphore 沒幫忙，反而是雙倍簿記（semaphore 計數 + bitmap），兩邊很容易不一致。
- 更好的做法（任一）：
  - **不用 semaphore**：純 atomic bitmap + spinlock + `find_first_zero_bit`，alloc 失敗就回 `-ENOSPC`。
  - **真要 block 等**：用 semaphore + `down_interruptible_timeout`，alloc 失敗（race）時迴圈 retry，避免 token 漏。

**Bug 4：`DSP_IPI_NR_CH = 65` 應該從 device tree 讀，不要 hardcode。** 這在 reviewer 眼裡是 portability 紅旗。

**修正版（推薦）：**

```c
struct dsp_ipi_chan_pool {
    spinlock_t          lock;
    unsigned long      *busy;
    int                 nr;
};

int dsp_ipi_channel_alloc(struct dsp_ipi_chan_pool *p) {
    int ch;
    unsigned long flags;
    spin_lock_irqsave(&p->lock, flags);
    ch = find_first_zero_bit(p->busy, p->nr);
    if (ch < p->nr)
        set_bit(ch, p->busy);
    else
        ch = -ENOSPC;
    spin_unlock_irqrestore(&p->lock, flags);
    return ch;
}
```

**Reviewer 想聽的核心觀念**：「你判斷需不需要 semaphore 的標準是什麼？」
> 答：呼叫者**會不會想 block 等**。如果是 best-effort、拿不到就退、上層自己 retry，就不該用 semaphore——應該用 spinlock + bitmap。如果是 must-have、拿不到就睡到有為止，才用 semaphore（或 wait_queue）。

</details>

---

### 題目 3：`dsp_map_memory_region()` — GB SDRAM 的多 reader 共享

**情境**：「AP 很多模組會讀 DSP 寫到共享 SDRAM 的 firmware log buffer，DSP 偶爾會 rewind buffer。」

```c
static struct semaphore region_lock;   // init 1
static void *dsp_log_buf;
static size_t dsp_log_size;

int dsp_log_read(char __user *to, size_t cnt, loff_t *pos) {
    down(&region_lock);
    if (*pos + cnt > dsp_log_size) cnt = dsp_log_size - *pos;
    if (copy_to_user(to, dsp_log_buf + *pos, cnt)) {
        up(&region_lock);
        return -EFAULT;
    }
    *pos += cnt;
    up(&region_lock);
    return cnt;
}

void dsp_log_rewind(void) {
    down(&region_lock);
    memset(dsp_log_buf, 0, dsp_log_size);
    up(&region_lock);
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（語意紅旗）：semaphore 當 mutex 用。**
- `init=1` 的 semaphore 在功能上等於 mutex，但：
  - mutex 有 owner 概念，可以 priority inherit、debug 工具能告訴你誰持有；semaphore 不行。
  - mutex 在 `CONFIG_DEBUG_MUTEXES` 下能抓到 unlock-by-non-owner、雙鎖；semaphore 抓不到。
- **修法**：改用 `struct mutex region_lock;` + `mutex_lock_interruptible()`。
- 這在 Linux kernel 裡是非常常見的 review 紅旗，[`Documentation/locking/mutex-design.rst`](https://docs.kernel.org/locking/mutex-design.html) 明確寫「prefer mutex over semaphore unless you really need counting semantics」。

**Bug 2（性能）：read 是高頻 + multi-reader，但這寫法強制 reader 之間互斥。**
- read 對 buffer 不寫，理論上多 reader 可並行。
- **修法**：用 `rwlock_t` 或 `struct rw_semaphore`（rwsem）。reader 用 `down_read`，rewind 用 `down_write`。
- 但要小心 writer starvation——`rw_semaphore` 在 Linux 是 fair（writer-preference），這點要在 review 時主動點出來。

**Bug 3：`copy_to_user` 是 may-sleep 的（會 page fault），這代表你**持有 lock 期間可能 sleep**。**
- 對 mutex / rwsem 是 OK 的（它們可以 sleep）。
- 對 spinlock **絕對不 OK**。所以這裡你要主動跟 reviewer 確認「我們選 sleeping lock 是因為要呼叫 `copy_to_user`」——這展現你懂 sleeping vs spinning lock 的選擇邏輯。

**Bug 4：`*pos` 是 caller 的 file offset（每個 fd 獨立），但 `dsp_log_buf` 是全域共享。**
- 兩個 reader 各自 `*pos`，但都讀同一塊 buffer。`dsp_log_rewind` 把 buffer 清空，reader 的 `*pos` 變得指向已清空區域 → 讀到 0。功能上 OK，但要在 doc / comment 裡寫清楚「rewind 後 reader 應自行 reset」。

**Bug 5：沒有 memory barrier。**
- DSP 寫 SDRAM、AP 讀 SDRAM，跨 processor 共享記憶體是 review 的隱形深水區。
- 至少要 `dma_rmb()` / `smp_rmb()` 在讀之前，配合 DSP 寫完後的 `dma_wmb()`。
- 更嚴格：用 `ioremap_wc` / `dma_alloc_coherent` 並搭配對應的 `dma_sync_*` API，cache coherency 由 DMA API 處理。

**Bug 6：`dsp_log_size` 是 `size_t`，`*pos` 是 `loff_t`（signed 64bit）。`*pos + cnt > dsp_log_size` 在 `*pos` 為負或非常大時可能 overflow。**
- 修法：先 `if (*pos < 0 || *pos >= dsp_log_size) return 0;` 再算 cnt。

</details>

---

### 題目 4：`dsp_mbox_probe()` — driver probe 與 firmware ready 的握手

**情境**：「driver probe 時要把 DSP firmware load 進 TCM、然後等 DSP boot 完發 'ready' IPI 才繼續。」

```c
static struct semaphore fw_ready;     // init 0

int dsp_mbox_probe(struct platform_device *pdev) {
    int ret;
    sema_init(&fw_ready, 0);

    ret = load_firmware_to_tcm(...);
    if (ret) return ret;

    writel(BOOT_CMD, mbox_base + BOOT_REG);

    down(&fw_ready);

    register_dsp_services();
    return 0;
}

irqreturn_t dsp_ready_isr(int irq, void *d) {
    up(&fw_ready);
    return IRQ_HANDLED;
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（致命，race window）：在 `sema_init` 之前 `dsp_ready_isr` 可能就觸發了。**
- 場景：上次 boot 殘留的 DSP 還活著，driver 一 load 立刻收到 IPI。`request_irq` 必須在 `sema_init` 之後、`writel(BOOT_CMD)` 之前。
- 但這段 code 連 `request_irq` 都沒寫（？），更別說 ordering。

**Bug 2：用 `semaphore` 而不是 `completion`。**
- One-shot signaling 的標準寫法是 `struct completion`：

```c
struct completion fw_ready;
init_completion(&fw_ready);
// ...
ret = wait_for_completion_interruptible_timeout(&fw_ready, msecs_to_jiffies(2000));
if (ret <= 0) return ret ? ret : -ETIMEDOUT;
// ISR
complete(&fw_ready);
```

- 為什麼？(a) `completion` 設計就是給「等一次事件」的；(b) 自動處理 multiple waiters；(c) `complete_all()` 可以 broadcast；(d) reviewer 一看就懂語意。

**Bug 3：沒 timeout。** firmware 死掉 → probe 永遠卡 → kernel boot 卡死 → 整台手機開不了機。一定要 `*_timeout` 變體。

**Bug 4：`down(&fw_ready)` 不可中斷。** 如果 driver 是 module load，user kill 不掉 modprobe。改 `down_interruptible_timeout` 或用 completion 對應 API。

**Bug 5：error path 沒有 cleanup。**
- 如果 `register_dsp_services` 失敗，`fw_ready` 變成 ready 狀態的 semaphore 沒人清。雖然 stack-allocated 隨函式結束就好，但若是 driver-state 的 field 就要 reset。
- `load_firmware_to_tcm` 成功之後要對應 `unload`；register IRQ 失敗要 `free_irq`，等等。Kernel driver probe 一般用 `goto err_xxx` ladder。

**Bug 6：`writel` 之後沒有 memory barrier。** 對 device register 的 write 通常要 `wmb()` 或用 `writel_relaxed` + 顯式 barrier 來保證 ordering。

**標準 probe 骨架（recommend 給 reviewer）**：

```c
int dsp_mbox_probe(struct platform_device *pdev) {
    struct dsp_dev *dsp;
    int ret;

    dsp = devm_kzalloc(&pdev->dev, sizeof(*dsp), GFP_KERNEL);
    if (!dsp) return -ENOMEM;

    init_completion(&dsp->fw_ready);
    spin_lock_init(&dsp->lock);

    dsp->mbox_base = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(dsp->mbox_base)) return PTR_ERR(dsp->mbox_base);

    ret = devm_request_irq(&pdev->dev, irq, dsp_ready_isr, 0, "dsp", dsp);
    if (ret) return ret;

    ret = load_firmware_to_tcm(dsp);
    if (ret) return ret;

    writel(BOOT_CMD, dsp->mbox_base + BOOT_REG);

    ret = wait_for_completion_interruptible_timeout(
              &dsp->fw_ready, msecs_to_jiffies(2000));
    if (ret == 0) { dev_err(&pdev->dev, "DSP boot timeout\n"); return -ETIMEDOUT; }
    if (ret < 0)  return ret;

    platform_set_drvdata(pdev, dsp);
    return register_dsp_services(dsp);
}
```

</details>

---

### 題目 5：`dsp_time_sync()` — AP/DSP 時鐘同步的 barrier

**情境**：「我要週期性對齊 AP/DSP 時鐘。AP 寫一個 timestamp 到 mbox，DSP 讀後也寫自己 timestamp 回來，AP 拿到後算 offset。」

```c
static struct semaphore tx_mtx;    // init 1, 序列化 timestamp 送出
static struct semaphore rx_done;   // init 0

static u64 dsp_ts;

int dsp_time_sync(s64 *offset) {
    u64 ap_ts1, ap_ts2;
    down(&tx_mtx);
    ap_ts1 = ktime_get_ns();
    writeq(ap_ts1, mbox_base + TS_REG);
    writel(IPI_TS_SYNC, mbox_base + IPI_TRIGGER);
    down(&rx_done);
    ap_ts2 = ktime_get_ns();
    *offset = dsp_ts - (ap_ts1 + ap_ts2) / 2;
    up(&tx_mtx);
    return 0;
}

irqreturn_t ts_sync_isr(int irq, void *d) {
    dsp_ts = readq(mbox_base + TS_REG);
    up(&rx_done);
    return IRQ_HANDLED;
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（致命）：`down(&tx_mtx)` 之後到 `up(&tx_mtx)` 之間的時間包含 `down(&rx_done)`，這會讓你的 mutex 持有期間 sleep——但用 `semaphore` 看不出來，reviewer 會質疑「為什麼不是 mutex？」**
- 用 semaphore 當 mutex 是上一題講過的紅旗，重複犯。

**Bug 2（致命）：`dsp_ts` 是全域變數，沒被 lock 保護。**
- 如果有兩個 thread 同時 `dsp_time_sync`，被 `tx_mtx` 序列化還算好；但 ISR 寫 `dsp_ts` 跟 caller 讀 `dsp_ts` 之間需要 memory barrier。
- 修法：用 per-call struct + completion，把 `dsp_ts` 變 local：

```c
struct ts_req {
    struct completion   done;
    u64                 dsp_ts;
};

static struct ts_req *current_req;
static DEFINE_SPINLOCK(req_lock);

int dsp_time_sync(s64 *offset) {
    struct ts_req req;
    u64 ap_ts1, ap_ts2;
    int ret;
    init_completion(&req.done);

    mutex_lock(&ctx.tx_mtx);
    spin_lock_irq(&req_lock);
    current_req = &req;
    spin_unlock_irq(&req_lock);

    ap_ts1 = ktime_get_ns();
    writeq(ap_ts1, mbox_base + TS_REG);
    wmb();
    writel(IPI_TS_SYNC, mbox_base + IPI_TRIGGER);

    ret = wait_for_completion_interruptible_timeout(&req.done,
                                                    msecs_to_jiffies(50));
    ap_ts2 = ktime_get_ns();

    spin_lock_irq(&req_lock);
    current_req = NULL;
    spin_unlock_irq(&req_lock);
    mutex_unlock(&ctx.tx_mtx);

    if (ret <= 0) return ret ? ret : -ETIMEDOUT;
    *offset = req.dsp_ts - (ap_ts1 + ap_ts2) / 2;
    return 0;
}
```

**Bug 3（時鐘演算法級）：`offset = dsp_ts - (ap_ts1 + ap_ts2) / 2` 假設 AP→DSP 與 DSP→AP 延遲對稱。**
- IPI 不一定對稱（mailbox HW、interrupt latency 不同）。如果你只是要 ballpark sync OK；要精確同步要用 PTP-like protocol（多次採樣、最小化 RTT 取樣）。
- Reviewer 加分：你主動指出「這是 NTP 的 simplified 版本」，並建議要採樣 N 次取最小 RTT 的那次。

**Bug 4：`ap_ts2` 的取樣時機。**
- 你把 `ap_ts2` 放在 `down(&rx_done)` 之後，但 `rx_done` 是 ISR 觸發的，ISR 到 `up()` 還有 ISR exit、wake_up scheduler、context switch 的延遲，這些都算進你的 RTT。
- 嚴格做法：`ap_ts2` 應該在 ISR 裡 sample（這時離真正收到 mailbox interrupt 最近）。

**Bug 5：`u64` 對 32-bit ARM 的 atomicity。**
- 32-bit kernel 上 `readq`/`writeq` 不是 single instruction，會被切成兩個 32-bit access。如果 ISR 寫 `dsp_ts` 高低兩半之間 caller 讀就會讀到 torn value。
- 修法：用 `lo_hi_writeq()` / `hi_lo_readq()` 的對稱組合，或用 spinlock 保護。

**Reviewer 在意的設計討論**：「你怎麼測這段程式碼？」
> 答：(a) inject random delay 在 ISR 模擬 jitter；(b) `CONFIG_PROVE_LOCKING` (lockdep) 跑出來看有沒有 ABBA；(c) `kunit` 寫 mbox simulator 跑 1000 次 sync 看 offset variance；(d) 真機測 thermal/freq scaling 下的 drift。

</details>

---

### 題目 6：`dsp_driver_remove()` / suspend — semaphore drain pattern

**情境**：「shutdown / suspend 時要等所有 in-flight IPI 結束、再停 firmware。」

```c
static atomic_t        inflight_cnt;
static struct semaphore inflight;       // init MAX_INFLIGHT
static bool            shutting_down;

int dsp_send_ipi(...) {
    if (shutting_down) return -ESHUTDOWN;
    down(&inflight);
    atomic_inc(&inflight_cnt);
    /* ... send and wait ack ... */
    atomic_dec(&inflight_cnt);
    up(&inflight);
    return 0;
}

int dsp_driver_remove(struct platform_device *pdev) {
    shutting_down = true;
    while (atomic_read(&inflight_cnt) > 0)
        msleep(10);
    stop_dsp_firmware();
    return 0;
}
```

<details>
<summary>👉 答案</summary>

**Bug 1（race window）：`if (shutting_down)` 與 `down(&inflight)` 之間 race。**
- thread A 通過 `if` 檢查（false），thread B 同時設 `shutting_down = true`，A 繼續 `down` 進去，shutdown 已經以為沒人了 → A 跑進 stopped firmware，crash。
- 修法：`shutting_down` 要是 `atomic_t` 或受 lock 保護，且 check 與 acquire 要 atomic：

```c
mutex_lock(&ctx.state_lock);
if (ctx.shutting_down) {
    mutex_unlock(&ctx.state_lock);
    return -ESHUTDOWN;
}
atomic_inc(&inflight_cnt);
mutex_unlock(&ctx.state_lock);
```

或更乾淨：用 `percpu_ref` / `srcu` 之類的 kernel 資源 reference counting 機制。

**Bug 2（性能與正確性）：`while (... > 0) msleep(10);` 是 busy-poll。**
- 萬一 inflight 永遠 > 0（某個 IPI 卡住），這裡無限 loop。
- 修法：用 `wait_event_interruptible_timeout`，或經典 **drain semaphore pattern**：

```c
/* drain：拿走所有 token，沒拿走的就是被別人持有，等完才拿得到 */
for (int i = 0; i < MAX_INFLIGHT; i++)
    down(&inflight);
/* 此時保證沒有 inflight，可以安全 stop */
stop_dsp_firmware();
```

或用 `struct completion` + counter：每次 inflight 歸零時 `complete()`，shutdown waiter `wait_for_completion`。

**Bug 3：`atomic_inc(&inflight_cnt)` 跟 `down(&inflight)` 是冗餘的雙重簿記。** counting semaphore 內部已經有 counter（`sem_getvalue` 等同），多搞一個 atomic 是多寫一份維護成本。

**Bug 4：`shutting_down = true` 是 plain bool 寫入，沒 memory barrier。**
- 別的 CPU 可能還沒看到。改 `WRITE_ONCE(ctx.shutting_down, true);` 並讀端用 `READ_ONCE`，或用 `atomic_set` + `smp_mb`。

**Bug 5（API design 紅旗）：`shutting_down` 設 true 之後，已經拿 token 的 in-flight 怎麼處理？**
- 兩種策略：(a) 等他們做完（current 寫法）；(b) 主動取消（呼叫 `complete(&req->done)` 設 `req->result = -ESHUTDOWN`，讓 wait 的 thread 立刻醒）。
- production driver 通常 (b) 比較合理——shutdown latency bounded。

**Reviewer 加分**：「你怎麼設計 suspend/resume？」
> 答：(1) suspend 進來先 set state；(2) drain inflight；(3) 把 DSP 切 deep sleep；(4) resume 時 reload firmware（如果 TCM 在 suspend 期間 lost power）；(5) wait_for_completion(fw_ready) 才放開 inflight semaphore。**這完全展現你懂 driver life-cycle**。

</details>

---

## 3. 給這題的 Code Review 重點清單（背起來）

### 同步原語選擇（最高頻紅旗）
- [ ] semaphore 當 mutex 用 → 改 `struct mutex`
- [ ] one-shot signal 用 `sema_init(0)` → 改 `struct completion`
- [ ] reader-heavy 共享資料用 mutex → 改 `rw_semaphore` 或 RCU
- [ ] 高頻短臨界區用 mutex → 考慮 spinlock
- [ ] interrupt context 試圖 sleep（`down`、`mutex_lock`、`copy_to_user`）→ 改 spinlock + bottom half

### Kernel API 紅旗
- [ ] `down()` 而非 `down_interruptible()` / `down_interruptible_timeout()`
- [ ] 沒 timeout 的等待（hang risk）
- [ ] error path 漏 `up()` / `mutex_unlock()` / `free_irq()`
- [ ] `request_irq` 在 `init_completion` 之前（probe ordering）
- [ ] 用 `signal()`、user-space API 在 kernel
- [ ] 沒檢查 `IS_ERR()`、`devm_*` 配對
- [ ] hardcode 數字（FIFO depth、channel count）→ 從 device tree

### IPC / Memory 紅旗
- [ ] 跨 processor 共享 memory 沒有 `dma_*mb()` / `smp_*mb()`
- [ ] `writeq`/`readq` 在 32-bit 平台 atomicity 假設
- [ ] mailbox register write 沒 ordering barrier
- [ ] 一個 ack semaphore 對應多個並行請求（channel mismatch）
- [ ] `*pos` / `loff_t` overflow

### 架構紅旗（senior 分數）
- [ ] 沒 suspend/resume 路徑
- [ ] shutdown 用 busy-poll
- [ ] error 沒 propagate 到 user space
- [ ] 沒考慮 firmware crash recovery
- [ ] 沒 tracepoint / `dev_dbg` for production debugging
- [ ] 沒 unit test（kunit / mock mailbox）

---

## 4. 面試現場 30 分鐘流程

```
0:00 - 0:03  通讀 + 反問：
  - 這 driver 跑在哪個 context？probe 時？runtime IPI 路徑？suspend 路徑？
  - 假設的 FIFO depth、channel count 從哪來？device tree？
  - DSP firmware 假設？coherent memory or non-coherent？

0:03 - 0:08  逐行掃，每行問：
  - 哪個 context 會走這行？process / softirq / hardirq？
  - 有沒有 sleep？allowed？
  - 共享 state 被誰保護？memory barrier？
  - error path 有沒有對稱 release？

0:08 - 0:18  排序 bug 並 narrate：
  優先序：correctness > deadlock/hang risk > robustness > readability > perf
  「我最擔心的是 X，因為……」

0:18 - 0:25  白板寫出修正版骨架，用 completion / mutex / spinlock 替代 semaphore
              並解釋為什麼選那個。

0:25 - 0:30  反問 reviewer：
  - 我有沒有遺漏 corner case（kdump？kexec？hotplug？）
  - 你們怎麼測這段？lockdep / KASAN / kunit？
  - 有沒有對應的 sysfs / debugfs 介面便於 production debug？
```

---

## 5. 最後一晚要做的事

1. **背 5 個原語的選擇邏輯**：mutex / spinlock / semaphore / completion / rwsem。每個寫 1 句「什麼時候用它」。
2. **手寫一次題目 1 的修正版骨架**（per-request completion 那段），不打草稿。
3. **看 `Documentation/locking/`**：`mutex-design.rst`、`semaphore.rst`、`spinlocks.rst` 各掃一遍。
4. **看 mainline 一個真實 mailbox driver**：`drivers/mailbox/mtk-cmdq-mailbox.c`、`drivers/remoteproc/qcom_q6v5_mss.c`，看真 production code 怎麼用 completion + mutex。
5. **想好你要反問什麼**：`device tree binding`、`ABI 穩定性`、`crash recovery`、`PM domain` 是 senior signal。
6. **睡飽**。Concurrency 題拼 working memory。

祝順利。

---

## 附錄：原系統設計題回顧（從圖片重建）

```
       AP                        DSP (Latency ↓)
   ┌────────┐    GB SDRAM    ┌───────────┐
   │        │ ─────────────► │           │
   │        │                │  MBox 0   │
   │        │ ◄──── IPI ───► │  (2/3/4/5)│  ◄── TCM
   │        │ ────────────►  │           │
   └────────┘                └───────────┘

要實作：
1. Driver Core (initial driver) — dsp_driver.h
   ① dsp_map_memory_region
   ② dsp_ipi_init { dsp_mbox_device, dsp_mbox_probe }
   ③ PLT init
   ④ Time Sync

2. IPI Manager — dsp_ipi_id.h, dsp_ipi_table.h
   - 65 channels
   - message dispatching
```

每一個 sub-component 都對應到上面其中一題：
- `dsp_ipi_init` / 同步 send → 題目 1
- channel allocation → 題目 2
- `dsp_map_memory_region` → 題目 3
- `dsp_mbox_probe` → 題目 4
- Time Sync → 題目 5
- driver life-cycle (remove/suspend) → 題目 6
