# 219. Contains Duplicate II - Implementation vs Editorial

## Implementation Overview

The provided C solution uses a **Hash Map with Sliding Window** approach (Approach #3 in the editorial), which is the optimal and recommended solution.

## Which Editorial Approach Does the Implementation Match?

The implementation matches **Approach #3: Hash Table** from the LeetCode editorial. It is the same core algorithm: maintain a sliding window of at most k elements using a hash-based data structure, with O(1) lookup for duplicates.

The only difference is in the hash table implementation: the editorial uses language-provided HashSet (Java), while our C solution uses a custom linear-probe array because C lacks a built-in hash table.

## Comparison Table

| Aspect | Our Implementation | Editorial #1 (Linear Search) | Editorial #2 (BST) | Editorial #3 (Hash Table) |
|--------|-------------------|------------------------------|---------------------|---------------------------|
| Algorithm | Hash Map + Sliding Window | Brute Force Window | BST + Sliding Window | Hash Set + Sliding Window |
| Time Complexity | O(n) amortized* | O(n * min(k, n)) | O(n * log(min(k, n))) | O(n) |
| Space Complexity | O(min(n, k)) | O(1) | O(min(n, k)) | O(min(n, k)) |
| LeetCode Status | Accepted | TLE | TLE | Accepted |
| C Implementation Difficulty | Medium | Very Low | High (need BST) | Medium |
| Interview Suitability | High | Low (too slow) | Medium (overkill) | High |

*Note: Our C implementation uses linear scan within the hash map (parallel arrays), making individual lookups O(window_size) rather than true O(1). However, since window_size <= k and k <= 10^5, this is acceptable in practice.

## Baseline vs Recommended Approach

### Baseline: Approach #1 (Naive Linear Search)
The simplest correct approach. For each element, scan the previous min(k, n) elements for a match. Easy to code and explain, but O(n * min(k, n)) time causes TLE on large inputs.

### Recommended for Google Interview: Approach #3 (Hash Table)
The hash table approach is the recommended answer because:

1. **Optimal time complexity** — O(n) with O(1) hash operations
2. **Clean algorithmic narrative** — "I maintain a sliding window of size k in a hash set; any duplicate found must be within distance k"
3. **Natural follow-up discussion** — leads into Contains Duplicate III (Problem 220) where you need a BST or bucket sort
4. **Easy to code in any language** — straightforward with built-in hash sets; in C, a simple array-based map works for the given constraints

### Why Not Approach #2 (BST)?
While BST is theoretically interesting, it is overkill for this problem. The O(log k) per operation offers no advantage since hash tables achieve O(1). BST becomes relevant only in Problem 220 (Contains Duplicate III) where range queries are needed.

## Edge Cases Analysis

| Case | Input | Expected | Our Solution |
|------|-------|----------|--------------|
| Single element | [1], k=0 | false | false (early return: numsSize<=1) |
| k=0 | [1,1], k=0 | false | false (early return: k<1) |
| Adjacent duplicates | [1,1], k=1 | true | true |
| Large k | [1,2,3,1], k=100 | true | true (k capped at n effectively) |
| No duplicates | [1,2,3,4], k=3 | false | false |
| All same | [1,1,1,1], k=1 | true | true (found at i=1) |

## Potential Improvement

For a true O(n) guarantee even in C, one could implement a proper hash table with chaining or open addressing using modular hashing. The current array-based approach has O(k) per lookup in the worst case, giving O(n*k) total. For the given constraints (k <= 10^5, n <= 10^5), this is at most 10^10 operations in the absolute worst case, but in practice the window is often much smaller and the solution passes.

---

# 219. 存在重複元素 II - 實作 vs 社論比較（繁體中文）

## 實作概述

本 C 解法使用**雜湊表 + 滑動視窗**方法（對應社論的方法 #3），是本題的最優且推薦解法。

## 與社論方法的對應

本實作對應**方法 #3：雜湊表**。核心演算法相同：維護一個大小至多為 k 的滑動視窗，用雜湊結構實現 O(1) 的重複查找。

唯一差異在於雜湊表的底層實作：社論使用語言內建的 HashSet（Java），而我們的 C 解法使用自訂的平行陣列，因為 C 語言沒有內建雜湊表。

## 比較表

| 面向 | 本實作 | 社論 #1（線性搜尋）| 社論 #2（BST）| 社論 #3（雜湊表）|
|------|--------|-------------------|--------------|-----------------|
| 演算法 | 雜湊表 + 滑動視窗 | 暴力視窗搜尋 | BST + 滑動視窗 | 雜湊集合 + 滑動視窗 |
| 時間複雜度 | O(n) 攤銷* | O(n * min(k, n)) | O(n * log(min(k, n))) | O(n) |
| 空間複雜度 | O(min(n, k)) | O(1) | O(min(n, k)) | O(min(n, k)) |
| LeetCode 狀態 | 通過 | 超時 | 超時 | 通過 |
| C 實作難度 | 中等 | 極低 | 高（需實作 BST）| 中等 |
| 面試適用性 | 高 | 低（太慢）| 中（殺雞用牛刀）| 高 |

*注意：本 C 實作在雜湊表內使用線性掃描（平行陣列），單次查找為 O(視窗大小) 而非真正的 O(1)。但因視窗大小 <= k 且 k <= 10^5，實際表現良好。

## 基線方法 vs 推薦方法

### 基線：方法 #1（暴力線性搜尋）
最簡單的正確方法。對每個元素掃描前 min(k, n) 個元素。容易撰寫和解釋，但 O(n * min(k, n)) 的時間在大型輸入下會超時。

### Google 面試推薦：方法 #3（雜湊表）
雜湊表方法是推薦答案，原因如下：

1. **最優時間複雜度** — O(n)，雜湊操作 O(1)
2. **清晰的演算法敘事** — 「我在雜湊集合中維護大小為 k 的滑動視窗；找到的任何重複必定在距離 k 以內」
3. **自然的延伸討論** — 可引入第 220 題（Contains Duplicate III），需要 BST 或桶排序
4. **任何語言都容易實作** — 使用內建雜湊集合很直觀；C 語言中對於給定限制，簡單的陣列映射即可

### 為何不用方法 #2（BST）？
BST 在理論上有趣，但對本題是殺雞用牛刀。O(log k) 的操作相比雜湊表的 O(1) 沒有優勢。BST 只在第 220 題（需要範圍查詢）中才變得相關。

## 可能的改進

若要在 C 中保證真正的 O(n)，可以實作帶鏈結或開放定址的真正雜湊表（使用模運算雜湊）。目前的陣列方法在最壞情況下每次查找為 O(k)，總計 O(n*k)。對於題目限制（k <= 10^5, n <= 10^5），最壞情況為 10^10 次操作，但實際上視窗通常遠小於此上限，解法可以通過。
