# 994. Rotting Oranges — Opus-4.6 Implementation Notes

## Implementation vs. Editorial Comparison

The current `Rotting Oranges.c` implements **multi-source BFS** — the same core algorithm as **Editorial Approach 1 (BFS)**. However, there is a minor implementation-level difference in how BFS levels are separated:

| Aspect | Editorial Approach 1 | Our `.c` Implementation |
|--------|----------------------|------------------------|
| Level separation | **Sentinel marker** `(-1, -1)` inserted into queue | **Level-size** technique: `size = (rear - front) / 2` |
| Minutes init | `minutesElapsed = -1` (sentinel popped before first real work) | `minutes = 0` (incremented only when fresh remain) |
| Queue contents | Cells + sentinel entries mixed in queue | Pure cell entries, no sentinels |
| Visited tracking | In-place: `grid[nr][nc] = 2` | Same: `grid[nr][nc] = 2` |
| BFS seeding | Enqueue all rotten cells, then one sentinel | Enqueue all rotten cells (no sentinel needed) |
| Queue structure | Java `ArrayDeque<Pair<Integer, Integer>>` | Flat `int*` array, two ints per cell |
| Direction iteration | `int[][] directions` (4×2 array) | `int directions[8]` with stride-2 loop |

**Verdict:** Same algorithm (multi-source BFS), different level-tracking mechanism. Both are O(NM) time and O(NM) space. The level-size technique is slightly simpler in C because it avoids sentinel insertion/detection logic and keeps the queue homogeneous.

## Which Approach Is Better?

| Criterion | Approach 1: BFS (sentinel) | Approach 1: BFS (level-size, our impl) | Approach 2: In-place BFS |
|-----------|----------------------------|---------------------------------------|--------------------------|
| Time complexity | O(N × M) | O(N × M) | O(N²M²) worst case |
| Space complexity | O(N × M) | O(N × M) | O(1) extra |
| Interview suitability | ★★★★★ | ★★★★★ | ★★☆☆☆ Too slow |
| C implementation difficulty | Easy (sentinel adds minor code) | Easy (slightly less code) | Medium (grid scan each round) |
| Off-by-one risk | Medium (minutesElapsed = -1 needs care) | Low (clean `if (fresh > 0) minutes++`) | Medium (timestamp arithmetic) |
| Queue homogeneity | Mixed (cells + sentinels) | Pure (cells only) | N/A (no queue) |

## Baseline vs. Recommended

- **Baseline:** Approach 2 (In-place BFS) — O(1) space is elegant, but O(N²M²) time makes it unsuitable when grid size matters. It is instructive for understanding in-place algorithm design but not practical for interviews.
- **Recommended (Google interview):** Approach 1 (BFS) with either sentinel or level-size technique. Both are equally valid. The level-size variant (our implementation) is slightly more natural in C because:
  1. No need to insert/detect sentinel values
  2. The flat `int*` queue stays homogeneous
  3. The `minutes` increment logic is cleaner (`if (fresh_count > 0) minutes++`)
  4. It directly maps to the standard "BFS = shortest path in unweighted graph" pattern that interviewers expect

The sentinel approach (editorial's version) is equally correct and common in Java/Python where queue types handle mixed entries more naturally.

---

# 994. 腐爛的橘子 — Opus-4.6 實作筆記

## 實作 vs. 題解對照

目前的 `Rotting Oranges.c` 實作的是**多源 BFS**——與**題解方法一（BFS）**的核心演算法相同。但在分隔 BFS 層的實作細節上有差異：

| 面向 | 題解方法一 | 我們的 `.c` 實作 |
|------|-----------|-----------------|
| 層級分隔 | **哨兵標記** `(-1, -1)` 插入佇列 | **層大小**技巧：`size = (rear - front) / 2` |
| 分鐘初始值 | `minutesElapsed = -1`（第一次彈出哨兵前尚無實際處理） | `minutes = 0`（僅在仍有新鮮橘子時遞增） |
| 佇列內容 | 格子 + 哨兵混合 | 純格子項目，無哨兵 |
| visited 追蹤 | 原地：`grid[nr][nc] = 2` | 相同：`grid[nr][nc] = 2` |
| BFS 播種 | 所有腐爛格子入佇列，再放一個哨兵 | 所有腐爛格子入佇列（不需哨兵） |
| 佇列結構 | Java `ArrayDeque<Pair<Integer, Integer>>` | 一維 `int*` 陣列，每格佔兩個 int |
| 方向迭代 | `int[][] directions`（4×2 陣列） | `int directions[8]` 步長 2 迴圈 |

**結論：** 相同演算法（多源 BFS），不同的層級追蹤機制。兩者皆為 O(NM) 時間和 O(NM) 空間。層大小技巧在 C 中稍微簡潔，因為避免了哨兵的插入/偵測邏輯，且佇列內容保持同質性。

## 哪種方法更好？

| 準則 | 方法一 BFS（哨兵） | 方法一 BFS（層大小，我們的實作） | 方法二：原地 BFS |
|------|-------------------|-------------------------------|-----------------|
| 時間複雜度 | O(N × M) | O(N × M) | O(N²M²) 最壞 |
| 空間複雜度 | O(N × M) | O(N × M) | O(1) 額外 |
| 面試適合度 | ★★★★★ | ★★★★★ | ★★☆☆☆ 太慢 |
| C 實作難度 | 簡單（哨兵增加少量程式碼） | 簡單（稍少程式碼） | 中等（每輪掃描網格） |
| Off-by-one 風險 | 中等（minutesElapsed = -1 需留意） | 低（乾淨的 `if (fresh > 0) minutes++`） | 中等（時間戳算術） |

## 基線 vs. 推薦

- **基線方法：** 方法二（原地 BFS）— O(1) 空間很優雅，但 O(N²M²) 時間在面試中不實用。適合理解原地演算法設計，但不適合實際面試。
- **推薦方法（Google 面試）：** 方法一（BFS），使用哨兵或層大小技巧皆可。兩者同樣有效。層大小變體（我們的實作）在 C 中更自然，因為：
  1. 不需要插入/偵測哨兵值
  2. 一維 `int*` 佇列保持同質性
  3. `minutes` 遞增邏輯更清晰（`if (fresh_count > 0) minutes++`）
  4. 直接對應面試官期望的標準「BFS = 無權圖最短路徑」模式

哨兵方法（題解版本）同樣正確，在 Java/Python 中更常見，因為這些語言的佇列類型更自然地處理混合項目。
