Approach 1: Sliding Window
Intuition
The question asks us to return the minimum window from the string S which has all the characters of the string T. Let us call a window desirable if it has all the characters from T.

We can use a simple sliding window approach to solve this problem.

In any sliding window based problem we have two pointers. One right pointer whose job is to expand the current window and then we have the left pointer whose job is to contract a given window. At any point in time only one of these pointers move and the other one remains fixed.

The solution is pretty intuitive. We keep expanding the window by moving the right pointer. When the window has all the desired characters, we contract (if possible) and save the smallest window till now.

The answer is the smallest desirable window.

For eg. S = "ABAACBAB" T = "ABC". Then our answer window is "ACB" and shown below is one of the possible desirable windows.


Algorithm
We start with two pointers, left and right initially pointing to the first element of the string S.

We use the right pointer to expand the window until we get a desirable window i.e. a window that contains all of the characters of T.

Once we have a window with all the characters, we can move the left pointer ahead one by one. If the window is still a desirable one we keep on updating the minimum window size.

If the window is not desirable any more, we repeat step2 onwards.


The above steps are repeated until we have looked at all the windows. The smallest window is returned.


Implementation

Complexity Analysis
Time Complexity: O(∣S∣+∣T∣) where |S| and |T| represent the lengths of strings S and T.
In the worst case we might end up visiting every element of string S twice, once by left pointer and once by right pointer. ∣T∣ represents the length of string T.

Space Complexity: O(∣S∣+∣T∣). ∣S∣ when the window size is equal to the entire string S. ∣T∣ when T has all unique characters.

Approach 2: Optimized Sliding Window
Intuition
A small improvement to the above approach can reduce the time complexity of the algorithm to O(2∗∣filtered_S∣+∣S∣+∣T∣), where filtered_S is the string formed from S by removing all the elements not present in T.

This complexity reduction is evident when ∣filtered_S∣<<<∣S∣.

This kind of scenario might happen when length of string T is way too small than the length of string S and string S consists of numerous characters which are not present in T.

Algorithm
We create a list called filtered_S which has all the characters from string S along with their indices in S, but these characters should be present in T.

  S = "ABCDDDDDDEEAFFBC" T = "ABC"
  filtered_S = [(0, 'A'), (1, 'B'), (2, 'C'), (11, 'A'), (14, 'B'), (15, 'C')]
  Here (0, 'A') means in string S character A is at index 0.
We can now follow our sliding window approach on the smaller string filtered_S.

Implementation

Complexity Analysis
Time Complexity : O(∣S∣+∣T∣) where |S| and |T| represent the lengths of strings S and T. The complexity is same as the previous approach. But in certain cases where ∣filtered_S∣ <<< ∣S∣, the complexity would reduce because the number of iterations would be 2∗∣filtered_S∣+∣S∣+∣T∣.
Space Complexity : O(∣S∣+∣T∣).

---

方法一：滑動視窗
直覺
這題要我們從字串 S 中找出最小的視窗，使這個視窗包含字串 T 的所有字元。如果一個視窗包含 T 中所有字元，我們就稱它為有效視窗。

我們可以用簡單的滑動視窗方法來解這個問題。

在任何以滑動視窗為基礎的問題中，通常都會有兩個指標。一個是右指標，負責擴張目前的視窗；另一個是左指標，負責收縮當前視窗。在任何時刻，通常只會有其中一個指標移動，而另一個維持不動。

這個解法相當直觀。我們持續移動右指標來擴張視窗。當視窗已經包含所有需要的字元後，就嘗試收縮視窗，並記錄到目前為止最小的有效視窗。

答案就是最小的有效視窗。

例如，S = "ABAACBAB"，T = "ABC"。那麼答案視窗是 "ACB"，下方展示的是其中一個可能的有效視窗。


演算法
一開始，我們使用兩個指標 left 和 right，兩者都先指向字串 S 的第一個字元。

我們使用右指標來擴張視窗，直到得到一個有效視窗，也就是一個包含 T 所有字元的視窗。

當我們得到一個包含所有字元的視窗後，就可以讓左指標一步一步往前移動。如果移動後視窗仍然有效，就持續更新最小視窗大小。

如果視窗不再有效，就重新從步驟 2 開始重複。


上述步驟會一直重複，直到我們檢查完所有可能的視窗。最後回傳最小的那一個視窗。


實作

複雜度分析
時間複雜度：O(∣S∣+∣T∣)，其中 |S| 和 |T| 分別代表字串 S 和字串 T 的長度。
在最壞情況下，字串 S 中的每個元素可能會被拜訪兩次，一次由左指標，一次由右指標。∣T∣ 代表字串 T 的長度。

空間複雜度：O(∣S∣+∣T∣)。當視窗大小等於整個字串 S 時會有 ∣S∣ 的部分；當 T 中所有字元都互不相同時，會有 ∣T∣ 的部分。

方法二：最佳化滑動視窗
直覺
對上面的方法做一點小改進，可以把演算法的時間複雜度降低成 O(2∗∣filtered_S∣+∣S∣+∣T∣)，其中 filtered_S 是從 S 中移除所有不在 T 內的字元後所形成的新字串。

當 ∣filtered_S∣ <<< ∣S∣ 時，這樣的複雜度下降會特別明顯。

這種情況可能發生在字串 T 的長度遠小於字串 S，而且字串 S 中包含大量不在 T 裡面的字元時。

演算法
我們建立一個名為 filtered_S 的列表，裡面包含字串 S 中所有同時存在於 T 的字元，以及它們在 S 中對應的索引位置。

  S = "ABCDDDDDDEEAFFBC" T = "ABC"
  filtered_S = [(0, 'A'), (1, 'B'), (2, 'C'), (11, 'A'), (14, 'B'), (15, 'C')]
  這裡的 (0, 'A') 表示在字串 S 中，字元 A 位於索引 0。
接著，我們就能在較小的 filtered_S 上套用原本的滑動視窗方法。

實作

複雜度分析
時間複雜度：O(∣S∣+∣T∣)，其中 |S| 和 |T| 分別代表字串 S 和字串 T 的長度。這個複雜度和前一種方法相同。不過在某些 ∣filtered_S∣ <<< ∣S∣ 的情況下，實際迭代次數會變成 2∗∣filtered_S∣+∣S∣+∣T∣，因此效率會更好。
空間複雜度：O(∣S∣+∣T∣)。
