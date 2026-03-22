/*
 * LeetCode 200. 島嶼數量
 * 難度：Medium
 * 演算法：DFS/BFS，標記訪問過的單元格
 * 時間：O(m*n)，空間：O(m*n)遞迴棧或隊列
 */

#include <stdlib.h>

void dfs(char** grid, int row, int col, int rows, int cols) {
    if (row < 0 || row >= rows || col < 0 || col >= cols || grid[row][col] == '0') {
        return;
    }

    grid[row][col] = '0';  /* 標記為已訪問 */

    dfs(grid, row + 1, col, rows, cols);
    dfs(grid, row - 1, col, rows, cols);
    dfs(grid, row, col + 1, rows, cols);
    dfs(grid, row, col - 1, rows, cols);
}

int numIslands(char** grid, int gridSize, int* gridColSize) {
    if (gridSize == 0) return 0;

    int count = 0;
    int rows = gridSize;
    int cols = gridColSize[0];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == '1') {
                count++;
                dfs(grid, i, j, rows, cols);
            }
        }
    }

    return count;
}
