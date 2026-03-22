/*
 * LeetCode 122. 買賣股票的最佳時機 II
 * 難度：Medium
 * 演算法：貪心法，累加所有正差價
 * 時間：O(n)，空間：O(1)
 */

int maxProfit(int* prices, int pricesSize) {
    if (pricesSize < 2) return 0;

    int max_profit = 0;

    /* 累加所有的正差價 */
    for (int i = 1; i < pricesSize; i++) {
        if (prices[i] > prices[i - 1]) {
            max_profit += prices[i] - prices[i - 1];
        }
    }

    return max_profit;
}
