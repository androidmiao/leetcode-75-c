# LeetCode 1071: Greatest Common Divisor of Strings — Editorial

## Overview

We are looking for the Greatest Common Divisor of two strings, which we call the **GCD string**. A string `base` divides another string `s` if `s` can be formed by concatenating `base` one or more times. We want the longest such `base` that divides both `str1` and `str2`.

## Approach 1: Brute Force

### Intuition

We check every possible prefix string of the shorter string (from longest to shortest) until we find one that divides both input strings.

Key observations:

- The GCD string must be a prefix of both `str1` and `str2`.
- We start with the longest possible candidate (`base = shorter string`) and shorten by removing the last character each time.
- To verify if `base` is a divisor of a string `s`, we check: (1) `len(s)` must be divisible by `len(base)`, and (2) repeating `base` exactly `len(s) / len(base)` times must produce `s`.

### Algorithm

1. Find the shorter string among `str1` and `str2`. Without loss of generality, let it be `str1`.
2. Start with `base = str1`, and check if both `str1` and `str2` are made of multiples of `base`.
   - If so, return `base`.
   - Otherwise, try a shorter string by removing the last character from `base`.
3. If we have checked all prefix strings without finding the GCD string, return `""`.

### Complexity Analysis

Let m, n be the lengths of the two input strings `str1` and `str2`.

- **Time Complexity:** O(min(m,n) · (m+n)) — We check every prefix string `base` of the shorter string (up to min(m,n) candidates), and each check involves iterating over both strings to verify divisibility, costing O(m+n).
- **Space Complexity:** O(min(m,n)) — We keep a copy of `base` in each iteration.

## Approach 2: GCD of Lengths (Mathematical)

### Intuition

This is a more mathematical and elegant approach. The key insight has two parts:

**1. Existence check via concatenation:** If there exists any divisible string at all, then both strings are made of multiples of the same base segment. This means `str1 + str2` must equal `str2 + str1`. If concatenating in both orders produces different results, no divisible string exists.

**2. The GCD string length equals gcd(m, n):** If divisible strings exist, the GCD string `gcdBase` has length equal to `gcd(len(str1), len(str2))`. This can be proven by contradiction — no shorter divisible string can exist without `gcdBase` also being divisible, and no longer divisible string can exist because its length would need to be a common divisor of m and n, contradicting the definition of GCD.

### Algorithm

1. Check if the concatenations of `str1` and `str2` in different orders are the same.
   - If not, return `""`.
2. Get the GCD `gcdLength` of the two lengths of `str1` and `str2`.
3. Return the prefix string with a length of `gcdLength` of either `str1` or `str2` as the answer.

### Complexity Analysis

Let m, n be the lengths of the two input strings `str1` and `str2`.

- **Time Complexity:** O(m+n) — We compare two concatenations of length O(m+n), and calculate the GCD using the Euclidean algorithm in O(log(m·n)) time. Overall O(m+n).
- **Space Complexity:** O(m+n) — We need to compare the two concatenations of length O(m+n).

---

# LeetCode 1071：字串的最大公因數 — 編輯評析

## 概覽

我們要尋找兩個字串的最大公因數字串（GCD string）。若字串 `base` 能整除另一個字串 `s`，代表 `s` 可以由 `base` 串接一次或多次所組成。我們的目標是找到最長的 `base`，使其同時整除 `str1` 和 `str2`。

## 方法 1：暴力法

### 直覺

從最長的候選前綴字串開始，逐步縮短，直到找到一個能同時整除兩個輸入字串的前綴。

關鍵觀察：

- GCD 字串必定是 `str1` 和 `str2` 的共同前綴。
- 從最長的候選（`base = 較短字串`）開始，每次移除最後一個字元來縮短。
- 驗證 `base` 是否能整除字串 `s`：(1) `len(s)` 必須能被 `len(base)` 整除，(2) 將 `base` 重複 `len(s) / len(base)` 次必須恰好等於 `s`。

### 演算法

1. 找到 `str1` 和 `str2` 中較短的字串，不失一般性設為 `str1`。
2. 從 `base = str1` 開始，檢查 `str1` 和 `str2` 是否都由 `base` 的倍數組成。
   - 若是，回傳 `base`。
   - 否則，移除 `base` 的最後一個字元，嘗試更短的字串。
3. 若所有前綴字串都檢查完仍未找到 GCD 字串，回傳 `""`。

### 複雜度分析

設 m、n 為兩個輸入字串 `str1` 和 `str2` 的長度。

- **時間複雜度：** O(min(m,n) · (m+n)) — 檢查較短字串的每個前綴（最多 min(m,n) 個候選），每次檢查需遍歷兩個字串驗證整除性，花費 O(m+n)。
- **空間複雜度：** O(min(m,n)) — 每次迭代需保留一份 `base` 的副本。

## 方法 2：長度的 GCD（數學方法）

### 直覺

這是一個更加數學化且優雅的方法。核心洞察分為兩部分：

**1. 透過串接檢驗存在性：** 若存在任何可整除的字串，則兩個字串都由相同的基礎片段的倍數組成。這表示 `str1 + str2` 必須等於 `str2 + str1`。若兩種順序的串接結果不同，則不存在可整除的字串。

**2. GCD 字串的長度等於 gcd(m, n)：** 若可整除的字串存在，GCD 字串 `gcdBase` 的長度等於 `gcd(len(str1), len(str2))`。這可透過反證法證明 — 任何更短的可整除字串存在時，`gcdBase` 也必然可整除；而任何更長的可整除字串的長度必須是 m 和 n 的公因數，與 GCD 的定義矛盾。

### 演算法

1. 檢查 `str1` 和 `str2` 以不同順序串接的結果是否相同。
   - 若不同，回傳 `""`。
2. 計算 `str1` 和 `str2` 長度的 GCD `gcdLength`。
3. 回傳 `str1` 或 `str2` 長度為 `gcdLength` 的前綴字串作為答案。

### 複雜度分析

設 m、n 為兩個輸入字串 `str1` 和 `str2` 的長度。

- **時間複雜度：** O(m+n) — 比較兩個長度為 O(m+n) 的串接結果，並使用歐幾里得演算法計算 GCD，花費 O(log(m·n))。總計 O(m+n)。
- **空間複雜度：** O(m+n) — 需要比較兩個長度為 O(m+n) 的串接結果。
