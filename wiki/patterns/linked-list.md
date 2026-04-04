# Linked List 鏈結串列操作模式

## 簡介

鏈結串列是一種基礎的線性資料結構，由節點組成，每個節點包含資料和指向下一個節點的指標。相比於陣列，鏈結串列提供了動態的記憶體配置和高效的插入/刪除操作。掌握鏈結串列的各種操作對於解決許多演算法問題至關重要。

## 辨識信號

鏈結串列操作的典型場景：

- **環偵測**：判斷鏈結串列是否包含環
- **反轉**：將鏈結串列的方向反轉
- **合併**：將多個有序鏈結串列合併成一個
- **重新排列**：奇偶分離、將值移動到指定位置
- **查找特定節點**：找中點、找倒數第 N 個節點、找交點
- **緩存設計**：LRU Cache 需要雙向鏈結串列

## 核心技術

### 1. 反轉（Reversal）

#### 迭代反轉

```c
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode* next;
};

struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode* prev = NULL;
    struct ListNode* current = head;

    while (current != NULL) {
        // 保存下一個節點
        struct ListNode* nextTemp = current->next;
        // 反轉指標
        current->next = prev;
        // 移動指標
        prev = current;
        current = nextTemp;
    }

    return prev;  // 新的頭部
}
```

**時間複雜度**: O(n)
**空間複雜度**: O(1)

#### 遞迴反轉

```c
struct ListNode* reverseListRecursive(struct ListNode* head) {
    if (!head || !head->next) {
        return head;  // 基礎情況：空或單節點
    }

    struct ListNode* newHead = reverseListRecursive(head->next);

    // 反轉邊：head->next 的 next 指標指回 head
    head->next->next = head;
    // 斷開原方向的邊
    head->next = NULL;

    return newHead;
}
```

**時間複雜度**: O(n)
**空間複雜度**: O(n) （遞迴棧）

---

### 2. 合併（Merge）

#### 合併兩個有序鏈結串列

```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = NULL;
    struct ListNode* current = &dummy;

    while (list1 != NULL && list2 != NULL) {
        if (list1->val <= list2->val) {
            current->next = list1;
            list1 = list1->next;
        } else {
            current->next = list2;
            list2 = list2->next;
        }
        current = current->next;
    }

    // 連接剩余的部分
    if (list1 != NULL) {
        current->next = list1;
    } else {
        current->next = list2;
    }

    return dummy.next;
}
```

#### 合併 k 個有序鏈結串列（分治法）

```c
struct ListNode* merge(struct ListNode* list1, struct ListNode* list2) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = NULL;
    struct ListNode* current = &dummy;

    while (list1 && list2) {
        if (list1->val <= list2->val) {
            current->next = list1;
            list1 = list1->next;
        } else {
            current->next = list2;
            list2 = list2->next;
        }
        current = current->next;
    }
    current->next = list1 ? list1 : list2;
    return dummy.next;
}

struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    if (listsSize == 0) return NULL;
    if (listsSize == 1) return lists[0];

    int step = 1;
    while (step < listsSize) {
        for (int i = 0; i < listsSize - step; i += step * 2) {
            lists[i] = merge(lists[i], lists[i + step]);
        }
        step *= 2;
    }

    return lists[0];
}
```

---

### 3. 環偵測與移除

#### Floyd 環偵測（龜兔賽跑）

```c
#include <stdbool.h>

bool hasCycle(struct ListNode* head) {
    if (!head || !head->next) return false;

    struct ListNode* slow = head;
    struct ListNode* fast = head->next;

    while (fast != NULL && fast->next != NULL) {
        if (slow == fast) return true;  // 環被偵測到
        slow = slow->next;
        fast = fast->next->next;
    }

    return false;
}
```

#### 尋找環的起點

```c
struct ListNode* detectCycle(struct ListNode* head) {
    if (!head || !head->next) return NULL;

    struct ListNode* slow = head;
    struct ListNode* fast = head;

    // 偵測環
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    // 未找到環
    if (!fast || !fast->next) return NULL;

    // 尋找環的起點：一個指標從頭開始，另一個從相遇點開始
    struct ListNode* ptr1 = head;
    struct ListNode* ptr2 = slow;
    while (ptr1 != ptr2) {
        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
    }

    return ptr1;  // 環的起點
}
```

---

### 4. 找中點和特定位置

#### 找鏈結串列的中點

```c
struct ListNode* findMiddle(struct ListNode* head) {
    if (!head) return NULL;

    struct ListNode* slow = head;
    struct ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  // 偶數長度返回後半的第一個，奇數長度返回中點
}
```

