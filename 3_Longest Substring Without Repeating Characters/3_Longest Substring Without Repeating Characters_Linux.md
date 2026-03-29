# LeetCode 3：最長不重複子字串 - Linux 核心對照分析

## 概述

LeetCode Problem 3 採用滑動視窗搭配直接存取表（direct-access table）`int lastSeen[128]`，以 O(n) 時間和 O(1) 空間求解。這個設計模式在 Linux 核心中也被廣泛應用於字元去重、網路封包解析、位元圖操作等場景。本文從核心開發者角度深入分析這一演算法與核心實作的關聯。

---

## 1. Linux 相關原始碼位置

### 1.1 直接存取表的核心應用

#### `lib/string.c` - 字串處理

Linux 核心的字串處理函式庫中，許多函式使用類似 `lastSeen[128]` 的直接存取表加速查詢：

```c
// 概念引用自 lib/string.c 中的字元集合操作
// 例如：strspn(), strcspn() 使用位元陣列或 lookup table
// 來快速判斷字元是否在集合內
int strspn(const char *s, const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;
    // 內部通常使用 lookup table 加速字元檢查
    // 類似於 lastSeen[] 的概念
}
```

**相關檔案**：`lib/string.c` (主要字串操作)

---

#### `include/linux/hashtable.h` 和 `lib/hash.c` - 雜湊表實作

核心的雜湊表實作使用碰撞解決策略（chaining），這與我們的直接存取表形成對比：

```c
// include/linux/hashtable.h 的概念
// 雜湊表使用鏈結法解決碰撞
struct hlist_node {
    struct hlist_node *next, **pprev;
};

// 相比之下，LeetCode 的 lastSeen[128] 完全避免碰撞
// 因為我們知道輸入字元集合的大小（ASCII 128 或擴展 ASCII 256）
```

**相關檔案**：`include/linux/hashtable.h`、`lib/hash.c`

---

#### `net/core/flow_dissector.c` - 網路封包解析

網路封包解析器使用滑動視窗概念逐位元組掃描封包資料，與 LeetCode Problem 3 的滑動視窗邏輯相似：

```c
// 概念引用：net/core/flow_dissector.c
// 典型的滑動視窗封包解析
size_t pkt_pos = 0;
size_t pkt_end = len;

while (pkt_pos < pkt_end) {
    // 處理目前視窗內的位元組
    // 根據協定規則推進左邊界和右邊界
    // 類似於 LeetCode 的 left/right 指標
    pkt_pos++;
}
```

**相關檔案**：`net/core/flow_dissector.c`

---

#### `include/linux/bitmap.h` 和 `lib/bitmap.c` - 位元圖和去重

位元圖操作中的直接存取模式與我們的字元去重邏輯相似：

```c
// include/linux/bitmap.h 中的直接存取概念
// 使用固定大小陣列快速判斷元素是否存在
#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

// 這類似於 lastSeen[128] 的「常數時間查詢」概念
// 但使用位元集合而非整數陣列
```

**相關檔案**：`include/linux/bitmap.h`、`lib/bitmap.c`

---

#### `net/netfilter/nf_conntrack*.c` - 連線追蹤和去重

Netfilter 連線追蹤模組使用雜湊表追蹤已看過的連線，以偵測和防止重複封包：

```c
// 概念引用自 net/netfilter/nf_conntrack_core.c
// 追蹤已見過的 (源 IP, 目標 IP, 埠) 組合，避免重複處理
static inline u32 hash_conntrack(const struct nf_conntrack_tuple *tuple)
{
    // 使用雜湊函式映射到固定大小的表
    // 類似 lastSeen[] 但透過雜湊來處理碰撞
}
```

**相關檔案**：`net/netfilter/nf_conntrack_core.c`、`net/netfilter/nf_conntrack_hash.c`

---

#### `fs/seq_file.c` - 檔案系統緩衝視窗

seq_file 提供的檔案讀取介面使用類似滑動視窗的緩衝管理：

```c
// fs/seq_file.c 的概念
// 維護讀取位置和緩衝視窗
struct seq_file {
    char *buf;               // 緩衝區
    size_t size;             // 緩衝區大小
    size_t from;             // 視窗左邊界（讀位置）
    size_t count;            // 視窗右邊界（寫位置）
    loff_t index;            // 邏輯位置指標
    ...
};

// 邏輯上就是一個滑動視窗：
// [from ... count] 定義了活躍的讀取視窗
```

**相關檔案**：`fs/seq_file.c`

---

### 1.2 核心中的字元去重和直接存取

