Overview
If you don't have much experience with binary-search-related problems, we strongly suggest you read this LeetCode Explore Card, our explore card for binary search! We'll cover four methods, the first three of which are closely related to those presented in this card, so it's helpful to look ahead!

Approach 1: Find the Exact Value
Intuition
We start from the most basic and elementary template.

First, we define the search space using two boundary indexes, left and right, all possible indexes are within the inclusive range [left, right]. We shall continue searching over the search space as long as it is not empty. A general way is to use a while loop with the condition left <= right, so we can break out of this loop if we empty the range or trigger other conditions which we will discuss later.

alt text

The next step is to find the 'pivot point', the middle index that divides the search space into two halves. We need to compare the value at the middle index nums[mid] with target, the purpose of this step is to cut one half that is guaranteed not to contain target.

If nums[mid] = target, it means we find target, and the job is done! We can break the loop by returning mid.
If nums[mid] < target, combined with the array is sorted, we know that all values in the left half are smaller than target, so we can safely cut this half by letting left = mid + 1.
If nums[mid] > target, it means all values in the right half are larger than target and can be cut safely!
alt text

Does this loop ever stop? Yes, take the following picture as an example, suppose we are searching over an array of size 1, in this case, left, right, and mid all stand for the only index in the array. In any of the three conditions, we trigger one of the break statements and stop the loop.

alt text


Algorithm
Initialize the boundaries of the search space as left = 0 and right = nums.size - 1.
If there are elements in the range [left, right], we find the middle index mid = (left + right) / 2 and compare the middle value nums[mid] with target:
If nums[mid] = target, return mid.
If nums[mid] < target, let left = mid + 1 and repeat step 2.
If nums[mid] > target, let right = mid - 1 and repeat step 2.
We finish the loop without finding target, return -1.
Implementation

Complexity Analysis
Let n be the size of the input array nums.

Time complexity: O(logn)

nums is divided into half each time. In the worst-case scenario, we need to cut nums until the range has no element, and it takes logarithmic time to reach this break condition.
Space complexity: O(1)

During the loop, we only need to record three indexes, left, right, and mid, they take constant space.

Approach 2: Find Upper bound
Intuition
Here we introduce an alternative way to implement binary search: instead of looking for target in the array nums, we look for the insert position where we can put target in without disrupting the order.

alt text

Generally, we have two inserting ways, insert into the rightmost possible position which we called finding the upper bound, and insert into the leftmost possible position which we called finding the lower bound. We will implement them in the following approaches.

