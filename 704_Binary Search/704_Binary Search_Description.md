Given an array of integers nums which is sorted in ascending order, and an integer target, write a function to search target in nums. If target exists, then return its index. Otherwise, return -1.

You must write an algorithm with O(log n) runtime complexity.

 

Example 1:

Input: nums = [-1,0,3,5,9,12], target = 9
Output: 4
Explanation: 9 exists in nums and its index is 4
Example 2:

Input: nums = [-1,0,3,5,9,12], target = 2
Output: -1
Explanation: 2 does not exist in nums so return -1
 

Constraints:

1 <= nums.length <= 104
-104 < nums[i], target < 104
All the integers in nums are unique.
nums is sorted in ascending order.

---

給定一個依照遞增順序排序的整數陣列 `nums`，以及一個整數 `target`，請撰寫一個函式在 `nums` 中搜尋 `target`。如果 `target` 存在，則回傳它的索引；否則回傳 `-1`。

你必須撰寫一個時間複雜度為 `O(log n)` 的演算法。

範例 1：

輸入：`nums = [-1,0,3,5,9,12]`, `target = 9`  
輸出：`4`  
說明：`9` 存在於 `nums` 中，而且它的索引是 `4`

範例 2：

輸入：`nums = [-1,0,3,5,9,12]`, `target = 2`  
輸出：`-1`  
說明：`2` 不存在於 `nums` 中，因此回傳 `-1`

限制條件：

`1 <= nums.length <= 10^4`  
`-10^4 < nums[i], target < 10^4`  
`nums` 中所有整數皆不重複。  
`nums` 依照遞增順序排序。
