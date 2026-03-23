# 210. Course Schedule II — Editorial

> Source: LeetCode Official Editorial (live authenticated session)
> Author: LeetCode · 721970 views · Jan 26, 2023

## Solution

### Overview

This problem extends Course Schedule (207) — instead of just detecting whether all courses can be finished, we must return a valid ordering. The underlying structure is the same: model the courses and prerequisites as a directed graph, then produce a **topological sort** of the graph. If a cycle exists, no valid ordering is possible and we return an empty array.

We represent the information as a directed, unweighted graph: each course is a vertex, and for each prerequisite `[a, b]` we draw a directed edge `b → a` (meaning `b` must come before `a`). The graph may be cyclic — if so, the answer is an empty array.

---

### Approach 1: Using Depth First Search

#### Intuition

During DFS traversal, we explore all possible paths from a node before finishing with that node. If we use a stack (or build the result in reverse), the DFS post-order naturally gives us a valid topological ordering.

The key idea: when we finish processing a node (all its descendants have been fully explored), we push it onto a stack. This ensures all of a node's prerequisites appear after it in the stack — and when we read the stack from top to bottom, we get a valid course ordering.

We also need cycle detection: if during DFS we revisit a node that is currently on the recursion stack (a "gray" node), we have found a back edge, indicating a cycle.

Pseudocode:

```
let S be a stack of courses
function dfs(node)
    for each neighbor in adjacency list of node
        dfs(neighbor)
    push node to S
```

#### Algorithm

1. Initialize a stack (or result array built in reverse) to hold the topological order.
2. Build the adjacency list from the edge pairs. Note that `[a, b]` means edge `b → a`, so `a` depends on `b`.
3. For each unvisited node, run DFS.
4. Use a 3-state color marking:
   - **WHITE (0):** unvisited
   - **GRAY (1):** currently on the recursion stack (being explored)
   - **BLACK (2):** fully processed
5. If we encounter a GRAY node during DFS, a cycle exists — return an empty array.
6. After all neighbors of a node are processed, add the node to the result.
7. The result array (built in DFS post-order) is already a valid topological order.

#### Complexity Analysis

- **Time Complexity: O(V + E)** where V = numCourses and E = number of prerequisites. We visit each node once and each edge once during DFS.
- **Space Complexity: O(V + E)**. The adjacency list stores O(E) edges. The state array and result array use O(V). The recursion call stack is at most O(V) in the worst case.

---

### Approach 2: Using Node Indegree (Kahn's Algorithm)

#### Intuition

This approach is often more intuitive. The first node in a topological ordering must have no incoming edges (in-degree 0) — it has no prerequisites. We process all such nodes first, then remove their outgoing edges, which may create new nodes with in-degree 0, and repeat.

If after processing there are still nodes remaining (their in-degree never reached 0), those nodes are part of a cycle and no valid ordering exists.

This is **Kahn's Algorithm** for topological sorting.

#### Algorithm

1. Initialize a queue `Q` to track all nodes with 0 in-degree.
2. Build the adjacency list and compute the `indegree` array from the edge pairs.
3. Add all nodes with `indegree == 0` to `Q`.
4. While `Q` is not empty:
   - Dequeue a node `N`.
   - Add `N` to the result ordering.
   - For each neighbor of `N`, decrement its `indegree` by 1. If it reaches 0, enqueue it.
5. If the result contains all `numCourses` nodes, return it. Otherwise return an empty array (cycle detected).

#### Complexity Analysis

- **Time Complexity: O(V + E)**. We process each node once and each edge once.
- **Space Complexity: O(V + E)**. The in-degree array requires O(V). The queue holds at most O(V) elements. The adjacency list stores O(E) edges.

---

## 中文翻譯

### 概述

本題是 Course Schedule（207）的延伸 — 不僅要判斷是否能完成所有課程，還要返回一個合法的修課順序。底層結構相同：將課程和先修條件建模為有向圖，然後對圖做**拓撲排序**。若存在環，則無合法順序，返回空陣列。

以有向無權圖表示：每門課是一個頂點，每個先修條件 `[a, b]` 代表一條有向邊 `b → a`（`b` 必須在 `a` 前面）。若圖有環，答案為空陣列。

---

### 方法一：深度優先搜尋（DFS）

#### 直覺

DFS 遍歷中，我們在完成一個節點（所有後代已探索完畢）後，將其推入堆疊。這保證了所有先修課程都排在該課程之前。同時用三色標記法偵測環：若遇到正在遞迴堆疊中的 GRAY 節點，代表有回邊，存在環。

虛擬碼：

```
令 S 為課程堆疊
function dfs(node)
    對 node 鄰接表中的每個 neighbor
        dfs(neighbor)
    將 node 推入 S
```

#### 演算法步驟

1. 初始化結果陣列，用於存放拓撲排序。
2. 從邊對建立鄰接表。`[a, b]` 意味著邊 `b → a`，所以 `a` 依賴 `b`。
3. 對每個未訪問節點執行 DFS。
4. 使用三色標記：
   - **WHITE (0)**：未訪問
   - **GRAY (1)**：在當前遞迴堆疊上
   - **BLACK (2)**：已完成處理
5. 若遇到 GRAY 節點 → 有環 → 返回空陣列。
6. 處理完節點的所有鄰居後，將節點加入結果。
7. DFS 後序建出的結果陣列就是合法的拓撲排序。

#### 複雜度

- **時間：O(V + E)** — 每個節點和邊各處理一次。
- **空間：O(V + E)** — 鄰接表 O(E)，狀態與結果陣列 O(V)，遞迴堆疊最壞 O(V)。

---

### 方法二：入度法（Kahn 演算法）

#### 直覺

拓撲排序的第一個節點一定是入度為 0 的節點（沒有先修條件）。先處理所有這類節點，移除它們的出邊後可能產生新的入度為 0 的節點，重複此過程。若處理完後仍有剩餘節點（入度未降為 0），代表存在環。

#### 演算法步驟

1. 初始化佇列 `Q` 存放所有入度為 0 的節點。
2. 從邊對建立鄰接表並計算 `indegree` 陣列。
3. 將所有 `indegree == 0` 的節點加入 `Q`。
4. 當 `Q` 不為空：
   - 取出節點 `N`，加入結果排序。
   - 對 `N` 的每個鄰居，`indegree` 減 1。若降到 0 則入佇列。
5. 若結果包含所有 `numCourses` 個節點，返回結果；否則返回空陣列（有環）。

#### 複雜度

- **時間：O(V + E)** — 每個節點和邊各處理一次。
- **空間：O(V + E)** — 入度陣列 O(V)，佇列最多 O(V) 個元素，鄰接表 O(E)。
