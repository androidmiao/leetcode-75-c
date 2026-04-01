# LeetCode 2130：最大對稱和 × Linux 核心

## 演算法概述

本題解使用三步策略：
1. **快慢指標定位中點** — 找到串列中點位置
2. **反轉第二半部分** — 原地反轉後半段
3. **配對求和比較** — 逐個計算孿生對的和並求最大值

```c
int pairSum(struct ListNode* head) {
    struct ListNode *slow = head, *fast = head->next->next;

    // 快慢指標找中點
    while (fast && fast->next)
        slow = slow->next, fast = fast->next->next;

    // 反轉第二半
    struct ListNode *prev = NULL, *curr = slow->next;
    while (curr) {
        struct ListNode *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    // 配對計算最大和
    int maxSum = 0;
    struct ListNode *first = head, *second = prev;
    while (second) {
        int sum = first->val + second->val;
        maxSum = (sum > maxSum) ? sum : maxSum;
        first = first->next;
        second = second->next;
    }

    return maxSum;
}
```

---

## Linux 相關原始碼位置

Linux 核心的串列實作遍佈各處，與此題高度相關的核心代碼如下：

### 1. **include/linux/list.h** — 核心迴圈雙向連結串列
- 定義 `struct list_head` — 嵌入式連結串列節點
- `list_add()`, `list_del()`, `list_for_each()` 等基礎 API
- 支援雙向走訪，但核心仍常需實作自定義走訪邏輯

### 2. **include/linux/llist.h** — 無鎖單向連結串列
最接近 LeetCode ListNode 的核心資料結構：
```c
struct llist_node {
    struct llist_node *next;
};

struct llist_head {
    struct llist_node *first;
};
```
特點：單向、無自旋鎖、適合原子操作場景

### 3. **lib/llist.c** — 單向串列操作實作
包含 `llist_reverse_order()` 函式，執行完全等同於本題 `reverseList()` 的操作：
```c
struct llist_node *llist_reverse_order(struct llist_node *head)
{
    struct llist_node *new_head = NULL;

    while (head) {
        struct llist_node *tmp = head;
        head = head->next;
        tmp->next = new_head;
        new_head = tmp;
    }

    return new_head;
}
```

### 4. **lib/list_sort.c** — 合併排序與快慢指標
核心的 `list_sort()` 使用**計數分割法**而非快慢指標，但原理相同：
- 透過 `list_cut_position()` 在中點分割串列（類似快慢指標的目的）
- 遞迴合併排序，無需額外記憶體結構

### 5. **net/core/skbuff.c** — 網路套接字緩衝區串列
核心網路棧使用 `struct sk_buff_head` 管理封包隊列，涉及大量串列走訪和反轉操作：
- 在 QoS、流量管制、重組等場景需要對套接字緩衝區進行串列操作
- 必須考慮並發安全（RCU 保護、自旋鎖）

### 6. **kernel/sched/fair.c** — CFS 排程器走訪
雖然不直接涉及反轉，但展示核心中串列走訪的典型模式：
- 完全公平排程器 (CFS) 使用紅黑樹，但內部的就緒隊列邏輯涉及串列操作
- 示範了核心級別的性能敏感代碼如何操作動態資料結構

---

## Kernel vs. LeetCode 實作對照

### 資料結構設計維度

| 維度 | Linux 核心 | LeetCode 題目 |
|------|-----------|-------------|
| **串列型態** | 迴圈雙向連結串列（主要）、無鎖單向串列（專用） | 單向連結串列 |
| **嵌入模式** | 侵入式：`list_head` 嵌入在使用者結構中 | 非侵入式：獨立 `ListNode` 物件 |
| **節點定義** | `struct list_head { prev, next }` | `struct ListNode { val, next }` |
| **記憶體管理** | 無動態分配（嵌入式）、RCU 保護 | C 語言手動管理 |

**核心設計哲學**：
- 核心採侵入式設計以**最小化記憶體開銷**（嵌入在現有結構中，無額外指標）
- LeetCode 採非侵入式便於教學，但浪費額外記憶體

### 反轉操作對照

**核心實作（lib/llist.c）**：
```c
struct llist_node *llist_reverse_order(struct llist_node *head)
{
    struct llist_node *new_head = NULL;
    while (head) {
        struct llist_node *tmp = head;
        head = head->next;
        tmp->next = new_head;
        new_head = tmp;
    }
    return new_head;
}
```

