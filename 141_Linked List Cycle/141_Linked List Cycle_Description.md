Given head, the head of a linked list, determine if the linked list has a cycle in it.

There is a cycle in a linked list if there is some node in the list that can be reached again by continuously following the next pointer. Internally, pos is used to denote the index of the node that tail's next pointer is connected to. Note that pos is not passed as a parameter.

Return true if there is a cycle in the linked list. Otherwise, return false.

Example 1:

Input: head = [3,2,0,-4], pos = 1
Output: true
Explanation: There is a cycle in the linked list, where the tail connects to the 1st node (0-indexed).

Example 2:

Input: head = [1,2], pos = 0
Output: true
Explanation: There is a cycle in the linked list, where the tail connects to the 0th node.

Example 3:

Input: head = [1], pos = -1
Output: false
Explanation: There is no cycle in the linked list.

Constraints:

The number of the nodes in the list is in the range [0, 10^4].

-10^5 <= Node.val <= 10^5

pos is -1 or a valid index in the linked-list.

Follow up: Can you solve it using O(1) (i.e. constant) memory?


---

給定 `head`，也就是 linked list 的頭節點，請判斷這個 linked list 裡是否存在 cycle（環）。

如果一個 linked list 中，存在某個節點可以透過持續沿著 `next` 指標往下走而再次回到自己，那麼這個 linked list 就有 cycle。題目內部會使用 `pos` 來表示 tail 的 `next` 指標連接到哪一個節點的索引位置。注意，`pos` 不會作為函式參數傳入。

如果 linked list 中有 cycle，回傳 `true`；否則回傳 `false`。

範例 1：

輸入：`head = [3,2,0,-4], pos = 1`
輸出：`true`
說明：這個 linked list 中有 cycle，因為尾端節點連回第 `1` 個節點（從 `0` 開始編號）。

範例 2：

輸入：`head = [1,2], pos = 0`
輸出：`true`
說明：這個 linked list 中有 cycle，因為尾端節點連回第 `0` 個節點。

範例 3：

輸入：`head = [1], pos = -1`
輸出：`false`
說明：這個 linked list 中沒有 cycle。

限制條件：

linked list 中節點的數量範圍為 `[0, 10^4]`。

`Node.val` 的範圍為 `-10^5 <= Node.val <= 10^5`。

`pos` 可能是 `-1`，或是 linked list 中一個有效的索引位置。

進階問題：你能否使用 `O(1)`（也就是常數）額外記憶體來解這題？
