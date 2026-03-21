# 234. Palindrome Linked List — 逐行中文解說與圖解

---

## 一、整體架構

```
isPalindrome(head)
│
├─ Step 1  endOfFirstHalf()  →  用快慢指標找前半段末尾
├─ Step 2  reverseList()     →  原地反轉後半段
├─ Step 3  比較兩端
├─ Step 4  reverseList()     →  復原後半段（讓串列回到原狀）
└─ Step 5  回傳結果
```

---

## 二、struct 定義

```c
#include <stdbool.h>          // 引入 bool、true、false（C99 之後才有）

struct ListNode {
    int val;                  // 節點儲存的整數值
    struct ListNode *next;    // 指向下一個節點的指標；最後一個節點的 next = NULL
};
```

**圖示：[1 → 2 → 2 → 1] 在記憶體中的樣子**

```
 head
  │
  ▼
┌───────┐    ┌───────┐    ┌───────┐    ┌───────┐
│ val=1 │───▶│ val=2 │───▶│ val=2 │───▶│ val=1 │───▶ NULL
│ next  │    │ next  │    │ next  │    │ next  │
└───────┘    └───────┘    └───────┘    └───────┘
  [0]          [1]          [2]          [3]
```

---

## 三、`endOfFirstHalf` — 快慢指標找前半段末尾

```c
static struct ListNode *endOfFirstHalf(struct ListNode *head) {
    struct ListNode *fast = head;   // fast 每次跳 2 步
    struct ListNode *slow = head;   // slow 每次跳 1 步
    while (fast->next != NULL && fast->next->next != NULL) {
        //  fast->next != NULL         → fast 後面還有至少 1 個節點
        //  fast->next->next != NULL   → fast 後面還有至少 2 個節點
        //  兩個條件都成立才能讓 fast 安全地跳 2 步
        fast = fast->next->next;    // fast 跳 2 步
        slow = slow->next;          // slow 跳 1 步
    }
    return slow;    // 迴圈結束時，slow 就在前半段的最後一個節點
}
```

### 圖解 A：偶數長度 [1, 2, 2, 1]

```
初始
  slow           fast
   │               │
   ▼               ▼
  [1] → [2] → [2] → [1] → NULL

迭代 1：fast->next=[1], fast->next->next=NULL → 條件不成立，迴圈結束！
  ∴ slow 停在索引 0 → 但還差一步？

讓我們重新完整走一遍：

初始：slow=index0(1), fast=index0(1)

檢查 while：fast->next = index1(2) ≠ NULL  ✓
           fast->next->next = index2(2) ≠ NULL  ✓  → 進入迴圈

  fast = fast->next->next = index2(2)
  slow = slow->next       = index1(2)

  slow           fast
   │               │
  [1]→[2]→[2]→[1]→NULL
        ↑       ↑

檢查 while：fast->next = index3(1) ≠ NULL  ✓
           fast->next->next = NULL  ✗  → 離開迴圈

回傳 slow = index1(val=2)  ← 前半段 [1,2] 的最後一個節點 ✓
```

```
最終位置：

  slow           fast
   │               │
  [1]→[2]→[2]→[1]→NULL
        ↑       ↑
  前半段末尾   fast 停在倒數第二個

前半段：[1, 2]（index 0–1）
後半段：[2, 1]（index 2–3）← 從 slow->next 開始
```

---

### 圖解 B：奇數長度 [1, 2, 3, 2, 1]

```
初始：slow=index0(1), fast=index0(1)

迭代 1：fast->next=index1(2)≠NULL ✓, fast->next->next=index2(3)≠NULL ✓
  fast = index2(3),  slow = index1(2)

迭代 2：fast->next=index3(2)≠NULL ✓, fast->next->next=index4(1)≠NULL ✓
  fast = index4(1),  slow = index2(3)  ← slow 抵達中間節點

迭代 3：fast->next = NULL ✗ → 離開迴圈

回傳 slow = index2(val=3)  ← 中間節點

  [1]→[2]→[3]→[2]→[1]→NULL
              ↑       ↑
           slow(末尾) fast

前半段：[1, 2, 3]（含中間節點）
後半段：[2, 1]（index 3–4）← 從 slow->next 開始
中間節點 3 被包在前半段，比較時 p2 先到 NULL，中間節點被自然跳過 ✓
```

**關鍵規律：**
```
串列長度    slow 停在      後半段長度
  偶數 2n    index n-1       n
  奇數 2n+1  index n（中間）  n  ← 中間節點留在前半段，比較時忽略
```

---

## 四、`reverseList` — 原地反轉串列

```c
static struct ListNode *reverseList(struct ListNode *head) {
    struct ListNode *prev = NULL;   // 反轉後，原本的頭節點 next 要指向 NULL
    struct ListNode *curr = head;   // 當前正在處理的節點
    while (curr != NULL) {
        struct ListNode *next = curr->next;  // 先暫存 curr 的下一個，否則斷鏈後找不到
        curr->next = prev;                   // 把 curr 的 next 反轉，指向前一個節點
        prev = curr;                         // prev 往前移一步
        curr = next;                         // curr 往前移一步（走原本的方向）
    }
    // curr == NULL 時，prev 指向最後一個節點，即反轉後的新頭
    return prev;
}
```

### 圖解：反轉 [2 → 1 → NULL]

