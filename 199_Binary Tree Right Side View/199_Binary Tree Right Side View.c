//Binary Tree - BFS: https://leetcode.com/problems/binary-tree-right-side-view/description/?envType=study-plan-v2&envId=leetcode-75

/*
queue 用來做層序走訪，每次先算出當前層有幾個節點 levelSize，然後跑完這一層；
其中「這一層最後一個被處理的節點」就是右視圖要看到的值，所以在 i == levelSize - 1 時把 node->val 存進 result。
*/

/*
這題可以用 **BFS（Breadth-First Search，廣度優先搜尋）** 來做。

核心概念是：
- 一層一層走訪整棵樹
- 每一層最後一個節點，就是從右邊看過去會看到的節點
*/

/*
## 請詳細說明內容

這題不是要你真的「從右邊看」整棵樹，而是要你找出：

**每一層最右邊的那個節點**

例如這棵樹：

```text
        1
       / \
      2   3
       \   \
        5   4
```

從右邊看會看到：

```text
1, 3, 4
```

原因是：
- 第 1 層最右邊是 `1`
- 第 2 層最右邊是 `3`
- 第 3 層最右邊是 `4`

所以答案是：

```text
[1, 3, 4]
```

---

## 圖解流程

範例：

```text
root = [1,2,3,null,5,null,4]
```

轉成樹：

```text
        1
       / \
      2   3
       \   \
        5   4
```

---

### Step 1：初始化

一開始把根節點放進 queue：

```text
queue = [1]
result = []
front = 0
back = 1
```

---

### Step 2：處理第 1 層

目前 queue 裡只有一個節點，所以：

```text
levelSize = 1
```

處理這一層：

```text
取出 1
把 2 和 3 放進 queue
queue = [2, 3]
```

因為 `1` 是這一層最後一個節點，所以加入答案：

```text
result = [1]
```

---

### Step 3：處理第 2 層

現在 queue 中有：

```text
queue = [2, 3]
levelSize = 2
```

先處理 `2`：

```text
取出 2
2 的左邊是 null，不放
2 的右邊是 5，放進 queue
queue = [3, 5]
```

再處理 `3`：

```text
取出 3
3 的左邊是 null，不放
3 的右邊是 4，放進 queue
queue = [5, 4]
```

因為 `3` 是這一層最後一個節點，所以加入答案：

```text
result = [1, 3]
```

---

### Step 4：處理第 3 層

現在 queue 中有：

```text
queue = [5, 4]
levelSize = 2
```

先處理 `5`：

```text
取出 5
沒有子節點
queue = [4]
```

再處理 `4`：

```text
取出 4
沒有子節點
queue = []
```

因為 `4` 是這一層最後一個節點，所以加入答案：

```text
result = [1, 3, 4]
```

---

### Step 5：queue 為空，結束

最後回傳：

```text
[1, 3, 4]
```

---

## 為什麼 BFS 很適合這題？

因為 BFS 天生就是：

**一層一層處理**

而這題剛好就是要找：

**每一層最後一個節點**

所以 BFS 會很直觀。

---

## 程式關鍵觀念

### 1. `queue[back++] = root;`

這行是把根節點放進佇列，表示 BFS 從根節點開始。

---

### 2. `while (front < back)`

這表示：

只要 queue 還有節點沒處理，就繼續跑。

---

### 3. `int levelSize = back - front;`

這一行非常重要。

它的意思是：

**目前這一層總共有幾個節點**

因為 BFS 每次都先記住當前層的大小，才能確保 `for` 迴圈只處理這一層，不會把下一層也一起混進來。

---

### 4. `if (i == levelSize - 1)`

這表示目前節點是：

**這一層最後一個節點**

也就是從右邊看到的節點，所以把它存進 `result`。

---

### 5. 把左右子節點放進 queue

```c
if (node->left != NULL) {
    queue[back++] = node->left;
}

if (node->right != NULL) {
    queue[back++] = node->right;
}
```

這樣下一輪 while 就能處理下一層。

---

## 時間與空間複雜度
- 時間複雜度：`O(n)`
  - 每個節點只會被拜訪一次

- 空間複雜度：`O(n)`
  - queue 最壞情況可能要存很多節點

這題的重點就是：
**用 BFS 一層一層掃描，並把每一層最後一個節點記錄下來。**
*/

#include <stdlib.h>  // 提供 malloc、free

// 這題使用 BFS（層序走訪）
// 時間複雜度：O(n)，每個節點最多進出佇列一次
// 空間複雜度：O(n)，最壞情況下佇列可能存放一整層節點

// 二元樹節點定義
struct TreeNode {
    int val;                 // 目前節點的值
    struct TreeNode *left;   // 指向左子節點
    struct TreeNode *right;  // 指向右子節點
};

int* rightSideView(struct TreeNode* root, int* returnSize) {
    // 如果 returnSize 指標本身是空的，就無法把答案長度回傳出去
    if (returnSize == NULL) {
        return NULL;
    }

    // 先預設答案長度為 0，避免呼叫端拿到未初始化的值
    *returnSize = 0;

    // 如果整棵樹是空的，右視圖也一定是空陣列
    if (root == NULL) {
        return NULL;
    }

    // 題目限制節點總數最多 100，所以答案陣列與佇列都開 100 就夠了
    int capacity = 100;

    // result 用來儲存每一層最右邊看到的節點值
    int* result = (int*)malloc(sizeof(int) * capacity);

    // queue 用陣列模擬 BFS 佇列，存放待處理的節點指標
    struct TreeNode** queue = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * capacity);

    // front 指向目前佇列的開頭位置
    int front = 0;

    // back 指向下一個可以放入新節點的位置
    int back = 0;

    // 一開始先把根節點放進佇列
    queue[back++] = root;

    // 只要佇列還有節點，就持續一層一層做 BFS
    while (front < back) {
        // 目前這一層的節點數量 = 佇列尾端 - 佇列前端
        int levelSize = back - front;

        // 依序處理這一層的每個節點
        for (int i = 0; i < levelSize; i++) {
            // 取出佇列最前面的節點，front 再往後移一格
            struct TreeNode* node = queue[front++];

            // 如果這是本層最後一個節點，它就是從右邊看到的節點
            if (i == levelSize - 1) {
                // 把這個節點值放進答案陣列
                result[*returnSize] = node->val;

                // 答案長度加 1
                (*returnSize)++;
            }

            // 如果有左子節點，就放進佇列，留待下一層處理
            if (node->left != NULL) {
                queue[back++] = node->left;
            }

            // 如果有右子節點，也放進佇列，留待下一層處理
            if (node->right != NULL) {
                queue[back++] = node->right;
            }
        }
    }

    // queue 只是輔助 BFS 的暫存空間，用完後釋放
    free(queue);

    // 回傳存有右視圖結果的動態陣列
    return result;
}
