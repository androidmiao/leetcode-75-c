Given the root of a binary tree, imagine yourself standing on the right side of it, return the values of the nodes you can see ordered from top to bottom.

 

Example 1:

Input: root = [1,2,3,null,5,null,4]

Output: [1,3,4]

Explanation:



Example 2:

Input: root = [1,2,3,4,null,null,null,5]

Output: [1,3,4,5]

Explanation:



Example 3:

Input: root = [1,null,3]

Output: [1,3]

Example 4:

Input: root = []

Output: []

 

Constraints:

The number of nodes in the tree is in the range [0, 100].
-100 <= Node.val <= 100

---

給定一棵二元樹的根節點 `root`，請想像你站在這棵樹的右側，回傳你從上到下所能看到的節點值。

範例 1：
輸入：`root = [1,2,3,null,5,null,4]`
輸出：`[1,3,4]`
說明：


範例 2：
輸入：`root = [1,2,3,4,null,null,null,5]`
輸出：`[1,3,4,5]`
說明：


範例 3：
輸入：`root = [1,null,3]`
輸出：`[1,3]`

範例 4：
輸入：`root = []`
輸出：`[]`

 

限制條件：
樹中的節點數量範圍為 `[0, 100]`。
`-100 <= Node.val <= 100`
