# 23. Merge k Sorted Lists - Editorial

## Approach 1: Brute Force

### Intuition & Algorithm

Traverse all the linked lists and collect the values of the nodes into an array. Sort the array and iterate over it to create a new sorted linked list.

### Steps

1. Traverse every linked list, collecting each node's value into a single array.
2. Sort the array.
3. Create a new linked list from the sorted values.

### Complexity Analysis

- **Time complexity:** O(N log N) where N is the total number of nodes.
  - Collecting all values costs O(N).
  - A stable sorting algorithm costs O(N log N).
  - Creating the linked list costs O(N).
- **Space complexity:** O(N).
  - Sorting costs O(N) space (for the values array).
  - Creating a new linked list costs O(N) space.

---

## Approach 2: Compare One by One

### Algorithm

Compare every k nodes (head of every linked list) and get the node with the smallest value. Extend the final sorted linked list with the selected nodes.

### Complexity Analysis

- **Time complexity:** O(kN) where k is the number of linked lists.
  - Almost every selection of a node in the final linked list costs O(k) (k-1 comparisons).
  - There are N nodes in the final linked list.
- **Space complexity:**
  - O(n) — creating a new linked list costs O(n) space.
  - O(1) — it's not hard to apply an in-place method: connect selected nodes instead of creating new nodes to fill the new linked list.

---

## Approach 3: Optimize Approach 2 by Priority Queue

### Algorithm

Almost the same as Approach 2, but optimize the **comparison process** by using a **priority queue** (min-heap). Instead of comparing all k heads linearly, use a heap to find the minimum in O(log k) time.

### Steps

1. Push the head of each non-empty list into the priority queue.
2. Pop the smallest node from the queue and append it to the result list.
3. If the popped node has a next node, push that next node into the queue.
4. Repeat until the queue is empty.

### Complexity Analysis

- **Time complexity:** O(N log k) where k is the number of linked lists.
  - The comparison cost is reduced to O(log k) for every pop and insertion to the priority queue.
  - Finding the node with the smallest value just costs O(1) time.
  - There are N nodes in the final linked list.
