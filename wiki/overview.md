# 專業學習地圖 — LeetCode / C / Kernel 三維整合

> 本頁是整個 `leetcode-75-c` 知識庫的最頂層 synthesis。
> 把 75 題實戰 + 15 個演算法模式 + 9 個 Kernel 子系統 + C 語言慣用寫法，整合成一張可循序攻讀的學習地圖。
> 建議從這裡開始閱讀，然後按興趣 / 需要往下鑽進各個 `patterns/`、`kernel/`、`c-idioms/`、`interview/` 檔案。

---

## 0. 這套專業到底在學什麼

面試者常把「刷題」當成一件事。實際上 Google / Meta / 系統程式職缺要求的是三個能力面同時成立：

| 維度 | 觀察指標 | 對應 wiki 目錄 |
|------|----------|----------------|
| **演算法思維** | 遇到新題能在 30 秒內分類到正確模式、說出複雜度 | `patterns/` |
| **C 語言工程力** | 指標、記憶體、邊界、整數溢位都能手寫、口述 invariant | `c-idioms/` + 各題 `_Memory.md` |
| **系統程式視野** | 知道這題在 Linux Kernel、網路、資料庫等真實系統裡怎麼被用 | `kernel/` + 各題 `_Linux.md` |

本專業的核心在於：**把三個維度同步打通**，而不是只會其中一個。這張地圖就是依這三個維度交叉編排的。

---

## 1. 演算法思維層：15 個核心模式

這 15 個 pattern 幾乎涵蓋 95% 的 LeetCode 面試題型。先按「辨識信號」記起來，而不是按題目。

### 1.1 陣列 / 字串基礎 (4 個 pattern)

| Pattern | 辨識信號 | 代表題 | 檔案 |
|---------|----------|--------|------|
| **Two Pointers** | 排序陣列、對撞或快慢、O(1) space | 11, 167, 141, 234 | [[patterns/two-pointers]] |
| **Sliding Window** | 連續子陣列/子字串、窗口內統計量 | 3, 76, 239, 424, 567 | [[patterns/sliding-window]] |
| **Prefix Sum** | 區間和、區間詢問 O(1)、差分 | 724, 1732, 238 | [[patterns/prefix-sum]] |
| **String Manipulation** | 反轉、分詞、大小寫、in-place 操作 | 151, 345, 1768, 2390 | [[patterns/string-manipulation]] |

**學習順序建議**：Two Pointers → Sliding Window → Prefix Sum → String。前三者共享「線性掃描 + 雙指標/累加」的思維內核。

### 1.2 資料結構驅動 (5 個 pattern)

| Pattern | 辨識信號 | 代表題 | 檔案 |
|---------|----------|--------|------|
| **Hash Table** | O(1) 查找、去重、頻率統計、配對 | 1, 49, 217, 242, 347 | [[patterns/hash-table]] |
| **Linked List** | 節點操作、反轉、找中點、環偵測 | 21, 23, 141, 206, 234 | [[patterns/linked-list]] |
| **Stack / Queue** | 配對、LIFO/FIFO、層序遍歷 | 20, 155, 232, 739 | [[patterns/stack-queue]] |
| **Heap (Priority Queue)** | 第 k 大/小、data stream、合併 k 路 | 23, 215, 295, 347, 703 | [[patterns/heap-priority-queue]] |
| **Monotonic Stack** | 下一個更大元素、柱狀圖面積 | 84, 239, 496, 739 | [[patterns/monotonic-stack]] |

**學習順序建議**：Hash Table → Stack/Queue → Linked List → Heap → Monotonic Stack。由易至難，Monotonic Stack 是「stack 的進階用法」。

### 1.3 搜尋與樹圖 (4 個 pattern)

| Pattern | 辨識信號 | 代表題 | 檔案 |
|---------|----------|--------|------|
| **Binary Search** | 排序陣列、單調函數上找答案、找邊界 | 33, 35, 162, 704, 374 | [[patterns/binary-search]] |
| **Tree** | 二元樹 DFS / BFS、遞迴分解、路徑問題 | 104, 199, 700 | [[patterns/tree]] |
| **BFS / DFS / Graph** | 連通性、最短步數、島嶼、拓樸 | 133, 200, 207, 210, 994 | [[patterns/bfs-dfs-graph]] |
| **Backtracking** | 全部組合/排列、剪枝、狀態回退 | 17, 39, 46 | [[patterns/backtracking]] |

**學習順序建議**：Binary Search → Tree → BFS/DFS/Graph → Backtracking。Backtracking 本質是 DFS + 狀態回滾。

### 1.4 優化與決策 (2 個 pattern)

| Pattern | 辨識信號 | 代表題 | 檔案 |
|---------|----------|--------|------|
| **Dynamic Programming** | 最佳子結構、重疊子問題、狀態轉移方程 | 53, 121, 122, 152, 334 | [[patterns/dynamic-programming]] |
| **Greedy** | 局部最優=全域最優、證明可 swap | 605, 121, 122 | [[patterns/greedy]] |

**學習順序建議**：Greedy → DP。Greedy 常是 DP 的特例（當子問題獨立時），建議先從 Greedy 入手建立直覺。

