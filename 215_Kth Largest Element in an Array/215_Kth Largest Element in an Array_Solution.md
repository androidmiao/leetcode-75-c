Approach 1: Sort
Intuition

Sort the array in descending order and then return the k 
th
  element. Note that this is the "trivial" approach and if asked this question in an interview, you would be expected to come up with a better solution than this.

Implementation

Note: k is 1-indexed, not 0-indexed. As such, we need to return the element at index k - 1 after sorting, not index k.


Complexity Analysis

Given n as the length of nums,

Time complexity: O(n⋅logn)

Sorting nums requires O(n⋅logn) time.

Space Complexity: O(logn) or O(n)

The space complexity of the sorting algorithm depends on the implementation of each programming language:

In Java, Arrays.sort() for primitives is implemented using a variant of the Quick Sort algorithm, which has a space complexity of O(logn)
In C++, the sort() function provided by STL uses a hybrid of Quick Sort, Heap Sort, and Insertion Sort, with a worst-case space complexity of O(logn)
In Python, the sort() function is implemented using the Timsort algorithm, which has a worst-case space complexity of O(n)

Approach 2: Min-Heap
Intuition

A heap is a very powerful data structure that allows us to efficiently find the maximum or minimum value in a dynamic dataset.

If you are not familiar with heaps, we recommend checking out the Heap Explore Card.

The problem is asking for the k 
th
  largest element. Let's push all the elements onto a min-heap, but pop from the heap when the size exceeds k. When we pop, the smallest element is removed. By limiting the heap's size to k, after handling all elements, the heap will contain exactly the k largest elements from the array.



It is impossible for one of the green elements to be popped because that would imply there are at least k elements in the array greater than it. This is because we only pop when the heap's size exceeds k, and popping removes the smallest element.

After we handle all the elements, we can just check the top of the heap. Because the heap is holding the k largest elements and the top of the heap is the smallest element, the top of the heap would be the k 
th
  largest element, which is what the problem is asking for.

Algorithm

Initialize a min-heap heap.
Iterate over the input. For each num:
Push num onto the heap.
If the size of heap exceeds k, pop from heap.
Return the top of the heap.
Implementation

Note: C++ std::priority_queue implements a max-heap. To achieve min-heap functionality, we will multiply the values by -1 before pushing them onto the heap.


Complexity Analysis

Given n as the length of nums,

Time complexity: O(n⋅logk)

Operations on a heap cost logarithmic time relative to its size. Because our heap is limited to a size of k, operations cost at most O(logk). We iterate over nums, performing one or two heap operations at each iteration.

We iterate n times, performing up to logk work at each iteration, giving us a time complexity of O(n⋅logk).

Because k≤n, this is an improvement on the previous approach.

Space complexity: O(k)

The heap uses O(k) space.


Approach 3: Quickselect
Intuition

This is a more advanced/esoteric algorithm. Do not feel discouraged if you are unable to derive it yourself. It is highly unlikely that you would be expected to come up with this solution in an interview without any help from the interviewer.

Quickselect, also known as Hoare's selection algorithm, is an algorithm for finding the k 
th
  smallest element in an unordered list. It is significant because it has an average runtime of O(n).

Quickselect uses the same idea as Quicksort. First, we choose a pivot index. The most common way to choose the pivot is randomly. We partition nums into 3 sections: elements equal to the pivot, elements greater than the pivot, and elements less than the pivot.

Next, we count the elements in each section. Let's denote the sections as follows:

left is the section with elements less than the pivot
mid is the section with elements equal to the pivot
right is the section with elements greater than the pivot
Quickselect is normally used to find the k 
th
  smallest element, but we want the k 
th
  largest. To account for this, we will swap what left and right represent - left will be the section with elements greater than the pivot, and right will be the section with elements less than the pivot.

If the number of elements in left is greater than or equal to k, the answer must be in left - any other element would be less than the k 
th
  largest element. We restart the process in left.



If the number of elements in left and mid is less than k, the answer must be in right - any element in left or mid would not be large enough to be the k 
th
  largest element. We restart the process in right.



There's one extra step if the answer is in right. When we go to search in right, we are effectively "deleting" the elements in left and mid (since they will never be considered again). Because the elements in left and mid are greater than the answer, deleting them means we must shift k. Let's say we're looking for the 8 
th
  greatest element, but then we delete the 4 greatest elements. In the remaining data, we would be looking for the 4 
