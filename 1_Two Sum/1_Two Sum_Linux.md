# LeetCode 1. Two Sum - Linux 核心實作對照

## 問題概述

給定一個整數陣列 `nums` 和一個整數 `target`，找出陣列中兩個數字，其和等於 `target`。時間複雜度目標 O(n)，採用 hash table 實現。

LeetCode C 解法採用**開放定址法 (open addressing)** 搭配**線性探測 (linear probing)**，而 Linux 核心則使用多種更複雜的雜湊表設計。

---

## 1. Linux 核心原始碼位置

### include/linux/hashtable.h
- **作用**: 核心提供的通用鏈式雜湊表巨集 (chained hash table macros)
- **特點**: 型別無關的泛型設計，基於 `hlist_head` 和 `hlist_node`
- **使用場景**: 核心內常見的動態鍵值集合管理 (如 PID 查詢、inode 查詢)
- **核心巨集**: `hash_for_each()`, `hash_add()`, `hash_del()`

### include/linux/hash.h
- **作用**: 核心層級的雜湊函數定義 (hash_long, hash_32, hash_64)
- **特點**: 採用**乘法雜湊 (multiplicative hashing)** 搭配黃金比例
- **公式**: `hash_long(val, bits) = (val * GOLDEN_RATIO_PRIME) >> (64 - bits)`
- **優勢**: 分佈均勻，避免線性相關輸入的叢集效應

### lib/rhashtable.c
- **作用**: 可變大小雜湊表實現 (resizable hash table)
- **特點**: 支援動態擴展與縮小，線上重新雜湊無阻塞
- **機制**: 增量遷移 (incremental migration)，哈希表大小為 2^n
- **適用**: 需要無界限數據結構的場景

### include/linux/jhash.h
- **作用**: Jenkins 雜湊函數族
- **特點**: 適合字節序列 (如字串、网络报文)，提供 `jhash()`, `jhash_1word()`, `jhash_2words()` 等
- **用途**: 網路協議、檔案系統等需要字節級雜湊

### include/linux/rhashtable.h
- **作用**: 可變雜湊表的標頭檔與公開 API
- **提供**: `rhashtable_init()`, `rhashtable_insert_fast()`, `rhashtable_lookup_fast()`
- **設計**: 無鎖讀取 (RCU-friendly)，支援並行修改

---

## 2. 核心實作 vs. LeetCode 實作對照

### 2.1 雜湊函數

| 維度 | Linux 核心 | LeetCode 解法 |
|------|-----------|-------------|
| **函數** | 乘法雜湊 (multiplicative) | 簡單模運算 |
| **公式** | `(val * GOLDEN_RATIO) >> shift` | `hash = key % table_size` |
| **常數** | GOLDEN_RATIO_PRIME = `11400714819323198549UL` | 無特殊常數 |
| **特點** | 理論上均勻分佈，防止線性相關輸入 | 簡單直觀，但易受 key 結構影響 |
| **缺點** | 計算成本較高 (64 位乘法) | 如果 table_size 不互質，易產生叢集 |

**核心原理**: 黃金比例 (φ = 1.618...) 的倒數乘以 2^64，再位移到所需位數。這確保了任何整數鍵都能均勻分佈在雜湊表。

### 2.2 碰撞處理

| 維度 | Linux 核心 (hashtable.h) | Linux 核心 (rhashtable.c) | LeetCode 解法 |
|------|-------------------------|----------------------|-------------|
| **方法** | 鏈結 (chaining) | 漸進式重新雜湊 | 開放定址 (linear probing) |
| **資料結構** | `hlist_head` + `hlist_node` | 二級表 + 中間表 | 單一陣列 + 狀態標記 |
| **效率 (hit)** | O(1) 平均 | O(1) 平均 | O(1) 平均 (最優情況) |
| **效率 (miss)** | O(1 + α) α=負載因子 | O(1) 保證 | O(probe_count) 線性探測 |
| **快取局部性** | 差 (指標追蹤) | 中 (二級結構) | 優 (連續記憶體) |
| **記憶體開銷** | 高 (節點 + 指標) | 中 (動態擴展) | 低 (預分配陣列) |

**權衡考量**:
- **鏈結法**: 空間開銷大，快取效率低，但簡單穩健，易擴展
- **線性探測**: 快取友好，記憶體連續，但易形成一級叢集，刪除困難
- **重新雜湊**: 複雜度高，但提供動態調整和無鎖並行性

