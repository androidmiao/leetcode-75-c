# 206. Reverse Linked List

**Difficulty:** Easy

## Problem Statement

Given the `head` of a singly linked list, reverse the list, and return *the reversed list*.

## Examples

### Example 1

```
Input: head = [1,2,3,4,5]
Output: [5,4,3,2,1]
```

Visual:

```
1 -> 2 -> 3 -> 4 -> 5
         |
         v
5 -> 4 -> 3 -> 2 -> 1
```

### Example 2

```
Input: head = [1,2]
Output: [2,1]
```

Visual:

```
1 -> 2
  |
  v
2 -> 1
```

### Example 3

```
Input: head = []
Output: []
```

## Constraints

- The number of nodes in the list is the range `[0, 5000]`.
- `-5000 <= Node.val <= 5000`

## Follow up

A linked list can be reversed either iteratively or recursively. Could you implement both?

---

---

# 206. Reverse Linked List — 題目描述（繁體中文）

**難度：** Easy

## 題目敘述

給定一個單向鏈結串列的 `head`，將串列反轉，並回傳*反轉後的串列*。

## 範例

### 範例 1

```
輸入：head = [1,2,3,4,5]
輸出：[5,4,3,2,1]
```

圖示：

```
1 -> 2 -> 3 -> 4 -> 5
         |
         v
5 -> 4 -> 3 -> 2 -> 1
```

### 範例 2

```
輸入：head = [1,2]
輸出：[2,1]
```

圖示：

```
1 -> 2
  |
  v
2 -> 1
```

### 範例 3

```
輸入：head = []
輸出：[]
```

## 限制條件

- 串列中的節點數量範圍為 `[0, 5000]`。
- `-5000 <= Node.val <= 5000`

## 進階

鏈結串列可以用迭代或遞迴的方式來反轉。你能兩種都實作嗎？
