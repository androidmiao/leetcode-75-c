/*
 * 33. Search in Rotated Sorted Array
 * Difficulty: Medium
 * Time Complexity:  O(log n)  — 單次二分搜尋
 * Space Complexity: O(1)      — 只用常數指標
 *
 * ─────────────────────────────────────────────────────────────────
 * 演算法：One-Pass Binary Search（單次二分搜尋判斷有序半段）
 * ─────────────────────────────────────────────────────────────────
 *
 * 核心想法
 * --------
 * 旋轉後的陣列雖然整體不是有序的，但在任一個 mid 點，
 * 必然有一半是「連續有序」的。
 * 利用這個性質，每次判斷哪一半有序，再決定往哪半搜尋。
 *
 * 判斷規則：
 *   1. 若 nums[l] <= nums[mid]，左半段 [l, mid] 是有序的：
 *      - 若 nums[l] <= target < nums[mid]，target 在左半段 → r = mid - 1
 *      - 否則 target 在右半段 → l = mid + 1
 *   2. 否則，右半段 [mid, r] 是有序的：
 *      - 若 nums[mid] < target <= nums[r]，target 在右半段 → l = mid + 1
 *      - 否則 target 在左半段 → r = mid - 1
 *
 * ASCII 示意圖
 * -----------
 * 例：nums = [4, 5, 6, 7, 0, 1, 2], target = 0
 *
 *   旋轉點（pivot/最小值）在索引 4（值=0）
 *   陣列：[4] [5] [6] [7] [0] [1] [2]
 *          0    1   2   3   4   5   6
 *
 *   第 1 輪: l=0, r=6, mid=3, nums[3]=7
 *            nums[0]=4 <= nums[3]=7 → 左半段有序 [4,5,6,7]
 *            target=0 不在 [4,7] 範圍 → l = 4
 *
 *   第 2 輪: l=4, r=6, mid=5, nums[5]=1
 *            nums[4]=0 <= nums[5]=1 → 左半段有序 [0,1]
 *            target=0 在 [0,1) 範圍（0 <= 0 < 1）→ r = 4
 *
 *   第 3 輪: l=4, r=4, mid=4, nums[4]=0 == target → 回傳 4
 *
 * 例：nums = [4, 5, 6, 7, 0, 1, 2], target = 3
 *
 *   搜尋結果：-1（不在陣列中）
 * ─────────────────────────────────────────────────────────────────
 */
int search(int* nums, int numsSize, int target) {
    int l = 0;             /* 搜尋區間左端點 */
    int r = numsSize - 1;  /* 搜尋區間右端點 */

    while (l <= r) {
        int mid = l + (r - l) / 2; /* 避免整數溢位 */

        if (nums[mid] == target) {
            return mid; /* 直接命中 */
        }

        /* 判斷左半段 [l, mid] 是否有序 */
        if (nums[l] <= nums[mid]) {
            /* 左半段有序：判斷 target 是否落在有序的左半段 */
            if (nums[l] <= target && target < nums[mid]) {
                r = mid - 1; /* target 在左半段 */
            } else {
                l = mid + 1; /* target 在右半段 */
            }
        } else {
            /* 右半段 [mid, r] 有序：判斷 target 是否落在有序的右半段 */
            if (nums[mid] < target && target <= nums[r]) {
                l = mid + 1; /* target 在右半段 */
            } else {
                r = mid - 1; /* target 在左半段 */
            }
        }
    }

    return -1; /* target 不在陣列中 */
}
