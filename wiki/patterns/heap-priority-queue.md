# Heap / Priority Queue 堆積與優先佇列模式

## 概述 Overview

堆積是一種特殊的樹形數據結構，滿足堆積性質。最小堆中，父節點 <= 子節點；最大堆中，父節點 >= 子節點。優先佇列通常用堆積實現，提供 O(log n) 的插入和刪除操作。

本模式涵蓋基於陣列的堆積實現、最小堆和最大堆、堆積排序、Top K 問題，以及雙堆維護中位數等應用。

## 核心概念 Core Concepts

### 1. 堆積的陣列表示

在陣列中，索引 i 的節點：
- 左子節點：2*i + 1
- 右子節點：2*i + 2
- 父節點：(i - 1) / 2

### 2. 最小堆操作

```c
// 堆積數據結構
typedef struct {
    int* data;
    int size;
    int capacity;
} MinHeap;

// 創建最小堆
MinHeap* create_min_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = (int*)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// 交換兩個元素
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 向上調整（插入後）
void sift_up(MinHeap* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->data[parent] > heap->data[index]) {
            swap(&heap->data[parent], &heap->data[index]);
            index = parent;
        } else {
            break;
        }
    }
}

// 向下調整（刪除後）
void sift_down(MinHeap* heap, int index) {
    while (1) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size && heap->data[left] < heap->data[smallest]) {
            smallest = left;
        }
        if (right < heap->size && heap->data[right] < heap->data[smallest]) {
            smallest = right;
        }

        if (smallest != index) {
            swap(&heap->data[smallest], &heap->data[index]);
            index = smallest;
        } else {
            break;
        }
    }
}

// 插入元素
void min_heap_insert(MinHeap* heap, int val) {
    if (heap->size >= heap->capacity) return;
    heap->data[heap->size] = val;
    sift_up(heap, heap->size);
    heap->size++;
}

// 提取最小值
int min_heap_extract(MinHeap* heap) {
    if (heap->size == 0) return -1;
    int min_val = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    if (heap->size > 0) {
        sift_down(heap, 0);
    }
    return min_val;
}

// 獲取最小值（不刪除）
int min_heap_peek(MinHeap* heap) {
    return heap->size > 0 ? heap->data[0] : -1;
}

// 釋放堆積
void free_min_heap(MinHeap* heap) {
    free(heap->data);
    free(heap);
}
```

### 3. 最大堆操作

最大堆邏輯與最小堆相同，只需改變比較方向：

```c
void sift_up_max(MinHeap* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->data[parent] < heap->data[index]) {
            swap(&heap->data[parent], &heap->data[index]);
            index = parent;
        } else {
            break;
        }
    }
}

void sift_down_max(MinHeap* heap, int index) {
    while (1) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size && heap->data[left] > heap->data[largest]) {
            largest = left;
        }
        if (right < heap->size && heap->data[right] > heap->data[largest]) {
            largest = right;
        }

        if (largest != index) {
            swap(&heap->data[largest], &heap->data[index]);
            index = largest;
        } else {
            break;
        }
    }
}
```

### 4. 自定義比較器

對於複雜對象（如頻率計數），需要自定義比較方式：

```c
// 頻率對象
typedef struct {
    int value;
    int freq;
} FreqItem;

typedef struct {
    FreqItem* data;
    int size;
    int capacity;
} MinHeapFreq;

// 自定義比較和調整
void sift_down_freq(MinHeapFreq* heap, int index) {
    while (1) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size &&
            heap->data[left].freq < heap->data[smallest].freq) {
            smallest = left;
        }
        if (right < heap->size &&
            heap->data[right].freq < heap->data[smallest].freq) {
            smallest = right;
        }

        if (smallest != index) {
            FreqItem temp = heap->data[smallest];
            heap->data[smallest] = heap->data[index];
            heap->data[index] = temp;
            index = smallest;
        } else {
            break;
        }
    }
}
```

## 常見應用 Common Applications

### 應用 1：Top K 問題

使用最小堆維護 k 個最大元素。時間複雜度 O(n log k)。

**Problem 215: Kth Largest Element in an Array**
```c
int findKthLargest(int* nums, int numsSize, int k) {
    MinHeap* heap = create_min_heap(k);

    // 建堆：前 k 個元素
    for (int i = 0; i < k && i < numsSize; i++) {
        min_heap_insert(heap, nums[i]);
    }

    // 遍歷剩餘元素
    for (int i = k; i < numsSize; i++) {
        if (nums[i] > min_heap_peek(heap)) {
            min_heap_extract(heap);
            min_heap_insert(heap, nums[i]);
        }
    }

    int result = min_heap_peek(heap);
    free_min_heap(heap);
    return result;
}
```

### 應用 2：QuickSelect 方式

QuickSelect 算法提供 O(n) 平均時間複雜度的解決方案，但最壞情況為 O(n²)。

```c
int partition(int* nums, int left, int right) {
    int pivot = nums[right];
    int i = left;

    for (int j = left; j < right; j++) {
        if (nums[j] > pivot) {  // 降序，大的在前
            swap(&nums[i], &nums[j]);
            i++;
        }
    }
    swap(&nums[i], &nums[right]);
    return i;
}

int quickselect(int* nums, int left, int right, int k_th) {
    if (left == right) {
        return nums[left];
    }

    int pivot_index = partition(nums, left, right);

    if (k_th == pivot_index) {
        return nums[k_th];
    } else if (k_th < pivot_index) {
        return quickselect(nums, left, pivot_index - 1, k_th);
    } else {
        return quickselect(nums, pivot_index + 1, right, k_th);
    }
}
```

