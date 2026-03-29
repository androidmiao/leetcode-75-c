# 122. Best Time to Buy and Sell Stock II

**Difficulty:** Medium

## Problem Statement

You are given an integer array `prices` where `prices[i]` is the price of a given stock on the i-th day.

On each day, you may decide to buy and/or sell the stock. You can only hold **at most one** share of the stock at any time. However, you can sell and buy the stock multiple times on the **same day**, ensuring you never hold more than one share of the stock.

Find and return the **maximum** profit you can achieve.

## Examples

### Example 1

```
Input: prices = [7,1,5,3,6,4]
Output: 7
Explanation: Buy on day 2 (price = 1) and sell on day 3 (price = 5), profit = 5-1 = 4.
Then buy on day 4 (price = 3) and sell on day 5 (price = 6), profit = 6-3 = 3.
Total profit is 4 + 3 = 7.
```

### Example 2

```
Input: prices = [1,2,3,4,5]
Output: 4
Explanation: Buy on day 1 (price = 1) and sell on day 5 (price = 5), profit = 5-1 = 4.
Total profit is 4.
```

### Example 3

```
Input: prices = [7,6,4,3,1]
Output: 0
Explanation: There is no way to make a positive profit, so we never buy the stock
to achieve the maximum profit of 0.
```

## Constraints

- `1 <= prices.length <= 3 * 10^4`
- `0 <= prices[i] <= 10^4`

---

# 122. 買賣股票的最佳時機 II

**難度：** 中等

## 題目描述

給定一個整數陣列 `prices`，其中 `prices[i]` 是某支股票在第 i 天的價格。

每天你都可以決定買入和／或賣出股票。任何時候你**最多**只能持有一股股票。不過，你可以在**同一天**買賣股票多次，確保你永遠不會持有超過一股股票。

找出並回傳你能獲得的**最大**利潤。

## 範例

### 範例 1

```
輸入：prices = [7,1,5,3,6,4]
輸出：7
說明：第 2 天買入（價格 = 1），第 3 天賣出（價格 = 5），利潤 = 5-1 = 4。
然後第 4 天買入（價格 = 3），第 5 天賣出（價格 = 6），利潤 = 6-3 = 3。
總利潤為 4 + 3 = 7。
```

### 範例 2

```
輸入：prices = [1,2,3,4,5]
輸出：4
說明：第 1 天買入（價格 = 1），第 5 天賣出（價格 = 5），利潤 = 5-1 = 4。
總利潤為 4。
```

### 範例 3

```
輸入：prices = [7,6,4,3,1]
輸出：0
說明：無法獲得正利潤，所以我們不買入股票，最大利潤為 0。
```

## 限制條件

- `1 <= prices.length <= 3 * 10^4`
- `0 <= prices[i] <= 10^4`
