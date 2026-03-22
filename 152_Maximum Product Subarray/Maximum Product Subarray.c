/*
 * LeetCode 152. 最大乘積子數組
 * 難度：Medium
 * 演算法：追踪最大和最小乘積(負×負=正)
 * 時間：O(n)，空間：O(1)
 */

#include <limits.h>

int maxProduct(int* nums, int numsSize) {
    if (numsSize == 0) return 0;

    int max_prod = nums[0];
    int min_prod = nums[0];
    int result = nums[0];

    for (int i = 1; i < numsSize; i++) {
        /* 當前數為負時，最小乘積可能變成最大 */
        /* 保存當前最大值，因為要用它計算新的最小值 */
        int temp_max = max_prod;

        max_prod = (nums[i] > max_prod * nums[i]) ? nums[i] : max_prod * nums[i];
        max_prod = (max_prod > min_prod * nums[i]) ? max_prod : min_prod * nums[i];

        min_prod = (nums[i] < temp_max * nums[i]) ? nums[i] : temp_max * nums[i];
        min_prod = (min_prod < min_prod * nums[i]) ? min_prod : min_prod * nums[i];

        result = (max_prod > result) ? max_prod : result;
    }

    return result;
}
