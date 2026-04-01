# 1732. Find the Highest Altitude — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The repository's `solution.c` uses the exact same algorithm as the editorial's single approach: **Prefix Sum**.

Both implementations:

1. Initialize `currentAltitude` (or `current`) and `highestPoint` (or `highest`) to 0
2. Iterate through `gain[]`, accumulating the running sum
3. Update the maximum after each addition
4. Return the maximum

The only difference is variable naming (`current`/`highest` vs. `currentAltitude`/`highestPoint`) and a minor style choice: the repo uses an `if` statement instead of `max()`.

## Which Approach Is Better

Since there is only one editorial approach and the repo implements the same algorithm, there is no "better" alternative to compare.

| Dimension | Repo (`solution.c`) | Editorial |
|---|---|---|
| Time complexity | O(n) | O(n) |
| Space complexity | O(1) | O(1) |
| Interview suitability | Excellent — simple, clear, optimal | Identical |
| C implementation difficulty | Trivial | Trivial |

## Baseline vs. Recommended

- **Baseline approach**: Prefix Sum with running max — this is both the baseline and the optimal solution.
- **Recommended approach**: Same. There is no meaningful alternative for this problem. A brute-force approach that first builds the entire altitude array would use O(n) extra space for no benefit.
- **Why**: The problem is fundamentally about computing a max prefix sum. Any correct O(n) solution will look essentially the same. In an interview, the key insight to articulate is that each altitude is a prefix sum, and we only need the maximum — so we can compute it in a single pass without storing all prefix sums.

---

# 1732. Find the Highest Altitude — 實作與 Editorial 比較

## 實作與 Editorial 對照

本 repo 的 `solution.c` 使用的演算法與 Editorial 唯一的做法完全相同：**前綴和（Prefix Sum）**。

兩者都是：

1. 將 `currentAltitude`（或 `current`）與 `highestPoint`（或 `highest`）初始化為 0
2. 走訪 `gain[]`，持續累加計算前綴和
3. 每次相加後更新最大值
4. 回傳最大值

差別僅在變數命名（`current`/`highest` vs. `currentAltitude`/`highestPoint`）與風格：repo 用 `if` 判斷取代 `max()`。

## 哪種做法比較好

由於 Editorial 只有一種做法，而 repo 實作的就是同一個演算法，無從比較「哪個更好」。

| 面向 | Repo (`solution.c`) | Editorial |
|---|---|---|
| 時間複雜度 | O(n) | O(n) |
| 空間複雜度 | O(1) | O(1) |
| 面試適合度 | 極佳 — 簡單、清晰、最佳 | 完全相同 |
| C 實作難度 | 極低 | 極低 |

## 基線做法 vs. 建議做法

- **基線做法**：前綴和 + 動態最大值 — 這同時是基線也是最佳解。
- **建議做法**：相同。此題沒有更好的替代方案。暴力做法先建完整海拔陣列會多用 O(n) 空間卻無任何好處。
- **原因**：這題本質上就是求「最大前綴和」。任何正確的 O(n) 解法看起來都幾乎一樣。面試時要表達的關鍵洞見是：每個海拔就是一個前綴和，而我們只需要最大值 — 所以一次掃描、不存陣列就夠了。
