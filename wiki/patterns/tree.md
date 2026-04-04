# Tree 樹結構模式

## 概述 Overview

樹是一種遞迴的分層數據結構，是圖論的特例（無環連通圖）。二叉樹是最常見的樹結構，每個節點最多有兩個子節點。本模式涵蓋三種遍歷方法（前序、中序、後序）和層級遍歷，以及二叉搜尋樹（BST）的特性。

樹的遍歷可以用遞迴或顯式堆棧實現。理解遞迴的含義對樹的問題至關重要。

## 核心概念 Core Concepts

### 1. 樹的數據結構

```c
// 二叉樹節點定義
typedef struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 創建新節點
TreeNode* create_node(int val) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->val = val;
    node->left = NULL;
    node->right = NULL;
    return node;
}
```

### 2. 深度優先遍歷 DFS

**前序遍歷（Pre-order）：根 -> 左 -> 右**
```c
void preorder(TreeNode* root) {
    if (root == NULL) return;

    // 訪問當前節點
    printf("%d ", root->val);

    // 訪問左子樹
    preorder(root->left);

    // 訪問右子樹
    preorder(root->right);
}
```

**中序遍歷（In-order）：左 -> 根 -> 右**
```c
void inorder(TreeNode* root) {
    if (root == NULL) return;

    inorder(root->left);
    printf("%d ", root->val);
    inorder(root->right);
}
```

**後序遍歷（Post-order）：左 -> 右 -> 根**
```c
void postorder(TreeNode* root) {
    if (root == NULL) return;

    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->val);
}
```

### 3. 深度計算

使用遞迴計算樹的最大深度。遞迴的基本情況是空節點（深度為 0），遞迴情況是 max(左子樹深度, 右子樹深度) + 1。

```c
int max_depth(TreeNode* root) {
    if (root == NULL) return 0;

    int left_depth = max_depth(root->left);
    int right_depth = max_depth(root->right);

    return (left_depth > right_depth ? left_depth : right_depth) + 1;
}
```

### 4. 寬度優先遍歷 BFS（層級遍歷）

使用隊列進行層級遍歷。

```c
// 隊列定義
typedef struct {
    TreeNode* items[1000];
    int front, rear;
} Queue;

void queue_enqueue(Queue* q, TreeNode* node) {
    q->items[q->rear++] = node;
}

TreeNode* queue_dequeue(Queue* q) {
    return q->items[q->front++];
}

bool queue_empty(Queue* q) {
    return q->front >= q->rear;
}

// 層級遍歷
int** level_order(TreeNode* root, int* returnSize, int** returnColumnSizes) {
    if (root == NULL) {
        *returnSize = 0;
        return NULL;
    }

    int** result = (int**)malloc(10000 * sizeof(int*));
    int* result_sizes = (int*)malloc(10000 * sizeof(int));
    Queue queue = {0};
    queue_enqueue(&queue, root);
    int level = 0;

    while (!queue_empty(&queue)) {
        int level_size = queue.rear - queue.front;
        result[level] = (int*)malloc(level_size * sizeof(int));
        result_sizes[level] = level_size;

        for (int i = 0; i < level_size; i++) {
            TreeNode* node = queue_dequeue(&queue);
            result[level][i] = node->val;

            if (node->left != NULL) {
                queue_enqueue(&queue, node->left);
            }
            if (node->right != NULL) {
                queue_enqueue(&queue, node->right);
            }
        }
        level++;
    }

    *returnSize = level;
    *returnColumnSizes = result_sizes;
    return result;
}
```

### 5. 二叉搜尋樹（BST）性質

BST 的中序遍歷結果是有序的。搜尋時利用 BST 的排序性質：左子樹所有值 < 根 < 右子樹所有值。

```c
// BST 搜尋
TreeNode* search_bst(TreeNode* root, int val) {
    if (root == NULL) return NULL;

    if (val == root->val) {
        return root;
    } else if (val < root->val) {
        // 目標在左子樹
        return search_bst(root->left, val);
    } else {
        // 目標在右子樹
        return search_bst(root->right, val);
    }
}
```

### 6. 迭代遍歷（使用堆棧）

用顯式堆棧替代遞迴，避免堆棧溢位。

**迭代前序遍歷：**
```c
void preorder_iterative(TreeNode* root) {
    if (root == NULL) return;

    TreeNode* stack[1000];
    int top = 0;
    stack[top++] = root;

    while (top > 0) {
        TreeNode* node = stack[--top];
        printf("%d ", node->val);

        // 注意：先壓右節點，再壓左節點，因為棧是 LIFO
        if (node->right != NULL) {
            stack[top++] = node->right;
        }
        if (node->left != NULL) {
            stack[top++] = node->left;
        }
    }
}
```

