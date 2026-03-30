A word of advice
This problem is a companion problem to 295. Find Median From Data Stream. This means that a lot of approaches to solve this problem are based on the methods to solve 295. Find Median From Data Stream. Perhaps try that problem before you approach this one.

Solution
Approach 1: Simple Sorting
Intuition

Do what the question says.

Algorithm

Store the numbers in a window container of size k. The following operations must take place:

Inserting the incoming element.
Deleting the outgoing element.
Sorting the window to find the medians.
One primitive approach is to copy k consecutive elements from the input to the window and keep sorting these every time. This constitutes duplication of effort.

We can do a bit better if we instead insert and delete one element per window shift. The challenge then is to maintain the window as sorted, before and after the insert and delete operations.

Python comes with an excellent bisect module to help perform efficient insert operations on lists while maintaining their sorted property.

Complexity Analysis

Time complexity: O(n⋅k).

Copying elements into the container takes about O(k) time each. This happens about (n−k) times.

Bisected insertion or deletion takes about O(logk) for searching and O(k) for actual shifting of elements. This takes place about n times.

Space complexity: O(k) extra linear space for the window container.

Approach 2: Two Heaps (Lazy Removal)
Intuition

The idea is the same as Approach 3 from 295. Find Median From Data Stream. The only additional requirement is removing the outgoing elements from the window.

Since the window elements are stored in heaps, deleting elements that are not at the top of the heaps is a pain.

Some languages (like Java) provide implementations of the PriorityQueue class that allow for removing arbitrarily placed elements. Generally, using such features is not efficient nor is their portability assured.

Assuming that only the tops of heaps (and by extension the PriorityQueue class) are accessible, we need to find a way to efficiently invalidate and remove elements that are moving out of the sliding window.

At this point, an important thing to notice is the fact that if the two heaps are balanced, only the top of the heaps are actually needed to find the medians. This means that as long as we can somehow keep the heaps balanced, we could also keep some extraneous elements.

Thus, we can use hash-tables to keep track of invalidated elements. Once they reach the heap tops, we remove them from the heaps. This is the lazy removal technique.

An immediate challenge at this point is balancing the heaps while keeping extraneous elements. This is done by actually moving some elements to the heap which has extraneous elements, from the other heap. This cancels out the effect of having extraneous elements and maintains the invariant that the heaps are balanced.

NOTE: When we talk about keeping the heaps balanced, we are not referring to the actual heap sizes. We are only concerned with valid elements and hence when we talk about balancing heaps, we are referring to count of such elements.

Algorithm

Two priority queues:

A max-heap lo to store the smaller half of the numbers
A min-heap hi to store the larger half of the numbers
A hash-map or hash-table hash_table for keeping track of invalid numbers. It holds the count of the occurrences of all such numbers that have been invalidated and yet remain in the heaps.

The max-heap lo is allowed to store, at worst, one more element more than the min-heap hi. Hence if we have processed k elements:

If k=2⋅n+1(∀n∈Z), then lo is allowed to hold n+1 elements, while hi can hold n elements.
If k=2⋅n(∀n∈Z), then both heaps are balanced and hold n elements each.
This gives us the nice property that when the heaps are perfectly balanced, the median can be derived from the tops of both heaps. Otherwise, the top of the max-heap lo holds the legitimate median.

NOTE: As mentioned before, when we are talking about keeping the heaps balanced, the actual sizes of the heaps are irrelevant. Only the count of valid elements in both heaps matter.

Keep a balance factor. It indicates three situations:

balance =0: Both heaps are balanced or nearly balanced.
balance <0: lo needs more valid elements. Elements from hi are moved to lo.
balance >0: hi needs more valid elements. Elements from lo are moved to hi.
Inserting an incoming number in_num:

If in_num is less than or equal to the top element of lo, then it can be inserted to lo. However this unbalances hi (hi has lesser valid elements now). Hence balance is incremented.

Otherwise, in_num must be added to hi. Obviously, now lo is unbalanced. Hence balance is decremented.

Lazy removal of an outgoing number out_num:

If out_num is present in lo, then invalidating this occurrence will unbalance lo itself. Hence balance must be decremented.

If out_num is present in hi, then invalidating this occurrence will unbalance hi itself. Hence balance must be incremented.

