Clarifying Notes
The video solution modifies the input by updating the value of the head. This is not recommended. It is better to create a new node to traverse the linked list, as shown in the solution below, so we can still access the head of the list.

Solution Article
This article is for beginners. It introduces the following ideas: Linked List, Hash Table and Two Pointers.


Approach 1: Hash Table
Intuition

To detect if a list is cyclic, we can check whether a node had been visited before. A natural way is to use a hash table.

Algorithm

We go through each node one by one and record each node's reference (or memory address) in a hash table. If the current node is null, we have reached the end of the list and it must not be cyclic. If current node’s reference is in the hash table, then return true.


Complexity analysis

Let n be the total number of nodes in the linked list.

Time complexity : O(n).
We visit each of the n elements in the list at most once. Adding a node to the hash table costs only O(1) time.

Space complexity: O(n).
The space depends on the number of elements added to the hash table, which contains at most n elements.



---
Approach 2: Floyd's Cycle Finding Algorithm
Intuition

Imagine two runners running on a track at different speed. What happens when the track is actually a circle?

Algorithm

The space complexity can be reduced to O(1) by considering two pointers at different speed - a slow pointer and a fast pointer. The slow pointer moves one step at a time while the fast pointer moves two steps at a time.

If there is no cycle in the list, the fast pointer will eventually reach the end and we can return false in this case.

Now consider a cyclic list and imagine the slow and fast pointers are two runners racing around a circle track. The fast runner will eventually meet the slow runner. Why? Consider this case (we name it case A) - The fast runner is just one step behind the slow runner. In the next iteration, they both increment one and two steps respectively and meet each other.

How about other cases? For example, we have not considered cases where the fast runner is two or three steps behind the slow runner yet. This is simple, because in the next or next's next iteration, this case will be reduced to case A mentioned above.


Complexity analysis

Time complexity : O(n).
Let us denote n as the total number of nodes in the linked list. To analyze its time complexity, we consider the following two cases separately.

List has no cycle:
The fast pointer reaches the end first and the run time depends on the list's length, which is O(n).

List has a cycle:
We break down the movement of the slow pointer into two steps, the non-cyclic part and the cyclic part:

The slow pointer takes "non-cyclic length" steps to enter the cycle. At this point, the fast pointer has already reached the cycle. Number of iterations=non-cyclic length=N

Both pointers are now in the cycle. Consider two runners running in a cycle - the fast runner moves 2 steps while the slow runner moves 1 steps at a time. Since the speed difference is 1, it takes  
difference of speed
distance between the 2 runners
​
  loops for the fast runner to catch up with the slow runner.As the distance is at most cyclic length K - 1 and the speed difference is 1, we conclude that
Number of iterations=at most cyclic length K - 1.

Therefore, the worst case time complexity is O(N+K), which is O(n).

Space complexity : O(1).
We only use two nodes (slow and fast) so the space complexity is O(1).


---

說明補充
影片中的解法會透過更新 `head` 的值來修改輸入。這樣並不建議。比較好的做法是另外建立一個新節點指標來走訪 linked list，就像下面的解法所示，這樣我們仍然可以保留對串列頭 `head` 的存取。

題解文章
這篇文章是寫給初學者的。它會介紹以下幾個概念：Linked List、Hash Table，以及 Two Pointers。


方法 1：Hash Table
直覺

要偵測一個串列是否有環，我們可以檢查某個節點是否曾經被拜訪過。一個很自然的方法，就是使用 hash table。

演算法

我們逐一走訪每個節點，並把每個節點的參考位址（或記憶體位址）記錄在 hash table 中。如果目前節點是 `null`，代表我們已經走到串列尾端，因此它一定不是環狀串列。如果目前節點的參考位址已經存在於 hash table 中，就回傳 `true`。


複雜度分析

設 `n` 為 linked list 中節點的總數。

時間複雜度：`O(n)`。
我們最多只會拜訪串列中的 `n` 個元素各一次。把一個節點加入 hash table 只需要 `O(1)` 時間。

空間複雜度：`O(n)`。
空間使用量取決於加入 hash table 的元素數量，而 hash table 最多會包含 `n` 個元素。



---
方法 2：Floyd's Cycle Finding Algorithm
直覺

想像有兩個跑者在跑道上以不同速度前進。如果這條跑道其實是一個圓，會發生什麼事？

演算法

我們可以使用兩個速度不同的指標，把空間複雜度降低到 `O(1)`，也就是一個慢指標 `slow` 和一個快指標 `fast`。`slow` 每次前進一步，而 `fast` 每次前進兩步。

如果串列中沒有環，快指標最後一定會走到尾端，此時我們就可以回傳 `false`。

現在來考慮一個有環的串列，並把慢指標與快指標想像成在圓形跑道上競速的兩位跑者。快跑者最終一定會遇到慢跑者。為什麼？先看一種情況（我們稱它為情況 A）：快跑者只落後慢跑者一步。在下一輪中，他們分別前進兩步與一步，最後就會相遇。

那其他情況呢？例如，我們還沒討論快跑者落後慢跑者兩步或三步的情況。其實這很簡單，因為在下一輪或下下輪迭代中，這些情況都會縮減成前面提到的情況 A。


複雜度分析

時間複雜度：`O(n)`。
設 `n` 為 linked list 中節點的總數。為了分析時間複雜度，我們把情況分成以下兩種：

串列沒有環：
快指標會先到達尾端，因此執行時間取決於串列長度，也就是 `O(n)`。

串列有環：
我們可以把慢指標的移動拆成兩個階段來看，也就是非環部分與環內部分：

慢指標需要走「非環長度」的步數才能進入環中。到這個時候，快指標也已經進入環中了。
迭代次數 = 非環長度 = `N`

接著兩個指標都在環中。把它們想成在圓形跑道上跑步的兩位跑者：快跑者每次走 2 步，慢跑者每次走 1 步。由於速度差是 1，所以快跑者追上慢跑者所需的圈數，等於兩者距離除以速度差。

而兩者之間的距離最多只會是環長 `K - 1`，速度差則是 1，因此我們可以得到：
迭代次數 = 最多為環長 `K - 1`

因此，最壞情況下的時間複雜度為 `O(N + K)`，也就是 `O(n)`。

空間複雜度：`O(1)`。
我們只使用了兩個節點指標（`slow` 和 `fast`），所以空間複雜度是 `O(1)`。
