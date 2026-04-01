# LeetCode 1768 - Merge Strings Alternately: Linux Kernel Perspective

## 問題概述

**題目**: Merge Strings Alternately
**難度**: Easy
**核心概念**: 雙指標 (Two Pointers) 交替合併字串

給定兩個字串 `word1` 和 `word2`，交替取出它們的字符，合併成一個新字串。若一個字串較短，則將另一個字串的剩餘字符附加到結果末尾。

```c
// 示例
Input: word1 = "abc", word2 = "pqr"
Output: "apbqcr"

Input: word1 = "ab", word2 = "pqrs"
Output: "apbqrs"
```

---

## 1. Linux 相關原始碼位置

### 1.1 Basic String Manipulation - `lib/string.c`
**位置**: `linux/lib/string.c`
**相關函式**: `memcpy()`, `strcpy()`, `strncat()`, `memmove()`
**連結**: 這些基礎函式提供了原始記憶體和字串操作的基礎。LeetCode 的合併操作本質上是連續的 `memcpy()` 或 `memmove()` 呼叫的應用。

### 1.2 Merge Sort Implementation - `lib/sort.c` / `lib/list_sort.c`
**位置**: `linux/lib/sort.c`, `linux/lib/list_sort.c`
**核心演算法**: Merge sort 的 merge 步驟
**連結**: Merge sort 中兩個排序序列的合併是經典的雙指標演算法。核心邏輯與字串合併相同：維持兩個指標，根據條件交替選取元素。在核心中，此模式用於合併排序的遞歸分割階段。

### 1.3 DMA Scatter-Gather Lists
**位置**: `drivers/base/dma-mapping.c`, `include/linux/dma-mapping.h`
**概念**: I/O 子系統中的 scatter-gather 列表交替地從多個記憶體位置收集資料
**連結**: 與字串合併類似，DMA 操作需要從多個來源交替提取資料段，填充單一連續的輸出緩衝區。

### 1.4 Network Packet Reassembly
**位置**: `net/ipv4/ip_fragment.c`, `net/ipv6/reassembly.c`
**概念**: 從多個分段 (fragments) 交替合併網路封包
**連結**: IP 片段重組需要從多個輸入佇列交替取出片段，重構完整的封包。這與字串合併在邏輯上相同。

### 1.5 Pipe Splicing - `fs/splice.c`
**位置**: `linux/fs/splice.c`
**概念**: 將資料從多個源交替合併到輸出管道 (pipe)
**連結**: Splice 操作支援從多個檔案描述子和管道合併資料流，通常涉及交替訪問多個輸入源。

### 1.6 Buffer Management - `kernel/trace/trace.c`
**位置**: `linux/kernel/trace/trace.c`
**概念**: Ring buffer 中的事件交錯與合併
**連結**: 核心追蹤子系統會從多個 CPU 的環形緩衝區交替讀取追蹤事件，類似於合併多個序列。

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 演算法比較

| 層面 | LeetCode 1768 | Linux Merge Sort (`lib/list_sort.c`) | DMA Scatter-Gather | Network Reassembly |
|-----|--------------|--------------------------------------|-------------------|-------------------|
| **輸入** | 兩個字串 | 兩個排序的鏈表/陣列 | 多個記憶體頁面 | 多個 IP 片段 |
| **合併邏輯** | 交替選取字符 | 根據大小比較選取 | 按優先級交替選取 | 按序號順序合併 |
| **輸出結構** | 單一連續字串 | 單一排序序列 | 單一連續記憶體 | 單一完整 IP 報文 |
| **時間複雜度** | O(m + n) | O(m + n) | O(m + n) | O(m + n) |
| **空間複雜度** | O(m + n) | O(1) 或 O(n) | O(1) | O(n) |

### 2.2 資料結構對照

**LeetCode Solution**:
```c
void mergeAlternately(char* word1, char* word2, char* result) {
    int i = 0, j = 0, k = 0;
    int len1 = strlen(word1);
    int len2 = strlen(word2);

    // 交替合併
    while (i < len1 && j < len2) {
        result[k++] = word1[i++];
        result[k++] = word2[j++];
    }

    // 附加剩餘字符
    while (i < len1) {
        result[k++] = word1[i++];
    }
    while (j < len2) {
        result[k++] = word2[j++];
    }
    result[k] = '\0';
}
```

