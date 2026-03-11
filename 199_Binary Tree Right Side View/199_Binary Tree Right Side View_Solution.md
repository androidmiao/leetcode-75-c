Overview
DFS vs. BFS

There are two ways to traverse the tree: DFS depth first search and BFS breadth first search. Here is a small summary

diff

BFS traverses level by level, and DFS first goes to the leaves.

diff

Which approach to choose, BFS or DFS?

The problem is to return a list of the last elements from all levels, so it's way more natural to implement BFS here.

Time complexity is the same O(N) both for DFS and BFS since one has to visit all nodes.

Space complexity is O(H) for DFS and O(D) for BFS, where H is a tree height, and D is a tree diameter. They both result in O(N) space in the worst-case scenarios: skewed tree for DFS and complete tree for BFS.

BFS wins for this problem, so let's use the opportunity to check out three different BFS implementations with the queue.

BFS implementation

All three implementations use the queue in a standard BFS way:

Push the root into the queue.

Pop-out a node from the left.

Push the left child into the queue, and then push the right child.

diff

Three BFS approaches

The difference is how to find the end of the level, i.e. the rightmost element:

Two queues, one for the previous level and one for the current.

One queue with a sentinel to mark the end of the level.

One queue + level size measurement.



Approach 1: BFS: Two Queues
Let's use two queues: one for the current level, and one for the next. The idea is to pop the nodes one by one from the current level and push their children into the next level queue. Each time the current queue is empty, we have the right side element in our hands.

diff

Algorithm

Initiate the list of the right side view rightside.

Initiate two queues: one for the current level, and one for the next. Add root into nextLevel queue.

While nextLevel queue is not empty:

Initiate the current level: currLevel = nextLevel, and empty the next level nextLevel.

While the current level queue is not empty:

Pop out a node from the current level queue.

Add first left and then right child node into nextLevel queue.

Now currLevel is empty, and the node we have in hands is the last one, and makes a part of the right side view. Add it into rightside.

Return rightside.

Implementation


Complexity Analysis

Time complexity: O(N) since one has to visit each node.

Space complexity: O(D) to keep the queues, where D is a tree diameter. Let's use the last level to estimate the queue size. This level could contain up to N/2 tree nodes in the case of complete binary tree.



Approach 2: BFS: One Queue + Sentinel
Another approach is to push all the nodes in one queue and to use a sentinel node to separate the levels. Typically, one could use null as a sentinel.

diff

The first step is to initiate the first level: root + null as a sentinel. Once it's done, continue to pop the nodes one by one from the left and push their children to the right. Stop each time the current node is null because it means we hit the end of the current level. Each stop is a time to update a right-side view list and to push null in the queue to mark the end of the next level.

Algorithm

Initiate the list of the right side view rightside.

Initiate the queue by adding a root. Add null sentinel to mark the end of the first level.

Initiate the current node as root.

While the queue is not empty:

Save the previous node prev = curr and pop the current node from the queue curr = queue.poll().

While the current node is not null:

Add first left and then right child node into the queue.

Update both previous and current nodes: prev = curr, curr = queue.poll().

Now the current node is null, i.e. we reached the end of the current level. Hence the previous node is the rightmost one and makes a part of the right side view. Add it into rightside.

If the queue is not empty, push the null node as a sentinel, to mark the end of the next level.

Return rightside.

Implementation

Note, that ArrayDeque in Java doesn't support null elements, and hence the data structure to use here is LinkedList.


Complexity Analysis

Time complexity: O(N) since one has to visit each node.

Space complexity: O(D) to keep the queues, where D is a tree diameter. Let's use the last level to estimate the queue size. This level could contain up to N/2 tree nodes in the case of complete binary tree.



Approach 3: BFS: One Queue + Level Size Measurements
Instead of using the sentinel, we could write down the length of the current level.

diff

Algorithm

Initiate the list of the right side view rightside.

Initiate the queue by adding a root.

While the queue is not empty:

Write down the length of the current level: levelLength = queue.size().

Iterate over i from 0 to level_length - 1:

Pop the current node from the queue: node = queue.poll().

If i == levelLength - 1, then it's the last node in the current level, push it to rightsize list.

Add first left and then right child node into the queue.

Return rightside.

Implementation


Complexity Analysis

Time complexity: O(N) since one has to visit each node.

Space complexity: O(D) to keep the queues, where D is a tree diameter. Let's use the last level to estimate the queue size. This level could contain up to N/2 tree nodes in the case of complete binary tree.



Approach 4: Recursive DFS
Everyone likes recursive DFS, so let's add it here as well. The idea is simple: to traverse the tree level by level, starting each time from the rightmost child.

Implementation


Complexity Analysis

Time complexity: O(N) since one has to visit each node.

Space complexity: O(H) to keep the recursion stack, where H is a tree height. The worst-case situation is a skewed tree when H=N.

---

總覽
DFS 與 BFS

走訪樹有兩種方式：DFS（深度優先搜尋）與 BFS（廣度優先搜尋）。下面是一個簡單摘要。

diff

BFS 是一層一層走訪，而 DFS 會先一路走到葉節點。

diff

該選哪一種方法，BFS 還是 DFS？

這題要求回傳每一層最後一個元素所組成的清單，所以用 BFS 來實作會自然得多。

DFS 與 BFS 的時間複雜度都一樣是 `O(N)`，因為兩者都必須拜訪所有節點。

