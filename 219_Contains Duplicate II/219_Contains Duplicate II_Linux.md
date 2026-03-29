# LeetCode 219: Contains Duplicate II - Linux Kernel 對照

## 問題概述

給定整數陣列 `nums` 和整數 `k`，判斷陣列中是否存在兩個不同的索引 `i` 和 `j`，使得 `nums[i] == nums[j]` 且 `abs(i - j) <= k`。

**解題策略**：使用滑動窗口搭配雜湊表，維護一個大小不超過 `k` 的窗口內的元素集合。

---

## 1. Linux 相關原始碼位置

### 1.1 核心雜湊表實現

| 檔案路徑 | 功能描述 | 相關性 |
|---------|---------|--------|
| `include/linux/hashtable.h` | 雜湊表巨集定義（`DEFINE_HASHTABLE`, `hash_add`, `hash_del`, `hash_for_each_possible`） | **最高** — 直接對標本解題的雜湊表操作 |
| `include/linux/hash.h` | 雜湊函數與整數雜湊實現 | **高** — 與鍵值雜湊方式相關 |
| `include/linux/jhash.h` | Jenkins Hash 函數（32位/64位整數） | **高** — 用於整數型鍵值的最佳哈希方式 |
| `include/linux/list.h` | 雙向鏈表實現（衝突鏈） | **高** — 雜湊表衝突解決的基礎結構 |
| `lib/rbtree.c` | 紅黑樹（部分情況替代雜湊表） | 中等 — 替代方案討論 |

### 1.2 核心應用案例

```
Kernel Hash Table 使用場景
┌─────────────────────────────────────────────────────┐
│  • Dentry Cache (目錄項快取)                         │
│    → 檔案路徑快速查詢                                │
│  • Inode Hash Table                                 │
│    → 檔案系統內節點快速查找                          │
│  • PID Hash Table                                   │
│    → 進程快速查詢（pid_hashfn）                      │
│  • Network Connection Lookup                        │
│    → TCP/UDP 連接查詢                               │
│  • Page Cache Hash                                  │
│    → 頁框內容查詢加速                                │
└─────────────────────────────────────────────────────┘
```

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 LeetCode 解題程式碼示例

```c
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define HASH_SIZE 10007

typedef struct {
    int key;
    int value;  // unused in this problem
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry* table;
    int size;
} HashTable;

HashTable* createHashTable(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    ht->table = (HashEntry*)calloc(size, sizeof(HashEntry));
    ht->size = size;
    return ht;
}

int hashFunction(int key, int size) {
    int h = key % size;
    return h < 0 ? h + size : h;
}

bool hashTableContains(HashTable* ht, int key) {
    int idx = hashFunction(key, ht->size);
    int start = idx;
    while (ht->table[idx].occupied) {
        if (ht->table[idx].key == key) {
            return true;
        }
        idx = (idx + 1) % ht->size;
        if (idx == start) break;  // 表滿
    }
    return false;
}

void hashTableAdd(HashTable* ht, int key) {
    int idx = hashFunction(key, ht->size);
    while (ht->table[idx].occupied && ht->table[idx].key != key) {
        idx = (idx + 1) % ht->size;
    }
    if (!ht->table[idx].occupied) {
        ht->table[idx].key = key;
        ht->table[idx].occupied = true;
    }
}

void hashTableRemove(HashTable* ht, int key) {
    int idx = hashFunction(key, ht->size);
    int start = idx;
    while (ht->table[idx].occupied) {
        if (ht->table[idx].key == key) {
            ht->table[idx].occupied = false;
            return;
        }
        idx = (idx + 1) % ht->size;
        if (idx == start) break;
    }
}

bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    if (k == 0) return false;

    HashTable* ht = createHashTable(HASH_SIZE);

    for (int i = 0; i < numsSize; i++) {
        if (hashTableContains(ht, nums[i])) {
            free(ht->table);
            free(ht);
            return true;
        }
        hashTableAdd(ht, nums[i]);

        // 保持窗口大小不超過 k
        if (i >= k) {
            hashTableRemove(ht, nums[i - k]);
        }
    }

    free(ht->table);
    free(ht);
    return false;
}
```

### 2.2 Linux Kernel 雜湊表實現對照

#### Kernel 雜湊表定義 (`include/linux/hashtable.h`)

```c
// Kernel 風格（簡化版）
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1U << (bits)] = { [0 ... ((1U << (bits)) - 1)] = HLIST_HEAD_INIT }

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

// Kernel 插入操作
#define hash_add(hashtable, node, key) \
    hlist_add_head(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])

// Kernel 遍歷操作
#define hash_for_each_possible(name, obj, member, key) \
    hlist_for_each_entry(obj, &name[hash_min(key, HASH_BITS(name))], member)

// Kernel 刪除操作
#define hash_del(node) hlist_del(node)
```

