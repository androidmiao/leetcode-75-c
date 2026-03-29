# 219. Contains Duplicate II - Editorial

## Summary

This article is for beginners. It introduces the following ideas: Linear Search, Binary Search Tree and Hash Table.

## Solution

### Approach #1: Naive Linear Search

**Intuition:** Look for duplicate element in the previous k elements.

**Algorithm:** This algorithm keeps a virtual sliding window of the previous k elements. We scan for the duplicate in this window. Same as the brute force approach from Contains Duplicate (Problem 217), except that it looks at previous k elements instead of all its previous elements.

**Steps:**
1. For each index i from 0 to n-1:
   - For each index j from max(0, i-k) to i-1:
     - If nums[i] == nums[j], return true
2. Return false

**Complexity Analysis:**
- Time complexity: O(n * min(k, n)). It costs O(min(k, n)) time for each linear search. Apparently we do at most n comparisons in one search even if k can be larger than n.
- Space complexity: O(1).

**Note:** Time Limit Exceeded for large inputs.

---

### Approach #2: Binary Search Tree

**Intuition:** Keep a sliding window of k elements using self-balancing Binary Search Tree (BST).

**Algorithm:** The key to improve upon Approach #1 is to reduce the search time of the previous k elements. Can we use an auxiliary data structure to maintain a sliding window of k elements with more efficient search, delete, and insert operations? Since elements in the sliding window are strictly First-In-First-Out (FIFO), queue is a natural data structure. A queue using a linked list implementation supports constant time delete and insert operations, however the search costs linear time, which is no better than Approach #1.

A better option is to use a self-balancing BST. A BST supports search, delete and insert operations all in O(log k) time, where k is the number of elements in the BST. In most interviews you are not required to implement a self-balancing BST, so you may think of it as a black box. Most programming languages provide implementations (Java: TreeSet/TreeMap; C++ STL: std::set/std::map).

**Steps:**
1. Loop through the array, for each element:
   - Search current element in the BST, return true if found
   - Put current element in the BST
   - If the size of the BST is larger than k, remove the oldest item
2. Return false

**Complexity Analysis:**
- Time complexity: O(n * log(min(k, n))). We do n operations of search, delete and insert. Each operation costs logarithmic time complexity in the sliding window which size is min(k, n). Note that even if k can be greater than n, the window size can never exceed n.
- Space complexity: O(min(n, k)). Space is the size of the sliding window which should not exceed n or k.

**Note:** The algorithm still gets Time Limit Exceeded for large n and k.

---

### Approach #3: Hash Table (Recommended)

**Intuition:** Keep a sliding window of k elements using Hash Table.

**Algorithm:** From the previous approaches, we know that even logarithmic performance in search is not enough. In this case, we need a data structure supporting constant time search, delete and insert operations. Hash Table is the answer. The algorithm and implementation are almost identical to Approach #2.

**Steps:**
1. Loop through the array, for each element:
   - Search current element in the HashTable, return true if found
   - Put current element in the HashTable
   - If the size of the HashTable is larger than k, remove the oldest item
2. Return false

**Complexity Analysis:**
- Time complexity: O(n). We do n operations of search, delete and insert, each with constant time complexity.
- Space complexity: O(min(n, k)). The extra space required depends on the number of items stored in the hash table, which is the size of the sliding window, min(n, k).

## Comparison

| Approach | Time | Space | Status |
|----------|------|-------|--------|
| #1 Naive Linear Search | O(n * min(k, n)) | O(1) | TLE |
| #2 Binary Search Tree | O(n * log(min(k, n))) | O(min(n, k)) | TLE |
| #3 Hash Table | O(n) | O(min(n, k)) | Accepted |

## See Also

- Problem 217 Contains Duplicate
- Problem 220 Contains Duplicate III

---

# 219. 存在重複元素 II - 社論（繁體中文）

## 摘要

本文適合初學者，介紹以下概念：線性搜尋、二元搜尋樹與雜湊表。

## 解法

