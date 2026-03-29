# LeetCode 35: Search Insert Position — Linux 核心原始碼分析

## 題目簡述
給定一個排序陣列和目標值，如果目標值存在則回傳其索引，否則回傳應該插入的位置。

```c
// LeetCode 版本
int searchInsert(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return left;  // 插入位置
}
```

---

## Linux 相關原始碼位置

| 檔案路徑 | 說明 |
|---------|------|
| `lib/bsearch.c` | Linux 核心的通用二元搜尋實作 |
| `include/linux/bsearch.h` | bsearch() 函式宣告 |
| `lib/btree.c` | B 樹實作，在節點內使用二元搜尋 |
| `fs/btrfs/extent-tree.c` | Btrfs 檔案系統使用二元搜尋查詢範圍資料 |
| `mm/interval_tree.c` | 虛擬記憶體區域 (VMA) 查詢使用二元搜尋 |
| `kernel/events/core.c` | Performance counter 使用二元搜尋 |

### 核心 bsearch() 實作示例

`lib/bsearch.c` 中的通用二元搜尋：

```c
// 來自 lib/bsearch.c
void *bsearch(const void *key, const void *base, size_t num,
              size_t size, cmp_func_t cmp)
{
    size_t start = 0, end = num;
    int result;

    while (start < end) {
        size_t mid = start + (end - start) / 2;

        result = cmp(key, base + mid * size);
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

---

## Kernel vs. LeetCode 實作對照

### 1. **泛型設計 (Generics)**

**Linux 核心：** 使用 `void*` 和函式指標，支持任意資料型別
```c
typedef int (*cmp_func_t)(const void *a, const void *b);
void *bsearch(const void *key, const void *base, size_t num,
              size_t size, cmp_func_t cmp);
```

**LeetCode：** 直接對整數陣列特化
```c
int searchInsert(int* nums, int numsSize, int target) {
    // 直接使用 int 陣列，無需泛型
}
```

**核心差異：** 核心版本需要處理任意結構體，因此使用位元組偏移；LeetCode 只需整數。

---

### 2. **搜尋目標 (Search Goal)**

| 面向 | 核心實作 | LeetCode |
|------|--------|---------|
| **找不到時的返回值** | `NULL`（純搜尋） | `left`（插入位置） |
| **使用場景** | 精確查詢檔案系統記錄、記憶體頁表 | 維護排序陣列的插入點 |
| **語義** | "此元素存在嗎？" | "應在何處維持排序？" |

---

### 3. **迴圈不變式 (Loop Invariant)**

**兩者都遵循相同的核心原則：**

```
// 迴圈維持的不變式
- 所有在 [0, left) 的元素都 < target
- 所有在 (right, numsSize) 的元素都 > target
- left 和 right 會收斂到相同位置（插入點）
```

**核心版本使用：** `while (start < end)` — 區間為 `[start, end)`

**LeetCode 版本使用：** `while (left <= right)` — 區間為 `[left, right]`

兩者數學上等價，但邊界條件不同。

---

### 4. **整數溢位防護 (Overflow Prevention)**

**兩者都使用：** `mid = left + (right - left) / 2`

```c
// 正確做法（同時出現在核心和 LeetCode）
mid = left + (right - left) / 2;

// 危險做法（可能整數溢位）
mid = (left + right) / 2;  // 在 C 語言中 int 溢位是 UB！
```

**Linux 核心風格：** 使用 `size_t` 進行無符號運算，進一步避免溢位問題。

---

## 主管 Code Review 角度考題

### 面試官可能提出的問題

#### Q1: 為何使用 `left + (right - left) / 2` 而非 `(left + right) / 2`？

**答案：**
```c
// 問題：可能整數溢位
int mid = (left + right) / 2;
// 當 left = 2^30, right = 2^30 時，left + right 溢位

// 解決：強制一個邊界在已知範圍內
int mid = left + (right - left) / 2;
// right - left 最多為陣列大小，不會溢位
```

這在 LeetCode 上是細節，但在生產代碼（如 Linux 核心）中至關重要。

---

#### Q2: 如何使用 void* 和比較函式使其泛型化？

**答案：** 參考核心的 `bsearch()` 實作

```c
typedef int (*cmp_t)(const void *a, const void *b);

void *generic_bsearch(const void *key, const void *base,
                      size_t count, size_t size, cmp_t cmp) {
    size_t left = 0, right = count;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        const void *mid_elem = base + mid * size;  // 位元組級別計算

        int cmp_result = cmp(key, mid_elem);
        if (cmp_result < 0)
            right = mid;
        else if (cmp_result > 0)
            left = mid + 1;
        else
            return (void *)mid_elem;
    }
    return NULL;
}
```

**關鍵點：** 必須使用 `size_t` 進行位元組偏移，因為 `void*` 沒有已知的大小。

---

#### Q3: lower_bound vs upper_bound 對插入位置有何影響？

**答案：** 處理重複元素時的行為不同

```c
// lower_bound: 返回第一個 >= target 的位置
// 用於「在此插入可保持排序」
int lower_bound(int *arr, int n, int target) {
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid;  // arr[mid] >= target，嘗試找更左邊的
    }
    return left;
}

