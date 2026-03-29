# 53. Maximum Subarray - Editorial

## English

### Approach 1: Optimized Brute Force

**Intuition:**
The most straightforward approach is to calculate the sum of all possible subarrays and track the maximum. A naive brute force solution would be O(N³) because we check all possible subarrays and calculate their sums. However, we can optimize this to O(N²) by recognizing that subarrays starting at the same index share a common prefix—we can calculate the next subarray's sum by simply adding the next element to the current sum.

**Algorithm:**
1. Initialize `maxSubarray = -infinity`
2. Use a for loop considering each index as the starting point of a subarray
3. For each starting point, create `currentSubarray = 0` and loop from the starting index, adding elements to `currentSubarray`
4. Continuously update `maxSubarray` with the maximum of `maxSubarray` and `currentSubarray`
5. Return `maxSubarray`

**Complexity:**
- Time: O(N²) - outer loop runs N times, inner loop runs up to N times
- Space: O(1) - only using constant extra space

---

### Approach 2: Dynamic Programming, Kadane's Algorithm (Recommended)

**Intuition:**
When you encounter a max/min optimization problem, consider dynamic programming. The key insight is deciding when a negative number is "worth" keeping. This approach uses Kadane's Algorithm with a greedy-like intuition.

The fundamental question is: at each position, where should the optimal subarray begin? If the prefix sum up to the current position is negative, it will always subtract from our total, so we should reset and start fresh. We maintain a running sum variable `currentSubarray` that tracks the best sum ending at the current position. When this value becomes negative, we reset it to 0 (representing an empty subarray). We update our global maximum `maxSubarray` every time we find a new maximum.

**Algorithm:**
1. Initialize two integer variables equal to the first element: `currentSubarray` (running sum) and `maxSubarray` (best sum seen so far)
2. Iterate from the second element onwards
3. Add the current element to `currentSubarray`
4. If `currentSubarray` becomes negative, reset it to 0 (discard the prefix)
5. Update `maxSubarray` with the maximum of itself and `currentSubarray` at each step
6. Return `maxSubarray`

**Implementation Trick:**
A concise way to implement this is: `currentSubarray = max(num, currentSubarray + num)`

If `currentSubarray` is negative, then `num > currentSubarray + num`, so we start fresh from the current element.

**Complexity:**
- Time: O(N) - single pass through the array
- Space: O(1) - only two variables

---

### Approach 3: Divide and Conquer (Advanced)

**Intuition:**
While slower than Kadane's Algorithm, divide and conquer is a good alternative interview approach that uses recursion. The key insight is that the optimal subarray either:
- Uses elements from the left half only, or
- Uses elements from the right half only, or
- Spans across both halves, crossing the midpoint

The answer is the largest of these three cases.

To find the best subarray crossing the midpoint, we iterate from the middle to the left end, tracking the running sum with a counter, then iterate from the middle to the right end, and combine the best left sum and best right sum.

**Algorithm:**
1. Define a helper function `findBestSubarray(left, right)` for recursion
2. Base case: if `left > right`, return `INT_MIN`
3. Find `mid = (left + right) / 2`
4. Iterate from `mid - 1` towards `left`, tracking `bestLeftSum` (the maximum subarray sum ending at `mid - 1`)
5. Iterate from `mid + 1` towards `right`, tracking `bestRightSum` (the maximum subarray sum starting at `mid + 1`)
6. Calculate the maximum subarray crossing the midpoint: `bestLeftSum + nums[mid] + bestRightSum`
7. Return the maximum of:
   - The crossing subarray sum
   - Recursive call on left half: `findBestSubarray(left, mid - 1)`
   - Recursive call on right half: `findBestSubarray(mid + 1, right)`
8. Call the helper function with the full array (`left = 0`, `right = length - 1`)

**Complexity:**
- Time: O(N log N) - similar to merge sort; we divide into halves and process N elements at each level
- Space: O(log N) - recursion stack depth

---

## Traditional Chinese (繁體中文)

### 方法 1: 優化的暴力法