- **Space complexity:**
  - O(n) — creating a new linked list costs O(n) space.
  - O(k) — the code above applies in-place method which costs O(1) space. And the priority queue (often implemented with heaps) costs O(k) space (it's far less than N in most situations).

---

## Approach 4: Merge Lists One by One

### Algorithm

Convert the merge k lists problem to merge 2 lists (k-1) times. Start with the first list, then merge the result with the next list, and so on.

### Complexity Analysis

- **Time complexity:** O(kN) where k is the number of linked lists.
  - We can merge two sorted linked lists in O(n) time where n is the total number of nodes in the two lists.
  - Sum up the merge process: O(Σᵢ₌₁ᵏ⁻¹ (i × (N/k) + N/k)) = O(kN).
- **Space complexity:** O(1).
  - We can merge two sorted linked lists in O(1) space.

---

## Approach 5: Merge with Divide and Conquer (Recommended)

### Intuition & Algorithm

This approach walks alongside Approach 4 but is improved a lot. We don't need to traverse most nodes many times repeatedly.

- Pair up k lists and merge each pair.
- After the first pairing, k lists are merged into k/2 lists with average 2N/k length, then k/4, k/8, and so on.
- Repeat this procedure until we get the final sorted linked list.

This optimization is achieved because instead of merging one list at a time (leading to repeatedly traversing the same nodes), we merge in a balanced binary tree fashion, ensuring each node is only involved in O(log k) merge operations.

### Complexity Analysis

- **Time complexity:** O(N log k) where k is the number of linked lists.
  - We can merge two sorted linked lists in O(n) time where n is the total number of nodes in the two lists.
  - Sum up the merge process: O(Σᵢ₌₁ˡᵒᵍᵏ N) = O(N log k).
- **Space complexity:** O(1).
  - We can merge two sorted linked lists in O(1) space.

---

## Summary Comparison

| Approach | Time | Space | Notes |
|----------|------|-------|-------|
| 1. Brute Force | O(N log N) | O(N) | Simple but not optimal |
| 2. Compare One by One | O(kN) | O(n) / O(1) | Linear scan of k heads each time |
| 3. Priority Queue | O(N log k) | O(k) | Heap optimizes comparison to O(log k) |
| 4. Merge One by One | O(kN) | O(1) | Sequential merge, repeated traversal |
| 5. Divide and Conquer | O(N log k) | O(1) | Balanced merge, optimal time + space |

Approaches 3 and 5 are both optimal in time at O(N log k). Approach 5 is preferred for its O(1) extra space (versus O(k) for the heap), making it the best overall choice.

---

# 23. 合併 K 個升序鏈結串列 - 題解

## 方法 1：暴力法

### 思路與演算法

遍歷所有鏈結串列，將所有節點的值收集到一個陣列中。對陣列排序後，遍歷陣列建立一個新的有序鏈結串列。

### 步驟

1. 遍歷每個鏈結串列，將每個節點的值收集到一個陣列中。
2. 對陣列進行排序。
3. 根據排序後的值建立新的鏈結串列。

### 複雜度分析

- **時間複雜度：** O(N log N)，其中 N 是所有節點的總數。
  - 收集所有值花費 O(N)。
  - 排序花費 O(N log N)。
  - 建立鏈結串列花費 O(N)。
- **空間複雜度：** O(N)。

---

## 方法 2：逐一比較

### 演算法

比較 k 個節點（每個鏈結串列的頭節點），取得值最小的節點。將選中的節點新增到最終的有序鏈結串列中。

### 複雜度分析

- **時間複雜度：** O(kN)，其中 k 是鏈結串列的數量。
  - 最終鏈結串列中幾乎每個節點的選取都需要 O(k) 次比較。
  - 最終鏈結串列中有 N 個節點。
- **空間複雜度：** O(n) 或 O(1)（原地方法）。

---

## 方法 3：用優先佇列優化方法 2

### 演算法

與方法 2 幾乎相同，但使用**優先佇列**（最小堆）來優化**比較過程**。不再線性掃描 k 個頭節點，而是用堆在 O(log k) 時間內找到最小值。

### 步驟

1. 將每個非空鏈結串列的頭節點放入優先佇列。
2. 彈出佇列中最小的節點，追加到結果鏈結串列。
3. 如果彈出的節點有下一個節點，將下一個節點壓入佇列。
4. 重複直到佇列為空。

### 複雜度分析

- **時間複雜度：** O(N log k)，其中 k 是鏈結串列的數量。
  - 每次彈出和插入優先佇列的比較成本降低到 O(log k)。
  - 取得最小值節點只需 O(1) 時間。
- **空間複雜度：** O(k)（堆的空間）。

---

## 方法 4：逐一合併

### 演算法

將合併 k 個鏈結串列的問題轉化為 k-1 次合併兩個鏈結串列。從第一個鏈結串列開始，依次與下一個鏈結串列合併。

### 複雜度分析

- **時間複雜度：** O(kN)，其中 k 是鏈結串列的數量。
  - 合併兩個有序鏈結串列需要 O(n) 時間。
  - 累加整個合併過程：O(Σᵢ₌₁ᵏ⁻¹ (i × (N/k) + N/k)) = O(kN)。
- **空間複雜度：** O(1)。

---

## 方法 5：分治合併（推薦）

### 思路與演算法

這個方法與方法 4 類似，但有很大改進。我們不需要多次重複遍歷大部分節點。

- 將 k 個鏈結串列配對，合併每一對。
- 第一輪配對後，k 個鏈結串列合併為 k/2 個鏈結串列，平均長度 2N/k，然後 k/4、k/8，依此類推。
- 重複此過程直到得到最終的有序鏈結串列。

這種優化的原因在於：與逐一合併（導致重複遍歷相同節點）不同，分治法以平衡二元樹的方式合併，確保每個節點只參與 O(log k) 次合併操作。

### 複雜度分析

- **時間複雜度：** O(N log k)，其中 k 是鏈結串列的數量。
  - 合併兩個有序鏈結串列需要 O(n) 時間。
  - 累加合併過程：O(Σᵢ₌₁ˡᵒᵍᵏ N) = O(N log k)。
- **空間複雜度：** O(1)。

---

## 總結對比

| 方法 | 時間 | 空間 | 說明 |
|------|------|------|------|
| 1. 暴力法 | O(N log N) | O(N) | 簡單但不最優 |
| 2. 逐一比較 | O(kN) | O(n) / O(1) | 每次線性掃描 k 個頭節點 |
| 3. 優先佇列 | O(N log k) | O(k) | 堆優化比較為 O(log k) |
| 4. 逐一合併 | O(kN) | O(1) | 順序合併，重複遍歷 |
| 5. 分治合併 | O(N log k) | O(1) | 平衡合併，時間空間最優 |

方法 3 和方法 5 的時間複雜度都是最優的 O(N log k)。方法 5 因為只需 O(1) 額外空間（相比堆的 O(k)），是整體最佳選擇。
