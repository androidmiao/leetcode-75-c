# 700. Search in a Binary Search Tree

**Difficulty:** Easy

## Problem Statement

You are given the `root` of a binary search tree (BST) and an integer `val`.

Find the node in the BST that the node's value equals `val` and return the subtree rooted with that node. If such a node does not exist, return `null`.

## Examples

### Example 1

```
Input: root = [4,2,7,1,3], val = 2
Output: [2,1,3]
```

Tree structure:

```
        4
       / \
      2   7
     / \
    1   3
```

We search for value 2 and find it as the left child of root. The returned subtree is rooted at node 2, which contains nodes 2, 1, and 3.

### Example 2

```
Input: root = [4,2,7,1,3], val = 5
Output: []
```

Value 5 does not exist in the BST, so we return null.

## Constraints

- The number of nodes in the tree is in the range `[1, 5000]`.
- `1 <= Node.val <= 10^7`
- `root` is a binary search tree.
- `1 <= val <= 10^7`

---

# 700. 搜尋二元搜尋樹

**難度：** Easy

## 題目敘述

給定一棵二元搜尋樹 (BST) 的根節點 `root` 和一個整數 `val`。

在 BST 中找到節點值等於 `val` 的節點，並回傳以該節點為根的子樹。若該節點不存在，則回傳 `null`。

## 範例

### 範例 1

```
輸入：root = [4,2,7,1,3], val = 2
輸出：[2,1,3]
```

樹的結構：

```
        4
       / \
      2   7
     / \
    1   3
```

搜尋值 2，在根節點的左子節點找到。回傳以節點 2 為根的子樹，包含節點 2、1、3。

### 範例 2

```
輸入：root = [4,2,7,1,3], val = 5
輸出：[]
```

值 5 不存在於 BST 中，因此回傳 null。

## 限制條件

- 樹中節點數量在 `[1, 5000]` 範圍內。
- `1 <= Node.val <= 10^7`
- `root` 是一棵二元搜尋樹。
- `1 <= val <= 10^7`
