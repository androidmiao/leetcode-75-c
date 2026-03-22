# 53. Maximum Subarray - Editorial

## Approach 1: Kadane's Algorithm (Recommended)

Track the maximum sum ending at current position. Update global maximum.

### Complexity: Time O(n), Space O(1)

## Approach 2: Divide and Conquer
Recursively split array, finding max in left, right, and crossing subarrays.

### Complexity: Time O(n log n), Space O(log n)
