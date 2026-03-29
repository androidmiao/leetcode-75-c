# 232. Implement Queue using Stacks — 記憶體位址與陣列索引圖解

> 所有位址均為假設值，僅供理解指標與記憶體佈局之用。

---

## 一、Struct 記憶體佈局

### `MyQueue` 結構體定義

```c
typedef struct {
    int* inbox;       /* 8 bytes（64-bit 指標） */
    int* outbox;      /* 8 bytes（64-bit 指標） */
    int inbox_top;    /* 4 bytes */
    int outbox_top;   /* 4 bytes */
    int capacity;     /* 4 bytes */
} MyQueue;            /* 總計 28 bytes + padding = 32 bytes */
```

### 單一 `MyQueue` 實例記憶體佈局（假設基址 0xA000）

```
          MyQueue @ 0xA000
          ┌────────────────────────────────────┐
  0xA000  │  inbox       = 0xC000              │  8 bytes（指向 inbox 陣列）
          ├────────────────────────────────────┤
  0xA008  │  outbox      = 0xD000              │  8 bytes（指向 outbox 陣列）
          ├────────────────────────────────────┤
  0xA010  │  inbox_top   = -1                  │  4 bytes
          ├────────────────────────────────────┤
  0xA014  │  outbox_top  = -1                  │  4 bytes
          ├────────────────────────────────────┤
  0xA018  │  capacity    = 1000                │  4 bytes
          ├────────────────────────────────────┤
  0xA01C  │  (padding)                         │  4 bytes（對齊至 8 bytes 邊界）
          └────────────────────────────────────┘
                         32 bytes 總計
```

### inbox 陣列（假設基址 0xC000）

```
  inbox array @ 0xC000（capacity=1000，每元素 4 bytes）
  ┌──────┬──────┬──────┬──────┬─────────────────────┐
  │ [0]  │ [1]  │ [2]  │ [3]  │  ...  [999]         │
  │0xC000│0xC004│0xC008│0xC00C│       0xCF9C         │
  └──────┴──────┴──────┴──────┴─────────────────────┘
    ↑
    inbox 指標 = 0xC000
```

### outbox 陣列（假設基址 0xD000）

```
  outbox array @ 0xD000（capacity=1000，每元素 4 bytes）
  ┌──────┬──────┬──────┬──────┬─────────────────────┐
  │ [0]  │ [1]  │ [2]  │ [3]  │  ...  [999]         │
  │0xD000│0xD004│0xD008│0xD00C│       0xDF9C         │
  └──────┴──────┴──────┴──────┴─────────────────────┘
    ↑
    outbox 指標 = 0xD000
```

### 指標關係總覽

```
  MyQueue @ 0xA000
  ┌──────────────┐
  │ inbox ───────────────────▶  int[1000] @ 0xC000
  │ outbox ──────────────────▶  int[1000] @ 0xD000
  │ inbox_top=-1 │
  │ outbox_top=-1│
  │ capacity=1000│
  └──────────────┘

  malloc 分配了 3 塊獨立記憶體：
  1. MyQueue 結構體本身（32 bytes）
  2. inbox 陣列（4000 bytes = 1000 × sizeof(int)）
  3. outbox 陣列（4000 bytes = 1000 × sizeof(int)）
```

---

## 二、完整資料結構記憶體配置

以 LeetCode 範例操作序列說明：`push(1), push(2), peek(), pop(), empty()`

### 狀態 1：初始化後（`myQueueCreate()`）

```
  MyQueue @ 0xA000
  ┌──────────────────────────┐
  │ inbox     = 0xC000       │───▶ [  ][  ][  ]...  全部未初始化
  │ outbox    = 0xD000       │───▶ [  ][  ][  ]...  全部未初始化
  │ inbox_top = -1  (空)     │
  │ outbox_top= -1  (空)     │
  │ capacity  = 1000         │
  └──────────────────────────┘

  邏輯隊列狀態：[ ]（空）
```

### 狀態 2：`push(1)` 後

