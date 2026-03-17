Approach 1: Brute Force

Algorithm

The brute force approach is simple. Loop through each element `x` and find if there is another value that equals `target - x`.

Implementation

Use two nested loops. For each index `i`, scan the rest of the array to find an index `j` such that `nums[i] + nums[j] == target`.

Complexity Analysis

Time complexity: `O(n^2)`.
For each element, we try to find its complement by looping through the rest of the array, which takes `O(n)` time. Therefore, the overall time complexity is `O(n^2)`.

Space complexity: `O(1)`.
The space required does not depend on the size of the input array, so only constant extra space is used.

Approach 2: Two-pass Hash Table

Intuition

To improve runtime complexity, we need a more efficient way to check whether the complement exists in the array. If the complement exists, we also need its index. A hash table is a good way to maintain a mapping from each number to its index.

We can reduce the lookup time from `O(n)` to near `O(1)` by trading space for speed. A hash table supports fast lookup in average cases. In the worst case, collisions can make lookup slower, but with a reasonable hash function, the amortized cost is still close to constant time.

Algorithm

This method uses two passes through the array. In the first pass, store each element and its index in the hash table. In the second pass, for each element, check whether its complement `target - nums[i]` exists in the hash table. If it exists and is not the same element, return the two indices.

Implementation

Build the hash table first, then scan the array again to search for complements.

Complexity Analysis

Time complexity: `O(n)`.
We traverse the array twice, and each hash table lookup is near `O(1)` on average. Therefore, the total time complexity is `O(n)`.

Space complexity: `O(n)`.
The extra space comes from the hash table, which stores up to `n` elements.

Approach 3: One-pass Hash Table

Algorithm

We can do even better in practice with one pass. While iterating through the array, we first check whether the current number's complement already exists in the hash table. If it does, we immediately return the answer. Otherwise, we insert the current number and its index into the hash table and continue.

Implementation

For each index `i`:
1. Compute `complement = target - nums[i]`.
2. Search the hash table for `complement`.
3. If found, return the stored index and `i`.
4. If not found, insert `nums[i]` and `i` into the hash table.

This is the approach used in the C solution file.

Complexity Analysis

Time complexity: `O(n)`.
We traverse the array only once, and each lookup or insertion in the hash table is near `O(1)` on average.

Space complexity: `O(n)`.
The hash table stores at most `n` elements, so the extra space is linear.


---
解法一：暴力法（Brute Force）

演算法

暴力法的概念很直接。逐一走訪每個元素 `x`，再去找陣列中是否存在另一個數值等於 `target - x`。

實作方式

使用兩層迴圈。對每個索引 `i`，再往後掃描其餘元素，尋找某個索引 `j`，使得 `nums[i] + nums[j] == target`。

複雜度分析

時間複雜度：`O(n^2)`。
對於每一個元素，都需要再掃描陣列剩下的部分來尋找補數，因此每次可能花費 `O(n)` 時間，所以整體時間複雜度是 `O(n^2)`。

空間複雜度：`O(1)`。
所需額外空間不會隨輸入陣列大小增加，因此只使用常數額外空間。

解法二：兩次走訪雜湊表（Two-pass Hash Table）

直覺

如果想改善執行時間，就需要一種更有效率的方法來判斷補數是否存在於陣列中。如果補數存在，我們還需要知道它的索引。雜湊表很適合用來維護「數值對應索引」這種映射關係。

透過以空間換時間，我們可以把查找時間從 `O(n)` 降到平均接近 `O(1)`。雜湊表在一般情況下能提供快速查找。雖然碰撞嚴重時最差情況可能退化，但只要雜湊函式設計合理，平均攤銷成本仍然接近常數時間。

演算法

這個方法需要走訪陣列兩次。第一次走訪時，先把每個元素和它的索引存進雜湊表。第二次走訪時，對每個元素檢查它的補數 `target - nums[i]` 是否存在於雜湊表中。如果存在，而且不是同一個元素，就回傳這兩個索引。

實作方式

先建立完整的雜湊表，再重新掃描一次陣列尋找補數。

複雜度分析

時間複雜度：`O(n)`。
我們走訪陣列兩次，而每次雜湊表查找平均接近 `O(1)`，因此總時間複雜度是 `O(n)`。

空間複雜度：`O(n)`。
額外空間主要來自雜湊表，最多會存放 `n` 個元素。

解法三：一次走訪雜湊表（One-pass Hash Table）

演算法

實務上還可以更精簡成一次走訪。當我們掃描陣列時，先檢查目前數字所需要的補數是否已經存在於雜湊表中。如果存在，就代表已經找到答案，可以立刻回傳。若不存在，再把目前的數字和索引存進雜湊表，繼續往下掃描。

實作方式

對每個索引 `i`：
1. 計算 `complement = target - nums[i]`。
2. 到雜湊表中查找 `complement`。
3. 如果找到了，就回傳先前儲存的索引以及目前的 `i`。
4. 如果還沒找到，就把 `nums[i]` 和 `i` 存進雜湊表。

這也是 `1_Two Sum.c` 所採用的做法。

複雜度分析

時間複雜度：`O(n)`。
整個陣列只需走訪一次，而每次雜湊表的查找或插入平均接近 `O(1)`。

空間複雜度：`O(n)`。
雜湊表最多儲存 `n` 個元素，所以額外空間是線性的。
