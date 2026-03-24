# 232. Implement Queue using Stacks — Implementation vs. Editorial Comparison

## Implementation vs. Editorial

The `.c` solution implements **Editorial Approach #2** (Two Stacks — Push O(1), Pop Amortized O(1)) with the lazy transfer strategy. It is the same core algorithm described in the editorial's second approach.

The naming differs slightly: the editorial uses `s1`/`s2`, while the implementation uses `inbox`/`outbox`, which is more descriptive of each stack's role.

One minor difference: the editorial tracks a `front` variable for peek when `s2` is empty, while the implementation simply triggers a transfer when `outbox` is empty (making `front` tracking unnecessary since `outbox` top always holds the front element after transfer).

## Which Approach Is Better

| Dimension | Approach #1 (Push O(n), Pop O(1)) | Approach #2 (Push O(1), Pop Amortized O(1)) |
|---|---|---|
| Push time | O(n) — must move all elements every push | O(1) — just append to inbox |
| Pop time | O(1) — always ready at top | Amortized O(1), worst-case O(n) |
| Peek time | O(1) | O(1) (via `front` variable or transfer) |
| Space | O(n) | O(n) |
| Interview suitability | Simple to explain but inefficient push | Better overall; amortized analysis is a strong talking point |
| C implementation difficulty | Slightly more code in push | Slightly more code in pop/transfer; overall similar |

## Baseline vs. Recommended

- **Baseline:** Approach #1 — Push O(n), Pop O(1). Every push rebuilds the entire stack order by transferring all elements through the auxiliary stack. Straightforward but wasteful for push-heavy workloads.

- **Recommended:** Approach #2 — Push O(1), Pop Amortized O(1). This is the implementation used in the `.c` file. Push is always O(1), and the transfer to `outbox` only happens when `outbox` is empty, amortizing the cost. This approach is better for interviews because:
  1. It demonstrates understanding of amortized analysis, which directly addresses the follow-up question.
  2. It is more efficient in practice for mixed push/pop workloads.
  3. The lazy transfer pattern appears in many real-world data structures (e.g., lazy propagation in segment trees, copy-on-write buffers).
  4. It shows the candidate can reason about worst-case vs. amortized complexity — a key skill interviewers look for.

---

# 232. 用棧實現隊列 — 實作與題解比較

## 實作與題解的對照

`.c` 解法實作的是**題解方法二**（兩個棧 — Push O(1)，Pop 攤還 O(1)），採用延遲轉移（Lazy Transfer）策略。這與題解第二個方法描述的核心演算法相同。

命名上稍有不同：題解使用 `s1`/`s2`，而實作使用 `inbox`/`outbox`，更能描述每個棧的角色。

一個小差異：題解追蹤一個 `front` 變數用於 `s2` 為空時的 peek 操作，而我們的實作在 `outbox` 為空時直接觸發轉移（因此不需要 `front` 追蹤——轉移後 `outbox` 頂部總是持有前端元素）。

## 哪個方法更好

| 面向 | 方法一（Push O(n)，Pop O(1)） | 方法二（Push O(1)，Pop 攤還 O(1)） |
|---|---|---|
| Push 時間 | O(n) — 每次 push 都要搬移所有元素 | O(1) — 只是追加到 inbox |
| Pop 時間 | O(1) — 頂部總是就緒 | 攤還 O(1)，最壞情況 O(n) |
| Peek 時間 | O(1) | O(1)（透過 `front` 變數或轉移） |
| 空間 | O(n) | O(n) |
| 面試適合度 | 簡單易解釋但 push 低效 | 整體更好；攤還分析是強力加分項 |
| C 實作難度 | push 的程式碼稍多 | pop/transfer 的程式碼稍多；整體相近 |

## 基線方案 vs. 推薦方案

- **基線方案：** 方法一 — Push O(n)，Pop O(1)。每次 push 都透過輔助棧轉移所有元素來重建整個棧的順序。直觀但對 push 密集的工作負載是浪費的。

- **推薦方案：** 方法二 — Push O(1)，Pop 攤還 O(1)。這是 `.c` 檔案中使用的實作。Push 永遠是 O(1)，而轉移到 `outbox` 只在 `outbox` 為空時才發生，攤還了成本。此方法更適合面試，因為：
  1. 它展示了對攤還分析的理解，直接回應了題目的進階問題。
  2. 對於混合 push/pop 的工作負載，實際上更有效率。
  3. 延遲轉移模式出現在許多真實世界的資料結構中（例如線段樹的延遲傳播、寫入時複製緩衝區）。
  4. 它展示候選人能夠推理最壞情況與攤還複雜度——這是面試官尋找的關鍵能力。
