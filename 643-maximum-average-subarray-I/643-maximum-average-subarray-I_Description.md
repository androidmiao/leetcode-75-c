You are given an integer array nums consisting of n elements, and an integer k.

Find a contiguous subarray whose length is equal to k that has the maximum average value and return this value. Any answer with a calculation error less than 10-5 will be accepted.

 

Example 1:

Input: nums = [1,12,-5,-6,50,3], k = 4
Output: 12.75000
Explanation: Maximum average is (12 - 5 - 6 + 50) / 4 = 51 / 4 = 12.75
Example 2:

Input: nums = [5], k = 1
Output: 5.00000
 

Constraints:

n == nums.length
1 <= k <= n <= 105
-104 <= nums[i] <= 104


---
給定一個由 n 個元素組成的整數陣列 `nums`，以及一個整數 `k`。

請找出長度恰好為 `k` 的連續子陣列，使其平均值最大，並回傳這個最大平均值。只要答案的計算誤差小於 `10^-5`，都會被接受。

範例 1：
輸入：`nums = [1,12,-5,-6,50,3]`, `k = 4`
輸出：`12.75000`
說明：最大平均值為 `(12 - 5 - 6 + 50) / 4 = 51 / 4 = 12.75`
/*
長度要固定是 4，所以所有可能的連續子陣列有：
[1,12,-5,-6]，總和 2，平均 0.5
[12,-5,-6,50]，總和 51，平均 12.75
[-5,-6,50,3]，總和 42，平均 10.5
最大平均值是 12.75，所以輸出 12.75000。
*/


範例 2：
輸入：`nums = [5]`, `k = 1`
輸出：`5.00000`
/*
陣列只有一個元素，且子陣列長度也要是 1，所以唯一的子陣列就是 [5]，平均值是 5，輸出 5.00000。
*/

限制條件：
`n == nums.length`
`1 <= k <= n <= 10^5`
`-10^4 <= nums[i] <= 10^4`

/*
意思是題目保證輸入一定符合這些範圍：

1. `n == nums.length`  
`n` 就是陣列 `nums` 的長度（元素個數）。

2. `1 <= k <= n <= 10^5`  
- `k` 至少是 1。  
- `k` 不會大於 `n`（子陣列長度不可能超過原陣列）。  
- `n` 最大是 `100000`。

3. `-10^4 <= nums[i] <= 10^4`  
每個元素值都在 `-10000` 到 `10000` 之間。  
`nums[i]` 代表第 `i` 個元素。
*/