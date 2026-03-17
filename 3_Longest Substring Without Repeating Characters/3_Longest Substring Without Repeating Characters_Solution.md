Overview
The primary challenge in this problem is to find an efficient way to get all possible longest substrings that contain no duplicate characters. To achieve this, we need to take advantage of Hash Table, which checks if a character occurs before quickly.

In the following three approaches, we utilize a hash table to guarantee substrings have no repeating characters and optimize the algorithm to query possible substrings step by step: the first approach is intuitive but may result in Time Limit Exceeded, and the second one uses a slide window method to narrow down the search range, and the third make further use of HashMap to reduce the search range faster.

Approach 1: Brute Force (TLE)
Intuition
Check all the substring one by one to see if it has no duplicate character.

Algorithm
Suppose we have a function boolean allUnique(String substring) which will return true if the characters in the substring are all unique, otherwise false. We can iterate through all the possible substrings of the given string s and call the function allUnique. If it turns out to be true, then we update our answer of the maximum length of substring without duplicate characters.

Now let's fill the missing parts:

To enumerate all substrings of a given string, we enumerate the start and end indices of them. Suppose the start and end indices are i and j, respectively. Then we have 0≤i<j≤n (here end index j is exclusive by convention). Thus, using two nested loops with i from 0 to n−1 and j from i+1 to n, we can enumerate all the substrings of s.

To check if one string has duplicate characters, we can use a set. We iterate through all the characters in the string and put them into the set one by one. Before putting one character, we check if the set already contains it. If so, we return false. After the loop, we return true.

Implementation

Complexity Analysis
Time complexity : O(n 
3
 ).

To verify if characters within index range [i,j) are all unique, we need to scan all of them. Thus, it costs O(j−i) time.

For a given i, the sum of time costed by each j∈[i+1,n] is

∑ 
i+1
n
​
 O(j−i)

Thus, the sum of all the time consumption is:

O(∑ 
i=0
n−1
​
 (∑ 
j=i+1
n
​
 (j−i)))=O(∑ 
i=0
n−1
​
  
2
(1+n−i)(n−i)
​
 )=O(n 
3
 )

Space complexity : O(min(n,m)). We need O(k) space for checking a substring has no duplicate characters, where k is the size of the Set. The size of the Set is upper bounded by the size of the string n and the size of the charset/alphabet m.

Approach 2: Sliding Window
Intuition
Given a substring with a fixed end index in the string, maintain a HashMap to record the frequency of each character in the current substring. If any character occurs more than once, drop the leftmost characters until there are no duplicate characters.

Algorithm
The naive approach is very straightforward. But it is too slow. So how can we optimize it?

In the naive approaches, we repeatedly check a substring to see if it has duplicate character. But it is unnecessary. If a substring s 
ij
​
  from index i to j−1 is already checked to have no duplicate characters. We only need to check if s[j] is already in the substring s 
ij
​
 .

To check if a character is already in the substring, we can scan the substring, which leads to an O(n 
2
 ) algorithm. But we can do better.

By using HashSet as a sliding window, checking if a character in the current can be done in O(1).

A sliding window is an abstract concept commonly used in array/string problems. A window is a range of elements in the array/string which usually defined by the start and end indices, i.e. [i,j) (left-closed, right-open). A sliding window is a window "slides" its two boundaries to the certain direction. For example, if we slide [i,j) to the right by 1 element, then it becomes [i+1,j+1) (left-closed, right-open).

Back to our problem. We use HashSet to store the characters in current window [i,j) (j=i initially). Then we slide the index j to the right. If it is not in the HashSet, we slide j further. Doing so until s[j] is already in the HashSet. At this point, we found the maximum size of substrings without duplicate characters start with index i. If we do this for all i, we get our answer.

Implementation

Complexity Analysis
Time complexity : O(2n)=O(n). In the worst case each character will be visited twice by i and j.

