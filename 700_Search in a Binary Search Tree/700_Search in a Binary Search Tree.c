/*
 * 700. Search in a Binary Search Tree
 * 難度：Easy
 * 時間複雜度：O(H)，其中 H 為樹高。平衡 BST 下為 O(log N)，最壞退化為 O(N)
 * 空間複雜度：O(1)（迭代法，不使用遞迴堆疊）
 *
 * 題目：
 *   給定一棵二元搜尋樹 (BST) 的根節點 root 和一個整數 val，
 *   在 BST 中找到節點值等於 val 的節點，並回傳以該節點為根的子樹。
 *   若找不到則回傳 NULL。
 *
 * 演算法（迭代法）：
 *   利用 BST 的性質：左子樹所有值 < 根 < 右子樹所有值
 *   從根節點開始，每次比較 val 與當前節點值：
 *     - 若相等，找到目標，回傳該節點
 *     - 若 val < 當前值，往左子樹走
 *     - 若 val > 當前值，往右子樹走
 *     - 若走到 NULL，代表找不到，回傳 NULL
 *
 * 具體範例 (Example 1)：root = [4,2,7,1,3], val = 2
 *
 *   初始 BST：
 *
 *           (4)          <-- root
 *          /   \
 *        (2)   (7)
 *       /   \
 *     (1)   (3)
 *
 *   搜尋過程：
 *
 *   Step 1: root->val = 4, val = 2
 *           2 < 4 → 往左走
 *
 *           (4)
 *          /
 *       >[2]<        <-- 移動到這裡
 *       /   \
 *     (1)   (3)
 *
 *   Step 2: root->val = 2, val = 2
 *           2 == 2 → 找到！回傳此節點
 *
 *   回傳的子樹：
 *        (2)
 *       /   \
 *     (1)   (3)
 *
 *   輸出：[2,1,3]
 *
 * 具體範例 (Example 2)：root = [4,2,7,1,3], val = 5
 *
 *   Step 1: root->val = 4, val = 5 → 5 > 4 → 往右走
 *   Step 2: root->val = 7, val = 5 → 5 < 7 → 往左走
 *   Step 3: root->left == NULL → 找不到，回傳 NULL
 *
 *   輸出：[]
 */

/* 二元樹節點定義 */
struct TreeNode {
    int val;                /* 節點儲存的值 */
    struct TreeNode *left;  /* 指向左子節點的指標 */
    struct TreeNode *right; /* 指向右子節點的指標 */
};

struct TreeNode* searchBST(struct TreeNode* root, int val) {
    /* 持續在樹中搜尋，直到走到 NULL 或找到目標 */
    while (root != NULL && root->val != val) {
        /*
         * BST 性質：
         *   val < root->val → 目標只可能在左子樹
         *   val > root->val → 目標只可能在右子樹
         */
        if (val < root->val) {
            root = root->left;   /* 往左子樹移動 */
        } else {
            root = root->right;  /* 往右子樹移動 */
        }
    }
    /* 迴圈結束時：root 為 NULL（找不到）或 root->val == val（找到） */
    return root;
}
