# LeetCode 334: Increasing Triplet Subsequence 與 Linux Kernel 連接分析

## 概述

LeetCode 334 問題採用**貪心單遍掃描**算法，維護兩個運行最小值 (`first` 和 `second`) 來偵測遞增三元組子序列。這種**單遍閾值追蹤**的方法在 Linux 內核中有多個類似的應用場景，特別是在網路擁塞控制、記憶體管理、和調度器負載追蹤等領域。

---

## 1. Linux 相關原始碼位置

### 1.1 TCP 擁塞控制 (Congestion Control)
**檔案**: `net/ipv4/tcp_*.c` (例如 `tcp_bbr.c`, `tcp_cubic.c`, `tcp_westwood.c`)

**相關概念**: 貪心追蹤 RTT 最小值，偵測 ACK 模式變化
- `net/ipv4/tcp_bbr.c` — BBR 算法維護 `bw_lo`, `bw_hi` 邊界進行帶寬偵測
- `net/ipv4/tcp_cubic.c` — 追蹤 `last_max_cwnd` 和當前窗口，單遍檢測擁塞

**核心邏輯**: 當接收 ACK 時，即時更新運行狀態，檢測是否觸發擁塞回避或增長狀態轉換。

### 1.2 記憶體管理 - 浮水位追蹤 (Watermark Tracking)
**檔案**: `mm/page_alloc.c`, `mm/vmscan.c`, `mm/kswapd.c`

**相關概念**: 維護 `min_free_kbytes`, `water_mark[HIGH/LOW/MIN]`，單遍掃描檢測記憶體壓力
- `mm/page_alloc.c:get_page_from_freelist()` — 檢查是否低於浮水位，觸發直接回收或喚醒 kswapd
- 類似 LeetCode 334：當前自由記憶體是新到達的數值，與 HIGH/LOW 浮水位進行比較

**核心邏輯**: 每次分配時進行單遍檢查，決定是否觸發記憶體回收動作。

### 1.3 調度器負載追蹤 (Scheduler Load Tracking)
**檔案**: `kernel/sched/fair.c`, `kernel/sched/core.c`

**相關概念**: 維護 `min_vruntime`, `max_vruntime`, 以及 CPU 利用率的運行最小/最大值
- `kernel/sched/fair.c:enqueue_task_fair()` — 更新負載權重，檢查是否觸發負載平衡
- Linux 3.8+ 採用 Per-entity load tracking，單遍更新運行時間戳

**核心邏輯**: 任務入隊時單遍檢查負載值，決定是否進行 CPU 遷移。

### 1.4 排序與比較模式 (Sort & Comparator Patterns)
**檔案**: `lib/sort.c`

**相關概念**: 單遍遍歷數組，維護運行的最小/最大值進行分區或快速選擇
- `lib/sort.c:sort()` / `lib/bsearch.c` — 某些變體採用貪心分區策略

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 算法模式對比

| 維度 | LeetCode 334 | Linux Kernel (TCP BBR/記憶體管理) |
|------|-------------|----------------------------------|
| **掃描方式** | 單遍，從左到右線性掃描 | 事件驅動（ACK 到達時、分配時、調度時）|
| **狀態維護** | 兩個運行最小值 (`first`, `second`) | 多個浮水位或閾值邊界 (`min`, `max`, `high`, `low`) |
| **閾值檢測** | 當 `num > second` 時立即返回 | 當超過浮水位時觸發動作 (`kswapd_wakeup`, 擁塞信號) |
| **動態更新** | 逐個元素更新 `first` 和 `second` | 每個事件（ACK、分配請求、任務入隊）更新狀態 |
| **目標** | 檢測存在性（boolean） | 決策動作（觸發回收、調整窗口、遷移任務） |

### 2.2 資料結構對比

#### LeetCode 334
```cpp
int first = INT_MAX;      // 第一個最小值
int second = INT_MAX;     // 第二個最小值
for (int num : nums) {
    if (num <= first) {
        first = num;
    } else if (num <= second) {
        second = num;
    } else {
        return true;      // 找到三元組
    }
}
```