Space complexity : O(min(m,n)). Same as the previous approach. We need O(k) space for the sliding window, where k is the size of the Set. The size of the Set is upper bounded by the size of the string n and the size of the charset/alphabet m.

Approach 3: Sliding Window Optimized
Intuition
The above solution requires at most 2n steps. In fact, it could be optimized to require only n steps. Instead of using a set to tell if a character exists or not, we could define a mapping of the characters to its index. Then we can skip the characters immediately when we found a repeated character.

Algorithm
The reason is that if s[j] have a duplicate in the range [i,j) with index j 
′
 , we don't need to increase i little by little. We can skip all the elements in the range [i,j 
′
 ] and let i to be j 
′
 +1 directly.

Here is a visualization of the above code.


Implementation

Complexity Analysis
Time complexity : O(n). Index j will iterate n times.

Space complexity : O(min(m,n)). Same as the previous approach.

Tips
All previous implementations have no assumption on the charset of the string s.

If we know that the charset is rather small, we can mimic what a HashSet/HashMap does with a boolean/integer array as direct access table. Though the time complexity of query or insertion is still O(1), the constant factor is smaller in an array than in a HashMap/HashSet. Thus, we can achieve a shorter runtime by the replacement here.

Commonly used tables are:

int[26] for Letters 'a' - 'z' or 'A' - 'Z'
int[128] for ASCII
int[256] for Extended ASCII

For this implementation, the space complexity is fixed to O(m) while the time complexity keeps unchanged. m is the size of the charset.

---

概述
這題的主要挑戰是，如何有效率地找出所有「不含重複字元」的最長子字串。為了做到這件事，我們需要利用雜湊表（Hash Table），因為它可以快速檢查某個字元是否曾經出現過。

在接下來的三種方法中，我們都利用雜湊表來保證子字串中沒有重複字元，並逐步優化搜尋方式：第一種方法直觀但可能會導致 Time Limit Exceeded；第二種方法使用滑動視窗（Sliding Window）縮小搜尋範圍；第三種方法則進一步利用 HashMap 更快地縮小搜尋範圍。

方法 1：暴力法（Brute Force，TLE）
直覺
把所有子字串逐一檢查，看它是否沒有重複字元。

演算法
假設我們有一個函式 `boolean allUnique(String substring)`，如果子字串中的所有字元都不重複，就回傳 `true`，否則回傳 `false`。我們可以走訪給定字串 `s` 的所有可能子字串，並呼叫 `allUnique`。如果結果為 `true`，就更新目前「最長不含重複字元子字串」的長度答案。

接著把缺少的部分補完整：

要列舉一個字串的所有子字串，我們可以列舉它們的起始索引與結束索引。假設起始索引與結束索引分別為 `i` 與 `j`，那麼有 `0 ≤ i < j ≤ n`（這裡依照慣例，結束索引 `j` 是不包含的）。因此，我們可以使用兩層迴圈，讓 `i` 從 `0` 到 `n−1`，`j` 從 `i+1` 到 `n`，就能列舉出 `s` 的所有子字串。

要檢查某個字串是否含有重複字元，可以使用集合（Set）。我們依序走訪字串中的每個字元，逐一放入集合。在放入之前，先檢查集合裡是否已經有這個字元；如果有，就回傳 `false`。如果整個迴圈跑完都沒有重複，就回傳 `true`。

實作

複雜度分析
時間複雜度：`O(n^3)`。

要驗證索引範圍 `[i, j)` 中的字元是否全都唯一，需要掃描該範圍中的所有字元，因此成本是 `O(j−i)`。

對於固定的 `i`，所有 `j ∈ [i+1, n]` 的時間成本總和為：

`∑ O(j−i)`

因此，總時間複雜度為：

`O(∑(∑(j−i))) = O(n^3)`

空間複雜度：`O(min(n, m))`。我們需要 `O(k)` 的空間來檢查某個子字串是否有重複字元，其中 `k` 是集合（Set）的大小。集合大小的上限受到字串長度 `n` 與字元集大小 `m` 的限制。

