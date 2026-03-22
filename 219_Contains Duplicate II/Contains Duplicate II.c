/*
 * LeetCode 219. 包含重複元素 II
 * 難度：Easy
 *
 * 問題描述：
 * 給定整數數組 nums 和整數 k，
 * 判斷數組中是否存在兩個不同的索引 i 和 j，
 * 使得 nums[i] == nums[j] 且 abs(i - j) <= k。
 *
 * 演算法：滑動視窗 + 雜湊表
 *
 * 核心思想：
 * - 維護一個大小為 k 的滑動視窗
 * - 視窗內放入訪問過的元素到雜湊表(或排序集合)
 * - 對每個新元素，檢查是否已在視窗內
 * - 如果是，找到重複(且距離 <= k)，返回true
 * - 否則加入雜湊表，並確保視窗大小 <= k
 *
 * 複雜度分析：
 * - 時間複雜度：O(n)，每個元素被訪問一次
 * - 空間複雜度：O(min(n, k))，視窗最多包含k個元素
 */

#include <stdlib.h>
#include <stdbool.h>

/* 使用數組實現雜湊表 */
/* 因為nums[i]範圍可能很大，我們使用索引映射 */
/* 關鍵：在視窗中存儲每個值最後出現的索引 */
typedef struct {
    /* 使用足夠大的數組或更複雜的結構 */
    /* 簡化版本：對於測試，我們使用動態數組 */
    int* keys;      /* 存儲視窗中的值 */
    int* values;    /* 存儲對應的索引 */
    int size;       /* 當前視窗中的元素個數 */
    int capacity;   /* 容量 */
} HashMap;

/* 初始化雜湊表 */
HashMap* hashmap_create(int capacity) {
    HashMap* hm = (HashMap*)malloc(sizeof(HashMap));
    hm->keys = (int*)malloc(capacity * sizeof(int));
    hm->values = (int*)malloc(capacity * sizeof(int));
    hm->size = 0;
    hm->capacity = capacity;
    return hm;
}

/* 在雜湊表中查找值，返回索引，如果不存在返回-1 */
int hashmap_get(HashMap* hm, int key) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            return hm->values[i];
        }
    }
    return -1;
}

/* 添加或更新雜湊表中的值 */
void hashmap_put(HashMap* hm, int key, int value) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            hm->values[i] = value;
            return;
        }
    }
    /* 如果不存在，添加新元素 */
    if (hm->size < hm->capacity) {
        hm->keys[hm->size] = key;
        hm->values[hm->size] = value;
        hm->size++;
    }
}

/* 從雜湊表中移除一個值 */
void hashmap_remove_key(HashMap* hm, int key) {
    for (int i = 0; i < hm->size; i++) {
        if (hm->keys[i] == key) {
            /* 將後面的元素前移 */
            for (int j = i; j < hm->size - 1; j++) {
                hm->keys[j] = hm->keys[j + 1];
                hm->values[j] = hm->values[j + 1];
            }
            hm->size--;
            return;
        }
    }
}

/* 釋放雜湊表 */
void hashmap_free(HashMap* hm) {
    free(hm->keys);
    free(hm->values);
    free(hm);
}

bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    /* 特殊情況處理 */
    if (numsSize <= 1 || k < 1) {
        return false;
    }

    /* 創建雜湊表，用於存儲視窗內的元素 */
    /* 容量為min(k, numsSize) */
    int capacity = k < numsSize ? k : numsSize;
    HashMap* hm = hashmap_create(capacity);

    /* 遍歷數組 */
    for (int i = 0; i < numsSize; i++) {
        /* 查找當前元素是否已在雜湊表中 */
        /* hashmap_get返回該值最後出現的索引 */
        int last_index = hashmap_get(hm, nums[i]);

        /* 如果找到，檢查距離是否 <= k */
        if (last_index != -1 && i - last_index <= k) {
            /* 找到滿足條件的重複 */
            hashmap_free(hm);
            return true;
        }

        /* 將當前元素及其索引添加到雜湊表 */
        hashmap_put(hm, nums[i], i);

        /* 確保視窗大小不超過 k */
        /* 移除最舊的元素(i-k處的元素) */
        if (i >= k) {
            /* 從視窗中移除 nums[i-k] */
            hashmap_remove_key(hm, nums[i - k]);
        }
    }

    /* 遍歷完成，未找到符合條件的重複 */
    hashmap_free(hm);
    return false;
}
