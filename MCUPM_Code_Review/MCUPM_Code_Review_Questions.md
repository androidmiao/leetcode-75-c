# MCUPM Driver — Google Code Review 題目集

> 取材來源：`drivers/remoteproc/mcupm/v2/`
> （`mcupm_driver.c`, `mcupm_plt.c`, `mcupm_timesync.c`）
> 這是一份「主管 Code Review 角度考題」風格的訓練題庫，模擬 Google L4/L5
> 面試官在 onsite 拿一段真實 MTK production code 給你 review 的情境。
> 每題包含：背景 → 程式碼 → 提問 → 期望回答方向 → 追問 → 真正在考什麼。

---

## 0. 背景：MCUPM Driver 在做什麼？

MCUPM (MCU Power Manager) 是 MediaTek 平台上一顆 co-processor 的 Linux kernel
driver。它做三件事：

1. **IPI / Mailbox 介面** (`mcupm_driver.c`)：透過 16 個 mailbox channel 與 MCU
   firmware 做 inter-processor 通訊，包裝 `mtk_mbox_*` / `mtk_ipi_*` 底層 API。
2. **Platform 層 sysfs 診斷** (`mcupm_plt.c`)：提供 `mcupm_alive`、
   `mcdi_dynamic_finegrain` 等 sysfs node，在 runtime 對 MCU 做 ping 與
   fine-grain 控制。
3. **Time Sync** (`mcupm_timesync.c`)：以 hrtimer 週期性地把 ARM arch_timer
   tick 與 sched_clock timestamp 透過 mailbox 同步給 MCU，讓兩顆 CPU 的
   timestamp 對齊；suspend/resume 時要 freeze/unfreeze。

題目刻意挑選**真實 production code**，你會發現這些 bug 都不是 textbook
範例，而是 review 現場常被打槍的點。

---

## Q1. Mailbox helper 的 `mbox` 參數沒有 bounds check

### 程式碼（`mcupm_driver.c` L131–173）

```c
static spinlock_t mcupm_mbox_lock[MCUPM_MBOX_TOTAL];

int mcupm_mbox_read(unsigned int mbox, unsigned int slot, void *buf,
                    unsigned int len)
{
    if (WARN_ON(len > (mcupm_mboxdev.pin_send_table[mbox]).msg_size)) {
        dev_err(&mcupm_pdev->dev, "mbox:%u warning\n", mbox);
        return -EINVAL;
    }
    return mtk_mbox_read(&mcupm_mboxdev, mbox, slot,
                         buf, len * MBOX_SLOT_SIZE);
}

int mcupm_mbox_write(unsigned int mbox, unsigned int slot, void *buf,
                     unsigned int len)
{
    unsigned long flags;
    ...
    if (WARN_ON(len > (mcupm_mboxdev.pin_send_table[mbox]).msg_size) ||
        WARN_ON(!buf)) {
        ...
    }
    spin_lock_irqsave(&mcupm_mbox_lock[mbox], flags);
    ...
}
```

### 提問

> 「這兩個 helper 是 driver 對外暴露的 API，會被其他子系統呼叫。
> 你看了之後最擔心什麼？如果 `mbox = 9999` 會發生什麼事？」

### 期望回答方向

- `mbox` 是 `unsigned int`，函式**完全沒有檢查上界**。
- 第一次 deref 就是 `pin_send_table[mbox].msg_size` → array OOB read，
  讀到的 `msg_size` 是隨機值，後續 `len > msg_size` 的檢查直接失效。
- 接著 `mcupm_mbox_lock[mbox]` 也是 OOB → spinlock 拿到一塊隨機記憶體當
  鎖，**會破壞 kernel state、製造 silent corruption**。
- 雖然 caller 多半在 driver 內部，但這是「對外 export 的 helper」，
  防禦性檢查是必要的（就像 syscall 一定要 sanity check user 傳進來的值）。

### 追問

