Overview
We are given two integer arrays, nums1 and nums2, and need to return a list of two lists. The first list has the elements that are present only in nums1, while the second list has the elements that are present only in nums2.


Approach 1: Brute Force
Intuition

To find the elements in a list that are not present in another list, we can loop over every element in the first list and for each element we loop over the elements in the second list to check if it's present or not. If we find the element, we will not store it in the answer list; otherwise, we can store it.

This way, we will have to apply the above method twice once for the elements that are only in nums1 and then again for the elements that are only present in nums2.

Algorithm

getElementsOnlyInFirstList function:

Initialize an empty set onlyInNums1 to store elements that are only in nums1.

Iterate over each element num in nums1:

Set a boolean flag existInNums2 to false.
Iterate over each element x in nums2:
If num is found in nums2 (i.e., x == num), set existInNums2 to true and break the inner loop.
If existInNums2 is still false, add num to the set onlyInNums1 (i.e., num exists in nums1 but not in nums2).
Convert onlyInNums1 set to a list and return it.

findDifference function:

Call getElementsOnlyInFirstList(nums1, nums2) to get elements only in nums1 and store the result.
Call getElementsOnlyInFirstList(nums2, nums1) to get elements only in nums2 and store the result.
Return a list of both results as a list of lists).
The overall result contains two lists:

The first list contains elements in nums1 that are not in nums2.
The second list contains elements in nums2 that are not in nums1.
Implementation


Complexity Analysis

Here, N is the length of list nums1, and M is the length of nums2.

Time complexity: O(N×M).

The outer function findDifference calls the helper function getElementsOnlyInFirstList twice, once for nums1 and once for nums2. For each element in nums1, we loop through all elements in nums2 to check for existence, which results in a nested loop. The first loop runs for N iterations, and for each iteration, the inner loop runs for M iterations. This leads to a time complexity of O(N×M).

Additionally, the process of inserting elements into the set and converting the set into a list both take linear time, which is O(N) for each. However, since N×M dominates, the overall time complexity remains O(N×M).

The second call to getElementsOnlyInFirstList(nums2, nums1) has similar behavior with M×N, but since multiplication is commutative, this still results in O(N×M) overall.

Space complexity: O(N+M).

The space complexity is primarily determined by the set and the list used to store unique elements. In the worst case, all elements of nums1 and nums2 are unique, leading to O(N) space for the set in the first call and O(M) space in the second call.

Since both data structures exist separately for each call and are converted to list, the overall space usage is O(N+M). The extra space for variables like existInNums2 and loop counters is negligible, making the auxiliary space O(1).

Approach 2: HashSet
Intuition

Instead of iterating over each element in the second array to check if it exists in the list or not, we can store the elements in a HashSet. Then we can find if an element exists in the list or not in O(1) time compared to O(N) time in the previous approach.

In this approach, we follow the above intuition. To find the elements that only exist in nums1, we first store the elements in nums2 in the HashSet. Then we iterate over each element in the list nums1, and for each element, we check if it's there in the HashSet; if yes, we skip the element; otherwise, we store it in the list onlyInNums1.

fig

Algorithm

getElementsOnlyInFirstList function:

Initialize an empty set onlyInNums1 to store elements that are only in nums1.

Create a set existsInNums2 to store all elements from nums2:

Iterate over each element num in nums2 and add it to the set existsInNums2.
Iterate over each element num in nums1:

If num is not in existsInNums2, add it to the set onlyInNums1 (i.e., num exists in nums1 but not in nums2).
Convert onlyInNums1 set to a list and return it.

findDifference function:

Call getElementsOnlyInFirstList(nums1, nums2) to get elements only in nums1 and store the result.
Call getElementsOnlyInFirstList(nums2, nums1) to get elements only in nums2 and store the result.
Return a list of both results as a list of lists (i.e., Arrays.asList).
The overall result contains two lists:

The first list contains elements in nums1 that are not in nums2.
The second list contains elements in nums2 that are not in nums1.
Implementation


Complexity Analysis

Here, N is the length of list nums1, and M is the length of nums2.

Time complexity: O(N+M).

In this implementation, the function getElementsOnlyInFirstList is called twice. For each call, we first iterate over nums2 to store its elements in a set. This operation takes O(M) time, where M is the length of nums2. Inserting elements into a set is on average an O(1) operation, so this step runs in O(M) time.

Next, we iterate over nums1 and for each element, we check if it exists in the existsInNums2 set. This lookup operation in a set also takes O(1) on average, meaning the entire iteration over nums1 takes O(N) time.

Since we perform these two operations (storing elements in a set and iterating over another set) for both nums1 and nums2, the total time complexity is O(N+M) for each call to getElementsOnlyInFirstList. Thus, the overall time complexity is O(N+M).

Space complexity: O(N+M).

The space complexity is primarily determined by the set data structures used to store the elements of nums2 and the unique elements from nums1. In the worst case, all elements of nums1 and nums2 are unique, meaning the set for nums2 will take O(M) space, and the set for the unique elements of nums1 will take O(N) space.

Additionally, the list created to store the result takes up O(N) space, as it needs to hold the elements from the set. Therefore, the total space complexity is O(N+M), as the two sets and the lists are the main contributors to space usage.

---

中文翻譯（對應上方英文原文）

Overview  
概觀  
我們會拿到兩個整數陣列 `nums1` 與 `nums2`，需要回傳一個包含兩個串列的結果。第一個串列放只出現在 `nums1` 的元素；第二個串列放只出現在 `nums2` 的元素。


Approach 1: Brute Force  
方法一：暴力法

Intuition  
直覺

要找出某個串列中「不存在於另一個串列」的元素，可以對第一個串列的每個元素逐一檢查，並在第二個串列中線性搜尋它是否存在。若找到就不加入答案；若沒找到就加入答案。

