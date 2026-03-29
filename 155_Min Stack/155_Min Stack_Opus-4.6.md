# 155. Min Stack — Implementation vs. Editorial Comparison

## Implementation Analysis

The repo's `Min Stack.c` uses **two parallel arrays** (`data[]` and `min_stack[]`) where `min_stack[i]` stores the global minimum when the stack height is `i+1`. This is functionally identical to **Editorial Approach 1 (Stack of Value/Minimum Pairs)**, implemented as struct-of-arrays rather than array-of-structs.

## Which Approach Does the Implementation Match?

**Same algorithm as Approach 1.** The editorial uses `stack<pair<int,int>>` (one stack of pairs); our C implementation uses two `int*` arrays indexed by the same `top` pointer. The semantics are identical: every push records both the value and the running minimum at that stack depth.

## Comparison Table

| Dimension | Approach 1 (Pairs / Our Impl.) | Approach 2 (Two Stacks) | Approach 3 (Improved Two Stacks) |
|---|---|---|---|
| Time complexity | O(1) all ops | O(1) all ops | O(1) all ops |
| Space complexity | O(n) always | O(n) worst, less on average | O(n) worst, best on repeated mins |
| C implementation difficulty | Very low | Moderate (conditional push/pop logic) | Higher (pair + count on min stack) |
| Interview suitability | Excellent — simple & correct | Good — shows optimization awareness | Good for follow-up discussion |
| Cache locality | Best — parallel arrays, sequential access | Moderate — two separate stacks | Moderate — pairs on min stack |

## Baseline vs. Recommended

- **Baseline approach:** Approach 1 (Value/Minimum Pairs) — simplest to implement, guaranteed correct, easy to explain.
- **Recommended approach for Google interview:** Start with Approach 1 as the implementation, then verbally discuss Approach 2/3 as optimizations. Approach 1 is the safest to code under time pressure. If the interviewer asks about space optimization, describe Approach 2 (only push to min-tracker when `val <= current_min`) — this shows you understand the invariant without adding implementation risk.
- **Why this recommendation:** In an interview, correctness and clarity win over micro-optimizations. Approach 1 has zero edge cases (no conditional push/pop on the min stack), making it nearly impossible to get wrong. The space "waste" is constant factor (2x), which doesn't change asymptotic complexity.

---

# 155. 最小棧 — 實作 vs. 題解比較

## 實作分析

本 repo 的 `Min Stack.c` 使用**兩個平行陣列**（`data[]` 和 `min_stack[]`），其中 `min_stack[i]` 儲存棧高度為 `i+1` 時的全域最小值。這在功能上等同於 **Editorial 方法一（值/最小值配對棧）**，只是用 struct-of-arrays 取代 array-of-structs。

## 實作對應哪個方法？

**與方法一相同的演算法。** 題解使用 `stack<pair<int,int>>`（一個配對棧）；我們的 C 實作使用兩個 `int*` 陣列，共享同一個 `top` 指標。語意完全相同：每次 push 都記錄值和該深度的滾動最小值。

## 比較表

| 面向 | 方法一（配對 / 我們的實作） | 方法二（兩個棧） | 方法三（改進的兩個棧） |
|---|---|---|---|
| 時間複雜度 | 所有操作 O(1) | 所有操作 O(1) | 所有操作 O(1) |
| 空間複雜度 | 總是 O(n) | 最壞 O(n)，平均較少 | 最壞 O(n)，重複最小值時最好 |
| C 實作難度 | 非常低 | 中等（條件式 push/pop 邏輯） | 較高（追蹤棧上的配對 + 計數） |
| 面試適合度 | 優秀 — 簡單且正確 | 好 — 展示優化意識 | 好 — 適合後續討論 |
| 快取局部性 | 最佳 — 平行陣列，循序存取 | 中等 — 兩個獨立棧 | 中等 — 追蹤棧上的配對 |

## 基準方案 vs. 建議方案

- **基準方案：** 方法一（值/最小值配對）— 最簡單的實作，保證正確，容易解釋。
- **Google 面試建議方案：** 以方法一作為實作，然後口頭討論方法二/三作為優化。方法一在時間壓力下最安全。若面試官問到空間優化，描述方法二（僅在 `val <= current_min` 時才推入追蹤棧）— 這展示你理解不變量，而不增加實作風險。
- **為何如此建議：** 在面試中，正確性和清晰度勝過微優化。方法一沒有邊界情況（追蹤棧上沒有條件式 push/pop），幾乎不可能寫錯。空間「浪費」只是常數因子（2 倍），不改變漸近複雜度。
