# Thu-3. Code Review Round — 解答檔

> 來源：`weekly-mock-bank.md` 第 625–662 行的 **Thu-3. Code Review Round（30 min）**
>
> 本檔案：
> 1. 將原始 sort + bsearch 風格代碼 **逐行加上繁體中文註解**，清楚標示每一行的用途與「地雷點」。
> 2. 依照面試官備忘提出的 **5 個問題**，給出修正後的完整版本，同樣逐行加上繁體中文註解。
>
> 目標讀者：Android/Linux kernel 面試自我演練用。本題重點 = **整數安全**（溢位、邊界）+ **錯誤碼語意**（不要用 `-1` 混用）。
>
> 搭配閱讀：[[kernel/sort-search]]、[[interview/edge-cases]]。

---

## 一、原始代碼（逐行繁體中文註解＋問題標註）

下列代碼每一行後方以 `//` 或獨立註解解說用途；其中有地雷的行會標上 **🔴 問題 N**，對應第二節的修正。

```c
/* ==============================================================
 *  compare —— qsort/sort 的比較函式
 * ============================================================== */

int compare(const void *a, const void *b)         // qsort/sort 規定簽名：兩個 void* 進，回 int
{
    return *(int *)a - *(int *)b;                 // 🔴 問題 1：用「相減」判大小會整數溢位
                                                  //          例如 a=INT_MAX, b=INT_MIN ⇒ INT_MAX - INT_MIN 折返成負數
                                                  //          排序順序會錯亂，且是 undefined behavior（signed overflow）
}

/* ==============================================================
 *  sort_copy —— 複製陣列後排序
 * ============================================================== */

int *sort_copy(int *arr, int n)
{
    int *copy = kmalloc(n * sizeof(int),          // 🔴 問題 2：n * sizeof(int) 是 size_t 算術；若 n 來自 user，n 大到讓乘法折返
                        GFP_KERNEL);              //          會分配遠小於預期的 buffer，後面 memcpy 立即 heap overflow
                                                  //          應改用 kmalloc_array() 內建溢位檢查
    /* 🔴 問題 3：沒檢查 kmalloc 回傳 NULL —— 下一行 memcpy 會 NULL deref → kernel oops */

    memcpy(copy, arr, n * sizeof(int));           // 把原陣列拷到新 buffer
                                                  // （這裡再次 n * sizeof(int)，與 kmalloc 同一個溢位風險）

    sort(copy, n, sizeof(int), compare, NULL);    // kernel 內建 sort（heapsort，不是 qsort）
                                                  // compare 帶有 Bug #1，整體排序結果可能錯誤

    return copy;                                  // 回傳新陣列；caller 必須記得 kfree —— 但這份 contract 沒寫在文件
}

/* ==============================================================
 *  binary_search —— 在排序陣列中找 target
 * ============================================================== */

int binary_search(int *arr, int n, int target)
{
    int lo = 0, hi = n;                           // [lo, hi) 半開區間
    while (lo < hi) {                             // 終止條件：區間為空
        int mid = (lo + hi) / 2;                  // 🔴 問題 4：lo + hi 會整數溢位
                                                  //          lo, hi 接近 INT_MAX 時溢位成負，mid 變負索引 → OOB 讀
                                                  //          雖然此函式 n 通常不大，但寫成 (lo + hi) 是壞習慣，code review 必抓
        if (arr[mid] == target) return mid;       // 命中，回傳索引
        if (arr[mid] < target) lo = mid + 1;      // 目標在右半
        else hi = mid;                            // 目標在左半（hi 是 exclusive，故不 -1）
    }
    return -1;                                    // 🔴 問題 5：用 -1 表示「找不到」
                                                  //          - 與「合法錯誤碼」混用：caller 拿到 -1 不知是 not-found 還是 -EPERM
                                                  //          - 索引型回傳應該用 ssize_t、ERR_PTR、或把結果丟到 out 參數
                                                  //          - kernel 慣例：ssize_t < 0 是 -errno；或回 bool + out 索引
}
```

### 面試官備忘整理（對應修正清單）

