# LeetCode 33: Search in Rotated Sorted Array

## 問題概述

在一個旋轉過的已排序陣列中搜尋目標值。陣列原本是遞增排序，但在某個位置進行了旋轉。例如：`[0,1,2,4,5,6,7]` 可能被旋轉為 `[4,5,6,7,0,1,2]`。要求時間複雜度 O(log n)。

### LeetCode 解法

```c
int search(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] == target) {
            return mid;
        }

        // 判斷左半邊是否有序
        if (nums[left] <= nums[mid]) {
            // 左半邊已排序，檢查目標是否在該範圍內
            if (nums[left] <= target && target < nums[mid]) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            // 右半邊已排序
            if (nums[mid] < target && target <= nums[right]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }

    return -1;
}
```

**時間複雜度**：O(log n)
**空間複雜度**：O(1)

**核心洞察**：在旋轉已排序陣列中，任何二元搜尋視窗的至少一半總是有序的。利用這個性質可以確定搜尋方向，在一次通過內完成搜尋。

---

## Linux 相關原始碼位置

### 1. **Kernel 通用二元搜尋：`lib/bsearch.c`**

Linux 核心提供了通用的二元搜尋實作，位置：
```
https://github.com/torvalds/linux/blob/master/lib/bsearch.c
```

**核心實作（精簡版）**：
```c
void *bsearch(const void *key, const void *base, size_t num, size_t size,
              int (*cmp)(const void *key, const void *elt))
{
    size_t start = 0, end = num;
    int result;

    while (start < end) {
        size_t mid = start + (end - start) / 2;
        result = (*cmp)(key, base + (mid * size));

        if (result < 0)
            end = mid;
        else if (result > 0)
            start = mid + 1;
        else
            return (void *)base + (mid * size);
    }

    return NULL;
}
```

**特點**：
- 使用 `void*` 指標和自訂比較函式，提供泛型能力
- 使用 `size_t` 進行陣列索引，避免整數溢位
- 採用 `base + (mid * size)` 進行位址計算
- 適用於已排序陣列，但不支援旋轉陣列

### 2. **排序陣列操作：`lib/sort.c`**

位置：
```
https://github.com/torvalds/linux/blob/master/lib/sort.c
```

提供 `sort()` 和 `heapsort()` 實作，但沒有直接的旋轉陣列搜尋邏輯。

### 3. **Kernel 中的二元搜尋應用**

#### a. **檔案系統（B-樹搜尋）：`fs/btrfs/`**
BTRFS 檔案系統在 B-樹操作中使用二元搜尋：
```
https://github.com/torvalds/linux/tree/master/fs/btrfs
```

在已排序的樹節點中搜尋鍵值。

#### b. **記憶體管理：`mm/page_alloc.c`**
在頁面分配時使用二元搜尋查找合適的記憶體區域。

#### c. **虛擬記憶體：`mm/vma.c`**
使用二元搜尋在虛擬記憶體區域（VMA）中快速定位。

#### d. **裝置樹解析：`drivers/of/`**
在已排序的裝置樹節點中進行二元搜尋。

---

## Kernel vs. LeetCode 實作對照

### 1. **泛型設計 vs. 特化實作**

| 面向 | Kernel `bsearch()` | LeetCode 33 |
|------|-------------------|------------|
| **比較方式** | 自訂 callback 函式 | 直接 `nums[mid] == target` |
| **資料型別** | `void*` 通用指標 | `int*` 陣列 |
| **靈活性** | 任何可排序結構 | 僅限整數陣列 |
| **複雜度** | 每次比較涉及函式呼叫開銷 | 直接內聯比較 |

**Kernel 通用 bsearch 簽名**：
```c
void *bsearch(const void *key, const void *base, size_t num,
              size_t size,
              int (*cmp)(const void *key, const void *elt))
```

**LeetCode 特化函式簽名**：
```c
int search(int* nums, int numsSize, int target)
```

### 2. **已排序 vs. 旋轉已排序**

**Kernel `bsearch()`**：假設陣列嚴格遞增或遞減
```c
// 簡化流程
while (start < end) {
    mid = start + (end - start) / 2;
    if (compare(key, array[mid]) < 0)
        end = mid;        // 搜尋左半邊
    else if (...)
        start = mid + 1;  // 搜尋右半邊
}
```

