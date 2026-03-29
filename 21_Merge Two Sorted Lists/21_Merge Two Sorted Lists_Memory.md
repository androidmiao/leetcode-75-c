# LeetCode 21 - Merge Two Sorted Lists - 記憶體分析

## 1. Struct 記憶體佈局

### ListNode 結構定義
```c
struct ListNode {
    int val;                  // 4 bytes (offset 0)
    struct ListNode *next;    // 8 bytes (offset 8, 64-bit)
};
// 64-bit 系統上：總大小 = 16 bytes
```

### 單一節點記憶體圖

假設節點位於 `0xA000`：

```
             0xA000                  0xA008
         ┌─────────────────┬─────────────────┐
         │                 │                 │
    val  │      int: 1     │    padding      │
         │   (4 bytes)     │  (4 bytes)      │
         ├─────────────────┼─────────────────┤
   next  │                                   │
         │  struct ListNode*: 0xA010         │
         │        (8 bytes)                  │
         └─────────────────┴─────────────────┘

         Offset:    0        4        8       16 (bytes)
         Size:    ════════════════════════════
                        16 bytes (total)
```

### 記憶體配置詳表

| 欄位 | 偏移量 (Offset) | 大小 (bytes) | 說明 |
|------|-------------|----------|------|
| val | 0 | 4 | 整數值 |
| padding | 4 | 4 | 對齐填充 (64-bit align) |
| next | 8 | 8 | 指向下一節點的指標 |
| **總計** | — | **16** | 每節點固定大小 |

---

## 2. 完整資料結構記憶體配置 - Example 1

### 輸入
- list1 = [1, 2, 4]
- list2 = [1, 3, 4]

### 節點記憶體對映表

| 節點編號 | 所屬列表 | 地址 | val | next 指標 | 指向地址 |
|----------|---------|------|-----|---------|---------|
| Node_L1_0 | list1 | 0xA000 | 1 | next → | 0xA010 |
| Node_L1_1 | list1 | 0xA010 | 2 | next → | 0xA020 |
| Node_L1_2 | list1 | 0xA020 | 4 | next → | NULL |
| Node_L2_0 | list2 | 0xB000 | 1 | next → | 0xB010 |
| Node_L2_1 | list2 | 0xB010 | 3 | next → | 0xB020 |
| Node_L2_2 | list2 | 0xB020 | 4 | next → | NULL |
| dummy | 堆疊 | 0x7FF0 | 0 | next → | 0xA000 (初始) |

### 完整記憶體圖

```
========== list1 (list1 = 0xA000) ==========
  0xA000              0xA010              0xA020
┌────────┐        ┌────────┐        ┌────────┐
│ val: 1 │        │ val: 2 │        │ val: 4 │
│next───────▶│next───────▶│next───────▶ NULL
└────────┘        └────────┘        └────────┘


========== list2 (list2 = 0xB000) ==========
  0xB000              0xB010              0xB020
┌────────┐        ┌────────┐        ┌────────┐
│ val: 1 │        │ val: 3 │        │ val: 4 │
│next───────▶│next───────▶│next───────▶ NULL
└────────┘        └────────┘        └────────┘


========== 堆疊上的 dummy 節點 ==========
  0x7FF0
┌────────┐
│ val: 0 │
│next─────────▶ 0xA000 (list1)
└────────┘
```

### 邏輯鏈表圖 (含地址註解)

```
合併過程 (merge operation)：

初始狀態：
  dummy (0x7FF0) ──▶ [指向 list1 起點]

合併後：
  dummy (0x7FF0)
    │
    ▼
  0xA000 (val:1) ──▶ 0xB000 (val:1) ──▶ 0xA010 (val:2) ──▶ 0xB010 (val:3)
    │
    └──▶ 0xA020 (val:4) ──▶ 0xB020 (val:4) ──▶ NULL
```

---

## 3. 搜尋／操作過程追蹤

### Example 1: list1=[1,2,4], list2=[1,3,4]

```
初始狀態：
  dummy (0x7FF0)
  tail = 0x7FF0
  list1 = 0xA000 (val:1)
  list2 = 0xB000 (val:1)
```

