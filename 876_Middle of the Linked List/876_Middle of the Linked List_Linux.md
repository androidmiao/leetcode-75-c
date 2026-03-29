# LeetCode 876 - Middle of the Linked List (Linux 核心知識對照)

## 問題概述

給定一個單向鏈表，找到其中點節點。若鏈表有偶數個節點，返回第二個中間節點。使用快慢指標（龜兔賽跑）技術，快指標每次跳 2 步，慢指標每次跳 1 步。

**LeetCode 解法複雜度:**
- 時間複雜度：O(n)
- 空間複雜度：O(1)

---

## 1. Linux 相關原始碼位置

### 核心鏈表實現
- **`include/linux/list.h`** — Linux 核心的雙向循環鏈表實現（`list_head` 結構）
  - 提供通用的鏈表操作巨集與內聯函數
  - 不同於 LeetCode 的單向鏈表設計

- **`lib/list_sort.c`** — 核心鏈表排序實現
  - 使用底層合併排序（bottom-up merge sort）
  - 無需尋找中點，改用 pending list 機制
  - 展示了核心工程師如何在空間受限時處理鏈表分割

- **`include/linux/hash.h`** 與 **`lib/hashtable.c`** — 雜湊表與鏈表結合
  - 鏈表在核心中廣泛用於衝突解決

- **`kernel/locking/` 相關檔案** — 同步原始碼
  - 鏈表與鎖機制的交互（影響遍歷效能）

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 資料結構差異

| 面向 | LeetCode 876 | Linux 核心 |
|------|--------------|----------|
| **鏈表型態** | 單向鏈表 | 雙向循環鏈表 (`list_head`) |
| **節點定義** | 包含資料的結構 | 嵌入式 `list_head`（與資料分離） |
| **查找中點** | 快慢指標直接找 | `list_sort.c` 用 pending list，不需尋找中點 |
| **指標方向** | `next` 單向 | `prev` / `next` 雙向 |
| **終止條件** | `NULL` 指標 | 迴圈回到 sentinel 頭 |

### 2.2 尋找中點的技術對比

**LeetCode 方式（快慢指標）：**
```c
// 龜兔賽跑法
struct ListNode *slow = head, *fast = head;
while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
}
return slow;  // slow 指向中點
```

**Linux 核心 `list_sort.c` 的做法（不尋找中點）：**
```c
// 底層合併排序，透過 pending list 累積已排序的塊
// 避免中點計算，改用位合併（bit-level merging）
for (bits = 0; k; bits++) {
    if (k & 1)
        pending[bits] = list;
    k >>= 1;
}
```
- **優勢**：避免第一次遍歷尋找長度或中點，直接進行合併
- **代價**：複雜度更高，但核心工程師優先考慮遍歷次數

### 2.3 快慢指標在核心中的應用

雖然核心鏈表排序不直接使用快慢指標找中點，但該技術在 **環檢測** 中被廣泛使用：

- **`kernel/sched/core.c`** 與調度器 — 檢測任務鏈表中的異常環
- **Rust for Linux** — 內存安全驗證時會用到環檢測
- **除錯工具** — `kdb` 與 `gdb` 外掛用於檢測鍊表腐敗

---

## 3. 主管 Code Review 角度考題

### 3.1 基礎設計問題

**Q: 為什麼 Linux 核心選擇雙向循環鏈表而不是單向鏈表？**

A:
- **刪除操作**：單向需要前驅指標，雙向直接取得
- **雙向遍歷**：核心需要前後遍歷（如任務隊列、待決鎖）
- **Sentinel 設計**：循環頭簡化邊界檢查，無須特殊 `NULL` 判斷
- **快取局部性**：雙向指標在現代 CPU 上效能更穩定

**Q: LeetCode 為什麼使用單向鏈表？**

A:
- 算法教學簡潔性
- 記憶體開銷最小
- 強調指標遍歷理解

### 3.2 效能與快取考量

**Q: 快慢指標遍歷對 CPU 快取的影響？**

A:
- **快指標**：跳躍訪問，快取命中率低（stride 為 2）
- **慢指標**：順序訪問，快取命中率高
- **實際場景**：在 L1 快取行寬度（通常 64B）內，單向鏈表節點稀疏分佈
- **核心優化**：使用 `likely()` / `unlikely()` 分支預測，但鏈表遍歷難以預測

### 3.3 邊界情況與一致性

**Q: 偶數節點鏈表中點的定義為何是"第二個中間節點"？**

A:
- LeetCode 約定：`[1,2,3,4]` → 返回節點 `3`（而非 `2`）
- **核心視角**：決定遍歷停止條件
  - `fast->next` 為 `NULL` → slow 停在第二中點
  - `fast` 為 `NULL` → slow 停在第一中點
- **一致性**：需明確文件化，避免不同模組產生衝突

**Q: 若鏈表有環，快慢指標會怎樣？**

A:
- 快指標最終追上慢指標（Floyd's cycle detection）
- LeetCode 假設無環；核心需防禦性編程
- 核心調試時會加入超時檢查，防止無限迴圈

