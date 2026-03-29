# 155. Min Stack — Linux Kernel 連結分析

## Linux 相關原始碼位置

| 檔案 | 說明 |
|------|------|
| `include/linux/list.h` | 核心雙向鏈結串列巨集，`list_head` 是 kernel 最基礎的資料結構 |
| `include/linux/llist.h` | Lock-less 單向鏈結串列 — 實質上是一個 LIFO 棧（push/pop 為 atomic） |
| `kernel/sched/core.c` | 排程器核心，維護每個 CPU 的 runqueue；`rt_rq` 中追蹤即時任務的最小 deadline |
| `kernel/sched/deadline.c` | SCHED_DEADLINE 排程類別，使用紅黑樹追蹤最早 deadline — 概念上類似「即時取得最小值」 |
| `include/linux/min_heap.h` | 通用最小堆實作（v5.13+），提供 `min_heap_push` / `min_heap_pop` / `min_heap_peek` |
| `lib/stackdepot.c` | Stack Depot — 壓縮儲存 call stack 的機制，用於 KASAN / KMSAN 等工具 |
| `arch/x86/kernel/irq_stack.c` | IRQ 棧管理 — 處理中斷時的棧切換邏輯 |

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode Min Stack | Linux Kernel 相關機制 |
|------|--------------------|-----------------------|
| **資料結構** | 兩個平行 `int[]` 陣列 | `llist_head` (lock-less stack)、per-CPU stack、irq stack |
| **「取得最小值」** | `min_stack[top]` — O(1) 直接讀取預計算值 | `rt_rq->rt_nr_migratable` / `dl_rq` 紅黑樹 leftmost — O(1) cached leftmost |
| **棧操作** | `top++` / `top--` — 索引移動 | `llist_add()` / `llist_del_first()` — CAS atomic 操作 |
| **記憶體管理** | `malloc` + `realloc` 動態擴容 | 固定大小棧（`THREAD_SIZE`，通常 8KB 或 16KB），絕不動態擴容 |
| **併發安全** | 無（單執行緒） | `llist` 用 `cmpxchg` 實現 lock-free；IRQ 棧用 per-CPU 避免共享 |
| **最小值追蹤策略** | 每層預計算並儲存 | 紅黑樹 `rb_leftmost` cached，或 min_heap `data[0]` |

### `llist` 作為 Lock-Free 棧

Linux 的 `llist_head` 本質上就是一個 LIFO 棧，使用 `cmpxchg`（Compare-And-Swap）實現無鎖的 push 和 pop：

```c
// include/linux/llist.h 簡化版
static inline bool llist_add(struct llist_node *new, struct llist_head *head)
{
    struct llist_node *first;
    do {
        new->next = first = READ_ONCE(head->first);
    } while (cmpxchg(&head->first, first, new) != first);
    return !first;  // 回傳棧是否之前為空
}
```

這和我們的 `minStackPush` 概念相同（新元素成為新的頂部），但 kernel 版本必須處理多 CPU 同時 push 的競爭情況。

### `min_heap` 與 Min Stack 的概念對比

Linux kernel 的 `min_heap`（`include/linux/min_heap.h`）提供 O(1) 的 `peek`（看最小值）和 O(log n) 的 `push`/`pop`。而 LeetCode 的 Min Stack 要求所有操作都是 O(1)，這只有在操作限制為 LIFO 順序時才可能。這是一個重要的 trade-off：

- **Min Stack（LIFO 限制）：** 所有操作 O(1)，但只能按 LIFO 順序移除
- **Min Heap（任意移除）：** peek O(1)，但 push/pop 是 O(log n)

### 排程器中的「即時最小值」

Linux 的 SCHED_DEADLINE 排程器需要快速找到最早 deadline 的任務（等同「取得最小值」）。它使用紅黑樹並 cache `leftmost` 節點，使得取得最小值為 O(1)。這與 Min Stack 的核心思想相同：**預先計算並快取最小值，而非每次查詢時重新計算。**

## 主管 Code Review 角度考題