1. 你會怎麼修？只加 `if (mbox >= MCUPM_MBOX_TOTAL)` 嗎？
2. `WARN_ON` 在這裡用得對不對？（會 dump stack，但繼續執行 `dev_err` 然後
   return，OK；但若 `WARN_ON` 條件本身就 OOB，stack trace 拿到的也是受
   汙染的狀態。）
3. 為什麼 `mcupm_mbox_lock` 用 static array 而不是動態 alloc？
   兩種選擇的 trade-off？

### 真正在考什麼

- Defensive programming for kernel-exposed APIs。
- 對 array indexing 的本能警覺。
- `WARN_ON` 與 `BUG_ON` 的使用情境差異。

---

## Q2. `mcupm_pdev` 全域變數的 use-after-free 風險

### 程式碼（`mcupm_driver.c` L20, L135, L152, L185, L208）

```c
static struct platform_device *mcupm_pdev;
...
int mcupm_mbox_read(...)
{
    ...
    dev_err(&mcupm_pdev->dev, "mbox:%u warning\n", mbox);  // L135
    ...
}

static int mcupm_device_probe(struct platform_device *pdev)
{
    ...
    mcupm_pdev = pdev;            // L185：probe 時記下來
    ret = mtk_ipi_init(pdev);
    ...
}

static int mcupm_device_remove(struct platform_device *pdev)
{
    /* TODO: implement remove ipi interface and memory */
    mcupm_plt_module_exit(&pdev->dev);
    return 0;                      // ← 沒有把 mcupm_pdev 設回 NULL
}
```

### 提問

> 「這個 `mcupm_pdev` 全域變數的生命週期 OK 嗎？
> 我把 driver `rmmod` 之後再 `insmod` 會發生什麼？」

### 期望回答方向

- `remove()` 沒有清掉 `mcupm_pdev`，也沒有 unregister IPI / 釋放 mailbox。
- 而 `mcupm_mbox_read/write` 是 export 出去的 API，其他模組可能在
  remove 之後仍持有指標並繼續呼叫 → **use-after-free**。
- `dev_err(&mcupm_pdev->dev, ...)` 在 remove 後是 dangling pointer。
- 註解 `/* TODO: implement remove ipi interface and memory */` 自己已經
  承認 remove path 沒做完。

### 追問

1. 你會怎麼設計 lifecycle？引入 reference counting？rcu？或乾脆把
   helper API 改成需要 caller 傳 device handle 進來？
2. 這個 driver 有 `MODULE_LICENSE("GPL")`，如果改成 builtin 是不是就沒事？
   （錯誤答案：仍然會在 system-level shutdown / unbind 時出問題）
3. 有沒有 modern kernel 設計可以避免這種「全域 pdev」反 pattern？
   （`devm_*`、`module_platform_driver` + per-device data）

### 真正在考什麼

- Driver lifecycle (probe/remove) 的對稱性。
- Kernel global state 的危險。
- 對 `// TODO` 留言的態度（不要當沒看到）。

---

## Q3. `unsigned int` 函式 return `-1`

### 程式碼（`mcupm_timesync.c` L159–168）

```c
unsigned int mcupm_timesync_init(struct device *dev)
{
    u64 wrap;

    timesync_workqueue = create_workqueue("mcupm_ts_wq");
    if (!timesync_workqueue) {
        dev_err(dev, "%s workqueue create failed\n", __func__);
        timesync_ctx.enabled = 0;
        return -1;                  // ← 注意這裡
    }
    ...
    return 0;
}
```

caller (`mcupm_driver.c` L201)：

```c
ret = mcupm_timesync_init(dev);
if (ret) {
    dev_err(dev, "MCUPM timesync init fail\n");
    return ret;
}
```

### 提問

> 「這個函式簽名跟 return value 你看了有沒有覺得哪裡怪怪的？」

### 期望回答方向