th
  greatest element, not the 8 
th
 . Therefore, we need to subtract the length of left and mid from k when we search for right.

We don't need to modify k when we search in left because when we search in left, we delete elements smaller than the answer, which doesn't affect k.



If the answer is in neither left or right, it must be in mid. Since mid only has elements equal to the pivot, the pivot must be the answer.

The easiest way to implement this repetitive process is by using recursion.

Algorithm

Note: the implementation we use here is not a standard Quickselect implementation. We will be using slightly more space (still the same complexity), but in exchange, we will be writing significantly less code.

Define a quickSelect function that takes arguments nums and k. This function will return the k 
th
  greatest element in nums (the nums and k given to it as input, not the original nums and k).

Select a random element as the pivot.
Create left, mid, and right as described above.
If k <= left.length, return quickSelect(left, k).
If left.length + mid.length < k, return quickSelect(right, k - left.length - mid.length).
Otherwise, return pivot.
Call quickSelect with the original nums and k, and return the answer.

Implementation


Complexity Analysis

Given n as the length of nums,

Time complexity: O(n) on average, O(n 
2
 ) in the worst case

Each call we make to quickSelect will cost O(n) since we need to iterate over nums to create left, mid, and right. The number of times we call quickSelect is dependent on how the pivots are chosen. The worst pivots to choose are the extreme (greatest/smallest) ones because they reduce our search space by the least amount. Because we are randomly generating pivots, we may end up calling quickSelect O(n) times, leading to a time complexity of O(n 
2
 ).

However, the algorithm mathematically almost surely has a linear runtime. For any decent size of nums, the probability of the pivots being chosen in a way that we need to call quickSelect O(n) times is so low that we can ignore it.

On average, the size of nums will decrease by a factor of ~2 on each call. You may think: that means we call quickSelect O(logn) times, wouldn't that give us a time complexity of O(n⋅logn)? Well, each successive call to quickSelect would also be on a nums that is a factor of ~2 smaller. This recurrence can be analyzed using the master theorem with a = 1, b = 2, k = 1:

T(n)=T( 
2
n
​
 )+O(n)=O(n)

Space complexity: O(n)

We need O(n) space to create left, mid, and right. Other implementations of Quickselect can avoid creating these three in memory, but in the worst-case scenario, those implementations would still require O(n) space for the recursion call stack.


Bonus

When we randomly choose pivots, Quickselect has a worst-case scenario time complexity of O(n 
2
 ).

By using the median of medians algorithm, we can improve to a worst-case scenario time complexity of O(n).

This approach is way out of scope for an interview, and practically it isn't even worth implementing because there is a large constant factor. As stated above, the random pivot approach will yield a linear runtime with mathematical certainty, so in all practical scenarios, it is sufficient.

The median of medians approach should only be appreciated for its theoretical beauty. Those who are interested can read more using the link above.

Approach 4: Counting Sort
Intuition

Counting sort is a non-comparison sorting algorithm. It can be used to sort an array of positive integers.

In this approach, we will sort the input using a slightly modified counting sort, and then return the k 
th
  element from the end (just like in the first approach).

Here is how we will sort the array:

First, find the maxValue in the array. Create an array count with size maxValue + 1.
Iterate over the array and find the frequency of each element. For each element num, increment count[num].
Now we know the frequency of each element. Each index of count represents a number. Create a new array sortedArr and iterate over count. For each index num, add count[num] copies of num to sortedArr. Because we iterate over the indices in sorted order, we will also add elements to sortedArr in sorted order.
The following animation demonstrates this process:

Current


There is one problem: as we are associating indices with numbers, this algorithm will not work if negative numbers are in the input. The constraints state that negative numbers can be in the input, so we need to account for this.

Let's also find minValue, the minimum value in the array. Instead of count having a size of maxValue + 1, we will make it have a size of maxValue - minValue + 1 (if minValue < 0, then this will appropriately increase the size of count).

Now, we can just apply an offset of minValue when mapping numbers to indices and vice-versa. When we populate count, given a num we will increment count[num - minValue]. count[num] will represent the frequency of num + minValue.

One more small optimization

Since we don't actually need to sort the array but only need to return the k 
th
  largest value, we will iterate over the indices of count in reverse order (get the larger numbers first). At each number, we will decrement k (or a variable we initialize as remain = k to avoid modifying the input) by the frequency. If remain <= 0, we can return the current number. With this optimization, we don't need to create sortedArr, saving us O(n) space.

