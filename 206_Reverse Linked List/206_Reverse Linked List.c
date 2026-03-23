/*
 * 206. Reverse Linked List
 * 難度：Easy
 *
 * 時間複雜度：O(n) — 遍歷一次串列
 * 空間複雜度：O(1) — 只用常數個指標
 *
 * ===== 演算法概述 =====
 *
 * 使用迭代法（iterative），以三個指標 prev、curr、next 逐步反轉每個節點的
 * next 指標方向。
 *
 * ===== 範例：head = [1, 2, 3, 4, 5] =====
 *
 * 初始狀態：
 *   prev = NULL
 *   curr = 1
 *
 *   NULL   1 --> 2 --> 3 --> 4 --> 5 --> NULL
 *    ^     ^
 *   prev  curr
 *
 * --- 第 1 步 ---
 *   next = curr->next = 2        （先保存下一個節點）
 *   curr->next = prev = NULL     （反轉指標）
 *   prev = curr = 1              （prev 前移）
 *   curr = next = 2              （curr 前移）
 *
 *   NULL <-- 1     2 --> 3 --> 4 --> 5 --> NULL
 *            ^     ^
 *           prev  curr
 *
 * --- 第 2 步 ---
 *   next = curr->next = 3
 *   curr->next = prev = 1
 *   prev = curr = 2
 *   curr = next = 3
 *
 *   NULL <-- 1 <-- 2     3 --> 4 --> 5 --> NULL
 *                  ^     ^
 *                 prev  curr
 *
 * --- 第 3 步 ---
 *   next = curr->next = 4
 *   curr->next = prev = 2
 *   prev = curr = 3
 *   curr = next = 4
 *
 *   NULL <-- 1 <-- 2 <-- 3     4 --> 5 --> NULL
 *                        ^     ^
 *                       prev  curr
 *
 * --- 第 4 步 ---
 *   next = curr->next = 5
 *   curr->next = prev = 3
 *   prev = curr = 4
 *   curr = next = 5
 *
 *   NULL <-- 1 <-- 2 <-- 3 <-- 4     5 --> NULL
 *                              ^     ^
 *                             prev  curr
 *
 * --- 第 5 步 ---
 *   next = curr->next = NULL
 *   curr->next = prev = 4
 *   prev = curr = 5
 *   curr = next = NULL           （迴圈結束）
 *
 *   NULL <-- 1 <-- 2 <-- 3 <-- 4 <-- 5
 *                                    ^
 *                                   prev  (新的 head)
 *
 * 返回 prev = 5，即反轉後的頭節點。
 */

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode* prev = NULL;  /* 前一個節點，初始為 NULL（反轉後的尾巴） */
    struct ListNode* curr = head;  /* 當前節點，從 head 開始 */

    while (curr) {                          /* 遍歷整個串列直到 curr 為 NULL */
        struct ListNode* next = curr->next; /* 暫存下一個節點，避免斷鏈後遺失 */
        curr->next = prev;                  /* 核心操作：反轉指標方向 */
        prev = curr;                        /* prev 前移到當前節點 */
        curr = next;                        /* curr 前移到下一個節點 */
    }

    return prev;  /* 迴圈結束時 prev 指向原串列的最後一個節點，即新的 head */
}