**Kernel Merge Sort (簡化)**:
```c
// From linux/lib/list_sort.c (conceptual merge)
static noinline void merge(void *priv,
                           int (*cmp)(void *priv, const struct list_head *a,
                                     const struct list_head *b),
                           struct list_head *a, struct list_head *b) {
    struct list_head head;
    struct list_head *tail = &head;

    // 交替合併兩個排序的鏈表
    while (a && b) {
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }

    // 附加剩餘元素
    tail->next = (a != NULL) ? a : b;
}
```

**Scatter-Gather DMA (概念)**:
```c
// From drivers/base/dma-mapping.c (simplified)
void sg_copy_from_multiple(struct scatterlist *sg_out,
                           struct scatterlist *sg_in1,
                           struct scatterlist *sg_in2,
                           size_t total_len) {
    struct scatterlist *curr_in1 = sg_in1;
    struct scatterlist *curr_in2 = sg_in2;
    struct scatterlist *curr_out = sg_out;
    int toggle = 0;  // 交替選擇輸入源

    // 交替從兩個來源複製資料
    while (total_len > 0) {
        struct scatterlist *src = (toggle == 0) ? curr_in1 : curr_in2;
        size_t to_copy = min(src->length, total_len);

        memcpy(sg_virt(curr_out), sg_virt(src), to_copy);
        total_len -= to_copy;
        curr_out = sg_next(curr_out);

        toggle = 1 - toggle;
    }
}
```

### 2.3 合併策略對照

| 策略 | LeetCode | Merge Sort | DMA | 網路重組 |
|-----|---------|-----------|-----|---------|
| **選擇標準** | 輪流交替 | 值比較 | 優先級/位置 | 序列號順序 |
| **邊界處理** | 分別附加 | 分別附加 | 根據 SG 數組 | 根據片段索引 |
| **同步方式** | 雙指標 | 雙指標 | SG 迭代器 | 序列號追蹤 |

### 2.4 複雜度分析

**時間複雜度**:
- LeetCode: **O(m + n)** — 每個字符訪問一次
- Merge Sort: **O(m + n)** — 每個元素訪問一次
- DMA: **O(m + n)** — 每個頁面訪問一次
- 網路重組: **O(m + n)** — 每個片段訪問一次

**空間複雜度**:
- LeetCode: **O(m + n)** — 輸出字串
- Merge Sort: **O(1)** (in-place) 或 **O(n)** (需要臨時空間)
- DMA: **O(1)** — 就地操作
- 網路重組: **O(n)** — 可能需要片段緩衝區

### 2.5 API 設計差異

| 方面 | LeetCode | Kernel |
|-----|---------|--------|
| **參數驗證** | 假設有效輸入 | 完整的邊界檢查和錯誤處理 |
| **記憶體管理** | 呼叫者負責配置 | `kmalloc()`, `vmalloc()`, `alloc_pages()` |
| **錯誤處理** | 無 | `errno`, `-ENOMEM`, `-EINVAL` 等 |
| **最佳化** | 無 | SIMD, 批次操作, 快取對齐 |
| **並發性** | 無 | 自旋鎖 (spinlock)、互斥鎖 (mutex)、RCU |

---

## 3. 主管 Code Review 角度考題

### 3.1 記憶體安全與邊界檢查

**Code Review 問題**:
> "你的合併函式假設輸出緩衝區有足夠空間。在核心代碼中，這樣的假設會導致什麼問題？你會如何進行防禦性編程？"

**預期回答方向**:
- 在核心中，緩衝區溢出會導致堆損壞、特權提升或崩潰
- 應該：(1) 傳入輸出緩衝區的最大大小；(2) 使用 `strlcat()` 或 `snprintf()` 而非 `strcpy()`；(3) 檢查邊界
- LeetCode 簡化了，但核心需要檢查

