Overview
This problem is practical, which can test one's basic knowledge about the data structure and algorithm.

First of all, let us clarify the problem a bit.
We are given a sequence of ping calls, i.e. [t 
1
​
 ,t 
2
​
 ,t 
3
​
 ,...t 
n
​
 ], ordered by the chronological order of their arrival time.

Given the current ping call t 
i
​
 , we are asked to count the number of previous calls that fall in the range of [t 
i
​
 −3000, t 
i
​
 ].

Voila. This is how we can reformulate the problem with the basic data structure such as Array.
Note, the sequence of calls is ever-increasing, and we are given the call one at a time.

By the way, we also have a dedicated Explore card called Array 101 where one can review the characteristics and operations about Array.
In addition, one will also discover many interesting problems that can be solved with Array.

Approach 1: Iteration over Sliding Window
Intuition

Now that we've clarified the nature of the problem, it shall not be difficult to come up with a solution.
In fact, the solution is as simple as iterating through an array or a list.

The idea is that we can use a container such as array or list to keep track of all the incoming ping calls. At each occasion of ping(t) call, first we append the call to the container, and then starting from the current call, we iterate backwards to count the calls that fall into the time range of [t-3000, t].

Before rushing to the implementation, let us dwell on the problem a bit more, since there are still plenty of things we could optimize.

One observation is that the sequence is ever-growing, so as our container.

On the other hand, once the ping calls become outdated, i.e. out of the scope of [t-3000, t], we do not need to keep them any longer in the container, since they will not contribute to the solution later.

As a result, one optimization that we could do is that rather than keeping all the historical ping calls in the container, we could remove the outdated calls on the go, which can avoid the overflow of the container and reduce the memory consumption to the least.

In summary, our container will function like a sliding window over the ever-growing sequence of ping calls.

sliding window

Based on the above description, the list data structure seems to be more fit as the container for our tasks, than the array.
Because the list is more adapted for the following two operations:

Appending: we will append each incoming call to the tail of the sliding window.

Popping: we need to pop out all the outdated calls from the head of the sliding window.

Algorithm

To implement the sliding window, we could use the LinkedList in Java or deque in Python.

Then the ping(t) function can be implemented in two steps:

Step 1): we append the current ping call to the tail of the sliding window.

Step 2): starting from the head of the sliding window, we remove the outdated calls, until we come across a still valid ping call.

As a result, the remaining calls in the sliding window are the ones that fall into the range of [t - 3000, t].


Complexity Analysis

First of all, let us estimate the upper-bound on the size of our sliding window.
Here we quote an important condition from the problem description: "It is guaranteed that every call to ping uses a strictly larger value of t than before."
Based on the above condition, the maximal number of elements in our sliding window would be 3000, which is also the maximal time difference between the head and the tail elements.

Time Complexity: O(1)

The main time complexity of our ping() function lies in the loop, which in the worst case would run 3000 iterations to pop out all outdated elements, and in the best case a single iteration.

Therefore, for a single invocation of ping() function, its time complexity is O(3000)=O(1).

If we assume that there is a ping call at each timestamp, then the cost of ping() is further amortized, where at each invocation, we would only need to pop out a single element, once the sliding window reaches its upper bound.

Space Complexity: O(1)

As we estimated before, the maximal size of our sliding window is 3000, which is a constant.
Discussion
Since the elements in our sliding window are strictly ordered, due to the condition of the problem, one might argue that it might be more efficient to use binary search to locate the most recent outdated calls and then starting from that point truncate all the previous calls.

In terms of search, binary search is seemingly more efficient than our linear search.
When the elements are held in the array data structure, it is true that binary search is more efficient.

However, it is not the case for the linked list, since there is no way to locate an element in the middle of a linked list instantly, which is a critical condition for binary search algorithm.

As a result, in order to apply binary search, we might have to opt for the Array data structure.
On the other hand, once we use the array as the container, we might have to keep all the historical elements, which in the long run is not space-efficient neither time-efficient later.
Or we have to find a way to efficiently remove the elements from array without frequently reallocating memory.

To conclude, it is doable to have a binary search solution.
Yet, it would complicate the design, and at the end the final solution is not necessarily more efficient than the above simple LinkedList-based sliding window.

Finally, if one is interested in such a problem, there is another rather similar problem called logger rate limiter.

---
概覽
這是一道很實用的題目，可以測試一個人對資料結構與演算法的基本理解。

首先，讓我們先把題目再釐清一下。
我們會得到一串 `ping` 呼叫，也就是 `[t1, t2, t3, ... tn]`，並且它們依照到達時間的先後順序排列。

給定目前這次 `ping` 呼叫 `ti`，我們需要計算有多少次先前的呼叫落在區間 `[ti - 3000, ti]` 內。

