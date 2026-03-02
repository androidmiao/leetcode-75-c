一樣，本質是同一個解法。

你目前的 C 程式是 [643-maximum-average-subarray-I.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/643-maximum-average-subarray-I/643-maximum-average-subarray-I.c) 的「滑動視窗（Approach #2）」：

- 先算前 `k` 個的 `windowSum`
- 每次右移一格用 `windowSum += nums[i] - nums[i-k]`
- 持續更新 `maxSum`
- 最後回傳 `maxSum / k`

所以和 [643-maximum-average-subarray-I_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/643-maximum-average-subarray-I/643-maximum-average-subarray-I_Solution.md) 的方法二一致，時間 `O(n)`、空間 `O(1)`。
