# 122. Best Time to Buy and Sell Stock II — Linux Kernel 連結分析

## Linux 相關原始碼位置

| 檔案路徑 | 說明 |
|---|---|
| `kernel/sched/fair.c` | CFS（完全公平排程器）中的 `update_curr()` 使用增量累加計算 vruntime，概念上類似逐日差價累加 |
| `kernel/sched/core.c` | 排程器核心，`calc_delta_fair()` 計算時間片差值 |
| `net/sched/sch_tbf.c` | Token Bucket Filter（令牌桶過濾器）— 網路 QoS 模組，逐封包累計 token 消耗，類似逐日利潤累加 |
| `drivers/cpufreq/cpufreq_ondemand.c` | ondemand CPU 頻率調節器 — 根據逐時間片的負載差值決定升頻或降頻，是貪心法在系統層面的應用 |
| `fs/proc/stat.c` | 匯出 `/proc/stat` 中的 CPU 時間統計，使用增量差值計算各狀態的時間消耗 |

## Kernel vs. LeetCode 實作對照

| 比較維度 | LeetCode 122（貪心法） | Linux Kernel（CFS vruntime） |
|---|---|---|
| 核心操作 | 累加所有正差價 `Σ max(0, Δprice)` | 累加所有時間增量 `Σ Δexec_time / weight` |
| 判斷條件 | `prices[i] > prices[i-1]` → 加入利潤 | `delta_exec > 0` → 更新 vruntime |
| 資料結構 | 純陣列遍歷 | 紅黑樹 + 增量更新 |
| 貪心性質 | 每個正差價獨立 → 局部最優 = 全域最優 | 每次排程選最小 vruntime → 近似公平 |
| 時間複雜度 | O(n) 單次遍歷 | O(1) 每次 update + O(log n) 排程選擇 |
| 空間複雜度 | O(1) | O(n) 紅黑樹節點 |

### cpufreq ondemand 調節器的對應

`cpufreq_ondemand` 是更直接的類比：

| 比較維度 | LeetCode 122 | cpufreq ondemand |
|---|---|---|
| 輸入序列 | 每日股價 | 每個取樣週期的 CPU 負載 |
| 正向觸發 | `price[i] > price[i-1]` → 賺差價 | `load > up_threshold` → 升頻 |
| 負向處理 | 不操作（跳過跌幅） | `load < down_threshold` → 降頻 |
| 策略 | 貪心：捕捉每段上漲 | 貪心：逐週期反應負載 |
| 全域最優性 | 數學可證（差價獨立） | 啟發式（不保證能耗最優） |

## 主管 Code Review 角度考題

### 問題 1：為什麼這個貪心策略是全域最優的？

**預期回答方向：** 任何一筆跨越多天的交易 buy@day_a → sell@day_b 的利潤 `prices[b] - prices[a]` 可以拆解為 `Σ(prices[i] - prices[i-1])` for i in [a+1, b]，其中我們只取正的部分。取正部分的總和一定 ≥ 直接差值（因為我們跳過了負的段落），所以逐日貪心永遠 ≥ 任何合併交易。

**追問：** 如果有交易手續費（LeetCode 714），這個貪心還成立嗎？

**測試目的：** 理解貪心策略的適用邊界，以及何時需要改用 DP。

### 問題 2：kernel 的 CFS 排程器也用類似的「增量累加」，為什麼不直接記錄絕對時間？

**預期回答方向：** 增量計算避免了整數溢位問題（`unsigned long` 在長時間運行後可能溢位），也更適合多處理器環境中的時間同步。此外，增量更新的快取命中率更好，因為只需要前一次的值。

**追問：** vruntime 溢位時 CFS 怎麼處理比較？

**測試目的：** 對增量計算 vs. 絕對計算的權衡有深入理解，以及溢位處理的防禦性程式設計意識。

### 問題 3：這個 O(n) 的遍歷在現代 CPU 上的快取行為如何？

**預期回答方向：** `int` 陣列的順序存取是最佳的快取存取模式 — 完美的空間局部性。每個快取行（通常 64 bytes）可容納 16 個 `int`，prefetcher 能準確預測存取模式。相比之下，峰谷法的兩個巢狀 while 雖然也是順序存取，但分支預測可能稍差。

**追問：** 如果 prices 改為 linked list，效能會受到什麼影響？

**測試目的：** 快取局部性意識、對 CPU pipeline 和 prefetch 的理解。

### 問題 4：如果要處理即時串流資料（prices 一個個到達），這個演算法還適用嗎？

**預期回答方向：** 完全適用。貪心策略天然支援串流處理：只需維護前一個價格 `prev_price` 和累計利潤 `profit`。每收到新價格，若 `new_price > prev_price` 就累加差值。不需要回看歷史資料。這正是 ondemand CPU 調節器的工作模式 — 逐取樣週期做決策。

**追問：** 串流版本的空間複雜度是什麼？能否做到 O(1)？

**測試目的：** 從離線演算法到線上演算法的轉化能力，以及系統設計思維。

## 面試加分總結

- **提及貪心最優性證明**：用「長交易可拆解為多個單日交易」的論證，展示數學推理能力
- **連結 CFS vruntime 增量累加**：展示對 kernel 排程器的理解，以及「增量 vs. 絕對」的設計取捨
- **連結 cpufreq ondemand**：展示對系統層面貪心策略的理解，以及啟發式 vs. 可證最優的差異
- **討論串流處理場景**：展示演算法到系統設計的遷移能力
- **快取友好性分析**：展示對硬體特性的意識

## Sources

- [kernel/sched/fair.c — CFS scheduler](https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c)
- [drivers/cpufreq/cpufreq_ondemand.c](https://github.com/torvalds/linux/blob/master/drivers/cpufreq/cpufreq_ondemand.c)
- [net/sched/sch_tbf.c — Token Bucket Filter](https://github.com/torvalds/linux/blob/master/net/sched/sch_tbf.c)
- [LWN: CFS scheduler overview](https://lwn.net/Articles/230501/)
- [LWN: CPU frequency governors](https://lwn.net/Articles/552904/)
