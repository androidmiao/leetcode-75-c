/*
 * LeetCode 207. 課程表
 * 難度：Medium
 * 演算法：DFS 循環檢測 (3色標記)
 * 時間：O(V+E)，空間：O(V)
 */

#include <stdlib.h>
#include <stdbool.h>

bool dfs(int course, int** adj, int* degrees, int* state) {
    /* 當前課程正在訪問 */
    state[course] = 1;

    /* 訪問所有先修課程 */
    for (int i = 0; i < degrees[course]; i++) {
        int prereq = adj[course][i];
        if (state[prereq] == 1) {
            /* 發現循環 */
            return false;
        }
        if (state[prereq] == 0) {
            /* 還未訪問，進行DFS */
            if (!dfs(prereq, adj, degrees, state)) {
                return false;
            }
        }
    }

    /* 課程訪問完成 */
    state[course] = 2;
    return true;
}

bool canFinish(int numCourses, int** prerequisites, int prerequisitesSize, int* prerequisitesColSize) {
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

    /* DFS 檢測循環 */
    int* state = (int*)malloc(numCourses * sizeof(int));
    for (int i = 0; i < numCourses; i++) {
        state[i] = 0;  /* 0: unvisited, 1: visiting, 2: visited */
    }

    bool has_cycle = false;

    for (int i = 0; i < numCourses && !has_cycle; i++) {
        if (state[i] == 0) {
            if (!dfs(i, adj, degrees, state)) {
                has_cycle = true;
            }
        }
    }

    /* 清理 */
    for (int i = 0; i < numCourses; i++) {
        free(adj[i]);
    }
    free(adj);
    free(degrees);
    free(state);

    return !has_cycle;
}
