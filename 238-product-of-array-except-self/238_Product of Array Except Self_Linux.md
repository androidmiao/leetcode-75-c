# LeetCode 238: Product of Array Except Self — Linux Kernel 連結分析

## Linux 相關原始碼位置

本題的核心手法是**前綴累積（prefix accumulation）+ 後綴累積（suffix accumulation）**，先正向掃一次收集左側資訊，再反向掃一次收集右側資訊，最後合併。這個 two-pass scan 模式在 Linux kernel 中多處出現：

| 原始碼位置 | 角色 |
|-----------|------|
| `kernel/sched/fair.c` — `update_curr()`, `calc_delta_fair()` | CFS 排程器的 `vruntime` 就是一個 running prefix sum：每次 timer tick 都把加權時間 delta 累加到 `se.vruntime`，使得排程器隨時知道「到目前為止的累積公平時間」 |
| `kernel/sched/fair.c` — `cfs_rq->load.weight` | 每當 task enqueue/dequeue，其 weight 會被加入或移除 `cfs_rq->load.weight`；這是一個動態維護的 prefix sum（所有 task 權重的累積），用於 load balancing 計算 |
| `lib/bitmap.c` — `bitmap_ord_to_pos()`, `bitmap_weight()` | `bitmap_weight()` 對所有 word 做 `hweight_long()` 並累加，本質上就是 prefix count；`bitmap_ord_to_pos()` 則用累積計數把 ordinal offset 映射到 bit position |
| `include/linux/log2.h` — `ilog2()` | 雖非 prefix product，但 kernel 用 bit-level 累積操作來快速計算 log₂，底層依賴類似的 scan 與 accumulate 思維 |
| `net/core/dev.c` — `napi_gro_receive()` | GRO（Generic Receive Offload）把小封包累積合併成大封包再往上送，概念類似 prefix aggregation |

### 最直接的對應：CFS vruntime

CFS 的 `vruntime`（virtual runtime）是最經典的 prefix sum 應用：

```
vruntime[t] = vruntime[t-1] + delta_exec * (NICE_0_LOAD / weight)
```

每個 task 的 `se.sum_exec_runtime` 記錄了從建立以來的**總 CPU 時間**（不加權的前綴和），而 `se.vruntime` 記錄的是**加權後的前綴和**。排程器選 vruntime 最小的 task 來執行，這讓 CFS 不需要除法就能達到公平排程 — 與本題「不使用除法」的精神不謀而合。

---

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 238 實作 | CFS vruntime / load tracking |
|------|-------------------|------------------------------|
| 累積方向 | 正向（left prefix）+ 反向（right suffix） | 單向（時間遞增的 prefix sum） |
| 資料結構 | 陣列 + 兩個純量累積變數 | 紅黑樹 + `se.vruntime` 欄位 |
| 合併方式 | 兩個 pass 的結果相乘 | 不需合併；vruntime 直接比較 |
| 空間策略 | O(1) 額外空間（重用 answer 陣列） | O(1) per task（欄位內嵌在 `sched_entity` 中） |
| 運算型態 | 乘法累積 | 加法累積（加權除法由 `calc_delta_fair()` 用位移近似） |
| 「不用除法」的處理 | 改用前綴乘積 × 後綴乘積 | 用 `__calc_delta()` 以乘法 + 右移取代除法，避免昂貴的硬體除法 |
| 動態更新 | 靜態一次性計算 | 動態：每次 enqueue / dequeue / timer tick 都增量更新 |

### 共同設計哲學

兩者的共同點在於：**避免對每個元素做全域運算（如除法或 O(N) 重掃），改用局部累積值來推導全域結果**。LeetCode 238 用 `left_product × right_product` 避免除法；CFS 用 `vruntime` 的前綴和避免每次排程都重新計算所有 task 的 CPU 佔比。

---

## 主管 Code Review 角度考題

### Q1: 為什麼不直接算全部乘積再除？
**期待答案方向**：除法有 edge case（元素為 0 時除以零未定義），且題目明確禁止。即使允許除法，也要處理 0 的個數（0 個、1 個、多個），程式碼反而更複雜。
**真正在測試**：邊界條件敏感度、是否能主動想到 zero-handling 而非只看約束。

