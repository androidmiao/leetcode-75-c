This question comes under a broad category of "Array Transformation". This category is the meat of tech interviews. Mostly because arrays are such a simple and easy to use data structure. Traversal or representation doesn't require any boilerplate code and most of your code will look like the Pseudocode itself.

The 2 requirements of the question are:

Move all the 0's to the end of array.

All the non-zero elements must retain their original order.

It's good to realize here that both the requirements are mutually exclusive, i.e., you can solve the individual sub-problems and then combine them for the final solution.

Approach #1 (Space Sub-Optimal) [Accepted]
Traverse the nums list first to count the number of zeroes. Then traverse the nums list again to store all non-zero elements in ans.

Algorithm:
Determine the size of the nums array and store it in n.

Count the number of zeroes in nums:

Initialize numZeroes to 0.
Iterate through each element in nums:
Increment numZeroes for each zero encountered.
Create a new vector ans to store non-zero elements in their original order:

Iterate through each element in nums:
Add non-zero elements to ans.
Append all zeroes to the end of the ans vector:

Append numZeroes zeroes to ans.
Update the original nums array with the elements from ans:

Copy each element from ans back to nums.

Complexity Analysis

Space Complexity : O(n). Since we are creating the "ans" array to store results.

Time Complexity: O(n). We traverse the nums list first to count the number of zeroes using O(n) time. Then, we traverse the nums list again to store all non-zero elements in ans which also costs O(n) time. Hence, the overall time complexity is O(2n), which is simplified to O(n). However, the total number of operations are sub-optimal. We can achieve the same result in less number of operations.

If asked in an interview, the above solution would be a good start. You can explain the interviewer(not code) the above and build your base for the next Optimal Solution.

Approach #2 (Space Optimal, Operation Sub-Optimal) [Accepted]
This approach works the same way as above, i.e. , first fulfills one requirement and then another. The catch? It does it in a clever way. The above problem can also be stated in alternate way, " Bring all the non 0 elements to the front of array keeping their relative order same".

This is a 2 pointer approach. The fast pointer(nums[i]) does the job of processing new elements. If the newly found element is not a 0, we record it just after the last found non-0 element. The position of last found non-0 element is denoted by the slow pointer lastNonZeroFoundAt variable. As we keep finding new non-0 elements, we just overwrite them at the lastNonZeroFoundAt + 1 'th index. This overwrite will not result in any loss of data because we already processed what was there(if it were non-0,it already is now written at it's corresponding index,or if it were 0 it will be handled later in time).

After the nums[i] reaches the end of array, we now know that all the non-0 elements have been moved to beginning of array in their original order. Now comes the time to fulfil other requirement, "Move all 0's to the end". We now simply need to fill all the indexes after the lastNonZeroFoundAt index with 0.

Algorithm:
Initialize lastNonZeroFoundAt to 0:

This variable tracks the position where the next non-zero element should be placed.
Iterate through each element in nums:

If the current element nums[i] is not zero:
Place nums[i] at index lastNonZeroFoundAt.
Increment lastNonZeroFoundAt to move to the next position for future non-zero elements.
After processing all elements:

Fill the remaining positions in the array (from lastNonZeroFoundAt to the end) with zeros.
This ensures that all non-zero elements are moved to the beginning of the array and all zeros are placed at the end.


Complexity Analysis

Space Complexity : O(1). Only constant space is used.

Time Complexity: O(n). We traverse the nums list first to move all non-zero elements to the beginning of array which costs O(n) time. At the worst case when the original array only consists of 0s, we will use O(n) time to fill all remaining elements with 0s. Hence, the overall time complexity is O(2n), which is simplified to O(n). However, the total number of operations are still sub-optimal. The total operations (array writes) that code does is n (Total number of elements).

Approach #3 (Optimal) [Accepted]
The total number of operations of the previous approach is sub-optimal. For example, the array which has all (except last) leading zeroes: [0, 0, 0, ..., 0, 1].How many write operations to the array? For the previous approach, it writes 0's n−1 times, which is not necessary. We could have instead written just once. How?
.....
By only fixing the non-0 element,i.e., 1.

The optimal approach is again a subtle extension of above solution. A simple realization is if the current element is non-0, its' correct position can at best be it's current position or a position earlier. If it's the latter one, the current position will be eventually occupied by a non-0 ,or a 0, which lies at a index greater than 'cur' index. We fill the current position by 0 right away,so that unlike the previous solution, we don't need to come back here in next iteration.

In other words, the code will maintain the following invariant:

All elements before the slow pointer (lastNonZeroFoundAt) are non-zeroes.

All elements between the current and slow pointer are zeroes.

Therefore, when we encounter a non-zero element, we need to swap elements pointed by current and slow pointer, then advance both pointers. If it's zero element, we just advance current pointer.

With this invariant in-place, it's easy to see that the algorithm will work.

Algorithm:
Initialize lastNonZeroFoundAt to 0 to track the position of the last non-zero element.

Iterate through each element in nums using cur as the index:

