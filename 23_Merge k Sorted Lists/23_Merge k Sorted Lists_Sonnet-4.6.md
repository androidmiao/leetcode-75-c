# 23. Merge k Sorted Lists — Implementation vs. Editorial Comparison

## Implementation vs. Editorial

The `.c` implementation uses **Divide and Conquer**, which is identical to **Editorial Approach 5 (Merge with Divide And Conquer)**. It is the same algorithm, not a variation — the recursion structure, the mergeTwoLists helper, and the halving strategy all match exactly.

## Which Approach Is Better

| Aspect | Approach 3: Priority Queue | Approach 5: Divide & Conquer (implemented) |
|--------|---------------------------|-------------------------------------------|
| Time complexity | O(N log k) | O(N log k) |
| Space complexity | O(k) (heap storage) | O(log k) (recursion stack) / O(1) iterative |
| Interview suitability | High — common pattern | High — clean and elegant |
| C implementation difficulty | Hard — C has no built-in heap; must hand-write a min-heap with custom comparator | Medium — only needs mergeTwoLists + recursive split |
| Code length | ~60–80 lines (with heap) | ~40 lines |
| Edge case handling | Must handle empty lists in heap init | Simple: left > right returns NULL |

## Baseline vs. Recommended

- **Baseline:** Approach 4 (Merge Lists One by One) — O(kN) time, O(1) space. Simplest to code: just loop through the lists calling mergeTwoLists sequentially. Good starting point in an interview to show understanding, but suboptimal.

- **Recommended:** Approach 5 (Divide and Conquer) — O(N log k) time, O(1) extra space (ignoring recursion stack). This is the best choice for a Google-style interview because:
  1. **Optimal complexity** — O(N log k) matches the priority queue approach but without needing a heap.
  2. **Simple C implementation** — only requires a standard mergeTwoLists plus a recursive split; no heap boilerplate.
  3. **Easy to explain** — the divide-and-conquer pattern is well-known and the progression from Approach 4 to Approach 5 is a natural follow-up question.
  4. **Space advantage** — O(log k) recursion stack is smaller than O(k) heap. For the given constraints (k ≤ 10⁴), that's at most 14 stack frames vs. 10⁴ heap entries.
  5. **Demonstrates algorithmic thinking** — shows the interviewer that you can optimize a sequential process into a balanced divide-and-conquer structure.

Approach 3 (Priority Queue) is also a strong interview answer and is sometimes preferred when the interviewer specifically asks about heap usage. However, in standalone C, the lack of a built-in priority queue makes Approach 5 significantly more practical and less error-prone.

---

# 23. 合併 K 個升序鏈結串列 — 實作與題解對比

## 實作 vs. 題解

`.c` 檔案的實作使用**分治法**，與**題解方法 5（分治合併）** 完全相同。這不是變體，而是同一個演算法——遞迴結構、mergeTwoLists 輔助函式、二分策略都完全一致。

## 哪種方法更好

| 維度 | 方法 3：優先佇列 | 方法 5：分治合併（已實作） |
|------|----------------|------------------------|
| 時間複雜度 | O(N log k) | O(N log k) |
| 空間複雜度 | O(k)（堆儲存） | O(log k)（遞迴堆疊）/ O(1) 迭代 |
| 面試適用性 | 高 — 常見模式 | 高 — 簡潔優雅 |
| C 實作難度 | 困難 — C 沒有內建堆；需要手寫帶自定義比較器的最小堆 | 中等 — 只需 mergeTwoLists + 遞迴拆分 |
| 程式碼長度 | ~60–80 行（含堆實作） | ~40 行 |
| 邊界處理 | 必須在堆初始化時處理空鏈結串列 | 簡單：left > right 回傳 NULL |

## 基線方案 vs. 推薦方案

- **基線方案：** 方法 4（逐一合併）— O(kN) 時間，O(1) 空間。最容易編碼：只需迴圈呼叫 mergeTwoLists 順序合併。適合面試中作為起點展示理解，但不是最優解。

- **推薦方案：** 方法 5（分治合併）— O(N log k) 時間，O(1) 額外空間（不計遞迴堆疊）。這是 Google 風格面試的最佳選擇，原因如下：
  1. **最優複雜度** — O(N log k) 與優先佇列方法相同，但無需實作堆。
  2. **C 實作簡單** — 只需標準的 mergeTwoLists 加遞迴拆分；無需堆的樣板程式碼。
  3. **易於解釋** — 分治模式廣為人知，從方法 4 到方法 5 的優化是自然的追問方向。
  4. **空間優勢** — O(log k) 遞迴堆疊比 O(k) 堆更小。在給定約束（k ≤ 10⁴）下，最多 14 層堆疊框架 vs. 10⁴ 個堆元素。
  5. **展示演算法思維** — 向面試官證明你能將順序處理優化為平衡的分治結構。

方法 3（優先佇列）也是很好的面試答案，當面試官特別考察堆的使用時可能更受青睞。但在純 C 環境中，由於缺少內建優先佇列，方法 5 明顯更實用、更不容易出錯。




@/Users/alex.miao/Documents/GitHub/leetcode-75-c/23_Merge k Sorted Lists kernel 的 list_sort 就是 merge sort 變體, 

```markdown
請列出Linux相關的code在哪裡？若以主管Code Review的角度, 可能會怎麼考? 把這個回覆記錄到23_Merge k Sorted Lists_Linux.md
```
