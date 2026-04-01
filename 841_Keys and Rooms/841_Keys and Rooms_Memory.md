# 841. Keys and Rooms - 記憶體參數佈局與運作追蹤

## 1. 記憶體參數佈局

### 函數簽名與參數大小

```c
bool canVisitAllRooms(int** rooms, int roomsSize, int* roomsColSize)
```

#### 參數欄位尺寸

| 參數名稱 | 資料型別 | 尺寸 | 說明 |
|---------|---------|------|------|
| `rooms` | `int**` | 8 bytes | 指向房間陣列的指標 |
| `roomsSize` | `int` | 4 bytes | 房間總數 (N) |
| `roomsColSize` | `int*` | 8 bytes | 指向各房間鑰匙數的陣列 |

#### 本地變數欄位尺寸

| 變數名稱 | 資料型別 | 尺寸 | 初始化 |
|---------|---------|------|--------|
| `visited` | `bool*` | 8 bytes | `calloc(roomsSize, sizeof(bool))` |
| `stack` | `int*` | 8 bytes | `malloc(sizeof(int) * roomsSize)` |
| `top` | `int` | 4 bytes | `0` (棧頂指標) |
| `visitedCount` | `int` | 4 bytes | `0` (已訪問房間計數器) |

### 假設記憶體位址 (Hypothetical Hex Addresses)

```
棧區域 (Stack Frame):
┌──────────────────────────────────────────────────┐
│ rsp + 40: rooms         → 0x7FFC0100              │
│ rsp + 32: roomsSize     = 4                       │
│ rsp + 24: roomsColSize  → 0x7FFC0150              │
│ rsp + 16: visited       → 0xAAAA0000              │
│ rsp +  8: stack         → 0xBBBB0000              │
│ rsp +  0: top, visitedCount (局部暫存器)         │
└──────────────────────────────────────────────────┘
```

---

## 2. 完整資料結構記憶體配置

### 例子 1: rooms = [[1],[2],[3],[]]

#### 棧記憶體配置表

| 名稱 | 位址 | 內容 | 說明 |
|------|------|------|------|
| `rooms` | 0x7FFC0100 | 0x1000 | 指向房間陣列的起始地址 |
| `roomsSize` | 0x7FFC0104 | 4 | 房間總數 |
| `roomsColSize` | 0x7FFC0108 | 0x7FFC0150 | 指向各房間鑰匙數陣列 |
| `visited` | 0x7FFC0110 | 0xAAAA0000 | 指向已訪問標記陣列 |
| `stack` | 0x7FFC0118 | 0xBBBB0000 | 指向棧陣列 |

#### 堆記憶體配置表

| 位址範圍 | 內容 | 描述 |
|---------|------|------|
| 0x1000～0x1020 | [0x2000, 0x3000, 0x4000, 0x5000] | `rooms` 陣列 (房間0～3 的鑰匙陣列指標) |
| 0x2000～0x2010 | [1] | `rooms[0]` 的鑰匙 (房間1的鑰匙) |
| 0x3000～0x3010 | [2] | `rooms[1]` 的鑰匙 (房間2的鑰匙) |
| 0x4000～0x4010 | [3] | `rooms[2]` 的鑰匙 (房間3的鑰匙) |
| 0x5000～0x5010 | [] | `rooms[3]` 的鑰匙 (空) |
| 0x7FFC0150～0x7FFC0160 | [1, 1, 1, 0] | `roomsColSize` (各房間鑰匙數) |
| 0xAAAA0000～0xAAAA0010 | [0,0,0,0] | `visited` 陣列 (初始全為false) |
| 0xBBBB0000～0xBBBB0020 | [?,?,?,?] | `stack` 陣列 (未初始化) |

#### 指標關係圖

```
        棧區域
┌─────────────────────────────┐
│ rooms ───▶ 0x1000          │
│ roomsSize = 4              │
│ roomsColSize ───▶ 0x7FFC0150
│ visited ───▶ 0xAAAA0000    │
│ stack ───▶ 0xBBBB0000      │
└─────────────────────────────┘

堆區域 - rooms 指標陣列:
┌─────────────────────────┐
│ 0x1000: rooms[0] ───┐   │
│ 0x1008: rooms[1] ───┼───┼─▶ 0x3000 [2]
│ 0x1010: rooms[2] ───┼───┼─▶ 0x4000 [3]
│ 0x1018: rooms[3] ───┼───┼─▶ 0x5000 []
│                     │   │
│                     ▼   │
│                  0x2000 [1]
└─────────────────────────┘

堆區域 - visited 陣列:
┌───────────────────────────┐
│ 0xAAAA0000: visited[0]=0  │
│ 0xAAAA0001: visited[1]=0  │
│ 0xAAAA0002: visited[2]=0  │
│ 0xAAAA0003: visited[3]=0  │
└───────────────────────────┘

堆區域 - stack 陣列:
┌──────────────────────────┐
│ 0xBBBB0000: stack[0]=?   │
│ 0xBBBB0004: stack[1]=?   │
│ 0xBBBB0008: stack[2]=?   │
│ 0xBBBB000C: stack[3]=?   │
└──────────────────────────┘
```

