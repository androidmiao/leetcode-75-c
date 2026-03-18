Overview
We are given an array of integers nums and there is a sliding window of size k which is moving from the left of the array to the right.

Our task is to return a list of integers that contains the largest element from each window.

Approach: Monotonic Deque

Intuition
An intuitive way to solve the problem is to iterate over all windows and then iterate over all the elements in a window to find the largest element. There are a total of n - k + 1 (where n is the size of nums) windows and it would take O(k) to find the largest element from each window. This strategy is too slow and would result in TLE.

Let's make some observations and reduce the time complexity.

We may observe that in a window, the elements that come before the largest element will never be selected as the largest element of any future windows. For example, consider a window [1, 2, 3, 4, 1]. Because the window is sliding left to right, any window with the first three elements 1, 2, and 3 would also have the 4.

However, we cannot ignore the items that follow the largest element. If we use the above example, we cannot ignore the last element 1 since there may be a window from the fourth index (0-based indexing) until the eighth index where 1 is the largest element. The elements at indices 3 and 4 are those that will be "useful" in the following windows.

Therefore, we can discard the first three elements and only take into account the elements at indices 3 and 4.

Now, let's consider the next element and call it x. We need to add x to the window to consider the next window. If x > 1, then we can now discard the 1 because x will be in any future windows that 1 is in. If x > 4, we can discard the 4 as well for the same reason.

In general, whenever we encounter a new element x, we want to discard all elements that are less than x before adding x. Let's say we currently have [63, 15, 8, 3] and we encounter 12. Any future window with 8 or 3 will also contain 12, so we can discard them. After discarding them and adding 12, we have [63, 15, 12]. As you can see, we keep elements in descending order.

To perform these operations, we can use a monotonic queue as it supports efficient insertion, deletion, and retrieval of elements from the ends of a window. We will implement it with the deque data structure.

A monotonic data structure is one where the elements are always sorted. In our case, we want a monotonic decreasing queue, which means that the elements in the queue are always sorted descending. When we want to add a new element x, we maintain the monotonic property by removing all elements less than x before adding x.

We initialize a deque of integers dq. It will contain the indices of the "useful" elements in the current window. The reason we need to store the indices instead of the elements themselves is that we need to detect when elements leave the window due to sliding too far to the right.

We also initialize an array of integers res to store the answer.

By maintaining the monotonic decreasing property, the largest element in the window must always be the first element in the deque, which is nums[dq[0]].

We initialize the first window with the first k elements. Then we iterate over the indices i in the range [k, n - 1], and for each element, we add its index to dq while maintaining the monotonic property. We also remove the first element in dq if it is too far to the left (dq[0] = i - k). After these operations, dq will correctly hold the indices of all useful elements in the current window in decreasing order. Thus, we can push nums[dq[0]] to the answer.

Here's an animation visually showing how the approach works:

Current

You can see that the deque's size never goes above k at any time. This is because we are only ever taking into account the "useful" elements of the current window.

There can be k indices in the deque when all the elements of a window are sorted in descending order. If the elements are not sorted, there will be some useless elements and the size of the deque would be less than k.

Since we consistently add the largest index to the end of the deque and remove some other indices that correspond to useless elements, we are essentially storing indices in ascending order. When taking into account a window with the last element at index i, all of the deque's elements are greater than i - k. As a result, the size of deque can never exceed k.

Algorithm
Create a deque dq of integers.
Create a list of integers res to store the answer.
Iterate over the first k elements from i = 0 to k - 1 and perform the following:
While dq is not empty and the current element nums[i] is greater or equal to nums[dq.peekLast()], continue to pop the last element.
Push i at the end of dq.
Push the largest element of the first window nums[dq.peekFirst()] to the answer.
We iterate over all the remaining elements from i = k to n - 1 to move to the next windows. We perform the following in this loop:
Check if the element at the front of dq is equal to i - k. If it is equal to i - k, it cannot be included in the current window. We pop this element.
While dq is not empty and the current element nums[i] is greater or equal to nums[dq.back()], continue to pop the last element.
Push i at the end of dq.
Push the largest element of the current window nums[dq.peekFirst()] to the answer.
Return res.

Implementation

Complexity Analysis
Here n is the size of nums.

Time complexity: O(n).

At first glance, it may look like the time complexity of this algorithm should be O(n^2), because there is a nested while loop inside the for loop. However, each element can only be added to the deque once, which means the deque is limited to n pushes. Every iteration of the while loop uses 1 pop, which means the while loop will not iterate more than n times in total, across all iterations of the for loop.

An easier way to think about this is that in the worst case, every element will be pushed and popped once. This gives a time complexity of O(2 * n) = O(n).

Space complexity: O(k).

The size of the deque can grow a maximum up to a size of k.


---
概述
我們會得到一個整數陣列 `nums`，以及一個大小為 `k` 的滑動視窗，這個視窗會從陣列左邊一路移動到右邊。

我們的任務是回傳一個整數列表，其中包含每一個視窗中的最大元素。

方法：單調雙端佇列

直覺
一個很直覺的解法，是枚舉所有視窗，再枚舉每個視窗中的所有元素，找出其中最大值。總共有 `n - k + 1` 個視窗（其中 `n` 是 `nums` 的大小），而每個視窗找最大值需要 `O(k)`。這個策略太慢，會導致 TLE。

接著我們做一些觀察，來降低時間複雜度。

我們可以觀察到，在一個視窗中，出現在最大元素之前的那些元素，未來都不可能再成為任何後續視窗的最大元素。舉例來說，考慮一個視窗 `[1, 2, 3, 4, 1]`。因為視窗是從左往右滑動，所以任何還包含前面三個元素 `1`、`2`、`3` 的未來視窗，也一定會包含 `4`。