#### Step 1: 比較 val:1 vs val:1
```
比較結果：list1->val (1) == list2->val (1)  ⟹ list1->val <= list2->val
行動：連接 list1 的節點 (0xA000)

  tail (0x7FF0)
    │
    ├─ next ──────▶ 0xA000 (val:1)
    │
    └─ [此步後 tail = 0xA000]

指標移動：
  list1: 0xA000 ──▶ 0xA010  (舊值 → 新值)
  list2: 0xB000  (保持不變)
  tail:  0x7FF0 ──▶ 0xA000
```

#### Step 2: 比較 val:2 vs val:1
```
比較結果：list1->val (2) > list2->val (1)  ⟹ list2->val < list1->val
行動：連接 list2 的節點 (0xB000)

  tail (0xA000)
    │
    ├─ next ──────▶ 0xB000 (val:1)
    │
    └─ [此步後 tail = 0xB000]

指標移動：
  list1: 0xA010  (保持不變)
  list2: 0xB000 ──▶ 0xB010  (舊值 → 新值)
  tail:  0xA000 ──▶ 0xB000
```

#### Step 3: 比較 val:2 vs val:3
```
比較結果：list1->val (2) <= list2->val (3)  ⟹ list1->val <= list2->val
行動：連接 list1 的節點 (0xA010)

  tail (0xB000)
    │
    ├─ next ──────▶ 0xA010 (val:2)
    │
    └─ [此步後 tail = 0xA010]

指標移動：
  list1: 0xA010 ──▶ 0xA020  (舊值 → 新值)
  list2: 0xB010  (保持不變)
  tail:  0xB000 ──▶ 0xA010
```

#### Step 4: 比較 val:4 vs val:3
```
比較結果：list1->val (4) > list2->val (3)  ⟹ list2->val < list1->val
行動：連接 list2 的節點 (0xB010)

  tail (0xA010)
    │
    ├─ next ──────▶ 0xB010 (val:3)
    │
    └─ [此步後 tail = 0xB010]

指標移動：
  list1: 0xA020  (保持不變)
  list2: 0xB010 ──▶ 0xB020  (舊值 → 新值)
  tail:  0xA010 ──▶ 0xB010
```

#### Step 5: 比較 val:4 vs val:4
```
比較結果：list1->val (4) == list2->val (4)  ⟹ list1->val <= list2->val
行動：連接 list1 的節點 (0xA020)

  tail (0xB010)
    │
    ├─ next ──────▶ 0xA020 (val:4)
    │
    └─ [此步後 tail = 0xA020]

指標移動：
  list1: 0xA020 ──▶ NULL  (舊值 → 新值)
  list2: 0xB020  (保持不變)
  tail:  0xB010 ──▶ 0xA020
```

#### Step 6: list1 已耗盡 (list1 == NULL)
```
行動：連接 list2 的剩餘部分

  tail (0xA020)
    │
    ├─ next ──────▶ 0xB020 (val:4)
    │
    └─ [此步後 tail = 0xB020]

指標移動：
  list1: NULL  (保持不變)
  list2: 0xB020 ──▶ NULL  (舊值 → 新值)
  tail:  0xA020 ──▶ 0xB020
```

#### Step 7: list2 亦已耗盡 (list2 == NULL)
```
迴圈終止

最終結構：
  dummy (0x7FF0)
    │
    ▼
  merged: 0xA000→0xB000→0xA010→0xB010→0xA020→0xB020→NULL
  值：    1    →  1   → 2   → 3   → 4   → 4   → NULL
```

---

### Example 2: list1=[], list2=[]

```
初始狀態：
  dummy (0x7FF0)
  tail = 0x7FF0
  list1 = NULL
  list2 = NULL

迴圈條件檢查：
  list1 && list2  ⟹  NULL && NULL  ⟹  假 (False)

迴圈不執行，直接進行尾部連接

結果連接：
  tail->next = list1  ⟹  0x7FF0->next = NULL

最終結構：
  dummy (0x7FF0) ──▶ NULL

返回 dummy.next = NULL
```

---

### Example 3: list1=[], list2=[0]

