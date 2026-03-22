# 219. Contains Duplicate II

## Problem Statement

Given an integer array `nums` and an integer `k`, return `true` if there are two distinct indices `i` and `j` in the array such that:
- `nums[i] == nums[j]` and
- `abs(i - j) <= k`

## Examples

### Example 1:
```
Input: nums = [99,99], k = 2
Output: true
```

### Example 2:
```
Input: nums = [99,99], k = 0
Output: false
```

### Example 3:
```
Input: nums = [1,2,3,1,2,3], k = 2
Output: false
```

## Constraints

- `1 <= nums.length <= 10^5`
- `-10^9 <= nums[i] <= 10^9`
- `0 <= k <= 2 × 10^4`

## Key Insights

1. **Sliding Window**: Maintain a window of at most k elements at any time. If a duplicate is found within the window, the distance condition is automatically satisfied.

2. **Hash Map/Set**: Store elements in the current window in a hash map (or set), mapping each value to its most recent index.

3. **Two Pointers**: Use left and right pointers to maintain the window size at most k.

4. **Distance Check**: When a duplicate is found, check if the distance between indices is <= k.

5. **Window Management**: When the window exceeds size k, remove the oldest element (the element at the left boundary).
