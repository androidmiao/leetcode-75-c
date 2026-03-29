# 994. Rotting Oranges — Editorial

> Source: Live authenticated LeetCode editorial page (2026-03-29)

## Overview

This is a 2D traversal problem. The common algorithmic strategies are **Breadth-First Search** (BFS) and **Depth-First Search** (DFS). BFS is the better fit here because the rotting process propagates layer by layer — rotten oranges contaminate their neighbours first, before reaching oranges farther away. The number of elapsed minutes equals the number of BFS levels traversed.

The problem can be visualized as a *graph* where each cell is a node and edges connect 4-directionally adjacent cells. Starting from the rotten orange(s), contamination propagates **layer by layer** (level by level) until it reaches the farthest fresh oranges.

---

## Approach 1: Breadth-First Search (BFS)

### Intuition

BFS uses a **queue** to track candidates to visit. At each iteration we pop an element, process it, and append its fresh neighbours to keep the BFS running.

The editorial's reference implementation applies several optimizations:

- **No `visited` table** — instead of a separate visited structure, we *reuse* the input grid. Marking a fresh orange as rotten (`grid[r][c] = 2`) serves the same purpose as adding it to a visited set, with O(1) constant-time lookup via array access.
- **Delimiter / sentinel** — a special marker `(row=-1, col=-1)` is inserted into the queue after the initial batch of rotten oranges. When this sentinel is dequeued, one round (one minute) has elapsed. A new sentinel is then appended to mark the end of the next round. `minutesElapsed` starts at `-1` to account for the first sentinel pop before any real processing.

### Algorithm

1. **Build initial set**: scan the grid, enqueue all rotten cells, count `freshOranges`. Insert sentinel `(-1, -1)` after all initial rotten cells.
2. **BFS loop**: while queue is non-empty:
   - Pop `(row, col)`.
   - If it is the sentinel `(-1, -1)`: increment `minutesElapsed`. If the queue is still non-empty, insert another sentinel to mark the next round's end.
   - Otherwise: for each of the 4 neighbours, if the neighbour is a fresh orange, mark it rotten, decrement `freshOranges`, and enqueue it.
3. **Result**: if `freshOranges == 0`, return `minutesElapsed`; otherwise return `-1`.

### Complexity

| Metric | Value |
|--------|-------|
| Time   | O(N × M) — scan for initial values O(NM), then BFS visits each cell at most once O(NM). Overall O(NM). |
| Space  | O(N × M) — in the worst case the queue holds all cells in the grid. |

### Our implementation note

The repo's `Rotting Oranges.c` uses a **level-size** technique instead of a sentinel marker: it computes `size = (rear - front) / 2` at the start of each round and processes exactly that many cells. This is an equivalent way to separate BFS levels without inserting a dummy element. Both approaches achieve the same O(NM) time and space.

---

## Approach 2: In-place BFS

### Intuition

Some Discussion-forum users (e.g. @manky, @votrubac) proposed BFS implementations with **constant O(1) extra space** by eliminating the queue entirely.

The secret is an *in-place algorithm* that transforms the input to solve the problem without auxiliary data structures. We already used the grid in-place for visited tracking in Approach 1; here we go further and also replace the **queue** with in-place timestamp marking.

The idea: at each *round* of BFS, we mark the cells to be visited in the input grid with a specific **timestamp**. Starting from `timestamp = 2` (the initial rotten oranges), at each round we:

1. Scan the entire grid for cells with the current timestamp value.
2. For each such cell, mark its fresh neighbours with `timestamp + 1`.
3. Increment timestamp and repeat until no new cells are marked.

This in-place modification replaces the queue — the grid itself records which cells to process next.

### Algorithm

1. Starting with `timestamp = 2`, cells with value `2` are the initial rotten oranges.
2. For each cell with the current timestamp, mark fresh neighbours with `timestamp + 1`.
3. After marking, we have `timestamp + 1` and the next round's candidates are already marked.
4. Repeat step (2) until no new candidates are generated.
5. If fresh oranges remain, return `-1`. Otherwise return `timestamp - 2` (number of rounds elapsed).

### Complexity

| Metric | Value |
|--------|-------|
| Time   | O(N²M²) — for each round of BFS we iterate through the entire grid. In the worst case (zigzag arrangement), the longest propagation chain can be NM/2 rounds. So total: O(NM) per scan × O(NM) rounds = O(N²M²). |
| Space  | O(1) — constant extra memory. The grid is modified in-place. This trades time complexity for space savings, a common scenario in in-place algorithms. |

---

# 994. 腐爛的橘子 — 題解

> 來源：即時已驗證的 LeetCode 題解頁面（2026-03-29）

