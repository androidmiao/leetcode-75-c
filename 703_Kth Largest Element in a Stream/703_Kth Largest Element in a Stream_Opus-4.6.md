# LeetCode 703: Kth Largest Element in a Stream

## Implementation vs. Editorial Analysis

The provided C solution uses **Min-Heap approach**, which **matches Editorial Approach 2** exactly. Both implementations maintain a min-heap of size k, where the root element represents the kth largest element in the stream.

### Algorithm Match
- **Editorial Approach 2**: Heap-based solution
- **C Implementation**: Min-heap with manual heapification
- **Verdict**: Same algorithm, different language implementation

---

## Approach Comparison

| Criteria | Editorial Approach 1 (Sorted List) | Editorial Approach 2 (Min-Heap) | C Implementation (Min-Heap) |
|----------|------------------------------------|---------------------------------|------------------------------|
| **Time Complexity (Constructor)** | O(N²) or O(N log N) | O(N log k) | O(N log k) |
| **Time Complexity (add)** | O(M·N) per insertion | O(log k) | O(log k) |
| **Space Complexity** | O(M + N) | O(k) | O(k) |
| **Interview Suitability** | Low (slow, complex) | High (efficient, clear) | High (optimal) |
| **C Implementation Difficulty** | High (need BST/sorted structure) | Medium (manual heap operations) | Medium (manual heap operations) |
| **Overall Performance** | ⭐⭐ Poor | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐⭐ Excellent |

---

## Baseline vs. Recommended Approach

### Baseline Approach: Sorted List (Approach 1)
- Maintains a sorted list of all M + N elements
- Every insertion requires finding the correct position (O(N) search, O(N) shift)
- Kth largest is simply element at index M+N-k
- **Why it's baseline**: Intuitive but inefficient; wastes space and time on irrelevant elements

### Recommended Approach: Min-Heap (Approach 2 & C Implementation)
- Maintains only k elements (the k largest)
- Uses a min-heap where root is the smallest of the k largest (= kth largest overall)
- When new element > root: replace root and heapify down
- When new element ≤ root: ignore it (not in top k)
- **Why recommended**:
  1. **Space efficiency**: O(k) instead of O(M+N)
  2. **Time efficiency**: O(log k) per add instead of O(N)
  3. **Interview appeal**: Shows understanding of heap data structures
  4. **Scalability**: Handles streaming data efficiently regardless of stream length

---

## Detailed C Implementation Breakdown

### Data Structure
```c
typedef struct {
    int* heap;
    int size;
    int capacity;
    int k;
} KthLargest;
```

### Key Operations

**Constructor**: O(N log k)
- Iterate through initial array
- Insert elements while size < k
- If size == k and num > heap[0], replace root and heapify down
- Result: heap contains exactly k largest elements

**add**: O(log k)
- If size < k: insert and heapify up
- If num > heap[0]: replace root and heapify down
- Otherwise: ignore (not in top k)
- Return heap[0] (the kth largest)

**heapifyUp**: Bubble up newly inserted element
**heapifyDown**: Bubble down replaced root to restore heap property

### Why This Works
- Min-heap property ensures parent ≤ children
- Root is always the minimum of the k largest elements
- Maintaining k elements means root = kth largest globally
- New element only matters if it's larger than the current "kth largest"

---

## Interview Discussion Points

1. **Why min-heap instead of max-heap?**
   - With max-heap, root = largest, not kth largest
   - Would need to track all elements, defeating space optimization

2. **Why only maintain k elements?**
   - Elements smaller than the kth largest will never be kth largest
   - Saves space (O(k) vs O(N)) and time (O(log k) vs O(N))

3. **Edge cases handled**:
   - Initial array smaller than k
   - All elements smaller than first element
   - Duplicate elements
   - Single element stream

---

---

# LeetCode 703: 流中第K大的元素

## 實現與官方解法的比較

所提供的 C 解決方案使用**最小堆方法**，這**完全符合官方解法2**。兩種實現都維護大小為 k 的最小堆，其中根節點表示流中第 k 大的元素。

### 演算法匹配
- **官方解法2**：基於堆的解決方案
- **C 實現**：手動實現的最小堆
- **結論**：相同演算法，不同語言實現

---

## 方法比較

| 標準 | 官方解法1（排序列表） | 官方解法2（最小堆） | C 實現（最小堆） |
|------|----------------------|--------------------|------------------|
| **時間複雜度（構造函數）** | O(N²) 或 O(N log N) | O(N log k) | O(N log k) |
| **時間複雜度（新增）** | O(M·N) 每次插入 | O(log k) | O(log k) |
| **空間複雜度** | O(M + N) | O(k) | O(k) |
| **面試適合度** | 低（慢、複雜） | 高（高效、清晰） | 高（最優） |
| **C 實現難度** | 高（需要 BST/排序結構） | 中等（手動堆操作） | 中等（手動堆操作） |
| **整體性能** | ⭐⭐ 差 | ⭐⭐⭐⭐⭐ 優異 | ⭐⭐⭐⭐⭐ 優異 |

---

## 基礎方法 vs. 推薦方法

### 基礎方法：排序列表（解法1）
- 維護所有 M + N 個元素的排序列表
- 每次插入需要找到正確位置（O(N) 搜索，O(N) 移動）
- 第 k 大元素就是索引為 M+N-k 的元素
- **為什麼是基礎方法**：直觀但效率低；浪費空間和時間在無關元素上

### 推薦方法：最小堆（解法2 與 C 實現）
- 僅維護 k 個元素（最大的 k 個）
- 使用最小堆，其中根是 k 個最大元素中最小的（= 整體第 k 大）
- 當新元素 > 根時：替換根並向下堆化
- 當新元素 ≤ 根時：忽略它（不在前 k 中）
- **為什麼推薦**：
  1. **空間效率**：O(k) 而非 O(M+N)
  2. **時間效率**：每次新增 O(log k) 而非 O(N)
  3. **面試吸引力**：展示對堆數據結構的理解
  4. **可擴展性**：無論流長度如何都能有效處理流式數據

---

## C 實現詳細分析

### 數據結構
```c
typedef struct {
    int* heap;
    int size;
    int capacity;
    int k;
} KthLargest;
```

### 關鍵操作

**構造函數**：O(N log k)
- 遍歷初始數組
- 當 size < k 時插入元素
- 當 size == k 且 num > heap[0] 時，替換根並向下堆化
- 結果：堆包含恰好 k 個最大的元素

**add**：O(log k)
- 如果 size < k：插入並向上堆化
- 如果 num > heap[0]：替換根並向下堆化
- 否則：忽略（不在前 k 中）
- 返回 heap[0]（第 k 大元素）

**heapifyUp**：向上冒泡新插入的元素
**heapifyDown**：向下冒泡替換的根以恢復堆性質

### 為什麼有效
- 最小堆性質確保父節點 ≤ 子節點
- 根始終是 k 個最大元素中的最小值
- 維護 k 個元素意味著根 = 全局第 k 大
- 新元素只在大於當前「第 k 大」時才重要

---

## 面試討論要點

1. **為什麼用最小堆而不是最大堆？**
   - 最大堆的根 = 最大元素，不是第 k 大
   - 需要跟蹤所有元素，破壞空間優化

2. **為什麼只維護 k 個元素？**
   - 小於第 k 大元素的元素永遠不會成為第 k 大
   - 節省空間（O(k) vs O(N)）和時間（O(log k) vs O(N)）

3. **處理的邊界情況**：
   - 初始數組小於 k
   - 所有元素都小於第一個元素
   - 重複元素
   - 單元素流
