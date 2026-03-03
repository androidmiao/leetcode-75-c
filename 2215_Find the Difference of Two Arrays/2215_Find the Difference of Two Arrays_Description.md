Given two 0-indexed integer arrays nums1 and nums2, return a list answer of size 2 where:

answer[0] is a list of all distinct integers in nums1 which are not present in nums2.
answer[1] is a list of all distinct integers in nums2 which are not present in nums1.
Note that the integers in the lists may be returned in any order.

 

Example 1:

Input: nums1 = [1,2,3], nums2 = [2,4,6]
Output: [[1,3],[4,6]]
Explanation:
For nums1, nums1[1] = 2 is present at index 0 of nums2, whereas nums1[0] = 1 and nums1[2] = 3 are not present in nums2. Therefore, answer[0] = [1,3].
For nums2, nums2[0] = 2 is present at index 1 of nums1, whereas nums2[1] = 4 and nums2[2] = 6 are not present in nums1. Therefore, answer[1] = [4,6].
Example 2:

Input: nums1 = [1,2,3,3], nums2 = [1,1,2,2]
Output: [[3],[]]
Explanation:
For nums1, nums1[2] and nums1[3] are not present in nums2. Since nums1[2] == nums1[3], their value is only included once and answer[0] = [3].
Every integer in nums2 is present in nums1. Therefore, answer[1] = [].
 

Constraints:

1 <= nums1.length, nums2.length <= 1000
-1000 <= nums1[i], nums2[i] <= 1000


---

中文翻譯：

給定兩個從 0 開始編號（0-indexed）的整數陣列 `nums1` 和 `nums2`，請回傳一個大小為 2 的串列 `answer`，其中：

`answer[0]` 是 `nums1` 中所有不在 `nums2` 出現的「相異整數」所組成的串列。  
`answer[1]` 是 `nums2` 中所有不在 `nums1` 出現的「相異整數」所組成的串列。  
注意，這些串列中的整數可以以任意順序回傳。


範例 1：

輸入：`nums1 = [1,2,3], nums2 = [2,4,6]`  
輸出：`[[1,3],[4,6]]`  
解釋：  
對於 `nums1`，`nums1[1] = 2` 出現在 `nums2` 的索引 0；而 `nums1[0] = 1` 和 `nums1[2] = 3` 沒有出現在 `nums2`。因此，`answer[0] = [1,3]`。  
對於 `nums2`，`nums2[0] = 2` 出現在 `nums1` 的索引 1；而 `nums2[1] = 4` 和 `nums2[2] = 6` 沒有出現在 `nums1`。因此，`answer[1] = [4,6]`。

範例 2：

輸入：`nums1 = [1,2,3,3], nums2 = [1,1,2,2]`  
輸出：`[[3],[]]`  
解釋：  
對於 `nums1`，`nums1[2]` 和 `nums1[3]` 沒有出現在 `nums2`。由於 `nums1[2] == nums1[3]`，其值只會被計入一次，所以 `answer[0] = [3]`。  
`nums2` 中的每個整數都出現在 `nums1`。因此，`answer[1] = []`。


限制條件：

`1 <= nums1.length, nums2.length <= 1000`  
`-1000 <= nums1[i], nums2[i] <= 1000`
