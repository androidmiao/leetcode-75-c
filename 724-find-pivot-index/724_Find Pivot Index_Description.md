# 724. Find Pivot Index

**Difficulty:** Easy

## Problem Statement

Given an array of integers `nums`, calculate the **pivot index** of this array.

The **pivot index** is the index where the sum of all the numbers **strictly** to the left of the index is equal to the sum of all the numbers **strictly** to the index's right.

If the index is on the left edge of the array, then the left sum is `0` because there are no elements to the left. This also applies to the right edge of the array.

Return the **leftmost pivot index**. If no such index exists, return `-1`.

## Examples

**Example 1:**
- Input: nums = [1,7,3,6,5,6]
- Output: 3
- Explanation: The pivot index is 3. Left sum = nums[0] + nums[1] + nums[2] = 1 + 7 + 3 = 11. Right sum = nums[4] + nums[5] = 5 + 6 = 11.

**Example 2:**
- Input: nums = [1,2,3]
- Output: -1
- Explanation: There is no index that satisfies the conditions in the problem statement.

**Example 3:**
- Input: nums = [2,1,-1]
- Output: 0
- Explanation: The pivot index is 0. Left sum = 0 (no elements to the left of index 0). Right sum = nums[1] + nums[2] = 1 + -1 = 0.

## Constraints

- 1 <= nums.length <= 10^4
- -1000 <= nums[i] <= 1000

## Note

This question is the same as 1991: https://leetcode.com/problems/find-the-middle-index-in-array/

---

# 724. 尋找樞紐索引

**難度:** 簡單

## 問題陳述

給定一個整數陣列 `nums`，計算該陣列的**樞紐索引**。

**樞紐索引**是指索引左側所有數字的總和**嚴格**等於索引右側所有數字總和的索引。

如果索引位於陣列的左邊緣，則左側和為 `0`，因為左側沒有元素。這也適用於陣列的右邊緣。

返回**最左邊的樞紐索引**。如果不存在這樣的索引，則返回 `-1`。

## 範例

**範例 1:**
- 輸入: nums = [1,7,3,6,5,6]
- 輸出: 3
- 說明: 樞紐索引為 3。左側和 = nums[0] + nums[1] + nums[2] = 1 + 7 + 3 = 11。右側和 = nums[4] + nums[5] = 5 + 6 = 11。

**範例 2:**
- 輸入: nums = [1,2,3]
- 輸出: -1
- 說明: 沒有任何索引滿足問題陳述中的條件。

**範例 3:**
- 輸入: nums = [2,1,-1]
- 輸出: 0
- 說明: 樞紐索引為 0。左側和 = 0（索引 0 的左側沒有元素）。右側和 = nums[1] + nums[2] = 1 + -1 = 0。

## 約束條件

- 1 <= nums.length <= 10^4
- -1000 <= nums[i] <= 1000

## 附註

此問題與 1991 相同: https://leetcode.com/problems/find-the-middle-index-in-array/
