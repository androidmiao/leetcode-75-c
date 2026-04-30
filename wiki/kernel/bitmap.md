# bitmap 位元圖

## 核心概念

Linux kernel 的 `bitmap` 是一塊「以位元為單位」的緊湊狀態陣列，常用來追蹤資源占用（CPU、IRQ vector、DMA channel、SCMI token、IPC channel、PID）。對外觀念上等同 `vector<bool>`；對內以 `unsigned long[]` 配合一系列 `find_first_zero_bit` / `set_bit` / `test_bit` API 操作。它的價值在於：**O(1) 占用、findfirst-zero 在現代 CPU 上是一條 instruction（`bsf` / `clz`）**。

### 一句話判斷

> 你需要管理一個固定大小的「資源池 / 編號空間 / 旗標集合」、查詢與分配都要 O(1)、占用空間 = N/8 bytes → 選 bitmap。其他狀態結構通常更貴。

---

## 基本宣告

```c
#include <linux/bitmap.h>
#include <linux/bitops.h>

#define IPI_NR_CH      65                          // 通道總數

DECLARE_BITMAP(busy, IPI_NR_CH);                   // 編譯期：unsigned long busy[ROUND_UP(65,64)/64]
                                                   //         = unsigned long busy[2]

bitmap_zero(busy, IPI_NR_CH);                      // 全 0 初始化

unsigned long *busy_dyn;
busy_dyn = bitmap_zalloc(N, GFP_KERNEL);           // 動態：N bits，alloc + zero
                                                   //   失敗回 NULL
                                                   //   釋放：bitmap_free(busy_dyn)

busy_dyn = devm_bitmap_zalloc(dev, N, GFP_KERNEL); // device-managed 版本，driver detach 自動釋放
```

`DECLARE_BITMAP(name, nbits)` 是 macro，展開為 `unsigned long name[BITS_TO_LONGS(nbits)]`，stack / static / struct 內都能塞。

---

## 三組核心 API

### 1. 單一 bit 操作（atomic 版本）

```c
set_bit(idx, bm);                                  // bm[idx/64] |= 1UL << (idx%64) (atomic, 含 barrier)
clear_bit(idx, bm);                                // bm[idx/64] &= ~(1UL << (idx%64)) (atomic)
change_bit(idx, bm);                               // toggle (atomic)
test_bit(idx, bm);                                 // read (non-atomic 但 READ_ONCE)

bool old = test_and_set_bit(idx, bm);              // atomic：set + return 原值
bool old = test_and_clear_bit(idx, bm);            // 同樣 atomic compare-and-swap

__set_bit(idx, bm);                                // non-atomic 版（你已握鎖時用）
__clear_bit(idx, bm);                              // 速度更快
```

「atomic 版」相對 spinlock 的成本：通常一條 `LOCK BTSL` (x86) 或 `LDXR/STXR` 對 (ARM)。`test_and_set_bit` 直接拿來當 try-lock 也很常見。

### 2. 批次掃描（找空位 / 找占用 / 算數量）

```c
unsigned long bit;

bit = find_first_zero_bit(bm, nbits);              // 找第一個 0 → 分配空位
                                                    //   找不到回 nbits

bit = find_first_bit(bm, nbits);                   // 找第一個 1 → 找占用
bit = find_next_zero_bit(bm, nbits, start);        // 從 start 開始找下一個 0
bit = find_next_bit(bm, nbits, start);             // 從 start 開始找下一個 1

unsigned int n = bitmap_weight(bm, nbits);         // popcount，現代 CPU 有 `popcnt` 指令

/* 標準走訪：拜訪所有「占用」的 bit */
for_each_set_bit(bit, bm, nbits) {
    handle(bit);
}
for_each_clear_bit(bit, bm, nbits) {
    free_resources(bit);
}
```

### 3. 整段操作（bitmap-as-set）

