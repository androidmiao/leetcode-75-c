# LeetCode 210. Course Schedule II — Linux 相關程式碼與 Code Review 觀點

## Linux 中的拓撲排序在哪裡？

### 1. `depmod` — 核心模組依賴排序

`depmod` 的原始碼在 `kmod` 專案中（以前在 `module-init-tools`）。核心邏輯在：

- `kmod` repo → `tools/depmod.c`
- 它解析每個 `.ko` 模組的 `depends` 欄位，建出模組間的依賴圖，然後做拓撲排序輸出 `modules.dep` 檔案
- 內核載入模組時，`modprobe` 讀這份排序好的依賴表，按拓撲順序載入

實際上 `depmod` 用的也是 Kahn 風格的做法——先算出每個模組的「被依賴數」，從沒有依賴的模組開始逐層展開。

### 2. `systemd` — 服務啟動順序

systemd 的依賴解析在：

- `src/core/job.c` 和 `src/core/transaction.c`
- Unit 之間的 `Requires=`, `After=`, `Before=` 形成有向圖
- systemd 在 `transaction_activate()` 時做環偵測 + 拓撲排序，決定啟動/停止順序
- 遇到循環依賴時，systemd 會嘗試打破環（drop non-essential edges）並印出警告

### 3. 其他 Linux 場景

- `tsort` 命令（`coreutils` 裡的 `src/tsort.c`）：最純粹的通用拓撲排序工具
- `make` 的依賴建構順序
- `apt`/`dpkg` 的套件安裝排序（Debian `dpkg` 原始碼中的 `trigproc.c`）

---

## 主管 Code Review 角度會怎麼考？

如果以 senior engineer 審這份 `210_Course Schedule II.c`，可能從以下幾個方向提問或挑戰：

### 正面肯定的部分

- 三色標記法是教科書級的正確做法，環偵測與拓撲排序融為一體
- 註解品質極高，ASCII diagram 與步驟拆解讓新人也能讀懂
- `has_cycle` 的剪枝提前結束所有遞迴，效率意識不錯

### 會追問的點

1. **`static bool has_cycle` 的執行緒安全性** — 在 Linux kernel 或 systemd 這種多執行緒環境裡，全域 static 變數是大忌。主管會問：「如果兩個 caller 同時呼叫 `findOrder`，會怎樣？」正確答案是把 `has_cycle` 包進一個 context struct 傳遞，或改用回傳值傳遞。

2. **記憶體配置策略** — `adj[i] = malloc(numCourses * sizeof(int))` 為每門課配了最壞情況的空間。主管會問：「如果 `numCourses = 100,000`，你配了 100K × 100K = 10G 的記憶體，能接受嗎？」更好的做法是先掃一遍算出每個節點的出度，再精確配置；或者用一個扁平陣列 + offset 表（Linux kernel 常見的 CSR 格式）。

3. **遞迴深度** — `numCourses` 最多 2000，DFS 遞迴最深 2000 層。主管會問：「如果 V = 100,000 怎麼辦？」在 Linux kernel 裡 stack 只有 8KB（或 16KB），遞迴深度是致命問題。這也是 systemd 和 depmod 偏好 Kahn 迭代式的原因之一。

4. **`has_cycle` 時 `result` 仍被 return** — 雖然 `*returnSize = 0`，但你 malloc 了 `result` 又 return 它。主管會問：「caller 是否知道要 free 這塊記憶體？API contract 清楚嗎？」在 LeetCode judge 中沒問題，但在真實系統中這是 memory leak 的溫床。

5. **「為什麼不用 Kahn？」** — 結合 Linux 實務，主管可能會問你在什麼場景下 DFS 比 Kahn 更適合。好的回答是：當你需要同時做 SCC（強連通分量）或需要 DFS tree 資訊時，DFS 框架更容易延伸；但純拓撲排序的 production code 通常偏好 Kahn。

### 如果要改寫成 production-grade Linux code

- 消滅所有 global state（`has_cycle` → context struct）
- 用 CSR（Compressed Sparse Row）取代 pointer-to-pointer 鄰接表
- 改成迭代式（顯式 stack 或 Kahn's）
- 加入錯誤回傳路徑（`malloc` 失敗時的處理）
- 考慮 `restrict` 修飾和 cache locality

這些就是 `.c` 解法在跨出 LeetCode 進入真實 Linux 系統時，會遇到的 code review 重點。演算法本身沒問題，差異全在工程品質的要求層級上。
