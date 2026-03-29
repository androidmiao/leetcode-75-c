# 295. Find Median from Data Stream - Editorial

## Solution

### Approach 1: Simple Sorting

**Intuition:** Store all numbers in a resizable container. Each time the median is needed, sort the entire container and pick the middle element(s).

**Algorithm:**
1. Maintain a dynamic array (vector) to store incoming numbers.
2. On `addNum`, simply append the number to the array.
3. On `findMedian`, sort the array, then return the middle element (odd length) or the average of the two middle elements (even length).

**Complexity Analysis:**
- Time complexity: O(n log n) per `findMedian` call due to sorting. `addNum` is amortized O(1).
- Space complexity: O(n) to store all elements.

---

### Approach 2: Insertion Sort

**Intuition:** Keep the container sorted at all times as an invariant. When a new number arrives, insert it into the correct position using binary search, then shift elements to make room.

**Algorithm:**
1. Maintain a sorted dynamic array.
2. On `addNum`, use binary search (e.g., `lower_bound`) to find the correct insertion position, then insert the number there. The binary search is O(log n), but the insertion (shifting) is O(n).
3. On `findMedian`, directly access the middle element(s) in O(1).

**Complexity Analysis:**
- Time complexity: O(n) per `addNum` (O(log n) for binary search + O(n) for shifting). `findMedian` is O(1).
- Space complexity: O(n) to store all elements.

**Pop quiz:** Can we use a linear search instead of a binary search to find the insertion position, without incurring any significant runtime penalty? Yes — since the shifting already costs O(n), the overall complexity remains O(n).

---

### Approach 3: Two Heaps (Recommended)

**Intuition:** The previous approaches reveal two key insights: (1) if we could maintain direct access to median elements, finding the median would be O(1); (2) we only need consistent access to the median elements — keeping the entire input sorted is not required.

Two data structures fit the job: heaps (priority queues) and self-balancing BSTs. Heaps provide O(log n) insertion and O(1) access to the max/min element.

**Algorithm:**

Maintain two priority queues:
1. A **max-heap** `lo` to store the smaller half of the numbers.
2. A **min-heap** `hi` to store the larger half of the numbers.

The max-heap `lo` is allowed to store at most one more element than the min-heap `hi`. This means:
- If k = 2n+1 (odd total), `lo` holds n+1 elements, `hi` holds n elements. The median is the top of `lo`.
- If k = 2n (even total), both heaps hold n elements. The median is the average of the tops of `lo` and `hi`.

**Adding a number `num`:**
1. Add `num` to max-heap `lo`. (This ensures `lo` receives the element first.)
2. Since `lo` received a new element, balance by moving the largest element from `lo` to `hi`. (This maintains the property that every element in `lo` ≤ every element in `hi`.)
3. If `hi` has more elements than `lo`, move the smallest element from `hi` back to `lo`. (This maintains the size invariant.)

**Finding the median:**
- If `lo` has more elements than `hi`, return the top of `lo`.
- Otherwise, return the average of the tops of `lo` and `hi`.

**Complexity Analysis:**
- Time complexity: O(log n) per `addNum` (at most 3 heap operations). O(1) per `findMedian`.
- Space complexity: O(n) to store all elements across both heaps.

---

### Approach 4: Multiset and Two Pointers

**Intuition:** Use a self-balancing BST (like `std::multiset` in C++) to store all elements in sorted order. Maintain two pointers/iterators that always point to the median element(s).

**Algorithm:**
1. Maintain a multiset (balanced BST) containing all elements.
2. Keep two iterators `lo_median` and `hi_median` pointing to the one or two median elements.
3. On `addNum`, insert the number and adjust the iterators based on the new element's position relative to the current medians and the parity of the total count.
4. On `findMedian`, dereference the iterators and return the average.

**Complexity Analysis:**
- Time complexity: O(log n) per `addNum` (BST insertion). O(1) per `findMedian`.
- Space complexity: O(n).

**Note:** This approach is harder to implement correctly in C (no built-in balanced BST), making Approach 3 (Two Heaps) the preferred interview solution.

---

### Follow-Up Answers

**If all integers are in [0, 100]:**
Use a counting array of size 101. `addNum` increments a counter in O(1). `findMedian` scans the counting array to find the median position in O(101) = O(1) constant time.

**If 99% of integers are in [0, 100]:**
Use the same counting array for [0, 100], plus two counters tracking how many numbers fall below 0 and above 100. The median is very likely within [0, 100] and can be found by scanning the counting array, adjusted by the below/above counters.

---

