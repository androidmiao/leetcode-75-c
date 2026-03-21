# 234. Palindrome Linked List — Sonnet 4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Reverse Second Half In-Place**, which is exactly the optimal approach described in the editorial. The algorithm, step structure, helper functions (`endOfFirstHalf`, `reverseList`), and complexity match the editorial's Approach 3 precisely.

## Approach Comparison

| | Approach 1: Array Copy | Approach 2: Recursive | Approach 3: Reverse In-Place |
|---|---|---|---|
| Time complexity | O(n) | O(n) | O(n) |
| Space complexity | O(n) | O(n) call stack | **O(1)** |
| Interview suitability | Easy to explain; good baseline | Clever but O(n) space; tricky for most interviewers | Best — directly answers the follow-up |
| C implementation difficulty | Simple | Awkward in C (no closures; need a global/static pointer) | Clean — just pointers and a helper reversal |
| Mutates input | No | No | Yes (temporarily; restored) |

## Baseline vs. Recommended

**Baseline:** Approach 1 (Copy into Array). Copy all values to a plain C array, use two-pointer palindrome check. O(n) time, O(n) space. Easy to code correctly and explain. Good opening answer when asked cold.

**Recommended:** Approach 3 (Reverse Second Half In-Place). O(n) time, O(1) space. This directly answers the follow-up constraint from the problem statement. In an interview you would first describe the baseline, then say "but we can do O(1) space by reversing the second half in place," and walk through the four steps. The mutation/restore pattern is worth calling out explicitly — it shows awareness that the function should leave the structure clean.

Approach 2 (Recursive) is worth knowing conceptually (it elegantly reverses traversal using the call stack) but should not be the primary answer because its space is O(n) and the C implementation requires a `static` or global pointer to share state across recursive frames, which is not idiomatic for an interview.

---

## 中文翻譯

## 實作與官方解法比較

本 `.c` 檔實作的是官方解法的**方法三：原地反轉後半段**，與 editorial 中 Approach 3 的演算法、步驟結構、輔助函式（`endOfFirstHalf`、`reverseList`）以及複雜度完全一致。

## 方法比較

| | 方法一：複製到陣列 | 方法二：遞迴 | 方法三：原地反轉後半段 |
|---|---|---|---|
| 時間複雜度 | O(n) | O(n) | O(n) |
| 空間複雜度 | O(n) | O(n) 呼叫堆疊 | **O(1)** |
| 面試適合度 | 容易解釋，適合當基準答案 | 巧妙但空間 O(n)；C 實作較尷尬 | 最佳——直接回答 follow-up |
| C 實作難度 | 簡單 | 需要 static/global 指標共享狀態，不夠優雅 | 乾淨，只用指標與輔助反轉函式 |
| 是否修改輸入 | 否 | 否 | 是（暫時；最後會復原） |

## 基準方法 vs. 推薦方法

**基準方法：方法一（複製到陣列）**。把所有值複製到 C 陣列，再用雙指標判斷回文。O(n) 時間、O(n) 空間。實作容易、說明清晰，適合在面試被問到時先給出這個開場答案。

**推薦方法：方法三（原地反轉後半段）**。O(n) 時間、O(1) 空間，直接達成題目 follow-up 的要求。面試時的理想流程：先說明基準做法，再說「但可以用 O(1) 空間——把後半段原地反轉」，然後逐步說明四個步驟。特別要提到「比較後復原串列結構」這個動作，展示對函式副作用的意識。

方法二（遞迴）概念上值得了解（用呼叫堆疊優雅地反向走訪），但不應作為主要答案，因為空間仍是 O(n)，且 C 語言實作需要 `static` 或全域指標來跨遞迴框架共享狀態，不是面試理想寫法。
