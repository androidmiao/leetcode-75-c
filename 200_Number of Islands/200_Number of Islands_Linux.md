# 200. Number of Islands — Linux Kernel 連結分析

## Linux 相關原始碼位置

| 原始碼路徑 | 說明 |
|-----------|------|
| `lib/find_bit.c` | 位元圖搜尋函式（`find_next_bit`, `find_first_bit`），用於在 bitmap 中線性掃描「已設定」的位元，概念上等同於在網格中掃描 `'1'` |
| `include/linux/bitmap.h` | bitmap 操作巨集與內聯函式，提供 `bitmap_zero()`, `bitmap_set()`, `bitmap_clear()` 等，對應本題中將 `'1'` 設為 `'0'` 的標記操作 |
| `kernel/cgroup/cgroup.c` | cgroup 子系統使用 DFS 遍歷 cgroup 樹狀結構（`css_next_descendant_pre()`），概念上與本題的 DFS 洪水填充相似 |
| `fs/namespace.c` | mount tree 的遍歷（`propagate_mnt()`, `propagation_next()`）使用類似 DFS 的方式走訪掛載點拓撲 |
| `net/core/dev.c` | 網路裝置的鄰居發現與拓撲遍歷，概念上類似在網格中探索相鄰節點 |
| `drivers/base/core.c` | 裝置樹（device tree）的 DFS 遍歷（`device_for_each_child()`），遞迴走訪子裝置 |
| `mm/page_alloc.c` | buddy system 中的區塊合併，概念上類似 Union-Find 的合併操作 |

## Kernel vs. LeetCode 實作對照

| 比較維度 | LeetCode 200 DFS | Linux Kernel 相關實作 |
|---------|-------------------|----------------------|
| 資料結構 | 二維 `char` 網格，直接修改原陣列標記已訪問 | bitmap（`unsigned long` 陣列）+ `test_and_set_bit()` 原子操作標記已處理 |
| 遍歷方式 | 遞迴 DFS，四方向展開 | 迭代式 DFS/BFS 為主（避免核心棧溢出），使用顯式工作佇列或棧 |
| 訪問標記 | 將 `grid[r][c]` 從 `'1'` 改為 `'0'` | 使用 `visited` bitmap 或 `test_and_clear_bit()` 進行原子標記 |
| 棧深度限制 | 無限制（用戶態遞迴） | 核心棧僅 8KB-16KB，必須避免深度遞迴 |
| 並行安全 | 單執行緒，無需同步 | 多 CPU 並行存取，需 spinlock / RCU / atomic 操作 |
| 連通分量 | 計數即可 | 通常需要對每個分量執行操作（初始化裝置、掛載檔案系統等） |

### 核心差異：為何 Kernel 避免遞迴 DFS

Linux 核心棧空間極為有限（x86_64 上通常為 16KB），深度遞迴極易觸發棧溢出導致 kernel panic。因此核心中的圖遍歷幾乎全部採用**迭代式**實作：

- `cgroup` 的 `css_next_descendant_pre()` 使用迭代式前序遍歷
- 裝置樹遍歷使用 `list_for_each_entry()` 迴圈
- 檔案系統掛載傳播使用顯式棧或佇列

若在面試中被問到「如何讓你的 DFS 解法更適合核心環境」，應回答：改為 BFS（使用佇列）或迭代式 DFS（使用顯式棧），並使用 `kmalloc()` / `kfree()` 管理佇列記憶體。

## 主管 Code Review 角度考題

### Q1: 為什麼你選擇遞迴 DFS 而不是迭代式 BFS？

**期望回答方向**：遞迴 DFS 程式碼最簡潔，本題限制 300×300 最壞遞迴深度 90,000 層，用戶態棧空間（預設 8MB）足夠。但在核心態（棧僅 16KB）必須改用迭代式方案。

**考察重點**：棧空間意識、核心態 vs. 用戶態差異

### Q2: 你的 DFS 修改了輸入網格，這在生產環境中有什麼問題？

**期望回答方向**：修改輸入資料違反「不可變輸入」原則。生產環境中可能有其他執行緒同時讀取該資料。替代方案是使用獨立的 `visited` 陣列或 bitmap，以 O(M×N) 額外空間換取安全性。

**追問**：如果記憶體有限，如何在不修改原始資料的情況下減少空間？ → 可用 bitmap（每格僅 1 bit vs. 1 byte），空間減少 8 倍。

**考察重點**：副作用意識、並行安全、空間最佳化

### Q3: 如果網格極大（10^6 × 10^6），你會怎麼做？

**期望回答方向**：單機記憶體不足，需要分塊處理（tiling/chunking）。可以按行或按區塊分割網格，每塊內部用 DFS/BFS 找連通分量，然後用 Union-Find 合併跨邊界的分量。這正是分散式系統中處理大圖的標準模式。

**追問**：分塊邊界的 union 操作如何處理？ → 掃描相鄰塊的邊界行/列，對兩側的分量做 union。

**考察重點**：大規模系統設計能力、分散式演算法

### Q4: 在 Linux 核心中實作此功能，你會如何處理記憶體分配？

**期望回答方向**：使用 `kmalloc()` 分配佇列或棧，注意 `GFP_KERNEL` vs. `GFP_ATOMIC` 旗標選擇。如果在中斷上下文中，不能使用可能睡眠的分配。對於大型分配，考慮使用 `vmalloc()` 或 per-CPU 記憶體池。

**考察重點**：核心記憶體管理 API 熟悉度、上下文感知

### Q5: 如何為你的解法加上單元測試？

**期望回答方向**：使用 KUnit（核心的單元測試框架）撰寫測試案例：空網格、單格、全陸地、全水域、L 形島嶼、對角不相連等邊界情況。每個測試驗證回傳的島嶼數以及網格是否被正確標記。

**考察重點**：測試思維、邊界案例覆蓋

## 面試加分總結

- **提及核心棧限制**：展示對系統底層的理解。解釋為何 300×300 的遞迴在用戶態安全但核心態不行，並提出 BFS / 迭代 DFS 替代方案。
- **bitmap 標記**：提到可用 `test_and_set_bit()` 風格的原子操作取代直接修改輸入，展示並行安全意識。
- **分塊 + Union-Find**：面對超大網格時提出分塊策略，說明跨邊界合併的方法，展示大規模系統設計能力。
- **cgroup / device tree 類比**：將 DFS 洪水填充與 cgroup 樹遍歷或裝置樹走訪做類比，展示對核心子系統的熟悉度。
- **KUnit 測試**：主動提到測試策略和邊界案例，展示工程紀律。

## Sources

- [lib/find_bit.c — Linux kernel source (GitHub)](https://github.com/torvalds/linux/blob/master/lib/find_bit.c)
- [include/linux/bitmap.h — Linux kernel source (GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h)
- [kernel/cgroup/cgroup.c — cgroup DFS traversal (GitHub)](https://github.com/torvalds/linux/blob/master/kernel/cgroup/cgroup.c)
- [fs/namespace.c — mount propagation traversal (GitHub)](https://github.com/torvalds/linux/blob/master/fs/namespace.c)
- [mm/page_alloc.c — buddy system merge (GitHub)](https://github.com/torvalds/linux/blob/master/mm/page_alloc.c)
- [drivers/base/core.c — device tree traversal (GitHub)](https://github.com/torvalds/linux/blob/master/drivers/base/core.c)
