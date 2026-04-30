# Fri-4. Code Review Round — 解答檔

> 來源：`weekly-mock-bank.md` 第 827–856 行的 **Fri-4. Code Review Round（20 min）**
>
> 本檔案：
> 1. 將原始 LRU snapshot 風格代碼 **逐行加上繁體中文註解**，清楚標示每一行的用途與「地雷點」。
> 2. 依照面試官備忘提出的 **4 個問題**（並補上額外幾個常被點名的 issue），給出修正後的完整版本，同樣逐行加上繁體中文註解。
>
> 目標讀者：Android/Linux kernel 面試自我演練用。本題重點 = **atomic context 規則**（spinlock 不可睡）+ **per-CPU 正確性**（preempt 與索引）+ **API 設計**（buffer 與長度的 contract）。
>
> 搭配閱讀：[[kernel/spinlock]]、[[kernel/lru]]、[[interview/code-review-questions]]。

---

## 一、原始代碼（逐行繁體中文註解＋問題標註）

下列代碼每一行後方以 `//` 或獨立註解解說用途；其中有地雷的行會標上 **🔴 問題 N**，對應第二節的修正。

```c
/* ==============================================================
 *  lru_get_inactive_pages —— 取出某 CPU 的 inactive LRU 頁面快照
 *  （原始版本：示範用，含多個地雷）
 * ============================================================== */

int *lru_get_inactive_pages(int cpu)              // 🔴 問題 5：cpu 由 caller 傳入，但下面馬上拿來索引 per-CPU 陣列
                                                  //          若 caller 不是在停 preempt 的情境取得這個 cpu 編號
                                                  //          進入函式後執行緒可能已被搬到別的 CPU → 索引到錯的 list
                                                  // 🔴 問題 6：回傳 int* 但 PFN 在 64-bit kernel 是 unsigned long
                                                  //          這裡用 int 會截斷 high bits，PFN > 2^31 直接壞掉
                                                  // 🔴 問題 7：沒回傳「實際填了幾個」，caller 只能假設 1024，
                                                  //          但若 list 不滿 1024，後面的 element 是未初始化（kmalloc 沒清零）
{
    spin_lock(&lru_lock);                         // 🔴 問題 1（致命）：先拿 spinlock → 進入 atomic context
                                                  //          atomic context 內絕對不能睡眠

    int *pages = kmalloc(1024 * sizeof(int),      // 🔴 問題 1（致命）：spinlock 內呼叫 GFP_KERNEL 分配 → 可能睡眠
                         GFP_KERNEL);             //          Lockdep 會噴 "BUG: sleeping function called from invalid context"
                                                  //          應該先在 lock 外 kmalloc，再進 critical section 拷資料
                                                  //
                                                  // 🔴 問題 2：1024 * sizeof(int) 是裸乘法
                                                  //          這裡因為 1024 是常數溢位風險低，但 review 仍應改 kmalloc_array
                                                  //          一致性比「這次安全」重要 —— 風格一致才不會某天變數化後出事
                                                  //
                                                  // 🔴 問題 3：1024 是 magic number
                                                  //          為什麼是 1024？文件 / 註解都沒交代
                                                  //          應該 #define LRU_SNAPSHOT_MAX 1024，或開放參數讓 caller 指定

    if (!pages) {                                 // 分配失敗的處理
        spin_unlock(&lru_lock);                   // 釋放鎖
        return NULL;                              // 🔴 問題 8：回 NULL 表示「失敗」，但成功路徑也可能因 list 空而看起來像沒資料
                                                  //          建議用 ERR_PTR(-ENOMEM) 與 IS_ERR / PTR_ERR 搭配
    }

    int i = 0;                                    // 🔴 問題 9：宣告與程式碼混用（C99 OK，但 kernel 風格偏好頂部宣告）
    struct page *p;                               // 迴圈用游標

    list_for_each_entry(p, &inactive_list[cpu], lru) {   // 🔴 問題 4：inactive_list[cpu] 的 per-CPU 設計是否正確？
                                                          //          (a) 若它其實是 per-NUMA node，這裡 cpu 索引就錯
                                                          //          (b) 即便真的是 per-CPU，也要先 get_cpu()/put_cpu()
                                                          //              或在 caller 端關 preempt，否則 cpu 變數可能已過時
                                                          //          (c) 走訪 list 的同時若沒拿對的 lock，list 可能正被改動

        pages[i++] = page_to_pfn(p);              // 🔴 問題 6（重申）：page_to_pfn 回傳 unsigned long
                                                  //          指派到 int*[i] 會 implicit truncation，64-bit 系統易爆
        if (i >= 1024) break;                     // 🔴 問題 3（重申）：又一個 magic 1024
    }

    spin_unlock(&lru_lock);                       // 釋放 spinlock
    return pages;                                 // 🔴 問題 7（重申）：caller 不知道實際 PFN 數
                                                  //          只能假設 1024，越界讀到 garbage 是常見後果
}
```

