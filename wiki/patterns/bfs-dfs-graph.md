# BFS / DFS / Graph 圖論搜尋模式

## 概述 Overview

圖論搜尋是解決連通性、路徑尋找和拓撲排序問題的基礎。深度優先搜尋（DFS）和寬度優先搜尋（BFS）是兩種核心遍歷策略。DFS 可以用遞迴或顯式堆棧實現，適合拓撲排序和環偵測。BFS 使用隊列，適合找最短路徑和層級遍歷。

本模式涵蓋圖的表示方法、DFS/BFS 的實現、拓撲排序和多源 BFS 等多個方面。

## 核心概念 Core Concepts

### 1. 圖的表示方法

**鄰接表表示（推薦用於稀疏圖）：**
```c
// 使用鄰接表
#define MAX_NODES 100
typedef struct {
    int* neighbors;
    int neighborCount;
} GraphNode;

typedef struct {
    GraphNode* nodes;
    int nodeCount;
} Graph;
```

**鄰接矩陣表示（用於稠密圖）：**
```c
// 使用鄰接矩陣
int graph[MAX_NODES][MAX_NODES];
```

### 2. 深度優先搜尋 DFS - 遞迴

DFS 適合樹形結構遍歷和需要後序遍歷的問題。

```c
// 訪問標記
bool visited[MAX_NODES];

void dfs(int node, GraphNode* nodes) {
    visited[node] = true;

    for (int i = 0; i < nodes[node].neighborCount; i++) {
        int neighbor = nodes[node].neighbors[i];
        if (!visited[neighbor]) {
            dfs(neighbor, nodes);
        }
    }
}
```

### 3. 深度優先搜尋 DFS - 迭代

使用顯式堆棧實現迭代 DFS，避免遞迴深度限制。

```c
// 定義堆棧
typedef struct {
    int items[MAX_NODES];
    int top;
} Stack;

void stack_push(Stack* s, int item) {
    s->items[s->top++] = item;
}

int stack_pop(Stack* s) {
    return s->items[--s->top];
}

void dfs_iterative(int start, GraphNode* nodes, int nodeCount) {
    bool visited[MAX_NODES] = {false};
    Stack stack = {0};

    stack_push(&stack, start);
    visited[start] = true;

    while (stack.top > 0) {
        int node = stack_pop(&stack);

        for (int i = 0; i < nodes[node].neighborCount; i++) {
            int neighbor = nodes[node].neighbors[i];
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                stack_push(&stack, neighbor);
            }
        }
    }
}
```

### 4. 寬度優先搜尋 BFS

BFS 找到最短路徑，適合層級遍歷。

```c
// 定義隊列
typedef struct {
    int items[MAX_NODES];
    int front, rear;
} Queue;

void queue_enqueue(Queue* q, int item) {
    q->items[q->rear++] = item;
}

int queue_dequeue(Queue* q) {
    return q->items[q->front++];
}

bool queue_empty(Queue* q) {
    return q->front >= q->rear;
}

void bfs(int start, GraphNode* nodes, int nodeCount) {
    bool visited[MAX_NODES] = {false};
    Queue queue = {0};

    queue_enqueue(&queue, start);
    visited[start] = true;

    while (!queue_empty(&queue)) {
        int node = queue_dequeue(&queue);

        for (int i = 0; i < nodes[node].neighborCount; i++) {
            int neighbor = nodes[node].neighbors[i];
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue_enqueue(&queue, neighbor);
            }
        }
    }
}
```

### 5. 拓撲排序

使用 DFS 進行拓撲排序，記錄後序遍歷順序。

```c
typedef struct {
    int items[MAX_NODES];
    int top;
} Stack;

void topological_sort_dfs(int node, bool* visited, Stack* result,
                         GraphNode* nodes) {
    visited[node] = true;

    for (int i = 0; i < nodes[node].neighborCount; i++) {
        int neighbor = nodes[node].neighbors[i];
        if (!visited[neighbor]) {
            topological_sort_dfs(neighbor, visited, result, nodes);
        }
    }

    // 後序：將節點加入結果
    result->items[result->top++] = node;
}

void topological_sort(GraphNode* nodes, int nodeCount, int* result) {
    bool visited[MAX_NODES] = {false};
    Stack stack = {0};

    for (int i = 0; i < nodeCount; i++) {
        if (!visited[i]) {
            topological_sort_dfs(i, visited, &stack, nodes);
        }
    }

    // 反轉結果以獲得拓撲順序
    for (int i = 0; i < nodeCount; i++) {
        result[i] = stack.items[nodeCount - 1 - i];
    }
}
```

### 6. 環偵測

使用色彩標記（白、灰、黑）檢測有向圖中的環。

```c
// 0 = 白色（未訪問），1 = 灰色（正在訪問），2 = 黑色（已訪問）
int color[MAX_NODES];
bool has_cycle = false;

bool has_cycle_dfs(int node, GraphNode* nodes) {
    color[node] = 1;  // 標記為灰色

    for (int i = 0; i < nodes[node].neighborCount; i++) {
        int neighbor = nodes[node].neighbors[i];

        if (color[neighbor] == 1) {
            // 找到後邊，存在環
            return true;
        } else if (color[neighbor] == 0) {
            // 未訪問，繼續搜尋
            if (has_cycle_dfs(neighbor, nodes)) {
                return true;
            }
        }
    }

    color[node] = 2;  // 標記為黑色
    return false;
}
```

## 常見變體 Common Variants

### 變體 1：島嶼數量 (Problem 200)

在二維網格上計算連通分量。

