# 84. Largest Rectangle in Histogram

**Difficulty:** Hard
**Topics:** Array, Stack, Monotonic Stack

## Problem Statement

Given an array of integers `heights` representing the histogram's bar height where the width of each bar is `1`, return the area of the largest rectangle in the histogram.

## Examples

### Example 1

```
Input: heights = [2,1,5,6,2,3]
Output: 10
Explanation: The above is a histogram where width of each bar is 1.
The largest rectangle is shown in the red area, which has an area = 10 units.
```

The largest rectangle spans bars with heights 5 and 6, using the minimum height of 5 across a width of 2: 5 × 2 = 10.

### Example 2

```
Input: heights = [2,4]
Output: 4
```

The largest rectangle is the single bar of height 4 (area = 4), or alternatively height 2 × width 2 = 4.

## Constraints

- `1 <= heights.length <= 10^5`
- `0 <= heights[i] <= 10^4`

---

# 84. 柱狀圖中最大的矩形

**難度：** 困難
**主題：** 陣列、堆疊、單調堆疊

## 題目描述

給定一個整數陣列 `heights`，代表柱狀圖中每根柱子的高度，每根柱子的寬度為 `1`，請回傳柱狀圖中能構成的最大矩形面積。

## 範例

### 範例 1

```
輸入：heights = [2,1,5,6,2,3]
輸出：10
說明：柱狀圖中每根柱子的寬度為 1。
最大矩形以紅色區域表示，其面積 = 10 個單位。
```

最大矩形橫跨高度為 5 和 6 的柱子，取最小高度 5、寬度 2：5 × 2 = 10。

### 範例 2

```
輸入：heights = [2,4]
輸出：4
```

最大矩形為單根高度 4 的柱子（面積 = 4），或者高度 2 × 寬度 2 = 4。

## 限制條件

- `1 <= heights.length <= 10^5`
- `0 <= heights[i] <= 10^4`
