# 20. Valid Parentheses

## Description

Given a string `s` containing just the characters `'('`, `')'`, `'{'`, `'}'`, `'['` and `']'`, determine if the input string is valid.

An input string is valid if:

1. Open brackets must be closed by the same type of brackets.
2. Open brackets must be closed in the correct order.
3. Every close bracket has a corresponding open bracket of the same type.

## Examples

**Example 1:**

```
Input: s = "()"
Output: true
```

**Example 2:**

```
Input: s = "()[]{}"
Output: true
```

**Example 3:**

```
Input: s = "(]"
Output: false
```

**Example 4:**

```
Input: s = "([])"
Output: true
```

**Example 5:**

```
Input: s = "([)]"
Output: false
```

## Constraints

- `1 <= s.length <= 10^4`
- `s` consists of parentheses only `'()[]{}'`.

---

# 20. 有效的括號

## 題目描述

給定一個僅包含字元 `'('`、`')'`、`'{'`、`'}'`、`'['` 及 `']'` 的字串 `s`，判斷該字串是否為有效的括號組合。

一個有效的括號字串需滿足以下條件：

1. 每個開括號必須由相同類型的閉括號關閉。
2. 開括號必須按照正確的順序關閉。
3. 每個閉括號都有一個對應的同類型開括號。

## 範例

**範例 1：**

```
輸入：s = "()"
輸出：true
```

**範例 2：**

```
輸入：s = "()[]{}"
輸出：true
```

**範例 3：**

```
輸入：s = "(]"
輸出：false
```

**範例 4：**

```
輸入：s = "([])"
輸出：true
```

**範例 5：**

```
輸入：s = "([)]"
輸出：false
```

## 限制條件

- `1 <= s.length <= 10^4`
- `s` 僅由括號字元 `'()[]{}'` 組成。
