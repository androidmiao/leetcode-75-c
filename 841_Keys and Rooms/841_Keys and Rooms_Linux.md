# 841. Keys and Rooms — Linux Kernel DFS 分析

## 問題概述

給定 `n` 個房間，從房間 0 開始探索。每個房間含有鑰匙，可以開啟其他房間。問題：能否訪問所有房間？

這是一個**圖可達性問題**。使用**疊式 DFS（iterative DFS with stack）**實作，時間複雜度 O(N + E)。

---

## 1. Linux 相關原始碼位置

### 1.1 Lockdep 循環偵測 (主要關聯)
**檔案**: `kernel/locking/lockdep.c`

Lockdep 是核心中的死鎖偵測機制，使用**有向圖 DFS** 檢查鎖的依賴關係。

- **圖構建**: 記錄每次鎖獲取的順序，建立鎖間依賴圖
- **DFS 偵測**: 使用深度優先搜尋偵測環（deadlock cycle）
- **堆疊使用**: 與本題相同，使用堆疊追蹤訪問路徑

相關函數：
```c
// kernel/locking/lockdep.c
static int __lockdep_acquire()  // 圖節點加入
static void check_chain_key()   // 依賴鏈檢查
static int check_path()         // DFS 路徑偵測
```

### 1.2 模組依賴解析 (depmod)
**檔案**: `kernel/module/main.c`, `scripts/mod/depmod.c`

Linux 模組系統中，模組間有依賴關係。`depmod` 工具解析模組依賴，類似於本題中「房間→房間」的有向圖。

- 每個模組可依賴其他模組（類似房間含有其他房間的鑰匙）
- 必須確保所有依賴可達（類似確保所有房間可訪問）

### 1.3 檔案系統命名空間傳播
**檔案**: `fs/namespace.c`

掛載命名空間的傳播使用類似 DFS 的遍歷機制，確保所有掛載點正確傳播到子命名空間。

```c
// fs/namespace.c
static void propagate_mnt()  // 使用 DFS 類遍歷傳播掛載
```

### 1.4 通用圖遍歷實用程式
**檔案**: `lib/list.h`, `lib/rbtree.c`

核心提供的通用資料結構和遍歷巨集，支持任意的 DFS/BFS 實作。

---

## 2. Kernel vs. LeetCode 實作對照

### LeetCode 841 解法 (疊式 DFS)

```python
def canVisitAllRooms(rooms):
    visited = set()
    stack = [0]

    while stack:
        room = stack.pop()
        if room in visited:
            continue
        visited.add(room)
        for key in rooms[room]:
            if key not in visited:
                stack.append(key)

    return len(visited) == len(rooms)
```

**特點**:
- 使用 Python 列表作為堆疊
- 追蹤已訪問節點 (visited set)
- 迭代處理鄰接節點

### Lockdep 循環偵測 (簡化版)

```c
// kernel/locking/lockdep.c (簡化示意)
static int check_path(struct lock_class *source,
                      struct lock_class *target)
{
    struct list_head *entry;
    struct lock_list *elem;

    // DFS: 從 source 開始，找是否能抵達 target
    list_for_each(entry, &source->locks_before) {
        elem = list_entry(entry, struct lock_list, entry);

        if (elem->class == target)
            return LOCK_CYCLE;  // 環偵測！

        if (check_path(elem->class, target) == LOCK_CYCLE)
            return LOCK_CYCLE;
    }

    return LOCK_OK;
}
```

**差異與相似性**:

| 面向 | LeetCode 841 | Lockdep |
|------|-------------|---------|
| **圖結構** | 房間 → 房間（人為指定） | 鎖 → 鎖（動態建立） |
| **遍歷方式** | 疊式 DFS（迴圈） | 遞迴 DFS |
| **目標** | 可達性（訪問所有節點） | 環偵測（找循環依賴） |
| **訪問記錄** | set (visited) | 鎖類位圖 (lock_gen_id) |
| **複雜度** | O(V + E) | O(V² + E) 最壞情況 |

**核心相同點**:
1. 兩者都使用 DFS 遍歷有向圖
2. 都需追蹤已訪問節點以避免無窮迴圈
3. 都關注**可達性**（lockdep 檢查「能否從 A 抵達 B」）

---

## 3. 主管 Code Review 角度考題

### Q1: 為什麼使用疊式 DFS 而非遞迴 DFS？

**預期回答**:
- 疊式 DFS 避免深遞迴引發**棧溢位**（stack overflow）
- 對於大規模圖（如 lockdep 中有數千個鎖類），迴圈更穩定
- 時間/空間複雜度相同 O(V+E)，但疊式更易控制

**這測試**:
- 對遞迴限制的理解
- 核心系統設計考量（lockdep 必須支持高峰值）

### Q2: 如何修改此演算法以偵測環（cycle detection）？

**預期回答**:
```python
def hasCycle(rooms):
    visited = set()
    rec_stack = set()  # 遞迴堆疊：當前路徑上的節點
    stack = [0]
    parent = {}  # 追蹤父節點

    while stack:
        room = stack.pop()
        if room in rec_stack:
            return True  # 環
        if room in visited:
            continue

        visited.add(room)
        rec_stack.add(room)

        for key in rooms[room]:
            if key not in visited:
                stack.append(key)
                parent[key] = room

        rec_stack.remove(room)  # 回溯

    return False
```

**這測試**:
- 是否理解 DFS 與環偵測的關係
- 是否知道 lockdep 使用類似技巧
- 對「遞迴堆疊」（記錄當前路徑）的認知

