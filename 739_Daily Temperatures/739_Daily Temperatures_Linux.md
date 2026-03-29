# LeetCode 739 Daily Temperatures — Linux 核心連結分析

## 問題概述

**LeetCode 739: Daily Temperatures**

給定一個整數陣列 `temperatures`，針對每個溫度找出其後更高溫度出現的天數距離（待查日期距離）。

**演算法**: Monotonic Decreasing Stack（單調遞減棧）

```c
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    int* result = (int*)calloc(temperaturesSize, sizeof(int));
    int* stack = (int*)malloc(temperaturesSize * sizeof(int));
    int top = -1;
    for (int i = 0; i < temperaturesSize; i++) {
        while (top >= 0 && temperatures[stack[top]] < temperatures[i]) {
            int prev_index = stack[top--];
            result[prev_index] = i - prev_index;
        }
        stack[++top] = i;
    }
    free(stack);
    *returnSize = temperaturesSize;
    return result;
}
```

---

## 1. Linux 相關原始碼位置

### 1.1 BPF 驗證器中的棧狀態追蹤 — `kernel/bpf/verifier.c`

**相關性**: **最高 ★★★★★**

Linux BPF 驗證器使用棧結構來追蹤暫存器和棧槽的狀態。該模式與單調棧共享核心概念：**狀態管理與向後回溯**。

**相似之處**:
- 維護一個狀態棧，追蹤每個暫存器和棧槽的 `bpf_reg_state`
- 當遇到新狀態時，逐步回溯及更新先前狀態的「活躍性」(liveness)
- 使用父指標指向前一個狀態，類似單調棧中的向前遍歷

**程式碼模式對比**:

| 特性 | Daily Temperatures | BPF Verifier |
|------|-------------------|--------------|
| 棧元素類型 | 索引 (int) | 狀態指標 (bpf_reg_state*) |
| 向後比較 | `temperatures[stack[top]]` | `bpf_state->parent` |
| 彈出條件 | `temperatures[stack[top]] < temperatures[i]` | 狀態剪枝條件 |
| 記錄結果 | `result[prev_index] = i - prev_index` | 更新活躍性資訊 |

**核心程式碼** (`include/linux/bpf_verifier.h`):
```c
struct bpf_reg_state {
    enum bpf_reg_type type;
    enum bpf_value_type value_from_signed;
    s64 imm;
    u64 umin_value, umax_value;
    s64 smin_value, smax_value;
    struct bpf_reg_state *parent;  // 類似 "向前回溯"
    // ... 更多追蹤欄位
};
```

**應用場景**: 當 BPF 程式執行時，驗證器需要在每個分支點（如條件跳轉）追蹤暫存器的狀態變化，並在返回時合併不同路徑的狀態。這類似於 Daily Temperatures 中對「已看過的溫度」的記錄與回溯。

**來源**: [kernel/bpf/verifier.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/kernel/bpf/verifier.c)

---

### 1.2 記憶體回收棧深度管理 — `mm/vmscan.c`

**相關性**: **中高 ★★★★**

Linux 記憶體回收子系統在掃描及比較 LRU (Least Recently Used) 頁面時產生深層呼叫棧。當 dirty page 回寫時，棧深度可能達到危險水位。

**相似之處**:
- 迭代掃描頁面列表，對每個頁面進行「比較」(age、access pattern、writeback state)
- 維持棧狀態以防棧溢出
- 需要在回收週期中記錄「已處理頁面」狀態

**棧管理問題**:

在 `mm/vmscan.c` 中，當直接回收 (direct reclaim) 觸發 page writeback 時：
```c
// 簡化示例：掃描 LRU 列表
while (scan_control->nr_to_scan > 0) {
    struct page *page = lru_to_page(lru);
    if (should_writeback(page)) {
        page_writeback(page);  // 可能導致深層呼叫棧
    }
    // ... 比較邏輯
}
```

**核心差異**:
- `mm/vmscan.c` 必須避免無限遞迴，透過設置 `may_writepage = false` 來防止棧溢出
- Daily Temperatures 使用顯式棧（array-based stack），而 vmscan 使用隱式呼叫棧

**最佳實踐**: Kernel 開發者學會監控棧深度，採用 `CONFIG_DEBUG_STACK_USAGE` 來偵測潛在溢出。

**來源**: [linux/mm/vmscan.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/mm/vmscan.c)

---

### 1.3 核心排序演算法 — `lib/sort.c`

**相關性**: **中 ★★★**

Linux kernel 的 `lib/sort.c` 實現 **heapsort**（非遞迴）以避免棧溢出。此設計哲學與 Daily Temperatures 的單調棧方法異曲同工：都強調 **O(1) 額外空間** 與 **棧安全**。