#### Linux Kernel - TCP BBR (淡化版本)
```c
/* kernel/sched/fair.c / net/ipv4/tcp_bbr.c */
struct bbr {
    u32 bw_lo;           // 下界帶寬估計
    u32 bw_hi;           // 上界帶寬估計
    u64 min_rtt;         // 最小 RTT 觀測值
    u32 cwnd;            // 當前擁塞窗口
};

// 在 ACK 處理中
if (ack_rtt > min_rtt_measured) {
    if (cwnd > bw_hi) {
        // 觸發擁塞迴避
        cwnd = bw_lo;
    }
}
```

#### Linux Kernel - 記憶體浮水位追蹤
```c
/* mm/page_alloc.c */
unsigned long watermark[NR_WMARK];  // HIGH, LOW, MIN

// 在 __alloc_pages_nodemask() 中
if (free_pages < watermark[WMARK_LOW]) {
    // 直接回收或喚醒 kswapd
    if (free_pages < watermark[WMARK_MIN]) {
        direct_reclaim();
    } else {
        wakeup_kswapd();
    }
}
```

### 2.3 閾值管理對比

| 層面 | LeetCode 334 | Linux Kernel |
|------|-------------|-------------|
| **閾值數量** | 2 個 (`first`, `second`) | 3–N 個（`HIGH`, `LOW`, `MIN`, 或 `lo`, `mid`, `hi`）|
| **閾值更新策略** | 貪心更新（保持最小值）| 動態調整（基於系統負載或 BBR 測量）|
| **觸發條件** | 單一條件 (`num > second`) | 多階段條件（級聯觸發） |
| **返回值/副作用** | 布林值 (true/false) | 系統動作（分配失敗、發送速率調整、任務遷移）|

---

## 3. 主管 Code Review 角度考題

### Q1: 為什麼 LeetCode 334 只需要兩個運行最小值，而 Linux 記憶體管理需要三個浮水位？

**預期答案**:
- LeetCode 334：問題簡化為「**存在性檢測**」，只需區分「未找到」vs「找到」，故兩個閾值（第一、第二最小值）足夠
- Linux 記憶體管理：需要**多級響應**機制
  - `WMARK_HIGH`: 無壓力，正常分配
  - `WMARK_LOW`: 輕度壓力，觸發非同步回收 (kswapd)
  - `WMARK_MIN`: 危急壓力，觸發同步回收，可能阻塞分配者
- 閾值數量與系統複雜度和響應層級成正比

**Code Review 教學點**:
- 設計時：先明確需要多少「狀態」，再決定閾值數量
- 單責原則：每個閾值應對應一種明確的動作或決策

### Q2: LeetCode 334 的貪心策略（始終保持最小值）為何正確？核心不變式是什麼？

**預期答案**:
- **不變式**：`first < second`（始終保持）
- **證明（直觀）**：
  1. 當 `first` 更新時，新值 `< second`（因為 `num <= first < old_second`）
  2. 當 `second` 更新時，新值必然 `> first`（因為 `first < num <= second`）
  3. 若有 `num > second`，即使 `first` 和 `second` 稍後被更新，已存在三元組
- **貪心正確性**：更新為更小的 `first` 或 `second` 只會擴大後續找到三元組的可能性

**Code Review 教學點**:
- 驗證不變式是單遍貪心算法的核心
- Linux 核心代碼中常見類似模式：在臨界區確保某些值的單調性

### Q3: 在 Linux 內核中，為何 TCP BBR 和記憶體浮水位都使用「單遍事件驅動」而不是「週期掃描」？

**預期答案**:
- **延遲敏感性**：
  - TCP：ACK 到達的毫秒級延遲不容許等待定期掃描
  - 記憶體：分配失敗或觸發 OOM 的實時性要求極高
- **效率**：避免冗余的週期掃描，只在「有必要」時更新狀態
- **可擴展性**：系統規模（流量、內存壓力）變化時，事件驅動能自適應

**類比 LeetCode**:
- LeetCode 採用「單遍掃描」，Linux 採用「單遍事件驅動」，本質都是 O(n) 或 O(1) 時間，避免多遍掃描

**Code Review 教學點**:
- 理解時間敏感的系統設計：單遍 + 事件驅動是高性能的核心
- 這直接影響 API 設計（回調 vs 輪詢）

