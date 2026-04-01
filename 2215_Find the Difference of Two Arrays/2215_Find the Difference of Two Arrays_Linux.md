---
problem: 2215
title: Find the Difference of Two Arrays
difficulty: Easy
link: https://leetcode.com/problems/find-the-difference-of-two-arrays/
---

# 2215. Find the Difference of Two Arrays — Linux Kernel 連接分析

## 概述

本題涉及「集合差集」與「去重」操作，在 Linux 核心中對應到兩個主要子系統：

1. **雜湊表 / 雜湊集合** — `include/linux/hashtable.h`、`lib/test_hash.c`
   用於高效的成員查詢（hash set）與開放定址碰撞解決
2. **點陣圖操作** — `include/linux/bitmap.h`、`lib/bitmap.c`
   固定值域場景下的位元映射（類似本題解法 Approach 1 的布林陣列）

---

## 一、Linux 相關原始碼位置

### 1.1 雜湊表實作

| 檔案路徑 | 角色 | 說明 |
|---------|------|------|
| `include/linux/hashtable.h` | 巨集頭檔 | 雜湊表容器的泛型巨集（open addressing，鏈式或開放定址） |
| `include/linux/hash.h` | 雜湊函式 | 核心提供的通用雜湊函式（`hash_ptr`, `hash_long`, `hash32`） |
| `lib/test_hash.c` | 測試套件 | 雜湊函式的單元測試與效能評估 |
| `lib/rbtree.c` | 紅黑樹實作 | 替代性的平衡資料結構（線性探測失敗時的備選） |

### 1.2 點陣圖操作

| 檔案路徑 | 角色 | 說明 |
|---------|------|------|
| `include/linux/bitmap.h` | 點陣圖巨集 | 位元級操作巨集（`set_bit`, `clear_bit`, `test_bit`） |
| `lib/bitmap.c` | 點陣圖實作 | 大型點陣圖的掃描、複製、邏輯運算函式 |
| `include/linux/bitops.h` | 位元運算 | CPU 相關的位元運算（如 `ffs`, `popcount`） |

### 1.3 相關測試與除錯檔案

| 檔案路徑 | 角色 | 說明 |
|---------|------|------|
| `lib/test_bitmap.c` | 點陣圖測試 | 點陣圖功能的測試案例 |
| `lib/test_rhashtable.c` | 可調整大小雜湊表 | 核心的可動態擴張雜湊表實作測試 |
| `include/linux/rhashtable.h` | 可調整雜湊表 | 生產級別的可調整大小雜湊表（核心網路堆疊使用） |

---

## 二、Kernel vs. LeetCode 實作對照

### 2.1 資料結構選擇對比

| 面向 | Kernel（`hashtable.h`） | Kernel（`bitmap.h`） | LeetCode Approach 1 | LeetCode Approach 2 |
|------|--------------------------|----------------------|-------------------|-------------------|
| **適用場景** | 通用值域、大規模集合 | 固定小範圍值域 | 已知值域 [-1000,1000] | 泛用、值域不固定 |
| **資料結構** | 鏈式雜湊表或開放定址 | 位元陣列 | `bool[2001]` | 自建 `IntHashSet` |
| **容量** | 動態調整（可調整表） | 靜態分配或動態（bitmap） | 固定 2001 個槽位 | 動態調整（2x+1）|
| **成員查詢** | O(1) 平均 | O(1) 常數（位元運算） | O(1) 常數（陣列索引） | O(1) 平均（雜湊） |
| **插入/去重** | 鏈式探測或重新散列 | `set_bit()` 或 `test_and_set_bit()` | 直接標記 boolean | 開放定址 + 線性探測 |
| **空間開銷** | 雜湊表大小 ≥ n (load factor) | 位元數 = 值域大小 / 8 | 2001 × sizeof(bool) ≈ 2 KB | 容量 × (sizeof(int) + sizeof(bool)) |

### 2.2 雜湊函式設計