#### 找倒數第 N 個節點

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    struct ListNode* first = &dummy;
    struct ListNode* second = &dummy;

    // 讓 first 先走 n+1 步
    for (int i = 0; i <= n; i++) {
        if (!first) return head;
        first = first->next;
    }

    // 兩個指標一起走
    while (first) {
        first = first->next;
        second = second->next;
    }

    // 刪除節點
    second->next = second->next->next;
    return dummy.next;
}
```

---

### 5. 鏈結串列重新排列

#### 奇偶分離（Odd Even Linked List）

```c
struct ListNode* oddEvenList(struct ListNode* head) {
    if (!head || !head->next) return head;

    struct ListNode* oddHead = head;
    struct ListNode* evenHead = head->next;
    struct ListNode* oddCurrent = oddHead;
    struct ListNode* evenCurrent = evenHead;

    while (evenCurrent && evenCurrent->next) {
        oddCurrent->next = evenCurrent->next;
        oddCurrent = oddCurrent->next;
        evenCurrent->next = oddCurrent->next;
        evenCurrent = evenCurrent->next;
    }

    oddCurrent->next = evenHead;
    return oddHead;
}
```

#### Palindrome 檢驗（回文鏈結串列）

```c
bool isPalindrome(struct ListNode* head) {
    if (!head || !head->next) return true;

    // 找中點
    struct ListNode* slow = head;
    struct ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 反轉後半部分
    struct ListNode* prev = NULL;
    struct ListNode* current = slow;
    while (current) {
        struct ListNode* nextTemp = current->next;
        current->next = prev;
        prev = current;
        current = nextTemp;
    }

    // 比較前半和反轉後的後半
    struct ListNode* left = head;
    struct ListNode* right = prev;
    while (right) {  // right 會先到 NULL（奇數時）
        if (left->val != right->val) return false;
        left = left->next;
        right = right->next;
    }

    return true;
}
```

---

## 常見問題及其解決方案

### 問題列表

| 題號 | 題目 | 難度 | 關鍵技術 |
|------|------|------|---------|
| 19 | Remove Nth Node From End | Medium | 雙指標間距法 |
| 21 | Merge Two Sorted Lists | Easy | 雙指標合併 |
| 23 | Merge k Sorted Lists | Hard | 分治合併 |
| 141 | Linked List Cycle | Easy | Floyd 快慢指標 |
| 146 | LRU Cache | Medium | 雙向鏈結串列+HashMap |
| 160 | Intersection of Two Linked Lists | Easy | 交叉遍歷 |
| 206 | Reverse Linked List | Easy | 迭代反轉 |
| 234 | Palindrome Linked List | Easy | 快慢+反轉+比較 |
| 287 | Find the Duplicate Number | Medium | 陣列上的 Floyd |
| 328 | Odd Even Linked List | Medium | 奇偶分離 |
| 876 | Middle of the Linked List | Easy | 快慢指標 |
| 2095 | Delete the Middle Node | Medium | 快慢+前驅 |
| 2130 | Maximum Twin Sum | Medium | 快慢+反轉 |

---

## 面試講解

### 30 秒版本

「鏈結串列是動態的線性資料結構。掌握反轉、合併、環偵測和尋找特定節點等核心操作至關重要。大多數技巧依賴於高效的指標操作和對邊界情況的仔細處理。」

### 2 分鐘版本

「鏈結串列問題的難度在於：一旦失去對前一個節點的引用，就無法回溯。這促使我們使用技巧如『虛擬頭節點』來簡化邊界處理。

**反轉**是基礎操作。關鍵是在移動指標前保存下一個節點，然後逐步重定向邊。遞迴版本優雅但使用棧空間，迭代版本更高效。

**快慢指標**（Floyd 演算法）在鏈結串列中應用廣泛。在環偵測中，快指標每次走兩步，慢指標走一步。如果存在環，它們最終必然相遇。尋找環的起點需要額外的推理：相遇點到環起點的距離等於頭部到環起點的距離。

**合併**操作涉及比較兩個指標指向的值，並小心地連接節點。在合併 k 個列表時，分治法比優先隊列更高效（在時間複雜度上）。

**回文檢驗**結合了多項技術：找中點（快慢指標）、反轉後半部分、然後逐步比較。這展示了如何組合多個基本操作解決複雜問題。

關鍵的軟技能包括：使用虛擬節點避免邊界情況、多指標追蹤、以及對遞迴與迭代權衡的理解。」

---

## 常見 Follow-up 問題

1. **「反轉中途停止可以嗎？」**
   - 可以的。反轉從某個節點開始到另一個節點結束的部分，只需在循環條件中添加終止檢查。

2. **「Floyd 環偵測的數學原理是什麼？」**
   - 基於鴿籠原理。如果有限的鏈結串列中存在環，快指標最終會進入環。在環內，快指標每次比慢指標多走一步，最終必然追上。

3. **「如何不反轉整個鏈結串列而驗證回文？」**
   - 使用棧：遍歷到中點，將前半部分壓入棧，然後比較棧中的值與後半部分。空間複雜度仍為 O(n)。

4. **「LRU Cache 為什麼要用雙向鏈結串列？」**
   - 為了在 O(1) 時間內移除任意節點。單向鏈結串列無法在不遍歷的情況下找到前驅節點。

5. **「合併 k 個列表時，是分治快還是堆快？」**
   - 時間複雜度都是 O(n log k)，其中 n 是所有元素的總數。分治法更簡潔，堆法也可行。

---

## 和 Kernel 的關聯

Linux 核心的 `list_head` 是一個侵入式雙向鏈結串列，應用廣泛：

### Kernel list_head 概要

```c
struct list_head {
    struct list_head *next, *prev;
};

