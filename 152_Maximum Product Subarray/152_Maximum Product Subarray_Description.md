# 152. Maximum Product Subarray

**Difficulty:** Medium

## Problem Statement

Given an integer array `nums`, find a subarray that has the largest product, and return the product.

The test cases are generated so that the answer will fit in a **32-bit** integer.

**Note** that the product of an array with a single element is the value of that element.

## Examples

### Example 1

```
Input: nums = [2,3,-2,4]
Output: 6
Explanation: [2,3] has the largest product 6.
```

### Example 2

```
Input: nums = [-2,0,-1]
Output: 0
Explanation: The result cannot be 2, because [-2,-1] is not a subarray.
```

## Constraints

- `1 <= nums.length <= 2 * 10⁴`
- `-10 <= nums[i] <= 10`
- The product of any subarray of `nums` is **guaranteed** to fit in a **32-bit** integer.

---

# 152. 最大乘積子陣列

**難度：** 中等

## 題目描述

給定一個整數陣列 `nums`，找出乘積最大的連續子陣列（至少包含一個數字），並回傳該乘積。

測試案例保證答案可以放入 **32 位元** 整數。

**注意**：只包含單一元素的陣列，其乘積就是該元素本身的值。

## 範例

### 範例 1

```
輸入: nums = [2,3,-2,4]
輸出: 6
說明: [2,3] 的乘積為 6，是最大的子陣列乘積。
```

### 範例 2

```
輸入: nums = [-2,0,-1]
輸出: 0
說明: 結果不能為 2，因為 [-2,-1] 不是一個連續子陣列。
```

## 限制條件

- `1 <= nums.length <= 2 * 10⁴`
- `-10 <= nums[i] <= 10`
- `nums` 任何子陣列的乘積保證可以放入 **32 位元** 整數。
