# 84. Largest Rectangle in Histogram — Editorial

## Overview

We need to find the rectangle of largest area that can be formed by using the given bars of a histogram, where each bar has width 1.

---

## Approach 1: Brute Force

For every possible pair of bars (i, j), find the minimum height bar lying between them and compute the area as min_height × (j − i + 1). Track the maximum area across all pairs.

**Algorithm:**
For each starting bar i, iterate through all ending bars j ≥ i. For each pair, scan from i to j tracking the minimum height, then compute the area.

**Complexity:**
- Time: O(n³) — triple nested loops (start, end, find-min)
- Space: O(1) — constant extra space

---

## Approach 2: Better Brute Force

Optimize Approach 1 by maintaining a running minimum height as we extend the right boundary. For each starting index i, iterate j from i to the end, keeping track of min_height incrementally. This eliminates the innermost loop.

**Algorithm:**
For each bar i, set min_height = heights[i]. Extend j rightward, update min_height = min(min_height, heights[j]), and compute area = min_height × (j − i + 1) at each step.

**Complexity:**
- Time: O(n²) — every pair is considered
- Space: O(1) — no extra space

---

## Approach 3: Divide and Conquer

The rectangle with maximum area will be one of three cases:
1. The widest possible rectangle with height equal to the shortest bar in the range.
2. The largest rectangle confined to the left of the shortest bar (subproblem).
3. The largest rectangle confined to the right of the shortest bar (subproblem).

Find the minimum height bar, compute the area spanning the full range at that height, then recursively solve the left and right subproblems.

**Complexity:**
- Time: O(n log n) average; O(n²) worst case (sorted array — minimum is always at one end)
- Space: O(n) — recursion stack depth

---

## Approach 4: Better Divide and Conquer (Segment Tree)

The worst case of Approach 3 arises from O(n) scans to find the minimum. Replace the linear scan with a Segment Tree for range-minimum queries in O(log n), reducing the overall worst case.

**Complexity:**
- Time: O(n log n) — segment tree query log n performed n times
- Space: O(n) — segment tree storage

---

## Approach 5: Using Stack (Optimal)

Maintain a stack of bar indices in increasing order of height. When we encounter a bar shorter than the stack top, we pop and compute the area that the popped bar can form as the shortest bar. The width extends from the current stack top + 1 to the current index − 1.

**Key insight:** Each bar is pushed and popped at most once, so the total work is O(n).

**Algorithm:**
1. Push a sentinel index −1 onto the stack (or use −1 as a boundary marker).
2. For each index i from 0 to n−1:
   - While the stack top's height ≥ heights[i], pop the top. The popped bar's height is the rectangle height. The width is i − stack_top − 1 (after the pop).
   - Update max_area if the new area is larger.
   - Push i onto the stack.
3. After the loop, pop remaining elements using n as the right boundary.

The repo's `.c` implementation uses an equivalent sentinel technique: prepend and append a 0-height bar to avoid special-casing the cleanup loop.

**Complexity:**
- Time: O(n) — each element pushed and popped at most once
- Space: O(n) — stack storage

---

---

# 84. 柱狀圖中最大的矩形 — 題解

## 概述

我們需要在柱狀圖中找出由柱子所能構成的最大矩形面積，每根柱子寬度為 1。

---

## 方法一：暴力法

對每一對柱子 (i, j)，找出它們之間最矮的柱子高度，以 min_height × (j − i + 1) 計算面積，追蹤所有配對中的最大面積。

**演算法：**
對每個起始柱子 i，遍歷所有結束柱子 j ≥ i。對每一對，掃描 i 到 j 追蹤最小高度，然後計算面積。

**複雜度：**
- 時間：O(n³) — 三重巢狀迴圈（起點、終點、找最小值）
- 空間：O(1) — 常數額外空間

---

## 方法二：改良暴力法

優化方法一：在擴展右邊界時維護一個遞增更新的最小高度。對每個起始索引 i，將 min_height 設為 heights[i]，向右延伸 j，逐步更新 min_height = min(min_height, heights[j])，每步計算 area = min_height × (j − i + 1)。

**複雜度：**
- 時間：O(n²) — 考慮所有配對
- 空間：O(1) — 無額外空間

---

## 方法三：分治法

最大面積矩形必為以下三種情況之一：
1. 以該範圍內最矮柱子高度為高、橫跨整個範圍的矩形。
2. 最矮柱子左側子問題中的最大矩形。
3. 最矮柱子右側子問題中的最大矩形。

找到最小高度柱子後，計算以該高度橫跨全範圍的面積，再遞迴求解左右子問題。

**複雜度：**
- 時間：O(n log n) 平均；O(n²) 最壞情況（已排序陣列 — 最小值總在一端）
- 空間：O(n) — 遞迴堆疊深度

---

## 方法四：改良分治法（線段樹）

方法三的最壞情況來自 O(n) 的線性掃描尋找最小值。以線段樹 (Segment Tree) 替代，實現 O(log n) 的區間最小值查詢，降低整體最壞情況複雜度。

**複雜度：**
- 時間：O(n log n) — 線段樹查詢 log n 執行 n 次
- 空間：O(n) — 線段樹儲存空間

---

## 方法五：使用堆疊（最佳解）

維護一個索引遞增（按高度）的堆疊。遇到高度低於堆疊頂端的柱子時，彈出頂端並計算以該柱子為最矮柱子所能形成的矩形面積。寬度從堆疊新頂端 + 1 延伸到當前索引 − 1。

**核心洞察：** 每根柱子最多被推入和彈出各一次，因此總工作量為 O(n)。

**演算法：**
1. 將哨兵索引 −1 推入堆疊（或用 −1 作為邊界標記）。
2. 對每個索引 i（從 0 到 n−1）：
   - 當堆疊頂端高度 ≥ heights[i] 時，彈出頂端。彈出柱子的高度即為矩形高度。寬度為 i − 堆疊新頂端 − 1。
   - 若新面積較大則更新 max_area。
   - 將 i 推入堆疊。
3. 迴圈結束後，以 n 為右邊界，彈出堆疊中剩餘元素。

本 repo 的 `.c` 實作使用等效的哨兵技巧：在首尾各添加一個高度為 0 的柱子，避免清理迴圈的特殊處理。

**複雜度：**
- 時間：O(n) — 每個元素最多推入和彈出各一次
- 空間：O(n) — 堆疊儲存空間