```c
bitmap_zero(dst, nbits);                            // dst = ∅
bitmap_fill(dst, nbits);                            // dst = U
bitmap_copy(dst, src, nbits);                       // dst = src

bitmap_and(dst, a, b, nbits);                       // dst = a ∩ b
bitmap_or(dst, a, b, nbits);                        // dst = a ∪ b
bitmap_xor(dst, a, b, nbits);                       // dst = a △ b
bitmap_andnot(dst, a, b, nbits);                    // dst = a \ b
bitmap_complement(dst, src, nbits);                 // dst = ¬src

bool eq = bitmap_equal(a, b, nbits);
bool subset = bitmap_subset(a, b, nbits);           // a ⊆ b
bool empty = bitmap_empty(bm, nbits);
```

`cpumask_t` / `nodemask_t` 都是 bitmap 的 typedef，所有 `cpumask_*` API 底層都直接 dispatch 到 `bitmap_*`。

---

## 三個 idiomatic 用法

### 用法 1：固定數量資源池（IPC channel / DMA slot）

```c
struct chan_pool {
    spinlock_t      lock;
    DECLARE_BITMAP(busy, NR_CH);
};

static struct chan_pool pool;

int alloc_channel(void)
{
    unsigned long flags;
    int ch;

    spin_lock_irqsave(&pool.lock, flags);
    ch = find_first_zero_bit(pool.busy, NR_CH);
    if (ch >= NR_CH) {
        spin_unlock_irqrestore(&pool.lock, flags);
        return -EBUSY;
    }
    set_bit(ch, pool.busy);                          // 因為已握 lock，這裡其實可以 __set_bit
    spin_unlock_irqrestore(&pool.lock, flags);
    return ch;
}

void free_channel(int ch)
{
    unsigned long flags;
    spin_lock_irqsave(&pool.lock, flags);
    __clear_bit(ch, pool.busy);
    spin_unlock_irqrestore(&pool.lock, flags);
}
```

正是 `[[interview/google_code_review_semaphore_prep_linux_solution]]` 中 DSP IPI channel pool 的「不要用 semaphore，改 bitmap + spinlock」修法。

### 用法 2：lock-free try-claim（不需 spinlock）

```c
int try_claim(int idx)
{
    if (test_and_set_bit(idx, busy))                // atomic：true 表「之前已被占」
        return -EBUSY;
    return 0;
}

void release(int idx)
{
    clear_bit(idx, busy);                            // atomic
}
```

當「分配的 idx 是 caller 自己決定的、衝突就退出」時，純 atomic bit op 足夠，不必 spinlock。Mailbox token / interrupt vector 申請常用這個模式。

### 用法 3：cpumask 子集運算

```c
cpumask_var_t targets;

if (!alloc_cpumask_var(&targets, GFP_KERNEL))
    return -ENOMEM;

cpumask_and(targets, cpu_online_mask, &task->cpus_allowed);
for_each_cpu(cpu, targets) {
    smp_call_function_single(cpu, func, info, false);
}

free_cpumask_var(targets);
```

`cpu_online_mask` / `cpu_present_mask` / `cpu_active_mask` 都是 bitmap，全 kernel 共享。任何 CPU hotplug、IPI、cpufreq 路徑都離不開它。

---

## 三大陷阱

### 陷阱 1：`__set_bit` 在 SMP 下漏 atomic

雙底線版本不含 LOCK 前綴，多 CPU 併發 `__set_bit` 同一個 word 內不同 bit 會導致 lost update。**只有在你已握 lock 或在 atomic context 互斥時才能用**。

### 陷阱 2：`find_first_zero_bit` 找不到時回 `nbits`

```c
ch = find_first_zero_bit(busy, NR_CH);
if (ch < NR_CH)              // ✅ 正確判斷
    set_bit(ch, busy);
```

不是回 `-1`、不是 `~0UL`，**就是 nbits 本身**。漏判邊界會把第 `NR_CH` 個 bit 寫進相鄰記憶體（OOB write）。

### 陷阱 3：bitmap 大小與 BITS_TO_LONGS 不對齊

```c
DECLARE_BITMAP(bm, 65);                              // 內部 = unsigned long[2]，共 128 bits
for_each_set_bit(bit, bm, 128) ...                   // ❌ 拿錯 nbits，會掃到 padding bits
for_each_set_bit(bit, bm, 65)  ...                   // ✅ 一定要傳邏輯上的 nbits
```