方法 2：滑動視窗（Sliding Window）
直覺
對於一個在原字串中具有固定結尾索引的子字串，我們維護一個 HashMap 來記錄目前子字串中每個字元出現的頻率。如果有任何字元出現超過一次，就持續移除最左邊的字元，直到沒有重複字元為止。

演算法
天真的方法很直接，但速度太慢。那麼要怎麼優化呢？

在暴力法中，我們會反覆檢查某個子字串是否含有重複字元，但其實沒有必要每次都重頭檢查。如果某個子字串 `s[i..j-1]` 已經確認沒有重複字元，那我們只需要檢查 `s[j]` 是否已經存在於這個子字串中即可。

若要檢查某個字元是否已經在子字串中，我們可以直接掃描這個子字串，這樣會得到 `O(n^2)` 的演算法。但我們還可以做得更好。

利用 HashSet 作為滑動視窗後，就能在 `O(1)` 時間內檢查目前字元是否已經存在於視窗中。

滑動視窗是一個在陣列或字串問題中很常見的抽象概念。所謂視窗，是陣列或字串中的一段範圍，通常用起始與結束索引來表示，也就是 `[i, j)`（左閉右開）。滑動視窗表示這個視窗的兩個邊界會朝某個方向滑動。例如，若把 `[i, j)` 向右滑動一格，就會變成 `[i+1, j+1)`。

回到這題。我們用 HashSet 儲存目前視窗 `[i, j)` 中的字元（初始時 `j = i`）。然後讓索引 `j` 持續往右移動。如果 `s[j]` 不在 HashSet 中，就繼續讓 `j` 往右走；直到 `s[j]` 已經存在於 HashSet 中。此時，我們就找到了所有以索引 `i` 為起點、且不含重複字元的子字串中的最大長度。對所有 `i` 都做一次，就能得到答案。

實作

複雜度分析
時間複雜度：`O(2n) = O(n)`。在最壞情況下，每個字元最多會被 `i` 與 `j` 各走訪一次。

空間複雜度：`O(min(m, n))`。與前一種方法相同。我們需要 `O(k)` 的空間來維護滑動視窗，其中 `k` 是集合大小；集合大小的上限受字串長度 `n` 與字元集大小 `m` 限制。

方法 3：最佳化滑動視窗（Sliding Window Optimized）
直覺
上面的解法最多需要 `2n` 步。實際上，它還可以進一步最佳化到只需要 `n` 步。與其用集合來判斷字元是否存在，我們可以直接建立「字元對應到索引」的映射。這樣一旦遇到重複字元，就能立刻跳過中間那一段。

演算法
原因是：如果 `s[j]` 在範圍 `[i, j)` 中有重複字元，其位置為 `j'`，那麼我們不需要讓 `i` 一格一格慢慢增加；可以直接略過 `[i, j']` 這整段，讓 `i` 直接變成 `j' + 1`。

上面的程式碼有一個對應的視覺化示意圖。

實作

複雜度分析
時間複雜度：`O(n)`。索引 `j` 只會迭代 `n` 次。

空間複雜度：`O(min(m, n))`。與前一種方法相同。

提示
前面所有實作都沒有對字串 `s` 的字元集做任何假設。

如果我們知道字元集很小，就可以用布林陣列或整數陣列來模擬 HashSet / HashMap 的功能，作為直接存取表（direct access table）。雖然查詢與插入的時間複雜度仍然是 `O(1)`，但陣列的常數因子通常比 HashMap / HashSet 更小，因此通常可以得到更短的執行時間。

常用的表格如下：

`int[26]` 對應字母 `'a'` 到 `'z'` 或 `'A'` 到 `'Z'`
`int[128]` 對應 ASCII
`int[256]` 對應 Extended ASCII

對這類實作來說，空間複雜度固定為 `O(m)`，而時間複雜度維持不變。這裡的 `m` 是字元集大小。
