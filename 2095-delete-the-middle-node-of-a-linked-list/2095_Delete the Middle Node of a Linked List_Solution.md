Overview
In this problem, we are given a linked list head and the task is to remove the middle node.

We are also given the definition of middle node. Suppose there are n nodes in the linked list, then the index of the middle node middleIndex is:
middleIndex=floor(n/2)−1, if we use 0-base indexing.

In the figure below, the middle node is colored in red.

img

We have a similar problem about finding the middle of a linked list, which is a simple version of this problem and you may like to practise on it as well!

Approach 1: 2 Passes
Intuition
Let's start with a simple approach.

We make the first iteration, starting from head, goint through the entire linked list and getting the total number of nodes (let's say count). According to the definition provided, the index of the middle node is middleIndex=floor(count / 2)−1.

Now we make a second iteration to the predecessor node of the middle node, which means that we stop at index middleIndex−1.

Once we reach the predecessor node of the middle node, we can remove the middle node from the linked list.

Take the slides below as an example:

Current


Algorithm
If there is only one node, return null.
Otherwise, initialize two pointers p1 = head and p2 = head.
Iterate the linked list with p1 and count the total number of nodes it has (count).
Let p2 move forward by floor(count / 2) - 1 nodes, now it is the predecessor of the middle node, delete the middle node.
Return head.
Implementation

Complexity Analysis
Let n be the length of the input linked list.

Time complexity: O(n)

We iterate over the linked list twice, the first time traversing the entire linked list and the second traversing half of it. Hence there are a total of O(n) steps.
In each step, we move a pointer forward by one node, which takes constant time.
Remove the middle node takes a constant amount of time.
In summary, the overall time complexity is O(n).
Space complexity: O(1)

We only need two pointers, thus the space complexity is O(1).

Approach 2: Fast and Slow Pointers
Intuition
The key of this approach is that we have two pointers fast and slow traversing the linked list at the same time, and fast traverses twice as fast as slow. Therefore, when fast reaches the end of the linked list, slow is right in the middle! We only need one iteration to reach the middle node!

All that needs to be determined are a few lookup details. If there is only one node in the linked list, this node is also the one to be deleted and there are no nodes left after the deletion. Therefore, instead of initializing two pointers for the following procedure, we can just return null.

Why we initialize fast = head.next.next at the begining?

The reason for this is that we want to deleted the middle node instead of finding it. Therefore, we are actually looking for the predecessor of the middle node, not the middle node itself, or rather, this is like moving slow backward one node after the iteration stops.

Certainly, we can't move a pointer backward on a singly linked list, thus we can show this one less step on slow by letting fast moves forward one more step (by two nodes, of course). Hence, slow will also point to the predecessor node of the middle node (rather than the middle node) at the end of the iteration.

img

Take the slides below as an example:

Current


Algorithm
If there is only one node, return null.
Otherwise, initialize two pointers slow and fast, with slow pointing to head and fast pointing to the second successor node of head.
While neither fast and fast.next is null:
we move fast forward by 2 nodes.
we move slow forward by 1 node.
Now slow is the predecessor of the middle node, delete the middle node.
Return head.
Implementation

Complexity Analysis
Let n be the length of the input linked list.

Time complexity: O(n)

We stop the iteration when the pointer fast reaches the end, fast moves forward 2 nodes per step, so there are at most n/2 steps.
In each step, we move both fast and slow, which takes a constant amount of time.
Removing the middle node also takes constant time.
In summary, the overall time complexity is O(n).
Space complexity: O(1)

We only need two pointers, so the space complexity is O(1).


---
中文翻譯

概述
在這個問題中，我們會得到一個鏈結串列的頭節點 `head`，任務是刪除其中間節點。

題目也給了中間節點的定義。假設鏈結串列中有 `n` 個節點，那麼中間節點的索引 `middleIndex` 為：
如果使用 0-based indexing，則 `middleIndex = floor(n / 2) - 1`。

在下圖中，中間節點會以紅色標示。

img

這題和「找出鏈結串列中間節點」很相似，那是一個較簡單的版本，你也可以順便練習。

方法一：走訪兩次
直覺
先從一個簡單的方法開始。

第一次走訪時，從 `head` 開始遍歷整個鏈結串列，計算總節點數（設為 `count`）。根據題目給的定義，中間節點的索引就是 `middleIndex = floor(count / 2) - 1`。

接著進行第二次走訪，走到中間節點的前一個節點，也就是停在索引 `middleIndex - 1` 的位置。

當我們到達中間節點的前一個節點後，就可以把中間節點從鏈結串列中移除。

以下面的投影片為例：

Current


演算法
如果串列中只有一個節點，回傳 `null`。
否則，初始化兩個指標 `p1 = head` 與 `p2 = head`。
使用 `p1` 走訪整個鏈結串列，並計算總節點數 `count`。
讓 `p2` 往前移動 `floor(count / 2) - 1` 步，這時它就會停在中間節點的前一個節點，接著刪除中間節點。
回傳 `head`。
實作

複雜度分析
令 `n` 為輸入鏈結串列的長度。

時間複雜度：`O(n)`

我們總共走訪鏈結串列兩次，第一次遍歷整條串列，第二次走大約一半，因此總步數為 `O(n)`。
每一步都只是把指標往前移動一個節點，所花時間是常數。
刪除中間節點也只需要常數時間。
總結來說，整體時間複雜度是 `O(n)`。
空間複雜度：`O(1)`

我們只需要兩個指標，因此空間複雜度為 `O(1)`。

方法二：快慢指標
直覺
這個方法的關鍵是使用兩個同時走訪鏈結串列的指標 `fast` 與 `slow`，其中 `fast` 的速度是 `slow` 的兩倍。因此，當 `fast` 抵達鏈結串列尾端時，`slow` 剛好會在中間位置。我們只需要走訪一次，就能到達中間節點。

接下來只需要決定一些細節。如果鏈結串列只有一個節點，那麼這個節點本身也是要被刪除的節點，而刪除後就沒有任何節點剩下。因此，不需要再初始化後續流程中的兩個指標，直接回傳 `null` 即可。

為什麼一開始要設定 `fast = head.next.next`？

原因是這題要做的是「刪除中間節點」，不是單純「找到中間節點」。因此，我們真正想找的是中間節點的前一個節點，而不是中間節點本身。也可以理解成：如果迴圈結束後 `slow` 停在中間節點本身，我們其實希望它能再往回退一步。

但單向鏈結串列無法讓指標往回移動，所以我們改成讓 `fast` 一開始先多往前走一步（實際上是兩個節點，因為 `fast` 每次本來就走兩步）。這樣一來，當迴圈結束時，`slow` 就會剛好停在中間節點的前一個節點，而不是停在中間節點本身。

img

以下面的投影片為例：

Current


演算法
如果串列中只有一個節點，回傳 `null`。
否則，初始化兩個指標 `slow` 和 `fast`，其中 `slow` 指向 `head`，而 `fast` 指向 `head` 的第二個後繼節點。
當 `fast` 與 `fast.next` 都不是 `null` 時：
讓 `fast` 往前移動 2 個節點。
讓 `slow` 往前移動 1 個節點。
當迴圈結束時，`slow` 就是中間節點的前一個節點，接著刪除中間節點。
回傳 `head`。
實作

複雜度分析
令 `n` 為輸入鏈結串列的長度。

時間複雜度：`O(n)`

當 `fast` 到達尾端時，迴圈就會停止。由於 `fast` 每一步移動 2 個節點，因此最多只會執行 `n / 2` 次左右。
每一步中，我們同時移動 `fast` 和 `slow`，所需時間都是常數。
刪除中間節點也只需要常數時間。
總結來說，整體時間複雜度是 `O(n)`。
空間複雜度：`O(1)`

我們只需要兩個指標，因此空間複雜度為 `O(1)`。
