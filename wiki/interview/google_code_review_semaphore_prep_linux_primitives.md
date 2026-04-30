# Linux Kernel 5 大同步原語選擇邏輯（面試最後一晚速複習）

> 配合 `google_code_review_semaphore_prep_linux_solution.md` 的題目 1–4 做圖解複習。
> 目標：reviewer 問「為什麼選這個原語、不選那個？」時，能在 30 秒內給出答案。

---

## 0. 總決策樹（先看這張，其餘是細節）

```
                       ┌─────────────────────────┐
                       │ 我要保護的是「資料」     │
                       │ 還是「事件」？           │
                       └──────────┬──────────────┘
                                  │
              ┌───────────────────┴───────────────────┐
              │                                       │
        【保護資料】                              【通知事件】
              │                                       │
   ┌──────────┴──────────┐                ┌──────────┴───────────┐
   │ 在 IRQ / 不能 sleep │                │ 一次性 (one-shot)?    │
   │  的 context 嗎？    │                │  還是資源計數？       │
   └──┬───────────────┬──┘                └──┬─────────────────┬─┘
      │YES            │NO                    │one-shot         │counting
      │               │                      │                 │
  ┌───▼────┐    ┌─────▼─────┐          ┌────▼──────┐    ┌─────▼──────┐
  │spinlock│    │ reader 多?│          │completion │    │ semaphore  │
  └────────┘    │ writer 少?│          │（題 4）   │    │（題 1 限流）│
                └──┬──────┬─┘          └───────────┘    └────────────┘
                YES│      │NO
                ┌──▼──┐ ┌─▼────┐
                │rwsem│ │mutex │
                │題 3 │ │題 1  │
                └─────┘ │tx_lock│
                        └──────┘
```

「資料 vs 事件」是第一刀，剩下的都是 context 限制與性能取捨。

---

## 1. spinlock — 「不能 sleep 的短臨界區」

### 什麼時候用它

只有在以下「任一」條件成立時用 spinlock：

- 臨界區會在 **interrupt context** 跑（ISR、softirq、tasklet）
- 臨界區會在 **不能 sleep** 的地方跑（持有其它 spinlock、preempt_disable、RCU read 區）
- 臨界區 **極短**（< 微秒級），sleep + wake 的 context switch 成本反而更貴

### 什麼時候**不要**用它

- 臨界區內呼叫任何會 sleep 的 API（`copy_to_user`、`kmalloc(GFP_KERNEL)`、`mutex_lock`、`down`）→ **kernel panic / deadlock**
- 臨界區可能跑 > 數十微秒 → CPU 空轉燒電浪費電池

### 變體口訣

| 變體 | 何時用 |
|---|---|
| `spin_lock()` | 只有 process context 競爭 |
| `spin_lock_bh()` | 與 softirq/tasklet 競爭 |
| `spin_lock_irq()` | 與硬中斷競爭，但已知中斷已關 |
| `spin_lock_irqsave()` | 與硬中斷競爭，**不確定中斷狀態 → driver 預設選這個** |

### 題目 1 圖解：`list_lock` 為什麼是 spinlock

```
  ┌─────────────────────────┐         ┌──────────────────┐
  │ Process: dsp_send_ipi() │         │ ISR: dsp_ipi_isr │
  │  list_add_tail(req)     │         │  list_del_init(r)│
  └────────────┬────────────┘         └─────────┬────────┘
               │                                │
               └──────► pending list ◄──────────┘
                          ↑
                ★ ISR 也會碰，所以一定要 spinlock_irqsave
                ★ 而且只是改 list pointer，臨界區極短
```

關鍵：**ISR 在參與競爭** → 排除 mutex/semaphore（會 sleep）→ 唯一選擇 spinlock，而且要 `irqsave` 變體。

### 題目 2 圖解：bitmap 的鎖

```
  Thread A: alloc                Thread B: alloc
     │                              │
     ▼                              ▼
   spin_lock_irqsave ──────► 序列化 ◄────── spin_lock_irqsave
     │                              │
     ▼                              ▼
   find_first_zero_bit             find_first_zero_bit
   set_bit                          set_bit
     │                              │
     ▼                              ▼
   spin_unlock_irqrestore         spin_unlock_irqrestore
```

為什麼**不**用 `test_bit()` + `set_bit()`？因為兩步之間有 race window，兩個 thread 會看到同一個 free bit，雙雙 set 上去 → channel 衝撞。`test_and_set_bit()` 雖然 atomic，但你還想做 `find_first_zero_bit`（不是 atomic），所以包整段在 spinlock 裡最乾淨。

