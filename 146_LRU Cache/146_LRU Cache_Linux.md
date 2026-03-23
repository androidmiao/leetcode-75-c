LRU 是 kernel 的核心機制——page cache 的回收策略、slab allocator 的 reclaim、dentry cache 的淘汰全部基於 LRU 或其變體（如 LRU-2Q）, 請列出Linux相關的code在哪裡？若以主管Code Review的角度, 可能會怎麼考?



# 146. LRU Cache — Linux Kernel LRU 機制與 Code Review 考題

LRU 是 kernel 的核心機制——page cache 的回收策略、slab allocator 的 reclaim、dentry cache 的淘汰全部基於 LRU 或其變體（如 LRU-2Q）。

---

## Linux Kernel LRU 程式碼位置

### 1. Page Cache LRU / Page Reclaim（頁面回收）

這是 kernel 中最核心的 LRU 應用，決定記憶體不夠時先淘汰哪些 page。

| 檔案 | 關鍵函式 / 結構 | 用途 |
|---|---|---|
| `mm/vmscan.c` | `shrink_lruvec()`, `shrink_inactive_list()`, `isolate_lru_folios()` | 頁面回收主引擎，掃描 LRU 串列決定淘汰誰 |
| `mm/swap.c` | `folio_rotate_reclaimable()`, `lru_add_fn()`, `folio_batch_move_lru()` | LRU 串列操作：加入、旋轉、批次搬移 |
| `mm/workingset.c` | working set 偵測 | 追蹤 page 的歷史存取頻率，判斷是否為 working set |
| `include/linux/mmzone.h` | `struct lruvec` | 核心資料結構，每個 node/memcg 持有 4 條 LRU 串列 |

傳統模型維護 **4 條 LRU 串列**（Active/Inactive × Anonymous/File）：`LRU_INACTIVE_ANON`, `LRU_ACTIVE_ANON`, `LRU_INACTIVE_FILE`, `LRU_ACTIVE_FILE`。這本質上是 **LRU-2Q** 的變體——新 page 先進 inactive list，被再次存取後才晉升到 active list，避免一次性大量 I/O 沖掉 working set。

### 2. Multi-Gen LRU（MGLRU，Linux 6.1+）

由 Google 的 Yu Zhao 開發，將傳統 2 級 active/inactive 推廣到**多世代**追蹤。

| 檔案 | 關鍵函式 / 結構 | 用途 |
|---|---|---|
| `mm/lru_gen.c` | `lru_gen_add_folio()`, `lru_gen_del_folio()`, `lru_gen_age_node()`, `lru_gen_scan_node()` | 多世代 LRU 的新增、刪除、老化、掃描 |
| `include/linux/mmzone.h` | `struct lru_gen_folio` | 每個世代的 folio 追蹤：`max_seq`, `min_seq[]`, `folios[gen][type][zone]` |

效果：kswapd CPU 使用降低約 40%，OOM kill 減少約 85%。runtime 開關在 `/sys/kernel/mm/lru_gen/enabled`。

### 3. Slab Allocator Reclaim（Slab 物件回收）

Slab 中的物件（如 dentry、inode）不直接放在 page LRU 上，而是透過 `list_lru` + `shrinker` 回呼機制。

| 檔案 | 關鍵函式 / 結構 | 用途 |
|---|---|---|
| `mm/list_lru.c` | `list_lru_add()`, `list_lru_del()`, `list_lru_walk_node()` | 通用的物件級 LRU 串列（per-node, per-memcg） |
| `include/linux/list_lru.h` | `struct list_lru`, `struct list_lru_one` | LRU 容器；每個 node 一條串列 + 計數器 + spinlock |
| `mm/slub.c` | `kmem_cache_shrink()`, `shrink_slab()` | shrinker 框架，記憶體壓力時回呼各 slab cache 的 shrink 函式 |

設計重點：`struct shrinker` 有一個 `seeks` 權重欄位，表示重新建立該物件的代價——代價越高越不願意回收。

### 4. Dentry Cache LRU（目錄項快取）

| 檔案 | 關鍵函式 / 結構 | 用途 |
|---|---|---|
| `fs/dcache.c` | `dentry_lru_add()`, `dentry_lru_del()`, `prune_dcache_sb()` | dentry 的 LRU 加入/移除/按 superblock 修剪 |
| `include/linux/dcache.h` | `struct dentry`（`d_lru` 成員） | dentry 結構中的 LRU 鏈結欄位 |

---

## 主管 Code Review 角度的考題

