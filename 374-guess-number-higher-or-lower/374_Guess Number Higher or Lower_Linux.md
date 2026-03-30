# LeetCode 374 與 Linux 核心二分查找的連接分析

## 題目背景

**LeetCode 374: Guess Number Higher or Lower**

給定範圍 [1, n]，使用二分查找找到目標數字。通過呼叫 `guess(num)` API 取得回饋：
- 返回 1：目標更大
- 返回 -1：目標更小
- 返回 0：找到目標

標準 C 解法使用迭代二分查找，核心公式為：
```c
int mid = left + (right - left) / 2;
```

---

## 1. Linux 相關原始碼位置

### 核心二分查找實現

**檔案位置：**
- `lib/bsearch.c` — 通用二分查找函數 `bsearch()`
- `include/linux/bsearch.h` — 函數原型與巨集定義
- `lib/btree.c` — B-tree 實現，使用二分查找進行節點查詢

### 核心 `bsearch()` 函數簽名

```c
// from lib/bsearch.c
void *bsearch(const void *key, const void *base, size_t num,
              size_t size, cmp_func_t cmp)
{
    size_t start = 0, end = num;
    int result;

    while (start < end) {
        size_t mid = start + (end - start) / 2;
        result = cmp(key, base + mid * size, size);

        if (result < 0)
            end = mid;
        else if (result > 0)
            start = mid + 1;
        else
            return (void *)base + mid * size;
    }
    return NULL;
}
```

**用途：** 核心各處進行快速有序陣列查詢，例如：
- 系統呼叫表查找
- 中斷處理程序查找
- 記憶體管理結構查詢

---

## 2. Kernel 與 LeetCode 實作對照

### 2.1 一般性設計差異

| 特性 | Kernel `bsearch()` | LeetCode 374 |
|------|-------------------|-------------|
| **輸入** | 任意排序陣列 + 比較器 | 整數範圍 [1, n] + guess API |
| **抽象層次** | 通用（void 指標） | 特定（整數） |
| **時間複雜度** | O(log n) | O(log n) |
| **空間複雜度** | O(1) | O(1) |
| **迭代 vs 遞迴** | 迭代 | 迭代或遞迴 |

### 2.2 中點計算的安全性

**LeetCode 做法：**
```c
int mid = left + (right - left) / 2;
```

**Kernel 做法：**
```c
size_t mid = start + (end - start) / 2;
// 或針對位元運算優化：
size_t mid = start + ((end - start) >> 1);
```

**為何避免 `(left + right) / 2`？**

假設 `left` 與 `right` 皆為 32 位帶號整數，若 `left = 2^30`，`right = 2^30 + 1000`：
```
left + right = 2^31 + 1000  // 整數溢出！
```

使用 `left + (right - left) / 2` 避免溢出，因為 `(right - left)` 必然小於原始範圍。

### 2.3 元素大小處理

**Kernel：** 通過 `size` 參數計算位址
```c
base + mid * size  // 任意元素大小
```

**LeetCode：** 隱式假設整數元素
```c
// 概念上：array[mid]，編譯器自動計算位移
```

### 2.4 比較機制

**Kernel：** 函數指標比較器
```c
result = cmp(key, base + mid * size, size);
if (result < 0)      // key 更小
    end = mid;
else if (result > 0) // key 更大
    start = mid + 1;
else                 // 相等
    return ...;
```

**LeetCode：** API 呼叫
```c
int cmp = guess(mid);
if (cmp < 0)        // 目標更小
    right = mid - 1;
else if (cmp > 0)   // 目標更大
    left = mid + 1;
else                // 找到
    return mid;
```

---

## 3. 主管 Code Review 角度考題

### 3.1 為何使用 `left + (right - left) / 2` 而非 `(left + right) / 2`？

**答案（面試標準答覆）：**

避免整數溢出。當 `left` 與 `right` 皆為大值時，`left + right` 可能超過資料型別的上限，導致未定義行為。

**Kernel 視角：** Linux 在 `lib/bsearch.c` 中明確使用此技巧，確保在極大陣列上的安全性。

**代碼示例：**
```c
// 不安全
int mid = (left + right) / 2;  // 若 left=1e9, right=1e9，則溢出

// 安全
int mid = left + (right - left) / 2;  // 等價於 left + (right - left)/2
```