| # | 位置 | 問題 | 修正方向 |
|---|------|------|---------|
| 1 | `compare` | 相減導致 signed overflow + UB | 改 `(x > y) - (x < y)` 或顯式 if/else 三分支 |
| 2 | `sort_copy` | `n * sizeof(int)` 乘法溢位 | 改 `kmalloc_array(n, sizeof(int), GFP_KERNEL)`，內建溢位偵測 |
| 3 | `sort_copy` | `kmalloc` 未檢查 NULL | `if (!copy) return ERR_PTR(-ENOMEM);` |
| 4 | `binary_search` | `(lo + hi) / 2` 加法溢位 | 改 `lo + (hi - lo) / 2`；變數型別建議 `size_t` |
| 5 | `binary_search` | `-1` 既是「找不到」也像錯誤碼 | 用 `ssize_t`（< 0 為 -errno），或改 bool + out 索引 |

---

## 二、修正後代碼（逐行繁體中文註解）

```c
#include <linux/slab.h>            // kmalloc / kmalloc_array / kfree
#include <linux/sort.h>            // sort()（kernel 內建 heapsort）
#include <linux/string.h>          // memcpy
#include <linux/err.h>             // ERR_PTR / PTR_ERR / IS_ERR
#include <linux/errno.h>           // -ENOMEM / -ENOENT / -EINVAL
#include <linux/types.h>           // size_t / ssize_t

/* ==============================================================
 *  compare_int —— 安全的整數比較函式
 *
 *  ✅ 修正 #1：用「比較式」代替「相減」，避免 signed overflow（UB）。
 *             (x > y) - (x < y) 為 1 / 0 / -1 三值，與 qsort/sort 約定相符。
 * ============================================================== */

static int compare_int(const void *a, const void *b)
{
    int x = *(const int *)a;                      // 取出 a 指向的整數（const 標註：我們不改它）
    int y = *(const int *)b;                      // 取出 b 指向的整數

    return (x > y) - (x < y);                     // ✅ 修正 #1：永不溢位的三值比較
                                                  //   x > y → (1) - (0) = +1
                                                  //   x < y → (0) - (1) = -1
                                                  //   x = y → (0) - (0) =  0
                                                  // 這個 idiom 在 kernel 與 BSD libc 廣泛使用
}

/* ==============================================================
 *  sort_copy —— 複製並排序（修正版）
 *
 *  Context: process context, may sleep (GFP_KERNEL).
 *  Return : 排好序的新陣列（caller 用完必呼叫 kfree）。
 *           分配失敗回 ERR_PTR(-ENOMEM)。              // ✅ 修正 #3、#5
 *           n == 0 或 arr == NULL 回 ERR_PTR(-EINVAL)。
 * ============================================================== */

int *sort_copy(const int *arr, size_t n)          // ✅ n 改 size_t：避免負數索引、與 kmalloc_array 對齊
{
    int *copy;                                    // 先宣告，分配後檢查
    size_t bytes;                                  // 為了訊息清楚而暫存

    might_sleep();                                // 標示可睡上下文（GFP_KERNEL）；DEBUG 配置會幫忙抓 context 錯用

    if (!arr || n == 0)                           // 防呆：caller 傳 NULL 或長度 0
        return ERR_PTR(-EINVAL);

    bytes = n * sizeof(*arr);                     // 注意：這行本身仍可能溢位（size_t），但 kmalloc_array 會幫我們攔
    (void)bytes;                                   // demo：實務上只用於 trace；分配交給 kmalloc_array

    copy = kmalloc_array(n, sizeof(*arr),         // ✅ 修正 #2：內建 size_mul_overflow 檢查；溢位時直接回 NULL
                         GFP_KERNEL);
    if (!copy)                                    // ✅ 修正 #3：必檢查 NULL（涵蓋 OOM 與乘法溢位兩種失敗）
        return ERR_PTR(-ENOMEM);                  // ✅ 修正 #5：用 ERR_PTR 而非裸 NULL，caller 用 IS_ERR 判斷

    memcpy(copy, arr, n * sizeof(*arr));          // 拷貝來源陣列；此處 n 已通過 kmalloc_array 隱含驗證
    sort(copy, n, sizeof(*arr), compare_int, NULL); // 用安全的 compare_int 排序（無 swap callback → 用內建記憶體 swap）

    return copy;                                  // 成功：回傳排好的新陣列
}

/* ==============================================================
 *  binary_search_int —— 安全的二分搜（修正版）
 *
 *  Return : ≥ 0 為找到的索引；
 *           -ENOENT 找不到；
 *           -EINVAL 參數非法。                          // ✅ 修正 #5：用 ssize_t + -errno 取代裸 -1
 *
 *  區間語意：[lo, hi)，半開區間。
 * ============================================================== */

ssize_t binary_search_int(const int *arr, size_t n, int target)
{
    size_t lo = 0;                                // ✅ size_t：不會出現負索引
    size_t hi = n;                                // 半開區間右界

    if (!arr)                                     // 防呆
        return -EINVAL;

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;          // ✅ 修正 #4：永不溢位的中點計算（業界標準寫法）
                                                  //   - (lo + hi) / 2 在 lo, hi 接近上限時加法會折返
                                                  //   - lo + (hi - lo) / 2 等價但安全（hi - lo ≥ 0）
        if (arr[mid] == target)
            return (ssize_t)mid;                  // 命中：回傳非負索引；轉型成 ssize_t 統一回傳型別
        if (arr[mid] < target)
            lo = mid + 1;                         // 目標在右半，縮左界
        else
            hi = mid;                             // 目標在左半（hi exclusive，不 -1）
    }
    return -ENOENT;                               // ✅ 修正 #5：找不到回 -ENOENT，不再與 -1 / 合法索引語意衝突
}

/* ==============================================================
 *  使用範例：呼叫端正確處理 ERR_PTR 與 -errno
 * ============================================================== */

static void caller_example(const int *src, size_t n, int target)
{
    int *sorted = sort_copy(src, n);              // 取得排序後的副本
    ssize_t idx;

    if (IS_ERR(sorted)) {                         // ✅ 先用 IS_ERR 判斷
        pr_warn("sort_copy failed: %ld\n", PTR_ERR(sorted));
        return;                                   // 錯誤路徑：sorted 不是合法指標，不能 kfree
    }

    idx = binary_search_int(sorted, n, target);
    if (idx >= 0)
        pr_info("found at index %zd: %d\n", idx, sorted[idx]);
    else if (idx == -ENOENT)
        pr_info("target %d not found\n", target);
    else
        pr_warn("binary_search_int failed: %zd\n", idx);

    kfree(sorted);                                // ✅ caller 一定要 kfree（contract 已寫在 sort_copy 文件）
}
```

