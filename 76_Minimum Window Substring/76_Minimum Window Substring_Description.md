Given two strings s and t of lengths m and n respectively, return the minimum window substring of s such that every character in t (including duplicates) is included in the window. If there is no such substring, return the empty string "".

The testcases will be generated such that the answer is unique.

Example 1:

Input: s = "ADOBECODEBANC", t = "ABC"
Output: "BANC"
Explanation: The minimum window substring "BANC" includes 'A', 'B', and 'C' from string t.

Example 2:

Input: s = "a", t = "a"
Output: "a"
Explanation: The entire string s is the minimum window.

Example 3:

Input: s = "a", t = "aa"
Output: ""
Explanation: Both 'a's from t must be included in the window.
Since the largest window of s only has one 'a', return empty string.

Constraints:

m == s.length
n == t.length
1 <= m, n <= 105
s and t consist of uppercase and lowercase English letters.

Follow up: Could you find an algorithm that runs in O(m + n) time?

Hint:
1. Use two pointers to create a window of letters in s, which would have all the characters from t.
2. Expand the right pointer until all the characters of t are covered.
3. Once all the characters are covered, move the left pointer and ensure that all the characters are still covered to minimize the subarray size.
4. Continue expanding the right and left pointers until you reach the end of s.


---
給定兩個字串 s 和 t，其長度分別為 m 和 n，請回傳 s 中的最小視窗子字串，使得 t 中的每個字元（包含重複字元）都包含在這個視窗內。如果不存在這樣的子字串，則回傳空字串 ""。

測試資料會保證答案是唯一的。

範例 1：

輸入：s = "ADOBECODEBANC", t = "ABC"
輸出："BANC"
說明：最小視窗子字串 "BANC" 包含了字串 t 中的 'A'、'B' 和 'C'。

範例 2：

輸入：s = "a", t = "a"
輸出："a"
說明：整個字串 s 本身就是最小視窗。

範例 3：

輸入：s = "a", t = "aa"
輸出：""
說明：t 中的兩個 'a' 都必須包含在視窗內。
由於 s 中最大的視窗也只有一個 'a'，因此回傳空字串。

限制條件：

m == s.length
n == t.length
1 <= m, n <= 10^5
s 和 t 皆由英文大小寫字母組成。

進階提問：你能否找出時間複雜度為 O(m + n) 的演算法？

提示：
1. 使用雙指標在 s 中建立一個視窗，並讓這個視窗包含 t 中所有字元。
2. 持續擴張右指標，直到 t 中所有字元都被包含。
3. 當所有字元都已被包含後，移動左指標，並在仍然涵蓋所有字元的前提下，盡可能縮小子陣列大小。
4. 持續移動右指標與左指標，直到到達 s 的結尾。
