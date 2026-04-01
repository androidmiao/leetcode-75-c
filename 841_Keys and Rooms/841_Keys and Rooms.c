/*
 * 841. Keys and Rooms
 * 難度：Medium
 * 分類：Graphs — DFS
 * 連結：https://leetcode.com/problems/keys-and-rooms/
 *
 * 時間複雜度：O(N + E)  N = 房間數，E = 所有鑰匙總數
 * 空間複雜度：O(N)       visited 陣列 + stack 各最多 N
 *
 * ===== 演算法概述 =====
 *
 * 核心做法：非遞迴 DFS（用自建 stack 模擬）
 *   1. 從 room 0 出發，將其放入 stack 並標記 visited
 *   2. 每次從 stack 取出一個房間 → 走訪其中所有鑰匙
 *   3. 遇到尚未拜訪的房間 → 標記 + 計數 + 推入 stack
 *   4. stack 清空後，visitedCount == roomsSize 即可全部到達
 *
 * ===== 範例 1 走訪圖解：rooms = [[1],[2],[3],[]] =====
 *
 *   有向圖（鄰接表 = rooms）：
 *
 *       ┌─────┐  key 1  ┌─────┐  key 2  ┌─────┐  key 3  ┌─────┐
 *       │  0  │───────▶│  1  │───────▶│  2  │───────▶│  3  │
 *       └─────┘         └─────┘         └─────┘         └─────┘
 *
 *   DFS 過程（stack 變化）：
 *
 *   步驟  取出  stack(後)  visited           count
 *   ───── ───── ─────────  ──────────────── ─────
 *   init  -     [0]        [T F F F]          1
 *    1    0     [1]        [T T F F]          2
 *    2    1     [2]        [T T T F]          3
 *    3    2     [3]        [T T T T]          4
 *    4    3     []         [T T T T]          4
 *
 *   count(4) == roomsSize(4) → return true
 *
 * ===== 範例 2 走訪圖解：rooms = [[1,3],[3,0,1],[2],[0]] =====
 *
 *   有向圖：
 *
 *       ┌─────┐  key 1  ┌─────┐
 *       │  0  │───────▶│  1  │
 *       │     │◀───────│     │
 *       └──┬──┘  key 0  └──┬──┘
 *     key 3│    key 3      │key 3
 *          ▼               ▼
 *       ┌─────┐  key 0  ┌ ─ ─ ┐
 *       │  3  │───────▶  │  0  │  (已拜訪，不再入 stack)
 *       └─────┘         └ ─ ─ ┘
 *
 *       ┌─────┐
 *       │  2  │  ← 鑰匙 2 只存在 room 2 裡，永遠拿不到
 *       └─────┘
 *
 *   count(3) != roomsSize(4) → return false
 */

// 提供 bool、true、false
#include <stdbool.h>
// 提供 malloc、calloc、free
#include <stdlib.h>

// LeetCode 給定的函式格式：
// rooms      : 二維陣列，rooms[i] 代表第 i 個房間內有哪些鑰匙
// roomsSize  : 房間總數
// roomsColSize : rooms[i] 的長度，也就是每個房間裡有幾把鑰匙
bool canVisitAllRooms(int** rooms, int roomsSize, int* roomsColSize) {
    // visited[i] = true 代表第 i 個房間已經拜訪過
    // calloc 會順便把所有值初始化成 0，也就是 false
    bool* visited = calloc(roomsSize, sizeof(bool));

    // stack 用來模擬 DFS
    // 最多只會把每個房間放進去一次，所以大小開 roomsSize 就夠了
    int* stack = malloc(sizeof(int) * roomsSize);

    // top 指向目前 stack 的下一個可放位置
    // 也可以理解成目前 stack 內元素的數量
    int top = 0;

    // visitedCount 記錄目前總共成功拜訪了幾個房間
    int visitedCount = 0;

    // 一開始一定能進入 0 號房間，所以先放進 stack
    stack[top++] = 0;

    // 標記 0 號房間已拜訪
    visited[0] = true;

    // 目前已拜訪房間數為 1
    visitedCount = 1;

    // 只要 stack 還有待處理的房間，就持續 DFS
    while (top > 0) {
        // 取出 stack 最上面的房間
        // --top 表示先把 top 減 1，再取出該位置的值
        int room = stack[--top];

        // 走訪目前這個房間裡的每一把鑰匙
        for (int i = 0; i < roomsColSize[room]; i++) {
            // nextRoom 是這把鑰匙可以打開的房間編號
            int nextRoom = rooms[room][i];

            // 如果這個房間還沒去過，就表示現在新發現了一個可達房間
            if (!visited[nextRoom]) {
                // 先標記已拜訪，避免之後重複加入 stack
                visited[nextRoom] = true;

                // 已拜訪房間數加 1
                visitedCount++;

                // 把新房間放進 stack，之後再繼續探索它裡面的鑰匙
                stack[top++] = nextRoom;
            }
        }
    }

    // 動態配置的記憶體用完後要釋放，避免記憶體洩漏
    free(visited);
    free(stack);

    // 如果拜訪數量等於總房間數，代表所有房間都能到達
    return visitedCount == roomsSize;
}

