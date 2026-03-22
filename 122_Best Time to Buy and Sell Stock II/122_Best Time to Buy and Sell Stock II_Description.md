# 122. Best Time to Buy and Sell Stock II

## Problem
Find the maximum profit with unlimited transactions. You must sell before buying again.

## Key Insight
**Greedy**: Add every positive price difference (consecutive days). Each upward spike = one profit opportunity.

## Examples
- [7,1,5,3,6,4] → 7 (1→5: +4, 3→6: +3)
- [1,2,3,4,5] → 4 (cumulative increases)
