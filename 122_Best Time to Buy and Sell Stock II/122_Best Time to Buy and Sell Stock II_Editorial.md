# 122. Best Time to Buy and Sell Stock II - Editorial

## Summary

We need to determine the maximum profit obtainable by making unlimited transactions (buy-sell pairs). The key is finding the sets of buying and selling prices that together maximize total profit.

## Solution Article

### Approach 1: Brute Force

Calculate the profit for every possible set of transactions and find the maximum. This uses a recursive function that tries every possible sell day after each buy day, then recurses on the remaining days.

**Algorithm:**
- For each starting index, try selling at every future index
- Recursively compute the best profit from the remaining days after each sale
- Track the global maximum across all combinations

**Complexity Analysis:**
- Time complexity: O(n^n) — the recursive function is called n^n times in the worst case
- Space complexity: O(n) — depth of recursion is n

---

### Approach 2: Peak Valley Approach

**Algorithm:**

Plot the prices on a graph. The key insight is that the total profit equals the sum of all (peak - valley) differences where each valley is followed by a peak.

Given array `[7, 1, 5, 3, 6, 4]`:
- Valley_i = 1, Peak_i = 5 → profit segment A = 4
- Valley_j = 3, Peak_j = 6 → profit segment B = 3
- Total profit = A + B = 7

The algorithm explicitly finds each consecutive valley-peak pair:
1. Skip descending elements to find the next valley
2. Skip ascending elements to find the next peak
3. Add (peak - valley) to profit
4. Repeat until the end of the array

Important: we cannot skip intermediate peaks even if a later peak is higher, because collecting each individual segment (A + B) always yields at least as much as a single larger segment (C), since C ≤ A + B.

**Complexity Analysis:**
- Time complexity: O(n) — single pass
- Space complexity: O(1) — constant space

---

### Approach 3: Simple One Pass

**Algorithm:**

This is a simplification of Approach 2. Instead of explicitly finding valley-peak pairs, we observe that the sum of consecutive positive differences equals the total peak-valley profit. We simply add `prices[i] - prices[i-1]` whenever it is positive.

The graph shows that the sum of individual upward segments A + B + C equals the difference D between the overall valley and peak. By collecting every positive consecutive difference, we automatically capture all profitable segments.

**Complexity Analysis:**
- Time complexity: O(n) — single pass
- Space complexity: O(1) — constant space

---

# 122. 買賣股票的最佳時機 II — 題解

## 摘要

我們需要在不限交易次數的條件下，找出最大利潤。核心在於找到所有能讓利潤最大化的買賣價格組合。

## 解題方法

### 方法一：暴力法

計算所有可能的交易組合的利潤，取最大值。使用遞迴函式，在每個買入日之後嘗試所有可能的賣出日，然後對剩餘天數遞迴求解。

**演算法：**
- 對每個起始索引，嘗試在每個未來索引賣出
- 遞迴計算每次賣出後剩餘天數的最佳利潤
- 追蹤所有組合中的全域最大值

**複雜度分析：**
- 時間複雜度：O(n^n) — 最壞情況下遞迴函式被呼叫 n^n 次
- 空間複雜度：O(n) — 遞迴深度為 n

---

### 方法二：峰谷法（Peak Valley Approach）

**演算法：**

將價格繪製在圖表上。關鍵洞察是：總利潤等於所有（波峰 - 波谷）差值的總和，其中每個波谷後面跟著一個波峰。

以陣列 `[7, 1, 5, 3, 6, 4]` 為例：
- 波谷_i = 1，波峰_i = 5 → 利潤段 A = 4
- 波谷_j = 3，波峰_j = 6 → 利潤段 B = 3
- 總利潤 = A + B = 7

演算法明確找出每對連續的波谷-波峰：
1. 跳過下降的元素以找到下一個波谷
2. 跳過上升的元素以找到下一個波峰
3. 將（波峰 - 波谷）加入利潤
4. 重複直到陣列結束

重要觀察：不能跳過中間的波峰，即使後面有更高的波峰，因為收集每個個別段（A + B）的利潤永遠大於或等於單一較大段（C）的利潤，因為 C ≤ A + B。

**複雜度分析：**
- 時間複雜度：O(n) — 單次遍歷
- 空間複雜度：O(1) — 常數空間

---

### 方法三：簡單單次遍歷（Simple One Pass）

**演算法：**

這是方法二的簡化版本。與其明確找出波谷-波峰對，我們觀察到：連續正差值的總和等於總波峰-波谷利潤。我們只需在 `prices[i] - prices[i-1]` 為正時將其加入總利潤。

從圖表可以看出，個別上升段的總和 A + B + C 等於整體波谷與波峰之間的差值 D。透過收集每個正的連續差值，我們自動捕捉了所有有利可圖的段落。

**複雜度分析：**
- 時間複雜度：O(n) — 單次遍歷
- 空間複雜度：O(1) — 常數空間
