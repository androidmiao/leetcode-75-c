/*
 * LeetCode Problem #121: Best Time to Buy and Sell Stock
 *
 * Difficulty: Easy
 * Time Complexity: O(n) - single pass through the prices array
 * Space Complexity: O(1) - only using constant extra space (min_price, max_profit)
 *
 * ============================================================================
 * ALGORITHM WALKTHROUGH:
 * ============================================================================
 *
 * Goal: Find the maximum profit from buying and selling a stock once.
 *       We must buy before we sell, and can only hold at most one share.
 *
 * Strategy: Single-pass greedy approach
 * - Track the minimum price seen so far as we iterate left to right
 * - At each price, calculate the profit if we sold at that price
 * - Keep track of the maximum profit found
 * - Update the minimum price only if we find a lower price
 *
 * Why it works:
 * The best sell price for any given minimum buy price is found by scanning
 * all prices to the right of it. By tracking the global minimum price seen
 * so far, we ensure we're always comparing each price against the lowest
 * possible buy price that came before it.
 *
 * ============================================================================
 * EXAMPLE: prices = [7, 1, 5, 3, 6, 4]
 * ============================================================================
 *
 * Price Chart:
 *     7
 *     |
 *   6 |       X (sell here for max profit)
 *   5 |   X   |
 *   4 |   |   |   X
 *   3 |   |   X   |
 *   2 |   |       |
 *   1 |   X       |   (minimum buy price)
 *   0 +---+---+---+---+---+---
 *       0   1   2   3   4   5  (indices)
 *
 * Step-by-step execution:
 *
 *   i=0: prices[0]=7 (initialization)
 *        min_price=7, max_profit=0
 *
 *   i=1: prices[1]=1
 *        profit = 1 - 7 = -6 (loss, not better)
 *        min_price = 1 (found lower price)
 *        max_profit = 0 (unchanged)
 *
 *   i=2: prices[2]=5
 *        profit = 5 - 1 = 4 (new best profit!)
 *        max_profit = 4
 *        min_price = 1 (unchanged)
 *
 *   i=3: prices[3]=3
 *        profit = 3 - 1 = 2 (less than current best)
 *        max_profit = 4 (unchanged)
 *        min_price = 1 (unchanged, 3 is not lower)
 *
 *   i=4: prices[4]=6
 *        profit = 6 - 1 = 5 (new best profit!)
 *        max_profit = 5
 *        min_price = 1 (unchanged)
 *
 *   i=5: prices[5]=4
 *        profit = 4 - 1 = 3 (less than current best)
 *        max_profit = 5 (unchanged)
 *        min_price = 1 (unchanged)
 *
 * Return: 5 (buy at price 1, sell at price 6)
 *
 * State Table:
 * ┌─────────────────────────────────────────────────────┐
 * │ i │ prices[i] │ profit │ min_price │ max_profit │
 * ├─────────────────────────────────────────────────────┤
 * │ - │    7      │   -    │     7     │      0     │
 * │ 1 │    1      │   -6   │     1     │      0     │
 * │ 2 │    5      │    4   │     1     │      4     │
 * │ 3 │    3      │    2   │     1     │      4     │
 * │ 4 │    6      │    5   │     1     │      5     │
 * │ 5 │    4      │    3   │     1     │      5     │
 * └─────────────────────────────────────────────────────┘
 *
 * ============================================================================
 */

#include <limits.h>

int maxProfit(int* prices, int pricesSize) {
    // 邊界情況：如果少於2個價格，無法買賣，返回0
    if (pricesSize < 2) return 0;

    // 初始化最低價格為第一個價格
    int min_price = prices[0];

    // 初始化最大利潤為0（沒有交易發生時）
    int max_profit = 0;

    // 從第二個價格開始迭代
    for (int i = 1; i < pricesSize; i++) {
        // 計算在當前價格賣出的利潤（使用迄今為止最低的買入價格）
        int profit = prices[i] - min_price;

        // 如果當前利潤大於已記錄的最大利潤，則更新最大利潤
        if (profit > max_profit) max_profit = profit;

        // 如果發現更低的價格，更新最低價格
        if (prices[i] < min_price) min_price = prices[i];
    }

    // 返回找到的最大利潤
    return max_profit;
}
