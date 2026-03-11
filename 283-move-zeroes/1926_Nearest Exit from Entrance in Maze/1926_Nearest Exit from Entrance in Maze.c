/*
這題使用 BFS（Breadth-First Search，廣度優先搜尋）。
原因是每走一步的成本都相同，所以 BFS 找到的第一個出口，
一定就是最短步數的出口。

時間複雜度：O(m * n)
空間複雜度：O(m * n)
*/

/*
每個變數的用途
BFS queue 的運作方式
為什麼 steps 要按層遞增
為什麼第一次碰到邊界空格就可以直接回傳
為什麼要把走過的位置改成 '+'
*/

// Breadth-First Search
// Time: O(m * n), Space: O(m * n)
int nearestExit(char** maze, int mazeSize, int* mazeColSize, int* entrance, int entranceSize) {
    // 迷宮總列數。
    int rows = mazeSize;
    // 迷宮總行數；LeetCode 這題每列長度相同，所以取第 0 列即可。
    int cols = mazeColSize[0];
    // BFS queue 的讀取指標，指向目前要處理的元素。
    int front = 0;
    // BFS queue 的寫入指標，指向下一個可放入的位置。
    int rear = 0;
    // 目前已經走了幾步；每處理完一層 BFS 就加 1。
    int steps = 0;
    // 入口的列座標。
    int startRow = entrance[0];
    // 入口的行座標。
    int startCol = entrance[1];
    // 四個移動方向：下、上、右、左。
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    // queueRows 與 queueCols 一起當作簡單 queue，分別存每個位置的列與行。
    int queueRows[10000];
    int queueCols[10000];

    // LeetCode 的函式簽名有 entranceSize，但這題解法中不需要實際使用它。
    (void)entranceSize;

    // 把入口放進 BFS queue。
    queueRows[rear] = startRow;
    // 入口的行座標也一起放進 queue。
    queueCols[rear] = startCol;
    // queue 已放入一個元素，所以 rear 往後移。
    rear++;
    // 把入口標記成已拜訪，避免之後重複走回來。
    maze[startRow][startCol] = '+';

    // 當 queue 還有節點可處理時，就持續做 BFS。
    while (front < rear) {
        // 目前這一層 BFS 有多少個節點要處理。
        int levelSize = rear - front;
        // 先把步數加 1，代表接下來要處理的是「再走一步」能到的所有位置。
        steps++;

        // 把同一層的所有節點逐一取出處理。
        for (int i = 0; i < levelSize; i++) {
            // 取出目前節點的列座標。
            int row = queueRows[front];
            // 取出目前節點的行座標。
            int col = queueCols[front];
            // 這個節點已取出，front 往後移到下一個。
            front++;

            // 嘗試往四個方向擴展。
            for (int d = 0; d < 4; d++) {
                // 算出下一個位置的列座標。
                int newRow = row + directions[d][0];
                // 算出下一個位置的行座標。
                int newCol = col + directions[d][1];

                // 如果超出迷宮邊界，這個方向無效，直接跳過。
                if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols) {
                    continue;
                }

                // 只有空地 '.' 可以走；牆壁 '+' 或已拜訪位置都不能再進去。
                if (maze[newRow][newCol] != '.') {
                    continue;
                }

                // 如果下一格是邊界上的空地，它就是出口，直接回傳目前步數。
                if (newRow == 0 || newRow == rows - 1 || newCol == 0 || newCol == cols - 1) {
                    return steps;
                }

                // 把這格標記為已拜訪，避免重複加入 queue。
                maze[newRow][newCol] = '+';
                // 將新位置加入 queue，之後在下一層 BFS 處理。
                queueRows[rear] = newRow;
                // 將新位置的行座標也加入 queue。
                queueCols[rear] = newCol;
                // queue 尾端往後移。
                rear++;
            }
        }
    }

    // 如果 BFS 做完都沒找到出口，代表無法離開迷宮。
    return -1;
}

/*
核心是用 BFS 找最短路徑。
gcc -std=c11 -Wall -Wextra -Werror -c '283-move-zeroes/1926_Nearest Exit from Entrance in Maze/1926_Nearest Exit from Entrance in Maze.c' -o /tmp/nearest-exit.o
這題要點很直接：從入口開始做 BFS，逐層擴展，第一次碰到邊界空格就是最近出口。程式我也做過編譯檢查，語法已通過。
*/

