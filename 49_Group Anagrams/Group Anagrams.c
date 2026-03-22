/*
 * LeetCode 49. 異位詞分組
 * 難度：Medium
 *
 * 問題描述：
 * 給定一個字符串數組，將異位詞組合在一起。異位詞是指由相同字符組成但順序不同的單詞。
 *
 * 演算法：雜湊表 + 字符串排序
 *
 * 核心思想：
 * - 對每個字符串進行排序，得到標準形式(sorted key)
 * - 相同的異位詞在排序後會有相同的標準形式
 * - 使用雜湊表(或在C中使用數組模擬)將標準形式作為鍵，將所有異位詞分組
 * - 最後將各組收集起來返回
 *
 * 複雜度分析：
 * - 時間複雜度：O(n * m log m)
 *   其中 n 是字符串個數，m 是最長字符串長度
 *   每個字符串排序需要 O(m log m)
 * - 空間複雜度：O(n * m)
 *   存儲所有字符串的副本(排序後的形式)
 */

#include <stdlib.h>
#include <string.h>

/* 比較函數：用於排序字符串中的字符 */
int compare_char(const void* a, const void* b) {
    return *(char*)a - *(char*)b;
}

/* 比較函數：用於排序結構體數組 */
/* 首先按排序後的字符串比較，相同則按原始字符串比較 */
int compare_pairs(const void* a, const void* b) {
    const char** pair_a = (const char**)a;
    const char** pair_b = (const char**)b;

    /* 先比較排序後的字符串(pair[0]) */
    int cmp = strcmp(pair_a[0], pair_b[0]);
    if (cmp != 0) return cmp;

    /* 如果排序後的字符串相同，再比較原始字符串(pair[1]) */
    return strcmp(pair_a[1], pair_b[1]);
}

/**
 * 返回值是一個指向字符串指針的指針的指針的數組，行數和列數通過指針返回
 */
char*** groupAnagrams(char** strs, int strsSize, int* returnSize, int** returnColumnSizes) {
    /* 特殊處理：空輸入 */
    if (strsSize == 0) {
        *returnSize = 0;
        *returnColumnSizes = (int*)malloc(0);
        return (char***)malloc(0);
    }

    /* 創建臨時數組存儲(排序後的字符串, 原始字符串)對 */
    /* 使用二維數組：pairs[i][0] = 排序後的字符串，pairs[i][1] = 原始字符串 */
    char** sorted_strs = (char**)malloc(strsSize * sizeof(char*));
    char** original_strs = (char**)malloc(strsSize * sizeof(char*));

    /* 第一步：對每個字符串進行排序，創建(排序字符串, 原始字符串)配對 */
    for (int i = 0; i < strsSize; i++) {
        int str_len = strlen(strs[i]);

        /* 復制字符串以便排序 */
        sorted_strs[i] = (char*)malloc((str_len + 1) * sizeof(char));
        strcpy(sorted_strs[i], strs[i]);

        /* 對復制的字符串進行排序 */
        /* qsort 使用 compare_char 比較函數按ASCII值排序字符 */
        qsort(sorted_strs[i], str_len, sizeof(char), compare_char);

        /* 保存原始字符串指針 */
        original_strs[i] = strs[i];
    }

    /* 第二步：按排序後的字符串排序，將異位詞組織在一起 */
    /* 創建配對數組用於排序 */
    char*** pairs = (char***)malloc(strsSize * sizeof(char**));
    for (int i = 0; i < strsSize; i++) {
        pairs[i] = (char**)malloc(2 * sizeof(char*));
        pairs[i][0] = sorted_strs[i];
        pairs[i][1] = original_strs[i];
    }

    /* 根據排序後的字符串進行排序 */
    /* 這樣相同的異位詞會被組織在一起 */
    qsort(pairs, strsSize, sizeof(char**), compare_pairs);

    /* 第三步：構造返回的二維數組 */
    /* 結果數組：result[i] 存儲第i個異位詞組的所有單詞 */
    char*** result = (char***)malloc(strsSize * sizeof(char**));

    /* 列大小數組：每個異位詞組有多少個單詞 */
    int* col_sizes = (int*)malloc(strsSize * sizeof(int));

    /* 記錄當前有多少個組 */
    int group_count = 0;

    /* 每個組當前有多少個單詞 */
    int current_group_size = 0;

    /* 當前組的排序字符串(用於判斷異位詞是否相同) */
    char* current_sorted = NULL;

    /* 遍歷排序後的配對數組 */
    for (int i = 0; i < strsSize; i++) {
        char* sorted = pairs[i][0];
        char* original = pairs[i][1];

        /* 如果這是新的一組(排序字符串與前一個不同) */
        if (current_sorted == NULL || strcmp(sorted, current_sorted) != 0) {
            /* 創建新組 */
            group_count++;

            /* 分配該組的字符串指針數組 */
            /* 初始分配，之後會根據需要調整 */
            result[group_count - 1] = (char**)malloc(strsSize * sizeof(char*));

            col_sizes[group_count - 1] = 0;
            current_group_size = 0;
            current_sorted = sorted;
        }

        /* 將原始字符串添加到當前組 */
        result[group_count - 1][current_group_size] = original;
        col_sizes[group_count - 1]++;
        current_group_size++;
    }

    /* 第四步：清理臨時數組 */
    for (int i = 0; i < strsSize; i++) {
        free(sorted_strs[i]);
        free(pairs[i]);
    }
    free(sorted_strs);
    free(original_strs);
    free(pairs);

    /* 設置返回的行數和列大小 */
    *returnSize = group_count;
    *returnColumnSizes = col_sizes;

    return result;
}
