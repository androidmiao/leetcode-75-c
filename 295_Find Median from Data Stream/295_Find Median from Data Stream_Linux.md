# LeetCode 295：尋找資料流中的中位數 - Linux Kernel 堆實作對照

## 問題概述

**題目**：設計一個資料結構支援：
- `addNum(num)` - 新增整數，時間複雜度 O(log n)
- `findMedian()` - 查詢中位數，時間複雜度 O(1)

**演算法**：Two Heaps（最大堆 + 最小堆）
- 最大堆：儲存較小的一半數字，堆頂為該半部的最大值
- 最小堆：儲存較大的一半數字，堆頂為該半部的最小值
- 不變式：`max_heap.size == min_heap.size` (偶數) 或 `max_heap.size == min_heap.size + 1` (奇數)

---

## Linux 相關原始碼位置

### 核心堆實作檔案

| 檔案位置 | 用途 | 特點 |
|---------|------|------|
| `include/linux/min_heap.h` | 通用最小堆實作（5.x+ 新增） | 基於陣列，泛型比較器設計 |
| `kernel/events/core.c` | perf 事件管理 | 使用 `min_heap.h` 進行事件堆積 |
| `lib/prio_heap.c` | 優先級堆（舊核心）| 已逐漸被 `min_heap.h` 取代 |
| `lib/min_heap.h` | 實作的標頭檔（核心堆操作） | 定義 push/pop/peek/sift 操作 |
| `kernel/time/timer.c` | 計時器管理 | 使用堆維護計時器隊列 |
| `kernel/workqueue.c` | 工作隊列 | 使用優先級堆管理延遲工作 |

### GitHub Kernel Source 連結

- Linux kernel min_heap 主要實作：
  https://github.com/torvalds/linux/blob/master/include/linux/min_heap.h

- Perf 事件堆積應用：
  https://github.com/torvalds/linux/blob/master/kernel/events/core.c

---

## Kernel vs. LeetCode 實作對照

### 1. 陣列表示法（Array-based Representation）

#### LeetCode 解決方案
```c
/* 分開維護最大堆和最小堆 */
typedef struct {
    int* max_heap;       // 陣列指標
    int* min_heap;       // 陣列指標
    int max_size;        // 目前元素數量
    int min_size;
    int max_capacity;    // 動態容量
    int min_capacity;
} MedianFinder;

/* 索引關係 */
parent(i) = (i - 1) / 2
left(i) = 2 * i + 1
right(i) = 2 * i + 2
```

#### Linux Kernel 實作（`min_heap.h`）
```c
/* 通用堆結構，支援任意型別 */
struct min_heap {
    void *data;          // 陣列指標（泛型）
    int nr;              // 目前元素數量
    int size;            // 容量大小
};

/* 操作定義為巨集，接受比較器 */
#define min_heap_push(heap, element, cmp, args) ({ ... })
#define min_heap_pop(heap, cmp) ({ ... })
#define min_heap_peek(heap) ((heap)->data[0])
```

**對照分析**：
| 面向 | LeetCode | Linux Kernel |
|-----|---------|--------------|
| **型別系統** | `int*` 硬編碼 | `void*` 泛型 + 比較器 |
| **堆操作** | 函式 (maxHeapifyUp/Down) | 巨集 (inline，更快) |
| **比較邏輯** | > 或 < (硬編碼) | 使用者提供的 `cmp` 函式指標 |
| **容量管理** | 分開管理 max/min_capacity | 統一容量管理 |
| **記憶體佈局** | 兩個不連續陣列 | 單一陣列（可能更好的 cache locality） |

---

### 2. Heapify 操作

#### 最大堆向上調整（LeetCode）
```c
void maxHeapifyUp(int* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[parent] >= heap[index]) break;  // 停止條件
        swap(&heap[parent], &heap[index]);
        index = parent;
    }
}
/* 時間複雜度：O(log n)，最多沿樹高向上移動 */
```

#### Linux Kernel 的 Sift-Up 邏輯
```c
/* 定義在 min_heap.h 中的巨集 */
#define min_heap_sift_up(heap, pos, cmp, args) ({
    /*
       - 邏輯類似，但使用 cmp 比較器
       - 通常內聯展開，避免函式呼叫開銷
       - 編譯器可優化為單個比較和交換迴圈
    */
})
```