---

## 2. mutex — 「process context 的互斥鎖」

### 什麼時候用它

- 臨界區在 **process context**（不在 IRQ 內）
- 臨界區可能會 sleep（呼叫 `kmalloc(GFP_KERNEL)`、I/O、`copy_to_user`）
- 想要 **明確的互斥語意**（一個 owner、可重入檢查、lockdep 友善）

### 什麼時候**不要**用它

- ISR / softirq → 不能 sleep，會 BUG
- 想做 reader-writer 並行 → 用 rwsem
- 想 signal one-shot 事件 → 用 completion
- 想限制資源 N 個 in-flight → 用 counting semaphore

### mutex vs binary semaphore（reviewer 常考）

| | mutex | semaphore (init 1) |
|---|---|---|
| owner 概念 | 有（誰 lock 誰 unlock） | 沒有 |
| priority inheritance | 有 | 沒有 |
| lockdep 支援 | 完整 | 弱 |
| Linus 偏好 | ★ 強烈推薦 | 「能不用就不用」 |

**結論**：在 process context 想互斥，**永遠先想 mutex**，除非真的需要「跨 thread 的 V 操作」（例如題 1 的 inflight 限流）。

### 題目 1 圖解：`tx_lock` 為什麼是 mutex 而不是 spinlock

```
  Thread A 持 tx_lock
       │
       ├── memcpy_toio(payload, 64 bytes)   ← 可能短暫 sleep（cache flush）
       ├── wmb()
       ├── writel(seq)                       ← MMIO，慢
       ├── writel(trigger)                   ← MMIO，慢
       │
       └── mutex_unlock(tx_lock)

  總臨界區：~微秒級的多次 MMIO write
  → 用 spinlock 會讓其他 CPU 空轉，浪費；用 mutex 讓他們 sleep 比較好
  → 而且 tx_lock 不會被 ISR 持有（ISR 只動 list_lock），所以 mutex 安全
```

兩個鎖分工清楚：

```
  ┌─────────────────────────────────────────────────────┐
  │ tx_lock (mutex):  序列化「寫 mbox register」        │
  │   ── 只有 process context 會持有                    │
  │   ── 持有時間中等，且寫硬體比較慢                   │
  ├─────────────────────────────────────────────────────┤
  │ list_lock (spinlock_irqsave): 保護 pending list     │
  │   ── ISR 也會持有，所以不能 mutex                   │
  │   ── 臨界區極短（list_add / list_del）              │
  └─────────────────────────────────────────────────────┘
```

---

## 3. semaphore — 「counting 限流 / 資源池」

### 什麼時候用它

只剩下這個用途：**counting semaphore，限制同時在用的資源數量到 N**。

- mailbox HW FIFO depth = 4 → `sema_init(&inflight, 4)`
- DMA channel pool = 8 → `sema_init(&dma_pool, 8)`
- 「拿不到就睡到有為止」的語意（best-effort 拿不到就走 → 用 spinlock + bitmap）

### 什麼時候**不要**用它

| 反模式 | 應該改成 |
|---|---|
| `sema_init(&s, 1)` 當 mutex 用 | `mutex` |
| `sema_init(&s, 0)` 當事件通知 | `struct completion` |
| 在 ISR 裡 `down()` | 改架構，ISR 內只能 `up()` 或 `down_trylock()` |
| `down()`（不可中斷） | `down_interruptible()` 或 `down_killable()` |
| best-effort 拿資源 | spinlock + bitmap（題 2） |

### 慣用 API 一覽

```
  down_interruptible(&s)        ← driver 預設
  down_killable(&s)             ← 只能被 SIGKILL 醒
  down_timeout(&s, jiffies)     ← 最常用：可被 signal + 帶 timeout
  down_trylock(&s)              ← 不阻塞，可在 atomic context 用
  up(&s)                        ← 任何 context 都可以
```

### 題目 1 圖解：`inflight` semaphore 限流

```
  inflight semaphore，初值 = 4 (mbox FIFO depth)

   token: [●][●][●][●]   ← 全滿，4 個 thread 可同時送

  Thread1 down_timeout ──► 拿走一個 token  [●][●][●][ ]
  Thread2 down_timeout ──► 拿走一個 token  [●][●][ ][ ]
  Thread3 down_timeout ──► 拿走一個 token  [●][ ][ ][ ]
  Thread4 down_timeout ──► 拿走一個 token  [ ][ ][ ][ ]
  Thread5 down_timeout ──► sleep 等 token  ⏳

  ── 任一 thread 完成 ──
  up(&inflight)        ──► 還回 token        [●][ ][ ][ ]
                          → Thread5 醒來繼續
```

