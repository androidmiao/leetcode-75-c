/*
 * LeetCode 219. Contains Duplicate II （包含重複元素 II）
 * 難度：Easy
 * 時間複雜度：O(n)
 * 空間複雜度：O(min(n, k))
 *
 * 題目：
 *   給定整數陣列 nums 和整數 k，
 *   判斷是否存在兩個不同索引 i, j 使得
 *   nums[i] == nums[j] 且 abs(i - j) <= k。
 *
 * 演算法：滑動視窗 + 雜湊表
 *
 * ═══════════════════════════════════════════════════════════════
 *  核心思想：維護一個大小最多為 k 的滑動視窗，
 *  視窗內的元素存入雜湊表。對每個新元素：
 *    1) 查表看是否已在視窗內 → 若是，距離必定 <= k，回傳 true
 *    2) 將新元素加入表中
 *    3) 若視窗超過 k，移除最舊元素（索引 i-k）
 * ═══════════════════════════════════════════════════════════════
 *
 * 範例 1：nums = [1, 2, 3, 1], k = 3
 *
 *   i=0: 視窗 = {}         → 查找 1? 不在 → 加入 {1}
 *   i=1: 視窗 = {1}        → 查找 2? 不在 → 加入 {1,2}
 *   i=2: 視窗 = {1,2}      → 查找 3? 不在 → 加入 {1,2,3}
 *   i=3: 視窗 = {1,2,3}    → 查找 1? 在！ → return true ✓
 *                                  距離 = |3-0| = 3 <= k=3
 *
 *   滑動視窗圖示（視窗大小上限 k=3）：
 *
 *   索引:   0   1   2   3
 *   陣列: [ 1 | 2 | 3 | 1 ]
 *          ├───────────┤
 *          └─ 視窗 [0..2] ─┘
 *
 *   i=3 時查找 nums[3]=1，發現 nums[0]=1 在視窗內
 *   距離 abs(3-0) = 3 <= k=3  →  true
 *
 * ─────────────────────────────────────────────────────────────
 *
 * 範例 2：nums = [1, 0, 1, 1], k = 1
 *
 *   i=0: 視窗 = {}         → 查找 1? 不在 → 加入 {1:0}
 *   i=1: 視窗 = {1:0}      → 查找 0? 不在 → 加入 {1:0, 0:1}
 *         i=1 >= k=1，移除 nums[0]=1 → 視窗 = {0:1}
 *   i=2: 視窗 = {0:1}      → 查找 1? 不在 → 加入 {0:1, 1:2}
 *         i=2 >= k=1，移除 nums[1]=0 → 視窗 = {1:2}
 *   i=3: 視窗 = {1:2}      → 查找 1? 在！距離 |3-2|=1 <= k=1
 *                                            → return true ✓
 *
 *   滑動視窗移動過程：
 *
 *   索引:  0   1   2   3
 *   陣列: [1 | 0 | 1 | 1]
 *
 *   i=0:  [1]              視窗={1}
 *   i=1:  [1  0]           視窗={1,0} → 移除1 → {0}
 *          ├──┤  (k=1)
 *   i=2:     [0  1]        視窗={0,1} → 移除0 → {1}
 *              ├──┤
 *   i=3:        [1  1]     查到1在視窗！→ true
 *                 ├──┤
 *
 * ─────────────────────────────────────────────────────────────
 *
 * 範例 3：nums = [1, 2, 3, 1, 2, 3], k = 2
 *
 *   i=0: 視窗 = {}           → 加入 {1:0}
 *   i=1: 視窗 = {1:0}        → 加入 {1:0, 2:1}
 *   i=2: 視窗 = {1:0, 2:1}   → 加入 {1:0, 2:1, 3:2}
 *         i=2 >= k=2，移除 nums[0]=1 → {2:1, 3:2}
 *   i=3: 視窗 = {2:1, 3:2}   → 查找 1? 不在（已被移除）→ 加入
 *         移除 nums[1]=2 → {3:2, 1:3}
 *   i=4: 視窗 = {3:2, 1:3}   → 查找 2? 不在（已被移除）→ 加入
 *         移除 nums[2]=3 → {1:3, 2:4}
 *   i=5: 視窗 = {1:3, 2:4}   → 查找 3? 不在 → 加入
 *         移除 nums[3]=1 → {2:4, 3:5}
 *   → return false
 *
 *   所有重複元素距離 = 3 > k = 2，每次重複元素都已被移出視窗
 *
 * ═══════════════════════════════════════════════════════════════
 *  雜湊表操作流程圖（以範例 1 為例）：
 *
 *  ┌─────────────┐  查找 nums[i]   ┌─────────────┐
 *  │  遍歷陣列    │───────────────▶│  雜湊表      │
 *  │  i: 0→n-1   │                │  (視窗內元素) │
 *  └─────────────┘                └──────┬──────┘
 *        │                                │
 *        │  找到？─── 是 ──▶ return true   │
 *        │    │                            │
 *        │   否                            │
 *        │    │                            │
 *        │    ▼                            │
 *        │  加入 nums[i]                   │
 *        │    │                            │
 *        │    ▼                            │
 *        │  i >= k ?─── 是 ──▶ 移除 nums[i-k]
 *        │    │
 *        │   否
 *        │    │
 *        └────┘ (繼續下一個 i)
 *
 * ═══════════════════════════════════════════════════════════════
 */

