/*
 * 605. Can Place Flowers
 * 難度：Easy
 * 時間複雜度：O(n)  — 單次掃描花圃陣列
 * 空間複雜度：O(1)  — 只使用兩個指標變數，無額外陣列
 *
 * ===== 演算法概述 =====
 *
 * 貪心法（Greedy）：從左到右掃描花圃，遇到空位就檢查左右鄰居。
 * 若左右鄰居皆為空（或不存在），則在此種花並跳過下一格。
 *
 * ===== 具體範例 =====
 *
 * 輸入：flowerbed = [1, 0, 0, 0, 1], n = 1
 *
 *   索引：  0    1    2    3    4
 *         [1]  [0]  [0]  [0]  [1]
 *          🌸             ?         🌸
 *
 *   i=0: flowerbed[0]=1，已種花，跳過
 *   i=1: flowerbed[1]=0，prev=&[0]=1 ≠ 0 → 不能種
 *   i=2: flowerbed[2]=0，prev=&[1]=0 ✓，next=&[3]=0 ✓，當前=0 ✓
 *        → 種花！n-- → n=0，i++ 跳過 i=3
 *
 *         [1]  [0]  [🌸] [0]  [1]
 *
 *   i=4: flowerbed[4]=1，已種花，跳過
 *   結束：n=0 ≤ 0 → 回傳 true ✓
 *
 * ===== 指標操作圖解 =====
 *
 *   當 i=2 時：
 *
 *     flowerbed:  [ 1 | 0 | 0 | 0 | 1 ]
 *                   ^       ^       ^
 *                   |       |       |
 *                  [0]     [2]     [4]
 *
 *     prev = flowerbed + 1  → 指向索引 1，*prev = 0  ✓
 *     next = flowerbed + 3  → 指向索引 3，*next = 0  ✓
 *     current = *(flowerbed + 2) = 0                  ✓
 *     → 三者皆為 0，可種花，n-- 並 i++ 跳過下一格
 *
 *   當 i=1 時：
 *
 *     prev = flowerbed + 0  → 指向索引 0，*prev = 1  ✗
 *     → prev 不為 0，短路求值，不需檢查 next，直接跳過
 *
 *   邊界情況（i=0）：
 *
 *     prev = NULL（i-1 < 0，不存在左鄰居）
 *     → prev == NULL 視為「左邊為空」，只需檢查 next 和 current
 */

#include "solution.h"
#include <stddef.h>
#include <stdio.h>

bool canPlaceFlowers(int* flowerbed, int flowerbedSize, int n) {
    /* 快速剪枝：花圃最多能種 (size+1)/2 朵花（全空時），
       若 n 超過 size/2 + 1 則一定不夠 */
    if (n > flowerbedSize / 2 + 1) {
        return false;                   /* 提前返回，避免無意義的掃描 */
    }

    for (int i = 0; i < flowerbedSize; i++) {
        /* 取得前一格的指標；若 i=0 則沒有前一格，設為 NULL */
        int* prev = i - 1 >= 0 ? flowerbed + i - 1 : NULL;

        /* 取得後一格的指標；若 i 是最後一個元素，設為 NULL */
        int* next = i + 1 < flowerbedSize ? flowerbed + i + 1 : NULL;

        /*
         * 三個條件同時成立才能種花：
         *   1. prev == NULL（邊界）或 *prev == 0（前一格為空）
         *   2. next == NULL（邊界）或 *next == 0（後一格為空）
         *   3. 當前格 *(flowerbed + i) == 0（當前為空）
         *
         * 利用短路求值：若 prev 不為 NULL 且 *prev 為 1，
         * 後面的條件不會被執行，效率更高。
         */
        if ((prev == NULL || !*prev) && (next == NULL || !*next) && !*(flowerbed + i)) {
            n--;                        /* 成功種一朵花，需求數減 1 */
            i++;                        /* 跳過下一格（已種花的右鄰必不可種） */
        }
    }

    return n <= 0;                      /* n ≤ 0 表示所有花都已種完 */
}