# 295. 求資料流的中位數 - 題解

## 解法

### 方法一：簡單排序

**直覺：** 將所有數字存入可調整大小的容器。每次需要中位數時，對整個容器排序，然後取中間元素。

**演算法：**
1. 維護一個動態陣列來儲存收到的數字。
2. `addNum` 時，直接將數字附加到陣列末尾。
3. `findMedian` 時，排序陣列，然後回傳中間元素（奇數長度）或兩個中間元素的平均值（偶數長度）。

**複雜度分析：**
- 時間複雜度：每次 `findMedian` 為 O(n log n)（排序）。`addNum` 為攤銷 O(1)。
- 空間複雜度：O(n)，儲存所有元素。

---

### 方法二：插入排序

**直覺：** 始終保持容器有序作為不變式。當新數字到達時，使用二分搜尋找到正確的插入位置，然後移動元素騰出空間。

**演算法：**
1. 維護一個有序動態陣列。
2. `addNum` 時，使用二分搜尋找到插入位置，然後插入。二分搜尋為 O(log n)，但插入（移動元素）為 O(n)。
3. `findMedian` 時，直接存取中間元素，O(1)。

**複雜度分析：**
- 時間複雜度：每次 `addNum` 為 O(n)（O(log n) 搜尋 + O(n) 移動）。`findMedian` 為 O(1)。
- 空間複雜度：O(n)。

---

### 方法三：兩個堆（推薦）

**直覺：** 前兩種方法揭示了兩個關鍵洞察：(1) 如果我們能直接存取中位數元素，查找中位數就是 O(1)；(2) 我們只需要一致地存取中位數元素——不需要保持整個輸入有序。

堆（優先佇列）提供 O(log n) 的插入和 O(1) 的最大/最小元素存取。

**演算法：**

維護兩個優先佇列：
1. **最大堆** `lo`：儲存較小的那一半數字。
2. **最小堆** `hi`：儲存較大的那一半數字。

最大堆 `lo` 最多比最小堆 `hi` 多存一個元素：
- 若總數 k = 2n+1（奇數），`lo` 有 n+1 個元素，`hi` 有 n 個元素。中位數是 `lo` 的堆頂。
- 若總數 k = 2n（偶數），兩個堆各有 n 個元素。中位數是兩個堆頂的平均值。

**新增數字 `num`：**
1. 將 `num` 加入最大堆 `lo`。
2. 將 `lo` 的最大元素移到 `hi`（確保 `lo` 中所有元素 ≤ `hi` 中所有元素）。
3. 若 `hi` 的元素比 `lo` 多，將 `hi` 的最小元素移回 `lo`（維護大小不變式）。

**查找中位數：**
- 若 `lo` 的元素比 `hi` 多，回傳 `lo` 的堆頂。
- 否則，回傳兩個堆頂的平均值。

**複雜度分析：**
- 時間複雜度：每次 `addNum` 為 O(log n)（最多 3 次堆操作）。`findMedian` 為 O(1)。
- 空間複雜度：O(n)，跨兩個堆儲存所有元素。

---

### 方法四：多重集合與雙指標

**直覺：** 使用自平衡二元搜尋樹（如 C++ 的 `std::multiset`）儲存所有元素。維護兩個迭代器始終指向中位數元素。

**演算法：**
1. 維護一個多重集合（平衡 BST）包含所有元素。
2. 保持兩個迭代器 `lo_median` 和 `hi_median` 指向一個或兩個中位數元素。
3. `addNum` 時，插入數字並根據新元素相對於當前中位數的位置及總數的奇偶性調整迭代器。
4. `findMedian` 時，解引用迭代器並回傳平均值。

**複雜度分析：**
- 時間複雜度：每次 `addNum` 為 O(log n)（BST 插入）。`findMedian` 為 O(1)。
- 空間複雜度：O(n)。

**注意：** 此方法在 C 中實作較困難（沒有內建的平衡 BST），因此方法三（兩個堆）是面試中首選的解法。

---

### 進階問題解答

**若所有整數都在 [0, 100] 範圍內：**
使用大小為 101 的計數陣列。`addNum` 遞增計數器為 O(1)。`findMedian` 掃描計數陣列找到中位數位置為 O(101) = O(1) 常數時間。

**若 99% 的整數在 [0, 100] 範圍內：**
對 [0, 100] 使用相同的計數陣列，另外用兩個計數器追蹤小於 0 和大於 100 的數字數量。中位數極大機率落在 [0, 100] 範圍內，可透過計數陣列快速找到，再依據低於/高於計數器做調整。