- 函式宣告為 `unsigned int`，但 return `-1`。
- C 語言會把 `-1` 隱式轉成 `0xFFFFFFFF`，**仍然是非零**，所以
  `if (ret)` 這個 caller 剛好還能抓到「失敗」這件事；但是：
  - probe 把 `0xFFFFFFFF` 當 `int` 回傳給上層 → kernel framework 收到
    一個亂七八糟的 errno；
  - 任何用 `IS_ERR`、`PTR_ERR` 或 `< 0` 判斷的程式碼都會壞掉；
  - static analyzer / sparse 會直接亮燈。
- 正確做法：
  - 簽名改成 `int`；
  - return 標準 errno：`-ENOMEM`（因為 workqueue 建立失敗本質就是 OOM）。

### 追問

1. Linux kernel 對 errno 的 convention 是什麼？（`-Exxx`，正值通常代表成功
   時的 byte count 或 length）
2. 為什麼 kernel 喜歡用 `IS_ERR_OR_NULL`？跟這題有什麼關係？
3. 你有沒有看過 `unsigned long` 被當 errno 回傳的真實案例？怎麼救？

### 真正在考什麼

- Type safety 與 kernel idiom。
- 對「靠運氣能跑」的 code 保持警戒。

---

## Q4. `free_page()` 被用在不是 page allocator 拿來的 buffer

### 程式碼（`mcupm_plt.c` L60–81）

```c
static ssize_t mcdi_dynamic_finegrain_store(struct device *kobj,
    struct device_attribute *attr, const char *buf, size_t n)
{
    struct mcupm_ipi_data_s mcdi_ipi_data;
    int ret = 0;

    mcdi_ipi_data.cmd = 0xB1;

    ret = kstrtou32(buf, 0, &mcdi_ipi_data.u.logger.enable);
    if (ret != 0) {
        free_page((unsigned long)buf);   // ← 這行
        return -EINVAL;
    }
    ...
}
```

### 提問

> 「這段是一個 sysfs `_store` callback。寫 `free_page((unsigned long)buf)`
> 之後再 return `-EINVAL`，你覺得會發生什麼事？」

### 期望回答方向

- `buf` 是 sysfs framework 傳進來的 kernel buffer，由 sysfs core
  管理，**不是 caller 自己 `__get_free_page()` 來的**。
- `free_page` 把這個位址視為一個 page 還回 buddy allocator → 把
  別人正在用的 page 釋放掉，**幾乎保證 kernel panic 或 silent corruption**。
- 而且 `buf` 是 `const char *`，被強制 cast 成 `unsigned long` 也是 red flag。
- 正確：直接 `return -EINVAL;` 就好，**什麼都不要釋放**。

### 追問

1. `kfree(buf)` 在這裡會比 `free_page` 安全嗎？（仍然錯，因為 buf 不是
   你 alloc 的）
2. 怎麼判斷一個 pointer「歸誰擁有」？ownership 在 kernel API 設計上怎麼表達？
3. `const char *` 被 cast 拿掉 const，這件事在 review 時的 alarm level？

### 真正在考什麼

- 對「pointer ownership」最基本的概念。
- 對 `free_*` 系列 API 的記憶體配置語意。
- 一眼看出 `(unsigned long) const char *` 這種強制轉換的 code smell。

---

## Q5. `mcupm_plt_ackdata` 全域變數的 race condition

### 程式碼（`mcupm_driver.c` L23 + `mcupm_plt.c`）

```c
/* mcupm_driver.c */
int mcupm_plt_ackdata;

ret = mtk_ipi_register(&mcupm_ipidev, CH_S_PLATFORM, NULL, NULL,
                       (void *) &mcupm_plt_ackdata);
```

```c
/* mcupm_plt.c */
extern int mcupm_plt_ackdata;

static ssize_t mcupm_alive_show(struct device *kobj,
                                struct device_attribute *attr, char *buf)
{
    struct mcupm_ipi_data_s ipi_data;
    int ret = 0;

    ipi_data.cmd = 0xDEAD;
    mcupm_plt_ackdata = 0;
    ret = mtk_ipi_send_compl(&mcupm_ipidev, CH_S_PLATFORM, IPI_SEND_WAIT,
                             &ipi_data, ..., 2000);

    return snprintf(buf, PAGE_SIZE, "%s MBOX%d ret=%d XXX\n",
                    mcupm_plt_ackdata ? "Alive" : "Dead",
                    CH_S_PLATFORM, ret);
}
```

