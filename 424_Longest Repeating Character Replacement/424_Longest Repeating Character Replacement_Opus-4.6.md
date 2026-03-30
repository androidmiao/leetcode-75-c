# 424. Longest Repeating Character Replacement — Implementation vs. Editorial

## Implementation Overview

The repo's C solution uses the **Expanding Sliding Window** approach (Editorial Approach 3), which is the optimal single-pass O(n) algorithm. The window never shrinks — it either expands or slides rightward, maintaining the largest valid window size seen so far.

## Which Editorial Approach Does the Implementation Match?

The implementation matches **Approach 3: Expanding Sliding Window** exactly. The key characteristics are:

- Single `for` loop with `right` iterating through the string
- `left` pointer advances at most once per iteration (window slides, never truly shrinks)
- `max_freq` is tracked globally and only increases — it is never recalculated when characters leave the window
- Validity check: `window_len - max_freq > k` triggers a single left-shift

## Comparison Table

| Aspect | Approach 1: Binary Search + SW | Approach 2: Per-Character SW | Approach 3: Expanding SW (Our Impl.) |
|--------|-------------------------------|------------------------------|--------------------------------------|
| Time Complexity | O(n log n) | O(26n) = O(n) | O(n) |
| Space Complexity | O(1) | O(1) | O(1) |
| Number of Passes | O(log n) passes of O(n) each | 26 passes of O(n) each | Single pass |
| Code Complexity | High (binary search + helper) | Medium (nested loops) | Low (single loop) |
| Interview Suitability | Poor — over-engineered | Fair — simple but slow constant | Excellent — clean and optimal |
| C Implementation Difficulty | Medium | Easy | Easy |
| Key Insight Required | Monotonicity of valid length | Per-character decomposition | Window-never-shrinks invariant |

## Baseline vs. Recommended

### Baseline Approach: Sliding Window + Binary Search (Approach 1)

Binary search over the answer length, checking each candidate with a fixed-size sliding window. While correct and demonstrating a useful technique (binary search on answer), it is O(n log n) and unnecessarily complex for this problem.

### Recommended Approach: Expanding Sliding Window (Approach 3 — our implementation)

Single-pass expanding window with the `max_freq` monotonicity trick. This is the recommended Google interview approach because:

1. **Optimal complexity**: O(n) time, O(1) space — cannot do better
2. **Elegant invariant**: the window size equals the best answer seen so far, which is easy to explain
3. **Minimal code**: ~20 lines of clean C, no helper functions needed
4. **Natural follow-up discussion**: the `max_freq` never-decreasing property is a non-obvious insight that demonstrates deep understanding
5. **Generalizable pattern**: the expanding window technique applies to many substring optimization problems

### Why Not Approach 2?

Per-character sliding window (Approach 2) is conceptually simple but runs 26 separate passes. In practice the constant factor makes it 5-10x slower, and it does not demonstrate the key insight of frequency-based window management. It is better as a teaching step toward Approach 3 than as a final answer.

## Edge Cases Handled

| Case | Input | Output | Why |
|------|-------|--------|-----|
| Single character | `"A"`, k=0 | 1 | Window of size 1 is always valid |
| All same | `"AAAA"`, k=0 | 4 | max_freq = window_len, need_change = 0 |
| Full replacement | `"ABAB"`, k=2 | 4 | Can change all non-majority chars |
| No replacement | `"ABCD"`, k=0 | 1 | Each window of size >1 needs changes |
| k >= n | `"ABC"`, k=5 | 3 | Entire string is always valid |

## Key Implementation Detail: Why max_freq Never Decreases

When the window slides (left moves right), we decrement `freq[s[left] - 'A']`, but we do **not** update `max_freq`. This seems incorrect at first glance — the actual maximum frequency in the current window might be lower than `max_freq`. However, this is intentional:

- We only care about finding windows **at least as large** as the current best
- A smaller `max_freq` would only allow the same or smaller window size
- Therefore, stale `max_freq` values can only cause the window to slide (not shrink below the best), which is correct behavior
- `max_freq` will be updated to a true value when we encounter a character that beats the historical maximum

