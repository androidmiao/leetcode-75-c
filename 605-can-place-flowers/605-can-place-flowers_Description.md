# 605. Can Place Flowers

**Difficulty:** Easy
**Topics:** Array, Greedy
**Companies:** LinkedIn, Amazon, Google, Meta

## Problem Statement

You have a long flowerbed in which some of the plots are planted, and some are not. However, flowers cannot be planted in **adjacent** plots.

Given an integer array `flowerbed` containing `0`'s and `1`'s, where `0` means empty and `1` means not empty, and an integer `n`, return `true` if `n` new flowers can be planted in the `flowerbed` without violating the no-adjacent-flowers rule and `false` otherwise.

### Example 1

```
Input: flowerbed = [1,0,0,0,1], n = 1
Output: true
```

### Example 2

```
Input: flowerbed = [1,0,0,0,1], n = 2
Output: false
```

### Constraints

- `1 <= flowerbed.length <= 2 * 10^4`
- `flowerbed[i]` is `0` or `1`.
- There are no two adjacent flowers in `flowerbed`.
- `0 <= n <= flowerbed.length`

---

## 題目描述（繁體中文）

你有一個很長的花圃，其中有些位置已經種了花，有些則沒有。然而，花不能種在**相鄰**的位置。

給定一個整數陣列 `flowerbed`，其中包含 `0` 和 `1`，`0` 代表空位、`1` 代表已種花，以及一個整數 `n`，請判斷是否能在花圃中再種入 `n` 朵花，且不違反「不相鄰」的規則。若可以則回傳 `true`，否則回傳 `false`。

### 範例 1

```
輸入：flowerbed = [1,0,0,0,1], n = 1
輸出：true
說明：位置 2（索引從 0 開始）的左邊（索引 1）和右邊（索引 3）都是空的，所以可以在位置 2 種花。
```

### 範例 2

```
輸入：flowerbed = [1,0,0,0,1], n = 2
輸出：false
說明：花圃 [1,0,0,0,1] 中只有位置 2 可以種花（只能種 1 朵），不足以種 2 朵。
```

### 限制條件

- `1 <= flowerbed.length <= 2 * 10^4`
- `flowerbed[i]` 只會是 `0` 或 `1`。
- 花圃中不會有兩朵相鄰的花（輸入保證合法）。
- `0 <= n <= flowerbed.length`