**迭代中序遍歷：**
```c
void inorder_iterative(TreeNode* root) {
    TreeNode* stack[1000];
    int top = 0;
    TreeNode* current = root;

    while (current != NULL || top > 0) {
        // 走到最左邊的節點
        while (current != NULL) {
            stack[top++] = current;
            current = current->left;
        }

        // 當前為 NULL，彈出棧頂元素
        current = stack[--top];
        printf("%d ", current->val);

        // 訪問右子樹
        current = current->right;
    }
}
```

## 常見變體 Common Variants

### 變體 1：最大深度 (Problem 104)

計算樹的最大深度。使用遞迴，基本情況是葉子節點。

```c
int maxDepth(TreeNode* root) {
    if (root == NULL) return 0;
    return 1 + (maxDepth(root->left) > maxDepth(root->right) ?
               maxDepth(root->left) : maxDepth(root->right));
}
```

### 變體 2：右視圖 (Problem 199)

層級遍歷，取每層最後一個節點（最右邊的）。

```c
int** rightSideView(TreeNode* root, int* returnSize, int** returnColumnSizes) {
    int** result = (int**)malloc(100 * sizeof(int*));
    *returnColumnSizes = (int*)malloc(100 * sizeof(int));
    *returnSize = 0;

    if (root == NULL) return result;

    Queue queue = {0};
    queue_enqueue(&queue, root);

    while (!queue_empty(&queue)) {
        int level_size = queue.rear - queue.front;
        result[*returnSize] = (int*)malloc(sizeof(int));
        (*returnColumnSizes)[*returnSize] = 1;

        // 取這一層最後一個節點
        for (int i = 0; i < level_size; i++) {
            TreeNode* node = queue_dequeue(&queue);

            if (i == level_size - 1) {
                result[*returnSize][0] = node->val;
            }

            if (node->left != NULL) {
                queue_enqueue(&queue, node->left);
            }
            if (node->right != NULL) {
                queue_enqueue(&queue, node->right);
            }
        }
        (*returnSize)++;
    }

    return result;
}
```

### 變體 3：BST 搜尋 (Problem 700)

利用 BST 的排序性質，選擇正確的子樹進行搜尋。

```c
TreeNode* searchBST(TreeNode* root, int val) {
    if (root == NULL) return NULL;

    if (val == root->val) {
        return root;
    } else if (val < root->val) {
        return searchBST(root->left, val);
    } else {
        return searchBST(root->right, val);
    }
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 104 | Maximum Depth of Binary Tree | Easy | 遞迴深度計算 |
| 199 | Binary Tree Right Side View | Med | 層級遍歷取最右 |
| 700 | Search in a Binary Search Tree | Easy | BST 性質遍歷 |

## 內核連接 Kernel Connection

Linux 內核在多個地方使用樹結構：

- **rbtree（紅黑樹）**：自平衡二叉搜尋樹，用於進程管理、記憶體管理等
- **VMA 樹**：虛擬記憶體區域的組織結構
- **Scheduler RB-tree**：用於管理可運行進程的優先級隊列
- **Augmented RB-trees**：帶額外信息的紅黑樹，提供區間查詢

## 時間和空間複雜度

- **DFS 遍歷**：
  - 時間複雜度：O(n)（訪問每個節點一次）
  - 空間複雜度：O(h)（h 是樹的高度，遞迴堆棧深度）

- **BFS 遍歷**：
  - 時間複雜度：O(n)
  - 空間複雜度：O(w)（w 是樹的最大寬度）

- **深度計算**：
  - 時間複雜度：O(n)
  - 空間複雜度：O(h)

- **BST 搜尋**：
  - 時間複雜度：O(log n)（平衡情況）或 O(n)（退化為鏈表）
  - 空間複雜度：O(h)

## 實踐建議 Practice Tips

1. 掌握三種 DFS 遍歷的遞迴實現
2. 理解遞迴終止條件和遞迴邏輯
3. 練習用顯式堆棧實現迭代遍歷
4. 理解 BST 的排序性質及其影響
5. 理解層級遍歷的隊列實現
6. 在紙上畫出樹的結構，然後執行遍歷
7. 認識何時應該用 DFS（深度相關）和何時應該用 BFS（層級相關）

## 常見錯誤 Common Mistakes

1. **遞迴終止條件錯誤**：導致空指針解參考或無限遞迴
2. **混淆遍歷順序**：前序、中序、後序的順序不對
3. **迭代遍歷中棧的順序錯誤**：特別是前序遍歷壓棧時的順序
4. **忘記檢查空子樹**：導致空指針異常
5. **複雜度分析錯誤**：遞迴調用次數計算不準確
6. **在修改樹時不小心**：可能導致樹結構破壞或內存洩漏