#### `lib/ctype.c` - 字元分類

```c
// lib/ctype.c 中的字元分類表
const unsigned char _ctype[] = {
    _C, _C, _C, _C, _C, _C, _C, _C,  /* 0-7 */
    _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,  /* 8-15 */
    _C, _C, _C, _C, _C, _C, _C, _C,  /* 16-23 */
    _C, _C, _C, _C, _C, _C, _C, _C,  /* 24-31 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P,  /* 32-39 */
    // ...
};

// 這是一個 256 元素的直接存取表，供 isalpha(), isdigit() 等函式使用
// 設計邏輯完全類似 lastSeen[128]：
// - 固定大小（針對 ASCII 或擴展 ASCII）
// - 常數時間查詢 O(1)
// - 初始化成本 O(256) 但攤銷到眾多查詢
```

**相關檔案**：`lib/ctype.c`

---

### 1.3 滑動視窗在核心驅動中的應用

#### 驅動程式的環形緩衝區（Circular Buffer）

許多驅動程式使用環形緩衝區來管理 I/O，本質上是滑動視窗的循環版本：

```c
// 典型的環形緩衝區實作（例如串行驅動）
struct uart_buf {
    char buffer[UART_BUFFER_SIZE];
    int head;   // 寫位置（類似 right）
    int tail;   // 讀位置（類似 left）
};

// 視窗大小 = (head - tail + BUFFER_SIZE) % BUFFER_SIZE
// 每次讀/寫會推進 left 或 right
```

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 演算法模式對比

| 特性 | LeetCode 3 解法 | Linux 核心 |
|------|-------------------|-----------|
| **資料結構** | 整數陣列 `int[128]` | 雜湊表、位元圖、陣列 |
| **衝突解決** | 完全避免（預知大小） | 鏈結法 (chaining) |
| **時間複雜度** | O(n)，無雜湊衝突成本 | O(n) 平均情況 |
| **空間複雜度** | O(1)（固定 128 位置） | O(m)（m = 表大小） |
| **初始化** | O(128) 一次性 | O(n) 可能動態增長 |

---

### 2.2 應用場景對比

#### LeetCode 的直接存取表方法：

**優點**：
- 輸入限制明確（ASCII 字元），無需動態大小調整
- 無雜湊碰撞，每次查詢都是 O(1)
- 快取友好（128 整數 = 512 字節，通常在 L1 快取）
- 初始化和查詢都極其簡單

**劣點**：
- 若擴展到 Unicode（2^21 個字元），表會爆炸
- 浪費空間（128 個位置中可能只用少數）

---

#### Linux 核心的動態雜湊表方法：

**優點**：
- 可處理任意大小的集合
- 動態調整大小
- 通用性強

**劣點**：
- 雜湊碰撞可能發生，最壞情況 O(n)
- 記憶體管理開銷（malloc/free）
- 快取不友好（鏈結節點分散在記憶體中）

---

### 2.3 字元/位元組去重在核心中的具體應用

#### 場景 1：Network Packet Parsing

```c
// 簡化的網路封包去重邏輯
// （概念引用自 net/core/）
typedef struct {
    unsigned char packet[MTU_SIZE];
    int lastSeen[256];  // 針對所有可能的位元組值
    int windowStart;
    int windowEnd;
} PacketProcessor;

// 掃描封包查找最長的「無重複位元組序列」
// 用於檢測網路異常或編碼效率
```

#### 場景 2：字元集合驗證

```c
// 檔案系統或使用者輸入驗證
// （概念引用自 lib/string.c）
int validate_allowed_chars(const char *input, const char *allowed_set)
{
    int seen[256];
    // 初始化
    for (int i = 0; i < 256; i++) seen[i] = -1;

    // 掃描輸入，確保所有字元都在允許集合內
    for (int i = 0; input[i]; i++) {
        unsigned char c = input[i];
        if (seen[c] >= 0) {
            // 發現重複
            return -1;
        }
        seen[c] = i;
    }
    return 1;
}
```

---

### 2.4 快取行為分析

#### LeetCode `int[128]` vs `int[256]`：

```c
// int[128] = 512 字節（假設 int = 4 字節）
// 典型 L1 快取行 = 64 字節
// 512 / 64 = 8 快取行

// int[256] = 1024 字節
// 1024 / 64 = 16 快取行

// 在現代 CPU 上，隨機存取時：
// - int[128] 的快取命中率更高
// - 但跨度差異不大（都遠小於 L2 快取 256KB）
```