**LeetCode 旋轉陣列搜尋**：先判斷哪半邊有序
```c
// 旋轉陣列特有的邏輯
if (nums[left] <= nums[mid]) {
    // 左半邊有序，檢查目標範圍
    if (nums[left] <= target && target < nums[mid]) {
        right = mid - 1;
    } else {
        left = mid + 1;
    }
} else {
    // 右半邊有序
    // ...
}
```

### 3. **位址計算與溢位預防**

**Kernel 實作**：
```c
// 正確的中點計算，避免整數溢位
size_t mid = start + (end - start) / 2;

// 位址計算使用 size_t（通常為 unsigned long）
void *addr = base + (mid * size);
```

**LeetCode 實作**：
```c
// 同樣避免溢位的計算方式
int mid = left + (right - left) / 2;

// 陣列索引直接訪問
int val = nums[mid];
```

**為什麼不用 `(left + right) / 2`？**
- 當 `left` 和 `right` 都很大時，相加可能溢位
- 先計算差值再除以 2 是安全方式

### 4. **邊界條件處理**

| 條件 | Kernel | LeetCode |
|------|--------|----------|
| **未找到** | 返回 `NULL` | 返回 `-1` |
| **空陣列** | `num == 0` | `numsSize == 0` |
| **單元素** | 比較後直接返回 | 邊界條件自動處理 |
| **重複元素** | 函式依賴比較器 | 問題明確排除重複 |

### 5. **效能特性比較**

```
┌─────────────────────────────────────────────────┐
│ 特性            │ Kernel         │ LeetCode    │
├─────────────────────────────────────────────────┤
│ 比較開銷        │ 函式指標呼叫   │ 直接內聯    │
│ 分支預測        │ 統一邏輯易預測 │ 旋轉判斷多  │
│ 快取友善度      │ 順序訪問       │ 順序訪問    │
│ 指令級平行性    │ 受限於邏輯     │ 受限於邏輯  │
└─────────────────────────────────────────────────┘
```

---

## 主管 Code Review 角度考題

### 1. **演算法設計權衡：為什麼不先找旋轉點？**

**次優方案**（找旋轉點法）：
```c
// 第一步：找旋轉點
int pivot = findPivot(nums, numsSize);  // O(log n)

// 第二步：在找到的半邊進行標準二元搜尋
int result = bsearch(nums, numsSize, target, pivot);  // O(log n)
```

**問題**：
- 需要兩次二元搜尋，常數因子更大（2 倍操作）
- 邊界條件複雜化（處理旋轉點、陣列邊界）
- 快取不連貫（兩個獨立的搜尋過程）

**一次通過方案的優勢**：
```
總操作：≈ 1.0 × log(n)   vs.  2.0 × log(n)
```

**面試回答要點**：
> "一次通過法在單個二元搜尋中就處理旋轉，避免了兩次搜尋的開銷。這樣雖然每次迭代的邏輯複雜度更高，但整體時間複雜度和實際執行時間都更優。在大陣列上（如 n > 100,000），常數因子的差異很明顯。"

### 2. **邊界意識：陣列包含重複元素怎麼辦？**

**當前實作的限制**：
```c
if (nums[left] <= nums[mid]) {
    // 這個判斷在有重複時失效
    // 例如：[1, 3, 1, 1, 1] 無法確定左邊是否真的有序
}
```

**含重複元素的修正版本**：
```c
int search_with_duplicates(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] == target) {
            return mid;
        }

        // 處理重複的邊界值
        while (left < mid && nums[left] == nums[mid]) {
            left++;
        }
        while (right > mid && nums[right] == nums[mid]) {
            right--;
        }

        // 現在可以判斷哪邊有序
        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid]) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            if (nums[mid] < target && target <= nums[right]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }

    return -1;
}
```

**複雜度分析**：
- 最壞情況：O(n)（例如 `[1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1]`）
- 平均情況：O(log n)

**面試回答要點**：
> "LeetCode 33 的問題陳述沒有重複元素，但實際應用中需要處理。含重複元素時，某些邊界無法確定是否有序，必須縮減邊界。最壞情況退化為 O(n)，但在典型情況下仍保持 O(log n)。這是演算法設計中 'worst-case vs. average-case' 的權衡。"

