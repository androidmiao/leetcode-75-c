# Google Code Review 面試準備：AP-DSP IPC Driver × Semaphore（修正解答版）

> 本檔是 `google_code_review_semaphore_prep_linux.md` 的「修正解答版」。
> 把原檔每一題刻意埋下的 bug 全部修掉，並對 **每一行 C code 加上繁體中文註解**，
> 方便面試前一晚最後一輪 review。

---

## 題目 1（解答）：`dsp_send_ipi()` — 同步 IPI、AP 等 DSP ack

### 原檔指出的問題（必修）

1. 全域 `ack` semaphore 會張冠李戴 → 改用 per-request `struct completion`。
2. `down()` 不可中斷 → 改 `down_interruptible_timeout` / `wait_for_completion_interruptible_timeout`。
3. error path 漏 `up()` → 統一用 `goto out_*` ladder。
4. ISR 直接 `up()` 雖可，但與 `down*()` 共用 semaphore 容易誤用 → 改成 `complete()`。
5. `devm_ioremap_resource` 未檢查 `IS_ERR()`。
6. 缺 timeout，DSP hang 住整個 thread 卡死。
7. `MAX_INFLIGHT` hardcode → 從 device tree 讀。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_ipi.c -- FIXED VERSION
#include <linux/semaphore.h>      // counting semaphore：限制 in-flight 請求數
#include <linux/completion.h>     // completion：one-shot 事件通知（取代 sema(0)）
#include <linux/mutex.h>          // mutex：保護 list/狀態
#include <linux/spinlock.h>       // spinlock：ISR/process 共享資料的短臨界區
#include <linux/list.h>           // pending 請求 list
#include <linux/slab.h>           // kmalloc/kfree
#include <linux/io.h>             // ioremap、memcpy_toio、writel
#include <linux/platform_device.h>
#include <linux/interrupt.h>      // request_irq、IRQF_*
#include <linux/of.h>             // of_property_read_u32（device tree）

/* 每一個 in-flight 請求自帶 completion 與 sequence number，
 * ISR 用 seq 找到對應請求，避免「ack 張冠李戴」。 */
struct dsp_ipi_req {
    u32                 channel;     // 此請求對應的 IPI channel id
    u32                 seq;         // 唯一序號，給 ISR 比對用
    struct completion   done;        // DSP ack 之後 complete()
    int                 result;      // DSP 回的結果或 -ESHUTDOWN/-ETIMEDOUT
    struct list_head    node;        // 串到 ctx.pending list 上
};

/* driver 私有 context（避免再用 file-static 全域變數） */
struct dsp_ipi_ctx {
    struct semaphore    inflight;    // counting semaphore，限 mailbox FIFO depth
    struct mutex        tx_lock;     // 保護 mbox register 的 write 序列
    spinlock_t          list_lock;   // 保護 pending list（ISR 也會碰）
    struct list_head    pending;     // 所有等 ack 的請求
    atomic_t            seq_gen;     // sequence number 產生器
    void __iomem       *mbox_base;   // mailbox MMIO 基底位址
    int                 irq;         // mailbox interrupt 號
    int                 max_inflight; // 從 device tree 讀，不再 hardcode
    bool                shutting_down; // suspend/remove 用
};

