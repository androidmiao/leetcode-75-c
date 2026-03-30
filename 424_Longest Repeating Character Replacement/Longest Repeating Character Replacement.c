/*
 * LeetCode 424. 最長重複字元替換
 * 難度：Medium
 *
 * 問題描述：
 * 給定一個字符串 s 和一個整數 k，你可以選擇字符串中的任何字符，將其更改為任何其他字符。
 * 在最多進行 k 次更改操作後，找出包含重複字符的最長子字符串的長度。
 *
 * 演算法：滑動視窗 (Sliding Window)
 *
 * 核心思想：
 * - 使用兩個指針維護一個滑動視窗
 * - 統計視窗內每個字符出現的頻率
 * - 關鍵不等式：(視窗長度 - 最大頻率字符數) <= k
 *   即：需要修改的字符數 <= k
 * - 當不滿足時，收縮左邊界；否則嘗試擴展右邊界
 *
 * 複雜度分析：
 * - 時間複雜度：O(n)，左右指針各遍歷一次數組
 * - 空間複雜度：O(1)，只需26個字母的頻率計數
 *
 * 具體演算法步驟：
 * 1. 初始化雙指針 left=0, right=0，字符頻率數組 freq[26]={0}，最大答案 max_len=0
 * 2. 遍歷字符串，right指針移動，更新 freq[s[right]-'A']++
 * 3. 同時記錄視窗內最大頻率 max_freq
 * 4. 當視窗長度 - max_freq > k 時，說明需要修改的字符超過k個，收縮視窗
 *    left指針右移，更新 freq[s[left]-'A']--
 * 5. 更新最長長度：max_len = max(max_len, right - left + 1)
 * 6. 返回 max_len
 *
 * ═══════════════════════════════════════════════════════════════════════════════════════
 * 範例1：s = "AABABBA", k = 1
 * ═══════════════════════════════════════════════════════════════════════════════════════
 *
 * 步驟 1：right=0, 字符='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *   ↑
 * left=0, right=0
 * freq[A]=1, max_freq=1, window_len=1, need_change=0 ✓
 * max_len=1
 *
 * 步驟 2：right=1, 字符='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *   └───┘
 * left=0, right=1
 * freq[A]=2, max_freq=2, window_len=2, need_change=0 ✓
 * max_len=2
 *
 * 步驟 3：right=2, 字符='B'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *   └───┴───┘
 * left=0, right=2
 * freq[A]=2, freq[B]=1, max_freq=2, window_len=3, need_change=1 ✓
 * max_len=3
 *
 * 步驟 4：right=3, 字符='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *   └───┴───┴───┘
 * left=0, right=3
 * freq[A]=3, freq[B]=1, max_freq=3, window_len=4, need_change=1 ✓
 * max_len=4
 *
 * 步驟 5：right=4, 字符='B'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *   └───┴───┴───┴───┘
 * left=0, right=4
 * freq[A]=3, freq[B]=2, max_freq=3, window_len=5, need_change=2 ✗ (超過k=1)
 * 需要收縮視窗
 *
 * 步驟 5a：left=0→1，移除's[0]'='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *       └───┴───┴───┘
 * left=1, right=4
 * freq[A]=2, freq[B]=2, max_freq=2, window_len=4, need_change=2 ✗
 * 繼續收縮視窗
 *
 * 步驟 5b：left=1→2，移除's[1]'='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *           └───┴───┘
 * left=2, right=4
 * freq[A]=1, freq[B]=2, max_freq=2, window_len=3, need_change=1 ✓
 * max_len=4 (不變)
 *
 * 步驟 6：right=5, 字符='B'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *           └───┴───┴───┘
 * left=2, right=5
 * freq[A]=1, freq[B]=3, max_freq=3, window_len=4, need_change=1 ✓
 * max_len=4 (不變)
 *
 * 步驟 7：right=6, 字符='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *           └───┴───┴───┴───┘
 * left=2, right=6
 * freq[A]=2, freq[B]=3, max_freq=3, window_len=5, need_change=2 ✗
 * 需要收縮視窗
 *
 * 步驟 7a：left=2→3，移除's[2]'='B'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *               └───┴───┴───┘
 * left=3, right=6
 * freq[A]=2, freq[B]=2, max_freq=2, window_len=4, need_change=2 ✗
 * 繼續收縮視窗
 *
 * 步驟 7b：left=3→4，移除's[3]'='A'
 * ┌───┬───┬───┬───┬───┬───┬───┐
 * │ A │ A │ B │ A │ B │ B │ A │
 * └───┴───┴───┴───┴───┴───┴───┘
 *                   └───┴───┘
 * left=4, right=6
 * freq[A]=1, freq[B]=2, max_freq=2, window_len=3, need_change=1 ✓
 * max_len=4 (不變)
 *
 * 最終答案：max_len = 4 (可以是 "AAAA" 或 "BBBA" 等)
 *
 * ═══════════════════════════════════════════════════════════════════════════════════════
 * 範例2：s = "ABAB", k = 2
 * ═══════════════════════════════════════════════════════════════════════════════════════
 *
 * 步驟 1→2：right=0→1
 * ┌───┬───┬───┬───┐
 * │ A │ B │ A │ B │
 * └───┴───┴───┴───┘
 *   └───┴───┘
 * left=0, right=1
 * freq[A]=1, freq[B]=1, max_freq=1, window_len=2, need_change=1 ✓
 * max_len=2
 *
 * 步驟 3：right=2
 * ┌───┬───┬───┬───┐
 * │ A │ B │ A │ B │
 * └───┴───┴───┴───┘
 *   └───┴───┴───┘
 * left=0, right=2
 * freq[A]=2, freq[B]=1, max_freq=2, window_len=3, need_change=1 ✓
 * max_len=3
 *
 * 步驟 4：right=3
 * ┌───┬───┬───┬───┐
 * │ A │ B │ A │ B │
 * └───┴───┴───┴───┘
 *   └───┴───┴───┴───┘
 * left=0, right=3
 * freq[A]=2, freq[B]=2, max_freq=2, window_len=4, need_change=2 ✓
 * max_len=4 (可以將所有字符改為'A'或'B')
 *
 * 最終答案：max_len = 4
 *
 */

