# 692. Top K Frequent Words — 記憶體配置與演算法追蹤

> Last verified (2026-04-01).

以下用假設的 hex 位址，逐步追蹤 hash table 頻率統計 + min heap top-k 兩個階段的記憶體變化。

---

## 1. Struct 記憶體佈局

### WordEntry（8 bytes on 64-bit, 考慮 alignment）

```
偏移   大小   欄位
─────────────────────
0x00   8B     char* word      （指向原始 words[i] 字串）
0x08   4B     int count       （出現次數）
0x0C   4B     (padding)       （對齊到 16 bytes）
```

假設位址 0xA000 的 WordEntry：
```
┌────────────────────────────────────────┐
│ 0xA000  word:  0xF000 ───▶ "i\0"      │
│ 0xA008  count: 2                       │
│ 0xA00C  (pad)                          │
└────────────────────────────────────────┘
```

### HashNode（24 bytes on 64-bit）

```
偏移   大小   欄位
─────────────────────
0x00   8B     char* key          （指向字串）
0x08   4B     int entryIndex     （對應 entries[] 中的索引）
0x0C   4B     (padding)
0x10   8B     struct HashNode* next （鏈結到下一個 collision 節點）
```

假設位址 0xC000 的 HashNode：
```
┌────────────────────────────────────────┐
│ 0xC000  key:        0xF000 ───▶ "i\0" │
│ 0xC008  entryIndex: 0                  │
│ 0xC00C  (pad)                          │
│ 0xC010  next:       NULL               │
└────────────────────────────────────────┘
```

---

## 2. 完整資料結構記憶體配置

以 Example 1 為例：`words = ["i","love","leetcode","i","love","coding"]`, `k = 2`

### 字串位址假設

| words 索引 | 字串 | 假設位址 |
|---|---|---|
| words[0] | `"i"` | 0xF000 |
| words[1] | `"love"` | 0xF010 |
| words[2] | `"leetcode"` | 0xF020 |
| words[3] | `"i"` | 0xF030（同內容，不同指標） |
| words[4] | `"love"` | 0xF040 |
| words[5] | `"coding"` | 0xF050 |

### entries[] 陣列（頻率統計完成後）

| 索引 | 假設位址 | word 指標 | count |
|---|---|---|---|
| entries[0] | 0xA000 | 0xF000 → `"i"` | 2 |
| entries[1] | 0xA010 | 0xF010 → `"love"` | 2 |
| entries[2] | 0xA020 | 0xF020 → `"leetcode"` | 1 |
| entries[3] | 0xA030 | 0xF050 → `"coding"` | 1 |

### Hash Table（部分 bucket 示意）

假設 `hashWord("i") % 1024 = 105`, `hashWord("love") % 1024 = 237`, `hashWord("leetcode") % 1024 = 512`, `hashWord("coding") % 1024 = 105`（假設 "coding" 和 "i" 碰撞在同一 bucket）:

```
table[105]:
  ┌──────────────┐     ┌──────────────┐
  │ 0xC030       │     │ 0xC000       │
  │ key:"coding" │     │ key: "i"     │
  │ idx: 3       │────▶│ idx: 0       │────▶ NULL
  └──────────────┘     └──────────────┘

table[237]:
  ┌──────────────┐
  │ 0xC010       │
  │ key:"love"   │
  │ idx: 1       │────▶ NULL
  └──────────────┘

table[512]:
  ┌──────────────────┐
  │ 0xC020           │
  │ key: "leetcode"  │
  │ idx: 2           │────▶ NULL
  └──────────────────┘
```

---

## 3. Min Heap 記憶體配置與操作追蹤

heap 是 `WordEntry*` 指標陣列，大小上限 `k = 2`。

### 陣列索引公式

```
parent(i) = (i - 1) / 2
left(i)   = 2 * i + 1
right(i)  = 2 * i + 2
```

### 完整 heap 操作追蹤（Example 1）

#### Step 1：push entries[0] = ("i", 2)

heap 未滿（heapSize=0 < k=2），直接放尾巴。

```
heap[0] = &entries[0]  (0xA000, "i", 2)
heapSize = 1

heap 陣列：
┌──────────────┐
│ [0]: 0xA000  │  "i", count=2
│ [1]: (empty) │
└──────────────┘

樹狀視圖：
     ("i", 2)
```

#### Step 2：push entries[1] = ("love", 2)

heap 未滿（heapSize=1 < k=2），放到 index 1，再 siftUp。

比較 heap[1] vs heap[0]：`compareForHeap(("love",2), ("i",2))`
- count 相同（都是 2）
- 比較 `strcmp("i", "love")` → 負值，所以 ("love",2) 比較差
- `compareForHeap` 回傳負值 → ("love",2) 應該在 ("i",2) 前面（靠近 root）
- 交換！

