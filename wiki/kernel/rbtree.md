# rbtree 紅黑樹

## 核心概念

Linux kernel 的紅黑樹實現在 `include/linux/rbtree.h` 和 `lib/rbtree.c` 中，是內核中最廣泛使用的平衡搜尋樹數據結構。與 LeetCode 問題不同，kernel 紅黑樹不提供通用搜尋函數，而是作為基礎構件供各子系統自定義比較邏輯。

## 紅黑樹性質

```c
struct rb_node {
  unsigned long __rb_parent_color;  // 父指標 + 顏色位
  struct rb_node *rb_right;
  struct rb_node *rb_left;
};

struct rb_root {
  struct rb_node *rb_node;
};
```

**紅黑樹的五大不變量**
1. 每個節點是紅色或黑色
2. 根節點是黑色
3. 葉節點（NULL）視為黑色
4. 紅色節點的子節點必為黑色
5. 從根到任一葉的所有路徑黑色節點數相同

**結論**：高度 O(log n)，保證平衡

## 設計特點

### 1. 父指標 + 顏色位壓縮

```c
// 顏色存儲在 __rb_parent_color 最低位
#define RB_RED 0
#define RB_BLACK 1

#define rb_color(r) ((r)->__rb_parent_color & 1)
#define rb_parent(r) (struct rb_node *)((r)->__rb_parent_color & ~3)
```

**優勢**
- 節省 8 字節（64-bit）或 4 字節（32-bit）記憶體
- 對齐指標末尾通常為零，安全利用
- 每棵樹可節省數百 KB 至數 MB

### 2. 無通用搜尋函數

Kernel 紅黑樹刻意不提供通用的 `rb_search()` 函數，原因：

**比較函數往往定製化**
```c
// VMA（虛擬記憶體區域）比較
static int vm_area_cmp(struct vm_area_struct *vma,
                       unsigned long addr)
{
  if (addr < vma->vm_start)
    return -1;
  if (addr >= vma->vm_end)
    return 1;
  return 0;
}

// 每個子系統實現自己的搜尋
struct vm_area_struct *find_vma(struct mm_struct *mm,
                                unsigned long addr)
{
  struct rb_node *rb_node = mm->mm_rb.rb_node;
  struct vm_area_struct *vma = NULL;

  while (rb_node) {
    struct vm_area_struct *v = rb_entry(rb_node, ...);
    if (addr < v->vm_start)
      rb_node = rb_node->rb_left;
    else {
      vma = v;
      rb_node = rb_node->rb_right;
    }
  }
  return vma;
}
```

### 3. 快取最左節點（leftmost）

```c
struct rb_root_cached {
  struct rb_root rb_root;
  struct rb_node *rb_leftmost;  // O(1) 訪問最小值
};
```

**應用**
- CFS 調度器中獲取下一個最小 vruntime 的任務
- 優先隊列中快速取最小元素
- 無需 O(log n) 遍歷至最左葉

## 核心使用案例

### 1. VMA 樹（mm/mmap.c）

管理進程虛擬記憶體區域。

```c
struct mm_struct {
  struct rb_root_cached mm_rb;  // 所有 VMA 的紅黑樹
  unsigned long mmap_base;
  unsigned long total_vm;
};

struct vm_area_struct *find_vma(struct mm_struct *mm,
                                unsigned long addr)
{
  // 自定義比較邏輯
  struct rb_node **link = &mm->mm_rb.rb_root.rb_node;
  // ... 搜尋與插入
}
```

**複雜度**
- 查找特定地址的 VMA：O(log n)
- 遍歷所有 VMA：O(n)

### 2. CFS 調度器（kernel/sched/fair.c）

管理可運行任務。

```c
struct cfs_rq {
  struct rb_root_cached tasks_timeline;  // runnable tasks
  struct sched_entity *curr;
};

// 獲取下一個運行最短的任務
struct sched_entity *__pick_next_entity(struct cfs_rq *cfs_rq)
{
  struct rb_node *left = cfs_rq->tasks_timeline.rb_leftmost;
  if (!left)
    return NULL;
  return rb_entry(left, struct sched_entity, run_node);
}
```

**關鍵特性**
- rb_leftmost 快取使 O(1) 挑選下一個任務
- 每次調度事件（進程阻塞、時間切片耗盡）只需 O(log n) 重新平衡

### 3. Deadline 調度器

類似 CFS，但按 deadline 排序。

```c
struct dl_rq {
  struct rb_root_cached root;  // deadline tasks
};
```

### 4. ext4 檔案系統範圍樹（fs/ext4/extents.c）

管理已分配範圍。

```c
struct ext4_extent_tree {
  struct rb_root extent_tree;
};

// 快速查詢給定邏輯塊的範圍
struct ext4_extent *ext4_find_extent(struct inode *inode,
                                      ext4_lblk_t block)
{
  // 二分搜尋範圍樹
}
```