**實作差異**：
- LeetCode：函式呼叫，可能有棧開銷
- Kernel：巨集，完全內聯，零開銷
- Kernel 會在編譯時為具體型別生成特化程式碼

#### 最小堆向下調整（LeetCode）
```c
void minHeapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {  // 至少有左子
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left] < heap[smallest])
            smallest = left;
        if (right < size && heap[right] < heap[smallest])
            smallest = right;

        if (smallest == index) break;
        swap(&heap[index], &heap[smallest]);
        index = smallest;
    }
}
```

**Linux Kernel 對應**：
```c
/* kernel/events/core.c 中使用示例 */
min_heap_pop(heap, perf_event_cmp);
/* 內部執行 sift-down，維護堆性質 */
```

---

### 3. 比較器設計（Comparator-based API）

#### LeetCode：硬編碼比較

```c
/* 最大堆：檢查 heap[parent] >= heap[index] */
if (heap[parent] >= heap[index]) break;

/* 最小堆：檢查 heap[parent] <= heap[index] */
if (heap[parent] <= heap[index]) break;
```

**缺點**：
- 想要堆排序不同型別？需要複製程式碼
- 想要自訂排序順序？無法靈活變更
- 代碼重複率高

#### Linux Kernel：通用比較器

```c
/* 定義比較函式 */
static int perf_event_cmp(const void *lhs, const void *rhs) {
    const struct perf_event *le = lhs;
    const struct perf_event *re = rhs;
    return le->attr.sample_period < re->attr.sample_period ? -1 : 1;
}

/* 直接使用通用堆 API */
min_heap_push(&event_heap, event, perf_event_cmp, NULL);

/* 或用於排序 */
min_heap_pop(&event_heap, perf_event_cmp);
```

**優點**：
- 單份堆實作，支援任意型別
- 比較邏輯可動態改變（函式指標）
- 符合函數式程式設計原則

---

### 4. 動態擴容（Dynamic Resizing）

#### LeetCode：倍增策略 + `realloc`

```c
if (obj->max_size >= obj->max_capacity) {
    obj->max_capacity *= 2;  // 容量翻倍
    obj->max_heap = (int*)realloc(obj->max_heap,
                                  obj->max_capacity * sizeof(int));
}
```

**風險點**：
- `realloc` 失敗時返回 NULL，直接賦值會導致原指標遺失（記憶體洩漏）
- 沒有檢查 `realloc` 返回值

**更安全的實作**：
```c
int* new_heap = (int*)realloc(obj->max_heap,
                              new_capacity * sizeof(int));
if (!new_heap) {
    /* 處理記憶體不足錯誤 */
    return ENOMEM;
}
obj->max_heap = new_heap;
obj->max_capacity = new_capacity;
```

#### Linux Kernel：`kvrealloc` + 錯誤處理

```c
/* kernel/events/core.c 中 */
void *new_data = kvrealloc(heap->data, old_size, new_size, GFP_KERNEL);
if (!new_data)
    return -ENOMEM;  // 明確的錯誤返回

heap->data = new_data;
heap->size = new_size;
```

**Kernel 優點**：
- `kvrealloc` 整合虛擬記憶體管理
- GFP flags 允許配置策略選擇（`GFP_KERNEL`, `GFP_ATOMIC` 等）
- 完整的錯誤路徑處理
- 不會因為重分配失敗而洩漏記憶體

---

## 主管 Code Review 角度考題

### Q1：為什麼選擇堆而不是紅黑樹？

**Kernel 視角**：
- **堆**：插入/刪除 O(log n)，查詢最值 O(1)
- **BST（如 RB-tree）**：所有操作 O(log n)，但常數係數大

在 `MedianFinder` 中，`findMedian()` 被頻繁呼叫。堆選擇使 `findMedian()` 達到 **O(1)**，這在流式場景中很關鍵。

**Kernel 例子**：
- **CFS 排程器**（舊版）：使用 RB-tree 管理可執行隊列
- **EEVDF 排程器**（Linux 6.6+）：仍使用 augmented tree，但加上堆優化
- **Perf events**：使用堆因為只關心最優先事件（堆頂），不需全序

### Q2：Cache Locality 分析

**陣列堆的優勢**：
```
記憶體布局：[0][1][2][3][4][5][6]...

導航：index i → parent (i-1)/2 → 通常在同一快取行內
訪問圖式：連續或接近地址
```