這樣一來，我們就可以把題目重新表述成一個可以用基本資料結構，例如陣列，來處理的問題。
注意，這個呼叫序列會持續遞增，而且題目是一次提供一個呼叫給我們。

順帶一提，還有一張專門介紹陣列的 Explore 卡片叫做 `Array 101`，可以用來複習陣列的特性與操作。
此外，也能在裡面看到許多可以用陣列解決的有趣題目。

方法一：對滑動視窗進行迭代
直覺

既然我們已經釐清了這題的本質，接下來就不難想到解法。
事實上，這個解法就像是單純地遍歷陣列或串列一樣直接。

核心想法是：我們可以使用某種容器，例如陣列或串列，來追蹤所有進來的 `ping` 呼叫。每次呼叫 `ping(t)` 時，先把這次呼叫加入容器中，然後從當前這次呼叫開始往前查看，計算落在 `[t - 3000, t]` 時間範圍內的呼叫數量。

不過，在急著實作之前，我們可以再多想一下，因為還有不少地方可以最佳化。

其中一個觀察是：這個序列會持續增長，因此我們的容器也會持續變大。

另一方面，一旦某些 `ping` 呼叫已經過期，也就是不再落在 `[t - 3000, t]` 的範圍內，我們就不需要再把它們留在容器中了，因為它們之後不會再對答案有任何幫助。

因此，一個可行的最佳化方式是：不要把所有歷史 `ping` 呼叫都一直保留在容器裡，而是在處理過程中即時移除過期的呼叫。這樣可以避免容器過大，也能把記憶體使用量降到最低。

總結來說，我們的容器就像是在這條持續增長的 `ping` 呼叫序列上維持一個滑動視窗。

滑動視窗

根據上面的描述，用串列作為容器會比使用陣列更適合這個任務。
因為串列更適合以下兩種操作：

加入尾端：我們會把每個新的呼叫加到滑動視窗的尾端。

從前端移除：我們需要把所有過期的呼叫從滑動視窗的前端移除。

演算法

要實作這個滑動視窗，在 Java 中可以使用 `LinkedList`，在 Python 中可以使用 `deque`。

接著，`ping(t)` 函式可以分成兩個步驟：

步驟 1）：把目前的 `ping` 呼叫加到滑動視窗的尾端。

步驟 2）：從滑動視窗的前端開始，把所有過期的呼叫移除，直到遇到第一個仍然有效的 `ping` 呼叫為止。

最後，滑動視窗中剩下的那些呼叫，就是所有落在 `[t - 3000, t]` 區間內的呼叫。


複雜度分析

首先，讓我們估計一下滑動視窗大小的上界。
這裡引用題目中的一個重要條件：`"It is guaranteed that every call to ping uses a strictly larger value of t than before."`
根據這個條件，滑動視窗中元素的最大數量會是 `3000`，因為視窗頭尾元素之間的最大時間差也是 `3000`。

時間複雜度：`O(1)`

`ping()` 函式的主要時間成本在那個迴圈上；最差情況下，它可能需要跑 `3000` 次來移除所有過期元素，最好情況則只需要一次。

因此，對單次 `ping()` 呼叫而言，時間複雜度是 `O(3000) = O(1)`。

如果我們假設每個時間點都會有一次 `ping` 呼叫，那麼 `ping()` 的成本還可以進一步做攤銷分析；當滑動視窗達到上限後，每次呼叫通常只需要移除一個元素。

空間複雜度：`O(1)`

如同前面估算的，滑動視窗的最大大小是 `3000`，這是一個常數。

討論
由於滑動視窗中的元素根據題目條件會保持嚴格遞增，因此有人可能會主張，使用二分搜尋去找出最近一個已過期的呼叫，再從那個位置開始一次截掉前面的所有元素，也許會更有效率。

從搜尋的角度來看，二分搜尋看起來確實比線性搜尋更有效率。
如果元素是存放在陣列資料結構中，這樣說是成立的。

但對 linked list 而言就不是這樣，因為我們無法立刻定位到 linked list 中間的某個元素，而這正是二分搜尋成立的重要前提。

因此，如果要套用二分搜尋，我們可能就得改用陣列作為資料結構。
但另一方面，一旦用陣列當容器，我們可能就得保留所有歷史元素；長期來看，這在空間與時間上都不夠有效率。
或者，我們還得再想辦法高效率地從陣列中移除元素，而不需要頻繁重新配置記憶體。

總結來說，二分搜尋解法確實做得到。
但它會讓整體設計更複雜，而且到最後也不一定比上面那個簡單的、基於 LinkedList 的滑動視窗解法更有效率。

最後，如果你對這類型題目有興趣，還有另一道相當類似的題目叫做 `logger rate limiter`。