**關鍵：是「資源計數」，不是「互斥」**。如果只允許 1 個 in-flight，那才是 mutex。允許 N 個並行，且滿了要排隊 → semaphore。

### 題目 2 反例：為什麼**不要**用 semaphore

原版用 `sema_init(&pool, 65)` 來「最多 65 個 channel 在用」，看起來合理。但：

```
  反模式：semaphore + bitmap 雙重簿記
  ┌──────────────────────────────────────────┐
  │ down(&pool)          ← 計數 -1           │
  │ test_bit + set_bit   ← 找空位（race！）  │
  │ ...                                      │
  │ ↑                                        │
  │ 兩套簿記不同步：                         │
  │  · semaphore 說還有 token，bitmap 全滿   │
  │  · semaphore 是 0 但 bitmap 有空位       │
  │  → 必有 leak 或卡死                      │
  └──────────────────────────────────────────┘

  正解：bitmap 自己就是計數，spinlock 包起來就好
  ┌──────────────────────────────────────────┐
  │ spin_lock_irqsave(&p->lock)              │
  │ ch = find_first_zero_bit(busy, nr)       │
  │ if (ch < nr) set_bit(ch, busy)           │
  │ else         ch = -ENOSPC  (best-effort) │
  │ spin_unlock_irqrestore(&p->lock)         │
  └──────────────────────────────────────────┘
```

**判斷準則**：caller「拿不到就走（回 -ENOSPC）」→ 不需要 sleep → 不該用 semaphore。

---

## 4. completion — 「one-shot 事件通知」

### 什麼時候用它

- **A thread 等 B 做完一件事**，B 做完用 `complete()` 通知
- one-shot 語意（每個 completion 只用一次，或重新 `reinit_completion`）
- per-request 的 ack：每個 request 自帶一個 completion
- driver probe 等 firmware ready
- 等 worker thread / kthread 結束（`kthread_stop` + `wait_for_completion`）

### 什麼時候**不要**用它

- 多個 waiter 等同一個事件，但每個都要拿到「自己」的結果 → 還是要 per-request completion
- 想做計數（剛好 N 次完成才算完）→ semaphore 或 atomic + waitqueue
- 想做 reader/writer 並行 → rwsem

### completion vs `sema_init(&s, 0)`

兩者語意上等價，但：

```
  semaphore init=0          completion
  ────────────────────      ────────────────────
  down(&s)                  wait_for_completion(&c)
  up(&s)                    complete(&c)
                            complete_all(&c)   ← 喚醒所有 waiter
                            reinit_completion(&c)
```

`completion` 是 idiomatic 寫法，reviewer 看到 `sema_init(&s, 0)` 會皺眉。

### 必背 API：永遠用「可中斷 + timeout」

```
  ✗ wait_for_completion(&c)                       ← unkillable，禁用
  ✗ wait_for_completion_interruptible(&c)         ← 沒 timeout，hang 風險
  ✓ wait_for_completion_interruptible_timeout(&c, msecs_to_jiffies(2000))
       ── 回傳：> 0 (剩餘 jiffies), 0 (timeout), < 0 (signal -ERESTARTSYS)
  ✓ wait_for_completion_killable_timeout(...)     ← 只接 SIGKILL
```

### 題目 1 圖解：per-request completion 解決「ack 張冠李戴」

```
  反模式：全域 ack semaphore
  ─────────────────────────────────────────────────────
  Thread A 送 channel 3                Thread B 送 channel 7
       │                                   │
       │ down(&ctx.ack) ⏳                 │ down(&ctx.ack) ⏳
       │                                   │
       │             DSP 先完成 ch7        │
       │             ISR: up(&ctx.ack)     │
       │   ✗ Thread A 被喚醒了 !!!         │
       │   ← 拿到 B 的結果，B 永遠等不到   │
       └───────────────────────────────────┘

  正解：per-request completion
  ─────────────────────────────────────────────────────
  Thread A: req_A.seq = 100        Thread B: req_B.seq = 101
       │ list_add(&req_A)              │ list_add(&req_B)
       │ wait_for_completion(&req_A.done)
       │                               │ wait_for_completion(&req_B.done)
       │                               │
       │       DSP 回 seq=101 ────► ISR
       │                               │  list_for_each:
       │                               │    if r.seq == 101: found = r
       │                               │  complete(&found->done)
       │                               │       ↓
       │                               │  ✓ 只喚醒 Thread B
```

「身份」是關鍵：semaphore 沒有身份概念，completion 配上 per-request struct 就有身份。

