# 234. Palindrome Linked List — 記憶體位址 / 陣列索引追蹤

> 以下所有十六進位位址皆為假設值，僅供說明用途。

---

## 1. Struct 記憶體佈局

```c
struct ListNode {
    int val;                  // 4 bytes (offset 0x00)
    struct ListNode *next;    // 8 bytes (offset 0x08, 64-bit pointer)
};
// sizeof(struct ListNode) = 16 bytes（含 4 bytes padding after val）
```

### 單一節點記憶體圖（假設位址 0xA000）

```
        0xA000                0xA008
       ┌──────────┬──────────┬──────────────────┐
       │  val (4B) │ pad (4B) │   next (8B)       │
       │    1      │  (unused)│   0xB000 ───────▶ │
       └──────────┴──────────┴──────────────────┘
       │◄────── 16 bytes (0x10) ──────────────▶│
```

- `val` 佔 offset 0x00–0x03（4 bytes）
- padding 佔 offset 0x04–0x07（4 bytes，對齊到 8-byte boundary）
- `next` 佔 offset 0x08–0x0F（8 bytes，64-bit 指標）

---

## 2. 完整資料結構記憶體配置

### Example 1: `head = [1, 2, 2, 1]`

#### 節點位址對應表

| 索引 | val | 假設位址 | next 指向 |
|------|-----|----------|-----------|
| 0 | 1 | 0xA000 | 0xB000 |
| 1 | 2 | 0xB000 | 0xC000 |
| 2 | 2 | 0xC000 | 0xD000 |
| 3 | 1 | 0xD000 | NULL (0x0) |

#### ASCII 記憶體圖

```
  head = 0xA000
   │
   ▼
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ 0xA000       │     │ 0xB000       │     │ 0xC000       │     │ 0xD000       │
│ val = 1      │     │ val = 2      │     │ val = 2      │     │ val = 1      │
│ next = 0xB000│────▶│ next = 0xC000│────▶│ next = 0xD000│────▶│ next = NULL  │
└──────────────┘     └──────────────┘     └──────────────┘     └──────────────┘
     [0]                  [1]                  [2]                  [3]
```

#### 邏輯串列圖

```
  [1] ───▶ [2] ───▶ [2] ───▶ [1] ───▶ NULL
0xA000   0xB000   0xC000   0xD000
```

---

## 3. 陣列索引版對應

Linked list 可對應到一維陣列（用 index 模擬 next 指標）：

| 陣列索引 | val | 模擬 next（索引） |
|----------|-----|-------------------|
| 0 | 1 | 1 |
| 1 | 2 | 2 |
| 2 | 2 | 3 |
| 3 | 1 | -1 (NULL) |

```
arr[] = { 1, 2, 2, 1 }
          [0] [1] [2] [3]
```

在陣列版中，回文判斷極為簡單：`arr[i] == arr[n-1-i]` for all `i < n/2`。

---

## 4. 搜尋 / 操作過程追蹤

### Example 1: `head = [1, 2, 2, 1]` → `true`

#### Phase 1: `endOfFirstHalf()` — 快慢指標

```
初始狀態：
  slow = 0xA000 (val=1)
  fast = 0xA000 (val=1)

                slow,fast
                   │
  [1] ───▶ [2] ───▶ [2] ───▶ [1] ───▶ NULL
0xA000   0xB000   0xC000   0xD000
```

**Step 1 檢查：** `fast->next(0xB000) != NULL ✓` 且 `fast->next->next(0xC000) != NULL ✓` → 進入迴圈

```
  fast = fast->next->next = 0xC000 (val=2)
  slow = slow->next       = 0xB000 (val=2)

            slow           fast
              │               │
  [1] ───▶ [2] ───▶ [2] ───▶ [1] ───▶ NULL
0xA000   0xB000   0xC000   0xD000
```

**Step 2 檢查：** `fast->next(0xD000) != NULL ✓` 但 `fast->next->next(NULL) == NULL ✗` → 離開迴圈

```
┌──────────────────────────────────┐
│ 指標移動總結                      │
│ slow: 0xA000 → 0xB000           │
│ fast: 0xA000 → 0xC000           │
│ 回傳: firstHalfEnd = 0xB000     │
└──────────────────────────────────┘
```