### 2.3 表大小與負載因子

| 維度 | Linux 核心 | LeetCode 解法 |
|------|-----------|-------------|
| **初始大小** | 可設定 (通常 16) | 2n + 1 (n = nums 陣列長度) |
| **調整策略** | 自動擴展至 2^n | 固定大小，無動態調整 |
| **負載因子** | 0.75 左右時觸發擴展 | 隱含受限於 2n + 1 (最多 50% 負載) |
| **衝擊** | 分攤 O(1) 插入 | 預分配足夠空間，無擴展開銷 |

**設計理由**: LeetCode 已知輸入大小 (n)，故可預分配 2n+1 的質數表，無需動態調整。核心無法預知負載，故採用漸進式擴展策略。

### 2.4 API 設計

**Linux 核心** (型別無關巨集):
```c
DEFINE_HASHTABLE(table, 10);  // 2^10 = 1024 槽位
hash_add(table, &node->hlist, key);
hash_for_each_possible(table, obj, hlist, key) { /* 掃描碰撞鏈 */ }
```

**LeetCode 解法** (型別特定結構):
```c
typedef struct {
    int key;
    int value;
    int occupied;  // 0: empty, 1: occupied, -1: deleted
} HashEntry;

int hash_index = key % HASH_TABLE_SIZE;
while (hash_table[hash_index].occupied != 0) {
    hash_index = (hash_index + 1) % HASH_TABLE_SIZE;
}
```

### 2.5 記憶體配置

| 維度 | Linux 核心 | LeetCode 解法 |
|------|-----------|-------------|
| **分配器** | slab 分配器 (kmalloc) | 堆配置 (calloc) |
| **優勢** | 碎片化少，快速重用，L1 快取親和 | 簡單可靠，適合使用者空間 |
| **追蹤** | 內核記憶體計帳 (memcg) | 無特殊追蹤 |

---

## 3. 主管 Code Review 角度考題

### Q1: 為什麼選擇線性探測而非鏈結法？

**答案框架**:
- **快取局部性**: 開放定址保持連續記憶體，一次 CPU 快取行載入可容納多個條目，碰撞查詢無指標追蹤。
- **記憶體效率**: 不需額外的節點指標，空間開銷 = 1.2n (4 bytes/entry) vs. 鏈結法 2.5n (entry + 2 指標)。
- **LeetCode 場景**: 輸入大小已知且有界 (n ≤ 10^4)，預分配 2n+1 足夠，無需動態擴展。
- **權衡**: 線性探測在小表、低負載情況下優於鏈結法，但若負載過高易形成一級叢集。

**核心知識結合**: Linux 核心在頻繁插刪且規模不確定的場景 (進程表、檔案描述符) 採用鏈結法，原因是負載因子變化大，線性探測會退化。

### Q2: 負載因子為何設定為 2n+1？

**答案框架**:
- **上界保證**: 若 nums 有 n 個元素，最多需要存儲 n 對 (key, value)，故容量 2n+1 足夠。
- **質數設計**: 2n+1 通常是質數 (如 n=3 時，7 是質數)，確保線性探測時能遍歷所有槽位 (gcd(步長, 表大小) = 1)。
- **負載因子** ≈ n / (2n+1) ≈ 50%，此時線性探測性能仍可接受 (平均探測長度 ≈ 1.5)。
- **記憶體權衡**: 過小會碰撞頻繁，過大浪費空間；2n+1 是合理折中。

**核心知識結合**: Linux rhashtable 動態調整，負載因子 > 0.75 時按 2^n 擴展。LeetCode 靜態表不需此邏輯。

### Q3: 核心如何處理雜湊碰撞，為何與我們不同？

**答案框架**:
- **核心鏈結法**: 多個 key 雜湊到同一槽位時，串成單向鏈 (hlist)，遍歷碰撞鏈以查詢。
  - 優勢: 碰撞後無需移動其他條目，刪除簡單 O(1)。
  - 劣勢: 指標追蹤、快取不友好。

- **我們的線性探測**: 碰撞時探測下一槽位，直至找到空位或目標鍵。
  - 優勢: 快取連續、記憶體緊湊。
  - 劣勢: 一級叢集、刪除困難 (需標記墓碑)、負載受限。

- **核心的重新雜湊** (rhashtable): 當負載過高時，整表重新雜湊至更大規模，分散碰撞。
  - 優勢: 避免碰撞鏈變長，保持 O(1) 平均查詢。
  - 成本: 遍歷全表，分攤攤銷。