---

### 例子 2: rooms = [[1,3],[3,0,1],[2],[0]]

#### 棧記憶體配置表

| 名稱 | 位址 | 內容 | 說明 |
|------|------|------|------|
| `rooms` | 0x7FFC0100 | 0x2000 | 指向房間陣列的起始地址 |
| `roomsSize` | 0x7FFC0104 | 4 | 房間總數 |
| `roomsColSize` | 0x7FFC0108 | 0x7FFC0150 | 指向各房間鑰匙數陣列 |
| `visited` | 0x7FFC0110 | 0xAAAA1000 | 指向已訪問標記陣列 |
| `stack` | 0x7FFC0118 | 0xBBBB1000 | 指向棧陣列 |

#### 堆記憶體配置表

| 位址範圍 | 內容 | 描述 |
|---------|------|------|
| 0x2000～0x2020 | [0x3000, 0x4000, 0x5000, 0x6000] | `rooms` 陣列 |
| 0x3000～0x3010 | [1, 3] | `rooms[0]` 的鑰匙 |
| 0x4000～0x4020 | [3, 0, 1] | `rooms[1]` 的鑰匙 |
| 0x5000～0x5010 | [2] | `rooms[2]` 的鑰匙 |
| 0x6000～0x6010 | [0] | `rooms[3]` 的鑰匙 |
| 0x7FFC0150～0x7FFC0160 | [2, 3, 1, 1] | `roomsColSize` |
| 0xAAAA1000～0xAAAA1010 | [0,0,0,0] | `visited` 陣列 |
| 0xBBBB1000～0xBBBB1020 | [?,?,?,?] | `stack` 陣列 |

#### 指標關係圖

```
        棧區域
┌─────────────────────────────┐
│ rooms ───▶ 0x2000          │
│ roomsSize = 4              │
│ roomsColSize ───▶ 0x7FFC0150
│ visited ───▶ 0xAAAA1000    │
│ stack ───▶ 0xBBBB1000      │
└─────────────────────────────┘

堆區域 - rooms 指標陣列:
┌──────────────────────────┐
│ 0x2000: rooms[0] ───┐    │
│ 0x2008: rooms[1] ───┼────┼─▶ 0x4000 [3,0,1]
│ 0x2010: rooms[2] ───┼────┼─▶ 0x5000 [2]
│ 0x2018: rooms[3] ───┼────┼─▶ 0x6000 [0]
│                     │    │
│                     ▼    │
│                  0x3000 [1,3]
└──────────────────────────┘

堆區域 - visited 陣列:
┌──────────────────────────────┐
│ 0xAAAA1000: visited[0]=0   │
│ 0xAAAA1001: visited[1]=0   │
│ 0xAAAA1002: visited[2]=0   │
│ 0xAAAA1003: visited[3]=0   │
└──────────────────────────────┘

堆區域 - stack 陣列:
┌──────────────────────────┐
│ 0xBBBB1000: stack[0]=?   │
│ 0xBBBB1004: stack[1]=?   │
│ 0xBBBB1008: stack[2]=?   │
│ 0xBBBB100C: stack[3]=?   │
└──────────────────────────┘
```

---

## 3. DFS 操作過程追蹤

### 例子 1: rooms = [[1],[2],[3],[]]

#### 初始狀態

```
棧初始化完成:
┌────────────────────────────────────┐
│ rooms ───▶ 0x1000 [0x2000,...]   │
│ roomsSize = 4                      │
│ visited ───▶ 0xAAAA0000           │
│ stack ───▶ 0xBBBB0000             │
│ top = 0                            │
│ visitedCount = 0                   │
└────────────────────────────────────┘

初始記憶體:
visited:  [0, 0, 0, 0]
stack:    [?, ?, ?, ?]
```

#### DFS 步驟追蹤

**步驟 0: 初始化棧，推送房間0**

