/*
================================================================================
 題號: 1431 - Kids With the Greatest Number of Candies (糖果最多的孩子)
 難度: Easy

 時間複雜度: O(n) - 需要遍歷陣列兩次（找最大值 + 判斷每個孩子）
 空間複雜度: O(n) - 返回布林陣列，大小為輸入陣列大小

================================================================================
 演算法步驟說明:

 第1步: 初始化並檢查輸入有效性
       - 為每個孩子配置一個布林值（記錄是否擁有最多糖果）
       - 若輸入陣列為空，提前返回已分配的空陣列

 第2步: 第一次遍歷 - 找出目前的最大糖果數
       - 遍歷整個陣列，追蹤最大值
       - greatest 初始為 -1，所以即使最小的非負數也會被記錄

 第3步: 第二次遍歷 - 判斷每個孩子加上額外糖果後是否達到最大值
       - 對每個孩子，計算其糖果總數 (原有 + 額外)
       - 若總數 >= 目前最大值，標記為 true
       - 否則標記為 false

================================================================================
 範例 1: candies=[2,3,5,1,3], extraCandies=3

 第一次遍歷 (找最大值):

   索引:    0   1   2   3   4
   糖果: ┌───┬───┬───┬───┬───┐
        │ 2 │ 3 │ 5 │ 1 │ 3 │
        └───┴───┴───┴───┴───┘
   greatest: 5 (最大值)

 第二次遍歷 (檢查: 糖果[i] + 3 >= 5?):

   索引:    0   1   2   3   4
   操作: ┌───┬───┬───┬───┬───┐
        │2+3│3+3│5+3│1+3│3+3│  (加上額外糖果)
        │ 5 │ 6 │ 8 │ 4 │ 6 │
        └───┴───┴───┴───┴───┘
        │ T │ T │ T │ F │ T │  (>= 5?)
        └───┴───┴───┴───┴───┘

   返回陣列: [true, true, true, false, true]

================================================================================
*/

#include "solution.h"
#include <stddef.h>
#include <stdlib.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
bool* kidsWithCandies(int* candies, int candiesSize, int extraCandies, int* returnSize) {
    /* 為布林陣列分配記憶體，大小等於孩子數量
       使用 (size_t) 轉換避免帶符號/無符號比較警告 */
    bool* table = malloc((size_t) candiesSize * sizeof(table));

    /* 檢查輸入陣列有效性
       若 candies 為 NULL，無法處理，直接返回已分配但未初始化的陣列 */
    if (candies == NULL) {
        return table;
    }

    /* 若呼叫端提供 returnSize 指標，設定輸出陣列的大小
       返回的陣列大小等於輸入陣列大小 */
    if (returnSize != NULL) {
        *returnSize = candiesSize;
    }

    /* 初始化最大值為 -1
       這樣即使輸入包含 0 或負數，也能正確更新 greatest
       不變式: greatest 記錄了 candies[0..i-1] 中的最大值 */
    int greatest = -1;

    /* 第一次遍歷: 尋找陣列中的最大值
       i 從 0 迭代到 candiesSize-1，遍歷每個孩子的糖果數 */
    for (size_t i = 0; i < (size_t) candiesSize; i++) {
        /* 若目前孩子的糖果數 >= 已知最大值，更新最大值
           使用 >= 而非 > 以處理重複值（不影響最終結果）
           不變式: greatest 始終是 candies[0..i] 中的最大值 */
        if (candies[i] >= greatest) {
            greatest = candies[i];
        }
    }

    /* 第二次遍歷: 判斷每個孩子加上額外糖果後是否擁有最多糖果
       i 從 0 迭代到 candiesSize-1，檢查每個孩子 */
    for (size_t i = 0; i < (size_t) candiesSize; i++) {
        /* 計算該孩子獲得額外糖果後的總數，與最大值比較
           若 candies[i] + extraCandies >= greatest，該孩子將擁有最多糖果
           設定 table[i] = true；否則設定為 false
           不變式: table[0..i-1] 已正確記錄前 i 個孩子的結果 */
        if (candies[i] + extraCandies >= greatest) {
            table[i] = true;
        } else {
            table[i] = false;
        }
    }

    /* 返回布林陣列，由呼叫端負責使用 free() 釋放記憶體 */
    return table;
}
