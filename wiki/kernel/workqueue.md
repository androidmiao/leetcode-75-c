# workqueue 工作佇列

## 核心概念

`workqueue` 是 Linux kernel **把工作從 atomic context（IRQ / softirq / spinlock-held）丟到 process context** 的標準機制。kernel thread 池（`kworker/N:M`）會把排進來的 `work_struct` 拿出來執行——可以睡眠、可以拿 mutex、可以 `kmalloc(GFP_KERNEL)`、可以做 I/O。

### 一句話判斷

> 你正在 IRQ / softirq / atomic context 收到事件，但要做的事「需要睡眠或會花較長時間」（拿鎖、I/O、`copy_to_user`、發 sysfs uevent、跑 firmware command）→ defer 到 workqueue。

---

## 為什麼需要它

中斷上半部（top half）有兩條鐵律：**不能睡眠**、**要儘量短**。一旦真正的處理需要拿 mutex、發 IPC、或可能 OOM 重試（`GFP_KERNEL`），你就必須把工作搬到別處。三條傳統路線：

| 機制 | context | 可睡 | 適用 |
|------|---------|------|------|
| **softirq / tasklet** | softirq | 否 | µs 級、純 CPU、不睡眠 |
| **threaded IRQ** | per-IRQ kthread | 是 | 1:1 對應一個 IRQ 的長處理 |
| **workqueue** | shared kthread pool | 是 | 一般 deferred work、可批次、可延遲 |

**threaded IRQ vs workqueue 選擇**：1 個 IRQ 1 個 thread 的 throughput 不如 thread pool；但 latency 更低、優先級可控。多數 driver 的「event handler 後處理」都選 workqueue。

---

## API：兩個層次

### 第 1 層：「我有東西要 defer，丟給系統共用 queue」

```c
#include <linux/workqueue.h>

struct my_ctx {
    int               event;
    struct work_struct work;                       // 嵌入式：work 是 ctx 的一部分
};

static void my_worker(struct work_struct *w)
{
    struct my_ctx *ctx = container_of(w, struct my_ctx, work);
    do_long_thing(ctx);                            // 在 process context 跑，可睡
}

INIT_WORK(&ctx->work, my_worker);                  // 初始化（一次就好）
schedule_work(&ctx->work);                         // 排入 system_wq（系統共用）
```

`schedule_work()` 排入的是 **`system_wq`**，是所有沒有特別需求的 deferred work 的預設目的地。它本質上是 `queue_work(system_wq, work)`。

### 第 2 層：「我需要自己的 workqueue（隔離 / 排序 / freezable / mem-reclaim）」

```c
struct workqueue_struct *wq;

wq = alloc_workqueue("my_wq", flags, max_active);
                                                   // flags：見下表
                                                   // max_active = 0 → 預設值（256）

queue_work(wq, &ctx->work);
queue_delayed_work(wq, &ctx->dwork, msecs_to_jiffies(100));

destroy_workqueue(wq);                             // module exit 時呼叫
```

#### `alloc_workqueue` 常見 flag

| Flag | 意義 | 何時要 |
|------|------|--------|
| `WQ_UNBOUND` | 不綁特定 CPU；scheduler 自由分配 | latency-insensitive、長跑、CPU 親和性沒意義 |
| `WQ_FREEZABLE` | suspend 時凍結，resume 時解凍 | 與電源狀態同步的工作（time-sync、PM event）|
| `WQ_MEM_RECLAIM` | 保留 rescuer thread，OOM 時也能跑 | 跑在 reclaim 路徑上的 work（block I/O、swap）|
| `WQ_HIGHPRI` | 用 high-priority worker pool | 對 latency 敏感的處理 |
| `WQ_CPU_INTENSIVE` | worker 不參與 concurrency 計數 | 長 CPU-bound 任務，避免擋住其他 work |
| `WQ_POWER_EFFICIENT` | 啟用 `wq_power_efficient` 時 unbound | 行動裝置省電 |