/*
**區塊講解**

這份程式的核心是「從 `0` 號房間開始做 DFS，看看最後能不能把所有房間都標記成已拜訪」。程式本體在 [841_Keys and Rooms.c:37](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L37)。

**1. 標頭與函式宣告**

[841_Keys and Rooms.c:28](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L28) 到 [841_Keys and Rooms.c:37](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L37) 是基本準備。

- `#include <stdbool.h>`：讓 C 可以使用 `bool`、`true`、`false`
- `#include <stdlib.h>`：讓 C 可以使用 `malloc`、`calloc`、`free`
- `bool canVisitAllRooms(...)`：LeetCode 指定的函式格式

參數意思：
- `rooms`：二維陣列，`rooms[i]` 是第 `i` 個房間裡的鑰匙
- `roomsSize`：總房間數
- `roomsColSize`：每個房間的鑰匙數量

**2. 建立 DFS 需要的資料結構**

[841_Keys and Rooms.c:38](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L38) 到 [841_Keys and Rooms.c:51](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L51)

這段是在準備搜尋用的工具：

- `visited`：記錄每個房間有沒有去過
- `stack`：模擬 DFS 的堆疊
- `top`：目前 stack 的長度，也可看成下一個可放資料的位置
- `visitedCount`：統計已經走到幾個房間

關鍵點：
- `calloc(roomsSize, sizeof(bool))` 會把整塊記憶體初始化成 0，所以一開始全部都是 `false`
- `stack` 開 `roomsSize` 大小就夠了，因為每個房間最多只會被放進去一次

**3. 初始化起點**

[841_Keys and Rooms.c:53](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L53) 到 [841_Keys and Rooms.c:60](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L60)

這段是在設定初始狀態：

- `stack[top++] = 0;`：把 `0` 號房間放進 stack
- `visited[0] = true;`：標記 `0` 號房間已拜訪
- `visitedCount = 1;`：目前已拜訪 1 個房間

`stack[top++] = 0` 要這樣理解：
- 先把 `0` 放到 `stack[top]`
- 再把 `top` 加 1

一開始 `top = 0`，所以執行後等於：
- `stack[0] = 0`
- `top = 1`

**4. DFS 主迴圈**

[841_Keys and Rooms.c:62](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L62) 到 [841_Keys and Rooms.c:85](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L85)

這是整題最重要的區塊。

`while (top > 0)` 表示：
- 只要 stack 還有房間沒處理，就繼續搜尋

`int room = stack[--top];` 表示：
- 先把 `top` 減 1
- 再取出 stack 最上層的房間

這就是 stack 的「後進先出」行為。

然後：

- `for (int i = 0; i < roomsColSize[room]; i++)`
  代表掃描目前房間中的每一把鑰匙
- `int nextRoom = rooms[room][i];`
  取得這把鑰匙能打開的房間
- `if (!visited[nextRoom])`
  如果這個房間還沒去過，就做三件事：
  - 標記已拜訪
  - `visitedCount++`
  - 放進 stack，之後再繼續探索

這樣就能一路把可到達的房間全部走完。

**5. 釋放記憶體與回傳答案**

[841_Keys and Rooms.c:87](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L87) 到 [841_Keys and Rooms.c:92](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L92)

- `free(visited);`
- `free(stack);`

這兩行是在釋放動態配置的記憶體，避免 memory leak。

最後：
- `return visitedCount == roomsSize;`

意思是：
- 如果拜訪到的房間數量等於總房間數，回傳 `true`
- 否則回傳 `false`

**6. 用一個小例子理解**

如果輸入是 `rooms = [[1],[2],[3],[]]`：

1. 先進 `0`，拿到 `1`
2. 再進 `1`，拿到 `2`
3. 再進 `2`，拿到 `3`
4. 再進 `3`

最後 `visitedCount = 4`，剛好等於 `roomsSize = 4`，所以回傳 `true`。

**7. 時間與空間複雜度**

- 時間複雜度：`O(N + E)`
  - `N` 是房間數
  - `E` 是所有鑰匙總數
- 空間複雜度：`O(N)`
  - `visited` 和 `stack` 都最多用到 `N`

如果你要，我可以下一步把這份程式再整理成「圖解版」，用 `rooms = [[1],[2],[3],[]]` 一步一步畫出 `stack`、`visited`、`visitedCount` 的變化。
*/

