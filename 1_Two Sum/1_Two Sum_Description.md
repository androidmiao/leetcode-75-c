Given an array of integers nums and an integer target, return indices of the two numbers such that they add up to target.

You may assume that each input would have exactly one solution, and you may not use the same element twice.

You can return the answer in any order.

Example 1:

Input: nums = [2,7,11,15], target = 9
Output: [0,1]
Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].

Example 2:

Input: nums = [3,2,4], target = 6
Output: [1,2]

Example 3:

Input: nums = [3,3], target = 6
Output: [0,1]

Constraints:

2 <= nums.length <= 10^4
-10^9 <= nums[i] <= 10^9
-10^9 <= target <= 10^9
Only one valid answer exists.

Follow-up: Can you come up with an algorithm that is less than O(n^2) time complexity?

Hint:

1. A really brute force way would be to search for all possible pairs of numbers but that would be too slow. Again, it's best to try out brute force solutions just for completeness. It is from these brute force solutions that you can come up with optimizations.
2. So, if we fix one of the numbers, say x, we have to scan the entire array to find the next number y which is value - x where value is the input parameter. Can we change our array somehow so that this search becomes faster?
3. The second train of thought is, without changing the array, can we use additional space somehow? Like maybe a hash map to speed up the search?


---
給定一個整數陣列 `nums` 與一個整數 `target`，請回傳兩個數字的索引，且這兩個數字相加後的結果必須等於 `target`。

你可以假設每一組輸入都恰好只會有一個解，而且不能重複使用同一個元素兩次。

你可以用任意順序回傳答案。

範例 1：

輸入：`nums = [2,7,11,15]`，`target = 9`
輸出：`[0,1]`
說明：因為 `nums[0] + nums[1] == 9`，所以回傳 `[0, 1]`。

範例 2：

輸入：`nums = [3,2,4]`，`target = 6`
輸出：`[1,2]`

範例 3：

輸入：`nums = [3,3]`，`target = 6`
輸出：`[0,1]`

限制條件：

`2 <= nums.length <= 10^4`
`-10^9 <= nums[i] <= 10^9`
`-10^9 <= target <= 10^9`
只會存在一個有效答案。

進階問題：你能否想出一個時間複雜度小於 `O(n^2)` 的演算法？

提示：

1. 最直覺的暴力法，就是把所有可能的數字配對都找出來，但這樣會太慢。不過一開始還是很適合先從暴力法思考，因為通常就是從暴力法出發，才比較容易進一步想出如何優化。
2. 如果我們先固定其中一個數字，例如 `x`，那就需要掃描整個陣列，去找另一個數字 `y`，其中 `y = target - x`。那麼，我們能不能先對陣列做某種改變，讓這個搜尋變得更快？
3. 另一個思考方向是，在不改變原本陣列的前提下，能不能使用額外空間來加速搜尋？例如，是否可以使用雜湊表（hash map）來提高查找效率？
