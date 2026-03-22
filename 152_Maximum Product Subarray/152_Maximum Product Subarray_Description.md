# 152. Maximum Product Subarray

## Problem
Find the contiguous subarray with the largest product.

## Key Insight
**Track both max and min**: Negative × negative = positive, so minimum might become maximum.

## Examples
- [2,3,-2,4] → 6 (2×3)
- [-2] → -2
