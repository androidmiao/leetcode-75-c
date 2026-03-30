/*
 * ============================================================================
 * 題號 334: Increasing Triplet Subsequence (遞增三元子序列)
 * 難度: Medium (中等)
 * ============================================================================
 *
 * 問題描述:
 *   給定整數陣列，判斷是否存在三個不同索引 i < j < k，
 *   使得 nums[i] < nums[j] < nums[k] 的遞增三元子序列。
 *
 * 時間複雜度: O(n) - 單次遍歷陣列
 * 空間複雜度: O(1) - 只使用常數額外空間
 *
 * ============================================================================
 * 演算法步驟:
 * ============================================================================
 *
 * 核心思想:
 *   維護兩個變數 first 和 second，分別代表：
 *   - first:  目前遇見的最小值
 *   - second: 在某個 first 之後遇見的最小值
 *
 *   關鍵洞察: 不需要追蹤索引！只需追蹤值的大小關係。
 *   - 若找到 num > second，則必定存在 i < j < k 使得
 *     nums[i] < nums[j] < nums[k]
 *
 * ============================================================================
 * 範例 1: nums = [2,1,5,0,4,6]
 * ============================================================================
 *
 * 初始狀態:
 *   first = INT32_MAX, second = INT32_MAX
 *
 * 步驟分解:
 *
 *   索引 0, nums[0] = 2:
 *   - nums[0] <= first (2 <= INT32_MAX) → true
 *   - first = 2
 *   狀態: first=2, second=INT32_MAX
 *         (first 代表迄今最小值)
 *
 *   索引 1, nums[1] = 1:
 *   - nums[1] <= first (1 <= 2) → true
 *   - first = 1
 *   狀態: first=1, second=INT32_MAX
 *         (找到更小的第一元素)
 *
 *   索引 2, nums[2] = 5:
 *   - nums[2] <= first (5 <= 1) → false
 *   - nums[2] <= second (5 <= INT32_MAX) → true
 *   - second = 5
 *   狀態: first=1, second=5
 *         (記住: 在某個值(≤1)之後有 5)
 *
 *   索引 3, nums[3] = 0:
 *   - nums[3] <= first (0 <= 1) → true
 *   - first = 0
 *   狀態: first=0, second=5
 *         (更新 first，但 second 保持5)
 *
 *   索引 4, nums[4] = 4:
 *   - nums[4] <= first (4 <= 0) → false
 *   - nums[4] <= second (4 <= 5) → true
 *   - second = 4
 *   狀態: first=0, second=4
 *         (記住: 在某個值(≤0)之後有 4)
 *
 *   索引 5, nums[5] = 6:
 *   - nums[5] <= first (6 <= 0) → false
 *   - nums[5] <= second (6 <= 4) → false
 *   - 進入 else 分支: 返回 true
 *   返回 TRUE
 *
 * 答案: 找到子序列 [1, 5, 6] (或 [0, 4, 6])，返回 true
 *
 * ============================================================================
 * 範例 2: nums = [4,2,3,1,5]
 * ============================================================================
 *
 * 初始: first = INT32_MAX, second = INT32_MAX
 *
 *   i=0, nums[0]=4: first=4
 *   i=1, nums[1]=2: first=2 (更新為更小的值)
 *   i=2, nums[2]=3: second=3 (3在某個≤2的值之後)
 *   i=3, nums[3]=1: first=1 (更新為更小的值)
 *   i=4, nums[4]=5: 5>second(3) → 返回 true
 *
 * 答案: 找到子序列 [1, 3, 5]，返回 true
 *
 * 關鍵洞察:
 *   為什麼 first=1 之後還能找到？
 *   - second=3 是在 "之前的 first" 之後記錄的
 *   - 即使 first 更新到 1，second=3 依然有效！
 *   - 因為存在某個索引位置: old_first < second < nums[4]
 *   - 索引順序: ... 2 (設置舊first) ... 3 (設置second) ... 5
 *
 * ============================================================================
 * ASCII Flowchart:
 * ============================================================================
 *
 *     遍歷陣列中的每個數字
 *            |
 *            v
 *    ┌─────────────────────┐
 *    │  num <= first ?      │
 *    └─────────────────────┘
 *         yes |              no
 *            v                 |
 *     [first=num]              |
 *            |                 v
 *            |        ┌─────────────────────┐
 *            |        │  num <= second ?     │
 *            |        └─────────────────────┘
 *            |             yes |              no
 *            |                v                 |
 *            |         [second=num]             |
 *            |                |                 v
 *            |                |        ┌─────────────────┐
 *            |                |        │  返回 TRUE      │
 *            |                |        │  (找到三元組)   │
 *            |                |        └─────────────────┘
 *            |                |
 *            └────────┬───────┘
 *                     |
 *                     v
 *            繼續處理下一個元素
 *
 * ============================================================================
 */