### 方法 #1：暴力線性搜尋

**直覺：** 在前 k 個元素中尋找重複元素。

**演算法：** 此演算法維護一個包含前 k 個元素的虛擬滑動視窗，在此視窗內掃描是否有重複。與 Contains Duplicate（第 217 題）的暴力法相同，差別在於只看前 k 個元素而非所有先前元素。

**步驟：**
1. 對每個索引 i（從 0 到 n-1）：
   - 對每個索引 j（從 max(0, i-k) 到 i-1）：
     - 若 nums[i] == nums[j]，回傳 true
2. 回傳 false

**複雜度分析：**
- 時間複雜度：O(n * min(k, n))。每次線性搜尋花費 O(min(k, n)) 時間。即使 k 大於 n，一次搜尋最多也只做 n 次比較。
- 空間複雜度：O(1)。

**注意：** 大型輸入會超時（TLE）。

---

### 方法 #2：二元搜尋樹

**直覺：** 使用自平衡二元搜尋樹（BST）維護大小為 k 的滑動視窗。

**演算法：** 改進方法 #1 的關鍵在於降低前 k 個元素的搜尋時間。是否能用輔助資料結構維護一個大小為 k 的滑動視窗，並提供更高效的搜尋、刪除、插入操作？

由於視窗中的元素嚴格按照先進先出（FIFO）順序，佇列是自然的選擇。但鏈結串列實作的佇列雖然支援 O(1) 的刪除和插入，搜尋仍然需要線性時間，與方法 #1 相比沒有改善。

更好的選擇是使用自平衡 BST。BST 的搜尋、刪除和插入操作都是 O(log k) 時間。面試中通常不需要自己實作自平衡 BST，可將其視為黑盒子。大多數程式語言都有內建實作（Java：TreeSet/TreeMap；C++ STL：std::set/std::map）。

**步驟：**
1. 遍歷陣列，對每個元素：
   - 在 BST 中搜尋當前元素，若找到則回傳 true
   - 將當前元素放入 BST
   - 若 BST 大小超過 k，移除最舊的元素
2. 回傳 false

**複雜度分析：**
- 時間複雜度：O(n * log(min(k, n)))。我們做 n 次搜尋、刪除和插入操作，每次操作在大小為 min(k, n) 的滑動視窗中花費對數時間。即使 k 大於 n，視窗大小也不會超過 n。
- 空間複雜度：O(min(n, k))。空間即滑動視窗的大小，不會超過 n 或 k。

**注意：** 此演算法在大的 n 和 k 下仍然會超時（TLE）。

---

### 方法 #3：雜湊表（推薦）

**直覺：** 使用雜湊表維護大小為 k 的滑動視窗。

**演算法：** 從前兩種方法得知，即使是對數級的搜尋效能也不夠。我們需要一個支援常數時間搜尋、刪除和插入的資料結構——雜湊表就是答案。演算法和實作與方法 #2 幾乎相同。

**步驟：**
1. 遍歷陣列，對每個元素：
   - 在雜湊表中搜尋當前元素，若找到則回傳 true
   - 將當前元素放入雜湊表
   - 若雜湊表大小超過 k，移除最舊的元素
2. 回傳 false

**複雜度分析：**
- 時間複雜度：O(n)。我們做 n 次搜尋、刪除和插入操作，每次操作都是常數時間。
- 空間複雜度：O(min(n, k))。額外空間取決於雜湊表中儲存的元素數量，即滑動視窗的大小 min(n, k)。

## 比較

| 方法 | 時間 | 空間 | 狀態 |
|------|------|------|------|
| #1 暴力線性搜尋 | O(n * min(k, n)) | O(1) | 超時 |
| #2 二元搜尋樹 | O(n * log(min(k, n))) | O(min(n, k)) | 超時 |
| #3 雜湊表 | O(n) | O(min(n, k)) | 通過 |

## 延伸閱讀

- 第 217 題 Contains Duplicate
- 第 220 題 Contains Duplicate III
