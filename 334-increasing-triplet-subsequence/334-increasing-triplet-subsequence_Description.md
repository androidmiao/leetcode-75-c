# Problem 334: Increasing Triplet Subsequence

## Difficulty
Medium

## Description
Given an integer array `nums`, return `true` if there exists a triple of indices `(i, j, k)` such that `i < j < k` and `nums[i] < nums[j] < nums[k]`. If no such indices exists, return `false`.

## Examples

### Example 1
- **Input:** `nums = [1,2,3,4,5]`
- **Output:** `true`
- **Explanation:** Any triplet where i < j < k is valid.

### Example 2
- **Input:** `nums = [5,4,3,2,1]`
- **Output:** `false`
- **Explanation:** No triplet exists.

### Example 3
- **Input:** `nums = [2,1,5,0,4,6]`
- **Output:** `true`
- **Explanation:** One of the valid triplet is (1, 4, 5), because nums[1] == 1 < nums[4] == 4 < nums[5] == 6.

## Constraints
- 1 <= nums.length <= 5 * 10^5
- -2^31 <= nums[i] <= 2^31 - 1

## Follow Up
Could you implement a solution that runs in O(n) time complexity and O(1) space complexity?

---

# 問題 334：遞增三元子序列

## 難度
中等

## 描述
給定一個整數陣列 `nums`，如果存在一個三元組的索引 `(i, j, k)` 使得 `i < j < k` 且 `nums[i] < nums[j] < nums[k]`，則返回 `true`。如果不存在這樣的索引，則返回 `false`。

## 範例

### 範例 1
- **輸入：** `nums = [1,2,3,4,5]`
- **輸出：** `true`
- **解釋：** 任何滿足 i < j < k 的三元組都是有效的。

### 範例 2
- **輸入：** `nums = [5,4,3,2,1]`
- **輸出：** `false`
- **解釋：** 不存在三元組。

### 範例 3
- **輸入：** `nums = [2,1,5,0,4,6]`
- **輸出：** `true`
- **解釋：** 其中一個有效的三元組是 (1, 4, 5)，因為 nums[1] == 1 < nums[4] == 4 < nums[5] == 6。

## 約束條件
- 1 <= nums.length <= 5 * 10^5
- -2^31 <= nums[i] <= 2^31 - 1

## 後續問題
能否實現一個時間複雜度為 O(n) 且空間複雜度為 O(1) 的解決方案？
