# 1431. Kids With the Greatest Number of Candies

## Solution

### Overview
We are given an integer array `candies`, where each `candies[i]` represents the number of candies the i-th kid has, and an integer `extraCandies`, denoting the number of extra candies. Our task is to return a boolean array `result` of length n, where `result[i]` is true if, after giving the i-th kid all the `extraCandies`, they will have the greatest number of candies among all the kids, or `false` otherwise.

### Approach: Ad Hoc

#### Intuition
We precompute the greatest number of candies that any kid(s) has, call it `maxCandies`. Following the precomputation, we iterate over `candies`, checking whether the total candies that the current kid has exceeds `maxCandies` after giving `extraCandies` to the kid. For every kid, we perform `candies[i] + extraCandies >= maxCandies` and push it into a boolean list called `result`. In the end, we return `result`.

The visual representation: for candies = [2,3,5,1,3], extraCandies = 3:
- First iteration: find maxCandies = 5
- Second iteration: check each kid:
  - index 0: 2+3>=5 → true
  - index 1: 3+3>=5 → true
  - index 2: 5+3>=5 → true
  - index 3: 1+3<5 → false
  - index 4: 3+3>=5 → true

#### Algorithm
1. Create an integer variable called `maxCandies` to store the greatest number of candies in `candies`. Initialize it with 0.
2. Iterate over `candies` and for each kid who has `candy` candies, perform `maxCandies = max(maxCandies, candy)` to get the greatest number.
3. Create a boolean list `answer`.
4. Iterate over `candies` once more, and for each kid who has `candy` candies, add `candy + extraCandies >= maxCandies` to `answer`.
5. Return `answer`.

#### Complexity Analysis
Here, n is the number of kids.

- Time complexity: O(n) — iterate over `candies` array to find `maxCandies` (O(n)), then iterate once more to check each kid (O(n)). Total: O(n).
- Space complexity: O(1) — without counting the space of input and output, only a few integer variables like `maxCandies` and `candy` are used.

---

# 1431. 擁有最多糖果的孩子們

## 解法

### 概述
我們收到一個整數陣列 `candies`，其中 `candies[i]` 代表第 i 個孩子擁有的糖果數量，以及一個整數 `extraCandies`，表示額外糖果的數量。我們的任務是返回一個長度為 n 的布林陣列 `result`，其中如果將所有 `extraCandies` 分給第 i 個孩子後，該孩子會擁有所有孩子中最多的糖果，則 `result[i]` 為 true，否則為 false。

### 方法：臨時法（Ad Hoc）

#### 直覺
我們預先計算任何孩子擁有的最多糖果數量，稱之為 `maxCandies`。在預先計算之後，我們遍歷 `candies`，檢查在給予該孩子 `extraCandies` 後，該孩子擁有的總糖果數是否超過 `maxCandies`。對每個孩子，我們執行 `candies[i] + extraCandies >= maxCandies` 並將結果推入一個名為 `result` 的布林列表中。最後，我們返回 `result`。

視覺化表示：對於 candies = [2,3,5,1,3]，extraCandies = 3：
- 第一次迭代：找出 maxCandies = 5
- 第二次迭代：檢查每個孩子：
  - 索引 0：2+3>=5 → true
  - 索引 1：3+3>=5 → true
  - 索引 2：5+3>=5 → true
  - 索引 3：1+3<5 → false
  - 索引 4：3+3>=5 → true

#### 演算法
1. 建立一個名為 `maxCandies` 的整數變數，用於儲存 `candies` 中的最多糖果數。將其初始化為 0。
2. 遍歷 `candies`，對於每個擁有 `candy` 個糖果的孩子，執行 `maxCandies = max(maxCandies, candy)` 以取得最大數量。
3. 建立一個布林列表 `answer`。
4. 再次遍歷 `candies`，對於每個擁有 `candy` 個糖果的孩子，將 `candy + extraCandies >= maxCandies` 的結果新增至 `answer`。
5. 返回 `answer`。

#### 複雜度分析
此處 n 是孩子的數量。

- 時間複雜度：O(n) — 遍歷 `candies` 陣列以找出 `maxCandies`（O(n)），然後再遍歷一次以檢查每個孩子（O(n)）。總計：O(n)。
- 空間複雜度：O(1) — 不計輸入和輸出的空間，僅使用少數整數變數，如 `maxCandies` 和 `candy`。
