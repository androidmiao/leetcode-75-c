# 242. Valid Anagram — Implementation vs. Editorial Comparison

## Implementation Overview

The `.c` solution uses **Approach 2: Frequency Counter** from the editorial — the recommended approach. The implementation is a direct, textbook version of the algorithm with two separate loops (one to increment from `s`, one to decrement from `t`) followed by a final zero-check loop.

## Which Editorial Approach Does the Implementation Match?

The implementation matches **Editorial Approach 2 (Frequency Counter)** exactly. It is the standard two-pass variant (not the single-loop variant, not the early-exit variant). This is the cleanest form for interview discussion.

## Comparison Table

| Aspect | Our `.c` Implementation | Editorial Approach 1 (Sorting) | Editorial Approach 2 (Frequency Counter) |
|--------|------------------------|-------------------------------|----------------------------------------|
| Algorithm | Frequency array, 2 loops + check | qsort both strings, strcmp | Frequency array (same idea, variants exist) |
| Time Complexity | O(n) | O(n log n) | O(n) |
| Space Complexity | O(1) — 26-int stack array | O(n) — strdup for both strings | O(1) — 26-int array |
| Early Exit on Length | Yes | Yes | Yes |
| Dynamic Memory | None | malloc + free for both copies | None |
| Interview Suitability | Excellent — simple, optimal | Acceptable — easy to explain | Excellent |
| C Implementation Difficulty | Very Low | Low (qsort comparator boilerplate) | Very Low |

## Baseline vs. Recommended

### Baseline Approach: Sorting

Sort both strings and compare. Easy to understand but O(n log n) time and requires O(n) extra memory in C (must duplicate strings before in-place sort). The `qsort` comparator boilerplate adds minor friction.

### Recommended Approach: Frequency Counter (Our Implementation)

Single 26-element stack array, two linear passes, one constant-time check. No dynamic memory. O(n) time, O(1) space. This is the approach to lead with in an interview: it demonstrates understanding of the character-counting pattern that generalizes to many string problems (group anagrams, minimum window substring, permutation in string, etc.).

### Why Recommended Is Better for Interview Discussion

1. **Optimal complexity** — O(n) vs. O(n log n) is an easy win to articulate.
2. **Zero allocation** — no `malloc`/`free` to worry about, no memory leaks.
3. **Pattern recognition** — the frequency-array technique recurs in dozens of LeetCode problems; demonstrating it here signals familiarity with the pattern.
4. **Follow-up ready** — the interviewer's natural follow-up is "what about Unicode?" and the answer (swap fixed array for hash table) is a clean extension of the same idea.

---

# 242. 有效的異位詞 — 實作與題解比較

## 實作概述

`.c` 解法採用**方法二：頻率計數**——即題解中的推薦方法。實作為教科書式的標準兩迴圈版本（一次遍歷 `s` 遞增、一次遍歷 `t` 遞減），最後再檢查全部計數是否歸零。

## 實作對應哪個題解方法？

實作完全對應**題解方法二（頻率計數）**。採用的是標準兩迴圈變體（非單迴圈變體、非提前退出變體），是面試討論中最乾淨的形式。

## 比較表

| 面向 | 本 `.c` 實作 | 題解方法一（排序） | 題解方法二（頻率計數） |
|------|-------------|------------------|---------------------|
| 演算法 | 頻率陣列，2 迴圈 + 檢查 | 排序兩字串後比較 | 頻率陣列（同概念，有不同變體） |
| 時間複雜度 | O(n) | O(n log n) | O(n) |
| 空間複雜度 | O(1) — 26 個 int 的堆疊陣列 | O(n) — 需複製兩個字串 | O(1) — 26 個 int 陣列 |
| 長度不等提前退出 | 是 | 是 | 是 |
| 動態記憶體配置 | 無 | 需 malloc + free | 無 |
| 面試適合度 | 極佳 — 簡潔、最優 | 可接受 — 容易解釋 | 極佳 |
| C 實作難度 | 非常低 | 低（需寫 qsort 比較函式） | 非常低 |

## 基線方法 vs. 推薦方法

### 基線方法：排序

將兩字串排序後比較。容易理解但時間為 O(n log n)，在 C 中需 O(n) 額外記憶體（必須先複製字串才能原地排序）。`qsort` 比較函式的樣板程式碼增加少許摩擦。

### 推薦方法：頻率計數（即本實作）

僅需一個 26 元素的堆疊陣列、兩次線性遍歷、一次常數時間檢查。無動態記憶體配置。O(n) 時間、O(1) 空間。面試中應以此方法開場：它展示了字元計數模式的理解，而此模式可推廣至許多字串問題（分組異位詞、最小覆蓋子字串、字串中的排列等）。

### 為何推薦方法更適合面試

1. **最優複雜度** — O(n) vs. O(n log n) 是容易闡述的優勢。
2. **零配置** — 無需 `malloc`/`free`，不存在記憶體洩漏風險。
3. **模式辨識** — 頻率陣列技巧在數十道 LeetCode 題目中反覆出現；在此展示代表對此模式的熟悉。
4. **進階問題就緒** — 面試官的自然追問是「如果是 Unicode 字元呢？」，而答案（將固定陣列替換為雜湊表）是同一概念的直接延伸。
