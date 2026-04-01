# LeetCode 2095 Delete the Middle Node of a Linked List - 記憶體分析

## 概述
此題目使用**快慢指標法** (fast/slow pointer technique) 在單向鏈結串列中尋找並刪除中間節點。解決方案為迭代式，空間複雜度為 O(1)。

```c
struct ListNode {
    int val;
    struct ListNode *next;
};
```

---

## 1. Struct 記憶體佈局

### 單一節點的記憶體結構 (64位元系統)

```
┌────────────────────────────────────────────┐
│  struct ListNode @ 0xA000                  │
├────────────────────────────────────────────┤
│  offset   field          size    value      │
├───────────────────────────────────────────┤
│  0x00    int val          4B      1         │
│  0x04    [padding]        4B      ----      │
│  0x08    struct ListNode  8B      0xA010    │
│          *next                              │
├────────────────────────────────────────────┤
│  總大小: 16 bytes (對齊到8位元組邊界)      │
└────────────────────────────────────────────┘

位元組配置圖:
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ 00  │ 01  │ 02  │ 03  │ 04  │ 05  │ 06  │ 07  │
├─ val (int32) ─┼─ padding (4B) ─┤
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ 08  │ 09  │ 0A  │ 0B  │ 0C  │ 0D  │ 0E  │ 0F  │
├──────── *next (pointer, 64-bit) ──────┤
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

**欄位說明:**
- `val`: int (4 bytes) @ offset 0x00
- **padding**: 4 bytes @ offset 0x04 (對齊到8位元組邊界)
- `*next`: 指標 (8 bytes) @ offset 0x08

**節點大小計算:**
```
sizeof(struct ListNode) = 16 bytes
                        = 4 (val) + 4 (padding) + 8 (*next)
```

---

## 2. 完整資料結構記憶體配置

### 範例 1: [1,3,4,7,1,2,6]

#### 2.1 節點地址映射表

| 節點 | 值 | 地址     | 下一個地址 |
|------|-----|---------|-----------|
| node0 | 1   | 0xA000  | 0xA010    |
| node1 | 3   | 0xA010  | 0xA020    |
| node2 | 4   | 0xA020  | 0xA030    |
| node3 | 7   | 0xA030  | 0xA040    |
| node4 | 1   | 0xA040  | 0xA050    |
| node5 | 2   | 0xA050  | 0xA060    |
| node6 | 6   | 0xA060  | NULL      |

#### 2.2 完整記憶體配置圖

```
堆積記憶體配置:

