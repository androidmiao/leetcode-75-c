`list_head` 操作基本功，kernel patch review 常見，請列出 Linux 相關的 code 在哪裡？若以主管 Code Review 的角度，可能會怎麼考？

# 206. Reverse Linked List — Linux Kernel 關聯與 Code Review 考題

---

## Linux Kernel 相關檔案總覽

**核心分析文件（已建立）：**

| 檔案 | 內容 |
|------|------|
| `146_LRU Cache/146_LRU Cache_Linux.md` | 最完整的一份——kernel LRU 機制對照、程式碼位置（`mm/vmscan.c`, `mm/swap.c` 等）、8 道 Code Review 考題 |
| `10-day-sprint-plan.md` | 全部題目按 kernel 相關度分類（★ ~ ★★★），十天衝刺計畫 |
| `problem-list_linux.md` / `problem-list_linux_todo.md` | 題目清單與進度追蹤 |

**Linked List 題目（`list_head` 直接相關，★★★）：**

| 題目 | 路徑 | Kernel 對應 |
|------|------|-------------|
| 206 Reverse Linked List | `206_Reverse Linked List/` + `206-reverse-linked-list/` | `list_head` 指標反轉操作 |
| 141 Linked List Cycle | `141_Linked List Cycle/` | 偵測環形結構（防止 kernel bug） |
| 160 Intersection of Two Linked Lists | `160_Intersection of Two Linked Lists/` | 兩條串列交匯偵測 |
| 21 Merge Two Sorted Lists | `21_Merge Two Sorted Lists/` | `list_merge()` 合併操作 |
| 23 Merge k Sorted Lists | `23_Merge k Sorted Lists/` | 多路合併（排程器、IO 合併） |
| 146 LRU Cache | `146_LRU Cache/` | page cache LRU、dentry cache |
| 328 Odd-Even Linked List | `328-odd-even-linked-list/` | 串列重排（partition 操作） |

---

## 主管 Code Review 角度怎麼考 `list_head` 基本功

以 206 Reverse Linked List 為例，一個熟悉 kernel 的主管會從你的解法出發，層層追問：

### Level 1：正確性與風格

**「你的 `while(1) + break` 和 editorial 的 `while(curr)` 有什麼差別？kernel 用哪種？」**

期望回答：kernel 的 `list_for_each_safe()` 本質上是 `while` 搭配預存 next 的模式，跟 editorial 風格一致。`while(1) + break` 容易在改動時漏掉 break 條件，review 時會被要求改掉。

### Level 2：封裝與 API 設計

**「kernel 的 `list_head` 為什麼把反轉拆成 `list_del()` + `list_add()` 而不是提供 `list_reverse()`？」**

期望回答：kernel 串列操作的哲學是提供原子級小操作讓呼叫者組合，而非提供高階函式。這降低了 API 維護成本，也讓鎖的粒度由呼叫者決定。

### Level 3：並行安全

**「如果你的 `reverseList` 要在 kernel 中使用，中間被中斷會怎樣？」**

期望回答：反轉過程中串列處於不一致狀態（部分反轉）。kernel 中必須持有對應的 spinlock（如 `lru_lock`）保護整段操作，或用 RCU 讓讀者看到一致的舊版本。

### Level 4：`container_of` 理解

**「kernel 的 `list_head` 嵌在 struct 內部，你怎麼從 `list_head*` 拿到外層 struct？」**

期望回答：`container_of(ptr, type, member)` 巨集，透過 `offsetof` 計算成員在結構體中的偏移量，將 `list_head*` 轉回外層結構體指標。這是 kernel 鏈結串列與 LeetCode 鏈結串列最根本的設計差異——kernel 的節點不持有資料，資料結構持有節點。

### Level 5：效能實務

**「`folio_batch` 的 batching 跟你逐節點操作有什麼差別？」**

期望回答：kernel 用 per-CPU batch 收集操作再一次拿鎖批量執行，大幅降低 lock contention。這是從 LeetCode 的單執行緒世界到 kernel 多核環境最關鍵的跨越。

---

## 總結

這些考題的共同主軸是：**從 LeetCode 的單執行緒、固定約束場景，推展到 kernel 的多核、動態負載、記憶體壓力場景**。主管想確認的是你能否看到自己寫的程式碼在 production 環境下的局限性，以及知道業界（kernel）是怎麼解決這些局限的。

完整版（含 8 道 Q&A）在 `146_LRU Cache/146_LRU Cache_Linux.md`，那是目前專案中最完整的 kernel code review 準備材料。
