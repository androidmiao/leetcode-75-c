Solution Article
Approach 1: Recursion
Intuition

We can recursively define the result of a merge operation on two lists as
the following (avoiding the corner case logic surrounding empty lists):

{ 
list1[0]+merge(list1[1:],list2)
list2[0]+merge(list1,list2[1:])
​
  
list1[0]<list2[0]
otherwise
​
 

Namely, the smaller of the two lists' heads plus the result of a merge on
the rest of the elements.

Algorithm

We model the above recurrence directly, first accounting for edge cases.
Specifically, if either of l1 or l2 is initially null, there is no
merge to perform, so we simply return the non-null list. Otherwise, we
determine which of l1 and l2 has a smaller head, and recursively set the
next value for that head to the next merge result. Given that both lists
are null-terminated, the recursion will eventually terminate.


Complexity Analysis

Time complexity : O(n+m)

Because each recursive call increments the pointer to l1 or l2 by one (approaching the dangling null at the end of each list), there will be exactly one call to mergeTwoLists per element in each list. Therefore, the time complexity is linear in the combined size of the lists.

Space complexity : O(n+m)

The first call to mergeTwoLists does not return until the ends of both l1 and l2 have been reached, so n+m stack frames consume O(n+m) space.


Approach 2: Iteration
Intuition

We can achieve the same idea via iteration by assuming that l1 is entirely
less than l2 and processing the elements one-by-one, inserting elements of
l2 in the necessary places in l1.

Algorithm

First, we set up a false "prehead" node that allows us to easily return the
head of the merged list later. We also maintain a prev pointer, which
points to the current node for which we are considering adjusting its next
pointer. Then, we do the following until at least one of l1 and l2 points
to null: if the value at l1 is less than or equal to the value at l2,
then we connect l1 to the previous node and increment l1. Otherwise, we
do the same, but for l2. Then, regardless of which list we connected, we
increment prev to keep it one step behind one of our list heads.

After the loop terminates, at most one of l1 and l2 is non-null.
Therefore (because the input lists were in sorted order), if either list is
non-null, it contains only elements greater than all of the
previously-merged elements. This means that we can simply connect the
non-null list to the merged list and return it.

To see this in action on an example, check out the animation below:

Current


Complexity Analysis

Time complexity : O(n+m)

Because exactly one of l1 and l2 is incremented on each loop
iteration, the while loop runs for a number of iterations equal to the
sum of the lengths of the two lists. All other work is constant, so the
overall complexity is linear.

Space complexity : O(1)

The iterative approach only allocates a few pointers, so it has a
constant overall memory footprint.

---
解題文章
方法一：遞迴
直覺

我們可以用遞迴的方式來定義兩個串列合併後的結果，如下所示
（先暫時不考慮空串列的邊界情況）：

{
list1[0] + merge(list1[1:], list2)
list2[0] + merge(list1, list2[1:])
}

當 `list1[0] < list2[0]` 時，選擇第一種情況；
否則選擇第二種情況。

也就是說，先取兩個串列頭節點中較小的那一個，
再加上其餘節點繼續合併後的結果。

演算法

我們可以直接依照上面的遞迴關係來實作，先處理邊界情況。
具體來說，如果 `l1` 或 `l2` 一開始就是 `null`，就表示不需要再做合併，
因此直接回傳另一條非空串列即可。否則，我們判斷 `l1` 和 `l2`
哪一個頭節點比較小，然後把該節點的 `next` 指向下一次遞迴合併的結果。
由於這兩條串列最後都會以 `null` 結尾，所以遞迴最終一定會停止。


複雜度分析

時間複雜度：`O(n+m)`

因為每一次遞迴呼叫都會讓 `l1` 或 `l2` 的指標往前移動一格
（逐步接近每條串列尾端的 `null`），所以每個節點都只會對應一次
`mergeTwoLists` 呼叫。因此，時間複雜度會和兩條串列總長度成線性關係。

空間複雜度：`O(n+m)`

第一次呼叫 `mergeTwoLists` 不會在兩條串列都走到尾端之前返回，
因此呼叫堆疊上會累積 `n+m` 個 stack frame，所以需要 `O(n+m)` 空間。


方法二：迭代
直覺

我們也可以用迭代方式實現同樣的想法。可以想像 `l1` 和 `l2`
中的節點會依序被取出，每次把較小的那個節點接到結果串列後面，
逐步完成合併。

演算法

首先，我們建立一個假的 `"prehead"` 節點，這樣之後就能方便地回傳
合併後串列的頭節點。我們同時維護一個 `prev` 指標，
它會指向目前正在處理、可能需要調整 `next` 的那個節點。
接著，當 `l1` 和 `l2` 都還不是 `null` 時，持續進行以下操作：
如果 `l1` 的值小於或等於 `l2` 的值，就把 `l1` 接到前一個節點後面，
然後把 `l1` 往前移動；否則就對 `l2` 做同樣的事。
每次接好一個節點後，不論接的是哪一條串列，都要把 `prev`
往前移動一格，讓它維持在目前合併結果的尾端。

當迴圈結束後，`l1` 和 `l2` 最多只會有一條不是 `null`。
因此（因為輸入串列原本就已排序），如果其中一條串列還有剩餘節點，
那些節點一定都大於或等於前面已經合併進結果串列的元素。
這代表我們只需要把那條非空串列直接接到結果串列尾端，然後回傳即可。

如果想看實際範例的動態過程，可以參考下方動畫：

Current


複雜度分析

時間複雜度：`O(n+m)`

因為每一次迴圈只會讓 `l1` 或 `l2` 其中之一往前移動一格，
所以 `while` 迴圈總共執行的次數，正好等於兩條串列長度總和。
其他操作都是常數時間，因此整體時間複雜度是線性的。

空間複雜度：`O(1)`

迭代作法只額外使用少量指標變數，因此整體額外記憶體使用量是常數級別。
