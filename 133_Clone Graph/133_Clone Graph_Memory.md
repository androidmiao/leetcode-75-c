# LeetCode 133 Clone Graph — 記憶體位址與指標圖解

## 問題概述

複製一個無向圖。給定圖的節點，返回複製後的圖。原始圖範例：
- 節點 1：neighbors = [2, 4]
- 節點 2：neighbors = [1, 3]
- 節點 3：neighbors = [2, 4]
- 節點 4：neighbors = [1, 3]

此圖形成一個正方形：`1──2──3──4──1`

---

## 1. Struct 記憶體佈局

### C 結構定義

```c
struct Node {
    int val;                    // 4 bytes (offset: 0)
    int numNeighbors;           // 4 bytes (offset: 4)
    struct Node** neighbors;    // 8 bytes (offset: 8) — pointer to pointer array
};
// 總大小：16 bytes
```

### 單一節點記憶體配置圖（假設起始位址 0xA000）

```
Address (0xA000 為例)：

0xA000  ┌─────────────────────┐
        │   val: 1 (4 bytes)  │  偏移量: +0
        │   [0x00 0x00 0x00 0x01]
0xA004  ├─────────────────────┤
        │ numNeighbors: 2     │  偏移量: +4
        │ (4 bytes)           │  [0x00 0x00 0x00 0x02]
0xA008  ├─────────────────────┤
        │ neighbors: 0xB000   │  偏移量: +8
        │ (8 bytes, 指標)     │  [0x00 0x00 0x00 0x00 0x00 0x0B 0x00 0x00]
0xA010  └─────────────────────┘
```

### 欄位對齐與填充
- 結構體在 64-bit 系統上通常按 8 bytes 對齐
- 由於最大欄位為指標（8 bytes），無額外填充需求
- 結構體總大小：**16 bytes**

---

## 2. 完整資料結構記憶體配置

### 原始圖四個節點的位址映射

| 節點 | 值 | 記憶體位址 | neighbors 陣列位址 | numNeighbors |
|------|-----|-----------|-------------------|--------------|
| Node 1 | 1 | 0xA000 | 0xB000 | 2 |
| Node 2 | 2 | 0xA100 | 0xB020 | 2 |
| Node 3 | 3 | 0xA200 | 0xB040 | 2 |
| Node 4 | 4 | 0xA300 | 0xB060 | 2 |

### 原始圖記憶體佈局 ASCII 圖

```
┌─────────────────────────────────────────────────────────────────┐
│                    Node 1 (0xA000)                              │
├─────────────────────────────────────────────────────────────────┤
│  val=1  │  numNeighbors=2  │  neighbors───▶ 0xB000             │
└────┬────────────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────┐
│  neighbors[0] ───┐  │ (0xB000)
│  neighbors[1] ─┐ │  │
└─────┬───────────┼──┘
      │           │
      ▼           ▼
    Node 2      Node 4
   (0xA100)    (0xA300)
    val=2        val=4
    numNeighbors=2
    neighbors───▶ 0xB020
        │
        ├───▶ Node 1 (0xA000)
        └───▶ Node 3 (0xA200)
             val=3
             numNeighbors=2
             neighbors───▶ 0xB040
                 ├───▶ Node 2 (0xA100)
                 └───▶ Node 4 (0xA300)


┌─────────────────────────────────────────────────────────────────┐
│                    原始圖邏輯結構                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│     ┌────▶ Node 2 (0xA100) ◀────┐                             │
│     │        (val=2)       │      │                            │
│     │                      ▼      │                            │
│  Node 1 (0xA000)     Node 3 (0xA200)                          │
│  (val=1)             (val=3)                                  │
│     │                      ▲      │                            │
│     │                      └──────│                            │
│     └────▶ Node 4 (0xA300) ◀────┘                             │
│            (val=4)                                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Neighbors 陣列記憶體詳細配置

```
0xB000 (Node 1 的 neighbors 陣列)
┌─────────────────────┐
│ [0]: 0xA100 (Node 2)│  偏移量 +0
├─────────────────────┤
│ [1]: 0xA300 (Node 4)│  偏移量 +8
└─────────────────────┘