**LeetCode 實作**：
```c
struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode *prev = NULL, *curr = head;
    while (curr) {
        struct ListNode *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
```

**相同點**：
- 都是 O(n) 時間、O(1) 空間的原地反轉
- 都使用 `prev → curr → next` 三指標法則

**差異**：
- 核心版使用 `new_head` 而 LeetCode 用 `prev`，語意相同
- 核心版返回新頭部，LeetCode 返回反轉後的頭

### 中點尋找對照

| 方法 | 核心實作 | LeetCode 實作 | 優缺點 |
|------|---------|-------------|-------|
| **計數分割** | `list_cut_position()` 計數到 n/2 | N/A | 準確，但需遍歷兩次 |
| **快慢指標** | N/A | `fast=head->next->next; slow=head` | 一次遍歷，空間 O(1)，更優雅 |

核心的 `list_sort()` 採計數法，因為：
1. 已知長度或可預計算
2. 需要精確控制分割點以實現歸併排序
3. 雙向指標允許向前/向後移動

LeetCode 採快慢指標，因為：
1. 不需預計算長度
2. 單次遍歷更高效
3. 面試中更能展示對指標操控的理解

### 雙向 vs 單向連結串列

**核心的雙向設計優勢**：
- 可從尾端向前走訪，無需反轉也能訪問前驅節點
- `list_del()` 從任意節點 O(1) 刪除（知道前驅）
- 本題若採核心的 `list_head`，完全無需反轉操作

**題目強制單向原因**：
- 測試反轉能力（核心面試要點）
- 模擬真實單向串列場景（如網路封包處理的某些層級）

### 記憶體模型與並發安全

**核心考量**：
- **中斷上下文**：不能動態分配記憶體，故採侵入式設計
- **RCU 保護**：`rcu_read_lock()` 保護讀者不被寫者阻塞
- **自旋鎖**：多核場景下保護串列修改操作
- **無記憶體屏障**：某些操作需 `smp_wmb()`/`smp_rmb()`

**LeetCode 簡化**：
- 單執行緒環境
- 無動態分配考量
- 無並發保護

---

## 主管 Code Review 角度考題

一位熟悉核心代碼的資深工程師可能會提出以下 6 個問題：

### 1. 為什麼不用雙向連結串列直接從尾端往回走？

**問題目的**：測試對單向串列限制的理解，以及在實際系統中的設計取捨認識。

**預期答案方向**：
- 題目明確定義了 `ListNode` 只有 `next` 指標，無法回溯
- 如果允許修改節點結構，可以加 `prev` 指標變成雙向（如核心的 `list_head`），但這增加每個節點的記憶體開銷 8 字節（64 位系統）
- 核心在某些熱路徑（如 CFS 排程）因此採雙向，但在約束環境（中斷、緊張記憶體）採侵入式無鎖單向設計

**追問**：
- "如果串列有百萬個節點，加一個 `prev` 指標會增加多少記憶體？"
- "核心的 `llist.h` 為何特意採單向？"

### 2. 你的 `reverseList()` 在核心環境下需要什麼額外保護？

**問題目的**：測試並發安全意識，以及對核心保護機制（RCU、自旋鎖、記憶體屏障）的認識。

**預期答案方向**：
- LeetCode 代碼假設單執行緒，核心環境中修改串列必須持有鎖（如 `spin_lock()` 或 `rcu_read_lock()`）
- 若其他 CPU 在反轉進行中讀取串列，可能看到不一致的中間狀態
- 需要保證：
  1. 寫者互斥（自旋鎖）
  2. 讀者不被阻塞（RCU）或讀者已等待（記憶體屏障）
- 看 `lib/llist.c` 中的 `llist_add()` / `llist_del_all()`，它們無鎖但使用原子操作和記憶體屏障

**追問**：
- "什麼是 RCU？為什麼核心偏好 RCU 而非全局鎖？"
- "如果反轉期間有讀者訪問，會發生什麼？"
- "`smp_wmb()` 和 `smp_rmb()` 的區別是什麼？"

### 3. 你的反轉後沒有恢復原始串列，如果 Caller 之後還需要用原始順序怎麼辦？

