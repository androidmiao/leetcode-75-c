# 207. Course Schedule — Linux Kernel Connection

## Linux 相關原始碼位置

| 應用場景 | 原始碼路徑 | 說明 |
|----------|-----------|------|
| 死鎖偵測（lockdep） | `kernel/locking/lockdep.c` | 執行期鎖定依賴驗證器，核心函數 `__bfs()`、`check_noncircular()` |
| 鎖依賴資料結構 | `include/linux/lockdep.h` | `lock_class`（節點）、`lock_list`（邊，含 BFS parent 指標） |
| 鎖依賴追蹤型別 | `include/linux/lockdep_types.h` | `struct lock_class_key`、`struct lockdep_map` |
| 模組依賴排序（depmod） | kmod 專案 `tools/depmod.c` | 掃描 `.ko` 模組的 symbol export/import，建立依賴 DAG |
| 模組載入順序 | `kernel/module/main.c` | `resolve_symbol()` 解析模組間 symbol 依賴 |

### lockdep 詳細說明

lockdep 是 Ingo Molnar 撰寫的執行期鎖定正確性驗證器。每當 thread 取得一把鎖，lockdep 記錄「持有 L1 的狀態下嘗試取得 L2」，形成 dependency edge L1 → L2。所有鎖的依賴關係構成一張有向圖。

新增一條邊 B → A 時，呼叫 `check_noncircular()` 檢查是否存在 A → ... → B 的路徑。若存在，則形成環 A → ... → B → A，即 deadlock。

關鍵函數：

- `__bfs()`：核心 BFS 遍歷，用 circular queue 實作，搜尋依賴圖中的目標節點
- `check_noncircular()`：檢查新增邊是否造成強依賴環（deadlock）
- `check_irq_usage()`：檢查 irq-safe → irq-unsafe 的衝突

### depmod 詳細說明

depmod 掃描 `/lib/modules/<kernel_version>/` 下所有 `.ko` 模組，分析 exported symbols 和 required symbols，建立模組間依賴有向圖，執行拓撲排序決定載入順序。若偵測到循環依賴，直接報錯 fatal。

模組 = 課程（vertex），symbol 依賴 = 先修關係（edge），能否產生合法載入順序 = `canFinish()` 回傳 true/false。depmod 實際上還需要輸出順序，對應 LeetCode 210（Course Schedule II）。

---

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 207（我們的 DFS 三色標記） | lockdep（`__bfs()`） |
|------|-------------------------------------|---------------------|
| 演算法 | DFS 三色標記（WHITE/GRAY/BLACK） | BFS（circular queue） |
| 圖的建構 | 一次性建鄰接表，離線處理 | 動態新增邊，每次鎖操作即時檢查 |
| 偵測時機 | 離線：建完圖再整體跑 DFS | 線上：每次加邊立即做可達性查詢 |
| 為何選此演算法 | DFS 是環偵測的經典教科書方法 | BFS 用 circular queue 不吃 kernel stack（只有 8-16KB） |
| 資料結構 | `int** adj`（鄰接表）+ `int* state` | `struct lock_list`（鏈結串列）+ circular queue |
| 結果用途 | 回傳 true/false | 印出完整 dependency chain 幫助 debug |
| 環的報告 | 只知道「有環」 | 回溯 parent 指標印出完整鎖依賴鏈 |
| 空間策略 | malloc 鄰接表，最壞 O(V²) | per-CPU circular queue，固定大小 buffer |

**歷史演變：** lockdep 早期使用 DFS（與我們的做法相同），後來因 kernel stack 空間限制（通常 8-16KB）被 patch 改為 BFS。DFS 遞迴在深度大的依賴鏈上有 stack overflow 風險，而 BFS 用 circular queue 完全在 heap 上運作。

---

## 主管 Code Review 角度考題

### Q1：為什麼 Linux lockdep 從 DFS 改成 BFS？

**期望答案方向：** Kernel stack 通常只有 8-16KB，DFS 遞迴深度受限於鎖依賴鏈長度，若依賴鏈很深會 stack overflow。BFS 用 heap 上的 circular queue，不佔 stack 空間。

**追問：** 「你的 LeetCode 解法 V 最多 2000，DFS 遞迴深度 2000 在 user space 可以嗎？」→ 可以，user space 預設 stack 通常 8MB，2000 層完全沒問題。但在 kernel 或嵌入式環境就要小心。

**真正考察：** stack 空間意識、kernel vs user space 的差異。

