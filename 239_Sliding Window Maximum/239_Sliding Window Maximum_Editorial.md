# 239. Sliding Window Maximum - Editorial

## Approach: Monotonic Deque

### Intuition

A brute force solution would examine each of the n-k+1 sliding windows individually, scanning all k elements per window to find the maximum. This results in O(n*k) time complexity, which is too slow for the problem constraints.

The key insight is recognizing which elements retain value as the window advances. Consider a window like [1, 2, 3, 4, 1]. Any element that appears before the maximum (here, 4) becomes permanently useless—since the window only moves rightward, any future window containing those elements will also contain 4, making them incapable of becoming the window maximum.

Conversely, elements positioned after the current maximum may become relevant. Once the current maximum slides out of the window, these later elements could potentially become the new maximum.

This observation suggests we should maintain only the "potentially useful" elements in a decreasing sequence. When we encounter a new element x, we can discard all smaller elements from our tracked set, since x will shadow them in all future windows containing both. By organizing these useful elements in strictly decreasing order, we create a monotonic structure that efficiently answers range maximum queries.

A deque (double-ended queue) is the ideal data structure for this approach. We need to support both efficient removal of old elements (from the front, when they exit the window) and efficient removal of dominated elements (from the back, when we add a larger element). To distinguish whether an element is still in the current window, we store indices rather than values.

### Algorithm

1. Initialize an empty deque `dq` to store indices.
2. Initialize an empty list `res` to accumulate the results.
3. **Process the initial window** (indices 0 to k-1):
   - For each index i from 0 to k-1:
     - While the deque is not empty and the element at `dq.back()` is less than or equal to `nums[i]`, remove it from the back.
     - Add index i to the back of the deque.
   - Append `nums[dq.front()]` to `res` (this is the maximum of the first window).
4. **Slide the window** through the remaining elements (indices k to n-1):
   - For each index i from k to n-1:
     - If the front of the deque points to an index that has exited the window (i.e., `dq.front() == i - k`), remove it.
     - While the deque is not empty and the element at `dq.back()` is less than or equal to `nums[i]`, remove it from the back.
     - Add index i to the back of the deque.
     - Append `nums[dq.front()]` to `res` (maximum of the current window).
5. Return `res`.

### Complexity Analysis

**Time Complexity: O(n)**

Each element is added to the deque exactly once and removed at most once. This gives a total of 2n operations across the entire algorithm, resulting in linear time complexity.

**Space Complexity: O(k)**

The deque stores at most k indices at any given moment, corresponding to the window size. In the worst case (strictly increasing arrays), the deque could hold all k elements of the current window.

---

# 239.滑動視窗最大值 - 編輯解析

## 方法：單調雙端隊列

### 直觀解釋

蠻力解法逐一檢查 n-k+1 個滑動視窗，對每個視窗掃描 k 個元素以找出最大值。這導致 O(n*k) 的時間複雜度，對於題目的限制條件而言過於緩慢。

關鍵洞察在於識別哪些元素會在視窗前進時保持其價值。考慮一個視窗 [1, 2, 3, 4, 1]。任何出現在最大值（此處為 4）之前的元素都永久失效——由於視窗只能向右移動，任何包含那些元素的未來視窗也會包含 4，使得這些元素不可能成為視窗最大值。

相反地，位於當前最大值之後的元素可能變得相關。一旦當前最大值滑出視窗，這些後來的元素可能成為新的最大值。

這個觀察建議我們應該只維護「潛在有用」的元素，並將其組織成遞減序列。當我們遇到新元素 x 時，可以丟棄所有小於 x 的被追蹤元素，因為 x 會在所有包含兩者的未來視窗中遮蔽它們。透過將這些有用元素嚴格按遞減順序組織，我們建立了一個單調結構，能夠有效地回答範圍最大值查詢。

雙端隊列（deque）是此方法的理想資料結構。我們需要支持高效移除舊元素（從前端，當它們離開視窗時）和高效移除被支配元素（從後端，當我們加入更大元素時）。為了區分一個元素是否仍在當前視窗內，我們存儲索引而非值。

### 演算法

1. 初始化空的雙端隊列 `dq` 以儲存索引。
2. 初始化空的列表 `res` 以累積結果。
3. **處理初始視窗**（索引 0 到 k-1）：
   - 對於每個索引 i 從 0 到 k-1：
     - 當雙端隊列非空且 `dq.back()` 處的元素小於或等於 `nums[i]` 時，將其從後端移除。
     - 將索引 i 加到雙端隊列的後端。
   - 將 `nums[dq.front()]` 附加到 `res`（這是第一個視窗的最大值）。
4. **滑動視窗**通過剩餘元素（索引 k 到 n-1）：
   - 對於每個索引 i 從 k 到 n-1：
     - 如果雙端隊列的前端指向已離開視窗的索引（即 `dq.front() == i - k`），將其移除。
     - 當雙端隊列非空且 `dq.back()` 處的元素小於或等於 `nums[i]` 時，將其從後端移除。
     - 將索引 i 加到雙端隊列的後端。
     - 將 `nums[dq.front()]` 附加到 `res`（當前視窗的最大值）。
5. 返回 `res`。

### 複雜度分析

**時間複雜度：O(n)**

每個元素恰好被加到雙端隊列一次，最多被移除一次。這在整個演算法中產生了 2n 次操作，結果為線性時間複雜度。

**空間複雜度：O(k)**

雙端隊列在任何給定時刻最多儲存 k 個索引，對應於視窗大小。在最壞情況下（嚴格遞增陣列），雙端隊列可能容納當前視窗的所有 k 個元素。
