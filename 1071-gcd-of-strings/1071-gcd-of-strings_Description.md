# 1071. Greatest Common Divisor of Strings

**Difficulty: Easy**

## Problem Description (English)

For two strings `s` and `t`, we say "`t` divides `s`" if and only if `s = t + t + t + ... + t + t` (i.e., `t` is concatenated with itself one or more times).

Given two strings `str1` and `str2`, return the largest string `x` such that `x` divides both `str1` and `str2`.

### Example 1

**Input:** `str1 = "ABCABC"`, `str2 = "ABC"`

**Output:** `"ABC"`

### Example 2

**Input:** `str1 = "ABABAB"`, `str2 = "ABAB"`

**Output:** `"AB"`

### Example 3

**Input:** `str1 = "LEET"`, `str2 = "CODE"`

**Output:** `""`

### Example 4

**Input:** `str1 = "AAAAAB"`, `str2 = "AAA"`

**Output:** `""`

### Constraints

- `1 <= str1.length, str2.length <= 1000`
- `str1` and `str2` consist of English uppercase letters.

---

# 1071. 字串的最大公因數

**難度：簡單**

## 問題描述（繁體中文）

對於兩個字串 `s` 和 `t`，當且僅當 `s = t + t + t + ... + t + t`（即 `t` 與自身串接一次或多次）時，我們稱「`t` 整除 `s`」。

給定兩個字串 `str1` 和 `str2`，返回最長的字串 `x`，使得 `x` 同時整除 `str1` 和 `str2`。

### 範例 1

**輸入：** `str1 = "ABCABC"`, `str2 = "ABC"`

**輸出：** `"ABC"`

### 範例 2

**輸入：** `str1 = "ABABAB"`, `str2 = "ABAB"`

**輸出：** `"AB"`

### 範例 3

**輸入：** `str1 = "LEET"`, `str2 = "CODE"`

**輸出：** `""`

### 範例 4

**輸入：** `str1 = "AAAAAB"`, `str2 = "AAA"`

**輸出：** `""`

### 限制條件

- `1 <= str1.length, str2.length <= 1000`
- `str1` 和 `str2` 僅由英文大寫字母組成。
