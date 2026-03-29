# 35. Search Insert Position

**Difficulty:** Easy
**Topics:** Array, Binary Search
**Link:** https://leetcode.com/problems/search-insert-position/

---

## Problem Statement

Given a sorted array of distinct integers and a target value, return the index if the target is found. If not, return the index where it would be inserted in order.

You must write an algorithm with **O(log n)** runtime complexity.

---

## Examples

### Example 1
```
Input:  nums = [1,3,5,6], target = 5
Output: 2
```

### Example 2
```
Input:  nums = [1,3,5,6], target = 2
Output: 1
```

### Example 3
```
Input:  nums = [1,3,5,6], target = 7
Output: 4
```

---

## Constraints

- `1 <= nums.length <= 10^4`
- `-10^4 <= nums[i] <= 10^4`
- `nums` contains **distinct** values sorted in **ascending** order.
- `-10^4 <= target <= 10^4`

---

## 中文翻譯（繁體中文）

### 題目描述

給定一個已排序的**不重複**整數陣列和一個目標值，若目標值存在於陣列中，回傳其索引；若不存在，回傳它應被插入的位置索引，以維持陣列的排序順序。

你必須設計一個時間複雜度為 **O(log n)** 的演算法。

---

### 範例

#### 範例 1
```
輸入：nums = [1,3,5,6], target = 5
輸出：2
```

#### 範例 2
```
輸入：nums = [1,3,5,6], target = 2
輸出：1
```

#### 範例 3
```
輸入：nums = [1,3,5,6], target = 7
輸出：4
```

---

### 限制條件

- `1 <= nums.length <= 10^4`
- `-10^4 <= nums[i] <= 10^4`
- `nums` 包含**不重複**的值，且按**遞增**順序排列。
- `-10^4 <= target <= 10^4`
