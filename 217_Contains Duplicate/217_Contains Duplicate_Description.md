# 217. Contains Duplicate

## Problem Statement

Given an integer array `nums`, return `true` if any value appears at least twice in the array, and return `false` if every element is distinct.

## Examples

### Example 1:
```
Input: nums = [1,2,3,1]
Output: true
```

### Example 2:
```
Input: nums = [1,2,3,4]
Output: false
```

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
