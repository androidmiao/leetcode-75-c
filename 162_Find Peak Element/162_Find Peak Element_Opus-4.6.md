# 162. Find Peak Element — Opus-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Iterative Binary Search**, which is the optimal approach from the editorial. The implementation matches the editorial pseudocode exactly:

- Compare `nums[mid]` with `nums[mid+1]`
- If `nums[mid] > nums[mid+1]`: set `r = mid` (peak in left half, including mid)
- Else: set `l = mid + 1` (peak in right half, excluding mid)
- Return `l` when `l == r`

There is no deviation from the editorial algorithm. The `.c` file is a direct, clean translation of the editorial's iterative binary search into C.

### Approach Comparison Table

| Criterion | Linear Scan (Approach 1) | Recursive Binary Search (Approach 2) | Iterative Binary Search (Approach 3) |
|-----------|--------------------------|--------------------------------------|--------------------------------------|
| Time Complexity | O(n) | O(log n) | **O(log n)** |
| Space Complexity | O(1) | O(log n) | **O(1)** |
| Meets O(log n) Constraint | No | Yes | **Yes** |
| Interview Suitability | Poor — fails requirement | Good | **Best** |
| C Implementation Difficulty | Trivial | Easy | **Easy** |
| Stack Safety in C | N/A | Risk on large n | **Safe** |

### Baseline vs. Recommended

**Baseline approach: Linear Scan**
- O(n) time — explicitly violates the problem's O(log n) requirement
- Useful only as a verbal starting point to demonstrate you understand the problem before optimizing

**Recommended approach: Iterative Binary Search**
- O(log n) time, O(1) space — satisfies both the time constraint and avoids recursion overhead
- The key invariant to articulate in an interview: "The search space [l, r] always contains at least one peak, because both boundaries are effectively -∞ and the array is finite with all adjacent elements distinct."
- The clever insight: we only compare `nums[mid]` vs. `nums[mid+1]`, never `nums[mid-1]`. This works because the direction we came from already guarantees the relationship with the left neighbor.
- Termination is clean: when `l == r`, return `l` — no special-case logic needed.
- In C, iterative avoids stack overflow concerns that recursive binary search introduces on large inputs.

**Why Iterative over Recursive for interviews:**
Both achieve O(log n) time. The iterative version uses O(1) space vs. O(log n) for recursion stack. In a Google-style interview, demonstrating awareness of stack depth and choosing the iterative form shows engineering maturity. The code is equally concise in C.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法三：迭代二分搜尋**，是最優方案。實作與 Editorial 的虛擬碼完全一致：比較 `nums[mid]` 和 `nums[mid+1]`，根據比較結果縮小搜尋空間，最後當 `l == r` 時回傳 `l`。

### 方法比較

| 標準 | 線性掃描 | 遞迴二分搜尋 | 迭代二分搜尋 |
|------|----------|-------------|-------------|
| 時間複雜度 | O(n) | O(log n) | **O(log n)** |
| 空間複雜度 | O(1) | O(log n) | **O(1)** |
| 符合 O(log n) 要求 | 否 | 是 | **是** |
| 面試適用性 | 差 | 好 | **最佳** |

### 基準方法 vs. 推薦方法

**基準方法：線性掃描**
- O(n) 時間 — 明確違反題目的 O(log n) 要求，面試中只能作為口頭破題的起點

**推薦方法：迭代二分搜尋**
- O(log n) 時間、O(1) 空間，同時滿足時間和空間要求
- 面試關鍵不變量：「搜尋空間 [l, r] 恆含至少一個峰值」——因為邊界視為 -∞，陣列有限且相鄰元素不等，必有局部最大值
- 巧妙觀察：只比較 `nums[mid]` 和 `nums[mid+1]`，無需比較 `nums[mid-1]`。因為移動方向已保證左鄰的關係
- 終止條件乾淨：`l == r` 時直接回傳，無需特殊處理
- 在 C 語言中，迭代版避免了遞迴堆疊深度的風險，展現了工程成熟度
