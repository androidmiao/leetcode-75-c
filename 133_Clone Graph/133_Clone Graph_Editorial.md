# 133. Clone Graph — Editorial

## Approach 1: Depth First Search (DFS)

### Intuition
The basic intuition is to copy nodes as we traverse the graph. Since the graph can have cycles (undirected edges mean A→B and B→A), we need a visited hash map to track already-cloned nodes and prevent infinite loops.

For an edge A-B, since A is connected to B and B is also connected to A, without a visited map we would get stuck in a cycle.

### Algorithm
1. Start traversing from the given node.
2. Use a hash map to store references of cloned nodes (key = original node, value = cloned node). If a node already exists in visited, return its clone.
3. If the node is not in visited, create a copy and add it to the hash map BEFORE entering recursion (critical to break cycles).
4. Recursively clone all neighbors. Each recursive call returns the clone of that neighbor. Collect these into the clone node's neighbor list.

### Complexity Analysis
- Time: O(N + M), where N = number of nodes, M = number of edges
- Space: O(N) for the visited hash map, plus O(H) recursion stack where H is the graph height. Overall O(N).

## Approach 2: Breadth First Search (BFS)

### Intuition
DFS uses recursion stack, which could be a concern for deep graphs. BFS offers an iterative alternative using a queue. Both approaches use the visited hash map identically.

### Algorithm
1. Use a hash map to store cloned nodes (key = original, value = clone).
2. Add the first node to the queue. Clone it and add to visited.
3. BFS traversal: dequeue a node, iterate its neighbors:
   - If neighbor already visited, get its clone from the map
   - Otherwise, create a clone, store in visited, enqueue the neighbor
   - Add the clone of the neighbor to the current clone's neighbor list

### Complexity Analysis
- Time: O(N + M)
- Space: O(N) for the visited dictionary plus O(W) for the queue where W is the width of the graph. Overall O(N).

---

# 133. 克隆圖 — 編輯社論

## 方法 1：深度優先搜尋 (DFS)

### 直覺
基本思路是在遍歷圖時複製節點。由於圖可能有環（無向邊表示 A→B 和 B→A），我們需要一個已訪問的哈希映射來追蹤已克隆的節點，並防止無限迴圈。

對於邊 A-B，由於 A 連接到 B，B 也連接到 A，如果沒有已訪問映射，我們會陷入迴圈。

### 演算法
1. 從給定節點開始遍歷。
2. 使用哈希映射來儲存克隆節點的引用（鍵 = 原始節點，值 = 克隆節點）。如果節點已存在於已訪問映射中，則返回其克隆。
3. 如果節點不在已訪問映射中，建立一份複本並將其新增到哈希映射中，然後進入遞迴（這對於破壞迴圈至關重要）。
4. 遞迴克隆所有鄰接點。每個遞迴呼叫都會返回該鄰接點的克隆。將這些收集到克隆節點的鄰接點列表中。

### 複雜度分析
- 時間複雜度：O(N + M)，其中 N = 節點數，M = 邊數
- 空間複雜度：O(N) 用於已訪問哈希映射，加上 O(H) 遞迴堆疊，其中 H 是圖的高度。總體 O(N)。

## 方法 2：廣度優先搜尋 (BFS)

### 直覺
DFS 使用遞迴堆疊，這對於深層圖可能會有問題。BFS 提供了一個使用佇列的迭代替代方案。兩種方法都以相同的方式使用已訪問哈希映射。

### 演算法
1. 使用哈希映射來儲存克隆節點（鍵 = 原始節點，值 = 克隆節點）。
2. 將第一個節點新增到佇列。克隆它並新增到已訪問映射中。
3. BFS 遍歷：出列一個節點，遍歷其鄰接點：
   - 如果鄰接點已訪問，則從映射中獲取其克隆
   - 否則，建立克隆，儲存在已訪問映射中，將鄰接點入佇列
   - 將鄰接點的克隆新增到當前克隆的鄰接點列表中

### 複雜度分析
- 時間複雜度：O(N + M)
- 空間複雜度：O(N) 用於已訪問字典，加上 O(W) 用於佇列，其中 W 是圖的寬度。總體 O(N)。
