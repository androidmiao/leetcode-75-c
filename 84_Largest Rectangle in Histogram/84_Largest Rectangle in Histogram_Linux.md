# LeetCode 84: Largest Rectangle in Histogram — Linux Kernel 實作對照

## 問題概述

LeetCode 84 要求在柱狀圖中找出最大矩形面積，核心演算法使用**單調堆疊 (monotonic stack)**。在 Linux 核心中，堆疊結構和單調序列的概念廣泛應用於記憶體管理、排程器、以及網路子系統。

---

## 1. Linux 相關原始碼位置

### 1.1 核心堆疊實作 — `lib/stack_depot.c`
- **位置**: `lib/stack_depot.c` / `include/linux/stackdepot.h`
- **用途**: 儲存和去重核心堆疊追蹤 (stack traces)，用於 KASAN、記憶體洩漏偵測等
- **相似性**: 使用堆疊結構儲存歷史資訊，類似本題的索引堆疊

### 1.2 VMA (Virtual Memory Area) 合併 — `mm/mmap.c`
- **位置**: `mm/mmap.c` 中的 `vma_merge()`
- **用途**: 合併相鄰且屬性相同的虛擬記憶體區域
- **相似性**: 掃描有序結構，找出可合併的相鄰區間，概念上類似尋找柱子能延伸的最大範圍

### 1.3 CFS 排程器的時間片管理 — `kernel/sched/fair.c`
- **位置**: `kernel/sched/fair.c`
- **用途**: 完全公平排程器 (CFS) 使用紅黑樹管理行程的虛擬執行時間
- **相似性**: 需要高效地維護有序序列和極值查詢，類似單調堆疊維護遞增序列

### 1.4 直方圖統計 — `lib/math/statistics.c` 和各子系統
- **位置**: `include/linux/log2.h`、`fs/ext4/mballoc.c`（buddy allocator 統計）
- **用途**: ext4 的多區塊配置器使用直方圖統計自由區塊大小分布
- **相似性**: 直接的直方圖 (histogram) 資料結構，分析區塊大小分布

### 1.5 perf 事件的直方圖功能 — `tools/perf/`
- **位置**: `tools/perf/builtin-sched.c`、`tools/perf/util/hist.c`
- **用途**: perf 工具使用直方圖分析延遲分布、排程統計
- **相似性**: 直接處理直方圖資料，統計各 bucket 的計數

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 堆疊使用模式對比

| 比較面向 | LeetCode 84 單調棧 | Linux Kernel Stack Depot |
|---------|-------------------|------------------------|
| **目的** | 找每根柱子的左右邊界 | 儲存和去重 stack traces |
| **資料結構** | 陣列模擬棧（索引陣列） | 雜湊表 + 棧池 (slab) |
| **Push 條件** | 當前高度 ≥ 棧頂高度 | 每次新的 stack trace |
| **Pop 條件** | 當前高度 < 棧頂高度 | 無 pop — 歷史資料永久保留 |
| **不變量** | 棧中索引對應的高度嚴格遞增 | 雜湊槽中 stack trace 唯一 |
| **哨兵** | 首尾各一個高度 0 的假柱子 | NULL 終止符 |

### 2.2 區間合併思維對比

| 比較面向 | LeetCode 84 | Linux VMA Merge |
|---------|-------------|-----------------|
| **掃描方式** | 左到右線性掃描 | 依地址排序的鏈結串列/紅黑樹 |
| **合併判斷** | pop 時計算矩形（以棧頂高度為最矮） | 檢查相鄰 VMA 的 flags、file 是否相同 |
| **時間複雜度** | O(n) 攤提 | O(log n) 單次查詢（紅黑樹） |
| **記憶體節省** | 不適用 | 合併減少 VMA 數量，節省結構體記憶體 |

### 2.3 哨兵技巧在核心中的應用

Linux 核心大量使用哨兵節點 (sentinel / dummy node) 來簡化邊界條件：

- **鏈結串列**: `list_head` 使用空的 head 節點作為哨兵，插入/刪除不需判斷空串列
- **紅黑樹**: `rb_root` 的 NIL 節點（`NULL`）作為所有葉子的哨兵
- **本題**: `new_heights[0] = 0` 和 `new_heights[n+1] = 0` 作為左右哨兵

