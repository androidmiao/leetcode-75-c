Solution Article

Summary

The official editorial presents two constant-space strategies:

1. Count the length first, then delete the corresponding node from the front.
2. Keep two pointers `n + 1` nodes apart so the trailing pointer lands on the predecessor of the node to remove.

The standalone C solution in this folder uses Approach 2.

Approach 1: Two Pass Algorithm
Intuition

If the list length is `L`, then removing the `n`th node from the end is the same as removing the `(L - n + 1)`th node from the front. After computing `L`, the deletion position is known.

A dummy node before `head` simplifies corner cases, especially when the removed node is the original head.

Algorithm

- Create `dummy -> head`.
- Traverse the list once to compute the length `L`.
- Start from `dummy` and move forward `L - n` steps.
- The next node is the target, so bypass it with `current->next = current->next->next`.
- Return `dummy.next`.

Complexity Analysis

Time complexity: `O(L)`

The list is traversed once to compute the length and once more to reach the predecessor of the target node.

Space complexity: `O(1)`

Only a few pointers and counters are used.

Approach 2: One Pass Algorithm
Intuition

We want `slow` to stop exactly one node before the node that should be removed. To do that cleanly, put both pointers at a dummy node, then move `fast` ahead by `n + 1` steps. This creates a fixed gap between `fast` and `slow`.

When `fast` reaches `NULL`, `slow` is immediately before the node we need to delete.

Algorithm

- Create a dummy node with `dummy.next = head`.
- Initialize `fast = &dummy` and `slow = &dummy`.
- Advance `fast` exactly `n + 1` steps.
- Move `fast` and `slow` forward together until `fast == NULL`.
- Delete `slow->next` by relinking `slow->next = slow->next->next`.
- Return `dummy.next`.

Complexity Analysis

Time complexity: `O(L)`

Even though one pointer starts earlier, the algorithm still performs a single forward sweep over the list.

Space complexity: `O(1)`

Only constant extra space is required.

---
中文整理

摘要

官方 editorial 主要整理了兩種 `O(1)` 額外空間的做法：

1. 先算整條串列長度，再換算成「從前面數過去要刪哪個節點」。
2. 用兩個指標維持固定距離，讓後面的指標最後停在目標節點的前一個位置。

這個資料夾中的 `.c` 解法採用方法二。

方法一：兩次走訪
直覺

如果串列長度是 `L`，那麼刪除倒數第 `n` 個節點，就等同於刪除正數第 `L - n + 1` 個節點。只要先把長度算出來，刪除位置就固定了。

另外，在 `head` 前面加上一個 `dummy` 節點，可以把「刪除原本頭節點」這種情況一起用同一套邏輯處理掉。

演算法

- 建立 `dummy -> head`
- 第一次走訪整條串列，算出長度 `L`
- 從 `dummy` 出發，往前走 `L - n` 步
- 此時下一個節點就是要刪掉的目標，直接做 `current->next = current->next->next`
- 回傳 `dummy.next`

複雜度分析

時間複雜度：`O(L)`

第一次用來計算長度，第二次用來走到目標節點的前一個位置，因此總體仍是線性時間。

空間複雜度：`O(1)`

只用了少量指標與整數變數。

方法二：一次走訪
直覺

我們真正想停住的位置，不是目標節點本身，而是它的前一個節點。做法是讓 `fast` 與 `slow` 都從 `dummy` 出發，先讓 `fast` 多走 `n + 1` 步，於是兩者之間就會維持固定距離。

接著只要同步移動這兩個指標，當 `fast` 走到 `NULL` 時，`slow` 就會剛好站在目標節點前面。

演算法

- 建立 `dummy`，並令 `dummy.next = head`
- 設定 `fast = &dummy`、`slow = &dummy`
- 先讓 `fast` 往前走剛好 `n + 1` 步
- 當 `fast != NULL` 時，`fast` 和 `slow` 一起前進
- 迴圈結束後，`slow->next` 就是要刪掉的節點
- 透過 `slow->next = slow->next->next` 完成刪除
- 回傳 `dummy.next`

複雜度分析

時間複雜度：`O(L)`

雖然兩個指標有先後差距，但整體仍然只是在串列上往前掃過一次。

空間複雜度：`O(1)`

只需要固定數量的額外指標。