### 提問

> 「假設兩個 user 同時 `cat /sys/.../mcupm_alive` 跟
> `cat /sys/.../mcdi_dynamic_finegrain`，你預期會發生什麼？」

### 期望回答方向

- `mcupm_plt_ackdata` 是**沒有任何鎖保護的 global**，被三個地方同時讀寫：
  - sysfs `mcupm_alive_show`
  - sysfs `mcdi_dynamic_finegrain_show`
  - IPI ack callback（在 IPI handler context 寫值）
- 兩條 sysfs 路徑都先寫 `= 0` 再讀 → **race**：
  - 讀者 A 把它清成 0、開始等 ack，讀者 B 也清成 0，A 拿到的值會被 B 蓋掉。
  - IPI handler 在中間任何時間點都可能寫入。
- 後果：`Alive` / `Dead` 字串完全不可靠；不是 functional bug 卻是診斷工具
  失效，更糟糕的是 `0xDEAD` 跟 `0xA1` 的 ack 互相覆蓋。
- 修法：
  - per-channel ackdata（不要共用一個 global）；
  - 用 `mutex` 序列化 sysfs callback；
  - ack 用 `completion` 物件而不是「sentinel int」。

### 追問

1. 為什麼 kernel 開發者很討厭 sentinel pattern？哪些 API 已經把這種
   pattern 包好了？（`completion`, `wait_event_*`）
2. sysfs 對 attribute callback 有沒有自帶 serialization？（沒有）
3. `READ_ONCE` / `WRITE_ONCE` 能不能解決？（解決 tearing 但不解決 logical
   race）

### 真正在考什麼

- Concurrency 直覺。
- 對 kernel 同步 primitive (`completion`, `mutex`, `atomic_t`) 的選型品味。
- 找出「能 work，但意義不可靠」的 code 的能力。

---

## Q6. Probe 失敗時資源沒被 unwound

### 程式碼（`mcupm_driver.c` L52–105）

```c
ret = mtk_mbox_probe(pdev, mbox_dev, 0);
if (ret) {
    dev_err(...);
    return -EINVAL;             // ← early return
}

for (i = 1; i < mbox_total_number; i++) {
    ...
    base = devm_ioremap(...);
    if (IS_ERR(base)) {
        ret = PTR_ERR(base);
        ...
        return ret;             // ← 中途失敗
    }
    ret = mtk_smem_init(pdev, mbox_dev, i, base, ...);
    if (ret) {
        ...
        return -EINVAL;         // ← 同樣中途失敗
    }
}

ret = mtk_ipi_device_register(...);
if (ret) { ... return -EINVAL; }
ret = mtk_ipi_register(...);
if (ret) { ... return -EINVAL; }
```

### 提問

> 「probe 失敗的 path 你覺得 OK 嗎？哪些 resource 是 leak 的？哪些不是？」

### 期望回答方向

- `devm_ioremap*` 是 device-managed，**probe 失敗時 driver core 會自動
  釋放**，這部分 OK。
- 但 `mtk_mbox_probe`、`mtk_smem_init`、`mtk_ipi_device_register`、
  `mtk_ipi_register` 都**不是** devm，需要呼叫對應的 unregister / cleanup。
  - 如果 `mtk_smem_init` 在 i=5 失敗，那 i=1..4 已經 init 完的 smem
    完全沒被 teardown → leak、也可能讓 MCU side 拿到半初始化狀態。
  - `mtk_ipi_device_register` 失敗時，前面成功的 mbox / smem 都沒回收。
