# 152. Maximum Product Subarray — Editorial

> Source: LeetCode Official Editorial (live authenticated session, accessed 2026-03-29)

It is advisable to approach the Maximum Subarray (Problem 53) problem first, as the intuition from that problem helps significantly here.

---

## Approach 1: Brute Force (Python TLE)

### Intuition

The naive approach is to enumerate every contiguous subarray starting from each element and compute their products. By accumulating the product as we extend the subarray (rather than recomputing from scratch), we can reduce the cubic brute force to O(N²).

This brute force approach is included to build the foundation for understanding the DP approach.

### Algorithm

For each starting index `i`, maintain a running product `accu` and extend to every ending index `j ≥ i`. Track the global maximum across all subarrays.

### Complexity

- **Time:** O(N²) — two nested loops over `nums`.
- **Space:** O(1) — only `result` and `accu` are used.

---

## Approach 2: Dynamic Programming

### Intuition

Instead of examining every subarray, we scan the array once and maintain two running values: `max_so_far` (the maximum product ending at the current position) and `min_so_far` (the minimum product ending at the current position).

Two things can disrupt a growing product chain:

1. **Zeros** — reset the chain entirely. The accumulated high score is safe in `result`; after a zero we restart fresh.
2. **Negative numbers** — a single negative number flips the largest positive product into a very small (most-negative) product. However, if a second negative number appears later, it can flip that most-negative value back into a large positive. This is why we track `min_so_far`.

### Update Rules

`max_so_far` is updated to the **maximum** of three candidates:

1. **Current number** — start a new subarray here (previous chain was too bad).
2. **max_so_far × current** — extend the positive chain.
3. **min_so_far × current** — a negative × negative recovery.

`min_so_far` is updated to the **minimum** of the same three candidates.

Because the new `min_so_far` depends on the old `max_so_far`, we must save `max_so_far` in a temporary variable before overwriting it.

### Algorithm

```
max_so_far = nums[0]
min_so_far = nums[0]
result     = nums[0]

for i = 1 to n-1:
    curr = nums[i]
    temp_max = max(curr, max_so_far * curr, min_so_far * curr)
    min_so_far = min(curr, max_so_far * curr, min_so_far * curr)
    max_so_far = temp_max
    result = max(result, max_so_far)

return result
```

### Complexity

- **Time:** O(N) — single pass through `nums`.
- **Space:** O(1) — only a fixed number of variables.

---

# 152. 最大乘積子陣列 — 題解

> 來源：LeetCode 官方題解（透過已驗證的瀏覽器 session 存取，2026-03-29）

建議先解 Maximum Subarray（第 53 題），從中獲得的直覺對本題很有幫助。

---

## 方法一：暴力法（Python 會 TLE）

### 直覺

最樸素的做法是列舉所有以每個元素為起點的連續子陣列，並計算其乘積。透過在擴展子陣列時累積乘積（而非從頭重算），可以將三次方暴力降為 O(N²)。

此暴力法的目的僅在於建立理解動態規劃方法的基礎。

### 演算法

對每個起始索引 `i`，維護一個累積乘積 `accu`，向右擴展到所有結束索引 `j ≥ i`。在所有子陣列中追蹤全域最大值。

### 複雜度

- **時間：** O(N²) — 兩層巢狀迴圈遍歷 `nums`。
- **空間：** O(1) — 只使用 `result` 和 `accu`。

---

## 方法二：動態規劃

### 直覺

不再列舉所有子陣列，而是一次掃描陣列，同時維護兩個滾動值：`max_so_far`（以當前位置結尾的最大乘積）和 `min_so_far`（以當前位置結尾的最小乘積）。

兩種情況會打斷成長中的乘積鏈：

1. **零** — 完全重置乘積鏈。已累積的高分安全地保存在 `result` 中；遇到零後從頭開始。
2. **負數** — 一個負數會把最大的正乘積翻轉成極小的負乘積。但如果之後再遇到另一個負數，又能把那個極小的負值翻轉回大正值。這就是我們追蹤 `min_so_far` 的原因。

### 更新規則

`max_so_far` 取以下三個候選值的**最大值**：

1. **當前數字** — 在此處開始新子陣列（前面的鏈太差了）。
2. **max_so_far × 當前數字** — 延續正數鏈。
3. **min_so_far × 當前數字** — 負 × 負的翻轉恢復。

`min_so_far` 取同樣三個候選值的**最小值**。

因為新的 `min_so_far` 依賴舊的 `max_so_far`，我們必須在覆蓋之前先將 `max_so_far` 保存到暫存變數。

### 演算法

```
max_so_far = nums[0]
min_so_far = nums[0]
result     = nums[0]

for i = 1 to n-1:
    curr = nums[i]
    temp_max = max(curr, max_so_far * curr, min_so_far * curr)
    min_so_far = min(curr, max_so_far * curr, min_so_far * curr)
    max_so_far = temp_max
    result = max(result, max_so_far)

return result
```

### 複雜度

- **時間：** O(N) — 單次遍歷 `nums`。
- **空間：** O(1) — 只使用固定數量的變數。
