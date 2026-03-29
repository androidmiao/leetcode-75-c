# 11. Container With Most Water

**Difficulty: Medium**

## Problem Description (English)

You are given an integer array `height` of length `n`. There are `n` vertical lines drawn such that the two endpoints of the `i`th line are `(i, 0)` and `(i, height[i])`.

Find two lines that together with the x-axis form a container, such that the container contains the most water.

Return *the maximum amount of water a container can store*.

**Notice:** You may not slant the container.

### Example 1

**Input:** `height = [1,8,6,2,5,4,8,3,7]`

**Output:** `49`

**Explanation:** The vertical lines are represented by array `[1,8,6,2,5,4,8,3,7]`. In this case, the maximum area of water (the blue section) that the container can hold is 49.

### Example 2

**Input:** `height = [1,1]`

**Output:** `1`

### Constraints

- `n == height.length`
- `2 <= n <= 10^5`
- `0 <= height[i] <= 10^4`

---

# 11. 盛最多水的容器

**難度：中等**

## 問題描述（繁體中文）

給定一個長度為 `n` 的整數陣列 `height`。有 `n` 條豎直線，其中第 `i` 條線的兩個端點分別為 `(i, 0)` 和 `(i, height[i])`。

請找到兩條線，使得這兩條線與 x 軸圍成一個容器，且容器所能盛放的水量最多。

返回*容器可以盛放的最大水量*。

**注意：** 容器不可以傾斜。

### 範例 1

**輸入：** `height = [1,8,6,2,5,4,8,3,7]`

**輸出：** `49`

**說明：** 豎直線由陣列 `[1,8,6,2,5,4,8,3,7]` 表示。在此情況下，容器可以盛放的最大水量（藍色區域部分）為 49。

### 範例 2

**輸入：** `height = [1,1]`

**輸出：** `1`

### 限制條件

- `n == height.length`
- `2 <= n <= 10^5`
- `0 <= height[i] <= 10^4`
