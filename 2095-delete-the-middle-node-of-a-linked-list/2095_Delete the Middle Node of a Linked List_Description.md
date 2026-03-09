You are given the head of a linked list. Delete the middle node, and return the head of the modified linked list.

The middle node of a linked list of size n is the ⌊n / 2⌋th node from the start using 0-based indexing, where ⌊x⌋ denotes the largest integer less than or equal to x.

For n = 1, 2, 3, 4, and 5, the middle nodes are 0, 1, 1, 2, and 2, respectively.
 

Example 1:


Input: head = [1,3,4,7,1,2,6]
Output: [1,3,4,1,2,6]
Explanation:
The above figure represents the given linked list. The indices of the nodes are written below.
Since n = 7, node 3 with value 7 is the middle node, which is marked in red.
We return the new list after removing this node. 
Example 2:


Input: head = [1,2,3,4]
Output: [1,2,4]
Explanation:
The above figure represents the given linked list.
For n = 4, node 2 with value 3 is the middle node, which is marked in red.
Example 3:


Input: head = [2,1]
Output: [2]
Explanation:
The above figure represents the given linked list.
For n = 2, node 1 with value 1 is the middle node, which is marked in red.
Node 0 with value 2 is the only node remaining after removing node 1.
 

Constraints:

The number of nodes in the list is in the range [1, 105].
1 <= Node.val <= 105

---
中文翻譯

給定一個鏈結串列的頭節點 `head`。請刪除中間節點，並回傳修改後鏈結串列的頭節點。

大小為 `n` 的鏈結串列其中間節點，是從起點開始、使用 0-based indexing 時的第 `⌊n / 2⌋` 個節點，其中 `⌊x⌋` 表示小於或等於 `x` 的最大整數。

當 `n = 1, 2, 3, 4, 5` 時，中間節點分別是 `0, 1, 1, 2, 2`。

範例 1：

輸入：`head = [1,3,4,7,1,2,6]`
輸出：`[1,3,4,1,2,6]`
說明：
上圖表示給定的鏈結串列。節點的索引寫在下方。
因為 `n = 7`，值為 `7` 的第 `3` 個節點是中間節點，圖中以紅色標示。
移除此節點後，回傳新的鏈結串列。

範例 2：

輸入：`head = [1,2,3,4]`
輸出：`[1,2,4]`
說明：
上圖表示給定的鏈結串列。
當 `n = 4` 時，值為 `3` 的第 `2` 個節點是中間節點，圖中以紅色標示。

範例 3：

輸入：`head = [2,1]`
輸出：`[2]`
說明：
上圖表示給定的鏈結串列。
當 `n = 2` 時，值為 `1` 的第 `1` 個節點是中間節點，圖中以紅色標示。
刪除第 `1` 個節點後，只剩下值為 `2` 的第 `0` 個節點。

限制條件：

串列中的節點數量範圍為 `[1, 10^5]`。
`1 <= Node.val <= 10^5`