## 與 LeetCode 問題的對應

### 相關問題

- **700 二叉搜尋樹搜尋**：基本 BST 搜尋，rbtree 執行相同邏輯但自平衡
- **104 二叉樹最大深度**：樹遍歷，rbtree 保證 O(log n) 深度
- **199 二叉樹右視圖**：層序遍歷，rbtree 中無特殊意義

### 面試轉換

- LeetCode 問題通常要求實現特定的樹操作
- Kernel 視角：樹是實現，資料結構接口（搜尋、遍歷）由上層定義

## 設計洞察：為何無通用搜尋？

### 1. 比較函數的多樣性

**VMA 搜尋**：給定地址，找包含它的 VMA（區間重疊問題）

```c
// 不是簡單的鍵比較
if (addr < vma->vm_start)
  return -1;  // 搜尋左子樹
if (addr >= vma->vm_end)
  return 1;   // 搜尋右子樹
return 0;     // 在區間內
```

**Deadline 調度**：按絕對截止時間排序，但支援 deadline 相同的任務。

```c
// 複雜的比較邏輯
int cmp = dl_entity_cmp(a, b);
if (cmp == 0)
  cmp = tie_break(a, b);  // 次級排序
```

### 2. 通用函數的成本

若提供 `rb_search(tree, key, cmp_fn)`，會引入函數指標呼叫開銷，在高頻操作（如調度）中不可接受。

### 3. 內聯優化

每個子系統內聯自己的搜尋邏輯，編譯器可進行激進優化。

```c
// 內聯搜尋，編譯器可展開
while (node) {
  if (addr < node->key) ...
  // 可能被優化為分支預測友好的形式
}
```

## 時間複雜度

| 操作 | 時間複雜度 | 備註 |
|------|-----------|------|
| 搜尋 | O(log n) | 需自定義比較 |
| 插入 | O(log n) | 包含重新平衡 |
| 刪除 | O(log n) | 包含重新平衡 |
| 查找最小值 | O(1) | 使用 rb_leftmost |
| 中序遍歷 | O(n) | 線性掃描 |

## 實現細節

### 旋轉與重新平衡

```c
// 右旋
static void rb_rotate_right(struct rb_node *node,
                            struct rb_root *root)
{
  struct rb_node *left = node->rb_left;
  node->rb_left = left->rb_right;
  if (left->rb_right)
    left->rb_right->__rb_parent_color = (unsigned long)node | RB_BLACK;
  left->__rb_parent_color = node->__rb_parent_color;
  ...
}
```

### 插入與著色

```c
void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
  while (true) {
    struct rb_node *parent = rb_parent(node);

    if (rb_color(parent) == RB_BLACK)
      return;  // 完成

    // 紅色父節點的情況（違反性質 4）
    // 根據叔節點顏色進行旋轉或著色
    if (uncle_is_red) {
      recolor_parent_and_uncle();
      node = parent->parent;
    } else {
      rotate_and_recolor();
      return;
    }
  }
}
```

## 面試考點

### 問題 1：為什麼 kernel 不提供通用搜尋函數？

**預期答案**
- 不同子系統的比較邏輯高度定製化
- 函數指標的開銷在高頻操作中不可接受
- 內聯搜尋允許編譯器激進優化
- 每個子系統可根據自身特性調優

### 問題 2：rb_leftmost 快取的重要性？

**預期答案**
- CFS 調度中，每次調度需取最小 vruntime 的任務
- 不快取需 O(log n) 左遍歷
- 快取使其 O(1)，在高頻調度中節省可觀成本
- 插入/刪除時需維護快取

### 問題 3：紅黑樹與 AVL 樹的權衡？

**預期答案**
- 紅黑樹旋轉次數更少（最壞 3 次）
- AVL 樹高度更小（log φ ≈ 1.44 log 2）
- Kernel 選擇紅黑樹因為修改成本低
- 查詢密集型場景 AVL 可能更佳

### 問題 4：如何在紅黑樹中查找範圍？

**預期答案**
```c
// VMA 案例：查找覆蓋 [start, end] 的所有 VMA
struct rb_node *node = mm->mm_rb.rb_node;
struct vm_area_struct *vma, *result = NULL;

while (node) {
  vma = rb_entry(node, struct vm_area_struct, vm_rb);
  if (start < vma->vm_end) {
    result = vma;  // 可能符合
    node = node->rb_left;
  } else {
    node = node->rb_right;
  }
}
```

## 參考資源

- **include/linux/rbtree.h**：核心 API
- **include/linux/rbtree_augmented.h**：擴增紅黑樹（支援區間查詢）
- **lib/rbtree.c**：旋轉和著色實現
- **mm/mmap.c**：VMA 樹管理
- **kernel/sched/fair.c**：CFS 調度器應用
- **kernel/sched/deadline.c**：Deadline 調度器應用
