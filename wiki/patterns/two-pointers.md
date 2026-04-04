# Two Pointers 雙指標模式

## 簡介

雙指標是一種高效的技巧，用於在線性資料結構（陣列、鏈結串列）中解決問題。透過同時維護兩個指標，可以減少巢狀迴圈，從而將時間複雜度從 O(n²) 降低到 O(n)。

## 辨識信號

使用雙指標模式的典型場景：

- **已排序陣列**：需要尋找配對元素，如兩數之和達到目標值
- **原地操作**：需要修改陣列但不允許使用額外空間
- **鏈結串列遍歷**：檢測環、找中點、移除倒數第 N 個節點
- **子序列/子字串匹配**：驗證順序包含或相同性質
- **移動指標以調整搜尋範圍**：基於某種條件縮小或擴大窗口

## 變體分類

### 1. 對向雙指標（相向而行）

兩個指標從陣列兩端向中心移動，常用於尋找配對或最大化某種值。

| 題號 | 題目 | 難度 | 關鍵差異 |
|------|------|------|---------|
| 11 | Container With Most Water | Medium | 貪心收縮，短邊移動 |
| 345 | Reverse Vowels of a String | Easy | 雙向過濾+交換 |
| 283 | Move Zeroes | Easy | 原地交換非零元素 |

#### 模板：對向雙指標

```c
#include <string.h>

void twoPointerOpposite(int* arr, int arrSize) {
    int left = 0, right = arrSize - 1;

    while (left < right) {
        // 根據條件判斷移動哪個指標
        if (shouldMoveLeft(arr[left], arr[right])) {
            left++;
        } else {
            right--;
        }
        // 處理當前指標對應的元素
    }
}
```

#### 例子：Container With Most Water (LeetCode 11)

```c
int maxArea(int* height, int heightSize) {
    int left = 0, right = heightSize - 1;
    int maxVol = 0;

    while (left < right) {
        int width = right - left;
        int currentHeight = (height[left] < height[right]) ? height[left] : height[right];
        int currentVol = width * currentHeight;

        if (currentVol > maxVol) {
            maxVol = currentVol;
        }

        // 移動高度較小的邊，嘗試找到更高的邊
        if (height[left] < height[right]) {
            left++;
        } else {
            right--;
        }
    }

    return maxVol;
}
```

---

### 2. 快慢指標（同向不同速）

兩個指標以不同速度在同一方向移動，常用於鏈結串列問題（環偵測、找中點）。

| 題號 | 題目 | 難度 | 關鍵差異 |
|------|------|------|---------|
| 141 | Linked List Cycle | Easy | Floyd 環偵測 |
| 234 | Palindrome Linked List | Easy | 找中點+反轉+比較 |
| 287 | Find the Duplicate Number | Medium | 陣列上的 Floyd 環偵測 |
| 876 | Middle of the Linked List | Easy | 快慢找中點 |
| 2095 | Delete the Middle Node | Medium | 快慢+前驅指標 |
| 2130 | Maximum Twin Sum | Medium | 快慢+反轉+比較 |

#### 模板：快慢指標（環偵測）

```c
#include <stdbool.h>

struct ListNode {
    int val;
    struct ListNode* next;
};

bool hasCycle(struct ListNode* head) {
    if (!head || !head->next) return false;

    struct ListNode* slow = head;
    struct ListNode* fast = head->next;

    while (fast && fast->next) {
        if (slow == fast) {
            return true;  // 環被偵測到
        }
        slow = slow->next;
        fast = fast->next->next;
    }

    return false;
}
```

#### 模板：快慢指標（找中點）

```c
struct ListNode* findMiddle(struct ListNode* head) {
    if (!head) return NULL;

    struct ListNode* slow = head;
    struct ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  // 偶數長度時，返回第二個中點
}
```

---

### 3. 前後指標（同向同速）

兩個指標以相同速度在同一方向移動，維持固定距離或用於遍歷配對。

| 題號 | 題目 | 難度 | 關鍵差異 |
|------|------|------|---------|
| 19 | Remove Nth Node From End | Medium | 固定間距法 |
| 160 | Intersection of Two Linked Lists | Easy | 交叉遍歷法 |
| 392 | Is Subsequence | Easy | 順序匹配掃描 |
| 328 | Odd Even Linked List | Medium | 奇偶分離重組 |

#### 模板：前後指標（固定間距）

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    struct ListNode* first = &dummy;
    struct ListNode* second = &dummy;

    // 讓 first 指標先走 n+1 步
    for (int i = 0; i <= n; i++) {
        if (first) first = first->next;
    }

    // 兩個指標一起走，直到 first 到達末尾
    while (first) {
        first = first->next;
        second = second->next;
    }

    second->next = second->next->next;
    return dummy.next;
}
```

#### 模板：前後指標（順序匹配）

```c
#include <stdbool.h>