#### 對照表

| 功能 | LeetCode 實作 | Kernel 實作 | 關鍵差異 |
|------|-------------|------------|---------|
| **資料結構** | 開放尋址（線性探測） | 鏈式衝突（hlist） | Kernel 採用鏈式，更靈活 |
| **插入** | `hashTableAdd()` | `hash_add()` 巨集 | Kernel 使用雙向鏈表 |
| **查詢** | 線性探測迴圈 | `hash_for_each_possible()` | Kernel 遍歷衝突鏈 |
| **刪除** | 標記 `occupied=false` | `hash_del()` 更新指針 | Kernel 直接拆除節點 |
| **雜湊函數** | 模運算 `key % size` | `hash_min(key, bits)` | Kernel 用位掩碼加快速度 |
| **調整因子** | 固定 `HASH_SIZE` | 動態位數 `bits` | Kernel 支持2的冪次大小 |

### 2.3 Kernel 雜湊函數對照 (`include/linux/hash.h` 與 `include/linux/jhash.h`)

```c
// Kernel 整數雜湊（32位）
static inline u32 hash_32(u32 val, unsigned int bits)
{
    /* High bits depend on low bits, so use full 32-bit mixing below. */
    return (val * GOLDEN_RATIO_32) >> (32 - bits);
}

// Jenkins Hash for 32-bit integers
#define __jhash_mix(a, b, c)  \
{                             \
    a += b; a ^= rot(b, 11);  \
    b += c; b ^= rot(c, 17);  \
    c += a; c ^= rot(a, 19);  \
}

// 對比 LeetCode 簡單模運算
// LeetCode: int idx = key % HASH_SIZE;
// Kernel:   int idx = (key * GOLDEN_RATIO_32) >> (32 - bits);
```

**Kernel 優勢**：
- 使用黃金比例 (GOLDEN_RATIO_32) 確保分散均勻
- 位移操作比模運算更快
- Jenkins Hash 對整數衝突敏感

---

## 3. 主管 Code Review 角度考題

### 3.1 演算法複雜度與權衡

**Review 問題 1**：*「為什麼選擇雜湊表而非其他資料結構？」*

```
回答要點：
┌────────────────────────────────────────────────────────┐
│ 資料結構選擇分析                                       │
├────────────────────────────────────────────────────────┤
│ 雜湊表                                                │
│  ✓ 平均查詢：O(1)                                      │
│  ✓ 空間：O(min(n, k))，只需保存窗口內元素              │
│  ✓ 滑動窗口：O(1) 插入、O(1) 刪除                       │
│  總時間：O(n)，總空間：O(min(n, k))                    │
│                                                       │
│ 平衡二叉樹 (TreeSet)                                   │
│  ✗ 查詢：O(log k)，整體 O(n log k)                      │
│  ✓ 無需雜湊函數調整                                    │
│  ✗ 空間：O(min(n, k))，但常數項大                      │
│                                                       │
│ 線性陣列                                              │
│  ✗ 查詢：O(k)，整體 O(n*k)                             │
│  ✗ 不可行於大 k 值                                     │
└────────────────────────────────────────────────────────┘
```

**Review 問題 2**：*「如何選擇合適的雜湊表大小？」*

```c
// 次優實作（固定大小）
#define HASH_SIZE 10007  // 素數，但不靈活

// 最優實作（動態調整）
int optimalHashSize(int k) {
    // 遵循 Kernel 實踐：使用 2 的冪次
    int size = 1;
    while (size < k) {
        size <<= 1;  // size *= 2
    }
    return size;
}
// 時間複雜度：O(log k) 初始化，後續 O(1) 平均
```

### 3.2 邊界條件與實作細節

**Review 問題 3**：*「邊界情況處理是否完整？」*

```c
// ❌ 不完整
bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    HashTable* ht = createHashTable(HASH_SIZE);
    for (int i = 0; i < numsSize; i++) {
        if (hashTableContains(ht, nums[i])) {
            return true;  // 沒有釋放 ht！
        }
        hashTableAdd(ht, nums[i]);
        if (i >= k) {
            hashTableRemove(ht, nums[i - k]);
        }
    }
    free(ht->table);
    free(ht);
    return false;
}

// ✓ 完整版本
bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    if (numsSize == 0 || k == 0) return false;
    if (k >= numsSize) k = numsSize - 1;  // 優化：窗口不超過陣列

    HashTable* ht = createHashTable(HASH_SIZE);
    bool result = false;

    for (int i = 0; i < numsSize; i++) {
        if (hashTableContains(ht, nums[i])) {
            result = true;
            break;
        }
        hashTableAdd(ht, nums[i]);

        if (i >= k) {
            hashTableRemove(ht, nums[i - k]);
        }
    }

    free(ht->table);
    free(ht);
    return result;
}
```

