# 207. Course Schedule — Memory & Array Index Diagrams

> 以下記憶體位址皆為假設值，用於說明演算法的指標操作與記憶體存取模式。

---

## Struct 記憶體佈局

本題的核心資料結構是鄰接表（adjacency list），由以下幾個陣列組成：

```c
int** adj;       // 指標陣列，adj[i] 指向課程 i 的先修課陣列
int* degrees;    // degrees[i] = adj[i] 的有效長度
int* state;      // state[i] = 0(WHITE), 1(GRAY), 2(BLACK)
```

### 單一 adj[i] 條目的記憶體佈局

假設 `numCourses = 4`，`adj[1]` 指向一個可容納 4 個 int 的陣列：

```
adj[1] @ 0xC000
┌────────┬────────┬────────┬────────┐
│ int[0] │ int[1] │ int[2] │ int[3] │
│  = 0   │  (空)  │  (空)  │  (空)  │
├────────┼────────┼────────┼────────┤
│ 4 bytes│ 4 bytes│ 4 bytes│ 4 bytes│
└────────┴────────┴────────┴────────┘
offset:  0        4        8       12

degrees[1] = 1  （只有 1 個有效元素）
```

---

## 完整資料結構記憶體配置

### 範例：numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]

#### 位址分配表

| 變數 | 假設位址 | 大小 | 內容 |
|------|---------|------|------|
| `adj` (指標陣列) | 0xA000 | 32 bytes (4 × 8B ptr) | 指向各課程的鄰居陣列 |
| `adj[0]` 陣列 | 0xB000 | 16 bytes (4 × 4B int) | `[]`（空） |
| `adj[1]` 陣列 | 0xC000 | 16 bytes (4 × 4B int) | `[0]` |
| `adj[2]` 陣列 | 0xD000 | 16 bytes (4 × 4B int) | `[0]` |
| `adj[3]` 陣列 | 0xE000 | 16 bytes (4 × 4B int) | `[1, 2]` |
| `degrees` | 0xF000 | 16 bytes (4 × 4B int) | `[0, 1, 1, 2]` |
| `state` | 0xF100 | 16 bytes (4 × 4B int) | `[0, 0, 0, 0]`（初始） |

#### ASCII 記憶體圖

```
adj @ 0xA000（指標陣列）
┌──────────┬──────────┬──────────┬──────────┐
│ adj[0]   │ adj[1]   │ adj[2]   │ adj[3]   │
│ =0xB000  │ =0xC000  │ =0xD000  │ =0xE000  │
└────┬─────┴────┬─────┴────┬─────┴────┬─────┘
     │          │          │          │
     ▼          ▼          ▼          ▼
  0xB000     0xC000     0xD000     0xE000
  ┌────┐     ┌────┐     ┌────┐     ┌────┬────┐
  │(空)│     │ 0  │     │ 0  │     │ 1  │ 2  │
  └────┘     └────┘     └────┘     └────┴────┘

degrees @ 0xF000
┌────┬────┬────┬────┐
│ 0  │ 1  │ 1  │ 2  │
└────┴────┴────┴────┘
 [0]  [1]  [2]  [3]

state @ 0xF100（初始全為 0 = WHITE）
┌────┬────┬────┬────┐
│ 0  │ 0  │ 0  │ 0  │
└────┴────┴────┴────┘
 [0]  [1]  [2]  [3]
```

#### 邏輯有向圖（附位址）

```
課程依賴方向（course → prereq）：

     3 (0xE000: [1,2])
    / \
   v   v
   1   2
(0xC000:[0]) (0xD000:[0])
    \ /
     v
     0
(0xB000: [])
```

---

## 陣列索引版對應

本題的圖結構用鄰接表表示，不像二元樹有隱式陣列對應。但 `adj`、`degrees`、`state` 三個陣列都以課程編號為索引，形成平行陣列（parallel arrays）：