Take the picture below as an example. Assume that we want to insert 9 into array A. If we look for the upper bound, we have to insert 9 to the right of all existing 9s in the array. Similarly, if we look for the lower bound, we have to insert 9 to the left of all existing 9s. (Although we don't have duplicate elements in this problem, having duplicate elements is more common in problems so we would better know this concept in advance!)

alt text

Now we start the binary search. Similar to the previous approach, we still use left and right as two boundary indexes. The question is, what is the next step after we find the middle index mid?

alt text

If nums[mid] < target, the insert position is on mid's right, so we let left = mid + 1 to discard the left half and mid.

If nums[mid] = target, the insert position is on mid's right, so we let left = mid + 1 to discard the left half and mid.

alt text

If nums[mid] > target, mid can also be the insert position. So we let right = mid to discard the right half while keeping mid.
Therefore, we merged the two conditions nums[mid] = target and nums[mid] < target and there are only two conditions in the if-else statement!

alt text

Once the loop stops, left stands for the insert position and left - 1 is the largest element that is no larger than target. We just need to check if nums[left - 1] equals target. Note this boundary condition where left = 0, which means all elements in nums are larger than target, so there is no target in nums.


Algorithm
Initialize the boundaries of the search space as left = 0 and right = nums.size (Note that the maximum insert position can be nums.size)
If there are elements in the range [left, right], we find the middle index mid = (left + right) / 2 and compare the middle value nums[mid] with target:
If nums[mid] <= target, let left = mid + 1 and repeat step 2.
If nums[mid] > target, let right = mid and repeat step 2.
We finish the loop and left stands for the insert position:
If left > 0 and nums[left - 1] = target, return left - 1.
Otherwise, return -1.
Implementation

Complexity Analysis
Let n be the size of the input array nums.

Time complexity: O(logn)

nums is divided into half each time. In the worst-case scenario, we need to cut nums until the range has no element, it takes logarithmic time to reach this break condition.
Space complexity: O(1)

During the loop, we only need to record three indexes, left, right, and mid, they take constant space.

Approach 3: Find Lower bound
Intuition
Different from the previous method, here we are looking for the leftmost insert position. Therefore, we will make the following changes to the judgment condition:

If nums[mid] < target, mid can also be the insertion position. So we let left = mid + 1, that is, discard the left half while keeping mid.

If nums[mid] = target, the insert position is on mid's left, so we let right = mid to discard both the right half and mid.

alt text

If nums[mid] > target, the insert position is on mid's left, so we let right = mid to discard both the right half and mid.
Therefore, we merged the two conditions nums[mid] = target and nums[mid] > target and there are only two conditions in the if-else statement!

alt text

Once the loop stops, left stands for the insert position and nums[left] is the smallest element that is no less than target. We just need to check if nums[left] equals target. Note this boundary condition left = nums.size, which means all elements in nums are smaller than target, so there is no target in nums.


Algorithm
Initialize the boundaries of the search space as left = 0 and right = nums.size (Note that the maximum insert position can be nums.size)
If there are elements in the range [left, right], we find the middle index mid = (left + right) / 2 and compare the middle value nums[mid] with target:
If nums[mid] >= target, let right = mid and repeat step 2.
If nums[mid] < target, let left = mid + 1 and repeat step 2.
We finish the loop and left stands for the insert position:
If left < nums.size and nums[left] = target, return left.
Otherwise, return -1.
Implementation

Complexity Analysis
Let n be the size of the input array nums.

Time complexity: O(logn)

nums is divided into half each time. In the worst-case scenario, we need to cut nums until the range has no element, it takes logarithmic time to reach this break condition.
Space complexity: O(1)

During the loop, we only need to record three indexes, left, right, and mid, they take constant space.

Approach 4: Use built-in tools.
Intuition
We have implemented various templates of binary search, now let's quickly go through the last approach that uses built-in functions. C++ provides the <algorithm> library that defines functions for binary searching, Python provides bisect module which also supports binary search functions. If we are solving some standard problems that do not require a lot of customization, it's feasible to rely on these built-in tools to save time.

Note that upper_bound and bisect.bisect_right look for the rightmost insertion position and bring the same result as approach 2, while lower_bound and bisect.bisect_left look for the leftmost insertion position and end up with the same result as approach 3. Once we find the insertion position, check if the value at the corresponding position equals target.

Here we implement the method that uses upper_bound or bisect.bisect_right and leave another half as a practice!


Algorithm
Use built-in tools to locate the rightmost insertion position idx.
If idx > 0 and nums[idx - 1] = target, return idx -1. Otherwise, return -1.
Implementation

Complexity Analysis
Let n be the size of the input array nums.

Time complexity: O(logn)

The time complexity of the built-in binary search is O(logn).
Space complexity: O(1)

The built-in binary search only takes O(1) space.

---

總覽
如果你對二分搜尋相關題目還不太熟，我們強烈建議你先閱讀 LeetCode Explore Card 中的 binary search 專題！我們會介紹四種方法，其中前三種和這張卡片中介紹的方法密切相關，所以先看過會很有幫助！

方法 1：尋找精確值
直覺
我們從最基本、最經典的模板開始。

首先，我們用兩個邊界索引 `left` 和 `right` 來定義搜尋空間，所有可能的索引都位於包含端點的區間 `[left, right]` 之中。只要搜尋空間不為空，我們就持續搜尋。一種通用寫法是使用條件為 `left <= right` 的 `while` 迴圈，這樣當區間被清空，或觸發後面會提到的其他條件時，就可以跳出迴圈。

alt text

下一步是找出「樞紐點（pivot point）」, 也就是把搜尋空間分成兩半的中間索引。我們需要比較中間位置的值 `nums[mid]` 和 `target`，這一步的目的是砍掉一半可以保證不包含 `target` 的區間。

如果 `nums[mid] = target`，表示我們找到了 `target`，任務完成！可以直接回傳 `mid` 來結束迴圈。  
如果 `nums[mid] < target`，再結合陣列已排序這個條件，我們知道左半邊所有值都比 `target` 小，因此可以安全地捨棄這一半，也就是令 `left = mid + 1`。  
如果 `nums[mid] > target`，表示右半邊所有值都比 `target` 大，因此也可以安全地捨棄右半邊！

alt text

這個迴圈真的會停嗎？會。以下圖為例，假設我們正在搜尋一個大小為 1 的陣列，此時 `left`、`right` 和 `mid` 都代表陣列中唯一的索引。在三種情況中的任一種，我們都會觸發其中一個結束條件，並停止迴圈。

alt text

演算法
將搜尋空間的邊界初始化為 `left = 0` 和 `right = nums.size - 1`。  
如果區間 `[left, right]` 中還有元素，就找出中間索引 `mid = (left + right) / 2`，並比較中間值 `nums[mid]` 和 `target`：  
如果 `nums[mid] = target`，回傳 `mid`。  
如果 `nums[mid] < target`，令 `left = mid + 1`，然後重複步驟 2。  
如果 `nums[mid] > target`，令 `right = mid - 1`，然後重複步驟 2。  
若整個迴圈結束仍未找到 `target`，回傳 `-1`。

實作

複雜度分析
令 `n` 為輸入陣列 `nums` 的大小。

時間複雜度：`O(log n)`

每一次都會把 `nums` 的搜尋範圍縮小成一半。在最壞情況下，我們需要持續切分 `nums`，直到區間中沒有元素為止，而到達這個終止條件需要對數時間。  
空間複雜度：`O(1)`

在迴圈中，我們只需要記錄三個索引：`left`、`right` 和 `mid`，它們只佔用固定空間。

方法 2：尋找 Upper bound
直覺
這裡我們介紹另一種實作二分搜尋的方式：不是直接在陣列 `nums` 中尋找 `target`，而是找出一個插入位置，讓 `target` 放進去之後仍然不會破壞排序。

alt text

一般來說，我們有兩種插入方式：  
插入到最右側可能的位置，這稱為尋找 `upper bound`；  
插入到最左側可能的位置，這稱為尋找 `lower bound`。  
接下來的幾種方法會分別實作它們。

以下圖為例。假設我們想把 `9` 插入陣列 `A`。如果找的是 `upper bound`，就必須把 `9` 插在陣列中所有現有 `9` 的右邊。相對地，如果找的是 `lower bound`，就必須把 `9` 插在所有現有 `9` 的左邊。（雖然這一題沒有重複元素，但很多題目都會出現重複值，因此先理解這個概念會比較好！）

alt text

現在開始二分搜尋。和前一種方法類似，我們仍然使用 `left` 和 `right` 兩個邊界索引。問題是，當找到中間索引 `mid` 之後，下一步該怎麼做？

alt text

如果 `nums[mid] < target`，表示插入位置在 `mid` 的右邊，因此令 `left = mid + 1`，捨棄左半邊以及 `mid`。  

如果 `nums[mid] = target`，表示插入位置也在 `mid` 的右邊，因此同樣令 `left = mid + 1`，捨棄左半邊以及 `mid`。

alt text

如果 `nums[mid] > target`，則 `mid` 也可能是插入位置。所以我們令 `right = mid`，捨棄右半邊，但保留 `mid`。  
因此，我們把 `nums[mid] = target` 和 `nums[mid] < target` 這兩種情況合併起來，整個 `if-else` 只剩下兩種分支！

alt text

當迴圈停止時，`left` 就代表插入位置，而 `left - 1` 是不大於 `target` 的最大元素。我們只需要檢查 `nums[left - 1]` 是否等於 `target`。請注意邊界情況 `left = 0`，這表示 `nums` 中所有元素都大於 `target`，因此陣列中不存在 `target`。

演算法
將搜尋空間的邊界初始化為 `left = 0` 和 `right = nums.size`（請注意，最大的插入位置可以是 `nums.size`）。  
如果區間 `[left, right]` 中還有元素，就找出中間索引 `mid = (left + right) / 2`，並比較中間值 `nums[mid]` 和 `target`：  
如果 `nums[mid] <= target`，令 `left = mid + 1`，然後重複步驟 2。  
如果 `nums[mid] > target`，令 `right = mid`，然後重複步驟 2。  
迴圈結束後，`left` 代表插入位置：  
如果 `left > 0` 且 `nums[left - 1] = target`，回傳 `left - 1`。  
否則回傳 `-1`。

實作

複雜度分析
令 `n` 為輸入陣列 `nums` 的大小。

時間複雜度：`O(log n)`

每一次都會把 `nums` 的搜尋範圍縮小一半。在最壞情況下，我們需要不斷切分 `nums`，直到區間中沒有元素為止，而這個終止條件需要對數時間才能到達。  
空間複雜度：`O(1)`

在迴圈中，我們只需要記錄三個索引：`left`、`right` 和 `mid`，它們只佔用固定空間。

方法 3：尋找 Lower bound
直覺
和前一種方法不同，這裡我們要找的是最左邊的插入位置。因此，判斷條件會改成下面這樣：

如果 `nums[mid] < target`，插入位置在 `mid` 的右邊，所以令 `left = mid + 1`，也就是捨棄左半邊以及 `mid`。  

如果 `nums[mid] = target`，插入位置在 `mid` 的左邊，所以令 `right = mid`，捨棄右半邊，但保留 `mid`。

alt text

如果 `nums[mid] > target`，插入位置也在 `mid` 的左邊，所以令 `right = mid`，捨棄右半邊，但保留 `mid`。  
因此，我們把 `nums[mid] = target` 和 `nums[mid] > target` 這兩種情況合併起來，整個 `if-else` 只剩下兩種分支！

alt text

當迴圈停止時，`left` 就代表插入位置，而 `nums[left]` 是不小於 `target` 的最小元素。我們只需要檢查 `nums[left]` 是否等於 `target`。請注意邊界情況 `left = nums.size`，這表示 `nums` 中所有元素都小於 `target`，因此陣列中不存在 `target`。

演算法
將搜尋空間的邊界初始化為 `left = 0` 和 `right = nums.size`（請注意，最大的插入位置可以是 `nums.size`）。  
如果區間 `[left, right]` 中還有元素，就找出中間索引 `mid = (left + right) / 2`，並比較中間值 `nums[mid]` 和 `target`：  
如果 `nums[mid] >= target`，令 `right = mid`，然後重複步驟 2。  
如果 `nums[mid] < target`，令 `left = mid + 1`，然後重複步驟 2。  
迴圈結束後，`left` 代表插入位置：  
如果 `left < nums.size` 且 `nums[left] = target`，回傳 `left`。  
否則回傳 `-1`。

實作

複雜度分析
令 `n` 為輸入陣列 `nums` 的大小。

時間複雜度：`O(log n)`

每一次都會把 `nums` 的搜尋範圍縮小一半。在最壞情況下，我們需要不斷切分 `nums`，直到區間中沒有元素為止，而這個終止條件需要對數時間才能到達。  
空間複雜度：`O(1)`

在迴圈中，我們只需要記錄三個索引：`left`、`right` 和 `mid`，它們只佔用固定空間。

方法 4：使用內建工具
直覺
我們已經實作了多種二分搜尋模板，現在快速看一下最後一種使用內建函式的方法。C++ 提供 `<algorithm>` 函式庫，其中定義了二分搜尋相關函式；Python 提供 `bisect` 模組，也支援二分搜尋功能。如果我們處理的是一些標準題型，不需要太多客製化，直接使用這些內建工具能節省時間。

請注意，`upper_bound` 和 `bisect.bisect_right` 會找最右邊的插入位置，結果和方法 2 相同；而 `lower_bound` 和 `bisect.bisect_left` 會找最左邊的插入位置，結果和方法 3 相同。當我們找到插入位置後，只要檢查對應位置的值是否等於 `target` 即可。

這裡我們實作使用 `upper_bound` 或 `bisect.bisect_right` 的方法，另一半就留給你當作練習！

演算法
使用內建工具找出最右邊的插入位置 `idx`。  
如果 `idx > 0` 且 `nums[idx - 1] = target`，回傳 `idx - 1`；否則回傳 `-1`。

實作

複雜度分析
令 `n` 為輸入陣列 `nums` 的大小。

時間複雜度：`O(log n)`

內建二分搜尋的時間複雜度是 `O(log n)`。  
空間複雜度：`O(1)`

內建二分搜尋只需要 `O(1)` 的空間。
