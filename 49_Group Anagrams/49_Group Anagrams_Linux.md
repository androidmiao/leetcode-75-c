# LeetCode 49 Group Anagrams - Linux 核心實作對照

## 1. Linux 相關原始碼位置

### 哈希表相關
- **`include/linux/hashtable.h`**
  核心的動態雜湊表巨集定義。提供 `DEFINE_HASHTABLE()`、`hash_add()`、`hash_for_each()` 等通用介面。使用開放定址（open addressing）搭配鍊式結構（hlist）以處理碰撞。

- **`include/linux/hlist.h`**
  雙向鍊表（list）與單向雜湊鍊表（hlist）定義。hlist 針對雜湊表中的碰撞鍊進行最佳化，節省記憶體（單向指標）。支援 O(1) 插入、刪除、走訪。

- **`lib/hash.c` / `lib/hashmod.c`**
  哈希函數實作。包括 `full_name_hash()`（字串雜湊）與模運算。字串雜湊採用滾動雜湊技巧以加速。

### 排序相關
- **`lib/sort.c`**
  核心堆排序（heapsort）實作，對應 `qsort()` 標準 C 庫函數。採用底層堆排序演算法，時間複雜度 O(n log n)，空間複雜度 O(1)（原地排序）。不依賴棧遞迴，適合嵌入式環境。

- **`include/linux/list.h`**
  通用鍊表巨集與操作（`list_add()`、`list_for_each()` 等）。常與雜湊表搭配使用以構建複雜的資料結構。

- **`include/linux/sort.h`**
  排序宣告，提供核心內部排序介面。

### 記憶體管理相關
- **`mm/page_alloc.c`**
  頁面分配器。LeetCode 使用 `malloc()` 動態配置；核心在虛擬記憶體層級進行更精細的管理。

- **`include/linux/slab.h`**
  核心物件快取（slab allocator）。相比 `malloc()` 的通用性，slab 為特定結構體大小預先分配以減少碎片。

---

## 2. Kernel vs. LeetCode 實作對照

### 演算法層面

| 面向 | LeetCode 解法 | Linux 核心 |
|------|--------------|----------|
| **查找方式** | 排序-掃描（Sorting + Linear Scan） | 雜湊表查找（Hash Table Lookup） |
| **時間複雜度** | O(NK log K) — 其中 N=字串數、K=最長字串長度 | O(N) — 平均情況下（雜湊表碰撞率低） |
| **空間複雜度** | O(NK) — 儲存排序副本 + 配對結構 | O(N) — 僅儲存元素指標，不複製字串 |
| **分組機制** | 排序使相同異位詞相鄰，一次掃描分組 | 使用同一雜湊值的所有項放在同一鍊上（hlist），走訪雜湊桶即可得到一組 |

### 資料結構層面

#### LeetCode 方案
```c
/* 配對陣列：(sorted_key, original_string) */
char*** pairs = (char***)malloc(strsSize * sizeof(char**));
for (int i = 0; i < strsSize; i++) {
    pairs[i] = (char**)malloc(2 * sizeof(char*));
    pairs[i][0] = sorted_strs[i];   /* 排序後字串作為分組鍵 */
    pairs[i][1] = original_strs[i];  /* 原始字串儲存 */
}
qsort(pairs, strsSize, sizeof(char**), compare_pairs);
```

**特點：**
- 明確的結構體設計（對偶）
- 外部排序後隱含分組
- 無顯式雜湊表資料結構

#### Linux 核心方案
```c
/* 核心的雜湊表定義（簡化版） */
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)]

struct hlist_node {
    struct hlist_node *next, **pprev;  /* 單向指標節點 */
};

struct anagram_group {
    char* sorted_key;
    struct hlist_head strings;  /* 鍊頭，串接所有同異位詞 */
    struct hlist_node node;
};
```

**特點：**
- 隱式分組（鍊結構本身定義邊界）
- 動態雜湊桶避免預先固定大小
- hlist 的單向設計節省記憶體（相比雙向鍊表）

### 碰撞處理

| 面向 | LeetCode | Linux 核心 |
|------|---------|-----------|
| **衝突類型** | 無直接碰撞概念（排序確保唯一性） | 雜湊碰撞（多個項對應同一雜湊值） |
| **解決策略** | 排序使關鍵字有序；線性掃描識別邊界 | 鍊式尋址（chaining via hlist）；雜湊桶串聯碰撞元素 |
| **查詢複雜度** | O(K log K)（排序每個字串） + O(N)（掃描） | O(1) 平均（直接索引 + 短鍊走訪） |

