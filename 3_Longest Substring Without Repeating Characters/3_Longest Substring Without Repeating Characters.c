//https://leetcode.com/problems/longest-substring-without-repeating-characters/description/?envType=problem-list-v2&envId=wl94y6ih

// 題目：3. Longest Substring Without Repeating Characters
// 解法：Sliding Window（滑動視窗）
// 時間複雜度：O(n)
// 空間複雜度：O(1) ，因為字元集合大小固定
//
// 圖解說明：
// 我們用一個「不含重複字元的視窗」在字串上往右滑動。
// left  是視窗左邊界
// right 是視窗右邊界
//
// 例子：s = "abcabcbb"
//
// 索引:   0   1   2   3   4   5   6   7
// 字元:   a   b   c   a   b   c   b   b
//
// Step 1: right = 0，看到 'a'
// 視窗: [a]
//       L,R
// 目前最長長度 = 1
//
// Step 2: right = 1，看到 'b'
// 視窗: [a b]
//       L   R
// 目前最長長度 = 2
//
// Step 3: right = 2，看到 'c'
// 視窗: [a b c]
//       L     R
// 目前最長長度 = 3
//
// Step 4: right = 3，看到 'a'
// 'a' 在目前視窗內重複了，之前出現在索引 0
// 所以 left 要跳到 0 + 1 = 1
// 視窗從 [a b c] 變成 [b c a]
//          L     R
// 目前最長長度仍然 = 3
//
// Step 5: right = 4，看到 'b'
// 'b' 在目前視窗內重複，之前出現在索引 1
// 所以 left 跳到 1 + 1 = 2
// 視窗變成 [c a b]
//            L   R
// 目前最長長度仍然 = 3
//
// 之後持續同樣操作：
// 1. right 每次向右走一格
// 2. 若字元重複，而且重複位置仍在視窗內，就把 left 跳過去
// 3. 每次更新目前視窗長度 right - left + 1
// 4. 用 maxLen 記錄最大值
//
// 為什麼 lastSeen[] 很重要？
// 因為它可以讓我們「直接知道重複字元上次出現在哪裡」，
// 不需要回頭重新掃描整個視窗，所以整體才是 O(n)。

int lengthOfLongestSubstring(char* s) {
    // lastSeen[c] 記錄字元 c 上一次出現的索引位置
    // 初始設為 -1，表示還沒出現過
    int lastSeen[128];

    // left 表示目前滑動視窗的左邊界
    int left = 0;

    // maxLen 記錄目前找到的最長不重複子字串長度
    int maxLen = 0;

    // right 用來一路往右掃描字串
    int right = 0;

    // 先把所有字元的上次出現位置初始化為 -1
    for (int i = 0; i < 128; i++) {
        lastSeen[i] = -1;
    }

    // 逐一處理字串中的每個字元，直到遇到字串結尾 '\0'
    while (s[right] != '\0') {
        // 轉成 unsigned char，避免 char 在某些編譯器下是 signed 時發生索引問題
        unsigned char currentChar = (unsigned char)s[right];

        // 如果目前字元曾經出現在視窗內
        // 就把 left 移到「上次出現位置的下一格」
        if (lastSeen[currentChar] >= left) {
            left = lastSeen[currentChar] + 1;
        }

        // 更新目前字元最後一次出現的位置
        lastSeen[currentChar] = right;

        // 計算目前視窗長度
        int currentLen = right - left + 1;

        // 如果目前視窗更長，就更新答案
        if (currentLen > maxLen) {
            maxLen = currentLen;
        }

        // 繼續往右掃描下一個字元
        right++;
    }

    // 回傳最長不重複子字串的長度
    return maxLen;
}
