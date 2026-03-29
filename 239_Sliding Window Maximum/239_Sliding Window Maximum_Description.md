# 239. Sliding Window Maximum

## Problem Statement

You are given an array of integers `nums`, there is a sliding window of size `k` which is moving from the very left of the array to the very right. You can only see the `k` numbers in the window. Each time the sliding window moves right by one position.

Return the max sliding window.

### Example 1

**Input:** `nums = [1,3,-1,-3,5,3,6,7], k = 3`

**Output:** `[3,3,5,5,6,7]`

**Explanation:**

| Window Position | Max |
|---|---|
| `[1  3  -1] -3  5  3  6  7` | 3 |
| `1 [3  -1  -3] 5  3  6  7` | 3 |
| `1  3 [-1  -3  5] 3  6  7` | 5 |
| `1  3  -1 [-3  5  3] 6  7` | 5 |
| `1  3  -1  -3 [5  3  6] 7` | 6 |
| `1  3  -1  -3  5 [3  6  7]` | 7 |

### Example 2

**Input:** `nums = [1], k = 1`

**Output:** `[1]`

### Constraints

- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`
- `1 <= k <= nums.length`

---

# 239. 滑動視窗最大值

## 問題陳述

給定一個整數數組 `nums`，有一個大小為 `k` 的滑動視窗從數組最左邊移動到最右邊。在任何時刻，你只能看到視窗內的 `k` 個數字。每次滑動視窗向右移動一個位置。

返回每個視窗位置對應的最大值。

### 示例 1

**輸入：** `nums = [1,3,-1,-3,5,3,6,7], k = 3`

**輸出：** `[3,3,5,5,6,7]`

**說明：**

| 視窗位置 | 最大值 |
|---|---|
| `[1  3  -1] -3  5  3  6  7` | 3 |
| `1 [3  -1  -3] 5  3  6  7` | 3 |
| `1  3 [-1  -3  5] 3  6  7` | 5 |
| `1  3  -1 [-3  5  3] 6  7` | 5 |
| `1  3  -1  -3 [5  3  6] 7` | 6 |
| `1  3  -1  -3  5 [3  6  7]` | 7 |

### 示例 2

**輸入：** `nums = [1], k = 1`

**輸出：** `[1]`

### 限制條件

- `1 <= nums.length <= 10^5`
- `-10^4 <= nums[i] <= 10^4`
- `1 <= k <= nums.length`
