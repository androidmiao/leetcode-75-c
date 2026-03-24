# 23. Merge k Sorted Lists

**Difficulty:** Hard

## Problem Statement

You are given an array of `k` linked-lists `lists`, each linked-list is sorted in ascending order.

*Merge all the linked-lists into one sorted linked-list and return it.*

## Examples

### Example 1

```
Input: lists = [[1,4,5],[1,3,4],[2,6]]
Output: [1,1,2,3,4,4,5,6]
Explanation: The linked-lists are:
[
  1->4->5,
  1->3->4,
  2->6
]
merging them into one sorted linked list:
1->1->2->3->4->4->5->6
```

### Example 2

```
Input: lists = []
Output: []
```

### Example 3

```
Input: lists = [[]]
Output: []
```

## Constraints

- `k == lists.length`
- `0 <= k <= 10^4`
- `0 <= lists[i].length <= 500`
- `-10^4 <= lists[i][j] <= 10^4`
- `lists[i]` is sorted in **ascending order**.
- The sum of `lists[i].length` will not exceed `10^4`.

---

# 23. 合併 K 個升序鏈結串列

**難度：** 困難

## 題目描述

給你一個鏈結串列陣列，每個鏈結串列都已經按升序排列。

*請你將所有鏈結串列合併到一個升序鏈結串列中，回傳合併後的鏈結串列。*

## 範例

### 範例 1

```
輸入：lists = [[1,4,5],[1,3,4],[2,6]]
輸出：[1,1,2,3,4,4,5,6]
說明：鏈結串列陣列如下：
[
  1->4->5,
  1->3->4,
  2->6
]
將它們合併到一個有序鏈結串列中得到：
1->1->2->3->4->4->5->6
```

### 範例 2

```
輸入：lists = []
輸出：[]
```

### 範例 3

```
輸入：lists = [[]]
輸出：[]
```

## 約束條件

- `k == lists.length`
- `0 <= k <= 10^4`
- `0 <= lists[i].length <= 500`
- `-10^4 <= lists[i][j] <= 10^4`
- `lists[i]` 按 **升序** 排列
- `lists[i].length` 的總和不超過 `10^4`
