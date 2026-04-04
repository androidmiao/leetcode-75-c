# Hash Table 核心雜湊表

## 核心概念

Linux kernel 實現了多種雜湊表數據結構，從簡單的鏈接法到複雜的可調整大小的無鎖雜湊表。核心文件位於 `include/linux/hashtable.h`, `include/linux/hash.h`, `include/linux/jhash.h` 和 `lib/rhashtable.c`。

## 基礎：hlist 與 list_head

### hlist（單向雜湊列表）

```c
struct hlist_head {
  struct hlist_node *first;  // 僅一個指標，節省空間
};

struct hlist_node {
  struct hlist_node *next;
  struct hlist_node **pprev;  // 指向上一節點的 next 指標
};
```

與 `list_head` 相比：

| 特性 | list_head | hlist |
|------|-----------|-------|
| 指標數（空表） | 2（next + prev） | 1（first） |
| 記憶體（1M 節點） | ~16 MB | ~8 MB |
| 環形 | 是 | 否 |
| 刪除成本 | O(1) | O(1) |
| 遍歷簡潔性 | 更簡潔 | 稍複雜 |

**為何選擇 hlist？**
雜湊表中，每個 bucket 只需單向鏈表。hlist 節省 50% 的指標空間，在大型雜湊表中意義重大。

### pprev 的巧妙之處

```c
// 刪除操作
static inline void hlist_del(struct hlist_node *n)
{
  struct hlist_node *next = n->next;
  struct hlist_node **pprev = n->pprev;
  *pprev = next;
  if (next)
    next->pprev = pprev;
}
```

**優勢**
- 無需遍歷找前驅，O(1) 刪除
- 不需要雙向指標
- pprev 指向前節點的 next 或頭節點的 first

```
頭: hlist_head.first ----> node1.next ----> node2.next ----> NULL
           ^                  ^                  ^
        pprev[0]          pprev[1]          pprev[2]
```

## 簡單雜湊表：hashtable.h

### 基礎 API

```c
#define DECLARE_HASHTABLE(name, bits) \
  struct hlist_head name[1 << (bits)]

#define hash_init(hashtable) \
  memset(&hashtable, 0, sizeof(hashtable))

#define hash_add(hashtable, node, key) \
  hlist_add_head(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])

#define hash_for_each_safe(name, bkt, tmp, obj, member) \
  for ((bkt) = 0, obj = NULL; obj == NULL && (bkt) < HASH_SIZE(name); (bkt)++) \
    hlist_for_each_entry_safe(obj, tmp, &name[bkt], member)
```

### 示例：inode 快取

```c
struct inode {
  unsigned long i_ino;
  struct hlist_node i_hash;  // 嵌入雜湊節點
  ...
};

// 定義 1024 個 bucket 的雜湊表
DECLARE_HASHTABLE(inode_hashtable, 10);

// 插入
struct inode *inode = ...;
hash_add(inode_hashtable, &inode->i_hash, inode->i_ino);

// 查找
struct inode *found = NULL;
hash_for_each_possible(inode_hashtable, found, i_hash, i_ino) {
  if (found->i_ino == target_ino)
    break;
}
```

## 複雜設計：rhashtable（調整大小的雜湊表）

### 問題陳述

簡單雜湊表需在初始化時決定大小。太小會碰撞多，太大浪費記憶體。

### rhashtable 解決方案

```c
struct rhashtable {
  struct rhashtable_bucket *buckets;
  unsigned int size;  // 當前大小
  unsigned int max_elems;
  struct rhashtable_params params;
};

struct rhashtable_params {
  size_t key_offset;
  size_t key_len;
  u32 (*hashfn)(const void *data, u32 len, u32 seed);
  struct rhashtable_comparison_func *obj_cmpfn;
  unsigned int max_shift;
  unsigned int min_shift;
};
```

### 核心特性

1. **非阻塞生長（Non-blocking Growth）**
   - 在保持所有讀取持續工作的情況下進行調整大小
   - 使用 RCU（Read-Copy-Update）保護

2. **獨立 bucket 鎖（Bucket Locking）**
   - 每個 bucket 有自己的鎖，減少爭用

3. **二級雜湊（Secondary Hash）**
   - 對於碰撞的元素，使用次級雜湊函數

```c
rhashtable_init(&my_table, &params);

// 插入
struct rhash_head *head = ...;
rhashtable_insert_fast(&my_table, head, my_params);

// 查找
head = rhashtable_lookup_fast(&my_table, key, my_params);

// 刪除
rhashtable_remove_fast(&my_table, head, my_params);

// 自動調整大小
rhashtable_walk_enter(&my_table, &iter);
```

## 哈希函數：jhash（Jenkins Hash）

### GOLDEN_RATIO 乘法雜湊

```c
#define GOLDEN_RATIO_32 0x61C88647
#define GOLDEN_RATIO_64 0x61C8864680B583EBul

static inline u32 hash_32(u32 val, unsigned int bits)
{
  return (val * GOLDEN_RATIO_32) >> (32 - bits);
}

static inline u32 hash_64(u64 val, unsigned int bits)
{
  return (val * GOLDEN_RATIO_64) >> (64 - bits);
}
```

**為什麼選擇 GOLDEN_RATIO？**
- 黃金比 φ ≈ 1.618 的二進制倒數
- 在整數域提供優良的離散特性
- 避免聚簇（clustering）
- 計算成本低（單一乘法和移位）

### Jenkins Hash 函數

