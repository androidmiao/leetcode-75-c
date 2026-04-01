# 724. Find Pivot Index - Editorial

## Approach #1: Prefix Sum [Accepted]

### Intuition

The core challenge is efficiently determining, for each position in the array, the sum of all elements to its left and the sum of all elements to its right. Rather than recomputing these sums repeatedly for each index, we can leverage a clever observation: if we know the total sum of the array and the cumulative sum up to a given index, we can derive the right sum in constant time.

### Algorithm

Let `totalSum` represent the sum of all elements in the array. When examining index `i`, we track `leftSum` — the cumulative sum of all elements before index `i`.

At any index `i`, the sum of elements to the right is calculated as:
```
rightSum = totalSum - leftSum - nums[i]
```

A pivot index is found when `leftSum == rightSum`. This condition becomes:
```
leftSum == totalSum - leftSum - nums[i]
```

We iterate through each index exactly once:
1. Check if the current index is a pivot by comparing `leftSum` with the calculated `rightSum`
2. If a pivot is found, return that index immediately
3. If not, add the current element to `leftSum` and continue to the next index
4. If no pivot exists after checking all indices, return -1

### Implementation

**Java:**
```java
class Solution {
    public int pivotIndex(int[] nums) {
        int totalSum = 0;
        for (int num : nums) {
            totalSum += num;
        }

        int leftSum = 0;
        for (int i = 0; i < nums.length; i++) {
            if (leftSum == totalSum - leftSum - nums[i]) {
                return i;
            }
            leftSum += nums[i];
        }

        return -1;
    }
}
```

**Python:**
```python
class Solution:
    def pivotIndex(self, nums: List[int]) -> int:
        total_sum = sum(nums)
        left_sum = 0

        for i in range(len(nums)):
            if left_sum == total_sum - left_sum - nums[i]:
                return i
            left_sum += nums[i]

        return -1
```

### Complexity Analysis

- **Time Complexity:** O(N), where N is the length of the input array. We traverse the array exactly twice: once to compute the total sum, and once to check each index for the pivot condition.
- **Space Complexity:** O(1), as we only use a constant amount of extra space for the `leftSum` and `totalSum` variables, regardless of input size.

---

# 724. 尋找樞紐指數 - 解題指南

## 方法 #1: 前綴和 [可接受]

### 直觀理解

核心問題是需要有效率地計算陣列中每個位置左側和右側的元素總和。與其反覆重新計算，我們可以利用一個巧妙的觀察：若已知陣列的總和以及某個位置之前的累積和，就能以常數時間計算該位置右側的總和。

### 演算法

設 `totalSum` 為陣列所有元素的總和。當檢查索引 `i` 時，我們維護 `leftSum` —— 在索引 `i` 之前所有元素的累積和。

在任何索引 `i` 處，右側元素的總和可以計算為：
```
rightSum = totalSum - leftSum - nums[i]
```

樞紐指數出現在 `leftSum == rightSum` 的位置。這個條件轉換為：
```
leftSum == totalSum - leftSum - nums[i]
```

我們逐一遍歷每個索引：
1. 透過比較 `leftSum` 與計算出的 `rightSum` 來檢查目前索引是否為樞紐
2. 若找到樞紐，立即返回該索引
3. 若未找到，將目前元素加入 `leftSum` 並移往下一個索引
4. 若檢查完所有索引後仍未找到樞紐，返回 -1

### 實現方式

**Java:**
```java
class Solution {
    public int pivotIndex(int[] nums) {
        int totalSum = 0;
        for (int num : nums) {
            totalSum += num;
        }

        int leftSum = 0;
        for (int i = 0; i < nums.length; i++) {
            if (leftSum == totalSum - leftSum - nums[i]) {
                return i;
            }
            leftSum += nums[i];
        }

        return -1;
    }
}
```

**Python:**
```python
class Solution:
    def pivotIndex(self, nums: List[int]) -> int:
        total_sum = sum(nums)
        left_sum = 0

        for i in range(len(nums)):
            if left_sum == total_sum - left_sum - nums[i]:
                return i
            left_sum += nums[i]

        return -1
```

### 複雜度分析

- **時間複雜度:** O(N)，其中 N 為輸入陣列的長度。我們共遍歷陣列兩次：一次用來計算總和，一次用來檢查每個索引是否為樞紐。
- **空間複雜度:** O(1)，因為我們僅使用常數量的額外空間來存放 `leftSum` 和 `totalSum` 變數，與輸入大小無關。
