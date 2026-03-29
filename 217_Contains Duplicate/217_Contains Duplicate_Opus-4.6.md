# 217. Contains Duplicate - Implementation vs Editorial Comparison

> Editorial content verified from live authenticated LeetCode session (2026-03-29).

## Implementation Overview

The repository's C solution uses the **Sorting + Adjacent Check** approach (official Editorial Approach #2), not the theoretically optimal Hash Table approach (Approach #3).

## Which Editorial Approach Does the Implementation Match?

The `.c` solution is **identical** to official Editorial Approach #2 (Sorting). It uses `qsort` for ascending sort followed by a single linear scan comparing adjacent elements.

**Important note from the official Editorial**: For certain test cases with not very large n, the Hash Table method (Approach #3) can actually be slower than Sorting (Approach #2) due to hash table maintenance overhead. Real-world performance can differ from Big-O notation — when n is not sufficiently large, an O(n) algorithm can be slower than an O(n log n) algorithm.

## Comparison Table

| Aspect | Current Implementation (Sorting) | Editorial Baseline (Brute Force) | Editorial Recommended (Hash Set) |
|--------|----------------------------------|----------------------------------|----------------------------------|
| Algorithm | qsort + adjacent scan | Nested loop all pairs | Single-pass with hash set |
| Time Complexity | O(n log n) | O(n²) | O(n) |
| Space Complexity | O(log n) stack | O(1) | O(n) |
| C Implementation Difficulty | Low — uses stdlib `qsort` | Very Low — two nested loops | Medium — must hand-roll hash table |
| Interview Suitability | Good — clean and explainable | Poor — too slow, shows no insight | Best — optimal, but harder in C |
| LeetCode Acceptance | Accepted | TLE on large inputs | Accepted (Optimal) |

## Baseline vs. Recommended

**Baseline**: Brute Force O(n²) — checks every pair. Simple but impractical for n = 10⁵.

**Recommended for Google Interview in C**: **Sorting approach** — the implementation in this repo.

**Why**: In a C interview without access to a hash library, the sorting approach is the best balance of clarity and efficiency. It demonstrates knowledge of `qsort`, comparison functions, and the insight that duplicates become adjacent after sorting. The hash set approach is theoretically better (O(n) vs O(n log n)) but requires implementing a hash table from scratch in C, which is error-prone and time-consuming in an interview setting. The sorting approach is easily explainable and defensible.

If the interviewer asks for a follow-up, you can describe the hash set approach verbally and explain the tradeoff: O(n) time with O(n) space vs O(n log n) time with O(1) extra space.

## Practical Performance (n = 10⁵)

| Approach | Operations | Estimated Time |
|----------|-----------|----------------|
| Brute Force | ~10¹⁰ | 20+ seconds (TLE) |
| Sorting | ~1.7 × 10⁶ | ~5 ms |
| Hash Set | ~10⁵ | ~1 ms |

Both Sorting and Hash Set are well within LeetCode time limits. The constant-factor difference is negligible for this constraint.

## Edge Cases

| Case | Input | Output | Notes |
|------|-------|--------|-------|
| Single element | `[1]` | false | Caught by `numsSize <= 1` guard |
| Two duplicates | `[1,1]` | true | Found at i=0 after sort |
| All same | `[1,1,1,1]` | true | First adjacent pair matches |
| All distinct | `[1,2,3,4,5]` | false | Full scan completes |
| Negative numbers | `[-1,-1,0]` | true | `compare` handles negatives correctly |

## compare Function Overflow Note

The `compare` function uses subtraction `*(int*)a - *(int*)b`. This is safe for this problem because the constraint is `-10⁹ <= nums[i] <= 10⁹`, so the maximum difference is 2 × 10⁹ which fits in a 32-bit signed int (max 2,147,483,647). For problems with wider ranges, a three-way conditional would be safer.

---

# 217. 包含重複元素 — 實作與題解比較

## 實作概述

本倉庫的 C 解法採用**排序 + 相鄰檢查**方法（題解中的方法二），而非理論最優的雜湊集合方法。

## 實作對應哪個題解方法？

`.c` 解法與題解方法二（排序）**完全一致**。使用 `qsort` 升序排序後，單次線性掃描比較相鄰元素。

## 比較表

| 面向 | 目前實作（排序） | 題解基線（暴力法） | 題解推薦（雜湊集合） |
|------|-----------------|-------------------|---------------------|
| 演算法 | qsort + 相鄰掃描 | 巢狀迴圈檢查所有配對 | 單次遍歷搭配雜湊集合 |
| 時間複雜度 | O(n log n) | O(n²) | O(n) |
| 空間複雜度 | O(log n) 堆疊 | O(1) | O(n) |
| C 實作難度 | 低 — 使用 stdlib 的 `qsort` | 非常低 — 兩層巢狀迴圈 | 中等 — 須手寫雜湊表 |
| 面試適用性 | 良好 — 乾淨且易解釋 | 差 — 太慢，無法展現洞察力 | 最佳 — 最優解，但在 C 中較難 |

## 基線 vs. 推薦方法

**基線**：暴力法 O(n²) — 檢查每一對。簡單但對 n = 10⁵ 不實用。

**Google 面試 C 語言推薦**：**排序法** — 即本倉庫的實作。

**原因**：在沒有雜湊函式庫的 C 面試中，排序法是清晰度與效率的最佳平衡。它展示了對 `qsort`、比較函式的理解，以及「排序後重複元素必定相鄰」的洞察。雜湊集合理論上更好（O(n) vs O(n log n)），但在 C 中需要從零實作雜湊表，在面試情境中容易出錯且耗時。排序法容易解釋且站得住腳。

若面試官追問，可口頭描述雜湊集合法並解釋取捨：O(n) 時間配 O(n) 空間 vs O(n log n) 時間配 O(1) 額外空間。

## 實際效能（n = 10⁵）

| 方法 | 運算次數 | 預估時間 |
|------|---------|---------|
| 暴力法 | ~10¹⁰ | 20+ 秒（超時） |
| 排序 | ~1.7 × 10⁶ | ~5 ms |
| 雜湊集合 | ~10⁵ | ~1 ms |

排序與雜湊集合皆在 LeetCode 時間限制內。常數因子差異在此限制條件下可忽略。

## compare 函式溢位說明

`compare` 函式使用減法 `*(int*)a - *(int*)b`。此題安全，因限制條件為 `-10⁹ <= nums[i] <= 10⁹`，最大差值 2 × 10⁹ 可容納於 32 位元有號整數（最大 2,147,483,647）。若值域更廣，應改用三元條件式比較。