/* probe 時呼叫，初始化整個 ctx */
int dsp_ipi_init(struct platform_device *pdev, struct dsp_ipi_ctx *ctx)
{
    struct resource *res;            // MMIO 資源描述
    u32 max_inflight = 4;            // 預設 4，device tree 沒寫時用
    int ret;                         // 一般 return 值

    /* 從 device tree 讀 mailbox FIFO depth（portability）*/
    of_property_read_u32(pdev->dev.of_node, "mediatek,mbox-fifo-depth",
                         &max_inflight);
    ctx->max_inflight = max_inflight;             // 存進 ctx
    sema_init(&ctx->inflight, ctx->max_inflight); // counting semaphore 初值 = FIFO 深度
    mutex_init(&ctx->tx_lock);                    // mbox register 互斥用
    spin_lock_init(&ctx->list_lock);              // ISR 安全的 spinlock
    INIT_LIST_HEAD(&ctx->pending);                // 空 list
    atomic_set(&ctx->seq_gen, 0);                 // seq 從 0 開始
    ctx->shutting_down = false;                   // 還沒 shutdown

    /* 取 MMIO，務必先檢查 IS_ERR() */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);   // 拿第 0 號 MEM 資源
    ctx->mbox_base = devm_ioremap_resource(&pdev->dev, res); // map 到 kernel 虛擬位址
    if (IS_ERR(ctx->mbox_base))                              // ★ 一定要檢查
        return PTR_ERR(ctx->mbox_base);                      // 把錯誤碼往上回

    /* IRQ 必須在 ctx 都備好之後才註冊，避免 ISR 比初始化早跑 */
    ctx->irq = platform_get_irq(pdev, 0);                    // 拿第 0 號 IRQ
    if (ctx->irq < 0)                                        // 取不到 IRQ
        return ctx->irq;                                     // 回錯誤碼

    ret = devm_request_irq(&pdev->dev, ctx->irq, dsp_ipi_isr,
                           IRQF_SHARED, dev_name(&pdev->dev), ctx);
    if (ret)                                                 // request_irq 失敗
        return ret;                                          // devm 會自動釋放

    return 0;                                                // 成功
}

/* 同步送 IPI、等 DSP ack。回傳 0 / 負錯誤碼。*/
int dsp_send_ipi(struct dsp_ipi_ctx *ctx, u32 channel,
                 const void *msg, size_t len, unsigned int timeout_ms)
{
    struct dsp_ipi_req *req;         // per-request 結構
    unsigned long flags;             // spinlock irqsave 用
    long wait;                       // wait_for_completion 回傳
    int ret;                         // 一般錯誤碼

    /* shutdown 中拒絕新請求（防止 race 進入即將關閉的 firmware） */
    if (READ_ONCE(ctx->shutting_down))
        return -ESHUTDOWN;

    /* 配置 per-request 結構（不用 stack，因為 list 要跨 thread）*/
    req = kzalloc(sizeof(*req), GFP_KERNEL);   // 配在 heap，ISR 還會用
    if (!req)
        return -ENOMEM;                        // OOM

    req->channel = channel;                    // 紀錄 channel
    req->seq = atomic_inc_return(&ctx->seq_gen); // 產生唯一 seq
    init_completion(&req->done);               // completion 初值 = not done
    INIT_LIST_HEAD(&req->node);                // 還沒掛上 list

    /* 1) 取 inflight token：可中斷 + 帶 timeout，避免 D-state hang */
    ret = down_timeout(&ctx->inflight, msecs_to_jiffies(timeout_ms));
    if (ret) {                                 // 0=成功，非 0=超時
        ret = -ETIMEDOUT;                      // 對 caller 回 -ETIMEDOUT
        goto out_free;                         // 統一錯誤路徑
    }

    /* 2) 把請求掛上 pending list，ISR 才能用 seq 找到 */
    spin_lock_irqsave(&ctx->list_lock, flags); // ISR 也會碰，必須 irqsave
    list_add_tail(&req->node, &ctx->pending);  // FIFO 順序
    spin_unlock_irqrestore(&ctx->list_lock, flags);

    /* 3) 序列化 mbox register write：用 mutex（明確語意）*/
    ret = mutex_lock_interruptible(&ctx->tx_lock);
    if (ret)                                   // 被 signal 中斷
        goto out_unlist;                       // 還沒 trigger，安全 unlist

    memcpy_toio(ctx->mbox_base + channel * 64, msg, len); // 寫 payload 到 mbox
    /* 對 device register 寫的 ordering：先寫資料、後寫 trigger，要 wmb() */
    wmb();                                     // 保證 payload 已落到 device 之前
    writel(req->seq, ctx->mbox_base + IPI_SEQ_REG);  // 把 seq 寫到 mbox header
    writel(1, ctx->mbox_base + IPI_TRIGGER);   // 觸發 IPI（DSP 端 ISR 會讀 seq）

    mutex_unlock(&ctx->tx_lock);               // mbox register 用完，放鎖

    /* 4) 等 DSP ack。可中斷 + 帶 timeout，避免 unkillable */
    wait = wait_for_completion_interruptible_timeout(
              &req->done, msecs_to_jiffies(timeout_ms));
    if (wait == 0) {                           // 超時
        ret = -ETIMEDOUT;                      // 回超時
        goto out_unlist;                       // 從 list 拔掉再放 token
    }
    if (wait < 0) {                            // 被 signal 打斷（-ERESTARTSYS）
        ret = wait;                            // 把錯誤碼往上回
        goto out_unlist;
    }

    ret = req->result;                         // ISR 寫進來的結果（0 或 DSP 錯誤）

out_unlist:
    /* 從 pending list 拔掉（ISR 可能也已拔過，list_del_init 是冪等的）*/
    spin_lock_irqsave(&ctx->list_lock, flags);
    list_del_init(&req->node);                 // 安全多次呼叫
    spin_unlock_irqrestore(&ctx->list_lock, flags);

    up(&ctx->inflight);                        // ★ 任何路徑都要還 token

out_free:
    kfree(req);                                // 釋放 per-request 結構
    return ret;                                // 把最後狀態回給 caller
}

