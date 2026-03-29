# LeetCode 104 - 二叉樹最大深度 與 Linux Kernel 連結分析

## 問題概述

**LeetCode 104**: 使用 DFS 遞迴找出二叉樹的最大深度

```python
def maxDepth(root):
    if not root:
        return 0
    return 1 + max(maxDepth(root.left), maxDepth(root.right))
```

---

## 1. Linux 相關原始碼位置

### 1.1 Device Tree (OF) 遞迴遍歷
**檔案**: `drivers/of/base.c`, `drivers/of/fdt.c`

Device Tree 是嵌入式系統硬體配置的樹狀結構，內核需要遞迴遍歷解析。

- `of_find_node_by_path()` - 遍歷 DT 樹找尋特定節點
- `of_get_next_available_child()` - 迭代子節點
- `unflatten_device_tree()` - 展開 FDT（Flattened Device Tree），涉及樹深度計算

### 1.2 紅黑樹深度控制
**檔案**: `lib/rbtree.c`, `include/linux/rbtree.h`

- 紅黑樹藉由顏色不變式保證最大深度 $O(\log n)$
- `rb_node` 結構體的深度由紅黑性質隱式限制
- 沒有顯式的 `height` 欄位，但內核依賴樹平衡性質

### 1.3 VFS Dentry 快取樹遍歷
**檔案**: `fs/dcache.c`

- 目錄樹的遞迴遍歷（例如 `dcache_walk()` 概念）
- Dentry 節點的父子關係形成樹結構
- 目錄深度影響路徑解析效能

### 1.4 Lockdep 相依性圖深度檢測
**檔案**: `kernel/locking/lockdep.c`

- 檢測死鎖時需要計算鎖相依性圖的深度
- `check_noncircular()` 執行 DFS 遍歷檢查環
- 類似於樹遍歷，但作用在有向圖上

### 1.5 Proc 檔案系統樹遍歷
**檔案**: `fs/proc/`

- `/proc` 虛擬檔案系統樹狀結構
- `proc_readdir()` 遞迴遍歷目錄項

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 演算法層面

| 面向 | LeetCode 104 | Linux Kernel |
|------|------------|--------------|
| **遍歷策略** | DFS 遞迴（深度優先） | DFS/BFS 混合，多數情境使用迭代以避免堆疊溢位 |
| **計算目標** | 單一數值：最大深度 | 多重目標：驗證結構、檢測環、搜尋特定節點 |
| **時間複雜度** | $O(n)$ - 訪問每個節點一次 | $O(n)$ 或更優，依具體子系統而異 |
| **空間複雜度** | $O(h)$ 遞迴棧，$h$ 為樹高 | 可控制，遞迴深度受 `CONFIG_MAX_STACK_DEPTH` 限制 |

### 2.2 資料結構對比

**LeetCode TreeNode**:
```c
struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};
```

**Linux OF node** (`include/linux/of.h`):
```c
struct device_node {
    const char *name;
    const char *type;
    phandle phandle;
    const char *full_name;
    struct fwnode_handle fwnode;

    struct device_node *parent;
    struct device_node *child;
    struct device_node *sibling;
    struct device_node *next;  /* next node in list */
    struct device_node *allnext;

    struct proc_dir_entry *pde;
    struct kref kref;
    unsigned long _flags;
    void *data;
    struct list_head properties;
};
```

**對比說明**:
- OF node 有 `parent` 指標（LeetCode 不需），支援反向遍歷
- OF node 使用 `child`/`sibling` 或 `next` 組織子節點，比二叉樹更複雜
- 內核節點包含中繼資料（`phandle`, `full_name`, `kref`），LeetCode 僅存值

### 2.3 遍歷策略差異

**LeetCode 遞迴深度優先** (簡潔):
```python
def maxDepth(root):
    if not root:
        return 0
    return 1 + max(maxDepth(root.left), maxDepth(root.right))
```

**Linux 迭代深度優先** (內核風格，避免堆疊溢位):
```c
void of_walk_tree(struct device_node *root) {
    struct device_node *node = root;
    struct list_head *queue = &root->child;

    while (queue != NULL) {
        /* Process node */
        visit(node);

        if (node->child) {
            queue = &node->child;
        } else if (node->sibling) {
            queue = &node->sibling;
        } else {
            /* Backtrack to parent's next sibling */
            node = node->parent;
        }
    }
}
```

