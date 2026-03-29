# 162. Find Peak Element

**Difficulty:** Medium
**Topics:** Array, Binary Search
**Link:** https://leetcode.com/problems/find-peak-element/

---

## Problem Statement

A peak element is an element that is **strictly greater** than its neighbors.

Given a 0-indexed integer array `nums`, find a peak element, and return its index. If the array contains multiple peaks, return the index to **any of the peaks**.

You may imagine that `nums[-1] = nums[n] = -∞`. In other words, an element is always considered to be strictly greater than a neighbor that is outside the array.

You must write an algorithm that runs in **O(log n)** time.

---

## Examples

### Example 1
```
Input:  nums = [1,2,3,1]
Output: 2
Explanation: 3 is a peak element and your function should return the index number 2.
```

### Example 2
```
Input:  nums = [1,2,1,3,5,6,4]
Output: 5
Explanation: Your function can return either index number 1 where the peak element is 2,
             or index number 5 where the peak element is 6.
```

---

## Constraints

- `1 <= nums.length <= 1000`
- `-2^31 <= nums[i] <= 2^31 - 1`
- `nums[i] != nums[i + 1]` for all valid `i`

---

## 中文翻譯（繁體中文）

### 題目敘述

**峰值元素**是指**嚴格大於**其相鄰元素的元素。

給定一個以 0 為索引的整數陣列 `nums`，請找出一個峰值元素並回傳其索引。若陣列包含多個峰值，回傳**任何一個峰值**的索引即可。

你可以假設 `nums[-1] = nums[n] = -∞`。換言之，一個元素永遠被視為嚴格大於陣列範圍之外的鄰居。

你必須撰寫一個時間複雜度為 **O(log n)** 的演算法。

### 範例

**範例 1**
```
輸入：nums = [1,2,3,1]
輸出：2
解釋：3 是峰值元素，函式應回傳索引 2。
```

**範例 2**
```
輸入：nums = [1,2,1,3,5,6,4]
輸出：5
解釋：函式可回傳索引 1（峰值元素為 2）或索引 5（峰值元素為 6）。
```

### 限制條件

- `1 <= nums.length <= 1000`
- `-2^31 <= nums[i] <= 2^31 - 1`
- 對所有合法的 `i`，`nums[i] != nums[i + 1]`
