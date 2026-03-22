# 53. Maximum Subarray

## Problem
Given an integer array `nums`, find the contiguous subarray (containing at least one number) which has the largest sum and return its sum.

## Key Insights
1. **Kadane's Algorithm**: Track current sum, if negative reset to 0
2. **Time**: O(n), **Space**: O(1)

## Examples
- Input: [-2,1,-3,4,-1,2,1,-5,4] → Output: 6 ([4,-1,2,1])
- Input: [5,4,-1,7,8] → Output: 23