#include "solution.h"
#include <stdio.h>
#include <stdint.h>

// void swap_xor(int* a, int* b) {
//     if (a == b)
//         return;
//
//     *a = *a ^ *b;
//     *b = *a ^ *b;
//     *a = *a ^ *b;
// }

bool increasingTripletBad(int* nums, int nums_size) {
    int triplet[3] = {nums[0], -1, -1};  // 陣列追蹤候選三元組: [第一個, 第二個, 第三個]
    int deep_triplet[3] = {-1, -1, -1};  // 備份陣列: 保存舊的三元組狀態，用於重啟時
    int idx = 0;  // 當前索引指標: 表示 triplet[] 中已填充元素的位置

    for (int i = 0; i < nums_size; i++) {
        // 檢查備份三元組: 若存在完整的舊三元組且新數字超過其第二個元素
        if (deep_triplet[1] != -1 && nums[i] > deep_triplet[1]) {
            printf("TRUE  | \"depth tripwire!\"\n");
            return true;  // 找到: deep_triplet[0] < deep_triplet[1] < nums[i]
        }

        // 情況1: 新數字 > 目前位置的值，向前推進三元組
        if (nums[i] > triplet[idx]) {
            idx++;  // 向下一個位置移動 (0→1 或 1→2)
            triplet[idx] = nums[i];  // 記錄更大的值
        }
        // 情況2: 新數字在合法範圍內，用更小值替換 (保留機會以接受更大的數字)
        else if (((idx > 0 && nums[i] > triplet[idx - 1]) || idx == 0) && nums[i] < triplet[idx]) {
            triplet[idx] = nums[i];  // 替換為較小的候選，改善未來找到更大值的機率
        }
        // 情況3: 新數字太小，破壞了目前的三元組，需要重啟但保存進度
        else if (nums[i] < triplet[0] && (nums[i] < deep_triplet[0] || deep_triplet[0] == -1)) {
            // 保存舊的三元組狀態，以防更新後的三元組無法找到答案
            deep_triplet[0] = triplet[0];
            deep_triplet[1] = triplet[1];
            deep_triplet[2] = triplet[2];

            // 重啟追蹤: 以新的最小值開始
            idx = 0;
            triplet[idx] = nums[i];
            triplet[1] = -1;  // 重置為空
            triplet[2] = -1;  // 重置為空
        }

        // 驗證: 檢查是否已完成遞增三元組 (三個位置都有有效值且遞增)
        if (triplet[0] < triplet[1] && triplet[1] < triplet[2]) {
            printf("TRUE | 0: %d, 1: %d, 2: %d\n", triplet[0], triplet[1], triplet[2]);
            return true;  // 找到答案！
        }
    }

    printf("FALSE | 0: %d, 1: %d, 2: %d\n", triplet[0], triplet[1], triplet[2]);
    return false;  // 遍歷完畢，未找到三元組
}

bool increasingTriplet(int* nums, int nums_size) {
    // 初始化: 兩個追蹤變數，初值為最大整數 (表示尚未初始化)
    int first = INT32_MAX;   // 第一個元素的候選值: 目前遇見的最小值
    int second = INT32_MAX;  // 第二個元素的候選值: 在某個 first 之後遇見的最小值

    for (int i = 0; i < nums_size; i++) {
        // 條件1: 若新數字 <= first，更新 first 為此數字 (貪心策略: 保持最小值)
        if (nums[i] <= first) {
            first = nums[i];  // 更新 first: 這個新值可能成為三元組的第一個元素
        }
        // 條件2: 否則若新數字 <= second，更新 second
        // 關鍵不變量: 此時存在某個過去的 first 值 < 這個新的 nums[i]
        // (過去的 first 可能不同於當前 first，但已被記錄為 second 的上界)
        else if (nums[i] <= second) {
            second = nums[i];  // 更新 second: 記錄在舊 first 之後找到的中間值
        }
        // 條件3: 否則 nums[i] > second (必然也 > first)
        // 此時已確保存在三個索引 i1, i2, i (按序)，滿足:
        // nums[i1] < nums[i2] < nums[i] (i1 記錄於 first, i2 記錄於 second)
        else {
            printf("TRUE\n");
            return true;  // 找到遞增三元組！三元組確實存在
        }
    }

    // 遍歷完整個陣列後仍未找到三元組
    printf("FALSE\n");
    return false;  // 不存在遞增三元子序列
}
