# 876. Middle of the Linked List — Sonnet 4.6 Notes

## Implementation vs. Editorial Comparison

**Verdict: Same algorithm — the `.c` solution is identical to Editorial Approach 2 (Fast and Slow Pointer).**

Both use:
- `slow` and `fast` both initialized to `head`
- Loop condition: `while (fast != NULL && fast->next != NULL)`
- `slow = slow->next`, `fast = fast->next->next` per iteration
- Return `slow`

The only difference is language: the editorial shows C++, the `.c` file is plain C — the logic is 1-to-1 equivalent.

## Which Approach Is Better?

The `.c` solution (= Editorial Approach 2) is **better than Editorial Approach 1** on every dimension that matters:

| Criterion | Approach 1: Output to Array | Approach 2: Fast & Slow Pointer |
|---|---|---|
| Time | O(N) | O(N) |
| Space | O(N) — array of N node pointers | **O(1)** — two pointers only |
| Interview suitability | Low — trivial but wastes space | **High** — canonical two-pointer pattern |
| C implementation | Needs dynamic array or fixed buffer | Two pointer variables, no allocation |

Approach 2 is the standard interview answer for this problem. It demonstrates the fast/slow pointer technique, which also forms the basis for cycle detection (LC 141) and other linked list problems. Interviewers expect this approach.

## Baseline vs. Recommended

- **Baseline (Approach 1):** collect nodes into array, return `A[N/2]`. Correct but O(N) space — acceptable only as a first-pass answer before the follow-up.
- **Recommended (Approach 2, implemented in `.c`):** fast/slow pointer. O(1) space. This is the approach to lead with and defend in an interview.

The `.c` solution already implements the recommended approach and needs no changes.

---

---

# 876. Middle of the Linked List — Sonnet 4.6 筆記

## 實作與 Editorial 比較

**結論：演算法完全相同 — `.c` 的解法與 Editorial 方法 2（快慢指標）一模一樣。**

兩者都使用：
- `slow` 和 `fast` 同時從 `head` 出發
- 迴圈條件：`while (fast != NULL && fast->next != NULL)`
- 每次迭代：`slow = slow->next`，`fast = fast->next->next`
- 回傳 `slow`

唯一的差異是語言：editorial 使用 C++，`.c` 檔使用純 C，邏輯完全等價。

## 哪個方法比較好？

`.c` 解法（= Editorial 方法 2）在所有重要維度上都**優於** Editorial 方法 1：

| 評估面向 | 方法 1：輸出成陣列 | 方法 2：快慢指標 |
|---|---|---|
| 時間複雜度 | O(N) | O(N) |
| 空間複雜度 | O(N) — 需儲存 N 個節點指標 | **O(1)** — 只需兩個指標 |
| 面試適合度 | 低 — 思路直白但浪費空間 | **高** — 經典雙指標模式 |
| C 語言實作 | 需要動態陣列或固定 buffer | 兩個指標變數，無需配置記憶體 |

方法 2 是這題的標準面試答案。它展示了快慢指標技巧，這個技巧也是環狀偵測（LC 141）及其他 linked list 題目的基礎。面試官期待這個解法。

## Baseline 與推薦方法

- **Baseline（方法 1）：** 把節點收集進陣列，回傳 `A[N/2]`。正確，但空間 O(N)，只適合作為「初步答案」，後續 follow-up 應升級。
- **推薦（方法 2，已在 `.c` 中實作）：** 快慢指標，O(1) 空間。這是面試時應直接提出並能清楚說明的解法。

`.c` 的解法已實作推薦方法，無需修改。
