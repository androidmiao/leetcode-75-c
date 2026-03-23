# 207. Course Schedule — Implementation vs Editorial Comparison

## Implementation vs. Editorial Comparison

The current `.c` solution uses **DFS with 3-color marking** (WHITE/GRAY/BLACK) to detect cycles in the directed prerequisite graph. This corresponds directly to **Editorial Approach 2: Depth First Search**.

The editorial's DFS approach uses two separate boolean arrays (`visit[]` and `inStack[]`), while our implementation uses a single integer `state[]` array with three values (0, 1, 2). These are equivalent: `state == 0` means unvisited, `state == 1` means in the recursion stack (`inStack`), and `state == 2` means fully visited (`visit`). The single-array approach is slightly more memory-efficient and arguably cleaner in C.

The editorial also presents **Approach 1: Kahn's Algorithm (BFS-based topological sort)**, which is a fundamentally different method — it iteratively removes nodes with in-degree 0 and checks whether all nodes can be removed.

## Which Approach Is Better

| Criterion                  | DFS (3-color, our impl) | Kahn's Algorithm (BFS) |
|----------------------------|------------------------|------------------------|
| Time complexity            | O(V + E)               | O(V + E)               |
| Space complexity           | O(V + E)               | O(V + E)               |
| Interview suitability      | High                   | High                   |
| C implementation difficulty| Easy                   | Easy                   |
| Detects cycle location     | Yes (via recursion stack) | No (only detects existence) |
| Produces topological order | Not directly            | Yes (natural byproduct) |
| Recursion depth concern    | Yes (V up to 2000)     | No (iterative)         |

Both approaches are equally efficient in time and space. The key trade-off:

- **DFS** is more intuitive for pure cycle detection and naturally extends to problems like finding the actual cycle path. It's the standard graph DFS pattern that interviewers expect candidates to know.
- **Kahn's Algorithm** is iterative (no recursion stack overflow risk), and naturally produces a topological ordering — which directly extends to LeetCode 210 (Course Schedule II) where you need to return the order.

## Baseline vs. Recommended

- **Baseline approach:** DFS with 3-color marking (current implementation) — straightforward cycle detection, familiar DFS pattern.
- **Recommended approach for interview:** **Kahn's Algorithm (BFS)** — it is iterative (avoids potential stack overflow for large inputs), naturally produces the topological order (directly useful for the follow-up problem 210), and the BFS loop is easy to explain step by step in an interview setting. When an interviewer asks "can you also return the course order?", you just collect the dequeued nodes — zero extra work.

For this problem alone (just true/false), both are equally good. But Kahn's gives you more room to discuss extensions, making it the stronger interview choice.

---

## 中文翻譯

## 實作 vs. Editorial 比較

目前的 `.c` 解法使用 **DFS 三色標記法**（WHITE/GRAY/BLACK）來偵測有向先修圖中的環。這直接對應 **Editorial 方法二：深度優先搜尋**。

Editorial 的 DFS 方法使用兩個獨立的布林陣列（`visit[]` 和 `inStack[]`），而我們的實作用一個整數 `state[]` 陣列搭配三個值（0, 1, 2）。兩者等價：`state == 0` 表示未訪問，`state == 1` 表示在遞迴堆疊中（`inStack`），`state == 2` 表示已完全訪問（`visit`）。單一陣列的做法稍微節省記憶體，在 C 語言中也更簡潔。

Editorial 另外也介紹了**方法一：Kahn 演算法（BFS 拓撲排序）**，這是根本不同的方法 — 反覆移除入度為 0 的節點，檢查是否所有節點都能被移除。

## 哪個方法更好

| 比較項目            | DFS（三色標記，我們的實作） | Kahn 演算法（BFS）      |
|---------------------|---------------------------|------------------------|
| 時間複雜度          | O(V + E)                  | O(V + E)               |
| 空間複雜度          | O(V + E)                  | O(V + E)               |
| 面試適用性          | 高                        | 高                     |
| C 語言實作難度      | 簡單                      | 簡單                   |
| 偵測環的位置        | 可以（透過遞迴堆疊）       | 不行（只偵測是否存在）   |
| 產生拓撲排序        | 不直接                    | 是（自然副產物）        |
| 遞迴深度風險        | 有（V 最多 2000）          | 無（迭代式）           |

兩者的時間和空間效率相同。主要取捨：

- **DFS** 對純粹的環偵測更直覺，且自然延伸到找出實際的環路徑。這是面試官期望候選人掌握的標準圖 DFS 模式。
- **Kahn 演算法** 是迭代式的（沒有遞迴堆疊溢出的風險），且自然產生拓撲排序 — 可直接延伸到 LeetCode 210（Course Schedule II），需要返回課程順序。

