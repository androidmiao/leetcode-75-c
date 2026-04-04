# LeetCode 面試知識庫 — 總目錄

## 概述

本知識庫整合 LeetCode 75 問題、常見解題模式、C 語言慣用寫法以及 Linux Kernel 核心資料結構與算法實現。旨在幫助軟體工程師在系統設計面試和實戰編程中深入理解基礎概念。

## Linux Kernel 交叉索引 (kernel/)

### 8 個核心主題

#### 1. **list_head.md** — 雙向環形鏈結串列
- **核心概念**：侵入式（intrusive）設計 vs 外部鏈表
- **關鍵 API**：list_add, list_del, list_entry, container_of, list_for_each_safe
- **源代碼**：include/linux/list.h, lib/list_sort.c
- **相關 LeetCode**：206, 141, 160, 21, 23, 876, 2095, 2130, 328, 234
- **面試考點**：
  - 侵入式設計為何節省記憶體？
  - 環形 vs 線性終止的權衡？
  - 如何安全地在遍歷中刪除？

**文檔大小**：200 行

---

#### 2. **rbtree.md** — 紅黑樹
- **核心概念**：自平衡搜尋樹，O(log n) 保證
- **關鍵特性**：
  - 父指標 + 顏色位壓縮（節省記憶體）
  - 無通用搜尋函數（各子系統自定義比較）
  - rb_leftmost 快取 O(1) 最小值訪問
- **源代碼**：include/linux/rbtree.h, lib/rbtree.c, rbtree_augmented.h
- **核心應用**：
  - VMA 樹（mm/mmap.c）：虛擬記憶體區域管理
  - CFS 調度器（kernel/sched/fair.c）：任務時間線
  - Deadline 調度器：按截止日期排序
  - ext4 範圍樹（fs/ext4/extents.c）：磁碟範圍管理
- **相關 LeetCode**：700, 104, 199
- **面試考點**：
  - 為何不提供通用搜尋函數？
  - rb_leftmost 為何重要？
  - 紅黑樹 vs AVL 樹的權衡？

**文檔大小**：150 行

---

#### 3. **hash-table.md** — 核心雜湊表
- **核心概念**：hlist（單向）vs list_head（雙向），鏈接法 vs 開放定址
- **關鍵實現**：
  - hlist：節省 50% 記憶體
  - GOLDEN_RATIO 乘法雜湊
  - rhashtable：調整大小的無鎖雜湊表
  - RCU 保護的無鎖讀
- **源代碼**：include/linux/hashtable.h, hash.h, jhash.h, lib/rhashtable.c
- **核心應用**：
  - Inode 快取（fs/inode.c）
  - Dentry 快取（fs/namei.c）
  - 連接追蹤（net/netfilter/nf_conntrack_core.c）
- **相關 LeetCode**：1, 49, 217, 219, 242, 347, 76, 3
- **面試考點**：
  - hlist 為何選擇單向而非雙向？
  - GOLDEN_RATIO 的離散特性？
  - rhashtable 如何實現無鎖調整大小？

**文檔大小**：150 行

---

#### 4. **sort-search.md** — 排序與搜尋
- **核心算法**：
  - Introsort（Quicksort + Heapsort 混合）
  - 溢出安全的二分搜尋
  - 鏈表底部向上歸併排序
- **設計約束**：棧預算限制（4-8 KB）
- **源代碼**：lib/sort.c, lib/bsearch.c, lib/list_sort.c
- **複雜度**：
  - Introsort：最壞 O(n log n)，棧深度受限
  - 二分搜尋：O(log n)，無整數溢出
  - 鏈表排序：O(n log n)，原地歸併
- **相關 LeetCode**：215, 217, 347, 692, 703, 704
- **面試考點**：
  - 為何 Introsort 而非純 Quicksort？
  - 二分搜尋如何避免整數溢出？
  - 鏈表排序為何選歸併而非快速排序？

**文檔大小**：120 行

---

#### 5. **memory-management.md** — 記憶體管理
- **核心概念**：
  - 傳統 2-LRU 和 MGLRU（多世代 LRU）
  - 頁面回收（mm/vmscan.c, mm/swap.c）
  - Slab 分配器（mm/slub.c）
  - 批處理優化（folio_batch）