We increment the count of this element in the hash_table table.

Once an invalid element reaches either of the heap tops, we remove them and decrement their counts in the hash_table table.

Complexity Analysis

Time complexity: O(nlogn)

Since hi and lo may grow up to O(n) elements due to the unbounded nature of their sizes in certain sequences (like a descending sequence), each insertion or removal operation in either heap takes O(logn) time. Over n steps, this results in a complexity of O(n \log n).

These still take O(1) time on average, but they don't change the overall time complexity, as O(nlogn) dominates.

Space complexity: O(n)+O(n−k)≈O(n) extra linear space.

The heaps collectively require O(n) space. Due to lazy removal, invalidated elements may remain in the heaps. For example, in a descending sequence, removed elements are never at the heap tops and accumulate, causing the heaps to grow up to O(n) in size.
The hash table needs about O(n−k) space.

Approach 3: Two Multisets
Intuition

One can see that multisets are a great way to keep elements sorted while providing efficient access to the first and last elements. Inserting and deleting arbitrary elements are also fairly efficient operations in a multiset. (Refer to Approach 4 Intuition for 295. Find Median From Data Stream)

Thus, if the previous approach gives you too much heartburn, consider replacing the use of priority_queue with multiset.

Algorithm

Inserting or deleting an element is straight-forward. Balancing the heaps takes the same route as Approach 3 of 295. Find Median From Data Stream.

Complexity Analysis

Time complexity: O((n−k)⋅6⋅logk)≈O(nlogk).

At worst, there are three set insertions and three set deletions from the start or end. Each of these takes about O(logk) time.
Finding the mean takes constant O(1) time since the start or ends of sets are directly accessible.
Each of these steps takes place about (n−k) times (the number of sliding window instances).
Space complexity: O(k) extra linear space to hold contents of the window.

Approach 4: Multiset and Two Pointers
Intuition

This is same as Approach 4 for 295. Find Median From Data Stream.

But, we don't actually need two pointers.

Median elements are derived using a single iterator position (when the window size k is odd) or two consecutive iterator positions (when k is even). Hence keeping track of only one pointer is sufficient. The other pointer can be implicitly derived when required.

Algorithm

A single iterator mid, which iterates over the window multiset. It is analogous to upper_median in Approach 4 for 295. Find Median From Data Stream. lower_median is implicitly derived from mid. It's either equal to mid (when the window size k is odd) or prev(mid) 1 (when the window size k is even).

We start with populating our multiset window with the first k elements. We set mid to the ⌊k/2⌋ indexed element in window (0-based indexing; Multisets always maintain their sorted property).

While inserting an element num into window, three cases arise:

num is less than the value of upper median mid.

num is greater than the value of upper median mid.

num is equal to the value of upper median mid. This situation is often handled as language-dependent. Since C++ multiset insert elements at the end of their equal range, this situation is essentially the same as the previous case.

For the first case, num is inserted before the upper median element mid. Thus mid now, no longer points to the ⌊k/2⌋ indexed element. In fact it points to the ⌊k/2⌋+1 indexed element. We fix that by decrementing mid.

For the second and third cases, num is inserted after the upper median element mid and hence does not spoil the mid iterator. It still points to the ⌊k/2⌋ indexed element.

Of course, the window size just increased to k+1 in all three cases. That will easily be fixed by removing the element that is about to exit the window.

While removing an element num, the same three cases arise as when we wanted to insert an element:

num is less than the value of upper median mid.

num is greater than the value of upper median mid.

num is equal to the value of upper median mid. Since mid will point to the first occurrence of num in the multiset window and we deterministically remove the first occurrence (take note that we use std::multiset::lower_bound() 2 to find the correct occurrence to erase), this case is handled in the same manner as the first case.

In the first and third cases, removing num will spoil the mid iterator. Thus we need to fix that by incrementing mid before we remove that element.

For the second case, the mid iterator is not spoiled. No further action is required.

Once this element has been removed, the window size returns to being k.

After insertion of the incoming element and removal of the outgoing element, we are left again with some nice invariants:

Window size is again k.
The window is still fully sorted.
mid still points to the ⌊k/2⌋ indexed element.
Finding the median of the window is easy! It is simply the mean of the elements pointed to by the two pointers lo_median and hi_median. In our case those are mid or prev(mid) (as decided by whether k is odd or even), and mid respectively.