```
初始：prev=NULL, curr=[2]

─── 第 1 圈 ───
  next = curr->next = [1]
  curr->next = prev = NULL
  prev = curr = [2]
  curr = next = [1]

  NULL ← [2]    [1] → NULL
          ↑      ↑
         prev   curr

─── 第 2 圈 ───
  next = curr->next = NULL
  curr->next = prev = [2]
  prev = curr = [1]
  curr = next = NULL

  NULL ← [2] ← [1]
                 ↑
                prev

─── curr == NULL，離開迴圈 ───

回傳 prev = [1]（反轉後的新頭）

結果：[1] → [2] → NULL  ✓
```

---

## 五、`isPalindrome` — 主函式，以 [1, 2, 2, 1] 為例完整走一遍

```c
bool isPalindrome(struct ListNode *head) {
    if (head == NULL) return true;   // 空串列視為回文（邊界條件）
```

### Step 1：找前半段末尾

```c
    struct ListNode *firstHalfEnd = endOfFirstHalf(head);
    // 由圖解 A 得知：firstHalfEnd = 索引 1（val=2）
```

```
 head         firstHalfEnd
  │                │
 [1]  →  [2]  →  [2]  →  [1]  →  NULL
                   ↑
            前半段到這裡結束
            firstHalfEnd->next = [2]（索引 2）← 後半段的起點
```

---

### Step 2：反轉後半段

```c
    struct ListNode *secondHalfStart = reverseList(firstHalfEnd->next);
    // firstHalfEnd->next 原本是索引 2（val=2）
    // reverseList([2]→[1]→NULL) 回傳 [1]→[2]→NULL
    // secondHalfStart = 索引 3（val=1）
```

```
反轉前（後半段）：   [2] → [1] → NULL
反轉後（後半段）：   [1] → [2] → NULL

整體串列此刻的狀態：
（注意：firstHalfEnd->next 暫時懸空，因為 reverseList 把它接到了 NULL）

 head      firstHalfEnd     secondHalfStart
  │              │                │
 [1] → [2] → [2] → NULL    [1] → [2] → NULL
  ↑─── 前半段 ───↑           ↑─── 後半段（已反轉）──↑

⚠️  此時串列暫時被「切斷」，firstHalfEnd->next 尚未重新接上
   （Step 4 會修復）
```

---

### Step 3：逐節點比較

```c
    struct ListNode *p1 = head;           // p1 從前半段頭部出發
    struct ListNode *p2 = secondHalfStart;// p2 從後半段（反轉後）頭部出發
    bool result = true;
    while (result && p2 != NULL) {        // p2 到 NULL 就停（後半段走完即可）
        if (p1->val != p2->val) result = false;
        p1 = p1->next;
        p2 = p2->next;
    }
```

```
比較過程：

回合 1：p1=[1], p2=[1] → 1==1 ✓
         p1→[2], p2→[2]

回合 2：p1=[2], p2=[2] → 2==2 ✓
         p1→[2], p2→NULL

p2 == NULL → 離開迴圈，result = true ✓
```

```
  p1  ─────────────────────────────────────▶
 [1] → [2] → [2] → NULL    [1] → [2] → NULL
                             ↑─── p2 ──────▶

  圈 1：p1.val(1) == p2.val(1)  ✓
  圈 2：p1.val(2) == p2.val(2)  ✓
  p2 下一步 = NULL，結束比較
```

---

### Step 4：復原串列

```c
    firstHalfEnd->next = reverseList(secondHalfStart);
    // reverseList([1]→[2]→NULL) = [2]→[1]→NULL
    // 把 firstHalfEnd->next 重新接回 [2]（索引 2）
```

```
復原後：

 head
  │
 [1] → [2] → [2] → [1] → NULL   ← 與輸入完全相同 ✓
```

---

### Step 5：回傳

```c
    return result;   // true（是回文）
}
```

---

## 六、反例驗證：[1, 2]（不是回文）

```
Step 1：endOfFirstHalf([1,2])
  初始：slow=[1], fast=[1]
  while：fast->next=[2]≠NULL ✓, fast->next->next=NULL ✗ → 不進迴圈
  回傳 slow=[1]  ← firstHalfEnd

Step 2：reverseList(firstHalfEnd->next = [2])
  [2]→NULL 反轉後仍是 [2]→NULL
  secondHalfStart = [2]

Step 3：比較
  p1=[1], p2=[2] → 1≠2 → result = false，離開迴圈

Step 4：復原（把 [2]→NULL 再接回 [1]）

Step 5：回傳 false ✓
```

---

## 七、複雜度總結

| 步驟 | 操作 | 時間 | 空間 |
|------|------|------|------|
| `endOfFirstHalf` | 快慢指標遍歷 | O(n/2) | O(1) |
| 第一次 `reverseList` | 反轉後半段 | O(n/2) | O(1) |
| 比較迴圈 | 逐節點比對 | O(n/2) | O(1) |
| 第二次 `reverseList` | 復原後半段 | O(n/2) | O(1) |
| **合計** | | **O(n)** | **O(1)** |

---

## 八、三個函式的職責總覽

```
isPalindrome(head)
│
├──① endOfFirstHalf(head)
│      輸入：完整串列的 head
│      輸出：前半段最後一個節點
│      技巧：fast/slow 指標（fast 速度是 slow 的 2 倍）
│
├──② reverseList(firstHalfEnd->next)
│      輸入：後半段的第一個節點
│      輸出：反轉後的新頭節點
│      技巧：prev/curr/next 三指標滾動反轉
│
├──③ 比較迴圈
│      p1 從頭走，p2 從反轉後半段的頭走
│      任何一對不等 → false
│
├──④ reverseList(secondHalfStart)  ← 復原
│      把後半段再反轉一次，接回 firstHalfEnd->next
│
└──⑤ return result
```
