# LeetCode 295 - Find Median from Data Stream 記憶體分析

## 概述
此題使用雙堆（Two Heaps）的設計，利用最大堆儲存資料流較小的一半，最小堆儲存較大的一半，維持平衡以快速計算中位數。本實現採用陣列型堆（array-based heap）搭配動態 realloc 調整容量。

---

## 1. Struct 記憶體佈局

### 結構定義與欄位大小

```c
typedef struct {
    int* max_heap;      // pointer to max-heap array (8 bytes, 64-bit)
    int* min_heap;      // pointer to min-heap array (8 bytes, 64-bit)
    int max_size;       // current count in max-heap (4 bytes)
    int min_size;       // current count in min-heap (4 bytes)
    int max_capacity;   // allocated capacity of max-heap (4 bytes)
    int min_capacity;   // allocated capacity of min-heap (4 bytes)
} MedianFinder;
// 總大小 = 8 + 8 + 4 + 4 + 4 + 4 = 32 bytes (64-bit 系統，無額外 padding)
```

### MedianFinder 結構在記憶體中的佈局 (假設結構位址 0xA000)

```
位址範圍          欄位名稱          大小      內容範例
┌─────────────┬──────────────┬────────┬─────────────────┐
│  0xA000     │ max_heap     │ 8 bytes│  0xC000 ────┐   │
├─────────────┼──────────────┼────────┼─────────────┼──┤
│  0xA008     │ min_heap     │ 8 bytes│  0xD000 ────┐  │
├─────────────┼──────────────┼────────┼─────────────┼──┤
│  0xA010     │ max_size     │ 4 bytes│  0x00000002 │  │
├─────────────┼──────────────┼────────┼─────────────┤  │
│  0xA014     │ min_size     │ 4 bytes│  0x00000001 │  │
├─────────────┼──────────────┼────────┼─────────────┤  │
│  0xA018     │ max_capacity │ 4 bytes│  0x00000010 │  │
├─────────────┼──────────────┼────────┼─────────────┤  │
│  0xA01C     │ min_capacity │ 4 bytes│  0x00000010 │  │
└─────────────┴──────────────┴────────┴─────────────┘  │
                                                       │
 ┌─────────────────────────────────────────────────────┘
 │
 ▼
max_heap 陣列 @ 0xC000
┌────────┬────────┬─────────────┬───────┬───────┐
│  0xC000│  0xC004│   0xC008    │ ..... │       │
├────────┼────────┼─────────────┼───────┴───────┘
│   2    │   1    │   unused    │
└────────┴────────┴─────────────┘

min_heap 陣列 @ 0xD000
┌────────┬────────┬─────────────┬───────┬───────┐
│  0xD000│  0xD004│   0xD008    │ ..... │       │
├────────┼────────┼─────────────┼───────┴───────┘
│   3    │ unused │   unused    │
└────────┴────────┴─────────────┘
```

**註記：** 所有位址均為假設位址，用於說明記憶體佈局。

---

## 2. 完整資料結構記憶體配置

### 操作序列：addNum(1), addNum(2), addNum(3) 的最終狀態

#### 逐步追蹤：

**步驟 1：addNum(1)**
- max_heap 為空，新元素添加至 max_heap
- max_heap = [1]，max_size = 1
- min_heap = []，min_size = 0
- 中位數 = 1.0

**步驟 2：addNum(2)**
- 2 > 1（max_heap 最大值），添加至 min_heap
- min_heap = [2]，min_size = 1
- max_heap = [1]，max_size = 1
- 大小相等，中位數 = (1 + 2) / 2 = 1.5

**步驟 3：addNum(3)**
- 3 > max_heap 最大值（1），添加至 min_heap
- min_heap = [2, 3]，min_size = 2
- 不平衡：min_size > max_size + 1，彈出 min_heap 最小值 2 至 max_heap
- 新增堆化：max_heap = [2, 1]，max_size = 2
- min_heap = [3]，min_size = 1
- 中位數 = max_heap 最大值 = 2.0

#### 最終記憶體配置（假設位址）