```
初始狀態：
  dummy (0x7FF0)
  tail = 0x7FF0
  list1 = NULL
  list2 = 0xC000 (val:0) → NULL

迴圈條件檢查：
  list1 && list2  ⟹  NULL && 0xC000  ⟹  假 (False)

迴圈不執行，直接進行尾部連接

結果連接：
  tail->next = list1  ⟹  0x7FF0->next = NULL

最終結構：
  dummy (0x7FF0) ──▶ NULL

但實際上應該連接 list2，修正如下：
  tail->next = (list1 != NULL) ? list1 : list2
           ⟹  0x7FF0->next = 0xC000

最終結構：
  dummy (0x7FF0)
    │
    ▼
  0xC000 (val:0) ──▶ NULL

返回 dummy.next = 0xC000
```

---

## 4. 指標變數追蹤總表

### Example 1: list1=[1,2,4], list2=[1,3,4]

| Step | tail 地址 | list1 地址 | list2 地址 | 比較結果 | 行動 |
|------|-----------|-----------|-----------|---------|------|
| 初始 | 0x7FF0 | 0xA000 (val:1) | 0xB000 (val:1) | — | 初始化 |
| 1 | 0x7FF0 | 0xA000 (val:1) | 0xB000 (val:1) | 1 ≤ 1 | 連接 list1，tail→0xA000，list1→0xA010 |
| 2 | 0xA000 | 0xA010 (val:2) | 0xB000 (val:1) | 2 > 1 | 連接 list2，tail→0xB000，list2→0xB010 |
| 3 | 0xB000 | 0xA010 (val:2) | 0xB010 (val:3) | 2 ≤ 3 | 連接 list1，tail→0xA010，list1→0xA020 |
| 4 | 0xA010 | 0xA020 (val:4) | 0xB010 (val:3) | 4 > 3 | 連接 list2，tail→0xB010，list2→0xB020 |
| 5 | 0xB010 | 0xA020 (val:4) | 0xB020 (val:4) | 4 ≤ 4 | 連接 list1，tail→0xA020，list1→NULL |
| 6 | 0xA020 | NULL | 0xB020 (val:4) | — | list1 耗盡，連接 list2，tail→0xB020，list2→NULL |
| 完成 | 0xB020 | NULL | NULL | — | 迴圈終止 |

**統計：**
- 總比較次數：5
- list1 節點被連接：3 個 (0xA000, 0xA010, 0xA020)
- list2 節點被連接：3 個 (0xB000, 0xB010, 0xB020)
- tail 指標移動：6 次

---

### Example 2: list1=[], list2=[]

| Step | tail 地址 | list1 地址 | list2 地址 | 比較結果 | 行動 |
|------|-----------|-----------|-----------|---------|------|
| 初始 | 0x7FF0 | NULL | NULL | — | 初始化 |
| 完成 | 0x7FF0 | NULL | NULL | — | 迴圈不執行，連接 NULL |

**統計：**
- 總比較次數：0
- list1 節點被連接：0 個
- list2 節點被連接：0 個
- tail 指標移動：0 次
- 返回值：NULL

---

### Example 3: list1=[], list2=[0]

| Step | tail 地址 | list1 地址 | list2 地址 | 比較結果 | 行動 |
|------|-----------|-----------|-----------|---------|------|
| 初始 | 0x7FF0 | NULL | 0xC000 (val:0) | — | 初始化 |
| 完成 | 0x7FF0 | NULL | 0xC000 (val:0) | — | 迴圈不執行，連接 list2 |

**統計：**
- 總比較次數：0
- list1 節點被連接：0 個
- list2 節點被連接：1 個 (0xC000)
- tail 指標移動：0 次 (在迴圈外)
- 返回值：0xC000

---

## 5. 記憶體存取模式比較

### 鏈表合併 vs 陣列合併

