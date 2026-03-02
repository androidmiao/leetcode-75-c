Given an integer array nums, move all 0's to the end of it while maintaining the relative order of the non-zero elements.

Note that you must do this in-place without making a copy of the array.

 

Example 1:

Input: nums = [0,1,0,3,12]
Output: [1,3,12,0,0]
Example 2:

Input: nums = [0]
Output: [0]
 

Constraints:

1 <= nums.length <= 10^4
-2^31 <= nums[i] <= 2^31 - 1
 

Follow up: Could you minimize the total number of operations done?


---
以下是中文翻譯：

給定一個整數陣列 `nums`，請將所有 `0` 移到陣列末尾，同時保持所有非零元素的相對順序不變。

請注意，你必須在**原地（in-place）**完成此操作，不能另外複製一份陣列。

---

範例 1：

輸入：`nums = [0,1,0,3,12]`  
輸出：`[1,3,12,0,0]`

範例 2：

輸入：`nums = [0]`  
輸出：`[0]`

---

限制條件：

- `1 <= nums.length <= 10^4`
- `-2^31 <= nums[i] <= 2^31 - 1`

---

進階問題：你能否將總操作次數降到最少？