### 3. **API 設計：如何將其做得像 Kernel 的 `bsearch()` 一樣泛型？**

**泛型化設計**：
```c
/**
 * 在旋轉已排序陣列中搜尋元素
 * @base: 陣列基位址
 * @num: 陣列元素個數
 * @size: 單個元素大小（位元組）
 * @key: 搜尋的鍵值
 * @cmp: 比較函式 (cmp(a, b) < 0 表示 a < b)
 *
 * 返回：找到返回陣列索引，未找到返回 -1
 */
int rotated_bsearch(const void *base, size_t num, size_t size,
                    const void *key,
                    int (*cmp)(const void *a, const void *b))
{
    int left = 0, right = num - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        const void *mid_elem = (const char *)base + mid * size;
        const void *left_elem = (const char *)base + left * size;
        const void *right_elem = (const char *)base + right * size;

        int cmp_result = cmp(key, mid_elem);

        if (cmp_result == 0) {
            return mid;
        }

        // 判斷左邊是否有序
        if (cmp(left_elem, mid_elem) <= 0) {
            // 左邊有序，檢查目標是否在該範圍
            if (cmp(left_elem, key) <= 0 && cmp(key, mid_elem) < 0) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            // 右邊有序
            if (cmp(mid_elem, key) < 0 && cmp(key, right_elem) <= 0) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }

    return -1;
}
```

**使用範例**：
```c
int int_cmp(const void *a, const void *b) {
    int val_a = *(const int *)a;
    int val_b = *(const int *)b;
    return (val_a > val_b) - (val_a < val_b);
}

int nums[] = {4, 5, 6, 7, 0, 1, 2};
int target = 0;
int index = rotated_bsearch(nums, 7, sizeof(int), &target, int_cmp);
```

**面試回答要點**：
> "Kernel 的 `bsearch()` 使用 callback 函式實現泛型，但在旋轉陣列的情況下，我們需要多個比較點（left, mid, right）。在實際生產環境中，應該提供兩個 API：特化版本用於性能關鍵路徑（如 LeetCode），泛型版本用於庫層級。巨集或模板也是選項（C++ 適合模板，C 適合 callback）。"

### 4. **系統感知：二元搜尋的快取行為**

**記憶體存取模式**：
```
旋轉已排序陣列搜尋的快取特性
├─ 陣列大小 ≤ L1 快取 (32 KB)
│  └─ 所有存取均命中 L1（～4 個週期）
├─ 陣列大小在 L2 快取內 (256 KB - 512 KB)
│  └─ 首次存取：L1 miss → L2 hit（～12 個週期）
│  └─ 隨後存取：L2 hit
└─ 陣列超過 L3 快取 (8-16 MB)
   └─ L3 miss → 主記憶體存取（～100+ 個週期）
```

**快取不連貫性分析**：

```c
// 次優訪問模式（旋轉檢查導致）
while (...) {
    mid = ...;
    if (nums[left] <= nums[mid]) {   // 存取 left
        // ...
    } else {
        // ...
    }
}
```

在極大陣列上，`nums[left]` 和 `nums[mid]` 可能在不同快取行上，導致額外的快取行填充。

**改進方式**：
```c
// 減少邊界存取
int left_val = nums[left];
int mid_val = nums[mid];
if (left_val <= mid_val) {
    // 使用暫存的值，避免重複存取
}
```

**面試回答要點**：
> "二元搜尋的快取友善度取決於陣列大小和存取模式。對於典型的 n ≤ 1M，陣列可能跨越 L2/L3 邊界。雖然二元搜尋相比線性搜尋已大幅減少存取次數（log n vs. n），但每次的記憶體延遲仍可能成為瓶頸。在 CPU 層級，可以通過減少邊界條件檢查和快取對齐來優化。"

### 5. **演算法廣度：插補搜尋 vs. 二元搜尋**

