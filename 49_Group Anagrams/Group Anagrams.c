/*
 * LeetCode 49. Group Anagrams（異位詞分組）
 * 難度：Medium
 * 時間複雜度：O(NK log K)，N = 字串個數，K = 最長字串長度
 * 空間複雜度：O(NK)
 *
 * ===== 演算法：排序字串 + qsort 分組 =====
 *
 * 核心思想：
 *   異位詞排序後一定相同，例如 "eat", "tea", "ate" 排序後都是 "aet"。
 *   因此可以用「排序後的字串」作為分組依據（canonical key）。
 *
 * 由於 C 語言沒有內建 HashMap，本解法改用 qsort 排序所有
 * (sorted_key, original_string) 配對，讓相同異位詞在排序後相鄰，
 * 再一次線性掃描即可分組。
 *
 * ===== 逐步演算流程（以 Example 1 為例）=====
 *
 * 輸入：strs = ["eat", "tea", "tan", "ate", "nat", "bat"]
 *
 * 步驟一：對每個字串排序，建立 (sorted, original) 配對
 *
 *   原始字串    排序後
 *   ─────────  ─────────
 *   "eat"   →  "aet"
 *   "tea"   →  "aet"
 *   "tan"   →  "ant"
 *   "ate"   →  "aet"
 *   "nat"   →  "ant"
 *   "bat"   →  "abt"
 *
 * 步驟二：按 sorted key 排序所有配對（相同 key 按原始字串排序）
 *
 *   排序後      原始字串
 *   ─────────  ─────────
 *   "abt"      "bat"       ← 群組 1
 *   "aet"      "ate"       ← 群組 2 開始
 *   "aet"      "eat"       │
 *   "aet"      "tea"       ← 群組 2 結束
 *   "ant"      "nat"       ← 群組 3 開始
 *   "ant"      "tan"       ← 群組 3 結束
 *
 * 步驟三：線性掃描，連續相同 sorted key 收入同一群組
 *
 *   result[0] = ["bat"]
 *   result[1] = ["ate", "eat", "tea"]
 *   result[2] = ["nat", "tan"]
 *
 * ===== ASCII 圖示：pairs 陣列排序前後 =====
 *
 * 排序前 pairs[]:
 * ┌───────────────────────────────────────────────────┐
 * │ [0] ("aet","eat")  [1] ("aet","tea")              │
 * │ [2] ("ant","tan")  [3] ("aet","ate")              │
 * │ [4] ("ant","nat")  [5] ("abt","bat")              │
 * └───────────────────────────────────────────────────┘
 *
 * 排序後 pairs[]（qsort by sorted key, then by original）:
 * ┌───────────────────────────────────────────────────┐
 * │ [0] ("abt","bat")                                  │  ← group 0
 * │ [1] ("aet","ate")  [2] ("aet","eat")              │
 * │ [3] ("aet","tea")                                  │  ← group 1
 * │ [4] ("ant","nat")  [5] ("ant","tan")              │  ← group 2
 * └───────────────────────────────────────────────────┘
 *        ▲                                    ▲
 *        │ strcmp 比較 sorted key              │
 *        │ 不同 → 開新群組                     │
 *        │ 相同 → 加入當前群組                 │
 *
 * ===== 記憶體配置示意 =====
 *
 *  result (char***)
 *  ┌─────────┬─────────┬─────────┐
 *  │ grp[0]  │ grp[1]  │ grp[2]  │
 *  └────┬────┴────┬────┴────┬────┘
 *       │         │         │
 *       ▼         ▼         ▼
 *    char**     char**     char**
 *   ┌──────┐  ┌──────┐  ┌──────┐
 *   │"bat" │  │"ate" │  │"nat" │
 *   └──────┘  │"eat" │  │"tan" │
 *             │"tea" │  └──────┘
 *             └──────┘
 */

#include <stdlib.h>
#include <string.h>

/* 比較函數：用於 qsort 排序字串中的字元（按 ASCII 值升序）*/
int compare_char(const void* a, const void* b) {
    return *(char*)a - *(char*)b;  /* 字元比較，回傳差值決定順序 */
}

/* 比較函數：用於 qsort 排序 (sorted_key, original) 配對陣列 */
/* 先依排序後字串比較；若相同，再依原始字串比較（確保穩定性） */
int compare_pairs(const void* a, const void* b) {
    const char** pair_a = (const char**)a;  /* pair_a[0]=sorted, pair_a[1]=original */
    const char** pair_b = (const char**)b;

    /* 先比較排序後的字串（canonical key） */
    int cmp = strcmp(pair_a[0], pair_b[0]);
    if (cmp != 0) return cmp;

    /* 若 sorted key 相同，比較原始字串（使同組內字串有確定順序） */
    return strcmp(pair_a[1], pair_b[1]);
}

