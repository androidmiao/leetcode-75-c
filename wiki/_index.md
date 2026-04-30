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
├── patterns/             ← 19 個算法模式（15 核心 + 4 補充）
├── c-idioms/             ← 4 個 C 語言工程主題
├── kernel/               ← 12 個 Linux Kernel 子系統（基礎結構 + 同步原語 + deferred work）
├── interview/            ← 15 份面試實戰文件（cheat sheet + weekly mock + code review 系列 + driver review）
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
| [[health/2026-04-30_lint]] | 最新一次健康檢查報告；缺口清單、矛盾檢查、下一步建議 |

---

## 📚 Patterns（19 個算法模式）

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

## 🐧 Kernel（12 個 Linux 子系統）

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

### 同步與併發原語（2026-04-21 新增；2026-04-29 補上原語選擇邏輯）

**[[kernel/spinlock]]** — 自旋鎖
- Busy-wait、禁止睡眠、唯一能在 atomic context 使用的鎖
- 三種變體：`spin_lock()` / `spin_lock_bh()` / `spin_lock_irqsave()`（擋 process / softirq / hard IRQ）
- 地雷：持鎖呼叫 `GFP_KERNEL`、鎖順序不一致、critical section 過長
- 源碼：`include/linux/spinlock.h`, `kernel/locking/spinlock.c`
- 對應 code review：[[interview/Mon-3-code-review-solution]] Bug #4（`GFP_KERNEL` 在 spinlock 內）

**5 大原語選擇邏輯**（mutex / spinlock / semaphore / completion / rwsem）
- 完整決策樹與「什麼時候用它」逐條判準：[[interview/google_code_review_semaphore_prep_linux_primitives]]
- 一句話：**資料用 mutex（IRQ 用 spinlock、多 reader 用 rwsem）；事件用 completion（要計數用 semaphore）**
- 反模式速查：`sema_init(&s,1)` 當 mutex、`sema_init(&s,0)` 當事件、ISR 內 `mutex_lock`、`spinlock` 持有期間 `copy_to_user`、semaphore + bitmap 雙重簿記
- 真實場景對照（AP-DSP IPC driver）：見 [[interview/google_code_review_semaphore_prep_linux_solution]] 題目 1–6

### Lockless 與 deferred work（2026-04-30 新增）

**[[kernel/rcu]]** — 讀-複製-更新（Read-Copy-Update）
- 讀者完全零鎖、寫者 copy-modify-publish + 延後釋放
- read-side critical section 不可睡（要睡用 SRCU）
- 永遠用 `rcu_assign_pointer` / `rcu_dereference` 配對；寫者間互斥另接 mutex/spinlock
- 源碼：`include/linux/rcupdate.h`, `include/linux/rculist.h`, `kernel/rcu/`
- 應用：dcache、路由表、SELinux AVC、module list、tracepoint
- 對應：[[kernel/spinlock]] 對比表中 RCU 欄、[[interview/mcupm-code-review]] Q5 ackdata race 進階修法

**[[kernel/bitmap]]** — 位元圖（資源池 / cpumask / IRQ vector）
- `DECLARE_BITMAP(name, nbits)` 緊湊狀態陣列，O(1) 占用 + O(N/64) 掃描
- `find_first_zero_bit` 找空位，回 `nbits` 表示沒找到（不是 -1）
- `set_bit` / `clear_bit` atomic；`__set_bit` / `__clear_bit` 非 atomic（已握鎖時用）
- `test_and_set_bit` 直接當 try-lock；`for_each_set_bit` 走訪
- 源碼：`include/linux/bitmap.h`, `include/linux/bitops.h`, `lib/bitmap.c`
- 應用：cpumask、IRQ matrix、SCMI token allocator、page block migrate type
- 對應：[[patterns/bit-manipulation]] 是其「N=64」特例、[[interview/google_code_review_semaphore_prep_linux_solution]] DSP IPI channel pool

**[[kernel/workqueue]]** — 工作佇列（atomic context → process context）
- `alloc_workqueue("name", flags, max_active)` 是現代寫法（`create_workqueue` 已 deprecated）
- 常用 flag：`WQ_UNBOUND` / `WQ_FREEZABLE` / `WQ_MEM_RECLAIM` / `WQ_HIGHPRI`
- delayed_work = work + timer；`mod_delayed_work` 重新計時、`cancel_*_sync` 安全 teardown
- 三步 teardown：設 stop flag → `cancel_*_sync` → `destroy_workqueue`
- 源碼：`include/linux/workqueue.h`, `kernel/workqueue.c`
- 應用：NIC link state、fs writeback、SCMI notification、MCUPM time sync
- 對應：[[interview/mcupm-code-review]] Q10 deprecated API + suspend/resume 對稱、Q11 module exit

### 韌體與平台（跨 wiki 參考，2026-04-20 新增關聯）

