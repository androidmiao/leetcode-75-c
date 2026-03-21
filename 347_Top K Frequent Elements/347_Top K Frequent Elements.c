//https://leetcode.com/problems/top-k-frequent-elements/description/?envType=problem-list-v2&envId=wl94y6ih
/*
 * 347. Top K Frequent Elements
 * LeetCode function signature:
 *     int* topKFrequent(int* nums, int numsSize, int k, int* returnSize)
 *
 * 解法：Bucket Sort + Frequency Counting
 *
 * 核心想法：
 * 1. 先算出每個數字出現幾次。
 * 2. 題目保證 nums[i] 落在 [-10000, 10000]，
 *    所以我們可以直接用固定大小陣列記錄頻率。
 * 3. 再把「出現次數相同」的數字放進同一個 bucket。
 * 4. 最後從高頻 bucket 往低頻 bucket 掃描，先拿到的就是答案。
 *
 * 為什麼 bucket 可行？
 * - 一個數字最多只可能出現 numsSize 次。
 * - 所以 frequency 的範圍一定在 [1, numsSize]。
 * - 也就是說，我們可以開 numsSize + 1 個 bucket，
 *   bucket[3] 就代表「所有出現 3 次的數字」。
 *
 * 圖解範例：
 * nums = [1,1,1,2,2,3], k = 2
 *
 * 第一步：計算頻率
 * value:      1   2   3
 * frequency:  3   2   1
 *
 * 第二步：依頻率分桶
 * bucket[1] -> [3]
 * bucket[2] -> [2]
 * bucket[3] -> [1]
 *
 * 第三步：從大頻率往小頻率掃
 * bucket[6] -> []
 * bucket[5] -> []
 * bucket[4] -> []
 * bucket[3] -> [1]   先拿 1
 * bucket[2] -> [2]   再拿 2
 *
 * 最後答案：
 * [1, 2]
 *
 * 時間複雜度：
 * - 計數：O(n)
 * - 建 bucket：O(n)
 * - 取答案：O(n)
 * 整體為 O(n)
 *
 * 空間複雜度：
 * - frequency 陣列：O(1) 相對於題目固定值域
 * - unique / bucket 結構：O(n)
 * 整體視為 O(n)
 */

#include <stdlib.h>

int* topKFrequent(int* nums, int numsSize, int k, int* returnSize) {
    /* 題目限制 nums[i] 落在 [-10000, 10000]。 */
    enum { OFFSET = 10000, RANGE = 20001 };

    /* 開一個固定大小的頻率表。 */
    int* frequencies = calloc(RANGE, sizeof(int));

    /* 用來記錄「有哪些不同的數字」出現過。 */
    int* uniqueValues = malloc((size_t)numsSize * sizeof(int));

    /* uniqueCount 表示目前一共有多少個不同數字。 */
    int uniqueCount = 0;

    /* 逐一掃描原始陣列。 */
    for (int i = 0; i < numsSize; i++) {
        /* 因為 nums[i] 可能是負數，所以加 OFFSET 轉成非負索引。 */
        int index = nums[i] + OFFSET;

        /* 如果這個值以前沒出現過，就把它放進 uniqueValues。 */
        if (frequencies[index] == 0) {
            uniqueValues[uniqueCount++] = nums[i];
        }

        /* 把這個值的出現次數加 1。 */
        frequencies[index]++;
    }

    /* bucketSizes[f] 表示「出現 f 次的數字一共有幾個」。 */
    int* bucketSizes = calloc((size_t)numsSize + 1, sizeof(int));

    /* 根據每個不同數字的頻率，先統計各 bucket 需要多大空間。 */
    for (int i = 0; i < uniqueCount; i++) {
        /* 找出 uniqueValues[i] 對應的頻率索引。 */
        int index = uniqueValues[i] + OFFSET;

        /* frequency 就是這個數字出現的次數。 */
        int frequency = frequencies[index];

        /* 對應 bucket 的容量需求加 1。 */
        bucketSizes[frequency]++;
    }

    /* buckets[f] 會指向「所有出現 f 次的數字」所組成的陣列。 */
    int** buckets = calloc((size_t)numsSize + 1, sizeof(int*));

    /* 根據剛才統計好的 bucketSizes，真正配置每個 bucket 的空間。 */
    for (int frequency = 1; frequency <= numsSize; frequency++) {
        /* 只有需要的 bucket 才配置記憶體。 */
        if (bucketSizes[frequency] > 0) {
            buckets[frequency] =
                malloc((size_t)bucketSizes[frequency] * sizeof(int));
        }
    }

    /* bucketWriteIndex[f] 表示 bucket[f] 目前已經寫到哪個位置。 */
    int* bucketWriteIndex = calloc((size_t)numsSize + 1, sizeof(int));

    /* 把每個不同數字實際放進對應頻率的 bucket。 */
    for (int i = 0; i < uniqueCount; i++) {
        /* 取出目前這個不同數字。 */
        int value = uniqueValues[i];

        /* 找出它的出現次數。 */
        int frequency = frequencies[value + OFFSET];

        /* 寫入 bucket[frequency] 的下一個可用位置。 */
        buckets[frequency][bucketWriteIndex[frequency]++] = value;
    }

    /* 配置答案陣列，大小剛好是 k。 */
    int* answer = malloc((size_t)k * sizeof(int));

    /* answerIndex 表示目前已經收集了幾個答案。 */
    int answerIndex = 0;

    /* 從最高頻率往下掃，先遇到的一定是更高頻的元素。 */
    for (int frequency = numsSize; frequency >= 1 && answerIndex < k; frequency--) {
        /* 依序取出 bucket[frequency] 裡的所有值。 */
        for (int i = 0; i < bucketWriteIndex[frequency] && answerIndex < k; i++) {
            /* 把高頻元素放進答案陣列。 */
            answer[answerIndex++] = buckets[frequency][i];
        }
    }

    /* LeetCode 需要我們把答案長度寫回 returnSize。 */
    if (returnSize != NULL) {
        *returnSize = k;
    }

    /* 逐一釋放每個 bucket。 */
    for (int frequency = 1; frequency <= numsSize; frequency++) {
        free(buckets[frequency]);
    }

    /* 釋放其他中間資料結構。 */
    free(buckets);
    free(bucketWriteIndex);
    free(bucketSizes);
    free(uniqueValues);
    free(frequencies);

    /* 回傳結果陣列給 LeetCode。 */
    return answer;
}
