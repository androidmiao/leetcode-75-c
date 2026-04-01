# 1732. Find the Highest Altitude — Linux Kernel 連結分析

## Linux 相關原始碼位置

前綴和（prefix sum）在 Linux kernel 中最直接的應用場景是 **運行時統計累加** 和 **per-CPU 計數器**。雖然 kernel 不會稱之為「prefix sum」，但本質相同：持續累加增量，並在需要時取最大值或當前值。

| 檔案 / 模組 | 角色說明 |
|---|---|
| `kernel/sched/stats.h` | 排程器統計：累加每次排程事件的增量（wait time、run time），維護最大值供 `/proc/schedstat` 回報 |
| `include/linux/percpu_counter.h` | Per-CPU 計數器：每個 CPU 各自累加局部增量，匯總時計算全域總和（概念上等於分散式前綴和） |
| `lib/math/` | 通用數學輔助，部分涉及累加與統計 |
| `net/core/gen_stats.c` | 網路流量統計：累加封包數、位元組數，計算速率（增量累加的典型應用） |
| `fs/proc/stat.c` | `/proc/stat` 的 CPU 時間統計：kernel 對 user/system/idle 等時間做持續累加 |

### 最貼近的類比：排程器的 `schedstat`

`kernel/sched/stats.h` 中的排程統計會在每次 context switch 時累加 `wait_sum`（等待時間）和 `run_delay`。這跟本題的操作幾乎一模一樣：

```
每次事件 → 累加增量到 running total → 需要時取最大或平均
```

對應到 LeetCode 1732：

```
每個 gain[i] → 累加到 current → 每步更新 highest
```

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 1732 | Linux Kernel（排程統計 / percpu counter） |
|---|---|---|
| 資料來源 | 靜態陣列 `gain[]` | 動態事件流（context switch、封包到達） |
| 累加方式 | 單一變數 `current += gain[i]` | per-CPU 局部計數器 `__this_cpu_add()` |
| 取最大值 | 即時 `if (current > highest)` | 部分場景用 `max()` 巨集；部分只記錄累計值 |
| 並行性 | 單執行緒，無需同步 | 多 CPU 並行累加，匯總時需 `percpu_counter_sum()` |
| 溢出處理 | int 範圍內保證不溢出 | 使用 `u64` 或 `atomic_long_t` 防溢出 |
| 複雜度 | O(n) 一次掃描 | O(1) 每次事件；O(NR_CPUS) 匯總 |

## 主管 Code Review 角度考題

### Q1：為什麼不先建完整個海拔陣列再找最大值？

**期望回答方向**：多一次 O(n) 掃描且多用 O(n) 空間，完全沒必要。一次掃描邊加邊比即可。

**真正在考的**：是否理解「streaming computation」— 不需要完整資料就能得到答案的場景。kernel 中大量統計都是這種模式。

**追問**：kernel 排程器為什麼不把每次 context switch 的時間都存下來？ → 記憶體成本、cache 壓力、以及 lock contention。

### Q2：如果 `gain[]` 非常大（百萬級），cache 行為如何？

**期望回答方向**：`gain[]` 是連續記憶體，循序存取，cache prefetcher 會自動預取。這是最友善的存取模式（sequential scan），L1 cache hit rate 極高。

**真正在考的**：cache locality 意識。kernel 中 per-CPU counter 的設計正是為了避免跨 CPU 的 cache line bouncing。

### Q3：如果這個累加要在多執行緒環境下執行呢？

**期望回答方向**：可以參考 kernel 的 `percpu_counter` 設計 — 每個執行緒維護局部累加值，最後匯總。或用 `atomic` 操作，但會有 cache line contention。

**真正在考的**：理解 concurrent prefix sum 的挑戰，以及 kernel 如何用 per-CPU 資料結構解決。

### Q4：`highest` 初始化為 0 而非 `INT_MIN`，正確嗎？

**期望回答方向**：正確，因為起點海拔就是 0，且 0 本身是一個合法候選答案。如果所有 `gain[i]` 都是負數，最高海拔就是起點的 0。

**真正在考的**：邊界條件的嚴謹思考、對題目語義的精確理解。kernel 中初始化值選錯會導致難以追蹤的 bug。

### Q5：這個演算法跟 Kadane's Algorithm（最大子陣列和）有什麼關係？

**期望回答方向**：Kadane's 是找「最大子陣列和」，允許重設起點；本題是固定從 0 開始的「最大前綴和」，不允許重設。Kadane's 多了一個 `if (current < 0) current = 0` 的重設機制。

**真正在考的**：能否看出問題間的結構關聯，以及變體推導能力。

## 面試加分總結

- 提到這題本質是 **max prefix sum**，只需一次 streaming scan，展示對 online algorithm 的理解
- 對比 kernel 的 `percpu_counter`：解釋為何單一全域計數器在多核場景下不好（cache line bouncing），per-CPU 設計如何解決
- 說明初始化 `highest = 0` 的語義正確性，而非機械式地用 `INT_MIN`
- 延伸到 Kadane's Algorithm，展示舉一反三的能力
- 提到 sequential memory access 的 cache 友善性，對比 kernel 中 random access 場景的不同策略

## Sources

- [kernel/sched/stats.h — Linux kernel source](https://github.com/torvalds/linux/blob/master/kernel/sched/stats.h)
- [include/linux/percpu_counter.h](https://github.com/torvalds/linux/blob/master/include/linux/percpu_counter.h)
- [lib/percpu_counter.c](https://github.com/torvalds/linux/blob/master/lib/percpu_counter.c)
- [net/core/gen_stats.c](https://github.com/torvalds/linux/blob/master/net/core/gen_stats.c)
