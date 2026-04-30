# 146. LRU Cache — 記憶體位址 / 陣列索引圖解

> 以下所有記憶體位址皆為**假設值**，用於說明指標、雜湊表 bucket 陣列與雙向鏈結串列之間的關係。
> 為了讓示意圖能在一頁內呈現，雜湊表大小以 `HASH_SIZE = 5` 演示；實際 `.c` 程式碼使用 `HASH_SIZE = 10007`。

---

## 1. Struct 記憶體佈局

`.c` 解法用三種 struct：`LRUCache`（容器）、`Node`（雙向鏈結串列節點）、`HashNode`（雜湊表 bucket 鏈節點）。

### 1.1 `Node` — 雙向鏈結串列節點

```c
typedef struct Node {
    int key;            // 4 bytes
    int val;            // 4 bytes
    struct Node *prev;  // 8 bytes
    struct Node *next;  // 8 bytes
} Node;                 // Total: 24 bytes
```

```
            Node @ 0xB000  (key=1, val=1)
        ┌────────────────────────────────┐
0xB000  │  key  = 1                      │  4 bytes
        ├────────────────────────────────┤
0xB004  │  val  = 1                      │  4 bytes
        ├────────────────────────────────┤
0xB008  │  prev = 0xA000  (head 哨兵)     │  8 bytes (指標)
        ├────────────────────────────────┤
0xB010  │  next = 0xA018  (tail 哨兵)     │  8 bytes (指標)
        └────────────────────────────────┘
```

### 1.2 `HashNode` — 雜湊表碰撞鏈節點

```c
typedef struct HashNode {
    int key;                // 4 bytes  + 4 bytes padding (對齊指標)
    Node *node;             // 8 bytes
    struct HashNode *next;  // 8 bytes
} HashNode;                 // Total: 24 bytes (含 4 bytes padding)
```

```
            HashNode @ 0xE000
        ┌────────────────────────────────┐
0xE000  │  key  = 1                      │  4 bytes
        ├────────────────────────────────┤
0xE004  │  (padding)                     │  4 bytes 對齊
        ├────────────────────────────────┤
0xE008  │  node = 0xB000  ──────────────► 指向 Node(key=1)
        ├────────────────────────────────┤
0xE010  │  next = NULL    (碰撞鏈尾)      │  8 bytes
        └────────────────────────────────┘
```

### 1.3 `LRUCache` — 容器

```c
typedef struct {
    int capacity;               // 4 bytes
    int size;                   // 4 bytes
    Node *head;                 // 8 bytes
    Node *tail;                 // 8 bytes
    HashNode *table[HASH_SIZE]; // 8 × HASH_SIZE bytes
} LRUCache;
```

```
            LRUCache obj @ 0x9000  (HASH_SIZE = 5 範例)
        ┌────────────────────────────────┐
0x9000  │  capacity = 2                  │  4 bytes
        ├────────────────────────────────┤
0x9004  │  size     = 0                  │  4 bytes
        ├────────────────────────────────┤
0x9008  │  head = 0xA000  ─────────────► head 哨兵
        ├────────────────────────────────┤
0x9010  │  tail = 0xA018  ─────────────► tail 哨兵
        ├────────────────────────────────┤
0x9018  │  table[0] = NULL               │  bucket 0
        ├────────────────────────────────┤
0x9020  │  table[1] = NULL               │  bucket 1
        ├────────────────────────────────┤
0x9028  │  table[2] = NULL               │  bucket 2
        ├────────────────────────────────┤
0x9030  │  table[3] = NULL               │  bucket 3
        ├────────────────────────────────┤
0x9038  │  table[4] = NULL               │  bucket 4
        └────────────────────────────────┘
```

---

## 2. 完整資料結構記憶體配置（初始狀態）

### 2.1 物件位址對應表

| 物件 | 假設位址 | 大小 | 說明 |
|------|----------|------|------|
| `LRUCache obj` | 0x9000 | 16 + 8×5 = 56 bytes | 容器（含 5-bucket 雜湊表） |
| `head` 哨兵 | 0xA000 | 24 bytes | 永不刪除 |
| `tail` 哨兵 | 0xA018 | 24 bytes | 永不刪除 |
| Node `key=1` | 0xB000 | 24 bytes | put(1,1) 時配置 |
| Node `key=2` | 0xC000 | 24 bytes | put(2,2) 時配置 |
| Node `key=3` | 0xD000 | 24 bytes | put(3,3) 時配置 |
| Node `key=4` | 0xF000 | 24 bytes | put(4,4) 時配置 |
| HashNode (k=1) | 0xE000 | 24 bytes | put(1,1) 時配置 |
| HashNode (k=2) | 0xE100 | 24 bytes | put(2,2) 時配置 |
| HashNode (k=3) | 0xE200 | 24 bytes | put(3,3) 時配置 |
| HashNode (k=4) | 0xE300 | 24 bytes | put(4,4) 時配置 |

