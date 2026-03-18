//https://leetcode.com/problems/sliding-window-median/description/?envType=problem-list-v2&envId=wl94y6ih

#include <stdbool.h>
#include <stdlib.h>

// 題目連結：
// https://leetcode.com/problems/sliding-window-median/
//
// 解法：
// 使用「雙堆 + 延遲刪除（lazy deletion）」。
//
// 核心想法：
// 1. max-heap（small）保存較小的一半元素
// 2. min-heap（large）保存較大的一半元素
// 3. 讓 small 的有效元素數量，永遠等於 large，或只多 1 個
// 4. 這樣：
//    - k 為奇數時，中位數就是 small 的堆頂
//    - k 為偶數時，中位數就是 small 堆頂與 large 堆頂的平均
//
// 困難點：
// 滑動視窗往右移時，舊元素不一定剛好在堆頂，不能直接 O(log k) 刪除。
// 所以這裡採用「延遲刪除」：
// - 先把要刪除的值記錄到雜湊表 delayed 中
// - 只有當它真的跑到堆頂時，才實際把它從堆中彈出
//
// 時間複雜度：O(n log k)
// 空間複雜度：O(k)

/*
圖解 1：雙堆分工

把目前視窗中的元素分成兩半：

small(max-heap)                 large(min-heap)
存較小的一半                   存較大的一半

        堆頂最大                        堆頂最小
           |                               |
           v                               v
      +-----------+                  +-----------+
      |     3     |                  |     5     |
      +-----------+                  +-----------+
      |     1     |                  |     6     |
      +-----------+                  +-----------+
      |    -1     |                  |     7     |
      +-----------+                  +-----------+

規則：
1. small 的有效元素數量 = large，或只比 large 多 1
2. small 裡的值 <= large 裡的值

所以：
- k 是奇數時，中位數 = small 堆頂
- k 是偶數時，中位數 = (small 堆頂 + large 堆頂) / 2

------------------------------------------------------------
圖解 2：以 nums = [1,3,-1,-3,5,3,6,7], k = 3 為例
------------------------------------------------------------

Step 1: 建立第一個視窗 [1, 3, -1]

排序後：[-1, 1, 3]

small(max-heap)                 large(min-heap)
      +-----+                        +-----+
      |  1  |  <-- 中位數              |  3  |
      +-----+                        +-----+
      | -1  |
      +-----+

因為 k = 3 是奇數，所以中位數 = small 堆頂 = 1

------------------------------------------------------------
Step 2: 視窗右移到 [3, -1, -3]
------------------------------------------------------------

離開視窗：1
進入視窗：-3

排序後：[-3, -1, 3]

small(max-heap)                 large(min-heap)
      +-----+                        +-----+
      | -1  |  <-- 中位數              |  3  |
      +-----+                        +-----+
      | -3  |
      +-----+

中位數 = -1

------------------------------------------------------------
Step 3: 視窗右移到 [-1, -3, 5]
------------------------------------------------------------

離開視窗：3
進入視窗：5

排序後：[-3, -1, 5]

small(max-heap)                 large(min-heap)
      +-----+                        +-----+
      | -1  |  <-- 中位數              |  5  |
      +-----+                        +-----+
      | -3  |
      +-----+

中位數 = -1

------------------------------------------------------------
圖解 3：為什麼需要延遲刪除
------------------------------------------------------------

假設目前某一步：

small(max-heap)
      +-----+
      |  5  |
      +-----+
      |  3  |
      +-----+
      |  1  |   <-- 這個 1 已經滑出視窗，但它不在堆頂
      +-----+

如果要刪掉 1：
- 陣列型堆不能直接快速找到它的位置
- 若硬找，最壞要掃整個堆，會變慢

所以改成：

delayed[1] = 1

意思是：
"值 1 之後如果浮到堆頂，就把它真的刪掉一次"

等未來某次平衡或彈出操作後，1 跑到堆頂：

small(max-heap)
      +-----+
      |  1  |  <-- 發現 delayed[1] > 0
      +-----+

這時 pruneHeap() 會做：
1. delayed[1]--
2. heapPop()

這樣就避免每次刪除都去堆內部暴力搜尋。

------------------------------------------------------------
圖解 4：整體流程
------------------------------------------------------------

for 每一步滑動：

新值進來 nums[i]
      |
      v
dualHeapInsert()
      |
      v
舊值出去 nums[i-k]
      |
      v
dualHeapErase()
      |
      v
makeBalance()
      |
      v
pruneHeap()
      |
      v
取得中位數 dualHeapGetMedian()

一句話記憶：
small 管左半邊，large 管右半邊，
舊值先標記刪除，等到浮到堆頂再真正移除。
*/

// ------------------------------
// Heap 結構
// ------------------------------

