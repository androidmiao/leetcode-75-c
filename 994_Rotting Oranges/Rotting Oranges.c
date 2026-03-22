/*
 * LeetCode 994. 腐爛的橙子
 * 難度：Medium
 * 演算法：多源BFS
 * 時間：O(m*n)，空間：O(m*n)
 */

#include <stdlib.h>
#include <string.h>

int orangesRotting(int** grid, int gridSize, int* gridColSize) {
    int rows = gridSize;
    int cols = gridColSize[0];
    int minutes = 0;
    int fresh_count = 0;

    int* queue = (int*)malloc(rows * cols * 2 * sizeof(int));
    int front = 0, rear = 0;

    /* 初始化：找出所有腐爛的橙子和新鮮橙子數量 */
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 2) {
                queue[rear++] = i;
                queue[rear++] = j;
            } else if (grid[i][j] == 1) {
                fresh_count++;
            }
        }
    }

    int directions[] = {-1, 0, 1, 0, 0, -1, 0, 1};

    while (fresh_count > 0 && front < rear) {
        int size = (rear - front) / 2;

        for (int i = 0; i < size; i++) {
            int r = queue[front++];
            int c = queue[front++];

            for (int d = 0; d < 8; d += 2) {
                int nr = r + directions[d];
                int nc = c + directions[d + 1];

                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc] == 1) {
                    grid[nr][nc] = 2;
                    fresh_count--;
                    queue[rear++] = nr;
                    queue[rear++] = nc;
                }
            }
        }

        if (fresh_count > 0) minutes++;
    }

    free(queue);
    return fresh_count == 0 ? minutes : -1;
}