不過，我們不能忽略最大元素之後的那些元素。以上面的例子來說，最後那個 `1` 仍然不能忽略，因為可能存在一個從第 4 個索引（以 0 為起始索引）到第 8 個索引的視窗，在那個視窗中，`1` 可能會是最大元素。索引 `3` 和 `4` 的元素，才是後續視窗中仍然「有用」的元素。

因此，我們可以捨棄前面三個元素，只保留索引 `3` 和 `4` 的元素來考慮。

現在再考慮下一個元素，稱它為 `x`。為了形成下一個視窗，我們需要把 `x` 加進來。如果 `x > 1`，那麼原本那個 `1` 就可以丟掉，因為凡是未來包含 `1` 的視窗，也一定會包含 `x`。如果 `x > 4`，那 `4` 也同樣可以丟掉，理由完全一樣。

更一般地說，每當遇到一個新元素 `x`，在把它加入之前，我們都希望先把所有小於 `x` 的元素丟掉。假設目前我們有 `[63, 15, 8, 3]`，接著遇到 `12`。任何未來包含 `8` 或 `3` 的視窗，也一定會包含 `12`，所以可以把它們捨棄。捨棄之後再加入 `12`，就會變成 `[63, 15, 12]`。可以看到，我們始終讓元素維持遞減順序。

為了有效完成這些操作，我們可以使用單調佇列，因為它支援從視窗兩端高效率地插入、刪除與取得元素。我們會用 `deque` 這個資料結構來實作。

單調資料結構，指的是其中元素始終保持排序的資料結構。在這題中，我們想要的是單調遞減佇列，也就是佇列中的元素永遠依照遞減順序排列。當我們想加入一個新元素 `x` 時，會先移除所有比 `x` 小的元素，來維持這個單調性。

我們初始化一個整數雙端佇列 `dq`。它裡面會存放目前視窗中那些「有用」元素的索引。我們之所以存索引而不是直接存元素值，是因為當視窗往右滑動太遠時，我們需要知道哪些元素已經離開視窗。

我們也會初始化一個整數陣列 `res`，用來存答案。

藉由維持單調遞減性，視窗中的最大元素一定會是 `deque` 的第一個元素，也就是 `nums[dq[0]]`。

我們先用前 `k` 個元素初始化第一個視窗。接著在索引範圍 `[k, n - 1]` 中迭代 `i`，對於每個元素，把它的索引加入 `dq`，同時維持單調性。如果 `dq` 的第一個元素太靠左，也就是已經不在視窗內（`dq[0] = i - k`），就把它移除。完成這些操作後，`dq` 就會正確保存目前視窗中所有有用元素的索引，而且依照遞減順序排列。因此，我們可以把 `nums[dq[0]]` 放進答案。

下面原本有一段動畫，用來視覺化展示這個方法如何運作：

Current

你可以看到，`deque` 的大小在任何時候都不會超過 `k`。這是因為我們只會保留目前視窗中那些「有用」的元素。

當一個視窗中的所有元素本來就已經是遞減排序時，`deque` 中可能會有 `k` 個索引。如果元素不是遞減排序，就會有一些沒用的元素被移除，因此 `deque` 的大小就會小於 `k`。

由於我們總是把較大的索引加到 `deque` 的尾端，並且移除一些對應沒用元素的索引，所以本質上我們存放的索引是遞增排序的。當我們考慮一個最後元素索引為 `i` 的視窗時，`deque` 裡的所有元素都會大於 `i - k`。因此，`deque` 的大小不可能超過 `k`。

演算法
建立一個整數雙端佇列 `dq`。
建立一個整數列表 `res` 用來存答案。
遍歷前 `k` 個元素，也就是從 `i = 0` 到 `k - 1`，並執行以下操作：
當 `dq` 非空，且目前元素 `nums[i]` 大於或等於 `nums[dq.peekLast()]` 時，持續彈出最後一個元素。
把 `i` 加到 `dq` 的尾端。
把第一個視窗的最大元素 `nums[dq.peekFirst()]` 加入答案。
接著遍歷剩下所有元素，也就是從 `i = k` 到 `n - 1`，以移動到下一個個視窗。在這個迴圈中執行以下操作：
檢查 `dq` 前端元素是否等於 `i - k`。如果等於 `i - k`，代表它已經不能包含在目前視窗內，將它彈出。
當 `dq` 非空，且目前元素 `nums[i]` 大於或等於 `nums[dq.back()]` 時，持續彈出最後一個元素。
把 `i` 加到 `dq` 的尾端。
把目前視窗的最大元素 `nums[dq.peekFirst()]` 加入答案。
回傳 `res`。

實作

複雜度分析
這裡 `n` 是 `nums` 的大小。

時間複雜度：`O(n)`。

乍看之下，這個演算法的時間複雜度好像應該是 `O(n^2)`，因為在 `for` 迴圈裡面還有一個巢狀的 `while` 迴圈。不過，每個元素最多只會被加入 `deque` 一次，也就是說，整個過程最多只會有 `n` 次 push。每次 `while` 迴圈執行都會對應一次 pop，因此把所有 `for` 迴圈中的 `while` 執行次數加總起來，最多也只會有 `n` 次。

更簡單的理解方式是，在最差情況下，每個元素都只會被 push 一次、pop 一次。所以總操作量是 `O(2 * n)`，也就是 `O(n)`。

空間複雜度：`O(k)`。

`deque` 的大小最多會成長到 `k`。
