//https://leetcode.com/problems/linked-list-cycle/description/


/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

/*
解題思路：
1. 使用兩個指標：
   - slow 每次走 1 步
   - fast 每次走 2 步
2. 如果 linked list 有環，fast 最後一定會追上 slow。
3. 如果 linked list 沒有環，fast 或 fast->next 會先變成 NULL。

圖解 1：沒有環的情況

head
 |
 v
[3] -> [2] -> [0] -> [-4] -> NULL

初始：
slow = head
fast = head

第 1 輪後：
slow 走 1 步，停在 [2]
fast 走 2 步，停在 [0]

[3] -> [2] -> [0] -> [-4] -> NULL
        s      f

第 2 輪後：
slow 走到 [0]
fast 會嘗試往後走兩步，但會先遇到 NULL

[3] -> [2] -> [0] -> [-4] -> NULL
               s

所以沒有環，回傳 false。

圖解 2：有環的情況

head
 |
 v
[3] -> [2] -> [0] -> [-4]
        ^              |
        |______________|

這表示尾端節點 [-4] 的 next 會連回 [2]，形成一個環。

初始：
slow = [3]
fast = [3]

第 1 輪後：
slow = [2]
fast = [0]

[3] -> [2] -> [0] -> [-4]
        ^   s    f      |
        |_______________|

第 2 輪後：
slow = [0]
fast = [2]

[3] -> [2] -> [0] -> [-4]
        ^   f    s      |
        |_______________|

第 3 輪後：
slow = [-4]
fast = [-4]

[3] -> [2] -> [0] -> [-4]
        ^              sf
        |_______________|

當 slow 和 fast 指向同一個節點時，就代表 linked list 有環。

時間複雜度：O(n)
空間複雜度：O(1)
*/
bool hasCycle(struct ListNode *head) {
    // slow 指標一次走一步，初始位置在 head
    struct ListNode *slow = head;

    // fast 指標一次走兩步，初始位置也在 head
    struct ListNode *fast = head;

    // 只要 fast 還能繼續往前兩步，就持續檢查
    while (fast != NULL && fast->next != NULL) {
        // slow 前進一步
        slow = slow->next;

        // fast 前進兩步
        fast = fast->next->next;

        // 如果兩個指標相遇，代表 linked list 中存在環
        if (slow == fast) {
            // 找到環，回傳 true
            return true;
        }
    }

    // 能走到這裡，代表 fast 已經遇到 NULL，表示沒有環
    return false;
}
