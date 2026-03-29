# LeetCode 287: Find the Duplicate Number - Editorial

---

## English Version

### Overview

Finding the Duplicate Number is a classic problem that demonstrates multiple algorithmic approaches across different design paradigms. This editorial presents 7 distinct approaches, with varying trade-offs between time complexity, space complexity, and array modification.

**Mathematical Foundation:** Why must a duplicate exist? The pigeonhole principle provides the answer: given n+1 numbers to place into n pigeonholes (each representing values 1 through n), at least one pigeonhole must contain 2 or more numbers. Therefore, a duplicate is guaranteed to exist.

The first four approaches modify the array during execution and do not satisfy the problem's constraints, but they represent important algorithmic concepts. The remaining three approaches maintain the original array integrity and offer increasingly optimal solutions.

---

### Approach 1: Sort

**Note:** This approach modifies the input array and does not satisfy the problem constraints of O(1) space and no array modification.

**Intuition:**
Sorting arranges equal elements adjacent to one another. By sorting the array, duplicate numbers will be placed next to each other, making detection straightforward.

**Algorithm:**
1. Sort the array in ascending order
2. Iterate through the sorted array comparing each element with its predecessor
3. When nums[i] == nums[i-1], return that element as the duplicate

**Complexity Analysis:**
- **Time Complexity:** O(n log n) due to sorting
- **Space Complexity:** O(log n) or O(n) depending on the sorting algorithm implementation (quicksort with recursion vs. merge sort)

---

### Approach 2: Set (HashSet)

**Note:** This approach uses O(n) additional space and does not satisfy the space constraint.

**Intuition:**
Maintain a set of seen values. As we iterate through the array, we check if the current element already exists in the set. If it does, we've found the duplicate.

