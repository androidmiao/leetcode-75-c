You are given an array of integers nums, there is a sliding window of size k which is moving from the very left of the array to the very right. You can only see the k numbers in the window. Each time the sliding window moves right by one position.

Return the max sliding window.

Example 1:

Input: nums = [1,3,-1,-3,5,3,6,7], k = 3
Output: [3,3,5,5,6,7]
Explanation:
Window position                Max
---------------               -----
[1  3  -1] -3  5  3  6  7       3
 1 [3  -1  -3] 5  3  6  7       3
 1  3 [-1  -3  5] 3  6  7       5
 1  3  -1 [-3  5  3] 6  7       5
 1  3  -1  -3 [5  3  6] 7       6
 1  3  -1  -3  5 [3  6  7]      7

Example 2:

Input: nums = [1], k = 1
Output: [1]

Constraints:

1 <= nums.length <= 10^5
-10^4 <= nums[i] <= 10^4
1 <= k <= nums.length

Hint:
1. How about using a data structure such as deque (double-ended queue)?
2. The queue size need not be the same as the window's size.
3. Remove redundant elements and the queue should store only elements that need to be considered.


---
給定一個整數陣列 `nums`，以及一個大小為 `k` 的滑動視窗，這個視窗會從陣列最左邊一路移動到最右邊。你每次只能看到視窗中的 `k` 個數字。每當滑動視窗向右移動一個位置時，都需要重新考慮目前視窗中的最大值。

請回傳每一次滑動視窗對應的最大值所組成的陣列。

範例 1：

輸入：`nums = [1,3,-1,-3,5,3,6,7]`，`k = 3`
輸出：`[3,3,5,5,6,7]`
說明：
視窗位置                     最大值
---------------             -----
`[1  3  -1] -3  5  3  6  7`    `3`
` 1 [3  -1  -3] 5  3  6  7`    `3`
` 1  3 [-1  -3  5] 3  6  7`    `5`
` 1  3  -1 [-3  5  3] 6  7`    `5`
` 1  3  -1  -3 [5  3  6] 7`    `6`
` 1  3  -1  -3  5 [3  6  7]`   `7`

範例 2：

輸入：`nums = [1]`，`k = 1`
輸出：`[1]`

限制條件：

`1 <= nums.length <= 10^5`
`-10^4 <= nums[i] <= 10^4`
`1 <= k <= nums.length`

提示：
1. 可以考慮使用像是 deque（雙端佇列）這樣的資料結構。
2. 佇列的大小不一定要和視窗大小相同。
3. 把多餘的元素移除後，佇列中只需要保留那些仍然有必要被考慮的元素。
