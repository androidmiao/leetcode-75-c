# Topological Sort 拓樸排序模式

## 簡介

拓樸排序是 **有向無環圖 (DAG)** 頂點的線性排序，使得每條邊 `u → v` 在排序中 u 出現在 v 之前。實務上就是「先做 A 才能做 B」類型問題的通解。若圖中存在環則不存在拓樸序，因此拓樸排序也是環偵測的一種方法。

兩個主流演算法：
- **Kahn 算法（BFS 版）** — 從入度 0 的節點出發，每次「拿走」一個節點並更新鄰居入度
- **DFS + 後序反向** — DFS 結束離開節點時壓 stack，最後倒出來

複雜度皆為 O(V + E)。

## 辨識信號

- 題目提到「先修課」、「前置作業」、「依賴」、「順序」
- 需要判斷能否完成所有任務（是否有環）
- 需要輸出一種可行的完成順序
- 題目涉及 **有向圖**，且重點在「依賴關係」而非路徑長度
- 建構系統裡的建構順序（make / bazel / ninja）
- 編譯器的 module 載入順序

## 變體分類

### 1. 能否完成（環偵測）

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 207 | Course Schedule | Med | 回傳 bool，有環即 false |
| 261 | Graph Valid Tree | Med | 無向樹：n-1 邊 + 全連通 |

### 2. 輸出完成順序

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 210 | Course Schedule II | Med | 回傳排序結果；無解則回空陣列 |
| 269 | Alien Dictionary | Hard | 從字典序推斷字元順序，再拓樸 |
| 2115 | Find All Possible Recipes | Med | 字串節點 + hash map + Kahn |

### 3. 最短 / 最長路徑於 DAG

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 329 | Longest Increasing Path in Matrix | Hard | 視格子為節點，DAG 上 DP |
| 310 | Minimum Height Trees | Med | 拓樸 + 逐層剝離葉子 |

### 4. 並行排程 / 層級

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 1136 | Parallel Courses | Med | Kahn 按層處理，每層一學期 |
| 2050 | Parallel Courses III | Hard | 加權 DAG 最長路徑 |

## C 語言模板

### Kahn 算法（推薦，易處理無解情況）

```c
#include <stdlib.h>
#include <string.h>

/*
 * 輸入：numCourses, prerequisites (每個 [a, b] 代表 b → a 要先 b 後 a)
 * 輸出：拓樸順序；有環則 returnSize = 0
 */
int* findOrder(int numCourses, int** prerequisites, int prereqSize,
               int* prereqColSize, int* returnSize) {
    // 1. 建鄰接表 + 入度
    int* indegree = calloc(numCourses, sizeof(int));
    int** graph = malloc(numCourses * sizeof(int*));
    int*  gcap  = malloc(numCourses * sizeof(int));
    int*  glen  = calloc(numCourses, sizeof(int));
    for (int i = 0; i < numCourses; i++) {
        gcap[i] = 4;
        graph[i] = malloc(4 * sizeof(int));
    }
    for (int i = 0; i < prereqSize; i++) {
        int a = prerequisites[i][0];
        int b = prerequisites[i][1];          // b → a
        if (glen[b] == gcap[b]) {
            gcap[b] *= 2;
            graph[b] = realloc(graph[b], gcap[b] * sizeof(int));
        }
        graph[b][glen[b]++] = a;
        indegree[a]++;
    }

    // 2. 把所有入度 0 的節點丟進 queue
    int* queue = malloc(numCourses * sizeof(int));
    int  head = 0, tail = 0;
    for (int i = 0; i < numCourses; i++)
        if (indegree[i] == 0) queue[tail++] = i;

    // 3. BFS：每次彈出一個，減鄰居入度
    int* order = malloc(numCourses * sizeof(int));
    int  olen = 0;
    while (head < tail) {
        int u = queue[head++];
        order[olen++] = u;
        for (int k = 0; k < glen[u]; k++) {
            int v = graph[u][k];
            if (--indegree[v] == 0) queue[tail++] = v;
        }
    }

    // 4. 若 olen < n，代表有環
    if (olen != numCourses) {
        free(order); order = NULL; olen = 0;
    }
    *returnSize = olen;

    // 清理
    for (int i = 0; i < numCourses; i++) free(graph[i]);
    free(graph); free(gcap); free(glen); free(indegree); free(queue);
    return order;
}
```

### DFS + 後序反向版本

```c
bool dfs(int u, int** graph, int* glen, int* color, int* order, int* idx) {
    color[u] = 1;          // 1 = visiting (在當前 DFS stack)
    for (int k = 0; k < glen[u]; k++) {
        int v = graph[u][k];
        if (color[v] == 1) return false;             // 回到 visiting → 有環
        if (color[v] == 0 && !dfs(v, graph, glen, color, order, idx))
            return false;
    }
    color[u] = 2;          // 2 = done
    order[(*idx)++] = u;   // 後序壓 stack
    return true;
}
```
最後把 `order` 反向即為拓樸序。