| 特性 | 鏈表合併 (Linked List) | 陣列合併 (Array) |
|------|----------------------|-----------------|
| **時間複雜度** | O(m + n) | O(m + n) |
| **空間複雜度** | O(1) (in-place) | O(m + n) (新陣列) |
| **快取行為** | 差 (指標追踪) | 優 (連續記憶體) |
| **記憶體存取模式** | 隨機 (非連續) | 順序 (cache-friendly) |
| **分支預測** | 較差 (指標值不確定) | 較優 (條件分支清晰) |
| **CPU 快取效率** | 低 (每次指標追踪新快取行) | 高 (預取機制有效) |

### 詳細分析

#### 鏈表合併的記憶體存取

```
Step 1: 訪問 list1->val
  ├─ 加載 0xA000 到快取 (快取行)
  ├─ 讀取 val = 1 ✓
  └─ list1 = list1->next (讀取指標)

Step 2: 訪問 list2->val
  ├─ 加載 0xB000 到快取 (新快取行，舊快取行可能被驅逐)
  ├─ 讀取 val = 1 ✓
  └─ list2 = list2->next (讀取指標)

Step 3: 訪問 list1->val
  ├─ 加載 0xA010 到快取 (又是新快取行)
  ├─ 讀取 val = 2 ✓
  └─ ...

→ 結果：每次迭代導致 L1/L2 快取缺失 (cache miss)
→ 記憶體延遲累積
```

#### 陣列合併的記憶體存取

```
Step 1: 訪問 arr1[0]
  ├─ 加載 0x1000 到快取 (快取行)
  ├─ 讀取 val = 1 ✓
  └─ 預取 arr1[1], arr1[2] 到快取 (prefetching)

Step 2: 訪問 arr2[0]
  ├─ 加載 0x2000 到快取 (新快取行)
  ├─ 讀取 val = 1 ✓
  └─ 預取 arr2[1], arr2[2] 到快取

Step 3: 訪問 arr1[1]
  ├─ 已在快取中 (快取命中) ✓
  ├─ 讀取 val = 2 ✓
  └─ ...

→ 結果：連續存取導致高快取命中率
→ 減少記憶體延遲
```

### 指標追蹤成本 (Pointer Chasing Cost)

```
鏈表合併每步驟的記憶體操作：
  1. 比較：list1->val vs list2->val
       ├─ 讀指標 list1 (已在暫存器)
       ├─ 讀 list1->val (可能快取缺失)
       ├─ 讀指標 list2 (已在暫存器)
       └─ 讀 list2->val (可能快取缺失)

  2. 連接：tail->next = list1
       ├─ 讀指標 tail (已在暫存器)
       ├─ 寫 tail->next = list1 (快取寫)

  3. 移動：list1 = list1->next
       └─ 讀 list1->next (同步驟 1 已加載)

每次迭代平均快取缺失：1～2 次

陣列合併每步驟的記憶體操作：
  1. 比較：arr1[i] vs arr2[j]
       ├─ arr1[i] (大概率快取命中)
       └─ arr2[j] (大概率快取命中)

  2. 複製：result[k] = arr1[i]
       └─ 寫結果陣列 (順序寫，快取友好)

  3. 遞增：i++

每次迭代平均快取缺失：0～0.1 次 (統計上)
```

### 實際性能差異

在現代 CPU 上（假設 L1 快取缺失 = 4 個週期，L3 缺失 = 12 個週期）：

```
鏈表合併 (m=1000, n=1000)：
  ├─ 順序操作：2000 次
  ├─ 預期快取缺失：1500～2000 次
  ├─ 額外延遲：1500*4 = 6000～8000 個週期
  └─ 估計總時間：10000～12000 個週期

陣列合併 (m=1000, n=1000)：
  ├─ 順序操作：2000 次
  ├─ 預期快取缺失：100～200 次 (預取覆蓋)
  ├─ 額外延遲：150*4 = 600 個週期
  └─ 估計總時間：2600～2800 個週期

→ 鏈表可能慢 3～4 倍 (實際硬體測試時)
```

---

## 6. 堆疊記憶體對比

### 迭代方法 (本解決方案)

```c
struct ListNode* mergeTwoLists(
    struct ListNode* list1,
    struct ListNode* list2) {

    struct ListNode dummy;      // 堆疊上
    dummy.val = 0;
    dummy.next = list1;

    struct ListNode* tail = &dummy;

    while (list1 && list2) {
        // ...
    }
    tail->next = list1 ? list1 : list2;

    return dummy.next;
}
```

