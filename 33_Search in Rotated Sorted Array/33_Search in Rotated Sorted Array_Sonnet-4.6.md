# 33. Search in Rotated Sorted Array — Sonnet-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 2: One-Pass Binary Search**, which is the more elegant and interview-preferred approach from the editorial.

The implementation directly determines which half is sorted and whether the target falls in that half, all in a single binary search pass.

### Approach Comparison Table

| Criterion | Find Pivot + Binary Search | One-Pass Binary Search |
|-----------|---------------------------|------------------------|
| Time Complexity | O(log n) | **O(log n)** |
| Space Complexity | O(1) | **O(1)** |
| Number of passes | 2 | **1** |
| Interview Suitability | Good | **Better** |
| C Implementation Difficulty | Medium | Medium |
| Code clarity | Moderate | **Higher** |

### Baseline vs. Recommended

**Baseline approach:** Find Pivot + Binary Search (Approach 1)
- Two separate binary searches: first to find pivot, then to search target.
- O(log n) time, O(1) space — correct but uses two passes.
- Good starting point to describe the structure, then optimize to one pass.

**Recommended approach:** One-Pass Binary Search (Approach 2)
- Single binary search leveraging the key invariant: one half is always sorted.
- Decision tree at each step:
  - `nums[l] <= nums[mid]` → left half sorted → check if target ∈ `[nums[l], nums[mid])`
  - Otherwise → right half sorted → check if target ∈ `(nums[mid], nums[r]]`
- The comparison `nums[l] <= nums[mid]` (note `<=` not `<`) handles the case where `l == mid` (single element).
- Return `-1` if `l > r` — no special cases needed.

**Key interview discussion points:**
1. Why one half is always sorted in a rotated array — this is the core insight.
2. Why we use `<=` in `nums[l] <= nums[mid]` — handles the `l == mid` edge case.
3. The strict inequalities when checking target range ensure we don't include `mid` itself in both branches.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法二：一次二分搜尋**，更優雅、更適合面試。

### 基準方法 vs. 推薦方法

**基準方法：找 Pivot + 二分搜尋（方法一）**
- 兩次二分搜尋：先找旋轉點，再搜尋目標
- 正確但需要兩次掃描，可作為描述問題結構的起點

**推薦方法：一次二分搜尋（方法二）**
- 核心不變量：任意 mid 點，必有一半是連續有序的
- 判斷順序：先確定哪半有序，再判斷 target 是否在有序的那一半
- 比較條件 `nums[l] <= nums[mid]`（注意 `<=` 而非 `<`）：處理 `l == mid` 時的邊界情況

**面試關鍵說明點：**
1. 為何旋轉後必有一半有序 — 這是算法的核心洞察
2. `<=` vs `<` 的差異 — 邊界條件的細節
3. 檢查 target 範圍時的嚴格不等式設計 — 確保不重疊搜尋
