# 217. Contains Duplicate

## Problem Statement

Given an integer array `nums`, return `true` if any value appears at least twice in the array, and return `false` if every element is distinct.

## Examples

### Example 1:
```
Input: nums = [1,2,3,1]
Output: true
```
Explanation: The element 1 occurs at index 0 and 3.

### Example 2:
```
Input: nums = [1,2,3,4]
Output: false
```
Explanation: All elements are distinct.

### Example 3:
```
Input: nums = [1,1,1,3,3,4,3,2,4,2]
Output: true
```

## Constraints

- `1 <= nums.length <= 10^5`
- `-10^9 <= nums[i] <= 10^9`

## Key Insights

1. **Hash Set Approach**: Using a hash set to track seen elements gives O(n) time.

2. **Sorting Approach**: In C, we can use qsort to sort the array and check adjacent elements. Any duplicate will be adjacent after sorting.

3. **Early Termination**: As soon as a duplicate is found, we can return true immediately without checking the rest of the array.

4. **Space Tradeoff**: Hash set uses O(n) space but gives O(n) time. Sorting modifies the array in-place (O(1) extra space) but takes O(n log n) time.

---

# 217. 包含重複元素

## 題目描述

給定一個整數陣列 `nums`，若陣列中任何值出現至少兩次，回傳 `true`；若每個元素皆不相同，回傳 `false`。

## 範例

### 範例 1：
```
輸入：nums = [1,2,3,1]
輸出：true
```
說明：元素 1 出現在索引 0 和 3。

### 範例 2：
```
輸入：nums = [1,2,3,4]
輸出：false
```
說明：所有元素皆不相同。

### 範例 3：
```
輸入：nums = [1,1,1,3,3,4,3,2,4,2]
輸出：true
```

## 限制條件

- `1 <= nums.length <= 10^5`
- `-10^9 <= nums[i] <= 10^9`

## 關鍵觀察

1. **雜湊集合法**：使用雜湊集合追蹤已見過的元素，可達到 O(n) 時間。

2. **排序法**：在 C 語言中，可用 qsort 排序陣列後檢查相鄰元素。排序後重複元素必定相鄰。

3. **提前終止**：一旦找到重複即可立即回傳 true，無需繼續檢查剩餘元素。

4. **空間取捨**：雜湊集合使用 O(n) 空間但時間為 O(n)。排序法原地修改陣列（O(1) 額外空間）但時間為 O(n log n)。