- **設計洞察**：
  - LRU 複雜性來自多流交互
  - MGLRU：減少鎖爭用，提高精度
  - Slab：避免外碎片，O(1) 分配
  - 批處理：減少鎖操作 97%
- **源代碼**：mm/vmscan.c, mm/swap.c, mm/slub.c, mm/page_alloc.c
- **相關 LeetCode**：146（LRU 快取）
- **面試考點**：
  - Kernel LRU 為何比 LeetCode 複雜？
  - MGLRU 如何改進 2-LRU？
  - Slab 分配相比 malloc 的優勢？

**文檔大小**：150 行

---

#### 6. **graph-cycle.md** — 圖論與環偵測
- **核心算法**：
  - BFS 環偵測（kernel/locking/lockdep.c）
  - Kahn 拓撲排序（模組依賴）
- **設計約束**：
  - DFS 遞迴深度無界，棧溢出風險
  - BFS 隊列分配，O(V + E) 保證
  - 實時系統不能有不可預測延遲
- **應用**：
  - Lockdep：動態死鎖檢測
  - depmod：模組依賴拓撲排序
- **相關 LeetCode**：207, 210, 141
- **面試考點**：
  - BFS vs DFS 在 Kernel 中的選擇？
  - Lockdep 如何檢測死鎖循環？
  - Kahn 算法如何自然偵測環？

**文檔大小**：120 行

---

#### 7. **circular-buffer.md** — 環形緩衝區
- **核心概念**：
  - FIFO，無邊界檢查，O(1) 操作
  - 位運算優化（冪次大小）
  - 計數宏：CIRC_CNT, CIRC_SPACE, CIRC_CNT_TO_END
  - kfifo：泛型、線程安全、分隔讀寫
- **源代碼**：include/linux/circ_buf.h, include/linux/kfifo.h, lib/kfifo.c
- **應用**：
  - 日誌環（kernel/printk/printk_ringbuffer.c）
  - UART 隊列（drivers/tty/serial/8250/）
  - 追蹤環（kernel/trace/ring_buffer.c）
- **相關 LeetCode**：232, 239, 933, 642
- **面試考點**：
  - 為何環形必須冪次大小？
  - CIRC_CNT 如何處理環繞？
  - 為何 in/out 指標永不重置？

**文檔大小**：100 行

---

#### 8. **network-sliding-window.md** — 網路滑動窗口
- **核心概念**：
  - 發送窗口（snd_wnd）和擁塞窗口（snd_cwnd）
  - 流量控制和擁塞控制
  - 序列號環繞（32-bit）處理
  - RED（隨機早期檢測）隊列管理
- **源代碼**：net/ipv4/tcp_input.c, tcp_output.c, tcp_timer.c, net/core/red.c
- **設計洞察**：
  - 相對序列號比較避免環繞點失敗
  - 兩個窗口交互實現雙層流控
  - RED 避免全局同步，改善吞吐量
- **相關 LeetCode**：76, 239, 3, 424, 567, 643, 480
- **面試考點**：
  - 為什麼需要相對序列號比較？
  - snd_wnd 和 snd_cwnd 的區別？
  - RED 為何使用隨機丟棄？

**文檔大小**：100 行

---

## 知識體系統計

### 核心 Kernel 子系統

| 子系統 | 檔案數 | 主要概念 | 複雜度 |
|--------|--------|--------|--------|
| 鏈表和樹 | 2 | list_head, rbtree | 基礎 |
| 雜湊表 | 1 | hlist, rhashtable | 中級 |
| 排序/搜尋 | 1 | Introsort, 二分搜尋 | 中級 |
| 記憶體 | 1 | LRU, MGLRU, Slab | 高級 |
| 同步/圖 | 1 | Lockdep, BFS | 中級 |
| 隊列 | 1 | 環形緩衝, FIFO | 基礎 |
| 網路 | 1 | 滑動窗口, 擁塞控制 | 高級 |

**總計**：8 個檔案，1200 行內容，涵蓋 30+ 核心源代碼位置

---

## 快速查找：問題 → 知識映射

