# 53. Maximum Subarray

## English

### Problem Statement

Given an integer array `nums`, find the subarray with the largest sum, and return its sum.

### Examples

#### Example 1
- **Input:** `nums = [-2,1,-3,4,-1,2,1,-5,4]`
- **Output:** `6`
- **Explanation:** The subarray `[4,-1,2,1]` has the largest sum 6.

#### Example 2
- **Input:** `nums = [1]`
- **Output:** `1`
- **Explanation:** The subarray `[1]` has the largest sum 1.

#### Example 3
- **Input:** `nums = [5,4,-1,7,8]`
- **Output:** `23`
- **Explanation:** The subarray `[5,4,-1,7,8]` has the largest sum 23.

### Constraints
- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`

### Follow Up
If you have figured out the O(n) solution, try coding another solution using the divide and conquer approach, which is more subtle.

---

## 繁體中文 (Traditional Chinese)

### 問題陳述

給定一個整數數組 `nums`，找到具有最大和的子數組，並返回其和。

### 例子

#### 例子 1
- **輸入：** `nums = [-2,1,-3,4,-1,2,1,-5,4]`
- **輸出：** `6`
- **解釋：** 子數組 `[4,-1,2,1]` 的最大和為 6。

#### 例子 2
- **輸入：** `nums = [1]`
- **輸出：** `1`
- **解釋：** 子數組 `[1]` 的最大和為 1。

#### 例子 3
- **輸入：** `nums = [5,4,-1,7,8]`
- **輸出：** `23`
- **解釋：** 子數組 `[5,4,-1,7,8]` 的最大和為 23。

### 約束條件
- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`

### 進階
如果您已經想出了 O(n) 的解決方案，請嘗試使用分治法編寫另一個解決方案，這更加微妙。
