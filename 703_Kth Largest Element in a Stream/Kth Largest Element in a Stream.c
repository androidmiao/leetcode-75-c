/*
 * ================================================================
 * LeetCode 703. 流中第K大的元素 (Kth Largest Element in a Stream)
 * 難度：Easy
 * ================================================================
 *
 * 時間複雜度：
 *   - kthLargestCreate: O(n log k)  [n個元素，每次插入O(log k)]
 *   - kthLargestAdd: O(log k)        [最多進行一次heapifyDown]
 *
 * 空間複雜度：O(k)                   [最小堆容量為k]
 *
 * ================================================================
 * 演算法概述 (Algorithm Walkthrough)
 * ================================================================
 *
 * 核心思想：使用最小堆維護流中最大的k個元素。
 * - 堆的大小恆定為k
 * - 堆頂(根節點)始終是第k個最大元素
 * - 新元素只有在大於堆頂時才被加入
 *
 * 步驟分解：
 * 1. 初始化：創建大小為k的最小堆，堆開始為空
 * 2. 填充階段：依序將nums陣列元素加入
 *    - 當堆未滿(size < k)：直接插入新元素，向上調整保持堆性質
 *    - 當堆滿(size == k)且新元素>堆頂：替換堆頂，向下調整
 *    - 否則忽略新元素(因為它不在前k大中)
 * 3. 查詢：堆頂元素始終是第k個最大值
 *
 * ================================================================
 * 具體範例 (Example: k=3, nums=[4,5,8,2])
 * ================================================================
 *
 * 初始化階段：
 * ─────────────────────────────────────────
 * 步驟1：插入4 (size=0 < k=3)
 *   heap = [4]，size=1
 *   樹狀：
 *       4
 *
 * 步驟2：插入5 (size=1 < k=3)
 *   heap = [4,5]，size=2
 *   樹狀(最小堆性質)：
 *       4
 *      /
 *     5
 *
 * 步驟3：插入8 (size=2 < k=3)
 *   heap = [4,5,8]，size=3
 *   樹狀(最小堆性質)：
 *       4         (堆頂=最小值=4，即第3大)
 *      / \
 *     5   8
 *
 * add(3)：
 * ─────────────────────────────────────────
 *   檢查：3 < 4 (堆頂) → 忽略，不加入
 *   heap = [4,5,8]，返回 4
 *
 * add(5)：
 * ─────────────────────────────────────────
 *   檢查：5 > 4 (堆頂) → 替換堆頂
 *   heap = [5,5,8]
 *   向下調整(heapifyDown)：
 *     比較5與兩個子節點5,8 → 5不變(無更小值)
 *   heap = [5,5,8]，返回 5
 *
 * add(10)：
 * ─────────────────────────────────────────
 *   檢查：10 > 5 (堆頂) → 替換堆頂
 *   heap = [10,5,8]
 *   向下調整：
 *     比較10與子節點5,8 → 5最小
 *     交換：heap = [5,10,8]
 *     比較10(現在在索引1) 無子節點(2*1+1=3>=size) → 調整完成
 *   最終heap = [5,10,8]，返回 5
 *   樹狀：
 *       5
 *      / \
 *    10   8
 *
 * add(9)：
 * ─────────────────────────────────────────
 *   檢查：9 > 5 (堆頂) → 替換堆頂
 *   heap = [9,10,8]
 *   向下調整：
 *     比較9與子節點10,8 → 8最小
 *     交換：heap = [8,10,9]
 *     比較9(現在在索引2) 無子節點 → 調整完成
 *   最終heap = [8,10,9]，返回 8
 *   樹狀：
 *       8
 *      / \
 *    10   9
 *
 * add(4)：
 * ─────────────────────────────────────────
 *   檢查：4 < 8 (堆頂) → 忽略
 *   heap = [8,10,9]，返回 8
 *
 * ================================================================
 * 數據結構：最小堆 (Min-Heap)
 * ================================================================
 * 最小堆性質：父節點 ≤ 所有子節點
 *
 * 陣列索引對應：
 *   - 節點i的父親：(i-1)/2
 *   - 節點i的左子：2*i+1
 *   - 節點i的右子：2*i+2
 *
 * 維護堆性質的操作：
 *   - heapifyUp：新元素插入末尾後，與父節點比較並向上冒泡
 *   - heapifyDown：元素替換後，與子節點比較並向下沉降
 *
 * ================================================================
 */

#include <stdlib.h>

typedef struct {
    int* heap;      // 最小堆陣列指標
    int size;       // 當前堆中元素個數
    int capacity;   // 堆的容量(初始化為k+1)
    int k;          // 目標：維護前k大
} KthLargest;

/*
 * swap - 交換兩個整數的值
 * @a: 指向第一個整數的指標
 * @b: 指向第二個整數的指標
 *
 * 通過指標交換，用於heapify操作中的元素交換
 */
void swap(int* a, int* b) {
    int temp = *a;       // temp = a的值
    *a = *b;             // a = b的值
    *b = temp;           // b = a的原值
}

