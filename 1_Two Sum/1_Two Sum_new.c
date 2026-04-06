/*
 * LeetCode 1. Two Sum（兩數之和）— 雜湊表法
 * 難度：Easy
 * 時間複雜度：O(n)    — 每個元素最多查詢與插入各一次，均攤 O(1)
 * 空間複雜度：O(n)    — 雜湊表最多存放 n 個元素
 *
 * ======================================================================
 * 演算法：雜湊表 (Hash Table) — 一次遍歷法
 * ======================================================================
 *
 * 核心思想：
 *   使用雜湊表儲存「已見過的數值 → 其索引」的對應關係。
 *   遍歷陣列時，對每個元素計算補數 complement = target - nums[i]：
 *     - 若 complement 已存在於雜湊表 → 找到答案，回傳兩個索引
 *     - 若 complement 不存在 → 將 nums[i] 及其索引插入雜湊表，繼續下一個
 *
 * 雜湊表實作細節（開放定址法 / Open Addressing + Linear Probing）：
 *   - 桶數量 capacity 取 numsSize * 2，降低碰撞機率
 *   - 每個桶有兩種狀態：EMPTY（空）、OCCUPIED（已佔用）
 *   - 每個桶額外存放 value 欄位，記錄該數值在原始陣列中的索引
 *   - 雜湊函式：將 key 映射到 [0, capacity) 的非負索引
 *     hash(key) = ((key % capacity) + capacity) % capacity
 *     先取餘再加 capacity 再取餘，確保負數也能正確映射
 *   - 碰撞處理：線性探測，逐一往後尋找空桶
 *
 * ======================================================================
 * 與 217. Contains Duplicate 的對比
 * ======================================================================
 *
 *   217 題：雜湊表當「集合 (Set)」用
 *     - 桶只存 key（數值本身）
 *     - 查到相同 key → 回傳 true（有重複）
 *
 *   1 題：雜湊表當「映射 (Map)」用
 *     - 桶存 key（數值）+ value（原始索引）
 *     - 查的不是自己，而是補數 complement
 *     - 查到 complement → 回傳兩個索引
 *
 * ======================================================================
 * 範例演示：nums = [2, 7, 11, 15]，target = 9，capacity = 8
 * ======================================================================
 *
 *   初始雜湊表（8 個桶，全部 EMPTY）：
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
 *     [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]
 *
 *   Step 1: 處理 nums[0] = 2，complement = 9 - 2 = 7
 *     查詢 hash(7) = 7 % 8 = 7 → 桶[7] 是 EMPTY → 補數不存在
 *     插入 hash(2) = 2 % 8 = 2 → 桶[2] 是 EMPTY → 插入 key=2, value=0
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ 2→i0  │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
 *                      ▲ 插入 key=2, value=0
 *
 *   Step 2: 處理 nums[1] = 7，complement = 9 - 7 = 2
 *     查詢 hash(2) = 2 % 8 = 2 → 桶[2] 已佔用且 key == 2 → 命中！
 *     補數 2 的索引 value = 0，當前索引 = 1
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ 2→i0  │ EMPTY │ EMPTY │ EMPTY │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
 *                      ▲ 命中！回傳 [0, 1] ✓
 *
 * ======================================================================
 * 範例演示：nums = [3, 2, 4]，target = 6，capacity = 6
 * ======================================================================
 *
 *   Step 1: 處理 nums[0] = 3，complement = 6 - 3 = 3
 *     查詢 hash(3) = 3 → 桶[3] 是 EMPTY → 補數不存在
 *     插入 hash(3) = 3 → 桶[3] 插入 key=3, value=0
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ EMPTY │ 3→i0  │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┘
 *
 *   Step 2: 處理 nums[1] = 2，complement = 6 - 2 = 4
 *     查詢 hash(4) = 4 → 桶[4] 是 EMPTY → 補數不存在
 *     插入 hash(2) = 2 → 桶[2] 插入 key=2, value=1
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ 2→i1  │ 3→i0  │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┘
 *
 *   Step 3: 處理 nums[2] = 4，complement = 6 - 4 = 2
 *     查詢 hash(2) = 2 → 桶[2] 已佔用且 key == 2 → 命中！
 *     補數 2 的索引 value = 1，當前索引 = 2
 *   ┌───────┬───────┬───────┬───────┬───────┬───────┐
 *   │ EMPTY │ EMPTY │ 2→i1  │ 3→i0  │ EMPTY │ EMPTY │
 *   └───────┴───────┴───────┴───────┴───────┴───────┘
 *                      ▲ 命中！回傳 [1, 2] ✓
 *
 * ======================================================================
 * 碰撞演示：假設 capacity = 6，插入 key=2 後再查詢 complement=8
 * ======================================================================
 *
 *   hash(2) = 2，hash(8) = 8 % 6 = 2 → 碰撞！
 *
 *   桶[2] 已佔用且 key=2 ≠ 8 → 線性探測 → 桶[3] 檢查 ...
 *   直到找到 key==8 的桶（命中）或遇到 EMPTY 桶（補數不存在）
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* 桶的狀態標記 */
#define EMPTY    0   /* 此桶尚未使用 */
#define OCCUPIED 1   /* 此桶已存放元素 */

