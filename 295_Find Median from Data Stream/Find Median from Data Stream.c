/*
 * LeetCode 295. Find Median from Data Stream
 * 難度：Hard
 *
 * 演算法：兩個堆（Two Heaps）
 * 時間複雜度：addNum O(log n)，findMedian O(1)
 * 空間複雜度：O(n)
 *
 * ====================================================================
 * 核心思路
 * ====================================================================
 *
 * 維護兩個堆：
 *   max_heap（最大堆）：儲存較小的那一半數字，堆頂是這一半的最大值
 *   min_heap（最小堆）：儲存較大的那一半數字，堆頂是這一半的最小值
 *
 * 不變式（Invariant）：
 *   (1) max_heap 的所有元素 ≤ min_heap 的所有元素
 *   (2) max_heap.size == min_heap.size      （偶數個元素）
 *       或 max_heap.size == min_heap.size + 1 （奇數個元素）
 *
 * 中位數：
 *   偶數個 → (max_heap[0] + min_heap[0]) / 2.0
 *   奇數個 → max_heap[0]
 *
 * ====================================================================
 * 範例演示：依序加入 [5, 2, 8, 1, 4]
 * ====================================================================
 *
 * 步驟 1：addNum(5)
 *   5 <= max_heap[0]? max_heap 空，直接加入 max_heap
 *   max_heap: [5]     min_heap: []
 *   大小：1 vs 0（差 1，合法）
 *   中位數 = 5.0
 *
 *        max_heap        min_heap
 *        ┌─────┐
 *        │  5  │         (空)
 *        └─────┘
 *
 * 步驟 2：addNum(2)
 *   2 <= 5? 是 → 加入 max_heap
 *   max_heap: [5, 2]   min_heap: []
 *   大小：2 vs 0（差 2，違反！）→ 從 max_heap 移出堆頂 5 到 min_heap
 *   max_heap: [2]       min_heap: [5]
 *   中位數 = (2 + 5) / 2.0 = 3.5
 *
 *        max_heap        min_heap
 *        ┌─────┐        ┌─────┐
 *        │  2  │        │  5  │
 *        └─────┘        └─────┘
 *
 * 步驟 3：addNum(8)
 *   8 <= 2? 否 → 加入 min_heap
 *   max_heap: [2]       min_heap: [5, 8]
 *   大小：1 vs 2（min > max，違反！）→ 從 min_heap 移出堆頂 5 到 max_heap
 *   max_heap: [5, 2]    min_heap: [8]
 *   中位數 = 5.0
 *
 *        max_heap        min_heap
 *        ┌─────┐        ┌─────┐
 *        │  5  │        │  8  │
 *        ├─────┤        └─────┘
 *        │  2  │
 *        └─────┘
 *
 * 步驟 4：addNum(1)
 *   1 <= 5? 是 → 加入 max_heap
 *   max_heap: [5, 2, 1]  min_heap: [8]
 *   大小：3 vs 1（差 2，違反！）→ 移出 5 到 min_heap
 *   max_heap: [2, 1]     min_heap: [5, 8]
 *   中位數 = (2 + 5) / 2.0 = 3.5
 *
 *        max_heap        min_heap
 *        ┌─────┐        ┌─────┐
 *        │  2  │        │  5  │
 *        ├─────┤        ├─────┤
 *        │  1  │        │  8  │
 *        └─────┘        └─────┘
 *
 * 步驟 5：addNum(4)
 *   4 <= 2? 否 → 加入 min_heap
 *   max_heap: [2, 1]     min_heap: [4, 5, 8]
 *   大小：2 vs 3（min > max，違反！）→ 移出 4 到 max_heap
 *   max_heap: [4, 2, 1]  min_heap: [5, 8]
 *   中位數 = 4.0
 *
 *        max_heap        min_heap
 *        ┌─────┐        ┌─────┐
 *        │  4  │        │  5  │
 *        ├──┬──┤        ├─────┤
 *        │ 2│ 1│        │  8  │
 *        └──┴──┘        └─────┘
 *
 *   排序後完整序列：[1, 2, 4, 5, 8]，中位數 = 4 ✓
 *
 * ====================================================================
 * 堆的陣列表示法（Array-based Heap）
 * ====================================================================
 *
 *   對於索引 i 的節點：
 *     父節點：(i - 1) / 2
 *     左子節點：2 * i + 1
 *     右子節點：2 * i + 2
 *
 *        index:    0
 *                /   \
 *               1     2
 *              / \   / \
 *             3   4 5   6
 *
 *   heapifyUp：新元素加入陣列末尾，向上調整直到堆性質恢復
 *   heapifyDown：堆頂被替換後，向下調整直到堆性質恢復
 */

