#include <stddef.h>

// Linked List: https://leetcode.com/problems/remove-nth-node-from-end-of-list/description/?envType=problem-list-v2&envId=wl94y6ih
/*
## Idea
Use a dummy node and two pointers.
- `fast` starts from `dummy`
- `slow` also starts from `dummy`
- move `fast` ahead by `n + 1` steps first
- then move both pointers together until `fast == NULL`
- now `slow->next` is exactly the node to remove

The `dummy` node lets us delete the original head node with the same logic.

## Complexity
- Time: `O(sz)`
- Space: `O(1)`
*/

/*
這題最核心的觀念：

我們不是要停在「倒數第 n 個節點」本身，
而是要停在「倒數第 n 個節點的前一個節點」。

因為真正的刪除動作是這一行：
`slow->next = slow->next->next;`

也就是把目標節點直接跳過去。

--------------------------------------------------
為什麼要加 dummy？
--------------------------------------------------

如果直接從 `head` 開始，當要刪掉的剛好是第一個節點時，
就會需要特判。

例如：
head = [1]
n = 1

如果沒有 dummy：
- 你最後找不到「head 前一個節點」

如果有 dummy：

dummy -> [1] -> NULL

那麼 dummy 就剛好扮演「head 前一個節點」的角色，
整題的刪除邏輯可以完全統一。

--------------------------------------------------
整體流程圖
--------------------------------------------------

以：
head = [1,2,3,4,5]
n = 2

為例，我們要刪掉倒數第 2 個節點，也就是值為 4 的節點。

先建立：

dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL

一開始：
- fast = dummy
- slow = dummy

```text
dummy     1       2       3       4       5
  |       |       |       |       |       |
  v       v       v       v       v       v
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
  ^ 
 fast, slow
```

--------------------------------------------------
Step 1: 先讓 fast 走 n + 1 步
--------------------------------------------------

這題 n = 2，所以 fast 要先走 3 步。

第 1 步後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
          ^
         fast
  ^
 slow
```

第 2 步後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                 ^
                fast
  ^
 slow
```

第 3 步後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                        ^
                       fast
  ^
 slow
```

此時 `fast` 和 `slow` 之間剛好差了 `n + 1` 個位置。

--------------------------------------------------
Step 2: fast 和 slow 一起前進
--------------------------------------------------

第 1 次一起移動後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
          ^              ^
         slow           fast
```

第 2 次一起移動後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                 ^              ^
                slow           fast
```

第 3 次一起移動後：
```text
[ 0 ] -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                        ^                     ^
                       slow                 fast(NULL)
```

現在 `fast == NULL`，停止。

這時候：
- `slow` 在值為 3 的節點
- `slow->next` 在值為 4 的節點
- 值為 4 的節點就是要刪掉的節點

--------------------------------------------------
Step 3: 刪除目標節點
--------------------------------------------------

原本：

```text
[ 3 ] -> [ 4 ] -> [ 5 ] -> NULL
  ^
 slow
```

做完：
`slow->next = slow->next->next;`

就會變成：

```text
[ 3 ] ---------> [ 5 ] -> NULL
  ^
 slow
```

整條串列最後就是：

```text
dummy -> [1] -> [2] -> [3] -> [5] -> NULL
```

回傳 `dummy.next` 後得到：

`[1,2,3,5]`

--------------------------------------------------
為什麼 fast 要先走 n + 1 步，不是 n 步？
--------------------------------------------------

因為我們要讓 `slow` 最後停在「待刪節點的前一個節點」，
不是停在待刪節點本身。

如果 `fast` 只先走 `n` 步，
那麼最後 `slow` 很容易停在錯的位置，
你就沒辦法直接安全地做：
`slow->next = slow->next->next;`

--------------------------------------------------
這份程式和 Description.md 一樣嗎？
--------------------------------------------------

不一樣。

`19_Remove Nth Node From End of List_Description.md`
只是題目敘述、範例、限制與提示，
它告訴你「要做什麼」。

這份 `.c`
才是具體的演算法實作，
它告訴你「怎麼做」。

如果要判斷哪個比較好，要看目的：
- 想知道題目要求：`Description.md` 是必要的，因為它是題目本身
- 想知道怎麼寫出可提交的程式：這份 `.c` 更直接、更有用

若以「解題」角度來說，這份 `.c` 比 `Description.md` 更完整，
因為它已經把提示落成可執行的 one-pass 解法。
*/

struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
    // 建立 dummy 節點，讓「刪除頭節點」也能用同一套流程處理。
    struct ListNode dummy;

    // `fast` 會先往前走，和 `slow` 拉開固定距離。
    struct ListNode* fast = &dummy;

    // `slow` 最後會停在「待刪節點的前一個節點」。
    struct ListNode* slow = &dummy;

    // `dummy.val` 不重要，這裡設成 0 只是給一個占位值。
    dummy.val = 0;

    // 讓 dummy 指向真正的 head。
    dummy.next = head;

    // 先讓 fast 往前走 n + 1 步。
    // 這樣當 fast 走到 NULL 時，
    // slow 才會剛好停在待刪節點的前一個位置。
    //
    // 依照題目限制，1 <= n <= 串列長度，
    // 所以這裡不會在非法輸入下解參考到無效位置。
    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }

    // 當 fast 還沒走到結尾，就讓 fast 和 slow 一起往前。
    while (fast != NULL) {
        // fast 每次往前走 1 步。
        fast = fast->next;

        // slow 也同步往前走 1 步。
        slow = slow->next;
    }

    // 迴圈結束時，slow->next 就是倒數第 n 個節點。
    // 直接把它跳過去，完成刪除。
    slow->next = slow->next->next;

    // 真正的新頭節點在 dummy.next。
    return dummy.next;
}
