# 121. Best Time to Buy and Sell Stock

## English Version

### Problem Description

You are given an array `prices` where `prices[i]` is the price of a given stock on the i-th day.

You want to maximize your profit by choosing a single day to buy one stock and choosing a different day in the future to sell that stock.

Return the maximum profit you can achieve from this transaction. If you cannot achieve any profit, return 0.

### Examples

#### Example 1
**Input:** prices = [7,1,5,3,6,4]
**Output:** 5
**Explanation:** Buy on day 2 (price = 1) and sell on day 5 (price = 6), profit = 6-1 = 5.
Note that buying on day 2 and selling on day 1 is not allowed because you must buy before you sell.

#### Example 2
**Input:** prices = [7,6,4,3,1]
**Output:** 0
**Explanation:** In this case, no transactions are done and the max profit = 0.

### Constraints

- 1 <= prices.length <= 10^5
- 0 <= prices[i] <= 10^4

---

## 繁體中文版本

### 問題描述

給定一個陣列 `prices`，其中 `prices[i]` 表示股票在第 i 天的價格。

你想通過選擇某一天買入一隻股票，並在未來的某一天賣出來最大化利潤。

返回你能從這筆交易中獲得的最大利潤。如果無法獲得任何利潤，則返回 0。

### 示例

#### 示例 1
**輸入:** prices = [7,1,5,3,6,4]
**輸出:** 5
**解釋:** 在第 2 天買入（價格 = 1）並在第 5 天賣出（價格 = 6），利潤 = 6-1 = 5。
注意：不允許在第 2 天買入又在第 1 天賣出，因為你必須先買後賣。

#### 示例 2
**輸入:** prices = [7,6,4,3,1]
**輸出:** 0
**解釋:** 在這種情況下，沒有進行交易，最大利潤 = 0。

### 限制條件

- 1 <= prices.length <= 10^5
- 0 <= prices[i] <= 10^4