**Algorithm:**
1. Initialize an empty hash set
2. Iterate through each element in the array
3. Check if the element exists in the set
4. If yes, return that element (it's the duplicate)
5. If no, add the element to the set and continue

**Complexity Analysis:**
- **Time Complexity:** O(n) - single pass through the array with O(1) set operations
- **Space Complexity:** O(n) - worst case stores up to n elements in the set

---

### Approach 3: Negative Marking

**Note:** This approach temporarily modifies the array by flipping signs and does not satisfy the strict constraint of no modification. However, it restores the array afterward.

**Intuition:**
Since all values are guaranteed to be in the range [1, n], we can use the array itself as a marker. Specifically, we use the sign of nums[|value|] to indicate whether that value has been encountered. The first time we encounter a negative number at position nums[|cur|], we know cur is the duplicate.

**Algorithm:**
1. Iterate through each element in the array
2. Get the absolute value of the current element: cur = abs(nums[i])
3. Check the sign of nums[cur]:
   - If nums[cur] < 0, we've seen this value before → return cur (duplicate found)
   - If nums[cur] >= 0, mark it as seen by negating: nums[cur] *= -1
4. After finding the duplicate, restore all signs in the array to leave it in original state

**Complexity Analysis:**
- **Time Complexity:** O(n) - single pass through the array
- **Space Complexity:** O(1) - only uses the input array as workspace

---

### Approach 4.1: Array as HashMap (Recursive Approach)

**Note:** This approach modifies the array and uses O(n) space due to the recursion call stack. It does not meet the constraints.

**Intuition:**
Treat the array as a hash map where each value (in range [1,n]) should map to its corresponding index. Since we have n+1 numbers and only n valid index positions, at least two numbers must map to the same index. We recursively place each number at its correct index until we find a position where the number already exists - that's our duplicate.

**Algorithm:**
1. Starting from index 0, recursively call store(nums, cur) where cur = nums[0]
2. In the recursive function:
   - If cur equals the index we're trying to place it at, we've found the duplicate
   - Otherwise, swap nums[cur] with nums[index] and recursively call store with the new value
3. The first value that equals its index is the duplicate

**Complexity Analysis:**
- **Time Complexity:** O(n) - each element is moved at most once
- **Space Complexity:** O(n) - due to recursion call stack depth

---

### Approach 4.2: Array as HashMap (Iterative Approach - Cyclic Sort)

**Note:** Also known as cyclic sort. This approach modifies the array and does not satisfy constraints.

**Intuition:**
This is the iterative version of Approach 4.1. Instead of recursion, we use a while loop to continuously swap nums[0] with nums[nums[0]] until we find the duplicate (when nums[0] == nums[nums[0]]).

**Algorithm:**
```
while nums[0] != nums[nums[0]]:
    swap(nums[0], nums[nums[0]])
return nums[0]
```

The loop terminates when nums[0] reaches a position where the value at that position equals the value at index 0, indicating the duplicate.

**Complexity Analysis:**
- **Time Complexity:** O(n) - each position is processed in constant time overall
- **Space Complexity:** O(1) - only swap operations, no extra data structure needed
- **Note:** Modifies the array, violating problem constraints

---

### Approach 5: Binary Search on Value Range

**Intuition:**
Rather than binary searching on indices, we binary search on the value range [1, n]. For each candidate value mid, we count how many numbers in the array are less than or equal to mid. This count function is monotonic: as mid increases, the count never decreases. If count > mid, the duplicate must be in the range [1, mid]. Otherwise, it's in (mid, n].

**Algorithm:**
1. Initialize left = 1, right = n
2. While left < right:
   - mid = (left + right) / 2
   - Count how many elements in nums are <= mid
   - If count > mid, the duplicate is in [left, mid], so right = mid
   - Otherwise, the duplicate is in (mid, right], so left = mid + 1
3. Return left when left == right

**Key Insight:** The count function is strictly monotonic because:
- For values [1, n], count of elements <= n in an (n+1)-sized array with all elements in [1,n] is always n+1
- For values [1, mid], if we have only duplicate numbers and no modifications, we can only have at most mid different values, so if count > mid, at least one value appears twice in the duplicate range

**Complexity Analysis:**
- **Time Complexity:** O(n log n) - O(log n) binary search iterations, each requiring O(n) counting
- **Space Complexity:** O(1) - no extra data structures needed
- **Note:** Meets space constraint but not the linear time requirement

---

### Approach 6: Sum of Set Bits

**Intuition:**
Compare the bit patterns of the actual array against a baseline array [1, 2, ..., n]. For each bit position, count how many numbers in the array have that bit set, and compare it to how many numbers in [1..n] have that bit set. If the actual count exceeds the baseline count, that bit must be set in the duplicate number. By reconstructing all bits, we recover the duplicate value.

**Algorithm:**
1. For each bit position from 0 to log(n):
   - Count how many numbers in [1..n] have that bit set (base_count)
   - Count how many numbers in nums have that bit set (nums_count)
   - If nums_count > base_count, set that bit in the result
2. Return the reconstructed result

**Explanation:** Since [1..n] contains each value exactly once, and nums contains the same values plus one duplicate, the extra count at each bit position can only come from the duplicate number.

**Complexity Analysis:**
- **Time Complexity:** O(n log n) - O(log n) bit positions, each requiring O(n) counting
- **Space Complexity:** O(1) - only storing the result integer
- **Note:** Meets space constraint but not linear time

---

### Approach 7: Floyd's Tortoise and Hare (Cycle Detection)

**Intuition:**
Interpret the array as a linked list where each element nums[i] points to index nums[i]. Starting from nums[0], this linked list structure contains a cycle. The duplicate number is precisely the entrance to this cycle.

**Mathematical Foundation:** Consider the linked list formed by following the pointers. Since all values are in [1, n] and the array has n+1 elements:
- Index 0 is never pointed to by any element (all values are 1 to n)
- Indices 1 to n are pointed to by elements, creating a connected structure
- Since we have n+1 elements mapping to n positions, there must be a cycle

The entrance to the cycle represents the value that appears more than once, because it's the index that's pointed to more than once.

**Algorithm:**

**Phase 1 - Find Intersection Point:**
1. Initialize tortoise = nums[0] (moves 1 step), hare = nums[0] (moves 2 steps)
2. Move tortoise: tortoise = nums[tortoise]
3. Move hare: hare = nums[nums[hare]]
4. Repeat until tortoise == hare (they meet at intersection point)

**Phase 2 - Find Cycle Entrance:**
1. Keep tortoise at current position
2. Reset hare to start: hare = nums[0]
3. Move both one step at a time: tortoise = nums[tortoise], hare = nums[hare]
4. Continue until tortoise == hare
5. Return the meeting point (this is the duplicate number)

**Mathematical Proof:**
Let:
- F = distance from start (index 0) to cycle entrance
- a = distance from cycle entrance to intersection point
- C = cycle length

After phase 1, both have traveled the same total time but different distances:
- Tortoise: F + a + (number of cycles) × C
- Hare: F + a + n × C (hare goes through n more cycles)
- Since hare travels 2x distance: 2(F + a) = F + a + n × C
- Therefore: F + a = n × C, which means F = n × C - a

In phase 2:
- Tortoise starts at intersection (distance F + a from start)
- Hare starts at index 0
- After F steps: tortoise reaches F + a + F = 2F + a = F + n × C = cycle entrance
- After F steps: hare reaches F = cycle entrance
- Both meet at the cycle entrance

**Complexity Analysis:**
- **Time Complexity:** O(n) - two linear passes through the sequence (finding intersection, then finding entrance)
- **Space Complexity:** O(1) - only two pointers needed
- **Optimal:** This approach satisfies all problem constraints: linear time, constant space, and no array modification

---

## 繁體中文版本

### 概述

尋找重複數字是一個經典問題,展示了多種演算法方法的應用。本編輯指南介紹了 7 種不同的解決方案,各方案在時間複雜度、空間複雜度和陣列修改方面有不同的權衡。

**數學基礎:** 為什麼必定存在重複數字?鴿籠原理提供了答案:給定 n+1 個數字要放入 n 個鴿籠(分別代表值 1 至 n),至少有一個鴿籠必須包含 2 個或更多數字。因此,重複數字的存在是必然的。

前四種方法在執行過程中會修改陣列,不符合問題的約束條件,但它們代表了重要的演算法概念。其餘三種方法保持原始陣列的完整性,並提供逐漸最佳化的解決方案。

---

### 方法 1: 排序

**注:** 此方法會修改輸入陣列,不符合 O(1) 空間複雜度和無陣列修改的問題約束。

**直覺:**
排序會將相等的元素放在彼此相鄰的位置。通過對陣列進行排序,重複的數字將被放在一起,使檢測變得簡單。

**演算法:**
1. 將陣列按升序排序
2. 迭代排序後的陣列,將每個元素與其前驅元素進行比較
3. 當 nums[i] == nums[i-1] 時,返回該元素作為重複值

**複雜度分析:**
- **時間複雜度:** O(n log n),由於排序操作
- **空間複雜度:** O(log n) 或 O(n),取決於排序演算法的實現(快速排序遞迴與合併排序)

---

### 方法 2: 集合(雜湊集合)

**注:** 此方法使用 O(n) 額外空間,不符合空間約束。

**直覺:**
維護一個已見值的集合。當我們遍歷陣列時,檢查當前元素是否已經存在於集合中。如果存在,我們找到了重複值。

**演算法:**
1. 初始化一個空的雜湊集合
2. 遍歷陣列中的每個元素
3. 檢查該元素是否存在於集合中
4. 如果存在,返回該元素(即重複值)
5. 如果不存在,將元素添加到集合中並繼續

**複雜度分析:**
- **時間複雜度:** O(n) - 單次遍歷陣列,集合操作為 O(1)
- **空間複雜度:** O(n) - 最壞情況下在集合中存儲 n 個元素

---

### 方法 3: 負號標記

**注:** 此方法通過翻轉符號暫時修改陣列,不符合嚴格的無修改約束。但最後會恢復陣列。

**直覺:**
由於所有值都保證在範圍 [1, n] 內,我們可以使用陣列本身作為標記。具體來說,我們使用 nums[|value|] 的符號來指示該值是否已被遇見。當我們在位置 nums[|cur|] 遇到負數時,我們知道 cur 是重複值。

**演算法:**
1. 遍歷陣列中的每個元素
2. 獲取當前元素的絕對值: cur = abs(nums[i])
3. 檢查 nums[cur] 的符號:
   - 如果 nums[cur] < 0,我們之前見過此值 → 返回 cur(找到重複值)
   - 如果 nums[cur] >= 0,通過求反來標記為已見: nums[cur] *= -1
4. 找到重複值後,恢復陣列中所有元素的符號,使其回到原始狀態

**複雜度分析:**
- **時間複雜度:** O(n) - 單次遍歷陣列
- **空間複雜度:** O(1) - 僅使用輸入陣列作為工作區

---

### 方法 4.1: 陣列作為雜湊表(遞迴方法)

**注:** 此方法會修改陣列,並由於遞迴呼叫堆棧使用 O(n) 空間。不符合約束。

**直覺:**
將陣列視為雜湊表,其中每個值(範圍在 [1,n] 內)應映射到對應的索引。由於我們有 n+1 個數字但只有 n 個有效的索引位置,至少兩個數字必須映射到同一索引。我們遞迴地將每個數字放在其正確的索引處,直到找到一個位置,其中該數字已經存在 - 那就是重複值。

**演算法:**
1. 從索引 0 開始,遞迴調用 store(nums, cur),其中 cur = nums[0]
2. 在遞迴函數中:
   - 如果 cur 等於我們嘗試將其放置的索引,我們找到了重複值
   - 否則,交換 nums[cur] 與 nums[index],並使用新值遞迴調用 store
3. 第一個等於其索引的值就是重複值

**複雜度分析:**
- **時間複雜度:** O(n) - 每個元素最多被移動一次
- **空間複雜度:** O(n) - 由於遞迴呼叫堆棧深度

---

### 方法 4.2: 陣列作為雜湊表(迭代方法 - 循環排序)

**注:** 也稱為循環排序。此方法會修改陣列,不符合約束。

**直覺:**
這是方法 4.1 的迭代版本。不使用遞迴,我們使用 while 迴圈持續交換 nums[0] 與 nums[nums[0]],直到找到重複值(當 nums[0] == nums[nums[0]] 時)。

**演算法:**
```
while nums[0] != nums[nums[0]]:
    swap(nums[0], nums[nums[0]])
return nums[0]
```

當 nums[0] 到達一個位置時,該位置的值等於索引 0 處的值,這表示找到了重複值。

**複雜度分析:**
- **時間複雜度:** O(n) - 整體上每個位置以常數時間處理
- **空間複雜度:** O(1) - 僅交換操作,不需要額外的資料結構
- **注:** 修改陣列,違反了問題約束

---

### 方法 5: 值範圍內的二分搜索

**直覺:**
不是對索引進行二分搜索,而是對值範圍 [1, n] 進行二分搜索。對於每個候選值 mid,我們計算陣列中有多少個數字小於或等於 mid。此計數函數是單調的:隨著 mid 增大,計數永遠不會減少。如果 count > mid,重複值必定在範圍 [1, mid] 內。否則,它在 (mid, n] 內。

**演算法:**
1. 初始化 left = 1, right = n
2. 當 left < right 時:
   - mid = (left + right) / 2
   - 計算 nums 中有多少個元素 <= mid
   - 如果 count > mid,重複值在 [left, mid] 內,所以 right = mid
   - 否則,重複值在 (mid, right] 內,所以 left = mid + 1
3. 當 left == right 時返回 left

**關鍵洞察:** 計數函數是嚴格單調的,因為:
- 對於值 [1, n],一個 (n+1) 大小的陣列中所有元素都在 [1,n] 內時,元素 <= n 的計數總是 n+1
- 對於值 [1, mid],如果我們只有重複數字且沒有修改,我們最多只能有 mid 個不同的值,所以如果 count > mid,至少有一個值在重複範圍內出現了兩次

**複雜度分析:**
- **時間複雜度:** O(n log n) - O(log n) 次二分搜索迭代,每次需要 O(n) 的計數
- **空間複雜度:** O(1) - 不需要額外的資料結構
- **注:** 符合空間約束但不符合線性時間要求

---

### 方法 6: 位集合和

**直覺:**
比較實際陣列的位模式與基準陣列 [1, 2, ..., n]。對於每個位位置,計算陣列中有多少個數字設置了該位,並將其與 [1..n] 中有多少個數字設置了該位進行比較。如果實際計數超過基準計數,該位必定在重複數字中被設置。通過重建所有位,我們恢復重複值。

**演算法:**
1. 對於從 0 到 log(n) 的每個位位置:
   - 計算 [1..n] 中有多少個數字設置了該位(base_count)
   - 計算 nums 中有多少個數字設置了該位(nums_count)
   - 如果 nums_count > base_count,在結果中設置該位
2. 返回重建的結果

**解釋:** 由於 [1..n] 包含每個值恰好一次,而 nums 包含相同的值加上一個重複值,每個位位置的額外計數只能來自重複數字。

**複雜度分析:**
- **時間複雜度:** O(n log n) - O(log n) 個位位置,每個需要 O(n) 的計數
- **空間複雜度:** O(1) - 僅存儲結果整數
- **注:** 符合空間約束但不符合線性時間要求

---

### 方法 7: Floyd 龜兔賽跑(循環檢測)

**直覺:**
將陣列解釋為一個連結串列,其中每個元素 nums[i] 指向索引 nums[i]。從 nums[0] 開始,這個連結串列結構包含一個循環。重複數字正好是這個循環的入口。

**數學基礎:** 考慮通過跟隨指標形成的連結串列。由於所有值都在 [1, n] 內,且陣列有 n+1 個元素:
- 索引 0 永遠不被任何元素指向(所有值都是 1 到 n)
- 索引 1 到 n 被元素指向,形成連接的結構
- 由於我們有 n+1 個元素映射到 n 個位置,必定存在一個循環

循環的入口代表出現多次的值,因為它是被指向多次的索引。

**演算法:**

**第 1 階段 - 尋找交點:**
1. 初始化 tortoise = nums[0](移動 1 步),hare = nums[0](移動 2 步)
2. 移動烏龜: tortoise = nums[tortoise]
3. 移動兔子: hare = nums[nums[hare]]
4. 重複直到 tortoise == hare(在交點相遇)

**第 2 階段 - 尋找循環入口:**
1. 保持烏龜在當前位置
2. 重置兔子到起點: hare = nums[0]
3. 以一步一步的方式移動兩者: tortoise = nums[tortoise], hare = nums[hare]
4. 繼續直到 tortoise == hare
5. 返回相遇點(這就是重複數字)

**數學證明:**
令:
- F = 從起點(索引 0)到循環入口的距離
- a = 從循環入口到交點的距離
- C = 循環長度

第 1 階段後,兩者花費相同的總時間但距離不同:
- 烏龜: F + a + (循環次數) × C
- 兔子: F + a + n × C(兔子通過 n 多個循環)
- 由於兔子旅行距離為 2 倍: 2(F + a) = F + a + n × C
- 因此: F + a = n × C,這意味著 F = n × C - a

在第 2 階段:
- 烏龜從交點開始(距起點 F + a)
- 兔子從索引 0 開始
- 經過 F 步後:烏龜到達 F + a + F = 2F + a = F + n × C = 循環入口
- 經過 F 步後:兔子到達 F = 循環入口
- 兩者在循環入口相遇

**複雜度分析:**
- **時間複雜度:** O(n) - 通過序列的兩次線性遍歷(尋找交點,然後尋找入口)
- **空間複雜度:** O(1) - 僅需要兩個指標
- **最優:** 此方法滿足所有問題約束:線性時間、常數空間,且不修改陣列

---