```
動作: stack[0] = 0; top = 1;
     visited[0] = true; visitedCount = 1;

棧狀態:
┌─────────────┐
│ stack[0]=0  │ ◄── top=1
│ stack[1]=?  │
│ stack[2]=?  │
│ stack[3]=?  │
└─────────────┘

visited 陣列:
0xAAAA0000: [1, 0, 0, 0]
             ▲

當前處理房間: 0
擁有的鑰匙: [1]
```

**步驟 1: 彈出房間0，訪問房間1**

```
動作: room = stack[--top]; // top=0, room=0
     keys = rooms[0] = [1] (在 0x2000)
     對於 key=1:
       if (!visited[1]) {
         stack[top++] = 1; // stack[0]=1, top=1
         visited[1] = true;
         visitedCount = 2;
       }

棧狀態:
┌──────────────────┐
│ stack[0]=1       │ ◄── top=1
│ stack[1]=?       │
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA0000: [1, 1, 0, 0]
             ▲  ▲

當前處理房間: 0 (彈出後)
新推送房間: 1
visitedCount: 2
```

**步驟 2: 彈出房間1，訪問房間2**

```
動作: room = stack[--top]; // top=0, room=1
     keys = rooms[1] = [2] (在 0x3000)
     對於 key=2:
       if (!visited[2]) {
         stack[top++] = 2; // stack[0]=2, top=1
         visited[2] = true;
         visitedCount = 3;
       }

棧狀態:
┌──────────────────┐
│ stack[0]=2       │ ◄── top=1
│ stack[1]=?       │
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA0000: [1, 1, 1, 0]
             ▲  ▲  ▲

當前處理房間: 1 (彈出後)
新推送房間: 2
visitedCount: 3
```

**步驟 3: 彈出房間2，訪問房間3**

```
動作: room = stack[--top]; // top=0, room=2
     keys = rooms[2] = [3] (在 0x4000)
     對於 key=3:
       if (!visited[3]) {
         stack[top++] = 3; // stack[0]=3, top=1
         visited[3] = true;
         visitedCount = 4;
       }

棧狀態:
┌──────────────────┐
│ stack[0]=3       │ ◄── top=1
│ stack[1]=?       │
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA0000: [1, 1, 1, 1]
             ▲  ▲  ▲  ▲

當前處理房間: 2 (彈出後)
新推送房間: 3
visitedCount: 4
```

**步驟 4: 彈出房間3，無鑰匙**

```
動作: room = stack[--top]; // top=0, room=3
     keys = rooms[3] = [] (在 0x5000, 空)
     無鑰匙推送

棧狀態:
┌──────────────────┐
│ stack[0]=?       │ ◄── top=0 (棧空)
│ stack[1]=?       │
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA0000: [1, 1, 1, 1]
             ▲  ▲  ▲  ▲

當前處理房間: 3 (彈出後)
可訪問房間數: 4
visitedCount: 4

迴圈結束 (top == 0)
```

#### 結論

```
return (visitedCount == roomsSize)
     = (4 == 4)
     = true ✓

所有房間已訪問！
```

---

### 例子 2: rooms = [[1,3],[3,0,1],[2],[0]]

#### 初始狀態

```
棧初始化完成:
┌────────────────────────────────────┐
│ rooms ───▶ 0x2000 [0x3000,...]   │
│ roomsSize = 4                      │
│ visited ───▶ 0xAAAA1000           │
│ stack ───▶ 0xBBBB1000             │
│ top = 0                            │
│ visitedCount = 0                   │
└────────────────────────────────────┘

初始記憶體:
visited:  [0, 0, 0, 0]
stack:    [?, ?, ?, ?]
```

#### DFS 步驟追蹤

**步驟 0: 初始化棧，推送房間0**

```
動作: stack[0] = 0; top = 1;
     visited[0] = true; visitedCount = 1;

棧狀態:
┌─────────────┐
│ stack[0]=0  │ ◄── top=1
│ stack[1]=?  │
│ stack[2]=?  │
│ stack[3]=?  │
└─────────────┘

visited 陣列:
0xAAAA1000: [1, 0, 0, 0]
             ▲

當前處理房間: 0
擁有的鑰匙: [1, 3] (在 0x3000)
```

**步驟 1: 處理房間0，推送房間3和房間1**