/* 雜湊表的桶結構 */
typedef struct {
    int key;         /* 儲存的整數值（nums[i] 的值） */
    int value;       /* 儲存的索引（nums[i] 在原始陣列中的位置） */
    int state;       /* 桶的狀態：EMPTY 或 OCCUPIED */
} Bucket;

/* 雜湊函式：將整數 key 映射到 [0, capacity) 的非負索引 */
/* 先取餘再加 capacity 再取餘，處理負數的情況 */
static inline int hashFunc(int key, int capacity) {
    return ((key % capacity) + capacity) % capacity;
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    /* 設定回傳長度為 2（題目保證恰好有一組解） */
    *returnSize = 2;

    /* 配置回傳用的陣列，存放兩個索引 */
    int* result = (int*)malloc(2 * sizeof(int));
    if (!result) {
        *returnSize = 0;
        return NULL;  /* 記憶體配置失敗，安全退出 */
    }

    /* 雜湊表容量設為 numsSize 的兩倍，降低碰撞機率（負載因子 ≤ 0.5） */
    int capacity = numsSize * 2;

    /* 配置雜湊表記憶體並初始化為全零（所有桶狀態為 EMPTY = 0） */
    Bucket* table = (Bucket*)calloc(capacity, sizeof(Bucket));
    if (!table) {
        free(result);
        *returnSize = 0;
        return NULL;  /* 記憶體配置失敗，安全退出 */
    }

    /* 遍歷陣列中的每個元素 */
    for (int i = 0; i < numsSize; i++) {
        int key = nums[i];                           /* 當前要處理的值 */
        int complement = target - key;               /* 計算補數 */
        int idx = hashFunc(complement, capacity);    /* 計算補數的雜湊索引 */

        /* ── 階段一：查詢補數是否已存在於雜湊表 ── */
        while (table[idx].state != EMPTY) {
            /* 桶已佔用，檢查是否為補數 */
            if (table[idx].key == complement) {
                /* @@@找到補數！組合答案後回傳 */
                result[0] = table[idx].value;  /* 補數在原始陣列中的索引 */
                result[1] = i;                 /* 當前元素的索引 */
                free(table);                   /* 釋放雜湊表記憶體 */
                return result;
            }
            /* 不是補數 → 碰撞，往下一個桶探測（環繞） */
            idx = (idx + 1) % capacity;
        }

        /* ── 階段二：補數不存在，將當前元素插入雜湊表 ── */
        idx = hashFunc(key, capacity);         /* 計算當前值的雜湊索引 */

        /* 線性探測：尋找空桶插入 */
        while (table[idx].state != EMPTY) {
            idx = (idx + 1) % capacity;        /* 碰撞，往下一個桶探測 */
        }

        /* 探測到空桶，插入當前元素 */
        table[idx].key = key;                  /* 存放鍵值（數值本身） */
        table[idx].value = i;                  /* 存放索引（在原始陣列的位置） */
        table[idx].state = OCCUPIED;           /* 標記為已佔用 */
    }

    /* 題目保證恰好有一組解，理論上不會走到這裡 */
    free(table);    /* 釋放雜湊表記憶體 */
    free(result);   /* 釋放結果陣列記憶體 */
    *returnSize = 0;
    return NULL;
}
