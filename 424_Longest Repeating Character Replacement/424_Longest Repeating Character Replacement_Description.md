# 424. Longest Repeating Character Replacement

## Problem Statement

You are given a string `s` and an integer `k`. You can choose any character of the string and change it to any other uppercase English character. You can perform this operation at most `k` times.

Return the length of the longest substring containing the same letter you can get after performing the above operations.

## Examples

### Example 1:
```
Input: s = "ABAB", k = 2
Output: 4
Explanation: Replace the two 'A's with two 'B's or vice versa.
```

### Example 2:
```
Input: s = "AABABBA", k = 1
Output: 4
Explanation: Replace the one 'A' in the middle with 'B' and form "AABBBBA".
The substring "BBBB" has the longest repeating letters, which is 4.
There may exists other ways to achieve this answer too.
```

## Constraints

- `1 <= s.length <= 10^5`
- `s` consists of only uppercase English letters.
- `0 <= k <= s.length`

## Key Insights

1. **Sliding Window Approach**: This is a classic sliding window problem where we maintain a window of characters that can be made the same with at most `k` changes.

2. **Valid Window Condition**: A window is valid if `(window_length - max_frequency) <= k`, meaning the number of characters we need to change is at most `k`.

3. **Two-Pointer Technique**: We expand the window by moving the right pointer and contract it by moving the left pointer when the condition is violated.

4. **Character Frequency**: We track the frequency of each character in the current window using a 26-element array (for uppercase English letters).

5. **Maximum Frequency**: We maintain the maximum frequency of any character in the current window to determine how many changes are needed.

---

# 424. 最長重複字元替換

## 題目描述

給定一個字串 `s` 和一個整數 `k`。你可以選擇字串中的任意字元，將其更改為任何其他大寫英文字母。此操作最多可以執行 `k` 次。

回傳在執行上述操作後，包含相同字母的最長子字串的長度。

## 範例

### 範例 1：
```
輸入：s = "ABAB", k = 2
輸出：4
說明：將兩個 'A' 替換為 'B'，或將兩個 'B' 替換為 'A'。
```

### 範例 2：
```
輸入：s = "AABABBA", k = 1
輸出：4
說明：將中間的 'A' 替換為 'B'，形成 "AABBBBA"。
子字串 "BBBB" 擁有最長的重複字母，長度為 4。
也可能存在其他方式達成此答案。
```

## 限制條件

- `1 <= s.length <= 10^5`
- `s` 僅由大寫英文字母組成。
- `0 <= k <= s.length`

## 關鍵洞察

1. **滑動視窗法**：這是一道經典的滑動視窗問題，我們維護一個視窗，使其中的字元可以在最多 `k` 次更改內變成相同字元。

2. **有效視窗條件**：當 `(視窗長度 - 最大頻率) <= k` 時，視窗是有效的，即需要更改的字元數最多為 `k`。

3. **雙指標技巧**：透過移動右指標來擴展視窗，當條件不滿足時透過移動左指標來收縮視窗。

4. **字元頻率**：使用一個大小為 26 的陣列（對應大寫英文字母）來追蹤目前視窗中每個字元的出現頻率。

5. **最大頻率**：維護目前視窗中任何字元的最大頻率，以判斷需要更改多少字元。
