# 23. Merge k Sorted Lists — Implementation vs. Editorial Comparison

## Implementation vs. Editorial

The `.c` implementation uses **Divide and Conquer (top-down recursive)**, which corresponds directly to **Editorial Approach 5 (Merge with Divide And Conquer)**. It is the same algorithm — not a variation — with identical structure:

- `mergeKListsHelper` recursively splits the `lists[left..right]` range at the midpoint.
- Base cases: `left == right` returns the single list; `left > right` returns `NULL`.
- `mergeTwoLists` merges two sorted linked lists using a dummy-head technique.
- The recursion tree has depth O(log k), and each level does O(N) total merge work.

The only minor implementation detail worth noting: the `.c` file uses a stack-allocated `struct ListNode dummy` (not heap-allocated) in `mergeTwoLists`, which is a clean C idiom that avoids `malloc`/`free` overhead.

## Which Approach Is Better

| Aspect | Approach 3: Priority Queue (Min-Heap) | Approach 5: Divide & Conquer (Implemented) |
|--------|---------------------------------------|-------------------------------------------|
| Time complexity | O(N log k) | O(N log k) |
| Space complexity | O(k) heap storage | O(log k) recursion stack |
| Interview suitability | High — demonstrates heap mastery | High — clean, elegant, easy to explain |
| C implementation difficulty | Hard — must hand-write min-heap with `ListNode*` entries and custom comparator | Medium — only `mergeTwoLists` + recursive split |
| Code length | ~60–80 lines (with heap infrastructure) | ~40 lines |
| Edge-case handling | Must guard against empty lists during heap initialization | Simple: `left > right → NULL` |
| Stability | Stable (same-value nodes preserve order from lower-index lists first, depending on tie-breaking) | Stable (left list wins ties in `mergeTwoLists`, preserving relative order) |

Both Approach 3 and 5 achieve the optimal O(N log k) time. Approach 5 wins on space (O(log k) vs. O(k)) and on C implementation simplicity.

## Baseline vs. Recommended

### Baseline: Approach 4 — Merge Lists One by One

- **Time:** O(kN) — the first list gets traversed k-1 times, the second k-2 times, etc.
- **Space:** O(1)
- **Strengths:** trivially correct; a 5-line loop calling `mergeTwoLists` sequentially.
- **Weakness:** suboptimal time; hard to defend in a Google interview when the interviewer expects O(N log k).
- **Use in interview:** good as a "let me start with the brute-force" opening before optimizing.

### Recommended: Approach 5 — Divide and Conquer

- **Time:** O(N log k)
- **Space:** O(log k) recursion stack (O(1) if implemented iteratively)
- **Why this is the best Google interview choice:**
  1. **Optimal complexity** without needing any auxiliary data structure.
  2. **Natural progression** from Approach 4 — the interviewer can see your optimization thought process: "Instead of merging sequentially (O(kN)), I pair-merge in a balanced tree fashion (O(N log k))."
  3. **Minimal boilerplate in C** — no heap struct, no `sift_down`, no comparator array. Just `mergeTwoLists` (which you might already have from LeetCode 21) plus a 10-line recursive helper.
  4. **Space advantage** — O(log k) stack frames vs. O(k) heap entries. For k ≤ 10⁴, that's at most 14 frames.
  5. **Easy to verify** — the recursion clearly terminates (range shrinks each call) and correctness follows from `mergeTwoLists` being correct for two lists.

### When to prefer Approach 3 (Priority Queue) instead

- The interviewer explicitly asks you to use a heap.
- You're in a language with a built-in priority queue (Python `heapq`, Java `PriorityQueue`, C++ `priority_queue`).
- The problem variant requires streaming / online merging where new lists arrive dynamically.

---

# 23. 合併 K 個升序鏈結串列 — 實作與題解對比

## 實作 vs. 題解

`.c` 檔案的實作使用**分治法（自頂向下遞迴）**，直接對應**題解方法 5（分治合併）**。這是完全相同的演算法，而非變體：

- `mergeKListsHelper` 以中點遞迴拆分 `lists[left..right]` 範圍。
- 基底情況：`left == right` 回傳該單一串列；`left > right` 回傳 `NULL`。
- `mergeTwoLists` 使用 dummy head 技巧合併兩個已排序鏈結串列。
- 遞迴樹深度為 O(log k)，每一層的合併總工作量為 O(N)。

唯一值得注意的實作細節：`.c` 檔案在 `mergeTwoLists` 中使用堆疊配置的 `struct ListNode dummy`（非堆配置），這是乾淨的 C 慣用法，避免了 `malloc`/`free` 的額外開銷。

## 哪種方法更好

| 維度 | 方法 3：優先佇列（最小堆） | 方法 5：分治合併（已實作） |
|------|-------------------------|------------------------|
| 時間複雜度 | O(N log k) | O(N log k) |
| 空間複雜度 | O(k) 堆儲存 | O(log k) 遞迴堆疊 |
| 面試適用性 | 高 — 展示堆的掌握度 | 高 — 簡潔優雅、易於解釋 |
| C 實作難度 | 困難 — 需手寫含 `ListNode*` 條目和自定義比較器的最小堆 | 中等 — 只需 `mergeTwoLists` + 遞迴拆分 |
| 程式碼長度 | ~60–80 行（含堆基礎設施） | ~40 行 |
| 邊界處理 | 堆初始化時需防護空串列 | 簡單：`left > right → NULL` |
| 穩定性 | 穩定（取決於平手時的處理方式） | 穩定（`mergeTwoLists` 中左串列在相同值時優先，保持相對順序） |

方法 3 和 5 都達到最優的 O(N log k) 時間。方法 5 在空間上勝出（O(log k) vs. O(k)），在 C 的實作簡潔度上也更優。

## 基線方案 vs. 推薦方案

### 基線方案：方法 4 — 逐一合併

- **時間：** O(kN) — 第一個串列被遍歷 k-1 次，第二個 k-2 次，以此類推。
- **空間：** O(1)
- **優點：** 正確性顯然；只需一個 5 行迴圈順序呼叫 `mergeTwoLists`。
- **缺點：** 時間不最優；在 Google 面試中當面試官期望 O(N log k) 時難以辯護。
- **面試用途：** 適合作為「先從暴力法開始」的開場，再進行優化。

### 推薦方案：方法 5 — 分治合併

- **時間：** O(N log k)
- **空間：** O(log k) 遞迴堆疊（迭代版為 O(1)）
- **為何這是 Google 面試的最佳選擇：**
  1. **最優複雜度**，無需任何輔助資料結構。
  2. **自然的優化過程** — 從方法 4 進化而來：「與其順序合併（O(kN)），我配對合併形成平衡樹（O(N log k)）。」
  3. **C 語言中樣板程式碼最少** — 無需堆結構、`sift_down`、比較器陣列。只需 `mergeTwoLists`（可能已從 LeetCode 21 實作過）加上 10 行遞迴輔助函式。
  4. **空間優勢** — O(log k) 堆疊框架 vs. O(k) 堆元素。k ≤ 10⁴ 時最多 14 層框架。
  5. **易於驗證** — 遞迴明確終止（範圍每次縮小），正確性由 `mergeTwoLists` 對兩個串列的正確性推導而來。

### 何時應選擇方法 3（優先佇列）

- 面試官明確要求使用堆。
- 使用內建優先佇列的語言（Python `heapq`、Java `PriorityQueue`、C++ `priority_queue`）。
- 題目變體要求串流 / 線上合併（新串列動態加入的情境）。
