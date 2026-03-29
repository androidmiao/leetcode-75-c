# LeetCode 121: Best Time to Buy and Sell Stock
## Implementation vs Editorial Comparison

---

## English Section

### Implementation vs. Editorial Comparison

The `.c` solution implements the **One Pass algorithm**, which is identical to the editorial's **Approach 2** in algorithmic strategy. Both approaches track two variables across a single pass through the prices array:

- **min_price (or minprice)**: The lowest price encountered so far
- **max_profit (or maxprofit)**: The maximum profit achievable up to the current position

**Key Differences:**

| Aspect | .c Implementation | Editorial Approach 2 |
|--------|-------------------|----------------------|
| min_price initialization | `prices[0]` (first price) | `INT_MAX` (theoretical maximum) |
| Loop start | `i = 1` | `i = 0` |
| Loop condition | Iterate from index 1 to n-1 | Iterate from index 0 to n-1 |
| Equivalence | Functionally identical | Functionally identical |
| Edge case handling | Implicitly handles by starting at prices[0] | Explicitly handles via INT_MAX comparison |

Both implementations are mathematically equivalent. The `.c` file avoids an unnecessary comparison on the first element, while the editorial's approach is more general and defensive.

### Approach Comparison: One Pass vs. Brute Force

| Criterion | Approach 1: Brute Force | Approach 2: One Pass |
|-----------|-------------------------|----------------------|
| **Time Complexity** | O(n²) | O(n) |
| **Space Complexity** | O(1) | O(1) |
| **Interview Suitability** | Poor—demonstrates incomplete understanding of optimization | Excellent—shows algorithmic thinking and optimization awareness |
| **C Implementation Difficulty** | Very Easy—nested loops, minimal logic | Easy—single loop with two variables |
| **Scalability** | Fails at large n (n > 10,000) | Performs well at any n |
| **Code Clarity** | Clear intent but inefficient | Clear and performant |

**Why One Pass is Better:**

The Brute Force approach examines every pair of (buy_day, sell_day) where buy_day < sell_day, leading to O(n²) complexity. For n = 100,000 (common in LeetCode), this results in 10 billion comparisons—unacceptable in interviews.

The One Pass approach recognizes a key insight: **at each price, the maximum profit is determined by the lowest price seen before it**. By maintaining the running minimum price and updating the maximum profit incrementally, we reduce time complexity to O(n) with a single pass. This is optimal (we must examine all prices at least once).

### Baseline vs. Recommended Approach

**Baseline:** Brute Force (Approach 1)
- Acceptable as a starting point to show understanding of the problem
- Demonstrates the ability to think about all possible transactions
- However, indicates lack of optimization awareness in interviews

**Recommended:** One Pass (Approach 2)
- Optimal time complexity: O(n)
- Optimal space complexity: O(1)
- Shows deeper algorithmic insight: recognizing the "minimum so far" pattern
- Demonstrates strong problem-solving intuition
- Easy to explain and code in C (or any language)

In interviews, start with Brute Force to clarify the problem, then immediately pivot to One Pass to show optimization skills. The interviewer will be impressed by the clarity of explanation: *"At each price, I track the minimum price before it, and calculate what profit I'd make if I sold at this price. I keep the maximum such profit."*

---

## 繁體中文版本

### 實現與官方解答的比較

`.c` 解決方案實現了**一次遍歷演算法**，與官方解答的**方法2**的演算法策略完全相同。兩種方法都通過單次掃描價格陣列來跟蹤兩個變數：

- **min_price（或minprice）**：迄今為止遇到的最低價格
- **max_profit（或maxprofit）**：當前位置可達成的最大利潤

**主要差異：**

| 方面 | .c 實現 | 官方方法2 |
|------|---------|---------|
| min_price 初始化 | `prices[0]`（第一個價格） | `INT_MAX`（理論最大值） |
| 迴圈開始 | `i = 1` | `i = 0` |
| 迴圈條件 | 從索引1迭代到n-1 | 從索引0迭代到n-1 |
| 等價性 | 功能上相同 | 功能上相同 |
| 邊界情況處理 | 通過從prices[0]開始隱式處理 | 通過INT_MAX比較顯式處理 |

兩種實現在數學上是等價的。`.c` 文件避免了對第一個元素的不必要的比較，而官方方法更加通用和防禦性。

### 方法比較：一次遍歷 vs. 暴力法

| 評估項目 | 方法1：暴力法 | 方法2：一次遍歷 |
|---------|-------------|-------------|
| **時間複雜度** | O(n²) | O(n) |
| **空間複雜度** | O(1) | O(1) |
| **面試適用性** | 差—表明對最佳化理解不完整 | 優秀—展現演算法思維和最佳化意識 |
| **C語言實現難度** | 非常簡單—嵌套迴圈，最少邏輯 | 簡單—單個迴圈加兩個變數 |
| **可伸縮性** | 在大n值下失敗（n > 10,000） | 在任何n值下性能良好 |
| **代碼清晰度** | 意圖清晰但低效 | 清晰且高效 |

**為什麼一次遍歷更優：**

暴力法檢查所有的(買入日期，賣出日期)對，其中買入日期 < 賣出日期，導致O(n²)的複雜度。對於n = 100,000（LeetCode常見規模），這會導致100億次比較—在面試中不可接受。

一次遍歷方法認識到一個關鍵洞察：**在每個價格點，最大利潤由其之前見到的最低價格決定**。通過維持運行中的最低價格並遞增地更新最大利潤，我們將時間複雜度降低到O(n)並只需單次遍歷。這是最優的（我們必須至少檢查每個價格一次）。

### 基礎方案 vs. 推薦方案

**基礎方案：** 暴力法（方法1）
- 作為展現對問題理解的起點是可以接受的
- 展現了對所有可能交易的思考能力
- 然而，表明在面試中缺乏最佳化意識

**推薦方案：** 一次遍歷（方法2）
- 最優時間複雜度：O(n)
- 最優空間複雜度：O(1)
- 展現更深層的演算法洞察：識別「迄今最小值」的模式
- 展現強大的問題解決直覺
- 易於在C語言（或任何語言）中解釋和編碼

在面試中，先從暴力法開始以澄清問題，然後立即轉向一次遍歷以展現最佳化能力。面試官會對解釋的清晰度印象深刻：*「在每個價格點，我跟蹤其之前的最低價格，並計算如果在該價格賣出我能獲得的利潤。我保留最大的此類利潤。」*