- 標準 kernel pattern：goto err1/err2/err3 倒序 cleanup，或一律改 devm 包裝。
- 還要注意：`return -EINVAL` 把底層真正的 errno（例如 `-ENODEV`、
  `-ENOMEM`）覆蓋掉了，丟失資訊。

### 追問

1. 寫一段「黃金標準」的 cleanup label code 給我看。
2. 為什麼 kernel 偏好 `goto err`，而不是寫多層 if？
3. 何時值得自己 wrapping 一層 `devm_*` helper？

### 真正在考什麼

- Error path / cleanup 的 sense。
- `devm_*` 與非 devm API 的混用紀律。
- 不要「把 errno 翻譯成 -EINVAL」這種反 pattern。

---

## Q7. Legacy DT 分支下 `offset` / `res_size` 未初始化就被印出

### 程式碼（`mcupm_driver.c` L58–93）

```c
for (i = 1; i < mbox_total_number; i++) {
    struct resource *res;
    resource_size_t offset, res_size;     // ← 注意：未初始化
    void __iomem *base = NULL;
    ...

    if (legacy_mbox_dt == false) {
        res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mbox0_base");
        ...
        res_size = resource_size(res);
        offset = res->start + (i * res_size);
        base = devm_ioremap(dev, offset, res_size);
    } else { /* legacy mbox */
        snprintf(name, sizeof(name), "mbox%d_base", i);
        res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);
        ...
        base = devm_ioremap_resource(dev, res);   // legacy 分支沒設 offset/res_size
    }

    if (IS_ERR(base)) {
        ret = PTR_ERR(base);
        dev_err(dev, "mbox-%d can't remap(%pa) size=%pa ret=%d\n", i,
                &offset,        // ← legacy 分支進來時，offset 是未初始化值
                &res_size,      // ← 同上
                ret);
        return ret;
    }
    ...
}
```

### 提問

> 「你用 legacy DT 跑這段，devm_ioremap_resource 失敗了，dmesg 會印什麼？」

### 期望回答方向

- legacy 分支沒有設定 `offset`、`res_size`，是 stack 上的 garbage。
- 失敗時 `dev_err(... "%pa ... %pa", &offset, &res_size, ...)`
  把 garbage 印出去 → undefined behaviour，輕則誤導 debug，重則被 sparse
  / coverity 標出來。
- 正確做法：
  - 把 print 改成只在 non-legacy 分支裡才印 offset/size；
  - 或者統一在 legacy 分支也設好 `offset = res->start; res_size =
    resource_size(res);`。

### 追問

1. `%pa` format specifier 的型別是什麼？傳錯指標型別會發生什麼？
   （UB；32-bit 跟 64-bit 也不一樣）
2. 這種 bug 為什麼 compiler 抓不到？要靠什麼工具？
3. 你會在 review 時怎麼一眼掃出這類「分支沒寫好初始化」的 bug？

### 真正在考什麼

- 對 control-flow 與 variable initialization 的細心。
- Format specifier 的正確使用。
- Tooling awareness（sparse, smatch, coverity）。

---

## Q8. `mcupm_mbox_write` 的 lock 釋放後又 write，window 過小但仍是 race

### 程式碼（`mcupm_driver.c` L143–173）

```c
int mcupm_mbox_write(unsigned int mbox, unsigned int slot, void *buf,
                     unsigned int len)
{
    unsigned long flags;
    unsigned int status;
    int ret;
    ...
    spin_lock_irqsave(&mcupm_mbox_lock[mbox], flags);
    status = mtk_mbox_check_send_irq(&mcupm_mboxdev, mbox,
                  (mcupm_mboxdev.pin_send_table[mbox]).pin_index);
    if (status != 0) {
        spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);
        return MBOX_PIN_BUSY;
    }
    spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);   // ← 先解鎖

    ret = mtk_mbox_write(&mcupm_mboxdev, mbox, slot,         // ← 再寫
                         buf, len * MBOX_SLOT_SIZE);
    if (ret != MBOX_DONE)
        return ret;
    mb();
    return 0;
}
```