Complexity Analysis

Time complexity: O((n−k)logk)+O(k)≈O(nlogk).

Initializing mid takes about O(k) time.
Inserting or deleting a number takes O(logk) time for a standard multiset scheme.
Finding the mean takes constant O(1) time since the median elements are directly accessible from mid iterator.
The last two steps take place about (n−k) times (the number of sliding window instances).
Space complexity: O(k) extra linear space to hold contents of the window.

Further Thoughts
As noted before, this problem is essentially an extension to 295. Find Median From Data Stream. That problem had a lot of ways to go about, that frankly, are not of much use in an interview. But they are interesting to follow all the same. If you are interested take a look here. Try extending those methods to this problem.

Footnotes
std::prev() is a C++ method to find the previous element to the current one being pointed to by an iterator.

Had we used std::multiset::find(), there was no guarantee that the first occurrence of num would be found. Although the contrary did happen in all of our tests, I don't recommend using it. Your mileage may vary.

Shout-out to @votrubac and @StefanPochmann!

Hinting can reduce that to amortized constant O(1) time.

------
一點建議
這題是 295. Find Median From Data Stream 的延伸題。這表示很多解這題的方法，都是建立在解 295. Find Median From Data Stream 的思路之上。你也許可以先做那一題，再回頭處理這一題。

解法
方法 1：直接排序
直覺

照題目字面要求去做。

演算法

用一個大小為 k 的視窗容器來存數字。每次都需要做以下操作：

插入新的進入元素。
刪除舊的離開元素。
將視窗排序以找出中位數。
一個很原始的做法，是每次都從輸入陣列複製連續 k 個元素到視窗中，然後重新排序。這樣會造成很多重複工作。

如果改成每次視窗右移時，只插入一個新元素、刪除一個舊元素，會稍微更好一些。真正的挑戰在於：如何在插入與刪除前後，都維持視窗內容仍然是排序好的。

Python 有很好用的 `bisect` 模組，可以協助在保持串列有序的前提下，高效率地進行插入操作。

複雜度分析

時間複雜度：`O(n⋅k)`。

每次把元素複製到容器裡，大約需要 `O(k)` 時間。這個動作大約會發生 `(n−k)` 次。

用二分搜尋定位插入或刪除位置，需要約 `O(logk)` 的搜尋時間，但實際移動元素仍要 `O(k)`。這類操作大約會進行 `n` 次。

空間複雜度：`O(k)`，需要額外線性空間來存視窗容器。

方法 2：雙堆（延遲刪除）
直覺

這個想法和 295. Find Median From Data Stream 的方法 3 相同。額外多出的需求，只是要把離開視窗的元素移除。

由於視窗元素是存放在堆中，不在堆頂的元素很難直接刪掉。

有些語言（例如 Java）提供的 `PriorityQueue` 類別，支援刪除任意位置的元素。不過一般來說，這種功能不夠高效，而且可攜性也不一定可靠。

如果假設我們只能存取堆頂元素，也就是只能用一般堆的標準操作，那就必須想辦法有效地標記並移除那些已經滑出視窗的元素。

這裡一個重要觀察是：只要兩個堆保持平衡，真正要拿來求中位數的，其實只有兩邊堆頂。因此，只要能維持平衡，我們其實可以容忍堆裡暫時存在一些多餘元素。

因此，可以用雜湊表來追蹤那些已失效的元素。等它們之後浮到堆頂時，再把它們真正從堆中移除。這就是延遲刪除技巧。

接下來的挑戰是：當堆中保留了這些多餘元素時，如何仍然維持兩個堆的平衡？作法是把一些元素從其中一個堆移到另一個堆，藉此抵銷那些失效元素造成的影響，維持兩堆在有效元素數量上的平衡。

注意：這裡所說的「平衡堆」，不是指堆的實際大小，而是指堆中「有效元素」的數量平衡。

演算法

使用兩個優先佇列：

一個 max-heap `lo`，用來存較小的一半數字
一個 min-heap `hi`，用來存較大的一半數字
一個 hash-map 或 hash-table `hash_table`，用來追蹤失效元素。它記錄那些已被標記刪除、但實際上還留在堆中的值及其出現次數。