/*
這題要用廣度優先搜尋（BFS）。
因為每走一步的成本都一樣，都是 `1`，所以在這種無權重網格中，BFS 最適合拿來找最短路徑。從 `entrance` 開始，一層一層往外擴展，第一次碰到的出口一定就是最近的出口。

演算法：
1. 先把入口放進 queue。
2. 把入口標記成已拜訪，直接把 `'.'` 改成 `'+'`。
3. 進行逐層 BFS。
4. 對每個位置嘗試往四個方向移動。
5. 如果超出邊界，或遇到牆壁 / 已拜訪格子，就跳過。
6. 如果下一格是空格，且位於邊界，直接回傳目前步數。
7. 否則就把它標記為已拜訪，並加入 queue。
8. 如果整個 BFS 結束都找不到出口，回傳 `-1`。

為什麼正確：
- BFS 會先處理距離入口 `k` 步的所有格子，才會處理距離 `k + 1` 步的格子。
- 所以第一個找到的出口，必然是最近的出口。

複雜度分析：
- 時間複雜度：`O(m * n)`，因為每個格子最多只會進 queue 一次。
- 空間複雜度：`O(m * n)`，最壞情況 queue 可能存很多格子。
*/

/*
下面用題目 `Example 1` 畫 BFS 的 `queue` 變化圖。

範例迷宮：

```text
maze = [
  ['+','+','.','+'],
  ['.','.','.','+'],
  ['+','+','+','.']
]
entrance = [1,2]
```

座標圖：

```text
(row,col)

      0   1   2   3
0   [ + ][ + ][ . ][ + ]
1   [ . ][ . ][ E ][ + ]
2   [ + ][ + ][ + ][ . ]
```

`E = entrance = [1,2]`

出口有哪些：
- `[1,0]`
- `[0,2]`
- `[2,3]`

但不是每個都走得到。

**BFS 流程圖**

Step 0: 起點進 queue

```text
queue = [[1,2]]
visited:
      0   1   2   3
0   [ + ][ + ][ . ][ + ]
1   [ . ][ . ][ V ][ + ]
2   [ + ][ + ][ + ][ . ]
```

`V` 表示已拜訪。

Step 1: 從 `[1,2]` 往四個方向擴展

可走的位置：
- 上 `[0,2]`  -> 空地，而且在邊界
- 左 `[1,1]`  -> 空地
- 下 `[2,2]`  -> 牆
- 右 `[1,3]`  -> 牆

圖解：

```text
從 [1,2] 出發：

         [0,2]  <- 出口
            ^
            |
[1,1] <-  [1,2]  -> [1,3] 牆
            |
            v
         [2,2] 牆
```

因為 `[0,2]` 是這一層就碰到的邊界空格，所以答案直接是 `1`。

**Queue 變化圖**

實際觀念上可以想成：

```text
初始：
queue = [[1,2]]

處理第 1 層前：
steps = 1

從 [1,2] 檢查四周：
- [0,2] 是空地
- [0,2] 在邊界
=> 直接 return 1
```

所以這個例子很快，queue 幾乎還沒來得及長大就找到答案了。

---

如果你想看比較完整的 BFS 擴展，我用同一題假設「先不立刻返回」，讓你看 queue 怎麼長：

```text
初始
queue = [[1,2]]

彈出 [1,2]
加入可走鄰居：
queue = [[0,2], [1,1]]

再彈出 [0,2]
[0,2] 已經是出口
```

但在真正程式裡，發現 `[0,2]` 是出口的當下就回傳，不會真的繼續跑。

---

**為什麼 BFS 一定是最短路徑**

因為 BFS 是「一層一層」找：

- 第 1 層：距離入口 1 步
- 第 2 層：距離入口 2 步
- 第 3 層：距離入口 3 步

所以你第一次遇到出口時，它一定是最近的出口。

可以想成：

```text
第 0 層: [1,2]

第 1 層: 所有從 [1,2] 走 1 步能到的位置
第 2 層: 所有從 [1,2] 走 2 步能到的位置
第 3 層: 所有從 [1,2] 走 3 步能到的位置
...
```

因此：
- 先找到的一定步數更少
- 不可能後面才出現更短路徑

---

**你這支程式對應的 queue 概念**

在 [1926_Nearest Exit from Entrance in Maze.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze.c) 裡：

- `queueRows[]`、`queueCols[]`：一起表示 queue
- `front`：目前要取出的元素
- `rear`：下一個要放入的位置
- `levelSize = rear - front`：這一層有幾個節點
- `steps++`：每處理完一層，就代表多走了一步

簡化圖：

```text
queueRows: [1, 0, 1, ...]
queueCols: [2, 2, 1, ...]
            ^     ^
          front  rear
```

這代表 queue 裡依序存的是：

```text
[1,2], [0,2], [1,1], ...
```

如果你要，我下一步可以再幫你畫一版更完整的：
1. 「記憶體位址 / 陣列索引版 queue 圖」
2. 「Example 2 的 BFS 分層圖」
3. 「每一次 while 迴圈的 front / rear 變化表」
*/