If nums[cur] is not zero:
Swap nums[lastNonZeroFoundAt] with nums[cur] to move the non-zero element to the correct position.
Increment lastNonZeroFoundAt to update the position for the next non-zero element.
Continue iterating until all elements are processed, ensuring all non-zero elements are moved to the front of the array and zeros are pushed to the end.


Complexity Analysis

Space Complexity : O(1). Only constant space is used.

Time Complexity: O(n). However, the total number of operations are optimal. The total operations (array writes) that code does is Number of non-0 elements.This gives us a much better best-case (when most of the elements are 0) complexity than last solution. However, the worst-case (when all elements are non-0) complexity for both the algorithms is same.

Analysis written by: @spandan.pathak



---
以下是 [283-move-zeroes_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/283-move-zeroes/283-move-zeroes_Solution.md) 的中文翻譯：

---

這題屬於廣義的「陣列轉換（Array Transformation）」類型。這類題是技術面試的核心，因為陣列是非常簡單、好用的資料結構。遍歷與表示都不需要太多樣板程式，很多程式碼幾乎就像偽程式碼本身。

題目的兩個要求是：

1. 把所有 `0` 移到陣列尾端。  
2. 所有非零元素必須保留原本相對順序。

這裡要意識到，這兩個要求其實可以分開處理，再合併成最終解法。

## 方法一（空間次優）[Accepted]

先遍歷 `nums` 計算 `0` 的數量；再遍歷一次，把所有非零元素放進 `ans`。

**演算法：**

1. 取得 `nums` 長度，記為 `n`。  
2. 計算 `nums` 中的 `0` 數量：  
   - `numZeroes = 0`  
   - 逐一遍歷 `nums`，遇到 `0` 就遞增 `numZeroes`。  
3. 建立新陣列 `ans`，依原順序存放非零元素：  
   - 再次遍歷 `nums`，把非零元素加入 `ans`。  
4. 在 `ans` 尾端補上 `numZeroes` 個 `0`。  
5. 把 `ans` 複製回原本的 `nums`。

**複雜度分析：**

- 空間複雜度：`O(n)`（建立了 `ans`）  
- 時間複雜度：`O(n)`（兩次遍歷，`O(2n)` 簡化為 `O(n)`）  

雖然可行，但總操作數不夠理想。面試時可先說這個解法打底，再推進到最佳化解法。

## 方法二（空間最佳、操作次優）[Accepted]

這個方法與上面一樣，先滿足一個需求再滿足另一個；差別是更巧妙。題目也可改寫成：「把所有非零元素依序搬到陣列前面」。

使用雙指標：  
- 快指標 `nums[i]`：負責掃描新元素  
- 慢指標 `lastNonZeroFoundAt`：記錄下一個非零元素應該放的位置  

當掃到非零元素，就把它寫到 `lastNonZeroFoundAt`，再遞增該指標。這樣覆寫不會丟資料，因為被覆寫位置要嘛已處理過，要嘛是 `0`，之後會處理。

掃描完後，非零元素已依序在前段；最後把 `lastNonZeroFoundAt` 之後全部補 `0`。

**演算法：**

1. `lastNonZeroFoundAt = 0`。  
2. 遍歷 `nums`：  
   - 若 `nums[i] != 0`：  
     - `nums[lastNonZeroFoundAt] = nums[i]`  
     - `lastNonZeroFoundAt++`  
3. 最後把 `lastNonZeroFoundAt` 到結尾全部設為 `0`。

**複雜度分析：**

- 空間複雜度：`O(1)`  
- 時間複雜度：`O(n)`（最壞仍可視為 `O(2n)`，簡化 `O(n)`）  

但寫入次數仍不算最佳，總陣列寫入次數大約是 `n`。

## 方法三（最佳）[Accepted]

前一種方法的總操作數仍可再降。比如 `[0,0,0,...,0,1]`：方法二會寫很多次 `0`，其實沒必要；只需要把非零元素放到正確位置即可。

最佳法是方法二的延伸：  
若目前元素是非零，它的正確位置只可能是「現在」或「更前面」。若要往前移，當前位置之後反正會被其他元素或 `0` 填上，因此可直接用交換（swap）維持不變量。

維持以下不變量：

1. 慢指標 `lastNonZeroFoundAt` 前面的元素都非零。  
2. 慢指標到目前掃描位置之間都是 `0`。  

所以遇到非零元素時，交換 `nums[lastNonZeroFoundAt]` 與 `nums[cur]`，再移動慢指標；若是 `0`，只移動目前指標。

**演算法：**

1. `lastNonZeroFoundAt = 0`。  
2. 以 `cur` 遍歷 `nums`：  
   - 若 `nums[cur] != 0`：  
     - 交換 `nums[lastNonZeroFoundAt]` 與 `nums[cur]`  
     - `lastNonZeroFoundAt++`  
3. 完成後，非零元素在前、`0` 在後。

**複雜度分析：**

- 空間複雜度：`O(1)`  
- 時間複雜度：`O(n)`  
- 寫入操作數更佳：大致與「非零元素數量」相關。  
  - 當 `0` 很多時，最佳情況明顯優於方法二  
  - 當全部都是非零時，最壞情況與方法二相近

分析作者：`@spandan.pathak`