### 3.4 `lib/list_sort.c` 設計深度

**Q: `list_sort.c` 為什麼不直接用快慢指標分割鏈表？**

A:
```c
/* list_sort.c 使用位級合併策略 */
// 原因：
// 1. 僅需一次遍歷（尋找中點需額外遍歷）
// 2. 利用二進位遞增合併（自然對應合併排序的 levels）
// 3. Pending list 累積已排序段，無須明確尋找中點
// 4. 空間複雜度 O(1)，時間仍為 O(n log n)
```

**效能差異：**
- 快慢指標法：需遍歷 2 次（查長度或找中點 + 排序），或遞迴（額外棧）
- `list_sort.c`：單次遍歷，位操作開銷小

### 3.5 若在核心中實現找中點

**Q: 在 Linux 核心中實現 LeetCode 876 風格的快慢指標函數，需注意什麼？**

A:
```c
/* 核心友善的鏈表中點查找 */
struct list_head *list_find_middle(struct list_head *head) {
    struct list_head *slow = head, *fast = head;

    /* 防守性檢查 */
    if (unlikely(!head || list_empty(head)))
        return NULL;

    /* 快慢指標 */
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  /* Sentinel 設計下安全 */
}
```

**核心工程師的審視重點：**
1. **NULL 檢查 vs. Sentinel** — `list_empty()` 巨集 vs. 直接指標檢查
2. **迴圈終止條件** — `fast != head` 而非 `fast->next`（防止雙向環的過度遍歷）
3. **預期用途** — 若只用於排序，建議採用 `list_sort.c` 方式
4. **鎖定策略** — 遍歷期間是否需要 RCU 或自旋鎖保護
5. **快取效應** — 在高並發下，快慢指標造成的額外遍歷是否可接受

---

## 4. 面試加分總結

### 4.1 展示核心知識的要點

- **雙向 vs. 單向**：解釋核心選擇雙向的原因（刪除、遍歷、Sentinel 設計）
- **Sentinel 設計的妙處**：無須 `NULL` 檢查邊界，減少分支預測失敗
- **`list_sort.c` 的精妙**：位級合併避免了顯式中點計算，展示高級工程思維
- **環檢測應用**：提及 Floyd 算法在核心調試中的使用
- **快取局部性**：討論指標跳躍（快指標）vs. 順序訪問（慢指標）的 CPU 快取影響
- **一致性與文件化**：強調邊界情況定義（偶數節點返回第二中點）

### 4.2 回答模板

當被問及「如何在核心中使用該算法」時，迴應：

> "LeetCode 的快慢指標在邏輯上是 O(n) 單遍的優雅方案。但在 Linux 核心，我們很少直接用於分割，反而在 `list_sort.c` 看到位級合併策略——為的是避免多遍遍歷。不過在環檢測、記憶體洩漏追蹤時，Floyd 算法（快慢指標的兄弟）是標配。核心工程強調減少遍歷次數與快取效應，這點比算法優雅性更重要。"

### 4.3 實戰應用場景

- **鎖與並發**：若鏈表在 RCU 保護下，快慢指標的額外遍歷可能加重競爭
- **嵌入式系統**：記憶體極緊張時，無須找中點的排序方式（如 `list_sort.c`）更優
- **即時系統**：快慢指標的頻繁指標解引用可能違反確定性需求

---

## 5. LeetCode 題解對應

### C 語言實現（單向鏈表）

```c
/**
 * Definition for singly-linked list node.
 */
struct ListNode {
    int val;
    struct ListNode *next;
};

/**
 * 快慢指標法找鏈表中點
 * 時間: O(n), 空間: O(1)
 */
struct ListNode *middleOfLinkedList(struct ListNode *head) {
    struct ListNode *slow = head;
    struct ListNode *fast = head;

    /* 快指標每次跳 2 步，慢指標跳 1 步
     * 當快指標到達末尾時，慢指標在中點
     */
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  /* 偶數節點時返回第二中點 */
}
```

---

## Sources

1. **Linux Kernel Source (GitHub)**
   - [`include/linux/list.h`](https://github.com/torvalds/linux/blob/master/include/linux/list.h) — 雙向循環鏈表 API
   - [`lib/list_sort.c`](https://github.com/torvalds/linux/blob/master/lib/list_sort.c) — O(1) 空間合併排序

2. **Floyd's Cycle Detection**
   - Wikipedia: [Cycle Detection](https://en.wikipedia.org/wiki/Cycle_detection)
   - 核心應用：記憶體洩漏檢測、任務隊列異常檢查

3. **LeetCode Problem 876**
   - [Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/)

4. **Kernel Documentation**
   - [Linux Kernel Documentation: Data Structures](https://www.kernel.org/doc/html/latest/kernel-hacking/index.html)

---

**修訂日期**: 2026-03-29
**語言**: Traditional Chinese (繁體中文) with English identifiers
**面向**: 後端工程師與系統核心開發者的 LeetCode 算法深化