```
heap[0] = &entries[1]  (0xA010, "love", 2)  ← 比較差，放 root
heap[1] = &entries[0]  (0xA000, "i", 2)
heapSize = 2

heap 陣列：
┌──────────────┐
│ [0]: 0xA010  │  "love", count=2  ← root（最差）
│ [1]: 0xA000  │  "i", count=2
└──────────────┘

樹狀視圖：
     ("love", 2)   ← root
        │
     ("i", 2)
```

#### Step 3：push entries[2] = ("leetcode", 1)

heap 已滿（heapSize=2 == k=2）。比較 candidate 和 heap[0]。

`isBetter(("leetcode",1), ("love",2))`？
- `compareForAnswer`：count 1 < count 2 → 回傳 1（正值）
- 所以 ("leetcode",1) 不比 ("love",2) 好 → **忽略**

heap 不變。

#### Step 4：push entries[3] = ("coding", 1)

heap 已滿。比較 candidate 和 heap[0]。

`isBetter(("coding",1), ("love",2))`？
- count 1 < count 2 → ("coding",1) 不比 ("love",2) 好 → **忽略**

heap 不變。

#### 最終 heap 狀態

```
heap 陣列：
┌──────────────┐
│ [0]: 0xA010  │  "love", count=2  ← root
│ [1]: 0xA000  │  "i", count=2
└──────────────┘
```

#### Pop 階段：反向填充答案

```
i = 1（k-1）：pop heap → 得到 ("love", 2)
  answer[1] = "love"

  pop 過程：
    top = heap[0] = ("love",2)
    heapSize: 2 → 1
    heap[0] = heap[1] = ("i",2)
    siftDown → 無子節點，不用動

i = 0：pop heap → 得到 ("i", 2)
  answer[0] = "i"

最終 answer = ["i", "love"] ✓
```

---

## 4. Example 2 操作追蹤

`words = ["the","day","is","sunny","the","the","the","sunny","is","is"]`, `k = 4`

### 頻率統計結果

| entries 索引 | word | count |
|---|---|---|
| 0 | `"the"` | 4 |
| 1 | `"day"` | 1 |
| 2 | `"is"` | 3 |
| 3 | `"sunny"` | 2 |

### Heap 操作逐步追蹤

**Step 1**: push ("the", 4) → heap 未滿
```
heap: [("the",4)]   heapSize=1
```

**Step 2**: push ("day", 1) → heap 未滿，siftUp
```
比較 ("day",1) vs ("the",4)：count 1<4 → ("day",1) 比較差 → 要靠近 root
交換後：heap: [("day",1), ("the",4)]   heapSize=2
```

**Step 3**: push ("is", 3) → heap 未滿，siftUp
```
index=2, parent=0
比較 ("is",3) vs ("day",1)：count 3>1 → ("is",3) 不比 ("day",1) 差 → 不交換
heap: [("day",1), ("the",4), ("is",3)]   heapSize=3
```

**Step 4**: push ("sunny", 2) → heap 未滿，siftUp
```
index=3, parent=1
比較 ("sunny",2) vs ("the",4)：count 2<4 → ("sunny",2) 比較差 → 交換
heap: [("day",1), ("sunny",2), ("is",3), ("the",4)]

繼續 siftUp：index=1, parent=0
比較 ("sunny",2) vs ("day",1)：count 2>1 → 不比 ("day",1) 差 → 停止

最終：heap: [("day",1), ("sunny",2), ("is",3), ("the",4)]   heapSize=4
```

完成！所有 4 個不同單字都進了 heap（uniqueCount == k == 4）。

### Heap 最終樹狀圖

```
           ("day", 1)          ← root（最差）
          /          \
   ("sunny", 2)    ("is", 3)
      /
 ("the", 4)
```

### Pop 階段

```
i=3: pop → ("day",1)     → answer[3] = "day"
  移 heap 末端到 root，siftDown
  heap: [("the",4), ("sunny",2), ("is",3)]

           ("the", 4)
          /          \
   ("sunny", 2)    ("is", 3)

  siftDown: ("the",4) 不比任何子節點差，停止（因為 count 較高=較不差）
  等等，再檢查：compareForHeap(("sunny",2), ("the",4))
    count 2 < 4 → ("sunny",2) 比較差 → 回傳 -1
    所以 smallest = leftChild
  compareForHeap(("is",3), ("sunny",2))
    count 3 > 2 → ("is",3) 不比 ("sunny",2) 差
    smallest 不變 = leftChild
  交換 root 和 leftChild：
  heap: [("sunny",2), ("the",4), ("is",3)]

i=2: pop → ("sunny",2)   → answer[2] = "sunny"
  heap: [("is",3), ("the",4)]
  siftDown: ("is",3) vs ("the",4)
    count 3<4 → ("is",3) 比較差 → smallest = 自己 → 停止
  heap: [("is",3), ("the",4)]

i=1: pop → ("is",3)      → answer[1] = "is"
  heap: [("the",4)]

i=0: pop → ("the",4)     → answer[0] = "the"

最終 answer = ["the", "is", "sunny", "day"] ✓
```