typedef struct {
    // data 用來存堆中的值。
    int* data;
    // size 是目前堆中實際存了幾個元素。
    int size;
    // capacity 是堆陣列可容納的最大元素數。
    int capacity;
    // isMinHeap = true  代表這是一個 min-heap。
    // isMinHeap = false 代表這是一個 max-heap。
    bool isMinHeap;
} Heap;

// 建立一個指定容量與型態的堆。
static void heapInit(Heap* heap, int capacity, bool isMinHeap) {
    // 配置堆的底層陣列。
    heap->data = (int*)malloc(sizeof(int) * capacity);
    // 一開始堆是空的，所以 size = 0。
    heap->size = 0;
    // 記錄容量。
    heap->capacity = capacity;
    // 記錄這是 min-heap 還是 max-heap。
    heap->isMinHeap = isMinHeap;
}

// 釋放堆的底層陣列。
static void heapFree(Heap* heap) {
    // 釋放 data。
    free(heap->data);
    // 避免懸空指標。
    heap->data = NULL;
    // 重設 size。
    heap->size = 0;
    // 重設 capacity。
    heap->capacity = 0;
}

// 比較兩個值在堆中的優先順序。
static bool heapHigherPriority(const Heap* heap, int a, int b) {
    // 如果是 min-heap，值較小者優先。
    if (heap->isMinHeap) {
        return a < b;
    }

    // 如果是 max-heap，值較大者優先。
    return a > b;
}

// 交換兩個整數。
static void swapInt(int* a, int* b) {
    // 暫存 a 的值。
    int temp = *a;
    // 把 b 放到 a。
    *a = *b;
    // 把原本 a 的值放到 b。
    *b = temp;
}

// 確保堆的底層陣列容量足夠；若不足就自動擴容。
static void heapEnsureCapacity(Heap* heap) {
    // 若目前還有空位，就不需要擴容。
    if (heap->size < heap->capacity) {
        return;
    }

    // 容量採倍增策略，避免每次只增加一點點造成頻繁重配。
    int newCapacity = heap->capacity * 2;

    // 若原本容量異常為 0，至少補成 1。
    if (newCapacity == 0) {
        newCapacity = 1;
    }

    // 重新配置更大的陣列。
    int* newData = (int*)realloc(heap->data, sizeof(int) * newCapacity);

    // 更新指標與容量。
    heap->data = newData;
    heap->capacity = newCapacity;
}

// 把新值放進堆中。
static void heapPush(Heap* heap, int value) {
    // lazy deletion 可能讓堆中殘留很多失效元素，所以實際堆大小可能超過 k。
    // 因此插入前必須先確保底層陣列容量足夠。
    heapEnsureCapacity(heap);

    // 先把新值放到最後面。
    int index = heap->size;
    heap->data[index] = value;
    heap->size++;

    // 持續往上調整，直到符合堆性質。
    while (index > 0) {
        // 找父節點索引。
        int parent = (index - 1) / 2;

        // 如果目前節點不需要比父節點更前面，就停止。
        if (!heapHigherPriority(heap, heap->data[index], heap->data[parent])) {
            break;
        }

        // 否則就與父節點交換。
        swapInt(&heap->data[index], &heap->data[parent]);
        // 繼續往上看。
        index = parent;
    }
}

// 取出堆頂值，但不刪除。
static int heapTop(const Heap* heap) {
    // 堆頂永遠在索引 0。
    return heap->data[0];
}

// 刪除堆頂元素。
static void heapPop(Heap* heap) {
    // 如果堆是空的，直接返回。
    if (heap->size == 0) {
        return;
    }

    // 把最後一個元素搬到堆頂。
    heap->data[0] = heap->data[heap->size - 1];
    // size 減 1。
    heap->size--;

    // 從堆頂開始往下調整。
    int index = 0;

    while (true) {
        // 左子節點索引。
        int left = index * 2 + 1;
        // 右子節點索引。
        int right = index * 2 + 2;
        // 先假設目前 index 就是最適合的位置。
        int best = index;

        // 如果左子節點存在，且優先順序更高，就更新 best。
        if (left < heap->size &&
            heapHigherPriority(heap, heap->data[left], heap->data[best])) {
            best = left;
        }

        // 如果右子節點存在，且優先順序更高，就更新 best。
        if (right < heap->size &&
            heapHigherPriority(heap, heap->data[right], heap->data[best])) {
            best = right;
        }

        // 如果 best 沒變，表示已恢復堆性質。
        if (best == index) {
            break;
        }

        // 否則就把目前節點和較佳子節點交換。
        swapInt(&heap->data[index], &heap->data[best]);
        // 繼續往下看。
        index = best;
    }
}

// ------------------------------
// HashMap：記錄延遲刪除次數
// ------------------------------

