Overview
We are given a string s made up of lower case english letters and stars *.

We have to perform some operations on the given string such that for each star, we must remove both the closest non-star character to its left and the star itself.

We have to return the formed string after all stars are removed.

Approach 1: Stack
Intuition
To solve the problem, we must keep track of the most recently seen non-star character while iterating from the beginning to the end of the string. Our task should be easy if we are able to use a data structure or write an algorithm that keeps track of the most recent non-star character, which when removed gives the second most recent non-star character, which when removed gives the third most recent non-star character, and so on.

There are several approaches to solving such a problem, one of which is to use a stack data structure.

If you are new to stack data structure, please see our Leetcode Explore Card for more information on it!

We iterate through the given string s from the start. We check the character at every index i of s. If s[i] is a non-star character, we push it into the stack. Otherwise, if s[i] is a star character, we pop the character from the stack because the character at the top of the stack is the most recently seen non-star character from the characters that have been seen up to this point and are not removed.

When we've finished iterating over the string, the stack will contain the required string in reverse order. We pop all of the characters from the stack to form a string and then return the reverse of that string as our answer.

Here's a visual representation of how the approach works in the first example given in the problem description:

Current

Algorithm
Initialize a stack of characters st.
Iterate over the string s from the start and for each index i of the string:
If s[i] == '*', we perform the pop operation to remove the top character from the stack.
Otherwise, we have a non-star character, so we push it into the stack.
Create an empty string variable answer.
While the stack is not empty:
Append the top character of the stack to answer and remove it from the stack.
Return the reverse of answer.
Implementation

Complexity Analysis
Here, n is the length of s.

Time complexity: O(n)

We iterate over s and for every character we either push it in the stack or pop the top character from the stack which takes O(1) time per character. It takes O(n) time for n characters.
To form the answer string, we remove all the characters from the stack. Because a stack can have maximum of n characters, it would also take O(n) time in that case.
We also require O(n) time to reverse answer which can have n characters.
Space complexity: O(n)

The stack used in the solution can grow to a maximum size of n. We would need O(n) space in that case.

Approach 2: Strings
Intuition
We can also use strings in place of a stack to handle the required operations. It can provide similar operations as stack when dealing with characters.

Algorithm
Create an empty string variable answer that will store the string while performing the required operations.
Iterate over the string s from start and for each index i of the string:
If s[i] == '*', delete the last character from answer.
Otherwise, we have a non-star character, so we append it to answer.
Return answer.
Note: This approach does not work for Python as the strings are immutable in Python, so this would result in an O(n 
2
 ) time complexity.

Implementation

Complexity Analysis
Time complexity: O(n)

We iterate over s and for every character we either append it to answer or delete the last character from answer which takes O(1) time per character. It takes O(n) time for n characters.
Space complexity: O(n)

The answer string can have a maximum of n characters, requiring O(n) space. Normally, we do not count the answer towards the space complexity, but in this case we are performing logic on the answer variable, so we are counting it.

Approach 3: Two Pointers
Intuition
As previously discussed, our task is to iterate over the string while keeping track of the most recently seen non-star character who has not yet been removed.

We can also use two pointers to solve this problem, one to iterate over the given string s and the other to point to the position where the most recent non-star character is to be inserted (it will also help in removing the characters).

We create an array of characters ch having the same size as s. We also create two pointers i = 0 and j = 0.

We will iterate over s with the pointer i and add and remove non-star characters with the pointer j. The pointer j will point to the index where next non-star character is to be inserted. We will insert the characters in a data structure ch, and after iterating over the entire string s, we will have our required string in ch from index 0 till j - 1 (both inclusive).

We iterate the given string s from the start. For every index i of the string, if s[i] is a non-star character, we add s[i] to ch at index j. We increment j by 1 to insert the next non-star character at the next position.

Otherwise, if s[i] is a star character we decrement j by 1, resulting in the removal of the last character.

If we decrement j, you'll notice that whenever a non-star character is now met, it will override some character previously added in ch at the j index, resulting in the removal of the required non-star character. If there are not enough non-star characters to cover the position until where j previously went, they are still removed because we are only using indices from 0 to j - 1 to form the required string.

Algorithm
Create an array of characters ch having the same size as s.
Create an integer variable j that will point to the index in ch where a non-star character is to be inserted. We initialize it to 0 because the first character should be inserted at index 0.
Iterate over the string s from the start and for each index i of the string:
If s[i] == '*', decrement j by 1 to remove the most recently seen non-star that hasn't already been removed.
Otherwise, we have a non-star character, we add it to ch at index j. We also increment j by 1.
Create an empty string variable answer.
Iterate over ch from index 0 till j - 1 and append all the characters to answer.
Return answer.
Implementation

Complexity Analysis
Time complexity: O(n)

It takes O(n) time to initialize a character array of n size.
Iterating over the string s to form the required string in ch also takes O(n) time.
As there can be at most n characters in answer, it takes O(n) time to form it.
Space complexity: O(n)

The character array ch takes O(n) space.

---

中文翻譯

概述

給定一個字串 `s`，它由小寫英文字母與星號 `*` 組成。