/**
 * groupAnagrams - 將異位詞分組
 * @strs:              輸入字串陣列
 * @strsSize:          字串個數
 * @returnSize:        [輸出] 群組數量
 * @returnColumnSizes: [輸出] 每個群組包含的字串數量
 *
 * 回傳值：char***，即二維字串陣列（每個群組是一個 char** 陣列）
 */
char*** groupAnagrams(char** strs, int strsSize, int* returnSize, int** returnColumnSizes) {
    /* 邊界情況：空輸入直接回傳 */
    if (strsSize == 0) {
        *returnSize = 0;
        *returnColumnSizes = (int*)malloc(0);
        return (char***)malloc(0);
    }

    /* ====== 步驟一：建立 (sorted_key, original) 配對 ====== */

    /* 分配暫存陣列：sorted_strs[i] = strs[i] 排序後的副本 */
    char** sorted_strs = (char**)malloc(strsSize * sizeof(char*));
    /* original_strs[i] = strs[i] 的原始指標（不複製，僅引用） */
    char** original_strs = (char**)malloc(strsSize * sizeof(char*));

    for (int i = 0; i < strsSize; i++) {
        int str_len = strlen(strs[i]);

        /* 複製字串以便排序（不破壞原始資料） */
        sorted_strs[i] = (char*)malloc((str_len + 1) * sizeof(char));
        strcpy(sorted_strs[i], strs[i]);

        /* 對副本中的字元按 ASCII 值排序 → 得到 canonical key */
        /* 例如 "eat" → "aet"，"tea" → "aet" */
        qsort(sorted_strs[i], str_len, sizeof(char), compare_char);

        /* 保存指向原始字串的指標 */
        original_strs[i] = strs[i];
    }

    /* ====== 步驟二：將所有配對按 sorted key 排序 ====== */

    /* pairs[i] 是一個 char*[2]：pairs[i][0]=sorted, pairs[i][1]=original */
    char*** pairs = (char***)malloc(strsSize * sizeof(char**));
    for (int i = 0; i < strsSize; i++) {
        pairs[i] = (char**)malloc(2 * sizeof(char*));
        pairs[i][0] = sorted_strs[i];   /* 排序後的字串（作為分組鍵） */
        pairs[i][1] = original_strs[i];  /* 原始字串（最終放入結果） */
    }

    /* 按 sorted key 排序 → 相同異位詞在陣列中相鄰 */
    qsort(pairs, strsSize, sizeof(char**), compare_pairs);

    /* ====== 步驟三：線性掃描建立分組結果 ====== */

    /* result[i] = 第 i 個群組的字串指標陣列 */
    char*** result = (char***)malloc(strsSize * sizeof(char**));
    /* col_sizes[i] = 第 i 個群組包含多少個字串 */
    int* col_sizes = (int*)malloc(strsSize * sizeof(int));

    int group_count = 0;         /* 目前群組總數 */
    int current_group_size = 0;  /* 當前群組已收集的字串數 */
    char* current_sorted = NULL; /* 當前群組的 sorted key（用於比較） */

    for (int i = 0; i < strsSize; i++) {
        char* sorted = pairs[i][0];    /* 當前配對的 sorted key */
        char* original = pairs[i][1];  /* 當前配對的原始字串 */

        /* 若 sorted key 與前一個不同 → 進入新的群組 */
        if (current_sorted == NULL || strcmp(sorted, current_sorted) != 0) {
            group_count++;
            /* 為新群組分配指標陣列（最多 strsSize 個元素） */
            result[group_count - 1] = (char**)malloc(strsSize * sizeof(char*));
            col_sizes[group_count - 1] = 0;
            current_group_size = 0;
            current_sorted = sorted;  /* 更新當前群組的 key */
        }

        /* 將原始字串加入當前群組 */
        result[group_count - 1][current_group_size] = original;
        col_sizes[group_count - 1]++;
        current_group_size++;
    }

    /* ====== 步驟四：釋放暫存記憶體 ====== */
    for (int i = 0; i < strsSize; i++) {
        free(sorted_strs[i]);  /* 釋放排序後的字串副本 */
        free(pairs[i]);        /* 釋放配對結構 */
    }
    free(sorted_strs);
    free(original_strs);
    free(pairs);

    /* 設定回傳參數 */
    *returnSize = group_count;
    *returnColumnSizes = col_sizes;

    return result;
}