```
MedianFinder 結構 @ 0xA000
┌──────────────────────────────────────────────┐
│ max_heap (pointer) ───▶ 0xC000              │
│ min_heap (pointer) ───▶ 0xD000              │
│ max_size = 2                                 │
│ min_size = 1                                 │
│ max_capacity = 10 (initial)                  │
│ min_capacity = 10 (initial)                  │
└──────────────────────────────────────────────┘

max_heap 陣列 @ 0xC000（最大堆，存儲較小的一半）
┌───────────┬───────────┬──────────┬──────────┐
│ 位址 0xC000│ 位址 0xC004│ 位址 0xC008│位址 0xC00C│
├───────────┼───────────┼──────────┼──────────┤
│    2      │    1      │ (unused) │ (unused) │
├───────────┴───────────┴──────────┴──────────┤
│ 當前元素個數：2                              │
│ 已分配容量：10 個 int (40 bytes)              │
└───────────────────────────────────────────────┘

min_heap 陣列 @ 0xD000（最小堆，存儲較大的一半）
┌───────────┬───────────┬──────────┬──────────┐
│ 位址 0xD000│ 位址 0xD004│ 位址 0xD008│位址 0xD00C│
├───────────┼───────────┼──────────┼──────────┤
│    3      │ (unused)  │ (unused) │ (unused) │
├───────────┴───────────┴──────────┴──────────┤
│ 當前元素個數：1                              │
│ 已分配容量：10 個 int (40 bytes)              │
└───────────────────────────────────────────────┘
```

---

## 3. 陣列索引版對應

### 堆陣列索引公式

對於索引為 `i` 的元素：
- **父節點索引** = `(i - 1) / 2`（整數除法）
- **左子節點索引** = `2 * i + 1`
- **右子節點索引** = `2 * i + 2`

### max_heap 邏輯樹結構：[2, 1]

```
max_heap (最大堆)
     2 (index=0, max_heap[0])
    /
   1 (index=1, max_heap[1])

索引映射：
  max_heap[0]=2 (根)
    │
    └─── max_heap[1]=1 (左子)
```

### min_heap 邏輯樹結構：[3]

```
min_heap (最小堆)
     3 (index=0, min_heap[0])
```

### 記憶體與邏輯樹的對應

```
max_heap 記憶體布局：
0xC000 ──▶ value=2  ◄── 邏輯根節點
   │
0xC004 ──▶ value=1  ◄── 邏輯左子
   │
0xC008 ──▶ (未使用)

邏輯樹視圖：
    [2]
    /
  [1]

heap 性質：
  - max_heap[0]=2 ≥ max_heap[1]=1 ✓ (max-heap property)
```

---

## 4. 搜尋／操作過程追蹤

### LeetCode 範例 1：Median from Data Stream

**初始化：**
```
MedianFinder* obj = medianFinderCreate();
max_heap: []  (capacity=1, size=0)
min_heap: []  (capacity=1, size=0)
```

#### **步驟 1：addNum(1)**

```
執行流程：
1. max_heap 為空 ── 新數字 1 應加入 max_heap
2. 分配記憶體（若需）
3. max_heap[0] = 1
4. max_size = 1

結果：
max_heap @ 0xC000: [1]      (size=1, capacity=1)
min_heap @ 0xD000: []       (size=0, capacity=1)

getMedian() ── max_heap.size=1, min_heap.size=0
  └─ 回傳 max_heap[0] = 1.0 ✓
```

#### **步驟 2：addNum(2)**

```
執行流程：
1. 2 > max_heap[0] (即 2 > 1)？是
2. 新數字 2 應加入 min_heap
3. min_heap[0] = 2
4. min_size = 1
5. 平衡檢查：max_size=1, min_size=1 ── 已平衡

結果：
max_heap @ 0xC000: [1]      (size=1)
min_heap @ 0xD000: [2]      (size=1)

getMedian() ── max_size=min_size=1
  └─ 回傳 (max_heap[0] + min_heap[0]) / 2.0
  └─ 回傳 (1 + 2) / 2.0 = 1.5 ✓
```

#### **步驟 3：addNum(3)**

