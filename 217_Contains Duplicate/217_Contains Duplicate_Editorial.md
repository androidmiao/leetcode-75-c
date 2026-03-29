# 217. Contains Duplicate - Editorial

> Content synced from the live authenticated LeetCode Editorial page (2026-03-29).

## Summary

This article covers three approaches for beginners, introducing the ideas of Loop Invariant, Linear Search, Sorting, and Hash Table.

## Approach #1: Naive Linear Search [Time Limit Exceeded]

### Intuition

For an array of n integers, there are C(n, 2) = n(n-1)/2 pairs of integers. We may check all pairs and see if any pair has duplicates.

### Algorithm

We employ the linear search algorithm, the simplest search algorithm. For the i-th integer `nums[i]`, we search in the previous `i-1` integers for a duplicate of `nums[i]`. If we find one, we return true; if not, we continue. Return false at the end of the program.

To prove correctness, we define the loop invariant: before the next search, there are no duplicate integers in the searched integers. The invariant holds true before the loop because there is no searched integer. Each time through the loop we look for any possible duplicate of the current element. If we found a duplicate, the function exits by returning true; if not, the invariant still holds true. Therefore, if the loop finishes, the invariant tells us there is no duplicate in all n integers.

### Complexity Analysis

- **Time complexity**: O(n²). In the worst case, there are n(n-1)/2 pairs of integers to check. Therefore, the time complexity is O(n²).
- **Space complexity**: O(1). We only used constant extra space.

### Note

This approach will get Time Limit Exceeded on LeetCode. Usually, if an algorithm is O(n²), it can handle n up to around 10⁴. It gets Time Limit Exceeded when n >= 10⁵.

## Approach #2: Sorting [Accepted]

### Intuition

If there are any duplicate integers, they will be consecutive after sorting.

### Algorithm

This approach employs sorting algorithm. Since comparison sorting algorithms like heapsort are known to provide O(n log n) worst-case performance, sorting is often a good preprocessing step. After sorting, we can sweep the sorted array to find if there are any two consecutive duplicate elements.

### Code

```c
int compare(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

bool containsDuplicate(int* nums, int numsSize) {
    if (numsSize <= 1) return false;

    qsort(nums, numsSize, sizeof(int), compare);

    for (int i = 0; i < numsSize - 1; i++) {
        if (nums[i] == nums[i + 1]) {
            return true;
        }
    }

    return false;
}
```

### Complexity Analysis

- **Time complexity**: O(n log n). Sorting is O(n log n) and the sweeping is O(n). The entire algorithm is dominated by the sorting step, which is O(n log n).
- **Space complexity**: O(1). Space depends on the sorting implementation which, usually, costs O(1) auxiliary space if `heapsort` is used.

### Note

The implementation here modifies the original array by sorting it. In general, it is not a good practice to modify the input unless it is clear to the caller that the input will be modified. One can make a copy of `nums` and operate on the copy instead.

## Approach #3: Hash Table [Accepted]

### Intuition

Utilize a dynamic data structure that supports fast search and insert operations.

### Algorithm

From Approach #1 we know that search operations in an unsorted array are O(n) and we did so repeatedly. Utilizing a data structure with faster search time will speed up the entire algorithm.

There are many data structures commonly used as dynamic sets such as Binary Search Tree and Hash Table. The operations we need to support here are `search()` and `insert()`. For a self-balancing Binary Search Tree (TreeSet or TreeMap in Java), `search()` and `insert()` are both O(log n) time. For a Hash Table (HashSet or HashMap in Java), `search()` and `insert()` are both O(1) on average. Therefore, by using hash table, we can achieve linear time complexity for finding duplicates in an unsorted array.

### Complexity Analysis

- **Time complexity**: O(n). We do `search()` and `insert()` for n times and each operation takes constant time.
- **Space complexity**: O(n). The space used by a hash table is linear with the number of elements in it.

### Note

For certain test cases with not very large n, the runtime of this method can be slower than Approach #2. The reason is hash table has some overhead in maintaining its property. One should keep in mind that real world performance can be different from what the Big-O notation says. The Big-O notation only tells us that for *sufficiently* large input, one will be faster than the other. Therefore, when n is not sufficiently large, an O(n) algorithm can be slower than an O(n log n) algorithm.

## Comparison

| Approach | Time | Space | Status | Pros | Cons |
|----------|------|-------|--------|------|------|
| Naive Linear Search | O(n²) | O(1) | TLE | Simple logic, loop invariant proof | Too slow for n >= 10⁵ |
| Sorting | O(n log n) | O(1) | Accepted | No extra space, heapsort guarantees worst-case | Modifies input |
| Hash Table | O(n) | O(n) | Accepted | Optimal time | Extra space, overhead for small n |

