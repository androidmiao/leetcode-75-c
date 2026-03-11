Approach 1: Brute Force
We check every number from 1 to n-1 and pass it to the guess function. The number
for which a 0 is returned is the required answer.


Complexity Analysis

Time complexity : O(n). We scan all the numbers from 1 to n.
Space complexity : O(1). No extra space is used.

Approach 2: Using Binary Search
Algorithm

We can apply Binary Search to find the given number. We start with the mid
number. We pass that number to the guess function. If it returns a -1, it implies that the guessed number is larger than the required one. Thus, we use Binary Search for numbers lower than itself. Similarly, if it returns a 1, we use Binary Search
for numbers higher than itself.


Complexity Analysis

Time complexity : O(log 
2
​
 n). Binary Search is used.
Space complexity : O(1). No extra space is used.

Approach 3: Ternary Search
Algorithm

In Binary Search, we choose the middle element as the pivot in splitting. In Ternary Search, we choose two pivots (say m1 and m2) such that the given range is divided into three equal parts. If the required number num is less than m1 then we apply ternary search on the left segment of m1. If num lies between m1 and m2, we apply ternary search between m1 and m2. Otherwise we will search in the segment right to m2.


Complexity Analysis

Time complexity : O(log 
3
​
 n). Ternary Search is used.
Space complexity : O(1). No extra space is used.

Follow up
It seems that ternary search is able to terminate earlier compared to binary search. But why is binary search more widely used?

Comparisons between Binary Search and Ternary Search
Ternary Search is worse than Binary Search. The following outlines the recursive formula to count comparisons of Binary Search in the worst case.

T(n)
T( 
2
n
​
  )
∴T(n)
​
  
=T( 
2
n
​
  )+2,T(1)=1
=T( 
2 
2
 
n
​
  )+2
=T( 
2 
2
 
n
​
  )+2×2
=T( 
2 
3
 
n
​
  )+3×2
=…
=T( 
2 
log 
2
​
 n
 
n
​
  )+2log 
2
​
 n
=T(1)+2log 
2
​
 n
=1+2log 
2
​
 n
​
 
The following outlines the recursive formula to count comparisons of Ternary Search in the worst case.

T(n)
T( 
3
n
​
  )
∴T(n)
​
  
=T( 
3
n
​
  )+4,T(1)=1
=T( 
3 
2
 
n
​
  )+4
=T( 
3 
2
 
n
​
  )+2×4
=T( 
3 
3
 
n
​
  )+3×4
=…
=T( 
3 
log 
3
​
 n
 
n
​
  )+4log 
3
​
 n
=T(1)+4log 
3
​
 n
=1+4log 
3
​
 n
​
 
As shown above, the total comparisons in the worst case for ternary and binary search are 1+4log 
3
​
 n and 1+2log 
2
​
 n comparisons respectively. To determine which is larger, we can just look at the expression 2log 
3
​
 n and log 
2
​
 n . The expression 2log 
3
​
 n can be written as  
log 
2
​
 3
2
​
 ×log 
2
​
 n . Since the value of  
log 
2
​
 3
2
​
  is greater than one, Ternary Search does more comparisons than Binary Search in the worst case.

Analysis written by: @vinod23

---

中文翻譯：

方法 1：暴力法（Brute Force）

我們從 `1` 一路檢查到 `n - 1` 的每一個數字，並把它傳給 `guess` 函式。當某個數字回傳 `0` 時，這個數字就是正確答案。


複雜度分析

時間複雜度：`O(n)`。我們需要掃描從 `1` 到 `n` 的所有數字。
空間複雜度：`O(1)`。沒有使用額外空間。

方法 2：使用二分搜尋（Binary Search）
演算法

我們可以使用二分搜尋來找到指定的數字。首先從中間值 `mid` 開始，將這個數字傳給 `guess` 函式。如果它回傳 `-1`，表示猜的數字比真正答案大，因此我們只需要在更小的那一半繼續做二分搜尋。類似地，如果它回傳 `1`，我們就改在更大的那一半繼續搜尋。


複雜度分析

時間複雜度：`O(log₂ n)`。因為使用了二分搜尋。
空間複雜度：`O(1)`。沒有使用額外空間。

方法 3：三分搜尋（Ternary Search）
演算法

在二分搜尋中，我們會選擇中間元素作為切分點。在三分搜尋中，我們會選擇兩個切分點（例如 `m1` 和 `m2`），讓目前區間被分成三個大小相近的部分。如果目標數字 `num` 小於 `m1`，那麼就在 `m1` 左邊那一段進行三分搜尋。如果 `num` 介於 `m1` 和 `m2` 之間，就在 `m1` 和 `m2` 之間搜尋。否則，就在 `m2` 右邊那一段搜尋。


複雜度分析

時間複雜度：`O(log₃ n)`。因為使用了三分搜尋。
空間複雜度：`O(1)`。沒有使用額外空間。

延伸問題
看起來三分搜尋似乎能比二分搜尋更早結束，那為什麼二分搜尋反而更常被使用？

二分搜尋與三分搜尋的比較
三分搜尋其實比二分搜尋更差。下面列出二分搜尋在最壞情況下，計算比較次數的遞迴公式。

T(n)
T( 
2
n
​
  )
∴T(n)
​
  
=T( 
2
n
​
  )+2,T(1)=1
=T( 
2 
2
 
n
​
  )+2
=T( 
2 
2
 
n
​
  )+2×2
=T( 
2 
3
 
n
​
  )+3×2
=…
=T( 
2 
log 
2
​
 n
 
n
​
  )+2log 
2
​
 n
=T(1)+2log 
2
​
 n
=1+2log 
2
​
 n
​
 
下面列出三分搜尋在最壞情況下，計算比較次數的遞迴公式。

T(n)
T( 
3
n
​
  )
∴T(n)
​
  
=T( 
3
n
​
  )+4,T(1)=1
=T( 
3 
2
 
n
​
  )+4
=T( 
3 
2
 
n
​
  )+2×4
=T( 
3 
3
 
n
​
  )+3×4
=…
=T( 
3 
log 
3
​
 n
 
n
​
  )+4log 
3
​
 n
=T(1)+4log 
3
​
 n
=1+4log 
3
​
 n
​
 
如上所示，三分搜尋與二分搜尋在最壞情況下的總比較次數，分別是 `1 + 4log₃ n` 與 `1 + 2log₂ n`。要判斷哪一個比較大，只需要觀察 `2log₃ n` 與 `log₂ n` 這兩個表達式。`2log₃ n` 可以寫成 `log₂(3²) × log₂ n` 的形式。由於 `log₂(3²)` 的值大於 1，因此在最壞情況下，三分搜尋的比較次數會比二分搜尋更多。

分析作者：@vinod23