我們需要對字串執行操作：對每個星號，都要移除它左邊最近的一個非星號字元，並同時移除這個星號本身。

最後回傳移除所有星號後得到的字串。

方法一：Stack（堆疊）
直覺

要解這題，我們在從頭到尾遍歷字串時，必須持續追蹤最近看到的非星號字元。若能用一種資料結構記住「最新的非星號字元」，而且刪除它後能回到次新的字元，就能自然完成題目要求。

這類問題常見的做法之一是使用堆疊（stack）。

如果你是第一次接觸 stack，建議先參考 LeetCode Explore Card 的說明。

我們從左到右遍歷字串 `s`。對每個索引 `i`：
- 若 `s[i]` 是非星號字元，push 進 stack。
- 若 `s[i]` 是星號，pop stack。

原因是 stack 頂端正好是目前最近、且尚未被移除的非星號字元。

當遍歷結束後，stack 會保存所需字串（但順序是反的）。把 stack 內容依序取出組成字串，再反轉即可得到答案。

以下是題目範例一的視覺化流程：

Current

演算法
- 初始化字元堆疊 `st`。
- 從頭遍歷字串 `s`，對每個索引 `i`：
- 若 `s[i] == '*'`，執行 pop 移除堆疊頂端。
- 否則把 `s[i]` push 到堆疊。
- 建立空字串變數 `answer`。
- 當堆疊不為空時：
- 取出堆疊頂端字元加到 `answer`，並移除該字元。
- 回傳 `answer` 的反轉字串。

實作

複雜度分析
這裡 `n` 是 `s` 的長度。

時間複雜度：`O(n)`

遍歷 `s` 時，每個字元只會做一次 push 或 pop，每次是 `O(1)`，總共 `O(n)`。
組答案時會把 stack 中所有字元取出，最多 `n` 個，所以是 `O(n)`。
最後反轉 `answer` 也需要 `O(n)`。

空間複雜度：`O(n)`

stack 在最壞情況下可成長到 `n`，所以需要 `O(n)` 空間。

方法二：Strings（字串）
直覺

也可以用字串來模擬 stack 的操作。對字元來說，字串能提供和 stack 類似的尾端新增/刪除能力。

演算法
- 建立空字串 `answer`，在處理過程中保存結果。
- 從頭遍歷字串 `s`，對每個索引 `i`：
- 若 `s[i] == '*'`，刪除 `answer` 最後一個字元。
- 否則把 `s[i]` 加到 `answer` 尾端。
- 回傳 `answer`。

注意：這種方法不適用於 Python，因為 Python 字串不可變，直接這樣做會導致 `O(n^2)` 的時間複雜度。

實作

複雜度分析
時間複雜度：`O(n)`

遍歷 `s` 時，每個字元不是被追加就是刪除尾端，單次 `O(1)`，總共 `O(n)`。

空間複雜度：`O(n)`

`answer` 最多可能有 `n` 個字元，因此需要 `O(n)` 空間。雖然一般有時不把輸出算進空間複雜度，但這裡 `answer` 本身也參與運算，因此納入計算。

方法三：Two Pointers（雙指標）
直覺

如前所述，重點是在遍歷時追蹤「最近看到、且還沒被移除」的非星號字元。

除了 stack，也可用雙指標：
- 一個指標遍歷原字串 `s`
- 另一個指標指向下一個非星號字元要寫入的位置（同時也能達成移除效果）

建立一個和 `s` 同長度的字元陣列 `ch`，並設定兩個指標 `i = 0`、`j = 0`。

用 `i` 遍歷 `s`，用 `j` 在 `ch` 中新增/移除非星號字元。`j` 表示下一個可寫入索引。遍歷結束後，`ch[0..j-1]` 就是答案。

遍歷過程：
- 若 `s[i]` 是非星號，寫到 `ch[j]`，然後 `j++`。
- 若 `s[i]` 是星號，`j--`，相當於移除上一個有效字元。

當 `j` 遞減後，之後新的非星號字元會覆蓋先前 `ch[j]` 的內容，等價於完成刪除。若剩餘非星號不足，也沒關係，因為最後只會使用 `0..j-1` 這段。

演算法
- 建立與 `s` 同大小的字元陣列 `ch`。
- 建立整數 `j`，表示下一個非星號要寫入 `ch` 的位置，初始為 `0`。
- 從頭遍歷字串 `s`，對每個索引 `i`：
- 若 `s[i] == '*'`，`j--`，移除最近未被移除的非星號。
- 否則把 `s[i]` 放到 `ch[j]`，再 `j++`。
- 建立空字串 `answer`。
- 遍歷 `ch` 的 `0` 到 `j-1`，把字元加入 `answer`。
- 回傳 `answer`。

實作

複雜度分析
時間複雜度：`O(n)`

初始化大小為 `n` 的字元陣列要 `O(n)`。
遍歷 `s` 並生成 `ch` 要 `O(n)`。
組合 `answer` 最多 `n` 字元，也要 `O(n)`。

空間複雜度：`O(n)`

字元陣列 `ch` 需要 `O(n)` 空間。
