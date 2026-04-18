# Union-Find 並查集模式

## 簡介

Union-Find（Disjoint Set Union, DSU）是一種追蹤「動態連通性」的資料結構。它支援兩個操作：`find(x)` 查詢 x 所屬集合的代表元；`union(x, y)` 合併兩個集合。加上 **路徑壓縮（path compression）** 與 **按秩合併（union by rank）** 兩種優化後，均攤複雜度接近 O(α(n))，α 是極慢成長的反 Ackermann 函數，實務上視為 O(1)。

## 辨識信號

出現以下任一，就該想到 Union-Find：

- 題目問「是否連通」、「連通分量有幾個」、「最少需要幾條邊連起來」
- 題目是 **動態圖**：邊是一條一條加進來的，需隨時回答連通性
- 需要偵測「冗餘邊 / 形成環的邊」
- 分組 / 等價類：朋友圈、相同身份、帳號合併
- 比 BFS/DFS 更適合的場景：**只關心連通性而不需要完整路徑**

反例：若題目需要最短路徑、需要實際走訪圖的結構，就用 BFS/DFS/Dijkstra，不是 UF。

## 變體分類

### 1. 基本連通性查詢

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 547 | Number of Provinces | Med | 直接計數 UF 的 root |
| 200 | Number of Islands | Med | 可用 UF 或 BFS/DFS；UF 更適合動態加島 |
| 990 | Satisfiability of Equality Equations | Med | 先處理 ==，再檢查 != |

### 2. 冗餘邊 / 環偵測

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 684 | Redundant Connection | Med | 逐條加邊，遇到已連通就是冗餘 |
| 685 | Redundant Connection II | Hard | 加上「入度 2」的特殊處理 |
| 261 | Graph Valid Tree | Med | n 節點 + n-1 邊 + 全部連通 ⇔ 樹 |

### 3. 帶權 / 加權 UF

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 399 | Evaluate Division | Med | 邊上帶比值權重，find 時累乘 |
| 737 | Sentence Similarity II | Med | 字串對映射 + UF |

### 4. 反向思考（倒序加邊）

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 803 | Bricks Falling When Hit | Hard | 正向刪難做，反向加邊後計算連通性變化 |
| 305 | Number of Islands II | Hard | 每次加一個陸地，動態更新連通分量 |

## C 語言模板

### 最簡版本（只做基本合併，無優化）

```c
#include <stdlib.h>

typedef struct {
    int* parent;
    int n;
} UF;

UF* uf_create(int n) {
    UF* u = malloc(sizeof(UF));
    u->parent = malloc(n * sizeof(int));
    u->n = n;
    for (int i = 0; i < n; i++) u->parent[i] = i;  // 每人自成一組
    return u;
}

int uf_find(UF* u, int x) {
    while (u->parent[x] != x) x = u->parent[x];
    return x;
}

void uf_union(UF* u, int a, int b) {
    int ra = uf_find(u, a), rb = uf_find(u, b);
    if (ra != rb) u->parent[ra] = rb;
}

void uf_free(UF* u) { free(u->parent); free(u); }
```

### 完整優化版本（路徑壓縮 + 按秩合併）

```c
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* parent;
    int* rank;        // 樹的高度（上界）
    int  count;       // 目前連通分量數
    int  n;
} UF;

UF* uf_create(int n) {
    UF* u = malloc(sizeof(UF));
    u->parent = malloc(n * sizeof(int));
    u->rank   = calloc(n, sizeof(int));
    u->count  = n;
    u->n      = n;
    for (int i = 0; i < n; i++) u->parent[i] = i;
    return u;
}

// 路徑壓縮：在 find 過程中把所有節點直接掛到 root
int uf_find(UF* u, int x) {
    if (u->parent[x] != x)
        u->parent[x] = uf_find(u, u->parent[x]);  // 遞迴 + 壓縮
    return u->parent[x];
}

// 按秩合併：把矮的樹掛到高的樹下，避免樹退化
bool uf_union(UF* u, int a, int b) {
    int ra = uf_find(u, a), rb = uf_find(u, b);
    if (ra == rb) return false;                   // 已連通，返回 false

    if (u->rank[ra] < u->rank[rb])       u->parent[ra] = rb;
    else if (u->rank[ra] > u->rank[rb])  u->parent[rb] = ra;
    else { u->parent[rb] = ra; u->rank[ra]++; }

    u->count--;
    return true;
}

bool uf_connected(UF* u, int a, int b) {
    return uf_find(u, a) == uf_find(u, b);
}

void uf_free(UF* u) { free(u->parent); free(u->rank); free(u); }
```

