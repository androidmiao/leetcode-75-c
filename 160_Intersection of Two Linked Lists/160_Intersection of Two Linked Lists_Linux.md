# 160. Intersection of Two Linked Lists — Linux Kernel 連結

## Linux 相關原始碼位置

| 檔案 | 角色 |
|------|------|
| `include/linux/list.h` | 核心雙向環狀鏈結串列 API（`struct list_head`、`list_for_each_entry` 等），是 Linux 中最廣泛使用的資料結構 |
| `include/linux/list.h` — `list_is_singular()` / `list_splice()` | 串列合併與判斷，涉及兩條串列的交互操作 |
| `net/core/skbuff.c` | Socket buffer 鏈結串列操作，sk_buff 在多個 queue 之間共享尾段（類似交叉串列的共享尾部結構） |
| `fs/dcache.c` | Dentry cache 中，多條路徑可能共享相同的 dentry 子樹，形成類似交叉串列的拓撲 |
| `kernel/pid.c` | PID namespace 層級結構中，不同 namespace 的 task list 可能匯聚到同一個 init process |
| `lib/list_sort.c` | 鏈結串列排序，使用 bottom-up merge sort，大量指標操作與本題的雙指標技巧同源 |

---

## Kernel vs. LeetCode 實作對照

| 維度 | LeetCode 160（Two Pointers） | Linux Kernel `list_head` |
|------|------------------------------|--------------------------|
| **串列結構** | 單向鏈結串列（`struct ListNode { val, *next }`） | 雙向環狀鏈結串列（`struct list_head { *next, *prev }`） |
| **交叉偵測方式** | 雙指標交叉走：pA 走完 A 跳到 B，pB 走完 B 跳到 A | Kernel 中不存在「交叉串列」的概念——`list_head` 是侵入式設計，一個物件只能屬於一條串列 |
| **共享結構** | 兩條串列共享尾部節點 | `sk_buff` 使用 `skb_shared_info` 實現 data buffer 共享，結構上類似交叉串列的共享尾段 |
| **指標重定向** | `pA = (pA == NULL) ? headB : pA->next` | `list_splice()` 將一條串列接入另一條；`list_move()` 將節點從一條串列移至另一條 |
| **環狀 vs. 終止** | 單向串列以 NULL 結尾；交叉走法保證有限步終止 | `list_head` 天生環狀，`list_for_each` 以回到 head 作為終止條件 |
| **時間複雜度** | O(M + N) | `list_for_each` 為 O(N)；串列合併 `list_splice` 為 O(1) |
| **空間複雜度** | O(1)——只用兩個指標 | O(1)——侵入式設計，不需額外記憶體配置 |

### 關鍵差異：侵入式 vs. 外部串列

Linux kernel 採用**侵入式鏈結串列**（intrusive linked list）：`list_head` 嵌入在宿主結構中，用 `container_of()` 巨集從 `list_head` 反推宿主結構位址。這種設計下，一個物件中嵌入多個 `list_head` 就能同時參與多條串列，而不像 LeetCode 題目中兩條串列透過共享相同節點來形成交叉。

```c
/* 侵入式設計——一個 task_struct 同時在多條串列上 */
struct task_struct {
    struct list_head tasks;      /* 全域 task list */
    struct list_head children;   /* 子行程串列 */
    struct list_head sibling;    /* 兄弟行程串列 */
    /* ... */
};
```

---

## 主管 Code Review 角度考題

### Q1：為什麼不用 hash set 而用 two pointers？

**期望答案方向：** O(1) 空間 vs. O(M) 空間。在 kernel 中，記憶體配置（`kmalloc`）是昂貴操作，且可能在中斷上下文中不允許 sleep，因此 O(1) 空間的演算法遠優於需要動態配置的方案。

**追問：** 如果在 interrupt context 中需要偵測兩條鏈結串列是否交叉，你能用 hash set 嗎？
- 不行，`kmalloc(GFP_KERNEL)` 在 interrupt context 中不允許 sleep；即使用 `GFP_ATOMIC` 也可能失敗。Two pointers 不需要任何記憶體配置，永遠安全。

**考查重點：** 記憶體配置意識、interrupt context 限制

---