### 2.4 複雜度考量

| 因素 | LeetCode | Kernel |
|------|---------|--------|
| **遞迴深度限制** | 無明確限制，依 Python 堆疊 | 受 `CONFIG_FRAME_WARN` 限制，通常 2048 bytes |
| **最壞情況堆疊** | 鏈表樹：$O(n)$ 堆疊 | 必須迭代實作或限制遞迴 |
| **效能最佳化** | 無，LeetCode 評判時間充足 | RCU 讀鎖、快取局部性最佳化 |

### 2.5 API 設計哲學

**LeetCode**: 函式簽章簡潔，輸入輸出清晰
```python
def maxDepth(root: Optional[TreeNode]) -> int:
```

**Linux**: API 需考慮並行性、記憶體管理、錯誤情況
```c
int of_get_tree_depth(struct device_node *node)
{
    if (!node)
        return 0;
    return 1 + of_get_child_depth(node);
}

/* 實務上內核會加：
   - RCU 保護
   - 檢查 node->kref 有效性
   - 錯誤返回値（可能 -EINVAL）
*/
```

---

## 3. 主管 Code Review 角度考題

### 3.1 題目：「為什麼遞迴實作在 Kernel 中不可行？」

**預期答案**:
- 核心問題：kernel 堆疊有限，通常 8KB（x86_64 預設），而遞迴深度 $O(n)$ 會導致棧溢位
- 不平衡樹（鏈表狀）會快速消耗堆疊空間
- 內核必須使用迭代或尾遞迴最佳化

**追問層級**:
1. 「那 Device Tree 通常有多深？」
   - 回答：一般 3~10 層，但某些 ARM SoC 可達 20+ 層
2. 「如何估算堆疊使用量？」
   - 回答：每層遞迴呼叫需 `sizeof(struct device_node*) + 局部變數`，約 32~64 bytes
3. 「如何驗證棧溢位不會發生？」
   - 回答：`CONFIG_DEBUG_STACK_USAGE` 和動態檢查，或使用迭代實作

### 3.2 題目：「為什麼 Kernel 的 OF node 比 LeetCode TreeNode 複雜？」

**預期答案**:
- LeetCode：算法教學，專注核心邏輯
- Kernel：工業級，需支援：
  - 並行訪問（RCU 保護）
  - 動態註冊/移除節點
  - 屬性管理（properties list）
  - 參考計數（kref）
  - 反向遍歷（parent 指標）

**追問**:
1. 「如何在不加鎖的情況下安全遍歷？」
   - 回答：RCU（Read-Copy-Update），讀側無鎖
2. 「parent 指標有什麼用？」
   - 回答：快速路徑搜尋、回溯操作、除錯

### 3.3 題目：「LeetCode maxDepth 和 lockdep 圖遍歷有何相似性和差異？」

**預期答案**:

| 相似處 | 差異 |
|------|------|
| 都是圖遍歷 | LeetCode：樹（無環） vs lockdep：DAG（需環檢測） |
| DFS 訪問模式 | LeetCode：計算高度 vs lockdep：檢測死鎖 |
| 遞迴或迭代 | LeetCode：遞迴簡潔 vs lockdep：迭代+栈以控制深度 |

**追問**:
1. 「為什麼 lockdep 需要環檢測？」
   - 回答：鎖相依性形成環會導致死鎖（A→B→A）
2. 「如果 Device Tree 有環（不應該），maxDepth 會怎樣？」
   - 回答：無限遞迴或返回錯誤；內核驗證樹結構完整性

### 3.4 題目：「給定 Dentry 樹，如何在 $O(1)$ 空間內計算最大深度？」

**預期答案**:
- 基礎：不可能在 $O(1)$ 空間內遍歷整棵樹
- 但若只計算根到葉子的**某條路徑深度**，可用指標走訪，空間 $O(1)$
- 完整深度計算需 $O(h)$ 或遞迴棧

**追問**:
1. 「Dentry 有 parent 指標，如何利用？」
   - 回答：反向遍歷到根，計算深度，但前向遍歷仍需 $O(h)$ 空間
2. 「實務中 Dentry 快取深度有上限嗎？」
   - 回答：檔案系統設計者需限制（防 DoS），VFS 檢查 `MAX_NESTED_LINKS`

