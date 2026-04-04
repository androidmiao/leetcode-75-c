# Sorting & Searching 排序與搜尋

## 核心概念

Linux kernel 的排序和搜尋實現位於 `lib/sort.c`, `lib/bsearch.c`, `lib/list_sort.c`。它們針對內核特性（記憶體限制、可預測性、性能）進行優化，與教科書算法有顯著差異。

## Introsort：lib/sort.c

### 算法選擇的演化

```
純 Quicksort → 可能退化至 O(n²)
Heapsort      → O(n log n) 但快取友善性差
Introsort      → Quicksort + Heapsort 混合 ✓
```

### 為什麼不用純 Quicksort？

```c
// 純 quicksort 的最壞情況
int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
sort(arr);  // O(n²) 分割深度 O(n)
```

在生產內核中，無法保證最壞情況不發生。攻擊者可構造惡意數據觸發退化。

### Introsort 實現

```c
#define SORT_THRESHOLD 4  // 小規模使用插入排序

void sort(void *base, size_t num, size_t size,
          int (*cmp_func)(const void *, const void *),
          void (*swap_func)(void *, void *, int size))
{
  if (num < 2)
    return;

  // 計算最大遞迴深度限制
  size_t limit = 2 * ilog2(num);

  do_sort(base, num, size, cmp_func, swap_func, &limit);
}

static void do_sort(void *base, size_t num, size_t size,
                    int (*cmp_func)(const void *, const void *),
                    void (*swap_func)(void *, void *, int size),
                    size_t *limit)
{
  if (num < SORT_THRESHOLD) {
    insertion_sort(base, num, size, cmp_func, swap_func);
    return;
  }

  if (*limit == 0) {
    // 遞迴深度超出，轉用 heapsort
    heapsort(base, num, size, cmp_func, swap_func);
    return;
  }

  (*limit)--;

  // 標準 quicksort 分割
  size_t pivot = partition(base, num, size, cmp_func, swap_func);
  do_sort(base, pivot, size, cmp_func, swap_func, limit);
  do_sort(base + pivot * size, num - pivot, size, cmp_func, swap_func, limit);
}
```

### 三階段流程

1. **Quicksort 主階段**
   - 平均 O(n log n)
   - 快取友善
   - 分割深度受限於 2 log₂(n)

2. **深度檢查**
   - 若深度 > 2 log₂(n)，表示不平衡分割
   - 轉用 Heapsort

3. **Heapsort 備用**
   - 保證 O(n log n)
   - 儘管快取友善性較差，但安全

### 複雜度分析

| 情況 | Quicksort | Introsort |
|------|-----------|-----------|
| 平均 | O(n log n) | O(n log n) |
| 最好 | O(n log n) | O(n log n) |
| 最壞 | O(n²) | O(n log n) |
| 空間 | O(log n) | O(log n) |

## 二分搜尋：lib/bsearch.c

### 問題陳述

```c
// 天真實現的危險
int bsearch_naive(void *key, void *base, size_t num,
                  size_t size,
                  int (*cmp)(const void *, const void *))
{
  int low = 0, high = num - 1;

  while (low <= high) {
    int mid = (low + high) / 2;  // 整數溢出！
    ...
  }
}
```

當 low + high > 2^31 時溢出。

### 安全實現

```c
void *bsearch(const void *key, const void *base,
              size_t num, size_t size,
              int (*cmp)(const void *, const void *))
{
  size_t start = 0, end = num;
  int result;

  while (start < end) {
    // 避免整數溢出
    size_t mid = start + (end - start) / 2;
    result = (*cmp)(key, base + mid * size);

    if (result < 0)
      end = mid;
    else if (result > 0)
      start = mid + 1;
    else
      return (void *)(base + mid * size);
  }

  return NULL;
}
```

**關鍵改進**
- `mid = start + (end - start) / 2` 代替 `(low + high) / 2`
- 避免大整數相加
- 支援 64-bit 地址空間

### 複雜度

| 操作 | 時間複雜度 | 空間複雜度 |
|------|-----------|-----------|
| 搜尋 | O(log n) | O(1) |

## 鏈表排序：lib/list_sort.c

### 問題陳述

陣列排序算法對鏈表不適用：

```c
// 不能隨機訪問鏈表
void *pivot = base + partition_idx * size;  // 無效！
```

### 解決方案：底部向上歸併排序

```c
void list_sort(void *priv,
               struct list_head *head,
               int (*cmp)(void *priv,
                         const struct list_head *a,
                         const struct list_head *b))
{
  // 計算運行長度（run length）
  struct list_head *list = head->next;
  int nruns = 0, minrun = compute_minrun(num);

  // 階段 1：分割成小運行
  while (list != head) {
    int len = 0;
    while (len < minrun && list != head) {
      list = list->next;
      len++;
    }
    nruns++;
  }

  // 階段 2：逐漸歸併運行
  int run_size = minrun;
  while (nruns > 1) {
    merge_consecutive_runs(&head, run_size, cmp, priv);
    run_size *= 2;
    nruns = (nruns + 1) / 2;
  }
}
```

### 為什麼選擇歸併排序？

**優勢**
- 鏈表天然適合歸併（沒有隨機訪問開銷）
- 穩定排序
- 無額外指標操作