### 題目 4 圖解：probe 的 startup barrier

```
  AP 端 (probe thread)              DSP 端
  ─────────────────────             ─────────────────
  init_completion(&fw_ready)
  request_irq(dsp_ready_isr)   ←── ★ 順序：先 init 再 register
  load_firmware_to_tcm()
  writel(BOOT_CMD)             ──► DSP 開始 boot
                                   │
  wait_for_completion           ⏳  │ boot 中…
   _interruptible_timeout            │
   (&fw_ready, 2 秒)                 │ boot 完成
                                   ┌─┘
  ◄─── mailbox interrupt ──── DSP 觸發 mbox
  dsp_ready_isr:
     complete(&fw_ready)
                            │
  wait_for_completion 返回 ✓│
  register_dsp_services()
  return 0  (probe success)
```

`completion` 完美對應「one-shot 啟動握手」的場景。注意 ordering：**先 init_completion 再 request_irq**，否則 ISR 可能在 completion 還沒初始化前就跑進來。

---

## 5. rw_semaphore (rwsem) — 「reader 多、writer 少」

### 什麼時候用它

- 資料**讀遠多於寫**（例如 99% read, 1% write）
- reader 與 reader 之間**完全可以並行**
- 持有期間**可能 sleep**（不能在 ISR 用）
- 不適合用 RCU（資料結構太複雜或 writer 路徑太重）

### 什麼時候**不要**用它

- read 與 write 比例接近 → mutex 更簡單，沒有讀寫切換成本
- 在 ISR 裡 → 用 `read_lock`（spinlock 變體）或 RCU
- writer 非常頻繁 → rwsem 沒比 mutex 快多少，反而複雜
- 對 read 性能極致要求 → RCU

### Linux rwsem 特性（reviewer 會問）

| 特性 | 說明 |
|---|---|
| 多 reader 並行 | 多個 thread 可同時 `down_read` |
| 單 writer 排他 | `down_write` 期間沒有任何 reader/writer |
| **writer-preference** | 有 writer 等待時，新 reader 要排隊 → 不會 starve writer |
| 可 sleep | reader 與 writer 期間都可以 sleep（如 `copy_to_user`） |
| 不可在 IRQ | 都會 sleep |

### API 對照

```
  down_read(&rwsem)                ↔  up_read(&rwsem)
  down_read_killable(&rwsem)       ← driver 推薦：可被 SIGKILL 醒
  down_read_trylock(&rwsem)        ← 不阻塞

  down_write(&rwsem)               ↔  up_write(&rwsem)
  down_write_killable(&rwsem)      ← driver 推薦
  downgrade_write(&rwsem)          ← write 鎖降級成 read 鎖
```

### 題目 3 圖解：log 多 reader

```
  讀 vs 寫 比例：
    dsp_log_read   ── 高頻，N 個 user space process 並行 cat /dev/dsplog
    dsp_log_rewind ── 極少，debug 工具偶爾呼叫一次

  rwsem 行為：
  ─────────────────────────────────────────────────────────
  Time →

  Reader1: ████████░░░░░░░░░░░████████████░░░░░░░░░░░░░░
  Reader2:    █████████░░░░░░░░░░░░░██████░░░░░░░░░░░░░░
  Reader3:      ███████░░░░░░░░░░░░░░░░░░░░░██████░░░░░░
  Writer:                ████████████ (rewind)
                          ↑          ↑
                     等所有 reader 走完  期間 reader 全擋住
                                       才能進
  ▲                                                     ▲
  │ Reader 並發  │  排他寫     │     Reader 並發       │

  好處：99% 時間 reader 全並行，效能 ≈ 無鎖
        1% 時間 writer 排他，正確性有保證
```

### 為什麼不能用 mutex？

```
  mutex 版本（反模式）：
  Reader1: ████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  Reader2:     ░░░░████░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  Reader3:         ░░░░░░░░████░░░░░░░░░░░░░░░░░░░░
                                ↑ 永遠序列化，浪費 CPU
```

### 為什麼不能用 spinlock？

`copy_to_user` 會 page fault → sleep。spinlock 持有期間 sleep = **kernel BUG**。

### 為什麼不用 RCU？

可以，但 RCU 寫者要做 copy + atomic pointer swap，對「memset 整個 8MB log buffer」這種寫入太重；rwsem 更直觀。

---

## 6. 題目 1–4 「鎖配置全圖」