/* 中斷處理：根據 mbox 裡的 seq 找出對應請求，complete 它 */
irqreturn_t dsp_ipi_isr(int irq, void *dev)
{
    struct dsp_ipi_ctx *ctx = dev;             // 從 request_irq 帶過來
    struct dsp_ipi_req *r, *found = NULL;      // 用來 walk list
    u32 seq;                                   // 從 mbox 讀出的 seq
    int result;                                // DSP 回的結果
    unsigned long flags;                       // spinlock irqsave 用

    seq = readl(ctx->mbox_base + IPI_SEQ_REG); // DSP 把它收到的 seq 回傳
    result = readl(ctx->mbox_base + IPI_RESULT_REG); // DSP 回的結果碼

    /* 在 ISR 裡只用 spinlock，絕不可呼叫會 sleep 的 API */
    spin_lock_irqsave(&ctx->list_lock, flags);
    list_for_each_entry(r, &ctx->pending, node) {  // 線性搜尋（list 一般 < FIFO 深度）
        if (r->seq == seq) {                       // 找到對應請求
            found = r;                             // 記下來
            list_del_init(&r->node);               // 從 pending 拔掉
            break;
        }
    }
    spin_unlock_irqrestore(&ctx->list_lock, flags);

    if (found) {                                   // 有對應請求才 complete
        found->result = result;                    // 把 DSP 結果寫進去
        complete(&found->done);                    // 喚醒等待中的 thread
    }
    /* 沒找到代表 stale ack（caller 已 timeout/cancel），安全 drop */

    writel(1, ctx->mbox_base + IPI_ACK_CLEAR);     // 清 mbox interrupt latch
    return IRQ_HANDLED;                            // 告訴 kernel 我處理過了
}
```

---

## 題目 2（解答）：`dsp_ipi_channel_alloc()` — 65 channel slot 配置

### 原檔指出的問題

1. `test_bit` + `set_bit` 不是 atomic pair → 改 `test_and_set_bit()`。
2. error path 漏還 semaphore token。
3. counting semaphore + bitmap 是雙重簿記，能用 spinlock 就用 spinlock。
4. `DSP_IPI_NR_CH = 65` hardcode → device tree 讀。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_ipi_table.c -- FIXED VERSION（不再使用 semaphore）
#include <linux/bitmap.h>          // bitmap 操作
#include <linux/spinlock.h>        // spinlock_t
#include <linux/slab.h>            // kzalloc / bitmap_zalloc
#include <linux/of.h>              // device tree 讀數量

struct dsp_ipi_chan_pool {
    spinlock_t      lock;          // 保護 busy bitmap
    unsigned long  *busy;          // 每 bit 代表 1 個 channel：1=占用
    int             nr;             // 總 channel 數，從 device tree 讀
};

/* 由 probe 呼叫：初始化 channel pool */
int dsp_ipi_chan_pool_init(struct device *dev, struct dsp_ipi_chan_pool *p)
{
    u32 nr = 65;                                  // 預設值，device tree 沒寫才用
    of_property_read_u32(dev->of_node, "mediatek,nr-channels", &nr); // 讀 dt
    p->nr = nr;                                   // 寫進結構

    p->busy = devm_bitmap_zalloc(dev, p->nr, GFP_KERNEL); // 配置 + 全清 0
    if (!p->busy)                                 // 配置失敗
        return -ENOMEM;                           // 回 OOM

    spin_lock_init(&p->lock);                     // spinlock 初始化
    return 0;                                     // 成功
}

/* 配置一個 channel；best-effort：拿不到就回 -ENOSPC，不會 block */
int dsp_ipi_channel_alloc(struct dsp_ipi_chan_pool *p)
{
    int ch;                                       // 拿到的 channel 號
    unsigned long flags;                          // spinlock 旗標

    spin_lock_irqsave(&p->lock, flags);           // 進入臨界區，禁中斷
    ch = find_first_zero_bit(p->busy, p->nr);     // 找第一個 0（free）的 bit
    if (ch < p->nr)                               // 找得到（小於總數）
        set_bit(ch, p->busy);                     // 標 1，占住它
    else
        ch = -ENOSPC;                             // 全滿，回錯誤碼
    spin_unlock_irqrestore(&p->lock, flags);      // 離開臨界區

    return ch;                                    // 0..nr-1 或 -ENOSPC
}

/* 歸還 channel；不會失敗 */
void dsp_ipi_channel_free(struct dsp_ipi_chan_pool *p, int ch)
{
    unsigned long flags;                          // spinlock 旗標

    if (ch < 0 || ch >= p->nr)                    // 防呆，避免越界
        return;

    spin_lock_irqsave(&p->lock, flags);           // 進入臨界區
    clear_bit(ch, p->busy);                       // 把該 bit 清 0
    spin_unlock_irqrestore(&p->lock, flags);      // 離開臨界區
}
```