### 2.2 初始狀態 ASCII 全景圖（capacity=2，尚未 put 任何 key）

```
  ┌─────── LRUCache obj @ 0x9000 ───────┐
  │ capacity=2  size=0                  │
  │ head=0xA000 ─┐    tail=0xA018 ─┐    │
  │ table[0..4] = {NULL, NULL, NULL, NULL, NULL}
  └───────────────┼──────────────────┼──┘
                  │                  │
                  ▼                  ▼
       0xA000 ┌────────┐       0xA018 ┌────────┐
              │ HEAD   │              │ TAIL   │
              │ prev=NULL              prev=0xA000
              │ next=0xA018  ───────►  next=NULL
              └────────┘              └────────┘

  雙向鏈結串列：HEAD ←─────────────► TAIL  （兩哨兵互指，串列為空）
  雜湊表：       全 bucket 為 NULL
```

---

## 3. 陣列索引版對應（雜湊表 bucket 陣列）

LRU Cache 的雙向鏈結串列**沒有**陣列索引版本（無法用 `2*i+1` 之類的公式），但**雜湊表 bucket 陣列**正是「陣列索引 + 鏈結」的混合存取方式：

### 3.1 索引公式

```c
static int hash(int key) {
    return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
}
```

`HASH_SIZE = 5` 時：

| key | `key % 5` | bucket index | bucket 位址 |
|-----|-----------|--------------|--------------|
| 1   | 1         | 1            | `&table[1]` = 0x9020 |
| 2   | 2         | 2            | `&table[2]` = 0x9028 |
| 3   | 3         | 3            | `&table[3]` = 0x9030 |
| 4   | 4         | 4            | `&table[4]` = 0x9038 |

陣列元素位址計算：`&table[i] = 0x9018 + i × 8`（指標陣列，每元素 8 bytes）。

### 3.2 陣列 → 鏈結混合存取示意

```
   bucket 陣列（O(1) 直接索引）        碰撞鏈（O(L) 線性掃描，L=鏈長）
   ┌──────────────────┐
   │ table[0] = NULL  │
   ├──────────────────┤      ┌─ HashNode ─┐    ┌─ HashNode ─┐
   │ table[1] = 0xE000├─────►│ key=1      │───►│ key=10006  │──► NULL
   ├──────────────────┤      │ node=0xB000│    │ node=0xZZZZ│
   │ table[2] = 0xE100├──┐   └────────────┘    └────────────┘
   ├──────────────────┤  │           ▲ 此鏈表示「不同 key 雜湊到同 bucket」
   │ table[3] = NULL  │  │
   ├──────────────────┤  │   ┌─ HashNode ─┐
   │ table[4] = NULL  │  └──►│ key=2      │──► NULL
   └──────────────────┘      │ node=0xC000│
                             └────────────┘

  存取 key=1 流程：
    step 1: i = hash(1) = 1   → O(1)（陣列下標計算）
    step 2: 走訪 table[1] 鏈   → O(L) 找 key=1 的條目
    step 3: 取出 entry->node   → 拿到 Node*，可直接操作鏈結串列
```

關鍵設計：**雜湊表（陣列）負責定位，雙向鏈結串列（指標）負責排序**。兩者透過 `HashNode->node` 指標串起來，缺一不可。

---

## 4. Example 1 操作過程追蹤

Example 1：`LRUCache(2)`，依序執行
`put(1,1) → put(2,2) → get(1) → put(3,3) → get(2) → put(4,4) → get(1) → get(3) → get(4)`，
預期回傳：`[null, null, null, 1, null, -1, null, -1, 3, 4]`。

### Step 0 — `LRUCache(2)` 初始化

`lRUCacheCreate(2)` 配置 LRUCache + head + tail，所有 bucket = NULL。對應 §2.2 全景圖。

```
  HEAD(0xA000) ◄═══════════════════► TAIL(0xA018)
  size=0   table = {NULL, NULL, NULL, NULL, NULL}
```

---

### Step 1 — `put(1, 1)`

1. `hashFind(1)` → `hash(1)=1`，掃 `table[1]` chain → NULL → 回傳 NULL（不存在）。
2. `malloc(Node)` 在 0xB000 配置新 Node `(1,1)`。
3. `hashInsert(1, 0xB000)`：`malloc(HashNode)` 在 0xE000，`entry->next = table[1] = NULL`，`table[1] = 0xE000`。
4. `insertBeforeTail(0xB000)`：