Algorithm

Find the maxValue and minValue of nums.
Create count, an array of size maxValue - minValue + 1.
Iterate over nums. For each num, increment count[num - minValue].
Set remain = k and iterate over the indices of count backward. For each index num:
Subtract the frequency of num from remain: remain -= count[num].
If remain <= 0, return the number that the current index represents: num + minValue.
The code should never reach this point. Return any value.
Implementation


Complexity Analysis

Given n as the length of nums and m as maxValue - minValue,

Time complexity: O(n+m)

We first find maxValue and minValue, which costs O(n).

Next, we initialize count, which costs O(m).

Next, we populate count, which costs O(n).

Finally, we iterate over the indices of count, which costs up to O(m).

Space complexity: O(m)

We create an array count with size O(m).


---

方法 1：排序
直覺

將陣列依照遞減順序排序，然後回傳第 `k` 個元素。請注意，這是最「直觀」的做法；如果這題出現在面試中，通常會期待你提出比這個更好的解法。

實作

注意：`k` 是從 1 開始編號，不是從 0 開始。因此排序後我們要回傳索引 `k - 1` 的元素，而不是索引 `k`。


複雜度分析

設 `n` 為 `nums` 的長度。

時間複雜度：`O(n⋅logn)`

對 `nums` 排序需要 `O(n⋅logn)` 的時間。

空間複雜度：`O(logn)` 或 `O(n)`

排序演算法的空間複雜度會依各程式語言的實作而不同：

在 Java 中，處理基本型別的 `Arrays.sort()` 採用 Quick Sort 的變形，空間複雜度為 `O(logn)`
在 C++ 中，STL 提供的 `sort()` 使用 Quick Sort、Heap Sort 與 Insertion Sort 的混合實作，最壞情況下空間複雜度為 `O(logn)`
在 Python 中，`sort()` 採用 Timsort，最壞情況下空間複雜度為 `O(n)`

方法 2：最小堆積
直覺

堆積（heap）是一種非常強大的資料結構，可以讓我們在動態資料集中有效率地找出最大值或最小值。

如果你對 heap 不熟，建議先參考 Heap Explore Card。

題目要求第 `k` 大元素。做法是把所有元素放進最小堆積，但當堆積大小超過 `k` 時就彈出頂端元素。每次彈出時，會移除目前最小的元素。由於我們把 heap 的大小限制在 `k`，當所有元素都處理完後，heap 內就會剛好保留陣列中最大的 `k` 個元素。



綠色區域中的元素不可能被彈出，因為那代表陣列中至少有 `k` 個元素比它更大。這是因為我們只有在 heap 大小超過 `k` 時才會彈出元素，而彈出的永遠是最小值。

當所有元素處理完成後，只要查看 heap 頂端即可。因為 heap 中存放的是最大的 `k` 個元素，而 heap 頂端又是其中最小的那個，所以 heap 頂端就是第 `k` 大元素，也正是題目要求的答案。

演算法

初始化一個最小堆積 `heap`。
遍歷輸入資料。對於每個 `num`：
將 `num` 放入 heap。
如果 `heap` 的大小超過 `k`，就從 heap 中彈出一個元素。
回傳 heap 的頂端元素。
實作

注意：C++ 的 `std::priority_queue` 預設是最大堆積。若要達成最小堆積的效果，我們會先把數值乘上 `-1` 再放入 heap。


複雜度分析

設 `n` 為 `nums` 的長度。

時間複雜度：`O(n⋅logk)`

heap 的操作時間與其大小呈對數關係。因為 heap 的大小最多只會是 `k`，所以每次操作的成本最多為 `O(logk)`。我們遍歷 `nums`，每次迭代會進行一到兩次 heap 操作。

總共迭代 `n` 次，每次最多做 `logk` 的工作，因此時間複雜度為 `O(n⋅logk)`。

因為 `k ≤ n`，所以這比前一種做法更有效率。

空間複雜度：`O(k)`

heap 需要 `O(k)` 的額外空間。


方法 3：Quickselect
直覺

這是一個較進階、也較偏技巧型的演算法。如果你沒辦法自行推導出來，不需要氣餒。在面試中，如果沒有面試官提示，通常也不太可能被期待直接想出這個解法。