// 嵌入在結構體中
struct process {
    int pid;
    struct list_head list;
};
```

### container_of 巨集

```c
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

struct process* proc = container_of(list_ptr, struct process, list);
```

### Kernel 常用操作

```c
// 遍歷
list_for_each(pos, head) {
    struct process* p = container_of(pos, struct process, list);
    // 處理 p
}

// 插入
list_add(new_node, head);     // 在 head 之後插入
list_add_tail(new_node, head); // 在 head 之前插入（尾部）

// 刪除
list_del(node);

// 檢查為空
if (list_empty(head)) { }
```

在核心中，這種設計允許同一個結構體同時存在於多個鏈結串列中，且無需額外的 wrapper 結構。

---

## 常見陷阱

### 1. **虛擬節點初始化**

```c
// 錯誤：未初始化 next
struct ListNode dummy;
dummy.next = head;
// dummy.val 是未初始化的垃圾值！

// 正確
struct ListNode dummy;
dummy.val = 0;  // 初始化所有欄位
dummy.next = head;
```

### 2. **忘記更新 next 指標**

```c
// 錯誤：合併時只添加了節點，沒有斷開舊連接
current->next = list1;
// 如果 list1 已經有後繼，會形成無法預測的結構

// 正確：逐步手動管理指標
current->next = list1;
list1 = list1->next;
```

### 3. **空指標檢查不完整**

```c
// 錯誤：假設所有節點都有 next
while (current) {
    if (current->next->val > target) {  // 當 next 為 NULL 時崩潰
        // ...
    }
    current = current->next;
}

// 正確
while (current && current->next) {
    if (current->next->val > target) {
        // ...
    }
    current = current->next;
}
```

### 4. **環的移除不完整**

```c
// 錯誤：找到環的起點後直接返回，未移除環
struct ListNode* cycleStart = detectCycle(head);
return head;  // 環仍然存在！

// 正確：需要額外的步驟找到環的末尾並斷開
struct ListNode* ptr = cycleStart;
while (ptr->next != cycleStart) {
    ptr = ptr->next;
}
ptr->next = NULL;
return head;
```

### 5. **遞迴反轉的棧溢出**

```c
// 對於非常長的鏈結串列，遞迴會因棧溢出而失敗
struct ListNode* head = createVeryLongList(1000000);
struct ListNode* reversed = reverseListRecursive(head);  // 可能棧溢出

// 解決：使用迭代版本
struct ListNode* reversed = reverseList(head);  // 安全
```

---

## 複雜度分析

| 操作 | 時間複雜度 | 空間複雜度 | 備註 |
|------|-----------|-----------|------|
| 反轉 (迭代) | O(n) | O(1) | 最優方案 |
| 反轉 (遞迴) | O(n) | O(n) | 棧深度 |
| 環偵測 | O(n) | O(1) | Floyd 演算法 |
| 找中點 | O(n) | O(1) | 快慢指標 |
| 合併兩個列表 | O(n+m) | O(1) | 不計返回值 |
| 合併 k 個列表 | O(n log k) | O(log k) | 分治法，遞迴棧 |
| 回文檢驗 | O(n) | O(1) | 修改後需還原 |

---

## 進階技巧

### 多層複雜操作：LRU Cache

```c
#include <stdlib.h>
#include <string.h>

typedef struct {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct {
    int capacity;
    int count;
    Node* head;
    Node* tail;
} LRUCache;

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->count = 0;
    cache->head = (Node*)malloc(sizeof(Node));
    cache->tail = (Node*)malloc(sizeof(Node));
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    return cache;
}

void removeNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void addToHead(LRUCache* cache, Node* node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

int lRUCacheGet(LRUCache* obj, int key) {
    // 簡化版本：實際需要 HashMap
    // 這裡僅示意雙向鏈結串列的操作
    return -1;
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    // 簡化版本：實際需要 HashMap
    // 移除舊節點，添加新節點到頭部
}
```

---

## 練習建議

1. 先從簡單的反轉開始（Reverse Linked List）
2. 學習快慢指標（Find Middle, Linked List Cycle）
3. 練習合併操作（Merge Two Lists）
4. 挑戰複雜的組合問題（Palindrome Linked List）
5. 實現 LRU Cache 整合多項技能
