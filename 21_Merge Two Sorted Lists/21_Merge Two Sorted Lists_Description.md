You are given the heads of two sorted linked lists list1 and list2.

Merge the two lists into one sorted list. The list should be made by splicing together the nodes of the first two lists.

Return the head of the merged linked list.

 

Example 1:


Input: list1 = [1,2,4], list2 = [1,3,4]
Output: [1,1,2,3,4,4]
Example 2:

Input: list1 = [], list2 = []
Output: []
Example 3:

Input: list1 = [], list2 = [0]
Output: [0]
 

Constraints:

The number of nodes in both lists is in the range [0, 50].
-100 <= Node.val <= 100
Both list1 and list2 are sorted in non-decreasing order.


---
你會拿到兩個已排序 linked list 的頭節點 `list1` 與 `list2`。

請將這兩個 linked list 合併成一個已排序 linked list。這個新串列應該透過拼接前兩個串列的節點來組成。

回傳合併後 linked list 的頭節點。

範例 1：

輸入：`list1 = [1,2,4], list2 = [1,3,4]`
輸出：`[1,1,2,3,4,4]`

範例 2：

輸入：`list1 = [], list2 = []`
輸出：`[]`

範例 3：

輸入：`list1 = [], list2 = [0]`
輸出：`[0]`

限制條件：

兩個串列中的節點總數範圍為 `[0, 50]`。
`-100 <= Node.val <= 100`
`list1` 和 `list2` 都已依照非遞減順序排序。
