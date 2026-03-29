# 234. Palindrome Linked List — Linux Kernel 連結分析

## Linux 相關原始碼位置

本題的核心技巧有三個：**快慢指標尋找中點**、**原地反轉 linked list**、以及**雙指標比較**。Linux kernel 中大量使用 linked list，其中反轉操作與雙向串列操作在多個子系統中皆有對應。

| 檔案路徑 | 角色說明 |
|---|---|
| `include/linux/list.h` | kernel 雙向環形 linked list（`struct list_head`）的核心巨集與 inline 函式，包括 `list_add`、`list_del`、`list_move`、`list_splice` 等 |
| `lib/list_sort.c` | 通用 linked list merge sort 實作，使用 bottom-up merge sort；其中合併階段需要反覆操作 `next`/`prev` 指標 |
| `include/linux/list.h` → `list_cut_position()` | 將串列從指定位置切斷為兩段——概念上等同於本題的「找到前半段末尾後切斷」|
| `include/linux/list.h` → `list_splice()` | 將兩段串列重新接合——概念上等同於本題 Step 4 的「復原」操作 |
| `kernel/sched/fair.c` | CFS 排程器中使用 RB-tree 但也有 linked list 走訪與比較的模式 |
| `net/core/skbuff.c` | socket buffer（`sk_buff`）鏈結串列的操作，包括 `skb_queue_reverse_walk` 反向走訪 |

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 234 實作 | Linux Kernel linked list |
|---|---|---|
| **串列類型** | Singly linked list（`struct ListNode { val, *next }`） | Doubly-linked circular list（`struct list_head { *next, *prev }`） |
| **找中點** | 快慢指標（fast 走 2 步、slow 走 1 步） | 不需要——雙向串列可直接從兩端同時走，或用 `list_cut_position()` 在已知位置切斷 |
| **反轉操作** | 三指標滾動（prev/curr/next）原地反轉 singly linked list | 雙向串列不需要專門反轉——反向走訪直接用 `list_for_each_prev()` 巨集；若需物理反轉可交換每個節點的 `next`/`prev` |
| **比較方式** | 兩個指標從兩端同步走，逐一比較 `val` | kernel 中通常用 `list_for_each_entry()` 配合自訂 comparator；不直接做「回文比較」 |
| **復原操作** | 再次反轉後半段，重新接回 `firstHalfEnd->next` | `list_splice()` 可將切斷的子串列重新接合；雙向串列的切斷/接合為 O(1) |
| **記憶體管理** | 題目假設節點已存在，不做 malloc/free | kernel 使用 slab allocator（`kmem_cache_alloc`/`kmem_cache_free`）管理節點記憶體 |
| **侵入式設計** | 非侵入式——node 結構包含 val 和 next | 侵入式——`list_head` 嵌入到 container 結構中，用 `container_of()` 取得外層結構 |
| **並行安全** | 無鎖——函式說明中提及需外部鎖定 | 必須配合 spinlock（`spin_lock`/`spin_unlock`）或 RCU（`rcu_read_lock`）保護 |

### 關鍵差異：為什麼 kernel 不需要快慢指標？

Linux kernel 的 `list_head` 是雙向環形串列，任何節點都可以透過 `prev` 指標反向走訪。因此：

- **找中點**：可以同時從 head 和 tail（`head->prev`）往中間走，O(n/2) 步即可相遇。
- **反轉**：不需要——直接用 `list_for_each_prev()` 反向走訪即可。
- **回文判斷**：從頭尾同時走，逐一比較值，若相遇或交錯則完成。

這是 singly linked list 相對於 doubly linked list 的核心劣勢：singly linked list 必須用快慢指標+原地反轉來模擬雙向存取，而 doubly linked list 天生就支持。

## 主管 Code Review 角度考題

### Q1：為什麼選擇原地反轉而非複製到陣列？

**期望回答方向：** O(1) 空間；在 kernel 等記憶體敏感環境中，動態分配一個 O(n) 陣列可能觸發 page fault 或 OOM。原地操作避免了額外的記憶體壓力。

**追問：** 「如果這個 linked list 有上百萬個節點、且系統記憶體壓力很大時，malloc O(n) 陣列可能會失敗，你怎麼處理？」