/*
用題目範例 `rooms = [[1],[2],[3],[]]` 來看最直觀。程式從 [841_Keys and Rooms.c:53](/Users/alex.miao/Documents/GitHub/leetcode-75-c/841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c#L53) 開始把 `0` 號房間放進 stack，然後反覆從 stack 取出房間、拿鑰匙、把新房間再放回 stack。

初始狀態：
```text
rooms = [[1],[2],[3],[]]

visited = [true, false, false, false]
stack   = [0]
top     = 1
count   = 1
```

第 1 輪，取出 `0`：
```text
room = stack[--top]
room = 0

stack   = []
top     = 0
```

查看 `rooms[0] = [1]`，表示在 0 號房間拿到鑰匙 1，所以把 1 號房間標記為可進入，並放進 stack：
```text
visited = [true, true, false, false]
stack   = [1]
top     = 1
count   = 2
```

第 2 輪，取出 `1`：
```text
room = 1

stack   = []
top     = 0
```

查看 `rooms[1] = [2]`，拿到鑰匙 2：
```text
visited = [true, true, true, false]
stack   = [2]
top     = 1
count   = 3
```

第 3 輪，取出 `2`：
```text
room = 2

stack   = []
top     = 0
```

查看 `rooms[2] = [3]`，拿到鑰匙 3：
```text
visited = [true, true, true, true]
stack   = [3]
top     = 1
count   = 4
```

第 4 輪，取出 `3`：
```text
room = 3

stack   = []
top     = 0
```

查看 `rooms[3] = []`，沒有新鑰匙，所以不會再加入任何房間。此時 `while (top > 0)` 結束。

最後判斷：
```text
visitedCount = 4
roomsSize    = 4
```

所以回傳 `true`。

如果你把它想成地圖，就是：

```text
0 --key--> 1 --key--> 2 --key--> 3
```

從 `0` 出發一路都拿得到下一間的鑰匙，因此全部房間都能到。

再看失敗例子 `rooms = [[1,3],[3,0,1],[2],[0]]`：
```text
0 可以到 1、3
1 可以到 3、0、1
3 可以到 0
但是永遠拿不到 2 的鑰匙
```

圖會像這樣：
```text
0 <----> 1
|        |
v        v
3 ------>0

2  自己鎖住自己

所以 `2` 永遠不會被標記成 `visited`，最後 `visitedCount != roomsSize`，回傳 `false`。

你也可以把這份程式濃縮成一句話理解：  
「用 stack 從 `0` 號房間開始做 DFS，只要拿到新鑰匙，就把對應房間加入探索，最後看是否所有房間都被拜訪。」
*/