### Q3: 在 lockdep 或模組系統中，此演算法如何應用於實際問題？

**預期回答**:
- **Lockdep**: 檢查新鎖獲取是否會形成環。若鎖 A → B → ... → A，則死鎖
- **Depmod**: 檢查模組依賴圖是否有環（循環依賴）
- **命名空間**: 確保掛載傳播不會陷入無窮遞迴

舉例（模組）:
```
模組 A 依賴 B
模組 B 依賴 C
模組 C 依賴 A  ← 環！depmod 應拒絕
```

**這測試**:
- 能否將 LeetCode 問題映射到真實核心系統
- 對核心設計決策的深層理解

### Q4: 此演算法的空間複雜度如何優化？

**預期回答**:
目前：O(V) 用於 visited set

在 lockdep 中的最佳化：
- 使用**位圖**（bitmap）而非 set，減少內存 40%+
- 使用**鎖 ID**（整數）而非完整物件指標
- 在高峰值（thousands of lock classes）時，空間優化關鍵

```c
// lockdep 使用位圖：
static unsigned char lockdep_visited[BITS_TO_LONGS(LOCK_TRACE_LEN)];
```

**這測試**:
- 系統層面的最佳化思維
- 對大規模系統設計的考慮

### Q5: 此演算法如何處理動態圖（新增邊/節點）？

**預期回答**:
LeetCode 841: 靜態圖（房間數固定）

Lockdep: 動態圖（執行時新增鎖）
- 需增量檢查（incremental checking）
- 新鎖獲取時，檢查是否與現有鎖形成環
- 需緩存（cache）之前的檢查結果以加速

```c
// lockdep 緩存鎖鏈：
struct lock_chain {
    u8 irq_context;
    u8 depth;
    u16 base;
    struct list_head entry;
    ...
};
```

**這測試**:
- 能否思考演算法在實際系統中的擴展性
- 對性能和正確性的平衡理解

---

## 4. 面試加分總結

### 知識亮點

1. **DFS 與環偵測的關聯**
   - LeetCode 841 是 DFS 可達性；lockdep 是 DFS 環偵測
   - 面試官會欣賞深層連結的理解

2. **從演算法到系統設計**
   - 能說出「lockdep 使用 DFS 檢查死鎖」
   - 說明核心如何應用 O(V+E) 演算法於實時死鎖偵測

3. **空間與時間權衡**
   - 疊式 DFS 避免遞迴溢位（核心關鍵考量）
   - 位圖優化記憶體（lockdep 實現細節）

4. **動態性與增量性**
   - 鎖在執行時動態加入核心
   - 演算法如何適應動態圖變化

5. **除錯與驗證**
   - Lockdep 自動偵測並警告死鎖風險
   - 相比 LeetCode「是否可達」，生產系統需更嚴格的檢查

### 推薦闡述順序

1. **第一層**:
   > "841 是圖可達性問題，使用疊式 DFS。"

2. **第二層**:
   > "核心的 lockdep 子系統也使用 DFS，但目的是環偵測以防止死鎖。"

3. **第三層** (如果面試官追問):
   > "Lockdep 維護有向圖，每條邊代表『鎖 A 在鎖 B 之前被獲取』。新鎖獲取時，DFS 檢查是否會形成環。若形成環，核心警告潛在死鎖。"

4. **第四層** (進階):
   > "為了在數千個鎖類上高效運行，lockdep 使用位圖記憶體和增量檢查策略，而非每次從零開始。"

---

## 5. Sources

### 核心原始碼

1. **Lockdep 主實作**
   - [kernel/locking/lockdep.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/locking/lockdep.c)
   - [kernel/locking/lockdep_internals.h](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/locking/lockdep_internals.h)

2. **模組系統**
   - [kernel/module/main.c - Module dependency resolution](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/module/main.c)
   - [scripts/mod/depmod.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/scripts/mod/depmod.c)

3. **檔案系統命名空間**
   - [fs/namespace.c - Mount namespace propagation](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/fs/namespace.c)

### LWN 文章

1. **Lockdep 介紹** (深度技術文章)
   - [LWN: The Lockdep Subsystem](https://lwn.net/Articles/185666/)

2. **Deadlock Detection in the Kernel**
   - [LWN: Deadlock detection in the kernel](https://lwn.net/Articles/170039/)

3. **Dependency Graphs and Lock Ordering**
   - [LWN: Debugging kernel locking](https://lwn.net/Articles/185423/)

### 相關資源

- [The Linux Kernel Documentation - Lock Debugging](https://www.kernel.org/doc/html/latest/locking/lockdep.html)
- [Kernel Lock Validation](https://www.kernel.org/doc/html/latest/locking/lockdep-design.html)
- [Graph Algorithms - Topological Sort & Cycle Detection](https://www.geeksforgeeks.org/topological-sorting/)

---

## 結論

問題 841 的「房間可達性」與 Linux 核心 lockdep 的「死鎖環偵測」皆為 **DFS 圖遍歷應用**。理解從演算法到生產系統的映射，展示：

- **演算法正確性**: O(V+E) DFS 足以解決圖可達性與環偵測
- **系統設計**: 核心如何將簡單演算法擴展至複雜、動態、高吞吐量場景
- **工程思維**: 記憶體最佳化、增量檢查、除錯工具整合

這些洞察在面試中展現**從基礎演算法到系統層面的深度理解**。
