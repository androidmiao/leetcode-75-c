# LeetCode 11: Container With Most Water — Editorial

## Overview

The problem asks us to find two vertical lines that form a container capable of holding the maximum amount of water. The water capacity is determined by the formula: **area = min(height[left], height[right]) × (right - left)**, where the minimum of the two heights represents the water level (limited by the shorter line) and the distance between the indices represents the width of the container.

## Approach 1: Brute Force

### Algorithm

The brute force strategy involves examining all possible pairs of lines and calculating the water area for each combination. We iterate through every position as a left boundary, then for each left boundary, iterate through all positions to the right as potential right boundaries. We compute the area for each pair and maintain a running maximum throughout the process.

### Complexity Analysis

- **Time Complexity:** O(n²) — We examine all n(n-1)/2 possible pairs of lines, which reduces to quadratic time complexity.
- **Space Complexity:** O(1) — We only use a constant amount of extra space for storing the maximum area and temporary variables.

**Note:** While this approach is straightforward and helps build intuition, it will exceed time limits in real interview or competitive programming settings due to its quadratic nature.

## Approach 2: Two Pointer Approach

### Algorithm

We employ a two-pointer technique to solve this efficiently:

1. Initialize two pointers: one at the leftmost index (left = 0) and one at the rightmost index (right = n-1).
2. Create a variable `maxArea` to track the maximum water area found so far.
3. In each iteration:
   - Calculate the current container area using the two pointers: area = min(height[left], height[right]) × (right - left)
   - Update `maxArea` if the current area is larger
   - Move the pointer that points to the shorter line one step inward
4. Continue until the two pointers meet.

### Why It Works

The key insight is understanding that the area is always constrained by the shorter of the two lines. When we move a pointer inward:

- **Moving the longer line's pointer:** This cannot improve the solution because:
  - The height is still limited by the shorter line (which remains unchanged)
  - The width decreases by moving inward
  - Therefore, the area can only stay the same or decrease

- **Moving the shorter line's pointer:** This offers potential improvement because:
  - While the width decreases, we have a chance to encounter a taller line
  - If we find a line taller than the previous shorter line, the area might increase despite reduced width
  - Even if the new line is shorter, we're moving toward the optimal solution systematically

This greedy strategy ensures we don't miss the maximum area while eliminating the need to check all pairs.

### Complexity Analysis

- **Time Complexity:** O(n) — We traverse the array once with two pointers, moving a total of n steps.
- **Space Complexity:** O(1) — We only maintain a constant number of variables regardless of input size.

---

# LeetCode 11：盛水最多的容器 — 編輯評析

## 概覽

本問題要求我們找到兩條豎直線，使其形成的容器能夠盛載最多的水。水的容量由以下公式決定：**面積 = min(高度[左], 高度[右]) × (右 - 左)**，其中兩條線的最小高度決定了水位（由較短的線限制），而兩個索引之間的距離則代表容器的寬度。

## 方法 1：暴力法

### 演算法

暴力法的策略是檢查所有可能的線對，並計算每對組合的盛水面積。我們遍歷每個位置作為左邊界，然後對於每個左邊界，再遍歷其右側的所有位置作為潛在的右邊界。我們計算每一對的面積，並在整個過程中維護一個最大面積值。

### 複雜度分析

- **時間複雜度：** O(n²) — 我們檢查所有 n(n-1)/2 種可能的線對組合，這簡化為二次方時間複雜度。
- **空間複雜度：** O(1) — 我們只使用常數級的額外空間來儲存最大面積和臨時變數。

**注意：** 雖然這個方法直觀易懂，有助於建立基本認識，但由於其二次方的性質，在實際面試或競賽中會超時。

## 方法 2：雙指標法

### 演算法

我們採用雙指標技巧來高效解決這個問題：

1. 初始化兩個指標：一個位於最左側（left = 0），一個位於最右側（right = n-1）。
2. 建立一個變數 `maxArea` 來追蹤迄今為止發現的最大盛水面積。
3. 在每次迭代中：
   - 使用兩個指標計算當前容器的面積：面積 = min(高度[left], 高度[right]) × (right - left)
   - 如果當前面積大於 `maxArea`，則更新 `maxArea`
   - 將指向較短線的指標向內移動一步
4. 重複此過程直到兩個指標相遇。

### 為什麼有效

關鍵的洞察在於理解面積總是受到兩條線中較短一條的限制。當我們移動指標時：

- **移動較長線的指標：** 這無法改進方案，因為：
  - 面積仍然受到較短線（保持不變）的限制
  - 向內移動會導致寬度減少
  - 因此面積只會保持不變或減少

- **移動較短線的指標：** 這提供了改進的可能性，因為：
  - 雖然寬度減少，但我們有機會遇到更高的線
  - 若找到高於之前較短線的新線，面積可能增加（儘管寬度縮小）
  - 即使新線更短，我們也在系統地朝向最優解移動

這種貪心策略確保我們不會錯過最大面積，同時消除了檢查所有對的必要性。

### 複雜度分析

- **時間複雜度：** O(n) — 我們用雙指標遍歷一次陣列，總共移動 n 步。
- **空間複雜度：** O(1) — 無論輸入大小如何，我們只維護常數個變數。