**追問**:
- "如果輸入不是 null-terminated，會怎樣？"
  - **答**: 需要同時傳入長度參數，不能假設字符串以 `\0` 結尾

- "在 DMA 操作中，這個問題會如何表現？"
  - **答**: DMA 只複製指定字節數，不檢查內容；必須由驅動程式確保長度正確

**考題本質**: 測試對**記憶體安全**、**防禦性編程**和 **kernel 代碼嚴謹性** 的理解

---

### 3.2 複雜度與效能最佳化

**Code Review 問題**:
> "你的實作在最好、平均和最壞情況下都是 O(m+n)。在 kernel 中，是否還有其他考量因素會影響實際效能？"

**預期回答方向**:
- **快取局部性**: 線性訪問有良好的快取命中率
- **分支預測**: 固定的交替模式易於預測
- **SIMD 優化**: 可以使用向量指令同時複製多個字符（如 SSE, AVX）
- **批次操作**: kernel 中的 `memcpy()` 已優化，包括：
  - 對齐檢查
  - 大量複製時使用 `rep movsq` (x86)
  - 小複製時的內聯優化

**追問**:
- "在 network reassembly 中，如果片段來自不同的 NUMA 節點，會發生什麼？"
  - **答**: 會產生遠程記憶體訪問延遲；應考慮本地化策略或使用 NUMA-aware 分配

- "DMA scatter-gather 列表中，如果頁面不連續，效能如何？"
  - **答**: DMA 透過 IOMMU 映射來處理，但如果頁面分散，TLB 命中率下降，導致翻譯延遲增加

**考題本質**: 測試對 **kernel 效能特性**、**硬體互動** 和 **最佳化策略** 的深度理解

---

### 3.3 並發與同步問題

**Code Review 問題**:
> "如果多個 CPU 同時執行 merge 操作，會存在什麼資料競爭問題？kernel 中如何處理？"

**預期回答方向**:
- LeetCode 中無並發問題（單線程）
- Kernel 中：
  - 如果多個任務讀寫同一緩衝區，需要 **同步原語** (spinlock, mutex, RCU)
  - 環形緩衝區 (ring buffer) 設計可以無鎖地支持一個生產者和一個消費者
  - 追蹤子系統中，每個 CPU 有獨立的環形緩衝區以避免爭鬥

**追問**:
- "在 network reassembly 中，多個 NIC 隊列同時填充片段，如何確保正確順序？"
  - **答**: 使用 per-queue 鎖、序列號追蹤和已排序的片段清單

- "使用 RCU (Read-Copy-Update) 如何改進合併操作？"
  - **答**: RCU 允許讀者無鎖訪問，寫者延遲更新直到寬限期；適用於多讀少寫場景

**考題本質**: 測試對 **kernel 並發模型**、**同步原語** 和 **實時約束** 的認識

---

### 3.4 API 設計與呼叫約定

**Code Review 問題**:
> "在 kernel 中，merge 函式通常有不同的參數約定。比較 LeetCode 與 kernel 實作，API 設計上的權衡是什麼？"

**預期回答方向**:

| 方面 | LeetCode | Kernel |
|-----|---------|--------|
| **返回值** | `void` (寫入輸出參數) | `int` (錯誤碼) 或結構體指標 |
| **參數** | 指標、隱含長度 (strlen) | 明確長度、邊界檢查 |
| **副作用** | 修改輸出緩衝區 | 可能涉及記憶體配置、鎖定 |
| **初始化** | 無 | 可能需要初始化結構體 |

**追問**:
- "為什麼 kernel 的 `merge()` 通常返回 `int` 而非 `void`？"
  - **答**: 為了報告錯誤（如記憶體不足、無效參數），允許呼叫者進行錯誤恢復

- "`list_sort()` 採用函式指標進行比較，這帶來什麼好處和開銷？"
  - **答**: **好處**: 通用性，支持任意資料類型；**開銷**: 函式指標調用的間接成本，但通常被演算法效率超過

**考題本質**: 測試對 **API 設計原則**、**靈活性與效能** 和 **錯誤處理模式** 的理解

