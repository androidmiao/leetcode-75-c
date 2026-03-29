# 33. Search in Rotated Sorted Array

**Difficulty:** Medium
**Topics:** Array, Binary Search
**Link:** https://leetcode.com/problems/search-in-rotated-sorted-array/

---

## Problem Statement

There is an integer array `nums` sorted in ascending order (with **distinct** values).

Prior to being passed to your function, `nums` is **possibly rotated** at an unknown pivot index `k` (`1 <= k < nums.length`) such that the resulting array is `[nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]]`.

For example, `[0,1,2,4,5,6,7]` might be rotated at pivot index 3 and become `[4,5,6,7,0,1,2]`.

Given the array `nums` after the possible rotation and an integer `target`, return *the index of* `target` *if it is in* `nums`, *or* `-1` *if it is not in* `nums`.

You must write an algorithm with **O(log n)** runtime complexity.

---

## Examples

### Example 1
```
Input:  nums = [4,5,6,7,0,1,2], target = 0
Output: 4
```

### Example 2
```
Input:  nums = [4,5,6,7,0,1,2], target = 3
Output: -1
```

### Example 3
```
Input:  nums = [1], target = 0
Output: -1
```

---

## Constraints

- `1 <= nums.length <= 5000`
- `-10^4 <= nums[i] <= 10^4`
- All values of `nums` are **unique**.
- `nums` is an ascending array that is possibly rotated.
- `-10^4 <= target <= 10^4`

---

## 繁體中文翻譯

### 題目敘述

給定一個整數陣列 `nums`，原本按照升序排列（元素值皆**不重複**）。

在傳入函式之前，`nums` 可能在某個未知的旋轉索引 `k`（`1 <= k < nums.length`）處被**旋轉**，使得旋轉後的陣列變成 `[nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]]`。

例如，`[0,1,2,4,5,6,7]` 在索引 3 處旋轉後變成 `[4,5,6,7,0,1,2]`。

給定旋轉後的陣列 `nums` 和一個整數 `target`，如果 `target` 存在於 `nums` 中，回傳其索引；否則回傳 `-1`。

你必須設計一個時間複雜度為 **O(log n)** 的演算法。

---

### 範例

**範例 1**
```
輸入：nums = [4,5,6,7,0,1,2], target = 0
輸出：4
```

**範例 2**
```
輸入：nums = [4,5,6,7,0,1,2], target = 3
輸出：-1
```

**範例 3**
```
輸入：nums = [1], target = 0
輸出：-1
```

---

### 限制條件

- `1 <= nums.length <= 5000`
- `-10^4 <= nums[i] <= 10^4`
- `nums` 中所有元素**不重複**
- `nums` 是一個可能被旋轉過的升序陣列
- `-10^4 <= target <= 10^4`
