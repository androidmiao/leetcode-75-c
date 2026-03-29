/*
 * ============================================================================
 * LeetCode 739. Daily Temperatures （每日溫度）
 * 難度：Medium
 * 演算法：單調遞減棧（Monotonic Decreasing Stack）
 * 時間複雜度：O(n) — 每個索引最多被 push 和 pop 各一次
 * 空間複雜度：O(n) — stack 和 result 陣列各需 n 個 int
 * ============================================================================
 *
 * 【題目】
 * 給定一個整數陣列 temperatures，回傳陣列 answer，其中 answer[i] 是從第 i 天
 * 起要等幾天才能遇到更高的溫度。若往後都沒有更高溫度的日子，answer[i] = 0。
 *
 * 【核心觀察】
 * 暴力法對每天向後掃描找更暖天 → O(n²)，太慢。
 * 如果連續幾天溫度遞減，它們的「答案天」可能是同一天。
 * 用一個棧來「延遲」回答：遇到更暖天時，一次性回答棧裡所有較冷的天。
 *
 * 【演算法步驟（以 Example 1 為例）】
 * temperatures = [73, 74, 75, 71, 69, 72, 76, 73]
 *                  0   1   2   3   4   5   6   7   ← 索引
 *
 * 棧存的是「索引」，棧底到棧頂對應的溫度是遞減的。
 *
 * i=0: temp=73, 棧空 → push 0
 *      stack: [0]                    result: [0,0,0,0,0,0,0,0]
 *
 * i=1: temp=74 > temp[0]=73 → pop 0, result[0]=1-0=1; push 1
 *      stack: [1]                    result: [1,0,0,0,0,0,0,0]
 *
 * i=2: temp=75 > temp[1]=74 → pop 1, result[1]=2-1=1; push 2
 *      stack: [2]                    result: [1,1,0,0,0,0,0,0]
 *
 * i=3: temp=71 < temp[2]=75 → push 3
 *      stack: [2,3]                  result: [1,1,0,0,0,0,0,0]
 *
 * i=4: temp=69 < temp[3]=71 → push 4
 *      stack: [2,3,4]                result: [1,1,0,0,0,0,0,0]
 *
 * i=5: temp=72 > temp[4]=69 → pop 4, result[4]=5-4=1
 *      temp=72 > temp[3]=71 → pop 3, result[3]=5-3=2; push 5
 *      stack: [2,5]                  result: [1,1,0,2,1,0,0,0]
 *
 * i=6: temp=76 > temp[5]=72 → pop 5, result[5]=6-5=1
 *      temp=76 > temp[2]=75 → pop 2, result[2]=6-2=4; push 6
 *      stack: [6]                    result: [1,1,4,2,1,1,0,0]
 *
 * i=7: temp=73 < temp[6]=76 → push 7
 *      stack: [6,7]                  result: [1,1,4,2,1,1,0,0]
 *
 * 遍歷結束，棧裡剩餘的索引 6, 7 對應的 result 保持 0（無更暖天）。
 *
 * 【ASCII 圖示：棧的變化過程】
 *
 *  索引 i:   0    1    2    3    4    5       6       7
 *  溫度:    73   74   75   71   69   72      76      73
 *
 *  棧狀態（底→頂）：
 *
 *  i=0: │ 0 │
 *       └───┘
 *
 *  i=1: │ 1 │          pop 0 → result[0]=1
 *       └───┘
 *
 *  i=2: │ 2 │          pop 1 → result[1]=1
 *       └───┘
 *
 *  i=3: │ 2 │ 3 │
 *       └───┴───┘
 *
 *  i=4: │ 2 │ 3 │ 4 │
 *       └───┴───┴───┘
 *
 *  i=5: │ 2 │ 5 │      pop 4 → result[4]=1, pop 3 → result[3]=2
 *       └───┴───┘
 *
 *  i=6: │ 6 │          pop 5 → result[5]=1, pop 2 → result[2]=4
 *       └───┘
 *
 *  i=7: │ 6 │ 7 │
 *       └───┴───┘
 *
 * 最終 result = [1, 1, 4, 2, 1, 1, 0, 0] ✓
 * ============================================================================
 */

#include <stdlib.h>

int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    /* 配置結果陣列，calloc 自動初始化為 0（沒有更暖天的預設值） */
    int* result = (int*)calloc(temperaturesSize, sizeof(int));

    /* 配置棧陣列，最壞情況需要存放所有 n 個索引（溫度單調遞減時） */
    int* stack = (int*)malloc(temperaturesSize * sizeof(int));

    /* top 指向棧頂元素的索引，-1 表示棧為空 */
    int top = -1;

    for (int i = 0; i < temperaturesSize; i++) {
        /*
         * 核心邏輯：當棧不為空，且當前溫度大於棧頂索引對應的溫度時，
         * 代表我們找到了棧頂那天的「第一個更暖天」。
         * 用 while 而非 if，因為當前天可能同時是多個較冷天的答案。
         */
        while (top >= 0 && temperatures[stack[top]] < temperatures[i]) {
            int prev_index = stack[top--]; /* 彈出棧頂索引 */
            result[prev_index] = i - prev_index; /* 天數差 = 當前索引 - 棧頂索引 */
        }

        /* 將當前日的索引推入棧，維持棧的單調遞減性質 */
        stack[++top] = i;
    }

    free(stack); /* 釋放工作用的棧記憶體 */
    *returnSize = temperaturesSize; /* 設定回傳陣列長度 */
    return result;
}
