/*
 * LeetCode 121. 買賣股票的最佳時機
 * 難度：Easy
 * 演算法：單次遍歷，追踪最小價格，計算最大利潤
 * 時間：O(n)，空間：O(1)
 */

#include <limits.h>

int maxProfit(int* prices, int pricesSize) {
    if (pricesSize < 2) return 0;

    int min_price = prices[0];
    int max_profit = 0;

    for (int i = 1; i < pricesSize; i++) {
        /* 計算在當前價格賣出的利潤 */
        int profit = prices[i] - min_price;

        /* 更新最大利潤 */
        if (profit > max_profit) {
            max_profit = profit;
        }

        /* 更新最小價格 */
        if (prices[i] < min_price) {
            min_price = prices[i];
        }
    }

    return max_profit;
}