### API 設計比較

#### LeetCode：命令式、顯式步驟
```c
/* 步驟 1：排序每個字串 */
for (int i = 0; i < strsSize; i++) {
    qsort(sorted_strs[i], strlen(strs[i]), sizeof(char), compare_char);
}

/* 步驟 2：排序配對 */
qsort(pairs, strsSize, sizeof(char**), compare_pairs);

/* 步驟 3：線性掃描分組 */
for (int i = 0; i < strsSize; i++) {
    if (current_sorted == NULL || strcmp(sorted, current_sorted) != 0) {
        /* 新群組 */
    }
}
```

#### Linux 核心：宣告式、巨集驅動
```c
/* 定義雜湊表（一行宣告） */
#define HASH_BITS 4
DEFINE_HASHTABLE(groups, HASH_BITS);

/* 查找/插入雜湊表（巨集操作） */
hash_add(groups, &node->node, hash_of_sorted_key);

/* 走訪同群組（巨集自動管理鍊） */
hash_for_each_possible(groups, entry, node, hash_value) {
    if (is_same_group(entry, current)) {
        /* 處理同群組 */
    }
}
```

**設計哲學差異：**
- **LeetCode：** 資料結構顯式化，利於教學與調試
- **核心：** 資料結構隱藏於巨集，利於性能與代碼簡潔

---

## 3. 主管 Code Review 角度考題

### Q1: 為何選擇排序而非直接使用雜湊表？

**預期答題方向：**
- C 語言無內建 HashMap；標準庫僅有 `qsort()`
- 排序法避免自實作雜湊函數與碰撞處理邏輯
- 時間複雜度 O(NK log K) 在字串較短時（K ≤ 26，英文字母）實用
- 空間複雜度線性，無額外雜湊表開銷

**追問：**
1. 若輸入字串極長（K > 1000），您會改用雜湊表嗎？為什麼？
2. 雜湊函數如何選擇？Linux 核心的 `full_name_hash()` 相比簡單模運算有何優勢？

**測試點：**
- 演算法選擇的權衡能力（時空複雜度、工程複雜度）
- 對標準庫侷限的認知

---

### Q2: 記憶體配置的潛在問題

**預期答題方向：**
```c
/* 現有代碼的問題 */
sorted_strs = (char**)malloc(strsSize * sizeof(char*));  /* 單層配置 */
for (int i = 0; i < strsSize; i++) {
    sorted_strs[i] = (char*)malloc((str_len + 1) * sizeof(char));  /* 多層配置 */
}
```

**問題清單：**
1. **片段化（Fragmentation）：** 多層小配置導致堆碎片
2. **錯誤處理缺失：** `malloc()` 失敗未檢查
3. **記憶體洩漏風險：** 若中途出錯，已配置記憶體未必釋放

**改進方向：**
- 預分配單一大塊記憶體，內部手動索引
- 或使用 Linux 核心的 slab 分配器概念，為固定大小（如 26 字元）預分配
- 加入 `if (!ptr) return error;` 檢查

**Linux 對標：**
```c
/* 核心的做法：預分配 slab 池 */
struct kmem_cache *anagram_cache = kmem_cache_create(
    "anagrams", sizeof(struct anagram_entry), 0, 0, NULL);
entry = kmem_cache_alloc(anagram_cache, GFP_KERNEL);
```

**測試點：**
- 堆記憶體管理經驗
- 大規模應用中的健壯性考量

---

### Q3: 排序穩定性與結果一致性

**預期答題方向：**

在 `compare_pairs()` 中：
```c
int cmp = strcmp(pair_a[0], pair_b[0]);
if (cmp != 0) return cmp;
return strcmp(pair_a[1], pair_b[1]);  /* 次要鍵 */
```

**問題：**
1. 排序穩定性對此問題影響如何？
2. 次要鍵（原始字串）的必要性？

**答題要點：**
- `qsort()` **不保證穩定性**（實作為不穩定排序，通常是堆排序）
- LeetCode 題目允許任意順序；故在同異位詞組內，順序非必需
- 次要鍵用於**確定性測試**（確保輸出可復現）
- Linux 核心中，`lib/sort.c` 的堆排序亦不穩定；若需穩定排序須用合併排序

