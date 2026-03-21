// https://leetcode.com/problems/middle-of-the-linked-list/description/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

/*
解題思路：快慢指標（Fast & Slow Pointer）

目標：找出 linked list 的中間節點。
      若有兩個中間節點，回傳第二個。

作法：
1. slow 每次走 1 步，fast 每次走 2 步。
2. 當 fast 走到尾端（fast == NULL 或 fast->next == NULL）時，
   slow 恰好停在中間節點。

圖解 1：奇數個節點 [1 -> 2 -> 3 -> 4 -> 5]

初始：
slow = 1, fast = 1

第 1 次後：
slow = 2, fast = 3

第 2 次後：
slow = 3, fast = 5

fast->next == NULL，迴圈結束。
回傳 slow = 3（節點值為 3）。

[1] -> [2] -> [3] -> [4] -> [5] -> NULL
               s             f

圖解 2：偶數個節點 [1 -> 2 -> 3 -> 4 -> 5 -> 6]

初始：
slow = 1, fast = 1

第 1 次後：
slow = 2, fast = 3

第 2 次後：
slow = 3, fast = 5

第 3 次後：
slow = 4, fast = NULL（fast = 5->next->next = 7，但 6->next = NULL）

實際上 fast 先走到 6，然後 fast->next == NULL，迴圈結束。
回傳 slow = 4（第二個中間節點）。

[1] -> [2] -> [3] -> [4] -> [5] -> [6] -> NULL
                      s             f(=6，其 next=NULL)

時間複雜度：O(n)
空間複雜度：O(1)
*/
struct ListNode* middleNode(struct ListNode* head) {
    // slow 每次前進一步
    struct ListNode *slow = head;

    // fast 每次前進兩步
    struct ListNode *fast = head;

    // 當 fast 和 fast->next 都不為 NULL 時繼續移動
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;           // slow 前進一步
        fast = fast->next->next;     // fast 前進兩步
    }

    // fast 到達尾端時，slow 剛好在中間（偶數時取第二個中間）
    return slow;
}