```
  修改 4 個指標：
    Node(0xB000)->prev = tail->prev = 0xA000  (HEAD)
    Node(0xB000)->next = tail = 0xA018
    HEAD(0xA000)->next = 0xB000     (原本指向 0xA018)
    TAIL(0xA018)->prev = 0xB000     (原本指向 0xA000)
```

5. `size = 1`，未超過 capacity=2，不淘汰。

```
  鏈結串列：HEAD(0xA000) ◄═► [Node 0xB000 k=1 v=1] ◄═► TAIL(0xA018)
  雜湊表：  table[1] ──► HashNode(0xE000 k=1, node=0xB000, next=NULL)
            其他 bucket 仍為 NULL
```

---

### Step 2 — `put(2, 2)`

`hashFind(2)` → `hash(2)=2`，table[2]=NULL → 不存在。
配置 Node `(2,2)` @0xC000，HashNode @0xE100，`table[2] = 0xE100`。
`insertBeforeTail(0xC000)` 把新節點插到 TAIL 之前。`size=2`，正好等於 capacity，不淘汰。

```
  指標修改細節（insertBeforeTail）：
    0xC000->prev = 0xB000        ← 原最新節點
    0xC000->next = 0xA018        ← TAIL
    0xB000->next = 0xC000        ← 原最新節點的 next 改指向新節點
    0xA018->prev = 0xC000        ← TAIL 的 prev 改指向新節點

  HEAD(0xA000) ◄═► [0xB000 k=1 v=1] ◄═► [0xC000 k=2 v=2] ◄═► TAIL(0xA018)
                       ↑LRU 端                ↑MRU 端

  雜湊表：
    table[1] ──► HashNode(0xE000, k=1, node=0xB000)
    table[2] ──► HashNode(0xE100, k=2, node=0xC000)
```

---

### Step 3 — `get(1)` → 回傳 `1`

`hashFind(1)` → `table[1]` chain → `entry->key==1` → 回傳 `entry->node = 0xB000`。
`removeNode(0xB000)`：

```
  removeNode 修改 2 個指標（不釋放記憶體）：
    0xB000->prev->next = 0xB000->next
      → HEAD(0xA000)->next = 0xC000
    0xB000->next->prev = 0xB000->prev
      → 0xC000->prev = 0xA000

  暫時狀態：HEAD ◄═► [0xC000 k=2] ◄═► TAIL
            (節點 0xB000 已從串列摘除，但記憶體還在)
```

`insertBeforeTail(0xB000)` 重新插到 TAIL 前：

```
  insertBeforeTail 修改 4 個指標：
    0xB000->prev = 0xC000        ← 目前最新節點
    0xB000->next = 0xA018        ← TAIL
    0xC000->next = 0xB000
    0xA018->prev = 0xB000

  HEAD ◄═► [0xC000 k=2 v=2] ◄═► [0xB000 k=1 v=1] ◄═► TAIL
              ↑新 LRU 端              ↑新 MRU 端

  回傳 0xB000->val = 1
```

雜湊表完全沒動（因為 Node 位址沒變，HashNode->node 仍正確）。

---

### Step 4 — `put(3, 3)` → **觸發淘汰**

`hashFind(3)` → `table[3]=NULL` → 不存在。
配置 Node `(3,3)` @0xD000，HashNode @0xE200，`table[3] = 0xE200`。
`insertBeforeTail(0xD000)`：

```
  HEAD ◄═► [0xC000 k=2] ◄═► [0xB000 k=1] ◄═► [0xD000 k=3] ◄═► TAIL
  size = 3 > capacity = 2  →  必須淘汰 LRU
```

淘汰流程：

```
  lru = HEAD->next = 0xC000  (key=2)

  step A: removeNode(0xC000)
    HEAD->next = 0xB000
    0xB000->prev = 0xA000

  step B: hashRemove(key=2)
    h = hash(2) = 2
    cur = table[2] = 0xE100
    cur->key == 2 → 從 chain 摘除（此 chain 只有一個條目）
    table[2] = cur->next = NULL
    free(0xE100)              ← HashNode 釋放

  step C: free(0xC000)        ← Node 釋放
  size = 2

  最終：HEAD ◄═► [0xB000 k=1 v=1] ◄═► [0xD000 k=3 v=3] ◄═► TAIL
        table[1] ──► HashNode(k=1, node=0xB000)
        table[2] = NULL                          ← 已清空
        table[3] ──► HashNode(0xE200, k=3, node=0xD000)
```