**問題目的**：測試 API 設計思維，以及副作用意識。

**預期答案方向**：
- 目前的 `reverseList()` 永久修改串列，屬於破壞性操作（destructive）
- 若 Caller 需保留原始順序，有幾個策略：
  1. **複製反轉**：複製串列後反轉副本（額外 O(n) 空間）
  2. **備份中點**：在反轉前保存原始中點的指標和連接信息，反轉後恢復
  3. **不反轉只讀**：使用遞迴或棧在不修改串列下實現從尾部讀取
  4. **API 分離**：提供 `reverseListInPlace()` 和 `reverseListNonDestructive()` 兩個版本

- 核心通常遵循 "no free lunch" 原則：`list_sort()` 也修改串列順序，但提供明確的文件警告
- 題目由於 "找到最大孿生和" 的目標，反轉後串列本身無用，故破壞性反轉可接受

**追問**：
- "如何用遞迴在不反轉的情況下比較孿生和？時間空間複雜度如何？"
- "核心的 `list_sort()` 如何處理這個問題？"

### 4. `fast` 從 `head->next->next` 開始而不是 `head`，這是為什麼？如果串列只有 2 個節點會怎樣？

**問題目的**：測試邊界條件敏感性和指標邏輯的精確性。

**預期答案方向**：
- 初始化 `fast = head->next->next` 是為了讓 `slow` 和 `fast` 的起點保證 **slow 總是指向第一個要反轉的節點**
- 具體分析：
  - n=2: `head → A → B → NULL`
    - `fast = head->next->next = B->next = NULL`，迴圈不執行
    - `slow = head`，反轉部分為 `B` 單獨成串列
    - 配對：`(A,B)`，正確

  - n=4: `head → A → B → C → D → NULL`
    - 初始：`slow=head(A), fast=C`
    - 第一步：`slow=B, fast=NULL`（快指標走到尾）
    - 反轉部分為 `C → D`，配對：`(A,D), (B,C)`，正確

  - 若改為 `fast=head`，則 `slow` 會停在第 n/2-1 位置，反轉起點錯誤

- 若用 `fast = head; slow = head` 再檢查 `fast->next->next`，也能工作但邏輯更複雜

**追問**：
- "n=1 時的邊界如何處理？（題目保證 n >= 2）"
- "如何改寫讓 fast/slow 初始化更直觀？"

### 5. 能否用 XOR 連結串列來達到 O(1) 空間且不修改原始串列？

**問題目的**：測試進階資料結構知識和創意思維。

**預期答案方向**：
- XOR 連結串列是極端的空間優化：用一個指標儲存 `prev XOR next`，達到雙向走訪但僅 O(1) 額外指標
- 具體實現：
  ```c
  struct XORNode {
      int val;
      struct XORNode *npx;  // next XOR prev
  };

  #define XOR(a, b) ((struct XORNode*)((uintptr_t)(a) ^ (uintptr_t)(b)))
  ```

- 優勢：無需反轉，知道前驅就能向前或向後；無需額外空間
- 劣勢：
  1. 代碼複雜度高，易出 Bug
  2. 要精確追蹤 prev/curr 關係
  3. 無法隨意訪問中間節點（必須從兩端開始）
  4. 在現代 CPU 上因指標追蹤成本反而慢

- **現實考量**：核心罕見使用 XOR 連結串列（太複雜），只在教科書演習中出現

**追問**：
- "XOR 連結串列的 prev/curr/next 遍歷邏輯如何實現？"
- "如何驗證 XOR 連結串列的正確性？"

### 6. 如果這個串列是用核心的 `llist` 實作的，你的 `reverseList` 和 `llist_reverse_order()` 有什麼差異？

**問題目的**：測試核心 API 熟悉度，以及在實際系統中應用演算法知識的能力。

**預期答案方向**：
- 直接比較（見下表）：

| 方面 | LeetCode `reverseList()` | 核心 `llist_reverse_order()` |
|------|----------------------|-------------------------|
| **輸入** | `struct ListNode *head` | `struct llist_node *head` |
| **返回** | 反轉後的頭部指標 | 反轉後的頭部指標 |
| **節點結構** | 非侵入式，含 `val` | 侵入式，僅 `next` |
| **實現方式** | 顯式三變數法 | 相同邏輯，但變數名略異 |
| **時間複雜度** | O(n) | O(n) |
| **空間複雜度** | O(1) | O(1) |
| **並發安全** | 無（假設單執行緒） | 無（Caller 需外部同步） |
| **使用場景** | 練習題、演算法課程 | 核心網路棧、中斷處理 |