## See Also

- Problem 219: Contains Duplicate II
- Problem 220: Contains Duplicate III

---

# 217. 包含重複元素 - 題解

> 內容同步自 LeetCode 官方 Editorial 頁面（已驗證登入，2026-03-29）。

## 摘要

本文針對初學者介紹三種方法，涉及迴圈不變式（Loop Invariant）、線性搜尋、排序與雜湊表等概念。

## 方法一：樸素線性搜尋 [超時]

### 直覺

對於 n 個整數的陣列，共有 C(n, 2) = n(n-1)/2 對整數。可以檢查所有配對來判斷是否有重複。

### 演算法

採用最簡單的線性搜尋演算法。對於第 i 個整數 `nums[i]`，在前面 `i-1` 個整數中搜尋是否有 `nums[i]` 的重複。若找到則回傳 true；否則繼續。程式結束時回傳 false。

為證明正確性，定義迴圈不變式：在下一次搜尋之前，已搜尋的整數中沒有重複。此不變式在迴圈開始前成立（因為沒有已搜尋的整數）。每次迴圈中尋找當前元素的可能重複。若找到重複，函式回傳 true 結束；否則不變式仍然成立。因此，若迴圈完成，不變式告訴我們所有 n 個整數中沒有重複。

### 複雜度分析

- **時間複雜度**：O(n²)。最壞情況下需檢查 n(n-1)/2 對整數。
- **空間複雜度**：O(1)。僅使用常數額外空間。

### 注意

此方法在 LeetCode 上會超時（TLE）。通常 O(n²) 演算法可處理 n 至約 10⁴。當 n >= 10⁵ 時會超時。

## 方法二：排序 [通過]

### 直覺

若有重複整數，排序後它們會相鄰。

### 演算法

此方法使用排序演算法。由於比較排序（如 heapsort）已知提供 O(n log n) 最壞情況效能，排序常作為良好的預處理步驟。排序後，掃描已排序陣列以查找是否有兩個連續的重複元素。

### 複雜度分析

- **時間複雜度**：O(n log n)。排序為 O(n log n)，掃描為 O(n)。整體由排序步驟主導，為 O(n log n)。
- **空間複雜度**：O(1)。空間取決於排序實作，若使用 `heapsort` 通常為 O(1) 輔助空間。

### 注意

此實作會透過排序修改原始陣列。一般而言，除非呼叫者明確知道輸入會被修改，否則不建議修改輸入。可以先複製 `nums` 再對副本操作。

## 方法三：雜湊表 [通過]

### 直覺

利用支援快速搜尋和插入操作的動態資料結構。

### 演算法

從方法一可知，在未排序陣列中的搜尋操作為 O(n)，且重複執行多次。使用搜尋時間更快的資料結構將加速整個演算法。

常用的動態集合資料結構包括二元搜尋樹和雜湊表。此處需要支援的操作為 `search()` 和 `insert()`。對於自平衡二元搜尋樹，兩個操作皆為 O(log n)。對於雜湊表，兩個操作平均皆為 O(1)。因此，使用雜湊表可在未排序陣列中以線性時間找到重複。

### 複雜度分析

- **時間複雜度**：O(n)。對 n 個元素執行 `search()` 和 `insert()`，每次操作為常數時間。
- **空間複雜度**：O(n)。雜湊表使用的空間與其中元素數量成線性關係。

### 注意

對於 n 不是很大的測試案例，此方法的執行時間可能比方法二慢。原因是雜湊表在維護其特性時有額外開銷。應記住，實際效能可能與 Big-O 表示法不同。Big-O 僅告訴我們對*足夠大*的輸入，哪個更快。因此，當 n 不夠大時，O(n) 演算法可能比 O(n log n) 演算法慢。

## 比較

| 方法 | 時間 | 空間 | 狀態 | 優點 | 缺點 |
|------|------|------|------|------|------|
| 樸素線性搜尋 | O(n²) | O(1) | 超時 | 邏輯簡單、可證明迴圈不變式 | n >= 10⁵ 時太慢 |
| 排序 | O(n log n) | O(1) | 通過 | 無額外空間、heapsort 保證最壞情況 | 修改輸入 |
| 雜湊表 | O(n) | O(n) | 通過 | 最佳時間 | 額外空間、小 n 時有開銷 |

## 另見

- Problem 219：Contains Duplicate II
- Problem 220：Contains Duplicate III