---

### Step 5 — `get(2)` → 回傳 `-1`

`hashFind(2)` → `table[2] = NULL` → 直接回傳 NULL → `get` 回傳 -1。記憶體與鏈結均不變。

---

### Step 6 — `put(4, 4)` → **再次觸發淘汰**

當前狀態：`HEAD ◄═► [0xB000 k=1] ◄═► [0xD000 k=3] ◄═► TAIL`，LRU 端是 0xB000 (k=1)。

```
  step 1: hashFind(4) → NULL（table[4]=NULL）
  step 2: malloc Node(4,4) @ 0xF000
  step 3: malloc HashNode @ 0xE300, table[4] = 0xE300
  step 4: insertBeforeTail(0xF000)
          → HEAD ◄═► [0xB000 k=1] ◄═► [0xD000 k=3] ◄═► [0xF000 k=4] ◄═► TAIL
          size = 3 > 2 → 淘汰
  step 5: lru = HEAD->next = 0xB000 (key=1)
          removeNode(0xB000); hashRemove(1) → free 0xE000, table[1]=NULL; free(0xB000)

  最終：HEAD ◄═► [0xD000 k=3 v=3] ◄═► [0xF000 k=4 v=4] ◄═► TAIL
        table = {NULL, NULL, NULL, 0xE200(k=3), 0xE300(k=4)}
```

---

### Step 7–9 — `get(1) → -1`，`get(3) → 3`，`get(4) → 4`

- `get(1)`：`table[1]=NULL` → -1。
- `get(3)`：`table[3]` chain → 找到 `node=0xD000`，移到 TAIL 前。
  ```
  removeNode(0xD000); insertBeforeTail(0xD000)
  → HEAD ◄═► [0xF000 k=4] ◄═► [0xD000 k=3] ◄═► TAIL
  回傳 0xD000->val = 3
  ```
- `get(4)`：`table[4]` chain → `node=0xF000`，移到 TAIL 前。
  ```
  removeNode(0xF000); insertBeforeTail(0xF000)
  → HEAD ◄═► [0xD000 k=3] ◄═► [0xF000 k=4] ◄═► TAIL
  回傳 0xF000->val = 4
  ```

---

## 5. 指標變數追蹤總表

### 5.1 每步驟的指標位址 / 值 / 操作

| Step | 操作 | 修改的指標 | 舊值 → 新值 | 回傳 |
|------|------|-----------|-------------|------|
| 0 | `LRUCache(2)` | `HEAD->next`, `TAIL->prev` | `?, ? → 0xA018, 0xA000` | obj |
| 1 | `put(1,1)` | `HEAD->next`, `TAIL->prev`, `0xB000->prev`, `0xB000->next`, `table[1]` | 4 條串列指標 + 1 條 bucket 指標 | null |
| 2 | `put(2,2)` | `0xB000->next`, `TAIL->prev`, `0xC000->prev`, `0xC000->next`, `table[2]` | 同上模式 | null |
| 3 | `get(1)` | `removeNode(0xB000)` 改 2 條，`insertBeforeTail(0xB000)` 改 4 條 | 共 6 條指標 | **1** |
| 4 | `put(3,3)` | `insertBeforeTail` 4 條 + `removeNode(0xC000)` 2 條 + `table[2]=NULL` | 共 7 條 | null |
| 5 | `get(2)` | 無 | — | **-1** |
| 6 | `put(4,4)` | `insertBeforeTail` 4 + `removeNode(0xB000)` 2 + `table[1]=NULL` | 共 7 條 | null |
| 7 | `get(1)` | 無 | — | **-1** |
| 8 | `get(3)` | `removeNode + insertBeforeTail` | 共 6 條 | **3** |
| 9 | `get(4)` | `removeNode + insertBeforeTail` | 共 6 條 | **4** |

### 5.2 統計

| 統計項目 | 數值 |
|---------|------|
| 總指標寫入次數 | 約 38 次 |
| `malloc` 次數 | 4 Node + 4 HashNode + 1 LRUCache + 2 sentinel = 11 |
| `free` 次數 | 2 Node + 2 HashNode = 4（淘汰兩次） |
| 雜湊表查找次數 | 9（每次 `get`/`put` 各一次） |
| 鏈結串列掃描次數 | **0**（這是 LRU 的核心優勢：絕不掃描串列） |

---

## 6. 記憶體存取模式比較