0xB020 (Node 2 的 neighbors 陣列)
┌─────────────────────┐
│ [0]: 0xA000 (Node 1)│  偏移量 +0
├─────────────────────┤
│ [1]: 0xA200 (Node 3)│  偏移量 +8
└─────────────────────┘

0xB040 (Node 3 的 neighbors 陣列)
┌─────────────────────┐
│ [0]: 0xA100 (Node 2)│  偏移量 +0
├─────────────────────┤
│ [1]: 0xA300 (Node 4)│  偏移量 +8
└─────────────────────┘

0xB060 (Node 4 的 neighbors 陣列)
┌─────────────────────┐
│ [0]: 0xA000 (Node 1)│  偏移量 +0
├─────────────────────┤
│ [1]: 0xA200 (Node 3)│  偏移量 +8
└─────────────────────┘
```

---

## 3. DFS 搜尋與複製過程追蹤

### 演算法概述

使用 DFS 遞迴搜尋，一個 visited 陣列跟蹤已複製的節點。

```c
struct Node* cloneGraph(struct Node* node) {
    if (!node) return NULL;

    struct Node** visited = (struct Node**)malloc(101 * sizeof(struct Node*));
    memset(visited, 0, 101 * sizeof(struct Node*));

    return dfs(node, visited);
}

struct Node* dfs(struct Node* node, struct Node** visited) {
    if (visited[node->val]) {
        return visited[node->val];
    }

    struct Node* clone = createNode(node->val);
    visited[node->val] = clone;

    for (int i = 0; i < node->numNeighbors; i++) {
        addNeighbor(clone, dfs(node->neighbors[i], visited));
    }

