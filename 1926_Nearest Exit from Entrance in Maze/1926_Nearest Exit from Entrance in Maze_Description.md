You are given an m x n matrix maze (0-indexed) with empty cells (represented as '.') and walls (represented as '+'). You are also given the entrance of the maze, where entrance = [entrancerow, entrancecol] denotes the row and column of the cell you are initially standing at.

In one step, you can move one cell up, down, left, or right. You cannot step into a cell with a wall, and you cannot step outside the maze. Your goal is to find the nearest exit from the entrance. An exit is defined as an empty cell that is at the border of the maze. The entrance does not count as an exit.

Return the number of steps in the shortest path from the entrance to the nearest exit, or -1 if no such path exists.

 

Example 1:


Input: maze = [["+","+",".","+"],[".",".",".","+"],["+","+","+","."]], entrance = [1,2]
Output: 1
Explanation: There are 3 exits in this maze at [1,0], [0,2], and [2,3].
Initially, you are at the entrance cell [1,2].
- You can reach [1,0] by moving 2 steps left.
- You can reach [0,2] by moving 1 step up.
It is impossible to reach [2,3] from the entrance.
Thus, the nearest exit is [0,2], which is 1 step away.
Example 2:


Input: maze = [["+","+","+"],[".",".","."],["+","+","+"]], entrance = [1,0]
Output: 2
Explanation: There is 1 exit in this maze at [1,2].
[1,0] does not count as an exit since it is the entrance cell.
Initially, you are at the entrance cell [1,0].
- You can reach [1,2] by moving 2 steps right.
Thus, the nearest exit is [1,2], which is 2 steps away.
Example 3:


Input: maze = [[".","+"]], entrance = [0,0]
Output: -1
Explanation: There are no exits in this maze.

Constraints:

maze.length == m
maze[i].length == n
1 <= m, n <= 100
maze[i][j] is either '.' or '+'.
entrance.length == 2
0 <= entrancerow < m
0 <= entrancecol < n
entrance will always be an empty cell.

Hint1: Which type of traversal lets you find the distance from a point?
Hint2: Try using a Breadth First Search.

---

你會得到一個 `m x n` 的迷宮矩陣 `maze`（從 `0` 開始編號），其中空格以 `'.'` 表示，牆壁以 `'+'` 表示。你也會得到迷宮的入口 `entrance`，其中 `entrance = [entrancerow, entrancecol]` 代表你一開始所在位置的列與行。

每一步你可以往上、下、左、右移動一格。你不能走進牆壁，也不能走出迷宮外。你的目標是找出從入口出發最近的出口。出口被定義為位於迷宮邊界上的空格。入口本身不算出口。

請回傳從入口到最近出口的最短路徑步數；如果不存在這樣的路徑，回傳 `-1`。

範例 1：

輸入：`maze = [["+","+",".","+"],[".",".",".","+"],["+","+","+","."]]`, `entrance = [1,2]`
輸出：`1`
解釋：這個迷宮中有 3 個出口，分別位於 `[1,0]`、`[0,2]` 和 `[2,3]`。
一開始你位於入口格 `[1,2]`。
- 你可以往左走 2 步到達 `[1,0]`。
- 你可以往上走 1 步到達 `[0,2]`。
無法從入口到達 `[2,3]`。
因此，最近的出口是 `[0,2]`，距離為 1 步。

範例 2：

輸入：`maze = [["+","+","+"],[".",".","."],["+","+","+"]]`, `entrance = [1,0]`
輸出：`2`
解釋：這個迷宮中只有 1 個出口，位於 `[1,2]`。
`[1,0]` 不算出口，因為它是入口格。
一開始你位於入口格 `[1,0]`。
- 你可以往右走 2 步到達 `[1,2]`。
因此，最近的出口是 `[1,2]`，距離為 2 步。

範例 3：

輸入：`maze = [[".","+"]]`, `entrance = [0,0]`
輸出：`-1`
解釋：這個迷宮中沒有任何出口。

限制條件：

`maze.length == m`
`maze[i].length == n`
`1 <= m, n <= 100`
`maze[i][j]` 只會是 `'.'` 或 `'+'`
`entrance.length == 2`
`0 <= entrancerow < m`
`0 <= entrancecol < n`
`entrance` 一定會是空格。

提示 1：哪一種走訪方式可以幫你找出某個點到其他位置的距離？
提示 2：試著使用廣度優先搜尋（Breadth First Search, BFS）。