Quickselect，也稱為 Hoare 的 selection algorithm，是一種用來在無序串列中找出第 `k` 小元素的演算法。它的重要之處在於其平均執行時間為 `O(n)`。

Quickselect 和 Quicksort 採用相同的核心概念。首先，我們會選一個 pivot 索引。最常見的方式是隨機選 pivot。接著將 `nums` 分成三個區段：等於 pivot 的元素、大於 pivot 的元素，以及小於 pivot 的元素。

接著，我們統計每個區段的元素數量。以下用這些名稱表示：

`left` 是小於 pivot 的區段
`mid` 是等於 pivot 的區段
`right` 是大於 pivot 的區段
一般 Quickselect 用來找第 `k` 小元素，但這題要求第 `k` 大元素。因此我們把 `left` 和 `right` 的意義對調，也就是說：`left` 改成存放大於 pivot 的元素，`right` 改成存放小於 pivot 的元素。

如果 `left` 中的元素數量大於等於 `k`，那答案一定在 `left` 裡，因為其他任何元素都會比第 `k` 大元素更小。我們就在 `left` 中重新進行同樣的流程。



如果 `left` 和 `mid` 的元素總數小於 `k`，那答案一定在 `right` 裡，因為 `left` 和 `mid` 中的元素都還不夠小，無法對應到第 `k` 大的位置。我們就在 `right` 中重新開始搜尋。



如果答案落在 `right`，還要多做一步。當我們進入 `right` 搜尋時，等於把 `left` 和 `mid` 中的元素「刪除」了，因為之後不會再考慮它們。由於 `left` 和 `mid` 的元素都比答案大，因此刪掉它們後，`k` 也必須跟著平移。假設我們原本要找第 `8` 大元素，但現在刪掉了前面 `4` 個更大的元素，那在剩下的資料中，我們實際上是在找第 `4` 大，而不是第 `8` 大。因此，進入 `right` 搜尋時，要把 `k` 減去 `left` 和 `mid` 的長度。

而當我們進入 `left` 搜尋時，不需要修改 `k`，因為被刪掉的是那些比答案小的元素，這不會影響第 `k` 大的相對位置。



如果答案既不在 `left` 也不在 `right`，那它一定在 `mid`。由於 `mid` 中的元素都等於 pivot，因此 pivot 就是答案。

實作這種重複流程最簡單的方式，就是使用遞迴。

演算法

注意：這裡使用的實作不是標準版 Quickselect。我們會多使用一些空間（但整體複雜度不變），換來的是大幅減少程式碼量。

定義一個 `quickSelect` 函式，參數為 `nums` 和 `k`。這個函式會回傳輸入給它的 `nums` 中第 `k` 大的元素（是傳入該函式的 `nums` 與 `k`，不是最原始的那份）。

隨機選一個元素作為 pivot。
建立如上所述的 `left`、`mid`、`right`。
如果 `k <= left.length`，回傳 `quickSelect(left, k)`。
如果 `left.length + mid.length < k`，回傳 `quickSelect(right, k - left.length - mid.length)`。
否則，回傳 pivot。
以原始的 `nums` 和 `k` 呼叫 `quickSelect`，並回傳答案。

實作


複雜度分析

設 `n` 為 `nums` 的長度。

時間複雜度：平均為 `O(n)`，最壞情況為 `O(n^2)`

每次呼叫 `quickSelect` 都需要 `O(n)` 的成本，因為我們必須遍歷 `nums` 來建立 `left`、`mid`、`right`。而會呼叫多少次 `quickSelect`，取決於 pivot 的選法。最差的 pivot 是極端值（最大或最小），因為它們幾乎無法有效縮小搜尋範圍。由於我們是隨機選 pivot，因此有可能會連續做 `O(n)` 次呼叫，使整體時間複雜度變成 `O(n^2)`。

不過，從數學上來看，這個演算法幾乎必然會呈現線性時間。對於任何稍有規模的 `nums` 而言，pivot 一直選得很差、導致需要呼叫 `quickSelect` 共 `O(n)` 次的機率極低，因此實務上可以忽略。

平均來說，每次呼叫後 `nums` 的大小大約會縮小成原本的 `~2` 分之 `1`。你可能會想：那不就是呼叫 `quickSelect` `O(logn)` 次，時間複雜度應該是 `O(n⋅logn)` 嗎？但問題在於，每一次後續呼叫所處理的 `nums` 也會縮小約一半。這個遞迴關係可用 master theorem 分析，其中 `a = 1`、`b = 2`、`k = 1`：

