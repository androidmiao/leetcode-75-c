/*
 * LeetCode 739. 每日溫度
 * 難度：Medium
 * 演算法：單調遞減棧
 * 時間：O(n)，空間：O(n)
 */

#include <stdlib.h>

int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    int* result = (int*)calloc(temperaturesSize, sizeof(int));
    int* stack = (int*)malloc(temperaturesSize * sizeof(int));
    int top = -1;

    for (int i = 0; i < temperaturesSize; i++) {
        while (top >= 0 && temperatures[stack[top]] < temperatures[i]) {
            int prev_index = stack[top--];
            result[prev_index] = i - prev_index;
        }
        stack[++top] = i;
    }

    free(stack);
    *returnSize = temperaturesSize;
    return result;
}