#### 堆疊記憶體佈局

```
ESP (棧指標)
 ▼
┌─────────────────────────────────────────┐
│ 返回地址 (8 bytes)                       │ ESP-0
├─────────────────────────────────────────┤
│ 舊 RBP (8 bytes)                         │ ESP+8
├─────────────────────────────────────────┤
│ struct ListNode dummy:                   │ ESP+16
│   ├─ val (4 bytes): 0                    │
│   ├─ padding (4 bytes)                   │
│   └─ next (8 bytes): 指向 list1         │
├─────────────────────────────────────────┤ ESP+32
│ struct ListNode* tail (8 bytes)          │ ESP+40
│   └─ 值：指向 &dummy                    │
├─────────────────────────────────────────┤ ESP+48
│ list1 (8 bytes, 參數)                   │ ESP+56
│ list2 (8 bytes, 參數)                   │ ESP+64
└─────────────────────────────────────────┘

堆疊深度：常數 O(1)
堆疊使用：約 48～64 bytes (單個函式幀)
```

---

### 遞迴方法 (假設實現)

```c
struct ListNode* mergeTwoListsRecursive(
    struct ListNode* list1,
    struct ListNode* list2) {

    // Base case
    if (!list1) return list2;
    if (!list2) return list1;

    // Recursive case
    if (list1->val <= list2->val) {
        list1->next = mergeTwoListsRecursive(list1->next, list2);
        return list1;
    } else {
        list2->next = mergeTwoListsRecursive(list1, list2->next);
        return list2;
    }
}
```

#### 堆疊記憶體佈局 (多層遞迴)

```
呼叫：mergeTwoListsRecursive(list1, list2)
  │
  ├─ Frame 1 (call depth=1)
  │  ├─ 返回地址 (8 bytes)
  │  ├─ 舊 RBP (8 bytes)
  │  ├─ list1 參數 (8 bytes)
  │  ├─ list2 參數 (8 bytes)
  │  └─ 局部變數 (0 bytes)
  │
  └─ 遞迴呼叫 mergeTwoListsRecursive(list1->next, list2)
     │
     ├─ Frame 2 (call depth=2)
     │  ├─ 返回地址 (8 bytes)
     │  ├─ 舊 RBP (8 bytes)
     │  ├─ list1 參數 (8 bytes)
     │  ├─ list2 參數 (8 bytes)
     │  └─ 局部變數 (0 bytes)
     │
     └─ 遞迴呼叫 mergeTwoListsRecursive(list1->next, list2)
        │
        ├─ Frame 3 (call depth=3)
        │  ├─ ...
        │  └─ 32 bytes per frame
        │
        └─ ...
           └─ Frame k (call depth=k)
              ├─ 返回地址 (8 bytes)
              ├─ 舊 RBP (8 bytes)
              ├─ list1 參數 (8 bytes)
              ├─ list2 參數 (8 bytes)
              └─ 局部變數 (0 bytes)


ESP (棧頂，frame k)
 ▼
┌─────────────────────────────────────────┐
│ Frame k (call depth=k)                   │
├─────────────────────────────────────────┤
│ Frame k-1                                 │
├─────────────────────────────────────────┤
│ Frame k-2                                 │
├─────────────────────────────────────────┤
│ ...                                       │
├─────────────────────────────────────────┤
│ Frame 2                                   │
├─────────────────────────────────────────┤
│ Frame 1 (call depth=1)                   │
├─────────────────────────────────────────┤
│ 主函式幀                                 │
└─────────────────────────────────────────┘

堆疊深度：O(m + n)  [最壞情況]
堆疊使用：32 * (m + n) bytes  [最壞情況]
```

#### Example 1 的遞迴深度追蹤

