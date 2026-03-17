//https://leetcode.com/problems/binary-search/description/?envType=problem-list-v2&envId=wl94y6ih

// 題目：704. Binary Search
// 解法：Binary Search（二分搜尋）
// 時間複雜度：O(log n)
// 空間複雜度：O(1)
//
// 圖解說明：
// 以 nums = [-1, 0, 3, 5, 9, 12]，target = 9 為例
//
// 索引：   0   1   2   3   4   5
// 陣列：  -1   0   3   5   9  12
//
// 第 1 次搜尋：
// left = 0, right = 5
// mid = 0 + (5 - 0) / 2 = 2
//
// 索引：   0   1   2   3   4   5
// 陣列：  -1   0   3   5   9  12
//                ^
//               mid
//
// nums[mid] = 3，因為 3 < 9
// 代表 target 只可能在右半邊
// 所以更新：left = mid + 1 = 3
//
// 第 2 次搜尋：
// left = 3, right = 5
// mid = 3 + (5 - 3) / 2 = 4
//
// 索引：   0   1   2   3   4   5
// 陣列：  -1   0   3   5   9  12
//                        ^
//                       mid
//
// nums[mid] = 9，剛好等於 target
// 所以回傳索引 4
//
// 核心觀念：
// 1. 每次都檢查中間位置 mid
// 2. 如果 target 比 nums[mid] 大，就往右半邊找
// 3. 如果 target 比 nums[mid] 小，就往左半邊找
// 4. 每找一次，都把搜尋範圍縮小一半
//
// 為什麼速度快？
// 因為不是一個一個找，而是每次直接砍掉一半區間
// 所以時間複雜度是 O(log n)

int search(int* nums, int numsSize, int target) {
    // left 表示目前搜尋區間的左邊界（包含 left）
    int left = 0;

    // right 表示目前搜尋區間的右邊界（包含 right）
    int right = numsSize - 1;

    // 只要 left 沒有超過 right，就代表搜尋區間還有效
    while (left <= right) {
        // 取中間位置
        // 用 left + (right - left) / 2 可以避免 left + right 直接相加時的整數溢位風險
        int mid = left + (right - left) / 2;

        // 如果中間值剛好等於 target，直接回傳索引 mid
        if (nums[mid] == target) {
            return mid;
        }

        // 如果中間值小於 target，代表 target 只可能出現在右半邊
        if (nums[mid] < target) {
            // 把左邊界移到 mid 的右邊，縮小搜尋範圍
            left = mid + 1;
        } else {
            // 否則代表 nums[mid] > target，target 只可能出現在左半邊
            // 把右邊界移到 mid 的左邊，縮小搜尋範圍
            right = mid - 1;
        }
    }

    // 迴圈結束後仍然找不到 target，表示它不在陣列中，回傳 -1
    return -1;
}
