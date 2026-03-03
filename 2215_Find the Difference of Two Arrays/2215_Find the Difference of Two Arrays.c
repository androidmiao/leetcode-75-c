//Hash Map/Set: https://leetcode.com/problems/find-the-difference-of-two-arrays/description/?envType=study-plan-v2&envId=leetcode-75
/*
Hint1: For each integer in nums1, check if it exists in nums2.
Hint2: Do the same for each integer in nums2.
*/

/*
Time complexity: `O(n + m + 2001)`  
Space complexity: `O(2001)` (excluding output)
*/

/*
這份程式用「布林陣列當集合」來做差集，核心是把值域 `[-1000,1000]` 映射到索引 `[0,2000]`，避免真的用 hash table。

1. 函式介面（LeetCode C 標準）
- 位置：[2215_Find the Difference of Two Arrays.c:15](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2215_Find the Difference of Two Arrays/2215_Find the Difference of Two Arrays.c:15)
- `findDifference(...)` 要回傳 `int**`（二維結果），並透過：
  - `*returnSize` 告訴外層有幾列（固定是 2）
  - `*returnColumnSizes` 告訴每列長度

2. 用固定值域做 membership set
- 位置：[2215_Find the Difference of Two Arrays.c:16](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2215_Find the Difference of Two Arrays/2215_Find the Difference of Two Arrays.c:16)
- `bool in1[2001], in2[2001]` 分別記錄某數值是否出現在 `nums1/nums2`
- 映射方式：`idx = value + 1000`
  - `-1000 -> 0`
  - `0 -> 1000`
  - `1000 -> 2000`

3. 先標記出現過的值（自動去重）
- 位置：[2215_Find the Difference of Two Arrays.c:21](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2215_Find the Difference of Two Arrays/2215_Find the Difference of Two Arrays.c:21)
- 掃 `nums1`：`in1[nums1[i] + 1000] = true`
- 掃 `nums2`：`in2[nums2[i] + 1000] = true`
- 因為是布林陣列，同一值重複出現只會變成一次 `true`，所以「distinct」條件自然成立

4. 找兩邊差集
- 位置：[2215_Find the Difference of Two Arrays.c:28](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2215_Find the Difference of Two Arrays/2215_Find the Difference of Two Arrays.c:28)
- 先配置暫存陣列 `only1/only2`（最大可能 2001 個值）
- 掃完整值域 `v=-1000..1000`：
  - 若 `in1[idx] && !in2[idx]`，放入 `only1`
  - 若 `in2[idx] && !in1[idx]`，放入 `only2`
- 對應題目：
  - `answer[0]`: 在 `nums1` 不在 `nums2`
  - `answer[1]`: 在 `nums2` 不在 `nums1`

5. 組 LeetCode 要的輸出格式
- 位置：[2215_Find the Difference of Two Arrays.c:38](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2215_Find the Difference of Two Arrays/2215_Find the Difference of Two Arrays.c:38)
- `ans` 配兩列：
  - `ans[0]` 長度 `c1`
  - `ans[1]` 長度 `c2`
- 把 `only1/only2` 複製到 `ans` 後，釋放暫存陣列
- 設定：
  - `*returnSize = 2`
  - `(*returnColumnSizes)[0] = c1`
  - `(*returnColumnSizes)[1] = c2`

6. 複雜度
- 時間：`O(nums1Size + nums2Size + 2001)`，等價於 `O(n+m)`（2001 是常數）
- 空間：`O(2001)` 額外布林表（不含輸出）

7. 一個實務注意點
- 這份碼假設值一定在 `[-1000,1000]`（符合本題 constraints）。若題目值域放大，這種固定陣列法就不適合，會改用真正的 hash set。
*/

#include <stdlib.h>
#include <stdbool.h>

int** findDifference(int* nums1, int nums1Size, int* nums2, int nums2Size, int* returnSize, int** returnColumnSizes) {
    // Constraints: -1000 <= nums[i] <= 1000
    // Shift by +1000 to map value range [-1000, 1000] -> [0, 2000]
    bool in1[2001] = {false};
    bool in2[2001] = {false};

    for (int i = 0; i < nums1Size; i++) {
        in1[nums1[i] + 1000] = true;
    }
    for (int i = 0; i < nums2Size; i++) {
        in2[nums2[i] + 1000] = true;
    }

    int* only1 = (int*)malloc(2001 * sizeof(int));
    int* only2 = (int*)malloc(2001 * sizeof(int));
    int c1 = 0, c2 = 0;

    for (int v = -1000; v <= 1000; v++) {
        int idx = v + 1000;
        if (in1[idx] && !in2[idx]) only1[c1++] = v;
        if (in2[idx] && !in1[idx]) only2[c2++] = v;
    }

    int** ans = (int**)malloc(2 * sizeof(int*));
    ans[0] = (int*)malloc(c1 * sizeof(int));
    ans[1] = (int*)malloc(c2 * sizeof(int));

    for (int i = 0; i < c1; i++) ans[0][i] = only1[i];
    for (int i = 0; i < c2; i++) ans[1][i] = only2[i];

    free(only1);
    free(only2);

    *returnSize = 2;
    *returnColumnSizes = (int*)malloc(2 * sizeof(int));
    (*returnColumnSizes)[0] = c1;
    (*returnColumnSizes)[1] = c2;

    return ans;
}