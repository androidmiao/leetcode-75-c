You are given two strings word1 and word2. Merge the strings by adding letters in alternating order, starting with word1. If a string is longer than the other, append the additional letters onto the end of the merged string.

Return the merged string.

 

Example 1:

Input: word1 = "abc", word2 = "pqr"
Output: "apbqcr"
Explanation: The merged string will be merged as so:
word1:  a   b   c
word2:    p   q   r
merged: a p b q c r
Example 2:

Input: word1 = "ab", word2 = "pqrs"
Output: "apbqrs"
Explanation: Notice that as word2 is longer, "rs" is appended to the end.
word1:  a   b 
word2:    p   q   r   s
merged: a p b q   r   s
Example 3:

Input: word1 = "abcd", word2 = "pq"
Output: "apbqcd"
Explanation: Notice that as word1 is longer, "cd" is appended to the end.
word1:  a   b   c   d
word2:    p   q 
merged: a p b q c   d
 

Constraints:

1 <= word1.length, word2.length <= 100
word1 and word2 consist of lowercase English letters.


------
以下是 `Description.md` 的中文翻譯（繁體）：

給你兩個字串 `word1` 和 `word2`。請將這兩個字串依照「交替加入字母」的方式合併，並且從 `word1` 開始。  
如果其中一個字串比另一個更長，請把多出來的字母接在合併後字串的尾端。

回傳合併後的字串。

---

**範例 1：**

輸入：`word1 = "abc"`, `word2 = "pqr"`  
輸出：`"apbqcr"`  
說明：合併方式如下：  
`word1:  a   b   c`  
`word2:    p   q   r`  
`merged: a p b q c r`

**範例 2：**

輸入：`word1 = "ab"`, `word2 = "pqrs"`  
輸出：`"apbqrs"`  
說明：由於 `word2` 較長，`"rs"` 會接在尾端。  
`word1:  a   b`  
`word2:    p   q   r   s`  
`merged: a p b q   r   s`

**範例 3：**

輸入：`word1 = "abcd"`, `word2 = "pq"`  
輸出：`"apbqcd"`  
說明：由於 `word1` 較長，`"cd"` 會接在尾端。  
`word1:  a   b   c   d`  
`word2:    p   q`  
`merged: a p b q c   d`

---

**限制條件：**

- `1 <= word1.length, word2.length <= 100`
- `word1` 與 `word2` 都只包含英文小寫字母。