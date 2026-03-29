# 200. Number of Islands

## Problem

Given an `m x n` 2D binary grid `grid` which represents a map of `'1'`s (land) and `'0'`s (water), return *the number of islands*.

An **island** is surrounded by water and is formed by connecting adjacent lands horizontally or vertically. You may assume all four edges of the grid are all surrounded by water.

## Examples

### Example 1

```
Input: grid = [
  ["1","1","1","1","0"],
  ["1","1","0","1","0"],
  ["1","1","0","0","0"],
  ["0","0","0","0","0"]
]
Output: 1
```

### Example 2

```
Input: grid = [
  ["1","1","0","0","0"],
  ["1","1","0","0","0"],
  ["0","0","1","0","0"],
  ["0","0","0","1","1"]
]
Output: 3
```

## Constraints

- `m == grid.length`
- `n == grid[i].length`
- `1 <= m, n <= 300`
- `grid[i][j]` is `'0'` or `'1'`.

---

# 200. 島嶼數量

## 題目描述

給定一個 `m x n` 的二維二進位網格 `grid`，其中 `'1'` 代表陸地、`'0'` 代表水域，請回傳**島嶼的數量**。

**島嶼**被水域包圍，由水平或垂直方向上相鄰的陸地連接而成。可以假設網格的四條邊全部被水域包圍。

## 範例

### 範例 1

```
輸入：grid = [
  ["1","1","1","1","0"],
  ["1","1","0","1","0"],
  ["1","1","0","0","0"],
  ["0","0","0","0","0"]
]
輸出：1
```

說明：左上方的陸地全部相連，構成一個島嶼。

### 範例 2

```
輸入：grid = [
  ["1","1","0","0","0"],
  ["1","1","0","0","0"],
  ["0","0","1","0","0"],
  ["0","0","0","1","1"]
]
輸出：3
```

說明：三塊不相連的陸地分別形成三個島嶼。

## 限制條件

- `m == grid.length`
- `n == grid[i].length`
- `1 <= m, n <= 300`
- `grid[i][j]` 為 `'0'` 或 `'1'`。
