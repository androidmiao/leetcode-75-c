# 207. Course Schedule — Editorial

> Source: LeetCode Official Editorial (live authenticated session)
> Author: LeetCode · 291054 views · May 27, 2023

## Solution

### Overview

We are given a total of `numCourses` courses labeled from `0` to `numCourses - 1`, and an array `prerequisites` where `prerequisites[i] = [a_i, b_i]` indicates that course `b_i` must be taken first before course `a_i`. Our task is to determine whether it is possible to complete all courses.

---

### Approach 1: Topological Sort Using Kahn's Algorithm

#### Intuition

We can model the problem as a directed graph: each course is a node, and for each prerequisite `[a_i, b_i]` we draw an edge from `b_i` to `a_i` (meaning `b_i` must come before `a_i`).

If there is a cycle in this directed graph, we cannot finish all courses — a cycle means a set of courses that mutually depend on each other, creating an impossible ordering. Conversely, if the graph is acyclic, a valid **topological sort** exists and all courses can be completed.

A topological sort of a directed graph is a linear ordering of its vertices such that for every directed edge `u -> v`, vertex `u` comes before `v` in the ordering. In a directed acyclic graph (DAG), Kahn's algorithm produces one such ordering.

Kahn's algorithm works by repeatedly removing nodes with in-degree 0 (no prerequisites remaining). The key insight is that if a cycle exists, the nodes in the cycle will never have their in-degree reduced to 0, so they will never be removed. If after processing we have visited all `numCourses` nodes, the graph is acyclic.

#### Algorithm

1. Create an `indegree` array of length `numCourses`, where `indegree[x]` stores the number of incoming edges to node `x`.
2. Build an adjacency list `adj` where `adj[x]` contains all nodes that depend on `x`. For each `prerequisite` in `prerequisites`, add `prerequisite[0]` to `adj[prerequisite[1]]` and increment `indegree[prerequisite[0]]`.
3. Initialize a queue and push all nodes with `indegree == 0` (leaf/root nodes with no prerequisites).
4. Create a counter `nodesVisited = 0`.
5. While the queue is not empty:
   - Dequeue a `node`.
   - Increment `nodesVisited`.
   - For each `neighbor` in `adj[node]`, decrement `indegree[neighbor]` by 1 (simulating deletion of the edge `node -> neighbor`).
   - If `indegree[neighbor] == 0`, push `neighbor` into the queue.
6. Return `nodesVisited == numCourses`. If all nodes were visited, the graph has no cycle; otherwise it does.

#### Complexity Analysis

Let `n` = `numCourses` and `m` = `prerequisites.length`.

- **Time complexity: O(n + m)**
  - Building the adjacency list takes O(m). Initializing `indegree` takes O(n).
  - Each node is enqueued and dequeued at most once: O(n). Each edge is visited once when decrementing in-degrees: O(m).
- **Space complexity: O(n + m)**
  - The adjacency list stores O(m) edges. The `indegree` array uses O(n). The queue holds at most O(n) elements.

---

### Approach 2: Depth First Search (DFS)

#### Intuition

We can also detect cycles using DFS traversal. During DFS, we maintain a recursion stack (the current path from root to the node being explored). If we ever visit a node that is already on the recursion stack, we have found a **back edge**, which indicates a cycle.

To implement this, we use two tracking arrays:
- `visit[]`: marks whether a node has been fully explored (all descendants processed).
- `inStack[]`: marks whether a node is currently on the recursion stack.

When visiting a neighbor:
- If the neighbor is in `inStack`, a cycle is detected — return `true` (has cycle).
- If the neighbor is already `visit`ed but not in `inStack`, it was fully processed previously without finding a cycle — skip it.
- Otherwise, recurse into the unvisited neighbor.

After all neighbors of a node are processed, remove it from `inStack` and mark it as `visit`ed.

#### Algorithm

1. Build an adjacency list `adj` from `prerequisites`, same as Approach 1.
2. Create two boolean arrays `visit` and `inStack`, each of size `numCourses`.
3. For each node, run `dfs(node, adj, visit, inStack)`:
   - If `node` is in `inStack`, return `true` (cycle found).
   - If `node` is already `visit`ed, return `false` (no cycle from this node).
   - Mark `node` as visited and add to `inStack`.
   - For each `neighbor` of `node`, recursively call `dfs`. If it returns `true`, propagate `true`.
   - After processing all neighbors, set `inStack[node] = false` and return `false`.
