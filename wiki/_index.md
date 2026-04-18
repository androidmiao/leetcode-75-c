# LeetCode 面試知識庫 — 總目錄

> 本知識庫依 Karpathy / LLM-wiki 方法論維護。整合 75 題實戰原始素材（`raw/` = 各題資料夾）與 AI 編譯的跨題知識（`wiki/`）。
>
> **入門順序**：從 [[overview]] 開始看全景圖 → 再依需要往下鑽進各個子目錄。

---

## 目錄結構

```
wiki/
├── _index.md             ← 本檔案（總目錄）
├── overview.md           ← 專業學習地圖 / 最頂層 synthesis
├── patterns/             ← 15 個算法模式
├── c-idioms/             ← 4 個 C 語言工程主題
├── kernel/               ← 8 個 Linux Kernel 子系統
├── interview/            ← 4 份面試實戰 cheat sheet
├── health/               ← 健康檢查報告（定期產生）
└── logs/                 ← 每次 session 的問答與 ingest 紀錄
```

### 相關文件（repo 根目錄）

- `SKILL.md` — 逐題初始化 / 編輯的 workflow 規範
- `knowledge-base-system.md` — 本知識庫的 schema 與維護方法論
- `llm-wiki.md` — Karpathy 原版 LLM-wiki 方法論
- `ALGORITHM_CATEGORIZATION.txt` — 按模式分類的 75 題對照表
- `C_IDIOMS_REPORT.md` — C 語言慣用寫法提煉報告

---

## 🗺️ 核心頁面

| 頁面 | 用途 |
|------|------|
| [[overview]] | **從這裡開始**。整個專業的學習地圖，把 patterns / c-idioms / kernel / interview 四層整合成一張循序攻讀路徑 |
| [[health/2026-04-18_lint]] | 最新一次健康檢查報告；缺口清單、矛盾檢查、下一步建議 |

---

## 📚 Patterns（15 個算法模式）

### 陣列 / 字串基礎

- [[patterns/two-pointers]] — 雙指標（對撞、快慢、同向）
- [[patterns/sliding-window]] — 滑動窗口（固定、可變、count-based）
- [[patterns/prefix-sum]] — 前綴和、差分、區間查詢
- [[patterns/string-manipulation]] — 反轉、分詞、in-place 操作

### 資料結構驅動

- [[patterns/hash-table]] — 查找、去重、配對、頻率統計
- [[patterns/linked-list]] — 節點操作、反轉、環偵測、中點
- [[patterns/stack-queue]] — LIFO / FIFO、括號匹配、monotonic
- [[patterns/heap-priority-queue]] — 第 k 大、data stream、合併 k 路
- [[patterns/monotonic-stack]] — 下一個更大元素、柱狀圖

### 搜尋與樹圖

- [[patterns/binary-search]] — 排序陣列、單調函數找答案、找邊界
- [[patterns/tree]] — 二元樹遞迴、DFS / BFS、路徑
- [[patterns/bfs-dfs-graph]] — 連通性、最短步數、島嶼
- [[patterns/backtracking]] — 全排列、組合、剪枝、狀態回退
- [[patterns/topological-sort]] — 依賴排序、DAG、Kahn / DFS 三色
- [[patterns/trie]] — 字首樹、自動補全、二進位 XOR Trie
- [[patterns/union-find]] — 並查集、動態連通、冗餘邊

### 優化與決策

- [[patterns/dynamic-programming]] — 狀態轉移方程、最佳子結構
- [[patterns/greedy]] — 局部最優 = 全域最優、swap argument
- [[patterns/bit-manipulation]] — XOR 消去、狀態壓縮、位元數學

---

## 🔧 C Idioms（4 個工程主題）

- [[c-idioms/pointer-manipulation]] — 指標操作、dummy node、雙指標、pointer-to-pointer
- [[c-idioms/manual-memory]] — malloc / realloc / free 配對、擴容策略
- [[c-idioms/comparator-patterns]] — qsort / bsearch callback、穩定性、強弱排序
- [[c-idioms/macro-tricks]] — `container_of`、`ARRAY_SIZE`、`BIT(n)`、list 遍歷

---

## 🐧 Kernel（8 個 Linux 子系統）