**Kernel 設計（`include/linux/hash.h`）：**
```c
/* 通用的 avalanche 雜湊，用於指標與長整數 */
static __always_inline unsigned long hash_long(unsigned long val, unsigned int bits) {
    unsigned long hash = val;
    hash ^= hash >> 16;
    hash *= 0x21c3d86d8a7b9d3dUL;
    hash ^= hash >> 16;
    return hash >> (64 - bits);
}
```

**LeetCode Approach 2 設計（自建）：**
```c
static unsigned int hash_int(int x) {
    unsigned int h = (unsigned int)x;
    h ^= h >> 16;
    h *= 0x7feb352dU;      /* 質數乘數 */
    h ^= h >> 15;
    h *= 0x846ca68bU;      /* 另一個質數乘數 */
    h ^= h >> 16;
    return h;              /* 無位元遮罩，留給 set_contains 處理 */
}
```

**對比發現：**
- 兩者都採用 **avalanche 混合**（異或 + 乘法）來打散位元
- Kernel 版使用 `64 位元質數乘數`，LeetCode 版使用 `32 位元質數`
- Kernel 版在末尾做 `>> (64 - bits)`，LeetCode 版在 `set_contains` 中做 `hash_int(key) & mask`

### 2.3 碰撞解決策略

| 策略 | Kernel 常見做法 | LeetCode Approach 1 | LeetCode Approach 2 |
|-----|----------------|--------------------|-------------------|
| **鏈式** | `hashtable.h` 支援（搭配 `hlist_head` / `hlist_node`） | 不適用（固定陣列） | 不使用 |
| **開放定址** | `rhashtable.h` 使用（可調整表） | N/A | **使用** ✓ |
| **線性探測** | 部分實作（如 `lib/rhashtable.c` 中的快速路徑） | N/A | **使用** ✓ |
| **步長** | 1（線性探測）或偽隨機 | 固定陣列索引 | 1（線性探測） |
| **裝載因子** | 典型 0.75 左右；支援動態擴張 | 固定 ≈ 1（僅 2001 個值） | 初始 `next_pow2(n*2+1)`；無動態擴張 |

### 2.4 API 設計

**Kernel 風格（泛型巨集）：**
```c
/* hashtable.h 中的巨集，接受任意容器類型 */
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)];

#define hash_for_each(table, node, obj, member) \
    /* 迭代表中所有節點 */
```

**LeetCode 方案對比：**
- Approach 1：無抽象，直接使用 `bool[]` + 值映射
- Approach 2：具體 `typedef struct IntHashSet` + 專用函式
  (`set_init`, `set_insert`, `set_contains`, `set_free`)

**啟示：**
- 核心使用巨集實現泛型，避免代碼重複
- LeetCode 受到 C 語言限制，採取「具體實作一個版本」的方式

### 2.5 記憶體配置策略

| 方案 | 配置方式 | 初期開銷 | 擴張機制 | 碎片化風險 |
|-----|---------|---------|--------|----------|
| Kernel（鏈式） | `kmalloc` 動態分配雜湊表 + 節點 | 取決於元素數 | 鏈延伸（無擴張） | 低（插入式結構） |
| Kernel（可調整表） | 初始小表 → 動態擴張 | 初期小 | 當超過裝載因子時重新散列 | 低（自動重新整理） |
| Approach 1 | 棧上分配 `bool[2001]` | 固定 ≈ 2 KB | 無 | 無（固定大小） |
| Approach 2 | 堆分配 `int[]` + `bool[]` | `capacity` × 12 位元組 | 無（一次分配足夠） | 可能過度配置（`next_pow2`） |

### 2.6 通用性 vs. 特化

| 維度 | Kernel | LeetCode Approach 1 | LeetCode Approach 2 |
|-----|--------|-------------------|-------------------|
| **假設綁定** | 鏈式表：假設足夠記憶體，元素可變 | 假設值域在 [-1000,1000] | 假設值域任意，但固定時分配 |
| **防禦性** | `rhashtable.h` 檢查裝載因子、動態調整 | 無防禦（假設成立） | 簡單檢查（容量預先計算） |
| **效能特化** | CPU 快取友善（某些實作）、可鎖定操作 | 完全棧最適化，零堆分配 | 堆分配 + 線性探測（快取友善） |
| **使用者文檔** | 複雜，需理解雜湊表與鏈表 | 簡單，無 API 學習成本 | 中等，需理解 hash set 設計 |