**現代寫法**：永遠用 `alloc_workqueue("name", flags, max_active)`，**不要用 `create_workqueue()` / `create_singlethread_workqueue()` / `create_freezable_workqueue()`**——這些都已 deprecated（內部就是 `alloc_workqueue` 加固定 flag）。`[[interview/mcupm-code-review]]` Q10 正是這個考點。

---

## delayed_work、deferrable timer

```c
struct delayed_work dwork;

INIT_DELAYED_WORK(&dwork, my_worker);
queue_delayed_work(wq, &dwork, msecs_to_jiffies(500));   // 0.5 秒後跑

mod_delayed_work(wq, &dwork, msecs_to_jiffies(100));     // 重新排到 100ms 後（如果還沒跑）

cancel_delayed_work_sync(&dwork);                        // 阻塞等到 callback 結束
```

`delayed_work` 內部 = `work_struct + timer_list`。timer 觸發後把 work 排入 queue，然後 worker thread 執行。timer 階段不耗 CPU；worker 階段才執行真正的程式碼。

---

## 同步與生命週期：四個必背 API

```c
flush_work(&ctx->work);                             // 等這個 work 跑完（可能還沒開始就直接返回）
flush_delayed_work(&ctx->dwork);                    // 含 timer 階段：取消 timer 改立刻排入再 flush

cancel_work_sync(&ctx->work);                       // 取消 + 等正在跑的跑完，保證 callback 已退出
cancel_delayed_work_sync(&ctx->dwork);              // 同上，含 timer 階段

flush_workqueue(wq);                                // 等整個 queue 內所有 work 跑完
drain_workqueue(wq);                                // 同上，且禁止再排新 work（destroy 前用）
```

**關鍵 invariant**：`destroy_workqueue(wq)` 之前，必須保證沒有任何 work 還在 queue 中、也沒有任何 work callback 還持有指向「即將消失資源」的指標。標準 teardown：

```c
ctx->shutting_down = true;                          // 通知 callback「不要再 re-arm」
cancel_delayed_work_sync(&ctx->dwork);              // 取消並等待
destroy_workqueue(wq);                              // 此時保證 wq 內已無 work
kfree(ctx);                                         // 釋放
```

---

## 三大陷阱

### 陷阱 1：work callback re-arm 自己，destroy 卻沒對稱關閉

```c
static void worker(struct work_struct *w)
{
    do_thing();
    queue_delayed_work(wq, &dwork, HZ);             // 自我重新排程
}
```

teardown 時若只 `cancel_delayed_work_sync` 一次，可能正好 callback 跑到一半又把自己排進去——回 true 但下一輪又進來。要嘛先設「停止 flag」讓 callback 自己不 re-arm，要嘛在 loop 裡重複 cancel + 檢查 `delayed_work_pending`。

### 陷阱 2：suspend / resume 不對稱

```c
/* probe */
wq = alloc_workqueue("ts", WQ_FREEZABLE, 0);
queue_delayed_work(wq, &dwork, ...);

/* suspend */
hrtimer_cancel(&timer);                              // ❌ 漏了 freeze workqueue
                                                     // ❌ 漏了 cancel pending dwork

/* resume */
hrtimer_start(...);                                  // 重新啟動
                                                     // ❌ 沒重新 schedule dwork → 永遠不跑
```

對稱 teardown 是 review 重點。`[[interview/mcupm-code-review]]` Q10 / Q9 詳述「freeze header / version 協定」與「workqueue freeze / unfreeze 對稱性」。

### 陷阱 3：在 work callback 裡持有 caller 釋放掉的物件

```c
schedule_work(&ctx->work);
kfree(ctx);                                          // ❌ work 還沒跑，callback 一進來就 UAF
```

正確路徑：`cancel_work_sync(&ctx->work);` 後再 `kfree(ctx)`，或用 refcount 控制。

---

## 與其他 deferred 機制的對比

