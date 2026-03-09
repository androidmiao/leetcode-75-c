大致上一樣，但**只對應到 `Approach 1: Recursion`**，不是另外兩種。

我寫的 C 解法在 [104_Maximum Depth of Binary Tree.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/104_Maximum Depth of Binary Tree/104_Maximum Depth of Binary Tree.c) 是這種邏輯：

```c
int leftDepth = maxDepth(root->left);
int rightDepth = maxDepth(root->right);
return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
```

這和 [104_Maximum Depth of Binary Tree_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/104_Maximum Depth of Binary Tree/104_Maximum Depth of Binary Tree_Solution.md) 裡的 `Approach 1: Recursion` 是同一種核心概念：
- 遇到 `NULL` 回傳 `0`
- 遞迴計算左子樹深度
- 遞迴計算右子樹深度
- 取較大值再加 `1`

不一樣的地方是，`Solution.md` 還有：
- `Approach 2`: Tail Recursion + BFS
- `Approach 3`: Iteration

而我目前寫的不是尾遞迴，也不是迭代 stack 版本。

所以結論是：**有一樣，但只跟文件的第 1 種遞迴解法一樣。**