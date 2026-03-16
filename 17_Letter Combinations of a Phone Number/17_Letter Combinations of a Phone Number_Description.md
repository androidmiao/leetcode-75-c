Given a string containing digits from 2-9 inclusive, return all possible letter combinations that the number could represent. Return the answer in any order.

A mapping of digits to letters (just like on the telephone buttons) is given below. Note that 1 does not map to any letters.


 

Example 1:

Input: digits = "23"
Output: ["ad","ae","af","bd","be","bf","cd","ce","cf"]
Example 2:

Input: digits = "2"
Output: ["a","b","c"]
 

Constraints:

1 <= digits.length <= 4
digits[i] is a digit in the range ['2', '9'].

---

給定一個只包含 2 到 9 數字的字串，回傳這個數字字串所能表示的所有字母組合。答案可以以任意順序回傳。

數字與字母的對應關係如下（就像電話按鍵上的字母對應一樣）。請注意，數字 1 不對應任何字母。


 

範例 1：

輸入：digits = "23"
輸出：["ad","ae","af","bd","be","bf","cd","ce","cf"]
範例 2：

輸入：digits = "2"
輸出：["a","b","c"]
 

限制條件：

1 <= digits.length <= 4
digits[i] 是一個介於 ['2', '9'] 範圍內的數字字元。
