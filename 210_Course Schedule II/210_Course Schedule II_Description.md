# 210. Course Schedule II

**Difficulty:** Medium
**Topics:** Depth-First Search, Breadth-First Search, Graph, Topological Sort

## Problem Statement

There are a total of `numCourses` courses you have to take, labeled from `0` to `numCourses - 1`. You are given an array `prerequisites` where `prerequisites[i] = [a_i, b_i]` indicates that you **must** take course `b_i` first if you want to take course `a_i`.

- For example, the pair `[0, 1]`, indicates that to take course `0` you have to first take course `1`.

Return **the ordering of courses you should take to finish all courses**. If there are many valid answers, return **any** of them. If it is impossible to finish all courses, return **an empty array**.

## Examples

### Example 1

```
Input: numCourses = 2, prerequisites = [[1,0]]
Output: [0,1]
Explanation: There are a total of 2 courses to take. To take course 1 you should have
finished course 0. So the correct course order is [0,1].
```

### Example 2

```
Input: numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]
Output: [0,2,1,3]
Explanation: There are a total of 4 courses to take. To take course 3 you should have
finished both courses 1 and 2. Both courses 1 and 2 should be taken after you finished
course 0.
So one correct course order is [0,2,1,3]. Another correct ordering is [0,1,2,3].
```

### Example 3

```
Input: numCourses = 1, prerequisites = []
Output: [0]
```

## Constraints

- `1 <= numCourses <= 2000`
- `0 <= prerequisites.length <= numCourses * (numCourses - 1)`
- `prerequisites[i].length == 2`
- `0 <= a_i, b_i < numCourses`
- `a_i != b_i`
- All the pairs `[a_i, b_i]` are **distinct**.

## Hints

1. This problem is equivalent to finding the topological order in a directed graph. If a cycle exists, no topological ordering exists.
2. [Topological Sort via DFS](https://www.youtube.com/watch?v=ozso3xxkVGU) — A great video tutorial (21 minutes) on Coursera explaining the basic concepts of Topological Sort.
3. Topological sort could also be done via [BFS](http://en.wikipedia.org/wiki/Topological_sorting#Algorithms).

---

## 中文翻譯

### 題目描述

你總共需要修 `numCourses` 門課程，編號從 `0` 到 `numCourses - 1`。給定一個陣列 `prerequisites`，其中 `prerequisites[i] = [a_i, b_i]` 表示如果你想修課程 `a_i`，**必須**先完成課程 `b_i`。

- 例如，`[0, 1]` 這對表示要修課程 `0`，你必須先修課程 `1`。

返回**你應該修課的順序以完成所有課程**。若有多個合法答案，返回**任一**即可。若不可能完成所有課程，返回**空陣列**。

### 範例

#### 範例 1

```
輸入：numCourses = 2, prerequisites = [[1,0]]
輸出：[0,1]
說明：總共有 2 門課要修。要修課程 1，你必須先完成課程 0。
所以正確的修課順序是 [0,1]。
```

#### 範例 2

```
輸入：numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]
輸出：[0,2,1,3]
說明：總共有 4 門課要修。要修課程 3，你必須先完成課程 1 和 2。
課程 1 和 2 都必須在完成課程 0 之後才能修。
所以一個正確的修課順序是 [0,2,1,3]。另一個正確的排序是 [0,1,2,3]。
```

#### 範例 3

```
輸入：numCourses = 1, prerequisites = []
輸出：[0]
```

### 約束條件

- `1 <= numCourses <= 2000`
- `0 <= prerequisites.length <= numCourses * (numCourses - 1)`
- `prerequisites[i].length == 2`
- `0 <= a_i, b_i < numCourses`
- `a_i != b_i`
- 所有 `[a_i, b_i]` 配對皆**不重複**。

### 提示

1. 此問題等價於找出有向圖中的拓撲排序。若存在環，則不存在拓撲排序。
2. [透過 DFS 進行拓撲排序](https://www.youtube.com/watch?v=ozso3xxkVGU) — Coursera 上的 21 分鐘影片教學，解釋拓撲排序的基本概念。
3. 拓撲排序也可以透過 [BFS](http://en.wikipedia.org/wiki/Topological_sorting#Algorithms) 實現。
