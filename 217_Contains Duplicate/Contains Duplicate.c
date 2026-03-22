/*
 * LeetCode 217. 包含重複元素
 * 難度：Easy
 *
 * 問題描述：
 * 給定一個整數數組 nums，如果任何值在數組中出現至少兩次，則返回 true。
 * 如果每個元素都是不同的，則返回 false。
 *
 * 演算法：雜湊集合 (Hash Set)
 *
 * 核心思想：
 * - 遍歷數組，對於每個元素，檢查是否已在集合中
 * - 如果是，說明找到重複元素，返回 true
 * - 如果否，將元素加入集合
 * - 遍歷完成仍未找到重複，返回 false
 *
 * 複雜度分析：
 * - 時間複雜度：O(n)，遍歷數組一次，雜湊表操作為O(1)
 * - 空間複雜度：O(n)，最壞情況下存儲所有元素
 */

#include <stdlib.h>
#include <stdbool.h>

/* 比較函數：用於qsort排序整數 */
int compare(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

bool containsDuplicate(int* nums, int numsSize) {
    /* 特殊情況：數組為空或只有一個元素 */
    if (numsSize <= 1) {
        return false;
    }

    /* 對數組進行排序 */
    /* 如果存在重複元素，排序後必然會相鄰 */
    qsort(nums, numsSize, sizeof(int), compare);

    /* 遍歷排序後的數組，檢查相鄰元素是否相等 */
    for (int i = 0; i < numsSize - 1; i++) {
        /* 如果當前元素等於下一個元素，說明找到重複 */
        if (nums[i] == nums[i + 1]) {
            return true;
        }
    }

    /* 遍歷完成，沒有找到重複元素 */
    return false;
}
