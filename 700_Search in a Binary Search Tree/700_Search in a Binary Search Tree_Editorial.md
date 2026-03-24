# 700. Search in a Binary Search Tree — Editorial

> Source: LeetCode Official Editorial (live authenticated session)

## Background: Binary Search Tree Property

A Binary Search Tree is a binary tree where the key in each node is greater than any key stored in the left sub-tree, and less than any key stored in the right sub-tree.

This property enables efficient search: at each node, we can eliminate half of the remaining tree from consideration.

---

## Approach 1: Recursion

### Algorithm

The recursion implementation follows the BST property directly:

1. If the tree is empty (`root == null`) or the value to find is here (`val == root.val`), return `root`.
2. If `val < root.val`, search into the left subtree: `searchBST(root.left, val)`.
3. If `val > root.val`, search into the right subtree: `searchBST(root.right, val)`.
4. Return `root`.

### Complexity Analysis

- **Time complexity:** O(H), where H is the tree height. This is O(log N) in the average case (balanced BST) and O(N) in the worst case (degenerate/skewed tree).

  Using the master theorem: T(N) = aT(N/b) + Θ(N^d). Here a = 1 (one subproblem), b = 2 (half the size), d = 0 (constant work per level). Since log_b(a) = 0 = d, this is case 2 of the master theorem, giving O(log N).

- **Space complexity:** O(H) to maintain the recursion stack. O(log N) average, O(N) worst case.

---

## Approach 2: Iteration

### Algorithm

To reduce the space complexity, convert the recursive approach into an iterative one:

1. While the tree is not empty (`root != null`) and the value to find is not here (`val != root.val`):
   - If `val < root.val`, go to search into the left subtree: `root = root.left`.
   - If `val > root.val`, go to search into the right subtree: `root = root.right`.
2. Return `root`.

### Complexity Analysis

- **Time complexity:** O(H), where H is the tree height. O(log N) average, O(N) worst case. Same master theorem analysis as Approach 1.

- **Space complexity:** O(1) — no recursion stack needed, only a single pointer variable.

---

# 700. 搜尋二元搜尋樹 — 官方題解

> 來源：LeetCode 官方 Editorial（透過已登入瀏覽器存取）

## 背景：二元搜尋樹的性質

二元搜尋樹 (BST) 是一種二元樹，每個節點的鍵值都大於其左子樹中的所有鍵值，且小於其右子樹中的所有鍵值。

這個性質使得搜尋非常高效：在每個節點處，我們可以排除剩餘樹的一半。

---

## 方法一：遞迴

### 演算法

遞迴實作直接利用 BST 性質：

1. 若樹為空（`root == null`）或當前節點值即為目標（`val == root.val`），回傳 `root`。
2. 若 `val < root.val`，遞迴搜尋左子樹：`searchBST(root.left, val)`。
3. 若 `val > root.val`，遞迴搜尋右子樹：`searchBST(root.right, val)`。
4. 回傳 `root`。

### 複雜度分析

- **時間複雜度：** O(H)，H 為樹高。平衡 BST 平均為 O(log N)，最壞情況（退化為鏈結串列）為 O(N)。

  利用主定理分析：T(N) = aT(N/b) + Θ(N^d)，其中 a = 1（一個子問題），b = 2（大小減半），d = 0（每層常數工作量）。因 log_b(a) = 0 = d，屬於主定理 case 2，故時間複雜度為 O(log N)。

- **空間複雜度：** O(H)，用於遞迴呼叫堆疊。平均 O(log N)，最壞 O(N)。

---

## 方法二：迭代

### 演算法

為了降低空間複雜度，可將遞迴改為迭代：

1. 當樹非空（`root != null`）且當前值不等於目標（`val != root.val`）時：
   - 若 `val < root.val`，往左子樹移動：`root = root.left`。
   - 若 `val > root.val`，往右子樹移動：`root = root.right`。
2. 回傳 `root`。

### 複雜度分析

- **時間複雜度：** O(H)，H 為樹高。平均 O(log N)，最壞 O(N)。主定理分析同方法一。

- **空間複雜度：** O(1) — 不需要遞迴堆疊，僅使用一個指標變數。