```
索引 i:     0       1       2       3
           ┌───────┬───────┬───────┬───────┐
adj[i]:    │  []   │  [0]  │  [0]  │ [1,2] │  ← 鄰居列表
           ├───────┼───────┼───────┼───────┤
degrees[i]:│   0   │   1   │   1   │   2   │  ← 鄰居數量
           ├───────┼───────┼───────┼───────┤
state[i]:  │   0   │   0   │   0   │   0   │  ← DFS 狀態
           └───────┴───────┴───────┴───────┘
```

等價的鄰接矩陣表示（1 = 有邊，course → prereq）：

```
        prereq →  0   1   2   3
course ↓
   0            [ 0   0   0   0 ]
   1            [ 1   0   0   0 ]   ← 1→0
   2            [ 1   0   0   0 ]   ← 2→0
   3            [ 0   1   1   0 ]   ← 3→1, 3→2
```

---

## 搜尋過程追蹤

### Example 1: numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]

#### DFS 指標版追蹤

**外層迴圈 i=0：state[0]==0 → 呼叫 dfs(0)**

```
Step 1: dfs(0)
  state[0] = 1 (GRAY)
  degrees[0] = 0 → 無鄰居，跳過 for 迴圈
  state[0] = 2 (BLACK)
  return true

state 變化：
  ┌────┬────┬────┬────┐
  │ 2  │ 0  │ 0  │ 0  │
  └────┴────┴────┴────┘
   [0]  [1]  [2]  [3]
  BLACK WHITE WHITE WHITE
```

**外層迴圈 i=1：state[1]==0 → 呼叫 dfs(1)**

```
Step 2: dfs(1)
  state[1] = 1 (GRAY)
  遍歷 adj[1]: degrees[1]=1
    i=0: prereq = adj[1][0] = 0
         state[0] == 2 (BLACK) → 跳過（已完成）
  state[1] = 2 (BLACK)
  return true

state 變化：
  ┌────┬────┬────┬────┐
  │ 2  │ 2  │ 0  │ 0  │
  └────┴────┴────┴────┘
   [0]  [1]  [2]  [3]
  BLACK BLACK WHITE WHITE
```

**外層迴圈 i=2：state[2]==0 → 呼叫 dfs(2)**

```
Step 3: dfs(2)
  state[2] = 1 (GRAY)
  遍歷 adj[2]: degrees[2]=1
    i=0: prereq = adj[2][0] = 0
         state[0] == 2 (BLACK) → 跳過
  state[2] = 2 (BLACK)
  return true

state 變化：
  ┌────┬────┬────┬────┐
  │ 2  │ 2  │ 2  │ 0  │
  └────┴────┴────┴────┘
   [0]  [1]  [2]  [3]
  BLACK BLACK BLACK WHITE
```

**外層迴圈 i=3：state[3]==0 → 呼叫 dfs(3)**

```
Step 4: dfs(3)
  state[3] = 1 (GRAY)
  遍歷 adj[3]: degrees[3]=2
    i=0: prereq = adj[3][0] = 1
         state[1] == 2 (BLACK) → 跳過
    i=1: prereq = adj[3][1] = 2
         state[2] == 2 (BLACK) → 跳過
  state[3] = 2 (BLACK)
  return true

state 最終：
  ┌────┬────┬────┬────┐
  │ 2  │ 2  │ 2  │ 2  │
  └────┴────┴────┴────┘
   [0]  [1]  [2]  [3]
  BLACK BLACK BLACK BLACK
```

**結果：has_cycle = false → return true（可完成所有課程）**

---

### Example 2: numCourses = 2, prerequisites = [[1,0],[0,1]]

#### 位址分配

| 變數 | 假設位址 | 內容 |
|------|---------|------|
| `adj[0]` 陣列 | 0xB000 | `[1]` |
| `adj[1]` 陣列 | 0xC000 | `[0]` |
| `degrees` | 0xF000 | `[1, 1]` |
| `state` | 0xF100 | `[0, 0]`（初始） |

#### DFS 指標版追蹤

**外層迴圈 i=0：state[0]==0 → 呼叫 dfs(0)**

