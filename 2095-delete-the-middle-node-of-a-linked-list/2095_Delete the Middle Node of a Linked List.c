//Linked List: https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/description/?envType=study-plan-v2&envId=leetcode-75
/*
Hint1: If a point with a speed s moves n units in a given time, a point with speed 2 * s will move 2 * n units at the same time. Can you use this to find the middle node of a linked list?
Hint2: If you are given the middle node, the node before it, and the node after it, how can you modify the linked list?
*/

/*
## Idea
Use fast and slow pointers.
- `fast` moves 2 steps each time
- `slow` moves 1 step each time
- let `fast` start from `head->next->next`

This makes `slow` stop at the node just before the middle node, so we can remove the middle node by:
slow->next = slow->next->next;

## Complexity
- Time: `O(n)`
- Space: `O(1)`
*/

/*
用這題的關鍵是：
- `slow` 每次走 `1` 步
- `fast` 每次走 `2` 步
- `fast` 一開始不是從 `head` 開始，而是從 `head->next->next` 開始
- 所以最後 `slow` 會停在「中間節點的前一個節點」

**例子 1**
鏈結串列：`[1,3,4,7,1,2,6]`
索引如下：
index:  0   1   2   3   4   5   6
value: [1] [3] [4] [7] [1] [2] [6]

一開始：
slow = head
fast = head->next->next
index:  0   1   2   3   4   5   6
value: [1] [3] [4] [7] [1] [2] [6]
        ^       ^
      slow     fast

第 1 次迴圈後：
- `slow` 往前 1 步，到 index 1
- `fast` 往前 2 步，從 index 2 到 index 4
index:  0   1   2   3   4   5   6
value: [1] [3] [4] [7] [1] [2] [6]
            ^           ^
          slow         fast

第 2 次迴圈後：
- `slow` 到 index 2
- `fast` 到 index 6
index:  0   1   2   3   4   5   6
value: [1] [3] [4] [7] [1] [2] [6]
                ^               ^
              slow             fast

接著停止，因為 `fast->next == NULL`。

這時：
- `slow` 在 index 2，值是 `4`
- `slow->next` 在 index 3，值是 `7`
- `7` 就是中間節點

所以做這行：
slow->next = slow->next->next;
就是：
原本:
[1] -> [3] -> [4] -> [7] -> [1] -> [2] -> [6]
slow 在 [4]
slow->next 是 [7]

改完後:
[1] -> [3] -> [4] --------> [1] -> [2] -> [6]
                 跳過 [7]
最後結果：
[1,3,4,1,2,6]

**為什麼 `fast` 要從 `head->next->next` 開始？**
因為這題不是要找到「中間節點本身」，
而是要找到「中間節點的前一個節點」。

如果 `fast` 從 `head` 開始，通常最後 `slow` 會停在中間節點本身。
但單向鏈結串列不能往回走，所以不好刪除。

因此讓 `fast` 先多走兩步，最後 `slow` 就會剛好停在中間節點前面。

**例子 2**
`[1,2,3,4]`
index:  0   1   2   3
value: [1] [2] [3] [4]

slow = index 0
fast = index 2
```

因為 `fast->next` 還在，所以進入一次迴圈：

```text
slow -> index 1
fast -> NULL
```

最後：

- `slow` 在 `2`
- `slow->next` 在 `3`

把 `3` 刪掉：

```text
[1] -> [2] -> [4]
```

**一句話記法**

這題可以記成：

- `fast` 跑兩步
- `slow` 跑一步
- `fast` 提前出發
- 讓 `slow` 停在中間節點前一格
- 再用 `slow->next = slow->next->next` 刪除中間節點

如果你要，我可以下一步幫你畫成「節點框框箭頭版 ASCII 圖」，會更像課本筆記。
*/

// Definition for singly-linked list.
// 單向鏈結串列的節點定義
struct ListNode {
    int val;                 // 節點儲存的值
    struct ListNode *next;   // 指向下一個節點
};