```
  inbox @ 0xC000                     outbox @ 0xD000
  ┌──────┬──────┬──────┐             ┌──────┬──────┬──────┐
  │  1   │      │      │  ...        │      │      │      │  ...
  │ [0]  │ [1]  │ [2]  │             │ [0]  │ [1]  │ [2]  │
  └──────┴──────┴──────┘             └──────┴──────┴──────┘
    ↑ inbox_top = 0                    outbox_top = -1 (空)

  操作：inbox[++inbox_top] = inbox[0] = 1
  位址：*(0xC000 + 0 * 4) = *(0xC000) = 1

  邏輯隊列狀態：[1]  ← front
```

### 狀態 3：`push(2)` 後

```
  inbox @ 0xC000                     outbox @ 0xD000
  ┌──────┬──────┬──────┐             ┌──────┬──────┬──────┐
  │  1   │  2   │      │  ...        │      │      │      │  ...
  │ [0]  │ [1]  │ [2]  │             │ [0]  │ [1]  │ [2]  │
  └──────┴──────┴──────┘             └──────┴──────┴──────┘
           ↑ inbox_top = 1            outbox_top = -1 (空)

  操作：inbox[++inbox_top] = inbox[1] = 2
  位址：*(0xC000 + 1 * 4) = *(0xC004) = 2

  邏輯隊列狀態：[1, 2]  ← 1 是 front
```

### 狀態 4：`peek()` — 觸發延遲轉移

```
  轉移前：outbox_top == -1 → 觸發 transfer()

  轉移步驟 1：inbox[1]=2 → outbox[0]
    inbox[inbox_top--] = inbox[1] = 2  →  outbox[++outbox_top] = outbox[0] = 2
    位址：讀 *(0xC004) = 2，寫 *(0xD000) = 2

    inbox @ 0xC000                   outbox @ 0xD000
    ┌──────┬──────┐                  ┌──────┬──────┐
    │  1   │ (2)  │  ...             │  2   │      │  ...
    │ [0]  │ [1]  │                  │ [0]  │ [1]  │
    └──────┴──────┘                  └──────┴──────┘
      ↑ inbox_top = 0                  ↑ outbox_top = 0

  轉移步驟 2：inbox[0]=1 → outbox[1]
    inbox[inbox_top--] = inbox[0] = 1  →  outbox[++outbox_top] = outbox[1] = 1
    位址：讀 *(0xC000) = 1，寫 *(0xD004) = 1

    inbox @ 0xC000                   outbox @ 0xD000
    ┌──────┬──────┐                  ┌──────┬──────┐
    │ (1)  │ (2)  │  ...             │  2   │  1   │  ...
    │ [0]  │ [1]  │                  │ [0]  │ [1]  │
    └──────┴──────┘                  └──────┴──────┘
      inbox_top = -1 (空)                     ↑ outbox_top = 1

  轉移後：
  ┌──────────────────────────────────────────────────────┐
  │  inbox（空）         outbox                          │
  │  ┌───┐               ┌───┐                          │
  │  │   │               │ 1 │ ← outbox_top=1（隊列前端）│
  │  │   │               ├───┤                          │
  │  │   │               │ 2 │    [0]（隊列後端）         │
  │  └───┘               └───┘                          │
  └──────────────────────────────────────────────────────┘

  peek() 返回 outbox[outbox_top] = outbox[1] = 1
  位址：*(0xD000 + 1 * 4) = *(0xD004) = 1 ✓
```

### 狀態 5：`pop()` — 從 outbox 彈出

```
  outbox 非空（outbox_top=1）→ 不觸發轉移

  pop() 返回 outbox[outbox_top--] = outbox[1] = 1
  位址：*(0xD004) = 1

    inbox @ 0xC000                   outbox @ 0xD000
    ┌──────┬──────┐                  ┌──────┬──────┐
    │ (1)  │ (2)  │  ...             │  2   │ (1)  │  ...
    │ [0]  │ [1]  │                  │ [0]  │ [1]  │
    └──────┴──────┘                  └──────┴──────┘
      inbox_top = -1 (空)              ↑ outbox_top = 0

  邏輯隊列狀態：[2]
  pop() 返回 1 ✓
```

