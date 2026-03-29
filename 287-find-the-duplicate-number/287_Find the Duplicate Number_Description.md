# 287. Find the Duplicate Number

## Difficulty
Medium

---

## English

### Problem Statement

Given an array of integers `nums` containing `n + 1` integers where each integer is in the range `[1, n]` inclusive. There is only **one repeated number** in `nums`, return this repeated number.

You must solve the problem **without** modifying the array `nums` and using only constant extra space.

### Examples

**Example 1:**
```
Input: nums = [1,3,4,2,2]
Output: 2
```

**Example 2:**
```
Input: nums = [3,1,3,4,2]
Output: 3
```

**Example 3:**
```
Input: nums = [3,3,3,3,3]
Output: 3
```

### Constraints

- `1 <= n <= 10^5`
- `nums.length == n + 1`
- `1 <= nums[i] <= n`
- All the integers in nums appear only once except for precisely one integer which appears two or more times.

### Follow Up

- How can we prove that at least one duplicate number must exist in nums?
- Can you solve the problem in linear runtime complexity?

---

## 繁體中文

### 問題陳述

給定一個整數數組 `nums`，包含 `n + 1` 個整數，其中每個整數都在範圍 `[1, n]` 內（含端點）。`nums` 中只有**一個重複的數字**，請返回這個重複的數字。

你必須在**不修改數組** `nums` 的情況下解決此問題，並且只能使用常數級別的額外空間。

### 示例

**示例 1：**
```
輸入：nums = [1,3,4,2,2]
輸出：2
```

**示例 2：**
```
輸入：nums = [3,1,3,4,2]
輸出：3
```

**示例 3：**
```
輸入：nums = [3,3,3,3,3]
輸出：3
```

### 約束條件

- `1 <= n <= 10^5`
- `nums.length == n + 1`
- `1 <= nums[i] <= n`
- `nums` 中的所有整數只出現一次，除了恰好有一個整數出現兩次或多次。

### 後續問題

- 我們如何證明 `nums` 中必須至少存在一個重複的數字？
- 你能否在線性時間複雜度內解決此問題？

