You are given a string s, which contains stars *.

In one operation, you can:

Choose a star in s.
Remove the closest non-star character to its left, as well as remove the star itself.
Return the string after all stars have been removed.

Note:

The input will be generated such that the operation is always possible.
It can be shown that the resulting string will always be unique.
 

Example 1:

Input: s = "leet**cod*e"
Output: "lecoe"
Explanation: Performing the removals from left to right:
- The closest character to the 1st star is 't' in "leet**cod*e". s becomes "lee*cod*e".
- The closest character to the 2nd star is 'e' in "lee*cod*e". s becomes "lecod*e".
- The closest character to the 3rd star is 'd' in "lecod*e". s becomes "lecoe".
There are no more stars, so we return "lecoe".
Example 2:

Input: s = "erase*****"
Output: ""
Explanation: The entire string is removed, so we return an empty string.
 

Constraints:

1 <= s.length <= 105
s consists of lowercase English letters and stars *.
The operation above can be performed on s.

---

中文翻譯：

給定一個字串 `s`，其中包含星號 `*`。

在一次操作中，你可以：

選擇 `s` 中的一個星號。
移除它左側最近的一個非星號字元，並同時移除這個星號本身。
回傳移除所有星號後的字串。

注意：

輸入會被設計成每次操作都一定可行。
可以證明，最後得到的字串一定是唯一的。

範例 1：

輸入：`s = "leet**cod*e"`
輸出：`"lecoe"`
解釋：依照從左到右的移除過程：
- 第 1 個星號左側最近的字元是 `"leet**cod*e"` 中的 `'t'`。`s` 變成 `"lee*cod*e"`。
- 第 2 個星號左側最近的字元是 `"lee*cod*e"` 中的 `'e'`。`s` 變成 `"lecod*e"`。
- 第 3 個星號左側最近的字元是 `"lecod*e"` 中的 `'d'`。`s` 變成 `"lecoe"`。
已經沒有更多星號，所以回傳 `"lecoe"`。

範例 2：

輸入：`s = "erase*****"`
輸出：`""`
解釋：整個字串都被移除了，所以回傳空字串。

限制條件：

`1 <= s.length <= 10^5`
`s` 由小寫英文字母與星號 `*` 組成。
上述操作可以在 `s` 上執行。