所有 API 都以「邏輯 nbits」為界，padding 部分是 unspecified。傳錯邊界會讓 `bitmap_weight` / `find_first_bit` 拿到垃圾結果。

---

## 與其他狀態結構的對比

| 結構 | 空間 | 查 / 改 | 場景 |
|------|------|--------|------|
| **bitmap** | N/8 bytes | O(1)（單 bit）/ O(N/64)（掃描） | 固定大小池、cpumask、IRQ vector |
| `IDR` / `XArray` | O(N) ~ O(N log N) | O(log N) | 動態 ID 對應 pointer（fd → file、tag → req） |
| hash table | O(buckets+items) | O(1) avg | 任意 key、open-ended 集合 |
| sorted array | O(N) | O(log N) 查 / O(N) 改 | 罕見變動的小集合 |
| `atomic_long_t` flag | O(1) | O(1) | 1–64 bit 旗標的小集合 |

**判斷口訣**：「資源編號是緊湊小整數」→ bitmap；「資源編號是 sparse / 任意 key」→ IDR / hashtable。

---

## 實戰：在 kernel 源碼中的定位

### 典型用例

- **`include/linux/cpumask.h`** — `cpumask_t` 是 `DECLARE_BITMAP(bits, NR_CPUS)`；CPU hotplug、scheduler、IPI 全靠它。
- **`kernel/irq/irqdesc.c` + `kernel/irq/matrix.c`** — IRQ vector 分配（x86 APIC）用 bitmap matrix 管 per-CPU vector。
- **`drivers/firmware/arm_scmi/`** — SCMI protocol 的 token allocator 用 bitmap 管 0–MAX_TOKEN 的轉發 ID。
- **`mm/page_alloc.c`** — page block migrate type、free area bitmap。
- **`lib/bitmap.c` + `include/linux/bitops.h`** — 所有 API 實作；架構特定的 `find_first_*` / `bsf` / `ctz` intrinsic 在 `arch/*/include/asm/bitops.h`。

### 與 LeetCode pattern 的呼應

- **`[[patterns/bit-manipulation]]`** — 單一 `int` / `long` 的位元操作，是 bitmap 的「N=64」特例。
- **集合表達**：類似 LeetCode 78 子集、137 出現一次的數字、201 區間 AND——把 32 / 64 個元素壓進一個 word。

---

## 相關 wiki

- `[[patterns/bit-manipulation]]` — 單字 bit op 基礎；bitmap 是其「多字」推廣
- `[[kernel/spinlock]]` — bitmap 多 bit 操作通常用 spinlock 保護；單 bit 用 atomic op
- `[[kernel/memory-management]]` — `bitmap_alloc` / `bitmap_zalloc` / `devm_bitmap_zalloc` 配對
- `[[interview/google_code_review_semaphore_prep_linux_solution]]` — DSP IPI channel pool 用 bitmap 取代 semaphore 的完整解
- `[[interview/google_code_review_semaphore_prep_linux_primitives]]` — 5 大原語對照中 bitmap-as-pool 的位置
- `[[interview/mcupm-code-review]]` — Q1 mailbox channel index 邊界檢查、Q5 ackdata race 都觸及 bitmap pattern

---

## 面試速記（背誦版）

1. `DECLARE_BITMAP(name, nbits)` = `unsigned long name[BITS_TO_LONGS(nbits)]`。
2. `find_first_zero_bit` / `find_first_bit` 找不到回 `nbits`，**不是 -1**。
3. `set_bit` / `clear_bit` 是 atomic；`__set_bit` / `__clear_bit` 不是。
4. `test_and_set_bit` 可以直接當 try-lock。
5. 走訪只用 `for_each_set_bit` / `for_each_clear_bit`，不要手寫 `i++`。
6. cpumask 是 bitmap 的 typedef，所有 `cpumask_*` 等價於 `bitmap_*`。
7. 適用：固定大小資源池、CPU 集合、旗標群；不適用：sparse key、需要伴隨 metadata（→ IDR / XArray）。