/*
 * heapifyUp - 向上調整最小堆(插入後)
 * @heap: 堆陣列
 * @index: 待調整元素的索引
 *
 * 新元素插入末尾後，如果小於父節點，與父節點交換並繼續向上
 * 保證最小堆性質(父≤子)
 *
 * 不變量：除了沿著index向上的路徑外，堆性質始終滿足
 */
void heapifyUp(int* heap, int index) {
    while (index > 0 && heap[(index - 1) / 2] > heap[index]) {
        // 當index>0表示還有父節點
        // 當父節點>當前節點時，違反最小堆性質
        swap(&heap[index], &heap[(index - 1) / 2]);  // 交換當前與父
        index = (index - 1) / 2;  // 向上移動到父節點的位置
    }
    // 循環終止時：index==0(到達根)或父≤子(堆性質恢復)
}

/*
 * heapifyDown - 向下調整最小堆(替換根節點後)
 * @heap: 堆陣列
 * @size: 堆的當前大小
 * @index: 待調整元素的索引(通常為0,即根節點)
 *
 * 替換根節點後，向下比較子節點，與較小子交換
 * 保證最小堆性質(父≤子)
 *
 * 不變量：除了沿著index向下的路徑外，堆性質始終滿足
 */
void heapifyDown(int* heap, int size, int index) {
    while (2 * index + 1 < size) {
        // 2*index+1 < size表示至少有左子節點存在
        int smallest = index;          // 假設當前節點最小
        int left = 2 * index + 1;      // 左子節點索引
        int right = 2 * index + 2;     // 右子節點索引(可能不存在)

        if (left < size && heap[left] < heap[smallest]) {
            // 左子存在且小於當前最小值，更新最小值位置
            smallest = left;
        }
        if (right < size && heap[right] < heap[smallest]) {
            // 右子存在且小於當前最小值，更新最小值位置
            smallest = right;
        }

        if (smallest == index) {
            // 當前節點已是三者中最小，堆性質恢復，退出
            break;
        }

        swap(&heap[index], &heap[smallest]);  // 交換當前與較小子
        index = smallest;  // 向下移動到較小子的位置，繼續調整
    }
    // 循環終止時：無左子節點或堆性質恢復
}

/*
 * kthLargestCreate - 初始化KthLargest物件
 * @k: 目標位置(求第k大)
 * @nums: 初始數字陣列
 * @numsSize: 陣列大小
 *
 * 返回：指向新建KthLargest物件的指標
 *
 * 策略：遍歷nums，維護一個k元素的最小堆
 * - 堆未滿時：直接插入
 * - 堆滿時：只保留>堆頂的元素
 */
KthLargest* kthLargestCreate(int k, int* nums, int numsSize) {
    KthLargest* obj = (KthLargest*)malloc(sizeof(KthLargest));  // 分配物件記憶體
    obj->k = k;                      // 記錄k值
    obj->capacity = k + 1;           // 容量設為k+1(預留)
    obj->heap = (int*)malloc(obj->capacity * sizeof(int));  // 分配堆陣列
    obj->size = 0;                   // 初始堆為空

    /* 迭代添加初始數字 */
    for (int i = 0; i < numsSize; i++) {
        if (obj->size < k) {
            // 當堆未滿：無條件加入
            obj->heap[obj->size] = nums[i];  // 插入末尾
            heapifyUp(obj->heap, obj->size);  // 向上調整維持堆性質
            obj->size++;  // 堆大小+1
        } else if (nums[i] > obj->heap[0]) {
            // 堆已滿且新元素>堆頂：刪除堆頂，加入新元素
            // 邏輯：只有>第k大的才可能進入前k大集合
            obj->heap[0] = nums[i];  // 替換堆頂(最小值)
            heapifyDown(obj->heap, obj->size, 0);  // 向下調整
        }
        // 否則忽略nums[i]：小於等於堆頂，不在前k大中
    }

    return obj;  // 返回初始化完成的物件
}

/*
 * kthLargestAdd - 向流中添加新元素，返回當前第k大
 * @obj: KthLargest物件指標
 * @val: 新增元素
 *
 * 返回：添加後的第k個最大元素(堆頂)
 *
 * 邏輯同kthLargestCreate，但每次只添加一個元素
 */
int kthLargestAdd(KthLargest* obj, int val) {
    if (obj->size < obj->k) {
        // 堆未滿：無條件加入
        obj->heap[obj->size] = val;  // 插入末尾
        heapifyUp(obj->heap, obj->size);  // 向上調整維持堆性質
        obj->size++;  // 堆大小+1
    } else if (val > obj->heap[0]) {
        // 堆已滿且新元素>堆頂：替換並調整
        obj->heap[0] = val;  // 替換根節點(當前最小的)
        heapifyDown(obj->heap, obj->size, 0);  // 向下調整
    }
    // 堆頂始終是第k大元素

    return obj->heap[0];  // 返回當前第k大(堆頂)
}

/*
 * kthLargestFree - 釋放KthLargest物件佔用的記憶體
 * @obj: 待釋放物件的指標
 *
 * 依次釋放堆陣列和物件結構體
 */
void kthLargestFree(KthLargest* obj) {
    free(obj->heap);  // 先釋放堆陣列記憶體
    free(obj);        // 再釋放物件結構體記憶體
}
