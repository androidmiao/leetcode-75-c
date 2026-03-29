# 234. Palindrome Linked List — Opus 4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Reverse Second Half In-Place**, which is the optimal approach described in the editorial. The algorithm structure matches the editorial's Approach 3 exactly:

1. `endOfFirstHalf()` uses the fast/slow pointer trick — identical to the editorial's step 1.
2. `reverseList()` performs standard in-place reversal with three rolling pointers (prev/curr/next) — identical to step 2.
3. The comparison loop walks `p1` from head and `p2` from `secondHalfStart`, stopping when `p2` reaches NULL — identical to step 3.
4. The list is restored by reversing the second half again and reconnecting it — identical to step 4.

**Verdict:** Same algorithm, same structure, same complexity. The implementation is a faithful C translation of the editorial's pseudocode.

## Approach Comparison

| Dimension | Approach 1: Array Copy | Approach 2: Recursive | Approach 3: Reverse In-Place ✓ |
|---|---|---|---|
| Time complexity | O(n) | O(n) | O(n) |
| Space complexity | O(n) | O(n) call stack | **O(1)** |
| Interview suitability | Good baseline — easy to explain and code correctly | Clever but space is still O(n); C implementation needs `static`/global pointer — awkward | **Best** — directly answers the follow-up question |
| C implementation difficulty | Simple (`malloc` + two-pointer) | Awkward (no closures; need static/global for `frontPointer`) | Clean — only pointer manipulation and a helper reversal |
| Mutates input? | No | No | Yes (temporarily; restored before return) |
| Concurrency safety | Safe | Safe | Unsafe during execution — list is temporarily broken |

## Baseline vs. Recommended

**Baseline:** Approach 1 (Copy into Array). Copy all `val` fields into a C array, then run a two-pointer palindrome check. O(n) time, O(n) space. Easy to code correctly in under 5 minutes. Good opening answer.

**Recommended (Google interview):** Approach 3 (Reverse Second Half In-Place). O(n) time, O(1) space. This is the follow-up answer the problem explicitly asks for. The ideal interview flow:

1. State the baseline (array copy) and its O(n) space limitation.
2. Explain: "We can achieve O(1) space by reversing the second half in place."
3. Walk through the four steps: find middle → reverse → compare → restore.
4. Proactively mention the restore step — it shows awareness of side effects and that the function should leave the data structure clean.
5. Note the concurrency caveat (list is temporarily broken) — this demonstrates systems-level thinking.

**Why not Approach 2?** The recursive approach is intellectually interesting — it reverses traversal order for free via the call stack. However, its space is O(n) (same as Approach 1), and the C implementation requires a `static` or global variable to share `frontPointer` across recursive frames. This is not idiomatic, not thread-safe, and harder to reason about in an interview setting. Mention it only as a "fun aside" if time permits.

---

## 中文翻譯

## 實作與官方解法比較

本 `.c` 檔實作的是官方解法的**方法三：原地反轉後半段（Reverse Second Half In-Place）**，與 Editorial 中 Approach 3 的演算法完全一致：

1. `endOfFirstHalf()` 使用快慢指標——對應 Editorial 步驟 1。
2. `reverseList()` 以三指標滾動（prev/curr/next）原地反轉——對應步驟 2。
3. 比較迴圈中 `p1` 從 head、`p2` 從 `secondHalfStart` 同步走——對應步驟 3。
4. 比較完畢後再次反轉後半段並重新接回——對應步驟 4。

**結論：** 演算法、結構、複雜度完全相同。本實作是 Editorial 虛擬碼的忠實 C 語言翻譯。

## 方法比較

| 面向 | 方法一：複製到陣列 | 方法二：遞迴 | 方法三：原地反轉後半段 ✓ |
|---|---|---|---|
| 時間複雜度 | O(n) | O(n) | O(n) |
| 空間複雜度 | O(n) | O(n) 呼叫堆疊 | **O(1)** |
| 面試適合度 | 好的基準答案——容易解釋與實作 | 巧妙但空間仍為 O(n)；C 實作需要 `static`/全域指標，不夠優雅 | **最佳**——直接回答 follow-up |
| C 實作難度 | 簡單（`malloc` + 雙指標） | 尷尬（無閉包；需 static/global 來跨遞迴框架共享狀態） | 乾淨——只有指標操作和輔助反轉函式 |
| 是否修改輸入？ | 否 | 否 | 是（暫時修改；回傳前復原） |
| 並行安全？ | 安全 | 安全 | 執行期間不安全——串列暫時被切斷 |

## 基準方法 vs. 推薦方法

**基準方法：方法一（複製到陣列）。** 將所有 `val` 複製到 C 陣列，再用雙指標判斷回文。O(n) 時間、O(n) 空間。五分鐘內可正確實作，適合作為面試開場答案。

**推薦方法（Google 面試）：方法三（原地反轉後半段）。** O(n) 時間、O(1) 空間，直接達成題目 follow-up 的要求。理想面試流程：

1. 先說明基準做法（陣列複製）及其 O(n) 空間的限制。
2. 解釋：「可以用 O(1) 空間——把後半段原地反轉。」
3. 逐步說明四個步驟：找中點 → 反轉 → 比較 → 復原。
4. 主動提及復原步驟——展示對函式副作用的意識，表明函式應保持資料結構的完整性。
5. 提及並行安全的注意事項（串列在操作期間暫時被切斷）——展現系統層級的思考能力。

**為什麼不推薦方法二？** 遞迴方法在概念上很有趣——利用呼叫堆疊免費反向走訪。但空間仍為 O(n)（與方法一相同），且 C 實作需要 `static` 或全域變數來跨遞迴框架共享 `frontPointer`。這不符合慣例、不具執行緒安全性，且在面試中較難推理。若時間允許，可作為「有趣的補充」提及即可。