**Kernel 實踐**：
```c
/* include/linux/min_heap.h 的設計目標之一 */
/* 陣列連續存儲，prefetch-friendly */
struct min_heap {
    void *data;    /* 單一連續分配 */
    int nr;
    int size;
};
```

**LeetCode 問題**：
```c
/* 兩個分開的陣列 */
int* max_heap;    /* 可能不連續 */
int* min_heap;    /* 可能不連續 */
```

改進方案：
```c
/* 單一陣列，分為上半部分（最大堆）和下半部分（最小堆） */
typedef struct {
    int* data;
    int max_end;   // max_heap 結尾索引
    int min_start; // min_heap 起始索引
} MedianFinder;
```

### Q3：Amortized 複雜度分析

**`realloc` 的攤銷成本**：

假設初始容量 1000，每次倍增：
```
元素數  容量  成本        攤銷成本
1000    1000  0           0
2000    2000  2000 次複製  1
4000    4000  4000 次複製  1
...
```

攤銷成本：每個元素平均 O(1) 次複製（透過倍增策略）

**Kernel 優化**：
```c
/* kernel/events/core.c 可能使用增量式擴容 */
if (heap->nr == heap->size) {
    size_t new_size = heap->size + (heap->size >> 1); // 1.5 倍
    new_data = kvrealloc(heap->data, ..., new_size, ...);
}
```

### Q4：防禦式程式設計：`realloc` 失敗

**現有代碼問題**：
```c
obj->max_heap = (int*)realloc(obj->max_heap,
                              obj->max_capacity * sizeof(int));
/* 若 realloc 失敗返回 NULL，原指標遺失 */
```

**Kernel 模式**：
```c
void *tmp = realloc(ptr, new_size);
if (!tmp) {
    /* 記錄錯誤 */
    pr_err("Heap reallocation failed\n");
    return -ENOMEM;
}
ptr = tmp;
```

**改進的 LeetCode 實作**：
```c
void* temp = realloc(obj->max_heap, new_capacity * sizeof(int));
if (!temp) {
    /* 選項 1：拋出錯誤或標記失敗狀態 */
    fprintf(stderr, "Memory allocation failed\n");
    return;  /* 或設置 error flag */
}
obj->max_heap = temp;
obj->max_capacity = new_capacity;
```

### Q5：為什麼 Linux Kernel 用巨集而不是內聯函式？

**巨集（`min_heap_push` 等）**：
```c
#define min_heap_push(heap, element, cmp, args) do { \
    ... \
} while(0)
```

**優點**：
- 編譯時完全展開，零函式呼叫開銷
- 支援任意型別（泛型）
- 比較器直接內聯優化
- 巨集引數沒有型別檢查但可完全最佳化

**缺點**：
- 偶然重新計算副作用引數
- 除錯較困難（巨集展開後看不到原始程式碼）

**LeetCode 可借鑑**：
- 如果效能關鍵，將 `heapifyUp/Down` 改為內聯函式
  ```c
  static inline void maxHeapifyUp(int* heap, int index)
  ```

### Q6：兩堆大小不變式的維護成本

**不變式檢查**：
```c
if (obj->max_size > obj->min_size + 1) { /* 移出 */ }
else if (obj->min_size > obj->max_size) { /* 移出 */ }
```

**實作成本**：
- 每次 `addNum()` 最多進行 1 次跨堆移動
- 跨堆移動 = pop + push = 2 × O(log n) = O(log n)
- 總時間仍為 O(log n)

**Kernel 觀點**：
- Perf events 也維護優先級約束
- 使用單一堆 + 外部樹維護多個約束條件
- 權衡點：額外的 pop/push vs. 複雜的重排邏輯

---

## 主要差異表格

| 特性 | LeetCode 295 | Linux Kernel min_heap.h |
|------|------------|------------------------|
| **資料型別** | `int` 硬編碼 | `void*` 泛型 |
| **堆操作** | 函式 (4 個：max/min up/down) | 巨集 (inline 最佳化) |
| **比較邏輯** | > 或 < 硬編碼 | 使用者提供的 `cmp` 函式指標 |
| **堆數量** | 2 個 (max + min) | 1 個（結構可重複使用） |
| **容量管理** | 分開 max_capacity / min_capacity | 統一 size / nr |
| **動態擴容** | `realloc` (倍增) | `kvrealloc`（支援 GFP flags） |
| **錯誤處理** | 無（NULL 指標未檢查） | 完整的 ENOMEM 返回 |
| **Cache 最佳化** | 兩個不連續陣列 | 單一連續陣列 |
| **適用場景** | LeetCode 競賽/簡單應用 | 核心級系統軟體（必須健壯） |

