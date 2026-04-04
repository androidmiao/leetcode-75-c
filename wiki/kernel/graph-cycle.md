# Graph & Cycle Detection 圖論與環偵測

## 核心概念

Linux kernel 的環偵測實現位於 `kernel/locking/lockdep.c`，用於檢測死鎖風險。模組依賴解析在 `depmod` 中使用拓撲排序。與 LeetCode 圖論問題不同，kernel 實現必須處理實時性、性能和可伸縮性約束。

## Lockdep：kernel/locking/lockdep.c

### 問題陳述

```c
// 死鎖場景：ABC 順序死鎖
Thread 1:         Thread 2:
lock(A);          lock(B);
lock(B);          lock(C);
lock(C);          lock(A);  // 等待 A，而 Thread 1 等待 C
```

### 動態死鎖檢測

Lockdep 在運行時追蹤所有鎖依賴關係，檢測環。

```c
struct lock_class {
  struct list_head hash_entry;
  unsigned int key;
  struct list_head locks_after;  // 這個鎖後面可能跟哪些鎖
  struct list_head locks_before; // 哪些鎖可能在這個鎖前面
};

struct lock_chain {
  u8 depth;
  u32 base_class;  // 鏈的起點
  u32 parent_class;
};

// 全局依賴圖
struct graph_lock_dep {
  struct list_head next;
  struct lock_class *class_from;
  struct lock_class *class_to;
};
```

### 為什麼選擇 BFS 而非 DFS？

#### 問題 1：棧限制

```c
// DFS 實現（遞迴）
bool has_cycle_dfs(struct lock_class *node,
                   struct set *visited,
                   struct set *rec_stack) {
  visited.add(node);
  rec_stack.add(node);

  for (lock_class *next : node->locks_after) {
    if (!visited.contains(next)) {
      if (has_cycle_dfs(next, visited, rec_stack))
        return true;  // 遞迴深度可達數百
    } else if (rec_stack.contains(next)) {
      return true;  // 發現環
    }
  }

  rec_stack.remove(node);
  return false;
}
```

**DFS 的棧成本**
```
內核棧：4-8 KB（某些架構）
鎖類別：1000+（實際系統）
DFS 遞迴：可達 O(圖直徑) ≈ 50-100
棧幀：每層 ~200 字節
需求：50 × 200 = 10 KB > 棧大小！
```

#### 問題 2：實時性

```c
// DFS 可能在深層次暴力搜尋，導致不可預測的延遲
if (has_cycle_dfs(...))  // 可能耗時 1 ms，不確定
```

#### BFS 解決方案

```c
// BFS 實現（隊列）
bool has_cycle_bfs(struct lock_class *start) {
  struct queue q;
  struct set visited;

  q.enqueue(start);
  visited.add(start);

  while (!q.empty()) {
    struct lock_class *node = q.dequeue();

    for (lock_class *next : node->locks_after) {
      if (next == start) {
        return true;  // 回到起點，環存在
      }

      if (!visited.contains(next)) {
        visited.add(next);
        q.enqueue(next);
      }
    }
  }

  return false;
}
```

**BFS 的優勢**
- **棧成本**：隊列 = 堆分配，不用棧
- **可預測性**：訪問每個節點一次，O(V + E)
- **早期終止**：一旦發現環路邊即可返回

### Lockdep 實現細節

```c
static int
check_dependency(struct lock_class *this,
                 struct lock_class *prev) {
  if (unlikely(prev == this))
    return 0;

  // BFS 檢查從 prev 到 this 的路徑
  struct queue q;
  struct list_head *next;

  q_init(&q);
  q_enqueue(&q, prev);

  for_each_entry(lock_b, &prev->locks_after, chain_entry) {
    if (lock_b == this)
      return -1;  // 發現直接依賴

    q_enqueue(&q, lock_b);
  }

  // 擴展搜尋
  while (!q_empty(&q)) {
    struct lock_class *lock_a = q_dequeue(&q);

    // ... 檢查 lock_a 的後繼
    // 避免棧溢出，使用隊列
  }

  return 0;
}
```

### 應用：鎖對分析

```c
void mark_lock(struct lock_class *this,
               enum lock_usage_bit new_bit) {
  if (new_bit == LOCK_USED_IN_SOFTIRQ &&
      this->usage & LOCK_HELD_AS_HARDIRQ) {
    // 衝突：持有硬中斷，在軟中斷中使用
    // 可能死鎖！
    print_warning(...);
  }

  // 記錄所有鎖對
  // this 和前面持有的鎖之間關係
  mark_lock_dependency(prev_held_lock, this);
}
```

## 模組依賴：depmod

### 拓撲排序

模組間存在依賴關係（驅動 A 依賴驅動 B）。系統啟動時必須按順序加載以避免未定義引用。

### 問題陳述

```
模組依賴：
  A 依賴 B, C
  B 依賴 D
  C 依賴 D
  E 無依賴
  F 依賴 G

拓撲排序結果：D, B, C, A, E, G, F（眾多有效順序之一）
```

### 算法：Kahn's Algorithm（BFS）