---

### 3.5 可測試性與邊界情況

**Code Review 問題**:
> "列舉你會在 code review 中測試的邊界情況。在 kernel 中，某些情況會更關鍵嗎？"

**預期回答方向**:

**邊界情況**:
1. **空字符串**: `word1=""`, `word2="abc"` → 應返回 `"abc"`
2. **等長字符串**: `word1="ab"`, `word2="cd"` → `"acbd"`
3. **單字符**: `word1="a"`, `word2="b"` → `"ab"`
4. **非常長的輸入**: 測試緩衝區大小和效能
5. **特殊字符**: 空字符、換行符、UTF-8 多字節序列

**Kernel 關鍵情況**:
- **NULL 指標**: 必須檢查，否則導致 NULL dereference → 核心轉儲
- **非對齊記憶體**: 某些硬體上導致例外
- **分頁記憶體**: 跨頁面邊界的複製可能涉及多個 TLB 項目
- **DMA 安全記憶體**: 某些驅動程式要求分配來自 `DMA-safe` 區域
- **大量輸入**: 堆棧溢出 (如在堆棧上分配輸出緩衝區)

**追問**:
- "如果 `word1` 或 `word2` 不是 null-terminated，標準 `strlen()` 會做什麼？"
  - **答**: 無定義行為；可能讀取超過預期的記憶體。應使用顯式長度參數或限制循環

- "在 DMA 操作中，page fault 會怎樣？"
  - **答**: DMA 引擎不能處理 page fault；記憶體必須已鎖定在物理記憶體中，通常使用 `get_user_pages()`

**考題本質**: 測試 **系統級思維**、**防禦性測試** 和 **生產級代碼質量標準**

---

### 3.6 文檔與可維護性

**Code Review 問題**:
> "你的合併實作缺少文檔。Kernel 代碼期望什麼樣的註釋和文檔風格？為什麼重要？"

**預期回答方向**:

Kernel Documentation 標準 (linux/Documentation/kernel-doc-nano-HOWTO.txt):
```c
/**
 * merge_strings_alternately - Merge two strings character by character
 * @word1: First input string
 * @word2: Second input string
 * @result: Output buffer (must be large enough for word1 + word2)
 * @result_size: Size of output buffer in bytes
 *
 * Merges @word1 and @word2 alternately into @result. If one string
 * is shorter, remaining characters from the longer string are appended.
 *
 * Return: 0 on success, -EINVAL if result_size is too small, -EFAULT
 *         if copying from user space fails.
 *
 * Context: Can sleep if called with __GFP_WAIT flag.
 */
int merge_strings_alternately(const char *word1, const char *word2,
                              char *result, size_t result_size);
```

**關鍵文檔要素**:
1. **函式功能**: 清晰描述輸入、輸出和行為
2. **參數**: 詳細說明每個參數的含義和約束
3. **返回值**: 成功和錯誤代碼的含義
4. **Context**: 函式可以在哪些上下文中調用 (atomic,可睡眠等)
5. **副作用**: 記憶體配置、鎖定持有等
6. **競爭條件**: 任何已知的並發問題

**追問**:
- "為什麼 Kernel 要求 'Context' 部分的註釋？"
  - **答**: 因為 kernel 代碼在不同的上下文運行 (IRQ handler, atomic, 可睡眠)；某些操作 (如 `kmalloc(GFP_KERNEL)`) 只在可睡眠上下文中有效

- "如果文檔過時，會帶來什麼風險？"
  - **答**: 誤用函式（如在不允許睡眠的上下文中調用）、資料競爭、死鎖，難以偵測和除錯

**考題本質**: 測試對 **文檔重要性**、**API 契約** 和 **可維護性** 的認識

---

## 4. 面試加分總結

### 4.1 核心知識點

1. **演算法基礎** ✓
   - 雙指標是處理多序列合併的經典範式
   - 時間 O(m+n)、空間 O(m+n) 最優
   - 應用場景廣泛：排序、字串處理、I/O 操作

