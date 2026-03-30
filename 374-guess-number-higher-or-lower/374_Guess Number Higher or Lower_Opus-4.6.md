# 374 Guess Number Higher or Lower - Implementation vs. Editorial Comparison

## English

### Implementation vs. Editorial Comparison

The C solution in `374_Guess Number Higher or Lower.c` **matches Approach 2 (Binary Search)** from the editorial exactly. The implementation uses the textbook binary search algorithm with the following key features:

- **Search Space**: Uses `left` and `right` pointers initialized to 1 and n
- **Mid Calculation**: Employs `mid = left + (right - left) / 2` to prevent integer overflow (a common pitfall in C)
- **Guess-Based Narrowing**: Calls `guess(mid)` and interprets the return value:
  - Return 0: Found the answer
  - Return -1: Guessed number is too high, search left half
  - Return 1: Guessed number is too low, search right half
- **Termination**: Continues until `left <= right` becomes false

This is a standard, production-ready implementation of binary search that directly corresponds to the editorial's Approach 2.

### Approach Comparison Table

| Aspect | Brute Force (Approach 1) | Binary Search (Approach 2) | Ternary Search (Approach 3) |
|--------|--------------------------|---------------------------|---------------------------|
| **Time Complexity** | O(n) | O(log₂ n) | O(log₃ n) |
| **Space Complexity** | O(1) | O(1) | O(1) |
| **Comparisons (Worst Case)** | n | 1 + 2log₂ n | 1 + 4log₃ n |
| **Algorithm Efficiency** | Poor (linear scan) | Excellent (eliminates half) | Worse than Binary (eliminates thirds but 4 comparisons per round) |
| **Interview Suitability** | Poor—shows lack of optimization skills | Excellent—demonstrates algorithmic knowledge | Poor—slower than binary despite log base 3 |
| **C Implementation Difficulty** | Trivial (simple loop) | Low (standard pattern) | Medium (two midpoints required) |
| **Practical Usage** | Not recommended | Widely preferred | Rarely used |

**Why Binary Search is Superior:**
- Binary Search: 1 + 2log₂ n ≈ 1 + 2(log₂ 1,000,000) ≈ 41 comparisons for n = 1 million
- Ternary Search: 1 + 4log₃ n ≈ 1 + 4(log₃ 1,000,000) ≈ 77 comparisons for the same input
- The factor `log₂(3²) ≈ 3.17 > 1` means ternary search performs approximately 1.9× more comparisons, making it algorithmically inferior despite the smaller logarithmic base

### Baseline vs. Recommended Approaches

| Category | Approach | Time | Space | Notes |
|----------|----------|------|-------|-------|
| **Baseline (Not Recommended)** | Brute Force | O(n) | O(1) | Simple but inefficient; useful only for small n or teaching loops |
| **Recommended (Industry Standard)** | Binary Search | O(log₂ n) | O(1) | Fast, elegant, and used in real systems; the C solution implements this |

---

## 繁體中文

### 實作與編輯社論的比較

C 檔案 `374_Guess Number Higher or Lower.c` 中的實作 **完全符合編輯社論的方法 2（二分搜尋）**。該實作採用標準的二分搜尋演算法，具備以下關鍵特性：

- **搜尋空間**：使用 `left` 和 `right` 指標，初始值分別為 1 和 n
- **中點計算**：採用 `mid = left + (right - left) / 2` 以避免整數溢位（C 語言的常見陷阱）
- **基於 Guess 的範圍縮小**：呼叫 `guess(mid)` 並解讀回傳值：
  - 回傳 0：找到答案
  - 回傳 -1：猜的數字太大，搜尋左半邊
  - 回傳 1：猜的數字太小，搜尋右半邊
- **終止條件**：持續迴圈直到 `left <= right` 變成假

這是一個標準的、生產級別的二分搜尋實作，直接對應編輯社論中的方法 2。

### 方法比較表

| 面向 | 暴力法（方法 1） | 二分搜尋（方法 2） | 三分搜尋（方法 3） |
|------|-----------------|-------------------|-------------------|
| **時間複雜度** | O(n) | O(log₂ n) | O(log₃ n) |
| **空間複雜度** | O(1) | O(1) | O(1) |
| **比較次數（最壞情況）** | n | 1 + 2log₂ n | 1 + 4log₃ n |
| **演算法效率** | 差（線性掃描） | 優秀（每輪消除一半） | 不如二分搜尋（消除三分之一但每輪需 4 次比較） |
| **面試適配性** | 差—顯示缺乏最佳化觀念 | 優秀—展示演算法知識 | 差—儘管對數底數是 3，但比二分搜尋更慢 |
| **C 語言實作難度** | 微不足道（簡單迴圈） | 低（標準模式） | 中等（需要兩個中點） |
| **實務應用** | 不推薦 | 廣泛使用 | 很少使用 |

**為什麼二分搜尋更優越：**
- 二分搜尋：1 + 2log₂ n ≈ 1 + 2(log₂ 1,000,000) ≈ 41 次比較（n = 100 萬時）
- 三分搜尋：1 + 4log₃ n ≈ 1 + 4(log₃ 1,000,000) ≈ 77 次比較（同一輸入）
- 因為 `log₂(3²) ≈ 3.17 > 1`，所以三分搜尋執行的比較次數大約是二分搜尋的 1.9 倍，使其在演算法上仍然不如二分搜尋，儘管對數的底數更小。

### 基準 vs. 推薦方法

| 分類 | 方法 | 時間 | 空間 | 說明 |
|------|------|------|------|------|
| **基準（不推薦）** | 暴力法 | O(n) | O(1) | 簡單但低效；僅適合小規模 n 或教學迴圈 |
| **推薦（業界標準）** | 二分搜尋 | O(log₂ n) | O(1) | 快速、優雅且用於實際系統；C 的實作採用此方法 |
