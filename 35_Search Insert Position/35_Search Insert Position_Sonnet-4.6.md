# 35. Search Insert Position — Sonnet-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 1: Binary Search**, which is the only approach in the editorial and the required approach (the problem mandates O(log n) runtime).

The implementation is identical to the editorial's algorithm. Both use the standard closed-interval binary search with `left = 0`, `right = n-1`, and return `left` as the insert position when the target is not found.

### Approach Comparison Table

| Criterion | Linear Scan (baseline) | Binary Search (editorial) |
|-----------|------------------------|--------------------------|
| Time Complexity | O(n) | **O(log n)** |
| Space Complexity | O(1) | **O(1)** |
| Interview Suitability | Poor (does not meet constraint) | **Required** |
| C Implementation Difficulty | Trivial | Easy |

### Baseline vs. Recommended

**Baseline approach:** Linear scan
- Traverse the array until finding `target` or an element greater than `target`.
- O(n) time — **does not meet the O(log n) constraint**.
- Not acceptable as a final answer.

**Recommended approach:** Binary Search
- O(log n) time, O(1) space — satisfies the explicit constraint.
- Key insight: when target is not found, the `left` pointer is exactly at the insert position because the loop invariant guarantees `nums[right] < target < nums[left]` at termination.
- The integer overflow trick (`left + (right - left) / 2`) is worth mentioning in an interview to show C/C++ awareness.

**Why Binary Search is the only acceptable answer for an interview:**
1. The problem statement explicitly requires O(log n) time.
2. Linear scan is O(n) and would be immediately rejected.
3. The implementation is short and clean — no excuse not to use binary search.
4. Mentioning the integer overflow consideration (`left + (right - left) / 2`) demonstrates C/C++ depth.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**唯一方法：二分搜尋**，且實作與 Editorial 完全一致。

### 基準方法 vs. 推薦方法

**基準方法：線性掃描**
- O(n) 時間 — 不滿足題目要求，面試中不可接受

**推薦方法：二分搜尋**
- O(log n) 時間、O(1) 空間 — 滿足題目明確的時間複雜度要求
- 核心洞察：找不到 target 時，`left` 指標恰好停在插入位置，因為迴圈不變量保證 `nums[right] < target < nums[left]`
- 在 C 語言面試中值得特別說明整數溢位的防範：用 `left + (right - left) / 2` 而非 `(left + right) / 2`

**面試關鍵：**
- 題目已明確要求 O(log n)，不使用二分搜尋直接扣分
- 程式碼短且清晰，面試中應流暢寫出
- 說明 `return left` 的原因能展示對二分搜尋邊界條件的深入理解