## 常見陷阱

1. **邊方向搞反** — `[a, b]` 在 LeetCode 課程題通常是「先修 b 才能修 a」，所以邊是 `b → a`，a 的入度加 1。建圖前先白紙上畫一次。
2. **無解時 return 空** — 有環時 Kahn 的 `olen < n`，要正確回空陣列而不是部分結果。
3. **DFS 三色標記** — 必須用 0/1/2 三色（未訪問 / 訪問中 / 已完成）。只用 visited 一色無法偵測環。
4. **鄰接表動態增長** — LeetCode C 的 prereqSize 不定，記得 `realloc` 或先一輪算入度預分配。
5. **字串節點** — 若節點是字串（如 2115），先用 hash map 離散化。

## 面試講解

### 30 秒版

> 拓樸排序是在 DAG 上給出一種線性順序，讓每條邊都從前指向後。兩個經典作法：Kahn 用 BFS 從入度 0 出發，逐層剝離；DFS 則在回溯時壓 stack 最後反向。兩者都 O(V+E)。Kahn 更容易判無解（olen<n 即有環），DFS 更容易改成找最長路徑。

### 2 分鐘版

> 拓樸排序解決的是「依賴先後」類問題。典型例子是課程先修、makefile 建構順序、模組載入。
>
> 最常用的是 Kahn 演算法：算出每個節點的入度，把入度 0 的丟進 queue，每次彈出一個加進結果、把它指向的鄰居入度 -1，減到 0 就再丟進 queue。跑完如果結果數量 < n，代表有節點永遠沒被拿出來，也就是存在環。這是 O(V+E) 的線性演算法。
>
> DFS 的做法是三色標記：白、灰、黑。進入節點變灰，離開變黑。如果 DFS 過程中遇到灰色節點，代表回到祖先，有環。離開節點時壓進 stack，最後倒出來就是拓樸序。這個做法的好處是在 DAG 上直接跑最長路徑只要多一個 DP 陣列。
>
> Kernel 裡最接近的應用是 depmod：分析模組之間的 symbol 依賴，產生正確的 modprobe 順序。還有 lockdep：它在執行期間建「鎖的取用順序圖」，偵測環狀鎖序 — 實作用的是 BFS 而非 Kahn，因為它要一路追蹤 cycle 的具體路徑給 developer 看，不只判存不存在。
>
> 實務 C 實作兩個地雷：一是邊方向，[a, b] 在題目語意下通常是 b→a；二是鄰接表動態增長，記得 realloc 或先掃一次算入度預分配。

### 常見 follow-up

| 追問 | 回答要點 |
|------|----------|
| "唯一解還是多解？" | 多解一般。唯一解要求每一步入度 0 的節點只有一個 |
| "如何找字典序最小的拓樸序？" | Kahn 的 queue 換成 min-heap (priority queue) |
| "如何算每個任務的最早開始時間？" | 在拓樸序上做 DP：`earliest[v] = max(earliest[u] + w(u,v))` |
| "能否並行處理？" | 按層拓樸（Kahn 每輪處理所有入度 0 的節點）；層數 = 關鍵路徑長度 |
| "無向圖能否拓樸？" | 不能。拓樸序只對 DAG 有意義。樹可以取一個 root 做 DFS 產生類似順序 |
| "邊有權重影響順序嗎？" | 基本拓樸不看權重；若要最短 / 最長路徑，需在拓樸序上再做 DP |

## 和 Kernel 的關聯

Linux Kernel 有幾個真實的拓樸排序應用：

- **`depmod` / `kmod`（usermode 工具但屬 kernel ecosystem）** — 讀取 `.ko` 檔的 `modinfo.depends`，跑拓樸排序產生 modprobe 順序。寫在 `kmod` 套件裡。
- **`lockdep`（`kernel/locking/lockdep.c`）** — 動態建立 lock order graph，用 BFS 偵測循環鎖序。詳見 `kernel/graph-cycle.md`。
- **`initcall` 機制** — 透過 `__initcall()` 不同 level（pure、core、arch、subsys、fs、device、late）隱含出一個粗粒度的拓樸排序。完整順序由 linker section 控制，見 `include/linux/init.h`。
- **`driver core probe/remove`** — 驅動程式的探測 / 卸載需滿足 device tree 的父子順序；本質也是拓樸排序。

**設計約束觀察**：Kernel 傾向不使用一般化的拓樸排序 library，而是在每個子系統用最貼合的實作。例如 initcall 用 section 排序 O(1)，lockdep 用 BFS + bitmap 追路徑。這是 kernel 「每個情境選最小可行工具」的一貫風格。

## 參見

- [[bfs-dfs-graph]] — 拓樸的底層圖搜尋
- [[dynamic-programming]] — DAG 上的最短 / 最長路徑 DP
- [[../kernel/graph-cycle]] — lockdep 的循環鎖序偵測
- [[../interview/follow-up-patterns]] — 拓樸 follow-up 詳細清單
