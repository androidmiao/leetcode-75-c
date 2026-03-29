# 739. Daily Temperatures

## English

### Problem Statement

Given an array of integers `temperatures` representing the daily temperatures, return an array `answer` such that `answer[i]` is the number of days you have to wait after the i-th day to get a warmer temperature. If there is no future day for which this is possible, keep `answer[i] == 0` instead.

### Examples

#### Example 1
**Input:** `temperatures = [73,74,75,71,69,72,76,73]`

**Output:** `[1,1,4,2,1,1,0,0]`

**Explanation:**
- Day 0 (73°F): The next warmer temperature is 74°F on day 1, so wait 1 day.
- Day 1 (74°F): The next warmer temperature is 75°F on day 2, so wait 1 day.
- Day 2 (75°F): The next warmer temperature is 76°F on day 6, so wait 4 days.
- Day 3 (71°F): The next warmer temperature is 72°F on day 5, so wait 2 days.
- Day 4 (69°F): The next warmer temperature is 72°F on day 5, so wait 1 day.
- Day 5 (72°F): The next warmer temperature is 76°F on day 6, so wait 1 day.
- Day 6 (76°F): There is no future day with a warmer temperature, so return 0.
- Day 7 (73°F): There is no future day with a warmer temperature, so return 0.

#### Example 2
**Input:** `temperatures = [30,40,50,60]`

**Output:** `[1,1,1,0]`

**Explanation:**
- Each temperature except the last is strictly less than the next temperature, so we wait 1 day for each.
- The last temperature has no future warmer day.

#### Example 3
**Input:** `temperatures = [30,60,90]`

**Output:** `[1,1,0]`

**Explanation:**
- 30 < 60 (wait 1 day), 60 < 90 (wait 1 day), 90 has no future warmer temperature.

### Constraints

- `1 <= temperatures.length <= 10^5`
- `30 <= temperatures[i] <= 100`

---

## 繁體中文

### 問題敘述

給定一個整數陣列 `temperatures` 代表每日氣溫，回傳一個陣列 `answer`，其中 `answer[i]` 是你在第 i 天之後需要等待多少天才能迎接一個更高的溫度。如果後續沒有日子能達到更高溫度，則保持 `answer[i] == 0`。

### 範例

#### 範例 1
**輸入:** `temperatures = [73,74,75,71,69,72,76,73]`

**輸出:** `[1,1,4,2,1,1,0,0]`

**解釋:**
- 第 0 天 (73°F)：下一個更高溫度是第 1 天的 74°F，所以等待 1 天。
- 第 1 天 (74°F)：下一個更高溫度是第 2 天的 75°F，所以等待 1 天。
- 第 2 天 (75°F)：下一個更高溫度是第 6 天的 76°F，所以等待 4 天。
- 第 3 天 (71°F)：下一個更高溫度是第 5 天的 72°F，所以等待 2 天。
- 第 4 天 (69°F)：下一個更高溫度是第 5 天的 72°F，所以等待 1 天。
- 第 5 天 (72°F)：下一個更高溫度是第 6 天的 76°F，所以等待 1 天。
- 第 6 天 (76°F)：後續沒有日子的溫度更高，回傳 0。
- 第 7 天 (73°F)：後續沒有日子的溫度更高，回傳 0。

#### 範例 2
**輸入:** `temperatures = [30,40,50,60]`

**輸出:** `[1,1,1,0]`

**解釋:**
- 除了最後一個溫度外，每個溫度都嚴格小於下一個溫度，所以每個都需等待 1 天。
- 最後一個溫度後續沒有更高的溫度。

#### 範例 3
**輸入:** `temperatures = [30,60,90]`

**輸出:** `[1,1,0]`

**解釋:**
- 30 < 60（等待 1 天）、60 < 90（等待 1 天）、90 後續沒有更高的溫度。

### 約束條件

- `1 <= temperatures.length <= 10^5`
- `30 <= temperatures[i] <= 100`