## 基線 vs. 建議方案

- **基線方案：** DFS 三色標記（目前的實作）— 直截了當的環偵測，熟悉的 DFS 模式。
- **面試建議方案：** **Kahn 演算法（BFS）** — 迭代式（避免大輸入時的堆疊溢出），自然產生拓撲順序（對後續問題 210 直接有用），BFS 迴圈在面試中容易逐步解釋。當面試官問「你能不能也返回課程順序？」時，只需收集出佇列的節點 — 零額外工作量。

對於本題單獨而言（只需 true/false），兩者一樣好。但 Kahn 演算法給你更多討論延伸的空間，因此是更強的面試選擇。



https://leetcode.com/problems/course-schedule/description/?envType=problem-list-v2&envId=wl94y6ih https://leetcode.com/problems/course-schedule/editorial/?envType=problem-list-v2&envId=wl94y6ih 

```markdown
依據 SKILL.md 來初始化 207_Course Schedule
```

@207_Course Schedule_Description.md 請加上繁體中文翻譯, 這個動作請加入SKILL.md


@/Users/alex.miao/Documents/GitHub/leetcode-75-c/207_Course Schedule Linux相關為module dependency DAG、死鎖偵測的環偵測

```markdown
請列出Linux相關的code在哪裡？若以主管Code Review的角度, 可能會怎麼考?
```

---

## Linux Kernel 中的對應實作：環偵測在真實系統中的應用

LeetCode 207 的核心演算法（有向圖環偵測）在 Linux kernel 中至少有兩個重要的生產級應用場景：

### 1. lockdep — 死鎖偵測（Lock Dependency Validator）

**原始碼位置：** `kernel/locking/lockdep.c`

lockdep 是 Linux kernel 的執行期鎖定正確性驗證器，由 Ingo Molnar 撰寫。它的核心任務是偵測鎖的循環依賴（deadlock），與 LeetCode 207 的環偵測本質上是同一個圖論問題。

**運作原理：**

每當 thread 取得一把鎖，lockdep 記錄「持有 L1 的狀態下嘗試取得 L2」，形成 dependency edge：L1 → L2。所有鎖的依賴關係構成一張有向圖。新增一條邊 B → A 時，呼叫 `check_noncircular()` 檢查是否存在 A → ... → B 的路徑，若存在則形成環 A → ... → B → A，即 deadlock。

**關鍵函數：**

| 函數 | 功能 |
|------|------|
| `__bfs()` | 核心 BFS 遍歷，用 circular queue 實作，搜尋依賴圖中的目標節點 |
| `check_noncircular()` | 檢查新增邊是否造成強依賴環（deadlock） |
| `check_irq_usage()` | 檢查 irq-safe → irq-unsafe 的衝突 |

**與我們的 DFS 三色標記的對比：**

| 面向 | LeetCode 207（我們的實作） | lockdep |
|------|--------------------------|---------|
| 演算法 | DFS 三色標記 | BFS（circular queue） |
| 圖的建構 | 一次性建鄰接表 | 動態新增邊（每次鎖操作） |
| 偵測時機 | 離線（建完圖再跑） | 線上（每次加邊即時檢查） |
| 選 BFS 的理由 | N/A | BFS 用 circular queue 不吃 kernel stack；DFS 遞迴會消耗寶貴的 kernel stack space |
| 結果用途 | true/false | 印出完整 dependency chain 幫助 debug |

**歷史演變：** lockdep 早期用 DFS 實作（與我們的做法相同），後來被 patch 改為 BFS（見 LWN "kernel:lockdep:replace DFS with BFS"），主因是 kernel stack 空間有限（通常只有 8KB-16KB），DFS 遞迴在深度大的依賴鏈上有 stack overflow 風險。BFS 用 circular queue 則完全在 heap 上運作。

### 2. depmod — 核心模組依賴解析

**原始碼位置：** `tools/depmod.c`（kmod 專案）

depmod 負責掃描 `/lib/modules/<kernel_version>/` 下所有模組，分析 symbol export/import 關係，產生 `modules.dep` 依賴檔。

**運作原理：** 解析每個 `.ko` 模組的 exported symbols 和 required symbols，建立模組間的依賴有向圖，執行拓撲排序決定載入順序（`dep_sort_idx` 欄位）。若偵測到循環依賴，直接報錯 fatal，因為環狀依賴的模組無法決定載入順序。

**與 LeetCode 207 的對應：** 模組 = 課程（vertex），symbol 依賴 = 先修關係（edge），能否產生合法載入順序 = canFinish() 回傳 true/false。depmod 實際上還要輸出順序，對應 LeetCode 210（Course Schedule II）。