**差異根源**: 核心需支援無界動態集合，故採用可擴展的鏈結 + 重新雜湊；LeetCode 已知邊界，靜態開放定址足夠。

### Q4: 病態輸入會發生什麼？(Pathological Inputs)

**答案框架**:

**場景 1: 所有 key 雜湊到同一槽位**
```c
// 假設 hash(x) = x % TABLE_SIZE，輸入全是 TABLE_SIZE 的倍數
int nums[] = {0, TABLE_SIZE, 2*TABLE_SIZE, ...};
```

- **線性探測**: 探測鏈形成，平均查詢時間 O(n)，退化為線性表。
- **核心鏈結法**: 單一碰撞鏈長度 O(n)，查詢仍 O(n)。
- **核心重新雜湊**: 偵測負載因子過高，觸發擴展到 2^(k+1)，打破雜湊碰撞。

**防禦策略**:
1. 使用高質量雜湊函數 (如核心的黃金比例乘法雜湊)。
2. 隨機化雜湊種子 (kernel 中的 `hash_seed`)，防止 DoS。
3. 動態擴展表大小 (如核心 rhashtable)。
4. LeetCode 場景: 輸入由題目保證，故無此顧慮。

### Q5: 核心為何採用黃金比例乘法雜湊？

**答案框架**:

**理論基礎** (Knuth):
- 乘法雜湊: `h(k) = ⌊ A · k mod 2^w ⌋ >> (w - m)`，其中 A = ⌊2^w / φ⌋。
- φ = (1 + √5) / 2 ≈ 1.618 (黃金比例)，其倒數 1/φ ≈ 0.618 無理數。
- 無理數特性: 對任意輸入 k，`k/φ` 的小數部分均勻分佈在 [0, 1)。

**優勢**:
1. **線性無關性**: 即使輸入呈算術級數 (如 0, s, 2s, 3s, ...)，雜湊值仍分散。
   - 對比簡單模運算: `hash(k) = k % m`，若輸入步長 s 與 m 有公因子，則叢集。
2. **二進位級別均勻**: 結合位移操作，每一位都均勻。
3. **高效實現**: `(k * GOLDEN_RATIO) >> shift` 用單次乘法與位移，快速。

**核心應用**:
```c
#define GOLDEN_RATIO_PRIME UINT64_C(11400714819323198549)
static inline u64 hash_64(u64 val, unsigned int bits)
{
    return (val * GOLDEN_RATIO_PRIME) >> (64 - bits);
}
```

**LeetCode 的簡化**: 輸入範圍小 (n < 10^5)，質數取模足夠；核心面臨海量異質輸入，故投資更強雜湊。

---

## 4. 面試加分總結

### 4.1 技術亮點

1. **認識多種雜湊表設計**:
   - 鏈結 (chaining): 簡單穩健，kernel 常用。
   - 開放定址 (open addressing): 快取友好，快速查詢。
   - 動態重新雜湊 (resizing): 自適應負載，無界集合。

2. **雜湊函數的數學基礎**:
   - 除法法: `h(k) = k mod m` (簡單，但易叢集)。
   - 乘法法: `h(k) = ⌊ A·k mod 2^w ⌋` (均勻，kernel 標準)。
   - 乘積法: 多個鍵合併雜湊。

3. **負載因子與性能**:
   - α = n/m (已儲存條目數 / 槽位數)。
   - 鏈結法: α 可 > 1，但查詢 O(1 + α)。
   - 開放定址: α < 1 必須，查詢 O(1 / (1-α))。
   - 核心 rhashtable: α ≈ 0.75 時擴展。

4. **核心技術應用**:
   - Linux kernel 用雜湊表優化 PID、inode、socket 查詢。
   - RCU (Read-Copy-Update) 使核心雜湊表支援無鎖讀。
   - Incremental resizing 避免一次性停頓。

### 4.2 面試回答範本

**Q: LeetCode Two Sum 用什麼資料結構？**

A: 使用**雜湊表 (hash table)** 實現 O(n) 時間複雜度。具體採用**開放定址法搭配線性探測**，初始化表大小為 2n+1。

優勢:
- 快取局部性優於鏈結法 (連續記憶體)。
- 記憶體開銷小 (無指標)。
- 預知輸入大小，無需動態擴展。

**Q: 為何 Linux 核心用鏈結法而非線性探測？**

