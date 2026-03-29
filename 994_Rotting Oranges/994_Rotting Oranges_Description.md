# 994. Rotting Oranges

## Problem Statement

You are given an `m x n` grid where each cell can have one of three values:

- `0` representing an empty cell,
- `1` representing a fresh orange,
- `2` representing a rotten orange.

Every minute, any fresh orange that is **4-directionally adjacent** (up, down, left, right) to a rotten orange becomes rotten.

Return *the minimum number of minutes that must elapse until no cell has a fresh orange*. If this is impossible, return `-1`.

## Examples

### Example 1

```
Input: grid = [[2,1,1],[1,1,0],[0,1,1]]
Output: 4
```

**Explanation:**

```
Minute 0:    Minute 1:    Minute 2:    Minute 3:    Minute 4:
2 1 1        2 2 1        2 2 2        2 2 2        2 2 2
1 1 0        2 1 0        2 2 0        2 2 0        2 2 0
0 1 1        0 1 1        0 1 1        0 2 1        0 2 2
```

### Example 2

```
Input: grid = [[2,1,1],[0,1,1],[1,0,1]]
Output: -1
```

**Explanation:** The orange in the bottom-left corner `(row 2, column 0)` is never reached because it has no adjacent rotten orange path to it.

### Example 3

```
Input: grid = [[0,2]]
Output: 0
```

**Explanation:** Since there are no fresh oranges at minute 0, the answer is 0.

## Constraints

- `m == grid.length`
- `n == grid[i].length`
- `1 <= m, n <= 10`
- `grid[i][j]` is `0`, `1`, or `2`.

---

# 994. 腐爛的橘子

## 題目敘述

給定一個 `m x n` 的網格 `grid`，每個格子有三種可能的值：

- `0` 代表空格，
- `1` 代表新鮮橘子，
- `2` 代表腐爛橘子。

每過一分鐘，所有與腐爛橘子 **四方向相鄰**（上、下、左、右）的新鮮橘子都會變成腐爛。

回傳 *直到所有格子都沒有新鮮橘子所需的最少分鐘數*。如果不可能，回傳 `-1`。

## 範例

### 範例 1

```
輸入: grid = [[2,1,1],[1,1,0],[0,1,1]]
輸出: 4
```

**說明：** 腐爛從左上角的橘子開始，向四個方向擴散。每分鐘傳播一層，共需 4 分鐘讓所有橘子腐爛。

### 範例 2

```
輸入: grid = [[2,1,1],[0,1,1],[1,0,1]]
輸出: -1
```

**說明：** 左下角 `(2, 0)` 的橘子無法被任何腐爛路徑觸及，因此永遠不會腐爛。

### 範例 3

```
輸入: grid = [[0,2]]
輸出: 0
```

**說明：** 第 0 分鐘時已經沒有新鮮橘子，答案為 0。

## 限制條件

- `m == grid.length`
- `n == grid[i].length`
- `1 <= m, n <= 10`
- `grid[i][j]` 只會是 `0`、`1` 或 `2`。
