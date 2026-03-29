/*
 * LeetCode 11: Container With Most Water
 * Difficulty: Medium
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 *
 * ALGORITHM WALKTHROUGH (Example: height = [1,8,6,2,5,4,8,3,7])
 *
 * 使用雙指標技巧(Two-Pointer Technique):
 * - 初始化左指標在陣列開頭,右指標在陣列末尾
 * - 計算當前位置的面積:面積 = min(height[left], height[right]) × (right - left)
 * - 更新最大面積
 * - 將較短邊的指標向中間移動(因為較長邊不會改善面積)
 * - 重複直到指標相遇
 *
 * STEP-BY-STEP WITH EXAMPLE:
 *
 * height = [1,8,6,2,5,4,8,3,7]
 * index    0 1 2 3 4 5 6 7 8
 *
 * Step 1:  left=0, right=8
 *          |                 | <- 指標位置
 *          1 8 6 2 5 4 8 3 7
 *          面積 = min(1,7) × (8-0) = 1 × 8 = 8
 *          height[0]=1 < height[8]=7, 移動左指標
 *
 * Step 2:  left=1, right=8
 *             |              |
 *          1 8 6 2 5 4 8 3 7
 *          面積 = min(8,7) × (8-1) = 7 × 7 = 49  ✓ 最大面積
 *          height[1]=8 > height[8]=7, 移動右指標
 *
 * Step 3:  left=1, right=7
 *             |           |
 *          1 8 6 2 5 4 8 3 7
 *          面積 = min(8,3) × (7-1) = 3 × 6 = 18
 *          height[1]=8 > height[7]=3, 移動右指標
 *
 * Step 4:  left=1, right=6
 *             |        |
 *          1 8 6 2 5 4 8 3 7
 *          面積 = min(8,8) × (6-1) = 8 × 5 = 40
 *          height[1]=8 = height[6]=8, 進入 else 分支,移動左指標
 *
 * ...繼續直到left >= right
 *
 * 最終答案: 49
 */

#include "solution.h"
#include <math.h>
#include <stddef.h>

int maxArea(int* height, int height_size) {
    int left = 0, right = height_size - 1;  /* 初始化雙指標:左指標在起點,右指標在末尾 */
    int area = 0;  /* 儲存最大面積 */

    while (right - left > 0) {  /* 當指標未相遇時,繼續迴圈 */
        int new_area = (int) fmin(height[left], height[right]) * (right - left);  /* 計算當前容器面積:高度×寬度 */

        area = new_area > area ? new_area : area;  /* 更新最大面積 */

        if (height[left] > height[right]) {  /* 若左邊更高,說明右邊是瓶頸 */
            right--;  /* 移動右指標,尋求更高的右邊界 */
        }
        else {  /* 否則右邊更高或相等 */
            left++;  /* 移動左指標,尋求更高的左邊界 */
       }
    }

    return area;  /* 回傳最大面積 */
}