// upper_bound: 返回第一個 > target 的位置
// 用於「在此後插入可保持排序」
int upper_bound(int *arr, int n, int target) {
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] <= target)
            left = mid + 1;
        else
            right = mid;  // arr[mid] > target，嘗試找更左邊的
    }
    return left;
}
```

**範例：** 陣列 `[1, 2, 2, 2, 3]` 搜尋 `2`
- LeetCode 題 35 返回現有 `2` 的索引（若存在），否則返回 lower_bound 位置
- lower_bound(2) = 1（第一個 2 的位置）
- upper_bound(2) = 4（最後一個 2 之後）

---

#### Q4: 核心如何在非連續資料上進行二元搜尋？

**答案：** 使用自訂比較函式或樹結構

**例 1：虛擬記憶體區域 (VMA) 搜尋**

Linux 用 `find_vma()` 搜尋進程的虛擬記憶體區域（儲存在紅黑樹中，非陣列）：

```c
// mm/mmap.c 的簡化版本
struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr) {
    // 核心在紅黑樹上進行搜尋，但邏輯遵循二元搜尋原則
    struct vm_area_struct *vma = mm->mmap_tree.rb_node;

    while (vma) {
        if (addr < vma->vm_start)
            vma = vma->rb_left;
        else if (addr >= vma->vm_end)
            vma = vma->rb_right;
        else
            return vma;
    }
    return NULL;
}
```

**例 2：檔案系統範圍查詢**

Btrfs 在 B 樹中搜尋檔案範圍：

```c
// fs/btrfs/extent-tree.c 的邏輯
// 搜尋給定位元組偏移的範圍項目
// 與線性陣列上的二元搜尋原則相同，但應用於樹結構
```

**核心概念：** 只要資料結構支持按索引或順序訪問，就可以應用二元搜尋邏輯。

---

#### Q5: 對小型陣列，二元搜尋 vs 線性掃描的 Cache 局部性如何？

**答案：**

```c
// 小陣列（< 64 字節 = 1 cache line）
// 線性掃描可能更快，因為整個陣列都在 L1 cache 中
for (int i = 0; i < 8; i++) {  // 8 個整數 = 32 字節
    if (arr[i] == target) return i;
}

// 大陣列（數百萬元素）
// 二元搜尋勝出，因為：
// - 線性掃描：O(n) 個 cache miss
// - 二元搜尋：O(log n) 個 cache miss（約 20 次查詢 100 萬元素）
```

**Linux 核心實踐：**

```c
// lib/bsearch.c 中的註釋指出：
// 對於小的 count 值，線性搜尋可能更快
// 但二元搜尋提供了更好的漸近複雜度保證
```

某些關鍵路徑使用「混合」方法：前幾步二元搜尋，然後線性掃描。

---

## 完整解決方案與最佳化

### LeetCode 標準解：

```c
int searchInsert(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) {
            return mid;
        } else if (nums[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    // left 現在是插入位置
    return left;
}
```

**時間複雜度：** O(log n)
**空間複雜度：** O(1)

---

### Linux 風格（泛型版本）：

```c
typedef int (*search_cmp_t)(const void *key, const void *elem);

int generic_search_insert(const void *key, void *base, int count,
                         int size, search_cmp_t cmp) {
    int left = 0, right = count;

    while (left < right) {
        int mid = left + (right - left) / 2;
        const void *mid_elem = base + mid * size;

        int cmp_result = cmp(key, mid_elem);
        if (cmp_result < 0) {
            right = mid;
        } else if (cmp_result > 0) {
            left = mid + 1;
        } else {
            return mid;  // 找到
        }
    }
    return left;  // 插入位置
}
```

---

## 面試加分總結

### 將 LeetCode 解答提升至生產級別的要點

1. **整數溢位防護**
   - 使用 `mid = left + (right - left) / 2` 避免整數溢位
   - 在 C 中，`int` 溢位是未定義行為（UB），會導致詭異 bug

2. **泛型設計**
   - 採用 `void*` 和比較函式指標，支持任意資料型別
   - 這是 Linux 核心 `bsearch()` 的設計方式

3. **邊界條件明確**
   - 了解 `while (left <= right)` vs `while (left < right)` 的差別
   - 清晰註釋迴圈不變式，防止 off-by-one 錯誤

4. **複雜資料結構上的推廣**
   - 二元搜尋不限於陣列（如虛擬記憶體樹、B 樹）
   - 只要資料維持排序性，即可應用二元搜尋邏輯

5. **效能考量**
   - 理解 cache 局部性（小陣列可用線性掃描）
   - 認識漸近複雜度 O(log n) 的重要性

6. **標準庫信任**
   - 了解何時複用 `bsearch()` vs 自己實作
   - 在 Linux 核心代碼中，`bsearch()` 是標準選擇

---

## Sources

- [Linux Kernel Source - lib/bsearch.c](https://github.com/torvalds/linux/blob/master/lib/bsearch.c)
- [Linux Kernel Source - include/linux/bsearch.h](https://github.com/torvalds/linux/blob/master/include/linux/bsearch.h)
- [Linux Kernel Source - lib/btree.c](https://github.com/torvalds/linux/blob/master/lib/btree.c)
- [Linux Kernel Source - mm/interval_tree.c](https://github.com/torvalds/linux/blob/master/mm/interval_tree.c)
- [Linux Kernel Source - fs/btrfs/extent-tree.c](https://github.com/torvalds/linux/blob/master/fs/btrfs/extent-tree.c)
- [LeetCode Problem 35 - Search Insert Position](https://leetcode.com/problems/search-insert-position/)
- [Binary Search Tutorial - GeeksforGeeks](https://www.geeksforgeeks.org/binary-search/)