### 狀態 6：`empty()` — 檢查兩棧

```
  inbox_top == -1  → inbox 空 ✓
  outbox_top == 0  → outbox 非空 ✗
  → 回傳 false ✓

  邏輯隊列仍有元素 [2]
```

---

## 三、陣列索引版對應

### 棧操作的陣列索引公式

此問題使用陣列模擬棧，索引公式極為直觀：

| 操作 | 實作 | 說明 |
|------|------|------|
| 棧推入（push） | `arr[++top] = val` | top 先遞增，再寫入 |
| 棧彈出（pop） | `val = arr[top--]` | 先讀取，top 再遞減 |
| 棧頂查看（peek） | `val = arr[top]` | 只讀不改 |
| 棧是否為空 | `top == -1` | -1 表示空棧 |

### 完整索引追蹤表

以 `push(1), push(2), peek(), pop(), empty()` 為例：

```
操作序列        inbox 陣列內容      inbox_top  outbox 陣列內容    outbox_top  返回值
─────────────  ─────────────────  ─────────  ────────────────  ──────────  ──────
初始化          [  ][  ][  ]...    -1         [  ][  ][  ]...   -1          —
push(1)        [ 1][  ][  ]...     0         [  ][  ][  ]...   -1          —
push(2)        [ 1][ 2][  ]...     1         [  ][  ][  ]...   -1          —
peek()         [  ][  ][  ]...    -1         [ 2][ 1][  ]...    1          1
 (transfer)
pop()          [  ][  ][  ]...    -1         [ 2][  ][  ]...    0          1
empty()        [  ][  ][  ]...    -1         [ 2][  ][  ]...    0          false
```

### 記憶體位址對應

```
inbox 陣列：
  索引:    [0]       [1]       [2]       [3]    ...
  位址:   0xC000    0xC004    0xC008    0xC00C  ...
  公式:   base + index × sizeof(int) = 0xC000 + i × 4

outbox 陣列：
  索引:    [0]       [1]       [2]       [3]    ...
  位址:   0xD000    0xD004    0xD008    0xD00C  ...
  公式:   base + index × sizeof(int) = 0xD000 + i × 4
```

---

## 四、搜尋／操作過程追蹤

### 範例 1：`["MyQueue","push","push","peek","pop","empty"]` / `[[],[1],[2],[],[],[]]`

#### 步驟 1：`myQueueCreate()`

```
  分配 MyQueue 結構：malloc(32) → 0xA000
  分配 inbox 陣列：malloc(4000) → 0xC000
  分配 outbox 陣列：malloc(4000) → 0xD000
  設定：inbox_top = -1, outbox_top = -1, capacity = 1000

  MyQueue @ 0xA000
  ┌──────────────────┐
  │ inbox ─────────────▶ 0xC000 [ ][ ][ ]...
  │ outbox ────────────▶ 0xD000 [ ][ ][ ]...
  │ inbox_top  = -1  │
  │ outbox_top = -1  │
  └──────────────────┘
```

#### 步驟 2：`myQueuePush(obj, 1)`

```
  obj->inbox_top: -1 → 0（++inbox_top）
  寫入位址：obj->inbox + 0 = *(0xC000) = 1

  ┌──────────────────────────────────────┐
  │ 指標變化：inbox_top: -1 ──▶ 0       │
  │ 記憶體寫入：*(0xC000) ← 1           │
  └──────────────────────────────────────┘

  inbox:  [ 1 ][ ][ ]...   outbox: [ ][ ][ ]...
           ↑top=0                   top=-1
```

#### 步驟 3：`myQueuePush(obj, 2)`

