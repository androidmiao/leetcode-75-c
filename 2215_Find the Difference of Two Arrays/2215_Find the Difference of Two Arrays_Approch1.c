/*
實作重點：
用雙層概念做 brute force membership check（exists_in_array）。
透過檢查 out 內容避免重複，滿足 distinct 要求。
回傳格式符合 LeetCode C 介面（returnSize = 2、returnColumnSizes）。
編譯驗證：

cc -std=c11 -Wall -Wextra -Werror -c ... 成功。
*/

#include <stdlib.h>
#include <stdbool.h>

static bool exists_in_array(const int* arr, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return true;
        }
    }
    return false;
}

static int collect_only_in_first(
    const int* first,
    int first_size,
    const int* second,
    int second_size,
    int* out
) {
    int count = 0;

    for (int i = 0; i < first_size; i++) {
        int v = first[i];

        // Distinct requirement: skip duplicates already added to output.
        if (exists_in_array(out, count, v)) {
            continue;
        }

        // Brute force membership check in second array.
        if (!exists_in_array(second, second_size, v)) {
            out[count++] = v;
        }
    }

    return count;
}

int** findDifference(
    int* nums1,
    int nums1Size,
    int* nums2,
    int nums2Size,
    int* returnSize,
    int** returnColumnSizes
) {
    int* tmp1 = (int*)malloc((size_t)nums1Size * sizeof(int));
    int* tmp2 = (int*)malloc((size_t)nums2Size * sizeof(int));

    int c1 = collect_only_in_first(nums1, nums1Size, nums2, nums2Size, tmp1);
    int c2 = collect_only_in_first(nums2, nums2Size, nums1, nums1Size, tmp2);

    int** ans = (int**)malloc(2 * sizeof(int*));
    ans[0] = (int*)malloc((size_t)c1 * sizeof(int));
    ans[1] = (int*)malloc((size_t)c2 * sizeof(int));

    for (int i = 0; i < c1; i++) {
        ans[0][i] = tmp1[i];
    }
    for (int i = 0; i < c2; i++) {
        ans[1][i] = tmp2[i];
    }

    free(tmp1);
    free(tmp2);

    *returnSize = 2;
    *returnColumnSizes = (int*)malloc(2 * sizeof(int));
    (*returnColumnSizes)[0] = c1;
    (*returnColumnSizes)[1] = c2;

    return ans;
}
