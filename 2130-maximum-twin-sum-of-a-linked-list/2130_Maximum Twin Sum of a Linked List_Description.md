# LeetCode 2130 - Maximum Twin Sum of a Linked List

## English

**Problem Number:** 2130
**Title:** Maximum Twin Sum of a Linked List
**Difficulty:** Medium
**Link:** https://leetcode.com/problems/maximum-twin-sum-of-a-linked-list/

### Problem Statement

Consider a linked list of size `n` where `n` is an even positive integer. Within this list, each node at position `i` (using 0-based indexing) forms a twin pair with the node at position `n-1-i`, for all valid indices where `0 <= i <= (n/2) - 1`. The twin sum is defined as the sum of values of a node and its corresponding twin node.

Your task is to find and return the maximum twin sum among all twin pairs in the linked list.

### Examples

**Example 1:**
```
Input: head = [5,4,2,1]
Output: 6
Explanation: The linked list has 4 nodes. Node at index 0 (value 5) is paired with node at index 3 (value 1), giving a sum of 6. Node at index 1 (value 4) is paired with node at index 2 (value 2), giving a sum of 6. All twin sums are equal to 6, so the maximum is 6.
```

**Example 2:**
```
Input: head = [4,2,2,3]
Output: 7
Explanation: The linked list has 4 nodes. Node at index 0 (value 4) pairs with node at index 3 (value 3) for a sum of 7. Node at index 1 (value 2) pairs with node at index 2 (value 2) for a sum of 4. The maximum twin sum is max(7, 4) = 7.
```

**Example 3:**
```
Input: head = [1,100000]
Output: 100001
Explanation: The linked list contains only one twin pair consisting of nodes with values 1 and 100000. Their sum is 1 + 100000 = 100001, which is the maximum (and only) twin sum.
```

### Constraints

- The number of nodes in the linked list is an even integer.
- The number of nodes is within the range `[2, 10^5]`.
- Each node's value is an integer within the range `[1, 10^5]`.

### Topics

- Linked List
- Two Pointers
- Stack

### Acceptance Rate

Approximately 81.7%

---

## 繁體中文

**題目編號:** 2130
**標題:** 鏈表中的最大孿生和
**難度:** 中等
**連結:** https://leetcode.com/problems/maximum-twin-sum-of-a-linked-list/

### 問題敘述

給定一個大小為 `n` 的鏈表，其中 `n` 是偶數。在此鏈表中，位置 `i` 的節點（使用 0 為基準的索引方式）與位置 `n-1-i` 的節點形成一對孿生節點配對，對於所有滿足 `0 <= i <= (n/2) - 1` 的有效索引都是如此。孿生和定義為一個節點及其對應孿生節點的數值之和。

你的任務是計算並返回鏈表中所有孿生節點配對中最大的孿生和。

### 示例

**示例 1:**
```
輸入: head = [5,4,2,1]
輸出: 6
說明: 鏈表包含 4 個節點。索引 0 位置的節點（數值 5）與索引 3 位置的節點（數值 1）配對，和為 6。索引 1 位置的節點（數值 4）與索引 2 位置的節點（數值 2）配對，和為 6。所有孿生和都等於 6，因此最大值為 6。
```

**示例 2:**
```
輸入: head = [4,2,2,3]
輸出: 7
說明: 鏈表包含 4 個節點。索引 0 位置的節點（數值 4）與索引 3 位置的節點（數值 3）配對，和為 7。索引 1 位置的節點（數值 2）與索引 2 位置的節點（數值 2）配對，和為 4。最大孿生和為 max(7, 4) = 7。
```

**示例 3:**
```
輸入: head = [1,100000]
輸出: 100001
說明: 鏈表中只有一對孿生節點，分別包含數值 1 和 100000。它們的和為 1 + 100000 = 100001，這是最大的（也是唯一的）孿生和。
```

### 限制條件

- 鏈表中的節點數量是偶數。
- 節點的數量在範圍 `[2, 10^5]` 內。
- 每個節點的數值是範圍 `[1, 10^5]` 內的整數。

### 主題標籤

- 鏈表
- 雙指針
- 堆棧

### 通過率

約為 81.7%