### 面試官備忘整理（對應修正清單）

| # | 位置 | 問題 | 修正方向 |
|---|------|------|---------|
| 1 | `spin_lock` 之後 `kmalloc(GFP_KERNEL)` | atomic context 不可睡 | 把分配移到 `spin_lock` 之前；或在 lock 內改 `GFP_ATOMIC`（但會增加分配失敗率，較不推薦） |
| 2 | `kmalloc(1024 * sizeof(int), …)` | 一致性：應使用 `kmalloc_array` | 改 `kmalloc_array(LRU_SNAPSHOT_MAX, sizeof(*pages), GFP_KERNEL)` |
| 3 | 寫死 `1024` | magic number、可讀性差 | `#define LRU_SNAPSHOT_MAX 1024` 或開放成參數 |
| 4 | `inactive_list[cpu]` | per-CPU 索引 + preempt 安全 | 用 `get_cpu()/put_cpu()` 或 `this_cpu_ptr()`；確認資料結構真的是 per-CPU；走訪期間持有對應 lock |
| 5 | `int cpu` 由 caller 傳 | caller 拿到 cpu 後可能已被搬走 | 使用 `get_cpu()` 在函式內鎖定當前 CPU；若一定要外部給 cpu，文件需註明前置條件 |
| 6 | `int *pages` / `int` 存 pfn | 64-bit 截斷 | 改 `unsigned long *` |
| 7 | 沒回傳實際長度 | caller 不知道有多少有效元素 | 加 out 參數 `size_t *out_n`；或回傳 `ssize_t`（≥0 為長度，<0 為 -errno） |
| 8 | 失敗時回 `NULL` | 與「成功但空」混淆 | 用 `ERR_PTR(-ENOMEM)` + `IS_ERR/PTR_ERR` |
| 9 | 宣告與程式碼混用 | kernel 風格 | 頂部宣告（kernel 慣例；C90 風格更嚴格） |

---

## 二、修正後代碼（逐行繁體中文註解）

