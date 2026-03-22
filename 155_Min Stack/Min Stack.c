/*
 * LeetCode 155. 最小棧
 * 難度：Medium
 * 演算法：輔助最小棧
 * 所有操作：O(1)
 */

#include <stdlib.h>
#include <limits.h>

typedef struct {
    int* data;
    int* min_stack;
    int top;
    int capacity;
} MinStack;

MinStack* minStackCreate() {
    MinStack* obj = (MinStack*)malloc(sizeof(MinStack));
    obj->capacity = 1000;
    obj->data = (int*)malloc(obj->capacity * sizeof(int));
    obj->min_stack = (int*)malloc(obj->capacity * sizeof(int));
    obj->top = -1;
    return obj;
}

void minStackPush(MinStack* obj, int val) {
    obj->top++;
    if (obj->top >= obj->capacity) {
        obj->capacity *= 2;
        obj->data = (int*)realloc(obj->data, obj->capacity * sizeof(int));
        obj->min_stack = (int*)realloc(obj->min_stack, obj->capacity * sizeof(int));
    }

    obj->data[obj->top] = val;
    if (obj->top == 0) {
        obj->min_stack[obj->top] = val;
    } else {
        int curr_min = obj->min_stack[obj->top - 1];
        obj->min_stack[obj->top] = (val < curr_min) ? val : curr_min;
    }
}

void minStackPop(MinStack* obj) {
    obj->top--;
}

int minStackTop(MinStack* obj) {
    return obj->data[obj->top];
}

int minStackGetMin(MinStack* obj) {
    return obj->min_stack[obj->top];
}

void minStackFree(MinStack* obj) {
    free(obj->data);
    free(obj->min_stack);
    free(obj);
}