```
執行流程：
1. 3 > max_heap[0] (即 3 > 1)？是
2. 嘗試加入 min_heap
3. min_heap 容量不足 ── realloc(min_heap, 2*capacity)
   - 舊容量：1，新容量：2
   - 記憶體複製：min_heap[0]=2 → 新址 0xD000
   - min_heap[1] = 3（新加入）
4. min_size = 2
5. 平衡檢查：max_size=1, min_size=2
   ── 不平衡！min_size > max_size + 1
6. 彈出 min_heap 最小值至 max_heap
   - pop_min_heap() 回傳 2
   - max_heap 容量不足 ── realloc(max_heap, 2*capacity)
   - max_heap[0]=1 → 新址 0xC000
   - max_heap[1] = 2（新加入）
   - 堆化 max_heap（向下調整）
     └─ max_heap[0]=1, max_heap[1]=2
     └─ 1 < 2，交換 ── max_heap[0]=2, max_heap[1]=1 ✓
7. max_size = 2, min_size = 1

結果：
max_heap @ 0xC000: [2, 1]   (size=2, capacity=2)
  [2] (index=0)
   │
  [1] (index=1)

min_heap @ 0xD000: [3]      (size=1, capacity=2)
  [3] (index=0)

getMedian() ── max_size=2, min_size=1
  └─ 回傳 max_heap[0] = 2.0 ✓
```

### 記憶體位址變化表

```
操作          max_heap 位址    min_heap 位址    max_size  min_size
──────────────────────────────────────────────────────────────────
初始化        (null)           (null)           0         0
addNum(1)     0xC000 [1]       (null)           1         0
addNum(2)     0xC000 [1]       0xD000 [2]       1         1
addNum(3)     0xC100 [2,1]     0xD100 [3]       2         1
             (realloc)        (realloc)
```

---

## 5. 指標變數追蹤總表

| 步驟 | 操作 | max_heap 狀態 | min_heap 狀態 | max_size | min_size | 計算中位數 | 備註 |
|------|------|--------------|-------------|----------|----------|----------|------|
| 0 | 初始化 | [] | [] | 0 | 0 | — | 兩堆皆空 |
| 1 | addNum(1) | [1] | [] | 1 | 0 | 1.0 | max_heap 單獨承載 |
| 2 | addNum(2) | [1] | [2] | 1 | 1 | 1.5 | 兩堆平衡 |
| 3a | addNum(3) 嘗試 | [1] | [2,3] | 1 | 2 | — | min_heap 增長 |
| 3b | addNum(3) 平衡 | [2,1] | [3] | 2 | 1 | 2.0 | min_heap 彈出 2 給 max_heap，堆化調整 |
| 4 | getMedian() | [2,1] | [3] | 2 | 1 | 2.0 | 偶數個元素，max_heap 最大值 |

---

## 6. 記憶體存取模式比較

### 比較三種實現方式

#### 方式 A：陣列型堆（Array-based Heap）— 本題採用

```
優點：
✓ 快速隨機存取 O(1)
✓ 卓越的快取局部性（Cache Locality）
  └─ 連續記憶體塊，CPU 快取命中率高
✓ 記憶體緊湊，開銷小

缺點：
✗ realloc 時的記憶體複製開銷
  └─ 當 size ≥ capacity 時觸發
  └─ 複製所有已有元素 O(n)
✗ 可能造成記憶體碎片化
✗ 動態調整容量時的數據移動

記憶體存取模式：
對於 addNum(x) 和 getMedian()
  ├─ max_heap[0] 快速存取（根節點）
  ├─ 堆化過程：max_heap[i]、max_heap[2*i+1]、max_heap[2*i+2]
  │  └─ 線性掃描堆結構，快取友好
  └─ 時間複雜度：O(log n)，常數較小

典型快取行為：
  0xC000 ◄── [2, 1, ?, ?, ...] ─ 單次快取行載入多個元素
           │   │   └─ 通常在同一快取行（64 bytes）
           └─ 極高命中率
```

#### 方式 B：鏈接式堆（Linked Node Heap）

