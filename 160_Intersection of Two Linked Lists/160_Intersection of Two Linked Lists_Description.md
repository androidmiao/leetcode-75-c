# 160. Intersection of Two Linked Lists

**Difficulty:** Easy
**Topics:** Hash Table, Linked List, Two Pointers
**Link:** https://leetcode.com/problems/intersection-of-two-linked-lists/

---

## Problem Statement

Given the heads of two singly linked-lists `headA` and `headB`, return *the node at which the two lists intersect*. If the two linked lists have no intersection at all, return `null`.

For example, the following two linked lists begin to intersect at node `c1`:

```
A:    a1 → a2
                ↘
                  c1 → c2 → c3
                ↗
B: b1 → b2 → b3
```

The test cases are generated such that there are **no cycles** anywhere in the entire linked structure.

**Note:** The linked lists must **retain their original structure** after the function returns.

---

## Custom Judge

The inputs to the judge are given as follows (your program is **not** given these inputs):

- `intersectVal` — The value of the node where the intersection occurs. This is `0` if there is no intersected node.
- `listA` — The first linked list.
- `listB` — The second linked list.
- `skipA` — The number of nodes to skip ahead in `listA` (starting from the head) to get to the intersected node.
- `skipB` — The number of nodes to skip ahead in `listB` (starting from the head) to get to the intersected node.

The judge will then create the linked structure based on these inputs and pass the two heads, `headA` and `headB`, to your program. If you correctly return the intersected node, then your answer will be **accepted**.

---

## Examples

### Example 1

```
Input:  intersectVal = 8, listA = [4,1,8,4,5], listB = [5,6,1,8,4,5], skipA = 2, skipB = 3
Output: Intersected at '8'
Explanation: The intersected node's value is 8 (note that this must not be 0 if the two lists intersect).
             From the head of A, it reads as 4 → 1 → 8 → 4 → 5.
             From the head of B, it reads as 5 → 6 → 1 → 8 → 4 → 5.
             There are 2 nodes before the intersected node in A; there are 3 nodes before it in B.
```

### Example 2

```
Input:  intersectVal = 2, listA = [1,9,1,2,4], listB = [3,2,4], skipA = 3, skipB = 1
Output: Intersected at '2'
```

### Example 3

```
Input:  intersectVal = 0, listA = [2,6,4], listB = [1,5], skipA = 3, skipB = 2
Output: No intersection
```

---

## Constraints

- The number of nodes of `listA` is in the `m`.
- The number of nodes of `listB` is in the `n`.
- `1 <= m, n <= 3 * 10^4`
- `1 <= Node.val <= 10^5`
- `0 <= skipA <= m`
- `0 <= skipB <= n`
- `intersectVal` is `0` if `listA` and `listB` do not intersect.
- `intersectVal == listA[skipA] == listB[skipB]` if `listA` and `listB` intersect.

---

## Follow Up

Could you write a solution that runs in **O(m + n)** time and use only **O(1)** memory?

---

## 中文翻譯

### 題目描述

給定兩個單向鏈結串列的頭節點 `headA` 和 `headB`，回傳兩條串列的**交叉節點**。若兩條串列完全沒有交叉，回傳 `null`。

例如下圖，兩條串列從節點 `c1` 開始交叉：

```
A:    a1 → a2
                ↘
                  c1 → c2 → c3
                ↗
B: b1 → b2 → b3
```

測試資料保證整個鏈結結構中**不存在任何環**。

**注意：** 函式回傳後，兩條鏈結串列必須**保持原始結構不變**。

---

### 自訂評測方式

評測系統的輸入格式如下（你的程式**不會**收到這些輸入）：

- `intersectVal` — 交叉節點的值。若無交叉則為 `0`。
- `listA` — 第一條鏈結串列。
- `listB` — 第二條鏈結串列。
- `skipA` — 從 `listA` 的頭跳過多少個節點可到達交叉節點。
- `skipB` — 從 `listB` 的頭跳過多少個節點可到達交叉節點。

評測系統會根據這些輸入建構鏈結結構，並將 `headA` 和 `headB` 傳給你的程式。若你正確回傳交叉節點，答案即為**通過**。

---

### 範例

**範例 1**

```
輸入：intersectVal = 8, listA = [4,1,8,4,5], listB = [5,6,1,8,4,5], skipA = 2, skipB = 3
輸出：Intersected at '8'
說明：交叉節點的值為 8。
     從 A 的頭讀取為 4 → 1 → 8 → 4 → 5。
     從 B 的頭讀取為 5 → 6 → 1 → 8 → 4 → 5。
     A 中交叉節點前有 2 個節點；B 中交叉節點前有 3 個節點。
```

**範例 2**

```
輸入：intersectVal = 2, listA = [1,9,1,2,4], listB = [3,2,4], skipA = 3, skipB = 1
輸出：Intersected at '2'
```

**範例 3**

```
輸入：intersectVal = 0, listA = [2,6,4], listB = [1,5], skipA = 3, skipB = 2
輸出：No intersection（無交叉）
```

---

### 限制條件

- `listA` 的節點數為 `m`。
- `listB` 的節點數為 `n`。
- `1 <= m, n <= 3 × 10⁴`
- `1 <= Node.val <= 10⁵`
- `0 <= skipA <= m`
- `0 <= skipB <= n`
- 若 `listA` 與 `listB` 不交叉，則 `intersectVal` 為 `0`。
- 若 `listA` 與 `listB` 交叉，則 `intersectVal == listA[skipA] == listB[skipB]`。

---

### 進階

你能否寫出時間複雜度 **O(m + n)** 且僅使用 **O(1)** 記憶體的解法？