**相似之處**:
- 都選擇迭代而非遞迴方法
- 都使用額外陣列來追蹤狀態而非依賴呼叫棧
- 都著重於 **時間複雜度** (O(n log n)) 與 **空間效率**

**設計對比**:

| 方面 | Daily Temperatures | lib/sort.c |
|------|-------------------|-----------|
| 主要容器 | 棧陣列 (stack[]) | Heap 陣列 |
| 迭代策略 | 向前掃描，向後比較 | 堆化 (heapify) |
| 棧溢出風險 | 無（陣列棧） | 無（非遞迴） |
| 時間複雜度 | O(n) | O(n log n) |

**來源**: [linux/lib/sort.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/lib/sort.c)

---

### 1.4 雙向連結串列的 LIFO 模式 — `include/linux/list.h`

**相關性**: **中 ★★★**

Linux 的內核連結串列實現提供 `list_add()` 函式（在頭部插入），可用於實現棧的 LIFO (Last-In-First-Out) 語義。

**相似之處**:
- `list_add(new, head)` 等同於「push 到棧頂」
- `list_del()` 等同於「pop 從棧頂」
- 使用 container_of() 巨集可從 list_head 恢復完整資料結構

**程式碼範例**:
```c
// Linux kernel list-based stack
struct stack_node {
    int value;
    struct list_head node;
};

struct list_head stack_head;
INIT_LIST_HEAD(&stack_head);

// Push: O(1)
list_add(&new_node->node, &stack_head);

// Pop: O(1)
struct stack_node *top = list_first_entry(&stack_head, struct stack_node, node);
list_del(&top->node);
```

**優點與缺點對比**:

| 特性 | Array-based Stack (LeetCode) | List-based Stack (Kernel) |
|------|----------------------------|------------------------|
| 時間複雜度 | O(1) push/pop | O(1) push/pop |
| 空間預分配 | 需要 | 動態 |
| 快取局域性 | 高 | 低 (指標追蹤) |
| 核心合適度 | 低 (固定大小) | 高 (動態) |

**來源**: [linux/include/linux/list.h at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/include/linux/list.h)

---

### 1.5 網路封包處理的隊列模式 — `net/core/skbuff.c`

**相關性**: **低-中 ★★**

Linux 網路棧使用 socket buffer (sk_buff) 隊列來管理網路封包。雖然主要是 FIFO 隊列而非 LIFO 棧，但其狀態管理與向後遍歷 (backtrack) 的概念與單調棧相關。

**相似之處**:
- 必須追蹤每個 sk_buff 的狀態（layer, checksum, references）
- 在特定條件下從隊列移除舊元素（類似棧的彈出）
- 需要高效的 O(1) 或 O(log n) 查找與移除

**程式碼片段** (`net/core/skbuff.c` 的概念):
```c
// 簡化：追蹤 sk_buff 狀態
struct sk_buff *skb = alloc_skb(len, GFP_ATOMIC);
skb->next = queue->head;  // 入隊列
queue->head = skb;

// 出隊列（類似棧的 pop）
struct sk_buff *dequeued = queue->head;
queue->head = dequeued->next;
```

**區別**: sk_buff 隊列通常是 FIFO，用於公平處理；而單調棧是 LIFO，用於快速回溯與比較。

