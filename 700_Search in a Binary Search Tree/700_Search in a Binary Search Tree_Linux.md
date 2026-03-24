# 700. Search in a Binary Search Tree — Linux Kernel 連結

## Linux 相關原始碼位置

Linux 核心大量使用樹結構進行搜尋，其中與 BST 搜尋最直接相關的是 **紅黑樹 (Red-Black Tree)** 的查找操作。紅黑樹是一種自平衡 BST，核心中用於排程器、記憶體管理、檔案系統等關鍵子系統。

| 檔案路徑 | 說明 |
|---------|------|
| `include/linux/rbtree.h` | 紅黑樹的公開 API 宣告，包含 `rb_node`、`rb_root` 結構體定義 |
| `lib/rbtree.c` | 紅黑樹的核心實作：插入後修復（rebalance）、刪除、旋轉等操作 |
| `include/linux/rbtree_augmented.h` | 擴展紅黑樹支援（augmented rbtree），用於區間樹等進階場景 |
| `Documentation/core-api/rbtree.rst` | 官方文件，說明如何在核心模組中使用紅黑樹 |
| `mm/mmap.c` | VMA (Virtual Memory Area) 管理，使用紅黑樹做地址空間搜尋 |
| `fs/ext4/extents.c` | ext4 檔案系統使用紅黑樹管理 extent 快取 |
| `kernel/sched/fair.c` | CFS 排程器使用紅黑樹按 vruntime 排序可執行任務 |

### 關鍵設計：核心不提供通用搜尋函式

與 LeetCode 的 `searchBST()` 不同，Linux 的 `rbtree` 刻意**不提供**通用的搜尋函式。`Documentation/core-api/rbtree.rst` 說明了原因：為了讓使用者自行定義比較邏輯（comparator），核心要求每個子系統自己撰寫搜尋函式。典型的搜尋模式如下：

```c
/* 以 mm/mmap.c 中的 VMA 搜尋為例，概念上類似於： */
struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr)
{
    struct rb_node *node = mm->mm_rb.rb_node;  /* 從根節點開始 */

    while (node) {
        struct vm_area_struct *vma = rb_entry(node, struct vm_area_struct, vm_rb);

        if (addr < vma->vm_start)
            node = node->rb_left;       /* 往左子樹走 */
        else if (addr >= vma->vm_end)
            node = node->rb_right;      /* 往右子樹走 */
        else
            return vma;                 /* 找到！ */
    }
    return NULL;
}
```

這與我們的 LeetCode 解法結構完全一致：`while` 迴圈 + 三路比較 + 左/右移動。

---

## Kernel vs. LeetCode 實作對照

| 比較維度 | LeetCode 700 | Linux rbtree 搜尋 |
|---------|-------------|-------------------|
| 樹的類型 | 普通 BST（無平衡保證） | 紅黑樹（自平衡 BST） |
| 搜尋演算法 | 迭代，三路比較 | 迭代，三路比較（完全相同的模式） |
| 比較邏輯 | 直接比較 `int val` | 透過 `rb_entry()` 巨集取得外層結構體後自定義比較 |
| 通用性 | 單一函式，hardcoded 比較 | 每個使用場景自行實作搜尋，支援任意鍵類型 |
| 節點嵌入方式 | 節點包含資料 (`val` 欄位) | **侵入式設計**：資料結構包含 `rb_node` 欄位 |
| 最壞時間複雜度 | O(N)（退化 BST） | O(log N)（紅黑樹保證高度 ≤ 2·log₂(N+1)） |
| 空間複雜度 | O(1) | O(1) |
| 快取友善性 | 不考慮 | 侵入式設計使節點與資料在同一塊記憶體，提升 cache locality |

### 侵入式設計（Intrusive Design）

核心的設計哲學與 LeetCode 截然不同。LeetCode 的 `TreeNode` 把 `val` 放在節點裡：

```c
struct TreeNode {
    int val;
    struct TreeNode *left, *right;
};
```

而 Linux 的 `rb_node` 只包含樹結構資訊，不包含任何使用者資料：

```c
struct rb_node {
    unsigned long __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};
```

使用者把 `rb_node` 嵌入自己的結構體中，再用 `rb_entry()` (本質是 `container_of()` 巨集) 從 `rb_node` 指標反推出外層結構體的指標。這就是所謂的「侵入式」容器。

---

## 主管 Code Review 角度考題

### Q1：為什麼選擇迭代而不是遞迴？