在最糟情況下，max-heap `lo` 允許比 min-heap `hi` 多一個元素。因此當我們已經處理了 k 個元素後：

如果 `k = 2⋅n + 1 (∀n∈Z)`，那麼 `lo` 可以有 `n + 1` 個元素，而 `hi` 有 `n` 個元素。
如果 `k = 2⋅n (∀n∈Z)`，那麼兩個堆會完全平衡，各有 `n` 個元素。
這帶來一個很好用的性質：當兩堆完全平衡時，中位數可以由兩個堆頂一起算出；否則，真正的中位數就在 max-heap `lo` 的堆頂。

注意：如前所述，當我們在談「平衡堆」時，不是在看堆實際裝了多少元素，而是在看其中有效元素的數量。

維護一個平衡因子 `balance`。它表示三種情況：

`balance = 0`：兩個堆已平衡，或接近平衡。
`balance < 0`：`lo` 需要更多有效元素，因此要把元素從 `hi` 移到 `lo`。
`balance > 0`：`hi` 需要更多有效元素，因此要把元素從 `lo` 移到 `hi`。
插入新進來的數字 `in_num`：

如果 `in_num` 小於等於 `lo` 的堆頂，就把它放進 `lo`。但這樣會讓 `hi` 的有效元素相對較少，所以 `balance` 要加一。

否則，`in_num` 必須放進 `hi`。這會讓 `lo` 相對失衡，所以 `balance` 要減一。

延遲刪除離開視窗的數字 `out_num`：

如果 `out_num` 屬於 `lo`，那麼把它標記失效會使 `lo` 本身少掉一個有效元素，所以 `balance` 要減一。

如果 `out_num` 屬於 `hi`，那麼把它標記失效會使 `hi` 本身少掉一個有效元素，所以 `balance` 要加一。

把這個元素在 `hash_table` 中的計數加一。

當失效元素浮到任一個堆頂時，再真正把它從堆中移除，並把 `hash_table` 中對應的計數減一。

複雜度分析

時間複雜度：`O(nlogn)`

由於在某些序列中，例如嚴格遞減序列，`hi` 和 `lo` 可能會因延遲刪除而累積到 `O(n)` 個元素，因此兩個堆的插入與刪除操作都可能要花 `O(logn)`。進行 `n` 步後，總時間複雜度就是 `O(n log n)`。

雜湊表操作平均仍然是 `O(1)`，但不會改變整體時間複雜度，因為主導項仍然是 `O(nlogn)`。

空間複雜度：`O(n) + O(n−k) ≈ O(n)` 額外線性空間。

兩個堆合計需要 `O(n)` 空間。由於延遲刪除，已失效的元素可能仍殘留在堆中。例如在遞減序列中，被移出的元素永遠不會跑到堆頂，會持續累積，導致堆大小成長到 `O(n)`。
雜湊表大約需要 `O(n−k)` 空間。

方法 3：兩個 multiset
直覺

可以看出，`multiset` 是一種很適合維持元素有序的資料結構，同時也能高效率存取最前面與最後面的元素。插入或刪除任意元素，在 `multiset` 中也相對有效率。（可參考 295. Find Median From Data Stream 的方法 4 直覺）

因此，如果前一種方法讓你覺得太麻煩，可以考慮用 `multiset` 取代 `priority_queue`。

演算法

插入與刪除元素本身都很直接。維持兩邊平衡的方式，和 295. Find Median From Data Stream 的方法 3 相同。

複雜度分析

時間複雜度：`O((n−k)⋅6⋅logk) ≈ O(nlogk)`。

最壞情況下，可能有三次從集合頭尾進行插入，以及三次從集合頭尾進行刪除。每次都大約需要 `O(logk)`。
求平均值則是常數時間 `O(1)`，因為集合的開頭與結尾元素都能直接取得。
這些步驟大約會發生 `(n−k)` 次，也就是滑動視窗的次數。
空間複雜度：`O(k)`，需要額外線性空間來存視窗內容。

方法 4：multiset + 雙指標
直覺

這和 295. Find Median From Data Stream 的方法 4 是同一個想法。

不過，這題其實不一定需要兩個指標。

