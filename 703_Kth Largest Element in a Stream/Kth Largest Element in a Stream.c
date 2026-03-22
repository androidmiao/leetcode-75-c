/*
 * LeetCode 703. 流中第K大的元素
 * 難度：Easy
 *
 * 問題描述：
 * 設計一個類，找流中第k個最大的元素。
 *
 * 演算法：最小堆 (Min-Heap)
 *
 * 核心思想：
 * - 維護一個大小為 k 的最小堆
 * - 堆的頂部元素是第k個最大元素
 * - 添加新元素時：
 *   如果堆大小 < k，直接加入
 *   否則，如果新元素 > 堆頂，刪除堆頂並加入新元素
 *
 * 時間複雜度：
 * - 構造函數：O(n log k)
 * - add：O(log k)
 *
 * 空間複雜度：O(k)
 */

#include <stdlib.h>

typedef struct {
    int* heap;
    int size;
    int capacity;
    int k;
} KthLargest;

/* 交換兩個元素 */
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* 向上調整堆(最小堆) */
void heapifyUp(int* heap, int index) {
    while (index > 0 && heap[(index - 1) / 2] > heap[index]) {
        swap(&heap[index], &heap[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

/* 向下調整堆(最小堆) */
void heapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left] < heap[smallest]) {
            smallest = left;
        }
        if (right < size && heap[right] < heap[smallest]) {
            smallest = right;
        }

        if (smallest == index) break;

        swap(&heap[index], &heap[smallest]);
        index = smallest;
    }
}

KthLargest* kthLargestCreate(int k, int* nums, int numsSize) {
    KthLargest* obj = (KthLargest*)malloc(sizeof(KthLargest));
    obj->k = k;
    obj->capacity = k + 1;
    obj->heap = (int*)malloc(obj->capacity * sizeof(int));
    obj->size = 0;

    /* 添加初始數字 */
    for (int i = 0; i < numsSize; i++) {
        if (obj->size < k) {
            obj->heap[obj->size] = nums[i];
            heapifyUp(obj->heap, obj->size);
            obj->size++;
        } else if (nums[i] > obj->heap[0]) {
            obj->heap[0] = nums[i];
            heapifyDown(obj->heap, obj->size, 0);
        }
    }

    return obj;
}

int kthLargestAdd(KthLargest* obj, int val) {
    if (obj->size < obj->k) {
        obj->heap[obj->size] = val;
        heapifyUp(obj->heap, obj->size);
        obj->size++;
    } else if (val > obj->heap[0]) {
        obj->heap[0] = val;
        heapifyDown(obj->heap, obj->size, 0);
    }

    return obj->heap[0];
}

void kthLargestFree(KthLargest* obj) {
    free(obj->heap);
    free(obj);
}