#### Phase 2: `reverseList(firstHalfEnd->next)` — 反轉後半段

輸入：`0xC000 (val=2) → 0xD000 (val=1) → NULL`

```
初始：prev = NULL, curr = 0xC000

─── 第 1 圈 ───
  next = curr->next = 0xD000
  curr->next = prev = NULL        // 0xC000->next 改為 NULL
  prev = curr = 0xC000
  curr = next = 0xD000

  NULL ◄── [2]    [1] ───▶ NULL
         0xC000  0xD000
            ↑       ↑
          prev    curr

─── 第 2 圈 ───
  next = curr->next = NULL
  curr->next = prev = 0xC000      // 0xD000->next 改為 0xC000
  prev = curr = 0xD000
  curr = next = NULL

  NULL ◄── [2] ◄── [1]
         0xC000  0xD000
                    ↑
                  prev  ← 回傳此值
```

```
┌──────────────────────────────────────────┐
│ 反轉結果                                  │
│ secondHalfStart = 0xD000 (val=1)         │
│ 鏈結：0xD000 → 0xC000 → NULL            │
└──────────────────────────────────────────┘
```

**整體串列此刻的狀態：**

```
  head              firstHalfEnd        secondHalfStart
0xA000              0xB000              0xD000
   │                   │                   │
  [1] ───▶ [2]───▶NULL    [1] ───▶ [2] ───▶ NULL
0xA000   0xB000          0xD000   0xC000
 前半段                    後半段（已反轉）
```

#### Phase 3: 逐節點比較

```
p1 = head = 0xA000
p2 = secondHalfStart = 0xD000

─── 回合 1 ───
  *p1 = *(0xA000) → val = 1
  *p2 = *(0xD000) → val = 1
  比較：1 == 1 ✓
  p1 = p1->next = 0xB000
  p2 = p2->next = 0xC000

─── 回合 2 ───
  *p1 = *(0xB000) → val = 2
  *p2 = *(0xC000) → val = 2
  比較：2 == 2 ✓
  p1 = p1->next = NULL（但此時 firstHalfEnd->next 已被切斷）
  p2 = p2->next = NULL

─── p2 == NULL → 離開迴圈 ───
  result = true ✓
```

#### Phase 4: 復原

```
reverseList(secondHalfStart = 0xD000)：
  [1] → [2] → NULL  反轉回  [2] → [1] → NULL
  0xD000→0xC000→NULL  →  0xC000→0xD000→NULL

firstHalfEnd->next = reverseList(0xD000) = 0xC000

復原後：
  [1] ───▶ [2] ───▶ [2] ───▶ [1] ───▶ NULL
0xA000   0xB000   0xC000   0xD000
                                       ✓ 與輸入相同
```

---

### Example 2: `head = [1, 2]` → `false`

#### 節點位址對應表

| 索引 | val | 假設位址 | next 指向 |
|------|-----|----------|-----------|
| 0 | 1 | 0xA000 | 0xB000 |
| 1 | 2 | 0xB000 | NULL (0x0) |

#### Phase 1: `endOfFirstHalf()`

```
初始：slow = 0xA000, fast = 0xA000
檢查：fast->next(0xB000) != NULL ✓, fast->next->next(NULL) == NULL ✗ → 不進迴圈
回傳：firstHalfEnd = 0xA000 (val=1)
```

#### Phase 2: `reverseList(0xB000)`

```
輸入：0xB000 (val=2) → NULL
只有一個節點，反轉後仍為 0xB000 → NULL
secondHalfStart = 0xB000
```

#### Phase 3: 比較

```
p1 = 0xA000 (val=1), p2 = 0xB000 (val=2)
比較：1 ≠ 2 → result = false
```

#### Phase 4: 復原

```
firstHalfEnd->next = reverseList(0xB000) = 0xB000
結果：[1] → [2] → NULL  ✓
```

---

## 5. 指標變數追蹤總表

### Example 1: `[1, 2, 2, 1]`