bool isSubsequence(char* s, char* t) {
    int i = 0;  // s 的指標
    int j = 0;  // t 的指標

    while (t[j] != '\0') {
        if (s[i] != '\0' && s[i] == t[j]) {
            i++;
        }
        j++;
    }

    return s[i] == '\0';
}
```

---

## 面試講解

### 30 秒版本

「雙指標是一種在有序陣列或鏈結串列中高效搜尋的技巧。透過同時操縱兩個指標，我們能夠避免巢狀迴圈，將複雜度從 O(n²) 降至 O(n)。常見的變體包括對向移動、快慢移動和固定間距移動。」

### 2 分鐘版本

「雙指標技巧基於一個核心觀察：在某些問題中，兩個指標的相對位置包含了關鍵資訊。

**對向指標**用於有序陣列問題，比如 Container With Most Water。我們從兩端開始，根據較短邊的高度來決定移動哪個指標。每次移動時，我們記錄當前的最大值。時間複雜度是 O(n)，因為每個元素最多被訪問一次。

**快慢指標**基於龜兔賽跑演算法（Floyd's Cycle Detection）。在鏈結串列中，如果存在環，快指標最終會趕上慢指標。這個技巧優雅地解決了環偵測問題，無需額外的雜湊表。

**前後指標**維持固定的距離或用於遍歷配對。在移除倒數第 N 個節點時，我們先讓一個指標走 N 步，然後兩個指標一起走，直到快指標到達末尾。

時間複雜度通常是 O(n)，空間複雜度是 O(1)，這使得雙指標在空間受限的場景中非常有價值。」

---

## 常見 Follow-up 問題

1. **「如何在 Container With Most Water 中優化空間？」**
   - 已經是 O(1) 空間，這就是最優解。但可以討論為什麼貪心策略有效：移動較長的邊不會改進結果，因為寬度只會減少。

2. **「Floyd 環偵測為什麼一定能找到環？」**
   - 如果環存在，快指標最終會進入環。在環內，快指標每次比慢指標多走一步，最終必然相遇。

3. **「如何在 Floyd 中找到環的起點？」**
   - 相遇後，將一個指標重置到頭部，兩個指標同速移動。它們再次相遇的點就是環的起點。

4. **「為什麼 Is Subsequence 不能直接用子字串方法？」**
   - 因為子序列不要求連續，而子字串要求連續。雙指標能正確處理非連續的匹配。

5. **「在環偵測中，為什麼不能用集合儲存已訪問節點？」**
   - 集合需要額外的 O(n) 空間，而 Floyd 演算法只需 O(1) 空間。在面試中，空間優化是一個重要的進階要點。

---

## 和 Kernel 的關聯

在 Linux 核心中，雙指標技巧也有應用：

- **list_head 遍歷**：核心的 `list_for_each` 巨集使用單一指標遍歷，但在某些場景（如檢測鏈結串列損壞）可以使用快慢指標。

- **快慢指標在核心結構中**：用於檢測循環引用或不當修改的鏈結串列。

- **固定間距指標**：類似於環形緩衝區（circ_buf）的讀寫指標，它們維持固定的相對位置。

```c
// Kernel-style example: 遍歷 list_head
struct list_head* slow = &head;
struct list_head* fast = head.next->next;

while (fast != &head && fast->next != &head) {
    if (slow == fast) {
        printk(KERN_ERR "Cycle detected in list!");
        break;
    }
    slow = slow->next;
    fast = fast->next->next;
}
```

---

## 常見陷阱

### 1. **指標邊界問題**

```c
// 錯誤：沒有檢查 fast->next
while (fast) {  // fast 可能為 NULL
    fast = fast->next->next;  // 崩潰！
}

// 正確：檢查 fast 和 fast->next
while (fast && fast->next) {
    fast = fast->next->next;
}
```

### 2. **忘記處理邊界情況**

```c
// 錯誤：空鏈結串列會導致崩潰
struct ListNode* middle = findMiddle(head);

// 正確：檢查 NULL
if (!head) return NULL;
struct ListNode* middle = findMiddle(head);
```

### 3. **off-by-one 錯誤**

```c
// 錯誤：迴圈條件不正確
for (int i = 0; i < n; i++) {
    first = first->next;  // 只走了 n-1 步
}

// 正確：走 n 步
for (int i = 0; i < n; i++) {
    if (first) first = first->next;
}
```

### 4. **修改鏈結串列後忘記還原**

在某些問題（如 Palindrome Linked List）中，我們可能反轉一部分鏈結串列。務必在驗證後恢復原始結構，或明確文件中說明不需要恢復。

### 5. **對向指標的移動邏輯**

```c
// 錯誤：總是移動 left
if (arr[left] < arr[right]) {
    left++;  // 錯誤的邏輯
} else {
    right--;
}

// 正確：根據實際問題邏輯決定
if (shouldMoveLeft(...)) {
    left++;
} else {
    right--;
}
```

---

## 複雜度分析

| 變體 | 時間複雜度 | 空間複雜度 | 應用場景 |
|------|-----------|-----------|--------|
| 對向指標 | O(n) | O(1) | 有序陣列配對問題 |
| 快慢指標 | O(n) | O(1) | 鏈結串列環偵測、中點尋找 |
| 前後指標 | O(n) | O(1) | 固定距離操作、順序匹配 |

---

## 進階技巧

### 多指標組合

某些複雜問題需要結合多個指標技巧：

```c
// 例子：同時使用快慢指標和前驅指標
struct ListNode* detectAndRemoveCycle(struct ListNode* head) {
    struct ListNode* slow = head, * fast = head;

    // 環偵測
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    if (!fast || !fast->next) return head;  // 無環

    // 尋找環起點
    struct ListNode* ptr1 = head, * ptr2 = slow;
    while (ptr1 != ptr2) {
        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
    }

    // 移除環：找到環的末尾
    while (ptr2->next != ptr1) {
        ptr2 = ptr2->next;
    }
    ptr2->next = NULL;

    return head;
}
```

---

## 練習建議

1. 先從簡單的對向指標問題開始（如 Two Sum II）
2. 然後學習快慢指標的基本用法（如 Linked List Cycle）
3. 最後練習複雜的組合問題（如 Palindrome Linked List）
4. 每道題都嘗試不同的實現方式，體會時間-空間的權衡
