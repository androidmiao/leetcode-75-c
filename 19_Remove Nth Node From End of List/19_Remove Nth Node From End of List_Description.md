You are given the head of a singly linked list. Remove the `n`th node counted from the end of the list, and return the head of the updated list.

Example 1:

Input: `head = [1,2,3,4,5], n = 2`
Output: `[1,2,3,5]`

Example 2:

Input: `head = [1], n = 1`
Output: `[]`

Example 3:

Input: `head = [1,2], n = 1`
Output: `[1]`

Constraints:

- The number of nodes in the list is `sz`.
- `1 <= sz <= 30`
- `0 <= Node.val <= 100`
- `1 <= n <= sz`

Follow-up:

Can you solve it in one traversal?

Hint:

Maintain two pointers and keep one of them delayed by `n` nodes.

---
中文翻譯

給定一個單向鏈結串列的頭節點 `head`，請刪除倒數第 `n` 個節點，並回傳刪除後鏈結串列的頭節點。

範例 1：

輸入：`head = [1,2,3,4,5], n = 2`
輸出：`[1,2,3,5]`

範例 2：

輸入：`head = [1], n = 1`
輸出：`[]`

範例 3：

輸入：`head = [1,2], n = 1`
輸出：`[1]`

限制條件：

- 串列中的節點數量為 `sz`
- `1 <= sz <= 30`
- `0 <= Node.val <= 100`
- `1 <= n <= sz`

進階挑戰：

你能不能在一次走訪中完成？

提示：

可以維護兩個指標，並讓其中一個指標比另一個晚 `n` 個節點。