| 階段 | 步驟 | 指標 | 位址 | *val | 比較/動作 |
|------|------|------|------|------|-----------|
| endOfFirstHalf | 初始 | slow | 0xA000 | 1 | — |
| | 初始 | fast | 0xA000 | 1 | — |
| | 迭代1 | slow | 0xB000 | 2 | slow 前進 1 步 |
| | 迭代1 | fast | 0xC000 | 2 | fast 前進 2 步 |
| | 結束 | — | — | — | fast->next->next == NULL，停止 |
| reverseList | 圈1 | curr | 0xC000 | 2 | next 改指向 NULL |
| | 圈2 | curr | 0xD000 | 1 | next 改指向 0xC000 |
| 比較 | 回合1 | p1 | 0xA000 | 1 | 1 == 1 ✓ |
| | 回合1 | p2 | 0xD000 | 1 | — |
| | 回合2 | p1 | 0xB000 | 2 | 2 == 2 ✓ |
| | 回合2 | p2 | 0xC000 | 2 | — |
| 復原 | — | — | — | — | 串列回到原狀 |

**統計：** 指標移動 10 次、節點存取 8 次、比較 2 次。

### Example 2: `[1, 2]`

| 階段 | 步驟 | 指標 | 位址 | *val | 比較/動作 |
|------|------|------|------|------|-----------|
| endOfFirstHalf | 初始 | slow | 0xA000 | 1 | 不進迴圈 |
| reverseList | 圈1 | curr | 0xB000 | 2 | 單節點，不變 |
| 比較 | 回合1 | p1 | 0xA000 | 1 | 1 ≠ 2 ✗ |
| | 回合1 | p2 | 0xB000 | 2 | result = false |

**統計：** 指標移動 3 次、節點存取 3 次、比較 1 次。

---

## 6. 記憶體存取模式比較

| 面向 | 指標版（Linked List） | 陣列版（Array） |
|------|----------------------|-----------------|
| **存取模式** | 間接跳轉：`node->next` 每次解引一個指標 | 連續存取：`arr[i]` 直接計算偏移 |
| **Cache 行為** | 差——節點散布在 heap 各處，每次 `next` 跳轉可能 cache miss | 好——元素連續排列，prefetcher 可預取整條 cache line |
| **空間開銷** | 每節點 16 bytes（val 4B + padding 4B + next 8B） | 每元素 4 bytes（只存 int val） |
| **額外記憶體** | O(1)（原地操作，只有幾個指標變數） | O(n)（需複製整個陣列） |
| **插入/刪除** | O(1)（給定前驅節點） | O(n)（需搬移元素） |
| **回文判斷** | 需要快慢指標 + 反轉——比較複雜 | 直接 `arr[i] == arr[n-1-i]`——極為簡單 |

### 每次存取的成本估計（現代 x86-64）

```
L1 cache hit:   ~1 ns     ← 陣列連續存取通常命中 L1
L2 cache hit:   ~4 ns
L3 cache hit:   ~12 ns
DRAM access:    ~60-100 ns ← linked list 隨機跳轉可能落到這裡
```

對於 n = 100,000 的串列，陣列版的比較階段（50,000 次存取 × ~1 ns）約 50 μs；linked list 版（100,000 次跳轉 × ~60 ns worst case）約 6 ms。Cache locality 差異可達 **100 倍**。

---

## 7. 堆疊記憶體對比

### 方法三（原地反轉）— 迭代版

```
isPalindrome() 的堆疊框架：

┌────────────────────────────┐  ◄─ RSP
│ head          (8B, 指標)    │
│ firstHalfEnd  (8B, 指標)    │
│ secondHalfStart (8B, 指標)  │
│ p1            (8B, 指標)    │
│ p2            (8B, 指標)    │
│ result        (1B, bool)   │
│ return addr   (8B)         │
└────────────────────────────┘
≈ 49 bytes → 對齊後約 64 bytes

endOfFirstHalf() 的堆疊框架：
┌────────────────────────────┐
│ head  (8B)                 │
│ fast  (8B)                 │
│ slow  (8B)                 │
│ return addr (8B)           │
└────────────────────────────┘
≈ 32 bytes

reverseList() 的堆疊框架：
┌────────────────────────────┐
│ head  (8B)                 │
│ prev  (8B)                 │
│ curr  (8B)                 │
│ next  (8B)                 │
│ return addr (8B)           │
└────────────────────────────┘
≈ 40 bytes

最大堆疊深度 = isPalindrome + endOfFirstHalf or reverseList
            ≈ 64 + 40 = 104 bytes  ← O(1)，與 n 無關
```

