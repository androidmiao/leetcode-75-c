# 84. Largest Rectangle in Histogram

## Problem
Given array of heights, find the largest rectangle area in histogram.

## Key Insight
**Monotonic Stack**: Add sentinel bars of height 0. Pop when shorter bar found.

## Examples
- [2,1,5,6,2,3] → 10 (height 5, width 2)
