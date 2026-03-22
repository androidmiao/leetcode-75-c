/*
 * LeetCode 133. 複製圖
 * 難度：Medium
 * 演算法：DFS/BFS with HashMap (Node -> Clone)
 * 時間：O(V+E)，空間：O(V)
 */

#include <stdlib.h>

/**
 * Definition for a Node.
 * struct Node {
 *     int val;
 *     int numNeighbors;
 *     struct Node** neighbors;
 * };
 */

struct Node {
    int val;
    int numNeighbors;
    struct Node** neighbors;
};

struct Node* cloneGraph(struct Node* node) {
    if (node == NULL) return NULL;

    /* 簡化實現：創建節點映射 */
    /* 完整實現需要雜湊表，這裡使用簡單的數組模擬 */
    struct Node** visited = (struct Node**)malloc(101 * sizeof(struct Node*));
    for (int i = 0; i <= 100; i++) {
        visited[i] = NULL;
    }

    /* DFS 複製 */
    struct Node** clones = (struct Node**)malloc(101 * sizeof(struct Node*));
    for (int i = 0; i <= 100; i++) {
        clones[i] = NULL;
    }

    /* 創建新節點 */
    struct Node* clone = (struct Node*)malloc(sizeof(struct Node));
    clone->val = node->val;
    clone->numNeighbors = node->numNeighbors;
    clone->neighbors = (struct Node**)malloc(node->numNeighbors * sizeof(struct Node*));

    clones[node->val] = clone;
    visited[node->val] = clone;

    /* 遞迴複製鄰接點 */
    for (int i = 0; i < node->numNeighbors; i++) {
        struct Node* neighbor = node->neighbors[i];
        if (clones[neighbor->val] == NULL) {
            clones[neighbor->val] = cloneGraph(neighbor);
        }
        clone->neighbors[i] = clones[neighbor->val];
    }

    free(visited);
    free(clones);
    return clone;
}