#include <stdlib.h>

/* MedianFinder 結構體：包含最大堆和最小堆 */
typedef struct {
    int* max_heap;      /* 最大堆陣列，儲存較小的一半數字 */
    int* min_heap;      /* 最小堆陣列，儲存較大的一半數字 */
    int max_size;       /* 最大堆目前的元素數量 */
    int min_size;       /* 最小堆目前的元素數量 */
    int max_capacity;   /* 最大堆陣列的容量（用於動態擴容） */
    int min_capacity;   /* 最小堆陣列的容量（用於動態擴容） */
} MedianFinder;

/* 交換兩個整數的值 */
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * 最大堆：向上調整（Sift Up）
 * 新元素插入陣列末尾後，不斷與父節點比較，
 * 若大於父節點就交換，直到堆頂或不再大於父節點為止。
 */
void maxHeapifyUp(int* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;  /* 計算父節點索引 */
        if (heap[parent] >= heap[index]) break;  /* 父節點已夠大，堆性質滿足 */
        swap(&heap[parent], &heap[index]);  /* 與父節點交換 */
        index = parent;  /* 繼續向上檢查 */
    }
}

/*
 * 最大堆：向下調整（Sift Down）
 * 堆頂被替換後，不斷與較大的子節點比較，
 * 若小於子節點就交換，直到葉節點或不再小於子節點為止。
 */
void maxHeapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {  /* 至少有左子節點 */
        int largest = index;
        int left = 2 * index + 1;   /* 左子節點索引 */
        int right = 2 * index + 2;  /* 右子節點索引 */

        /* 找出當前節點、左子、右子中的最大者 */
        if (left < size && heap[left] > heap[largest]) largest = left;
        if (right < size && heap[right] > heap[largest]) largest = right;

        if (largest == index) break;  /* 當前節點已是最大，堆性質滿足 */

        swap(&heap[index], &heap[largest]);  /* 與最大子節點交換 */
        index = largest;  /* 繼續向下檢查 */
    }
}

/*
 * 最小堆：向上調整（Sift Up）
 * 邏輯與最大堆對稱，但比較方向相反（小於父節點時交換）。
 */
void minHeapifyUp(int* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;  /* 計算父節點索引 */
        if (heap[parent] <= heap[index]) break;  /* 父節點已夠小，堆性質滿足 */
        swap(&heap[parent], &heap[index]);  /* 與父節點交換 */
        index = parent;  /* 繼續向上檢查 */
    }
}

/*
 * 最小堆：向下調整（Sift Down）
 * 邏輯與最大堆對稱，但比較方向相反（找最小子節點交換）。
 */
void minHeapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {  /* 至少有左子節點 */
        int smallest = index;
        int left = 2 * index + 1;   /* 左子節點索引 */
        int right = 2 * index + 2;  /* 右子節點索引 */

        /* 找出當前節點、左子、右子中的最小者 */
        if (left < size && heap[left] < heap[smallest]) smallest = left;
        if (right < size && heap[right] < heap[smallest]) smallest = right;

        if (smallest == index) break;  /* 當前節點已是最小，堆性質滿足 */

        swap(&heap[index], &heap[smallest]);  /* 與最小子節點交換 */
        index = smallest;  /* 繼續向下檢查 */
    }
}

/*
 * 建立 MedianFinder 物件
 * 初始容量為 1000，可動態擴容（realloc 倍增策略）。
 */
MedianFinder* medianFinderCreate() {
    MedianFinder* obj = (MedianFinder*)malloc(sizeof(MedianFinder));
    obj->max_capacity = 1000;  /* 最大堆初始容量 */
    obj->min_capacity = 1000;  /* 最小堆初始容量 */
    obj->max_heap = (int*)malloc(obj->max_capacity * sizeof(int));
    obj->min_heap = (int*)malloc(obj->min_capacity * sizeof(int));
    obj->max_size = 0;  /* 初始時兩個堆都是空的 */
    obj->min_size = 0;
    return obj;
}