---

## 4. 面試加分總結

### 關鍵 Talking Points

1. **樹/圖遍歷在系統軟體的廣泛應用**
   - 不只是 LeetCode 題目，Device Tree、Dentry、Lockdep 都是實例
   - 秀出對 Linux kernel 實際需求的理解

2. **遞迴 vs 迭代的取捨**
   ```
   - 面試官會問：「為什麼 Kernel 很少用遞迴？」
   - 回答：堆疊有限、可預測性要求高
   - 舉例：`drivers/of/base.c` 多數使用迭代
   ```

3. **並行性和同步考量**
   - LeetCode：無需同步
   - Kernel：RCU、自旋鎖、原子操作
   - 秀出對工業級代碼的認識

4. **複雜度分析的細節**
   - 不只說 $O(n)$，要說清楚：
     - 堆疊空間 $O(h)$（高度依賴）
     - 在不平衡樹上可能退化為 $O(n)$
     - 如何驗證上界

5. **程式碼品質要素**
   - 錯誤處理（null check、無效節點檢測）
   - 邊界情況（空樹、單節點、環狀結構）
   - 效能考量（快取友善性、RCU 與並行訪問）

### 面試回答範例

**問**: 「如何改進 LeetCode maxDepth 使其適合在 Linux kernel 中使用？」

**答**:
```c
/**
 * of_get_node_depth - Get depth of device tree node (iterative)
 * @node: device node pointer
 *
 * Returns: depth of node from root, or negative on error
 *
 * Note: Uses iteration to avoid stack overflow on deeply nested trees.
 *       Protected by RCU for concurrent access.
 */
int of_get_node_depth(struct device_node *node)
{
    int depth = 0;
    struct device_node *p;

    if (unlikely(!node))
        return -EINVAL;

    rcu_read_lock();
    for (p = node; p->parent != NULL; p = p->parent) {
        depth++;
        if (depth > MAX_DEVICE_TREE_DEPTH) {
            rcu_read_unlock();
            pr_warn("Device tree depth overflow at %s\n", p->name);
            return -EOVERFLOW;
        }
    }
    rcu_read_unlock();

    return depth;
}
```

**改進點**:
- 迭代而非遞迴（避免棧溢位）
- RCU 保護（允許並行讀取）
- 錯誤檢查和邊界限制
- 日誌記錄以利除錯
- 文檔化的行為和限制

---

## 5. Sources

### 官方 Kernel 原始碼 (GitHub Linux Kernel Mirror)

1. **Device Tree 相關**
   - https://github.com/torvalds/linux/blob/master/drivers/of/base.c
   - https://github.com/torvalds/linux/blob/master/drivers/of/fdt.c
   - https://github.com/torvalds/linux/blob/master/include/linux/of.h

2. **Dentry Cache**
   - https://github.com/torvalds/linux/blob/master/fs/dcache.c
   - https://github.com/torvalds/linux/blob/master/include/linux/dcache.h

3. **紅黑樹**
   - https://github.com/torvalds/linux/blob/master/lib/rbtree.c
   - https://github.com/torvalds/linux/blob/master/include/linux/rbtree.h

4. **Lockdep**
   - https://github.com/torvalds/linux/blob/master/kernel/locking/lockdep.c
   - https://github.com/torvalds/linux/blob/master/include/linux/lockdep.h

5. **Proc 檔案系統**
   - https://github.com/torvalds/linux/blob/master/fs/proc/base.c
   - https://github.com/torvalds/linux/blob/master/fs/proc/

### 補充閱讀

- **Device Tree Specification**: https://devicetree-specification.readthedocs.io/
- **Linux RCU (Read-Copy-Update)**: https://www.kernel.org/doc/html/latest/RCU/
- **Kernel Memory Management**: https://www.kernel.org/doc/html/latest/core-api/memory-management.html

---

## 結論

LeetCode 104 的簡潔遞迴解法展現了演算法本質，但 Linux kernel 中的樹遍歷涉及：
- **並行性**：RCU、鎖、原子操作
- **健壯性**：錯誤檢查、邊界限制、stack 溢位預防
- **效能**：快取局部性、複雜度控制、最佳化路徑

理解這些差異，能在面試中展現從算法到系統軟體的完整認識。