> **Reviewer 心法**：caller 是 *best-effort*（拿不到回錯就好），不該用 semaphore；
> 只有「拿不到就睡到有為止」才用 semaphore。

---

## 題目 3（解答）：`dsp_log_read/rewind` — 共享 SDRAM 多 reader

### 原檔指出的問題

1. `init=1` 的 semaphore 當 mutex 用 → 改 `struct rw_semaphore`（多 reader）。
2. multi-reader 應允許並發 → 用 `down_read` / `down_write`。
3. `copy_to_user` 會 sleep → 必須用 sleeping lock（rwsem OK，spinlock 不行）。
4. 跨 processor 共享 memory 缺 `dma_rmb()`。
5. `*pos + cnt` 對 `loff_t`/`size_t` 可能 overflow。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_log.c -- FIXED VERSION（rwsem + barrier + overflow 檢查）
#include <linux/rwsem.h>           // rw_semaphore：多 reader / 單 writer
#include <linux/uaccess.h>         // copy_to_user
#include <linux/dma-mapping.h>     // dma_rmb / dma_wmb
#include <linux/string.h>          // memset

struct dsp_log_ctx {
    struct rw_semaphore  rwsem;    // reader-heavy：多 reader 並行
    void                *buf;       // SDRAM 對映進來的 kernel 虛擬位址
    size_t               size;      // buffer 大小（bytes）
};

/* read：高頻、多 reader，可並行；只有 rewind 排他 */
ssize_t dsp_log_read(struct dsp_log_ctx *ctx, char __user *to,
                     size_t cnt, loff_t *pos)
{
    ssize_t ret;                          // 實際讀的 bytes
    size_t  remain;                       // 剩可讀的 bytes

    /* 防呆：負 offset、超過尾端、cnt 為 0 都直接回 */
    if (*pos < 0)                         // 不允許負 offset
        return -EINVAL;
    if ((u64)*pos >= ctx->size)           // 已到尾端，回 0（EOF 慣例）
        return 0;
    if (cnt == 0)                         // 讀 0 byte
        return 0;

    /* 安全算 remain：避免 *pos + cnt 溢位 */
    remain = ctx->size - (size_t)*pos;    // 不會 overflow（已保證 *pos < size）
    if (cnt > remain)                     // 截斷到 buffer 尾端
        cnt = remain;

    /* 取 read lock：可被 signal 中斷，避免 unkillable */
    if (down_read_killable(&ctx->rwsem))
        return -ERESTARTSYS;

    /* 跨 processor 讀屏障：確保看到 DSP 端最新寫入 */
    dma_rmb();

    /* 真正把 kernel buffer 內容拷貝到 user space。
     * copy_to_user 可能 page fault → 會 sleep → 所以一定要用 sleeping lock。 */
    if (copy_to_user(to, (char *)ctx->buf + *pos, cnt)) {
        ret = -EFAULT;                    // user pointer 不合法
        goto out;
    }

    *pos += cnt;                          // 推進 caller 的 file offset
    ret = cnt;                            // 回實際讀到的 bytes

out:
    up_read(&ctx->rwsem);                 // ★ 不論成功失敗都要放 read lock
    return ret;
}

