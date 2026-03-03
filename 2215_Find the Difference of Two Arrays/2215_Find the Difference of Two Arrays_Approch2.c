/*
重點變更：
移除固定值域 [-1000,1000] 的布林陣列解法。
改為自建 IntHashSet（open addressing + linear probing）。
先建 set1/set2 做 membership 查詢，再用 collect_diff_unique 產出兩邊的 distinct 差集。
保留 LeetCode C 介面（returnSize、returnColumnSizes）不變。

編譯驗證：
cc -std=c11 -Wall -Wextra -Werror -c ... 成功。
*/

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* keys;
    bool* used;
    int capacity;
} IntHashSet;

static unsigned int hash_int(int x) {
    unsigned int h = (unsigned int)x;
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return h;
}

static int next_pow2(int n) {
    int cap = 16;
    while (cap < n) {
        cap <<= 1;
    }
    return cap;
}

static void set_init(IntHashSet* set, int expected_size) {
    int cap = next_pow2(expected_size * 2 + 1);
    set->capacity = cap;
    set->keys = (int*)malloc((size_t)cap * sizeof(int));
    set->used = (bool*)calloc((size_t)cap, sizeof(bool));
}

static void set_free(IntHashSet* set) {
    free(set->keys);
    free(set->used);
}

static bool set_contains(const IntHashSet* set, int key) {
    unsigned int mask = (unsigned int)(set->capacity - 1);
    unsigned int idx = hash_int(key) & mask;

    while (set->used[idx]) {
        if (set->keys[idx] == key) {
            return true;
        }
        idx = (idx + 1) & mask;
    }
    return false;
}

static bool set_insert(IntHashSet* set, int key) {
    unsigned int mask = (unsigned int)(set->capacity - 1);
    unsigned int idx = hash_int(key) & mask;

    while (set->used[idx]) {
        if (set->keys[idx] == key) {
            return false;
        }
        idx = (idx + 1) & mask;
    }
    set->used[idx] = true;
    set->keys[idx] = key;
    return true;
}

static int collect_diff_unique(
    int* src,
    int src_size,
    const IntHashSet* other_set,
    int* out
) {
    IntHashSet seen;
    int count = 0;
    set_init(&seen, src_size);

    for (int i = 0; i < src_size; i++) {
        int v = src[i];
        if (set_insert(&seen, v) && !set_contains(other_set, v)) {
            out[count++] = v;
        }
    }

    set_free(&seen);
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
    IntHashSet set1, set2;
    set_init(&set1, nums1Size);
    set_init(&set2, nums2Size);

    for (int i = 0; i < nums1Size; i++) {
        set_insert(&set1, nums1[i]);
    }
    for (int i = 0; i < nums2Size; i++) {
        set_insert(&set2, nums2[i]);
    }

    int* tmp1 = (int*)malloc((size_t)nums1Size * sizeof(int));
    int* tmp2 = (int*)malloc((size_t)nums2Size * sizeof(int));
    int c1 = collect_diff_unique(nums1, nums1Size, &set2, tmp1);
    int c2 = collect_diff_unique(nums2, nums2Size, &set1, tmp2);

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
    set_free(&set1);
    set_free(&set2);

    *returnSize = 2;
    *returnColumnSizes = (int*)malloc(2 * sizeof(int));
    (*returnColumnSizes)[0] = c1;
    (*returnColumnSizes)[1] = c2;

    return ans;
}