**劣勢**
- 需 O(n) 額外空間（在歸併時），但內核優化為原地歸併

### 複雜度

| 情況 | 時間 | 空間 |
|------|------|------|
| 平均 | O(n log n) | O(1)* |
| 最好 | O(n) | O(1) |
| 最壞 | O(n log n) | O(1) |

*原地歸併

## 棧預算約束

### 內核設計約束

```c
// 內核棧有限（通常 4-8 KB 在某些架構）
struct task_struct {
  unsigned long stack;      // 單個棧
};
```

**Quicksort 遞迴深度**
```
理想：O(log n)
最壞：O(n)
內核：限制為 2 log n（Introsort）
```

**Mergesort 遞迴深度**
```
理想：O(log n)
最壞：O(log n)
內核：無最壞情況深度爆炸
```

### 棧節點成本

```c
// 每個棧幀的成本
size_t stack_frame_size = 64 + 8 * local_var_count;  // bytes

// Introsort 遞迴深度限制
size_t max_depth = 2 * log2(n);
size_t max_stack = stack_frame_size * max_depth;

// 例：1M 元素
max_depth = 2 * 20 = 40
max_stack = 64 * 40 = 2560 bytes  // 安全
```

Heapsort 遞迴深度可達 n，棧成本 O(n)，不可接受。

## 與 LeetCode 問題的對應

### 相關問題

- **215 陣列中第 K 個最大元素**：quickselect，類似 Introsort 分割邏輯
- **217 存在重複元素**：排序或雜湊，內核排序提供基礎
- **347 前 K 個高頻元素**：部分排序，可用堆或 quickselect
- **692 前 K 個高頻單詞**：排序 + 字典序，內核 sort 支援自定義比較
- **703 流的第 K 大元素**：小頂堆，與排序互補
- **704 二分搜尋**：bsearch 直接應用

## 設計洞察

### 1. 為什麼選擇 Introsort？

**對比分析**

| 算法 | 性能 | 安全性 | 快取 | 實現 |
|------|------|--------|------|------|
| Quicksort | ★★★★★ 快 | ★☆ 不安全 | ★★★★★ 優 | ★ 簡 |
| Heapsort | ★★★ 中 | ★★★★★ 安全 | ★★ 差 | ★★ 複 |
| Introsort | ★★★★ 很快 | ★★★★★ 安全 | ★★★★ 好 | ★★★ 中 |

Kernel 選擇 Introsort 因為：
- 99% 時間跑 Quicksort 的速度
- 最壞情況有 O(n log n) 保證
- 棧使用可預測

### 2. 避免整數溢出

```c
// 錯誤：當 low + high 溢出時失敗
int mid = (low + high) / 2;

// 正確：等價，但無溢出風險
int mid = low + (high - low) / 2;

// Kernel 風格：用無符號算術
size_t mid = start + (end - start) / 2;
```

### 3. 棧預算 vs 遞迴深度

內核在構造資料結構和算法時，始終考慮棧成本：

```c
// 不可接受（最壞情況棧爆炸）
void recursive_sort(int *arr, int n) {
  if (n <= 1) return;
  int pivot = partition(arr, n);
  recursive_sort(arr, pivot);           // 可能深度 n
  recursive_sort(arr + pivot, n - pivot);
}

// 可接受（深度受限）
void introsort(int *arr, int n, int limit) {
  if (limit == 0) {
    heapsort(arr, n);  // 備用，無遞迴
    return;
  }
  int pivot = partition(arr, n);
  introsort(arr, pivot, limit - 1);
  introsort(arr + pivot, n - pivot, limit - 1);
}
```

## 面試考點

### 問題 1：為什麼 kernel 排序使用 Introsort？

**預期答案**
- 平均性能：O(n log n)，快於 Heapsort
- 最壞保證：O(n log n)，安全於純 Quicksort
- 棧預算：深度限制為 2 log n，防止棧溢出
- 適應性：已排序資料退化為 O(n)

### 問題 2：二分搜尋為何需要 `mid = low + (high - low) / 2`？

**預期答案**
- `(low + high) / 2` 在大數組上溢出
- `low + high` 可能 > 2^31（在 32-bit 或跨越 64-bit 地址邊界）
- 等價計算避免中間溢出
- Kernel 支援大型地址空間，必須考慮

### 問題 3：鏈表排序為何不用快速排序？

**預期答案**
- 快速排序依賴隨機訪問：arr[mid]
- 鏈表無 O(1) 隨機訪問，任何分割都是 O(n)
- 歸併排序利用鏈表天然結構，分割和歸併都是 O(1)
- 原地歸併可實現 O(1) 額外空間

### 問題 4：Introsort 何時轉用 Heapsort？

**預期答案**
- 遞迴深度超過 2 * log₂(n) 時
- 表示分割極度不平衡，可能進入 O(n²) 軌跡
- Heapsort 備用保證 O(n log n)
- 實踐中很少觸發，大多數資料集用 Quicksort 完成

## 參考資源

- **lib/sort.c**：Introsort 實現
- **lib/bsearch.c**：溢出安全的二分搜尋
- **lib/list_sort.c**：鏈表歸併排序
- **include/linux/sort.h**：排序 API
- **kernel/power/snapshot.c**：排序使用案例
