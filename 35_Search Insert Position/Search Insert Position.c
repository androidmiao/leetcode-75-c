/*
 * 35. Search Insert Position
 * Difficulty: Easy
 * Time Complexity:  O(log n)  — 每次砍半搜尋空間
 * Space Complexity: O(1)      — 只用常數指標
 *
 * ─────────────────────────────────────────────────────────────────
 * 演算法：Binary Search（二分搜尋）
 * ─────────────────────────────────────────────────────────────────
 *
 * 核心想法
 * --------
 * 對已排序且無重複元素的陣列做二分搜尋：
 *   - 找到 target → 回傳其索引
 *   - 找不到 target → 回傳應插入的位置（即 left）
 *
 * 「為何找不到時回傳 left？」
 * 當迴圈結束時，right < left，此時：
 *   nums[right] < target < nums[left]
 * 所以 target 應插在 left 的位置（把原本的元素往右移）。
 *
 * ASCII 示意圖
 * -----------
 * 例：nums = [1, 3, 5, 6], target = 2
 *
 *   索引:   0   1   2   3
 *   值:    [1] [3] [5] [6]
 *
 *   第 1 輪: left=0, right=3, pivot=1, nums[1]=3
 *            target(2) < nums[pivot](3) → right = pivot-1 = 0
 *
 *   第 2 輪: left=0, right=0, pivot=0, nums[0]=1
 *            target(2) > nums[pivot](1) → left = pivot+1 = 1
 *
 *   left(1) > right(0) → 跳出迴圈
 *   回傳 left = 1  ← 插入位置（在 3 的前面）
 *
 * 例：nums = [1, 3, 5, 6], target = 5
 *
 *   第 1 輪: left=0, right=3, pivot=1, nums[1]=3
 *            target(5) > 3 → left = 2
 *   第 2 輪: left=2, right=3, pivot=2, nums[2]=5
 *            target == nums[pivot] → 回傳 2
 *
 * ─────────────────────────────────────────────────────────────────
 * 整數溢位注意事項
 * ---------------
 * pivot = left + (right - left) / 2
 * 不使用 (left + right) / 2，避免兩者之和超過 INT_MAX。
 * ─────────────────────────────────────────────────────────────────
 */
int searchInsert(int* nums, int numsSize, int target) {
    int left  = 0;           /* 搜尋區間左端點（含） */
    int right = numsSize - 1; /* 搜尋區間右端點（含） */

    while (left <= right) {
        /* 避免整數溢位：用加偏移而非直接相加 */
        int pivot = left + (right - left) / 2;

        if (nums[pivot] == target) {
            return pivot; /* 找到目標，直接回傳索引 */
        } else if (nums[pivot] < target) {
            left = pivot + 1;  /* 目標在右半段，縮小左邊界 */
        } else {
            right = pivot - 1; /* 目標在左半段，縮小右邊界 */
        }
    }

    /*
     * 迴圈結束時 right < left，此刻：
     *   nums[right] < target <= nums[left]（或 left 已超出陣列）
     * 插入位置即為 left
     */
    return left;
}