4. If any `dfs` call returns `true`, return `false` (cannot finish). Otherwise return `true`.

#### Complexity Analysis

Let `n` = `numCourses` and `m` = `prerequisites.length`.

- **Time complexity: O(n + m)**
  - Building the adjacency list: O(m). Initializing `visit` and `inStack`: O(n).
  - The `dfs` function processes each node once (O(n)) and each edge once (O(m)).
- **Space complexity: O(n + m)**
  - The adjacency list: O(n + m). The `visit` and `inStack` arrays: O(n).
  - The recursion call stack: at most O(n) in the worst case.

---

## 中文翻譯

### 概述

給定 `numCourses` 門課程（編號 `0` 到 `numCourses - 1`）和一個先修條件陣列 `prerequisites`，其中 `prerequisites[i] = [a_i, b_i]` 表示必須先完成課程 `b_i` 才能修課程 `a_i`。判斷是否能完成所有課程。

---

### 方法一：Kahn 演算法拓撲排序（BFS）

#### 直覺

把每門課看作圖的節點，對每個先修條件 `[a, b]` 從 `b` 連一條有向邊到 `a`。如果有向圖中存在環，就不可能完成所有課程；如果是有向無環圖（DAG），就能做拓撲排序，所有課程可以完成。

Kahn 演算法反覆移除入度為 0 的節點。如果存在環，環上的節點入度永遠不會降到 0，因此不會被移除。處理結束後，若訪問了所有節點，代表圖無環。

#### 演算法步驟

1. 建立 `indegree` 陣列記錄每個節點的入度。
2. 建立鄰接表 `adj`，對每個先修條件 `[a, b]`，把 `a` 加入 `adj[b]`，同時 `indegree[a]++`。
3. 將所有入度為 0 的節點放入佇列。
4. 計數器 `nodesVisited = 0`。
5. 從佇列中取出節點，`nodesVisited++`；對其每個鄰居減少入度，若降到 0 則入佇列。
6. 最終返回 `nodesVisited == numCourses`。

#### 複雜度

- **時間：O(n + m)** — 建鄰接表 O(m)，BFS 處理每個節點和邊各一次。
- **空間：O(n + m)** — 鄰接表 O(m)，入度陣列與佇列 O(n)。

---

### 方法二：深度優先搜尋（DFS）

#### 直覺

使用 DFS 遍歷有向圖來偵測環。在 DFS 過程中維護一個遞迴堆疊（當前路徑），如果遇到一個已在遞迴堆疊中的節點，就表示發現了一條**回邊**（back edge），代表存在環。

實作上需要兩個追蹤陣列：
- `visit[]`：標記節點是否已完全探索完畢（所有後代已處理）。
- `inStack[]`：標記節點是否在當前遞迴堆疊上。

訪問鄰居時：
- 若鄰居在 `inStack` 中 → 發現環，返回 `true`。
- 若鄰居已 `visit` 但不在 `inStack` → 先前已安全處理，跳過。
- 否則遞迴進入未訪問的鄰居。

處理完節點的所有鄰居後，將其從 `inStack` 移除並標記為 `visit`。

#### 演算法步驟

1. 建鄰接表，同方法一。
2. 建兩個布林陣列 `visit` 和 `inStack`。
3. 對每個節點執行 `dfs`：
   - 若節點在 `inStack` 中 → 返回 `true`（有環）。
   - 若節點已 `visit` → 返回 `false`（此路徑無環）。
   - 標記已訪問並加入 `inStack`。
   - 遞迴處理所有鄰居，若任一返回 `true` 則傳遞 `true`。
   - 處理完畢後設 `inStack[node] = false`，返回 `false`。
4. 若任一 `dfs` 返回 `true`，則返回 `false`（無法完成）。否則返回 `true`。

#### 複雜度

- **時間：O(n + m)** — 每個節點和邊各處理一次。
- **空間：O(n + m)** — 鄰接表 O(n + m)，追蹤陣列 O(n)，遞迴堆疊最壞 O(n)。