中位數元素可以透過一個 iterator 位置來求得，當 `k` 為奇數時只需要一個位置；當 `k` 為偶數時，則需要兩個連續位置。因此只追蹤一個指標就夠了，另一個需要時可隱含推導出來。

演算法

使用單一 iterator `mid` 來走訪視窗 `multiset`。它對應到 295. Find Median From Data Stream 方法 4 中的 `upper_median`。`lower_median` 可以由 `mid` 隱含推導出來：當 `k` 為奇數時它就是 `mid` 本身，當 `k` 為偶數時則是 `prev(mid)`。

一開始先把前 k 個元素放進 `multiset` 視窗中，再把 `mid` 設定到 `window` 中第 `⌊k/2⌋` 個索引位置（以 0-based indexing 計算；`multiset` 會自動維持排序）。

當把元素 `num` 插入 `window` 時，會有三種情況：

`num` 小於 `upper median` 的值 `mid`。

`num` 大於 `upper median` 的值 `mid`。

`num` 等於 `upper median` 的值 `mid`。這種情況通常和語言實作有關。由於 C++ 的 `multiset` 會把新元素插到相等區間的尾端，所以這種情況本質上與前一個情況相同。

對於第一種情況，`num` 會插在 `mid` 前面，因此 `mid` 就不再指向第 `⌊k/2⌋` 個元素，而會變成指向第 `⌊k/2⌋ + 1` 個元素。這時就需要將 `mid` 向前移一格修正。

對於第二與第三種情況，`num` 會插在 `mid` 後面，所以不會破壞 `mid` 的位置，它仍然指向第 `⌊k/2⌋` 個元素。

當然，插入後視窗大小會暫時變成 `k + 1`。這可以透過接下來移除即將離開視窗的元素來修正。

當移除元素 `num` 時，也會有和插入時類似的三種情況：

`num` 小於 `upper median` 的值 `mid`。

`num` 大於 `upper median` 的值 `mid`。

`num` 等於 `upper median` 的值 `mid`。由於 `mid` 會指向 `multiset window` 中 `num` 的第一個出現位置，而我們也會確定性地刪除第一個出現位置（注意：這裡使用 `std::multiset::lower_bound()` 來找到正確要刪除的那個位置），因此這種情況的處理方式和第一種情況相同。

在第一種與第三種情況下，移除 `num` 會破壞 `mid` 指標，因此需要在刪除前先把 `mid` 向後移一格修正。

在第二種情況下，`mid` 不會受到影響，不需要額外處理。

當元素刪除後，視窗大小就會回到 `k`。

在插入新元素與移除舊元素之後，會再次滿足幾個很好的不變條件：

視窗大小重新變成 `k`。
視窗仍然完全有序。
`mid` 仍然指向第 `⌊k/2⌋` 個索引位置。
此時求中位數就很容易了。它就是 `lo_median` 與 `hi_median` 兩個位置所指元素的平均值。對我們而言，這兩個位置就是 `mid` 或 `prev(mid)`（依 `k` 是奇數還是偶數決定），以及 `mid` 本身。

複雜度分析

時間複雜度：`O((n−k)logk) + O(k) ≈ O(nlogk)`。

初始化 `mid` 大約需要 `O(k)` 時間。
插入或刪除一個數字，在標準 `multiset` 實作下需要 `O(logk)`。
求平均值是常數時間 `O(1)`，因為中位數位置可由 `mid` 直接存取。
後兩個步驟大約會執行 `(n−k)` 次，也就是滑動視窗的次數。
空間複雜度：`O(k)`，需要額外線性空間來保存視窗內容。

進一步思考
如前所述，這題本質上是 295. Find Median From Data Stream 的延伸。那一題其實有很多種解法，老實說，其中不少在面試裡實用性不高，但作為思路延伸仍然很有意思。如果你有興趣，可以再去看看那些方法，並試著把它們延伸到這一題。

註腳
`std::prev()` 是 C++ 用來取得 iterator 前一個元素的方法。

如果我們使用的是 `std::multiset::find()`，就不能保證找到的是 `num` 的第一個出現位置。雖然在我們的測試中剛好都找到了，但我仍然不建議這樣做。結果可能因情況而異。

特別感謝 @votrubac 與 @StefanPochmann！

如果提供 hint，插入時間還能進一步降到 amortized constant `O(1)`。