### 提問

> 「這個鎖到底在保護什麼？check_send_irq 跟 mbox_write 中間放一個
> unlock，你覺得 race 模型是什麼？」

### 期望回答方向

- 直覺：`check_send_irq` 是檢查「mbox 是否 busy」的 sentinel，
  然後 `mbox_write` 真的把 data 寫進 SRAM 並 trigger IRQ。
- 但中間 unlock 一下，讓另一個 caller 也通過 `check_send_irq == 0` 的
  檢查 → **TOCTOU**：兩個 writer 都覺得自己有權寫入，會踩寫 mailbox 上的
  data。
- 正確：lock 應該包到 `mtk_mbox_write` 結束。
- 進階問題：這把 spinlock 是不是該換成 mutex？要看 `mtk_mbox_write` 是否
  可能 sleep（看起來是 SRAM IO，不會 sleep，spinlock 可）。

### 追問

1. spinlock 的 critical section 應該越短越好，但「短到變成兩段」就破壞語意。
   怎麼平衡？
2. 如果底層 `mtk_mbox_write` 有自己的鎖呢？這層鎖還有意義嗎？
3. `mb()` 在 spin_unlock 之後是否多餘？（spin_unlock 已含 release barrier）

### 真正在考什麼

- TOCTOU pattern 識別。
- Critical section 設計。
- Memory barrier 與 lock 的互動。

---

## Q9. `mcupm_ts_update` 的「version + freeze header」協定設計

### 程式碼（`mcupm_timesync.c` L48–87）

```c
static void mcupm_ts_update(int suspended, u64 tick, u64 ts)
{
    u32 header, val;

    mcupm_base_ver = (mcupm_base_ver + 1) % (TIMESYNC_MAX_VER + 1);

    header = suspended ? TIMESYNC_HEADER_FREEZE : 0;
    header |= ((mcupm_base_ver << TIMESYNC_HEADER_VER_OFS) &
               TIMESYNC_HEADER_VER_MASK);

    /* update tick, h -> l */
    val = (tick >> 32) & TIMESYNC_MASK_32_BITS;
    val |= header;
    mcupm_ts_write(MCUPM_TS_MBOX_TICK_H, val);
    mb();

    val = tick & TIMESYNC_MASK_32_BITS;
    mcupm_ts_write(MCUPM_TS_MBOX_TICK_L, val);
    mb();

    /* update ts, l -> h */
    val = ts & TIMESYNC_MASK_32_BITS;
    mcupm_ts_write(MCUPM_TS_MBOX_TS_L, val);
    mb();

    val = (ts >> 32) & TIMESYNC_MASK_32_BITS;
    val |= header;
    mcupm_ts_write(MCUPM_TS_MBOX_TS_H, val);
    mb();
}
```

### 提問

> 「這段把 64-bit tick / ts 拆成 4 個 32-bit slot 寫到 mailbox，
> header 帶一個 version。你來設計 MCU 端的讀取邏輯，會怎麼讀？
> 這個 protocol 有沒有漏洞？」

### 期望回答方向

- 設計意圖：讓 MCU 在讀 4 個 slot 的時候，能用 header 上的 version 判斷
  AP 有沒有寫到一半 → 「seqlock 風格」的 reader-side validation。
- 但這個實作有問題：
  - **TICK_H 跟 TS_H 都帶相同 version**，TICK_L、TS_L 不帶 → 中間 race
    時 reader 看不到 partial-update。
  - `mcupm_base_ver` 是普通 `u8` 全域變數，沒鎖保護；雖然
    `timesync_sync_base_internal` 外層有 spinlock，但只要有任何 path
    繞過 spinlock 就壞掉。
  - 7-bit version (3-bit header) 上限是 8 → wrap 很快，reader 必須能容忍 wrap。
  - 寫入順序：tick(H→L)、ts(L→H)。**結尾 TS_H 帶新 version**，所以正確的
    MCU 讀法是：先讀 TS_H 取得 version，再讀 TS_L、TICK_L、TICK_H，
    最後重新讀 TS_H 比對 version 是否一致；不一致就重試。
  - 但 code 裡沒任何註解講這個 contract，**靠 4 個 `mb()` 強制順序卻沒解
    釋為什麼順序是這個**，這是 review 的重大缺陷。
