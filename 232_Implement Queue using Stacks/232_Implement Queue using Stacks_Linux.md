# 232. Implement Queue using Stacks — Linux Kernel `kfifo`、`workqueue` 對照分析

## Linux 相關原始碼位置

| 檔案 | 用途 |
|------|------|
| `include/linux/kfifo.h` | kfifo 資料結構定義與巨集 API（type-safe FIFO 佇列） |
| `lib/kfifo.c` | kfifo 核心實作：初始化、in/out、記憶體管理 |
| `kernel/workqueue.c` | Concurrency Managed Workqueue (cmwq) 完整實作 |
| `include/linux/workqueue.h` | `work_struct`、`delayed_work` 結構與 API 宣告 |
| `include/linux/wait.h` | Wait queue：睡眠等待條件的 FIFO 佇列 |

---

## 一、kfifo：Kernel 的高效 FIFO 佇列

### 核心資料結構

```c
/* include/linux/kfifo.h */
struct __kfifo {
    unsigned int in;      /* 寫入指標（生產者推進） */
    unsigned int out;     /* 讀取指標（消費者推進） */
    unsigned int mask;    /* size - 1，用於位元遮罩取代 modulo */
    unsigned int esize;   /* 每個元素的大小（bytes） */
    void *data;           /* 指向環形緩衝區 */
};
```

### kfifo 的精華：unsigned int 自然溢位 + power-of-2 位元遮罩

kfifo 不用兩個棧來實現 FIFO，而是用**環形緩衝區 + 兩個單調遞增指標**：

```
Buffer size = 8 (power of 2), mask = 7 (0b111)

          ┌───┬───┬───┬───┬───┬───┬───┬───┐
  data:   │ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │
          └───┴───┴───┴───┴───┴───┴───┴───┘
                ↑               ↑
            out & mask      in & mask
             (= 1)           (= 5)

  in  = 13  → 13 & 7 = 5  (實際寫入位置)
  out = 9   → 9  & 7 = 1  (實際讀取位置)
  元素數量 = in - out = 13 - 9 = 4
```

關鍵技巧：

1. **Power-of-2 大小**：緩衝區強制為 2 的冪，所以 `index % size` 等價於 `index & (size - 1)`，用位元 AND 取代昂貴的除法
2. **指標永不回繞**：`in` 和 `out` 是 `unsigned int`，只會單調遞增。當溢位時，C 語言的無號整數模運算自動處理回繞——`UINT_MAX + 1 = 0`，差值運算依然正確
3. **空/滿判斷極簡**：`empty ↔ in == out`，`full ↔ (in - out) > mask`

### kfifo API

```c
/* 初始化 */
int kfifo_alloc(struct kfifo *fifo, unsigned int size, gfp_t gfp_mask);
void kfifo_init(struct kfifo *fifo, void *buffer, unsigned int size);

/* 單元素操作（巨集，type-safe） */
kfifo_put(fifo, val)      /* 寫入一個元素，滿則回傳 0 */
kfifo_get(fifo, val)      /* 讀出一個元素，空則回傳 0 */

/* 批次操作 */
unsigned int kfifo_in(fifo, buf, n);   /* 寫入 n 個元素 */
unsigned int kfifo_out(fifo, buf, n);  /* 讀出 n 個元素 */

/* 查詢 */
kfifo_len(fifo)           /* 已用元素數：in - out */
kfifo_is_empty(fifo)      /* in == out */
kfifo_is_full(fifo)       /* len > mask */
kfifo_avail(fifo)         /* 可用空間 */

/* 清理 */
kfifo_free(fifo)
kfifo_reset(fifo)         /* 將 in = out = 0 */
```

### 無鎖 SPSC（Single Producer, Single Consumer）

kfifo 在單一生產者、單一消費者的場景下不需要鎖——只需記憶體屏障：

```c
/* 生產者：先寫資料，再推進 in */
memcpy(fifo->data + (fifo->in & fifo->mask) * esize, from, esize);
smp_wmb();           /* 寫入屏障：確保資料先於指標可見 */
fifo->in++;

/* 消費者：先讀指標，再讀資料 */
smp_rmb();           /* 讀取屏障：確保看到最新的 in 值後再讀資料 */
memcpy(to, fifo->data + (fifo->out & fifo->mask) * esize, esize);
fifo->out++;
```