**插補搜尋基本概念**：
```c
// 插補搜尋在均勻分佈資料上可達 O(log log n)
int interpolation_search(int* nums, int size, int target) {
    int left = 0, right = size - 1;

    while (left <= right && target >= nums[left] && target <= nums[right]) {
        // 線性插值估計位置，而非中點
        int pos = left + ((double)(target - nums[left]) /
                          (nums[right] - nums[left])) *
                         (right - left);

        if (nums[pos] == target) {
            return pos;
        }

        if (nums[pos] < target) {
            left = pos + 1;
        } else {
            right = pos - 1;
        }
    }

    return -1;
}
```

**二元搜尋 vs. 插補搜尋**：

| 特性 | 二元搜尋 | 插補搜尋 |
|------|--------|--------|
| **時間複雜度** | O(log n) | O(log log n)* |
| **最壞情況** | O(log n) | O(n) |
| **資料分佈** | 無要求 | 需要均勻分佈 |
| **實現複雜度** | 低 | 中 |
| **實際應用** | 通用、穩定 | 特定場景 |
| **旋轉陣列適配** | 容易 | 困難 |

**面試回答要點**：
> "插補搜尋在均勻分佈的龐大資料集上理論上更快（O(log log n)），但最壞情況退化為 O(n)，且實現複雜度高。對於旋轉陣列，插補搜尋更難適配。在實際應用中，二元搜尋因其穩定的 O(log n) 和簡潔性成為首選。只有在非常具體的使用場景下（如整數均勻分佈在 [0, 10M) 範圍），才會考慮插補搜尋。"

### 6. **實際應用：記憶體映射檔案上的旋轉陣列搜尋**

**Linux 核心 mmap 場景**：
```c
// 檔案被映射到虛擬記憶體，可能跨越多個頁面邊界
// 檔案內容：[4, 5, 6, 7, 0, 1, 2] (已排序並旋轉)

void *file_base = mmap(NULL, file_size, PROT_READ,
                       MAP_SHARED, fd, 0);

int *nums = (int *)file_base;
int target = 0;

// 直接二元搜尋，OS 會按需進行頁面換入
int result = search((int *)file_base, array_size, target);
```

**頁面故障特性**：
```
頁面大小通常為 4 KB
搜尋模式：logₙ 次存取
每次存取可能觸發頁面故障：～5-10 微秒
總額外延遲：log(n) × 5μs ≈ 10-50 微秒（n ≤ 1M）
```

**優化考量**：
```c
// 遇到頁面邊界時進行預讀
void optimized_rotated_search(const void *file_base,
                              size_t num,
                              int target) {
    // 1. 預讀前 N 個元素（通常一頁內）
    // 2. 使用 madvise(MADV_SEQUENTIAL) 提示 OS
    // 3. 考慮使用 hugepages (2 MB 頁) 減少頁表層級

    madvise(file_base, num * sizeof(int), MADV_SEQUENTIAL);

    // 然後進行二元搜尋
    return search((int *)file_base, num, target);
}
```

**面試回答要點**：
> "在記憶體映射檔案上進行二元搜尋時，每次隨機存取都可能觸發頁面故障。雖然二元搜尋次數是 O(log n)，但每次故障的代價很高。優化策略包括：(1) 使用 hugepages 減少頁表層級，(2) 提供存取模式提示給 OS，(3) 考慮批次預讀，(4) 在關鍵路徑上使用 NUMA 感知的記憶體局部性。"

---

## 面試加分總結

### 核心技能展示

**1. 演算法深度理解**
- 掌握二元搜尋在非標準資料結構上的應用
- 理解「一次通過」vs. 「多次通過」的工程權衡
- 能夠分析邊界情況（重複元素、陣列大小、整數溢位）

**2. Kernel 知識整合**
- 熟悉 Linux 核心的 `bsearch()` 實作與設計哲學
- 理解通用 API 設計（callback、泛型、size_t）
- 能夠解釋核心級二元搜尋的應用（B-樹、VMA、頁面分配）

**3. 系統意識**
- 快取行為分析與最佳化
- 記憶體存取模式與性能影響
- 頁面故障、NUMA、hugepages 等高級概念

**4. 工程設計能力**
- 從特化實作（LeetCode）到泛型設計（Kernel）的過渡
- API 設計的 callback vs. 模板折衷
- 生產環境的邊界條件處理

### 回答策略

