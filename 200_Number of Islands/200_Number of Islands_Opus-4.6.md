# 200. Number of Islands — Implementation vs. Editorial Comparison

## Implementation vs. Editorial

The repository's `Number of Islands.c` uses **Approach #1: DFS** from the editorial. The algorithm is identical: linear scan the grid, and when a `'1'` is found, increment the counter and trigger a recursive DFS that marks the entire island as `'0'` (visited). The code structure matches the editorial's C++ DFS approach translated into plain C with explicit row/col parameter passing.

## Approach Comparison

| Dimension | Approach #1: DFS | Approach #2: BFS | Approach #3: Union-Find |
|-----------|-----------------|-----------------|------------------------|
| Time Complexity | O(M × N) | O(M × N) | O(M × N) amortised |
| Space Complexity | O(M × N) worst case (recursion stack) | O(min(M, N)) (queue) | O(M × N) (parent + rank arrays) |
| Interview Suitability | ★★★★★ Excellent — simplest to code and explain | ★★★★ Good — iterative, no stack overflow risk | ★★★ Moderate — impressive but more code to write |
| C Implementation Difficulty | Very Easy — ~15 lines, no extra data structures | Easy — needs a queue (manual in C) | Medium — needs Union-Find struct with path compression + union by rank |
| Stack Overflow Risk | Yes, on 300×300 all-land grid | No | No |

## Baseline vs. Recommended

- **Baseline approach**: DFS (Approach #1) — the simplest correct solution. Directly scan + recursive flood-fill. Extremely natural to explain in an interview.
- **Recommended approach**: DFS (Approach #1) — for this problem, the baseline is already the best interview choice. It has the shortest code, clearest logic, and O(M×N) time. BFS is a strong alternative if the interviewer raises stack overflow concerns (the 300×300 constraint makes DFS safe in practice). Union-Find is worth mentioning as a follow-up to demonstrate breadth of knowledge, but it adds unnecessary complexity for this problem.

---

# 200. 島嶼數量 — 實作 vs. 官方題解比較

## 實作與題解對照

本倉庫的 `Number of Islands.c` 採用的是官方題解的**方法一：DFS（深度優先搜尋）**。演算法完全相同：線性掃描網格，遇到 `'1'` 時將計數器加一，並觸發遞迴 DFS 將整座島嶼標記為 `'0'`（已訪問）。程式碼結構與題解的 C++ DFS 版本一致，只是翻譯成純 C 並以明確的 row/col 參數傳遞。

## 方法比較

| 比較維度 | 方法一：DFS | 方法二：BFS | 方法三：Union-Find（並查集） |
|---------|-----------|-----------|--------------------------|
| 時間複雜度 | O(M × N) | O(M × N) | O(M × N) 攤銷 |
| 空間複雜度 | 最壞 O(M × N)（遞迴棧） | O(min(M, N))（佇列） | O(M × N)（parent + rank 陣列） |
| 面試適合度 | ★★★★★ 極佳 — 最簡潔、最易解釋 | ★★★★ 良好 — 迭代式，無棧溢出風險 | ★★★ 中等 — 展現功力但程式碼較多 |
| C 語言實作難度 | 非常簡單 — 約 15 行，無需額外資料結構 | 簡單 — 需手動實作佇列 | 中等 — 需實作含路徑壓縮與按秩合併的並查集 |
| 棧溢出風險 | 有，在 300×300 全陸地時 | 無 | 無 |

## 基線方法 vs. 推薦方法

- **基線方法**：DFS（方法一）— 最簡單的正確解法。直接掃描加遞迴洪水填充，在面試中解釋起來最自然。
- **推薦方法**：DFS（方法一）— 對於本題，基線本身已是最佳面試選擇。它的程式碼最短、邏輯最清晰、時間複雜度 O(M×N)。若面試官提出棧溢出疑慮，BFS 是很好的替代方案（300×300 的限制下 DFS 實際上是安全的）。Union-Find 值得作為延伸話題提及以展示知識廣度，但對本題而言增加了不必要的複雜度。