這是 kernel 中 lock-free programming 的經典範例。

---

## 二、workqueue：用 FIFO 佇列排程延遲工作

### 核心概念

workqueue 是 kernel 中延遲執行工作的機制。工作項目（`work_struct`）被排入佇列，由 worker thread 以 **FIFO 順序**取出執行。

```c
/* include/linux/workqueue.h */
struct work_struct {
    atomic_long_t data;        /* 狀態旗標 + pool ID */
    struct list_head entry;    /* 掛在 worklist 上的鏈結串列節點 */
    work_func_t func;          /* 要執行的函式 */
};
```

### 排隊與執行流程

```
使用者程式碼                    Kernel Worker Pool
────────────                  ─────────────────
queue_work(wq, &work)
      │
      ▼
  pool->worklist  ←──────── FIFO 鏈結串列
  ┌──────┬──────┬──────┐
  │work_A│work_B│work_C│   worker thread 從頭取出
  └──────┴──────┴──────┘
      │
      ▼
  worker_thread()
      → process_one_work(work_A)
      → process_one_work(work_B)  ... FIFO 順序
```

### Ordered Workqueue：嚴格 FIFO

```c
/* 建立嚴格序列化的工作佇列，一次只執行一個工作 */
struct workqueue_struct *wq = alloc_ordered_workqueue("my_wq", 0);
/* 內部設定 max_active=1 + WQ_UNBOUND，保證嚴格 FIFO */
```

這和 LeetCode 232 的佇列語義完全一致：先排入的工作先被執行。

---

## 三、Kernel vs. LeetCode 實作對照

| 維度 | LeetCode 232（兩個棧） | kfifo（環形緩衝區） | workqueue（鏈結串列） |
|------|----------------------|--------------------|--------------------|
| FIFO 實現方式 | 透過兩次 LIFO 反轉順序 | 環形緩衝區 + in/out 指標 | `list_head` 雙向鏈結串列 |
| Push 複雜度 | O(1) | O(1) | O(1)（`list_add_tail`） |
| Pop 複雜度 | 攤還 O(1)，最壞 O(n) | O(1) 嚴格 | O(1)（`list_del` + 執行） |
| 空間 | O(n)，兩個棧 | O(n)，預分配環形緩衝區 | O(n)，每個 work 自帶節點 |
| 容量 | 動態（棧可成長） | 固定大小（power-of-2） | 無限（動態鏈結串列） |
| 並行安全 | 不考慮 | SPSC 無鎖；MPMC 需 spinlock | spinlock + cmwq 並行控制 |
| 記憶體配置 | 每次 push 可能 realloc | 初始化時一次 kmalloc | 每個 work_struct 嵌入呼叫者 |

### 為什麼 Kernel 不用「兩個棧」實現佇列？

1. **O(n) 最壞情況不可接受**：kernel 的中斷處理、驅動程式需要嚴格 O(1) 的 enqueue/dequeue。LeetCode 232 的延遲轉移在攤還意義上是 O(1)，但單次最壞 O(n) 在即時系統中是災難
2. **Cache 不友善**：兩個棧之間的搬移會造成大量 cache line 失效。kfifo 的環形緩衝區是連續記憶體，具有極佳的 cache locality
3. **無鎖設計困難**：兩個棧的延遲轉移涉及兩個資料結構的同步修改，很難做到 lock-free。kfifo 的 SPSC 模型只需記憶體屏障

### LeetCode 232 的延遲轉移 vs. kfifo 的 in/out 指標

```
LeetCode 232:                    kfifo:
   inbox → outbox 轉移              in 指標推進 → out 指標推進
   ┌───┐    ┌───┐                    ┌───┬───┬───┬───┐
   │ 3 │    │ 1 │← top               │ A │ B │ C │   │
   │ 2 │    │ 2 │                     └───┴───┴───┴───┘
   │ 1 │    │ 3 │                       ↑           ↑
   └───┘    └───┘                      out          in
   全量搬移：O(n)                    直接移動指標：O(1)
```

LeetCode 232 的價值在於展示「如何用受限的原語（棧）模擬更強的抽象（佇列）」。但在實際系統中，kernel 直接使用環形緩衝區，因為它沒有「只能用棧」的限制。