```
Step 1: dfs(0)
  state[0] = 1 (GRAY)

  state:
  ┌────┬────┐
  │ 1  │ 0  │
  └────┴────┘
   [0]  [1]
  GRAY  WHITE

  遍歷 adj[0]: degrees[0]=1
    i=0: prereq = adj[0][0] = 1
         state[1] == 0 (WHITE) → 遞迴 dfs(1)

Step 2: dfs(1)（遞迴呼叫）
  state[1] = 1 (GRAY)

  state:
  ┌────┬────┐
  │ 1  │ 1  │
  └────┴────┘
   [0]  [1]
  GRAY  GRAY   ← 兩個都是 GRAY！

  遍歷 adj[1]: degrees[1]=1
    i=0: prereq = adj[1][0] = 0
         state[0] == 1 (GRAY) ← ★ 發現回邊！有環！
         return false

Step 1 續: dfs(0) 收到 false → return false
```

**結果：has_cycle = true → return false（無法完成所有課程）**

#### 環偵測視覺化

```
遞迴堆疊（call stack）在偵測到環的瞬間：

┌─────────────────────┐
│ dfs(1)              │ ← 當前：檢查鄰居 0
│   state[1] = GRAY   │
├─────────────────────┤
│ dfs(0)              │ ← 呼叫者
│   state[0] = GRAY   │
├─────────────────────┤
│ canFinish()         │ ← 外層迴圈
└─────────────────────┘

圖中的回邊：
  0 ──GRAY──▶ 1 ──GRAY──▶ 0
              ↑                ↑
              └── 回邊（back edge）！環！
```

---

## 指標變數追蹤總表

### Example 1（無環）

| 步驟 | 函數呼叫 | course | adj 位址 | prereq | state[prereq] | 動作 |
|------|---------|--------|---------|--------|---------------|------|
| 1 | dfs(0) | 0 | 0xB000 | — | — | 無鄰居，直接 BLACK |
| 2 | dfs(1) | 1 | 0xC000 | 0 | 2 (BLACK) | 跳過 |
| 3 | dfs(2) | 2 | 0xD000 | 0 | 2 (BLACK) | 跳過 |
| 4a | dfs(3) | 3 | 0xE000 | 1 | 2 (BLACK) | 跳過 |
| 4b | dfs(3) | 3 | 0xE000 | 2 | 2 (BLACK) | 跳過 |

**統計：** 4 次 dfs 呼叫，5 次鄰居檢查，0 次遞迴深入，最大遞迴深度 = 1。

### Example 2（有環）

| 步驟 | 函數呼叫 | course | adj 位址 | prereq | state[prereq] | 動作 |
|------|---------|--------|---------|--------|---------------|------|
| 1 | dfs(0) | 0 | 0xB000 | 1 | 0 (WHITE) | 遞迴進入 dfs(1) |
| 2 | dfs(1) | 1 | 0xC000 | 0 | 1 (GRAY) | ★ 偵測到環！return false |

**統計：** 2 次 dfs 呼叫，2 次鄰居檢查，1 次遞迴深入，最大遞迴深度 = 2。

---

## 記憶體存取模式比較

### 鄰接表（Adjacency List）vs 鄰接矩陣（Adjacency Matrix）

| 面向 | 鄰接表（本題實作） | 鄰接矩陣 |
|------|-------------------|----------|
| 空間 | O(V + E)，稀疏圖高效 | O(V²)，稀疏圖浪費 |
| 遍歷鄰居 | 只走有效邊，O(degree) | 掃描整列，O(V) |
| Cache 行為 | 每個 adj[i] 是獨立 malloc，指標跳躍，cache miss 多 | 連續 2D 陣列，整列在同一 cache line，prefetch 友好 |
| 新增邊 | 需要考慮動態擴容（或預分配最壞大小） | 直接 `matrix[u][v] = 1`，O(1) |
| 判斷邊是否存在 | O(degree) 線性搜尋 | O(1) 直接索引 |