**Review 問題 4**：*「是否考慮整數溢位與負數？」*

```c
// 雜湊函數必須處理負數
int hashFunction(int key, int size) {
    int h = key % size;
    // 確保結果為正
    return (h < 0) ? (h + size) : h;
}

// 驗證
// key = -5, size = 10
// h = -5 % 10 = -5（在 C 中可能為負）
// 修正：(-5 % 10) + 10 = 5 ✓
```

### 3.3 Kernel 最佳實踐應用

**Review 問題 5**：*「能否應用 Kernel 中的最佳實踐？」*

```c
// Kernel 風格的改進實作
#include <stdint.h>

// 使用黃金比例替代模運算
#define GOLDEN_RATIO_32 0x61C88647U
#define HASH_BITS 14  // 16K 項，2^14

static inline uint32_t hash_32_kernel_style(int key) {
    uint32_t val = (uint32_t)key;
    return (val * GOLDEN_RATIO_32) >> (32 - HASH_BITS);
}

// 鏈式衝突改進
typedef struct HashNode {
    int key;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode** table;
    int bits;
} HashTableChain;

HashTableChain* createHashTableChain(int bits) {
    HashTableChain* ht = malloc(sizeof(HashTableChain));
    ht->bits = bits;
    ht->table = calloc(1 << bits, sizeof(HashNode*));
    return ht;
}

bool hashTableChainContains(HashTableChain* ht, int key) {
    uint32_t idx = hash_32_kernel_style(key);
    for (HashNode* node = ht->table[idx]; node; node = node->next) {
        if (node->key == key) return true;
    }
    return false;
}

void hashTableChainAdd(HashTableChain* ht, int key) {
    uint32_t idx = hash_32_kernel_style(key);
    HashNode* node = malloc(sizeof(HashNode));
    node->key = key;
    node->next = ht->table[idx];
    ht->table[idx] = node;
}

void hashTableChainRemove(HashTableChain* ht, int key) {
    uint32_t idx = hash_32_kernel_style(key);
    HashNode **pp = &ht->table[idx];
    while (*pp) {
        if ((*pp)->key == key) {
            HashNode* tmp = *pp;
            *pp = (*pp)->next;
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}
```

---

## 4. 面試加分總結

### 4.1 Kernel 知識加分點

```
面試官評分維度
┌─────────────────────────────────────────────────────┐
│ ★ 基礎演算法知識（必備）                            │
│   □ 滑動窗口 + 雜湊表組合理解                        │
│   □ 時間複雜度 O(n)、空間複雜度 O(min(n,k))         │
│   □ 邊界條件正確處理                                │
│                                                     │
│ ★★ Kernel 知識層面（中級加分）                     │
│   □ 了解 hashtable.h 中的 DEFINE_HASHTABLE 巨集     │
│   □ 認識鏈式衝突 vs 開放尋址的優劣                   │
│   □ 理解 Kernel 為何使用 hlist_head / hlist_node    │
│                                                     │
│ ★★★ 系統級優化（高級加分）                        │
│   □ 黃金比例 (GOLDEN_RATIO_32) 的應用               │
│   □ Jenkins Hash 對隨機性的重要性                   │
│   □ 位掩碼運算替代模運算的性能優勢                  │
│   □ Kernel hash_min() 與動態位數調整                │
│                                                     │
│ ★★★★ 工程思維（頂級加分）                        │
│   □ Kernel 使用鏈式不同於教科書原因                 │
│   □ 常數項優化、CPU 快取考慮                        │
│   □ 如何應對真實負載（突發、動態擴展）              │
│   □ 內存碎片化問題與解決方案                        │
└─────────────────────────────────────────────────────┘
```

### 4.2 關鍵論述框架

**面試加分段落 1**：*雜湊表設計選擇*

```
「在這道題中，我採用雜湊表搭配滑動窗口，
直接受到 Linux Kernel dentry_cache 設計的啟發。

Kernel 為了加快檔案查詢，維護一個全局雜湊表，
對每個目錄項進行鏈式衝突處理。本題同樣利用：

1. 邊界時間窗口 [i-k, i] 對應 dentry_cache 的限定作用域
2. 雜湊表快速插入/刪除對應檔案系統熱操作需求
3. 空間優化 O(min(n,k)) 對應 Kernel 內存受限場景」
```

**面試加分段落 2**：*雜湊函數最佳實踐*

```
「簡單的模運算 (key % size) 會導致均勻分佈差。
Kernel 採用 GOLDEN_RATIO_32 乘法雜湊：

    hash_idx = (val * 0x61C88647U) >> (32 - bits)

相比模運算的優勢：
• 位移運算比整除快 10-20%
• 黃金比例確保 key 的低位變化也影響高位
• 天然對齊 2^n 大小，避免 rehash 開銷

若面試題要求優化，這是立竿見影的改進。」
```