---

## 四、主管 Code Review 角度考題

### 1. 「kfifo 為什麼強制 power-of-2 大小？不能用任意大小嗎？」

**考察重點：** 位元操作 vs. 除法的效能意識

**期望回答方向：**

如果 size 是 power-of-2，`index % size` 可以用 `index & (size - 1)` 取代。在 kernel 中，整數除法指令在某些架構（如早期 ARM）上非常昂貴（數十個 cycle），而位元 AND 只需 1 個 cycle。kfifo 在 hot path 上被大量呼叫（例如 DMA 緩衝區、串口驅動），這個最佳化效果顯著。

如果需要任意大小，可以用 `if (index >= size) index -= size` 取代 modulo，但這引入了分支（branch），在 pipeline 深的處理器上可能造成 branch misprediction。Power-of-2 的位元遮罩是 branchless 的。

**面試追問：** 「如果使用者請求 size=100，kfifo 會怎麼處理？」（答：`roundup_pow_of_two(100)` → 128，浪費 28 個元素的空間換取 O(1) 位元操作。）

### 2. 「kfifo 的 in/out 指標永遠不回繞，溢位怎麼辦？」

**考察重點：** 對 unsigned integer overflow 的理解

**期望回答方向：**

`unsigned int` 在 C 語言中溢位是**定義行為**（defined behavior），而非未定義行為。`UINT_MAX + 1 == 0`，差值運算在模 2^32 下依然正確。

舉例：假設 `in = 4294967290`，`out = 4294967285`，元素數量 = `in - out = 5`。即使 `in` 溢位到 `in = 3`（加了 9），`in - out` 在 unsigned 運算下仍然是 `3 - 4294967285 = 14`（模 2^32），正確反映新增了 14 個元素。

這是一個教科書級的 unsigned overflow 應用，在 kernel 的計時器（jiffies）、序列號中隨處可見。

**面試追問：** 「如果把 in/out 改成 signed int 會怎樣？」（答：signed overflow 是 UB，編譯器可能做出意外最佳化，例如假設 `in >= out` 永遠成立。）

### 3. 「你的 LeetCode 解法用了兩個陣列模擬棧，kfifo 只用一個緩衝區。從記憶體使用角度看有什麼差別？」

**考察重點：** 記憶體效率、cache 行為

**期望回答方向：**

LeetCode 232 預分配了兩個陣列（`inbox` 和 `outbox`），各 1000 個 int。最壞情況下只有一半的空間被有效使用（所有元素都在同一個棧中）。而且轉移時需要逐一複製，觸碰兩塊不同的記憶體區域。

kfifo 只分配一個環形緩衝區，利用率接近 100%（power-of-2 向上取整帶來的浪費通常不超過 2x）。讀寫操作在同一塊連續記憶體上移動指標，具有最佳的 spatial locality。

**面試追問：** 「在什麼場景下，兩個棧的方案反而比環形緩衝區更適合？」（答：當你的原語真的只有棧操作時——例如某些 hardware stack machines，或者 undo/redo 系統中需要反轉最近操作的場景。）

### 4. 「workqueue 用 list_head 鏈結串列做 FIFO，kfifo 用環形緩衝區。什麼時候該用哪個？」

**考察重點：** 系統設計、工具選擇判斷力

**期望回答方向：**

| 場景 | 選擇 | 原因 |
|------|------|------|
| 固定大小、高頻率、小元素 | kfifo | 連續記憶體、cache 友善、無鎖 SPSC |
| 大小不固定、元素自帶節點 | list_head | 零額外配置（intrusive list） |
| 需要取消或重排佇列中的項目 | list_head | O(1) 刪除任意節點 |
| DMA 緩衝區、串口收發 | kfifo | 需要連續記憶體給 DMA 引擎 |
| 需要優先級排程 | 都不適合 | 用 priority queue 或 rbtree |

workqueue 選擇 `list_head` 是因為 `work_struct` 已經嵌入了 `list_head entry`（intrusive design），不需要額外配置記憶體。而且 workqueue 需要支援 `cancel_work_sync()`——在鏈結串列中 O(1) 移除節點，但在環形緩衝區中需要搬移元素。

### 5. 「kfifo 的 SPSC 無鎖設計，和你的 LeetCode 解法有沒有類似的『延遲』概念？」