#include <stdlib.h>
#include <stdbool.h>

/*
 * 自訂雜湊表結構
 * 因為 C 語言沒有內建雜湊表，這裡用平行陣列模擬。
 * keys[] 存放視窗中的值，values[] 存放對應的最近索引。
 * 在視窗大小 <= k 的前提下，線性搜尋的代價為 O(k)，
 * 對於本題 k <= 10^5 的限制下實際表現良好。
 */
typedef struct {
    int* keys;      /* 存儲視窗中的值 */
    int* values;    /* 存儲對應的索引 */
    int size;       /* 當前視窗中的元素個數 */
    int capacity;   /* 容量上限 */
} HashMap;

/* 建立雜湊表，分配 capacity 大小的平行陣列 */
HashMap* hashmap_create(int capacity) {
    HashMap* hm = (HashMap*)malloc(sizeof(HashMap));
    hm->keys = (int*)malloc(capacity * sizeof(int));
    hm->values = (int*)malloc(capacity * sizeof(int));
    hm->size = 0;
    hm->capacity = capacity;
    return hm;
}

/* 在雜湊表中查找 key，回傳對應的索引值；若不存在則回傳 -1 */
/* 線性搜尋：逐一比對 keys[] 陣列中的每個元素 */
int hashmap_get(HashMap* hm, int key) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            return hm->values[i]; /* 找到，回傳該值對應的陣列索引 */
        }
    }
    return -1; /* 不存在 */
}

/* 插入或更新鍵值對 */
/* 若 key 已存在，更新其 value（最近索引）；否則新增一筆 */
void hashmap_put(HashMap* hm, int key, int value) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            hm->values[i] = value; /* 更新為最新索引 */
            return;
        }
    }
    /* key 不存在，新增至尾端 */
    if (hm->size < hm->capacity) {
        hm->keys[hm->size] = key;
        hm->values[hm->size] = value;
        hm->size++;
    }
}

/* 從雜湊表中移除指定的 key */
/* 將被刪除位置之後的元素依序前移，維持陣列緊湊 */
void hashmap_remove_key(HashMap* hm, int key) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            /* 前移後續元素，覆蓋被刪除的位置 */
            for (int j = i; j < hm->size - 1; j++) {
                hm->keys[j] = hm->keys[j + 1];
                hm->values[j] = hm->values[j + 1];
            }
            hm->size--;
            return;
        }
    }
}

/* 釋放雜湊表佔用的記憶體 */
void hashmap_free(HashMap* hm) {
    free(hm->keys);
    free(hm->values);
    free(hm);
}

bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    /* 邊界條件：只有一個元素不可能有重複；k=0 代表要求同一索引，不合題意 */
    if (numsSize <= 1 || k < 1) {
        return false;
    }

    /* 建立雜湊表，容量為 min(k+1, numsSize) */
    /* +1 是因為視窗最多包含 k+1 個元素（加入後再移除） */
    int capacity = (k + 1) < numsSize ? (k + 1) : numsSize;
    HashMap* hm = hashmap_create(capacity);

    for (int i = 0; i < numsSize; i++) {
        /* 步驟 1：查找當前元素是否已在視窗（雜湊表）中 */
        int last_index = hashmap_get(hm, nums[i]);

        /* 若找到且距離 <= k，代表符合條件 */
        if (last_index != -1 && i - last_index <= k) {
            hashmap_free(hm);
            return true; /* 找到近距離重複 */
        }

        /* 步驟 2：將當前元素及其索引加入雜湊表 */
        hashmap_put(hm, nums[i], i);

        /* 步驟 3：維護視窗大小不超過 k */
        /* 當 i >= k 時，將離開視窗的最舊元素 nums[i-k] 移除 */
        if (i >= k) {
            hashmap_remove_key(hm, nums[i - k]);
        }
    }

    /* 遍歷完成，未找到任何符合條件的重複 */
    hashmap_free(hm);
    return false;
}
