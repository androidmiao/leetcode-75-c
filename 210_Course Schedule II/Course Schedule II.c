/*
 * LeetCode 210. 課程表 II
 * 難度：Medium
 * 演算法：拓撲排序 (DFS後序 或 Kahn)
 * 時間：O(V+E)，空間：O(V+E)
 */

#include <stdlib.h>

void dfs(int course, int** adj, int* degrees, int* state, int* result, int* result_index) {
    /* 當前課程正在訪問 */
    state[course] = 1;

    /* 訪問所有先修課程 */
    for (int i = 0; i < degrees[course]; i++) {
        int prereq = adj[course][i];
        if (state[prereq] == 0) {
            dfs(prereq, adj, degrees, state, result, result_index);
        }
    }

    /* 課程訪問完成，加入結果 */
    state[course] = 2;
    result[(*result_index)++] = course;
}

int* findOrder(int numCourses, int** prerequisites, int prerequisitesSize, int* prerequisitesColSize, int* returnSize) {
    /* 構建鄰接表 */
    int** adj = (int**)malloc(numCourses * sizeof(int*));
    int* degrees = (int*)malloc(numCourses * sizeof(int));

    for (int i = 0; i < numCourses; i++) {
        adj[i] = (int*)malloc(numCourses * sizeof(int));
        degrees[i] = 0;
    }

    for (int i = 0; i < prerequisitesSize; i++) {
        int course = prerequisites[i][0];
        int prereq = prerequisites[i][1];
        adj[course][degrees[course]++] = prereq;
    }

    /* DFS 拓撲排序 */
    int* state = (int*)malloc(numCourses * sizeof(int));
    for (int i = 0; i < numCourses; i++) {
        state[i] = 0;  /* 0: unvisited, 1: visiting, 2: visited */
    }

    int* result = (int*)malloc(numCourses * sizeof(int));
    int result_index = 0;
    int has_cycle = 0;

    for (int i = 0; i < numCourses; i++) {
        if (state[i] == 0) {
            dfs(i, adj, degrees, state, result, &result_index);
        }
    }

    /* 檢查是否存在循環（簡化檢測） */
    for (int i = 0; i < numCourses; i++) {
        if (state[i] != 2) {
            has_cycle = 1;
            break;
        }
    }

    /* 清理 */
    for (int i = 0; i < numCourses; i++) {
        free(adj[i]);
    }
    free(adj);
    free(degrees);
    free(state);

    if (has_cycle) {
        *returnSize = 0;
    } else {
        *returnSize = numCourses;
    }

    return result;
}
