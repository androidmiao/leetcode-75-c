# 33. Search in Rotated Sorted Array — Editorial

Source: Live authenticated LeetCode session (Editorial tab).

---

## Overview

Define the **pivot index** as the index of the smallest element in `nums`. The rotated array consists of two sorted subarrays: `nums[0 ~ pivot-1]` and `nums[pivot ~ n-1]`.

```
nums = [4, 5, 6, 7, 0, 1, 2]
                   ↑
                 pivot (index 4, value 0)
```

---

## Approach 1: Find Pivot Index + Binary Search

**Algorithm:**
1. Use binary search to locate the pivot (smallest element):
   - If `nums[mid] > nums[n-1]`, pivot is to the right of mid → `left = mid + 1`
   - Otherwise, pivot could be at mid or to the left → `right = mid - 1`
   - Pivot = `left` when the loop ends.
2. Now `nums` consists of two sorted subarrays: `[0, pivot-1]` and `[pivot, n-1]`.
3. Perform standard binary search on the appropriate subarray.

**Complexity:**
- Time: O(log n)
- Space: O(1)

---

## Approach 2: One-Pass Binary Search (Recommended)

**Intuition:** At any midpoint, one of the two halves `[l, mid]` or `[mid, r]` must be sorted (even in a rotated array). Use this to determine which half to search.

**Algorithm:**
1. Initialize `l = 0`, `r = n - 1`.
2. While `l <= r`:
   - Compute `mid = l + (r - l) / 2`.
   - If `nums[mid] == target`: return `mid`.
   - **Left half `[l, mid]` is sorted** if `nums[l] <= nums[mid]`:
     - If `nums[l] <= target < nums[mid]`: target is in left half → `r = mid - 1`
     - Else: target is in right half → `l = mid + 1`
   - **Right half `[mid, r]` is sorted** otherwise:
     - If `nums[mid] < target <= nums[r]`: target is in right half → `l = mid + 1`
     - Else: target is in left half → `r = mid - 1`
3. Return `-1` if not found.

**Complexity:**
- Time: O(log n)
- Space: O(1)

---

---

## 中文翻譯

### 概述

定義 **pivot 索引** 為陣列中最小元素的位置。旋轉後的陣列由兩個有序子陣列組成：`nums[0 ~ pivot-1]` 和 `nums[pivot ~ n-1]`。

### 方法一：找到 Pivot + 二分搜尋

**演算法：**
1. 用二分搜尋找到 pivot（最小元素的位置）
2. 對兩個有序子陣列之一做二分搜尋

**複雜度：** 時間 O(log n)，空間 O(1)

---

### 方法二：一次二分搜尋（推薦）

**直覺：** 在任意 mid 點，左半段 `[l, mid]` 或右半段 `[mid, r]` 必有一個是連續有序的，利用這個性質一次掃描。

**判斷規則：**
- 若 `nums[l] <= nums[mid]`：左半段有序
  - 若 `nums[l] <= target < nums[mid]`：往左搜 → `r = mid - 1`
  - 否則：往右搜 → `l = mid + 1`
- 否則：右半段有序
  - 若 `nums[mid] < target <= nums[r]`：往右搜 → `l = mid + 1`
  - 否則：往左搜 → `r = mid - 1`

**複雜度：** 時間 O(log n)，空間 O(1)
