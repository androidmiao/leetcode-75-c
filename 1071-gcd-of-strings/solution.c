/*
 * 1071. Greatest Common Divisor of Strings（字串的最大公因數）
 * 難度：Easy
 * 時間複雜度：O(m + n)（最佳情況），O(min(m,n) * (m+n))（最差情況）
 * 空間複雜度：O(n)，其中 n 為較短字串的長度
 *
 * 解法：暴力前綴檢查（Brute Force Prefix Check）
 *
 * 核心觀念：
 *   GCD 字串必定是較短字串的某個前綴。從較短字串本身開始，
 *   逐步縮短候選長度，直到找到能同時整除兩個字串的前綴。
 *
 * 演算法步驟（以 str1="ABCABC", str2="ABC" 為例）：
 *
 *   Step 1: 確保 str1 是較長的字串
 *     str1 = "ABCABC" (len=6)
 *     str2 = "ABC"    (len=3)
 *     str1 >= str2 ✓ 不需交換
 *
 *   Step 2: 複製 str2 到 ret 作為初始候選
 *     ret = "ABC" (len=3)
 *
 *   Step 3: 找到能同時整除兩個長度的最大 len
 *     len=3: 6%3==0 且 3%3==0 ✓ → len=3
 *
 *   Step 4: 驗證 ret[0..len-1] 是否能重複覆蓋 str1
 *     pass 0: "ABC" vs str1[0..2] = "ABC" ✓
 *     pass 1: "ABC" vs str1[3..5] = "ABC" ✓
 *     → str1 驗證通過
 *
 *   Step 5: 驗證 ret[0..len-1] 是否能重複覆蓋 str2
 *     pass 0: "ABC" vs str2[0..2] = "ABC" ✓
 *     → str2 驗證通過
 *
 *   Step 6: 回傳 "ABC"
 *
 * 圖解範例 (str1="ABABAB", str2="ABAB"):
 *
 *   候選 ret = "ABAB" (len=4)
 *   檢查長度整除性：
 *     6 % 4 = 2 ≠ 0  → len-- → len=3
 *     6 % 3 = 0, 4 % 3 = 1 ≠ 0  → len-- → len=2
 *     6 % 2 = 0, 4 % 2 = 0  → ✓ 進入驗證
 *
 *   驗證 str1="ABABAB" 以 ret[0..1]="AB" 重複：
 *     ┌────┬────┬────┐
 *     │ AB │ AB │ AB │  ← str1 分成 3 段
 *     └────┴────┴────┘
 *     pass 0: "AB" == "AB" ✓
 *     pass 1: "AB" == "AB" ✓
 *     pass 2: "AB" == "AB" ✓
 *
 *   驗證 str2="ABAB" 以 ret[0..1]="AB" 重複：
 *     ┌────┬────┐
 *     │ AB │ AB │  ← str2 分成 2 段
 *     └────┴────┘
 *     pass 0: "AB" == "AB" ✓
 *     pass 1: "AB" == "AB" ✓
 *
 *   → 回傳 "AB"
 */

#include "solution.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *gcdOfStrings(char *str1, char *str2) {
  const size_t str1len = strlen(str1); /* str1 的長度 */
  const size_t str2len = strlen(str2); /* str2 的長度 */

  /* 遞迴確保 str1 是較長的字串，簡化後續邏輯 */
  if (str1len < str2len) {
    return gcdOfStrings(str2, str1);
  }

  /* 此處保證 str1 長度 >= str2 長度 */

  size_t len = str2len; /* 候選 GCD 字串的長度，從 str2 的完整長度開始 */
  char *ret = malloc(str2len + 1); /* 分配空間儲存候選字串 */
  strcpy(ret, str2); /* 將 str2 複製為初始候選 */

  /* 逐步縮小 len，直到 len 同時整除 str1len 和 str2len */
  while (str1len % len != 0 || str2len % len != 0) {
    len--;
  }

  /* 第一輪：驗證 ret[0..len-1] 是否能重複覆蓋 str1 */
  size_t pass = 0;
  do {
    for (size_t i = 0; i < len; i++) {
      /* 比較候選的第 i 個字元與 str1 中對應位置的字元 */
      if (*(ret + i) != *(str1 + pass * len + i)) {
        if (pass > 0) {
          /* 第一輪之後的不匹配 → 候選已確定但無法覆蓋，直接失敗 */
          len = 0;
          goto terminate;
        }
        /* 第一輪中不匹配 → 縮短候選長度再試 */
        if (len > 0)
          len--;
        if (i > 0)
          i--; /* 退回一步重新比對 */
        continue;
      }
    }
    pass++;
  } while (pass < str1len / len); /* 重複直到覆蓋完整個 str1 */

  /* 第二輪：驗證 ret[0..len-1] 是否能重複覆蓋 str2 */
  pass = 0;
  do {
    for (size_t i = 0; i < len; i++) {
      /* 比較候選的第 i 個字元與 str2 中對應位置的字元 */
      if (*(ret + i) != *(str2 + pass * len + i)) {
        if (pass > 0) {
          /* 第一輪之後的不匹配 → 直接失敗 */
          len = 0;
          goto terminate;
        }
        /* 第一輪中不匹配 → 縮短候選長度再試 */
        if (len > 0)
          len--;
        if (i > 0)
          i--;
        continue;
      }
    }
    pass++;
  } while (pass < str2len / len); /* 重複直到覆蓋完整個 str2 */

terminate:
  ret = realloc(ret, len + 1); /* 調整記憶體大小為最終結果長度 */
  ret[len] = '\0'; /* 加上空字元結尾 */

  return ret; /* 回傳 GCD 字串（若 len=0 則為空字串） */
}