### 基礎資料結構

**[[kernel/list-head]]** — 雙向環形鏈表
- 侵入式設計、`container_of`、`list_for_each_safe`
- 源碼：`include/linux/list.h`, `lib/list_sort.c`
- 相關題：206, 141, 160, 21, 23, 876, 2095, 2130, 328, 234

**[[kernel/rbtree]]** — 紅黑樹
- 父指標 + 顏色位壓縮、無通用搜尋函數、`rb_leftmost` O(1)
- 源碼：`include/linux/rbtree.h`, `lib/rbtree.c`
- 應用：VMA tree、CFS scheduler、ext4 extents
- 相關題：700, 104, 199

**[[kernel/hash-table]]** — hlist / rhashtable
- hlist 省 50% 記憶體、GOLDEN_RATIO 乘法雜湊、rhashtable 無鎖 resize
- 源碼：`include/linux/hashtable.h`, `lib/rhashtable.c`
- 應用：inode cache、dentry cache、netfilter conntrack
- 相關題：1, 49, 217, 219, 242, 347, 76, 3

### 演算法內核

**[[kernel/sort-search]]** — Introsort + bsearch + list_sort
- Introsort（quicksort + heapsort 混合）、棧預算 4-8 KB
- 溢位安全的二分搜尋、鏈表底部向上歸併
- 源碼：`lib/sort.c`, `lib/bsearch.c`, `lib/list_sort.c`
- 相關題：215, 217, 347, 692, 703, 704

**[[kernel/graph-cycle]]** — Lockdep / depmod
- BFS（非 DFS）環偵測：棧預算 + 延遲可預測
- Kahn 拓樸排序於 module dependency
- 源碼：`kernel/locking/lockdep.c`
- 相關題：207, 210, 141

### 系統應用

**[[kernel/memory-management]]** — LRU / MGLRU / slub
- 多世代 LRU 減少鎖爭用、slab 避免外碎片、批處理 folio_batch
- 源碼：`mm/vmscan.c`, `mm/swap.c`, `mm/slub.c`
- 相關題：146

**[[kernel/circular-buffer]]** — kfifo / printk ringbuf
- 冪次大小 + 位運算、in/out 永不 reset、`CIRC_CNT`
- 源碼：`include/linux/circ_buf.h`, `include/linux/kfifo.h`, `lib/kfifo.c`
- 相關題：232, 239, 933, 642

**[[kernel/network-sliding-window]]** — TCP snd_wnd / snd_cwnd
- 相對序列號比較避免 32-bit 環繞、雙層流控、RED 隨機丟棄
- 源碼：`net/ipv4/tcp_input.c`, `tcp_output.c`, `net/core/red.c`
- 相關題：76, 239, 3, 424, 567, 643, 480

---

## 🎯 Interview（4 份實戰 cheat sheet）

- [[interview/complexity-cheatsheet]] — 每個資料結構 / 演算法的時間與空間複雜度表
- [[interview/edge-cases]] — 空輸入、單元素、極值、整數溢位、重複元素
- [[interview/follow-up-patterns]] — 面試官常見追問方向與應對
- [[interview/code-review-questions]] — 主管角度的 code review 考題清單

---

## 📅 日誌與健康檢查

- [[logs/2026-04-18_session]] — 本次 session 的建構紀錄
- [[health/2026-04-18_lint]] — 本次健康檢查報告

---

## 🔍 快速查找：問題 → 知識映射

### 按 Kernel 概念分類

**list_head 相關**：206, 141, 160, 21, 23, 876, 2095, 2130, 328, 234

**hash-table 相關**：1, 49, 217, 219, 242, 347, 76, 3

**sort-search 相關**：215, 217, 347, 692, 703, 704

**memory-management 相關**：146

**graph-cycle 相關**：207, 210, 141

**circular-buffer 相關**：232, 239, 933, 642

**network-sliding-window 相關**：76, 239, 3, 424, 567, 643, 480

**rbtree 相關**：700, 104, 199

### 按 Pattern 快速查

以下列出有明確題目對應的 pattern（無對應題但仍完整收錄的 pattern 也見 patterns/ 檔案本身）：

