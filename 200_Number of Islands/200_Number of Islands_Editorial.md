# 200. Number of Islands — Editorial

## Solution

### Approach #1: DFS [Accepted]

**Intuition**

Treat the 2D grid map as an undirected graph where there is an edge between two horizontally or vertically adjacent nodes of value `'1'`.

**Algorithm**

Linear scan the 2D grid map. If a node contains a `'1'`, it is a root node that triggers a Depth First Search. During DFS, every visited node should be set as `'0'` to mark it as visited. Count the number of root nodes that trigger DFS — this number is the number of islands, since each DFS starting at some root identifies one complete island.

**Complexity Analysis**

- Time complexity: O(M × N) where M is the number of rows and N is the number of columns.
- Space complexity: worst case O(M × N) in case the grid map is filled with lands where DFS goes by M × N deep.

### Approach #2: BFS [Accepted]

**Algorithm**

Linear scan the 2D grid map. If a node contains a `'1'`, it is a root node that triggers a Breadth First Search. Put it into a queue and set its value as `'0'` to mark it as visited. Iteratively search the neighbours of enqueued nodes until the queue becomes empty.

**Complexity Analysis**

- Time complexity: O(M × N) where M is the number of rows and N is the number of columns.
- Space complexity: O(min(M, N)) because in the worst case where the grid is filled with lands, the size of queue can grow up to min(M, N).

### Approach #3: Union Find (aka Disjoint Set) [Accepted]

**Algorithm**

Traverse the 2D grid map and union adjacent lands horizontally or vertically. At the end, return the number of connected components maintained in the UnionFind data structure.

The UnionFind class uses path compression in the `find` operation and union by rank in the `union` operation for near-constant amortised time per operation.

**Complexity Analysis**

- Time complexity: O(M × N) where M is the number of rows and N is the number of columns. Note that Union operation takes essentially constant time when UnionFind is implemented with both path compression and union by rank.
- Space complexity: O(M × N) as required by the UnionFind data structure.

---

# 200. 島嶼數量 — 官方題解

## 解法

### 方法一：DFS（深度優先搜尋）[通過]

**直覺**

將二維網格視為一個無向圖，在所有值為 `'1'` 且水平或垂直相鄰的節點之間存在邊。

**演算法**

線性掃描整個二維網格。當遇到一個 `'1'` 時，它就是一個根節點，觸發一次深度優先搜尋（DFS）。在 DFS 過程中，每個被訪問的節點都設為 `'0'`，標記為已訪問。統計觸發 DFS 的根節點數量即為島嶼數量，因為每次 DFS 從某個根節點出發會恰好標識一座完整的島嶼。

**複雜度分析**

- 時間複雜度：O(M × N)，其中 M 為列數、N 為欄數。
- 空間複雜度：最壞情況 O(M × N)，當整個網格全部為陸地時，DFS 遞迴深度最大達 M × N。

### 方法二：BFS（廣度優先搜尋）[通過]

**演算法**

線性掃描整個二維網格。當遇到一個 `'1'` 時，將其放入佇列並設為 `'0'` 標記為已訪問，觸發一次廣度優先搜尋（BFS）。反覆取出佇列中的節點並探索其鄰居，直到佇列為空。

**複雜度分析**

- 時間複雜度：O(M × N)，其中 M 為列數、N 為欄數。
- 空間複雜度：O(min(M, N))，因為最壞情況下（全部為陸地），佇列大小最多增長到 min(M, N)。

### 方法三：Union Find（並查集 / Disjoint Set）[通過]

**演算法**

遍歷整個二維網格，將水平或垂直相鄰的陸地合併（union）。最後回傳並查集（UnionFind）資料結構中維護的連通分量數目。

並查集使用「路徑壓縮」(path compression) 與「按秩合併」(union by rank) 兩項最佳化，使每次操作的攤銷時間接近常數。

**複雜度分析**

- 時間複雜度：O(M × N)，其中 M 為列數、N 為欄數。由於並查集同時使用路徑壓縮與按秩合併，Union 操作的攤銷時間接近 O(1)。
- 空間複雜度：O(M × N)，為並查集所需的額外空間。