A: 核心需支援無界、動態的資料集合，鏈結法的優勢是:
- 刪除簡單 (移除節點即可)。
- 負載因子可超過 1，無性能退化。
- 支援漸進式重新雜湊 (rhashtable)，無停頓擴展。

而線性探測在大負載下易形成一級叢集，查詢退化為 O(n)。

**Q: 黃金比例雜湊函數有何優勢？**

A: 核心採用 `h(k) = (k * GOLDEN_RATIO) >> shift` 而非簡單模運算，因為:
- 黃金比例的倒數是無理數，保證任意輸入都均勻分佈。
- 防止算術級數輸入的叢集 (如 0, s, 2s, ... 若 s 與表大小共因子)。
- 只需一次乘法與位移，計算高效。

LeetCode 場景輸入小、已驗證，簡單模運算足夠。但了解高質量雜湊設計是進階知識。

---

## 5. Sources

### Linux 核心原始碼

- [include/linux/hashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h) — 核心通用鏈式雜湊表巨集
- [include/linux/hash.h](https://github.com/torvalds/linux/blob/master/include/linux/hash.h) — 乘法雜湊函數 (hash_long, hash_32, hash_64)
- [lib/rhashtable.c](https://github.com/torvalds/linux/blob/master/lib/rhashtable.c) — 可變大小雜湊表實現
- [include/linux/rhashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/rhashtable.h) — rhashtable API 標頭檔
- [include/linux/jhash.h](https://github.com/torvalds/linux/blob/master/include/linux/jhash.h) — Jenkins 雜湊函數
- [include/linux/hlist.h](https://github.com/torvalds/linux/blob/master/include/linux/hlist.h) — 雙指標链表實現 (用於碰撞鏈)

### 理論參考

- Donald Knuth, "The Art of Computer Programming, Vol. 3: Sorting and Searching", Section 6.4 (Hashing)
- Thomas H. Cormen et al., "Introduction to Algorithms" (CLRS), Chapter 11 (Hash Tables)

---

## 附錄: 程式碼對照

### LeetCode Two Sum (線性探測)
```c
#define HASH_TABLE_SIZE (2 * numsSize + 1)

typedef struct {
    int key;
    int value;
    int occupied;  // 0: empty, 1: occupied, -1: deleted
} HashEntry;

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    HashEntry* table = (HashEntry*)calloc(HASH_TABLE_SIZE, sizeof(HashEntry));
    int* result = (int*)malloc(2 * sizeof(int));

    for (int i = 0; i < numsSize; i++) {
        int complement = target - nums[i];
        int hash_index = complement % HASH_TABLE_SIZE;
        if (hash_index < 0) hash_index += HASH_TABLE_SIZE;

        // Linear probing to find complement
        while (table[hash_index].occupied == 1) {
            if (table[hash_index].key == complement) {
                result[0] = table[hash_index].value;
                result[1] = i;
                *returnSize = 2;
                free(table);
                return result;
            }
            hash_index = (hash_index + 1) % HASH_TABLE_SIZE;
        }

        // Insert current number
        hash_index = nums[i] % HASH_TABLE_SIZE;
        if (hash_index < 0) hash_index += HASH_TABLE_SIZE;
        while (table[hash_index].occupied == 1) {
            hash_index = (hash_index + 1) % HASH_TABLE_SIZE;
        }
        table[hash_index].key = nums[i];
        table[hash_index].value = i;
        table[hash_index].occupied = 1;
    }

    free(table);
    *returnSize = 0;
    return result;
}
```

### Linux 核心風格 (鏈式雜湊表)
```c
#include <linux/hashtable.h>

#define HASH_BITS 10
DEFINE_HASHTABLE(two_sum_table, HASH_BITS);

struct entry {
    int key;
    int value;
    struct hlist_node node;
};

// Lookup
struct entry *e;
hash_for_each_possible(two_sum_table, e, node, complement) {
    if (e->key == complement) {
        // Found pair
        return;
    }
}

// Insert
e = kmalloc(sizeof(*e), GFP_KERNEL);
e->key = nums[i];
e->value = i;
hash_add(two_sum_table, &e->node, nums[i]);
```

**關鍵差異**:
- **記憶體**: LeetCode 預分配，核心動態配置每個節點。
- **碰撞**: LeetCode 線性探測推進指標，核心遍歷鏈。
- **刪除**: LeetCode 標記墓碑，核心直接 `hlist_del()`。
