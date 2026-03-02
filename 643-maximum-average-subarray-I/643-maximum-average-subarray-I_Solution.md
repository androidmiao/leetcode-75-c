Approach #1 Cumulative Sum [Accepted]
Algorithm

We know that in order to obtain the averages of subarrays with length k, we need to obtain the sum of these k length subarrays. One of the methods of obtaining this sum is to make use of a cumulative sum array, sum, which is populated only once. Here, sum[i] is used to store the sum of the elements of the given nums array from the first element upto the element at the i 
th
  index.

Once the sum array has been filled up, in order to find the sum of elements from the index i to i+k, all we need to do is to use: sum[i]−sum[i−k]. Thus, now, by doing one more iteration over the sum array, we can determine the maximum average possible from the subarrays of length k.

The following animation illustrates the process for a simple example.

Current


Complexity Analysis

Time complexity : O(n). We iterate over the nums array of length n once to fill the sum array. Then, we iterate over n−k elements of sum to determine the required result.

Space complexity : O(n). We make use of a sum array of length n to store the cumulative sum.

Approach #2 Sliding Window [Accepted]
Algorithm

Instead of creating a cumulative sum array first, and then traversing over it to determine the required sum, we can simply traverse over nums just once, and on the go keep on determining the sums possible for the subarrays of length k. To understand the idea, assume that we already know the sum of elements from index i to index i+k, say it is x.

Now, to determine the sum of elements from the index i+1 to the index i+k+1, all we need to do is to subtract the element nums[i] from x and to add the element nums[i+k+1] to x. We can carry out our process based on this idea and determine the maximum possible average.


Complexity Analysis

Time complexity : O(n). We iterate over the given nums array of length n once only.

Space complexity : O(1). Constant extra space is used.


---
方法一：前綴和（Cumulative Sum）[Accepted]
演算法

我們知道，要取得長度為 `k` 的子陣列平均值，先要取得這些長度為 `k` 的子陣列總和。計算這些總和的一種方式，是使用只建立一次的前綴和陣列 `sum`。其中，`sum[i]` 用來儲存給定陣列 `nums` 從第一個元素到第 `i` 個索引為止的元素總和。

當 `sum` 陣列填好之後，若要計算索引區間 `i` 到 `i+k` 的元素總和，只需要使用：`sum[i] - sum[i-k]`。因此，再對 `sum` 陣列做一次迭代，就能找出所有長度為 `k` 的子陣列中，可能的最大平均值。

下方動畫展示了一個簡單範例的流程。

Current

複雜度分析

時間複雜度：`O(n)`。我們先對長度為 `n` 的 `nums` 迭代一次來填入 `sum` 陣列，接著再對 `sum` 的 `n-k` 個元素迭代，以求得結果。

空間複雜度：`O(n)`。使用了一個長度為 `n` 的 `sum` 陣列來儲存前綴和。

方法二：滑動視窗（Sliding Window）[Accepted]
演算法

另一種方式是不先建立前綴和陣列再遍歷，而是直接遍歷 `nums` 一次，並在過程中持續計算長度為 `k` 的子陣列總和。為了理解這個想法，假設我們已經知道索引 `i` 到索引 `i+k` 的元素總和為 `x`。

現在，若要得到索引 `i+1` 到索引 `i+k+1` 的總和，只需要從 `x` 減去 `nums[i]`，再加上 `nums[i+k+1]`。根據這個想法持續更新，就能找出最大可能平均值。

複雜度分析

時間複雜度：`O(n)`。只需對長度為 `n` 的 `nums` 陣列迭代一次。

空間複雜度：`O(1)`。僅使用常數額外空間。
