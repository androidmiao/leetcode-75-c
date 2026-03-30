# 328. Odd Even Linked List

## English

### Problem Description

Given the `head` of a singly linked list, group all the nodes with odd indices together followed by the nodes with even indices, and return the reordered list.

The first node is considered odd, and the second node is even, and so on.

Note that the relative order inside both the even and odd groups should remain as it was in the input.

You must solve the problem in O(1) extra space complexity and O(n) time complexity.

### Examples

#### Example 1
- **Input:** head = [1,2,3,4,5]
- **Output:** [1,3,5,2,4]

#### Example 2
- **Input:** head = [2,1,3,5,6,4,7]
- **Output:** [2,3,6,7,1,5,4]

### Constraints

- The number of nodes in the linked list is in the range [0, 10^4].
- -10^6 <= Node.val <= 10^6

---

## Traditional Chinese (繁體中文)

### 問題描述

給定一個單向鏈表的頭節點 `head`，將所有奇數索引的節點組合在一起，然後是所有偶數索引的節點，並返回重新排列後的鏈表。

第一個節點被視為奇數位置，第二個節點為偶數位置，以此類推。

注意，偶數組和奇數組內部的相對順序應保持與輸入時相同。

你必須以 O(1) 的額外空間複雜度和 O(n) 的時間複雜度解決此問題。

### 範例

#### 範例 1
- **輸入:** head = [1,2,3,4,5]
- **輸出:** [1,3,5,2,4]

#### 範例 2
- **輸入:** head = [2,1,3,5,6,4,7]
- **輸出:** [2,3,6,7,1,5,4]

### 約束條件

- 鏈表中的節點數在範圍 [0, 10^4] 內。
- -10^6 <= Node.val <= 10^6