```
結構：
typedef struct Node {
    int value;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;  // 每個節點：4(value) + 8 + 8 + 8 = 28 bytes (64-bit)

優點：
✓ 無需 realloc，插入時直接分配新節點
✓ 節點位址可以任意分散

缺點：
✗ 指標追蹤開銷（每節點 3 個指標 = 24 bytes）
✗ 快取局部性差
  └─ 左右子節點位址隨機分散
  └─ 堆化過程涉及多次指標解引（pointer dereference）
  └─ 每次解引可能引發 L1 快取未命中
✗ 虛擬記憶體頁錯誤（page faults）風險提高

典型快取缺失：
  0xC000 (node1)
    ├─ left ──▶ 0x9800 (node2)  ◄── 不同快取行，未命中
    └─ right ──▶ 0xE200 (node3) ◄── 不同快取行，未命中

結果：
  - addNum(x)：O(log n)，但常數因子大（多次快取未命中）
  - 記憶體開銷：每個節點 28 bytes vs 陣列 4 bytes
```

#### 方式 C：有序陣列（Sorted Array）

```
結構：int* sorted_arr（維持升序）

優點：
✓ getMedian() 極快：O(1) 直接計算中位數位置
✓ 記憶體緊湊

缺點：
✗ addNum(x) 效率低：O(n)
  └─ 需要在有序位置插入元素
  └─ 後續元素全部右移

典型操作：
  sorted_arr = [1, 2, 3]
  addNum(2.5) ──▶ [1, 2, ?, 3] ◄── 插入位置找到
                 移位 ──▶ [1, 2, 2.5, 3] ◄── 複製開銷
                 時間：O(n)

當有大量 addNum 操作時，遠不如堆高效
  - 例：1000 次 addNum → O(n²) = 500 萬次移位操作
```

### 效能對比表格

| 操作 | 陣列堆 | 鏈接堆 | 有序陣列 |
|------|--------|--------|---------|
| addNum(x) 時間 | O(log n) | O(log n) | O(n) |
| addNum(x) 空間 | O(1) | O(1) 動態 | O(1) |
| getMedian() 時間 | O(1) | O(1) | O(1) |
| 快取效率 | 優秀 ⭐⭐⭐ | 差 ⭐ | 優秀 ⭐⭐⭐ |
| 記憶體開銷 | 小 | 大 | 小 |
| realloc 開銷 | 偶發 O(n) | 無 | 頻繁 O(n) |
| 總體評分 | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ |

---

## 7. 堆疊記憶體對比

### 迭代方案（本題採用）— Stack O(1)

此實現採用**完全迭代**方式，無遞迴調用。

```c
// 堆化（向下調整）── 迭代實現
void max_heapify_down(int* heap, int size, int index) {
    while (1) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left] > heap[largest]) {
            largest = left;
        }
        if (right < size && heap[right] > heap[largest]) {
            largest = right;
        }

        if (largest == index) break;  // 已是堆性質

        // 交換並繼續向下
        int temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;

        index = largest;  // 迭代下移
    }
}
```

**堆疊使用：**
```
執行堆化時的堆疊狀態：
┌──────────────────────────────────┐
│ 返回位址 (addNum 函數)           │
├──────────────────────────────────┤
│ 局部變數：index, largest, left   │
│            right, temp           │
├──────────────────────────────────┤
│ (無遞迴調用，無新堆疊幀)         │
└──────────────────────────────────┘

堆疊深度：常數 O(1)
堆疊記憶體：≈ 100 bytes（包含函數參數和局部變數）
```

### 遞迴方案（假設實現）— Stack O(log n)

如果使用遞迴實現堆化：

```c
// 堆化（向下調整）── 遞迴實現（非高效）
void max_heapify_down_recursive(int* heap, int size, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < size && heap[left] > heap[largest]) {
        largest = left;
    }
    if (right < size && heap[right] > heap[largest]) {
        largest = right;
    }

    if (largest != index) {
        int temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;

        max_heapify_down_recursive(heap, size, largest);  // 遞迴呼叫
    }
}
```

