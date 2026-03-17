Given a string s, find the length of the longest substring without duplicate characters.

 

Example 1:

Input: s = "abcabcbb"
Output: 3
Explanation: The answer is "abc", with the length of 3. Note that "bca" and "cab" are also correct answers.
Example 2:

Input: s = "bbbbb"
Output: 1
Explanation: The answer is "b", with the length of 1.
Example 3:

Input: s = "pwwkew"
Output: 3
Explanation: The answer is "wke", with the length of 3.
Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
 

Constraints:

0 <= s.length <= 5 * 104
s consists of English letters, digits, symbols and spaces.

Hint1: Since maximum string size is at most 26, generate and check all possible substrings with length at most 26.

給定一個字串 `s`，請找出「不含重複字元的最長子字串」的長度。

---

範例 1：

輸入：`s = "abcabcbb"`
輸出：`3`
說明：答案是 `"abc"`，長度為 `3`。注意，`"bca"` 和 `"cab"` 也都是正確答案。
範例 2：

輸入：`s = "bbbbb"`
輸出：`1`
說明：答案是 `"b"`，長度為 `1`。
範例 3：

輸入：`s = "pwwkew"`
輸出：`3`
說明：答案是 `"wke"`，長度為 `3`。
請注意，答案必須是子字串，`"pwke"` 是子序列，不是子字串。



限制條件：

`0 <= s.length <= 5 * 10^4`
`s` 由英文字母、數字、符號與空白組成。

提示 1：由於最大字串大小最多為 `26`，可以產生並檢查所有長度最多為 `26` 的可能子字串。
