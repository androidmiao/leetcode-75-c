# LeetCode 2095: Delete the Middle Node of a Linked List — Linux 核心觀點

## 1. Linux 相關原始碼位置

Linux 核心使用鏈表的地方甚多，以下是最相關的文件：

| 檔案位置 | 描述 |
|---------|------|
| `include/linux/list.h` | 核心雙向迴圈鏈表實作，提供 `list_add()`, `list_del()`, `list_for_each()` 等巨集 |
| `lib/list_sort.c` | 鏈表排序實作，使用指標走訪和分割合併演算法，類似快慢指標的模式 |
| `kernel/sched/core.c` | 排程器中大量使用鏈表管理行程隊列，涉及指標追蹤與節點操作 |
| `fs/dcache.c` | 目錄項目快取（dentry）使用鏈表，涉及指標追蹤與刪除操作 |
| `mm/page_alloc.c` | 頁面配置器使用鏈表，涉及複雜的指標操作與記憶體管理 |

LeetCode 題目中使用快慢指標定位中間節點的模式，在核心中也可見於**環形檢測** (`kernel/rcu.c` 中的 RCU 走訪驗證) 與**平衡演算法**中。

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 資料結構差異

**LeetCode 單向鏈表：**
```c
struct ListNode {
    int val;
    struct ListNode *next;
};
```

**Linux Kernel 雙向迴圈鏈表：**
```c
struct list_head {
    struct list_head *next;
    struct list_head *prev;
};
```

### 2.2 刪除節點的方式

| 面向 | LeetCode 單向 | Kernel 雙向 |
|-----|-------------|-----------|
| **時間複雜度** | O(n)：需找到前驅節點才能刪除 | O(1)：直接刪除任意節點 |
| **指標操作** | `prev->next = current->next` | `list_del(entry)` 內部自動更新 |
| **前驅追蹤** | 必須手動追蹤前一個節點 | `prev` 指標已內建 |
| **記憶體釋放** | 使用 `free()` (用戶空間) | 使用 `kfree()` (核心) |

### 2.3 快慢指標模式

**LeetCode 實作：**
```c
struct ListNode *slow = head, *fast = head;
struct ListNode *prev = NULL;

while (fast && fast->next) {
    prev = slow;
    slow = slow->next;
    fast = fast->next->next;
}
```

**Kernel 應用場景：**
- **環形檢測** (`kernel/rcu.c`)：偵測迴圈鏈表中的異常
- **中點查詢** (`lib/list_sort.c`)：分割鏈表進行合併排序

### 2.4 走訪與修改安全性

**Kernel 提供的安全走訪巨集：**
```c
// 標準走訪（不允許修改）
list_for_each(pos, head) { ... }

// 安全走訪（允許刪除當前節點）
list_for_each_safe(pos, n, head) {
    list_del(pos);
}

// RCU 保護的走訪（用於並行讀取）
list_for_each_entry_rcu(entry, head, list) { ... }
```

---

## 3. 主管 Code Review 角度考題

### 3.1 設計決策

**Q1: 為何不採用雙向鏈表？**

A:
- **LeetCode 場景**：題目明確指定單向鏈表，刻意測試指標操作與快慢指標技巧
- **Kernel 選擇**：雙向鏈表在頻繁刪除場景下更高效（O(1) vs O(n)），但佔用更多記憶體

**Q2: 刪除後的記憶體管理？**

A:
```c
// 用戶空間（LeetCode）
struct ListNode *temp = slow;
prev->next = slow->next;
free(temp);  // 必須釋放

// Kernel 空間
struct entry *victim = list_entry(ptr, struct entry, list);
list_del(&victim->list);
kfree(victim);  // 使用 kfree
```

**Q3: 邊界情況處理？**

A:
- **單節點鏈表**：fast 和 slow 同時走，無法取得有效的前驅
- **雙節點鏈表**：slow 指向第二個節點，需刪除最後一個
- **Kernel 防護**：使用 `list_empty()` 巨集預檢

### 3.2 並行存取安全

**Q4: 如何在多線程環境下安全地刪除中間節點？**

A: Kernel 提供的三種機制：

1. **自旋鎖（Spinlock）：**
```c
struct list_head mylist;
spinlock_t list_lock;

spin_lock(&list_lock);
list_del(&entry->list);
spin_unlock(&list_lock);
```