This subtle invariant is the crux of the O(n) solution and a strong interview talking point.

---

# 424. 最長重複字元替換 — 實作與社論比較

## 實作概述

本倉庫的 C 解法使用**擴展滑動視窗**方法（社論方法 3），是最優的單次遍歷 O(n) 演算法。視窗永遠不會縮小——它只會擴展或向右滑動，始終維持迄今為止看到的最大有效視窗大小。

## 實作對應哪個社論方法？

實作完全對應**方法 3：擴展滑動視窗**。主要特徵：

- 單一 `for` 迴圈，`right` 遍歷整個字串
- `left` 指標每次迭代最多前進一步（視窗滑動，從不真正收縮）
- `max_freq` 全域追蹤且只會增加——當字元離開視窗時不會重新計算
- 有效性檢查：`window_len - max_freq > k` 時觸發一次左移

## 比較表

| 面向 | 方法 1：二分搜尋 + 滑窗 | 方法 2：逐字元滑窗 | 方法 3：擴展滑窗（我們的實作） |
|------|------------------------|-------------------|-------------------------------|
| 時間複雜度 | O(n log n) | O(26n) = O(n) | O(n) |
| 空間複雜度 | O(1) | O(1) | O(1) |
| 遍歷次數 | O(log n) 次，每次 O(n) | 26 次，每次 O(n) | 單次遍歷 |
| 程式碼複雜度 | 高（二分搜尋 + 輔助函式） | 中等（巢狀迴圈） | 低（單一迴圈） |
| 面試適用性 | 差——過度設計 | 尚可——簡單但常數較大 | 優秀——簡潔且最優 |
| C 實作難度 | 中等 | 簡單 | 簡單 |
| 所需關鍵洞察 | 有效長度的單調性 | 逐字元分解 | 視窗永不收縮不變式 |

## 基線方法 vs. 推薦方法

### 基線方法：滑動視窗 + 二分搜尋（方法 1）

對答案長度進行二分搜尋，以固定大小滑動視窗檢查每個候選長度。雖然正確且展示了有用的技巧（對答案二分搜尋），但 O(n log n) 的複雜度對此問題而言過於複雜。

### 推薦方法：擴展滑動視窗（方法 3——我們的實作）

單次遍歷的擴展視窗配合 `max_freq` 單調性技巧。這是推薦的 Google 面試方法，原因：

1. **最優複雜度**：O(n) 時間、O(1) 空間——無法更優
2. **優雅的不變式**：視窗大小等於迄今為止的最佳答案，容易解釋
3. **極少程式碼**：約 20 行乾淨的 C，不需要輔助函式
4. **自然的後續討論**：`max_freq` 永不遞減的性質是非顯而易見的洞察，展示深層理解
5. **可泛化的模式**：擴展視窗技巧適用於許多子字串最佳化問題

### 為何不選方法 2？

逐字元滑動視窗（方法 2）概念簡單，但要執行 26 次獨立遍歷。實際上常數因子使其慢 5-10 倍，且未能展示頻率基礎視窗管理的關鍵洞察。作為通往方法 3 的教學步驟比作為最終答案更適合。

## 關鍵實作細節：為何 max_freq 永不遞減

當視窗滑動（left 右移）時，我們遞減 `freq[s[left] - 'A']`，但**不更新** `max_freq`。乍看之下似乎不正確——當前視窗中的實際最大頻率可能低於 `max_freq`。然而這是刻意為之：

- 我們只關心找到**至少與當前最佳一樣大**的視窗
- 較小的 `max_freq` 只會允許相同或更小的視窗大小
- 因此，過時的 `max_freq` 值只會導致視窗滑動（不會縮小到低於最佳值），這是正確的行為
- 當遇到超過歷史最大值的字元時，`max_freq` 會更新為真實值

這個微妙的不變式是 O(n) 解法的核心，也是面試中的有力談話要點。