### Q4: 如果需要在 LeetCode 334 中支持「返回增序三元組本身」而不僅是布林值，演算法如何修改？與 Linux 記憶體回收的決策鏈有何相似之處？

**預期答案**:
- **LeetCode 修改**：維護額外的索引或值追蹤
  ```cpp
  struct Triplet { int first_idx, second_idx; };
  Triplet tracked = {-1, -1};
  for (int i = 0; i < nums.size(); i++) {
      if (nums[i] <= first) {
          first = nums[i];
          tracked.first_idx = i;
      } else if (nums[i] <= second) {
          second = nums[i];
          tracked.second_idx = i;
      } else {
          return {tracked.first_idx, tracked.second_idx, i};
      }
  }
  ```
- **Linux 記憶體回收鏈**：類似的**決策路徑記錄**
  ```c
  /* 記錄導致回收的頁面分配請求 */
  struct alloc_context {
      int order;
      gfp_t gfp_mask;
      int preferred_zone;
  };
  // 若觸發直接回收，記錄觸發點
  if (direct_reclaim_needed) {
      log_reclaim_trigger(ctx);
  }
  ```

**Code Review 教學點**:
- 演算法增強時須同步增加狀態追蹤，成本線性增長
- Linux 核心在除錯和效能分析中大量使用「軌跡記錄」（tracepoints），是類似思想的應用

### Q5: LeetCode 334 的時間複雜度是 O(n)，空間複雜度是 O(1)。在 Linux 內核的調度器負載追蹤中，如何在有限的 CPU 週期內完成類似的追蹤？

**預期答案**:
- **LeetCode**：純計算，無 I/O，O(1) 空間簡單明快
- **Linux 調度器**：
  - 必須在 **context switch** 或 **tick** 中更新（CPU 週期有限）
  - 採用 **lazy evaluation**（延遲計算）與 **decay**（衰減）機制避免複雜的全掃描
  - 例如 `sched_entity::avg` 使用指數加權移動平均 (EWMA)，而非精確追蹤每個瞬間

**Code Review 教學點**:
- 實時系統中的算法必須在 CPU 預算內完成，常需要精度與速度的權衡
- 面試時可提及「時空權衡」的實際應用

---

## 4. 面試加分總結

### 核心論點結構

1. **演算法基礎** (基礎級別)
   - LeetCode 334 是單遍貪心 + 運行最小值追蹤
   - 時間 O(n)，空間 O(1)，理想效率

2. **系統設計視角** (中級)
   - Linux 內核中的 TCP BBR (net/ipv4/tcp_bbr.c) 使用類似的多閾值邊界追蹤
   - 記憶體管理 (mm/page_alloc.c) 的浮水位機制是「多級反應」的貪心策略
   - 調度器 (kernel/sched/fair.c) 的負載追蹤在事件驅動下維護運行統計

3. **深度技術連接** (高級)
   - 共同模式：**單遍掃描 + 貪心狀態更新 + 閾值觸發**
   - 在 Linux 內核中，這種模式支撐網路控制迴路、記憶體壓力反應、和進程調度
   - 實時性與可靠性需求決定了實現細節（事件驅動 vs 時鐘驅動）

4. **代碼設計啟示**
   - 驗證不變式（invariant）的重要性：LeetCode 334 的 `first < second` 保證正確性
   - 閾值數量與系統狀態空間成正比
   - 單責原則：每個閾值對應一種動作或決策

### 面試高分回答模板

**問題**: "解釋 LeetCode 334 的演算法，為什麼是最優的？"

**高分回答**:
> "LeetCode 334 採用單遍貪心掃描，維護兩個運行最小值 `first` 和 `second`。此算法是最優的，因為時間複雜度達到下界 O(n)（必須檢視每個元素）。
>
> 我認為這個模式在大規模系統中很常見。例如，Linux 內核的 TCP BBR 擁塞控制（net/ipv4/tcp_bbr.c）也採用單遍事件驅動的方式，在每個 ACK 到達時更新帶寬估計的上下界。記憶體管理系統（mm/page_alloc.c）則維護 HIGH/LOW/MIN 三個浮水位，當自由頁面跌破某個閾值時立即觸發相應的回收動作。
>
> 關鍵的設計洞見是：驗證不變式。在 LeetCode 334 中，不變式是 `first < second`。在 Linux 內核中，類似的不變式保證了浮水位的有效性（HIGH > LOW > MIN）。這是單遍貪心算法的核心。"