```
動作: room = 0 (棧頂，暫不彈出)
     keys = rooms[0] = [1, 3] (在 0x3000)

     對於 key=1:
       if (!visited[1]) {
         stack[1] = 1; top = 2;
         visited[1] = true;
         visitedCount = 2;
       }

     對於 key=3:
       if (!visited[3]) {
         stack[2] = 3; top = 3;
         visited[3] = true;
         visitedCount = 3;
       }

棧狀態:
┌──────────────────┐
│ stack[0]=0       │
│ stack[1]=1       │
│ stack[2]=3       │ ◄── top=3
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA1000: [1, 1, 0, 1]
             ▲  ▲     ▲

動作結束，開始下一輪迴圈
visitedCount: 3
```

**步驟 2: 彈出房間3，獲得房間0的鑰匙**

```
動作: room = stack[--top]; // top=2, room=3
     keys = rooms[3] = [0] (在 0x6000)

     對於 key=0:
       if (!visited[0]) {  // visited[0]=true，跳過
       }

棧狀態:
┌──────────────────┐
│ stack[0]=0       │
│ stack[1]=1       │
│ stack[2]=?       │ ◄── top=2
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA1000: [1, 1, 0, 1]
             ▲  ▲     ▲

當前處理房間: 3 (已彈出)
無新房間推送
visitedCount: 3
```

**步驟 3: 彈出房間1，獲得房間3、房間0和房間1的鑰匙**

```
動作: room = stack[--top]; // top=1, room=1
     keys = rooms[1] = [3, 0, 1] (在 0x4000)

     對於 key=3:
       if (!visited[3]) {  // visited[3]=true，跳過
       }

     對於 key=0:
       if (!visited[0]) {  // visited[0]=true，跳過
       }

     對於 key=1:
       if (!visited[1]) {  // visited[1]=true，跳過
       }

棧狀態:
┌──────────────────┐
│ stack[0]=0       │
│ stack[1]=?       │ ◄── top=1
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA1000: [1, 1, 0, 1]
             ▲  ▲     ▲

當前處理房間: 1 (已彈出)
無新房間推送 (所有鑰匙對應的房間已訪問)
visitedCount: 3
```

**步驟 4: 彈出房間0，推送房間2**

```
動作: room = stack[--top]; // top=0, room=0
     keys = rooms[0] = [1, 3] (在 0x3000)

     對於 key=1:
       if (!visited[1]) {  // visited[1]=true，跳過
       }

     對於 key=3:
       if (!visited[3]) {  // visited[3]=true，跳過
       }

棧狀態:
┌──────────────────┐
│ stack[0]=?       │ ◄── top=0 (棧空)
│ stack[1]=?       │
│ stack[2]=?       │
│ stack[3]=?       │
└──────────────────┘

visited 陣列:
0xAAAA1000: [1, 1, 0, 1]
             ▲  ▲     ▲

當前處理房間: 0 (已彈出)
無新房間推送
visitedCount: 3

迴圈結束 (top == 0)
```

#### 結論

```
return (visitedCount == roomsSize)
     = (3 == 4)
     = false ✗

房間2未被訪問！
```

---

## 4. 指標變數追蹤總表

### 例子 1: rooms = [[1],[2],[3],[]]

| 步驟 | 棧狀態 | 當前房間 | 獲得鑰匙 | 執行動作 | visitedCount |
|------|--------|---------|---------|---------|--------------|
| 0 | [0, ?, ?, ?] top=1 | — | — | 初始化，推送房間0 | 1 |
| 1 | [1, ?, ?, ?] top=1 | 0 | [1] | 彈出房間0，推送房間1 | 2 |
| 2 | [2, ?, ?, ?] top=1 | 1 | [2] | 彈出房間1，推送房間2 | 3 |
| 3 | [3, ?, ?, ?] top=1 | 2 | [3] | 彈出房間2，推送房間3 | 4 |
| 4 | [?, ?, ?, ?] top=0 | 3 | [] | 彈出房間3，無新房間 | 4 |
| 結束 | 棧空 | — | — | 返回 true | 4 |

### 例子 2: rooms = [[1,3],[3,0,1],[2],[0]]

| 步驟 | 棧狀態 | 當前房間 | 獲得鑰匙 | 執行動作 | visitedCount |
|------|--------|---------|---------|---------|--------------|
| 0 | [0, ?, ?, ?] top=1 | — | — | 初始化，推送房間0 | 1 |
| 1 | [0, 1, 3, ?] top=3 | 0 | [1, 3] | 訪問鑰匙1和3，推送房間1和3 | 3 |
| 2 | [0, 1, ?, ?] top=2 | 3 | [0] | 彈出房間3，房間0已訪問 | 3 |
| 3 | [0, ?, ?, ?] top=1 | 1 | [3,0,1] | 彈出房間1，所有鑰匙房間已訪問 | 3 |
| 4 | [?, ?, ?, ?] top=0 | 0 | [1, 3] | 彈出房間0，所有鑰匙房間已訪問 | 3 |
| 結束 | 棧空 | — | — | 返回 false | 3 |

