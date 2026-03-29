# LeetCode 739: Daily Temperatures Editorial

## English Version

### Overview

We are given a list of temperatures where the temperature at index i is the temperature of the i-th day. Our goal is to find for each day the number of days until the next warmer day. The brute-force approach iterates for each day through all remaining days to find a warmer temperature — O(N²) time, which is too slow given N < 10^5.

Key insight: If you have multiple days in a row with decreasing temperatures and then one very hot day (e.g., [40, 39, 38, 37, 36, 35, 34, 65]), the final day is the "answer" day for all the others. We can "delay" finding the answer and then move backward to find the answer for all at once. This matches the behavior of a stack.

### Approach 1: Monotonic Stack

#### Intuition

Use a Monotonic Stack — a stack where elements are always in sorted order. We use a monotonic decreasing stack to hold temperatures. Since the problem asks for the number of days, we store indices instead of temperatures themselves, and use `temperatures[i]` to find the temperature.

On each day, two possibilities:
1. If the current temperature is NOT warmer than the stack top → push the current day onto the stack (maintains sorted property)
2. If the current temperature IS warmer than the stack top → this is the first warmer day for all cooler days on top. Pop from the stack, set `answer[prevDay] = currDay - prevDay`. Keep popping while the stack top is still cooler.

#### Algorithm

1. Initialize array `answer` with the same length as `temperatures`, with all values set to 0. Initialize a stack as empty.
2. Iterate through `temperatures`. At each index `currDay`:
   - While the stack is not empty and the current temperature is warmer than the temperature of `prevDay` (the index at the top of the stack):
     - Set `answer[prevDay] = currDay - prevDay`
     - Pop from the stack
   - Push the current index `currDay` onto the stack.
3. Return `answer`.

#### Complexity Analysis

- **Time Complexity:** O(n) — each index is pushed and popped from the stack at most once
- **Space Complexity:** O(n) — for the stack and the answer array

### Approach 2: Array, Optimized Space

#### Intuition

Instead of using a separate stack, iterate backwards through the array and use the `answer` array itself to "jump" forward. For each day, look at the next day. If it's not warmer, use `answer[next]` to skip ahead to the next warmer day from that position. This leverages already-computed answers to avoid redundant comparisons.

Additionally, track the `hottest` temperature seen so far (from the right). If the current day's temperature is greater than or equal to `hottest`, we know there's no warmer day in the future, so we can skip the search entirely.

#### Algorithm

1. Initialize array `answer` with the same length as `temperatures`, all values set to 0. Initialize `hottest = 0`.
2. Iterate backwards through `temperatures`. At each index `currDay`:
   - If `temperatures[currDay] >= hottest`, update `hottest` and continue (answer stays 0).
   - Otherwise, initialize `days = 1`. While `temperatures[currDay + days] <= temperatures[currDay]`:
     - Add `answer[currDay + days]` to `days` (jump forward using the already-computed answer).
   - Set `answer[currDay] = days`.
3. Return `answer`.

#### Complexity Analysis

- **Time Complexity:** O(n) — the "jumps" prevent any index from being visited more than twice across all iterations
- **Space Complexity:** O(1) — only constant extra space beyond the output array (the `answer` array is required by the output format and does not count)

---

## 繁體中文版本

### 概述

我們得到一個溫度列表，其中索引 i 處的溫度是第 i 天的溫度。我們的目標是找到每一天距離下一個更溫暖日子的天數。暴力破解的方法是對每一天遍歷所有剩餘的日子以找到更溫暖的溫度 — 時間複雜度為 O(N²)，在 N < 10^5 的情況下太慢了。

關鍵洞察：如果連續多天的溫度遞減，然後出現一個非常炎熱的日子（例如 [40, 39, 38, 37, 36, 35, 34, 65]），最後一天是所有其他日子的「答案」日。我們可以「延遲」尋找答案，然後向後移動以一次性找到所有答案。這符合棧的行為特性。

### 方法 1：單調棧

#### 直觀理解

使用單調棧 — 一個元素始終保持排序順序的棧。我們使用單調遞減棧來存儲溫度。由於問題要求的是天數，我們存儲索引而不是溫度本身，並使用 `temperatures[i]` 來找到溫度。

在每一天，有兩種可能性：
1. 如果當前溫度不比棧頂溫度更高 → 將當前日期推入棧（保持排序特性）
2. 如果當前溫度比棧頂溫度更高 → 這是棧頂所有較冷日期的第一個更溫暖的日子。從棧中彈出，設置 `answer[prevDay] = currDay - prevDay`。繼續彈出，直到棧頂仍然較冷。

#### 算法

1. 初始化數組 `answer`，長度與 `temperatures` 相同，所有值都設為 0。將棧初始化為空。
2. 遍歷 `temperatures`。在每個索引 `currDay` 處：
   - 當棧不為空且當前溫度高於 `prevDay`（棧頂的索引）的溫度時：
     - 設置 `answer[prevDay] = currDay - prevDay`
     - 從棧中彈出
   - 將當前索引 `currDay` 推入棧。
3. 返回 `answer`。

#### 複雜度分析

- **時間複雜度：** O(n) — 每個索引最多被推入和彈出棧一次
- **空間複雜度：** O(n) — 用於棧和答案數組

### 方法 2：陣列、空間優化

#### 直觀理解

不使用額外的棧，改為從後往前遍歷陣列，並利用 `answer` 陣列本身來「跳躍」前進。對於每一天，先看下一天。如果下一天的溫度不夠高，就利用 `answer[next]` 跳過已知不會更暖的日子。這樣利用已計算的答案來避免重複比較。

另外，追蹤從右側看過的最高溫度 `hottest`。如果當前天的溫度大於等於 `hottest`，就知道未來不會有更暖的天，可以直接跳過搜索。

#### 算法

1. 初始化數組 `answer`，長度與 `temperatures` 相同，所有值設為 0。初始化 `hottest = 0`。
2. 從後往前遍歷 `temperatures`。在每個索引 `currDay` 處：
   - 如果 `temperatures[currDay] >= hottest`，更新 `hottest` 並繼續（answer 保持 0）。
   - 否則，初始化 `days = 1`。當 `temperatures[currDay + days] <= temperatures[currDay]` 時：
     - 將 `answer[currDay + days]` 加到 `days`（利用已計算的答案向前跳躍）。
   - 設置 `answer[currDay] = days`。
3. 返回 `answer`。

#### 複雜度分析

- **時間複雜度：** O(n) — 「跳躍」機制確保每個索引在所有迭代中不會被訪問超過兩次
- **空間複雜度：** O(1) — 除了輸出陣列外只使用常數額外空間（`answer` 陣列是輸出格式要求，不計入空間複雜度）
