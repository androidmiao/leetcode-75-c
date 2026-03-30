# 283. Move Zeroes — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The current `283-move-zeroes.c` implements **Approach #3 (Optimal Swap)** from the LeetCode editorial. This is the same algorithm: a single-pass two-pointer technique where `cur` scans left-to-right and `lastNonZero` tracks the placement position. When a non-zero element is found, it is swapped with the element at `lastNonZero`, maintaining the invariant that all elements before `lastNonZero` are non-zero in original order and all elements between `lastNonZero` and `cur` are zero.

The repo also contains `solution.c`, which uses an XOR-swap variant of the same approach. It finds the first zero index explicitly (`zero_idx = -1` sentinel) before starting swaps. This is functionally equivalent to Approach #3 but with a different initialization strategy and XOR swap instead of temp-variable swap.

## Which Approach Is Better

| Dimension | Approach #2 (Two-pass) | Approach #3 (Swap) | XOR-swap variant (`solution.c`) |
|---|---|---|---|
| Time complexity | O(n) | O(n) | O(n) |
| Space complexity | O(1) | O(1) | O(1) |
| Array writes (best) | n | non-zero count | non-zero count |
| Array writes (worst) | 2n | n | n |
| Interview suitability | Good starting point | Best — clean, optimal | Acceptable but XOR swap is a distraction |
| C implementation difficulty | Very easy | Easy | Slightly trickier (XOR pitfall when `a == b`) |

## Baseline vs. Recommended

- **Baseline**: Approach #2 (two-pass: collect non-zeros, then fill zeros). Simple to explain and implement. Good as a warm-up answer.
- **Recommended**: Approach #3 (single-pass swap). Fewer total writes, cleaner invariant to explain, and directly answers the follow-up question about minimizing operations. This is the approach to present in a Google interview after briefly mentioning the baseline.

Approach #3 is better for interview discussion because it demonstrates understanding of the invariant (non-zeros before slow pointer, zeros between slow and fast) and shows awareness of operation count optimization — a common follow-up probe.

---

# 283. Move Zeroes — 實作 vs. 題解對照（繁體中文）

## 實作與題解對照

目前 `283-move-zeroes.c` 實作的是 LeetCode 題解中的 **方法三（最佳 Swap 版）**。演算法完全一致：單次掃描的雙指標技巧，`cur` 從左到右掃描，`lastNonZero` 追蹤放置位置。遇到非零元素就與 `lastNonZero` 位置交換，維持不變量：`lastNonZero` 前面全部非零且保持原始順序，`lastNonZero` 到 `cur` 之間全部是零。

資料夾中的 `solution.c` 使用 XOR 交換的變體版本。它先明確找到第一個零的索引（`zero_idx = -1` 哨兵值），然後再開始交換。功能上等同方法三，但初始化策略不同，且使用 XOR swap 而非暫存變數。

## 哪個方法較好

| 面向 | 方法二（兩段式） | 方法三（Swap） | XOR-swap 變體 (`solution.c`) |
|---|---|---|---|
| 時間複雜度 | O(n) | O(n) | O(n) |
| 空間複雜度 | O(1) | O(1) | O(1) |
| 陣列寫入（最佳） | n | 非零元素數 | 非零元素數 |
| 陣列寫入（最差） | 2n | n | n |
| 面試適合度 | 好的起點 | 最佳——簡潔、最優 | 可以但 XOR swap 容易分散注意力 |
| C 實作難度 | 非常簡單 | 簡單 | 稍難（`a == b` 時 XOR 會出錯） |

## 基線 vs. 推薦方法

- **基線方法**：方法二（兩段式：先收集非零，再補零）。簡單好講、好實作，適合當暖身答案。
- **推薦方法**：方法三（單次掃描 swap）。總寫入次數更少、不變量解釋更乾淨，且直接回答了進階問題「能否讓總操作次數最少？」。在 Google 面試中，先簡述基線方法，再呈現方法三是最佳策略。

方法三更適合面試討論，因為它展示了對不變量的理解（慢指標前全非零、中間全零），也展現了對操作次數最佳化的意識——這是常見的追問考點。
