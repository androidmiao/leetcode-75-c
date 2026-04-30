# MCUPM Driver Code Review — wiki 索引

> **原始素材**：
> - [`MCUPM_Code_Review_Questions.md`](../../MCUPM_Code_Review/MCUPM_Code_Review_Questions.md)（11 題 + 加分題 + checklist，755 行）
> - [`MCUPM_Code_Review_Answers.md`](../../MCUPM_Code_Review/MCUPM_Code_Review_Answers.md)（每題逐行繁中註解修正版，857 行）
>
> **取材來源**：MediaTek `drivers/remoteproc/mcupm/v2/`（`mcupm_driver.c`、`mcupm_plt.c`、`mcupm_timesync.c`）——真實 production code，非 textbook 範例。
>
> **適用情境**：Google L4 / L5 onsite「主管 Code Review」風格——reviewer 拿一段 driver 給你 30–45 分鐘 review，你要找出 bug、講清楚根因、提出可上線的修法。

---

## 為什麼這份題庫值得單獨建索引

整個 wiki 的 code review 系列大致分三層：

| 層次 | 代表檔 | 題目來源 |
|------|--------|----------|
| 教材級 mock | `[[interview/weekly-mock-bank]]`、`[[interview/Mon-3-code-review-solution]]` | 為練習設計的小段 |
| 主題式速成 | `[[interview/google_code_review_semaphore_prep]]` 系列 | semaphore / Linux primitive 專題 |
| **真實 driver review** | **本檔 + MCUPM_Code_Review/** | **MediaTek production code** |

第三層的價值在於：bug 不是 textbook 案例，而是 review 現場常被打槍的點——「這個 unsigned int 沒檢查上界」「這個 deprecated API 換掉」「這個 suspend/resume 不對稱」。能在這層次答得好，才是真的能 own driver review。

---

## 背景：MCUPM Driver 在做什麼

MCUPM (MCU Power Manager) 是 MediaTek 平台上一顆 co-processor 的 Linux kernel driver，做三件事：

1. **IPI / Mailbox 介面** (`mcupm_driver.c`) — 16 個 mailbox channel，包裝 `mtk_mbox_*` / `mtk_ipi_*` 底層 API。
2. **Platform 層 sysfs 診斷** (`mcupm_plt.c`) — `mcupm_alive`、`mcdi_dynamic_finegrain` 等 sysfs node 做 runtime ping 與 fine-grain 控制。
3. **Time Sync** (`mcupm_timesync.c`) — hrtimer 週期把 ARM `arch_timer` tick 與 `sched_clock` timestamp 透過 mailbox 同步給 MCU；suspend/resume 要 freeze/unfreeze。

每個面向都有典型 bug 模式，正好對應到 wiki 已有的各個主題。

---

## 11 題對照表

| # | bug 類型 | 涉及子系統 | 對應 wiki 章節 |
|---|----------|-----------|-----------------|
| Q1 | `unsigned int mbox` 沒上界檢查 → array OOB read + spinlock OOB | mailbox helper API | `[[kernel/bitmap]]` 邊界判斷、`[[kernel/spinlock]]` defensive use |
| Q2 | `mcupm_pdev` global UAF：probe 記下、remove 沒清 | global state lifetime | `[[interview/code-review-questions]]` lifecycle 章節 |
| Q3 | `unsigned` 函式 return `-1` → caller 永遠看不到錯誤 | error code convention | `[[c-idioms/comparator-patterns]]` signed/unsigned 陷阱 |
| Q4 | `free_page()` 用在不是 page allocator 拿來的 buffer | memory ownership | `[[kernel/memory-management]]` allocator 配對、`[[c-idioms/manual-memory]]` |
| Q5 | `mcupm_plt_ackdata` global sentinel race（多個 sysfs 同時 ping）| sysfs callback race | `[[kernel/spinlock]]`、`[[kernel/rcu]]`（讀多寫少改 SRCU 的延伸選項）|
| Q6 | Probe 失敗時資源沒 unwound（沒 goto cleanup label）| error path | `[[interview/Mon-3-code-review-solution]]` cleanup pattern |
| Q7 | Legacy DT 分支下 `offset` / `res_size` 未初始化就被印出 | uninitialized var | `[[interview/edge-cases]]`、`[[interview/code-review-questions]]` |
| Q8 | `mcupm_mbox_write` lock 釋放後又 write — TOCTOU 視窗 | locking scope | `[[kernel/spinlock]]` critical section 完整性 |
| Q9 | Time Sync 「version + freeze header」協定 → 應改 seqlock 語義 + barrier | producer/consumer 協定 | `[[kernel/circular-buffer]]`、`[[kernel/network-sliding-window]]` |
| Q10 | `create_workqueue` 已 deprecated；suspend/resume freeze/unfreeze 不對稱 | workqueue lifecycle + power | **`[[kernel/workqueue]]`** 主題核心 |
| Q11 | Module exit 整段空白（兩個 `// TODO`）| teardown 完整性 | `[[kernel/workqueue]]` teardown 三步驟 |

---

## 三條分類軸：bug 嚴重度

主管 review 的最後一步是給結論，不是逐條列點。本題庫示範的分類：

| 等級 | 涵蓋題號 | 評論口吻 |
|------|---------|----------|
| ❌ **NACK**（必須修才能 land） | Q1, Q4 | "array OOB 跟 `free_page` mis-use 都是 kernel panic 級" |
| 🟡 **Major**（設計缺陷，follow-up patch） | Q2, Q5, Q6, Q9, Q10 | "lifecycle / locking / power 對稱性，要分頭解" |
| 🟢 **Nit**（hygiene，本 patch 或 follow-up 都可） | Q3, Q7, Q11 | "errno convention / 初始化 / TODO 補完" |

主管要看的是「能不能 own 一個 review process」，不是「能不能挑 bug」——這份分類本身就是面試考點。

---

## Review 自我檢查清單（11 條速記）

從 `MCUPM_Code_Review_Questions.md` 末段濃縮：

1. 看到 `unsigned int idx` 立刻檢查 array 邊界。
2. 看到 global state 立刻問 lifetime / locking。
3. 看到 `unsigned` 函式 return 負值立刻起疑。
4. 看到 `(unsigned long) buf` 強制 cast 立刻檢查 buffer ownership。
5. 看到 sysfs callback 用 global ack 立刻檢查 race。
6. 看到 probe path 多個 early return 立刻檢查 cleanup。
7. 看到分支內初始化變數立刻檢查所有分支。
8. 看到 lock/unlock 中間還有共享狀態存取立刻檢查 TOCTOU。
9. 看到 mailbox / IPI / shared memory 立刻檢查 protocol 文件 + barrier。
10. 看到 deprecated API（`create_workqueue`、`create_singlethread_workqueue`）立刻提替代。
11. 看到 `// TODO` 立刻問 owner / ETA。

---

## 與其他 wiki 的交叉

### 與 kernel 主題

- **`[[kernel/spinlock]]`** — Q1 / Q5 / Q8 都跨在「spinlock 怎麼包」這條軸上。
- **`[[kernel/workqueue]]`** — Q10 / Q11 是 workqueue 主題的真實案例。
- **`[[kernel/bitmap]]`** — Q1 邊界檢查與 `[[kernel/bitmap]]` 提到的 `find_first_zero_bit` 邊界檢查同源。
- **`[[kernel/memory-management]]`** — Q4 是 allocator 配對的經典反例。
- **`[[kernel/rcu]]`** — Q5 ackdata race 的進階修法可考慮 SRCU。

### 與其他 code review 教材

- **`[[interview/google_code_review_semaphore_prep_linux_solution]]`** — DSP IPI driver 6 題。MCUPM 是「mailbox + sysfs + timesync」的 superset；先做 DSP IPI 比較簡單，再來 MCUPM。
- **`[[interview/code-review-questions]]`** — 主管視角 review checklist；本檔的 11 條速記可視為 checklist 的「mailbox driver 版」。
- **`[[interview/weekly-mock-bank]]`** — 4 個週訓 round；MCUPM 適合放在「最後一週的高難度週末練習」。

### 與 patterns

- **`[[patterns/bit-manipulation]]`** — Q1 array index 檢查；mailbox channel 編號通常用 bitmap。
- 本題庫主要是 kernel-side，patterns 對應較少。

---

## 學習建議

### 第一遍（45 分鐘 timer）

1. 只看 `MCUPM_Code_Review_Questions.md` 的程式碼段，不看「期望回答方向」。
2. 自己寫 review comment（NACK / Major / Nit + 理由）。
3. 結束後對照「期望回答方向」校驗。

### 第二遍（深入修法）

1. 對照 `MCUPM_Code_Review_Answers.md` 的逐行繁中註解。
2. 注意每題「修正重點說明」：很多時候不是改一行，而是改設計（global → per-instance、sentinel → completion、create_workqueue → alloc_workqueue）。
3. 記住每題對應的 kernel idiom，將來 review 別人 driver 時直接套用。

### 第三遍（融會貫通）

挑 3 題串成 follow-up：
- **Lifecycle 串**：Q2 (global) → Q6 (probe cleanup) → Q11 (module exit)
- **Concurrency 串**：Q1 (bounds) → Q5 (sysfs race) → Q8 (TOCTOU) → Q9 (protocol barrier)
- **Power / API hygiene 串**：Q10 (workqueue + suspend) → Q11 (exit) → Q3 (errno) → Q7 (init)

---

## Sources

- `MCUPM_Code_Review/MCUPM_Code_Review_Questions.md`
- `MCUPM_Code_Review/MCUPM_Code_Review_Answers.md`
- `drivers/remoteproc/mcupm/v2/mcupm_driver.c`
- `drivers/remoteproc/mcupm/v2/mcupm_plt.c`
- `drivers/remoteproc/mcupm/v2/mcupm_timesync.c`
