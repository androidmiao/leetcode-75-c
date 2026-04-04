# list_head 雙向環形鏈結串列

## 核心概念

Linux kernel 的 `list_head` 是一個極為優雅的侵入式（intrusive）雙向環形鏈結串列實現，與 LeetCode 常見的外部鏈結串列設計有根本差異。

### 為何稱為「侵入式」？

- **外部設計（External）**：LeetCode 中的 ListNode 封裝資料與指標
  ```c
  struct ListNode {
    int val;
    struct ListNode *next;
  };
  ```

- **侵入式設計（Intrusive）**：內核將鏈結嵌入資料結構
  ```c
  struct list_head {
    struct list_head *next;
    struct list_head *prev;
  };

  struct my_object {
    int id;
    char name[32];
    struct list_head node;  // 嵌入鏈結
  };
  ```

優勢：
- 無需額外記憶體配置鏈結節點
- 同一物件可參與多個鏈結
- 快取友善（cache-friendly）
- 零指標追蹤成本

### 環形 vs 線性終止

- **LeetCode 線性**：NULL 作為終止標記
- **Kernel 環形**：最後一個節點指向頭，頭的前驅指向最後一個節點

```c
struct list_head {
  struct list_head *next;
  struct list_head *prev;
};

// 初始化：自指（空表）
list_head.next = &list_head;
list_head.prev = &list_head;
```

優勢：
- 無需檢查 NULL
- 遍歷代碼統一簡潔
- 哨兵節點天然實現
- 環形屬性在某些算法中自然表達

## 核心 API

### list_add(new, head)
在 head 後方插入新節點（不一定是鏈表末尾）。

```c
static inline void list_add(struct list_head *new, struct list_head *head)
{
  __list_add(new, head, head->next);
}

static inline void __list_add(struct list_head *new,
                               struct list_head *prev,
                               struct list_head *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}
```

時間複雜度：O(1)

### list_del(entry)
移除某個節點。

```c
static inline void list_del(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
}

static inline void __list_del(struct list_head *prev,
                               struct list_head *next)
{
  next->prev = prev;
  prev->next = next;
}
```

時間複雜度：O(1)

### list_entry(ptr, type, member)
從 list_head 指標取得包含它的物件指標（container_of 的別名）。

```c
#define list_entry(ptr, type, member) \
  container_of(ptr, type, member)
```

### container_of(ptr, type, member)
給定結構成員指標，計算包含結構的指標。

```c
#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr) - offsetof(type, member)))
```

關鍵：
- 編譯時已知 member 偏移量
- 指標運算無成本
- 常量表達式優化

### list_for_each(pos, head)
遍歷鏈表中所有節點。

```c
#define list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head); pos = pos->next)
```

### list_for_each_safe(pos, n, head)
安全遍歷（允許遍歷中刪除）。

```c
#define list_for_each_safe(pos, n, head) \
  for (pos = (head)->next, n = pos->next; pos != (head); \
       pos = n, n = pos->next)
```

## 與 LeetCode 鏈表問題的對應

### 差異分析

| 面向 | LeetCode | Kernel list_head |
|------|----------|-------------------|
| 結構 | 內含資料 | 內嵌於資料結構 |
| 終止 | NULL | 環形自指 |
| 方向性 | 單向或雙向 | 永遠雙向 |
| 長度查詢 | 通常需遍歷 | 通常需遍歷 |
| 插入/刪除 | O(1)，需引用 | O(1)，需引用 |

### 相關 LeetCode 問題

- **206 反轉鏈表**：單純的反轉邏輯，list_head 中因已雙向，反轉較為平凡
- **141 環形鏈表**：環形檢測，kernel 中環形是標準設計，不是異常
- **160 相交鏈表**：指標比較邏輯相同
- **21 合併兩個排序鏈表**：遍歷合併，API 不同但邏輯相同
- **23 合併 K 個排序鏈表**：分治或堆策略相同
- **876 鏈表中點**：快慢指標技巧相同
- **2095 刪除鏈表中間的節點**：O(1) 刪除，list_del 更直接
- **2130 鏈表最大孿生和**：遍歷求值，邏輯相同
- **328 奇偶位置節點**：重組鏈表，list_add 操作相同
- **234 回文鏈表**：可用快慢指標或反轉，雙向性無特殊益處

## 設計洞察：為何選擇侵入式與環形？

### 1. 侵入式的優勢

**記憶體效率**
```
外部設計：資料結構 + list_node 結構 + 指標追蹤
侵入式：資料結構（包含 list_head）

在 VMA 樹、工作隊列、任務隊列中，節省可觀的記憶體。
```

**同時參與多個鏈表**
```c
struct task_struct {
  struct list_head run_list;        // CPU run queue
  struct list_head sibling;         // 兄弟程序
  struct list_head children;        // 子程序列表
  struct list_head all_proc_list;   // 全局程序列表
};
```

**無指標的間接成本**
```
外部：追蹤 ListNode → 追蹤資料
侵入式：直接在資料結構中操作
```

### 2. 環形設計的優勢

**統一的遍歷邏輯**
```c
// 無需檢查 NULL
list_for_each(pos, &list_head) {
  // 安全處理 pos
}
```

**哨兵節點自然實現**
```c
struct list_head list;  // 頭和哨兵合一
list_add(&new_item, &list);
list_del(&item);
```

**環形屬性在算法中自然表達**
- 循環隊列
- 時間輪（timer wheel）
- 環形緩衝區

## 核心操作的時間複雜度

| 操作 | 時間複雜度 | 備註 |
|------|-----------|------|
| 插入 | O(1) | 需知引用位置 |
| 刪除 | O(1) | 需知節點指標 |
| 遍歷 | O(n) | 線性掃描 |
| 查找 | O(n) | 無隨機訪問 |
| 長度查詢 | O(n) | 需遍歷 |

## 面試考點

### 問題 1：為什麼 kernel 選擇侵入式而非外部鏈表？

**預期答案**
- 記憶體效率：節省結構指標和額外分配
- 緩存局部性：資料和鏈結相鄰，減少快取未命中
- 多重隸屬：同一物件可無成本地參與多個鏈表

### 問題 2：環形鏈表有什麼優勢？

**預期答案**
- 統一遍歷邏輯：無 NULL 檢查
- 哨兵節點天生設計
- 環形操作（輪轉、循環隊列）自然表達
- 尾部插入成本相同：O(1)

### 問題 3：如何高效地遍歷刪除某類節點？

**預期答案**
```c
list_for_each_safe(pos, n, head) {
  obj = list_entry(pos, struct my_obj, node);
  if (should_delete(obj)) {
    list_del(pos);
    free(obj);
  }
}
```

### 問題 4：侵入式設計在同時参與多個鏈表時的優勢？

**預期答案**
```c
struct task_struct t;
list_add(&t.run_list, &cpu_queue);
list_add(&t.all_proc_list, &all_procs);
// 同一 task_struct，無重複分配
```

## 代碼審查要點

1. **指標有效性**：list_del 後的指標變成懸垂指標，需謹慎
2. **初始化**：未初始化的 list_head 會導致亂遍歷
3. **嵌套鎖定**：在遍歷中修改鏈表需考慮鎖定策略
4. **RCU 相關**：list_del_rcu, list_add_rcu 變種用於無鎖讀
5. **類型安全**：container_of 是指標運算，編譯時無類型檢查

## 參考資源

- **include/linux/list.h**：核心定義，150 行程式碼，高度註釋
- **lib/list_sort.c**：針對 list_head 的歸併排序實現
- **mm/mmap.c**：VMA 鏈表使用示例
- **kernel/sched/core.c**：任務隊列應用
