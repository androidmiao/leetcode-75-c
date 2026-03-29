# 133. Clone Graph

**Difficulty:** Medium

## Problem Statement

Given a reference of a node in a connected undirected graph, return a deep copy (clone) of the graph.

Each node in the graph contains a value (int) and a list (List[Node]) of its neighbors.

```
class Node {
    public int val;
    public List<Node> neighbors;
}
```

## Test Case Format

For simplicity, each node's value is the same as the node's index (1-indexed). The first node has val == 1, the second has val == 2, and so on. The graph is represented as an adjacency list.

An adjacency list is a collection of unordered lists used to represent a finite graph. Each list describes the set of neighbors of a node in the graph.

The given node will always be the first node with val = 1. You must return the copy of the given node as a reference to the cloned graph.

## Examples

### Example 1
- Input: adjList = [[2,4],[1,3],[2,4],[1,3]]
- Output: [[2,4],[1,3],[2,4],[1,3]]
- Explanation: There are 4 nodes in the graph. 1st node (val=1) neighbors are 2nd (val=2) and 4th (val=4). 2nd node (val=2) neighbors are 1st (val=1) and 3rd (val=3). 3rd node (val=3) neighbors are 2nd (val=2) and 4th (val=4). 4th node (val=4) neighbors are 1st (val=1) and 3rd (val=3).

### Example 2
- Input: adjList = [[]]
- Output: [[]]
- Explanation: The input contains one empty list. The graph consists of only one node with val=1 and no neighbors.

### Example 3
- Input: adjList = []
- Output: []
- Explanation: This is an empty graph with no nodes.

## Constraints
- The number of nodes in the graph is in the range [0, 100].
- 1 <= Node.val <= 100
- Node.val is unique for each node.
- There are no repeated edges and no self-loops in the graph.
- The Graph is connected and all nodes can be visited starting from the given node.

---

# 133. 克隆圖

**難度：** 中等

## 問題敘述

給定一個連通無向圖的某個節點的引用，返回該圖的深拷貝（克隆）。

圖中的每個節點都包含一個值（int）和一個鄰接節點的列表（List[Node]）。

```
class Node {
    public int val;
    public List<Node> neighbors;
}
```

## 測試案例格式

為了簡化起見，每個節點的值等於該節點的索引（1-indexed）。第一個節點的 val == 1，第二個節點的 val == 2，以此類推。圖以鄰接表的形式表示。

鄰接表是用來表示有限圖的無序列表集合。每個列表描述了圖中某個節點的鄰接節點集合。

給定的節點始終是值為 1 的第一個節點。你必須返回給定節點作為被克隆圖的引用。

## 範例

### 範例 1
- 輸入：adjList = [[2,4],[1,3],[2,4],[1,3]]
- 輸出：[[2,4],[1,3],[2,4],[1,3]]
- 解釋：圖中有 4 個節點。第 1 個節點（val=1）的鄰接節點是第 2 個（val=2）和第 4 個（val=4）。第 2 個節點（val=2）的鄰接節點是第 1 個（val=1）和第 3 個（val=3）。第 3 個節點（val=3）的鄰接節點是第 2 個（val=2）和第 4 個（val=4）。第 4 個節點（val=4）的鄰接節點是第 1 個（val=1）和第 3 個（val=3）。

### 範例 2
- 輸入：adjList = [[]]
- 輸出：[[]]
- 解釋：輸入包含一個空列表。該圖只有一個值為 1 的節點，且沒有鄰接節點。

### 範例 3
- 輸入：adjList = []
- 輸出：[]
- 解釋：這是一個沒有任何節點的空圖。

## 限制條件
- 圖中節點的數量在範圍 [0, 100] 內。
- 1 <= Node.val <= 100
- 每個節點的 Node.val 是唯一的。
- 圖中沒有重複邊和自環。
- 該圖是連通的，從給定節點出發可以訪問所有節點。
