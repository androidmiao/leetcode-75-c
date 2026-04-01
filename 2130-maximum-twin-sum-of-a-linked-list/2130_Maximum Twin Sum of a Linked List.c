/*
================================================================================
Problem: 2130. Maximum Twin Sum of a Linked List (Medium)
================================================================================

Time Complexity: O(n) - single pass through the list
Space Complexity: O(1) - only using pointers, reversing in place

Algorithm: Reverse Second Half In Place
    1. Use fast/slow pointers to find the middle of the list
    2. Reverse the second half of the list in-place
    3. Traverse both halves simultaneously, comparing twin pairs
    4. Track the maximum sum encountered

================================================================================
DETAILED WALKTHROUGH with Example 1: [5, 4, 2, 1]
================================================================================

PHASE 1 - Find Middle Using Fast/Slow Pointers
────────────────────────────────────────────────

Initial State:
  head = &node[5], fast = head->next->next = &node[2]

  Index:  0   1   2   3
  Value: [5] → [4] → [2] → [1] → NULL
          ^           ^
         slow        fast
        (head)   (head->next->next start)

After Loop Iteration:
  - fast moves 2 steps: node[2] → node[1] (1st step) → NULL (2nd step, fast->next exists)
  - fast->next is NULL, so condition (fast && fast->next) is false → EXIT LOOP
  - slow stays at node[5] after checking condition

  Node Positions:
  [5] → [4] → [2] → [1] → NULL
         ^
        slow (after loop, slow points to index 1)

Result: slow->next points to node[2], which is the START OF SECOND HALF
        (For a 4-node list, nodes at indices 2,3 form the second half)

PHASE 2 - Reverse Second Half Using reverseList(slow->next)
────────────────────────────────────────────────────────────

Input to reverseList: head = node[2]

Before Reversal:
  [2] → [1] → NULL
  ^
  head

Step-by-step Reversal Process:
  1. curr = head->next = node[1]
  2. head->next = NULL (break first node's forward link)
  3. Loop iteration 1:
     - next = curr->next = NULL
     - curr->next = head → node[1]->next = node[2]
     - head = curr → head now points to node[1]
     - next is NULL, so break
  4. Return curr = node[1]

After Reversal (return value becomes new slow):
  [1] → [2] → NULL
  ^
  slow (now points to reversed second half)

First Half Remains:
  [5] → [4] → NULL (these were never modified)

PHASE 3 - Compare Twin Pairs from Both Halves
──────────────────────────────────────────────

Initial:
  head points to: [5] → [4] → [NULL after phase 1 break]
  slow points to: [1] → [2] → NULL

Iteration 1:
  sum = max(5 + 1 = 6, 0) = 6
  head → node[4], slow → node[2]

Iteration 2:
  sum = max(4 + 2 = 6, 6) = 6
  head → NULL, slow → NULL
  Loop exits

Return: 6

================================================================================
Edge Cases:
  - Single node: returns 0 (no twin exists)
  - Two nodes: reverseList on single node returns that node unchanged
  - Odd length: slow->next points to middle+1 (second half starts after middle)
  - Even length: works correctly, both halves have equal length
================================================================================
*/

#include <stdio.h>

/* Definition for singly-linked list node */
struct ListNode {
    int val;
    struct ListNode *next;
};

/*
 * reverseList: Reverse a linked list in-place
 * head: pointer to the head of the sublist to reverse
 * returns: pointer to the new head (last node of original list)
 *
 * Algorithm:
 *   - If head is NULL or has no next node, return unchanged (base cases)
 *   - Break the link from head to curr (head->next = NULL)
 *   - Iterate through remaining nodes: save next, point curr back to head, advance
 *   - Return the final node (which becomes the new head)
 */
inline struct ListNode* reverseList(struct ListNode* head) {
    /* 邊界條件1: 空列表直接返回 | Edge case: empty list */
    if (!head) return head;

    /* 將目前節點的下一個節點設為指針 | Get the next node */
    struct ListNode* curr = head->next;

    /* 邊界條件2: 單個節點的列表不需反轉 | Single node, already reversed */
    if (!curr) return head;

    /* 斷開首節點的連結，避免循環 | Break the link to prevent a cycle */
    head->next = NULL;

    while (1) {
        /* 保存當前節點的後繼，因為下一行會修改 curr->next | Save next node before modifying */
        struct ListNode* next = curr->next;

        /* 將當前節點指向前驅，實現反轉 | Reverse the link: point curr back to head */
        curr->next = head;

        /* 推進 head 指針到已反轉部分的新頭 | Move head forward in reversed section */
        head = curr;

        /* 如果已處理完所有節點，終止迴圈 | If no more nodes, we're done */
        if (!next) break;

        /* 推進 curr 到下一個待反轉節點 | Move curr forward to next unreversed node */
        curr = next;
    }

    /* 返回反轉後列表的新頭（原列表的尾節點）| Return new head of reversed list */
    return curr;
}

/*
 * pairSum: Find the maximum sum of twin pairs in a linked list
 * head: pointer to the head of the linked list
 * returns: the maximum sum of any twin pair (i-th and (n-1-i)-th nodes)
 *
 * Strategy:
 *   1. Use fast (moves 2 steps) and slow (moves 1 step) pointers to find the middle
 *   2. Reverse the second half starting at slow->next
 *   3. Traverse both halves: first half from head, second half reversed from new slow
 *   4. Compare each pair and track the maximum sum
 */
int pairSum(struct ListNode* head) {
    /* 邊界條件：空列表或單節點列表沒有twins | Edge case: no twins exist */
    if (!head || !head->next) return 0;

    /* 快指針初始化在 head->next->next，每次跳2步 | Fast pointer moves 2 steps per iteration */
    struct ListNode* fast = head->next->next;

    /* 慢指針初始化在 head，每次跳1步，用於找到中點 | Slow pointer moves 1 step, finds middle */
    struct ListNode* slow = head;

    /* 用於記錄最大 twins 和，初始化為0 | Track maximum twin sum found so far */
    int sum = 0;

    /*
     * 迴圈不變式: 快指針走兩倍的距離，慢指針走一倍
     * 當快指針到達尾部時，慢指針恰好在中點
     * Invariant: when fast reaches end, slow is at middle
     * Exit when: fast is NULL or fast->next is NULL (even or odd length list)
     */
    while (fast && fast->next) {
        /* 快指針跳兩個節點 | Advance fast by 2 nodes */
        fast = fast->next->next;

        /* 慢指針跳一個節點 | Advance slow by 1 node */
        slow = slow->next;
    }

    /*
     * slow 現在指向第一半的最後一個節點
     * slow->next 指向第二半的第一個節點
     * Reverse the second half starting at slow->next
     * After reversal, slow points to the new head of reversed second half
     */
    slow = reverseList(slow->next);

    /*
     * 現在 head 指向第一半開始，slow 指向反轉後的第二半開始
     * 同時遍歷兩半，比較對應的 twins
     * Traverse both halves: first half from head, reversed second half from slow
     * Both pointers move forward, comparing values at each step
     */
    while (slow) {
        /* 計算當前 twins 的和 | Calculate twin sum */
        int new_sum = slow->val + head->val;

        /*
         * 更新最大值：如果新和大於目前最大和就更新，否則保持原值
         * 使用三元運算子避免分支，可能有微小性能優勢
         * Update maximum if new sum is larger
         */
        sum = new_sum > sum ? new_sum : sum;

        /* 移動指針到下一對 twins | Advance to next pair */
        slow = slow->next;
        head = head->next;
    }

    /* 返回所有 twins 對中的最大和 | Return the maximum twin sum found */
    return sum;
}