2. **讀寫鎖（RW-Lock）：**
```c
struct rw_semaphore list_sem;

down_write(&list_sem);
list_del(&entry->list);
up_write(&list_sem);
```

3. **RCU (Read-Copy-Update)：**
```c
rcu_assign_pointer(entry->next, new_next);
synchronize_rcu();
kfree(entry);  // 延遲釋放
```

### 3.3 快取局部性與效能

**Q5: 鏈表走訪的快取效能問題？**

A:
- **快慢指標走訪**：O(n) 時間，但指標跳躍導致 **L1 快取未命中率高**
- **Kernel 最佳化**：
  - 使用 `prefetch()` 巨集預取下一個節點：`prefetch(entry->next)`
  - 採用 `list_sort.c` 中的分割策略減少走訪深度
  - 考慮用陣列 + 索引替代鏈表（若存取模式確定）

### 3.4 走訪期間的結構修改

**Q6: 如果鏈表在快慢指標走訪時被其他線程修改怎麼辦？**

A:
- **LeetCode**：假設單線程，無需考慮
- **Kernel 實務**：
  - 走訪前加鎖，防止其他線程修改
  - 使用 `list_for_each_safe()` 只能防止當前節點刪除，不防止結構改變
  - RCU 可允許讀者無鎖，但寫者仍需同步

```c
// 不安全：可能遍歷不完整鏈表
list_for_each_entry(entry, head, list) {
    if (condition(entry))
        list_del(&entry->list);  // 可能跳過節點
}

// 安全做法
struct list_head *safe_pos;
list_for_each_safe(pos, safe_pos, head) {
    // 仍需加鎖保護鏈表結構
    list_del(pos);
}
```

---

## 4. 面試加分總結

### 4.1 技術亮點

✅ **指標操作精度**
- 正確追蹤前驅節點，避免懸垂指標
- 理解快慢指標原理與應用場景

✅ **核心知識遷移**
- 知道 Kernel 中雙向鏈表如何實現 O(1) 刪除
- 能對比單向 vs 雙向鏈表的權衡

✅ **並行程式設計**
- 提及多線程環境下的鎖定策略
- 認識 RCU 與無鎖演算法的優勢

✅ **記憶體管理**
- 準確釋放被刪除的節點
- 理解用戶空間 `free()` vs 核心 `kfree()`

### 4.2 進階擴展點

1. **時間複雜度優化**
   - 提出若改用雙向鏈表或陣列，如何達成 O(1) 或 O(n log n)
   - 討論快取局部性對實際效能的影響

2. **並行演算法**
   - 實現鎖自由（lock-free）的節點刪除（使用 CAS）
   - 利用 RCU 實現高效讀多寫少場景

3. **核心經驗**
   - 引用 `include/linux/list.h` 的 `container_of` 巨集與通用資料結構設計
   - 提及 `list_sort.c` 如何結合快慢指標與分割合併

### 4.3 面試對答範例

**Interviewer:** "如果要在 Linux 核心中實現這個功能，你會怎麼做？"

**回答框架：**
> "在核心中，我會使用雙向迴圈鏈表 `list_head`，藉由內建的 `prev` 指標實現 O(1) 刪除。走訪時使用 `list_for_each_safe()` 防止迭代器失效。並發存取則透過自旋鎖或 RCU 保護。若場景是讀多寫少，RCU 會提供更優的效能。最後，刪除後務必呼叫 `kfree()` 釋放節點記憶體，防止核心記憶體洩漏。"

---

## 5. Sources

- **Linux Kernel List Implementation**
  https://github.com/torvalds/linux/blob/master/include/linux/list.h

- **Kernel List Sort (Merge Sort on Linked Lists)**
  https://github.com/torvalds/linux/blob/master/lib/list_sort.c

- **RCU (Read-Copy-Update) Documentation**
  https://www.kernel.org/doc/html/latest/RCU/

- **Kernel Memory Management - kfree()**
  https://github.com/torvalds/linux/blob/master/mm/slab.c

- **Spinlock & Synchronization Primitives**
  https://www.kernel.org/doc/html/latest/kernel-hacking/locking.html

- **LeetCode 2095 Problem**
  https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

---

**文件生成日期：** 2026-04-01
**適用對象：** 準備 Linux 核心系統工程師面試的候選人
**難度等級：** 中階～高階