// 刪除鏈結串列的中間節點，並回傳新的頭節點
struct ListNode* deleteMiddle(struct ListNode* head) {
    // 如果串列是空的，或只有一個節點
    // 刪掉中間節點後就會變成空串列，因此回傳 NULL
    if (head == NULL || head->next == NULL) {
        return NULL;
    }

    // slow 每次走 1 步
    // 最後會停在「中間節點的前一個節點」
    struct ListNode* slow = head;

    // fast 每次走 2 步
    // 這裡先從 head 的下下個節點開始
    // 這樣當 fast 走到尾端時，slow 剛好在中間節點的前一個位置
    struct ListNode* fast = head->next->next;

    // 當 fast 還能再往前走 2 步時，持續移動
    while (fast != NULL && fast->next != NULL) {
        // slow 前進 1 步
        slow = slow->next;
        // fast 前進 2 步
        fast = fast->next->next;
    }

    // 此時 slow->next 就是中間節點
    // 直接讓 slow 指向中間節點的下一個節點
    // 等於把中間節點從串列中移除
    slow->next = slow->next->next;

    // 回傳原本的頭節點
    return head;
}


/*
下面用「節點框框箭頭版 ASCII 圖」說明。

題目的程式在 [2095_Delete the Middle Node of a Linked List.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2095-delete-the-middle-node-of-a-linked-list/2095_Delete the Middle Node of a Linked List.c)。

**範例**
`head = [1,3,4,7,1,2,6]`

中間節點是 `7`，我們要把它刪掉。

**Step 0: 初始位置**

- `slow = head`
- `fast = head->next->next`

所以：

```text
head
 |
 v
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
| 1 | o----->| 3 | o----->| 4 | o----->| 7 | o----->| 1 | o----->| 2 | o----->| 6 | NULL |
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
  ^                           ^
  |                           |
 slow                        fast
```

**Step 1: 進入 while 第 1 次**

- `slow = slow->next`
- `fast = fast->next->next`

所以：

```text
head
 |
 v
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
| 1 | o----->| 3 | o----->| 4 | o----->| 7 | o----->| 1 | o----->| 2 | o----->| 6 | NULL |
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
              ^                                       ^
              |                                       |
             slow                                    fast
```

**Step 2: 進入 while 第 2 次**

- `slow` 再走 1 步
- `fast` 再走 2 步

所以：

```text
head
 |
 v
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
| 1 | o----->| 3 | o----->| 4 | o----->| 7 | o----->| 1 | o----->| 2 | o----->| 6 | NULL |
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
                          ^                                                           ^
                          |                                                           |
                         slow                                                        fast
```

這時 `fast` 已經在最後一個節點 `6`，
因為 `fast->next == NULL`，所以迴圈停止。

**Step 3: 準備刪除中間節點**

現在：

- `slow` 在值 `4`
- `slow->next` 在值 `7`

也就是說：

```text
head
 |
 v
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
| 1 | o----->| 3 | o----->| 4 | o----->| 7 | o----->| 1 | o----->| 2 | o----->| 6 | NULL |
+---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+---+    +---+------+
                          ^
                          |
                         slow
```

執行這行：

```c
slow->next = slow->next->next;
```

意思是把：

- `4` 原本連到 `7`
- 改成直接連到 `1`

**Step 4: 刪除後**

```text
head
 |
 v
+---+---+    +---+---+    +---+---+                 +---+---+    +---+---+    +---+------+
| 1 | o----->| 3 | o----->| 4 | o------------------>| 1 | o----->| 2 | o----->| 6 | NULL |
+---+---+    +---+---+    +---+---+                 +---+---+    +---+---+    +---+------+

被跳過的中間節點：
+---+---+
| 7 | o-----> ...
+---+---+
```

最後結果就是：

```text
[1,3,4,1,2,6]
```

**一句話理解這個圖**

- `fast` 跑得比較快
- 當 `fast` 到尾巴時，`slow` 正好停在中間節點前面
- 然後把 `slow` 的箭頭直接改接到下一個節點，就完成刪除

如果你要，我可以再幫你畫一版 `[1,2,3,4]` 的 ASCII 圖，因為偶數長度也很常考。
*/