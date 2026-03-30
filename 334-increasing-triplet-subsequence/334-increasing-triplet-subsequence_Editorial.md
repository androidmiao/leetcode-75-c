# Editorial: Increasing Triplet Subsequence

## Approach 1: Linear Scan with Two Tracking Variables

### Intuition

The core insight for solving this problem efficiently is to maintain a rolling pair of values representing the two smallest elements of a potential increasing triplet as we traverse the array. Rather than searching for all possible combinations, we can determine the existence of an increasing triplet in a single pass.

The strategy uses two variables:
- **first_num**: tracks the smallest element seen so far
- **second_num**: tracks the smallest element that comes after first_num

As we iterate through the array, three outcomes are possible for each new element:
1. The element is smaller than or equal to first_num—update first_num to maintain the minimum
2. The element is larger than first_num but smaller than or equal to second_num—update second_num
3. The element is larger than second_num—we've found an increasing triplet subsequence

The critical insight is that we don't need to track indices or actual subsequence ordering. We only need to prove that three increasing values exist somewhere in the array. This allows a subtle but valid property: first_num and second_num may not maintain temporal order (first_num could be updated to a value appearing after second_num's index), yet if we find a third value exceeding second_num, we know an increasing triplet existed because a smaller value must have preceded second_num.

### Behavior Analysis

**Descending array**: When nums is sorted in descending order, every element becomes the new minimum, repeatedly updating first_num. The second_num never advances beyond its initial state, so the method returns False.

**Ascending array**: The first element updates first_num. Subsequent larger elements update second_num when they exceed first_num. The next even larger element triggers the third condition, returning True immediately.

**Unsorted array**: The algorithm progressively narrows down to the smallest and second-smallest viable candidates, updating them opportunistically. Once any third element exceeds second_num, the answer is found.

### Example Walkthrough

Consider nums = [1, 2, 0, 3]:
- Index 0: n=1 → first_num becomes 1
- Index 1: n=2 → 2 > first_num, so second_num becomes 2
- Index 2: n=0 → 0 < first_num, so first_num becomes 0
- Index 3: n=3 → 3 > second_num → Return True

Note at index 3: even though first_num (0) comes after second_num's original assignment (2), we can still return True because an increasing triplet [1, 2, 3] existed prior to the update.

### Implementation

Initialize both tracking variables to the maximum possible integer value. Use a single pass through the array with conditional branching:

```java
int first_num = Integer.MAX_VALUE;
int second_num = Integer.MAX_VALUE;

for (int n : nums) {
    if (n <= first_num) {
        first_num = n;
    } else if (n <= second_num) {
        second_num = n;
    } else {
        // n > second_num, and second_num > first_num
        return true;
    }
}
return false;
```

The sequence of conditions ensures proper handling: we only consider updating second_num if we've already found a suitable first_num.

### Complexity Analysis

- **Time Complexity**: O(N), where N is the length of nums. The algorithm performs exactly one pass through the array.
- **Space Complexity**: O(1). Only two integer variables are used regardless of input size.

---

# 編輯講解：遞增三元子序列

## 方法一：單次掃描與雙變量跟蹤

### 直觀概念

解決此問題的高效關鍵在於，在遍歷陣列的過程中，持續維護兩個代表潛在遞增三元組最小兩個元素的追蹤值。與其窮舉所有組合，我們可以在單一輪次內確定遞增三元組的存在性。

策略採用兩個變數：
- **first_num**：記錄迄今最小的元素
- **second_num**：記錄在 first_num 之後出現的最小元素

在陣列迭代中，每個新元素會導致三種結果之一：
1. 元素小於等於 first_num——更新 first_num 以維持最小值
2. 元素大於 first_num 但小於等於 second_num——更新 second_num
3. 元素大於 second_num——找到遞增三元子序列

關鍵洞察在於，我們無需追蹤索引或實際子序列的時間順序。我們只需證明陣列中存在三個遞增值。這允許一個微妙但有效的特性：first_num 和 second_num 可能在時間順序上不保持遞增（first_num 可能被更新為在 second_num 指標之後出現的值），但若我們找到第三個值超過 second_num，我們就知道遞增三元組確實存在過，因為在 second_num 之前必定有更小的值。

### 行為分析

**遞減陣列**：當 nums 按遞減順序排列時，每個元素都成為新的最小值，反覆更新 first_num。second_num 始終保持初始狀態未改變，因此方法返回 False。

**遞增陣列**：第一個元素更新 first_num。後續較大的元素在超過 first_num 時更新 second_num。下一個更大的元素將觸發第三個條件，立即返回 True。

**無序陣列**：演算法逐步縮小至最小和次小的可行候選值，機會性地更新它們。一旦任何第三個元素超過 second_num，答案便已找到。

### 範例演練

考慮 nums = [1, 2, 0, 3]：
- 索引 0：n=1 → first_num 變為 1
- 索引 1：n=2 → 2 > first_num，因此 second_num 變為 2
- 索引 2：n=0 → 0 < first_num，因此 first_num 變為 0
- 索引 3：n=3 → 3 > second_num → 返回 True

注意在索引 3：儘管 first_num（0）出現在 second_num 原始指派位置之後，我們仍可返回 True，因為遞增三元組 [1, 2, 3] 在更新發生前確實存在過。

### 實作

將兩個追蹤變數初始化為最大可能的整數值。使用條件分支進行單次陣列遍歷：

```java
int first_num = Integer.MAX_VALUE;
int second_num = Integer.MAX_VALUE;

for (int n : nums) {
    if (n <= first_num) {
        first_num = n;
    } else if (n <= second_num) {
        second_num = n;
    } else {
        // n > second_num，且 second_num > first_num
        return true;
    }
}
return false;
```

條件序列的設計確保了適當的處理：我們只有在已經找到合適的 first_num 後，才考慮更新 second_num。

### 複雜度分析

- **時間複雜度**：O(N)，其中 N 是 nums 的長度。演算法恰好執行一次陣列遍歷。
- **空間複雜度**：O(1)。無論輸入大小如何，僅使用兩個整數變數。