**本題的選擇：** 鄰接表。因為 V ≤ 2000、E ≤ 5000，圖是相當稀疏的。鄰接矩陣需要 2000×2000 = 4MB，而鄰接表只需大約 V + E ≈ 7000 個 int ≈ 28KB。

### 改進 Cache 效率的方式

目前實作每個 `adj[i]` 獨立 `malloc(numCourses * sizeof(int))`：

```
adj[0] → 0xB000（可能在 heap 的某處）
adj[1] → 0xC000（可能離很遠）
adj[2] → 0xD000（又是另一處）
```

改用 CSR（Compressed Sparse Row）格式可大幅改善：

```
一個大陣列 edges[] = [0, 0, 1, 2]   ← 所有邊連續存放
offset[] = [0, 0, 1, 2, 4]           ← adj[i] 的起始位置

遍歷 adj[i] 的鄰居：
  for (j = offset[i]; j < offset[i+1]; j++)
      neighbor = edges[j];
```

CSR 的好處：一次 malloc、連續記憶體、cache line prefetch 友好。

---

## 堆疊記憶體對比

### 遞迴 DFS vs 迭代 BFS（Kahn's）

#### 遞迴 DFS 堆疊使用（最壞情況）

```
假設鏈狀圖：0 → 1 → 2 → ... → 1999

call stack（最壞深度 = V = 2000）：
┌───────────────────────────┐
│ dfs(1999)                 │ ← 頂層
│   locals: course, i, prereq│
│   ~32 bytes               │
├───────────────────────────┤
│ dfs(1998)                 │
│   ~32 bytes               │
├───────────────────────────┤
│       ...（共 2000 層）    │
├───────────────────────────┤
│ dfs(0)                    │
│   ~32 bytes               │
├───────────────────────────┤
│ canFinish()               │
│   locals: adj, degrees, state, ...│
└───────────────────────────┘

估算：每層約 32-64 bytes × 2000 = 64-128 KB
User space stack (8MB)：✓ 完全安全
Kernel stack (8-16KB)：✗ 可能溢出！
```

#### 迭代 BFS（Kahn's）堆疊使用

```
call stack（固定深度 = 1）：
┌───────────────────────────┐
│ canFinish()               │
│   locals: adj, degrees,   │
│   indegree, queue, ...    │
│   ~堆上分配               │
└───────────────────────────┘

堆上（heap）：
  queue[]: 最多 V 個元素 = 2000 × 4 bytes = 8KB
  indegree[]: V 個元素 = 8KB

估算：stack 上幾乎不佔空間，所有資料在 heap
User space：✓
Kernel space：✓ （這就是 lockdep 選擇 BFS 的原因）
```

---

## 退化情況分析

### 最壞情況 1：鏈狀圖（最深遞迴）

```
prerequisites: [[1,0],[2,1],[3,2],...,[1999,1998]]

圖結構：
  1999 → 1998 → 1997 → ... → 1 → 0

DFS 從 1999 開始會一路遞迴到 0：
  遞迴深度 = 2000（最壞）
  每個節點恰好訪問一次
  時間 = O(V + E) = O(2000 + 1999)
```

### 最壞情況 2：完全圖的環（所有邊都存在）

```
V=2000, E=5000（約束上限）

若所有邊形成一個大環：
  0 → 1 → 2 → ... → 4999 → 0

DFS 在遍歷到第一條回邊時就會偵測到環並返回。
最壞情況下要走完整個環才發現回邊，但這仍是 O(V + E)。
```

### 最壞情況 3：記憶體過度分配

```
numCourses = 2000, prerequisites = []（沒有邊）

目前實作的鄰接表分配：
  adj: 2000 個指標 × 8 bytes = 16 KB
  每個 adj[i]: 2000 × 4 bytes = 8 KB
  總共：2000 × 8 KB + 16 KB ≈ 15.6 MB

實際需要的記憶體：幾乎為零（沒有邊）

改用 CSR 或動態陣列可將此降至 O(V) ≈ 8 KB。
```