- `mb()` 是 full barrier 對 SRAM-mapped IO 不見得是必要的（要看 mailbox
  HW 的 ordering 保證）；可能用 `wmb()` 就夠。

### 追問

1. 你覺得這個 protocol 像哪個 Linux kernel pattern？（seqlock）
2. 怎麼把這個改成 single-slot 64-bit atomic write？硬體能不能做？
3. 假如 AP 寫到 TS_L 之後 crash，下次 boot 時 MCU 看到什麼？怎麼設計
   能讓 MCU 偵測到 stale data？

### 真正在考什麼

- 跨處理器 protocol design。
- Memory barrier 的 mental model。
- 把「沒 doc 的 binary protocol」逆向出意圖的能力。

---

## Q10. `create_workqueue` 的選用 + suspend/resume 對稱性

### 程式碼（`mcupm_timesync.c` L159–233）

```c
unsigned int mcupm_timesync_init(struct device *dev)
{
    ...
    timesync_workqueue = create_workqueue("mcupm_ts_wq");
    if (!timesync_workqueue) { ... return -1; }
    ...
    timesync_ctx.enabled = 1;
    timesync_sync_base(TIMESYNC_FLAG_SYNC);
    return 0;
}

void mcupm_timesync_suspend(void)
{
    if (!timesync_ctx.enabled)
        return;

    hrtimer_cancel(&timesync_refresh_timer);
    timesync_sync_base(TIMESYNC_FLAG_SYNC | TIMESYNC_FLAG_FREEZE);
}

void mcupm_timesync_resume(void)
{
    if (!timesync_ctx.enabled)
        return;

    timecounter_init(&timesync_counter, &timesync_cc, sched_clock());
    hrtimer_start(&timesync_refresh_timer,
                  timesync_ctx.wrap_kt, HRTIMER_MODE_REL);
    timesync_sync_base(TIMESYNC_FLAG_SYNC | TIMESYNC_FLAG_UNFREEZE);
}
```

### 提問

> 「這段 init / suspend / resume 你看得出哪幾個對稱性問題？」

### 期望回答方向

- **API 選擇**：`create_workqueue` 已經是 deprecated，新 code 應該用
  `alloc_workqueue("mcupm_ts_wq", WQ_*, max_active)`，明確指定 flag
  （是不是 `WQ_FREEZABLE`、`WQ_MEM_RECLAIM`、`WQ_UNBOUND`）。
- **沒有對應的 destroy**：`mcupm_timesync` 沒有 `_exit`，driver remove
  時 workqueue 與 hrtimer 都不會被 destroy → leak + dangling callback。
- **Suspend / resume 路徑**：
  - Suspend 只 cancel hrtimer，沒 flush workqueue → 已 queue 的 work
    可能在 freeze 之後還跑，把 freeze 狀態又改掉。
  - Resume 直接 `hrtimer_start`，沒檢查 hrtimer 是否還活著（如果中間
    被 driver remove 過會 oops）。
- `timesync_ctx.enabled` 是普通 `u8`，多 CPU 之間沒 barrier，
  suspend/resume 路徑與 work callback 之間的 visibility 要靠 luck。
- 還有：`mcupm_timesync_init` 在 `timesync_sync_base(SYNC)` 前才把
  `enabled = 1`，但 `timesync_sync_base` 裡是
  `if (!timesync_ctx.enabled) return;` → init 結尾這次同步沒問題；但 race
  視角下，hrtimer 可能在 enabled = 1 那一瞬間就被 fire 了（因為前面已經
  `hrtimer_start` 過 wrap 時間），這也 OK，但要 defend 起來不容易。