```
mergeTwoListsRecursive(0xA000, 0xB000)  [list1=[1,2,4], list2=[1,3,4]]
  │
  │ list1->val (1) <= list2->val (1) ✓
  │ 遞迴：mergeTwoListsRecursive(0xA010, 0xB000)
  │
  ├─ mergeTwoListsRecursive(0xA010, 0xB000)  [list1=[2,4], list2=[1,3,4]]
  │   │
  │   │ list1->val (2) > list2->val (1) ✗
  │   │ 遞迴：mergeTwoListsRecursive(0xA010, 0xB010)
  │   │
  │   ├─ mergeTwoListsRecursive(0xA010, 0xB010)  [list1=[2,4], list2=[3,4]]
  │   │   │
  │   │   │ list1->val (2) <= list2->val (3) ✓
  │   │   │ 遞迴：mergeTwoListsRecursive(0xA020, 0xB010)
  │   │   │
  │   │   ├─ mergeTwoListsRecursive(0xA020, 0xB010)  [list1=[4], list2=[3,4]]
  │   │   │   │
  │   │   │   │ list1->val (4) > list2->val (3) ✗
  │   │   │   │ 遞迴：mergeTwoListsRecursive(0xA020, 0xB020)
  │   │   │   │
  │   │   │   ├─ mergeTwoListsRecursive(0xA020, 0xB020)  [list1=[4], list2=[4]]
  │   │   │   │   │
  │   │   │   │   │ list1->val (4) <= list2->val (4) ✓
  │   │   │   │   │ 遞迴：mergeTwoListsRecursive(NULL, 0xB020)
  │   │   │   │   │
  │   │   │   │   ├─ mergeTwoListsRecursive(NULL, 0xB020)
  │   │   │   │   │   │
  │   │   │   │   │   │ !list1 為真
  │   │   │   │   │   │ 返回 list2 = 0xB020 ◀ Base case
  │   │   │   │   │   │
  │   │   │   │   ├─ 返回 0xB020 至呼叫者
  │   │   │   │   └─ list1->next = 0xB020
  │   │   │   │
  │   │   │   ├─ 返回 0xA020 至呼叫者
  │   │   │   └─ list2->next = 0xA020
  │   │   │
  │   │   ├─ 返回 0xA010 至呼叫者
  │   │   └─ list1->next = 0xA010 (已遞迴修改)
  │   │
  │   ├─ 返回 0xB000 至呼叫者
  │   └─ list2->next = 0xB000 (已遞迴修改)
  │
  ├─ 返回 0xA000 至呼叫者
  └─ 完成合併

堆疊深度：5 次遞迴呼叫（基礎案例）
堆疊使用：5 * 32 = 160 bytes
```

---

## 7. 記憶體比較總結

### 堆疊使用對比表

| 指標 | 迭代方法 | 遞迴方法 |
|------|--------|--------|
| 空間複雜度 | O(1) | O(m + n) |
| 單個函式幀大小 | 48～64 bytes | 32 bytes |
| m=10, n=10 堆疊使用 | 64 bytes | 640 bytes |
| m=100, n=100 堆疊使用 | 64 bytes | 6400 bytes |
| m=1000, n=1000 堆疊使用 | 64 bytes | 64000 bytes |
| 堆疊溢位風險 | 無 | 高 (深層遞迴) |
| 快取友好性 | 好 (局部變數) | 差 (堆疊切換) |

### 空間效率

```
迭代方法：
  heap space = 0 (in-place merge)
  stack space = 64 bytes (固定)
  total = 64 bytes

遞迴方法 (m+n=2000)：
  heap space = 0 (in-place merge)
  stack space = 2000 * 32 = 64000 bytes
  total = 64000 bytes

效率比：64 / 64000 = 0.1%  ◀ 迭代方法效率更高
```

---

## 總結

本解決方案採用**迭代方法**，利用 dummy 節點和 tail 指標：

1. **記憶體配置**：O(1) 額外空間，堆疊使用固定
2. **時間複雜度**：O(m + n) 單次遍歷
3. **操作過程**：逐步比較和連接，指標追蹤清晰
4. **快取效率**：儘管鏈表本身快取不友好，但迭代避免遞迴堆疊開銷
5. **穩定性**：無堆疊溢位風險，適合大型鏈表

相比遞迴方法，迭代方案在所有方面都更優勢。
