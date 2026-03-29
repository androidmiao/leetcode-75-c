# 122. Best Time to Buy and Sell Stock II — Implementation vs Editorial

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Simple One Pass** from the editorial. It is identical in algorithm: iterate through the array once, accumulating every positive consecutive difference `prices[i] - prices[i-1]`.

This is mathematically equivalent to **Approach 2: Peak Valley**, but avoids the explicit valley/peak finding logic with two nested while-loops, making it simpler to code and reason about.

## Which Approach is Better

| Criterion | Approach 1: Brute Force | Approach 2: Peak Valley | Approach 3: Simple One Pass (our `.c`) |
|---|---|---|---|
| Time Complexity | O(n^n) | O(n) | O(n) |
| Space Complexity | O(n) | O(1) | O(1) |
| Interview Suitability | Poor — too slow | Good — shows insight | Best — concise & elegant |
| C Implementation Difficulty | High (recursion) | Medium (nested while) | Low (single for-loop) |

## Baseline vs. Recommended

- **Baseline approach:** Approach 1 (Brute Force) — enumerate all possible transaction sets recursively. Useful only as a correctness reference; impractical due to exponential time.
- **Recommended approach:** Approach 3 (Simple One Pass / Greedy) — the approach used in our `.c` solution. It is the best for interview discussion because:
  1. It is trivially simple to implement (5 lines of core logic)
  2. It demonstrates greedy thinking: every positive day-over-day gain is captured
  3. It naturally leads to a follow-up discussion about why this greedy choice is globally optimal (each positive difference is independent; skipping any positive difference can only reduce total profit)
  4. Approach 2 (Peak Valley) is worth mentioning as the intuitive justification for why Approach 3 works, but implementing Approach 2 in an interview adds unnecessary complexity with the nested while-loops

---

# 122. 買賣股票的最佳時機 II — 實作與題解對照

## 實作與題解對照

`.c` 解法實作的是題解中的**方法三：簡單單次遍歷（Simple One Pass）**。演算法完全相同：遍歷陣列一次，累加所有正的連續差價 `prices[i] - prices[i-1]`。

這在數學上等價於**方法二：峰谷法（Peak Valley）**，但省去了用兩個巢狀 while 迴圈明確尋找波谷/波峰的邏輯，使得程式碼更簡潔、更容易理解。

## 哪種方法更好

| 比較項目 | 方法一：暴力法 | 方法二：峰谷法 | 方法三：簡單單次遍歷（我們的 `.c`） |
|---|---|---|---|
| 時間複雜度 | O(n^n) | O(n) | O(n) |
| 空間複雜度 | O(n) | O(1) | O(1) |
| 面試適用性 | 差 — 太慢 | 好 — 展示洞察力 | 最佳 — 簡潔且優雅 |
| C 實作難度 | 高（遞迴） | 中等（巢狀 while） | 低（單一 for 迴圈） |

## 基線方法 vs. 推薦方法

- **基線方法：** 方法一（暴力法）— 遞迴列舉所有可能的交易組合。僅作為正確性參考有用；因指數級時間複雜度而不實際。
- **推薦方法：** 方法三（簡單單次遍歷 / 貪心法）— 我們 `.c` 解法所使用的方法。它最適合面試討論，原因如下：
  1. 實作極其簡單（核心邏輯僅 5 行）
  2. 展示貪心思維：捕捉每一個正的日間漲幅
  3. 自然引導出後續討論：為什麼這個貪心選擇是全域最優的（每個正差值是獨立的；跳過任何正差值只會減少總利潤）
  4. 方法二（峰谷法）值得作為直覺上的正當性說明來提及，但在面試中實作方法二會因巢狀 while 迴圈而增加不必要的複雜度