- 邏輯上完全相同，差異僅在於資料結構定義和應用場景
- 若要用 `llist` 實作 LeetCode 題目：
  1. 在每個 `llist_node` 中嵌入 `val` 字段
  2. 直接呼叫 `llist_reverse_order()`
  3. 使用 `llist_node->next` 進行配對走訪

**追問**：
- "核心為什麼選擇侵入式設計而 LeetCode 採非侵入式？"
- "如何在核心中使用 `llist` 實作孿生和演算法？"

---

## 面試加分總結

若在面試中恰當融合核心知識，可顯著提升評分：

### 演算法層面加分

- **提及 `llist_reverse_order()`**：展示對開源代碼的實際瞭解
  > "這個反轉邏輯與 Linux 核心的 lib/llist.c 中的 `llist_reverse_order()` 完全相同，都是 O(n) 時間 O(1) 空間的原地反轉。"

- **比較快慢指標 vs 計數法**：展示設計思維的多面性
  > "核心的 list_sort() 採計數分割，而我們用快慢指標一次遍歷；各有優缺點，快慢指標在面試中更能展示指標操控能力。"

- **討論中點定位邊界**：展示對細節的敏感性
  > "初始化 fast=head->next->next 確保反轉起點正確，尤其是 n=2 的邊界情況。"

### 系統設計層面加分

- **侵入式 vs 非侵入式**：展示記憶體和 API 設計的權衡
  > "核心採侵入式設計，在結構中嵌入 list_head 來最小化開銷；LeetCode 的非侵入式設計對教學更友善但浪費額外指標。在百萬級節點場景，差異明顯。"

- **並發安全意識**：展示對實際系統的深層次思考
  > "LeetCode 假設單執行緒，但在核心環境，修改串列需持有自旋鎖或受 RCU 保護，防止讀寫競爭。"

- **API 破壞性操作**：展示對副作用的警惕
  > "我們的 reverseList() 永久修改串列，類似核心的 list_sort()。若 Caller 需保留原始順序，應提供非破壞性版本或明確文件警告。"

### 效能最佳化層面加分

- **O(1) 空間的重要性**
  > "在核心中，棧空間極其寶貴，尤其在中斷上下文（預留空間極少）。O(1) 空間的反轉避免了遞迴棧溢出。"

- **快慢指標的高效性**
  > "相比兩次遍歷（計算長度、找中點），快慢指標一次遍歷兼具高效和優雅，缺點是不知道總長度，但對孿生和題無關。"

- **Cache Locality**（進階）
  > "單向遍歷有更好的 cache locality；核心的 list_sort() 採歸併排序以保持記憶體訪問的局部性。"

### 追蹤題外擴展

若面試官反應良好，可引導至以下話題：

1. **底層同步原語**
   > "若要在多核環境並行走訪這個串列，可以考慮 RCU（Read-Copy-Update），核心在 net/core/ 中大量使用。"

2. **更複雜的串列演算法**
   > "Merging two sorted linked lists 的核心實現在 lib/list_sort.c，採用了類似的快慢指標思想。"

3. **記憶體管理**
   > "核心的 SLAB 分配器在內部使用雙向連結串列管理緩存，這是追蹤記憶體洩漏的關鍵。"

---

## Sources

- [Linux kernel: include/linux/list.h](https://github.com/torvalds/linux/blob/master/include/linux/list.h)
- [Linux kernel: lib/llist.c](https://github.com/torvalds/linux/blob/master/lib/llist.c)
- [Linux kernel: lib/list_sort.c](https://github.com/torvalds/linux/blob/master/lib/list_sort.c)
- [Linux kernel: include/linux/llist.h](https://github.com/torvalds/linux/blob/master/include/linux/llist.h)
- [Linux kernel: net/core/skbuff.c](https://github.com/torvalds/linux/blob/master/net/core/skbuff.c)
- [Linux kernel: kernel/sched/fair.c](https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c)
