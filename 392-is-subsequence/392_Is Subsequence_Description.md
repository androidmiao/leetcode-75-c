# LeetCode Problem 392: Is Subsequence

## English Section

### Problem Statement

Given two strings `s` and `t`, determine whether `s` is a subsequence of `t`. Return `true` if `s` is indeed a subsequence, otherwise return `false`.

A subsequence is formed by deleting some (or none) of the characters from the original string while maintaining the relative order of the remaining characters. For instance, "ace" represents a valid subsequence of "abcde" because the characters appear in order, while "aec" does not qualify as a subsequence of "abcde" since the relative order is broken.

### Examples

#### Example 1
- **Input:** `s = "abc"`, `t = "ahbgdc"`
- **Output:** `true`
- **Explanation:** The characters 'a', 'b', and 'c' appear in "ahbgdc" in the correct order (at indices 0, 2, and 5 respectively), forming a valid subsequence.

#### Example 2
- **Input:** `s = "axc"`, `t = "ahbgdc"`
- **Output:** `false`
- **Explanation:** While 'a' and 'c' are present in "ahbgdc", the character 'x' does not exist in "ahbgdc", making it impossible to form the subsequence "axc".

### Constraints

- `0 <= s.length <= 100`: The source string `s` has a maximum length of 100 characters.
- `0 <= t.length <= 10^4`: The target string `t` has a maximum length of 10,000 characters.
- Both `s` and `t` consist exclusively of lowercase English letters (a-z).

### Problem Topics

- Two Pointers
- String Manipulation
- Dynamic Programming

### Follow-up Challenge

Consider a scenario where you receive multiple incoming strings: `s1`, `s2`, ..., `sk` where `k >= 10^9` (billions of queries). You need to check each one individually to determine if string `t` contains it as a subsequence. How would you optimize your approach for this scenario where the same target string `t` is checked against many different query strings?

---

## 繁體中文版本

### 問題陳述

給定兩個字串 `s` 和 `t`，判斷 `s` 是否為 `t` 的子序列。如果 `s` 確實是 `t` 的子序列，則傳回 `true`，否則傳回 `false`。

子序列是通過從原始字串中刪除某些（或不刪除）字符而形成的新字串，同時保持剩餘字符的相對順序。例如，"ace" 是 "abcde" 的有效子序列，因為這些字符按順序出現；而 "aec" 不是 "abcde" 的子序列，因為相對順序被打亂了。

### 範例

#### 範例 1
- **輸入：** `s = "abc"`，`t = "ahbgdc"`
- **輸出：** `true`
- **解釋：** 字符 'a'、'b' 和 'c' 在 "ahbgdc" 中以正確的順序出現（分別位於索引 0、2 和 5），形成了一個有效的子序列。

#### 範例 2
- **輸入：** `s = "axc"`，`t = "ahbgdc"`
- **輸出：** `false`
- **解釋：** 雖然 'a' 和 'c' 在 "ahbgdc" 中存在，但字符 'x' 不在 "ahbgdc" 中，因此無法形成子序列 "axc"。

### 約束條件

- `0 <= s.length <= 100`：來源字串 `s` 的最大長度為 100 個字符。
- `0 <= t.length <= 10^4`：目標字串 `t` 的最大長度為 10,000 個字符。
- `s` 和 `t` 都只包含小寫英文字母（a-z）。

### 問題主題

- 雙指針技術
- 字串操作
- 動態規劃

### 進階問題

考慮這樣一種場景：你會陸續收到多個輸入字串：`s1`、`s2`、...、`sk`，其中 `k >= 10^9`（十億級別的查詢）。你需要逐一檢查每個字串，以確定字串 `t` 中是否包含它作為子序列。在這種情況下，當同一個目標字串 `t` 需要對比多個不同的查詢字串時，你會如何優化你的方法？