```c
void mark_island(int** grid, int gridSize, int* gridColSize,
                 int row, int col) {
    if (row < 0 || row >= gridSize || col < 0 || col >= gridColSize[0]) {
        return;
    }
    if (grid[row][col] == '0') {
        return;
    }

    grid[row][col] = '0';  // 標記為已訪問

    mark_island(grid, gridSize, gridColSize, row + 1, col);
    mark_island(grid, gridSize, gridColSize, row - 1, col);
    mark_island(grid, gridSize, gridColSize, row, col + 1);
    mark_island(grid, gridSize, gridColSize, row, col - 1);
}

int numIslands(char** grid, int gridSize, int* gridColSize) {
    int count = 0;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridColSize[0]; j++) {
            if (grid[i][j] == '1') {
                mark_island(grid, gridSize, gridColSize, i, j);
                count++;
            }
        }
    }
    return count;
}
```

### 變體 2：課程排序 (Problem 207/210)

檢測課程依賴圖中的環，並進行拓撲排序。

```c
bool can_finish(int numCourses, int** prerequisites, int prerequisitesSize) {
    // 構建圖
    int* in_degree = (int*)calloc(numCourses, sizeof(int));
    int** adj = (int**)malloc(numCourses * sizeof(int*));

    for (int i = 0; i < prerequisitesSize; i++) {
        int course = prerequisites[i][0];
        int prereq = prerequisites[i][1];
        in_degree[course]++;
    }

    // Kahn 算法：基於入度的拓撲排序
    int* queue = (int*)malloc(numCourses * sizeof(int));
    int front = 0, rear = 0;

    for (int i = 0; i < numCourses; i++) {
        if (in_degree[i] == 0) {
            queue[rear++] = i;
        }
    }

    int count = 0;
    while (front < rear) {
        int course = queue[front++];
        count++;

        for (int i = 0; i < prerequisitesSize; i++) {
            if (prerequisites[i][1] == course) {
                in_degree[prerequisites[i][0]]--;
                if (in_degree[prerequisites[i][0]] == 0) {
                    queue[rear++] = prerequisites[i][0];
                }
            }
        }
    }

    free(in_degree);
    free(adj);
    free(queue);

    return count == numCourses;
}
```

### 變體 3：多源 BFS (Problem 994)

從多個起點同時開始 BFS，用於找距離最近的目標。

```c
int orangesRotting(int** grid, int gridSize, int* gridColSize) {
    int rows = gridSize, cols = gridColSize[0];
    int queue[rows * cols][2];
    int front = 0, rear = 0;
    int fresh_count = 0;
    int minutes = 0;

    // 初始化：加入所有爛橙子
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 2) {
                queue[rear][0] = i;
                queue[rear][1] = j;
                rear++;
            } else if (grid[i][j] == 1) {
                fresh_count++;
            }
        }
    }

    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    while (front < rear && fresh_count > 0) {
        int size = rear - front;

        for (int i = 0; i < size; i++) {
            int row = queue[front][0];
            int col = queue[front][1];
            front++;

            for (int d = 0; d < 4; d++) {
                int new_row = row + directions[d][0];
                int new_col = col + directions[d][1];

                if (new_row >= 0 && new_row < rows &&
                    new_col >= 0 && new_col < cols &&
                    grid[new_row][new_col] == 1) {
                    grid[new_row][new_col] = 2;
                    queue[rear][0] = new_row;
                    queue[rear][1] = new_col;
                    rear++;
                    fresh_count--;
                }
            }
        }
        minutes++;
    }

    return fresh_count == 0 ? minutes : -1;
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 133 | Clone Graph | Med | DFS + Hash Map 複製圖 |
| 200 | Number of Islands | Med | DFS/BFS 計算連通分量 |
| 207 | Course Schedule | Med | 拓撲排序+環偵測 |
| 210 | Course Schedule II | Med | DFS 後序拓撲排序 |
| 841 | Keys and Rooms | Med | DFS 可達性檢驗 |
| 994 | Rotting Oranges | Med | 多源 BFS 層級遍歷 |
| 1926 | Nearest Exit from Entrance in Maze | Med | BFS 最短路徑 |

## 內核連接 Kernel Connection

Linux 內核在以下地方使用圖論演算法：

- **lockdep（鎖依賴檢測）**：使用 DFS 檢測死鎖（環偵測）
- **CPU 拓撲**：使用 BFS 遍歷 CPU 層級結構
- **設備發現**：使用 DFS 遍歷設備樹
- **記憶體管理**：使用圖論演算法進行頁面遷移和 NUMA 優化

## 時間和空間複雜度

- **DFS**：
  - 時間複雜度：O(V + E)
  - 空間複雜度：O(V)（遞迴堆棧或顯式堆棧）

- **BFS**：
  - 時間複雜度：O(V + E)
  - 空間複雜度：O(V)（隊列）

- **拓撲排序**：
  - 時間複雜度：O(V + E)
  - 空間複雜度：O(V)

## 實踐建議 Practice Tips

1. 理解圖的表示方法，選擇合適的數據結構
2. 掌握 DFS 的遞迴和迭代實現
3. 熟悉 BFS 的隊列實現
4. 練習識別何時使用 DFS（深度優先）或 BFS（層級順序）
5. 對於拓撲排序，学習兩種方法：DFS 和 Kahn 算法
6. 理解環偵測的色彩標記方法
7. 在實現前，先在紙上畫出圖結構

## 常見錯誤 Common Mistakes

1. **忘記標記已訪問的節點**：導致無限迴圈
2. **混淆 DFS 和 BFS**：使用錯誤的數據結構（堆棧 vs 隊列）
3. **環偵測邏輯錯誤**：混淆色彩狀態的含義
4. **邊界檢查不足**：特別在二維網格上容易出錯
5. **忘記初始化數據結構**：導致隨機行為