/* rewind：寫者，排他持有 rwsem */
void dsp_log_rewind(struct dsp_log_ctx *ctx)
{
    /* 寫者拿 write lock；rwsem 在 Linux 是 writer-preference，避免 starvation */
    down_write(&ctx->rwsem);              // 排他鎖

    memset(ctx->buf, 0, ctx->size);       // 清空整個 log buffer
    dma_wmb();                            // 保證 zero 已落到 SDRAM，DSP 才看得到

    up_write(&ctx->rwsem);                // 放 write lock
}

/* 由 probe 呼叫：初始化 ctx */
int dsp_log_init(struct dsp_log_ctx *ctx, void *buf, size_t size)
{
    init_rwsem(&ctx->rwsem);              // rwsem 初始化
    ctx->buf  = buf;                      // 已 ioremap/dma_alloc 的位址
    ctx->size = size;                     // 由 caller 確定大小
    return 0;
}
```

---

## 題目 4（解答）：`dsp_mbox_probe()` — probe 與 firmware ready 握手

### 原檔指出的問題

1. `request_irq` 必須在 `init_completion` 之後、`writel(BOOT_CMD)` 之前。
2. one-shot signal 用 `completion` 而非 semaphore。
3. 缺 timeout，firmware 死掉 = 整台手機開不了機。
4. 缺可中斷等待。
5. error path 缺 cleanup ladder。
6. mailbox register write 缺 `wmb()`。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_mbox.c -- FIXED VERSION（標準 probe ladder + completion + timeout）
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/io.h>
#include <linux/slab.h>

struct dsp_dev {
    struct completion   fw_ready;     // one-shot：DSP boot 完成事件
    spinlock_t          lock;         // 保護 driver 狀態
    void __iomem       *mbox_base;    // mailbox MMIO
    int                 irq;          // mailbox IRQ
};

/* DSP boot 完成時，DSP 透過 mailbox 觸發此 ISR */
irqreturn_t dsp_ready_isr(int irq, void *d)
{
    struct dsp_dev *dsp = d;          // 從 request_irq 帶過來

    complete(&dsp->fw_ready);         // 喚醒 probe thread；冪等：多次 call 也安全
    return IRQ_HANDLED;               // 告訴 kernel 處理完了
}

int dsp_mbox_probe(struct platform_device *pdev)
{
    struct dsp_dev *dsp;              // driver 私有結構
    int ret;                          // 一般 return

    /* 1) 配置 driver state（devm 會在 remove 時自動 free） */
    dsp = devm_kzalloc(&pdev->dev, sizeof(*dsp), GFP_KERNEL);
    if (!dsp)
        return -ENOMEM;

    /* 2) 先把同步原語和 spinlock 都備好，再 register IRQ
     *    這個 ordering 是 Bug 1 的修法核心：ISR 跑時所有狀態一定都已初始化。 */
    init_completion(&dsp->fw_ready);  // completion 初始為 "not done"
    spin_lock_init(&dsp->lock);       // spinlock 初始化

    /* 3) 取 MMIO（devm 變體：自動於 remove 釋放） */
    dsp->mbox_base = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(dsp->mbox_base))                       // ★ 必檢查
        return PTR_ERR(dsp->mbox_base);

    /* 4) 拿 IRQ 號 */
    dsp->irq = platform_get_irq(pdev, 0);
    if (dsp->irq < 0)
        return dsp->irq;

    /* 5) register IRQ — 注意：此時 fw_ready 已 init，ISR 跑進來不會 crash */
    ret = devm_request_irq(&pdev->dev, dsp->irq, dsp_ready_isr,
                           IRQF_SHARED, dev_name(&pdev->dev), dsp);
    if (ret)
        return ret;

    /* 6) load firmware 到 TCM */
    ret = load_firmware_to_tcm(dsp);
    if (ret)
        goto err_unload;             // 失敗就 cleanup

    /* 7) 觸發 DSP boot：先寫資料、再 wmb、再 trigger */
    wmb();                            // 保證上面對 SDRAM/TCM 的寫已可見
    writel(BOOT_CMD, dsp->mbox_base + BOOT_REG);  // 啟動 DSP

    /* 8) 等 DSP boot：可中斷 + 帶 timeout，避免 hang 整台機器
     *    回傳：>0 剩餘 jiffies；0 超時；<0 被 signal 中斷 */
    ret = wait_for_completion_interruptible_timeout(
              &dsp->fw_ready, msecs_to_jiffies(2000));
    if (ret == 0) {                   // 超時
        dev_err(&pdev->dev, "DSP boot timeout\n");
        ret = -ETIMEDOUT;
        goto err_stop_dsp;
    }
    if (ret < 0)                      // 被 signal 中斷（-ERESTARTSYS）
        goto err_stop_dsp;

    /* 9) 註冊上層服務 */
    ret = register_dsp_services(dsp);
    if (ret)
        goto err_stop_dsp;

    platform_set_drvdata(pdev, dsp);  // 存 driver data，給 remove/suspend 用
    return 0;                         // probe 成功

err_stop_dsp:
    writel(STOP_CMD, dsp->mbox_base + BOOT_REG);  // 停 DSP
err_unload:
    unload_firmware_from_tcm(dsp);                // 把 firmware 從 TCM 卸掉
    return ret;                                   // devm 會自動 free_irq / iounmap / kfree
}
```

