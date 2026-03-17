//https://leetcode.com/problems/two-sum/description/?envType=problem-list-v2&envId=wl94y6ih

#include <stdlib.h>

// 題目：1. Two Sum
// 解法：Hash Table（雜湊表）
// 時間複雜度：O(n)
// 空間複雜度：O(n)
//
// 核心觀念：
// 1. 走訪陣列時，先計算目前數字需要的另一半值 complement = target - nums[i]
// 2. 去雜湊表中查詢 complement 是否已經出現過
// 3. 如果出現過，代表答案已找到，直接回傳兩個索引
// 4. 如果還沒出現，就把目前的 nums[i] 和它的索引存進雜湊表
//
// 圖解範例：
// nums = [2, 7, 11, 15], target = 9
//
// 陣列索引：
// index:  0   1   2   3
// nums : [2,  7, 11, 15]
//
// --------------------------------------------------
// 第 1 輪：i = 0
// 目前數字 nums[0] = 2
// 需要找的值 complement = 9 - 2 = 7
//
// 先查雜湊表：
// table 中還沒有 7
//
// 所以把目前資料存進去：
// value 2 -> index 0
//
// 雜湊表概念圖：
// +-----------+-----------+
// | key(value)| index     |
// +-----------+-----------+
// |     2     |    0      |
// +-----------+-----------+
//
// --------------------------------------------------
// 第 2 輪：i = 1
// 目前數字 nums[1] = 7
// 需要找的值 complement = 9 - 7 = 2
//
// 先查雜湊表：
// table 中已經有 2 -> index 0
//
// 代表：
// nums[0] + nums[1] = 2 + 7 = 9
//
// 所以答案就是：
// [0, 1]
//
// --------------------------------------------------
// 為什麼這樣快？
// 暴力法要對每個數字再往後找一次，時間複雜度是 O(n^2)
// 雜湊表可以把「找某個值是否存在」壓到接近 O(1)
// 所以整體只要走一遍陣列，時間複雜度就是 O(n)

// 用來表示雜湊表中的一個格子。
typedef struct {
    // used = 0 表示這個格子還沒被使用。
    int used;

    // key 儲存數值本身，也就是 nums[i]。
    int key;

    // index 儲存該數值在原始陣列中的索引位置。
    int index;
} HashEntry;

// 把整數轉成合法的雜湊索引。
static int hashIndex(int key, int tableSize) {
    // 先取餘數，讓 key 對應到表格範圍內。
    int hash = key % tableSize;

    // C 語言中負數取餘數可能是負的，所以要補回正值。
    if (hash < 0) {
        hash += tableSize;
    }

    // 回傳最終的索引位置。
    return hash;
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    // LeetCode 要求回傳答案長度，因此固定設為 2。
    *returnSize = 2;

    // 配置回傳用的陣列，裡面會放兩個索引。
    int* result = (int*)malloc(2 * sizeof(int));

    // 題目保證一定有解；這裡仍先檢查記憶體配置是否成功。
    if (result == NULL) {
        *returnSize = 0;
        return NULL;
    }

    // 把雜湊表大小開成 numsSize 的兩倍加一，降低碰撞機率。
    int tableSize = numsSize * 2 + 1;

    // 配置雜湊表空間。
    HashEntry* table = (HashEntry*)calloc(tableSize, sizeof(HashEntry));

    // 如果雜湊表配置失敗，先釋放 result 再回傳 NULL。
    if (table == NULL) {
        free(result);
        *returnSize = 0;
        return NULL;
    }

    // 逐一掃描整個陣列。
    for (int i = 0; i < numsSize; i++) {
        // 計算目前數字需要搭配的另一個值。
        int complement = target - nums[i];

        // 從 complement 對應的雜湊位置開始找。
        int findPos = hashIndex(complement, tableSize);

        // 線性探測：如果該位置被占用但不是我們要的值，就繼續往後找。
        while (table[findPos].used) {
            // 如果找到 key 等於 complement，代表答案已經出現。
            if (table[findPos].key == complement) {
                // 第一個索引是先前存進雜湊表的那個數字位置。
                result[0] = table[findPos].index;

                // 第二個索引是目前掃描到的位置 i。
                result[1] = i;

                // 回傳前記得釋放雜湊表，避免記憶體洩漏。
                free(table);

                // 回傳答案陣列。
                return result;
            }

            // 發生碰撞時，往下一格繼續找。
            findPos = (findPos + 1) % tableSize;
        }

        // 如果 complement 還沒找到，就把目前的 nums[i] 存進雜湊表。
        int insertPos = hashIndex(nums[i], tableSize);

        // 如果插入位置已被使用，就用線性探測找下一個空格。
        while (table[insertPos].used) {
            insertPos = (insertPos + 1) % tableSize;
        }

        // 標記這個位置已經使用。
        table[insertPos].used = 1;

        // 存入目前數字。
        table[insertPos].key = nums[i];

        // 存入目前數字對應的索引。
        table[insertPos].index = i;
    }

    // 題目保證一定有解，理論上不會走到這裡。
    // 這段是保險寫法，避免函式沒有回傳值。
    free(table);
    free(result);
    *returnSize = 0;
    return NULL;
}