- **Two Pointers**：11, 167, 141, 234, 876, 283, 392
- **Sliding Window**：3, 76, 239, 424, 567, 643, 480, 219, 1456
- **Prefix Sum**：724, 1732, 238
- **String Manipulation**：151, 345, 1768, 2390, 605
- **Hash Table**：1, 49, 217, 242, 347, 2215, 1
- **Linked List**：19, 21, 23, 141, 160, 206, 234, 328, 876, 2095, 2130
- **Stack / Queue**：20, 155, 232, 739, 2390
- **Heap**：23, 215, 295, 347, 703, 692
- **Monotonic Stack**：84, 239, 496, 739
- **Binary Search**：33, 35, 162, 374, 704
- **Tree**：104, 199, 700
- **BFS / DFS / Graph**：133, 200, 207, 210, 841, 994, 1926
- **Backtracking**：17
- **Topological Sort**：207, 210
- **DP**：53, 121, 122, 152, 334
- **Greedy**：121, 122, 605
- **Union-Find / Trie / Bit Manipulation**：repo 內 LeetCode 75 無直接題，但屬 Google 面試常考補充模式（見對應 pattern 檔）

---

## 📊 知識庫統計

| 類別 | 檔案數 | 總行數 |
|------|--------|--------|
| Patterns | 19 | ~7,200 |
| C Idioms | 4 | ~1,400 |
| Kernel | 8 | ~2,900 |
| Interview | 4 | ~1,600 |
| Overview + Index + Health + Log | 4 | ~900 |
| **總計** | **39** | **~14,000** |

- **Raw 題目資料夾**：75（部分含舊/新雙重目錄待清理，見健康檢查）
- **涵蓋 Kernel 源代碼位置**：30+
- **最後更新**：2026-04-18
- **最近一次 lint**：2026-04-18（見 [[health/2026-04-18_lint]]）

---

## 🧭 使用建議

### 對於面試準備（有基礎者，4-6 週）

1. **Week 1**：`c-idioms/pointer-manipulation` + `c-idioms/manual-memory` + `patterns/two-pointers` + `patterns/sliding-window` + `patterns/prefix-sum`
2. **Week 2**：`patterns/linked-list` + `patterns/hash-table` + `patterns/stack-queue` + `kernel/list-head` + `kernel/hash-table` + `kernel/circular-buffer`
3. **Week 3**：`patterns/binary-search` + `patterns/tree` + `patterns/bfs-dfs-graph` + `patterns/topological-sort` + `kernel/rbtree` + `kernel/graph-cycle`
4. **Week 4**：`patterns/dynamic-programming` + `patterns/greedy` + `patterns/monotonic-stack` + `patterns/heap-priority-queue` + `patterns/union-find` + `patterns/trie` + `patterns/bit-manipulation`
5. **Week 5-6**：模擬面試 + 所有 `interview/` 內容 + 回頭做健康檢查

完整學習路徑見 [[overview]]。

### 對於代碼審查

- 檢查清單見 [[interview/code-review-questions]]
- Kernel 視角的效能考量：棧預算、整數溢位、鎖爭用瓶頸

### 對於系統設計

- VMA 管理 → [[kernel/rbtree]]
- 進程調度 → [[kernel/rbtree]]（CFS）
- 網路協議 → [[kernel/network-sliding-window]]
- 快取系統 → [[kernel/hash-table]] + [[kernel/memory-management]]

---

## 📖 參考資源

### 官方文檔

- [Linux Kernel Documentation](https://www.kernel.org/doc/)
- [LWN.net](https://lwn.net/)
- RFC 793（TCP 協議）

### 推薦閱讀

1. *Understanding the Linux Kernel* — Bovet & Cesati
2. *The Linux Programming Interface* — Michael Kerrisk
3. LeetCode 官方解題卡

### 進階主題（未涵蓋 / TODO）

- **RCU（Read-Copy-Update）** — 無鎖讀優化
- **Bitmaps / `include/linux/bitmap.h`** — 高效集合表示（部分於 `patterns/bit-manipulation` 觸及）
- **Work Queue** — 異步任務隊列
- **Memory Cgroups** — 資源隔離
- **eBPF** — 核心內虛擬機