以 `146_LRU Cache.c` 為基礎，一個熟悉 kernel 的主管可能會從以下方向追問：

### Level 1：基礎正確性

**Q1：「你的 hash table 碰撞處理為什麼選 separate chaining？open addressing 不行嗎？」**

期望回答：兩者都可以。Chaining 實作簡單、delete 容易（直接從鏈上摘除），不需要處理 tombstone。Open addressing（linear probing / Robin Hood hashing）cache locality 更好，但刪除需要標記 tombstone 或 backward-shift，在 C 中實作複雜度高。LeetCode 場景下 chaining 是最務實的選擇。Kernel 的 `rhashtable`（resizable hash table）用的是 separate chaining + bucket lock。

**Q2：「`HASH_SIZE = 10007` 是怎麼選的？如果 key 範圍改成 0~10^9 呢？」**

期望回答：10007 是大於 10000（key 上限）的質數，保證在目前約束下幾乎不碰撞。若 key 範圍擴大，需要考慮 load factor 控制在 0.75 左右，可能需要動態 resize（類似 kernel 的 `rhashtable` 用 RCU + resize worker）。

### Level 2：並行與鎖

**Q3：「如果這個 LRU Cache 要在多執行緒環境使用，你怎麼加鎖？」**

期望回答：最簡單是一把大鎖保護整個結構。但 kernel 的做法更細緻——`lruvec` 用 `lru_lock`（spinlock）保護串列操作，`list_lru_one` 每個 node 有自己的 spinlock。進階方案是讀寫分離：hash table 用 RCU 讀、寫端加鎖；串列操作用 per-node lock 降低競爭。

**Q4：「kernel 的 page LRU 為什麼用 batch 操作（`folio_batch`）而不是每次操作都直接改 LRU 串列？」**

期望回答：減少 lock contention。每次 `lru_add` 不是立刻拿 lru_lock 插入，而是先放進 per-CPU 的 pagevec/folio_batch，攢滿一批後一次性拿鎖批量插入。這是典型的 **batching amortization** 技巧——LeetCode 解法沒有這個需求，但在高併發場景是必要的。

### Level 3：設計演進

**Q5：「傳統 active/inactive 兩級 LRU 有什麼問題？MGLRU 怎麼解決？」**

期望回答：傳統 2 級模型的 aging 精度太粗——一個 page 只有「active」和「inactive」兩個狀態，晉升/降級的判斷依賴 reference bit 的單次掃描。MGLRU 引入多個世代（generation），每個 page 帶有世代編號，aging 時透過 page table walk 直接檢查 access bit 而非依賴 LRU 串列掃描順序，精度更高、掃描量更少。

**Q6：「你的 `removeNode` + `insertBeforeTail` 拆成兩步，kernel 裡的 `list_move_tail()` 是一步完成的。哪種更好？」**

期望回答：`list_move_tail()` 是原子操作——先 `list_del()` 再 `list_add_tail()`，跟我的做法本質一樣，但封裝成一個函式呼叫。在有鎖的環境下這很重要：如果 remove 和 insert 之間被中斷，串列會處於不一致狀態。面試中拆成兩步更清楚；production code 應封裝成 `moveToTail()`。

### Level 4：記憶體管理

**Q7：「`lRUCacheFree` 裡你先走串列 free 所有 Node，再走 hash table free 所有 HashNode。如果 free 順序反過來會怎樣？」**

期望回答：不影響正確性，因為 Node 和 HashNode 是獨立的 malloc 區塊。但如果改成 slab-style 的 memory pool（所有 Node 從同一塊記憶體分配），就需要注意釋放順序和 use-after-free。Kernel 中 slab 的 destroy 順序是先從 LRU 移除所有物件的參照、再 `kmem_cache_destroy()`。

**Q8：「你用了兩種 malloc 的 struct（Node 和 HashNode）。能不能合併成一個？」**

期望回答：可以，讓 Node 同時包含 `hash_next` 指標，就不需要獨立的 HashNode。好處是減少 malloc 次數（從每次 put 的 2 次降為 1 次）和 memory fragmentation。壞處是 Node 變大（多一個指標），即使在 hash chain 很短的情況下也佔空間。Kernel 的 `struct page`（現在是 `struct folio`）就是這種「一個結構身兼多職」的設計哲學。

---

## 總結

這些問題的共同主軸是：**從 LeetCode 的單執行緒、固定約束場景，推展到 kernel 的多核、動態負載、記憶體壓力場景**。主管想確認的是你能否看到自己寫的程式碼在 production 環境下的局限性，以及知道業界（kernel）是怎麼解決這些局限的。
