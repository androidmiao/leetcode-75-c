# 703. Kth Largest Element in a Stream - Editorial

## Overview
The problem requires designing a class that can efficiently find the kth largest element in a stream of numbers. As new elements are added to the stream, the class must maintain the ability to quickly return the current kth largest value.

---

## Approach 1: Maintain Sorted List

### Intuition
The straightforward approach is to maintain a sorted list of all numbers encountered in the stream. When a new element arrives, we insert it at the correct position in sorted order using binary search. The kth largest element is simply the element at position `size - k` from the beginning (or position `k-1` from the end in a zero-indexed array).

### Algorithm
1. **Constructor**: Initialize the value of `k`, initialize an empty list `stream`, add all initial numbers to the stream, and sort the list in ascending order.
2. **add(val)**: Use binary search via a helper function `getIndex(val)` to find the correct insertion position. Insert `val` at that position, and return `stream[stream.size() - k]`, which is the kth largest element.
3. **getIndex(val)**: Implement binary search with pointers `left = 0` and `right = stream.size() - 1`. While `left <= right`, check the middle element:
   - If `stream[mid] == val`, return `mid`
   - If `stream[mid] < val`, move `left` to `mid + 1`
   - If `stream[mid] > val`, move `right` to `mid - 1`
   - Return `left` as the insertion position

### Complexity Analysis
- **Time Complexity**: O(N² + N*M) for total operations
  - Constructor: O(M log M) where M is the size of initial array
  - Each add operation: O(M + N) due to binary search O(log M) and insertion shifting O(M)
- **Space Complexity**: O(M + N) to store all elements

---

## Approach 2: Heap (Recommended)

### Intuition
Since we only care about the kth largest element, we don't need to maintain a sorted list of all elements. Instead, we can use a min-heap of size k. The key insight is that the smallest element in a heap of the k largest elements is always the kth largest element overall.

When a new element arrives:
- If it is smaller than or equal to the current kth largest (the root of the min-heap), we can ignore it
- If it is larger than the kth largest or the heap has fewer than k elements, we add it to the heap
- If the heap size exceeds k, we remove the minimum element to maintain the invariant

### Algorithm
1. **Constructor**: Initialize `k`, create a min-heap (using a PriorityQueue or similar data structure), and iterate through the initial numbers array, calling `add(num)` for each element.
2. **add(val)**:
   - Check if `val` is greater than the top of the min-heap OR if the heap size is less than `k`
   - If either condition is true, add `val` to the min-heap
   - If the heap size exceeds `k`, remove the minimum element (the root)
   - Return the top element of the min-heap, which is the current kth largest element

### Complexity Analysis
- **Time Complexity**: O((M + N) * log k)
  - Each add or remove operation on the heap is O(log k)
  - Total of M+N operations across constructor and subsequent add calls
- **Space Complexity**: O(k) to store at most k elements in the heap

---

# 703. 數據流中第K大的元素 - 編輯社論

## 概述
此問題要求設計一個類別，能夠高效地找到資料流中的第k大元素。當新元素被加入資料流時，該類別必須能夠快速返回當前的第k大值。

---

## 方法1：維護排序列表

### 直觀理解
最直接的方法是維護一個包含資料流中所有數字的排序列表。當新元素到達時，我們使用二元搜尋來找到正確的插入位置，並將其插入列表中。第k大元素就是從列表開頭數起第 `size - k` 個位置的元素（或從末尾數起第 `k-1` 個位置）。

### 演算法
1. **建構函式**：初始化 `k` 值，初始化一個空列表 `stream`，將所有初始數字加入列表，並將列表按升序排序。
2. **add(val)**：使用輔助函式 `getIndex(val)` 進行二元搜尋，找到正確的插入位置。在該位置插入 `val`，然後返回 `stream[stream.size() - k]`，即第k大元素。
3. **getIndex(val)**：實現二元搜尋，使用指標 `left = 0` 和 `right = stream.size() - 1`。當 `left <= right` 時，檢查中間元素：
   - 若 `stream[mid] == val`，返回 `mid`
   - 若 `stream[mid] < val`，將 `left` 移到 `mid + 1`
   - 若 `stream[mid] > val`，將 `right` 移到 `mid - 1`
   - 返回 `left` 作為插入位置

### 複雜度分析
- **時間複雜度**：總操作時間為 O(N² + N*M)
  - 建構函式：O(M log M)，其中M為初始陣列大小
  - 每個add操作：O(M + N)，包括二元搜尋 O(log M) 和插入移位 O(M)
- **空間複雜度**：O(M + N) 用於儲存所有元素

---

## 方法2：堆積（推薦）

### 直觀理解
由於我們只關心第k大元素，不需要維護所有元素的排序列表。我們可以改用大小為k的最小堆。關鍵洞察是，最小堆中k個最大元素中的最小元素（堆的根節點）始終是整體的第k大元素。

當新元素到達時：
- 如果它小於或等於當前第k大元素（最小堆的根），我們可以忽略它
- 如果它大於第k大元素或堆的大小小於k，我們將其加入堆
- 如果堆的大小超過k，我們移除最小元素以維持不變量

### 演算法
1. **建構函式**：初始化 `k`，建立一個最小堆（使用優先佇列或類似的資料結構），並遍歷初始數字陣列，對每個元素呼叫 `add(num)`。
2. **add(val)**：
   - 檢查 `val` 是否大於最小堆的頂部，或堆的大小是否小於 `k`
   - 若任一條件為真，將 `val` 加入最小堆
   - 若堆的大小超過 `k`，移除最小元素（根節點）
   - 返回最小堆的頂部元素，這是當前的第k大元素

### 複雜度分析
- **時間複雜度**：O((M + N) * log k)
  - 堆上的每個add或remove操作為 O(log k)
  - 建構函式和後續add呼叫的總操作數為M+N
- **空間複雜度**：O(k) 用於在堆中儲存最多k個元素