**位元運算優化（Kernel 常用）：**
```c
size_t mid = start + ((end - start) >> 1);  // 右移取代除以 2
```

### 3.2 Kernel bsearch 如何處理不同元素大小？

**答案：**

通過 `size` 參數與位址算術實現通用性。

```c
void *bsearch(const void *key, const void *base,
              size_t num, size_t size, cmp_func_t cmp)
{
    // base 為 void*，每次跳躍 mid * size 位元組
    void *mid_element = (char *)base + mid * size;
    // 比較器負責型別轉換
}
```

**LeetCode 與 Kernel 的權衡：**
- Kernel：通用但需要呼叫函數指標（微小額外開銷）
- LeetCode：特化優化，直接整數操作

### 3.3 二分查找在連續陣列 vs. 樹結構上的快取行為差異？

**答案：**

**連續陣列（Kernel bsearch 場景）：**
- 優點：快取局部性（Spatial Locality）高。相鄰元素在記憶體中連續，CPU 預取（prefetch）效果佳
- L1 快取命中率：~90-95%（現代 CPU）
- 典型延遲：~4-6 個時鐘週期

**樹結構（Kernel btree 場景）：**
- 缺點：節點分散記憶體，快取未命中率高
- L1 快取命中率：~40-60%
- 典型延遲：~10-20 個時鐘週期（需取用主記憶體）

**影響：**
```
n = 1,000,000 元素

陣列二分查找：   log2(n) ≈ 20 次比較 × 5 週期 ≈ 100 週期
樹查找：         log_b(n) × 15 週期（B-tree 度數 b 較大，層數少，但延遲高）
```

**結論：** 對於排序資料，連續陣列二分查找通常快於樹，除非涉及頻繁插入/刪除。

### 3.4 二分查找能否應用於未排序資料？

**答案：否。**

二分查找的正確性依賴於 **單調性假設**。若資料未排序，中點比較無法安全地排除一半搜尋空間。

```c
// 未排序陣列上的危險
int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
int result = bsearch_unsafe(&target, arr, 8);  // 結果未定義！
```

**Kernel 防禦：** 許多使用 `bsearch()` 的子系統（如系統呼叫表）由核心開發者管理，保證資料排序。外部使用者需自行驗證排序性。

### 3.5 何時三分查找優於二分查找？

**答案：在特定場景，三分查找優於二分查找，但不是基於比較次數。**

**單調函數評估場景：**

若搜尋目標不是「找陣列元素」，而是「找函數極值」（如找到最小值），三分查找有優勢：

```c
// 假設 f(x) 在 [0, n] 上單峰（先增後減）
// 二分查找：無法直接應用
// 三分查找：每次去掉 1/3 搜尋空間
while (right - left > 2) {
    int mid1 = left + (right - left) / 3;
    int mid2 = right - (right - left) / 3;
    if (f(mid1) < f(mid2))
        right = mid2;
    else
        left = mid1;
}
```

**複雜度：**
- 二分查找：O(log₂ n) 次函數呼叫
- 三分查找：O(log₃ n) 次函數呼叫 ≈ 0.63 × O(log₂ n)

**但如果成本是「比較」而非「函數呼叫」？**
- 二分查找：O(log₂ n) 次比較
- 三分查找：O(log₃ n) 次區間排除，但每次需 2 次比較 → O(2 log₃ n) ≈ 1.26 × O(log₂ n)

**結論：** 三分查找在比較成本高（如複雜物件比較）或函數評估成本為主的場景更優。陣列查找上二分優於三分。

---

## 4. 面試加分總結（與 Kernel 知識連接）

### 核心概念掌握（Grade: A）
- ✅ 理解整數溢出風險與防禦（`left + (right - left) / 2`）
- ✅ 知道 O(log n) 時間、O(1) 空間的精確性
- ✅ 能用遞迴或迭代兩種方式實現

### Kernel 層級深度（Grade: A+）
- ✅ 認識 Linux `lib/bsearch.c` 的通用實現與應用場景
- ✅ 理解函數指標比較器的抽象設計（`cmp_func_t`）
- ✅ 能解釋位址算術 `base + mid * size` 的用途
- ✅ 了解快取局部性對性能的實際影響（連續 vs. 樹）

