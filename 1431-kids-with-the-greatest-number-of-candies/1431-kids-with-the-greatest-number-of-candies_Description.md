# 1431. Kids With the Greatest Number of Candies

**Difficulty:** Easy
**URL:** https://leetcode.com/problems/kids-with-the-greatest-number-of-candies/

## Description

There are n kids with candies. You are given an integer array `candies`, where each `candies[i]` represents the number of candies the i-th kid has, and an integer `extraCandies`, denoting the number of extra candies that you have.

Return a boolean array `result` of length n, where `result[i]` is true if, after giving the i-th kid all the `extraCandies`, they will have the greatest number of candies among all the kids, or false otherwise.

Note that multiple kids can have the greatest number of candies.

### Example 1

```
Input: candies = [2,3,5,1,3], extraCandies = 3
Output: [true,true,true,false,true]
Explanation: If you give all extraCandies to:
- Kid 1, they will have 2 + 3 = 5 candies, which is the greatest among the kids.
- Kid 2, they will have 3 + 3 = 6 candies, which is the greatest among the kids.
- Kid 3, they will have 5 + 3 = 8 candies, which is the greatest among the kids.
- Kid 4, they will have 1 + 3 = 4 candies, which is not the greatest among the kids.
- Kid 5, they will have 3 + 3 = 6 candies, which is the greatest among the kids.
```

### Example 2

```
Input: candies = [4,2,1,1,2], extraCandies = 1
Output: [true,false,false,false,false]
Explanation: There is only 1 extra candy. Kid 1 will always have the greatest number of candies, even if a different kid is given the extra candy.
```

### Example 3

```
Input: candies = [12,1,12], extraCandies = 10
Output: [true,false,true]
```

### Constraints

- n == candies.length
- 2 <= n <= 100
- 1 <= candies[i] <= 100
- 1 <= extraCandies <= 50

---

# 1431. 孩子們能獲得的最大糖果數

**難度:** 簡單
**URL:** https://leetcode.com/problems/kids-with-the-greatest-number-of-candies/

## 描述

有 n 個孩子擁有糖果。你會獲得一個整數陣列 `candies`，其中 `candies[i]` 表示第 i 個孩子擁有的糖果數，還有一個整數 `extraCandies`，表示你擁有的額外糖果數。

返回一個長度為 n 的布林陣列 `result`，其中 `result[i]` 為 true，當且僅當在將所有 `extraCandies` 分給第 i 個孩子後，該孩子將擁有所有孩子中最多的糖果數；否則為 false。

注意，多個孩子可能擁有相同的最多糖果數。

### 範例 1

```
輸入: candies = [2,3,5,1,3], extraCandies = 3
輸出: [true,true,true,false,true]
說明: 如果你將所有額外糖果分給：
- 孩子 1，他將擁有 2 + 3 = 5 個糖果，這是所有孩子中最多的。
- 孩子 2，他將擁有 3 + 3 = 6 個糖果，這是所有孩子中最多的。
- 孩子 3，他將擁有 5 + 3 = 8 個糖果，這是所有孩子中最多的。
- 孩子 4，他將擁有 1 + 3 = 4 個糖果，這不是所有孩子中最多的。
- 孩子 5，他將擁有 3 + 3 = 6 個糖果，這是所有孩子中最多的。
```

### 範例 2

```
輸入: candies = [4,2,1,1,2], extraCandies = 1
輸出: [true,false,false,false,false]
說明: 只有 1 個額外糖果。孩子 1 將始終擁有最多的糖果，即使其他孩子獲得額外糖果。
```

### 範例 3

```
輸入: candies = [12,1,12], extraCandies = 10
輸出: [true,false,true]
```

### 約束條件

- n == candies.length
- 2 <= n <= 100
- 1 <= candies[i] <= 100
- 1 <= extraCandies <= 50
