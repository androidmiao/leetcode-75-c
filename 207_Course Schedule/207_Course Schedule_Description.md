# 207. Course Schedule

**Difficulty:** Medium
**Topics:** Depth-First Search, Breadth-First Search, Graph, Topological Sort

## Problem Statement

There are a total of `numCourses` courses you have to take, labeled from `0` to `numCourses - 1`. You are given an array `prerequisites` where `prerequisites[i] = [a_i, b_i]` indicates that you **must** take course `b_i` first if you want to take course `a_i`.

- For example, the pair `[0, 1]`, indicates that to take course `0` you have to first take course `1`.

Return `true` if you can finish all courses. Otherwise, return `false`.

## Examples

### Example 1

```
Input: numCourses = 2, prerequisites = [[1,0]]
Output: true
Explanation: There are a total of 2 courses to take.
To take course 1 you should have finished course 0. So it is possible.
```

### Example 2

```
Input: numCourses = 2, prerequisites = [[1,0],[0,1]]
Output: false
Explanation: There are a total of 2 courses to take.
To take course 1 you should have finished course 0, and to take course 0 you should
also have finished course 1. So it is impossible.
```

## Constraints

- `1 <= numCourses <= 2000`
- `0 <= prerequisites.length <= 5000`
- `prerequisites[i].length == 2`
- `0 <= a_i, b_i < numCourses`
- All the pairs `prerequisites[i]` are **unique**.

## Hints

1. This problem is equivalent to finding if a cycle exists in a directed graph. If a cycle exists, no topological ordering exists and therefore it will be impossible to take all courses.
2. [Topological Sort via DFS](https://www.cs.princeton.edu/~wayne/kleinberg-tardos/pdf/03Graphs.pdf) — A great tutorial explaining the basic concepts of Topological Sort.
3. Topological sort could also be done via [BFS](http://en.wikipedia.org/wiki/Topological_sorting#Algorithms).

---

## 中文翻譯

### 題目描述

你總共需要修 `numCourses` 門課程，編號從 `0` 到 `numCourses - 1`。給定一個陣列 `prerequisites`，其中 `prerequisites[i] = [a_i, b_i]` 表示如果你想修課程 `a_i`，**必須**先完成課程 `b_i`。

- 例如，`[0, 1]` 這對表示要修課程 `0`，你必須先修課程 `1`。

如果你能完成所有課程，返回 `true`；否則返回 `false`。

### 範例

#### 範例 1

```
輸入：numCourses = 2, prerequisites = [[1,0]]
輸出：true
說明：總共有 2 門課要修。
要修課程 1，你必須先完成課程 0。這是可行的。
```

#### 範例 2

```
輸入：numCourses = 2, prerequisites = [[1,0],[0,1]]
輸出：false
說明：總共有 2 門課要修。
要修課程 1，你必須先完成課程 0；而要修課程 0，你也必須先完成課程 1。
這是不可能的。
```

### 約束條件

- `1 <= numCourses <= 2000`
- `0 <= prerequisites.length <= 5000`
- `prerequisites[i].length == 2`
- `0 <= a_i, b_i < numCourses`
- 所有 `prerequisites[i]` 配對皆**不重複**。

### 提示

1. 此問題等價於判斷有向圖中是否存在環。若存在環，則不存在拓撲排序，因此不可能完成所有課程。
2. [透過 DFS 進行拓撲排序](https://www.cs.princeton.edu/~wayne/kleinberg-tardos/pdf/03Graphs.pdf) — 一篇很好的拓撲排序基本概念教學。
3. 拓撲排序也可以透過 [BFS](http://en.wikipedia.org/wiki/Topological_sorting#Algorithms) 實現。
