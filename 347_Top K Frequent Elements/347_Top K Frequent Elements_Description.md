# 347. Top K Frequent Elements Description

This note is a high-fidelity rewrite of the live LeetCode problem page, arranged as bilingual study material for this repo.

---

## English Version

### Problem Statement

You are given an integer array `nums` and an integer `k`. Your task is to return the `k` values that occur most frequently in the array. The returned values may appear in any order.

### Example 1

Input: `nums = [1,1,1,2,2,3]`, `k = 2`

Output: `[1,2]`

Explanation: value `1` appears three times, value `2` appears twice, and value `3` appears once. Therefore, the two most frequent values are `1` and `2`.

### Example 2

Input: `nums = [1]`, `k = 1`

Output: `[1]`

Explanation: there is only one distinct value in the array, so that value must be the answer.

### Example 3

Input: `nums = [1,2,1,2,1,2,3,1,3,2]`, `k = 2`

Output: `[1,2]`

Explanation: both `1` and `2` appear four times, while `3` appears only twice, so the top two frequent values are `1` and `2`.

### Constraints

- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`
- `k` is always at least `1` and at most the number of distinct elements in `nums`
- the answer is guaranteed to be unique

### Follow-up

The problem asks for a solution whose time complexity is better than `O(n log n)`. This requirement is the main reason why simple full sorting is not considered the best final approach.

---

## 中文翻譯

### 題目敘述

給定一個整數陣列 `nums` 與一個整數 `k`，請找出陣列中出現次數最高的 `k` 個元素。答案的回傳順序不限。

### 範例 1

輸入：`nums = [1,1,1,2,2,3]`, `k = 2`

輸出：`[1,2]`

說明：數字 `1` 出現三次，數字 `2` 出現兩次，數字 `3` 出現一次，所以前兩個最高頻元素是 `1` 和 `2`。

### 範例 2

輸入：`nums = [1]`, `k = 1`

輸出：`[1]`

說明：陣列中只有一個不同的數字，因此答案只能是它本身。

### 範例 3

輸入：`nums = [1,2,1,2,1,2,3,1,3,2]`, `k = 2`

輸出：`[1,2]`

說明：`1` 和 `2` 都出現四次，而 `3` 只出現兩次，因此前兩個高頻元素是 `1` 和 `2`。

### 限制條件

- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`
- `k` 的範圍至少是 `1`，且不會超過陣列中不同元素的總數
- 題目保證答案唯一

### 進階要求

題目特別要求演算法時間複雜度必須優於 `O(n log n)`。這也是為什麼直接對所有候選元素完整排序，通常不會被視為最理想解法。