### 方法二（遞迴）— 堆疊爆炸

```
每層遞迴框架：
┌────────────────────────────┐
│ currentNode  (8B)          │
│ return addr  (8B)          │
│ saved registers (~16B)     │
└────────────────────────────┘
≈ 32 bytes per frame

n = 100,000 時：
  堆疊使用 ≈ 32 × 100,000 = 3.2 MB

Linux 預設 thread stack size = 8 MB
  → 100K 節點用掉 40% 堆疊空間
  → 200K 節點可能 stack overflow！
```

```
遞迴展開示意（n=4, [1,2,2,1]）：

  ┌─ recursivelyCheck(node[0]=1) ─────────────┐
  │   ┌─ recursivelyCheck(node[1]=2) ─────────┤
  │   │   ┌─ recursivelyCheck(node[2]=2) ─────┤
  │   │   │   ┌─ recursivelyCheck(node[3]=1) ─┤
  │   │   │   │   ┌─ recursivelyCheck(NULL)    │
  │   │   │   │   │   return true              │
  │   │   │   │   └───────────────────── ◄─────┤
  │   │   │   │   compare: node[3].val(1)==front.val(1) ✓
  │   │   │   │   front = front->next
  │   │   │   └─────────────────────── ◄───────┤
  │   │   │   compare: node[2].val(2)==front.val(2) ✓
  │   │   └───────────────────────── ◄─────────┤
  │   │   compare: node[1].val(2)==front.val(2) ✓
  │   └─────────────────────────── ◄───────────┤
  │   compare: node[0].val(1)==front.val(1) ✓
  └───────────────────────────── ◄─────────────┘
  return true

  最大堆疊深度 = n 層 = O(n)
```

---

## 8. 退化情況分析

### 單節點串列 `[5]`

```
  head = 0xA000
   │
  [5] ───▶ NULL

endOfFirstHalf: fast->next == NULL → 不進迴圈 → 回傳 0xA000
reverseList(NULL): curr == NULL → 立即回傳 NULL → secondHalfStart = NULL
比較: p2 == NULL → 不進迴圈 → result = true（正確：單節點為回文）
復原: firstHalfEnd->next = reverseList(NULL) = NULL → 不變
```

### 全相同值 `[7, 7, 7, 7, 7]`（n=5, 奇數）

```
  [7] → [7] → [7] → [7] → [7] → NULL
0xA000 0xB000 0xC000 0xD000 0xE000

endOfFirstHalf:
  迭代1: slow=0xB000, fast=0xC000
  迭代2: slow=0xC000, fast=0xE000
  fast->next == NULL → 停止
  firstHalfEnd = 0xC000 (中間節點 val=7)

後半段: 0xD000(7) → 0xE000(7) → NULL
反轉:   0xE000(7) → 0xD000(7) → NULL

比較: 7==7 ✓, 7==7 ✓ → true（正確）

  中間節點 0xC000(7) 被包在前半段，比較時被忽略 ✓
```

### 最長可能串列 n = 100,000

```
方法三（原地反轉）：
  - 額外記憶體：~104 bytes（堆疊），與 n 無關
  - 指標跳轉：~4 × n/2 = ~200,000 次
  - 估計延遲：200K × ~30 ns (avg cache miss) ≈ 6 ms

方法一（陣列複製）：
  - 額外記憶體：100,000 × 4 bytes = 400 KB
  - 陣列存取：~1.5n = ~150,000 次
  - 估計延遲：150K × ~1 ns (L1 hit) ≈ 0.15 ms ← 更快！

結論：方法三省記憶體（O(1) vs O(n)），但因 cache 效能較差，
     實際執行速度可能比方法一慢。這是經典的 時間-空間 trade-off。
```