#### 核心中的實際考慮：

Linux 核心開發者通常會：
1. 根據目標架構選擇表大小
2. 考慮 NUMA 親和性（多核系統）
3. 使用 `__read_mostly` 和 `__cache_aligned` 標記優化快取
4. 在性能關鍵路徑上避免碰撞（寧可多用記憶體）

---

## 3. 主管 Code Review 角度考題

### 問題 1：為什麼使用 `int[128]` 而非 `int[256]`？

**提問背景**：
展示對輸入假設和資源權衡的理解。

**預期方向**：

候選人應說明：
- **輸入限制**：LeetCode 問題通常限制為標準 ASCII（128 字元）
- **記憶體成本**：`int[128]` = 512 字節 vs `int[256]` = 1024 字節
- **快取效率**：128 更可能完全駐留在 L1 快取（通常 32-64KB）
- **實踐考慮**：若輸入可能包含擴展 ASCII 或 UTF-8，應改用 256 或更大

**核心考查**：
- 是否理解系統資源（記憶體、快取）的重要性
- 是否思考過算法的實務部署

**進階回答**：
> 我會根據輸入來動態選擇。若問題明確為 ASCII，用 128。若允許擴展 ASCII（如 Latin-1），用 256。若要支援 Unicode，改用雜湊表或 map，因為直接存取表會變成 2^21 個位置，完全不可行。

---

### 問題 2：如何處理任意 Unicode 字串？

**提問背景**：
評估候選人對國際化和實務挑戰的理解。

**預期方向**：

候選人應認識到：
- **Unicode 範圍**：U+0000 到 U+10FFFF，共 1,114,112 個字元
- **不可行方案**：直接 `int[1114112]`（太大）
- **替代方案**：
  1. **雜湊表/Map**：`std::unordered_map<char, int>` 或 `std::map<char, int>`
  2. **動態陣列**：按需分配
  3. **字節級處理**：若輸入是 UTF-8 編碼的位元組陣列，則保留 `[256]`（針對位元組，非字元）

**核心考查**：
- 是否認識到演算法的假設和限制
- 是否能提出實用的替代方案

**進階回答**：

```c
// 使用雜湊表方案
#include <stdint.h>

typedef struct {
    uint32_t codepoint;
    int lastIndex;
} CharEntry;

// 或簡單使用 C++ 的 unordered_map
#include <unordered_map>
int lengthOfLongestSubstring_Unicode(const std::string& s) {
    std::unordered_map<char, int> lastSeen;
    int left = 0, maxLen = 0;

    for (int right = 0; right < s.length(); right++) {
        unsigned char c = s[right];
        if (lastSeen.find(c) != lastSeen.end() && lastSeen[c] >= left) {
            left = lastSeen[c] + 1;
        }
        lastSeen[c] = right;
        maxLen = std::max(maxLen, right - left + 1);
    }
    return maxLen;
}
```

---

### 問題 3：快取行為對性能的影響有多大？

**提問背景**：
評估候選人對低階硬體特性的理解。

**預期方向**：

候選人應能討論：

**定量分析**：
```
假設 Intel Core i7：
- L1 快取：32 KB，64 字節/行
- 存取延遲：~4 周期
- 快取未命中延遲：~200 周期
- 分支預測失誤：~20 周期

int[128]：
- 大小：512 字節 = 8 快取行
- 首次載入成本：8 × 64 字節 ~ 9 微秒
- 後續存取：O(1) L1 快取命中

int[256]：
- 大小：1024 字節 = 16 快取行
- 首次載入成本：16 × 64 字節 ~ 18 微秒
- 但仍在 L1 快取範圍內
```

**實踐影響**：
- 對於短字串（< 1000 字元），差異微乎其微
- 對於超大字串（> 1M 字元），快取命中率可能差 5-10%
- **真正的瓶頸**：字元檢查邏輯本身，而非表的大小

**核心考查**：
- 是否懂得測量而非臆測
- 是否理解微最佳化 vs. 真正優化的區別

**進階回答**：

> 快取行為確實重要，但不是決定因素。對於 LeetCode 問題的規模，int[128] vs int[256] 的影響 < 1%。真正的性能優化應該關注：
> 1. 避免多餘的字串掃描（已做）
> 2. 分支預測友好（我們的循環很直線性）
> 3. 編譯器最佳化（開啟 -O3）
>
> 如果需要極端性能，應該實施 SIMD（如 AVX-512）來並行檢查多個字元，但這對 LeetCode 來說是過度設計。

---