```
  obj->inbox_top: 0 → 1（++inbox_top）
  寫入位址：obj->inbox + 1 = *(0xC004) = 2

  ┌──────────────────────────────────────┐
  │ 指標變化：inbox_top: 0 ──▶ 1        │
  │ 記憶體寫入：*(0xC004) ← 2           │
  └──────────────────────────────────────┘

  inbox:  [ 1 ][ 2 ][ ]...   outbox: [ ][ ][ ]...
                 ↑top=1                top=-1
```

#### 步驟 4：`myQueuePeek(obj)` → 觸發 transfer → 返回 1

```
  呼叫 transfer(obj)：outbox_top == -1 → 觸發！

  Transfer 迴圈迭代 1：
    讀取：inbox[inbox_top] = inbox[1] = 2  位址 *(0xC004)
    inbox_top: 1 → 0
    寫入：outbox[++outbox_top] = outbox[0] = 2  位址 *(0xD000)
    outbox_top: -1 → 0

    ┌──────────────────────────────────────────┐
    │ inbox_top: 1 ──▶ 0                      │
    │ outbox_top: -1 ──▶ 0                    │
    │ 讀 *(0xC004)=2 → 寫 *(0xD000)←2        │
    └──────────────────────────────────────────┘

  Transfer 迴圈迭代 2：
    讀取：inbox[inbox_top] = inbox[0] = 1  位址 *(0xC000)
    inbox_top: 0 → -1
    寫入：outbox[++outbox_top] = outbox[1] = 1  位址 *(0xD004)
    outbox_top: 0 → 1

    ┌──────────────────────────────────────────┐
    │ inbox_top: 0 ──▶ -1                     │
    │ outbox_top: 0 ──▶ 1                     │
    │ 讀 *(0xC000)=1 → 寫 *(0xD004)←1        │
    └──────────────────────────────────────────┘

  Transfer 結束：inbox_top < 0，迴圈結束。

  inbox:  [ 1 ][ 2 ][ ]...   outbox: [ 2 ][ 1 ][ ]...
           top=-1 (空)                       ↑top=1

  peek 返回 outbox[outbox_top] = outbox[1] = *(0xD004) = 1 ✓
```

#### 步驟 5：`myQueuePop(obj)` → 返回 1

```
  呼叫 transfer(obj)：outbox_top == 1 ≠ -1 → 不觸發轉移

  返回 outbox[outbox_top--] = outbox[1] = *(0xD004) = 1
  outbox_top: 1 → 0

  ┌──────────────────────────────────────┐
  │ outbox_top: 1 ──▶ 0                 │
  │ 讀 *(0xD004) = 1                    │
  └──────────────────────────────────────┘

  inbox:  [ 1 ][ 2 ][ ]...   outbox: [ 2 ][ 1 ][ ]...
           top=-1 (空)          ↑top=0

  pop() 返回 1 ✓
```

#### 步驟 6：`myQueueEmpty(obj)` → 返回 false

```
  inbox_top == -1  → true
  outbox_top == 0  → outbox_top != -1 → false
  結果：true && false = false

  empty() 返回 false ✓（隊列仍有元素 2）
```

---

## 五、指標變數追蹤總表

### 範例 1 追蹤

| 步驟 | 操作 | inbox_top | outbox_top | 讀取位址 | 讀取值 | 寫入位址 | 寫入值 | 返回 |
|------|------|-----------|------------|----------|--------|----------|--------|------|
| 1 | create | -1 | -1 | — | — | — | — | obj |
| 2 | push(1) | -1→0 | -1 | — | — | 0xC000 | 1 | — |
| 3 | push(2) | 0→1 | -1 | — | — | 0xC004 | 2 | — |
| 4a | peek→transfer | 1→0 | -1→0 | 0xC004 | 2 | 0xD000 | 2 | — |
| 4b | peek→transfer | 0→-1 | 0→1 | 0xC000 | 1 | 0xD004 | 1 | — |
| 4c | peek 返回 | -1 | 1 | 0xD004 | 1 | — | — | 1 |
| 5 | pop | -1 | 1→0 | 0xD004 | 1 | — | — | 1 |
| 6 | empty | -1 | 0 | — | — | — | — | false |