/*
下面用你這支 BFS 程式的 queue 寫法來畫「陣列索引版 queue 圖」。

在 [1926_Nearest Exit from Entrance in Maze.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze.c) 裡，queue 不是用結構體，而是拆成兩個平行陣列：

```c
int queueRows[10000];
int queueCols[10000];
```

意思是：

- `queueRows[i]` 存第 `i` 個節點的 `row`
- `queueCols[i]` 存第 `i` 個節點的 `col`

合起來才是一個座標：

```text
第 i 筆節點 = (queueRows[i], queueCols[i])
```

---

**1. 初始放入 entrance**

假設入口是：

```text
entrance = [1,2]
```

執行這幾行：

```c
queueRows[rear] = startRow;
queueCols[rear] = startCol;
rear++;
```

一開始：

```text
front = 0
rear  = 0
```

放入後：

```text
queueRows index:   0    1    2    3    4   ...
queueRows value:  [1]  [?]  [?]  [?]  [?] ...

queueCols index:   0    1    2    3    4   ...
queueCols value:  [2]  [?]  [?]  [?]  [?] ...

front = 0
rear  = 1
```

這表示 queue 裡目前有一個元素：

```text
index 0 -> (1,2)
```

可視化：

```text
           front
             v
queue:    [ (1,2) ]
             ^
            rear-1

rear = 1  表示下一個新資料要放到 index 1
```

---

**2. 取出 queue[front]**

程式這樣取：

```c
int row = queueRows[front];
int col = queueCols[front];
front++;
```

取出前：

```text
queueRows: [1, ?, ?, ?, ...]
queueCols: [2, ?, ?, ?, ...]

front = 0
rear  = 1
```

取出的是：

```text
(queueRows[0], queueCols[0]) = (1,2)
```

取出後：

```text
front = 1
rear  = 1
```

這時 queue 在邏輯上是空的，因為：

```text
front == rear
```

注意：
- 陣列裡 `index 0` 的值還在
- 但邏輯上已經「不算 queue 內容」
- queue 是否有資料，是看 `front < rear`

---

**3. 如果擴展出兩個新節點**

假設從 `(1,2)` 找到兩個可走位置：

- `(0,2)`
- `(1,1)`

加入 queue：

```c
queueRows[rear] = newRow;
queueCols[rear] = newCol;
rear++;
```

第一個新點 `(0,2)` 放入：

```text
queueRows: [1, 0, ?, ?, ...]
queueCols: [2, 2, ?, ?, ...]

front = 1
rear  = 2
```

第二個新點 `(1,1)` 放入：

```text
queueRows: [1, 0, 1, ?, ...]
queueCols: [2, 2, 1, ?, ...]

front = 1
rear  = 3
```

現在 queue 邏輯上包含：

```text
index 1 -> (0,2)
index 2 -> (1,1)
```

畫成索引圖：

```text
index:        0       1       2       3
queueRows:   [1]     [0]     [1]     [?]
queueCols:   [2]     [2]     [1]     [?]

front = 1
rear  = 3
```

queue 範圍只看 `[front, rear)`：

```text
有效範圍 = [1, 3)

也就是：
index 1, 2 有效
index 0 無效（已出隊）
```

---

**4. 這就是陣列版 queue 的核心觀念**

不是把資料真的刪掉，而是靠 `front` / `rear` 控制有效範圍。

```text
有效 queue 區間 = front 到 rear - 1
```

例如：

```text
index:        0       1       2       3       4
queueRows:   [1]     [0]     [1]     [2]     [?]
queueCols:   [2]     [2]     [1]     [1]     [?]

front = 2
rear  = 4
```

此時邏輯上的 queue 是：

```text
index 2 -> (1,1)
index 3 -> (2,1)
```

雖然 index 0、1 還有舊值，但已經不算了。

---

**5. 記憶體視角**

可以把它想成兩塊連續記憶體：

```text
queueRows
地址:   base+0   base+4   base+8   base+12  ...
內容:     1        0        1        2

queueCols
地址:   base+0   base+4   base+8   base+12  ...
內容:     2        2        1        1
```

如果 `int` 是 4 bytes，則：

- `queueRows[0]` 在 `base`
- `queueRows[1]` 在 `base + 4`
- `queueRows[2]` 在 `base + 8`

同理 `queueCols` 也是另一塊自己的連續記憶體。

所以第 `i` 個節點座標是：

```text
row = queueRows[i]
col = queueCols[i]
```

也就是把兩塊記憶體的第 `i` 格配對起來。

---

**6. 對應你的程式流程**

這段：

```c
int row = queueRows[front];
int col = queueCols[front];
front++;
```

等於：

```text
讀出 queue 第 front 筆資料
把它當成目前 BFS 要處理的節點
然後 front 往右移
```

這段：

```c
queueRows[rear] = newRow;
queueCols[rear] = newCol;
rear++;
```

等於：

```text
把新發現的節點放到 queue 尾端
然後 rear 往右移
```

---

**7. 為什麼這樣就能 FIFO**

Queue 要求先進先出。

因為：

- 新資料永遠加在 `rear`
- 舊資料永遠從 `front` 取出

所以順序自然就是 FIFO。

圖：

```text
enqueue: 放 rear
dequeue: 取 front

        dequeue ->
index:   0    1    2    3    4
data:   [A]  [B]  [C]  [D]  [?]
         ^
       front

rear = 4
```

取出 `A` 後：

```text
index:   0    1    2    3    4
data:   [A]  [B]  [C]  [D]  [?]
              ^
            front

rear = 4
```

再加入 `E`：

```text
index:   0    1    2    3    4
data:   [A]  [B]  [C]  [D]  [E]
              ^
            front

rear = 5
```

邏輯順序就是：

```text
B -> C -> D -> E
```

---

**8. 你這題的 BFS 分層和 queue 關係**

BFS 每一層其實就是 queue 裡某一段連續區間。

```c
int levelSize = rear - front;
```

意思是：

```text
目前 queue 中，尚未處理的這一段長度
就是這一層 BFS 的節點數
```

例如：

```text
front = 3
rear  = 6
```

那這一層就有：

```text
index 3, 4, 5
```

共 3 個節點。

處理完這 3 個之後，新擴展出來的節點會接在後面，變成下一層。

---

如果你要，我下一步可以直接幫你畫：
1. `front / rear / levelSize` 的完整表格版
2. `Example 2` 的 queue 逐步變化圖
3. `queueRows[]` 和 `queueCols[]` 對應到迷宮座標的動畫式分步圖
*/