### 追問

1. `create_workqueue` 跟 `alloc_workqueue` 差別？什麼時候要 `WQ_FREEZABLE`？
2. 為什麼 suspend path 通常要 `flush_workqueue`？跟這題的 `freeze` 語意衝突嗎？
3. hrtimer + workqueue + suspend 三件事的標準 pattern 是？

### 真正在考什麼

- Modern kernel API 知識。
- 對「init 跟 exit / suspend 跟 resume / start 跟 stop」的對稱思維。
- 對 freeze / thaw / hibernation 流程的理解。

---

## Q11.（bonus）Module exit 為什麼整段都是空的？

### 程式碼

```c
/* mcupm_driver.c */
static int mcupm_device_remove(struct platform_device *pdev)
{
    /* TODO: implement remove ipi interface and memory */
    mcupm_plt_module_exit(&pdev->dev);
    return 0;
}

/* mcupm_plt.c */
void mcupm_plt_module_exit(struct device *dev)
{
    /* TODO: release resource */
    dev_dbg(dev, "[MCUPM] mcupm plt module exit.\n");
}
```

### 提問

> 「這個 driver 是 builtin 還是 module？exit path 完全沒做事，你會 LGTM 嗎？」

### 期望回答方向

- 兩個 `// TODO` 自爆：作者知道沒做完。
- 即使現在是 builtin，也不能假設未來不會被改成 module。
- Code review 的態度：**TODO 要不就動手做、要不就立 ticket，不能
  等到使用者出事**。
- 至少要 unregister IPI、destroy workqueue、cancel hrtimer、清 `mcupm_pdev`。

### 真正在考什麼

- 對 `// TODO` 的職業敏感度。
- 區分「能 build pass」與「能上 production」的差距。

---

## 額外加分：你 review 時會怎麼下評論？

| 等級 | 範例 |
|------|------|
| ❌ NACK | "Q1 / Q4 必須修：array OOB 跟 `free_page` mis-use 都是 kernel panic 級。" |
| 🟡 Major | "Q2 / Q5 / Q6 / Q9 / Q10 是設計缺陷，要 follow-up patch。" |
| 🟢 Nit | "Q3 / Q7 / Q11 比較像 hygiene；可以這個 patch 一起改，也可以 follow-up。" |

實際面試時，能把這個分類講清楚會比指出 bug 本身更加分——主管要的是
「你能不能 own 一個 review process」，不是「你能不能挑 bug」。

---

## 繁體中文 Review 自我檢查清單

- [ ] 看到 `unsigned int idx` 立刻檢查 array 邊界。
- [ ] 看到 global state 立刻問 lifetime / locking。
- [ ] 看到 `unsigned` 函式 return 負值立刻起疑。
- [ ] 看到 `(unsigned long) buf` 強制 cast 立刻檢查 buffer ownership。
- [ ] 看到 sysfs callback 用 global ack 立刻檢查 race。
- [ ] 看到 probe path 多個 early return 立刻檢查 cleanup。
- [ ] 看到分支內初始化變數立刻檢查所有分支。
- [ ] 看到 lock/unlock 中間還有共享狀態存取立刻檢查 TOCTOU。
- [ ] 看到 mailbox / IPI / shared memory 立刻檢查 protocol 文件 + barrier。
- [ ] 看到 deprecated API（`create_workqueue` 等）立刻提替代。
- [ ] 看到 `// TODO` 立刻問 owner / ETA。

---

## Sources

- `drivers/remoteproc/mcupm/v2/mcupm_driver.c`
- `drivers/remoteproc/mcupm/v2/mcupm_plt.c`
- `drivers/remoteproc/mcupm/v2/mcupm_timesync.c`
- `drivers/remoteproc/mcupm/include/mcupm_driver.h`
- `drivers/remoteproc/mcupm/include/mcupm_plt.h`
- `drivers/remoteproc/mcupm/include/mcupm_timesync.h`