**核心類比：**
```c
/* Linux 在 fs/dcache.c 中使用穩定排序保持字典序 */
/* 若需穩定排序，得用 mergesort（複雜度更高） */
```

**追問：**
1. 若要求結果按字典序嚴格排列怎辦？
2. 穩定排序與不穩定排序的時空開銷差異？

**測試點：**
- 排序演算法細節理解
- 演算法選擇與題目需求匹配度

---

### Q4: 雜湊函數設計 vs. 排序鍵設計

**預期答題方向：**

LeetCode 方案：
- 鍵 = 排序後的字串（如 "aet"）
- 完全一致性（所有異位詞排序後字符相同）
- 無碰撞風險

Linux 核心做法：
- 鍵 = `hash(sorted_key)`（整數雜湊值）
- 效率高（O(1) 查表），但存碰撞風險
- 需額外處理碰撞（鍊走訪）

**問題：**
1. 若字串長度達 10K，直接排序鍵性能如何？
2. 雜湊值碰撞如何與排序副鍵結合保證正確性？

**答題方向：**
```c
/* 核心級做法：雜湊 + 二次驗證 */
hash_value = full_name_hash(sorted_key, strlen(sorted_key));
hash_add(groups, &entry->node, hash_value);

/* 查找時 */
hash_for_each_possible(groups, entry, node, hash_value) {
    if (strcmp(entry->sorted_key, sorted_key) == 0) {  /* 二次驗證 */
        /* 找到真正的群組 */
    }
}
```

**測試點：**
- 雜湊表性能認知
- 碰撞處理機制
- 字串比較與雜湊的平衡

---

### Q5: 時間複雜度細節分析

**預期答題方向：**

宣稱 O(NK log K)，但細節檢視：
```
步驟 1：排序每個字串       → N × (K log K)     // qsort(char[K])
步驟 2：排序 N 配對        → N log N × O(比較) // qsort(pairs)
        每次配對比較       → O(K)              // strcmp()
        總計：            → O(NK log N)
步驟 3：線性掃描           → O(N)
```

**實際複雜度：** O(NK log K + NK log N)

**追問：**
1. 當 N 遠大於 K（如 10M 短字串）時，NK log N 主導；如何優化？
2. 能否用計數排序（radix sort）替代 qsort？

**改進方案：**
- 計數排序字符：O(NK)（字母表固定 ≤ 26）
- 避免 qsort() 的 O(log N) 係數

**Linux 對標：**
```c
/* 核心使用精細的複雜度分析決定演算法 */
/* 例如 fs/ext4 的索引節點排序選擇受輸入大小影響 */
```

**測試點：**
- 複雜度分析精確度
- 實際性能瓶頸識別能力
- 優化思路

---

### Q6: 快取局部性（Cache Locality）

**預期答題方向：**

LeetCode 方案：
```c
char*** pairs = malloc(strsSize * sizeof(char**));
/* pairs 是指標陣列，指向分散的字串記憶體 */
for (int i = 0; i < strsSize; i++) {
    pairs[i][0] = sorted_strs[i];    /* 指標跳躍 */
    pairs[i][1] = original_strs[i];
}
qsort(pairs, ...);  /* 排序時 CPU 訪問分散位置 */
```

**問題：**
1. `pairs` 陣列本身連續，但指向的字串可能不連續
2. 每次 `strcmp()` 都可能引發快取未命中
3. 實際性能可能劣於理論複雜度

**Linux 核心最佳實踐：**
```c
/* 核心應用的改進 */
struct anagram_entry {
    char sorted_key[256];      /* 嵌入字符，提升局部性 */
    char original_str[256];
    struct hlist_node node;
};

/* 配置一個連續的 struct 陣列，快取行對齐 */
struct anagram_entry *entries = kmem_cache_alloc(...);
```

**追問：**
1. 如何量測快取效應的實際影響？
2. 預對齐（alignment）如何幫助 CPU 預取？

**測試點：**
- 系統程式設計的低層考量
- 性能調優能力

---

### Q7: 並行性與鎖機制

**預期答題方向：**

LeetCode 單線程；但若多線程訪問：
```c
/* 問題：缺乏同步 */
qsort(pairs, strsSize, ...);  /* 非原子操作 */
```