### 1.5 缺口補充 pattern (見 `health/2026-04-18_lint.md`)

為了完整覆蓋 Google / Meta 面試常考範圍，本知識庫補上四個單獨檔案：

| Pattern | 辨識信號 | 檔案 |
|---------|----------|------|
| **Union-Find** | 動態連通、連通分量計數、冗餘邊 | [[patterns/union-find]] |
| **Trie** | 字首匹配、字典、字串搜尋 | [[patterns/trie]] |
| **Bit Manipulation** | 單獨數字、位元遮罩 DP、XOR 性質 | [[patterns/bit-manipulation]] |
| **Topological Sort** | 有向圖排序、課程依賴、建構順序 | [[patterns/topological-sort]] |

---

## 2. C 語言工程力層：4 大主題

面試官真正在乎的 C 語言能力不是語法，而是「能不能手寫並口述指標與記憶體操作」。

| 主題 | 核心能力 | 檔案 |
|------|----------|------|
| **指標操作** | 指向指標、前後指標、dummy node、pointer-to-pointer 簡化邊界 | [[c-idioms/pointer-manipulation]] |
| **手動記憶體** | malloc / realloc / free pairing、擴容策略、記憶體洩漏檢查 | [[c-idioms/manual-memory]] |
| **比較器模式** | qsort / bsearch 的 callback、穩定性、強弱排序 | [[c-idioms/comparator-patterns]] |
| **巨集技巧** | container_of、ARRAY_SIZE、min/max、list_for_each | [[c-idioms/macro-tricks]] |

**學習順序建議**：指標 → 記憶體 → 比較器 → 巨集。每題 `_Memory.md` 都是這四個主題的實戰練習材料。

---

## 3. 系統程式視野層：9 個 Kernel 子系統

這一層是本知識庫最具辨識度的部分。面試時若能在談完 LeetCode 解法後延伸到 Linux Kernel 的真實實作，立即和 90% 的候選人拉開距離。

| 子系統 | 對應的 LeetCode 題型 | 關鍵洞察 | 檔案 |
|--------|----------------------|-----------|------|
| **list_head** (雙向環形鏈表) | 所有鏈表題 (206, 141, 234, 23) | **侵入式設計**：指標嵌進 struct，一個節點可在多條鏈上 | [[kernel/list-head]] |
| **rbtree** (紅黑樹) | BST (700), 樹深度 (104), 右視圖 (199) | 無通用比較函數；父指標+顏色位壓縮；rb_leftmost O(1) | [[kernel/rbtree]] |
| **hash-table** (hlist / rhashtable) | Two Sum (1), Group Anagrams (49), Top K (347) | hlist 省 50% 記憶體；rhashtable 無鎖 resize | [[kernel/hash-table]] |
| **sort / search** (Introsort + list_sort) | Kth (215, 703), Binary Search (704) | Introsort 混合 quicksort + heapsort；棧預算 4-8KB | [[kernel/sort-search]] |
| **memory-management** (LRU / MGLRU / slub) | LRU Cache (146) | Kernel LRU 複雜性來自多流互動；slab 避免外碎片 | [[kernel/memory-management]] |
| **graph-cycle** (lockdep / depmod) | 207, 210, 141 | 核心選 BFS 不選 DFS — 棧預算 + 延遲可預測性 | [[kernel/graph-cycle]] |
| **circular-buffer** (kfifo / printk ringbuf) | 232, 239, 933 | 冪次大小 + 位運算；in/out 指標永不 reset | [[kernel/circular-buffer]] |
| **network-sliding-window** (TCP cwnd) | 76, 239, 3, 424, 480, 567, 643 | 相對序列號比較避免 32-bit 環繞；RED 隨機丟棄避免同步 | [[kernel/network-sliding-window]] |
| **spinlock** (同步原語) | Mon-3 code review（Bug #4） | 唯一能在 atomic context 使用的鎖；busy-wait、禁止睡眠；三變體擋 process / softirq / IRQ | [[kernel/spinlock]] |

**學習順序建議**：基礎資料結構 (list_head → hash-table → rbtree) → 演算法內核 (sort-search → graph-cycle) → 子系統應用 (memory-management → circular-buffer → network-sliding-window) → 同步原語 (spinlock)。

---

## 4. 面試實戰層：4 份 cheat sheet

當你把前三層都打通，面試時真正決勝的是「怎麼講、怎麼回應 follow-up、怎麼接 code review」。

| 主題 | 用途 | 檔案 |
|------|------|------|
| **複雜度速查** | 每個資料結構 / 演算法的時間與空間複雜度表 | [[interview/complexity-cheatsheet]] |
| **邊界案例** | 空輸入、單元素、整數溢位、極值測試 | [[interview/edge-cases]] |
| **Follow-up 模式** | 面試官常見的追問方向與應對 | [[interview/follow-up-patterns]] |
| **Code Review 考題** | 主管角度的程式碼審查問題清單 | [[interview/code-review-questions]] |

---

## 5. 學習路徑建議

依據程度與時間，提供三種攻讀順序。