---

## 三、主管 Code Review 角度考題

### 3.1 資料結構選擇

**Q1: 為什麼 Approach 1 使用布林陣列而不用真正的雜湊表？**

**預期答案方向：**
- 「值域固定在 [-1000,1000]，範圍小且已知」
- 「布林陣列成本 O(2001) = O(1)，常數小」
- 「避免雜湊函式、碰撞解決的開銷」
- 「在 C 中無內建雜湊表，自己實作成本較高」

**追問：** 「如果值域擴大到 [-10^9, 10^9]，你會怎麼改？」
- 測試對 **約束變化的適應性** 與 **權衡思維**
- 預期：應該立即轉向 Approach 2（真正的雜湊表）

---

**Q2: Approach 2 的開放定址為何選用線性探測而非二次探測或雙雜湊？**

**預期答案方向：**
- 「線性探測最簡單，程式碼少」
- 「在 C 中，簡潔實作往往更可靠」
- 「如果 load factor 控制好（< 0.75），線性探測效能不劣」
- 「二次探測/雙雜湊的常數係數更大，且難以正確實作」

**追問：** 「會不會出現一次聚集（primary clustering）問題？如何衡量？」
- 測試對 **碰撞解決策略的權衡理解**
- 預期：知道線性探測的弱點，但在給定約束下合理

---

**Q3: Kernel 的 `hashtable.h` 和 `rhashtable.h` 分別何時使用？**

**預期答案方向：**
- `hashtable.h`：簡單、固定大小的雜湊表，不需要動態擴張
- `rhashtable.h`：複雜、需要動態調整、多執行緒並行更新
- 「本題規模小（≤1000 元素），`hashtable.h` 夠用」

**追問：** 「核心中哪些子系統使用 `rhashtable`？」
- 測試 **核心代碼深度瞭解**
- 預期：提到網路堆疊（inode hash、netfilter conntrack 等）

---

### 3.2 碰撞解決與效能

**Q4: Approach 2 的 `set_insert` 實作中，為何使用 `& mask` 而不是 `% capacity`？**

**預期答案方向：**
```c
unsigned int idx = hash_int(key) & mask;  // mask = capacity - 1
```
- 「位元與 `&` 比模運算 `%` 快」
- 「要求 capacity 是 2 的冪（`next_pow2`）才能保證等價」
- 「2 的冪 - 1 的二進位全 1，所以 `& mask` 等同於 `% capacity`」

**追問：** 「如果 capacity 不是 2 的冪會發生什麼？」
- 測試 **位元操作的理解**
- 預期：某些雜湊值不會被映射到完整的 capacity 範圍，導致空檔，效能下降

---

**Q5: Kernel 的 `hash.h` 中為什麼使用多個質數乘數？**

**預期答案方向：**
```c
hash ^= hash >> 16;
hash *= 0x7feb352dU;   /* 質數 1 */
hash ^= hash >> 15;
hash *= 0x846ca68bU;   /* 質數 2 */
hash ^= hash >> 16;
```
- 「多個乘數和移位組合，打散低位和高位偏差」
- 「質數乘數避免某些位元的週期性」
- 「異或和移位是 avalanche 混合的標準技巧」

**追問：** 「這種設計對什麼類型的輸入最有效？」
- 測試對 **雜湊函式設計的深度理解**
- 預期：提到連續整數、指標對齊等常見的壞情況

---

### 3.3 去重與集合語義

**Q6: Approach 1 中，為什麼掃描值域時選擇 `for (int v = -1000; v <= 1000; v++)` 而不是直接掃輸入陣列？**