    return clone;
}
```

### 執行追蹤（假設從 Node 1 開始，0xA000）

#### 步驟 1：進入 dfs(Node 1, visited[])

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 1：dfs(0xA000, visited)                                │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[1] == 0 (NULL) ？ YES，未訪問過             │
│ 動作：                                                      │
│   • 分配新 Clone Node 1' 於 0xC000                        │
│   • clone->val = 1                                          │
│   • clone->numNeighbors = 0 (待填)                         │
│   • visited[1] = 0xC000                                    │
│                                                             │
│ visited 陣列狀態：                                          │
│   visited[1] = 0xC000  (Clone Node 1)                      │
│   visited[2] = NULL                                         │
│   visited[3] = NULL                                         │
│   visited[4] = NULL                                         │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 2：遞迴呼叫 neighbors[0]（Node 2，0xA100）

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 2：dfs(0xA100, visited)  [Node 2 在記憶體位址 0xA100] │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[2] == 0 (NULL) ？ YES，未訪問過             │
│ 動作：                                                      │
│   • 分配新 Clone Node 2' 於 0xC100                        │
│   • clone->val = 2                                          │
│   • visited[2] = 0xC100                                    │
│                                                             │
│ 記憶體狀態：                                                │
│   原始 Node 1 (0xA000)  ──neighbors[0]──▶  0xA100         │
│                                                             │
│   Clone Node 1' (0xC000) ──neighbors[0]──▶  0xC100 (待配) │
│                                                             │
│ visited 陣列狀態：                                          │
│   visited[1] = 0xC000  (Clone Node 1)                      │
│   visited[2] = 0xC100  (Clone Node 2)                      │
│   visited[3] = NULL                                         │
│   visited[4] = NULL                                         │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 3：遞迴呼叫 Node 2 的 neighbors[0]（Node 1，0xA000）

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 3：dfs(0xA000, visited)  [Node 1 再次訪問]           │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[1] == 0xC000 (已存在) ？ YES，已訪問        │
│ 動作：                                                      │
│   • 直接返回 visited[1] = 0xC000                          │
│   • 無需重複建立                                            │
│                                                             │
│ 指標連結：                                                  │
│   Clone Node 2' (0xC100) ──neighbors[0]──▶ 0xC000         │
│                                           (Clone Node 1')   │
│                                                             │
│ visited 陣列狀態：(無變化)                                  │
│   visited[1] = 0xC000                                       │
│   visited[2] = 0xC100                                       │
│   visited[3] = NULL                                         │
│   visited[4] = NULL                                         │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 4：回到 Node 2，遞迴呼叫 neighbors[1]（Node 3，0xA200）

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 4：dfs(0xA200, visited)  [Node 3 新節點]             │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[3] == NULL ？ YES，未訪問過                 │
│ 動作：                                                      │
│   • 分配新 Clone Node 3' 於 0xC200                        │
│   • clone->val = 3                                          │
│   • visited[3] = 0xC200                                    │
│                                                             │
│ 指標連結：                                                  │
│   Clone Node 2' (0xC100) ──neighbors[1]──▶ 0xC200         │
│                                           (Clone Node 3')   │
│                                                             │
│ visited 陣列狀態：                                          │
│   visited[1] = 0xC000                                       │
│   visited[2] = 0xC100                                       │
│   visited[3] = 0xC200  (Clone Node 3) ◀── 新增             │
│   visited[4] = NULL                                         │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 5：Node 3 的 neighbors[0]（Node 2，0xA100）—— 已訪問

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 5：dfs(0xA100, visited)  [Node 2 已訪問]             │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[2] == 0xC100 (已存在) ？ YES               │
│ 動作：                                                      │
│   • 返回 visited[2] = 0xC100                              │
│                                                             │
│ 指標連結：                                                  │
│   Clone Node 3' (0xC200) ──neighbors[0]──▶ 0xC100         │
│                                           (Clone Node 2')   │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 6：Node 3 的 neighbors[1]（Node 4，0xA300）

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 6：dfs(0xA300, visited)  [Node 4 新節點]             │
├─────────────────────────────────────────────────────────────┤
│ 檢查：visited[4] == NULL ？ YES，未訪問過                 │
│ 動作：                                                      │
│   • 分配新 Clone Node 4' 於 0xC300                        │
│   • clone->val = 4                                          │
│   • visited[4] = 0xC300                                    │
│                                                             │
│ 指標連結：                                                  │
│   Clone Node 3' (0xC200) ──neighbors[1]──▶ 0xC300         │
│                                           (Clone Node 4')   │
│                                                             │
│ visited 陣列狀態：                                          │
│   visited[1] = 0xC000                                       │
│   visited[2] = 0xC100                                       │
│   visited[3] = 0xC200                                       │
│   visited[4] = 0xC300  (Clone Node 4) ◀── 新增             │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 7-10：Node 4 的 neighbors 遞迴

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 7：dfs(0xA000, visited)  [Node 1 已訪問]             │
├─────────────────────────────────────────────────────────────┤
│ 返回 visited[1] = 0xC000                                   │
│ Clone Node 4' (0xC300) ──neighbors[0]──▶ 0xC000           │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ 步驟 8：dfs(0xA200, visited)  [Node 3 已訪問]             │
├─────────────────────────────────────────────────────────────┤
│ 返回 visited[3] = 0xC200                                   │
│ Clone Node 4' (0xC300) ──neighbors[1]──▶ 0xC200           │
└─────────────────────────────────────────────────────────────┘
```

#### 步驟 9：回到 Node 1 的 neighbors[1]（Node 4，0xA300）—— 已訪問

```
┌─────────────────────────────────────────────────────────────┐
│ 步驟 9：dfs(0xA300, visited)  [Node 4 已訪問]             │
├─────────────────────────────────────────────────────────────┤
│ 返回 visited[4] = 0xC300                                   │
│ Clone Node 1' (0xC000) ──neighbors[1]──▶ 0xC300           │
│                                           (Clone Node 4')   │
└─────────────────────────────────────────────────────────────┘
```

