# 692. Top K Frequent Words — Linux Kernel 連結

> Last verified (2026-04-01).

本題用到兩個核心資料結構：**hash table**（頻率統計）和 **min heap**（維護 top-k）。Linux kernel 對這兩者都有生產等級的實作，以下對照分析。

---

## Linux 相關原始碼位置

### Hash Table 相關

| 檔案 | 角色 |
|---|---|
| `include/linux/hashtable.h` | Kernel 通用 hash table 巨集，基於固定大小的 bucket 陣列 + `hlist`（雙向鏈結串列），提供 `hash_add()`、`hash_for_each_possible()` 等操作 |
| `include/linux/jhash.h` | Jenkins hash（`jhash_1word`、`jhash_2words`、`jhash`）— kernel 最常用的整數與位元組序列 hash function |
| `include/linux/hash.h` | 乘法 hash `hash_long()` / `hash_32()` — 用 golden ratio 常數做快速整數 hash |
| `lib/rhashtable.c` | Resizable hash table — 支援動態擴縮容量、RCU 讀取、自動 rehash，用於 netfilter conntrack 等高流量場景 |
| `include/linux/stringhash.h` | 字串專用 hash（`hashlen_string()`、`full_name_hash()`）— 在 dcache（目錄快取）中對檔名做 hash |

### Heap / Priority Queue 相關

| 檔案 | 角色 |
|---|---|
| `lib/prio_heap.c` | 固定容量的 min heap — 最多容納 `max` 個元素，滿了之後新元素只有比 heap top 更好才會替換。這正好對應本題 size-k min heap 的邏輯 |
| `include/linux/prio_heap.h` | `struct ptr_heap` 的定義：`void **ptrs`（指標陣列）、`int max`（最大容量）、`int size`（目前大小），搭配使用者自訂 comparator |
| `lib/min_heap.c` / `include/linux/min_heap.h` | 較新的泛用 min heap（5.x+ 核心），用 `DEFINE_MIN_HEAP()` 巨集宣告，支援 `min_heap_push()`、`min_heap_pop()`、`min_heap_peek()`。perf subsystem 用它管理 hardware event 優先級 |

---

## Kernel vs. LeetCode 實作對照

### Hash Table 對照

| 面向 | LeetCode 692 C 實作 | Linux `hashtable.h` |
|---|---|---|
| Bucket 數量 | `#define HASH_BUCKETS 1024`（固定） | `DEFINE_HASHTABLE(name, bits)` — bucket 數量 = `1 << bits`，同樣固定大小 |
| Hash function | djb2（`hash * 33 + char`） | `jhash()` 或 `hash_long()`；字串場景用 `full_name_hash()` |
| Collision 處理 | Separate chaining（自訂 `HashNode` 單向鏈結） | `hlist_head` / `hlist_node` 雙向鏈結（`hlist` 是省記憶體的雙向 list，head 只有一個指標） |
| 遍歷 | 手動 `while (current != NULL)` | `hash_for_each_possible()` 巨集，展開後邏輯一樣 |
| 動態 resize | 無 | `rhashtable` 支援；基本 `hashtable.h` 不支援 |

**重點差異**：kernel 用 `hlist`（head 只有 `first` 指標、node 有 `next` 和 `pprev`）而非標準雙向 list，因為 hash bucket 數量龐大時，每個 bucket 省一個指標可以節省大量記憶體。面試時若被問到 "kernel 的 hash table 和你寫的有什麼不同"，這是最值得提的點。

### Min Heap 對照

| 面向 | LeetCode 692 C 實作 | Linux `lib/prio_heap.c` |
|---|---|---|
| 資料型別 | `WordEntry**`（指標陣列） | `void **ptrs`（通用指標陣列） |
| 容量上限 | `k`（由函式參數決定） | `heap->max`（建立時指定） |
| Comparator | 寫死在 `compareForHeap()` 裡 | 由呼叫者傳入的 function pointer `int (*gt)(void *, void *)` |
| 插入邏輯 | 未滿 → 放尾巴 + sift up；滿了 → 和 root 比，更好才替換 + sift down | 完全相同模式：`heap_insert()` 先檢查是否已滿，滿了就和 `ptrs[0]` 比 |
| Sift 方向 | `siftUp` + `siftDown` 分開寫 | 只有 sift up（因為替換 root 後直接做 sift up 從 index 0 開始，等效 sift down） |
| 使用場景 | 找 top-k 頻率單字 | kernel tracing（`lib/prio_heap.c` 最初為 LTTng tracer 寫的）、perf event 管理 |