### Q2：你的鄰接表每個 adj[i] 都 malloc 了 numCourses 大小，如何改善？

**期望答案方向：**
1. 兩趟掃描：第一趟計算每個節點的出度，第二趟按實際大小 malloc 並填入。
2. CSR（Compressed Sparse Row）格式：一個大的邊陣列 + offset 陣列，一次 malloc 搞定。
3. lockdep 的做法：用鏈結串列（`struct lock_list`），動態新增，不預分配。

**追問：** 「CSR 在 cache 行為上有什麼好處？」→ 連續記憶體，sequential access 對 cache line prefetch 友好。

**真正考察：** 記憶體效率意識、cache locality 理解。

### Q3：如果依賴關係是動態新增的（像 lockdep 一樣），你會怎麼改？

**期望答案方向：** 每次加邊時做一次單源可達性查詢（BFS/DFS 從新邊的目標節點出發，看能否到達新邊的來源節點）。不需要重建整張圖。時間複雜度 O(V + E) per edge insertion，但 amortized 通常比重建好很多。

**追問：** 「如果需要更快的線上環偵測呢？」→ 可以考慮 incremental topological sort 演算法（如 Marchetti-Spaccamela et al.），amortized O(m^{1/2}) per edge。

**真正考察：** 離線 vs 線上演算法的 trade-off，實務系統設計能力。

### Q4：malloc 失敗（回傳 NULL）的處理？

**期望答案方向：** Production code 中每個 `malloc` 都必須檢查回傳值。失敗時要 graceful cleanup（釋放已分配的資源）後回傳錯誤碼。Kernel 中對應的是 `kmalloc` 搭配 `goto cleanup` pattern。

**追問：** 「kernel 裡怎麼做 error cleanup？」→ `goto` label chain（`goto out_free_adj`, `goto out_free_state`），這是 kernel coding style 中被認可的 `goto` 使用情境。

**真正考察：** 防禦性程式設計、C 語言的記憶體管理紀律。

### Q5：state 陣列用 magic number 0/1/2，production code 會怎麼寫？

**期望答案方向：** 使用 `enum { WHITE = 0, GRAY = 1, BLACK = 2 };` 或 `#define`。函數命名 `dfs()` 太通用，應改成 `detect_cycle_dfs()` 或 `has_back_edge()`。`adj` 在 DFS 中只讀，可用 `const` 修飾。

**真正考察：** 程式碼可讀性意識、coding convention 嚴謹度。

### Q6：如果需要找出「所有」的環而不只是判斷有沒有環？

**期望答案方向：** Tarjan's SCC 演算法找所有 strongly connected components（SCC 中的節點都互相可達，包含所有環）。或 Johnson's algorithm 枚舉所有 elementary cycles。

**追問：** 「lockdep 需要找所有環嗎？」→ 不需要，lockdep 只需要在第一次偵測到環時報告那一條 dependency chain 就好，因為任何環都代表 potential deadlock，應該立即修復。

**真正考察：** 圖演算法深度、SCC 的概念。

---

## 面試加分總結

- **提及 lockdep 從 DFS 改 BFS 的歷史**，展示對 kernel stack 限制的理解，以及「教科書正確的演算法在 production 中不一定最佳」的工程判斷力。
- **比較離線 vs 線上環偵測**，展示系統設計層次的思考——LeetCode 是離線建完圖再跑，而 lockdep 是動態加邊即時偵測。
- **提到 CSR 格式或鏈結串列優化鄰接表**，展示記憶體效率和 cache locality 意識。
- **提到 `goto cleanup` pattern**，展示對 kernel coding style 的熟悉以及 C 語言記憶體管理的嚴謹度。
- **連結到後續問題**：LeetCode 210（Course Schedule II）= depmod 的完整拓撲排序輸出，自然帶出 Kahn's Algorithm 的優勢。

---

## Sources

- [kernel/locking/lockdep.c](https://github.com/torvalds/linux/blob/master/kernel/locking/lockdep.c)
- [include/linux/lockdep.h](https://github.com/torvalds/linux/blob/master/include/linux/lockdep.h)
- [include/linux/lockdep_types.h](https://github.com/torvalds/linux/blob/master/include/linux/lockdep_types.h)
- [kernel/module/main.c](https://github.com/torvalds/linux/blob/master/kernel/module/main.c)
- [LWN: lockdep BFS conversion](https://lwn.net/Articles/335950/)
- [kmod / depmod](https://github.com/kmod-project/kmod)
