# 283. Move Zeroes — Linux Kernel 連結分析

## Linux 相關原始碼位置

| 檔案路徑 | 說明 |
|---|---|
| `lib/string.c` — `memmove()` | 記憶體區塊搬移，處理 overlap 時本質上也是「把有效資料搬到前面，空位留在後面」 |
| `mm/compaction.c` | 記憶體壓縮（memory compaction）：把分散的 used pages 搬到一端，free pages 搬到另一端，與 Move Zeroes 的「非零搬前、零搬後」概念高度相似 |
| `mm/migrate.c` | 頁面遷移（page migration）：NUMA balancing 時把 page 從一個 node 搬到另一個，搬移過程需要維持原始映射順序 |
| `fs/ext4/mballoc.c` | ext4 multi-block allocator：在 buddy bitmap 中尋找連續空閒區塊時，會做類似的 compaction 操作 |
| `include/linux/list.h` — `list_move()`, `list_move_tail()` | 鏈結串列的元素搬移，概念上是「把特定節點移到串列頭/尾」 |
| `kernel/sched/core.c` | 排程器在做 load balancing 時，會把 task 從一個 runqueue「搬」到另一個，維持優先權順序不變 |

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 283 (Move Zeroes) | Linux mm/compaction.c |
|---|---|---|
| 目標 | 非零元素搬前、零搬後 | used pages 搬到一端、free pages 搬到另一端 |
| 資料結構 | 連續陣列（int[]） | page frame 陣列 + migrate/free scanner |
| 掃描方向 | 單向（左到右） | 雙向（migrate scanner 從低往高，free scanner 從高往低） |
| 穩定性 | 穩定（保持非零元素相對順序） | 不保證原始 page 順序（只要求搬到正確 zone） |
| 交換機制 | 陣列元素 swap | page migration（涉及 PTE 更新、TLB flush） |
| 時間複雜度 | O(n) 單次掃描 | O(n) 掃描 + 每次 migration 涉及 lock / flush 開銷 |
| 失敗處理 | 無（陣列操作不會失敗） | migration 可能失敗（page locked、writeback 中等），需要 retry 或 skip |

### 關鍵差異

Memory compaction 是 Move Zeroes 的「進階版」：

1. **雙向掃描** — kernel 使用兩個 scanner 從兩端向中間靠攏（類似 partition），而 LeetCode 只用單向掃描。這是因為 kernel 不需要維持順序穩定性，只需要把 used/free 分開。

2. **搬移成本** — 陣列 swap 是 O(1)，但 page migration 涉及：分配目標 page → 複製內容 → 更新 page table → flush TLB → 釋放原 page。每一步都可能失敗。

3. **併發控制** — kernel 需要 `zone->lock`、`lru_lock` 等鎖，LeetCode 則在單執行緒下操作。

## 主管 Code Review 角度考題

### Q1: 你的 swap 版本跟 kernel 的 compaction 有什麼差別？為什麼 kernel 用雙向掃描？

**預期回答方向**：kernel 不需要穩定排序，只要分成 used/free 兩區即可，所以用 partition-style 雙向掃描更高效。LeetCode 要求維持非零元素相對順序，所以必須用單向掃描。

**追問**：如果 LeetCode 也不要求穩定順序呢？ → 可以用 partition（類似 quicksort 的 partition step），從兩端向中間掃描交換。

**考察重點**：穩定性（stability）的理解、演算法選型依據。

### Q2: 如果陣列極大（比如 1GB），你的方法對 cache 友善嗎？

**預期回答方向**：單向掃描是 sequential access，對 CPU cache 非常友善（prefetch 有效）。Swap 版本兩個指標都往同一方向走，spatial locality 很好。相比之下，kernel 的雙向掃描 cache locality 較差。

**追問**：如果是 NUMA 系統呢？ → 要考慮 page 在哪個 node、跨 node access penalty。kernel 的 `migrate_pages()` 就是處理這個問題。

**考察重點**：cache locality 意識、NUMA awareness。

### Q3: XOR swap 跟 temp-variable swap 你會選哪個？為什麼？

**預期回答方向**：temp-variable swap。理由：
- XOR swap 在 `a == b` 時會歸零（bug 來源）
- 現代 CPU pipeline 下 XOR swap 反而更慢（data dependency chain 更長）
- 可讀性差
- kernel coding style 也偏好簡單直接的寫法

**追問**：那 kernel 裡有用 XOR swap 嗎？ → 幾乎沒有，`include/linux/swab.h` 裡有一些 byte-swap 操作但那是不同用途。

**考察重點**：工程判斷力、對 micro-optimization 迷思的免疫力。

### Q4: 如果這個操作要 thread-safe，你會怎麼改？

**預期回答方向**：
- 最簡單：全域 mutex lock（但失去並行優勢）
- 分段鎖：把陣列切成 chunk，每個 chunk 獨立做 move zeroes，最後合併
- Lock-free：用 CAS（compare-and-swap）做原子交換，但 Move Zeroes 需要維持順序，很難完全 lock-free

**追問**：kernel 的 compaction 怎麼處理併發？ → zone lock + per-page lock + careful scanner coordination

**考察重點**：併發程式設計、鎖粒度選擇。

### Q5: 這個問題的 follow-up 說「能否最少操作次數」，kernel 有類似的最佳化思路嗎？

**預期回答方向**：有。kernel compaction 有 `compact_priority` 機制，可以選擇不同的 compaction 強度（`COMPACT_PRIO_SYNC_LIGHT` vs `COMPACT_PRIO_SYNC_FULL`）。輕量版跳過 locked pages（減少操作），完整版會等待所有 pages（操作更多但成功率更高）。這就是「操作次數」vs「完成度」的 tradeoff。

**考察重點**：對 kernel tuning 的了解、tradeoff 思維。

## 面試加分總結

- **建立類比**：「Move Zeroes 就像簡化版的 memory compaction — 把有效資料壓到一端，空位推到另一端」。這句話能讓面試官知道你理解底層系統。
- **提到穩定性**：主動說明你的解法保持了元素相對順序（stable），而 kernel compaction 不需要穩定性所以可以用雙向 partition。
- **展示 cache 意識**：「單向掃描的 sequential access pattern 對 L1/L2 cache 非常友善，這也是為什麼 kernel compaction 的效能瓶頸不在 scanning 而在 page migration 本身。」
- **工程判斷**：用 temp-variable swap 而非 XOR swap，因為正確性 > 微小效能差異，跟 kernel coding style 一致。
- **擴展討論**：如果面試官問「大規模陣列怎麼辦」，可以引出 NUMA-aware migration 的概念。

## Sources

- [mm/compaction.c (kernel source)](https://github.com/torvalds/linux/blob/master/mm/compaction.c)
- [mm/migrate.c (kernel source)](https://github.com/torvalds/linux/blob/master/mm/migrate.c)
- [include/linux/list.h (kernel source)](https://github.com/torvalds/linux/blob/master/include/linux/list.h)
- [LWN: Memory compaction](https://lwn.net/Articles/368869/)
- [LWN: Proactive compaction](https://lwn.net/Articles/817905/)