| 機制 | context | 可睡 | latency | 適用 |
|------|---------|------|---------|------|
| **softirq** | softirq | 否 | sub-µs | 網路收發、timer |
| **tasklet** | softirq | 否 | sub-µs | 已建議 deprecate，新 code 用 workqueue 或 BH-locked work |
| **hrtimer** | hard IRQ context（callback）| 否 | µs 級精準 | 需要高精度週期 |
| **threaded IRQ** | per-IRQ kthread | 是 | µs–ms | 1:1 IRQ 後處理 |
| **workqueue** | shared kthread pool | 是 | µs–ms | 一般 deferred work、可批次 |
| **delayed_work** | workqueue + timer | 是 | timer 精度 | 「N ms 後做一件事」、可 mod |
| **kthread_run** | dedicated kthread | 是 | depends | 長期跑的 worker（不適合短工作）|

**選擇口訣**：短而不睡 → softirq；要睡 → workqueue；高精度週期 + 不睡 → hrtimer；高精度週期 + 要睡 → hrtimer + 在 callback 裡 `queue_work`。

---

## 實戰：在 kernel 源碼中的定位

### 典型用例

- **`drivers/net/ethernet/`** — NIC RX 完成後 `napi_schedule` → softirq；長處理（link state change、ethtool）→ workqueue。
- **`fs/`** — writeback、journal commit、flush 都用 dedicated workqueue。
- **`drivers/firmware/arm_scmi/`** — SCMI notification 從 mailbox IRQ 收到後，丟 workqueue 給 user 端 callback。
- **`drivers/remoteproc/mcupm/`** — `mcupm_timesync.c` 用 hrtimer 觸發 + workqueue 做 mailbox sync（`[[interview/mcupm-code-review]]` Q10）。
- **`kernel/workqueue.c` + `include/linux/workqueue.h`** — 實作與 API；`Documentation/core-api/workqueue.rst` 是官方說明。

### 與 LeetCode pattern 的呼應

- **`[[patterns/stack-queue]]`** — workqueue 本質是 FIFO；`work_struct` 是 work item。
- **生產者–消費者**：IRQ 是 producer，kworker thread 是 consumer，中間是 work list。

---

## 相關 wiki

- `[[kernel/spinlock]]` — IRQ handler 內常見的「短鎖 + 把長工作丟 workqueue」模式
- `[[kernel/list-head]]` — workqueue 內部 work list 就是 list_head
- `[[kernel/circular-buffer]]` — workqueue 與 ring buffer 是兩種 producer–consumer：buffer 帶資料、wq 帶任務
- `[[kernel/memory-management]]` — work callback 可用 `GFP_KERNEL`，是 defer 的主要原因之一
- `[[kernel/rcu]]` — `kfree_rcu()` 內部就是排入 RCU callback queue（概念類似但獨立子系統）
- `[[interview/mcupm-code-review]]` — Q10 `create_workqueue` deprecated + suspend/resume 對稱性、Q11 module exit 為何空
- `[[interview/code-review-questions]]` — 主管視角 review checklist 中 deferred-work 章節
- `[[interview/google_code_review_semaphore_prep_linux_primitives]]` — softirq / workqueue / threaded IRQ 三選一決策圖

---

## 面試速記（背誦版）

1. workqueue = 把 work 從 atomic context 丟到 kthread (process context)，可睡。
2. 預設用 `alloc_workqueue("name", flags, max_active)`，**不要用 deprecated 的 `create_workqueue`**。
3. 想要 freeze on suspend → `WQ_FREEZABLE`；走 reclaim 路徑 → `WQ_MEM_RECLAIM`。
4. teardown 三步：設 stop flag → `cancel_*_sync` → `destroy_workqueue`。
5. `flush_work` ≠ `cancel_work_sync`：前者只等跑完，後者連排隊未跑都取消。
6. 短而不睡用 softirq；高精度週期用 hrtimer；需要睡用 workqueue。
7. delayed_work = work + timer；`mod_delayed_work` 是「重新計時」的標準寫法。