#### 最終複製圖結構

```
┌─────────────────────────────────────────────────────────────┐
│                  複製後的圖邏輯結構                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│     ┌────▶ Clone Node 2' (0xC100) ◀────┐                 │
│     │            (val=2)          │     │                 │
│     │                             ▼     │                 │
│  Clone Node 1'              Clone Node 3'                 │
│  (0xC000)                   (0xC200)                      │
│  (val=1)                    (val=3)                       │
│     │                             ▲     │                 │
│     │                             └─────│                 │
│     └────▶ Clone Node 4' (0xC300) ◀────┘                 │
│            (val=4)                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. 指標變數追蹤總表

| 步驟 | 當前節點<br/>記憶體位址 | 節點值 | 動作 | visited[1] | visited[2] | visited[3] | visited[4] |
|------|--------------------------|--------|------|-----------|-----------|-----------|-----------|
| 1 | 0xA000 (Node 1) | 1 | 建立 Clone 1' (0xC000) | 0xC000 | NULL | NULL | NULL |
| 2 | 0xA100 (Node 2) | 2 | 建立 Clone 2' (0xC100) | 0xC000 | 0xC100 | NULL | NULL |
| 3 | 0xA000 (Node 1) | 1 | 已訪問，返回 0xC000 | 0xC000 | 0xC100 | NULL | NULL |
| 4 | 0xA200 (Node 3) | 3 | 建立 Clone 3' (0xC200) | 0xC000 | 0xC100 | 0xC200 | NULL |
| 5 | 0xA100 (Node 2) | 2 | 已訪問，返回 0xC100 | 0xC000 | 0xC100 | 0xC200 | NULL |
| 6 | 0xA300 (Node 4) | 4 | 建立 Clone 4' (0xC300) | 0xC000 | 0xC100 | 0xC200 | 0xC300 |
| 7 | 0xA000 (Node 1) | 1 | 已訪問，返回 0xC000 | 0xC000 | 0xC100 | 0xC200 | 0xC300 |
| 8 | 0xA200 (Node 3) | 3 | 已訪問，返回 0xC200 | 0xC000 | 0xC100 | 0xC200 | 0xC300 |
| 9 | 0xA300 (Node 4) | 4 | 已訪問，返回 0xC300 | 0xC000 | 0xC100 | 0xC200 | 0xC300 |

### Visited 陣列記憶體佈局

```
visited 陣列（大小 101，malloc 於 0xD000）

0xD000  ┌─────────────────────────────────────────┐
        │ [0]: NULL                               │
0xD008  ├─────────────────────────────────────────┤
        │ [1]: 0xC000  (Clone Node 1)            │  ◀── 指向 Clone Node 1'
0xD010  ├─────────────────────────────────────────┤
        │ [2]: 0xC100  (Clone Node 2)            │  ◀── 指向 Clone Node 2'
0xD018  ├─────────────────────────────────────────┤
        │ [3]: 0xC200  (Clone Node 3)            │  ◀── 指向 Clone Node 3'
0xD020  ├─────────────────────────────────────────┤
        │ [4]: 0xC300  (Clone Node 4)            │  ◀── 指向 Clone Node 4'
0xD028  ├─────────────────────────────────────────┤
        │ [5..100]: NULL                          │
        │                                         │
0xD1F8  └─────────────────────────────────────────┘
```

---

## 5. 記憶體存取模式比較

### 方案 1：陣列式 Visited（本實作）

```
┌─────────────────────────────────────────────────────────────────┐
│ 陣列式 Visited: int visited[101]                               │
├─────────────────────────────────────────────────────────────────┤
│ 優點：                                                          │
│  • O(1) 查詢時間：直接索引 visited[node->val]                  │
│  • 連續記憶體存取，快取命中率高                                │
│  • 無 hash 函數計算開銷                                        │
│  • 存取模式可預測，CPU 預取有效                               │
│                                                                 │
│ 缺點：                                                          │
│  • 固定空間 101 * 8 = 808 bytes（不論節點數量）               │
│  • 若節點值不連續，浪費記憶體                                  │
│  • 必須預知最大節點值                                          │
│                                                                 │
│ 快取行為：                                                      │
│  典型 CPU 快取行大小：64 bytes                                 │
│  單個指標：8 bytes                                             │
│  一行可容納：64 / 8 = 8 個條目                                │
│  存取 visited[1] → visited[7] 皆在同一快取行 ✓              │
└─────────────────────────────────────────────────────────────────┘
```

#### 陣列式記憶體存取圖

```
讀取序列：visited[1] → visited[2] → visited[3] → visited[4]

