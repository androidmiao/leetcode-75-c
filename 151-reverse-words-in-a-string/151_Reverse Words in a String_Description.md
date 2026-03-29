# 151. Reverse Words in a String

**Difficulty:** Medium

## Problem Statement

Given an input string `s`, reverse the order of the **words**.

A **word** is defined as a sequence of non-space characters. The **words** in `s` will be separated by at least one space.

Return *a string of the words in reverse order concatenated by a single space*.

**Note** that `s` may contain leading or trailing spaces or multiple spaces between two words. The returned string should only have a single space separating the words. Do not include any extra spaces.

## Examples

### Example 1

```
Input: s = "the sky is blue"
Output: "blue is sky the"
```

### Example 2

```
Input: s = "  hello world  "
Output: "world hello"
Explanation: Your reversed string should not contain leading or trailing spaces.
```

### Example 3

```
Input: s = "a good   example"
Output: "example good a"
Explanation: You need to reduce multiple spaces between two words to a single space in the reversed string.
```

## Constraints

- `1 <= s.length <= 10^4`
- `s` contains English letters (upper-case and lower-case), digits, and spaces `' '`.
- There is **at least one** word in `s`.

## Follow-up

If the string data type is mutable in your language, can you solve it **in-place** with `O(1)` extra space?

---

# 繁體中文翻譯

## 題目敘述

給定一個輸入字串 `s`，反轉其中 **單詞** 的順序。

**單詞** 定義為一連串非空白字元的序列。`s` 中的 **單詞** 之間至少以一個空格分隔。

回傳*一個字串，其中單詞以相反順序排列，並以單一空格連接*。

**注意**：`s` 可能包含前導空格、尾端空格，或單詞之間有多個空格。回傳的字串中，單詞之間應只有一個空格，不可包含額外空格。

## 範例

### 範例 1

```
輸入: s = "the sky is blue"
輸出: "blue is sky the"
```

### 範例 2

```
輸入: s = "  hello world  "
輸出: "world hello"
說明: 反轉後的字串不應包含前導或尾端空格。
```

### 範例 3

```
輸入: s = "a good   example"
輸出: "example good a"
說明: 需要將單詞之間的多個空格縮減為單一空格。
```

## 限制條件

- `1 <= s.length <= 10^4`
- `s` 包含英文字母（大寫與小寫）、數字及空格 `' '`。
- `s` 中**至少有一個**單詞。

## 進階挑戰

如果你的程式語言中字串資料型別是可變的（mutable），你能否以 **O(1)** 的額外空間**原地（in-place）**解決此問題？
