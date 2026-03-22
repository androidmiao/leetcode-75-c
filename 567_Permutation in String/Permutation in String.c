/*
 * 567. Permutation in String
 * Difficulty: Medium
 * Time Complexity:  O(l1 + l2)  — 建立頻率表 O(l1)，滑窗掃描 O(l2)
 * Space Complexity: O(1)         — 固定大小的 26 個字元頻率表
 *
 * ─────────────────────────────────────────────────────────────────
 * 演算法：Fixed-Size Sliding Window + Frequency Count（固定大小滑動視窗）
 * ─────────────────────────────────────────────────────────────────
 *
 * 核心想法
 * --------
 * s1 的任意排列（Permutation）都有相同的字元頻率。
 * 若 s2 中有一個長度為 len(s1) 的子字串與 s1 的字元頻率相同，
 * 表示該子字串是 s1 的某個排列，回傳 true。
 *
 * 做法：
 * 1. 建立 s1 的頻率表 count1[26]
 * 2. 用固定大小為 len(s1) 的視窗掃描 s2
 * 3. 每次右移視窗：加入新字元，移除最舊字元
 * 4. 維護 matches 計數器（count1[i] == count2[i] 的字元種數）
 *    → matches == 26 時，兩個頻率表完全相同 → 回傳 true
 *
 * ASCII 示意圖
 * -----------
 * 例：s1 = "ab", s2 = "eidbaooo"
 *
 *   count1: a=1, b=1, 其他=0
 *
 *   視窗大小 = 2
 *   s2: e i d b a o o o
 *       ^ ^               視窗 "ei"：count2 e=1,i=1   matches≠26
 *         ^ ^             視窗 "id"：count2 i=1,d=1   matches≠26
 *           ^ ^           視窗 "db"：count2 d=1,b=1   matches≠26
 *             ^ ^         視窗 "ba"：count2 b=1,a=1   matches==26 → true!
 *
 * matches 維護邏輯（加入新字元 c 時）：
 *   先更新 count2[c]++
 *   若更新後 count2[c] == count1[c] → matches++
 *   若更新後 count2[c] == count1[c] + 1 → matches--（之前相等，現在多了一個）
 * ─────────────────────────────────────────────────────────────────
 */
#include <stdbool.h>
#include <string.h>

bool checkInclusion(char* s1, char* s2) {
    int l1 = (int)strlen(s1); /* s1 的長度 */
    int l2 = (int)strlen(s2); /* s2 的長度 */

    if (l1 > l2) return false; /* s1 比 s2 長，不可能是子字串 */

    int count1[26] = {0}; /* s1 的字元頻率表 */
    int count2[26] = {0}; /* 當前視窗的字元頻率表 */

    /* 建立 s1 的頻率表，並初始化視窗（前 l1 個字元） */
    for (int i = 0; i < l1; i++) {
        count1[s1[i] - 'a']++;
        count2[s2[i] - 'a']++;
    }

    /* 計算初始 matches 數：count1[i] == count2[i] 的字元種數 */
    int matches = 0;
    for (int i = 0; i < 26; i++) {
        if (count1[i] == count2[i]) matches++;
    }

    /* 滑動視窗：右端加入 s2[r]，左端移除 s2[r - l1] */
    for (int r = l1; r < l2; r++) {
        if (matches == 26) return true; /* 所有字元頻率相同 → 找到排列 */

        int in  = s2[r] - 'a';          /* 新加入視窗的字元 */
        int out = s2[r - l1] - 'a';     /* 移出視窗的字元 */

        /* 加入新字元：先更新 count2，再更新 matches */
        count2[in]++;
        if (count2[in] == count1[in]) {
            matches++; /* 更新後恰好與 count1 相等 → matches 增加 */
        } else if (count2[in] == count1[in] + 1) {
            matches--; /* 更新前相等，更新後多了一個 → matches 減少 */
        }

        /* 移出舊字元：先更新 count2，再更新 matches */
        count2[out]--;
        if (count2[out] == count1[out]) {
            matches++; /* 更新後恰好與 count1 相等 → matches 增加 */
        } else if (count2[out] == count1[out] - 1) {
            matches--; /* 更新前相等，更新後少了一個 → matches 減少 */
        }
    }

    return matches == 26; /* 檢查最後一個視窗 */
}