**實務建議**：
- 面試時「路徑壓縮」幾乎必寫，它讓單次操作攤銷近 O(1)。
- 「按秩合併」可用「按大小合併」代替（更直覺），效果相同。
- 若題目 n 上限 10^4 以內，甚至可以不做任何優化。能寫乾淨比能寫最快更重要。

## 常見陷阱

1. **忘記把 `parent[i] = i` 初始化** — 會指到 0，全部連通。
2. **用了路徑壓縮還遞迴太深** — C 遞迴無 tail call 保證，極端輸入可能爆 stack，改成 iterative 版本：
   ```c
   int uf_find(UF* u, int x) {
       int root = x;
       while (u->parent[root] != root) root = u->parent[root];
       while (u->parent[x] != root) {                 // 第二次走，每步指回 root
           int next = u->parent[x];
           u->parent[x] = root;
           x = next;
       }
       return root;
   }
   ```
3. **字串節點需先離散化** — 用 hash map 把字串映射到 0..n-1 的整數，再丟進 UF。
4. **動態擴展 n** — UF 一般要求事先知道 n。若 n 未知可用 `hash map<int, int>` 作為 parent，代價是常數變大。

## 面試講解

### 30 秒版

> Union-Find 是一個追蹤動態連通性的資料結構。核心兩個操作是 find 和 union。搭配路徑壓縮和按秩合併兩種優化後，均攤複雜度接近 O(1)。適合用在「連通分量計數」、「冗餘邊偵測」、以及任何動態加邊的圖題。相比 BFS/DFS，它不需要重新走訪整張圖，只要 O(α(n)) 就能回答連通性問題。

### 2 分鐘版

> 先從問題出發：給我一個圖，我一條一條地加邊，你得隨時告訴我有幾個連通分量 — 每加一條邊就跑一次 BFS 太浪費了，這就是 Union-Find 存在的原因。
>
> 資料結構本體是一個陣列 `parent`，`parent[i]` 指向 i 的「父節點」，每個集合的 root 指向自己。find(x) 就沿著 parent 往上爬直到遇到 root，union(a, b) 就把其中一邊的 root 掛到另一邊。
>
> 沒優化的話樹會退化成鏈，最壞 O(n)。所以加兩個優化：一是「路徑壓縮」，find 的時候順便把路上所有節點都直接指向 root，下次查就 O(1)；二是「按秩合併」，把矮的樹掛到高的樹下面，避免人為製造出深樹。兩個一起用，均攤是 O(α(n))，實務上算 O(1)。
>
> 在 Linux Kernel 裡面最像的概念是 `idr` 和 cgroup 階層的管理，但不是完全一樣的。真正用 UF 的是使用者空間的 `git merge-base`，它用 UF 的 variant 來算 LCA。

### 常見 follow-up

| 追問 | 回答要點 |
|------|----------|
| "如果節點是字串怎麼辦？" | 先用 hash map 離散化成 int，再丟 UF |
| "如果節點數事前不知道？" | 動態擴展 parent 陣列，或改用 hash map 當 parent |
| "如何支援 union 後再 split？" | 標準 UF 不支援，需改用 Link-Cut Tree 或持久化 UF |
| "如何算最小生成樹？" | Kruskal 演算法：把邊排序後用 UF 判斷是否形成環 |
| "如果加邊同時也會問是否有環？" | union 返回 false 就代表這條邊形成環（冗餘邊） |

## 和 Kernel 的關聯

Linux Kernel **主線程式碼中沒有通用 Union-Find 實作**（截至 v6.x）。原因：
1. Kernel 裡連通性問題罕見 — 大多數結構是樹或 DAG，而非動態圖。
2. 核心情境（如 lockdep 循環偵測）對「可預測延遲」要求高，傾向 BFS + bitmap 而非 UF 的攤銷複雜度。

但有幾個近親：
- **`include/linux/idr.h` / `ida`** — ID 配置器，概念上管「哪個 id 被誰佔用」，但不做合併。
- **BTRFS 子卷管理** — 透過 `struct btrfs_root` 的樹狀關係追蹤克隆與快照，概念類似「帶權 UF + 父指標」，但實作遠複雜於教科書 UF。
- **namespace 階層（user_namespace）** — 每個 namespace 指向 parent，查祖先的 pattern 就是 UF 的 find，但沒有 union 操作。

面試若要舉 Kernel 例子，**建議說「Kernel 不直接用 UF，常見的連通性問題（如 lockdep）選 BFS 是為了延遲可預測」**，比硬湊例子可信。

## 參見

- [[bfs-dfs-graph]] — 當題目需要實際走訪或最短路徑時的替代方案
- [[../kernel/graph-cycle]] — Kernel 為何選 BFS + bitmap 而非 UF
- [[../interview/follow-up-patterns]] — UF 相關 follow-up 的完整清單
