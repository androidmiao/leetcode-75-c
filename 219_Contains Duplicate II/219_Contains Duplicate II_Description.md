# 219. Contains Duplicate II

## Problem Statement

Given an integer array `nums` and an integer `k`, return `true` if there are two *distinct indices* `i` and `j` in the array such that `nums[i] == nums[j]` and `abs(i - j) <= k`.

## Examples

### Example 1:
```
Input: nums = [1,2,3,1], k = 3
Output: true
```

### Example 2:
```
Input: nums = [1,0,1,1], k = 1
Output: true
```

### Example 3:
```
Input: nums = [1,2,3,1,2,3], k = 2
Output: false
```

## Constraints

- `1 <= nums.length <= 10^5`
- `-10^9 <= nums[i] <= 10^9`
- `0 <= k <= 10^5`

## Follow-up

Could you solve it in O(n) time?

---

# 219. 存在重複元素 II（繁體中文）

## 題目描述

給定一個整數陣列 `nums` 和一個整數 `k`，如果陣列中存在兩個**不同的索引** `i` 和 `j`，使得 `nums[i] == nums[j]` 且 `abs(i - j) <= k`，則回傳 `true`。

## 範例

### 範例 1：
```
輸入：nums = [1,2,3,1], k = 3
輸出：true
說明：nums[0] == nums[3]，且 abs(0 - 3) = 3 <= 3，滿足條件。
```

### 範例 2：
```
輸入：nums = [1,0,1,1], k = 1
輸出：true
說明：nums[2] == nums[3]，且 abs(2 - 3) = 1 <= 1，滿足條件。
```

### 範例 3：
```
輸入：nums = [1,2,3,1,2,3], k = 2
輸出：false
說明：雖然 nums[0] == nums[3]，但 abs(0 - 3) = 3 > 2，不滿足距離條件。
      同理 nums[1] == nums[4]，abs(1 - 4) = 3 > 2。
      nums[2] == nums[5]，abs(2 - 5) = 3 > 2。
      所有重複元素的距離都大於 k = 2。
```

## 限制條件

- `1 <= nums.length <= 10^5`
- `-10^9 <= nums[i] <= 10^9`
- `0 <= k <= 10^5`

## 核心觀察

1. **滑動視窗思維**：只需關注大小為 k 的視窗內是否有重複元素。若視窗內找到重複，距離條件自動滿足。
2. **雜湊表加速查找**：用雜湊表（或集合）記錄視窗內的元素，讓查找操作從 O(k) 降到 O(1)。
3. **視窗維護**：當視窗超過大小 k 時，移除最舊的元素（索引 i-k 處的元素）。

## 進階

你能在 O(n) 時間內解決嗎？
