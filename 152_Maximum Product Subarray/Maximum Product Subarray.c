/*
 * ============================================================
 *  LeetCode 152. Maximum Product Subarray（最大乘積子陣列）
 *  難度：Medium
 *  演算法：動態規劃 — 同時追蹤以當前位置結尾的最大與最小乘積
 *  時間複雜度：O(n)    空間複雜度：O(1)
 * ============================================================
 *
 *  【核心觀察】
 *  乘積與加法不同，負數 × 負數 = 正數，因此：
 *  - 目前為止的「最小乘積」在遇到負數時，可能一翻成為「最大乘積」
 *  - 必須同時追蹤 max_prod 與 min_prod
 *
 *  【演算法步驟 — 以 nums = [2, 3, -2, 4] 為例】
 *
 *  初始化：max_prod = min_prod = result = nums[0] = 2
 *
 *  i=1  nums[1]=3
 *       候選值：3, max_prod*3=6, min_prod*3=6
 *       max_prod = max(3, 6, 6) = 6
 *       min_prod = min(3, 6, 6) = 3
 *       result   = max(6, 2)    = 6
 *
 *  i=2  nums[2]=-2
 *       候選值：-2, max_prod*(-2)=-12, min_prod*(-2)=-6
 *       temp_max = max(-2, -12, -6) = -2
 *       min_prod = min(-2, -12, -6) = -12
 *       max_prod = temp_max = -2
 *       result   = max(-2, 6)       = 6
 *
 *  i=3  nums[3]=4
 *       候選值：4, max_prod*4=-8, min_prod*4=-48
 *       temp_max = max(4, -8, -48) = 4
 *       min_prod = min(4, -8, -48) = -48
 *       max_prod = temp_max = 4
 *       result   = max(4, 6)       = 6
 *
 *  最終答案：6（子陣列 [2, 3]）
 *
 *  【ASCII 狀態圖 — nums = [2, 3, -2, 4]】
 *
 *   index:    0       1       2       3
 *   nums:  [ 2 ]   [ 3 ]   [-2 ]   [ 4 ]
 *            │       │       │       │
 *   max:     2 ───▶  6 ───▶ -2 ───▶  4
 *   min:     2 ───▶  3 ───▶-12 ───▶-48
 *   result:  2 ───▶  6 ───▶  6 ───▶  6
 *                    ▲
 *                    └── 最終答案 = 6
 *
 *  【零的影響 — nums = [-2, 0, -1]】
 *
 *   index:    0       1       2
 *   nums:  [-2 ]   [ 0 ]   [-1 ]
 *            │       │       │
 *   max:    -2 ───▶  0 ───▶  0
 *   min:    -2 ───▶  0 ───▶  0
 *   result: -2 ───▶  0 ───▶  0
 *                            ▲
 *     零把乘積鏈完全重置 ─────┘  答案 = 0
 *
 * ============================================================
 */

#include <limits.h>

/* 三元素取最大值的輔助巨集 */
static inline int max3(int a, int b, int c) {
    int m = (a > b) ? a : b;       /* 先比較前兩個 */
    return (m > c) ? m : c;        /* 再與第三個比較 */
}

/* 三元素取最小值的輔助巨集 */
static inline int min3(int a, int b, int c) {
    int m = (a < b) ? a : b;       /* 先比較前兩個 */
    return (m < c) ? m : c;        /* 再與第三個比較 */
}

int maxProduct(int* nums, int numsSize) {
    if (numsSize == 0) return 0;   /* 防禦性檢查，題目保證 n ≥ 1 */

    /* 以第一個元素初始化三個追蹤變數 */
    int max_prod = nums[0];        /* 以當前位置結尾的最大乘積 */
    int min_prod = nums[0];        /* 以當前位置結尾的最小乘積 */
    int result   = nums[0];        /* 全域最大乘積（最終答案） */

    for (int i = 1; i < numsSize; i++) {
        int curr = nums[i];        /* 當前元素 */

        /*
         * 計算三個候選值：
         *   1) curr            — 從此處重新開始子陣列
         *   2) max_prod * curr — 延續正乘積鏈
         *   3) min_prod * curr — 負×負 翻轉恢復
         *
         * 必須用 temp_max 暫存，因為計算 min_prod 時
         * 還需要用到「舊的」max_prod 值
         */
        int temp_max = max3(curr, max_prod * curr, min_prod * curr);
        min_prod     = min3(curr, max_prod * curr, min_prod * curr);
        max_prod     = temp_max;    /* 覆蓋為新的最大乘積 */

        /* 更新全域最大值 */
        if (max_prod > result) {
            result = max_prod;
        }
    }

    return result;                  /* 回傳最大乘積子陣列的乘積 */
}