**直覺:**
最直接的方法是計算所有可能子陣列的總和，並追蹤最大值。天真的暴力解法時間複雜度為 O(N³)，因為我們檢查所有可能的子陣列並計算其總和。但是，我們可以將其優化到 O(N²)，通過認識到從同一索引開始的子陣列共享公共前綴——我們可以通過簡單地將下一個元素加到當前總和來計算下一個子陣列的總和。

**算法:**
1. 初始化 `maxSubarray = -無窮大`
2. 使用 for 迴圈，將每個索引視為子陣列的起始點
3. 對於每個起始點，創建 `currentSubarray = 0`，並從起始索引循環，將元素加到 `currentSubarray`
4. 持續用 `maxSubarray` 和 `currentSubarray` 的最大值更新 `maxSubarray`
5. 返回 `maxSubarray`

**複雜度:**
- 時間: O(N²) - 外迴圈運行 N 次，內迴圈最多運行 N 次
- 空間: O(1) - 只使用常數額外空間

---

### 方法 2: 動態規劃，Kadane 算法（推薦）

**直覺:**
當遇到最大/最小值優化問題時，考慮動態規劃。關鍵洞察是判斷何時負數「值得」保留。這種方法使用 Kadane 算法，具有類似貪心的直覺。

根本問題是：在每個位置，最優子陣列應該從哪裡開始？如果到當前位置的前綴和為負，它將始終從我們的總和中扣除，所以我們應該重置並重新開始。我們維護一個運行和變數 `currentSubarray`，追蹤以當前位置結尾的最佳總和。當此值變為負時，我們將其重置為 0（表示空子陣列）。我們在每次找到新的最大值時更新全局最大值 `maxSubarray`。

**算法:**
1. 初始化兩個整數變數，都等於第一個元素：`currentSubarray`（運行總和）和 `maxSubarray`（迄今為止最佳總和）
2. 從第二個元素開始迭代
3. 將當前元素加到 `currentSubarray`
4. 如果 `currentSubarray` 變為負數，將其重置為 0（丟棄前綴）
5. 在每一步用 `currentSubarray` 的最大值更新 `maxSubarray`
6. 返回 `maxSubarray`

**實現技巧:**
一種簡潔的實現方式是：`currentSubarray = max(num, currentSubarray + num)`

如果 `currentSubarray` 為負，則 `num > currentSubarray + num`，所以我們從當前元素重新開始。

**複雜度:**
- 時間: O(N) - 單次遍歷陣列
- 空間: O(1) - 只需要兩個變數

---

### 方法 3: 分治法（進階）

**直覺:**
雖然比 Kadane 算法慢，但分治法是一個很好的備選面試方法，它使用遞歸。關鍵洞察是最優子陣列要麼：
- 僅使用左半部分的元素，要麼
- 僅使用右半部分的元素，要麼
- 跨越兩個半部分，穿過中點

答案是這三種情況中最大的。

為了找到跨越中點的最優子陣列，我們從中點向左端迭代，用計數器追蹤運行總和，然後從中點向右端迭代，並結合最佳左總和和最佳右總和。

**算法:**
1. 定義遞歸的輔助函數 `findBestSubarray(left, right)`
2. 基本情況：如果 `left > right`，返回 `INT_MIN`
3. 找到 `mid = (left + right) / 2`
4. 從 `mid - 1` 向 `left` 迭代，追蹤 `bestLeftSum`（以 `mid - 1` 結尾的最大子陣列總和）
5. 從 `mid + 1` 向 `right` 迭代，追蹤 `bestRightSum`（從 `mid + 1` 開始的最大子陣列總和）
6. 計算跨越中點的最大子陣列：`bestLeftSum + nums[mid] + bestRightSum`
7. 返回以下項的最大值：
   - 跨越子陣列總和
   - 左半部分的遞歸呼叫：`findBestSubarray(left, mid - 1)`
   - 右半部分的遞歸呼叫：`findBestSubarray(mid + 1, right)`
8. 使用完整陣列呼叫輔助函數（`left = 0`，`right = length - 1`）

**複雜度:**
- 時間: O(N log N) - 類似於歸併排序；我們分成兩半，並在每一級處理 N 個元素
- 空間: O(log N) - 遞歸棧深度