### 問題 4：Linux 核心的雜湊表如何處理碰撞，與我們的方法有何區別？

**提問背景**：
評估候選人對資料結構權衡和核心設計原理的理解。

**預期方向**：

核心使用 **鏈結法** (Separate Chaining)：

```c
// include/linux/hashtable.h 的概念
struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

// 碰撞時，新節點插入鏈首（O(1) 插入）
// 查找時，遍歷鏈表（最壞情況 O(n)）
```

**對比分析**：

| 方面 | 直接存取 (LeetCode) | 鏈結法 (核心) |
|------|------------|---------|
| 碰撞 | 完全避免 | 允許並處理 |
| 查詢 | O(1) 保證 | O(1) 平均、O(n) 最壞 |
| 插入 | O(1) | O(1) 平均 |
| 刪除 | 不適用 | O(1) 平均 |
| 動態大小調整 | 固定 | 動態可行 |
| 記憶體浪費 | 已知大小時少 | 鏈表指標開銷 |

**核心為何選擇鏈結法**：
1. **通用性**：不知道鍵的範圍
2. **動態性**：可能需要任意大小的集合
3. **不同場景**：無法為每個場景硬編碼表大小

**LeetCode 為何用直接存取**：
1. **明確限制**：ASCII 字元
2. **簡潔性**：一行初始化，一行查詢
3. **最優性**：已知輸入域，無需通用方案

**核心考查**：
- 是否理解「沒有銀彈」原則
- 是否能根據約束條件選擇最優方案

**進階回答**：

> 直接存取和鏈結法各有用途。LeetCode 問題明確限制輸入為標準字串（通常 ASCII），所以直接存取完美適配。
>
> 但在 Linux 核心中，函式需要處理任意資料和哈希值，無法預知輸入範圍。例如 inode 表、網路連線追蹤表等，可能需要儲存數百萬項。此時鏈結法提供了可擴展性。
>
> 核心也有針對性的最佳化：
> - 使用 `__hash_32()` 等內建雜湊函式減少碰撞
> - 定期重新計算 rehash 以保持平衡
> - 在快取路徑（hot paths）上使用 RCU (Read-Copy-Update) 而非鎖定

---

### 問題 5：此代碼在生產系統中如何防守性編程？

**提問背景**：
評估候選人對邊界條件、安全性和可靠性的關注。

**預期方向**：

生產代碼應補充：

```c
int lengthOfLongestSubstring(const char* s) {
    // 防守性：檢查空指標
    if (s == NULL) {
        return 0;  // 或記錄錯誤
    }

    int lastSeen[128];
    int left = 0;
    int maxLen = 0;

    // 防守性：初始化應使用 memset 而非循環（更快、更安全）
    memset(lastSeen, -1, sizeof(lastSeen));

    // 防守性：界限檢查
    size_t len = strlen(s);
    if (len == 0) {
        return 0;
    }

    for (size_t right = 0; right < len; right++) {
        unsigned char currentChar = (unsigned char)s[right];

        // 防守性：assert 確保索引在範圍內
        assert(currentChar < 128);

        if (lastSeen[currentChar] >= left) {
            left = lastSeen[currentChar] + 1;
        }

        lastSeen[currentChar] = (int)right;

        int currentLen = (int)(right - left) + 1;
        if (currentLen > maxLen) {
            maxLen = currentLen;
        }
    }

    return maxLen;
}
```

**防守性編程的考量**：
1. **null 指標檢查**
2. **整數溢位檢查**（很少發生，但可能）
3. **類型轉換的顯式性**
4. **常數邊界假設的驗證**（如 `currentChar < 128`）
5. **日誌記錄**（用於調試）
6. **單元測試**（邊界案例）

**核心中的實踐**：

Linux 核心對防守編程非常嚴格：

```c
// 典型的核心風格
if (unlikely(s == NULL)) {
    pr_err("invalid input to func\n");
    return -EINVAL;
}

if (unlikely(len > MAX_SAFE_SIZE)) {
    return -ENOMEM;
}

// 核心也廣泛使用 might_sleep(), must_hold_lock() 等巨集
might_sleep();
```

**核心考查**：
- 是否考慮過邊界和錯誤情況
- 是否理解防守性編程的成本與益處

---

## 4. 面試加分總結

### 關鍵加分點

#### 1. **演算法複雜度分析的深度**
- 說出時間複雜度 O(n) 是必須的
- 加分：解釋為什麼避免了常數因子（無雜湊碰撞）
- 加分++：討論空間複雜度 O(1) 的假設前提（輸入限制）