**期望回答方向：** BST 搜尋的遞迴形式是尾遞迴（tail recursion），理論上編譯器可以優化。但在核心空間中，堆疊大小極為有限（通常 8KB 或 16KB），不應依賴編譯器的尾遞迴優化。迭代版保證 O(1) 額外空間，是核心程式碼的標準做法。

**追問：** 核心的堆疊大小限制是多少？為什麼這麼小？
**測試重點：** 堆疊預算意識、核心空間 vs. 使用者空間的差異理解

### Q2：為什麼 Linux rbtree 不提供通用的 search 函式？

**期望回答方向：** 為了零開銷抽象（zero-cost abstraction）。若提供通用搜尋，需要函式指標做回呼（callback），每次比較都有間接呼叫的開銷。讓使用者自行撰寫搜尋函式，編譯器可以將比較邏輯完全內聯（inline），消除所有間接呼叫。

**追問：** 如果用 C++ 模板可以解決嗎？核心為何不用 C++？
**測試重點：** 效能敏感設計、對函式指標開銷的理解

### Q3：`rb_entry()` 巨集的原理是什麼？

**期望回答方向：** `rb_entry()` 本質是 `container_of()` 巨集，利用 `offsetof()` 計算 `rb_node` 成員在外層結構體中的偏移量，然後從 `rb_node` 的位址減去偏移量，得到外層結構體的起始位址。這是 C 語言指標算術的經典應用。

**追問：** 這在什麼情況下會出問題？（提示：對齊、padding）
**測試重點：** 指標精通度、C 記憶體模型理解

### Q4：如果 BST 退化成鏈結串列，搜尋變成 O(N)。核心如何避免這個問題？

**期望回答方向：** 使用紅黑樹而非普通 BST。紅黑樹在每次插入/刪除後通過旋轉和重新著色維持平衡，保證樹高不超過 2·log₂(N+1)，從而確保搜尋始終為 O(log N)。

**追問：** 紅黑樹 vs. AVL 樹各自的取捨是什麼？為何核心選擇紅黑樹？
**測試重點：** 資料結構選擇的工程判斷、平衡條件的理解

### Q5：在 CFS 排程器中，搜尋紅黑樹的 cache 效能如何？

**期望回答方向：** 紅黑樹是指標連結結構，節點在記憶體中不一定連續，cache locality 較差。但侵入式設計讓 `rb_node` 與排程實體（`sched_entity`）在同一塊記憶體中，至少在存取節點時能命中同一個 cache line。對於需要更好 cache 效能的場景，核心近年也引入了 maple tree 作為部分場景的替代方案。

**追問：** maple tree 相比紅黑樹的優勢是什麼？
**測試重點：** cache locality 意識、對核心演進趨勢的了解

---

## 面試加分總結

- **從 LeetCode 解法連結到核心實作：** 展示 BST 搜尋不只是刷題，而是真實系統中最基本的操作之一。核心的 VMA 查找、排程器任務排序、ext4 extent 管理都用完全相同的搜尋模式。
- **提及侵入式資料結構：** 說明 `container_of()` / `rb_entry()` 的原理，展示對 C 語言指標算術和記憶體佈局的深入理解。
- **空間意識：** 解釋為何核心偏好迭代而非遞迴（堆疊預算有限），展現對系統程式設計約束的理解。
- **效能意識：** 討論為何不提供通用搜尋函式（避免函式指標的間接呼叫開銷），展示對微觀效能優化的敏感度。
- **演進趨勢：** 提及 maple tree 等新型資料結構作為紅黑樹在特定場景下的替代方案，展示持續關注核心發展。

---

## Sources

- [Linux rbtree implementation (lib/rbtree.c)](https://github.com/torvalds/linux/blob/master/lib/rbtree.c)
- [Linux rbtree header (include/linux/rbtree.h)](https://github.com/torvalds/linux/blob/master/include/linux/rbtree.h)
- [Linux rbtree documentation (Documentation/core-api/rbtree.rst)](https://github.com/torvalds/linux/blob/master/Documentation/core-api/rbtree.rst)
- [Linux VMA management (mm/mmap.c)](https://github.com/torvalds/linux/blob/master/mm/mmap.c)
- [CFS scheduler (kernel/sched/fair.c)](https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c)
- [LWN: Trees I: Radix trees](https://lwn.net/Articles/175432/)
- [LWN: The maple tree](https://lwn.net/Articles/845507/)
