/*
 * 104. Maximum Depth of Binary Tree
 * 難度：Easy
 * 時間複雜度：O(n)，n 為節點數量，每個節點恰好拜訪一次
 * 空間複雜度：O(h)，h 為樹高，來自遞迴呼叫堆疊
 *             最差 O(n)（退化成鏈結），最佳 O(log n)（完全平衡）
 *
 * 演算法：DFS 遞迴
 * ─────────────────
 * 核心觀念：一個節點的最大深度 = max(左子樹深度, 右子樹深度) + 1
 *
 * 步驟：
 *   1. 若目前節點為 NULL → 回傳 0（遞迴終止條件）
 *   2. 遞迴計算左子樹的最大深度 leftDepth
 *   3. 遞迴計算右子樹的最大深度 rightDepth
 *   4. 回傳 max(leftDepth, rightDepth) + 1
 *
 * 範例 1：root = [3,9,20,null,null,15,7]
 *
 *         3          depth = max(1, 2) + 1 = 3
 *        / \
 *       9   20       9: max(0,0)+1=1    20: max(1,1)+1=2
 *          /  \
 *         15   7     15: max(0,0)+1=1   7: max(0,0)+1=1
 *
 * 遞迴展開流程：
 *
 *   maxDepth(3)
 *   ├─ maxDepth(9)
 *   │  ├─ maxDepth(NULL) = 0
 *   │  └─ maxDepth(NULL) = 0
 *   │  => max(0, 0) + 1 = 1
 *   └─ maxDepth(20)
 *      ├─ maxDepth(15)
 *      │  ├─ maxDepth(NULL) = 0
 *      │  └─ maxDepth(NULL) = 0
 *      │  => max(0, 0) + 1 = 1
 *      └─ maxDepth(7)
 *         ├─ maxDepth(NULL) = 0
 *         └─ maxDepth(NULL) = 0
 *         => max(0, 0) + 1 = 1
 *      => max(1, 1) + 1 = 2
 *   => max(1, 2) + 1 = 3  ← 答案
 *
 * 範例 2：root = [1,null,2]
 *
 *     1         depth = max(0, 1) + 1 = 2
 *      \
 *       2       depth = max(0, 0) + 1 = 1
 *
 *   maxDepth(1)
 *   ├─ maxDepth(NULL) = 0
 *   └─ maxDepth(2)
 *      ├─ maxDepth(NULL) = 0
 *      └─ maxDepth(NULL) = 0
 *      => max(0, 0) + 1 = 1
 *   => max(0, 1) + 1 = 2  ← 答案
 */

/* 二元樹節點的定義 */
struct TreeNode {
    int val;                /* 節點儲存的值 */
    struct TreeNode *left;  /* 指向左子節點的指標 */
    struct TreeNode *right; /* 指向右子節點的指標 */
};

int maxDepth(struct TreeNode* root) {
    /* 遞迴終止條件：空節點代表深度為 0 */
    if (root == NULL) {
        return 0;
    }

    /* 遞迴計算左子樹的最大深度 */
    int leftDepth = maxDepth(root->left);
    /* 遞迴計算右子樹的最大深度 */
    int rightDepth = maxDepth(root->right);

    /* 取左右子樹中較大的深度，再加 1（目前這一層） */
    return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
}