```
  ┌──────────────────────────────────────────────────────────────┐
  │  題目 1: dsp_send_ipi  ── 4 種原語齊用                        │
  ├──────────────────────────────────────────────────────────────┤
  │                                                                │
  │   [semaphore inflight]  ← 限流 (mbox FIFO 4 槽)               │
  │           │                                                    │
  │           ▼                                                    │
  │   [mutex tx_lock]       ← 序列化 mbox register write          │
  │           │                                                    │
  │           ▼                                                    │
  │   [spinlock list_lock]  ← 與 ISR 共享 pending list            │
  │           │                                                    │
  │           ▼                                                    │
  │   [completion req.done] ← per-request 等 DSP ack              │
  │                                                                │
  └──────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────┐
  │  題目 2: dsp_ipi_channel_alloc  ── 只用 spinlock              │
  ├──────────────────────────────────────────────────────────────┤
  │                                                                │
  │   [spinlock] + [bitmap]                                        │
  │       ↑ best-effort 拿不到回 -ENOSPC，不需要 sleep            │
  │       ↑ bitmap 本身就是計數，不需要 semaphore 雙重簿記         │
  │                                                                │
  └──────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────┐
  │  題目 3: dsp_log_read / rewind  ── 只用 rwsem                 │
  ├──────────────────────────────────────────────────────────────┤
  │                                                                │
  │   [rw_semaphore]                                               │
  │       reader: down_read_killable  → copy_to_user → up_read    │
  │       writer: down_write          → memset       → up_write   │
  │       ↑ 多 reader 並行，rewind 排他                            │
  │       ↑ copy_to_user 會 sleep，spinlock 不行                   │
  │                                                                │
  └──────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────┐
  │  題目 4: dsp_mbox_probe  ── completion + spinlock             │
  ├──────────────────────────────────────────────────────────────┤
  │                                                                │
  │   [completion fw_ready]    ← one-shot：等 DSP boot 完           │
  │   [spinlock lock]           ← 保護 driver 狀態（如有需要）      │
  │       ↑ probe 是典型 startup barrier 場景                       │
  │       ↑ 一定要：先 init → 再 request_irq → 再 trigger          │
  │                                                                │
  └──────────────────────────────────────────────────────────────┘
```

---

## 7. 30 秒判斷流程（面試現場用）

當 reviewer 指著一段 code 問「這個鎖選對了嗎？」：

1. **Context 檢查**
   - 會在 IRQ / softirq / atomic context 跑嗎？
     → YES：只能 spinlock 或 atomic 操作。其他 sleeping lock（mutex/rwsem/semaphore）一律 BUG。

2. **語意檢查**
   - 是「保護資料」還是「通知事件」？
     → 事件、且 one-shot：completion
     → 事件、且資源計數：counting semaphore
     → 資料：往下看 #3

3. **持有時間檢查**
   - 臨界區會 sleep（`copy_to_user`、`kmalloc(GFP_KERNEL)`、I/O）嗎？
     → YES：mutex 或 rwsem
     → NO 且極短：spinlock（即使 process context，省 context switch）

4. **並行度檢查**
   - read >> write，且 reader 真的可以並行？
     → YES：rwsem（或 RCU）
     → NO：mutex

5. **最後反問**：
   - 「我用 semaphore 是不是其實在當 mutex / completion 用？」→ 換掉
   - 「我有沒有在 ISR 裡寫了會 sleep 的東西？」→ 重構

---

## 8. 反模式速查（reviewer 看到就會打槍）

| 反模式 | 修法 |
|---|---|
| `sema_init(&s, 1)` 當互斥 | `mutex` |
| `sema_init(&s, 0)` 當事件 | `completion` |
| `down(&s)` 不可中斷 | `down_interruptible` / `down_timeout` |
| `wait_for_completion(&c)` 沒 timeout | `wait_for_completion_interruptible_timeout` |
| ISR 內 `mutex_lock` / `down` / `kmalloc(GFP_KERNEL)` | 改 ISR 只做最小工作，剩下 schedule_work / threaded IRQ |
| `spinlock` 持有期間 `copy_to_user` | 改 mutex / rwsem |
| 全域 ack semaphore | per-request completion |
| `test_bit` + `set_bit` 不 atomic | `test_and_set_bit` 或包 spinlock |
| semaphore + bitmap 雙重簿記 | spinlock + bitmap |
| `if (shutting_down) down(&inflight)` 兩步有 race | gate 用 mutex 包成 atomic check + acquire |

---

## 9. 一句話總結（面試前最後一秒）

> **「資料用 mutex（除非 IRQ 用 spinlock，或多 reader 用 rwsem）；事件用 completion（除非要計數用 semaphore）。」**

祝面試順利。