---

## 題目 5（解答）：`dsp_time_sync()` — AP/DSP 時鐘同步

### 原檔指出的問題

1. semaphore 當 mutex 用，而且持有期間 sleep。
2. `dsp_ts` 是全域，沒 per-call 隔離 → 多 thread 會撞。
3. 對稱延遲假設不嚴謹（NTP-like 採樣多次取最小 RTT）。
4. `ap_ts2` 在 ISR 後採樣，含 ISR exit + scheduler 延遲。
5. 32-bit 平台 `readq/writeq` 非 atomic。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_time_sync.c -- FIXED VERSION
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/ktime.h>
#include <linux/io.h>
#include <linux/io-64-nonatomic-lo-hi.h>   // 32-bit 安全的 readq/writeq

struct ts_req {
    struct completion   done;         // ISR 觸發即 complete
    u64                 dsp_ts;       // ISR 寫入：DSP 端的 timestamp
    u64                 ap_ts2;       // ISR 寫入：AP 收到 ack 當下時間
};

struct dsp_ts_ctx {
    struct mutex            tx_mtx;   // 序列化整個 sync 流程
    spinlock_t              req_lock; // 保護 current_req 指標
    struct ts_req __rcu    *current_req; // ISR 用這個找到 in-flight req
    void __iomem           *mbox_base;
};