```c
#include <linux/slab.h>           // kmalloc_array / kfree
#include <linux/spinlock.h>       // spin_lock / spin_unlock
#include <linux/list.h>           // list_for_each_entry
#include <linux/mm.h>             // struct page / page_to_pfn
#include <linux/smp.h>            // get_cpu / put_cpu / this_cpu_ptr
#include <linux/percpu.h>         // DEFINE_PER_CPU / per_cpu
#include <linux/err.h>            // ERR_PTR / PTR_ERR / IS_ERR
#include <linux/errno.h>          // -ENOMEM / -EINVAL
#include <linux/types.h>          // size_t / ssize_t
#include <linux/printk.h>         // pr_warn / pr_info

/* ==============================================================
 *  常數與 per-CPU 結構
 *
 *  ✅ 修正 #3：把 magic number 提升為具名常數，集中維護。
 *  ✅ 修正 #4：明確宣告 inactive_list 為 per-CPU，並各自帶一把 lock，
 *             避免「全域 lru_lock 保護所有 CPU 的 list」造成的 cache 抖動。
 * ============================================================== */

#define LRU_SNAPSHOT_MAX  1024U                    // 一次最多取 1024 筆 PFN（含義寫在名字裡）

struct lru_cpu {                                   // 每個 CPU 一份 LRU 元資料
    spinlock_t       lock;                         // 保護自己這份 list（per-CPU lock，不再爭全域鎖）
    struct list_head inactive_list;                // inactive LRU 鏈結串列頭
};

static DEFINE_PER_CPU(struct lru_cpu, lru_cpu);    // 真正的 per-CPU 變數（每 CPU 一份）

/* ==============================================================
 *  lru_get_inactive_pages —— 取出「目前 CPU」的 inactive LRU 頁面快照（修正版）
 *
 *  Context: process context, may sleep（GFP_KERNEL）。
 *           本函式內部使用 get_cpu()，呼叫者不需另外關 preempt。
 *
 *  參數：
 *      max     —— caller 期望最多取多少筆（會與 LRU_SNAPSHOT_MAX 取 min）
 *      out_n   —— 出參，回傳實際填入幾筆
 *
 *  Return：
 *      成功 → 指向 unsigned long 陣列（PFN），caller 用完必呼叫 kfree。
 *      失敗 → ERR_PTR(-ENOMEM)、ERR_PTR(-EINVAL) 等。       // ✅ 修正 #8
 * ============================================================== */

unsigned long *lru_get_inactive_pages(size_t max,        // ✅ 修正 #5：不再讓 caller 傳 cpu，內部自取
                                      size_t *out_n)
{
    unsigned long *pages;                          // ✅ 修正 #6：PFN 必須是 unsigned long（64-bit 安全）
    struct lru_cpu *lc;                            // 指向當前 CPU 的 lru_cpu 結構
    struct page    *p;                             // 走訪游標
    size_t          i = 0;                         // 已填入元素數
    int             cpu;                           // 用於解 get_cpu() 的 token

    might_sleep();                                 // 標示「本函式可能睡」（給 lockdep 抓誤用）

    if (!out_n || max == 0)                        // 參數防呆
        return ERR_PTR(-EINVAL);

    if (max > LRU_SNAPSHOT_MAX)                    // ✅ 修正 #3：用具名常數限制上界，避免 caller 誤傳超大值
        max = LRU_SNAPSHOT_MAX;

    /* --------------------------------------------------------------
     * Step 1：先在 lock 外配置記憶體（GFP_KERNEL 可睡）。
     *          ✅ 修正 #1：絕對不要在 spinlock 內 GFP_KERNEL！
     *          ✅ 修正 #2：用 kmalloc_array 取代裸乘法（一致性 + 內建溢位偵測）。
     * -------------------------------------------------------------- */
    pages = kmalloc_array(max, sizeof(*pages), GFP_KERNEL);
    if (!pages)                                    // 分配失敗：直接回錯誤指標
        return ERR_PTR(-ENOMEM);                   // ✅ 修正 #8：用 ERR_PTR 而非 NULL

    /* --------------------------------------------------------------
     * Step 2：鎖定當前 CPU，取出 per-CPU 結構指標。
     *          ✅ 修正 #4 / #5：get_cpu() 會關 preempt，
     *          回傳的 cpu 在配對的 put_cpu() 之前不會被搬走，
     *          this_cpu_ptr() 安全地拿到「就是這顆 CPU」的資料。
     * -------------------------------------------------------------- */
    cpu = get_cpu();                               // 關 preempt + 拿到當前 cpu 編號
    lc  = this_cpu_ptr(&lru_cpu);                  // 取得當前 CPU 的 lru_cpu 結構

    /* --------------------------------------------------------------
     * Step 3：拿 per-CPU 的 spinlock，走訪 inactive_list。
     *          critical section 內只做「短、快、不睡」的事：
     *          - 讀 PFN
     *          - 寫進已分配好的 pages[]
     *          完全不碰任何可能睡的 API。
     * -------------------------------------------------------------- */
    spin_lock(&lc->lock);                          // 拿這顆 CPU 自己的 lock（不爭全域鎖）

    list_for_each_entry(p, &lc->inactive_list, lru) {
        pages[i++] = page_to_pfn(p);               // ✅ 修正 #6：unsigned long 陣列接得住 64-bit PFN
        if (i >= max)                              // ✅ 修正 #3：用變數 max，不再寫死 1024
            break;
    }

    spin_unlock(&lc->lock);                        // 盡早離開 critical section

    /* --------------------------------------------------------------
     * Step 4：解開 preempt 鎖（與 get_cpu() 配對）。
     *          注意 get_cpu() / put_cpu() 必須成對出現，
     *          且兩者之間「絕對不能睡」（spinlock 內天經地義不睡）。
     * -------------------------------------------------------------- */
    put_cpu();                                     // 開回 preempt

    *out_n = i;                                    // ✅ 修正 #7：把實際填入的數量告訴 caller
    return pages;                                  // 成功：回傳新陣列（caller 負責 kfree）
}

/* ==============================================================
 *  使用範例：呼叫端正確處理 ERR_PTR 與 out 參數
 * ============================================================== */

static void caller_example(void)
{
    size_t           n      = 0;                   // 收實際長度
    unsigned long   *snap;                         // 收 PFN 陣列
    size_t           i;

    snap = lru_get_inactive_pages(LRU_SNAPSHOT_MAX, &n);

    if (IS_ERR(snap)) {                            // ✅ 一定要先 IS_ERR，再用指標
        pr_warn("lru snapshot failed: %ld\n", PTR_ERR(snap));
        return;                                    // 錯誤路徑：snap 不是合法指標，不可 kfree
    }

    pr_info("got %zu inactive pages\n", n);
    for (i = 0; i < n; i++)
        pr_info("  pfn[%zu] = %lu\n", i, snap[i]);

    kfree(snap);                                   // ✅ 成功路徑：caller 必須 kfree
}
```