/*
下面用這題 `Example 2` 來畫 `front / rear / levelSize` 的完整表格，因為它比 `Example 1` 多跑幾步，較適合看 BFS queue 變化。

題目：

```text
maze = [
  ['+','+','+'],
  ['.','.','.'],
  ['+','+','+']
]
entrance = [1,0]
```

座標圖：

```text
      0   1   2
0   [ + ][ + ][ + ]
1   [ E ][ . ][ . ]
2   [ + ][ + ][ + ]
```

- 入口：`[1,0]`
- 出口：`[1,2]`
- 注意：`[1,0]` 雖然在邊界，但它是入口，不算出口

---

**初始狀態**

程式先做：

```c
queueRows[0] = 1;
queueCols[0] = 0;
rear++;
maze[1][0] = '+';
```

所以一開始：

```text
front = 0
rear  = 1
steps = 0
queue = [(1,0)]
```

---

**BFS 完整表格**

| 階段 | steps | front | rear | levelSize = rear-front | 本層要處理的節點 | 處理結果 |
|---|---:|---:|---:|---:|---|---|
| 初始入隊後 | 0 | 0 | 1 | 1 | `[(1,0)]` | 只有入口在 queue 中 |
| 進入 while 第 1 次 | 1 | 0 | 1 | 1 | `[(1,0)]` | 準備處理距離入口 1 步的位置 |
| 處理 `(1,0)` 前 | 1 | 0 | 1 | 1 | `[(1,0)]` | 取出 `queue[0]` |
| 處理 `(1,0)` 後 | 1 | 1 | 2 | 1 | `[(1,1)]` | `(1,1)` 可走，加入 queue |
| 進入 while 第 2 次 | 2 | 1 | 2 | 1 | `[(1,1)]` | 準備處理距離入口 2 步的位置 |
| 處理 `(1,1)` 前 | 2 | 1 | 2 | 1 | `[(1,1)]` | 取出 `queue[1]` |
| 處理 `(1,1)` 時 | 2 | 2 | 2 | 0 | `[]` | 檢查到 `(1,2)` 是邊界空地 |
| 直接 return | 2 | 2 | 2 | 0 | `[]` | 回傳 `2` |

---

**展開成更細的 queue 索引表**

### 0. 初始放入入口

```text
index:       0       1       2       3
queueRows:  [1]     [?]     [?]     [?]
queueCols:  [0]     [?]     [?]     [?]

front = 0
rear  = 1
steps = 0
```

邏輯上的 queue：

```text
[ (1,0) ]
```

---

### 1. 進入第一輪 while

先算：

```c
levelSize = rear - front = 1 - 0 = 1;
steps++;
```

所以：

```text
front = 0
rear  = 1
levelSize = 1
steps = 1
```

代表這一層只處理 1 個節點，也就是：

```text
(1,0)
```

---

### 2. 處理 `(1,0)`

取出：

```c
row = queueRows[front];   // 1
col = queueCols[front];   // 0
front++;
```

取出後：

```text
front = 1
rear  = 1
```

檢查四個方向：

- 下 `(2,0)` -> `+`，跳過
- 上 `(0,0)` -> `+`，跳過
- 右 `(1,1)` -> `.`，可走，但不是出口
- 左 `(1,-1)` -> 越界，跳過

因此加入 `(1,1)`：

```text
index:       0       1       2       3
queueRows:  [1]     [1]     [?]     [?]
queueCols:  [0]     [1]     [?]     [?]

front = 1
rear  = 2
```

邏輯上的 queue：

```text
[ (1,1) ]
```

第一層結束。

---

### 3. 進入第二輪 while

先算：

```c
levelSize = rear - front = 2 - 1 = 1;
steps++;
```

所以：

```text
front = 1
rear  = 2
levelSize = 1
steps = 2
```

代表這一層也只處理 1 個節點：

```text
(1,1)
```

---

### 4. 處理 `(1,1)`

取出：

```text
row = 1
col = 1
front = 2
```

此時：

```text
front = 2
rear  = 2
```

檢查四個方向：

- 下 `(2,1)` -> `+`
- 上 `(0,1)` -> `+`
- 右 `(1,2)` -> `.`
- 左 `(1,0)` -> 已拜訪，已改成 `+`

其中：

```text
(1,2)
```

符合：
- 是空地 `'.'`
- 在邊界
- 不是入口

所以直接：

```c
return steps;   // return 2
```

答案就是 `2`。

---

**你最該記住的三個數字**

1. `front`
表示下一個要取出的 queue 位置

2. `rear`
表示下一個要放入的新位置

3. `levelSize = rear - front`
表示「目前這一層 BFS 有幾個節點」

---

**為什麼 `levelSize` 很重要**

因為 BFS 要分層。

假設某一刻：

```text
front = 3
rear  = 7
```

那就代表目前 queue 中尚未處理的節點有：

```text
index 3, 4, 5, 6
```

共 4 個，所以：

```text
levelSize = 7 - 3 = 4
```

這 4 個是同一層，距離入口的步數相同。

等這 4 個都處理完之後，新加入 queue 的節點就屬於下一層，也就是「再多走一步」才到得到的格子。

---

**一個極簡公式版**

```text
queue 有效範圍 = [front, rear)
本層節點數     = rear - front
處理完一層後   = steps + 1
```

---

**你這題的 Example 2 最終變化摘要**

```text
初始:
front=0, rear=1, steps=0
queue=[(1,0)]

第1層:
levelSize=1, steps=1
處理 (1,0)
加入 (1,1)
結束後:
front=1, rear=2
queue=[(1,1)]

第2層:
levelSize=1, steps=2
處理 (1,1)
發現 (1,2) 是出口
return 2
```

如果你要，我下一步可以再幫你畫成更直觀的格式：
1. `front/rear` 指標左右移動的「時間軸圖」
2. `Example 1` 和 `Example 2` 的對照表
3. 直接對照你程式每一行的「變數值追蹤表」
*/