快取行 0 (0xD000 - 0xD03F, 64 bytes):
┌──┬──┬──┬──┬──┬──┬──┬──┐
│[0]│[1]│[2]│[3]│[4]│[5]│[6]│[7]│  第一次快取命中
│  │ ✓ │ ✓ │ ✓ │ ✓ │   │   │   │  無額外快取失誤
└──┴──┴──┴──┴──┴──┴──┴──┘

時間軸：
    L1 Hit (visited[1])
    L1 Hit (visited[2])
    L1 Hit (visited[3])
    L1 Hit (visited[4])
```

### 方案 2：Hash 表式 Visited（uthash）

```
┌─────────────────────────────────────────────────────────────────┐
│ Hash 表式 Visited: uthash                                      │
├─────────────────────────────────────────────────────────────────┤
│ 優點：                                                          │
│  • 動態大小，只佔用 O(n) 空間（n = 節點數）                    │
│  • 節點值無需連續，更靈活                                      │
│  • 無需預知最大值                                              │
│                                                                 │
│ 缺點：                                                          │
│  • O(1) 平均查詢，但有雜湊衝突風險                             │
│  • hash 函數計算開銷 (~10-20 CPU cycles)                      │
│  • 記憶體非連續分佈，快取命中率低                             │
│  • 存取模式不可預測，CPU 預取無效                             │
│  • 每個條目額外開銷 (~32 bytes for hash node)                 │
│                                                                 │
│ 空間開銷：                                                      │
│  4 個節點 → ~4 * (8 + 32) = 160 bytes（vs 陣列 808 bytes）   │
│  100 個節點 → ~100 * 40 = 4000 bytes（vs 陣列 808 bytes）    │
│  但若有稀疏值，hash 表優勢明顯                                │
│                                                                 │
│ 快取行為：                                                      │
│  節點散佈在記憶體各處                                          │
│  visited[key1] → visited[key2] 可能跨越多個快取行            │
│  大量快取失誤，記憶體延遲較高                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### Hash 表記憶體存取圖

```
雜湊表結構（開放定址法或鏈結法示意）：

記憶體分佈（不連續）：
0x1A00  Node 1 hash entry (計算地址需要 hash 函數)
0x2B00  Node 2 hash entry (記憶體距離大，快取失誤)
0x3C00  Node 3 hash entry
0x4D00  Node 4 hash entry

讀取序列：visited[1] → visited[2] → visited[3]

時間軸：
    計算 hash(1) → L1 Hit (visited[1])
    計算 hash(2) → L1 Miss (traversal to 0x2B00)
                → 等待 ~10-100 cycles
    計算 hash(3) → L1 Miss (traversal to 0x3C00)
                → 等待 ~10-100 cycles
```

### 性能對比總結

| 指標 | 陣列式 | Hash 表式 |
|------|--------|----------|
| 查詢時間（平均） | O(1), ~1-3 cycles | O(1), ~15-30 cycles |
| 空間（節點數=4） | 808 bytes | 160 bytes |
| 空間（節點數=100） | 808 bytes | 4000 bytes |
| 快取命中率 | 高（~95%) | 低（~30%) |
| 記憶體頻寬使用 | 高效 | 低效 |
| 預測性 | 可預測 | 不可預測 |
| 實現複雜度 | 簡單 | 複雜 |

---

