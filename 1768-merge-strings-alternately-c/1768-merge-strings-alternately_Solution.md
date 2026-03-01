Overview
We are given two strings word1 and word2.

Our task is to merge the strings by adding letters in alternating order, starting with word1. If one string is longer than the other, the additional letters must be appended to the end of the merged string.

We must return the merged string that has been formed.

Approach 1: Two Pointers
Intuition
There are numerous ways in which we can combine the given strings. We've covered a few of them in this article.

An intuitive method is to use two pointers to iterate over both strings. Assume we have two pointers, i and j, with i pointing to the first letter of word1 and j pointing to the first letter of word2. We also create an empty string result to store the outcome.

We append the letter pointed to by pointer i i.e., word1[i], and increment i by 1 to point to the next letter of word1. Because we need to add the letters in alternating order, next we append word2[j] to result. We also increase j by 1.

We continue iterating over the given strings until both are exhausted. We stop appending letters from word1 when i reaches the end of word1, and we stop appending letters from word2' when j reaches the end of word2.

Here's a visual representation of how the approach works in the second example given in the problem description:

Current

Algorithm
Create two variables, m and n, to store the length of word1 and word2.
Create an empty string variable result to store the result of merged words.
Create two pointers, i and j to point to indices of word1 and word2. We initialize both of them to 0.
While i < m || j < n:
If i < m, it means that we have not completely traversed word1. As a result, we append word1[i] to result. We increment i to point to next index of word1.
If j < n, it means that we have not completely traversed word2. As a result, we append word2[j] to result. We increment j to point to next index of word2.
Return result.
It is important to note how we form the result string in the following codes:
- cpp: The strings are mutable in cpp, which means they can be changed. As a result, we used the string variable and performed all operations on it. It takes constant time to append a character to the string.
- java: The String class is immutable in java. So we used the mutable StringBuilder to concatenate letters to result.
- python: Strings are immutable in python as well. As a result, we used the list result to append letters and later joined the list with an empty string to return it as a string object. The join operation takes linear time equal to the length of result to merge result with empty string.

Implementation

Complexity Analysis
Here, m is the length of word1 and n is the length of word2.

Time complexity: O(m+n)

We iterate over word1 and word2 once and push their letters into result. It would take O(m+n) time.
Space complexity: O(1) or O(m+n)

Without considering the space consumed by the input strings (word1 and word2) and the output string (result), we do not use more than constant space.
In Java, the StringBuilder requires O(n+m) space to store the merged result. Thus, while auxiliary space is O(1), total space complexity is O(n+m).

Approach 2: One Pointer
Intuition
To merge the given words, we can also use a single pointer.

Let i be the pointer that we'll use. We begin with i = 0 and progress to the size of the longer word between word1 and word2, i.e., till i = max(word1.length(), word2.length()).

As we progress to the size of a longer word, we check each time if i points to an index that is in bounds of the words or not. If i < word1.length(), we append word1[i] to result. Similarly if i < word2.length(), we append word2[i] to results.

However, if i exceeds the length of any word, we don't have any letters to add from that word, so we ignore it and continue adding the letter from the longer word.

Algorithm
Create two variables, m and n, to store the length of word1 and word2.
Create an empty string variable result to store the result of merged words.
Iterate over word1 and word2 using a loop running from i = 0 to i < max(m, n) and keep incrementing i by 1 after each iteration:
If i < m, it means that we have not completely traversed word1. As a result, we append word1[i] to result.
If i < n, it means that we have not completely traversed word2. As a result, we append word2[i] to result.
Return result.
Implementation

Complexity Analysis
Here, m is the length of word1 and n is the length of word2.

Time complexity: O(m+n)

We iterate over word1 and word2 once pushing their letters into result. It would take O(m+n) time.
Space complexity: O(1)

Without considering the space consumed by the input strings (word1 and word2) and the output string (result), we do not use more than constant space.

---

## 中文翻譯（繁體）

### 概覽（Overview）
給定兩個字串 `word1` 與 `word2`。

我們的任務是從 `word1` 開始，依照交替順序加入字元來合併字串。若其中一個字串比另一個長，剩餘字元必須接在合併後字串的尾端。

最後回傳形成的合併字串。

### 方法一：雙指標（Approach 1: Two Pointers）
#### 直覺（Intuition）
有很多方式可以合併給定字串，本文介紹其中幾種。