---

## 三、Atomic Context 速查（背誦版）

| 場景 | ❌ 危險寫法 | ✅ 安全寫法 |
|------|-----------|-----------|
| spinlock 內配置 | `spin_lock(); kmalloc(.., GFP_KERNEL);` | 先 `kmalloc(GFP_KERNEL)` 再 `spin_lock()`；或用 `GFP_ATOMIC`（次選） |
| 取得當前 CPU | `int cpu = smp_processor_id();`（沒關 preempt） | `cpu = get_cpu(); … put_cpu();` |
| 訪問 per-CPU 變數 | `per_cpu(x, smp_processor_id())` 裸用 | `this_cpu_ptr(&x)` 搭配 `get_cpu/put_cpu` 或 `local_irq_disable` |
| `n * sizeof(T)` 分配 | `kmalloc(n * sizeof(T), gfp)` | `kmalloc_array(n, sizeof(T), gfp)` |
| 失敗回值 | `return NULL;` | `return ERR_PTR(-ENOMEM);` 搭配 `IS_ERR` |
| 回傳長度 | 假設 caller 知道 | out 參數 `size_t *out_n` 或 `ssize_t` 回傳 |
| PFN 型別 | `int pfn` | `unsigned long pfn` |
| magic number | 寫死 `1024` | `#define LRU_SNAPSHOT_MAX 1024` |

### 五個 review 必問

1. **這段 critical section 會不會睡？** 任何 `kmalloc(GFP_KERNEL)`、`mutex_lock`、`copy_from_user`、`msleep`、`schedule()` 都不能在 spinlock / IRQ-off context 出現。
2. **per-CPU 索引前有沒有關 preempt？** `smp_processor_id()` 在 preemptible context 拿到的值馬上過時 → BUG。`get_cpu()` 是正解。
3. **資料結構真的是 per-CPU 嗎？** 名字像 per-CPU 不代表是 per-CPU；可能是 per-NUMA-node、per-zone。看 `DEFINE_PER_CPU` 才算數。
4. **alloc 大小有沒有來自外部輸入？** 有的話用 `kmalloc_array` / `struct_size` / `size_mul_overflow` 避免乘法溢位。
5. **失敗路徑會不會洩漏？** lock 漏解、buffer 漏 free、ERR_PTR 與 NULL 語意混用都是常見 bug。

---

## 四、相關 wiki

- `[[interview/Mon-3-code-review-solution]]` — list_head + spinlock + refcount 版本
- `[[interview/Tue-3-code-review-solution]]` — hash-table + RCU + kfree_rcu 版本
- `[[interview/Thu-3-code-review-solution]]` — sort + bsearch 整數安全版本
- `[[kernel/spinlock]]` — 同步原語與 atomic context 規則
- `[[kernel/lru]]` — kernel LRU 設計（多條 list、per-NUMA / per-CPU 區別）
- `[[kernel/percpu]]` — `DEFINE_PER_CPU` / `this_cpu_ptr` / `get_cpu` 慣用法
- `[[interview/code-review-questions]]` — 主管視角 atomic context / per-CPU 章節
- `[[interview/weekly-mock-bank]]` — 本題出處（Fri-4）