## 6. DFS 遞迴堆疊追蹤

### 遞迴堆疊深度分析

在最壞情況下（鏈表形圖），堆疊深度 = 節點數。
在本例（正方形圖），最深堆疊深度為 **4**。

### 最深堆疊點 ASCII 堆疊圖

```
堆疊底層                      堆疊頂層
(Frame 0)                     (Frame 3)
  ▼                             ▼

┌────────────────────┐
│  main() Frame      │  (Frame 0)
├────────────────────┤
│ 返回位址: ...      │
│ 區域變數:          │
│   visited ─────────────▶ 0xD000
│ 呼叫：cloneGraph() │
└────────────────────┘
          △
          │ 呼叫 cloneGraph(0xA000)
          │
┌────────────────────┐
│  cloneGraph Frame  │  (Frame 1)
├────────────────────┤
│ 返回位址: main     │
│ 參數:              │
│   node ─────────────▶ 0xA000 (原 Node 1)
│ 區域變數:          │
│   visited ─────────────▶ 0xD000
│ 呼叫：dfs()        │
└────────────────────┘
          △
          │ 呼叫 dfs(0xA000, 0xD000)
          │
┌────────────────────┐
│   dfs Frame 1      │  (Frame 2)
├────────────────────┤
│ 返回位址: cloneGraph
│ 參數:              │
│   node ─────────────▶ 0xA000 (Node 1)
│   visited ─────────────▶ 0xD000
│ 區域變數:          │
│   clone ────────────▶ 0xC000 (Clone Node 1')
│   i: 0             │
│ 呼叫：dfs()        │
└────────────────────┘
          △
          │ 呼叫 dfs(0xA100, 0xD000)  [neighbors[0]]
          │
┌────────────────────────────┐
│     dfs Frame 2            │  (Frame 3) ◀── 最深堆疊點
├────────────────────────────┤
│ 返回位址: dfs Frame 1      │
│ 參數:                      │
│   node ────────────────────▶ 0xA100 (Node 2)
│   visited ────────────────▶ 0xD000
│ 區域變數:                  │
│   clone ──────────────────▶ 0xC100 (Clone Node 2')
│   i: 0                     │
│ 呼叫：dfs(0xA000, ...)     │
│        ↓                   │
│      (already visited)     │
│      dfs(0xA200, ...)      │ ◀── 此次呼叫進入 Frame 4
└────────────────────────────┘
          △
          │ 呼叫 dfs(0xA200, 0xD000)  [neighbors[1]]
          │
┌────────────────────────────┐
│     dfs Frame 3            │  (Frame 4)
├────────────────────────────┤
│ 返回位址: dfs Frame 2      │
│ 參數:                      │
│   node ────────────────────▶ 0xA200 (Node 3)
│   visited ────────────────▶ 0xD000
│ 區域變數:                  │
│   clone ──────────────────▶ 0xC200 (Clone Node 3')
│   i: 0                     │
│ 遞迴呼叫 dfs(0xA100, ...) │ (已訪問，快速返回)
└────────────────────────────┘
```

### 堆疊記憶體佈局（假設堆疊起始 0x7FFF0000，向下生長）

```
堆疊高位址 (Frame 0)
0x7FFF0000  ┌───────────────────┐
            │  main() Frame      │  大小: ~64 bytes
0x7FFEFF40  ├───────────────────┤
            │ cloneGraph Frame   │  大小: ~32 bytes
0x7FFEFF20  ├───────────────────┤
            │   dfs Frame 1      │  大小: ~48 bytes (node, visited, clone, i)
0x7FFEFF00  ├───────────────────┤
            │   dfs Frame 2      │  大小: ~48 bytes
0x7FFEFEE0  ├───────────────────┤
            │   dfs Frame 3      │  大小: ~48 bytes ◀── SP (堆疊指標)
0x7FFEFEC0  ├───────────────────┤
            │ 未使用             │
            │
堆疊低位址 (Frame N)
```

### 每個 Frame 的詳細佈局