### 應用 3：中位數維護 (Problem 295)

使用兩個堆：最大堆存儲較小的一半，最小堆存儲較大的一半。

```c
typedef struct {
    MinHeap* small_max_heap;  // 較小一半，最大堆
    MinHeap* large_min_heap;  // 較大一半，最小堆
} MedianFinder;

double findMedian(MedianFinder* obj) {
    if (obj->small_max_heap->size > obj->large_min_heap->size) {
        return obj->small_max_heap->data[0];
    } else {
        return (obj->small_max_heap->data[0] +
                obj->large_min_heap->data[0]) / 2.0;
    }
}

void addNum(MedianFinder* obj, int num) {
    // 維持不變量：small_max_heap 的所有元素 <= large_min_heap 的所有元素
    if (obj->small_max_heap->size == 0 ||
        num <= obj->small_max_heap->data[0]) {
        // 插入最大堆
        // ... 需要實現最大堆插入
    } else {
        // 插入最小堆
        min_heap_insert(obj->large_min_heap, num);
    }

    // 調整堆的大小，保持平衡
    // ...
}
```

### 應用 4：Top K 頻繁元素 (Problem 347)

使用最小堆或桶排序。

**最小堆方式：**
```c
// 假設已計算頻率並存儲在 freq_items 中
typedef struct {
    int value;
    int freq;
} Item;

int compare(const void* a, const void* b) {
    Item* x = (Item*)a;
    Item* y = (Item*)b;
    return x->freq - y->freq;  // 升序，便於提取最小

}

int* topKFrequent(int* nums, int numsSize, int k, int* returnSize) {
    // 1. 計算頻率
    // 2. 建立最小堆（大小為 k）
    // 3. 堆中存儲 k 個頻率最高的元素
    // ...
}
```

**桶排序方式：**
```c
int* topKFrequent_bucket(int* nums, int numsSize, int k, int* returnSize) {
    // 1. 計算頻率
    // 2. 建立桶，桶的索引是頻率（0 到 n）
    // 3. 將元素放入對應頻率的桶
    // 4. 從最後一個桶開始，收集 k 個元素

    // 空間複雜度：O(n)
    // 時間複雜度：O(n)
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 215 | Kth Largest Element in an Array | Med | QuickSelect + 堆積 |
| 295 | Find Median from Data Stream | Hard | 雙堆維護中位數 |
| 347 | Top K Frequent Elements | Med | 桶排序/最小堆 |
| 480 | Sliding Window Median | Hard | 雙堆+延遲刪除 |
| 692 | Top K Frequent Words | Med | 最小堆+自訂比較器 |
| 703 | Kth Largest Element in a Stream | Easy | 最小堆維護 k 個最大 |

## 內核連接 Kernel Connection

Linux 內核在以下地方使用堆積：

- **min_heap.h**：內核提供的堆積實現，用於優先級隊列
- **定時器管理**：使用堆積管理到期時間最早的定時器
- **進程調度**：CFS 調度器使用紅黑樹（一種自平衡堆積），但堆積概念相似
- **Perf 事件**：使用堆積管理性能事件的採樣

## 時間和空間複雜度

- **堆積插入**：O(log n)
- **堆積刪除根**：O(log n)
- **建堆**：O(n)（使用自下而上的方法）
- **堆積排序**：O(n log n)
- **Top K（堆積方式）**：O(n log k)
- **Top K（QuickSelect）**：O(n) 平均，O(n²) 最壞
- **中位數查詢**：O(1) 查詢，O(log n) 插入

空間複雜度通常為 O(k) 或 O(n) 取決於具體實現。

## 除錯技巧 Debugging Tips

1. **驗證堆積性質**：在每次操作後檢查堆積是否滿足堆積性質
2. **列印堆積結構**：用層級順序打印堆積，視覺化其結構
3. **測試邊界情況**：單元素、兩元素、k = 1 等
4. **驗證索引計算**：確保左/右子節點和父節點計算正確
5. **測試 sift_up/sift_down**：單獨測試向上和向下調整函數
6. **檢查陣列邊界**：確保訪問索引不超出堆積大小

## 實踐建議 Practice Tips

1. 先實現基本的最小堆（插入、刪除、堆積化）
2. 理解陣列索引與樹結構的對應關係
3. 掌握 sift_up 和 sift_down 的邏輯
4. 從 Top K 問題開始應用堆積
5. 練習使用堆積解決其他問題
6. 理解為什麼某些問題用堆積比排序高效
7. 學習雙堆的思想，用於中位數等問題

## 常見錯誤 Common Mistakes

1. **索引計算錯誤**：左右子節點或父節點計算不對
2. **堆積性質破壞**：sift_up 或 sift_down 實現不當
3. **邊界檢查不足**：訪問超出堆積大小的位置
4. **初始化不完整**：未正確初始化堆積結構
5. **比較方向混亂**：最大堆和最小堆的比較方向搞反
6. **忽視延遲刪除**：在某些問題中需要標記為刪除而不是實際刪除
7. **自定義比較器錯誤**：用於複雜對象時，比較邏輯不正確