### 進階優化與權衡（Grade: A++）
- ✅ 討論位元運算優化：`(end - start) >> 1` vs. `(end - start) / 2`
- ✅ 分析不同資料結構（陣列、B-tree、紅黑樹）上查找的時空權衡
- ✅ 能指出三分查找的適用場景與複雜度分析

### 代碼審查視角（Grade: A++）
- ✅ 檢查邊界條件：`left == right`、`mid` 計算、回傳值
- ✅ 識別溢出漏洞與修復
- ✅ 評估迴圈不變式（Loop Invariant）：搜尋目標必在 `[left, right]` 內

### 系統設計聯想（Grade: A+++）
- ✅ 將二分查找的想法應用於 IP 路由表（最長前綴匹配）
- ✅ 在排序日誌或時間序列資料上進行範圍查詢（Kernel VFS）
- ✅ 結合 Kernel 記憶體管理的 VMA（Virtual Memory Area）紅黑樹查找

---

## 5. LeetCode 374 標準解法 + Kernel 風格註釋

```c
/**
 * Forward declaration of the guess API.
 * @param num your guess
 * @return -1 if num is higher than the secret number
 *         1 if num is lower than the secret number
 *         0 if num is equal to the secret number
 * int guess(int num);
 */

int guessNumber(int n) {
    /**
     * 初始化搜尋邊界
     * 不變式：秘密數字在 [left, right] 內
     */
    int left = 1, right = n;

    while (left <= right) {
        /**
         * 計算中點，避免整數溢出
         * 等同於 (left + right) / 2，但安全
         *
         * Kernel 類比：lib/bsearch.c 採用同樣技巧
         */
        int mid = left + (right - left) / 2;

        /**
         * 呼叫 API 進行單次比較
         * 時間複雜度：O(1)，假設 guess() 為原子操作
         */
        int result = guess(mid);

        if (result == 0) {
            // 找到秘密數字
            return mid;
        } else if (result < 0) {
            /**
             * 秘密數字大於 mid
             * 排除左半部分 [left, mid)
             */
            left = mid + 1;
        } else {
            /**
             * 秘密數字小於 mid
             * 排除右半部分 (mid, right]
             */
            right = mid - 1;
        }
    }

    /**
     * 搜尋失敗（理論上不應發生，若輸入保證秘密數字在 [1, n]）
     */
    return -1;
}

/**
 * 時間複雜度：O(log n)
 *   每次迭代排除一半搜尋空間，最多 log₂(n) 次迭代
 *
 * 空間複雜度：O(1)
 *   僅使用常數級別額外變數（left, right, mid）
 *
 * 與 Kernel bsearch 的關鍵差異：
 *   - 本解法特化於整數範圍
 *   - Kernel bsearch 通用於任意資料型別
 *   - 本解法無需比較器函數指標
 */
```

---

## 6. Sources

### Linux Kernel 原始碼
- [Linux lib/bsearch.c on GitHub](https://github.com/torvalds/linux/blob/master/lib/bsearch.c)
- [Linux include/linux/bsearch.h](https://github.com/torvalds/linux/blob/master/include/linux/bsearch.h)
- [Linux lib/btree.c](https://github.com/torvalds/linux/blob/master/lib/btree.c)

### 相關文獻與資源
- Donald Knuth, *The Art of Computer Programming*, Vol. 3: Sorting and Searching
- [GeeksforGeeks: Binary Search](https://www.geeksforgeeks.org/binary-search/)
- [LeetCode 374 Official Discussion](https://leetcode.com/problems/guess-number-higher-or-lower/discuss/)
- [CPU Cache Locality and Binary Search Performance](https://en.algorithmica.org/hashing/)

### 延伸閱讀
- Linux VMA（Virtual Memory Area）紅黑樹實現
- B-tree 在 Kernel 檔案系統的應用（如 ext4、Btrfs）
- Intel CPU 快取一致性協議（MESI）與 Prefetch 機制

---

**文檔撰寫日期：2026-03-30**

**適用場景：**
- LeetCode 374 深度複習
- 系統設計面試準備
- Kernel 貢獻者入門參考
- 二分查找性能最佳化研究