`T(n)=T(n/2)+O(n)=O(n)`

空間複雜度：`O(n)`

我們需要 `O(n)` 的空間來建立 `left`、`mid`、`right`。其他 Quickselect 實作可以避免在記憶體中建立這三個陣列，但在最壞情況下，那些做法仍然會因遞迴呼叫堆疊而需要 `O(n)` 空間。


補充

當我們使用隨機 pivot 時，Quickselect 的最壞情況時間複雜度是 `O(n^2)`。

若改用 median of medians 演算法，則可以把最壞情況的時間複雜度改善到 `O(n)`。

不過這個方法在面試中遠遠超出範圍，而且在實務上通常也不值得實作，因為常數成本很大。正如前面所說，隨機 pivot 的做法在數學上幾乎必然會得到線性時間，因此在實際情境中已經足夠。

median of medians 這個方法更多是體現在理論上的優美性。有興趣的人可以透過上方連結進一步閱讀。

方法 4：計數排序
直覺

計數排序是一種非比較式排序演算法，可用於排序正整數陣列。

在這個方法中，我們會使用稍作修改的計數排序來排序輸入，然後回傳從尾端數來第 `k` 個元素（和第一種方法相同）。

排序流程如下：

先找出陣列中的 `maxValue`。建立一個大小為 `maxValue + 1` 的陣列 `count`。
遍歷陣列並統計每個元素出現的頻率。對於每個元素 `num`，執行 `count[num]++`。
現在我們知道每個元素的出現次數了。`count` 的每個索引代表一個數值。接著建立新陣列 `sortedArr`，並遍歷 `count`。對每個索引 `num`，把 `count[num]` 個 `num` 放進 `sortedArr`。由於我們是依照索引遞增順序遍歷，因此放進 `sortedArr` 的元素也會是有序的。
下方動畫展示了這個過程：

Current


但這裡有一個問題：因為我們是把索引對應到數值，如果輸入中含有負數，這個演算法就無法直接運作。題目的限制條件明確表示輸入可能包含負數，因此我們必須處理這件事。

我們再找出 `minValue`，也就是陣列中的最小值。此時 `count` 的大小不再是 `maxValue + 1`，而是 `maxValue - minValue + 1`。如果 `minValue < 0`，這樣的寫法就能正確把範圍擴充進去。

接著，只要在數值和索引之間加入 `minValue` 的偏移量即可。當我們填入 `count` 時，對於某個 `num`，實際上更新的是 `count[num - minValue]`。因此 `count[i]` 代表的是數值 `i + minValue` 的出現頻率。

再做一個小優化

因為我們其實不需要真的把整個陣列排好序，只需要回傳第 `k` 大的值，所以我們可以反向遍歷 `count` 的索引（先處理較大的數）。在每個數值位置，把 `k` 減去它的出現次數，或者用一個 `remain = k` 的變數來避免修改原輸入。如果 `remain <= 0`，就表示目前這個數就是答案。透過這個優化，我們不需要建立 `sortedArr`，因此可節省 `O(n)` 的空間。

演算法

找出 `nums` 中的 `maxValue` 和 `minValue`。
建立 `count`，大小為 `maxValue - minValue + 1`。
遍歷 `nums`。對於每個 `num`，執行 `count[num - minValue]++`。
設定 `remain = k`，然後反向遍歷 `count` 的索引。對於每個索引 `num`：
用該數值的頻率更新 `remain`：`remain -= count[num]`。
如果 `remain <= 0`，回傳目前索引所代表的數值，也就是 `num + minValue`。
理論上程式不應該執行到這裡；隨便回傳任意值即可。
實作


複雜度分析

設 `n` 為 `nums` 的長度，`m` 為 `maxValue - minValue`。

時間複雜度：`O(n+m)`

首先我們找 `maxValue` 和 `minValue`，成本是 `O(n)`。

接著初始化 `count`，成本是 `O(m)`。

然後填入 `count`，成本是 `O(n)`。

最後反向遍歷 `count` 的索引，最多需要 `O(m)`。

空間複雜度：`O(m)`

我們建立了一個大小為 `O(m)` 的 `count` 陣列。