---

## 5. 指標變數追蹤總表

### Example 1

| 步驟 | 操作 | heap[0] | heap[1] | heapSize | 決策 |
|---|---|---|---|---|---|
| 1 | push ("i",2) | ("i",2) | — | 1 | 未滿，直接放 |
| 2 | push ("love",2) | ("love",2) | ("i",2) | 2 | siftUp 交換 |
| 3 | 嘗試 ("leetcode",1) | ("love",2) | ("i",2) | 2 | 不比 root 好，忽略 |
| 4 | 嘗試 ("coding",1) | ("love",2) | ("i",2) | 2 | 不比 root 好，忽略 |
| pop1 | pop root | ("i",2) | — | 1 | answer[1]="love" |
| pop2 | pop root | — | — | 0 | answer[0]="i" |

統計：heap push 2 次、比較後忽略 2 次、pop 2 次。

### Example 2

| 步驟 | 操作 | heap 內容 (root→) | heapSize | 決策 |
|---|---|---|---|---|
| 1 | push ("the",4) | [("the",4)] | 1 | 未滿 |
| 2 | push ("day",1) | [("day",1), ("the",4)] | 2 | siftUp 交換 |
| 3 | push ("is",3) | [("day",1), ("the",4), ("is",3)] | 3 | 不交換 |
| 4 | push ("sunny",2) | [("day",1), ("sunny",2), ("is",3), ("the",4)] | 4 | siftUp 一次 |
| pop1 | pop | [("sunny",2), ("the",4), ("is",3)] | 3 | answer[3]="day" |
| pop2 | pop | [("is",3), ("the",4)] | 2 | answer[2]="sunny" |
| pop3 | pop | [("the",4)] | 1 | answer[1]="is" |
| pop4 | pop | [] | 0 | answer[0]="the" |

統計：heap push 4 次、pop 4 次。因為 uniqueCount == k，所有候選都進了 heap。

---

## 6. 記憶體存取模式分析

### Hash Table 存取

本題的 hash table 以陣列 `table[HASH_BUCKETS]` 為骨幹，每個 bucket 是一條 linked list。

| 面向 | 分析 |
|---|---|
| 空間局部性 | `table[]` 本身是連續陣列，cache friendly；但鏈結上的 `HashNode` 是個別 `malloc`，散落在 heap 各處 |
| 時間局部性 | 第一次 pass 建表時，每個 bucket 只寫入一次；第二次 pass（若有相同字）沿鏈走訪，長度通常很短 |
| 改善方式 | 可以用 open addressing（線性探測）避免鏈結的 pointer chasing，但增加 collision 處理複雜度 |

### Heap 存取

heap 是 `WordEntry*` 指標陣列。

| 面向 | 分析 |
|---|---|
| 空間局部性 | 指標陣列本身連續，但指向的 `WordEntry` 散落在 `entries[]` 各處（不過 `entries` 本身也是連續陣列，所以整體還算 cache friendly） |
| siftDown 路徑 | 每次 siftDown 最多走 log₂(k) 步，k ≤ 500，所以最多 9 步 — 幾乎不會 cache miss |
| 和陣列排序比較 | 如果改用 qsort（Approach 1），排序時的比較次數更多（O(N log N) vs O(N log k)），但 qsort 在連續陣列上的 cache 行為比 heap 的 pointer indirection 更好 |

---

## 7. Hash Table 鏈結 vs. 開放定址對比

| 面向 | Separate Chaining（本題） | Open Addressing（線性探測） |
|---|---|---|
| 額外記憶體 | 每個 entry 一個 `HashNode`（24 bytes） | 無額外 node，但 bucket 陣列本身要預留空位 |
| Cache 行為 | pointer chasing，較差 | 線性掃描連續記憶體，較好 |
| 刪除 | 直接 unlink 節點 | 需要 tombstone 或 Robin Hood 搬移 |
| 實作難度 | 低 | 中（需處理 clustering） |
| 面試建議 | 白板上最直覺，推薦使用 | 可以口頭提到作為優化方向 |