**重點差異**：kernel 的 `prio_heap.c` 在替換 root 後用的是 sift up（從 index 0 開始往下找較大子節點交換），邏輯上等同本題的 `siftDown()`，只是命名習慣不同。面試時可以說明你知道有這兩種寫法。

---

## 主管 Code Review 角度考題

### Q1：為什麼用 djb2 而不是 kernel 慣用的 jhash？

**期望方向**：djb2 夠簡單、好記、面試白板上秒寫；但 jhash 的 avalanche 特性更好，collision 更少。實務上 bucket 數量只有 1024，collision 率已經可接受。

**追問**：如果 wordsSize 增到 10^6，你會怎麼調 HASH_BUCKETS？

**考察重點**：load factor 意識、rehash 成本、是否知道 power-of-2 bucket + bitwise AND 取代 modulo。

### Q2：你的 hash table 用單向鏈結，kernel 用 hlist（雙向），差在哪？

**期望方向**：hlist 的 `pprev`（指向前一個節點的 next 指標的指標）讓 O(1) 刪除不需要掃鏈。本題只需要查找和插入，所以單向就夠了。

**追問**：如果要支援刪除特定 word，你的結構要怎麼改？

**考察重點**：pointer-to-pointer 技巧、是否知道 kernel 的 `hlist_del()` 為什麼能 O(1)。

### Q3：heap 的 comparator 可以抽出來用 function pointer 嗎？

**期望方向**：可以，kernel 的 `prio_heap` 和新版 `min_heap` 都用 function pointer / struct callback。這樣同一套 heap 邏輯能復用在不同排序需求。

**追問**：function pointer 在 hot path 上對 cache 有什麼影響？

**考察重點**：indirect call vs. inline 的 icache / branch prediction 代價；kernel 傾向用 `likely()`/`unlikely()` 與 static key 做優化。

### Q4：為什麼 heap 替換 root 後你用 siftDown 而不是 siftUp？

**期望方向**：替換 root 後只有 root 可能違反 heap 性質，往下修正是 O(log k)；siftUp 是從葉節點往上修正，用在插入新元素到尾巴的情境。kernel 的 `prio_heap.c` 把這兩個操作統一成一個函式，但邏輯上做的事相同。

**考察重點**：是否真正理解 heap 的兩個方向修正各自的適用場景。

### Q5：如果這段 code 要放進 kernel module，你需要改什麼？

**期望方向**：`malloc` → `kmalloc` / `kzalloc`，加上 `GFP_KERNEL` 或 `GFP_ATOMIC` flag；`free` → `kfree`；`strcmp` → kernel 已有；不能用 `<stdlib.h>`，要用 `<linux/slab.h>` 和 `<linux/string.h>`。

**追問**：如果在 interrupt context 裡跑，GFP flag 要怎麼選？

**考察重點**：memory allocation context 意識，是否知道 `GFP_ATOMIC` 不能睡眠。

---

## 面試加分總結

- **Hash table**：先講自己的 djb2 + separate chaining 方案，再主動提到 kernel 用 `jhash` + `hlist`，並解釋 `hlist` 為什麼對 bucket-heavy 場景省記憶體 — 展現你理解 production-grade 的取捨。
- **Min heap**：指出 kernel 的 `lib/prio_heap.c` 做的事和你的 `pushTopKHeap()` 幾乎一樣（固定容量、和 root 比較、更好才替換），說明你的寫法不是臨時想的，而是有 kernel 先例。
- **Comparator 抽象**：提到 kernel 用 function pointer 讓 heap 通用化，而你在面試場景為了可讀性選擇寫死 — 展現你知道兩種做法的 tradeoff。
- **Memory allocation**：如果面試官追問 kernel 適配，能快速說出 `kmalloc` / `kfree` / GFP flag — 展現系統程式經驗。

---

## Sources

- [include/linux/hashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h)
- [include/linux/jhash.h](https://github.com/torvalds/linux/blob/master/include/linux/jhash.h)
- [include/linux/hash.h](https://github.com/torvalds/linux/blob/master/include/linux/hash.h)
- [include/linux/stringhash.h](https://github.com/torvalds/linux/blob/master/include/linux/stringhash.h)
- [lib/rhashtable.c](https://github.com/torvalds/linux/blob/master/lib/rhashtable.c)
- [lib/prio_heap.c](https://github.com/torvalds/linux/blob/master/lib/prio_heap.c)
- [include/linux/prio_heap.h](https://github.com/torvalds/linux/blob/master/include/linux/prio_heap.h)
- [include/linux/min_heap.h](https://github.com/torvalds/linux/blob/master/include/linux/min_heap.h)