#### 2. **跨領域知識應用**
- 說出「滑動視窗」是基礎
- 加分：舉例 Linux 核心或其他系統如何應用滑動視窗（網路封包、檔案 I/O）
- 加分++：討論在核心中為何用雜湊表而非直接存取表

#### 3. **資源感知**
- 基礎：說出 O(1) 空間，因為字符集固定
- 加分：計算 `int[128]` 的實際字節數（512 字節）
- 加分++：分析快取行為，討論 int[128] vs int[256] 的權衡

#### 4. **實務關心**
- 基礎：提交AC代碼
- 加分：考慮 UTF-8 或 Unicode 的情況，說明改進方案
- 加分++：提出防守性編程改進（null 檢查、邊界驗證）

#### 5. **系統思維**
- 基礎：單一演算法解決問題
- 加分：將演算法放在更大背景下（核心應用、工業實踐）
- 加分++：討論權衡（通用性 vs. 效率）和選擇準則

### 經典回答框架

**"我會這樣回答與核心相關的問題"**：

> 這個問題本質上是用直接存取表實現字符去重。在 LeetCode 的背景下，限定了字符集（ASCII 128），所以我選擇了 `int[128]` 作為 lookup table。
>
> **核心啟發**：Linux 核心在類似情況下也做過相同權衡。例如在 `lib/ctype.c` 中，有一個 256 元素的字符分類表供 `isalpha()`、`isdigit()` 等函數使用。原理相同，但核心選了 256 是因為要支援擴展 ASCII。
>
> **性能對標**：核心在性能關鍵路徑（如 `net/core/flow_dissector.c` 的封包解析）使用類似的直接存取模式。其原因是：無碰撞、快取友好、預測性佳。
>
> **可擴展性**：若問題改為支援 Unicode，我會改用雜湊表（C++ 的 `unordered_map` 或 C 的 `struct hlist`）。這樣做就像核心選擇用 `hashtable.h` 來處理任意鍵的情況。
>
> **防守性**：實務代碼我會加入 null 指標檢查、類型轉換驗證等，模仿核心的風格。

---

## 5. Sources

### Linux 核心源碼 (GitHub - torvalds/linux)

- **字符處理**
  - `lib/string.c` - https://github.com/torvalds/linux/blob/master/lib/string.c
  - `lib/ctype.c` - https://github.com/torvalds/linux/blob/master/lib/ctype.c

- **雜湊表和資料結構**
  - `include/linux/hashtable.h` - https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h
  - `lib/hash.c` - https://github.com/torvalds/linux/blob/master/lib/hash.c
  - `include/linux/bitmap.h` - https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h
  - `lib/bitmap.c` - https://github.com/torvalds/linux/blob/master/lib/bitmap.c

- **網絡和流量處理**
  - `net/core/flow_dissector.c` - https://github.com/torvalds/linux/blob/master/net/core/flow_dissector.c
  - `net/netfilter/nf_conntrack_core.c` - https://github.com/torvalds/linux/blob/master/net/netfilter/nf_conntrack_core.c

- **文件系統**
  - `fs/seq_file.c` - https://github.com/torvalds/linux/blob/master/fs/seq_file.c

---

## 補充：實作比較表

### 核心概念對標

| 核心概念 | LeetCode 3 | Linux 核心示例 | 通用原理 |
|---------|-----------|--------------|--------|
| **直接存取表** | `int lastSeen[128]` | `_ctype[256]` in ctype.c | 輸入域已知，無碰撞 |
| **滑動視窗** | left/right 指標 | flow_dissector 的 pkt_pos | 流式處理，固定或動態窗口 |
| **字符去重** | 單一最長子字串 | 連線追蹤去重 | 快速成員檢查 |
| **雜湊表** | 不適用（固定域） | hlist_* in hashtable.h | 任意大小集合，允許碰撞 |
| **動態大小** | 固定 128 | 動態 rehash | 根據負載調整 |

---

## 總結

LeetCode Problem 3 的滑動視窗 + 直接存取表方案，是對已知輸入約束的完美利用。相較之下，Linux 核心需要處理無限多樣的輸入，因此依賴通用的雜湊表和動態資料結構。但兩者背後的思想是一致的：

1. **選擇最適合輸入域的資料結構**
2. **在時間和空間上找到平衡點**
3. **考慮硬體特性（快取、分支預測）**
4. **在防守性編程上保持警惕**

掌握這些觀點，不僅能解決 LeetCode 問題，更能與核心開發者對話，展現系統級的思維。