---

## 三、整數安全速查（背誦版）

| 場景 | ❌ 危險寫法 | ✅ 安全寫法 |
|------|-----------|-----------|
| 兩整數比較 | `return a - b;` | `return (a > b) - (a < b);` |
| `n * sizeof(T)` 分配 | `kmalloc(n * sizeof(T), gfp)` | `kmalloc_array(n, sizeof(T), gfp)` |
| `a * b + c` 分配 | `kmalloc(a*b+c, gfp)` | `struct_size()` / `size_mul()` + `size_add()` 系列 |
| 二分搜中點 | `(lo + hi) / 2` | `lo + (hi - lo) / 2` |
| 找不到的回傳 | `return -1;` | `return -ENOENT;`（型別 `ssize_t`） |
| 指標型錯誤 | `return NULL;` | `return ERR_PTR(-ENOMEM);` 搭配 `IS_ERR` |
| user-supplied length | 直接信任 | `if (n > MAX) return -EINVAL;` |
| 索引型別 | `int i` | `size_t i`（迴圈計數）/ `ssize_t`（含負錯誤） |

### 五個 review 必問

1. **整數運算會不會溢位？** signed overflow 是 UB，unsigned 會折返；任何 `a + b` / `a * b` / `a - b` 都要先想最大可能值。
2. **記憶體分配的大小有沒有來自外部輸入？** 有的話用 `*_array` 系列、`struct_size`、`size_mul_overflow`。
3. **回傳值是不是混用語意？** 索引、錯誤碼、特殊標記不可共用一條值域。
4. **失敗路徑有沒有洩漏？** 每個 `kmalloc` / `kmalloc_array` 都要對應一條失敗清理；caller 取得指標前所有臨時資源都要釋放。
5. **比較函式對極端值安全嗎？** `INT_MIN` / `INT_MAX` / `0` / 等值四個邊界都要在腦中驗證。

---

## 四、相關 wiki

- `[[interview/Mon-3-code-review-solution]]` — list_head + spinlock + refcount 版本
- `[[interview/Tue-3-code-review-solution]]` — hash-table + RCU + kfree_rcu 版本
- `[[kernel/sort-search]]` — kernel 內建 `sort()`（heapsort）與 `bsearch()` 的設計取捨
- `[[kernel/spinlock]]` — 同步原語與 atomic context 規則
- `[[interview/edge-cases]]` — 整數溢位、空陣列、單元素、極值清單
- `[[interview/code-review-questions]]` — 主管視角整數安全 / 錯誤碼章節
- `[[interview/weekly-mock-bank]]` — 本題出處（Thu-3）