```c
void topological_sort(struct module_graph *graph) {
  int *in_degree = malloc(sizeof(int) * graph->n_modules);
  struct queue q;

  // 計算入度
  for (int i = 0; i < graph->n_modules; i++) {
    in_degree[i] = 0;
    for (struct dependency *dep : graph->modules[i]->depends_on) {
      in_degree[dep->target]++;
    }
  }

  // 入隊所有入度為 0 的模組
  q_init(&q);
  for (int i = 0; i < graph->n_modules; i++) {
    if (in_degree[i] == 0) {
      q_enqueue(&q, graph->modules[i]);
    }
  }

  // 處理隊列
  struct list_head sorted = LIST_HEAD_INIT(sorted);
  while (!q_empty(&q)) {
    struct module *m = q_dequeue(&q);
    list_add_tail(&m->sort_link, &sorted);

    // 移除依賴邊，更新後繼的入度
    for (struct module *dependent : m->dependents) {
      in_degree[dependent->id]--;
      if (in_degree[dependent->id] == 0) {
        q_enqueue(&q, dependent);
      }
    }
  }

  // 檢查環
  for (int i = 0; i < graph->n_modules; i++) {
    if (in_degree[i] != 0) {
      fprintf(stderr, "Circular dependency detected!\n");
      return -1;
    }
  }

  return 0;
}
```

### 複雜度

| 操作 | 時間 | 空間 |
|------|------|------|
| 入度計算 | O(V + E) | O(V) |
| 處理隊列 | O(V + E) | O(V) |
| 總計 | O(V + E) | O(V) |

## 與 LeetCode 問題的對應

### 相關問題

- **207 課程表**：拓撲排序，檢測環，Kahn 算法直接應用
- **210 課程表 II**：拓撲排序並輸出順序
- **141 環形鏈表**：環偵測，但無向圖問題，Floyd 快慢指標更適合

### 問題轉換

| LeetCode | Kernel | 差異 |
|----------|--------|------|
| 簡單圖 | 複雜度爆炸 | 1000+ 節點，邊更多 |
| DFS/遞迴 | BFS/隊列 | 棧成本限制 |
| 算法精度 | 檢測準確性 | 假陽性成本（警告無實際環）vs 假陰性（漏檢死鎖） |

## 設計洞察

### 1. BFS vs DFS 在核心中的權衡

**DFS 優勢**
- 代碼簡潔（遞迴）
- 快取局部性好（深度優先訪問相近節點）
- 發現環的第一條路徑

**DFS 劣勢**
- 棧成本不可預測
- 最壞情況 O(圖直徑) 遞迴深度

**BFS 優勢**
- 棧成本有界（隊列堆分配）
- O(V + E) 保證
- 預測性強

**BFS 劣勢**
- 隊列分配開銷
- 快取局部性較差

**Kernel 選擇 BFS 因為**
- 可預測性至關重要
- 棧是稀缺資源
- 無界遞迴是禁忌

### 2. 環偵測的成本

```c
// Lockdep 檢查的成本
每次獲取鎖：O(V + E) BFS
系統運行時間內鎖獲取次數：1000 億+
總成本：無法承受！

// 解決方案：快取
已檢查的鎖對：記錄 (lock_a, lock_b) 無環
重用結果，避免重新計算
```

### 3. 實時系統中的環偵測

```
目標：檢測死鎖風險，但不能阻塞系統

動態方法：
  訪問鎖 → lockdep 記錄依賴 → 後臺檢查

非同步檢查：
  check_dependency() 可能延遲，非阻塞

報警機制：
  若檢測到潛在環 → 日誌警告，但允許操作
  真正死鎖時核心堆棧追蹤揭示
```

## 時間複雜度

| 操作 | 時間 | 備註 |
|------|------|------|
| 檢測環 | O(V + E) | BFS，一次遍歷 |
| 拓撲排序 | O(V + E) | Kahn's 算法 |
| 快取命中 | O(1) | 已知無環 |

## 面試考點

### 問題 1：為什麼 lockdep 使用 BFS 而非 DFS？

**預期答案**
- **棧限制**：內核棧 4-8 KB，圖可達 1000+ 節點，DFS 遞迴深度不可控
- **可預測性**：BFS O(V + E)，每個節點訪問一次，時間有界
- **實時性**：系統必須無延遲，遞迴可能導致數毫秒延遲

### 問題 2：BFS 在環偵測中如何終止？

**預期答案**
```c
// 開始 BFS 於鎖 A，搜尋是否存在到 B 的路徑
BFS(A) {
  for (next in A.dependents) {
    if (next == B) return true;  // 環！
    queue.enqueue(next);
  }
  while (queue not empty) {
    node = queue.dequeue();
    for (next in node.dependents) {
      if (next == B) return true;  // 環！
      queue.enqueue(next);
    }
  }
  return false;
}
```

### 問題 3：模組依賴的拓撲排序為什麼用 Kahn 而非 DFS？

**預期答案**
- **環偵測**：Kahn 自然檢測環（入度永遠不為零）
- **建構順序**：隊列順序即有效加載順序
- **簡單性**：無遞迴，無棧問題
- **適配性**：支援動態模組加載（新模組加入時增量更新）

### 問題 4：如何優化實時系統中反覆的環檢查？

**預期答案**
- **快取已知對**：(lock_a, lock_b) 無環 → 記錄，後續 O(1)
- **非同步檢查**：不在關鍵路徑，後臺線程運行
- **增量更新**：新依賴不與現有覆蓋，只需檢查子圖
- **採樣**：只檢查熱鎖，冷鎖延遲檢查

## 參考資源

- **kernel/locking/lockdep.c**：動態死鎖檢測，3000+ 行
- **Documentation/locking/lockdep-design.txt**：設計文檔
- **scripts/depmod.c**：模組依賴排序（用戶空間）
- **kernel/module/main.c**：模組加載器
- **Documentation/core-api/kernel-api.rst**：鎖定 API 文檔