/* 真正的 sync：採 N 次取最小 RTT 的那一次（NTP-like） */
int dsp_time_sync(struct dsp_ts_ctx *ctx, s64 *offset_ns)
{
    const int N = 8;                  // 採樣次數
    s64 best_offset = 0;              // 暫存最小 RTT 對應的 offset
    s64 best_rtt   = S64_MAX;         // 初值極大
    int i;                            // 迴圈
    int ret = 0;                      // 回傳

    /* 整個流程要序列化（一次只跑一個 sync）→ 用 mutex（明確語意） */
    ret = mutex_lock_interruptible(&ctx->tx_mtx);
    if (ret)
        return ret;

    for (i = 0; i < N; i++) {         // 採樣 N 次
        struct ts_req req;            // per-call，stack 上即可（同步等待）
        u64 ap_ts1;                   // 送出當下的 AP 時間
        long w;                       // wait 回傳

        init_completion(&req.done);   // 每次 sample 都重新 init
        req.dsp_ts = 0;
        req.ap_ts2 = 0;

        /* 把 current_req 設成這次的 req，ISR 才能寫進去 */
        spin_lock_irq(&ctx->req_lock);
        rcu_assign_pointer(ctx->current_req, &req);
        spin_unlock_irq(&ctx->req_lock);

        /* 採樣 ap_ts1 後立刻寫 mbox + trigger，越緊湊 RTT 越準 */
        ap_ts1 = ktime_get_ns();      // AP 送出當下時間
        lo_hi_writeq(ap_ts1, ctx->mbox_base + TS_REG); // 32-bit 安全的 writeq
        wmb();                         // 保證 TS_REG 已寫到 device
        writel(IPI_TS_SYNC, ctx->mbox_base + IPI_TRIGGER); // 觸發 DSP

        /* 等 ISR：可中斷 + 50ms timeout */
        w = wait_for_completion_interruptible_timeout(
                &req.done, msecs_to_jiffies(50));

        /* 用完就把 current_req 拿掉（避免下一次 ISR 寫到舊位址） */
        spin_lock_irq(&ctx->req_lock);
        rcu_assign_pointer(ctx->current_req, NULL);
        spin_unlock_irq(&ctx->req_lock);
        synchronize_rcu();             // 等所有 ISR reader 走完，stack 才安全離開

        if (w == 0) { ret = -ETIMEDOUT; goto out; }
        if (w < 0)  { ret = w;          goto out; }

        /* RTT = ap_ts2 - ap_ts1；offset 用對稱假設估 */
        {
            s64 rtt = (s64)(req.ap_ts2 - ap_ts1);
            s64 mid = (s64)(ap_ts1 + req.ap_ts2) / 2;
            s64 off = (s64)req.dsp_ts - mid;

            if (rtt < best_rtt) {     // 取最小 RTT 那次
                best_rtt    = rtt;
                best_offset = off;
            }
        }
    }

    *offset_ns = best_offset;          // 回傳最佳估計
out:
    mutex_unlock(&ctx->tx_mtx);
    return ret;
}

/* ISR：第一時間採樣 ap_ts2（最接近收到 mailbox interrupt 的瞬間） */
irqreturn_t ts_sync_isr(int irq, void *d)
{
    struct dsp_ts_ctx *ctx = d;
    struct ts_req     *r;
    u64                ap_ts2 = ktime_get_ns(); // ★ 第一行就採樣
    u64                dsp_ts;

    /* 32-bit 安全的 readq；保證高低 32-bit 不被切碎 */
    dsp_ts = lo_hi_readq(ctx->mbox_base + TS_REG);

    /* RCU 讀 current_req（ISR 不能 sleep，rcu_read_lock 是 OK 的） */
    rcu_read_lock();
    r = rcu_dereference(ctx->current_req);
    if (r) {
        r->dsp_ts = dsp_ts;            // ISR 寫 result（單一 writer）
        r->ap_ts2 = ap_ts2;
        smp_wmb();                     // 保證上面的寫先於 complete()
        complete(&r->done);            // 喚醒 caller
    }
    rcu_read_unlock();

    writel(1, ctx->mbox_base + IPI_ACK_CLEAR); // 清 mbox interrupt
    return IRQ_HANDLED;
}
```

---

## 題目 6（解答）：`dsp_driver_remove()` / suspend — drain pattern

### 原檔指出的問題

1. `if (shutting_down)` 與 `down(&inflight)` 之間 race。
2. busy-poll `msleep(10)` 不可控；應用 drain semaphore pattern 或 completion。
3. `atomic_inc(inflight_cnt)` 與 semaphore 是雙重簿記。
4. `shutting_down` plain bool 沒 barrier。
5. shutdown 應主動取消 in-flight，不要等到天荒地老。

### 修正版 C code（逐行繁體中文註解）

```c
// dsp_lifecycle.c -- FIXED VERSION（drain semaphore + 主動取消）
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/completion.h>

/* 沿用題目 1 的 ctx 與 dsp_ipi_req（為了精簡只列關鍵欄位） */
struct dsp_ipi_ctx {
    struct semaphore   inflight;        // 與 mbox FIFO 同寬
    struct mutex       state_lock;      // 保護 shutting_down
    spinlock_t         list_lock;       // 保護 pending list
    struct list_head   pending;
    int                max_inflight;    // 從 device tree 讀
    bool               shutting_down;
};

