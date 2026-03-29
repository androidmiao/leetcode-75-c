# 242. Valid Anagram

## Problem Statement

Given two strings `s` and `t`, return `true` if `t` is an anagram of `s`, and `false` otherwise.

An **Anagram** is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

## Examples

### Example 1:
```
Input: s = "anagram", t = "nagaram"
Output: true
```

### Example 2:
```
Input: s = "rat", t = "car"
Output: false
```

## Constraints

- `1 <= s.length, t.length <= 5 × 10^4`
- `s` and `t` consist of lowercase English letters.

## Follow up

What if the inputs contain Unicode characters? How would you adapt your solution to such a case?

---

# 242. 有效的異位詞

## 題目描述

給定兩個字串 `s` 和 `t`，若 `t` 是 `s` 的異位詞則回傳 `true`，否則回傳 `false`。

**異位詞（Anagram）** 是指將某個單詞或片語的字母重新排列後所形成的另一個單詞或片語，通常會恰好使用所有原始字母各一次。

## 範例

### 範例 1：
```
輸入：s = "anagram", t = "nagaram"
輸出：true
說明："nagaram" 是 "anagram" 的字母重新排列。
```

### 範例 2：
```
輸入：s = "rat", t = "car"
輸出：false
說明："car" 無法由 "rat" 的字母重新排列而成，因為字母組成不同。
```

## 限制條件

- `1 <= s.length, t.length <= 5 × 10^4`
- `s` 和 `t` 僅包含小寫英文字母。

## 進階問題

如果輸入包含 Unicode 字元怎麼辦？你會如何調整你的解法來處理這種情況？
