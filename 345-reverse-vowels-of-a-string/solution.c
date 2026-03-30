/*
 * 345. Reverse Vowels of a String (Easy)
 *
 * 時間複雜度：O(n)  —  左右指標各最多遍歷字串一次
 * 空間複雜度：O(n)  —  malloc 了一份字串副本作為回傳值
 *
 * ========== 演算法概述 ==========
 *
 * 使用「雙指標」技巧：一個從左往右、一個從右往左，
 * 各自跳過非母音字元，遇到母音就交換，直到兩指標交叉。
 *
 * ========== 範例追蹤：s = "IceCreAm" ==========
 *
 *  索引:   0   1   2   3   4   5   6   7
 *  字元:   I   c   e   C   r   e   A   m
 *          L                           R      ← 初始狀態
 *
 *  步驟 1: L=0('I') 是母音 ✓，R=7('m') 不是 → R-- → R=6('A') 是母音 ✓
 *          交換 s[0] 與 s[6]：
 *          A   c   e   C   r   e   I   m
 *              L                   R          ← L++, R--
 *
 *  步驟 2: L=1('c') 不是母音 → L++ → L=2('e') 是母音 ✓
 *          R=5('e') 是母音 ✓
 *          交換 s[2] 與 s[5]：
 *          A   c   e   C   r   e   I   m      ← 'e' 與 'e' 交換，值不變
 *                  L           R              ← L++, R--
 *
 *  步驟 3: L=3('C') 不是母音 → L++ → L=4('r') 不是母音 → L++
 *          L=5 ≥ R=4 → 迴圈結束
 *
 *  結果: "AceCreIm"
 *
 * ========== 指標移動示意圖 ==========
 *
 *    L ───────────────────────▶
 *    ┌───┬───┬───┬───┬───┬───┬───┬───┐
 *    │ I │ c │ e │ C │ r │ e │ A │ m │
 *    └───┴───┴───┴───┴───┴───┴───┴───┘
 *    ◄─────────────────────────────── R
 *
 *    母音位置: 0(I)  2(e)  5(e)  6(A)
 *    交換配對: [0,6] → I↔A     [2,5] → e↔e
 */

#include "solution.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* 判斷字元 c 是否為母音字母（大小寫皆算） */
bool isVowel(char c) {
    switch (c) {
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'i':
    case 'I':
    case 'o':
    case 'O':
    case 'u':
    case 'U':
        return true;   /* 匹配任何一個母音即回傳 true */
    default:
        return false;  /* 非母音回傳 false */
    }
}

/*
 * XOR 交換法：不需暫存變數即可交換兩個 char 的值。
 * 前提：a 與 b 不能指向同一記憶體位址，否則會歸零。
 */
void swap_xor(char* a, char* b) {
    if (a == b) return;  /* 同一位址時直接返回，避免歸零 */

    *a = *a ^ *b;  /* 步驟 1：a 存入 a⊕b */
    *b = *a ^ *b;  /* 步驟 2：b = (a⊕b)⊕b = a → b 現在是原始的 a */
    *a = *a ^ *b;  /* 步驟 3：a = (a⊕b)⊕a = b → a 現在是原始的 b */
}

char* reverseVowels(char* s) {

    /* 配置新字串：因為 LeetCode 的 C 介面預期回傳新記憶體 */
    char* str = malloc(strlen(s) + 1);
    strcpy(str, s);  /* 將原始字串複製到新配置的空間 */

    char* s_lptr = str;                    /* 左指標：從字串開頭出發 */
    char* s_rptr = str + strlen(s) - 1;    /* 右指標：從字串結尾出發 */

    /* 當左指標尚未超過右指標時，持續搜尋並交換母音 */
    while (s_lptr < s_rptr) {
        if (isVowel(*s_lptr)) {
            /* 左指標已在母音上 → 從右邊找下一個母音 */
            while (!isVowel(*s_rptr)) {
                s_rptr--;  /* 右指標向左跳過非母音字元 */
            }

            swap_xor(s_lptr, s_rptr);  /* 交換左右兩端的母音 */
            s_rptr--;  /* 右指標內縮，避免下次重複交換同一個已歸位的母音 */
        }
        s_lptr++;  /* 左指標無條件右移一格（不論是否剛交換過） */
    }

    return str;  /* 回傳反轉母音後的新字串 */
}