**Firmware / AP↔SCP 子系統**（交叉引用 `common-android-mainline` wiki）
- ARM SCMI（10 protocols、4 transports、19,481 行）
- Google ACPM（GS101 Pixel 系列、1,111 行、2024 upstream）
- Qualcomm RPMh + CPUCP（硬體 TCS 旁路、X Elite 引入 SCMI perf）
- 源碼：`drivers/firmware/arm_scmi/`, `drivers/firmware/samsung/exynos-acpm.c`, `drivers/firmware/qcom/`
- 對應 code review：`[[interview/code-review-questions]]` §14-§17
- 對應模擬日：`[[interview/weekly-mock-bank]]` 附錄 E
- 深度分析：`/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/analyses/scmi-vs-google-acpm.md`

---

## 🎯 Interview（15 份實戰文件）

### 通用面試素材

- [[interview/complexity-cheatsheet]] — 每個資料結構 / 演算法的時間與空間複雜度表
- [[interview/edge-cases]] — 空輸入、單元素、極值、整數溢位、重複元素
- [[interview/follow-up-patterns]] — 面試官常見追問方向與應對
- [[interview/code-review-questions]] — 主管角度的 code review 考題清單；**2026-04-20 擴充**第 6-13 章以 Linux Kernel 視角（coding style、ERR_PTR、goto cleanup、GFP、RCU、lockdep、cache line、checkpatch/KASAN 等）；**2026-04-20 追加**第 14-17 章 Firmware 子系統視角（ARM SCMI、Google ACPM、Qualcomm RPMh 三方 review 差異）
- [[interview/weekly-mock-bank]] — **新增（2026-04-20）**：一週 7 天輪班制模擬面試題庫，每天對應一個 kernel 子系統，含熱身 + 演算法 Round + kernel code review + deep-dive + 自評；**同日追加附錄 E** Firmware / AP↔SCP 選修模擬日（bounded token allocator + SCMI vendor protocol review + GS101 DVFS deep-dive）

### 一週模擬面試逐日解答

- [[interview/Mon-3-code-review-solution]] — Mon-3 code review 題的逐行繁中註解解答，對應 spinlock / GFP / refcount / 錯誤路徑訓練
- [[interview/Tue-3-code-review-solution]] — Tue-3 code review 題的逐行繁中註解解答
- [[interview/Thu-3-code-review-solution]] — Thu-3 code review 題的逐行繁中註解解答
- [[interview/Fri-4-code-review-solution]] — Fri-4 code review 題的逐行繁中註解解答

### Google Code Review 面試準備系列（AP-DSP IPC × 同步原語，2026-04-29 完整化）

> 主軸：**AP ↔ DSP 通訊 driver**（Linux kernel driver / Android embedded SoC 場景）。
> 圍繞 6 題 code review 模擬，串起 mutex / spinlock / semaphore / completion / rwsem 五大同步原語。

- [[interview/google_code_review_semaphore_prep]] — 通用版（user space + POSIX 視角）面試準備
- [[interview/google_code_review_semaphore_prep_linux]] — **Linux kernel 版本**：6 題故意埋 bug 的 code review，含 reviewer 心法與「Bug 1 致命：全域 ack semaphore 張冠李戴」等經典反模式
- [[interview/google_code_review_semaphore_prep_linux_solution]] — 上述 6 題的**修正解答版**，每行 C code 加繁中註解（`dsp_send_ipi`、`dsp_ipi_channel_alloc`、`dsp_log_read`、`dsp_mbox_probe`、`dsp_time_sync`、`dsp_driver_remove` drain pattern）
- [[interview/google_code_review_semaphore_prep_linux_primitives]] — **5 大同步原語選擇邏輯**速查（mutex / spinlock / semaphore / completion / rwsem），含總決策樹 + 用題目 1–4 做圖解複習 + 30 秒判斷流程 + 反模式速查表

### Driver-level / 教材級 code review（2026-04-30 新增）

- [[interview/mcupm-code-review]] — **MediaTek MCUPM driver 真實 production code review** 索引；對應 `MCUPM_Code_Review/` 兩份 27KB / 25KB 大文件（11 題 + 加分題 + 11 條 review checklist），題目來源 `drivers/remoteproc/mcupm/v2/{mcupm_driver, mcupm_plt, mcupm_timesync}.c`；對接 [[kernel/spinlock]] / [[kernel/workqueue]] / [[kernel/bitmap]] / [[kernel/memory-management]] / [[kernel/rcu]]
- [[interview/google-stack-with-min]] — `interview/google_stack_with_min/` 八件套教材（`original.c` / `solution.c` / `README.md` / `TRACE.md` / `TESTING.md` / `tests.c` / `bug_demo.c` / `Makefile`）的 wiki 索引；SKILL.md 明文指定的 **Interview review style 範例實作**；含五個 bug 對照表 + 四層偵測方法論（精準 unit test / sanitizer / 警告 / 靜態分析）+ 投毒法