### 面試中可觸及的延伸話題

- **Can you implement a variant where you return the actual triplet indices instead of just boolean?** → 引入 Linux 內核中「決策路徑記錄」的概念
- **How would you adapt this to detect k-element increasing subsequences?** → 多閾值設計，與記憶體浮水位多層級的類比
- **What if the array is too large to fit in memory?** → 流式處理（streaming），與 TCP 接收緩衝區的單遍掃描類比
- **How is this different from using a hash map or balanced BST approach?** → 時空權衡，為何 O(1) 空間的貪心更優

---

## 5. Sources

### LeetCode 官方資源
- [LeetCode 334: Increasing Triplet Subsequence](https://leetcode.com/problems/increasing-triplet-subsequence/)

### Linux Kernel Source (GitHub - torvalds/linux)

#### TCP 擁塞控制
- [net/ipv4/tcp_bbr.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_bbr.c) — BBR 算法實現，維護 `bw_lo`, `bw_hi`, `min_rtt`
- [net/ipv4/tcp_cubic.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_cubic.c) — CUBIC 算法，追蹤最大擁塞窗口

#### 記憶體管理
- [mm/page_alloc.c](https://github.com/torvalds/linux/blob/master/mm/page_alloc.c) — 頁面分配器，浮水位檢查 (搜尋 `watermark[WMARK_LOW]`, `watermark[WMARK_MIN]`)
- [mm/vmscan.c](https://github.com/torvalds/linux/blob/master/mm/vmscan.c) — 頁面回收邏輯
- [mm/kswapd.c](https://github.com/torvalds/linux/blob/master/mm/kswapd.c) — 非同步回收執行緒

#### 調度器
- [kernel/sched/fair.c](https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c) — 公平調度，負載追蹤 (搜尋 `min_vruntime`, `enqueue_task_fair`)
- [kernel/sched/core.c](https://github.com/torvalds/linux/blob/master/kernel/sched/core.c) — 核心排程邏輯

#### 排序與搜尋
- [lib/sort.c](https://github.com/torvalds/linux/blob/master/lib/sort.c) — 內核排序實現
- [lib/bsearch.c](https://github.com/torvalds/linux/blob/master/lib/bsearch.c) — 二元搜尋

### 相關技術文檔

- [Linux TCP BBR: BBR Congestion-Based Congestion Control](https://queue.acm.org/detail.cfm?id=3022184) — Neal Cardwell et al., 解釋 BBR 的設計原理
- [Understanding the Linux Memory Allocator](https://www.kernel.org/doc/html/latest/core-api/memory-allocation.html) — 官方內核文檔
- [Linux Kernel Scheduler: A Visual Introduction](https://blog.acolyer.org/2016/04/26/the-linux-scheduler-a-decade-of-wasted-cores/) — 深入淺出的調度器介紹

---

## 附錄：快速參考表

### 演算法維度對比

| 特性 | LeetCode 334 | TCP BBR | 記憶體浮水位 | 調度器負載追蹤 |
|------|------------|---------|-----------|------------|
| **掃描模式** | 線性迭代 | 事件驅動 (ACK) | 事件驅動 (分配) | 事件驅動 (context switch) |
| **狀態變數** | 2 個 | 4–6 個 | 3 個 | 8–10 個 |
| **時間複雜度** | O(n) | O(1) per ACK | O(1) per alloc | O(1) per event |
| **空間複雜度** | O(1) | O(1) | O(1) | O(n) (task 數量) |
| **不變式** | `first < second` | `min_rtt ≤ measured_rtt` | `HIGH > LOW > MIN` | 負載權重單調性 |
| **觸發動作** | 返回 true | 調整窗口或發送速率 | 喚醒 kswapd 或直接回收 | 遷移任務或調整優先級 |
| **主要挑戰** | 無 | RTT 測量精度 | 浮水位動態調整 | 實時性與精度平衡 |

---

**文檔版本**: 1.0
**撰寫日期**: 2026-03-30
**目標讀者**: LeetCode 刷題者、系統軟體工程師、準備技術面試者