**當被問到「怎樣改進這個解法？」時**：
```
優先順序：
1. 分析當前複雜度（時間、空間、快取、頁面故障）
2. 權衡改進的成本（實現複雜度、代碼可維護性）
3. 提供 Kernel 層級的例子佐證
4. 指出實際應用場景的限制
```

**當被問到「這和 Kernel 有什麼關係？」時**：
```
立即回答：
1. Kernel bsearch() 是通用版本，LeetCode 是特化版本
2. 設計思想相同（避免溢位、邊界檢查），但泛型性不同
3. 旋轉陣列搜尋是 LeetCode 獨有的變種
4. Kernel 應用於 B-樹、VMA、頁面管理等實際場景
5. 性能優化考量完全不同（快取局部性 vs. 頁面故障）
```

**量化成績表現**：
- 能說出 `bsearch()` 位於 `lib/bsearch.c`
- 能對比泛型 vs. 特化設計的權衡
- 能討論快取級別與記憶體延遲
- 能舉出 Kernel 中二元搜尋的具體應用
- 能提出含重複元素的改進版本

### 亮點應答範例

**Q: 為什麼 Kernel 的 bsearch() 不直接支援旋轉陣列？**

**A:（展示系統思維）**
> Kernel 的 bsearch() 是通用原語，設計用於任何已排序資料結構。旋轉陣列是特定的變種，屬於上層應用層的問題。Kernel 提供的是基礎設施（通用二元搜尋），上層應用（如檔案系統、LeetCode）根據具體場景進行特化。這是分層設計的體現。同時，支援旋轉陣列會增加通用 bsearch() 的代碼複雜度和分支預測難度，對所有使用者都有性能代價。

**Q: 這個演算法在記憶體受限的環境（如嵌入式系統）如何適配？**

**A:（展示實踐考量）**
> 旋轉陣列搜尋已經是 O(1) 空間，無法進一步優化。但在嵌入式環境，需要考慮：(1) 指令快取壓力—邏輯複雜導致分支預測失敗，(2) 資料快取—陣列可能全部在 L1 內，邊界條件的優化空間有限，(3) 棧深度—遞迴版本不適合。在極度受限的環境下，預先計算旋轉點並使用兩次簡單的二元搜尋可能更優，因為邏輯更簡單、分支預測更好。

---

## Sources

### Kernel 原始碼參考

1. **lib/bsearch.c - 通用二元搜尋實作**
   - https://github.com/torvalds/linux/blob/master/lib/bsearch.c
   - 提供 O(log n) 的通用二元搜尋

2. **lib/sort.c - 排序與搜尋**
   - https://github.com/torvalds/linux/blob/master/lib/sort.c
   - Kernel 內部的排序演算法集合

3. **fs/btrfs/ctree.c - BTRFS B-樹搜尋**
   - https://github.com/torvalds/linux/blob/master/fs/btrfs/ctree.c
   - 檔案系統中的二元搜尋應用案例

4. **mm/interval_tree.c - 虛擬記憶體區域搜尋**
   - https://github.com/torvalds/linux/blob/master/mm/interval_tree.c
   - VMA 管理中的區間搜尋

5. **mm/page_alloc.c - 頁面分配**
   - https://github.com/torvalds/linux/blob/master/mm/page_alloc.c
   - 記憶體管理中的二元搜尋使用

6. **include/linux/bsearch.h - Header 定義**
   - https://github.com/torvalds/linux/blob/master/include/linux/bsearch.h
   - bsearch() 的公開接口定義

### LeetCode 問題

7. **LeetCode Problem 33**
   - https://leetcode.com/problems/search-in-rotated-sorted-array/
   - 旋轉已排序陣列搜尋問題描述與測試用例

### 相關文獻與資源

8. **Binary Search 演算法綜論**
   - https://en.wikipedia.org/wiki/Binary_search_algorithm
   - 二元搜尋的理論基礎與變種

9. **Linux Kernel Documentation**
   - https://kernel.org/doc/html/latest/
   - 官方 Kernel 文檔

10. **CPU Cache 與記憶體層級**
    - Computer Architecture: A Quantitative Approach (Hennessy & Patterson)
    - 快取行為分析的標準參考

---

**文件建立日期**：2026-03-29
**難度等級**：Medium
**核心洞察**：在旋轉已排序陣列中，任何二元搜尋視窗的至少一半總是有序的。
