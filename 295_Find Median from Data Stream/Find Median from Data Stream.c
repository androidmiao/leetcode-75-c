/*
 * LeetCode 295. 求資料流的中位數
 * 難度：Hard
 *
 * 兩個堆方法：最大堆(下半部分) + 最小堆(上半部分)
 * 維護平衡：max_heap.size == min_heap.size 或 max_heap.size == min_heap.size + 1
 *
 * 時間複雜度：O(log n)
 * 空間複雜度：O(n)
 */

#include <stdlib.h>

typedef struct {
    int* max_heap;      /* 最大堆，存儲較小的一半數字 */
    int* min_heap;      /* 最小堆，存儲較大的一半數字 */
    int max_size;
    int min_size;
    int max_capacity;
    int min_capacity;
} MedianFinder;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* 最大堆：向上調整 */
void maxHeapifyUp(int* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[parent] >= heap[index]) break;
        swap(&heap[parent], &heap[index]);
        index = parent;
    }
}

/* 最大堆：向下調整 */
void maxHeapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left] > heap[largest]) largest = left;
        if (right < size && heap[right] > heap[largest]) largest = right;

        if (largest == index) break;

        swap(&heap[index], &heap[largest]);
        index = largest;
    }
}

/* 最小堆：向上調整 */
void minHeapifyUp(int* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[parent] <= heap[index]) break;
        swap(&heap[parent], &heap[index]);
        index = parent;
    }
}

/* 最小堆：向下調整 */
void minHeapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left] < heap[smallest]) smallest = left;
        if (right < size && heap[right] < heap[smallest]) smallest = right;

        if (smallest == index) break;

        swap(&heap[index], &heap[smallest]);
        index = smallest;
    }
}

MedianFinder* medianFinderCreate() {
    MedianFinder* obj = (MedianFinder*)malloc(sizeof(MedianFinder));
    obj->max_capacity = 1000;
    obj->min_capacity = 1000;
    obj->max_heap = (int*)malloc(obj->max_capacity * sizeof(int));
    obj->min_heap = (int*)malloc(obj->min_capacity * sizeof(int));
    obj->max_size = 0;
    obj->min_size = 0;
    return obj;
}

void medianFinderAddNum(MedianFinder* obj, int num) {
    /* 添加到最大堆或最小堆 */
    if (obj->max_size == 0 || num <= obj->max_heap[0]) {
        /* 添加到最大堆 */
        if (obj->max_size >= obj->max_capacity) {
            obj->max_capacity *= 2;
            obj->max_heap = (int*)realloc(obj->max_heap, obj->max_capacity * sizeof(int));
        }
        obj->max_heap[obj->max_size] = num;
        maxHeapifyUp(obj->max_heap, obj->max_size);
        obj->max_size++;
    } else {
        /* 添加到最小堆 */
        if (obj->min_size >= obj->min_capacity) {
            obj->min_capacity *= 2;
            obj->min_heap = (int*)realloc(obj->min_heap, obj->min_capacity * sizeof(int));
        }
        obj->min_heap[obj->min_size] = num;
        minHeapifyUp(obj->min_heap, obj->min_size);
        obj->min_size++;
    }

    /* 平衡兩個堆 */
    if (obj->max_size > obj->min_size + 1) {
        int top = obj->max_heap[0];
        obj->max_heap[0] = obj->max_heap[obj->max_size - 1];
        obj->max_size--;
        maxHeapifyDown(obj->max_heap, obj->max_size, 0);

        obj->min_heap[obj->min_size] = top;
        minHeapifyUp(obj->min_heap, obj->min_size);
        obj->min_size++;
    } else if (obj->min_size > obj->max_size) {
        int top = obj->min_heap[0];
        obj->min_heap[0] = obj->min_heap[obj->min_size - 1];
        obj->min_size--;
        minHeapifyDown(obj->min_heap, obj->min_size, 0);

        obj->max_heap[obj->max_size] = top;
        maxHeapifyUp(obj->max_heap, obj->max_size);
        obj->max_size++;
    }
}

double medianFinderFindMedian(MedianFinder* obj) {
    if (obj->max_size == obj->min_size) {
        return (double)(obj->max_heap[0] + obj->min_heap[0]) / 2.0;
    } else {
        return (double)obj->max_heap[0];
    }
}

void medianFinderFree(MedianFinder* obj) {
    free(obj->max_heap);
    free(obj->min_heap);
    free(obj);
}