空間複雜度方面，DFS 是 `O(H)`，BFS 是 `O(D)`，其中 `H` 是樹高，`D` 是樹的直徑。兩者在最壞情況下都可能達到 `O(N)` 空間：DFS 對應到斜樹，BFS 對應到完全二元樹。

對這一題而言，BFS 更合適，所以我們就藉這個機會來看看三種使用佇列的 BFS 實作方式。

BFS 實作

這三種實作都以標準 BFS 的方式使用佇列：

把根節點放入佇列。

從左側取出一個節點。

先把左子節點放入佇列，再把右子節點放入佇列。

diff

三種 BFS 方法

差別在於如何找到每一層的結尾，也就是最右邊的元素：

使用兩個佇列，一個存前一層，一個存目前這一層。

使用一個佇列，再加上一個哨兵值來標記層的結尾。

使用一個佇列，再搭配每層大小的測量。



方法 1：BFS：兩個佇列
我們使用兩個佇列：一個給目前這一層，另一個給下一層。想法是從目前層的佇列中逐一取出節點，並把它們的子節點放入下一層的佇列中。每當目前層的佇列空了，我們手上的節點就是右側視圖中的元素。

diff

演算法

建立右側視圖清單 `rightside`。

建立兩個佇列：一個給目前層，一個給下一層。先把 `root` 放入 `nextLevel` 佇列。

當 `nextLevel` 佇列不為空時：

初始化目前層：`currLevel = nextLevel`，並清空下一層 `nextLevel`。

當目前層佇列不為空時：

從目前層佇列取出一個節點。

先把左子節點加入 `nextLevel`，再把右子節點加入 `nextLevel`。

當 `currLevel` 為空時，我們手上的節點就是這一層最後一個節點，因此它屬於右側視圖，把它加入 `rightside`。

回傳 `rightside`。

實作


複雜度分析

時間複雜度：`O(N)`，因為每個節點都需要被拜訪一次。

空間複雜度：`O(D)`，用來維持佇列，其中 `D` 是樹的直徑。可以用最後一層來估計佇列大小；在完全二元樹的情況下，這一層最多可能包含 `N/2` 個節點。



方法 2：BFS：一個佇列 + 哨兵
另一種做法是把所有節點都放進同一個佇列，並使用一個哨兵節點來分隔各層。通常可以用 `null` 當作哨兵。

diff

第一步是初始化第一層：`root + null` 作為哨兵。完成後，就持續從左側逐一取出節點，並把它們的子節點加入右側。每次當目前節點是 `null` 時就停止，因為這表示我們碰到了目前層的結尾。每次停下來時，就要更新右側視圖清單，並把 `null` 放回佇列中，用來標記下一層的結尾。

演算法

建立右側視圖清單 `rightside`。

初始化佇列，先加入根節點。再加入 `null` 哨兵，用來標記第一層的結尾。

把目前節點初始化為根節點。

當佇列不為空時：

先保存前一個節點 `prev = curr`，再從佇列中取出目前節點 `curr = queue.poll()`。

當目前節點不是 `null` 時：

先把左子節點加入佇列，再把右子節點加入佇列。

更新前一個節點與目前節點：`prev = curr`，`curr = queue.poll()`。

當目前節點變成 `null` 時，表示我們已經走到這一層的結尾。因此前一個節點就是最右邊的節點，把它加入 `rightside`。

如果佇列還不為空，就把 `null` 節點加入佇列，作為下一層結尾的哨兵。

回傳 `rightside`。

實作

注意，Java 的 `ArrayDeque` 不支援 `null` 元素，因此這裡要使用的資料結構是 `LinkedList`。


複雜度分析

時間複雜度：`O(N)`，因為每個節點都需要被拜訪一次。

空間複雜度：`O(D)`，用來維持佇列，其中 `D` 是樹的直徑。可以用最後一層來估計佇列大小；在完全二元樹的情況下，這一層最多可能包含 `N/2` 個節點。



方法 3：BFS：一個佇列 + 每層大小測量
除了使用哨兵之外，我們也可以直接記錄目前這一層的長度。

diff

演算法

建立右側視圖清單 `rightside`。

初始化佇列，先加入根節點。

當佇列不為空時：

記錄目前層的長度：`levelLength = queue.size()`。

讓 `i` 從 `0` 走到 `levelLength - 1`：

從佇列中取出目前節點：`node = queue.poll()`。

如果 `i == levelLength - 1`，表示它是目前層最後一個節點，就把它加入 `rightside` 清單。

先把左子節點加入佇列，再把右子節點加入佇列。

回傳 `rightside`。

實作


複雜度分析

時間複雜度：`O(N)`，因為每個節點都需要被拜訪一次。

空間複雜度：`O(D)`，用來維持佇列，其中 `D` 是樹的直徑。可以用最後一層來估計佇列大小；在完全二元樹的情況下，這一層最多可能包含 `N/2` 個節點。



方法 4：遞迴 DFS
大家都喜歡遞迴 DFS，所以這裡也補上這種方法。概念很簡單：每次都先從最右邊的子節點開始，逐層走訪整棵樹。

實作


複雜度分析

時間複雜度：`O(N)`，因為每個節點都需要被拜訪一次。

空間複雜度：`O(H)`，用來維持遞迴呼叫堆疊，其中 `H` 是樹高。最壞情況是樹呈現斜樹，此時 `H = N`。