**真正在考：** 記憶體管理意識、對系統資源壓力的敏感度。

### Q2：在反轉期間，另一個執行緒讀取這個串列會發生什麼？

**期望回答方向：** 串列在 Step 2 到 Step 4 之間暫時被切斷為兩段，另一個執行緒可能看到不一致的狀態（部分反轉）。需要加鎖（spinlock 或 mutex）或使用 RCU 來保護。

**追問：** 「kernel 中你會選擇 spinlock 還是 mutex？為什麼？」→ 取決於是否在中斷上下文中；spinlock 適用於不可睡眠的上下文。

**真正在考：** 並行程式設計意識、鎖的選型能力。

### Q3：`endOfFirstHalf` 的迴圈條件為什麼是 `fast->next != NULL && fast->next->next != NULL` 而不是 `fast != NULL && fast->next != NULL`？

**期望回答方向：** 兩種寫法會讓 `slow` 停在不同位置。目前的寫法確保 `slow` 停在前半段的**最後一個節點**（而非後半段的第一個節點），這樣 `firstHalfEnd->next` 就是後半段的起點，也方便 Step 4 復原。另一種寫法 `slow` 會多走一步，需要額外邏輯來處理切斷與復原。

**追問：** 「如果串列只有 1 個節點或 2 個節點，這個條件還正確嗎？」→ 是的，1 個節點時 `fast->next == NULL`，不進迴圈，`slow = head`，後半段為空；2 個節點時同理。

**真正在考：** 邊界條件處理、指標安全存取（避免 NULL dereference）。

### Q4：復原操作（Step 4）是必要的嗎？LeetCode 上不復原也能 AC。

**期望回答方向：** 就 LeetCode 而言確實不必要。但在生產環境中，caller 可能仍持有對原始串列的引用，如果不復原就改變了 caller 看到的資料結構，這是一個隱藏的副作用。在 kernel 中，修改了共享資料結構卻不復原可能導致其他子系統行為異常。

**追問：** 「如果你在 kernel 中寫了一個函式修改了傳入的 `list_head` 但沒有在文件中說明，code review 會怎麼看？」→ 這違反了 least surprise 原則；kernel 社群非常重視函式的副作用文件化。

**真正在考：** 防禦性程式設計意識、API 設計原則。

### Q5：這個演算法的 cache locality 如何？

**期望回答方向：** Linked list 節點散布在 heap 的各處，每次 `node->next` 跳轉都可能是一次 cache miss。相比之下，陣列方法（Approach 1）把值複製到連續記憶體中，比較階段的 cache hit rate 會高很多。在 kernel 中，`sk_buff` 等高頻存取的 linked list 會使用 slab allocator 來盡量讓同類節點在相鄰的 cache line 上。

**追問：** 「如果你要在 kernel 中對一個大型 linked list 做回文判斷，你會怎麼優化 cache 效能？」

**真正在考：** Cache locality 意識、硬體與軟體的交互理解。

## 面試加分總結

- **提及 `list_head` 的雙向環形設計**：展示你了解 kernel linked list 的基本結構，以及為什麼 singly linked list 需要更多技巧來模擬雙向存取。
- **討論 `list_cut_position()` 和 `list_splice()`**：表明你知道 kernel 提供了原生的「切斷」和「接合」操作，這正好對應本題的 Step 1（切斷）和 Step 4（復原）。
- **強調並行安全**：主動提到 spinlock/RCU，而不是等面試官問。
- **提及 cache locality 的權衡**：說明為什麼陣列方法在 cache 效能上可能更好，而 linked list 的優勢在於 O(1) 插入/刪除。
- **復原操作 = 防禦性程式設計**：強調不修改 caller 的資料結構是好的 API 設計，與 kernel 社群的 coding style 一致。

## Sources

- [include/linux/list.h — kernel `list_head` 巨集定義](https://github.com/torvalds/linux/blob/master/include/linux/list.h)
- [lib/list_sort.c — kernel linked list merge sort](https://github.com/torvalds/linux/blob/master/lib/list_sort.c)
- [net/core/skbuff.c — socket buffer 鏈結串列操作](https://github.com/torvalds/linux/blob/master/net/core/skbuff.c)
- [LWN.net — The kernel linked-list API](https://lwn.net/Articles/735472/)