| 維度 | 雙向鏈結串列（指標） | 雜湊表 bucket 陣列（混合） |
|------|---------------------|---------------------------|
| 存取方式 | `node->next` / `node->prev` 隨機跳躍 | `table[i]` 連續陣列 + 內部鏈結 |
| Cache locality | **差** — 節點散落各處 | bucket 陣列連續（佳），鏈內較差 |
| 空間額外開銷 | 每節點 16 bytes（兩個指標） | 每 HashNode 16 bytes + 4 bytes padding |
| O(1) 操作 | 給定節點指標可 O(1) 移除/插入 | 給定 key 平均 O(1) 查找 |
| 為何兩者結合？ | 串列無法 O(1) 找到「key=X 的節點」 | 雜湊表無法 O(1) 維護「最近使用順序」 |

雙向鏈結串列「節點散落」的代價是使用碎片化記憶體；好處是「持有指標就能 O(1) 修改鏈結」。**雜湊表記住節點位址 → 鏈結串列負責排序** 的組合，是 LRU 必須這樣設計的根本原因。

---

## 7. 堆疊 vs. 堆積（Stack vs. Heap）記憶體對比

LRU Cache 的所有資料都在 **heap**（透過 `malloc`），堆疊只用於函式呼叫。

```
  ┌──── 函式呼叫堆疊（呼叫 lRUCachePut 時）─────┐
  │ lRUCachePut frame                          │
  │   - obj  : LRUCache* (8 bytes)             │  區域變數都是
  │   - key  : int       (4 bytes)             │  指標或基本型別
  │   - value: int       (4 bytes)             │  → 不會遞迴
  │   - node : Node*     (8 bytes)             │  → 堆疊深度恆為 O(1)
  │   - newNode/lru ...                        │
  └─────────────────────────────────────────────┘

  ┌──── 堆積（Heap）─────────────────────┐
  │ LRUCache obj  @ 0x9000                │
  │ HEAD/TAIL 哨兵 @ 0xA000 / 0xA018       │
  │ Node × N      @ 0xB000, 0xC000, ...   │ ← 動態增減
  │ HashNode × N  @ 0xE000, 0xE100, ...   │ ← 動態增減
  └────────────────────────────────────────┘
```

對比遞迴版（例如 DFS 走訪 N-ary tree）會在堆疊堆積 O(深度) 個 frame；LRU 完全是迴圈，堆疊使用 **O(1)**，是 production code 偏好的設計。

---

## 8. 退化情況分析（碰撞鏈過長）

當不同 key 全部雜湊到同一個 bucket 時，雜湊表退化為單向鏈結串列：

```
  最壞情況：所有 key 都使 hash(key) == 1

  table[1] ──► HN(k=1) ──► HN(k=6) ──► HN(k=11) ──► HN(k=16) ──► ... ──► NULL
              |←──────────── chain length L = N ────────────→|

  每次 hashFind 退化為 O(N) → get/put 退化為 O(N)
```

實際 `.c` 程式碼用 `HASH_SIZE = 10007`（質數）對抗 `1 ≤ capacity ≤ 3000` 的題目限制，使期望 chain 長度約 `3000 / 10007 ≈ 0.3`，極低。

防退化策略對比：

| 策略 | 本實作 | Linux kernel `rhashtable` | Java `HashMap` |
|------|--------|--------------------------|----------------|
| 偵測碰撞 | 不偵測（容量上限固定） | 監測 load factor，動態 resize | load factor > 0.75 自動 resize |
| 處理長鏈 | 接受退化 | 雙表 + RCU 漸進搬遷 | 鏈長 ≥ 8 時轉紅黑樹（O(log N)） |
| 記憶體成本 | 固定 80 KB（10007 × 8） | 隨資料動態調整 | 隨資料動態調整 |

對於 LeetCode 題目這已足夠；對於 production 系統（如 page cache、dentry cache），通常需要動態 resize 機制。

---

## 9. 雙向鏈結串列「為何雙向」的記憶體層面解釋

```
  單向鏈結串列：HEAD ──► A ──► B ──► C ──► NULL
  若要刪除 B，必須從 HEAD 走到 A（B 的前驅），才能把 A->next 改成 C。
  → 給定 B 的指標，刪除仍是 O(N)。

  雙向鏈結串列：HEAD ◄═► A ◄═► B ◄═► C ◄═► TAIL
  刪除 B：B->prev->next = B->next; B->next->prev = B->prev;
  → 純粹兩次指標寫入，與串列長度無關，**O(1)**。
```

LRU Cache 的 `removeNode` 之所以 O(1)，正是因為「拿到 Node 指標 + 雙向鏈結」這兩個條件同時成立。少任何一個都退回 O(N)。