/*
 * 加入新數字到資料結構中
 *
 * 策略：根據 num 與 max_heap 堆頂的大小關係決定放入哪個堆，
 *       然後檢查兩堆大小是否符合不變式，必要時重新平衡。
 *
 * 本實作採用「先分類再平衡」的方式：
 *   - num <= max_heap[0] → 加入 max_heap
 *   - num >  max_heap[0] → 加入 min_heap
 *   - 若 max_heap 比 min_heap 多超過 1 個元素 → 移出 max_heap 堆頂到 min_heap
 *   - 若 min_heap 比 max_heap 多 → 移出 min_heap 堆頂到 max_heap
 *
 * 注意：Editorial 的方法三採用「全部先加 lo，再彈到 hi，再視需要彈回 lo」，
 *       兩種方式等價，都能維護不變式。本實作的方式在平均情況下可能減少一次堆操作。
 */
void medianFinderAddNum(MedianFinder* obj, int num) {
    /* === 第一步：將 num 放入適當的堆 === */
    if (obj->max_size == 0 || num <= obj->max_heap[0]) {
        /* num 屬於較小的那一半 → 加入 max_heap */
        if (obj->max_size >= obj->max_capacity) {
            obj->max_capacity *= 2;  /* 容量不足時倍增 */
            obj->max_heap = (int*)realloc(obj->max_heap,
                                          obj->max_capacity * sizeof(int));
        }
        obj->max_heap[obj->max_size] = num;     /* 放到陣列末尾 */
        maxHeapifyUp(obj->max_heap, obj->max_size);  /* 向上調整恢復堆性質 */
        obj->max_size++;
    } else {
        /* num 屬於較大的那一半 → 加入 min_heap */
        if (obj->min_size >= obj->min_capacity) {
            obj->min_capacity *= 2;  /* 容量不足時倍增 */
            obj->min_heap = (int*)realloc(obj->min_heap,
                                          obj->min_capacity * sizeof(int));
        }
        obj->min_heap[obj->min_size] = num;     /* 放到陣列末尾 */
        minHeapifyUp(obj->min_heap, obj->min_size);  /* 向上調整恢復堆性質 */
        obj->min_size++;
    }

    /* === 第二步：平衡兩個堆的大小 === */

    /* 情況 A：max_heap 比 min_heap 多超過 1 個 → 移出 max_heap 堆頂到 min_heap */
    if (obj->max_size > obj->min_size + 1) {
        int top = obj->max_heap[0];  /* 取出 max_heap 的最大值 */

        /* 用 max_heap 的最後一個元素替換堆頂，然後向下調整 */
        obj->max_heap[0] = obj->max_heap[obj->max_size - 1];
        obj->max_size--;
        maxHeapifyDown(obj->max_heap, obj->max_size, 0);

        /* 將取出的元素加入 min_heap */
        obj->min_heap[obj->min_size] = top;
        minHeapifyUp(obj->min_heap, obj->min_size);
        obj->min_size++;
    }
    /* 情況 B：min_heap 比 max_heap 多 → 移出 min_heap 堆頂到 max_heap */
    else if (obj->min_size > obj->max_size) {
        int top = obj->min_heap[0];  /* 取出 min_heap 的最小值 */

        /* 用 min_heap 的最後一個元素替換堆頂，然後向下調整 */
        obj->min_heap[0] = obj->min_heap[obj->min_size - 1];
        obj->min_size--;
        minHeapifyDown(obj->min_heap, obj->min_size, 0);

        /* 將取出的元素加入 max_heap */
        obj->max_heap[obj->max_size] = top;
        maxHeapifyUp(obj->max_heap, obj->max_size);
        obj->max_size++;
    }
}

/*
 * 查找中位數
 *
 * 根據兩堆大小關係：
 *   相等 → 中位數是兩堆堆頂的平均值
 *   max_heap 多一個 → 中位數是 max_heap 的堆頂
 */
double medianFinderFindMedian(MedianFinder* obj) {
    if (obj->max_size == obj->min_size) {
        /* 偶數個元素：取兩堆堆頂的平均 */
        return (double)(obj->max_heap[0] + obj->min_heap[0]) / 2.0;
    } else {
        /* 奇數個元素：max_heap 堆頂即中位數 */
        return (double)obj->max_heap[0];
    }
}

/*
 * 釋放 MedianFinder 佔用的所有記憶體
 * 順序：先釋放兩個堆陣列，再釋放結構體本身。
 */
void medianFinderFree(MedianFinder* obj) {
    free(obj->max_heap);   /* 釋放最大堆陣列 */
    free(obj->min_heap);   /* 釋放最小堆陣列 */
    free(obj);             /* 釋放結構體 */
}
