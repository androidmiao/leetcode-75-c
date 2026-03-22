/*
 * LeetCode 84. 柱狀圖中最大的矩形
 * 難度：Hard
 * 演算法：單調遞增棧
 * 時間：O(n)，空間：O(n)
 */

#include <stdlib.h>

int largestRectangleArea(int* heights, int heightsSize) {
    int* stack = (int*)malloc((heightsSize + 2) * sizeof(int));
    int top = -1;
    int max_area = 0;

    /* 添加哨兵：開始和結束都是0 */
    int* new_heights = (int*)malloc((heightsSize + 2) * sizeof(int));
    new_heights[0] = 0;
    for (int i = 0; i < heightsSize; i++) {
        new_heights[i + 1] = heights[i];
    }
    new_heights[heightsSize + 1] = 0;
    int total = heightsSize + 2;

    for (int i = 0; i < total; i++) {
        while (top >= 0 && new_heights[stack[top]] > new_heights[i]) {
            int h_index = stack[top--];
            int h = new_heights[h_index];
            int w = (top >= 0) ? (i - stack[top] - 1) : 0;
            int area = h * w;
            if (area > max_area) {
                max_area = area;
            }
        }
        stack[++top] = i;
    }

    free(stack);
    free(new_heights);
    return max_area;
}