typedef struct {
    // key 是數值本身。
    int key;
    // count 是這個數值還有幾次待刪除。
    int count;
    // used = 0 代表這格從未使用。
    // used = 1 代表這格已經使用過。
    char used;
} HashEntry;

typedef struct {
    // entries 是 open addressing 的雜湊表陣列。
    HashEntry* entries;
    // capacity 是表格大小。
    int capacity;
} HashMap;

// 取不小於 x 的最小 2 的次方，方便做位元遮罩。
static int nextPowerOfTwo(int x) {
    // 從 1 開始倍增。
    int power = 1;

    // 持續倍增到 >= x。
    while (power < x) {
        power <<= 1;
    }

    // 回傳結果。
    return power;
}

// 對 int 做簡單混合，讓分布更平均。
static unsigned int hashInt(int key) {
    // 先轉成 unsigned，避免負數位元操作的問題。
    unsigned int x = (unsigned int)key;
    // 一些常見的 bit mixing。
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    // 回傳 hash 值。
    return x;
}

// 初始化雜湊表。
static void hashMapInit(HashMap* map, int expectedSize) {
    // 容量抓大一點，降低碰撞。
    int capacity = nextPowerOfTwo(expectedSize * 4 + 1);

    // 配置 entries 陣列，calloc 會自動清成 0。
    map->entries = (HashEntry*)calloc((size_t)capacity, sizeof(HashEntry));
    // 記錄容量。
    map->capacity = capacity;
}

// 釋放雜湊表。
static void hashMapFree(HashMap* map) {
    // 釋放 entries。
    free(map->entries);
    // 避免懸空指標。
    map->entries = NULL;
    // 重設容量。
    map->capacity = 0;
}

// 找到 key 所在的位置；若不存在則找到可插入的位置。
static int hashMapFindIndex(const HashMap* map, int key) {
    // 用 capacity - 1 當遮罩，前提是 capacity 為 2 的次方。
    int mask = map->capacity - 1;
    // 初始索引。
    int index = (int)(hashInt(key) & (unsigned int)mask);

    // 線性探測。
    while (map->entries[index].used && map->entries[index].key != key) {
        index = (index + 1) & mask;
    }

    // 回傳找到的位置。
    return index;
}

// 取得某個 key 的計數；若不存在則回傳 0。
static int hashMapGet(const HashMap* map, int key) {
    // 找對應位置。
    int index = hashMapFindIndex(map, key);

    // 若該格未使用，代表不存在。
    if (!map->entries[index].used) {
        return 0;
    }

    // 回傳 count。
    return map->entries[index].count;
}

// 把某個 key 的計數增加 delta。
static void hashMapAdd(HashMap* map, int key, int delta) {
    // 找對應位置。
    int index = hashMapFindIndex(map, key);

    // 如果原本沒用過，先建立新項目。
    if (!map->entries[index].used) {
        map->entries[index].used = 1;
        map->entries[index].key = key;
        map->entries[index].count = 0;
    }

    // 更新計數。
    map->entries[index].count += delta;
}

// ------------------------------
// DualHeap：這題的主結構
// ------------------------------

typedef struct {
    // small 是 max-heap，存較小的一半。
    Heap small;
    // large 是 min-heap，存較大的一半。
    Heap large;
    // delayed 用來記錄「待刪除值 -> 次數」。
    HashMap delayed;
    // smallSize 是 small 的「有效元素數量」。
    int smallSize;
    // largeSize 是 large 的「有效元素數量」。
    int largeSize;
    // k 是視窗大小。
    int k;
} DualHeap;

// 把堆頂那些已標記延遲刪除的值真的彈掉。
static void pruneHeap(Heap* heap, HashMap* delayed) {
    // 只要堆不為空，就持續檢查堆頂。
    while (heap->size > 0) {
        // 取目前堆頂值。
        int value = heapTop(heap);
        // 查詢它是否被標記為待刪除。
        int delayedCount = hashMapGet(delayed, value);

        // 如果沒有待刪除，就停止。
        if (delayedCount == 0) {
            break;
        }

        // 把這次待刪除扣掉 1。
        hashMapAdd(delayed, value, -1);
        // 真的把堆頂彈出。
        heapPop(heap);
    }
}

// 維持兩個堆的有效大小平衡。
static void makeBalance(DualHeap* dh) {
    // 如果 small 比 large 多超過 1 個，就把 small 堆頂搬到 large。
    if (dh->smallSize > dh->largeSize + 1) {
        // 先把 small 堆頂放進 large。
        heapPush(&dh->large, heapTop(&dh->small));
        // 再把 small 堆頂彈出。
        heapPop(&dh->small);
        // 更新有效大小。
        dh->smallSize--;
        dh->largeSize++;
        // small 的新堆頂可能剛好是延遲刪除元素，所以順手清掉。
        pruneHeap(&dh->small, &dh->delayed);
    }
    // 如果 small 比 large 還少，代表要從 large 搬一個回來。
    else if (dh->smallSize < dh->largeSize) {
        // 把 large 堆頂放進 small。
        heapPush(&dh->small, heapTop(&dh->large));
        // 再把 large 堆頂彈出。
        heapPop(&dh->large);
        // 更新有效大小。
        dh->smallSize++;
        dh->largeSize--;
        // large 的新堆頂可能是延遲刪除元素，所以順手清掉。
        pruneHeap(&dh->large, &dh->delayed);
    }
}