0xA000 ┌─────────────────────┐
       │ [val: 1 | pad]      │
       │ [next: ────────────────┐
0xA008 │                     │  │
       └─────────────────────┘  │
                                │
0xA010 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 3 | pad]      │
       │ [next: ────────────────┐
0xA018 │                     │  │
       └─────────────────────┘  │
                                │
0xA020 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 4 | pad]      │
       │ [next: ────────────────┐
0xA028 │                     │  │
       └─────────────────────┘  │
                                │
0xA030 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 7 | pad]      │  ◄──── 中間節點 (待刪除)
       │ [next: ────────────────┐
0xA038 │                     │  │
       └─────────────────────┘  │
                                │
0xA040 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 1 | pad]      │
       │ [next: ────────────────┐
0xA048 │                     │  │
       └─────────────────────┘  │
                                │
0xA050 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 2 | pad]      │
       │ [next: ────────────────┐
0xA058 │                     │  │
       └─────────────────────┘  │
                                │
0xA060 ◄────────────────────────┘
       ┌─────────────────────┐
       │ [val: 6 | pad]      │
       │ [next: NULL      ]  │
0xA068 │                     │
       └─────────────────────┘
```

#### 2.3 邏輯鏈結串列視圖

**操作前:**
```
[1] ───▶ [3] ───▶ [4] ───▶ [7] ───▶ [1] ───▶ [2] ───▶ [6] ───▶ NULL
0xA000   0xA010   0xA020   0xA030   0xA040   0xA050   0xA060
                                      ▲
                                  (中間節點)
```

**操作後:**
```
[1] ───▶ [3] ───▶ [4] ───▶ [1] ───▶ [2] ───▶ [6] ───▶ NULL
0xA000   0xA010   0xA020   0xA040   0xA050   0xA060
                          │
                  node2.next 指向 node4
```

---

## 3. 陣列索引版對應

### 3.1 陣列表示法

```
陣列索引對應:
┌────┬────┬────┬────┬────┬────┬────┐
│ 0  │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │
├────┼────┼────┼────┼────┼────┼────┤
│ 1  │ 3  │ 4  │ 7  │ 1  │ 2  │ 6  │
└────┴────┴────┴────┴────┴────┴────┘
              ▲
         中間索引 (3)
         length = 7, mid = 7/2 = 3
```

### 3.2 陣列與鏈結串列的差異

**鏈結串列方式 (本題方案):**
```
1. 快慢指標遍歷，時間 O(n)，無元素移動
2. 直接修改指標: node2.next = node4
3. 刪除 node3，無需移位
4. 記憶體碎片化可能增加，但無複製開銷
```

**陣列方式:**
```
原陣列: [1, 3, 4, 7, 1, 2, 6]
                   ▲
              刪除索引 3

刪除後需要左移:
[1, 3, 4, _, 1, 2, 6]
           └─► 1 右移至索引 4
               [1, 3, 4, 1, 2, 6, _]

時間複雜度: O(n) (複製 n/2 個元素)
空間開銷: 連續記憶體，快取友善
```

### 3.3 複雜度對比

| 操作 | 鏈結串列 | 陣列 |
|------|---------|------|
| 查找中間節點 | O(n) 遍歷 | O(1) 索引 |
| 刪除節點 | O(1) 指標修改 | O(n) 元素移位 |
| 記憶體訪問 | 隨機 (指標跳躍) | 順序 (快取友善) |
| 額外空間 | O(1) 指標 | 原地操作 O(1) 或 O(n) |

---

## 4. 搜尋／操作過程追蹤

### 範例 1: [1,3,4,7,1,2,6] → 刪除 7 → [1,3,4,1,2,6]

#### 4.1 初始狀態
```
head (0xA000)
  │
  ▼
[1] ───▶ [3] ───▶ [4] ───▶ [7] ───▶ [1] ───▶ [2] ───▶ [6] ───▶ NULL
0xA000   0xA010   0xA020   0xA030   0xA040   0xA050   0xA060
```

**指標初始化:**
```
slow = head (0xA000)       // 慢指標指向第一個節點
fast = head (0xA000)       // 快指標也指向第一個節點
prev = NULL                // 前驅節點
```

#### 4.2 快慢指標遍歷步驟

**步驟 1: slow 移動 1 步，fast 移動 2 步**
```
prev = slow (0xA000) ← slow 前驅
slow = slow->next = 0xA010
fast = fast->next->next = 0xA020

狀態:
prev (0xA000)   [1]
slow (0xA010)   [3] ◄─── 當前位置
fast (0xA020)   [4]

指標圖:
[1] ───prev───▶ [3] ───slow───▶ [4] ───fast───▶ [7] ───▶ ...
0xA000           0xA010           0xA020           0xA030
```

**步驟 2: slow 移動 1 步，fast 移動 2 步**
```
prev = slow (0xA010)
slow = slow->next = 0xA020
fast = fast->next->next = 0xA040

狀態:
prev (0xA010)   [3]
slow (0xA020)   [4] ◄─── 當前位置
fast (0xA040)   [1]

指標圖:
[3] ───prev───▶ [4] ───slow───▶ [7] ───▶ [1] ───fast───▶ [2] ───▶ ...
0xA010           0xA020           0xA030   0xA040
```

**步驟 3: slow 移動 1 步，fast 移動 2 步**
```
prev = slow (0xA020)
slow = slow->next = 0xA030
fast = fast->next->next = 0xA060

狀態:
prev (0xA020)   [4]
slow (0xA030)   [7] ◄─── 當前位置（中間節點）
fast (0xA060)   [6]

指標圖:
[4] ───prev───▶ [7] ───slow───▶ [1] ───▶ [2] ───▶ [6] ───fast───▶ NULL
0xA020           0xA030           0xA040   0xA050   0xA060
```

**步驟 4: fast 移動檢查，確認到達末尾**
```
fast = fast->next->next
try: 0xA060->next = NULL
     NULL->next → 無效訪問

條件檢查: fast != NULL && fast->next != NULL
         → false (fast->next 為 NULL)

迴圈終止 ✓
```

#### 4.3 刪除操作

```
識別要刪除的節點:
  slow 指向 [7] @ 0xA030 (中間節點)

執行刪除:
  prev (0xA020) ───▶ [4]
                     │
                     ├─▶ slow (0xA030) [7] (待刪除)
                     │    │
                     │    └─▶ slow->next (0xA040) [1]
                     │
  prev->next = slow->next
              = 0xA030->next = 0xA040

修改指標:
  0xA020 的 next 欄位從 0xA030 改為 0xA040

執行前:
[4] ───▶ [7] ───▶ [1]
0xA020   0xA030   0xA040

執行後:
[4] ───▶ [1]
0xA020   0xA040
        (指標跳過 0xA030)
```

#### 4.4 最終結果

```
head (0xA000)
  │
  ▼
[1] ───▶ [3] ───▶ [4] ───▶ [1] ───▶ [2] ───▶ [6] ───▶ NULL
0xA000   0xA010   0xA020   0xA040   0xA050   0xA060

刪除成功: [7] 已從鏈結串列中移除
```

---

### 範例 2: [1,2,3,4] → 刪除 3 → [1,2,4]

#### 4.5 快慢指標過程

```
初始:
[1] ───▶ [2] ───▶ [3] ───▶ [4] ───▶ NULL
0xB000   0xB010   0xB020   0xB030

slow = 0xB000 [1]
fast = 0xB000 [1]
prev = NULL
```

**迭代 1:**
```
prev = 0xB000 [1]
slow = 0xB010 [2]
fast = 0xB020 [3]

slow:  ●─▶ [2]
fast:       ●─▶ [3]
prev:   ●
```

**迭代 2:**
```
prev = 0xB010 [2]
slow = 0xB020 [3]         ◄──── 中間節點 (length=4, mid=2)
fast = NULL (4 的 next->next)

fast 檢查: fast->next = NULL → 終止
```

**刪除操作:**
```
prev->next = slow->next
0xB010->next = 0xB020->next = 0xB030

[2] ───▶ [4]
0xB010   0xB030

結果: [1,2,4]
```

---

### 範例 3: [2,1] → 刪除 1 → [2]

#### 4.6 快慢指標過程

```
初始:
[2] ───▶ [1] ───▶ NULL
0xC000   0xC010

slow = 0xC000 [2]
fast = 0xC000 [2]
prev = NULL
```

**迭代 1:**
```
prev = 0xC000 [2]
slow = 0xC010 [1]         ◄──── 中間節點 (length=2, mid=1)
fast = NULL (1 的 next 為 NULL，1->next->next 無效)

fast 檢查: fast = NULL → 終止
```

**刪除操作:**
```
prev->next = slow->next
0xC000->next = 0xC010->next = NULL

[2] ───▶ NULL
0xC000

結果: [2]
```

---

## 5. 指標變數追蹤總表

### 範例 1: [1,3,4,7,1,2,6]

| 步驟 | slow 地址 | slow->val | fast 地址 | fast->val | prev 地址 | 動作 |
|------|-----------|-----------|-----------|-----------|-----------|------|
| 0 (init) | 0xA000 | 1 | 0xA000 | 1 | NULL | 初始化 |
| 1 | 0xA010 | 3 | 0xA020 | 4 | 0xA000 | slow++, fast+=2 |
| 2 | 0xA020 | 4 | 0xA040 | 1 | 0xA010 | slow++, fast+=2 |
| 3 | 0xA030 | 7 | 0xA060 | 6 | 0xA020 | slow++, fast+=2 |
| 4 (終止) | 0xA030 | 7 | NULL | - | 0xA020 | fast->next==NULL |
| 5 (刪除) | - | - | - | - | - | prev->next = 0xA040 |

**統計:**
- 總遍歷步驟: 4
- 中間節點地址: 0xA030
- 中間節點值: 7
- 刪除成功: 是

---

### 範例 2: [1,2,3,4]

| 步驟 | slow 地址 | slow->val | fast 地址 | fast->val | prev 地址 | 動作 |
|------|-----------|-----------|-----------|-----------|-----------|------|
| 0 (init) | 0xB000 | 1 | 0xB000 | 1 | NULL | 初始化 |
| 1 | 0xB010 | 2 | 0xB020 | 3 | 0xB000 | slow++, fast+=2 |
| 2 | 0xB020 | 3 | NULL | - | 0xB010 | slow++, fast->next==NULL |
| 3 (刪除) | - | - | - | - | - | prev->next = 0xB030 |

**統計:**
- 總遍歷步驟: 2
- 中間節點地址: 0xB020
- 中間節點值: 3
- 刪除成功: 是

---

### 範例 3: [2,1]

| 步驟 | slow 地址 | slow->val | fast 地址 | fast->val | prev 地址 | 動作 |
|------|-----------|-----------|-----------|-----------|-----------|------|
| 0 (init) | 0xC000 | 2 | 0xC000 | 2 | NULL | 初始化 |
| 1 | 0xC010 | 1 | NULL | - | 0xC000 | slow++, fast==NULL |
| 2 (刪除) | - | - | - | - | - | prev->next = NULL |

**統計:**
- 總遍歷步驟: 1
- 中間節點地址: 0xC010
- 中間節點值: 1
- 刪除成功: 是

---

## 6. 記憶體存取模式比較

### 6.1 鏈結串列 vs 陣列

| 特徵 | 鏈結串列 | 陣列 |
|------|---------|------|
| **記憶體佈局** | 分散 (堆積各處) | 連續 |
| **快取行為** | 差 (L1/L2 快取命中率低) | 好 (預取友善) |
| **單節點大小** | 16 bytes (含指標) | 4 bytes (int 僅) |
| **節點訪問時間** | ~100+ 週期 (快取缺失) | ~4 週期 (快取命中) |
| **空間開銷** | 100% (每節點 8 bytes 指標) | 0% (無額外開銷) |
| **刪除時間** | O(1) (指標修改) | O(n) (元素移位) |
| **插入時間** | O(1) (若已知位置) | O(n) (元素移位) |
| **搜尋時間** | O(n) (順序遍歷) | O(n) (順序) |

### 6.2 本題記憶體存取模式

```
快慢指標算法的記憶體訪問序列:

遍歷例 1 的訪問順序:
  0xA000 → 0xA010 → 0xA020 → 0xA030 → 0xA040 → 0xA050 → 0xA060

每次迭代訪問:
  1. slow->val (讀)
  2. slow->next (讀指標)
  3. fast->next (讀指標)
  4. fast->next->next (讀指標)

典型訪問時序:
  時間 0: 讀取 slow 節點 → L1 快取缺失 (100 週期)
  時間 100: 讀取 fast 節點 → L1 快取缺失 (100 週期)
  時間 200: 下一次迭代

總時間: ~7 次迭代 × 200 週期 = ~1400 週期

對比陣列:
  順序讀取 7 個元素
  全部在快取中: 7 × 4 週期 = 28 週期
  速度比: 1400 / 28 ≈ 50 倍慢
```

### 6.3 指標追蹤的記憶體足跡

```
在堆積中的指標跳躍:

鏈結串列方式:
  [1]@0xA000 ────▶ [3]@0xA010 ────▶ [4]@0xA020 ────▶ ...
   │                 │                 │
   4KB 間距          4KB 間距          4KB 間距
   (典型)            (典型)            (典型)

   快取行 (64 bytes):
   ┌─────────────────────┐
   │ ... [1]@0xA000 ... │  ◄ 載入
   └─────────────────────┘
                             ┌─────────────────────┐
                             │ ... [3]@0xA010 ... │  ◄ 新的快取行載入
                             └─────────────────────┘
                                                     ┌─────────────────────┐
                                                     │ ... [4]@0xA020 ... │  ◄ 又一個新的快取行
                                                     └─────────────────────┘

   每次指標跳躍都可能造成快取缺失
```

---

## 7. 堆疊記憶體對比

### 7.1 本題方案: 迭代式 (O(1) 堆疊)

```c
// 迭代方案 - 堆疊使用最小
struct ListNode* deleteMiddle(struct ListNode* head) {
    if (!head || !head->next) return NULL;

    struct ListNode *slow = head;      // 堆疊: 8 bytes
    struct ListNode *fast = head;      // 堆疊: 8 bytes
    struct ListNode *prev = NULL;      // 堆疊: 8 bytes

    // 無遞迴調用，堆疊深度 = 1
    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    prev->next = slow->next;
    return head;
}

堆疊佈局:
┌──────────────────┐
│ 返回地址         │  ◄ main 的指令指標
├──────────────────┤
│ head (8B)        │  ◄ 參數
├──────────────────┤
│ slow (8B)        │  ◄ 本地變數
│ fast (8B)        │
│ prev (8B)        │
├──────────────────┤
│ [暫存器溢出]     │  (通常不需要)
└──────────────────┘

堆疊深度: 1 層函數調用
堆疊使用: ~48 bytes (返回地址 + 參數 + 本地變數)
時間複雜度: O(n) 單次遍歷
空間複雜度: O(1) 常數堆疊
```

### 7.2 假設遞迴方案: O(n) 堆疊

```c
// 遞迴方案 (較差) - 堆疊使用 O(n)
struct ListNode* deleteMiddleRecursive(struct ListNode* node,
                                       struct ListNode* head,
                                       int* count) {
    if (!node) {
        *count = 0;
        return node;
    }

    // 遞迴調用
    struct ListNode* temp = deleteMiddleRecursive(node->next, head, count);

    (*count)++;
    if (*count == length / 2) {  // 需要知道長度
        return node->next;       // 跳過此節點
    }

    node->next = temp;
    return node;
}

堆疊佈局 (鏈表長度 7):
層級 7: [..., count=1]
層級 6: [..., count=2]
層級 5: [..., count=3]     ◄ 刪除此節點 (count == 3)
層級 4: [..., count=4]
層級 3: [..., count=5]
層級 2: [..., count=6]
層級 1: [..., count=7]
層級 0: [main 調用]

堆疊深度: 7 層 (最大遞迴深度)
堆疊使用: ~7 × 56 bytes = 392 bytes (每層 8B 返回地址 + 8B node + 8B head + 8B count 指標)
時間複雜度: O(n) 兩次遍歷 (下降 + 上升)
空間複雜度: O(n) 遞迴堆疊

風險:
  - 堆疊溢位 (大規模鏈表)
  - 快取效能更差 (頻繁上下文切換)
  - 編譯器優化困難
```

### 7.3 對比表

| 特徵 | 迭代方案 | 遞迴方案 |
|------|---------|---------|
| 堆疊深度 | 1 (常數) | O(n) (線性) |
| 堆疊使用量 | ~48 bytes | ~56n bytes |
| 時間複雜度 | O(n) | O(n) |
| 空間複雜度 | O(1) | O(n) |
| 編譯器優化 | 尾調用優化適用 | 尾調用優化困難 |
| 堆疊溢位風險 | 無 | 有 (長鏈表) |
| 快取效能 | 線性訪問模式 | 遞迴深度造成分散 |
| 代碼可讀性 | 中等 | 高 |

### 7.4 堆疊記憶體時序

**迭代方案:**
```
時間線:
  t=0: 函數進入，堆疊分配 48 bytes
  t=1-n: 迴圈執行，堆疊內容保持不變
  t=n+1: 函數返回，堆疊釋放 48 bytes

記憶體使用曲線:
  │
  │           ┌────────────────┐
  │           │ 48 bytes       │
  │           │                │
  └───────────┴────────────────┴─────────
    進入      執行迴圈         返回
```

**遞迴方案:**
```
時間線:
  t=0: deleteMiddleRecursive(node0) - 堆疊 56 bytes
  t=1: deleteMiddleRecursive(node1) - 堆疊 112 bytes (56 + 56)
  t=2: deleteMiddleRecursive(node2) - 堆疊 168 bytes
  ...
  t=3: deleteMiddleRecursive(node3) - 堆疊 224 bytes (最大)
  ...
  t=6: deleteMiddleRecursive(node6) - 堆疊 56 bytes (回溯)
  t=7: 函數返回 - 堆疊 0 bytes

記憶體使用曲線:
  │
  │                 ╱╲
  │                ╱  ╲
  │               ╱    ╲
  │         ╱────╱      ╲────╲
  │        ╱              ╲
  └───────────────────────────────────
    遞迴深入                回溯

最大堆疊: 56 × 7 ≈ 392 bytes (深度 7 時)
```

---

## 8. 演算法核心邏輯總結

### 8.1 快慢指標的工作原理

```
直覺:
  • slow 每次移動 1 步 (速度: v)
  • fast 每次移動 2 步 (速度: 2v)
  • 當 fast 到達末尾時，slow 恰好在中點

數學證明 (奇數長度 n=7):
  • 中點索引: (7-1)/2 = 3
  • 迭代次數: k
  • slow 位置: k
  • fast 位置: 2k

  當 fast 越界: 2k >= 7
  最小 k: 2k = 7 + 1 = 8 (考慮 ->next->next 訪問)
  實際 k = 3 時: fast = 6, fast->next->next 會訪問越界
  所以 k = 3, slow = 3 ✓

偶數長度 n=4:
  • 中點索引: (4-1)/2 = 1 (向下取整)
  • 當 fast->next == NULL 時:
    fast 位置 = 2k, fast->next = NULL
    2k >= 4 時發生
    k = 2: fast = 4 (越界, fast->next = NULL)
    slow = 2 ✓ (正確的中點)
```

### 8.2 終止條件

```
迴圈條件: while (fast && fast->next)

情況 1: fast == NULL
  → 鏈表長度為奇數，fast 已超過最後一個節點

情況 2: fast->next == NULL
  → 鏈表長度為偶數，fast 正好在最後一個節點
    (下一個 fast->next->next 無法進行)

情況 3: fast->next->next == NULL
  → 邏輯上會停止，但在迴圈檢查中已排除 (fast->next->next 未檢查)
    此時 fast->next != NULL，迴圈繼續
    下一次迭代會因為 fast = fast->next->next = NULL 而停止
```

### 8.3 邊界情況處理

```
情況 1: 空鏈表 (head == NULL)
  前置檢查: if (!head || !head->next) return NULL;

情況 2: 單節點 (head->next == NULL)
  前置檢查: if (!head || !head->next) return NULL;

情況 3: 兩節點 [a, b]
  slow = head [a]
  fast = head [a]
  迭代 1: prev = [a], slow = [b], fast = NULL
  終止 (fast == NULL)
  刪除: head->next = NULL
  結果: [a]

情況 4: 三節點 [a, b, c]
  迭代 1: prev = [a], slow = [b], fast = [c]
  迭代 2: prev = [b], slow = [c], fast = NULL
  終止
  刪除: [b]->next = NULL
  結果: [a, b]
```

---

## 9. 程式碼實現參考

```c
#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

/**
 * 刪除鏈表中間節點
 * 時間複雜度: O(n) - 單次遍歷
 * 空間複雜度: O(1) - 常數額外空間
 *
 * @param head: 鏈表頭節點指標
 * @return: 修改後的鏈表頭節點
 */
struct ListNode* deleteMiddle(struct ListNode* head) {
    // 邊界檢查: 空鏈表或單節點
    if (!head || !head->next) {
        free(head);
        return NULL;
    }

    // 快慢指標初始化
    struct ListNode *slow = head;
    struct ListNode *fast = head;
    struct ListNode *prev = NULL;

    // 快慢指標遍歷至中間節點
    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // 刪除中間節點: 前驅節點的 next 指向中間節點的 next
    prev->next = slow->next;

    // 釋放中間節點記憶體
    free(slow);

    return head;
}

// 輔助函數: 建立鏈表
struct ListNode* createNode(int val) {
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    node->val = val;
    node->next = NULL;
    return node;
}

// 輔助函數: 列印鏈表
void printList(struct ListNode *head) {
    printf("[");
    while (head) {
        printf("%d", head->val);
        if (head->next) printf(",");
        head = head->next;
    }
    printf("]\n");
}

// 主程式測試
int main() {
    // 測試範例 1: [1,3,4,7,1,2,6]
    struct ListNode *head1 = createNode(1);
    head1->next = createNode(3);
    head1->next->next = createNode(4);
    head1->next->next->next = createNode(7);
    head1->next->next->next->next = createNode(1);
    head1->next->next->next->next->next = createNode(2);
    head1->next->next->next->next->next->next = createNode(6);

    printf("範例 1 前: ");
    printList(head1);
    head1 = deleteMiddle(head1);
    printf("範例 1 後: ");
    printList(head1);

    return 0;
}
```

**輸出:**
```
範例 1 前: [1,3,4,7,1,2,6]
範例 1 後: [1,3,4,1,2,6]
```

---

## 10. 結論

本題使用**快慢指標**法在單向鏈表中高效地定位並刪除中間節點:

**關鍵特點:**
- ✓ 時間複雜度: O(n) - 單次遍歷
- ✓ 空間複雜度: O(1) - 僅 3 個指標變數
- ✓ 原地修改 - 無需額外資料結構
- ✓ 堆疊安全 - 迭代式無遞迴開銷

**記憶體優勢:**
- 相比陣列刪除的 O(n) 元素移位，此法只需 O(1) 指標修改
- 代價是快取效能較差 (指標跳躍)，但對實時性敏感應用優勢明顯

**應用場景:**
- 動態鏈表管理
- 中點查詢/分割
- 雙指標模板的變體應用
