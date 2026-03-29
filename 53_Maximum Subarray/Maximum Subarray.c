/*
 * ╔════════════════════════════════════════════════════════════════════════════╗
 * ║ LeetCode Problem #53: Maximum Subarray                                     ║
 * ║ Difficulty: Medium                                                         ║
 * ║ Time Complexity: O(n)  - Single pass through array                         ║
 * ║ Space Complexity: O(1) - Only using two variables (current_sum, max_sum)   ║
 * ╚════════════════════════════════════════════════════════════════════════════╝
 *
 * ALGORITHM: Kadane's Algorithm
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * STEP-BY-STEP WALKTHROUGH:
 * ──────────────────────────
 * 1. Initialize max_sum to INT_MIN and current_sum to 0
 * 2. Iterate through each element in the array
 * 3. At each position i:
 *    - Update current_sum = max(nums[i], current_sum + nums[i])
 *      This decides: start a new subarray at i, OR extend the previous one
 *    - Update max_sum = max(max_sum, current_sum)
 *      Track the best sum seen so far
 * 4. Return max_sum (the maximum subarray sum)
 *
 * KEY INSIGHT:
 * At each position, we make a local optimal choice:
 * - If current_sum + nums[i] > nums[i], extend the current subarray
 * - Otherwise, start a new subarray from nums[i]
 * This greedy choice leads to the global optimum.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * CONCRETE EXAMPLE: nums = [-2, 1, -3, 4, -1, 2, 1, -5, 4]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *   Index:      0   1   2   3   4   5   6   7   8
 *   nums:     [-2] [1] [-3] [4] [-1] [2] [1] [-5] [4]
 *
 * ASCII DIAGRAM: State changes at each step
 * ──────────────────────────────────────────────────────────────────────────────
 *
 * Step 0: Initialize
 *   max_sum = INT_MIN, current_sum = 0
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: INT_MIN  current_sum: 0    │
 *   └─────────────────────────────────────┘
 *
 * Step 1: Process nums[0] = -2
 *   current_sum = max(-2, 0 + (-2)) = max(-2, -2) = -2
 *   max_sum = max(INT_MIN, -2) = -2
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: -2    current_sum: -2      │
 *   │ Subarray: [-2]                      │
 *   └─────────────────────────────────────┘
 *
 * Step 2: Process nums[1] = 1
 *   current_sum = max(1, -2 + 1) = max(1, -1) = 1
 *   max_sum = max(-2, 1) = 1
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 1     current_sum: 1       │
 *   │ Subarray: [1]  (new start)          │
 *   └─────────────────────────────────────┘
 *
 * Step 3: Process nums[2] = -3
 *   current_sum = max(-3, 1 + (-3)) = max(-3, -2) = -2
 *   max_sum = max(1, -2) = 1
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 1     current_sum: -2      │
 *   │ Subarray: [1, -3]                   │
 *   └─────────────────────────────────────┘
 *
 * Step 4: Process nums[3] = 4
 *   current_sum = max(4, -2 + 4) = max(4, 2) = 4
 *   max_sum = max(1, 4) = 4
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 4     current_sum: 4       │
 *   │ Subarray: [4]  (new start)          │
 *   └─────────────────────────────────────┘
 *
 * Step 5: Process nums[4] = -1
 *   current_sum = max(-1, 4 + (-1)) = max(-1, 3) = 3
 *   max_sum = max(4, 3) = 4
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 4     current_sum: 3       │
 *   │ Subarray: [4, -1]                   │
 *   └─────────────────────────────────────┘
 *
 * Step 6: Process nums[5] = 2
 *   current_sum = max(2, 3 + 2) = max(2, 5) = 5
 *   max_sum = max(4, 5) = 5
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 5     current_sum: 5       │
 *   │ Subarray: [4, -1, 2]                │
 *   └─────────────────────────────────────┘
 *
 * Step 7: Process nums[6] = 1
 *   current_sum = max(1, 5 + 1) = max(1, 6) = 6
 *   max_sum = max(5, 6) = 6
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 6     current_sum: 6       │
 *   │ Subarray: [4, -1, 2, 1] ✓✓✓         │
 *   │ BEST ANSWER FOUND!                  │
 *   └─────────────────────────────────────┘
 *
 * Step 8: Process nums[7] = -5
 *   current_sum = max(-5, 6 + (-5)) = max(-5, 1) = 1
 *   max_sum = max(6, 1) = 6
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 6     current_sum: 1       │
 *   │ Subarray: [4, -1, 2, 1, -5]         │
 *   └─────────────────────────────────────┘
 *
 * Step 9: Process nums[8] = 4
 *   current_sum = max(4, 1 + 4) = max(4, 5) = 5
 *   max_sum = max(6, 5) = 6
 *   ┌─────────────────────────────────────┐
 *   │ max_sum: 6     current_sum: 5       │
 *   │ Final Answer: 6                     │
 *   │ Maximum Subarray: [4, -1, 2, 1]    │
 *   └─────────────────────────────────────┘
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <limits.h>

int maxSubArray(int* nums, int numsSize) {
    // 初始化：將目前最大和設為最小整數值，以便任何第一個元素都會更新它
    // Initialize: max_sum tracks the maximum sum found so far
    int max_sum = INT_MIN;      // 存儲目前發現的最大子陣列和（初始為最小可能值）

    // 初始化目前子陣列的和為0
    // Initialize: current_sum tracks the sum of the current subarray being considered
    int current_sum = 0;        // 存儲目前檢視中的子陣列和

    // 遍歷陣列中的每個元素
    // Iterate through each element in the input array
    for (int i = 0; i < numsSize; i++) {
        // 卡丹演算法的核心邏輯：決定是否應該延續目前的子陣列，或開始一個新的子陣列
        // Core Kadane's logic: decide whether to extend the current subarray OR start fresh
        // 如果單獨的元素 nums[i] 比將其加入目前子陣列的總和更大，就開始新的子陣列
        // If starting a new subarray from nums[i] is better than extending, start new
        current_sum = (nums[i] > current_sum + nums[i]) ? nums[i] : current_sum + nums[i];
        // 邏輯等價於: current_sum = max(nums[i], current_sum + nums[i])

        // 更新目前發現的最大子陣列和
        // Update the global maximum if the current sum is better
        max_sum = (current_sum > max_sum) ? current_sum : max_sum;
        // 邏輯等價於: max_sum = max(max_sum, current_sum)
    }

    // 返回找到的最大子陣列和
    // Return the maximum subarray sum found during the entire traversal
    return max_sum;
}