int characterReplacement(char* s, int k) {
    /* 初始化26個字母的頻率計數數組 */
    int freq[26] = {0};

    /* left 是滑動視窗的左指針 */
    int left = 0;

    /* max_freq 記錄視窗內出現最多的字符的頻率 */
    int max_freq = 0;

    /* max_len 是答案，記錄最長有效視窗的長度 */
    int max_len = 0;

    /* 遍歷字符串，right是滑動視窗的右指針 */
    for (int right = 0; s[right] != '\0'; right++) {
        /* 將當前字符的頻率增加1 */
        /* s[right]-'A' 將字符轉換為0-25的索引 */
        freq[s[right] - 'A']++;

        /* 更新當前視窗內最大的字符頻率 */
        if (freq[s[right] - 'A'] > max_freq) {
            max_freq = freq[s[right] - 'A'];
        }

        /* 計算需要修改的字符數 = 視窗長度 - 最大頻率 */
        /* 當這個值 > k 時，說明不能通過k次修改來讓所有字符相同，需要收縮視窗 */
        int window_len = right - left + 1;
        int need_change = window_len - max_freq;

        if (need_change > k) {
            /* 收縮視窗左邊界 */
            /* 首先減少左邊界字符的頻率計數 */
            freq[s[left] - 'A']--;
            /* 左指針右移 */
            left++;
        }

        /* 更新最長的有效視窗長度 */
        /* 當前視窗長度 = right - left + 1 */
        int current_len = right - left + 1;
        if (current_len > max_len) {
            max_len = current_len;
        }
    }

    /* 返回最長子字符串的長度 */
    return max_len;
}