---

## 5. 記憶體存取模式分析

### 5.1 鄰接表 (Adjacency List) vs. 鄰接矩陣 (Adjacency Matrix)

#### 方案A: 鄰接表 (目前實現)

```c
// int** rooms + 各房間的 int* 陣列
// 記憶體結構:
//   rooms: 指標陣列 (N × 8 bytes)
//   rooms[0]: 指標 → 房間0的鑰匙陣列
//   rooms[1]: 指標 → 房間1的鑰匙陣列
//   ...
//   rooms[i]: 指標 → 房間i的鑰匙陣列

// 空間複雜度: O(N + E)
//   N: 房間數量
//   E: 所有鑰匙總數
```

**優點:**
- 稀疏圖效率高 (只存儲存在的邊)
- 節省空間，特別是鑰匙數量少的情況
- 例子1: 4房間，4條邊 → 4 × 8 + 4 × 4 = 48 bytes (原始數據)

**缺點:**
- 多次指標解引用 (需要先找到 rooms[i]，再訪問鑰匙)
- 快取不友好 (記憶體分散在堆上各處)

#### 方案B: 鄰接矩陣 (假設實現)

```c
// bool visited_matrix[roomsSize][roomsSize]
// 記憶體結構:
//   矩陣[i][j] = true 表示房間i有房間j的鑰匙

// 空間複雜度: O(N²)
```

**優點:**
- 單次查詢時間常數 O(1)
- 記憶體連續，快取友好

**缺點:**
- 對稀疏圖浪費空間
- 例子1: 4房間 → 4 × 4 = 16 bytes (矩陣)
- 例子2: 4房間 → 4 × 4 = 16 bytes (矩陣)
- 對於大型稀疏圖，消耗是 O(N²)

#### 記憶體存取對比

| 操作 | 鄰接表 (本實現) | 鄰接矩陣 |
|------|-----------------|---------|
| 訪問房間i的所有鑰匙 | rooms[i] → int* | visited_matrix[i][] |
| 指標解引用次數 | 1次 + 遍歷陣列 | 0次 |
| 記憶體連續性 | 低 (分散在堆上) | 高 (單一矩陣) |
| 快取效率 | 低 (多次跳躍) | 高 (順序存取) |
| 適用情況 | 稀疏圖 | 稠密圖 |

### 5.2 基於棧的DFS vs. 遞迴DFS

#### 方案A: 基於棧的DFS (目前實現)

```c
bool canVisitAllRooms(int** rooms, int roomsSize, int* roomsColSize) {
    bool* visited = calloc(roomsSize, sizeof(bool));
    int* stack = malloc(sizeof(int) * roomsSize);
    int top = 0;
    int visitedCount = 0;

    // 初始化
    stack[top++] = 0;
    visited[0] = true;
    visitedCount++;

    // DFS 迴圈
    while (top > 0) {
        int room = stack[--top];
        for (int i = 0; i < roomsColSize[room]; i++) {
            int key = rooms[room][i];
            if (!visited[key]) {
                stack[top++] = key;
                visited[key] = true;
                visitedCount++;
            }
        }
    }

    bool result = (visitedCount == roomsSize);
    free(visited);
    free(stack);
    return result;
}
```

**記憶體使用:**
```
堆分配:
  visited: roomsSize × sizeof(bool) = 4 × 1 = 4 bytes
  stack:   roomsSize × sizeof(int)  = 4 × 4 = 16 bytes
  總計: 20 bytes

棧區域:
  局部變數: rooms, roomsSize, roomsColSize, visited, stack, top, visitedCount
         = 8 + 4 + 8 + 8 + 8 + 4 + 4 = 44 bytes
```

**優點:**
- 完全控制記憶體
- 避免棧溢出 (遞迴深度問題)
- 可預測的記憶體使用量

**缺點:**
- 手動管理堆記憶體 (malloc/free)
- 代碼較複雜
- 需要額外的棧陣列

#### 方案B: 遞迴DFS (假設實現)