### 5.1 有基礎、備戰 4-6 週面試（本帳號的目標）

**Week 1：指標功夫 + 陣列字串**
- 讀 `c-idioms/pointer-manipulation.md` + `c-idioms/manual-memory.md`
- 掃 `patterns/two-pointers` / `sliding-window` / `prefix-sum`
- 刷題對照：1, 11, 121, 122, 167, 238, 283, 643, 724, 1732
- Kernel 延伸：`kernel/network-sliding-window.md`

**Week 2：資料結構實作**
- `patterns/linked-list` + `patterns/hash-table` + `patterns/stack-queue`
- 刷題：21, 23, 141, 146, 206, 234, 20, 155, 232, 739
- Kernel 延伸：`kernel/list-head.md` + `kernel/hash-table.md` + `kernel/circular-buffer.md`

**Week 3：搜尋、樹、圖**
- `patterns/binary-search` + `patterns/tree` + `patterns/bfs-dfs-graph` + `patterns/topological-sort`
- 刷題：33, 35, 104, 199, 200, 207, 210, 700, 704, 994
- Kernel 延伸：`kernel/rbtree.md` + `kernel/graph-cycle.md`

**Week 4：優化模式 + 進階資料結構**
- `patterns/dynamic-programming` + `patterns/greedy` + `patterns/monotonic-stack` + `patterns/heap-priority-queue` + `patterns/union-find` + `patterns/trie` + `patterns/bit-manipulation`
- 刷題：53, 152, 215, 295, 347, 703, 84, 239, 605
- Kernel 延伸：`kernel/sort-search.md` + `kernel/memory-management.md`

**Week 5-6：模擬面試 + Code Review 訓練**
- 每題用 3 層結構回答：模式辨識 → C 實作 → Kernel 延伸
- 讀 `interview/` 四份 cheat sheet，每份做一輪 mock 問答
- 回頭做健康檢查（見下）

### 5.2 純初學者：系統化入門路徑

依 §1-§3 的學習順序建議，每個 pattern 先只做 1-2 題最 Easy 版本，目標是建立「模式辨識 → 模板套用」的反射。第一輪完成後再回頭吃 Medium / Hard。

### 5.3 進階工程師：從系統程式反推面試

從 `kernel/` 開始讀 — 把每個 kernel 子系統當主題，每個主題下做相關 LeetCode 題目。對已有系統程式背景的人，這條路徑最能把既有知識結構化。

---

## 6. 如何使用本知識庫

### 6.1 典型操作

| 需求 | 做法 |
|------|------|
| **遇到新題，不知道哪個模式** | 先看 [[_index]] 的「快速查找：問題 → 知識映射」，或讀 `patterns/` 每個檔最上方的「辨識信號」 |
| **需要模板程式碼** | 去 `patterns/<模式>.md` 找「C 語言模板」段落 |
| **複習某個 Kernel 子系統** | 去 `kernel/<子系統>.md`；每個檔案底部都列了相關 LeetCode 題 |
| **模擬 code review** | 去 `interview/code-review-questions.md` 挑幾題問自己 |
| **查詢複雜度** | 去 `interview/complexity-cheatsheet.md` |

### 6.2 知識庫的維護節奏

依照 LLM-wiki 方法論：

- **每完成 5-10 題** → 跑一次「AI 編譯」：讓 Agent 掃新的題目資料夾，更新對應的 `patterns/` 與 `kernel/` 檔案，追加 log
- **每 1-2 週** → 跑一次「健康檢查」：產出 `health/YYYY-MM-DD_lint.md`，找出缺口、矛盾、陳舊資訊
- **每次有價值的問答** → 讓 Agent 把摘要回存到 `logs/`，並合併新知識點到對應 wiki 檔

### 6.3 三個不要

1. **不要自己寫 wiki/** — 所有 `wiki/` 檔案由 LLM agent 維護，你寫在各題 `_Opus-4.6.md` 裡。
2. **不要只刷題不編譯** — 沒編譯成 pattern 知識的題目，等於只背了答案。
3. **不要只讀不驗證** — 每個模式至少自己 from scratch 寫一題，才算吃下。

---

## 7. 本知識庫的統計概覽

| 項目 | 數量 |
|------|------|
| Raw 原始素材題目資料夾 | 75 |
| Wiki patterns 檔案 | 15 (含本次補充 4 個：union-find, trie, bit-manipulation, topological-sort) |
| Wiki c-idioms 檔案 | 4 |
| Wiki kernel 檔案 | 8 |
| Wiki interview 檔案 | 4 |
| Wiki 總行數 | ≈ 9,500+ (含新補 pattern 後) |
| 最後 lint 日期 | 2026-04-18 (見 `health/2026-04-18_lint.md`) |

---

## 8. 參見

- [[_index]] — 總目錄（含 kernel 詳細索引與「問題 → 知識映射」表）
- [[../knowledge-base-system]] — 本知識庫的 schema 與方法論（= LLM-wiki 的 CLAUDE.md）
- [[../SKILL]] — 每題資料夾的逐題 workflow 規範
- [[../llm-wiki]] — Karpathy 原版 LLM-wiki 方法論