### Q1: 為什麼你選擇 realloc 動態擴容？在 kernel 中這樣做安全嗎？

**預期回答方向：** Kernel 中棧大小是固定的（`THREAD_SIZE`），因為 `kmalloc`/`krealloc` 可能睡眠（`GFP_KERNEL`）或在中斷 context 中失敗。正確做法是在初始化時預分配固定大小，或使用 per-CPU 棧。LeetCode 的 `realloc` 在 userspace 可以接受，但 kernel 中應避免在 hot path 上做記憶體配置。

**真正在測試：** 對 kernel 記憶體配置限制的理解（能否在中斷/原子 context 中配置記憶體）。

### Q2: 如果多個執行緒同時操作這個 Min Stack，會發生什麼問題？怎麼修？

**預期回答方向：** 資料競爭 — `top++` 和陣列寫入不是 atomic。可以用 mutex 保護（類似 kernel 的 `spin_lock`），或改用 lock-free 設計（類似 `llist_head` 用 `cmpxchg`）。但 lock-free Min Stack 更複雜，因為 `min_stack[]` 的更新依賴前一層的值。

**真正在測試：** 併發程式設計意識、鎖粒度選擇。

### Q3: `min_stack[]` 陣列永遠和 `data[]` 一樣大，有沒有辦法節省空間？

**預期回答方向：** Editorial Approach 2/3 — 只在新最小值出現時才推入追蹤棧，可以節省空間。Approach 3 更進一步用 `(value, count)` 配對處理重複最小值。但在最壞情況（嚴格遞減序列）下，空間仍是 O(n)。

**真正在測試：** 是否理解空間優化的 trade-off，以及對題解方法的熟悉程度。

### Q4: kernel 的 stack depot 是什麼？為什麼它不用傳統棧結構？

**預期回答方向：** Stack Depot（`lib/stackdepot.c`）用於壓縮儲存大量 call stack（用於 KASAN、KMSAN 等記憶體偵錯工具）。它不用傳統棧因為目標是去重 — 多個相同的 call stack 只儲存一份，透過 hash table 索引。這展示了「棧」作為概念 vs. 作為資料結構的區別。

**真正在測試：** 對 kernel 偵錯基礎設施的廣度認識。

### Q5: 如果棧的容量上限已知（如 `3 * 10^4`），你的設計會怎麼改？

**預期回答方向：** 直接靜態配置 `int data[30001]` 和 `int min_stack[30001]`，省去 `malloc`/`realloc`。在 kernel 中這更安全（避免動態配置），在 LeetCode 中也更快（避免 `realloc` 的複製成本）。但會浪費未使用的記憶體。

**真正在測試：** 靜態 vs. 動態配置的 trade-off 判斷。

## 面試加分總結

- 提到 kernel 的 `llist_head` 就是一個 lock-free LIFO 棧，展示你了解無鎖資料結構。
- 將 Min Stack 的「預計算最小值」與排程器的 `rb_leftmost` cache 做類比，展示你理解「以空間換時間預快取」的通用模式。
- 解釋為什麼 kernel 棧是固定大小（`THREAD_SIZE`）而非動態擴容 — 中斷 context 中不能做記憶體配置。
- 討論 `min_heap.h` 與 Min Stack 的 trade-off：LIFO 限制換來所有操作 O(1)，vs. 任意順序移除但 push/pop O(log n)。
- 提到 stack depot 作為「棧壓縮儲存」的實際應用，展示對 kernel 偵錯工具的認識。

## Sources

- [include/linux/llist.h](https://github.com/torvalds/linux/blob/master/include/linux/llist.h)
- [include/linux/min_heap.h](https://github.com/torvalds/linux/blob/master/include/linux/min_heap.h)
- [lib/stackdepot.c](https://github.com/torvalds/linux/blob/master/lib/stackdepot.c)
- [kernel/sched/deadline.c](https://github.com/torvalds/linux/blob/master/kernel/sched/deadline.c)
- [arch/x86/kernel/irq_stack.c](https://github.com/torvalds/linux/blob/master/arch/x86/kernel/irq_stack.c)