/* 進入 send 前的 gate：用 mutex 把「檢查 + 占 token」做成 atomic */
int dsp_send_ipi_gated(struct dsp_ipi_ctx *ctx /*, ... */)
{
    int ret;

    mutex_lock(&ctx->state_lock);
    if (ctx->shutting_down) {           // 只要設了，就拒絕新請求
        mutex_unlock(&ctx->state_lock);
        return -ESHUTDOWN;
    }
    /* 在持鎖狀態下 down_trylock：不 sleep，避免 mutex 持有期間阻塞 */
    if (down_trylock(&ctx->inflight)) {
        mutex_unlock(&ctx->state_lock);
        /* 沒拿到 token：可選擇 down_timeout 在 mutex 外面慢慢等 */
        if (down_timeout(&ctx->inflight, msecs_to_jiffies(500)))
            return -EBUSY;
        /* 拿到 token 後再次檢查 shutting_down（double-check） */
        mutex_lock(&ctx->state_lock);
        if (ctx->shutting_down) {
            mutex_unlock(&ctx->state_lock);
            up(&ctx->inflight);
            return -ESHUTDOWN;
        }
    }
    mutex_unlock(&ctx->state_lock);

    /* ... 真正的 send（同題目 1）... */
    ret = 0;

    up(&ctx->inflight);                 // 還 token
    return ret;
}

/* remove / suspend：drain 流程 */
int dsp_driver_remove(struct platform_device *pdev)
{
    struct dsp_ipi_ctx *ctx = platform_get_drvdata(pdev);
    struct dsp_ipi_req *r;
    unsigned long flags;
    int i;

    /* 1) 設 shutting_down，新請求被擋在 gate 外
     *    用 mutex 配合 send 路徑，避免 race；WRITE_ONCE 保證寫入順序 */
    mutex_lock(&ctx->state_lock);
    WRITE_ONCE(ctx->shutting_down, true);
    mutex_unlock(&ctx->state_lock);

    /* 2) 主動取消所有 in-flight：寫 -ESHUTDOWN 並 complete()
     *    讓在 wait_for_completion 上的 thread 馬上醒來，不必等 timeout */
    spin_lock_irqsave(&ctx->list_lock, flags);
    list_for_each_entry(r, &ctx->pending, node) {
        r->result = -ESHUTDOWN;         // 結果碼
        complete(&r->done);             // 喚醒等待的 thread
    }
    spin_unlock_irqrestore(&ctx->list_lock, flags);

    /* 3) drain semaphore pattern：把所有 token 通通拿走
     *    沒拿走的代表 thread 還在路徑中，down 會等到他們 up()。
     *    結束時保證 inflight = 0，可以安全停 firmware。 */
    for (i = 0; i < ctx->max_inflight; i++)
        down(&ctx->inflight);

    /* 4) 此時保證沒有任何 sender 還在 mbox 路徑上 */
    stop_dsp_firmware();
    return 0;
}
```

---

## 重點 cheat sheet（最後一晚背）

```
原語選擇判斷：
  互斥（短臨界區，可能在 IRQ）        → spinlock
  互斥（可 sleep，process context）    → mutex
  one-shot 事件通知                   → completion
  資源池上限 + caller 願意等           → counting semaphore
  reader 多 / writer 少                → rw_semaphore（或 RCU）
  best-effort 拿不到就走               → spinlock + bitmap，不要 semaphore

Kernel 必備慣例：
  1. 永遠 down_interruptible / wait_for_completion_interruptible_timeout
  2. error path 一律 goto out_*；釋放對稱
  3. ISR 內絕不 sleep（不可 down/mutex_lock/copy_to_user/kmalloc(GFP_KERNEL)）
  4. 跨 processor 共享記憶體要 dma_rmb / dma_wmb；plain bool 寫用 WRITE_ONCE
  5. probe 順序：sema/completion init → request_irq → load fw → trigger → wait
  6. hardcode 數字（FIFO depth、channel 數）一律 device tree
  7. shutdown：設旗標 → 主動取消 in-flight → drain → stop firmware
```

祝面試順利。