```c
void dfs_recursive(int** rooms, int roomsSize, bool* visited, int room) {
    visited[room] = true;
    for (int i = 0; i < roomsSize; i++) {
        int key = rooms[room][i];
        if (key < roomsSize && !visited[key]) {
            dfs_recursive(rooms, roomsSize, visited, key);
        }
    }
}

bool canVisitAllRooms_recursive(int** rooms, int roomsSize, int* roomsColSize) {
    bool* visited = calloc(roomsSize, sizeof(bool));
    dfs_recursive(rooms, roomsSize, visited, 0);

    int count = 0;
    for (int i = 0; i < roomsSize; i++) {
        if (visited[i]) count++;
    }

    bool result = (count == roomsSize);
    free(visited);
    return result;
}
```

**記憶體使用:**
```
堆分配:
  visited: roomsSize × sizeof(bool) = 4 × 1 = 4 bytes

棧區域 (每層遞迴):
  第1層 (main): 8 + 4 + 8 + 8 = 28 bytes
  第2層 (dfs): rooms, roomsSize, visited, room, i = 8 + 4 + 8 + 4 + 4 = 28 bytes
  第3層 (dfs): 同上
  ...
  深度: O(roomsSize) 最壞情況
  總棧使用: 28 × roomsSize = 112 bytes (最壞)
```

**優點:**
- 代碼簡潔，邏輯清晰
- 自動記憶體管理

**缺點:**
- 棧空間成本高 (每次遞迴調用增加棧幀)
- 深度圖會導致棧溢出風險
- 例子1: 4層遞迴深度 × 28 bytes = 112 bytes
- 最壞情況: O(N) 深度

#### 記憶體成本對比表

| 項目 | 基於棧 (本實現) | 遞迴 |
|------|-----------------|------|
| 堆分配 | visited + stack = 20 bytes | visited = 4 bytes |
| 最深棧深度 | 1 (只有main) | N (遞迴深度) |
| 單層棧幀 | ~44 bytes | ~28 bytes |
| 總棧使用 | 44 bytes | 28 × N bytes |
| 例子1總記憶體 | 20 + 44 = 64 bytes | 4 + 28 × 4 = 116 bytes |
| 例子2總記憶體 | 20 + 44 = 64 bytes | 4 + 28 × 4 = 116 bytes |
| 棧溢出風險 | 低 | 高 (深圖) |
| 快取效率 | 低 (多次跳躍) | 中 (連續棧) |

### 5.3 訪問計數 vs. 遍歷檢查

#### 方案A: 計數器方式 (本實現)

```c
int visitedCount = 0;  // 4 bytes

// DFS 過程中每訪問新房間就遞增
visitedCount++;

// 最後比較
return (visitedCount == roomsSize);
```

**優點:**
- 快速檢查 O(1)
- 節省遍歷時間

**缺點:**
- 需要額外變數 (4 bytes)

#### 方案B: 遍歷檢查方式

```c
// DFS 後遍歷 visited 陣列計數
int visitedCount = 0;
for (int i = 0; i < roomsSize; i++) {
    if (visited[i]) visitedCount++;
}
return (visitedCount == roomsSize);
```

**優點:**
- 無需額外變數

**缺點:**
- O(N) 時間複雜度
- 多次記憶體訪問

#### 性能對比

| 操作 | 計數器方式 | 遍歷方式 |
|------|-----------|---------|
| 每次訪問新房間 | visitedCount++ (1 操作) | 無操作 |
| 最終檢查 | 1次比較 O(1) | O(N) 遍歷 |
| 例子1額外成本 | 4次 visitedCount++ | 4次比較 |
| 例子2額外成本 | 3次 visitedCount++ | 4次比較 |
| 變數儲存 | +4 bytes | 0 bytes |

---

## 總結

### 記憶體配置
- **例子1 (簡單線性圖)**: 訪問路徑 0→1→2→3，所有房間可達
- **例子2 (分支圖)**: 訪問路徑 0→[1,3]，房間2不可達 (無鑰匙)

### 棧操作
- 基於棧的迭代DFS 避免遞迴棧溢出
- 棧大小固定為 roomsSize，最壞情況下同時存儲所有房間編號

### 指標追蹤
- rooms[i] 進行 1次指標解引用，訪問房間i的鑰匙陣列
- visited[j] 進行 1次陣列索引，檢查房間j的訪問狀態
- stack[top] 進行 1次陣列索引，操作LIFO棧

### 時間複雜度: O(N + E)
- N: 房間數量
- E: 所有鑰匙總數 (邊數)

### 空間複雜度: O(N)
- visited: O(N)
- stack: O(N)
- 堆分配總計: O(N)