一個直觀的方法是使用兩個指標同時走訪兩個字串。假設有兩個指標 `i`、`j`，其中 `i` 指向 `word1` 的第一個字元，`j` 指向 `word2` 的第一個字元。我們另外建立一個空字串 `result` 用來存放結果。

先把指標 `i` 指向的字元，也就是 `word1[i]`，加入 `result`，再把 `i` 加 1 指到下一個字元。因為需要交替加入字元，接著把 `word2[j]` 加入 `result`，並把 `j` 加 1。

持續迭代直到兩個字串都走完。當 `i` 到達 `word1` 結尾時，就不再從 `word1` 加字元；當 `j` 到達 `word2` 結尾時，就不再從 `word2` 加字元。

以下是此方法在題目第二個範例中的示意：

Current

#### 演算法（Algorithm）
1. 建立兩個變數 `m`、`n`，分別儲存 `word1` 與 `word2` 的長度。
2. 建立空字串變數 `result`，用來儲存合併結果。
3. 建立兩個指標 `i`、`j` 指向 `word1` 與 `word2` 的索引，初始值都為 `0`。
4. 當 `i < m || j < n` 時重複：
   - 若 `i < m`，代表 `word1` 尚未走完，將 `word1[i]` 加入 `result`，並將 `i` 往後移一格。
   - 若 `j < n`，代表 `word2` 尚未走完，將 `word2[j]` 加入 `result`，並將 `j` 往後移一格。
5. 回傳 `result`。

需要注意不同語言中 `result` 的建構方式：
- `cpp`：字串可變（mutable），可直接在 `string` 變數上操作，附加單一字元為常數時間。
- `java`：`String` 不可變（immutable），所以使用可變的 `StringBuilder` 來串接字元。
- `python`：字串同樣不可變，因此先用 `list` 收集字元，最後以空字串 `""` 做 `join` 轉回字串。`join` 需線性時間，與 `result` 長度相同。

#### 實作（Implementation）

#### 複雜度分析（Complexity Analysis）
其中，`m` 是 `word1` 長度，`n` 是 `word2` 長度。

- 時間複雜度：`O(m+n)`
  只需各走訪 `word1` 和 `word2` 一次，並把字元放入 `result`。
- 空間複雜度：`O(1)` 或 `O(m+n)`
  若不把輸入字串（`word1`、`word2`）與輸出字串（`result`）算入，額外空間為常數。
  在 Java 中，`StringBuilder` 需 `O(m+n)` 空間儲存結果，因此雖然輔助空間是 `O(1)`，總空間複雜度是 `O(m+n)`。

### 方法二：單指標（Approach 2: One Pointer）
#### 直覺（Intuition）
合併這兩個字串也可以只用一個指標。

令 `i` 為唯一指標，從 `i = 0` 開始，前進到較長字串的長度，也就是直到
`i = max(word1.length(), word2.length())`。

在每次前進時，檢查 `i` 是否仍在字串索引範圍內。若 `i < word1.length()`，就把 `word1[i]` 加入 `result`；同理，若 `i < word2.length()`，就把 `word2[i]` 加入 `result`。

若 `i` 已超過某個字串長度，代表該字串沒有可加入字元，就略過它，繼續加入較長字串的字元。

#### 演算法（Algorithm）
1. 建立兩個變數 `m`、`n`，分別儲存 `word1` 與 `word2` 的長度。
2. 建立空字串變數 `result`，用來儲存合併結果。
3. 用迴圈從 `i = 0` 跑到 `i < max(m, n)`，每輪 `i` 加 1：
   - 若 `i < m`，代表 `word1` 尚未走完，將 `word1[i]` 加入 `result`。
   - 若 `i < n`，代表 `word2` 尚未走完，將 `word2[i]` 加入 `result`。
4. 回傳 `result`。

#### 實作（Implementation）

#### 複雜度分析（Complexity Analysis）
其中，`m` 是 `word1` 長度，`n` 是 `word2` 長度。

- 時間複雜度：`O(m+n)`
  只需各走訪 `word1` 和 `word2` 一次並將字元加入 `result`。
- 空間複雜度：`O(1)`
  若不計輸入字串與輸出字串本身所占空間，額外使用空間為常數。