```c
u32 jenkinsohash(const void *key, u32 length, u32 initval)
{
  u32 a, b, c, len = length;
  const u8 *k = key;

  a = b = c = JHASH_INITVAL + len + initval;

  while (len > 0) {
    switch (len) {
    case 3:
      c += k[2] << 16;
    case 2:
      b += k[1] << 8;
    case 1:
      a += k[0];
      __jhash_final(a, b, c);
      return c;
    ...
    }
  }
  return c;
}
```

**特性**
- 雪崩特性：輸入微小變化導致完全不同的輸出
- 均勻分佈：輸出在所有 hash 值上均勻分佈
- 快速：byte-by-byte 處理，無分支預測困難

## 衝突處理

### 鏈接法（Chaining）

```c
// bucket 連結衝突元素
bucket[0] --> elem1 --> elem2 --> elem3 --> NULL
bucket[1] --> NULL
bucket[2] --> elem4 --> NULL
```

**優點**
- 簡單實現
- 支援動態調整大小
- Kernel 中大量採用

**缺點**
- 差的快取局部性
- 衝突時效能下降

### 開放定址（Open Addressing）

Kernel 中較少使用，但在某些場景（如 eBPF）中出現。

```c
// 元素直接存儲在 bucket 中
bucket[h(key)] 或
bucket[(h(key) + 1) % size] 或
bucket[(h(key) + 2) % size] ...
```

## 與 LeetCode 問題的對應

### 相關問題

- **1 兩數之和**：最簡單的雜湊表應用，rhashtable 提供生產級別實現
- **49 字母異位詞分組**：字符計數雜湊，key_len 和 hashfn 定製
- **217 存在重複元素**：簡單成員資格測試，hash_add 和 hash_for_each_possible
- **219 存在重複元素 II**：滑動窗口 + 雜湊表，rhashtable 支援實時調整大小
- **242 有效的字母異位詞**：字符頻率比較，定製 hashfn
- **347 前 K 個高頻元素**：頻率計數雜湊表，與小頂堆結合
- **76 最小覆蓋子字符串**：字符統計雜湊，滑動窗口
- **3 無重複字符的最長子字符串**：滑動窗口 + 雜湊表位置記錄

## 設計洞察

### 1. 為什麼選擇鏈接法？

**Kernel 視角**
- 動態元素數量
- 無法預先配置大小
- 支援在線調整大小

**替代方案的局限**
- 開放定址：元素數量變化時需完全重建
- 凱克亞特雜湊（Cuckoo Hash）：更複雜，不適合動態場景

### 2. hlist 與 list_head 的權衡

**hlist（雜湊表 bucket）**
```
優勢：節省記憶體（50% 指標減少）
劣勢：單向，操作 API 相對複雜
```

**list_head（通用鏈接）**
```
優勢：雙向，統一 API
劣勢：記憶體開銷大
```

Kernel 實現了兩者，按場景選擇。

### 3. RCU 與 rhashtable

RCU（Read-Copy-Update）允許無鎖讀：

```c
// 讀取路徑（無鎖）
rcu_read_lock();
struct elem *e = rhashtable_lookup_fast(...);
use(e);  // 安全，e 不會在 RCU 寬限期內被釋放
rcu_read_unlock();

// 寫入路徑（帶鎖）
spin_lock(&bucket_lock);
rhashtable_remove_fast(...);
spin_unlock(&bucket_lock);
synchronize_rcu();  // 等待所有讀者完成
free(e);
```

這在高度競爭的場景（如文件系統 inode 快取）中至關重要。

## 時間複雜度

| 操作 | 平均情況 | 最壞情況 | 備註 |
|------|--------|---------|------|
| 插入 | O(1) | O(n) | 碰撞時鏈表遍歷 |
| 刪除 | O(1) | O(n) | 同上 |
| 查找 | O(1) | O(n) | 同上 |
| 調整大小 | O(n) | O(n) | rhashtable 非阻塞 |

## 面試考點

### 問題 1：為什麼 kernel 選擇 hlist 而非 list_head 作為雜湊表 bucket？

**預期答案**
- hlist 節省 50% 記憶體（1 個指標而非 2 個）
- 對於大型雜湊表（TB 級別），節省可觀
- 雜湊表 bucket 不需要雙向遍歷
- pprev 技巧支援 O(1) 刪除

### 問題 2：GOLDEN_RATIO 為何適合乘法雜湊？

**預期答案**
- 黃金比的倒數在二進制中有均勻分佈
- 避免對齐和批次元素聚簇
- 快速計算（乘法和移位）
- 在整數模 2^n 算術中表現優異

### 問題 3：rhashtable 如何在調整大小時保持無鎖讀？

**預期答案**
- 使用 RCU（Read-Copy-Update）
- 讀取不需要鎖，只有寫入爭用
- 新舊 bucket 同時存在短時間
- 二級雜湊或增量遷移
- synchronize_rcu() 等待寬限期後釋放舊結構

### 問題 4：開放定址在 kernel 中為何不常見？

**預期答案**
- 元素數量難以預測
- 刪除操作複雜（探針序列）
- 擴展時需完全重建
- RCU 友善性較差

## 參考資源

- **include/linux/hashtable.h**：簡單雜湊表 API
- **include/linux/hash.h**：GOLDEN_RATIO 乘法雜湊
- **include/linux/jhash.h**：Jenkins Hash 實現
- **lib/rhashtable.c**：調整大小的雜湊表實現
- **fs/namei.c**：dentry 快取應用（大型 rhashtable）
- **fs/inode.c**：inode 快取應用
- **net/netfilter/nf_conntrack_core.c**：連接追蹤雜湊表