**預期答案方向：**
- 「輸入陣列可能有重複，直接掃不能保證『distinct』」
- 「掃完整值域，每個可能出現的值恰好檢查一次」
- 「題目要求 distinct，所以不能靠輸入的順序」

**追問：** 「這樣的時間複雜度是多少？能最佳化嗎？」
- 測試 **演算法分析能力**
- 預期：O(2001) = O(1)；無法最佳化（必須掃所有可能值）；或改用 Approach 2 來避免固定常數

---

**Q7: Approach 2 中 `collect_diff_unique` 使用了另一個臨時 `IntHashSet seen` 來追蹤已輸出的值，為什麼不直接掃輸入陣列？**

**預期答案方向：**
```c
static int collect_diff_unique(...) {
    IntHashSet seen;
    // ... 用 seen 追蹤已輸出的不同值
    // 透過 set_insert(&seen, v) 回傳 true 表示新值
    if (set_insert(&seen, v) && !set_contains(other_set, v)) {
        out[count++] = v;
    }
}
```
- 「輸入陣列 `src` 可能重複，直接掃會在 out 中加入多個相同值」
- 「用 `seen` hash set 確保『出現過的值只加一次』」
- 「這是 hash set 的典型去重模式」

**追問：** 「能用布林陣列替代 `seen` hash set 嗎？」
- 測試 **演算法適應性**
- 預期：可以，但因為 `src` 值域不固定，需要 Approach 2 的通用雜湊表

---

### 3.4 核心相關

**Q8: Kernel 的 `set_bit` / `clear_bit` / `test_bit` 與本題 `bool[]` 解法有什麼關聯？**

**預期答案方向：**
- 「Approach 1 的 `bool in1[2001]` 本質是位陣圖」
- 「Kernel 的位元操作是硬體等級最佳化版本」
- 「在真實核心代碼中，會用 `set_bit(idx, bitmap)` 代替 `bitmap[idx/8] |= (1 << (idx%8))`」
- 「Approach 1 用 `bool[]` 因為 LeetCode 追求代碼簡潔」

**追問：** 「Kernel 的 `test_and_set_bit` 與你的實作有什麼差別？」
- 測試 **原子操作與並行意識**
- 預期：提到原子性、多執行緒安全；Approach 1/2 都不需要（單執行緒）

---

**Q9: 在 Kernel 中，如果要為本題實作一個通用 hash set（支援任意值域），會怎麼設計？**

**預期答案方向：**
- 「使用 `hashtable.h` 或 `rhashtable.h` 的巨集」
- 「定義 `hlist_node` 在自己的結構體中」
- 「實作 comparator 函式（或巨集）來檢查相等性」
- 「動態分配節點，鏈接到雜湊表」

**範例概念：**
```c
struct my_int_node {
    int value;
    struct hlist_node node;
};

/* 在 hashtable 中搜尋 value == target 的節點 */
```

**追問：** 「這樣的設計與 Approach 2 的開放定址相比，優缺點是什麼？」
- 測試對 **鏈式 vs. 開放定址權衡的理解**

---

### 3.5 面試層面的檢查

**Q10: 給定本題，你會如何向不同經驗級別的面試官解釋你的選擇？**

**預期答案方向（視面試官背景）：**

- **未用過 Kernel 的面試官：**
  - 「值域固定，用布林陣列是常見技巧」
  - 「如果值域不固定，改用雜湊表」

- **Kernel 工程師：**
  - 「Approach 1 類似 Kernel 的點陣圖思想」
  - 「Approach 2 類似 Kernel hashtable + open addressing」
  - 「Kernel 版本會用 `rhashtable.h` 以支援動態擴張與並行」

- **系統程序設計面試官：**
  - 「時間複雜度、空間複雜度、常數係數的權衡」
  - 「Approach 1 的 O(2001) 是否接受，取決於值域固定假設」

---

## 四、面試加分總結

### 4.1 展示 Kernel 知識