**堆疊使用：**
```
递归过程（堆高度 h = log₂(n)）：

調用 1：max_heapify_down_recursive(heap, size, 0)
┌───────────────────────────────────────────┐
│ 堆疊幀 1：index=0, largest, left, right  │
│ 返回位址指向 addNum()                    │
└───────────────────────────────────────────┘
                  │ 遞迴呼叫
                  ▼
調用 2：max_heapify_down_recursive(heap, size, 1)
┌───────────────────────────────────────────┐
│ 堆疊幀 1：index=0, largest, left, right  │
├───────────────────────────────────────────┤
│ 堆疊幀 2：index=1, largest, left, right  │
│ 返回位址指向 幀 1                        │
└───────────────────────────────────────────┘
                  │ 遞迴呼叫
                  ▼
調用 3：max_heapify_down_recursive(heap, size, 3)
┌───────────────────────────────────────────┐
│ 堆疊幀 1：index=0, ...                   │
├───────────────────────────────────────────┤
│ 堆疊幀 2：index=1, ...                   │
├───────────────────────────────────────────┤
│ 堆疊幀 3：index=3, largest, left, right  │
│ 返回位址指向 幀 2                        │
└───────────────────────────────────────────┘
                  │
                  ▼ (葉子節點，返回)
             (返回 幀 3)
                  │
             (返回 幀 2)
                  │
             (返回 幀 1)
                  │
             (返回 main)

堆疊深度：O(log n)
  n=1000  ──▶ 堆疊幀數 ≈ 10
  n=1,000,000 ──▶ 堆疊幀數 ≈ 20

每幀大小：≈ 50 bytes（參數 + 局部變數 + 返回位址）
總堆疊使用：O(log n) × 50 bytes ≈ 1 KB（最壞情況）
```

### 堆疊使用對比表格

| 方案 | 堆疊深度 | 堆疊記憶體 | 快取效率 | 上下文切換 |
|------|----------|----------|--------|----------|
| 迭代（本題） | O(1) 常數 | ≈ 100 bytes | 高 ⭐⭐⭐ | 無開銷 |
| 遞迴 | O(log n) | ≈ 20 KB | 低 ⭐ | 每層調用 |
| n=10000 對比 | — | — | — | — |
| 迭代 | 1 層 | 100 B | — | 0 次額外調用 |
| 遞迴 | 14 層 | 14 × 50 B = 700 B | — | 14 次額外調用 |

**結論：** 迭代方案明顯優於遞迴，無堆疊溢出風險，快取效率更高。

---

## 8. 退化情況分析

### 情景 1：Realloc 連鎖反應 — 容量翻倍模式

**初始化與成長模式：**

```
初始容量：max_capacity = 1

addNum 序列模擬：
┌──────────────┬────────────┬──────────────┬─────────────┐
│ 累積元素數   │ 當前容量   │ 觸發 realloc │ 新容量      │
├──────────────┼────────────┼──────────────┼─────────────┤
│ 1            │ 1          │ 否           │ 1           │
│ 2            │ 1          │ 是 (2 > 1)   │ 2           │
│ 3            │ 2          │ 是 (3 > 2)   │ 4           │
│ 5            │ 4          │ 是 (5 > 4)   │ 8           │
│ 9            │ 8          │ 是 (9 > 8)   │ 16          │
│ 17           │ 16         │ 是 (17 > 16) │ 32          │
│ ...          │ ...        │ ...          │ ...         │
│ 1000         │ 512        │ 是           │ 1024        │
│ 10000        │ 8192       │ 否           │ 8192        │
└──────────────┴────────────┴──────────────┴─────────────┘

realloc 次數估算（達成 n 個元素）：
  f(n) ≈ log₂(n) 次

例：n=1000
  log₂(1000) ≈ 10 次 realloc
```

### 情景 2：記憶體碎片化

```
realloc 過程：

初始分配 @ 0x1000：capacity=4
┌─────────────────────────┐
│ [1, 2, 3, 4, ?, ?, ...] │
└─────────────────────────┘
0x1000     0x1010     0x1020

需要 realloc，容量翻倍至 8
舊記憶體釋放（可能不立即回收），新記憶體 @ 0x3000
┌─────────────────────────────────────────────┐
│ [1, 2, 3, 4, 5, 6, 7, 8, ?, ?, ...] (容量 8)│
└─────────────────────────────────────────────┘
0x3000     0x3020     0x3040

0x1000 的 16 bytes 成為碎片
  └─ 若頻繁 realloc，記憶體碎片堆積
  └─ 系統可用連續記憶體減少
  └─ 最終可能導致記憶體不足（即使總可用 > 需要）
```

### 情景 3：堆結構本身無退化

