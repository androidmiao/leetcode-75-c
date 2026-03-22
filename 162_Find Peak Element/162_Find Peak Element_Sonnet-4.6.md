# 162. Find Peak Element — Sonnet-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Iterative Binary Search**, which is the optimal approach from the editorial.

The implementation matches the editorial exactly: compare `nums[mid]` with `nums[mid+1]`, shrink the right boundary to `mid` or advance `left` to `mid+1`, until `l == r`.

### Approach Comparison Table

| Criterion | Linear Scan | Recursive Binary Search | Iterative Binary Search |
|-----------|-------------|------------------------|------------------------|
| Time Complexity | O(n) | O(log n) | **O(log n)** |
| Space Complexity | O(1) | O(log n) | **O(1)** |
| Meets O(log n) Constraint | No | Yes | **Yes** |
| Interview Suitability | Poor | Good | **Best** |
| C Implementation Difficulty | Trivial | Easy | **Easy** |

### Baseline vs. Recommended

**Baseline approach:** Linear Scan
- O(n) time — fails the explicit O(log n) requirement
- Only useful as a verbal starting point before pivoting to binary search

**Recommended approach:** Iterative Binary Search
- O(log n) time, O(1) space — satisfies both the time constraint and avoids recursion overhead
- The key invariant to state in an interview: "The search space [l, r] always contains at least one peak."
- Comparing only `nums[mid]` vs `nums[mid+1]` (not `nums[mid-1]`) is the clever observation — we don't need to check the left neighbor because we arrived at mid from the left, meaning nums[mid-1] < nums[mid] is already guaranteed.
- Return `l` when `l == r` — no special-case return needed.

**Why Iterative over Recursive:**
The recursive version (Approach 2) has O(log n) stack depth. The iterative version avoids this and is O(1) space. In C, avoiding deep recursion also prevents stack overflow on very large inputs. The iterative version is preferred for interviews.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法三：迭代二分搜尋**，是最優方案。

### 基準方法 vs. 推薦方法

**基準方法：線性掃描**
- O(n) 時間 — 不滿足題目明確的 O(log n) 要求，面試中只能作為初步口頭描述

**推薦方法：迭代二分搜尋**
- O(log n) 時間、O(1) 空間，同時滿足時間和空間要求
- 面試關鍵洞察：「搜尋空間 [l, r] 恆含至少一個峰值」這個不變量支撐整個算法的正確性
- 只比較 `nums[mid]` 和 `nums[mid+1]`，不需要比較左鄰：因為我們從左邊到達 mid，前一步保證了 `nums[mid-1] < nums[mid]`
- 迭代版本優於遞迴版本：避免 O(log n) 的遞迴堆疊深度，在 C 語言中更能防止棧溢位