### 3. 其他環偵測應用場景

systemd unit 依賴（service 之間的 Requires/After 關係也構成 DAG）、package manager（apt/yum/pacman）的套件依賴解析、build system（Make/CMake/Bazel）的 target 依賴圖等。

---

## 主管 Code Review 角度：可能怎麼考？

以一個主管或 senior engineer 做 Code Review 的角度，針對我們的 `207_Course Schedule.c` 實作，可能會從以下幾個層面提問或考察：

### 第一層：正確性與邊界條件

1. **空圖處理：** `numCourses > 0` 但 `prerequisitesSize == 0` 時行為正確嗎？→ 是的，所有節點保持 WHITE，迴圈跑完不會觸發 has_cycle，返回 true。
2. **自環（self-loop）：** `prerequisites = [[0,0]]` 能偵測到嗎？→ 可以，`adj[0][0] = 0`，DFS 進入 0 後標記 GRAY，遍歷鄰居時遇到 state[0]==1，回傳 false。
3. **孤立節點：** 有些課程完全沒有先修關係也沒被當先修 → 無影響，它們各自 DFS 一輪就變 BLACK。

### 第二層：記憶體管理（C 特有的重點）

4. **鄰接表過度分配：** 每個 `adj[i]` 都 malloc 了 `numCourses * sizeof(int)`，最壞是 O(V²) 記憶體。Review 常見問題：「為什麼不用兩趟掃描（first pass 計算 degree，second pass 填入），或用動態陣列？」
5. **提前返回時的記憶體洩漏：** 目前 `has_cycle = true` 後會跳出迴圈走到 free 區塊，這是正確的。但如果改成在 DFS 中直接 `return false`，就會洩漏。Reviewer 會注意這類 pattern。
6. **`calloc` vs `malloc` + 手動初始化：** `state` 陣列用 `malloc` 再手動迴圈清零，可以用 `calloc(numCourses, sizeof(int))` 一行搞定。

### 第三層：效能與可擴展性

7. **遞迴深度：** 題目限制 V ≤ 2000，DFS 遞迴深度最壞 2000 層。在 kernel context 下（stack 只有 8-16KB）這不可接受，這正是 lockdep 改用 BFS 的原因。面試中提到這個 trade-off 是加分項。
8. **能否改成迭代式 DFS？** 用顯式 stack 取代遞迴，避免 stack overflow。
9. **BFS（Kahn's）替代方案的優勢：** Reviewer 可能會問「你知道不用遞迴的做法嗎？」→ 指向 Kahn's Algorithm。

### 第四層：Production Code 品質

10. **Magic numbers：** `state` 的 0、1、2 應該用 `enum` 或 `#define`：`enum { WHITE = 0, GRAY = 1, BLACK = 2 };`
11. **函數命名：** `dfs()` 太通用，production code 中應叫 `detect_cycle_dfs()` 或 `has_back_edge()` 這類自描述命名。
12. **錯誤處理：** `malloc` 回傳 NULL 時完全沒有檢查。Production code 中必須處理 OOM。
13. **const 修飾：** `adj` 在 DFS 中只讀，可以傳 `const int* const*`，明確表達 intent。

### 第五層：系統設計延伸（Staff+ 層級）

14. **若依賴關係是動態新增的呢？** → lockdep 的做法：每次加邊時做一次 BFS 檢查，不需要重建整張圖。
15. **若需要輸出所有的環呢？** → Johnson's algorithm 或 Tarjan's SCC 找所有 strongly connected components。
16. **若圖很大（百萬節點）呢？** → 考慮 Kahn's（迭代，memory-friendly），或分區平行處理。

---

## 中文摘要：Linux 核心相關原始碼位置

| 應用場景 | 原始碼路徑 | 環偵測演算法 |
|----------|-----------|-------------|
| 死鎖偵測（lockdep） | `kernel/locking/lockdep.c` | BFS（circular queue），核心函數 `__bfs()`、`check_noncircular()` |
| 模組依賴排序（depmod） | `tools/depmod.c`（kmod 專案） | 拓撲排序 + 環偵測，`dep_sort_idx` |
| 鎖依賴追蹤資料結構 | `include/linux/lockdep.h` | `lock_class`、`lock_list`（含 BFS parent 指標） |

**關鍵啟示：** Linux kernel 的 lockdep 早期和我們一樣用 DFS，後來因為 kernel stack 空間限制改成 BFS。這正好呼應了上半部的結論 — Kahn's（BFS）在 production 環境中是更穩健的選擇。