---

## 面試加分總結

### 1. 核心演算法理解
- 能解釋兩堆不變式為何維持 O(1) 查詢中位數
- 理解為何堆優於 BST（此情境下）
- 知道時空權衡：空間 O(n)，時間 addNum O(log n) + findMedian O(1)

### 2. 實作細節掌握
- 陣列堆的索引運算（parent, left, right）
- heapifyUp 和 heapifyDown 的停止條件
- 跨堆平衡邏輯的完整性檢查

### 3. Linux Kernel 知識加分點
- 提及 `include/linux/min_heap.h` 的泛型堆實作
- 説明 Kernel 如何用巨集實現 O(1) 內聯堆操作
- 對比硬編碼比較 vs. 函式指標比較的優缺點
- 知道 `kvrealloc` 和 `realloc` 的差異（GFP flags、錯誤處理）

### 4. 防禦式程式設計
- 指出 LeetCode 解中 `realloc` 失敗未檢查的風險
- 建議檢查返回值並更新指標的正確模式
- 討論 double-free 和記憶體洩漏的預防

### 5. 效能最佳化意識
- Cache locality 分析：為什麼單一連續陣列優於兩個分離陣列
- 巨集 vs. 函式呼叫的開銷（Kernel 選擇巨集的理由）
- Amortized 分析：倍增擴容的攤銷成本

### 6. 系統設計視角
- 流式資料場景下 O(1) 查詢的重要性
- 可擴充性：泛型堆 API vs. 硬編碼特化
- 系統級考慮：記憶體配置策略（GFP flags）、錯誤恢復

---

## 程式碼片段對照

### 場景：擴容時的安全做法

#### LeetCode 現有（不安全）
```c
obj->max_capacity *= 2;
obj->max_heap = (int*)realloc(obj->max_heap,
                              obj->max_capacity * sizeof(int));
/* 若 realloc 失敗，obj->max_heap 變為 NULL，原資料遺失 */
```

#### Linux Kernel 模式（安全）
```c
size_t old_size = heap->size * sizeof(void*);
size_t new_size = (heap->size * 3 / 2) * sizeof(void*);

void *new_data = kvrealloc(heap->data, old_size, new_size, GFP_KERNEL);
if (!new_data) {
    pr_err("Failed to resize heap\n");
    return -ENOMEM;  /* 保留舊資料 */
}
heap->data = new_data;
heap->size = new_size / sizeof(void*);
```

#### LeetCode 改進版
```c
int new_capacity = obj->max_capacity * 2;
int* new_heap = (int*)realloc(obj->max_heap,
                              new_capacity * sizeof(int));
if (!new_heap) {
    perror("realloc");
    /* 保留舊容量和舊資料，可選：停止操作或傳播錯誤 */
    return;
}
obj->max_heap = new_heap;
obj->max_capacity = new_capacity;
```

---

## Sources

### Kernel 源代碼

- **include/linux/min_heap.h** - Linux 5.x+ 通用堆實作
  https://github.com/torvalds/linux/blob/master/include/linux/min_heap.h

- **kernel/events/core.c** - Perf 事件堆實作應用
  https://github.com/torvalds/linux/blob/master/kernel/events/core.c

- **lib/min_heap.c** - 堆輔助函式（較新核心）
  https://github.com/torvalds/linux/blob/master/lib/min_heap.c

- **kernel/time/timer.c** - 計時器隊列管理
  https://github.com/torvalds/linux/blob/master/kernel/time/timer.c

- **kernel/workqueue.c** - 工作隊列優先級管理
  https://github.com/torvalds/linux/blob/master/kernel/workqueue.c

### 相關 LeetCode 題目

- LeetCode 295：Find Median from Data Stream
- LeetCode 703：Kth Largest Element in a Stream（單堆應用）
- LeetCode 347：Top K Frequent Elements（堆排序）

### 參考文件

- Linux Kernel Documentation: min_heap API
  https://www.kernel.org/doc/html/latest/core-api/kernel-api.html

- 堆資料結構與陣列實作：
  - Cormen, Leiserson, Rivest, Stein. *Introduction to Algorithms* (Ch. 6)