```
單個 dfs Frame 記憶體佈局（以 Frame 2 為例）：

┌────────────────────────────────────┐
│ 0x7FFEFF20  返回位址 (8 bytes)     │  0x400456 (dfs Frame 1 指令)
├────────────────────────────────────┤
│ 0x7FFEFF28  參數 node (8 bytes)    │  0xA000 (原 Node 2 位址)
├────────────────────────────────────┤
│ 0x7FFEFF30  參數 visited (8 bytes) │  0xD000 (visited 陣列位址)
├────────────────────────────────────┤
│ 0x7FFEFF38  區域變數 clone (8 bytes)│  0xC100 (Clone Node 2')
├────────────────────────────────────┤
│ 0x7FFEFF40  區域變數 i (4 bytes)   │  0 (迴圈計數)
├────────────────────────────────────┤
│ 0x7FFEFF44  填充 (4 bytes)         │  (對齊)
└────────────────────────────────────┘
  Frame 大小: 48 bytes
```

### 遞迴呼叫序列與堆疊變化時間圖

```
時間軸 →

dfs(0xA000)  ┐
             ├──▶ 深度 1
             │
    dfs(0xA100)  ┐
                 ├──▶ 深度 2
                 │
        dfs(0xA000) [已訪問，快速返回]

        dfs(0xA200)  ┐
                     ├──▶ 深度 3
                     │
            dfs(0xA100) [已訪問，快速返回]

            dfs(0xA300)  ┐
                         ├──▶ 深度 4 ◀── 最大深度
                         │
                dfs(0xA000) [已訪問，快速返回]
                dfs(0xA200) [已訪問，快速返回]

                ◀── 返回到深度 3


堆疊大小：
┌─────┐
│4 * 48 bytes = 192 bytes (最大時) │
│ (4 層 dfs 框架，每層 ~48 bytes)  │
└─────┘
```

### 堆疊使用情況總結

| 深度 | 框架內容 | 堆疊指標(SP) | 堆疊佔用 |
|------|---------|-----------|---------|
| 0 | main() | 0x7FFF0000 | ~64 bytes |
| 1 | cloneGraph() | 0x7FFEFF40 | ~32 bytes |
| 2 | dfs(0xA000) | 0x7FFEFF20 | ~48 bytes |
| 3 | dfs(0xA100) | 0x7FFEFF00 | ~48 bytes |
| 4 | dfs(0xA200) | 0x7FFEFEE0 | ~48 bytes |
| **最大** | **深度 4** | **0x7FFEFEE0** | **~240 bytes** |

---

## 7. 完整程式碼實作與記憶體詮釋

### 程式碼

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    int val;
    int numNeighbors;
    struct Node** neighbors;
};

struct Node* dfs(struct Node* node, struct Node** visited) {
    // 檢查 visited[node->val] 是否已建立複製
    // 位址存取：visited + node->val * sizeof(struct Node*)
    if (visited[node->val]) {
        return visited[node->val];
    }

    // 分配新節點（heap 上）
    struct Node* clone = (struct Node*)malloc(sizeof(struct Node));
    clone->val = node->val;
    clone->numNeighbors = node->numNeighbors;
    clone->neighbors = (struct Node**)malloc(node->numNeighbors * sizeof(struct Node*));

    // 記錄到 visited 陣列
    visited[node->val] = clone;

    // 遞迴複製所有鄰接點
    for (int i = 0; i < node->numNeighbors; i++) {
        clone->neighbors[i] = dfs(node->neighbors[i], visited);
    }

    return clone;
}

struct Node* cloneGraph(struct Node* node) {
    if (!node) return NULL;

    // 分配 visited 陣列（stack）
    // 大小 101 * 8 = 808 bytes
    struct Node** visited = (struct Node**)malloc(101 * sizeof(struct Node*));
    memset(visited, 0, 101 * sizeof(struct Node*));

    struct Node* result = dfs(node, visited);

