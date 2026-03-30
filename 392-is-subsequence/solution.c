/*
 * 問題 #392: Is Subsequence
 * 難度: Easy
 *
 * 時間複雜度: O(n)，其中 n = len(t)
 * 空間複雜度: O(1)
 *
 * 演算法說明:
 * 使用雙指標掃描法來判斷 s 是否為 t 的子序列。
 * - s_idx: 追蹤 s 中目前對應的字符位置
 * - t_idx: 掃描 t 中的每個字符
 * 當 s[s_idx] == t[t_idx] 時，我們找到了一個匹配，將 s_idx 向前移動。
 * 如果 s_idx 達到 s 的長度，表示整個 s 都被找到了。
 *
 * 範例 1: s="abc", t="ahbgdc"
 * 步驟演示:
 *
 *     初始化:
 *     s: [a][b][c]
 *        ^
 *     t: [a][h][b][g][d][c]
 *        ^
 *     s_idx=0, t_idx=0
 *
 *     步驟 0: t[0]='a' == s[0]='a' ✓
 *     s: [a][b][c]
 *           ^
 *     t: [a][h][b][g][d][c]
 *        ^
 *     動作: s_idx++ → s_idx=1
 *
 *     步驟 1: t[1]='h' != s[1]='b'
 *     t: [a][h][b][g][d][c]
 *           ^
 *     動作: 無; t_idx++ → t_idx=2
 *
 *     步驟 2: t[2]='b' == s[1]='b' ✓
 *     s: [a][b][c]
 *           ^
 *     t: [a][h][b][g][d][c]
 *              ^
 *     動作: s_idx++ → s_idx=2
 *
 *     步驟 3: t[3]='g' != s[2]='c'
 *     t: [a][h][b][g][d][c]
 *                 ^
 *     動作: 無; t_idx++ → t_idx=4
 *
 *     步驟 4: t[4]='d' != s[2]='c'
 *     t: [a][h][b][g][d][c]
 *                    ^
 *     動作: 無; t_idx++ → t_idx=5
 *
 *     步驟 5: t[5]='c' == s[2]='c' ✓
 *     s: [a][b][c]
 *              ^
 *     t: [a][h][b][g][d][c]
 *                       ^
 *     動作: s_idx++ → s_idx=3
 *           s_idx == s_len(3)，回傳 true
 *
 * 範例 2: s="axc", t="ahbgdc"
 * 步驟演示:
 *
 *     初始化:
 *     s: [a][x][c]
 *        ^
 *     t: [a][h][b][g][d][c]
 *        ^
 *     s_idx=0, t_idx=0
 *
 *     步驟 0: t[0]='a' == s[0]='a' ✓
 *     s: [a][x][c]
 *           ^
 *     t: [a][h][b][g][d][c]
 *        ^
 *     動作: s_idx++ → s_idx=1
 *
 *     步驟 1: t[1]='h' != s[1]='x'
 *     步驟 2: t[2]='b' != s[1]='x'
 *     步驟 3: t[3]='g' != s[1]='x'
 *     步驟 4: t[4]='d' != s[1]='x'
 *     步驟 5: t[5]='c' != s[1]='x'
 *
 *     迴圈結束，s_idx=1 < s_len(3)
 *     回傳 false：'x' 在 t 中不存在，s 不是 t 的子序列
 */

#include "solution.h"
#include <stddef.h>
#include <string.h>

bool isSubsequence(char* s, char* t) {
    /* 計算 s 和 t 的長度 */
    size_t s_len = strlen(s);
    size_t t_len = strlen(t);

    /* 邊界情況: 若兩個字串皆為空，視為 s 是 t 的子序列 */
    if (s_len == 0 && t_len == 0) {
        return true;
    }

    /* s_idx: 指向 s 中目前要匹配的字符位置; 不變式: s[0..s_idx-1] 都已在 t 中找到 */
    size_t s_idx = 0;

    /* 迭代遍歷 t 中的每個字符，尋找 s 中的字符 */
    for (size_t t_idx = 0; t_idx < t_len; t_idx++) {
        /* 若當前 t 的字符與 s 的待匹配字符相同，則推進 s_idx */
        if (s[s_idx] == t[t_idx]) {
            s_idx++;  /* 保維 s_idx: 已匹配的 s 字符數量增加 */
        }

        /* 若 s_idx 已達 s 的長度，表示 s 的所有字符都已找到，立即回傳 true */
        if (s_idx == s_len) return true;
    }

    /* 若迴圈完成仍未找到所有 s 的字符，回傳 false */
    return false;
}
