# 104. Maximum Depth of Binary Tree

**Difficulty:** Easy
**Topics:** Tree, Depth-First Search, Breadth-First Search, Binary Tree
**Companies:** Adobe, Amazon, Apple

Given the `root` of a binary tree, return *its maximum depth*.

A binary tree's **maximum depth** is the number of nodes along the longest path from the root node down to the farthest leaf node.

**Example 1:**

```
        3
       / \
      9   20
         /  \
        15   7
```

- Input: `root = [3,9,20,null,null,15,7]`
- Output: `3`

**Example 2:**

- Input: `root = [1,null,2]`
- Output: `2`

**Constraints:**

- The number of nodes in the tree is in the range `[0, 10^4]`.
- `-100 <= Node.val <= 100`

---

# 104. 二元樹的最大深度

**難度：** 簡單
**主題：** 樹、深度優先搜尋、廣度優先搜尋、二元樹

給定一個二元樹的根節點 `root`，回傳它的**最大深度**。

二元樹的**最大深度**是指：從根節點一路往下到最遠葉節點的最長路徑上，所包含的**節點數量**。

**範例 1：**

```
        3
       / \
      9   20
         /  \
        15   7
```

- 輸入：`root = [3,9,20,null,null,15,7]`
- 輸出：`3`

**範例 2：**

- 輸入：`root = [1,null,2]`
- 輸出：`2`

**限制條件：**

- 樹中的節點數量介於 `[0, 10^4]` 範圍內。
- `-100 <= Node.val <= 100`
