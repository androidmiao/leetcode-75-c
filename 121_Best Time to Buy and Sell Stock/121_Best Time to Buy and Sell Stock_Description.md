# 121. Best Time to Buy and Sell Stock

## Problem
Given an array `prices` where prices[i] is the price on ith day, find the maximum profit by buying and selling once.

## Key Insights
1. **Track minimum**: Keep track of lowest price seen so far
2. **Calculate profit**: Profit at each step = current price - min price
3. **Time**: O(n), **Space**: O(1)

## Examples
- [7,1,5,3,6,4] → 5 (buy at 1, sell at 6)
- [7,6,4,3,1] → 0 (prices only decrease)