## 概述

這是一道 2D 遍歷問題。常見的演算法策略有**廣度優先搜尋**（BFS）和**深度優先搜尋**（DFS）。BFS 更適合此題，因為腐爛過程是逐層傳播的——腐爛橘子先感染鄰居，再傳到更遠的橘子。經過的分鐘數等於 BFS 遍歷的層數。

可以將問題視為一個*圖*，每個格子是一個節點，邊連接四方向相鄰的格子。從腐爛橘子出發，感染**逐層**（level by level）傳播，直到到達最遠的新鮮橘子。

---

## 方法一：廣度優先搜尋（BFS）

### 直覺

BFS 使用**佇列**追蹤待訪問的候選格子。每次迭代中，取出一個元素、處理它、並將其新鮮鄰居加入佇列以維持 BFS 運行。

官方題解的參考實作應用了幾個優化技巧：

- **不使用 `visited` 表** — 直接*重用*輸入網格。將新鮮橘子標記為腐爛 (`grid[r][c] = 2`) 就等同於加入 visited 集合，且具有 O(1) 常數時間的陣列存取速度。
- **分隔符 / 哨兵** — 在佇列中插入特殊標記 `(row=-1, col=-1)`，放在初始腐爛橘子批次之後。當取出此哨兵時，代表一輪（一分鐘）已經過。接著再放入新的哨兵標記下一輪的結束。`minutesElapsed` 從 `-1` 開始，以補償第一次取出哨兵前尚未進行實際處理的情況。

### 演算法

1. **建立初始集合**：掃描網格，將所有腐爛格子入佇列，計算 `freshOranges`。在所有初始腐爛格子之後插入哨兵 `(-1, -1)`。
2. **BFS 迴圈**：當佇列非空：
   - 取出 `(row, col)`。
   - 若為哨兵 `(-1, -1)`：遞增 `minutesElapsed`。若佇列仍非空，插入另一個哨兵標記下一輪結束。
   - 否則：檢查 4 個鄰居，若鄰居是新鮮橘子，標記為腐爛、`freshOranges--`、入佇列。
3. **結果**：若 `freshOranges == 0`，回傳 `minutesElapsed`；否則回傳 `-1`。

### 複雜度

| 指標 | 值 |
|------|-----|
| 時間 | O(N × M) — 初始掃描 O(NM)，BFS 每格最多訪問一次 O(NM)，整體 O(NM)。 |
| 空間 | O(N × M) — 最壞情況下佇列容納所有格子。 |

### 本 repo 實作說明

本 repo 的 `Rotting Oranges.c` 使用**層大小（level-size）**技巧而非哨兵標記：在每輪開始時計算 `size = (rear - front) / 2`，恰好處理該數量的格子。這是分隔 BFS 層的等價方式，不需要插入虛擬元素。兩種方法達到相同的 O(NM) 時間和空間複雜度。

---

## 方法二：原地 BFS（In-place BFS）

### 直覺

Discussion 論壇中的一些使用者（如 @manky、@votrubac）提出了**常數 O(1) 額外空間**的 BFS 實作，完全消除佇列。

秘密在於使用*原地演算法*——直接轉換輸入來解決問題，不需要輔助資料結構。方法一中我們已經用網格本身做 visited 追蹤；此處更進一步，也用原地時間戳標記來**取代佇列**。

想法：在每一*輪* BFS 中，我們用特定的**時間戳**標記待訪問的格子。從 `timestamp = 2`（初始腐爛橘子）開始，每輪：

1. 掃描整個網格，找出當前時間戳值的格子。
2. 對每個這樣的格子，將其新鮮鄰居標記為 `timestamp + 1`。
3. 遞增時間戳並重複，直到沒有新格子被標記。

### 演算法

1. 從 `timestamp = 2` 開始，值為 `2` 的格子是初始腐爛橘子。
2. 對每個具有當前時間戳的格子，將新鮮鄰居標記為 `timestamp + 1`。
3. 標記完成後，下一輪的候選已在網格中標記好。
4. 重複步驟 (2) 直到沒有新候選產生。
5. 若仍有新鮮橘子，回傳 `-1`。否則回傳 `timestamp - 2`（經過的輪數）。

### 複雜度

| 指標 | 值 |
|------|-----|
| 時間 | O(N²M²) — 每輪 BFS 遍歷整個網格 O(NM)。最壞情況（鋸齒排列）下，最長傳播鏈可達 NM/2 輪。總計：O(NM) × O(NM) = O(N²M²)。 |
| 空間 | O(1) — 常數額外記憶體。網格原地修改。這是典型的時間換空間取捨。 |