// 初始化 DualHeap。
static void dualHeapInit(DualHeap* dh, int k) {
    // 一開始先抓 k + 1 作為初始容量。
    // 注意：因為 lazy deletion 可能讓堆內暫留失效元素，
    // 所以實際堆大小有機會超過 k，後續會靠自動擴容處理。
    heapInit(&dh->small, k + 1, false);
    heapInit(&dh->large, k + 1, true);
    // delayed 最多只需要處理視窗中的不同值，抓 4k 左右很夠。
    hashMapInit(&dh->delayed, k + 1);
    // 一開始兩邊都沒有有效元素。
    dh->smallSize = 0;
    dh->largeSize = 0;
    // 記錄視窗大小。
    dh->k = k;
}

// 釋放 DualHeap。
static void dualHeapFree(DualHeap* dh) {
    // 釋放兩個堆。
    heapFree(&dh->small);
    heapFree(&dh->large);
    // 釋放 delayed hash map。
    hashMapFree(&dh->delayed);
}

// 插入一個新值。
static void dualHeapInsert(DualHeap* dh, int num) {
    // 如果 small 為空，或 num 應該屬於較小的一半，就放進 small。
    if (dh->smallSize == 0 || num <= heapTop(&dh->small)) {
        heapPush(&dh->small, num);
        dh->smallSize++;
    } else {
        // 否則放進 large。
        heapPush(&dh->large, num);
        dh->largeSize++;
    }

    // 插入後重新平衡。
    makeBalance(dh);
}

// 移除一個舊值，使用延遲刪除。
static void dualHeapErase(DualHeap* dh, int num) {
    // 先把這個值記錄成「待刪除」。
    hashMapAdd(&dh->delayed, num, 1);

    // num 若小於等於 small 的堆頂，代表它應該屬於 small 這一側的有效集合。
    if (num <= heapTop(&dh->small)) {
        dh->smallSize--;

        // 如果它剛好就在 small 堆頂，就可以立刻清理。
        if (num == heapTop(&dh->small)) {
            pruneHeap(&dh->small, &dh->delayed);
        }
    } else {
        // 否則它屬於 large 這一側。
        dh->largeSize--;

        // 如果它剛好就在 large 堆頂，就可以立刻清理。
        if (dh->large.size > 0 && num == heapTop(&dh->large)) {
            pruneHeap(&dh->large, &dh->delayed);
        }
    }

    // 刪除後重新平衡。
    makeBalance(dh);
}

// 取得目前視窗的中位數。
static double dualHeapGetMedian(DualHeap* dh) {
    // 先保證兩邊堆頂都不是延遲刪除元素。
    pruneHeap(&dh->small, &dh->delayed);
    pruneHeap(&dh->large, &dh->delayed);

    // 如果 k 是奇數，中位數就是 small 堆頂。
    if ((dh->k & 1) == 1) {
        return (double)heapTop(&dh->small);
    }

    // 如果 k 是偶數，中位數是兩個堆頂平均。
    return ((double)((long long)heapTop(&dh->small) +
                     (long long)heapTop(&dh->large))) / 2.0;
}

// LeetCode 需要的主函式。
double* medianSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    // 總共有 numsSize - k + 1 個視窗。
    *returnSize = numsSize - k + 1;

    // 配置答案陣列。
    double* result = (double*)malloc(sizeof(double) * (*returnSize));

    // 建立雙堆。
    DualHeap dh;
    dualHeapInit(&dh, k);

    // 先把前 k 個元素放進雙堆，形成第一個完整視窗。
    for (int i = 0; i < k; i++) {
        dualHeapInsert(&dh, nums[i]);
    }

    // 第一個視窗的中位數。
    result[0] = dualHeapGetMedian(&dh);

    // 從第 k 個元素開始，讓視窗一路往右滑。
    for (int i = k; i < numsSize; i++) {
        // 新元素進入視窗。
        dualHeapInsert(&dh, nums[i]);
        // 舊元素離開視窗。
        dualHeapErase(&dh, nums[i - k]);
        // 記錄目前視窗的中位數。
        result[i - k + 1] = dualHeapGetMedian(&dh);
    }

    // 釋放雙堆資源。
    dualHeapFree(&dh);

    // 回傳答案陣列。
    return result;
}