### Q2: 你的 two-pass 做法在 cache 行為上有什麼特性？
**期待答案方向**：第一趟正向掃描和第二趟反向掃描都是循序存取（sequential access），對 CPU cache 非常友好。正向掃描是 stride-1 forward，反向掃描是 stride-1 backward；現代 CPU 的 hardware prefetcher 兩種方向都能預取。
**追問**：如果陣列極大（遠超 L3 cache），兩趟掃描的 cache miss 成本會如何？（答：每趟都是 cold miss，但因為是 streaming access pattern，miss 率是最低的 O(N/cache_line_size)。）
**真正在測試**：cache locality 意識、對 hardware prefetching 的理解。

### Q3: `solution.c` 使用 VLA `int arr[n]`，在 kernel 裡可以這樣寫嗎？
**期待答案方向**：Linux kernel 從 4.20 起禁止 VLA（`-Wvla` 編譯警告），因為 VLA 放在 stack 上，kernel stack 很小（通常 8KB 或 16KB），大的 VLA 會導致 stack overflow。改用 `kmalloc()` / `kzalloc()` 在 heap 配置。
**追問**：如果 n 保證很小（如 ≤ 16），VLA 可以接受嗎？（答：kernel policy 是一律禁止，即使大小已知也用固定大小陣列而非 VLA。）
**真正在測試**：kernel stack budget 意識、defensive programming。

### Q4: `malloc` 之後你有檢查 NULL，但下游呼叫端如果忘記 free 怎麼辦？
**期待答案方向**：LeetCode 的 contract 註明 "assume caller calls free()"。在 kernel 中，記憶體配置通常遵循 "who allocates, who frees" 原則，或透過 reference counting (`kref`) 和 callback 機制確保釋放。也可以提到 `kmemleak` 工具用於偵測洩漏。
**真正在測試**：記憶體管理紀律、對 ownership model 的理解。

### Q5: 如果要把這個演算法改成支援動態更新（某個元素改變後快速重算答案），你會怎麼做？
**期待答案方向**：使用 segment tree 或 binary indexed tree（Fenwick tree）維護前綴乘積，可以做到 O(log N) 的單點更新和區間查詢。也可以提到 kernel 的 `perf_event` 子系統就用類似結構來維護事件計數器的區間統計。
**追問**：乘法有逆運算（除法），可以用 Fenwick tree 直接做嗎？如果元素可能為 0 呢？（答：有 0 的話除法失效，需要 segment tree 記錄完整區間乘積。）
**真正在測試**：資料結構擴展能力、是否了解 prefix sum 的進階用途。

---

## 面試加分總結

- **提到 CFS 的 vruntime 是 prefix sum** — 展示你對排程器核心機制的理解，而非只會解 LeetCode 題目
- **說明 kernel 為何禁止 VLA** — 顯示你知道 kernel stack 限制和防禦性程式設計
- **分析 two-pass 的 cache 行為** — 說明你思考效能時不只看 Big-O，也考慮硬體層面
- **提到 `__calc_delta()` 用乘法 + 右移避免除法** — 直接呼應本題「不使用除法」的約束，展示 kernel 也面對相同設計取捨
- **討論動態更新場景的 segment tree 延伸** — 表明你能從面試題延伸到系統設計層面

---

## Sources

- [CFS Scheduler Documentation](https://docs.kernel.org/scheduler/sched-design-CFS.html)
- [kernel/sched/fair.c (GitHub)](https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c)
- [lib/bitmap.c (GitHub)](https://github.com/torvalds/linux/blob/master/lib/bitmap.c)
- [LWN: Load tracking in the scheduler](https://lwn.net/Articles/639543/)
- [VLA removal in Linux kernel](https://www.phoronix.com/news/Linux-Kills-The-VLA)
- [net/core/dev.c — NAPI GRO](https://github.com/torvalds/linux/blob/master/net/core/dev.c)