這種技巧在核心中被稱為「dummy head pattern」，與本題的哨兵陣列概念完全一致。

---

## 3. 主管 Code Review 角度考題

### Q1：為什麼選擇在陣列前後加哨兵，而不是在迴圈中用條件判斷？
- **預期答案方向**: 哨兵消除分支預測失敗的可能性，在熱迴圈 (hot loop) 中減少條件判斷可改善流水線效率。額外 O(n) 記憶體複製代價通常小於分支預測錯誤的代價。
- **追問**: 如果 n 極大（接近記憶體上限），你還會使用陣列複製嗎？
- **考察重點**: 效能分析能力、空間 vs. 時間 trade-off 意識

### Q2：這個 malloc/free 模式在核心中可以用嗎？核心中你會怎麼配置這個堆疊？
- **預期答案方向**: 核心中避免在中斷上下文使用 `kmalloc(GFP_KERNEL)`，可改用 `kmalloc(GFP_ATOMIC)` 或預分配。若 n 有上限（如 10⁵），可用 VLA 或 `alloca`（但核心已禁止 VLA）。最安全的做法是 `kmalloc_array()` 搭配溢位檢查。
- **追問**: stack overflow 的風險呢？核心棧大小限制是多少？
- **考察重點**: 核心記憶體配置 API 熟悉度、棧空間預算意識

### Q3：如果輸入陣列極大且不可複製，如何修改演算法避免 new_heights 的 O(n) 額外空間？
- **預期答案方向**: 改用棧哨兵策略（push −1 到棧底），直接操作原始陣列，主迴圈後加清理迴圈。空間從 2×O(n) 降到 1×O(n)。
- **追問**: 這樣做會影響 cache 行為嗎？
- **考察重點**: 記憶體存取模式優化、cache locality 意識

### Q4：單調棧的攤提分析 (amortized analysis) 怎麼證明是 O(n)？
- **預期答案方向**: 使用聚合分析 (aggregate analysis)：每個元素最多被 push 一次、pop 一次。n 次 push + 至多 n 次 pop = 2n 次操作，攤提到每個元素為 O(1)。
- **追問**: 在最壞情況下（已排序輸入），pop 操作的分布是什麼樣的？
- **考察重點**: 複雜度分析能力、攤提分析的正式論證

### Q5：這個算法的 cache 行為如何？可以改善嗎？
- **預期答案方向**: `new_heights` 陣列的線性掃描是 cache-friendly 的。棧的隨機存取深度受限於 n，通常在 L1/L2 cache 中。對比分治法（Approach 3），棧法的 cache 局部性更好，因為不需要遞迴。
- **追問**: 如果柱子數量超過 L2 cache 大小呢？
- **考察重點**: 快取層次意識、實際效能分析

---

## 4. 面試加分總結

- **哨兵技巧**：提及 Linux `list_head` 的 dummy node 設計，展示工程直覺一致性
- **單調棧**：強調攤提 O(n) 的證明方式，展示演算法分析深度
- **記憶體配置**：討論 `malloc` vs. 核心 `kmalloc_array` 的差異，展示系統程式設計經驗
- **Cache 行為**：比較棧法 vs. 分治法的 cache locality，展示效能優化意識
- **防禦性程式設計**：討論整數溢位風險（h × w 可能超過 int 範圍）和 malloc 失敗處理

---

## Sources

- [Linux Stack Depot — stackdepot.h](https://github.com/torvalds/linux/blob/master/include/linux/stackdepot.h)
- [Linux VMA merge — mm/mmap.c](https://github.com/torvalds/linux/blob/master/mm/mmap.c)
- [Linux list_head sentinel pattern — include/linux/list.h](https://github.com/torvalds/linux/blob/master/include/linux/list.h)
- [ext4 multiblock allocator — fs/ext4/mballoc.c](https://github.com/torvalds/linux/blob/master/fs/ext4/mballoc.c)
- [perf histogram utilities — tools/perf/util/hist.c](https://github.com/torvalds/linux/blob/master/tools/perf/util/hist.c)