**Linux 核心做法：**
```c
/* 保護雜湊表的讀寫 */
spinlock_t hash_lock = __SPIN_LOCK_UNLOCKED(hash_lock);

hash_add(groups, &entry->node, hash_val);  /* 需加鎖 */

hash_for_each_possible(groups, entry, ...) {  /* RCU 或讀鎖 */
    /* 走訪 */
}
```

**答題方向：**
- 若多線程，需 mutex 或 RWlock 保護整個排序流程
- 或分區雜湊表減少鎖競爭（如核心的 hash table per-bucket locking）
- LeetCode 無此需求，但生產代碼必考慮

**測試點：**
- 並行編程認知
- 核心同步原語理解

---

## 4. 面試加分總結

### 核心知識點

1. **雜湊表設計權衡**
   - 理解 LeetCode 選擇排序的實際原因（無內建 HashMap）
   - 知道 Linux 核心如何用 hlist + 巨集抽象化
   - 能比較動態雜湊表（rehashing）vs. 固定大小桶的利弊

2. **排序演算法在實務中的應用**
   - 知道 `qsort()` 背後是堆排序（O(n log n)，O(1) 空間）
   - 理解穩定性、複雜度分析的實際含義
   - 能判斷何時用計數排序 vs. 比較排序

3. **記憶體層階與性能**
   - 了解堆碎片化、快取行對齁的概念
   - 知道 slab 分配器如何減輕碎片
   - 能估計實際性能（複雜度只是理論值）

4. **標準庫 vs. 核心實作的區別**
   - `malloc()` 的通用性 vs. `kmem_cache` 的專用性
   - `qsort()` 的簡便性 vs. 自定排序的控制力
   - 何時信賴標準，何時自定實作

5. **大規模系統設計考量**
   - 錯誤處理（malloc 失敗）
   - 並行安全（鎖機制）
   - 記憶體上限與預分配策略

### 面試加分話術

**場景 1：解釋演算法選擇**
> "这题的核心是C语言没有HashMap，所以我选择排序+扫描。LeetCode上这样行得通，但在Linux核心中，他们用hlist+hash_add()宏，避免自己管理碰撞，性能更好。如果字符串特别长，我会切换到哈希表方案。"

**場景 2：談及記憶體優化**
> "我的方案多层malloc可能导致碎片。核心的做法是预分配一个大块内存或用slab分配器，这样CPU缓存友好，还能减少分配开销。如果是生产代码，我会检查malloc失败。"

**場景 3：涉及複雜度分析**
> "表面上是O(NK log K)，但实际包括了排序字符、排序对、以及字符串比较。当N远大于K时，NK log N 才是瓶颈。我可以用基数排序优化到O(NK)，或者直接用哈希表O(N)。"

**場景 4：系統設計延伸**
> "如果要多线程，整个排序流程需加锁。但核心的解决方案更精细——分区哈希表、RCU同步，避免全局锁竞争。这在高并发场景下性能差异巨大。"

---

## Sources

### Linux 核心原始碼
- [include/linux/hashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h) - 動態雜湊表巨集定義
- [include/linux/hlist.h](https://github.com/torvalds/linux/blob/master/include/linux/list.h) - 單向鍊式雜湊鍊表
- [lib/sort.c](https://github.com/torvalds/linux/blob/master/lib/sort.c) - 堆排序實作
- [lib/hash.c](https://github.com/torvalds/linux/blob/master/lib/hash.c) - 雜湊函數實作
- [include/linux/slab.h](https://github.com/torvalds/linux/blob/master/include/linux/slab.h) - slab 分配器

### 核心概念參考
- [Linux Kernel Newbies - Hash Tables](https://kernelnewbies.org/FAQ/data_structures) - 核心資料結構簡介
- [The Linux Programming Interface](https://man7.org/tlpi/) - 系統程式設計書籍
- C Standard Library `qsort()` 文檔：man qsort

### LeetCode 相關
- [LeetCode 49 - Group Anagrams](https://leetcode.com/problems/group-anagrams/)
- 本仓库實作：Group Anagrams.c

---

**文件撰寫日期：** 2026 年 3 月
**適用版本：** Linux 6.0+ （hashtable.h API 穩定）
**涵蓋語言：** C（標準庫 + 核心子集）
