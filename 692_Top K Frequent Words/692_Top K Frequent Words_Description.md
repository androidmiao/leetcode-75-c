# 692. Top K Frequent Words Description

This note is a high-fidelity rewrite of the live LeetCode problem page, arranged as bilingual study material for this repo.

---

## English Version

### Problem Statement

You are given an array of strings `words` and an integer `k`. Return the `k` most frequent strings.

The result must be ordered from higher frequency to lower frequency. If two strings have the same frequency, the lexicographically smaller string must come first.

### Example 1

Input: `words = ["i","love","leetcode","i","love","coding"]`, `k = 2`

Output: `["i","love"]`

Explanation:

- `"i"` appears 2 times
- `"love"` appears 2 times
- `"coding"` appears 1 time
- `"leetcode"` appears 1 time

Because `"i"` and `"love"` have the same frequency, lexicographical order is used, so `"i"` comes before `"love"`.

### Example 2

Input: `words = ["the","day","is","sunny","the","the","the","sunny","is","is"]`, `k = 4`

Output: `["the","is","sunny","day"]`

Explanation:

- `"the"` appears 4 times
- `"is"` appears 3 times
- `"sunny"` appears 2 times
- `"day"` appears 1 time

So the top 4 most frequent strings are returned in the required order.

### Constraints

- `1 <= words.length <= 500`
- `1 <= words[i].length <= 10`
- `words[i]` consists of lowercase English letters
- `k` is in the range `[1, number of unique words]`

### Follow-up

Try to solve it in `O(n log k)` time and `O(n)` extra space.

---

## 中文翻譯

### 題目敘述

給定一個字串陣列 `words` 和一個整數 `k`，請回傳出現次數最高的前 `k` 個單字。

答案必須依照頻率由高到低排列。如果兩個單字出現次數相同，則字典序較小的單字必須排在前面。

### 範例 1

輸入：`words = ["i","love","leetcode","i","love","coding"]`, `k = 2`

輸出：`["i","love"]`

說明：

- `"i"` 出現 2 次
- `"love"` 出現 2 次
- `"coding"` 出現 1 次
- `"leetcode"` 出現 1 次

因為 `"i"` 和 `"love"` 的出現次數相同，所以要再依字典序排序，`"i"` 會排在 `"love"` 前面。

### 範例 2

輸入：`words = ["the","day","is","sunny","the","the","the","sunny","is","is"]`, `k = 4`

輸出：`["the","is","sunny","day"]`

說明：

- `"the"` 出現 4 次
- `"is"` 出現 3 次
- `"sunny"` 出現 2 次
- `"day"` 出現 1 次

因此前 4 個最高頻單字依序為題目要求的結果。

### 限制條件

- `1 <= words.length <= 500`
- `1 <= words[i].length <= 10`
- `words[i]` 只包含小寫英文字母
- `k` 的範圍是 `[1, 不同單字的總數]`

### 進階要求

請嘗試在 `O(n log k)` 時間與 `O(n)` 額外空間內完成。
