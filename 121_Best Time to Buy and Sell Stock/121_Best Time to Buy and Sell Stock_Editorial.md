# LeetCode 121: Best Time to Buy and Sell Stock - Editorial

## English Section

### Problem Overview

The core challenge is to identify the maximum profit achievable by selecting two prices from the given array, where the selling price must occur after the buying price in the sequence. Mathematically, we seek to maximize `prices[j] - prices[i]` where `j > i`.

### Approach 1: Brute Force Solution (Time Limit Exceeded)

**Strategy:**
The straightforward approach examines all possible pairs of indices where a purchase and subsequent sale could occur. For each position `i` representing a potential buy point, we iterate through all positions `j` that come after it to find the maximum profit possible from that buy point.

**Implementation Details:**
- Utilize nested loops where the outer loop represents buying positions and the inner loop represents selling positions
- For each combination, calculate the profit and maintain a running maximum
- The inner loop iterates `n(n-1)/2` times in total

**Complexity Analysis:**
- **Time Complexity:** O(n²) - Due to the nested loop structure examining all pairs
- **Space Complexity:** O(1) - Only constant space needed for tracking maximum profit and temporary profit calculations

**Limitations:**
This approach exceeds time limits on large inputs due to its quadratic nature, making it impractical for substantial datasets.

### Approach 2: Single Pass Optimization

**Strategy:**
Rather than examining all pairs, we recognize that maximum profit requires tracking the lowest price encountered so far (representing the best buying opportunity up to each point) and the highest profit achievable at each position. By making a single traversal through the prices, we can efficiently identify these key values.

**Key Insight:**
For any selling price, the maximum profit is determined by subtracting the minimum price seen before it. We don't need to store all previous prices—only the minimum encountered and the best profit found.

**Algorithm Steps:**
1. Initialize `minprice` to a large value and `maxprofit` to 0
2. Traverse the price array from left to right
3. At each price point:
   - If the current price is lower than `minprice`, update `minprice` to this new minimum
   - Otherwise, calculate the profit if we sell at the current price and bought at `minprice`
   - If this profit exceeds `maxprofit`, update the maximum profit
4. Return the accumulated maximum profit

**Complexity Analysis:**
- **Time Complexity:** O(n) - A single pass through the entire array is sufficient
- **Space Complexity:** O(1) - Only two variables are maintained regardless of input size

**Advantages:**
This approach dramatically improves efficiency by eliminating redundant comparisons and is suitable for handling large datasets within practical time constraints.

---

## 繁體中文部分

### 問題概述

核心任務是找出從給定陣列中選擇兩個價格所能獲得的最大利潤，其中賣出價格必須發生在買入價格之後。用數學語言表達，我們要最大化 `prices[j] - prices[i]`，其中 `j > i`。

### 方法一：暴力搜尋法（超時）

**策略：**
直觀的方式是檢查所有可能的買賣點組合。對於每一個作為潛在買入點的位置 `i`，我們遍歷所有在其後的位置 `j` 以找出該買入點能產生的最大利潤。

**實現細節：**
- 採用雙層迴圈，外層迴圈代表買入位置，內層迴圈代表賣出位置
- 對於每個組合計算利潤並維護一個運行的最大值
- 內層迴圈總共執行 `n(n-1)/2` 次

**複雜度分析：**
- **時間複雜度：** O(n²) - 由於雙層迴圈結構需要檢查所有點對
- **空間複雜度：** O(1) - 僅需常數級空間來追蹤最大利潤和暫時計算

**局限性：**
由於二次方的特性，此方法在大規模輸入上會超過時間限制，使其對於較大的資料集不實用。

### 方法二：單次遍歷最佳化

**策略：**
與其檢查所有組合不如我們認識到：最大利潤取決於追蹤迄今為止遇到的最低價格（代表該點之前的最佳購買機會）以及每個位置可達成的最高利潤。透過單次遍歷陣列，我們可以高效地識別這些關鍵數值。

**核心觀察：**
對於任何賣出價格，最大利潤是透過從該價格減去之前遇到的最低價格來決定的。我們無需存儲所有先前的價格，只需維護遇到的最小值和找到的最佳利潤。

**演算法步驟：**
1. 將 `minprice` 初始化為一個很大的數值，`maxprofit` 初始化為 0
2. 從左到右遍歷價格陣列
3. 在每個價格點：
   - 若當前價格低於 `minprice`，將 `minprice` 更新為此新最小值
   - 否則，計算在此價格賣出且在 `minprice` 買入的利潤
   - 若此利潤超過 `maxprofit`，更新最大利潤
4. 返回累積的最大利潤

**複雜度分析：**
- **時間複雜度：** O(n) - 僅需單次遍歷整個陣列
- **空間複雜度：** O(1) - 無論輸入大小如何，只需維護兩個變數

**優勢：**
此方法透過消除冗餘比較大幅提升效率，適合於在實際時間限制內處理大規模資料集。