因此這個流程要做兩次：  
第一次找「只在 `nums1`」的元素；  
第二次找「只在 `nums2`」的元素。

Algorithm  
演算法

`getElementsOnlyInFirstList` 函式：

初始化一個空集合 `onlyInNums1`，用來儲存只在 `nums1` 出現的元素。  
對 `nums1` 的每個元素 `num` 迭代：

將布林旗標 `existInNums2` 設為 `false`。  
對 `nums2` 的每個元素 `x` 迭代：  
若 `x == num`，代表 `num` 在 `nums2` 中，將 `existInNums2` 設為 `true`，並中止內層迴圈。  
若迭代完後 `existInNums2` 仍為 `false`，把 `num` 加入 `onlyInNums1`（表示它在 `nums1` 中、但不在 `nums2` 中）。  
最後把 `onlyInNums1` 轉成串列並回傳。

`findDifference` 函式：

呼叫 `getElementsOnlyInFirstList(nums1, nums2)` 取得只在 `nums1` 的元素。  
呼叫 `getElementsOnlyInFirstList(nums2, nums1)` 取得只在 `nums2` 的元素。  
將兩個結果組成一個二維結果回傳。  
整體結果包含兩個串列：

第一個串列：`nums1` 中不在 `nums2` 的元素。  
第二個串列：`nums2` 中不在 `nums1` 的元素。

Implementation  
實作


Complexity Analysis  
複雜度分析

其中 `N` 是 `nums1` 長度，`M` 是 `nums2` 長度。

Time complexity: `O(N×M)`  
時間複雜度：`O(N×M)`

外層函式 `findDifference` 會呼叫輔助函式 `getElementsOnlyInFirstList` 兩次（一次以 `nums1` 為主，一次以 `nums2` 為主）。  
在每次呼叫中，對第一個陣列的每個元素，都要掃過第二個陣列檢查是否存在，形成巢狀迴圈。第一層約 `N` 次、內層約 `M` 次，因此為 `O(N×M)`。

此外，把元素插入集合與把集合轉成串列，各自是線性時間（約 `O(N)` 或 `O(M)`）。但主要成本仍由 `N×M` 主導，所以整體仍是 `O(N×M)`。

第二次呼叫 `getElementsOnlyInFirstList(nums2, nums1)` 對應到 `M×N`，量級相同，因此總體仍是 `O(N×M)`。

Space complexity: `O(N+M)`  
空間複雜度：`O(N+M)`

空間主要來自用來儲存唯一元素的集合與結果串列。最壞情況下，`nums1` 與 `nums2` 的元素都互不重複，第一次呼叫可能用到 `O(N)`，第二次呼叫可能用到 `O(M)`。

因為兩次呼叫各自建立資料結構並轉成串列，整體空間為 `O(N+M)`。像 `existInNums2` 與迴圈變數等額外變數可視為常數空間，所以輔助空間為 `O(1)`。


Approach 2: HashSet  
方法二：HashSet

Intuition  
直覺

與其對第二個陣列逐一掃描來檢查元素是否存在，不如先把元素放進 `HashSet`。  
如此一來，查找平均可從前一法的 `O(N)` 降到 `O(1)`。

在這個方法中，要找只存在於 `nums1` 的元素時，先把 `nums2` 的元素放入 `HashSet`，再遍歷 `nums1`。若元素在集合中就略過；不在集合中就加入 `onlyInNums1`。

fig  
（圖示）

Algorithm  
演算法

`getElementsOnlyInFirstList` 函式：

初始化空集合 `onlyInNums1`，儲存只在 `nums1` 的元素。  
建立集合 `existsInNums2`，把 `nums2` 的元素全部放入：

遍歷 `nums2` 的每個元素 `num`，加入 `existsInNums2`。  
接著遍歷 `nums1` 的每個元素 `num`：

若 `num` 不在 `existsInNums2`，加入 `onlyInNums1`（代表在 `nums1`、不在 `nums2`）。  
最後把 `onlyInNums1` 轉成串列並回傳。

`findDifference` 函式：

呼叫 `getElementsOnlyInFirstList(nums1, nums2)` 取得只在 `nums1` 的元素。  
呼叫 `getElementsOnlyInFirstList(nums2, nums1)` 取得只在 `nums2` 的元素。  
把兩個結果組成二維結果回傳（例如 `Arrays.asList`）。  
整體結果包含兩個串列：

第一個串列：`nums1` 中不在 `nums2` 的元素。  
第二個串列：`nums2` 中不在 `nums1` 的元素。

Implementation  
實作


Complexity Analysis  
複雜度分析

其中 `N` 為 `nums1` 長度，`M` 為 `nums2` 長度。

Time complexity: `O(N+M)`  
時間複雜度：`O(N+M)`

此實作中，`getElementsOnlyInFirstList` 呼叫兩次。每次呼叫先遍歷 `nums2` 建立集合，耗時 `O(M)`；集合插入平均為 `O(1)`，所以這步是 `O(M)`。

接著遍歷 `nums1`，每個元素都在 `existsInNums2` 做一次集合查找；查找平均 `O(1)`，因此整段為 `O(N)`。

由於這兩步會對兩組輸入各做一次，整體為線性量級，最後時間複雜度為 `O(N+M)`。

Space complexity: `O(N+M)`  
空間複雜度：`O(N+M)`

空間主要由集合結構決定：一個集合放 `nums2` 的元素，另一個集合放從 `nums1` 篩出的唯一元素。最壞情況下兩邊元素都互異，分別需要 `O(M)` 與 `O(N)`。

此外，輸出串列本身也需要對應空間（最多線性）。因此總空間複雜度是 `O(N+M)`。