### 統計摘要

| 指標 | 值 |
|------|---|
| 記憶體寫入次數 | 4 次（2 push + 2 transfer 寫入） |
| 記憶體讀取次數 | 5 次（2 transfer 讀取 + peek 讀取 + pop 讀取 + empty 讀取） |
| transfer 觸發次數 | 1 次（peek 時） |
| 元素搬移次數 | 2（每個元素從 inbox → outbox 一次） |
| 總 push 到棧操作 | 4（push×2 到 inbox + transfer×2 到 outbox） |
| 總 pop 從棧操作 | 3（transfer×2 從 inbox + pop×1 從 outbox） |

---

## 六、記憶體存取模式比較

### 陣列模擬棧（本題解法）vs. 鏈結串列模擬棧

| 面向 | 陣列棧（本題） | 鏈結串列棧 |
|------|---------------|-----------|
| 存取模式 | 連續記憶體，循序存取 | 散布記憶體，指標追蹤 |
| Cache 行為 | 良好——inbox/outbox 各自連續 | 差——每個節點可能在不同 cache line |
| Push/Pop 開銷 | 1 次陣列寫入/讀取 + 索引遞增/遞減 | 1 次 malloc/free + 指標更新 |
| 空間浪費 | 預分配 2×1000×4 = 8000 bytes（多數閒置） | 每節點 8-16 bytes 額外負擔（指標 + 對齊） |
| 記憶體配置 | 3 次 malloc（建立時） | 每次 push 1 次 malloc，每次 pop 1 次 free |
| Transfer 時的 cache | inbox→outbox：兩塊連續記憶體，預取友善 | 需逐一追蹤指標，cache miss 頻繁 |

### 記憶體佈局對比圖

```
  陣列棧方案（本題）：
  ┌────────────────────────────────────┐
  │ inbox 陣列 @ 0xC000               │
  │ [ 1 ][ 2 ][ 3 ][   ][   ]...     │  ← 連續記憶體
  └────────────────────────────────────┘
  ┌────────────────────────────────────┐
  │ outbox 陣列 @ 0xD000              │
  │ [ 3 ][ 2 ][ 1 ][   ][   ]...     │  ← 連續記憶體
  └────────────────────────────────────┘
  Cache: 同一陣列的元素在相鄰 cache line，prefetch 有效

  鏈結串列棧方案（替代）：
  ┌──────┐    ┌──────┐    ┌──────┐
  │val: 3│    │val: 2│    │val: 1│
  │next ─────▶│next ─────▶│next→NULL│
  │@0x7F10│   │@0x3A80│   │@0x91C0│     ← 散布在 heap 各處
  └──────┘    └──────┘    └──────┘
  Cache: 每個節點可能需要載入新的 cache line
```

---

## 七、堆疊記憶體對比（迭代 vs. 遞迴）

本題的 transfer 函式使用 `while` 迴圈（迭代），不涉及遞迴。但如果用遞迴實現 transfer：

### 迭代版（本題實作）

```
  呼叫棧：固定深度

  ┌───────────────────────────┐
  │ main()                    │
  │   myQueuePeek()           │
  │     transfer()            │  ← while 迴圈在此，不增加棧深度
  └───────────────────────────┘

  堆疊記憶體：O(1)，恆定 3 層呼叫棧
```

### 假想遞迴版

```c
void transfer_recursive(MyQueue* obj) {
    if (obj->inbox_top < 0) return;
    int val = obj->inbox[obj->inbox_top--];
    transfer_recursive(obj);     // 遞迴呼叫
    obj->outbox[++obj->outbox_top] = val;  // 回溯時推入
    // 注意：這個遞迴版會反轉兩次，不正確！僅為堆疊說明用。
}
```