2. **Kernel 源碼理解** ✓
   - Merge sort (`lib/list_sort.c`): 同樣的雙指標邏輯，但基於比較
   - DMA scatter-gather: 交替從多個來源複製資料
   - 網路棧重組: 合併片段序列
   - 各模塊實現細節展示了演算法在實務中的應用

3. **效能考量** ✓
   - 快取局部性: 線性訪問優於隨機訪問
   - SIMD 優化: 向量指令加速複製
   - NUMA 感知: 記憶體位置影響延遲
   - 硬體互動: DMA、IOMMU、TLB

4. **系統級思維** ✓
   - 記憶體安全: 邊界檢查、緩衝區溢出防御
   - 並發控制: 同步原語、無鎖設計、RCU
   - 錯誤處理: 返回碼、異常路徑恢復
   - API 設計: 靈活性與效能的權衡

### 4.2 面試答題策略

**當被問 "Merge Strings Alternately"**:

1. **先解決基礎問題** (2-3 分鐘)
   - 清晰講述演算法: "使用兩個指標 i 和 j，交替取字符..."
   - 寫出幹淨的代碼
   - 分析時空複雜度

2. **自然過度到 Kernel 連結** (深度面試)
   - "這個模式在 Linux kernel 中也很常見..."
   - 舉例: "Merge sort 中的 merge 步驟使用相同邏輯"
   - "DMA scatter-gather 也是交替合併多個來源"

3. **展示系統思維** (senior 面試)
   - 討論邊界情況和錯誤處理
   - 提出記憶體安全和並發問題
   - 說明實務中的最佳化考量

4. **回答深入追問** (technical leadership)
   - 比較不同的實現方式
   - 討論 API 設計權衡
   - 分析效能特性和限制

### 4.3 加分表述

| 加分點 | 表述方式 |
|------|---------|
| **LeetCode + Kernel** | "雙指標合併不僅在 LeetCode 中常見，Linux kernel 的 `lib/list_sort.c` 中的 merge 步驟使用相同的思路..." |
| **實務考量** | "在實現時，我會考慮記憶體安全、邊界檢查和效能最佳化，就像 kernel 代碼一樣..." |
| **細節展示** | "就像 DMA scatter-gather 那樣，需要處理多個來源、邊界條件和資源清理..." |
| **全局思維** | "這個演算法在不同層級都適用：算法課、系統代碼、硬體驅動..." |

### 4.4 避免的陷阱

- 僅停留在 "O(m+n)" 複雜度分析，不深入實務考量
- 聲稱對 kernel 很熟悉但舉例不當確
- 忽視邊界情況和錯誤處理
- 無法連結不同層級的概念 (算法 → 系統 → 硬體)

---

## 5. Sources

### Linux Kernel 源碼

- **Merge Sort Implementation**
  https://github.com/torvalds/linux/blob/master/lib/list_sort.c
  https://github.com/torvalds/linux/blob/master/lib/sort.c

- **String Functions**
  https://github.com/torvalds/linux/blob/master/lib/string.c

- **DMA Mapping & Scatter-Gather**
  https://github.com/torvalds/linux/blob/master/drivers/base/dma-mapping.c
  https://github.com/torvalds/linux/blob/master/include/linux/dma-mapping.h

- **Network Packet Reassembly**
  https://github.com/torvalds/linux/blob/master/net/ipv4/ip_fragment.c
  https://github.com/torvalds/linux/blob/master/net/ipv6/reassembly.c

- **Pipe Splicing**
  https://github.com/torvalds/linux/blob/master/fs/splice.c

- **Kernel Tracing Ring Buffer**
  https://github.com/torvalds/linux/blob/master/kernel/trace/trace.c

### Kernel 文檔

- **Kernel Doc Format**
  https://www.kernel.org/doc/html/latest/doc-guide/kernel-doc.html

- **Memory Management**
  https://www.kernel.org/doc/html/latest/core-api/memory-allocation.html

- **Synchronization Primitives**
  https://www.kernel.org/doc/html/latest/core-api/index.html

---

**最後更新**: 2026-04-01
**難度**: Easy
**核心技能**: 雙指標、字串操作、系統級思維