### 按 Kernel 概念分類

**list_head 相關**
- 206：反轉鏈表
- 141：環形鏈表檢測
- 160：相交鏈表
- 21：合併兩個排序鏈表
- 23：合併 K 個排序鏈表
- 876：鏈表的中點
- 2095：刪除鏈表中間的節點
- 2130：鏈表最大孿生和
- 328：奇偶位置節點重組
- 234：回文鏈表檢測

**hash-table 相關**
- 1：兩數之和
- 49：字母異位詞分組
- 217：存在重複元素
- 219：存在重複元素 II
- 242：有效的字母異位詞
- 347：前 K 個高頻元素
- 76：最小覆蓋子字符串
- 3：無重複字符的最長子字符串

**sort-search 相關**
- 215：陣列中第 K 大元素（quickselect）
- 217：存在重複元素（排序）
- 347：前 K 高頻（堆或排序）
- 692：前 K 個高頻單詞
- 703：流的第 K 大元素（堆）
- 704：二分搜尋

**memory-management 相關**
- 146：LRU 快取設計

**graph-cycle 相關**
- 207：課程表
- 210：課程表 II（拓撲排序）
- 141：環形鏈表（環偵測）

**circular-buffer 相關**
- 232：用棧實現隊列
- 239：滑動窗口最大值
- 933：最近的請求次數
- 642：搜索自動完成系統

**network-sliding-window 相關**
- 76：最小覆蓋子字符串
- 239：滑動窗口最大值
- 3：無重複字符的最長子字符串
- 424：用至多 k 次操作替換字符
- 567：字符串排列
- 643：子陣列最大平均數 I
- 480：滑動窗口中位數

**rbtree 相關**
- 700：二叉搜尋樹搜尋
- 104：二叉樹最大深度
- 199：二叉樹右視圖

---

## 使用建議

### 對於面試準備

1. **第 1 週**：掌握基礎資料結構
   - list_head：理解侵入式設計
   - hash-table：hlist vs list_head 的權衡
   - circular-buffer：環形設計的位運算優化

2. **第 2 週**：深入算法
   - sort-search：Introsort 的自適應性
   - graph-cycle：BFS vs DFS 的實時性考量
   - memory-management：LRU 的多層設計

3. **第 3 週**：系統設計視角
   - rbtree：應用於調度和記憶體管理
   - network-sliding-window：擁塞控制的雙層設計
   - 整合跨領域概念

### 對於代碼審查

1. 檢查清單：
   - 是否考慮了棧預算（recursion vs iteration）？
   - 是否避免了整數溢出（序列號比較）？
   - 是否有鎖爭用瓶頸（批處理）？

2. 效能優化：
   - 位運算代替模運算（circular buffer）
   - 快取局部性（list_head vs 外部指標）
   - 無邊界檢查（環形設計）

### 對於系統設計

- **VMA 管理**：rbtree + 二分搜尋，O(log n) 映射查詢
- **進程調度**：rbtree 追蹤優先級，O(1) 選擇下一任務
- **網路協議**：滑動窗口 + RED，避免全局同步
- **快取系統**：雜湊表 + LRU，O(1) 訪問 + 老化淘汰

---

## 統計概覽

- **Kernel 檔案總計**：8 個
- **涵蓋源代碼位置**：30+ 處
- **LeetCode 問題覆蓋**：40+ 個（重點 75 中的 40）
- **總文檔行數**：~1200 行
- **更新日期**：2026-04-04

---

## 參考資源

### 官方文檔
- [Linux Kernel Documentation](https://www.kernel.org/doc/)
- [LWN.net](https://lwn.net/)
- RFC 793（TCP 協議）

### 推薦閱讀
1. *Understanding the Linux Kernel* - Bovet & Cesati
2. *The Linux Programming Interface* - Michael Kerrisk
3. LeetCode 官方解題卡

### 進階主題（未涵蓋）
- **RCU（Read-Copy-Update）**：無鎖讀優化
- **Bitmaps 和 Bitmasks**：高效集合表示
- **Work Queue**：異步任務隊列
- **Memory Cgroups**：資源隔離
- **eBPF**：核心內虛擬機