### Q2：`pA = (pA == NULL) ? headB : pA->next;` 這一行有什麼潛在問題？

**期望答案方向：** 在 kernel 中存取指標前需考慮：
1. `pA->next` 是否可能指向已被釋放的記憶體（use-after-free）？
2. 是否需要 `rcu_dereference()` 來確保 memory ordering？
3. 併發情況下，另一個 CPU 是否可能同時修改串列結構？

**追問：** 如果要在 RCU 保護的串列上做類似操作，你需要改什麼？
- 需要用 `rcu_dereference(pA->next)` 取代直接的 `pA->next`，確保在 weakly-ordered 架構上正確讀取。

**考查重點：** RCU 意識、memory ordering、併發安全

---

### Q3：kernel 的 `list_head` 是環狀的，如果兩條環狀串列有共享節點，你的演算法還能用嗎？

**期望答案方向：** 不能直接用。環狀串列沒有 NULL 作為終止條件，交叉走法會無限迴圈。需要改用其他方法：
1. 先用 Floyd 的龜兔演算法偵測環
2. 計算各環的長度
3. 比較是否共享環上的節點

**追問：** Linux kernel 中 `list_for_each` 如何避免無限迴圈？
- 以 `pos != head` 作為終止條件（回到起始的哨兵節點）。

**考查重點：** 環狀結構意識、演算法限制分析

---

### Q4：如果串列非常長（數百萬節點），cache 效能如何？

**期望答案方向：**
- 鏈結串列天生對 cache 不友善：節點在記憶體中不連續，每次 `pA->next` 都可能 cache miss。
- Two pointers 方法走兩遍串列，第二遍時節點可能仍在 cache 中（取決於串列長度和 cache 大小）。
- Hash set 方法更糟：除了串列遍歷的 cache miss，hash table 本身也會造成隨機存取模式。
- Kernel 中的 `SLAB allocator` 將同型別的小物件集中配置，一定程度上改善鏈結串列的 cache locality。

**考查重點：** Cache locality 意識、SLAB allocator 知識

---

### Q5：這個解法是否 thread-safe？如何讓它在 kernel 中 thread-safe？

**期望答案方向：**
- 不是 thread-safe——如果另一個 thread 同時修改串列（插入、刪除節點），pA/pB 可能讀到不一致的狀態。
- Kernel 中的保護方式：
  1. **Spinlock**：`spin_lock(&list_lock)` — 適用於短暫的串列遍歷
  2. **RCU（Read-Copy-Update）**：讀取端幾乎零成本，適合讀多寫少的場景
  3. **Mutex**：適用於可能 sleep 的長時間操作

**考查重點：** 併發控制、鎖的選擇

---

## 面試加分總結

- **提及 O(1) 空間在 kernel 中的重要性：** interrupt context 禁止 sleep，動態記憶體配置可能失敗，O(1) 空間演算法在 kernel 中是強烈偏好。
- **連結到侵入式串列設計：** 說明 kernel 用 `list_head` + `container_of()` 的侵入式設計避免了 LeetCode 式的外部指標交叉，展現對 kernel 資料結構哲學的理解。
- **提及 RCU：** 在討論 thread-safety 時自然帶出 RCU，這是 kernel 中最重要的無鎖讀取機制。
- **提及 cache locality：** 鏈結串列的 cache 缺點是 kernel 開發者的常識，能說明 SLAB allocator 如何緩解更佳。
- **提及 sk_buff 的共享結構：** 網路子系統中 `sk_buff` 的 data sharing 是 kernel 中最接近「交叉串列」的真實案例。

---

## Sources

- [include/linux/list.h](https://github.com/torvalds/linux/blob/master/include/linux/list.h) — 核心鏈結串列 API
- [lib/list_sort.c](https://github.com/torvalds/linux/blob/master/lib/list_sort.c) — 鏈結串列排序實作
- [net/core/skbuff.c](https://github.com/torvalds/linux/blob/master/net/core/skbuff.c) — Socket buffer 串列操作
- [include/linux/rculist.h](https://github.com/torvalds/linux/blob/master/include/linux/rculist.h) — RCU 保護的鏈結串列操作
- [LWN: Intrusive linked lists](https://lwn.net/Articles/336255/) — 侵入式鏈結串列設計理念
