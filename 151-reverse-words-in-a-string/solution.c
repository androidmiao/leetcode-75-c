/*
 * LeetCode Problem 151: Reverse Words in a String
 * Difficulty: Medium
 * Time Complexity: O(n) - one full string reversal + one pass for word processing
 * Space Complexity: O(1) - in-place transformation, only pointers used
 *
 * ALGORITHM WALKTHROUGH (Example: "the sky is blue  ")
 *
 * Step 1: Initial state
 *   Input: "the sky is blue  " (length 17, with trailing spaces)
 *   Goal: Reverse word order while removing extra spaces
 *
 * Step 2: Full string reversal
 *   After reverse(s, 0, 16): "  eulb si yks eht"
 *   - Every character is flipped, including spaces
 *   - Word boundaries are reversed (rightmost word is now leftmost)
 *
 * Step 3: Iterate through, reverse individual words and compact
 *   State progression:
 *
 *   Initial (after full reversal):
 *     Index: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 *     Chars:                 e u l b   s i   y  k  s     e  h  t
 *     left=0, right=0, k=0
 *
 *   After processing word "eulb" (reverse to "blue"):
 *     Index: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 *     Chars: b l u e   s i   y  k  s     e  h  t
 *     left=5, k=5 (space added at k=4 before copying)
 *
 *   After processing word "si" (reverse to "is"):
 *     Index: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 *     Chars: b l u e   i s   y  k  s     e  h  t
 *     left=8, k=8 (space added at k=7 before copying)
 *
 *   After processing word "yks" (reverse to "sky"):
 *     Index: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 *     Chars: b l u e   i s   s  k  y     e  h  t
 *     left=12, k=11 (space added at k=11 before copying)
 *
 *   After processing word "eht" (reverse to "the"):
 *     Index: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 *     Chars: b l u e   i s   s  k  y   t  h  e
 *     left=17 ('\0'), k=14
 *
 * Step 4: Null-terminate at position k
 *   s[14] = '\0'
 *   Final output: "blue is sky the"
 *
 * KEY INVARIANTS:
 * - After first reversal, words appear in reverse order but reversed internally
 * - `left` marks the start of the current word being processed
 * - `right` marks the end of the current word (exclusive, points to space or '\0')
 * - `k` marks the write position for the compacted result (removes extra spaces)
 * - At each iteration, we reverse the current word to correct its direction
 *   and copy it to position k (with leading space if not first word)
 */

#include <stddef.h>
#include <string.h>

void swap_xor(char* a, char* b) {
    // 檢查是否為同一位址，避免自身與自身互斥異或導致歸零
    if (a == b)
        return;
    // 使用異或運算交換：a = a^b, b = a^b (此時b被修改), a = a^b
    *a = *a ^ *b;  // a現在包含原始a和b的異或
    *b = *a ^ *b;  // b = (a^b)^b = a，得到原始a的值
    *a = *a ^ *b;  // a = (a^b)^a = b，得到原始b的值
}

void reverse(char* s, short left, short right) {
    // 迴圈不變式：[0, left)和(right, n)的元素已交換，[left, right]待交換
    while (left < right) {
        // 交換left和right位置的字元
        swap_xor(&s[left], &s[right]);
        // left指針向右移動一位，逼近中心
        left++;
        // right指針向左移動一位，逼近中心
        right--;
    }
}

char* reverseWords(char* s) {
    // n為整個字串的長度（包括所有空格），使用strlen計算
    size_t n = strlen(s);

    // 第一步：將整個字串完全反轉，使得字詞順序反轉
    // 反轉後每個字詞內部也被反轉，稍後需要再次反轉各字詞
    reverse(s, 0, (short) n - 1);

    // left：指向當前待處理字詞的起始位置
    // right：指向當前待處理字詞之後的位置（第一個非字詞字元，通常是空格或'\0'）
    // k：指向輸出字串的寫入位置，用於移除多餘空格並緊湊結果
    short left = 0, right = 0, k = 0;

    // 迴圈遍歷整個字串，直到到達字串結尾的空字元
    while (s[left] != '\0') {

        // 如果當前字元是空格，跳過它，left向前移動
        // 目的是跳過所有連續的空格以尋找下一個字詞
        if (s[left] == ' ') {
            left++;
            continue;
        }

        // 此時s[left]是一個非空格字元，為當前字詞的起始
        right = left;

        // 找到當前字詞的結尾：掃描直到遇到空格或字串結尾
        // 迴圈不變式：[left, right)內的字元都不是空格，s[right]是空格或'\0'
        while (s[right] != ' ' && s[right] != '\0') {
            right++;
        }

        // 反轉當前字詞（從left到right-1）以修正其方向
        // 因為整個字串已被反轉，字詞內部方向也被反轉，現在需要修正
        reverse(s, left, right - 1);

        // 如果k不為0，表示已經有一個字詞被寫入，需要先寫入空格分隔
        // k為0時為第一個字詞，不需要前導空格
        if (k != 0) {
            s[k++] = ' ';
        }
        // 複製當前字詞到k位置，逐字元複製並推進k指標
        // 此過程同時去除所有多餘空格，因為我們只複製非空格字元
        for (short i = left; i < right; i++) {
            s[k++] = s[i];
        }

        // left移動到right位置，準備處理下一個字詞
        // right現在指向空格或'\0'，left移動到此位置以繼續掃描
        left = right;

    }
    // 在緊湊後的字串末尾寫入空字元，標記字串結尾
    s[k] = '\0';

    // 返回修改後的字串指標
    return s;
}
