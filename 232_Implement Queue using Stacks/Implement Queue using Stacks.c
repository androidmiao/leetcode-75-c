/*
 * LeetCode 232. 用棧實現隊列
 * 難度：Easy
 * 演算法：兩個棧，延遲轉移
 * 時間：Amortized O(1) 所有操作
 * 空間：O(n)
 */

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* inbox;   /* 用於push */
    int* outbox;  /* 用於pop/peek */
    int inbox_top;
    int outbox_top;
    int capacity;
} MyQueue;

MyQueue* myQueueCreate() {
    MyQueue* queue = (MyQueue*)malloc(sizeof(MyQueue));
    queue->capacity = 1000;
    queue->inbox = (int*)malloc(queue->capacity * sizeof(int));
    queue->outbox = (int*)malloc(queue->capacity * sizeof(int));
    queue->inbox_top = -1;
    queue->outbox_top = -1;
    return queue;
}

void myQueuePush(MyQueue* obj, int x) {
    obj->inbox[++obj->inbox_top] = x;
}

void transfer(MyQueue* obj) {
    /* 只有當outbox為空時才轉移 */
    if (obj->outbox_top == -1) {
        while (obj->inbox_top >= 0) {
            obj->outbox[++obj->outbox_top] = obj->inbox[obj->inbox_top--];
        }
    }
}

int myQueuePop(MyQueue* obj) {
    transfer(obj);
    return obj->outbox[obj->outbox_top--];
}

int myQueuePeek(MyQueue* obj) {
    transfer(obj);
    return obj->outbox[obj->outbox_top];
}

bool myQueueEmpty(MyQueue* obj) {
    return obj->inbox_top == -1 && obj->outbox_top == -1;
}

void myQueueFree(MyQueue* obj) {
    free(obj->inbox);
    free(obj->outbox);
    free(obj);
}