**面試加分段落 3**：*衝突解決策略*

```
「開放尋址 vs 鏈式衝突的選擇：

開放尋址（本題初版實作）：
  優點：無額外指針開銷，快取局部性好
  缺點：刪除複雜，一個長衝突鏈導致整體性能下降
  應用：Redis、Go 哈希表等追求性能場景

鏈式衝突（Kernel 採用）：
  優點：刪除 O(1)、衝突不互相影響、支持動態擴展
  缺點：多指針、快取失效、內存碎片化
  應用：Kernel、Java HashMap、通用場景

對於本題的滑動窗口：頻繁刪除使鏈式優勢明顯。」
```

### 4.3 延伸技術問題準備

| 問題 | 回答要點 | Kernel 參考 |
|------|---------|------------|
| 如何處理 `k > n`？ | 窗口上限應為 `n-1`，避免無謂計算 | 資源管理的邊界保護 |
| 如果 `k` 動態變化？ | 需要動態調整窗口大小，插入/刪除邏輯不變 | Kernel 衝突鏈支持動態調整 |
| 數據量 1000 萬能否優化？ | 考慮位陣列替代（若值域有限）或外存雜湊 | Kernel mmap 大數據結構 |
| Collision 過多怎麼辦？ | Rehash 到更大表；Kernel 用二級雜湊表 | hashtable.h 中的懶加載策略 |
| 多線程環境下？ | 加鎖或用無鎖結構（RCU in Kernel） | Kernel 讀拷貝更新機制 |

---

## 5. Sources

### 5.1 Linux Kernel 原始碼（GitHub 鏡像）

- **hashtable.h 完整實現**
  https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h

- **hash.h 與整數雜湊**
  https://github.com/torvalds/linux/blob/master/include/linux/hash.h

- **jhash.h Jenkins Hash**
  https://github.com/torvalds/linux/blob/master/include/linux/jhash.h

- **list.h 鏈表實現（衝突鏈基礎）**
  https://github.com/torvalds/linux/blob/master/include/linux/list.h

- **dentry.c 實際應用案例**
  https://github.com/torvalds/linux/blob/master/fs/dcache.c

### 5.2 LeetCode 問題參考

- **LeetCode 219 - Contains Duplicate II**
  https://leetcode.com/problems/contains-duplicate-ii/

- **相關題目**
  - LeetCode 217: Contains Duplicate（無距離限制）
  - LeetCode 220: Contains Duplicate III（無距離限制 + 值域限制）

### 5.3 進階閱讀

- **Linux 核心設計與實現** 第 11-12 章（雜湊表與快取）
- **Algorithm Design Manual** - Skiena 著，第 12 章雜湊表
- **Performance Analysis and Tuning on Modern CPUs** - Yasin 著

---

## 補充：執行軌跡示例

```
輸入: nums = [99,99], k = 2

執行流程（滑動窗口示意）：
┌──────────────────────────────────────────────┐
│ 初始狀態：窗口 = {}，雜湊表 = 空               │
├──────────────────────────────────────────────┤
│ i=0: nums[0]=99                              │
│   • 查詢(99): 不存在                          │
│   • 插入(99)，窗口 = {99}                     │
│   • i=0 < k=2，不刪除                         │
│   • 時間點：hash_add(99) → O(1)               │
├──────────────────────────────────────────────┤
│ i=1: nums[1]=99                              │
│   • 查詢(99): 存在！ → 返回 true              │
│   • 驗證：abs(1-0) = 1 ≤ k=2 ✓                │
├──────────────────────────────────────────────┤
│ 時間複雜度：O(n) = O(2)
│ 空間複雜度：O(min(n,k)) = O(min(2,2)) = O(1)  │
└──────────────────────────────────────────────┘
```

```
進階例：nums = [1,2,3,1,2,3], k = 2

i=0: 窗口[0,0] = {1}
i=1: 窗口[0,1] = {1,2}
i=2: 窗口[0,2] = {1,2,3}
     i ≥ k，移除 nums[0]=1，窗口[1,2] = {2,3}
i=3: 窗口[1,3] = {2,3,1}
     i ≥ k，移除 nums[1]=2，窗口[2,3] = {3,1}
i=4: nums[4]=2，查詢(2) → 不存在
     插入(2)，窗口[3,4] = {1,2,3}
     移除 nums[2]=3，窗口[3,4] = {1,2}
i=5: nums[5]=3，查詢(3) → 不存在 ✗
     (因為 nums[2]=3 已被移除，不在窗口內)
     返回 false
```

---

**文件生成日期**: 2026-03-29
**面向**: LeetCode 面試準備 + Kernel 系統知識融合
**難度等級**: 中等 (Medium) 兼具進階思維訓練