    free(visited);
    return result;
}
```

### 記憶體存取詮釋

```
行 15: visited[node->val]
       ↓
       位址計算：visited_base_addr + node->val * 8
       = 0xD000 + 1 * 8 = 0xD008 (for node->val == 1)
       ↓
       讀取 8 bytes ▶ 獲得指標值（或 NULL）
       CPU 快取行：0xD000-0xD03F（包含該位址）
       快取命中機率：高 (空間局部性)

行 20: malloc(sizeof(struct Node))
       ↓
       系統呼叫，分配 16 bytes 於 heap
       返回 0xC000, 0xC100, 0xC200, 0xC300...
       ↓
       初始化結構體欄位：
       *(clone + 0) = node->val  [寫入 4 bytes 於 0xC000+0]
       *(clone + 4) = node->numNeighbors [寫入 4 bytes 於 0xC000+4]
       *(clone + 8) = neighbors_ptr [寫入 8 bytes 於 0xC000+8]

行 25: malloc(node->numNeighbors * 8)
       ↓
       分配 neighbors 陣列於 heap
       clone->neighbors 指標更新
       地址寫入位於 clone 結構體的 +8 offset

行 28: visited[node->val] = clone
       ↓
       位址計算：0xD000 + node->val * 8
       寫入 8 bytes clone 指標
       ↓
       快取行為：寫入快取（write-back 或 write-through）
```

---

## 8. 總結與重點

### 記憶體地址統計

```
原始圖 (Original Graph):
  Node 1-4: 0xA000 - 0xA300   (4 × 16 bytes = 64 bytes)
  Neighbors 陣列: 0xB000 - 0xB070  (~128 bytes)
  小計：~192 bytes

複製圖 (Cloned Graph):
  Clone Node 1'-4': 0xC000 - 0xC300  (64 bytes)
  Clone Neighbors 陣列: 0xB100 - 0xB170  (~128 bytes)
  小計：~192 bytes

Visited 陣列:
  0xD000 - 0xD1F8  (101 × 8 = 808 bytes)

總計：~1200 bytes (假設)
```

### 指標流向圖

```
main()
  ↓ 呼叫 cloneGraph(node)
  ├─ node ─────────▶ 0xA000 (原 Node 1)
  ├─ visited ──────▶ 0xD000 (visited 陣列)
  ├─ 呼叫 dfs(0xA000, 0xD000)
  │
  ├─ visited[1] ──────────▶ 0xC000 (Clone Node 1)
  ├─ visited[2] ──────────▶ 0xC100 (Clone Node 2)
  ├─ visited[3] ──────────▶ 0xC200 (Clone Node 3)
  ├─ visited[4] ──────────▶ 0xC300 (Clone Node 4)
  │
  └─ 返回 0xC000 (複製圖的起始節點)
```

### 快取局部性分析

**時間局部性**（Temporal Locality）：
- visited 陣列重複存取，高命中率
- 同一節點多次檢查（visited 檢查，建立，返回）

**空間局部性**（Spatial Locality）：
- visited 陣列連續存取：visited[1] → visited[2] → visited[3] → visited[4]
- neighbors 陣列連續存取：neighbors[0] → neighbors[1]
- 整體快取行為優良

### DFS 特性

- **時間複雜度**：O(V + E)（每個節點與邊訪問一次）
- **空間複雜度**：O(V)（visited 陣列 + 遞迴堆疊）
- **堆疊深度**：O(V)（最壞情況，鏈表形圖）
- **記憶體碎片化**：moderate（heap 分配多個小物件）

---

## 附註

- 所有記憶體位址（0xA000, 0xB000, 0xC000, 0xD000 等）為**假設值**，實際運行時會不同
- 結構體大小與對齐受平台影響（本例基於 64-bit Linux/x86-64）
- visited 陣列大小固定為 101（對應節點值範圍 1-100），可改進為動態大小或 hash 表
- 圖形假設連通且有向邊存在（節點 1 指向 2，2 指向 1 等循環存在）