```
堆的結構性質：
  ✓ 永遠維持「完全二叉樹」(Complete Binary Tree)
  ✓ 高度 h = ⌈log₂(n)⌉，不會不平衡

反例 — 二叉搜尋樹會退化：
        1          ◄── 未排序插入可導致線性鏈
        │
        2
        │
        3
        │
        ...
        │
        n

但堆不同：
         4
        / \
       2   3
      / \
     1   5  ◄── 總是完全二叉樹，高度 O(log n)
```

### 情景 4：最壞情況 — 大量 realloc

```
測試場景：連續插入 1,000,000 個元素

操作計次：
  - addNum() 調用：1,000,000 次
  - heapify_up/down：每次 O(log n)，共 ≈ 20,000,000 次操作
  - realloc 觸發：≈ 20 次
    └─ 每次複製 O(n)，第 k 次複製 2^(k-1) 個元素
    └─ 總複製元素數：1 + 2 + 4 + ... + 524288 ≈ 1,000,000

時間複雜度分析：
  T = 1,000,000 × O(log n) + realloc 複製開銷
    = 20,000,000 次堆操作 + 1,000,000 次記憶體複製
    ≈ 20M + 1M = 21M 單位操作
    (相比鏈接堆的 0 realloc，多了 ~5% 開銷)

記憶體峰值：
  - 最終堆大小：1M × 4 bytes = 4 MB
  - 最後一次 realloc 時臨時使用 2 個副本：8 MB 峰值
  - 碎片化影響：≈ 10-20% 記憶體浪費

結論：
  - 對 LeetCode 規模（通常 ≤ 100k 元素），無關痛癢
  - 生產系統可考慮使用固定大小預分配或鏈接堆
  - 本實現仍然高效 ✓
```

### realloc 倍增成本曲線

```
記憶體分配與複製成本示意圖：

容量
  |
1K|                           ╱
  |                       ╱
  |                   ╱
512|               ╱
  |           ╱
  |       ╱
256|   ╱
  | ╱
128|______╱
  |
  0────────────────────── addNum 操作次序
    1   2  4  8 16 32 64 128 256 512 1K

realloc 觸發點（容量超出）：
  操作 1：capacity=1 (無 realloc)
  操作 2：capacity=2 (realloc #1，複製 1 個元素)
  操作 3：capacity=4 (realloc #2，複製 2 個元素)
  操作 5：capacity=8 (realloc #3，複製 4 個元素)
  操作 9：capacity=16 (realloc #4，複製 8 個元素)
  ...

攤銷複雜度（Amortized Complexity）：
  總複製元素 = 1 + 2 + 4 + 8 + ... + n/2 = n - 1
  平均每個 addNum：(n - 1) / n ≈ O(1)
  ✓ 攤銷後 addNum 時間複雜度 = O(log n) + O(1) = O(log n)
```

---

## 9. 總結

### 本實現的關鍵特點

```
┌─────────────────────────────────────────────────────────┐
│ 雙堆 (Two Heaps) 設計                                  │
├─────────────────────────────────────────────────────────┤
│ max_heap：存儲較小的一半（最大堆性質）                │
│ min_heap：存儲較大的一半（最小堆性質）                │
│                                                         │
│ 關鍵不變式：                                           │
│  1. max_heap.size ∈ {min_heap.size, min_heap.size+1}  │
│  2. max_heap 的最大值 ≤ min_heap 的最小值             │
│  3. 中位數 = max_heap.top (奇數) 或                   │
│            (max_heap.top + min_heap.top) / 2 (偶數)  │
│                                                         │
│ 時間複雜度：                                           │
│  - addNum(x)：O(log n) 堆化 + O(1) 攤銷 realloc       │
│  - getMedian()：O(1)                                   │
│                                                         │
│ 空間複雜度：O(n) 堆陣列 + O(1) 結構開銷                │
└─────────────────────────────────────────────────────────┘

記憶體架構優勢：
  ✓ 連續陣列，快取友好
  ✓ 無指標追蹤開銷（vs 鏈接堆）
  ✓ 簡潔的索引公式（parent, left, right）
  ✓ 迭代實現，堆疊開銷最小

適用場景：
  ✓ 流式資料中位數計算（LeetCode 295）
  ✓ 動態排序的前 k 大/小元素
  ✓ 優先佇列實現
  ✗ 極度動態的環境（頻繁刪除任意元素）
```

---

**文件結束**

*所有位址均為假設位址，僅供記憶體佈局說明之用。*
