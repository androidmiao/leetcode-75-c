/*
 * ======================================================================
 * 題目編號: 133
 * 題目名稱: Clone Graph (圖的深拷貝)
 * 難度: Medium
 * 時間複雜度: O(N + E) - N個節點，E條邊，每個節點和邊訪問一次
 * 空間複雜度: O(N) - visited陣列和遞迴堆棧深度
 * ======================================================================
 *
 * 演算法步驟說明:
 * ===============
 * 1. 使用深度優先搜尋(DFS)遍歷原圖中的每個節點
 * 2. 維護一個 visited[101] 陣列作為雜湊映射，索引為節點值(1-100)
 *    - visited[val] 存儲該節點對應的克隆副本
 *    - 如果已存在克隆，直接返回(避免重複克隆和處理環)
 * 3. 對於未訪問的節點:
 *    a. 建立新的克隆節點
 *    b. 將克隆節點存入 visited[val]
 *    c. 遞迴地克隆所有鄰居節點
 *    d. 連接克隆節點的鄰居指針
 * 4. 返回克隆後的圖副本
 *
 * 具體範例:
 * =========
 * 輸入: adjList = [[2,4],[1,3],[2,4],[1,3]]
 * 原圖結構:
 *   Node 1 -> [Node 2, Node 4]
 *   Node 2 -> [Node 1, Node 3]
 *   Node 3 -> [Node 2, Node 4]
 *   Node 4 -> [Node 1, Node 3]
 *
 * DFS遍歷順序及克隆過程:
 * =====================
 *
 *   原圖                     克隆過程(DFS遞迴樹)
 *   ====                     =================
 *
 *    1 --- 2                  cloneGraph(1)
 *    |\ /|                      ├─ 建立 clone[1]
 *    | X |                      ├─ cloneGraph(2)
 *    |/ \|                      │   ├─ 建立 clone[2]
 *    4 --- 3                    │   ├─ cloneGraph(1) -> 返回 clone[1] (已存在)
 *                               │   └─ cloneGraph(3)
 *                               │       ├─ 建立 clone[3]
 *                               │       ├─ cloneGraph(2) -> 返回 clone[2] (已存在)
 *                               │       └─ cloneGraph(4)
 *                               │           ├─ 建立 clone[4]
 *                               │           ├─ cloneGraph(1) -> 返回 clone[1] (已存在)
 *                               │           └─ cloneGraph(3) -> 返回 clone[3] (已存在)
 *                               └─ cloneGraph(4) -> 返回 clone[4] (已存在)
 *
 * 克隆後的圖結構(與原圖完全相同):
 *   clone[1] -> [clone[2], clone[4]]
 *   clone[2] -> [clone[1], clone[3]]
 *   clone[3] -> [clone[2], clone[4]]
 *   clone[4] -> [clone[1], clone[3]]
 *
 * 關鍵優化:
 * ========
 * - visited陣列避免重複克隆(提高效率)
 * - visited陣列同時作為環檢測機制(防止無窮遞迴)
 * - 索引直接使用節點值，無需雜湊函數(O(1)查找)
 *
 * ======================================================================
 */

#include <stdlib.h>
#include <string.h>

/* 圖中的節點結構體定義 */
struct Node {
    int val;
    int numNeighbors;
    struct Node** neighbors;
};

/* 全局訪問陣列：索引為節點值(1-100)，值為該節點的克隆副本
   使用靜態陣列在遞迴調用中保持狀態 */
static struct Node* visited[101];

/* ===== DFS遞迴克隆函數 =====
 * 參數: node - 當前要克隆的原圖節點指針
 * 返回: 克隆後的節點指針
 * 機制: 如果節點已克隆則返回其副本；否則建立新副本並遞迴克隆鄰居 */
static struct Node* dfs(struct Node* node) {
    /* 邊界條件：空節點直接返回NULL */
    if (node == NULL) {
        return NULL;
    }

    /* 檢查該節點是否已被克隆(環檢測 + 去重)
       若已克隆過，直接返回其副本(避免重複建立和無窮遞迴) */
    if (visited[node->val] != NULL) {
        return visited[node->val];
    }

    /* 為當前節點建立克隆副本
       malloc分配新的Node結構體記憶體空間 */
    struct Node* clone = (struct Node*)malloc(sizeof(struct Node));

    /* 複製原節點的值 */
    clone->val = node->val;

    /* 複製原節點的鄰居數量 */
    clone->numNeighbors = node->numNeighbors;

    /* 為克隆節點的鄰居指針陣列分配記憶體
       大小為 numNeighbors 個指針 */
    clone->neighbors = (struct Node**)malloc(node->numNeighbors * sizeof(struct Node*));

    /* 將新克隆節點存入visited陣列，索引為其值
       此步驟標記該節點已被訪問(環檢測關鍵) */
    visited[node->val] = clone;

    /* 遞迴克隆原節點的所有鄰居
       迴圈遍歷原節點的每個鄰居 */
    for (int i = 0; i < node->numNeighbors; i++) {
        /* 遞迴調用dfs克隆第i個鄰居，將返回的克隆鄰居連接到克隆節點 */
        clone->neighbors[i] = dfs(node->neighbors[i]);
    }

    /* 返回克隆後的節點 */
    return clone;
}

/* ===== 主克隆函數(LeetCode入口) =====
 * 參數: s - 原圖的起始節點指針(1到100之間的節點)
 * 返回: 克隆後的整個圖(起始節點的副本) */
struct Node* cloneGraph(struct Node* s) {
    /* 初始化visited陣列：將所有位置設為NULL，表示尚未克隆任何節點 */
    memset(visited, 0, sizeof(visited));

    /* 使用DFS遞迴函數克隆整個圖，並返回克隆後的起始節點 */
    return dfs(s);
}