---

## 📅 日誌與健康檢查

- [[logs/2026-04-18_session]] — 首次建構紀錄
- [[logs/2026-04-25_session]] — wiki hygiene pass 紀錄
- [[logs/2026-04-30_session]] — 新增 kernel/{rcu, bitmap, workqueue} 與 interview/{mcupm-code-review, google-stack-with-min}
- [[health/2026-04-18_lint]] — 首次健康檢查報告
- [[health/2026-04-25_lint]] — 第二次 hygiene 檢查
- [[health/2026-04-30_lint]] — 最新健康檢查報告

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

**spinlock 相關**：併發保護、IRQ handler、atomic context（主要概念題，對應 [[interview/Mon-3-code-review-solution]]）

**rcu 相關**：lockless lookup、reader-heavy 結構、publish/grace period（主要概念題，對應 [[interview/code-review-questions]]、[[interview/mcupm-code-review]] Q5）

**bitmap 相關**：固定大小資源池、cpumask、find-first-zero（對應 [[interview/google_code_review_semaphore_prep_linux_solution]] DSP IPI channel pool）

**workqueue 相關**：deferred work、suspend/resume freeze、`alloc_workqueue` flag 選擇（對應 [[interview/mcupm-code-review]] Q10/Q11）

### 按 Pattern 快速查

以下列出有明確題目對應的 pattern（無對應題但仍完整收錄的 pattern 也見 patterns/ 檔案本身）：

- **Two Pointers**：11, 167, 141, 234, 876, 283, 392
- **Sliding Window**：3, 76, 239, 424, 567, 643, 480, 219, 1456
- **Prefix Sum**：724, 1732, 238
- **String Manipulation**：151, 345, 1768, 2390, 605
- **Hash Table**：1, 49, 217, 242, 347, 2215
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
| Patterns | 19 | ~6,200 |
| C Idioms | 4 | ~1,400 |
| Kernel | 12 | ~3,800 |
| Interview | 15 | ~8,100 |
| Overview + Index + Health + Log | 8 | ~1,200 |
| **總計** | **58** | **~20,700** |

- **Raw 題目資料夾**：75（部分含舊/新雙重目錄待清理，見健康檢查）
- **涵蓋 Kernel 源代碼位置**：35+
- **最後更新**：2026-04-30（新增 [[kernel/rcu]]、[[kernel/bitmap]]、[[kernel/workqueue]] 三個 P3 主題；新增 [[interview/mcupm-code-review]] 編入 MediaTek MCUPM 真實 driver review 兩份大文件；新增 [[interview/google-stack-with-min]] 編入 SKILL.md 指定的八件套教材；同步 _index.md / overview.md 統計與交叉引用；移除 README 末段已完成的 RCU / Bitmap / Workqueue TODO）
- **前次更新**：2026-04-29（新增 [[interview/google_code_review_semaphore_prep_linux_primitives]] 五大同步原語選擇邏輯速查；補登 Tue/Thu/Fri code review solutions 與 google_code_review_semaphore_prep 系列共 7 份檔案進索引；同步 Kernel 同步原語區段交叉引用）
- **前前次更新**：2026-04-25（wiki hygiene pass：同步統計、健康檢查、交叉引用）
- **內容增量**：2026-04-21（[[kernel/spinlock]] + [[interview/Mon-3-code-review-solution]]）/ 2026-04-20（Firmware 子系統視角 code review §14-§17 + 附錄 E Firmware 模擬日）
- **最近一次 lint**：2026-04-30（見 [[health/2026-04-30_lint]]）

---

## 🧭 使用建議

### 對於面試準備（有基礎者，4-6 週）

1. **Week 1**：`c-idioms/pointer-manipulation` + `c-idioms/manual-memory` + `patterns/two-pointers` + `patterns/sliding-window` + `patterns/prefix-sum`
2. **Week 2**：`patterns/linked-list` + `patterns/hash-table` + `patterns/stack-queue` + `kernel/list-head` + `kernel/hash-table` + `kernel/circular-buffer` + `kernel/spinlock`
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

- ~~**RCU（Read-Copy-Update）**~~ — ✅ 2026-04-30 已新增 [[kernel/rcu]]
- ~~**Bitmaps / `include/linux/bitmap.h`**~~ — ✅ 2026-04-30 已新增 [[kernel/bitmap]]
- ~~**Work Queue**~~ — ✅ 2026-04-30 已新增 [[kernel/workqueue]]
- **Memory Cgroups** — 資源隔離
- **eBPF** — 核心內虛擬機
- **Per-CPU 變數 / `local_lock`** — `percpu_counter`、`__this_cpu_*`、scheduler 與 mm 的 per-cpu 路徑
- **dm-verity / fsverity** — Android Verified Boot 驗證機制（與 firmware review 線整合）