```
  遞迴版呼叫棧（假設 inbox 有 n=3 個元素）：

  ┌──────────────────────────────────────────┐
  │ transfer_recursive()  val=3  @0xFFE0     │  frame 1
  │   ┌──────────────────────────────────────┐│
  │   │ transfer_recursive()  val=2  @0xFFC0 ││  frame 2
  │   │   ┌──────────────────────────────────┐│
  │   │   │ transfer_recursive()  val=1      ││  frame 3
  │   │   │   ┌──────────────────────────────┐│
  │   │   │   │ transfer_recursive()  return ││  base case
  │   │   │   └──────────────────────────────┘│
  │   │   └──────────────────────────────────┘│
  │   └──────────────────────────────────────┘│
  └──────────────────────────────────────────┘

  堆疊記憶體：O(n)，每層 frame 約 32-48 bytes
  n=100 時：100 × ~48 = ~4800 bytes（在限制內但不必要）
```

### 比較

| 面向 | 迭代（本題） | 遞迴（假想） |
|------|-------------|-------------|
| 堆疊深度 | O(1) | O(n) |
| 每次 transfer 堆疊用量 | ~48 bytes 固定 | ~48 × n bytes |
| stack overflow 風險 | 無 | n 極大時有風險 |
| 效能 | 無函式呼叫開銷 | 每層遞迴有呼叫/返回開銷 |

---

## 八、退化情況分析

### 退化情況：交替 push-pop

```
  操作序列：push(1), pop(), push(2), pop(), push(3), pop(), ...

  每次 pop 都觸發 transfer（因為 outbox 永遠為空）：

  push(1):  inbox=[1], outbox=[]       inbox_top=0, outbox_top=-1
  pop():    transfer → inbox=[], outbox=[1] → pop → outbox=[]
            inbox_top=-1, outbox_top=-1
  push(2):  inbox=[2], outbox=[]       inbox_top=0, outbox_top=-1
  pop():    transfer → inbox=[], outbox=[2] → pop → outbox=[]
            inbox_top=-1, outbox_top=-1
  ...

  每次 pop 都觸發 transfer，但因為 inbox 只有 1 個元素，
  transfer 只搬 1 個元素 → 每次 pop 仍是 O(1)。
  攤還複雜度依然成立。
```

### 退化情況：大量 push 後一次 pop

```
  操作序列：push(1), push(2), ..., push(n), pop()

  push(1): inbox=[1]                    inbox_top=0
  push(2): inbox=[1,2]                  inbox_top=1
  ...
  push(n): inbox=[1,2,...,n]            inbox_top=n-1

  pop():   outbox 為空 → transfer 搬移全部 n 個元素
           inbox=[1,2,...,n] → outbox=[n,...,2,1]
           然後 pop outbox 頂部 = 1

  ┌────────────────────────────────────────┐
  │ transfer 搬移 n 個元素：               │
  │                                        │
  │ inbox:  [1][2][3]...[n]                │
  │         搬移方向 ──────▶               │
  │ outbox: [n]...[3][2][1]                │
  │                     ↑ top（最早的元素）  │
  └────────────────────────────────────────┘

  這次 pop 花費 O(n)，但後續 n-1 次 pop 各 O(1)。
  n 次 push + n 次 pop = 2n 次操作，
  總搬移次數 = n（transfer）+ n（pop）= 2n，
  攤還每次操作 = 2n / 2n = O(1)。
```

### 記憶體浪費分析

```
  本題預分配 capacity=1000，但題目限制最多 100 次呼叫且 1 ≤ x ≤ 9。
  最多同時存在 100 個元素。

  實際使用空間上限：
    inbox:  100 × 4 bytes = 400 bytes
    outbox: 100 × 4 bytes = 400 bytes
    總計：800 bytes

  預分配空間：
    inbox:  1000 × 4 bytes = 4000 bytes
    outbox: 1000 × 4 bytes = 4000 bytes
    總計：8000 bytes

  浪費率：(8000 - 800) / 8000 = 90%

  最佳化建議：將 capacity 改為 100 即可（節省 7200 bytes）。
  但在面試場景中，這不是重點。
```