**來源**: [networking/skbuff.rst — The Linux Kernel documentation](https://docs.kernel.org/networking/skbuff.html)

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 資料結構選擇

#### LeetCode 實作 (Daily Temperatures)
```c
int* stack = (int*)malloc(temperaturesSize * sizeof(int));
int top = -1;
```

**特點**:
- 陣列式棧，預分配固定大小
- 簡單的 `top` 指標追蹤
- 不需要動態調整

#### Kernel 實作示例 (BPF Verifier)
```c
struct bpf_reg_state {
    // ... fields ...
    struct bpf_reg_state *parent;
};
```

**特點**:
- 鏈式棧，利用指標回溯
- 狀態結構複雜，包含多個追蹤欄位
- 可動態擴展，適合未知規模

### 2.2 迭代與比較策略

#### LeetCode: 單向掃描 + 向後比較
```c
for (int i = 0; i < temperaturesSize; i++) {
    while (top >= 0 && temperatures[stack[top]] < temperatures[i]) {
        // 彈出並記錄
    }
    stack[++top] = i;
}
```

**優勢**:
- 單次遍歷，時間 O(n)
- 快取友善 (sequential access)

#### Kernel: 多層次狀態追蹤 (BPF Verifier)
```c
// 偽代碼：棧深度追蹤
void check_state_liveness(struct bpf_reg_state *state) {
    while (state->parent != NULL) {
        update_liveness(state);
        state = state->parent;  // 向後回溯
    }
}
```

**優勢**:
- 支援複雜狀態轉換
- 允許多次回溯與修正
- 更適合驗證邏輯

### 2.3 記憶體配置與釋放

#### LeetCode
```c
int* stack = (int*)malloc(temperaturesSize * sizeof(int));
// ... 使用 ...
free(stack);
```

**特點**:
- 明確的分配與釋放
- 單一擁有者
- 沒有引用計數

#### Kernel (如 BPF)
```c
struct bpf_verifier_state *state = kzalloc(sizeof(*state), GFP_KERNEL);
// ... 使用，可能被多個執行路徑參考 ...
free_verifier_state(state);  // 可能延遲釋放
```

**特點**:
- 複雜的生命週期管理
- 可能多個引用 (refcount)
- 需要 RCU (Read-Copy-Update) 或其他同步機制

### 2.4 棧溢出防護

#### LeetCode
- 預分配大小 = 陣列大小，無法溢出
- 時間複雜度有界 → 空間複雜度有界

#### Kernel (vmscan.c 的例子)
```c
if (page_writeback_may_cause_stack_overflow())
    avoid_writepage = true;
```

**防護機制**:
- `CONFIG_DEBUG_STACK_USAGE` 監控
- 明確檢查棧深度
- 必要時禁用某些操作

---

## 3. 主管 Code Review 角度考題

### Q1: 棧溢出風險與邊界檢查
**評論**: 「為什麼選擇陣列棧而不是動態擴展？」

**預期答案**:
- 陣列棧大小 = 輸入陣列大小，永不溢出
- 動態擴展需要 realloc()，增加複雜性與失敗風險
- Daily Temperatures 的單調棧性質保證：每個元素最多被推入一次、彈出一次 → 最大棧深度 = n

**進一步追問**:
- 「如果輸入陣列非常大（例如 10^7），是否存在記憶體問題？」
  - 答: 不會，因為棧與結果陣列都是 O(n) 空間，輸入本身也是 O(n)
- 「如何驗證棧絕不會溢出？」
  - 答: 使用不變式 (invariant) 證明：輸入長度 n，棧最多儲存 n 個元素

**核心檢查點**: **記憶體安全** ✓

---

### Q2: 時間複雜度的細節
**評論**: 「這個演算法宣稱 O(n) 時間複雜度。請解釋為什麼內層 while 迴圈不會導致 O(n²)。」

**預期答案**:
每個元素在整個執行過程中：
1. 被推入棧一次
2. 被彈出棧一次（最多）
→ 總操作數 = 2n = O(n)

**進一步追問**:
- 「kernel 中的 BPF 驗證器是否也有類似的『攤銷分析』？」
  - 答: 是，但更複雜。驗證器可能需要探索多條執行路徑，導致摊销複雜度更高
- 「如果狀態空間爆炸，驗證器如何應對？」
  - 答: 使用「狀態剪枝」(state pruning)，跳過等價狀態，減少搜尋空間

**核心檢查點**: **複雜度分析嚴謹性** ✓

---

### Q3: 快取局域性與性能
**評論**: 「這個實作使用順序陣列訪問。請比較與kernel中的鏈式棧設計的快取行為。」

**預期答案**:

| 方面 | Array-based (LeetCode) | Pointer-based (Kernel) |
|------|----------------------|----------------------|
| 快取命中率 | 高 (連續記憶體) | 低 (指標間接) |
| 預取效率 | 高 (CPU 能預測存取) | 低 (指標追蹤困難) |
| L1/L2 快取行數 | 低 (緊湊) | 高 (結構分散) |

**進一步追問**:
- 「能否用效能指標 (perf) 驗證？」
  - 答: 是，可以測量 L1/L2 快取未命中率、CPU 週期 (cycles per operation)
- 「kernel 為何仍選擇鏈式設計？」
  - 答: 因為 kernel 需要支援動態大小、複雜狀態、多執行路徑等，陣列不適合；犧牲快取性能以換取靈活性

**核心檢查點**: **性能感知** ✓

---

### Q4: 邊界條件與初始化
**評論**: 「初始化時 `top = -1` 且結果陣列 `calloc`。為什麼不用 `malloc` + 顯式清零？」

**預期答案**:
- `calloc` = 分配 + 清零，保證 `result[i] = 0`（表示「未找到更高溫度」）
- `malloc` 後需要手動初始化，容易出錯，且多一步操作
- 對於使用者來說，`calloc` 的語義更清晰：「我要一個初始化為零的陣列」

**進一步追問**:
- 「kernel 中是否有類似的習慣？」
  - 答: 是，kernel 偏好 `kzalloc()` (分配 + 清零) 或 `kmalloc() + memset()`
- 「是否應該檢查 malloc/calloc 的返回值？」
  - 答: **是的！** 實際 code review 應該指出遺漏的 NULL 檢查

```c
int* result = (int*)calloc(temperaturesSize, sizeof(int));
if (result == NULL) {
    // 錯誤處理
    return NULL;
}
```

**核心檢查點**: **防禦性程式設計** ✗ (發現缺陷)

---

### Q5: 棧與結果指標的生命週期
**評論**: 「函式返回時，棧陣列被 free，但結果陣列被返回。如何確保呼叫者能正確管理記憶體？」

**預期答案**:
- 這是 API 設計的問題
- 函式傳出的指標責任轉移給呼叫者
- 呼叫者**必須**稍後 free 返回的指標
- 應在註解或文件中清晰說明所有權轉移

**改進版本**:
```c
/**
 * Find the next warmer day for each day.
 *
 * Returns a dynamically allocated array.
 * Caller MUST free the returned pointer.
 *
 * @param temperatures: input array
 * @param temperaturesSize: size of input
 * @param returnSize: output parameter, set to temperaturesSize
 * @return: pointer to result array, or NULL on allocation failure
 */
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize);
```

**Kernel 參考**:
Kernel 通常使用：
1. 預分配的緩衝區 (caller 提供)
2. 傳入指標參數，寫入結果
3. 明確的錯誤碼 (errno)

例 (BPF Verifier):
```c
int check_prog(...) {
    // 不傳回新指標，修改傳入的 env 結構
    return error_code;
}
```

**核心檢查點**: **API 安全性與文件** ✗ (缺乏文件)

---

### Q6: 線程安全與並發
**評論**: 「這個函式是線程不安全的（使用靜態陣列）。Kernel 中如何處理？」

**預期答案**:
實際上本題目的函式不使用靜態陣列，是線程安全的。但審視者可能會問：
- 「如果輸入陣列被多個線程修改怎麼辦？」
  - 答: 呼叫者責任 (caller responsibility)
  - 輸入應視為 read-only
  - 如需同步，由上層應用負責

**Kernel 對標**:
Kernel 大量使用：
- `spinlock`
- `mutex`
- `RCU` (Read-Copy-Update)
- 原子操作 (atomic_t)

例如 BPF Verifier 使用 `struct bpf_verifier_env` 與鎖保護狀態：
```c
struct bpf_verifier_env {
    // ...
    struct mutex mutex;  // 保護 verifier 狀態
};
```

**核心檢查點**: **並發性考量** ✓ (在單線程假設下)

---

### Q7: 演算法正確性證明
**評論**: 「請用循環不變式 (loop invariant) 驗證演算法的正確性。」

**預期答案**:

**循環不變式** (在第 i 次迭代開始):
1. 棧中存儲索引 j₁, j₂, ..., jₖ，滿足 `temperatures[j₁] > temperatures[j₂] > ... > temperatures[jₖ]`（單調遞減）
2. 對於所有已出棧的索引 m，`result[m]` 已被設為 `i - m` (其後更高溫度的距離)
3. 棧中的索引都 < i

**初始化**:
- 棧為空，滿足單調性
- result 全為 0

**遞推**:
- 若 `temperatures[i] <= temperatures[top]`：將 i push，保持單調性
- 若 `temperatures[i] > temperatures[top]`：
  - pop top，設 `result[top] = i - top`（top 的答案）
  - 重複直至恢復單調性，再 push i

**終止**:
- 所有未設結果的索引在棧中，應保持為 0（表示無更高溫度）

**Kernel 中的相似做法**:
BPF 驗證器有類似的「狀態不變式」證明：
- 每個執行路徑的狀態都被正確檢查
- 所有可達狀態都被驗證

**核心檢查點**: **形式驗證與正確性** ✓

---

## 4. 面試加分總結

### 核心亮點

1. **棧設計理解**
   - 認識 array-based vs. pointer-based 棧的權衡
   - 了解單調棧性質如何保證複雜度
   - **說法**: 「單調棧保證每個元素最多進棧和出棧各一次，因此儘管有內層 while 迴圈，總時間仍是 O(n)」

2. **Kernel 知識遷移**
   - 提到 BPF Verifier 的狀態棧追蹤
   - 比較 vmscan.c 的棧深度管理問題
   - **說法**: 「kernel 中的 BPF 驗證器也維護一個狀態棧，在探索不同執行路徑時需要向後回溯狀態，類似本題的單調棧思想」

3. **性能與記憶體安全**
   - 強調 O(1) 額外空間、O(n) 時間的優勢
   - 指出棧溢出防護（預分配確保上界）
   - 快取局域性優勢
   - **說法**: 「本實作使用陣列棧而非動態結構，確保不會溢出，同時擁有良好的快取局域性」

4. **Code Review 視角**
   - 指出缺少的 NULL 檢查
   - 明確所有權轉移
   - 循環不變式驗證
   - **說法**: 「在實際 code review 中，應新增對 calloc 返回值的檢查，並在函式註解中明確指出呼叫者必須釋放返回的指標」

5. **演算法設計思想**
   - 解釋為什麼 heapsort 被用於 kernel 而非 quicksort（棧安全）
   - Kernel 對棧深度的重視
   - **說法**: 「kernel 中 lib/sort.c 採用 heapsort 而非 quicksort，正是因為要避免遞迴導致的棧溢出問題，這與我們在 LeetCode 題目中對棧效率的考量同源」

### 面試中的表現方向

#### 如果面試官問「如何優化這個演算法？」
- **回答**: 「從演算法角度，O(n) 時間和 O(n) 空間已經是最優。但在 kernel 場景下，可以考慮：
  1. 使用鏈式棧（避免預分配大小限制）
  2. 添加 RCU 同步（如果有並發讀者）
  3. 使用 vmalloc() 而非 malloc()（支援大陣列）」

#### 如果面試官問「為什麼要用單調棧？」
- **回答**: 「單調棧利用『往後一旦出現更大的值，就不會再關心之前的較小值』的特性。在 kernel 中，類似的思想用於 BPF 驗證器的狀態剪枝—不需要追蹤等價的舊狀態。」

#### 如果面試官問「這和 kernel 有什麼關係？」
- **回答**: 「我學到的核心啟示是：
  1. **棧安全**: Kernel 對棧深度極其敏感（中斷上下文、核心棧只有幾 KB）
  2. **迭代勝於遞迴**: Kernel 偏好迭代實現以控制棧使用
  3. **複雜度平衡**: 有時 LeetCode 優化複雜度，Kernel 優化棧安全性與快取局域性
  4. **狀態管理**: 無論是溫度比較還是驗證器，都在管理『先前狀態與當前狀態的比較』」

---

## 5. Sources

### 核心參考資源

#### BPF Verifier (最直接相關)
- [kernel/bpf/verifier.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/kernel/bpf/verifier.c)
- [include/linux/bpf_verifier.h at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/include/linux/bpf_verifier.h)
- [eBPF Verifier Documentation — The Linux Kernel documentation](https://docs.kernel.org/bpf/verifier.html)
- [A look inside the BPF verifier [LWN.net]](https://lwn.net/Articles/982077/)

#### 記憶體管理 (棧深度問題)
- [linux/mm/vmscan.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/mm/vmscan.c)
- [Reclaim — The Linux Kernel documentation](https://docs.kernel.org/driver-api/cxl/allocation/reclaim.html)
- [Concepts overview — The Linux Kernel documentation](https://docs.kernel.org/admin-guide/mm/concepts.html)

#### 排序與演算法
- [linux/lib/sort.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/lib/sort.c)
- [linux/lib/list_sort.c at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/lib/list_sort.c)

#### 連結串列實現
- [linux/include/linux/list.h at master · torvalds/linux](https://github.com/torvalds/linux/blob/master/include/linux/list.h)
- [Linked Lists in Linux — The Linux Kernel documentation](https://docs.kernel.org/core-api/list.html)
- [FAQ/LinkedLists — Linux Kernel Newbies](https://kernelnewbies.org/FAQ/LinkedLists)

#### 網路處理 (Queue 機制)
- [struct sk_buff — The Linux Kernel documentation](https://docs.kernel.org/networking/skbuff.html)
- [Scaling in the Linux Networking Stack — The Linux Kernel documentation](https://www.kernel.org/doc/html/latest/networking/scaling.html)

---

## 結語

LeetCode 739 Daily Temperatures 的單調棧解法看似簡單，但其背後的設計原則與 Linux kernel 中的多個子系統相通：

1. **狀態追蹤與向後回溯**: BPF Verifier 的狀態棧
2. **棧溢出防護**: mm/vmscan.c 的棧深度管理
3. **非遞迴迭代設計**: lib/sort.c 的 heapsort
4. **高效的資料結構**: include/linux/list.h 的 LIFO 模式

在面試中，將算法思想與 kernel 實踐相連，能展現深度的系統思維，也是從「做對題目」進階到「理解系統」的關鍵。
