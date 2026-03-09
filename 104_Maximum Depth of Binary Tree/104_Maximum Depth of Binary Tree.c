//Use DFS recursion:
/*
Explanation:
- If the node is `NULL`, depth is `0`.
- Recursively get the left subtree depth and right subtree depth.
- Return the larger one plus `1` for the current node.

Time complexity: `O(n)`
Space complexity: `O(h)` where `h` is the tree height
*/
// 使用 DFS 遞迴解法
// 觀念：一個節點的最大深度 = max(左子樹深度, 右子樹深度) + 1
// 時間複雜度：O(n)，每個節點都只會被拜訪一次
// 空間複雜度：O(h)，h 是樹高，主要來自遞迴呼叫堆疊

/*
畫「記憶體位址/陣列索引版」的圖
下面用這題的 Example 1 來畫：
root = [3,9,20,null,null,15,7]

這棵樹長這樣：

```text
        3
       / \
      9   20
         /  \
        15   7
```

記憶體位址版

假設每個節點在記憶體中的位址如下：

```text
root
 |
 v
+--------+       +--------+       +--------+       +--------+       +--------+
| 0x1000 | ----> | val=3  |       | 0x2000 | ----> | val=9  |       | 0x3000 |
|        |       | left --------->|        |       | left=NULL      | val=20 |
|        |       | right--------->|        |       | right=NULL     | left --+----> 0x4000
+--------+       +--------+       +--------+       +--------+       | right -+----> 0x5000
                                                                     +--------+

+--------+       +--------+       +--------+       +--------+
| 0x4000 | ----> | val=15 |       | 0x5000 | ----> | val=7  |
|        |       | left=NULL      |        |       | left=NULL
|        |       | right=NULL     |        |       | right=NULL
+--------+       +--------+       +--------+       +--------+
```

也可以寫成更精簡的指標關係：

```text
root = 0x1000

0x1000: { val=3,  left=0x2000, right=0x3000 }
0x2000: { val=9,  left=NULL,   right=NULL   }
0x3000: { val=20, left=0x4000, right=0x5000 }
0x4000: { val=15, left=NULL,   right=NULL   }
0x5000: { val=7,  left=NULL,   right=NULL   }
```

你的 C 程式在做的事就是：

```c
int leftDepth = maxDepth(root->left);
int rightDepth = maxDepth(root->right);
```

例如當 `root = 0x1000` 時：

```text
root->left  = 0x2000
root->right = 0x3000
```

所以會遞迴去算：

```text
maxDepth(0x2000)   // 節點 9
maxDepth(0x3000)   // 節點 20
```

陣列索引版

LeetCode 題目中的：

```text
[3,9,20,null,null,15,7]
```

是用陣列來表示樹。

索引對照：

```text
index:  0   1   2    3     4    5   6
value: [3,  9, 20, null, null, 15,  7]
```

對應關係：

```text
index 0 = 3        -> 根節點
index 1 = 9        -> 3 的左子節點
index 2 = 20       -> 3 的右子節點
index 3 = null     -> 9 的左子節點
index 4 = null     -> 9 的右子節點
index 5 = 15       -> 20 的左子節點
index 6 = 7        -> 20 的右子節點
```

圖示：

```text
            index0=3
           /        \
     index1=9      index2=20
      /    \        /      \
  idx3=NULL idx4=NULL idx5=15 idx6=7
```

程式遞迴流程圖

```text
maxDepth(3)
├─ maxDepth(9)
│  ├─ maxDepth(NULL) = 0
│  └─ maxDepth(NULL) = 0
│  => max(0, 0) + 1 = 1
└─ maxDepth(20)
   ├─ maxDepth(15)
   │  ├─ maxDepth(NULL) = 0
   │  └─ maxDepth(NULL) = 0
   │  => 1
   └─ maxDepth(7)
      ├─ maxDepth(NULL) = 0
      └─ maxDepth(NULL) = 0
      => 1
   => max(1, 1) + 1 = 2

=> max(1, 2) + 1 = 3
```

所以答案是：
最大深度 = 3
*/


//Definition for a binary tree node
// 二元樹節點的定義
struct TreeNode {
    int val;                // 節點儲存的值
    struct TreeNode *left;  // 指向左子節點
    struct TreeNode *right; // 指向右子節點
};

int maxDepth(struct TreeNode* root) {
    // 如果目前節點是 NULL，代表這裡沒有節點，深度為 0
    if (root == NULL) {
        return 0;
    }

    // 遞迴計算左子樹的最大深度
    int leftDepth = maxDepth(root->left);
    // 遞迴計算右子樹的最大深度
    int rightDepth = maxDepth(root->right);

    // 回傳左右子樹中較大的深度，再加上目前這一層
    return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
}

