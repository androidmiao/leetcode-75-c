/*
 * LeetCode 53. 最大子數組和
 * 難度：Medium
 *
 * 演算法：Kadane's Algorithm
 * 核心：追踪當前子數組和，若為負則重置為0
 * 時間：O(n)，空間：O(1)
 */

#include <limits.h>

int maxSubArray(int* nums, int numsSize) {
    if (numsSize == 0) return 0;

    int max_sum = nums[0];
    int current_sum = nums[0];

    for (int i = 1; i < numsSize; i++) {
        /* 決定是繼續當前子數組還是開始新的 */
        current_sum = (nums[i] > current_sum + nums[i]) ? nums[i] : current_sum + nums[i];

        /* 更新最大值 */
        if (current_sum > max_sum) {
            max_sum = current_sum;
        }
    }

    return max_sum;
}