**考察重點：** 對 lazy evaluation / deferred work 的抽象理解

**期望回答方向：**

有。LeetCode 232 的核心是「延遲轉移」（Lazy Transfer）——只在 outbox 為空時才批次搬移。kfifo 的「延遲」體現在不同層面：

1. **記憶體屏障的延遲**：`smp_wmb()` 不是立即同步，而是保證**順序性**——生產者的資料寫入在指標更新之前對消費者可見。這是一種「延遲但有序」的同步
2. **指標回繞的延遲**：`in` 和 `out` 永遠不主動回繞，讓 unsigned overflow 自然處理——延遲到硬體層面解決

兩者共同的哲學：**能不做的事就不做，等到真正需要時再處理。** 這在 kernel 中是核心設計原則——從 copy-on-write、demand paging 到 lazy TLB invalidation 都是如此。

### 6. 「如果要在 kernel driver 中實現一個日誌系統，你會用 kfifo 還是自己寫兩個棧？」

**考察重點：** 實務經驗、API 選擇

**期望回答方向：**

毫無疑問用 kfifo。原因：

1. 日誌是典型的 SPSC 場景（driver 寫入、userspace 透過 `/proc` 或 `debugfs` 讀取），kfifo 天然支援無鎖操作
2. kfifo 提供 `kfifo_to_user()` 巨集，可以直接將緩衝區內容複製到 userspace，配合 `copy_to_user()` 使用
3. 固定大小的環形緩衝區在日誌場景中是優點而非缺點——自動丟棄最舊的日誌，防止記憶體無限成長
4. 兩個棧的方案在日誌場景毫無意義——你永遠不需要「反轉」日誌順序

**面試追問：** 「如果日誌可能從多個 CPU 同時寫入呢？」（答：使用 `kfifo_in_spinlocked()` 或 per-CPU kfifo + 合併。）

---

## 五、Kernel 中使用 kfifo 的實際場景

| 子系統 | 用途 |
|--------|------|
| **drivers/usb/serial/** | USB 串口的收發緩衝區 |
| **drivers/tty/** | TTY 層的輸入/輸出緩衝 |
| **drivers/input/** | 輸入事件佇列（鍵盤、滑鼠事件排隊） |
| **net/sunrpc/** | RPC 請求佇列 |
| **drivers/media/** | 視訊串流的 frame buffer 管理 |
| **sound/core/** | ALSA 音訊驅動的 PCM 緩衝區 |

共同特徵：高頻率生產/消費、固定大小容忍溢出、需要最小延遲。

---

## 面試加分總結

1. **提到 kfifo 的 unsigned overflow 技巧**，展示對 C 語言未定義行為 vs. 定義行為的精確理解
2. **解釋 power-of-2 位元遮罩取代 modulo**，展示底層效能意識
3. **對比「兩個棧」vs.「環形緩衝區」的設計取捨**，展示從教學範例到產品系統的思維跨度
4. **討論 kfifo 的 SPSC 無鎖設計與記憶體屏障**，展示並行程式設計知識
5. **連結 workqueue 的 FIFO 語義與 ordered workqueue**，展示對 kernel 延遲執行機制的理解
6. **將「延遲轉移」抽象為 kernel 的 lazy evaluation 哲學**，展示系統層面的設計思維

---

## Sources

- [Linux kernel include/linux/kfifo.h (GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/kfifo.h)
- [Linux kernel lib/kfifo.c (GitHub)](https://github.com/torvalds/linux/blob/master/lib/kfifo.c)
- [Linux kernel kernel/workqueue.c (GitHub)](https://github.com/torvalds/linux/blob/master/kernel/workqueue.c)
- [Linux kernel include/linux/workqueue.h (GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/workqueue.h)
- [Kernel Documentation — Circular Buffers](https://docs.kernel.org/core-api/circular-buffers.html)
- [Kernel Documentation — Workqueue](https://docs.kernel.org/core-api/workqueue.html)
- [LWN — A Simple FIFO Implementation (kfifo)](https://lwn.net/Articles/101808/)
- [LWN — A New API for kfifo?](https://lwn.net/Articles/347619/)
- [LWN — Concurrency-managed Workqueues](https://lwn.net/Articles/355346/)