- **提及 hashtable.h 與 bitmap.h 的區別**
  「這題體現了固定值域用點陣圖（Approach 1）、不定值域用雜湊表（Approach 2）的選擇」

- **解釋碰撞解決策略**
  「開放定址的線性探測比鏈式簡單，但會有聚集。Kernel 在 `rhashtable.h` 中用可調整表避免此問題」

- **討論位元運算最佳化**
  「用 `& mask` 代替 `% capacity` 是因為 capacity = 2^k，所以 `mask = 2^k - 1` 的所有位元都是 1」

### 4.2 展示語言精通度

- **能解釋為什麼用 `bool[2001]` 而不用 `int[]` 或 `char[]`**
  「`bool` 在 C99 後定義為 1 位元組，足以表示真假，且提高可讀性」

- **能比較不同去重方式的複雜度**
  - Approach 1（掃值域）: O(2001)，常數但受值域限制
  - Approach 2（`collect_diff_unique` 用 `seen` set）: O(n + m)，更通用

- **指出 Approach 2 中 `next_pow2` 的作用**
  「確保 capacity 是 2 的冪，讓 `& mask` 等同於模運算，避免浪費槽位」

### 4.3 展示系統思維

- **能討論 trade-off**
  - Approach 1: 空間最小（O(2001)），時間固定（O(2001)），但受限於值域
  - Approach 2: 空間動態（O(n+m)），時間平均 O(n+m)，通用性好

- **能想到退化情況**
  「如果 Approach 2 的雜湊函式設計不好，開放定址可能形成一次聚集，退化成 O(n^2)」

- **能提出最佳化方向**
  - 「使用 avalanche hash 與良好的乘數選擇來改善分佈」
  - 「如果效能仍不夠，改用 Kernel 的 `rhashtable.h` 來支援動態擴張」

### 4.4 展示面試風格

- **主動展示對 Kernel 代碼的熟悉**
  「這讓面試官看到你有系統層級的思維，不只停留在 LeetCode 技巧」

- **能在高層架構與細節間切換**
  - 高層：「集合差集用雜湊表」
  - 細節：「線性探測需要注意聚集，以及 capacity 為 2 的冪時可用位元與」

- **能舉例說明**
  「Kernel 的 `hashtable.h` 用於 inode cache，`rhashtable.h` 用於 netfilter conntrack，都是實戰例子」

---

## 五、Sources

### 主要參考

- [Linux Kernel hashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h)
  標準的鏈式雜湊表巨集定義

- [Linux Kernel hash.h](https://github.com/torvalds/linux/blob/master/include/linux/hash.h)
  通用雜湊函式（指標、長整數）

- [Linux Kernel rhashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/rhashtable.h)
  可調整大小的雜湊表，支援動態擴張與多執行緒

- [Linux Kernel bitmap.h](https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h)
  位陣圖操作巨集

- [Linux Kernel bitmap.c](https://github.com/torvalds/linux/blob/master/lib/bitmap.c)
  位陣圖實作（掃描、複製、邏輯運算）

- [Linux Kernel test_hash.c](https://github.com/torvalds/linux/blob/master/lib/test_hash.c)
  雜湊函式測試與效能評估

- [Linux Kernel test_bitmap.c](https://github.com/torvalds/linux/blob/master/lib/test_bitmap.c)
  位陣圖功能測試

### 補充閱讀

- [Linux Kernel bitops.h](https://github.com/torvalds/linux/blob/master/include/linux/bitops.h)
  CPU 相關的位元運算原語

- [Kernel Source Search (LXR)](https://elixir.bootlin.com/linux/latest/source)
  線上核心代碼瀏覽工具

- [LWN: Hash tables in the kernel](https://lwn.net/)
  (搜尋 "hash tables", "rhashtable" 等關鍵字)

---

**檔案建檔日期**: 2026-04-01
**相關 LeetCode 實作**:
- Main: `2215_Find the Difference of Two Arrays.c` (Approach 1, bool array)
- Alternative: `2215_Find the Difference of Two Arrays_Approch2.c` (IntHashSet, open addressing)
