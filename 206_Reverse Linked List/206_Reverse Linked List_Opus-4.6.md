# 206. Reverse Linked List — Opus-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution in this folder uses the **iterative approach**, which is the same algorithm as **Editorial Approach 1 (Iterative)**. The logic is identical: maintain `prev`, `curr`, and a temporary `next` pointer, reversing each node's `next` link one at a time while walking through the list.

The previous implementation (in `206-reverse-linked-list/solution.c`) used a slightly different coding style — it checked for empty/single-node lists explicitly, started `curr` from `head->next`, manually set `head->next = NULL`, and used an infinite `while(1)` loop with a break condition. The new standalone version uses the cleaner editorial pattern: `prev = NULL`, `curr = head`, and a `while (curr)` loop. Both produce the same result, but the editorial pattern is more conventional and easier to explain in an interview.

## Which Approach Is Better

| Criterion                  | Approach 1: Iterative       | Approach 2: Recursive        |
|----------------------------|-----------------------------|------------------------------|
| Time complexity            | O(n)                        | O(n)                         |
| Space complexity           | O(1)                        | O(n) (call stack)            |
| Interview suitability      | Excellent — clear, linear   | Good — tests recursion skill |
| C implementation difficulty| Very easy                   | Easy, but stack overflow risk|
| Readability                | Straightforward loop        | Elegant but less intuitive   |

## Baseline vs. Recommended

- **Baseline approach:** Iterative (Approach 1) — the simplest correct solution with O(1) space.
- **Recommended approach:** Iterative (Approach 1) — for interviews, the iterative approach is preferred because it avoids O(n) stack usage, is easy to explain step-by-step with pointer diagrams, and has no risk of stack overflow on large inputs. The recursive approach is worth knowing as a follow-up but should not be the primary answer.

---

---

# 206. Reverse Linked List — Opus-4.6 筆記

## 實作與官方題解的比較

本資料夾的 `.c` 解法使用的是**迭代法**，與**官方題解方法一（Iterative）**完全相同。邏輯一致：維護 `prev`、`curr` 和暫時的 `next` 指標，在遍歷串列的過程中逐一反轉每個節點的 `next` 連結。

先前的實作（位於 `206-reverse-linked-list/solution.c`）使用了略有不同的寫法——它明確檢查空串列和單節點串列、從 `head->next` 開始設定 `curr`、手動將 `head->next` 設為 `NULL`，並使用無窮迴圈 `while(1)` 搭配 break 條件。新的 standalone 版本使用更簡潔的官方題解模式：`prev = NULL`、`curr = head`，以及 `while (curr)` 迴圈。兩者結果相同，但官方題解的模式更符合慣例，在面試中也更容易解釋。

## 哪種方法更好

| 評估標準                   | 方法一：迭代法              | 方法二：遞迴法              |
|----------------------------|-----------------------------|------------------------------|
| 時間複雜度                 | O(n)                        | O(n)                         |
| 空間複雜度                 | O(1)                        | O(n)（呼叫堆疊）            |
| 面試適用性                 | 極佳 — 清晰、線性           | 佳 — 測試遞迴能力           |
| C 語言實作難度             | 非常簡單                    | 簡單，但有堆疊溢位風險      |
| 可讀性                     | 直觀的迴圈                  | 優雅但不夠直覺              |

## 基線方法 vs. 推薦方法

- **基線方法：** 迭代法（方法一）— 最簡單的正確解法，空間複雜度 O(1)。
- **推薦方法：** 迭代法（方法一）— 在面試中，迭代法更受青睞，因為它避免了 O(n) 的堆疊使用、容易用指標圖示逐步解釋，且在大型輸入時不會有堆疊溢位的風險。遞迴法值得作為追問（follow-up）來了解，但不應作為主要答案。
