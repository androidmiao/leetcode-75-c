/*
 * 234. Palindrome Linked List
 * 難度：Easy
 *
 * 題目：給定 singly linked list 的 head，若為回文則回傳 true，否則 false。
 *
 * 方法：Reverse Second Half In-Place（原地反轉後半段）
 * 時間：O(n)   空間：O(1)
 *
 * ═══════════════════════════════════════════════════════════
 *  整體流程（以 [1 → 2 → 2 → 1] 為例）
 * ═══════════════════════════════════════════════════════════
 *
 *  原始串列
 *  head
 *   │
 *  [1] → [2] → [2] → [1] → NULL
 *
 *  Step 1  endOfFirstHalf()：快慢指標找前半段末尾
 *
 *   slow 每次 +1，fast 每次 +2
 *   fast 無法再走 2 步時，slow 停在前半段最後一個節點
 *
 *    初始        迭代1(條件成立)    迭代2(條件不成立→停)
 *    s,f          s     f              s        f
 *    ↓            ↓      ↓              ↓          ↓
 *   [1]→[2]→[2]→[1]  [1]→[2]→[2]→[1]  ── 不再進迴圈
 *
 *   回傳 slow = index1 (val=2) ← firstHalfEnd
 *
 *  Step 2  reverseList(firstHalfEnd->next)：反轉後半段
 *
 *   反轉前：[2] → [1] → NULL
 *   反轉後：[1] → [2] → NULL   ← secondHalfStart = val=1 的節點
 *
 *   整體狀態：
 *   head          firstHalfEnd   secondHalfStart
 *    │                 │               │
 *   [1] → [2] → NULL  [1] → [2] → NULL
 *   └──前半段──┘        └──後半段(已反轉)──┘
 *                 ↑
 *   firstHalfEnd->next 暫時指向 NULL（Step 4 復原）
 *
 *  Step 3  逐節點比較
 *
 *   p1 從 head 走，p2 從 secondHalfStart 走
 *   圈1：p1.val(1) == p2.val(1) ✓
 *   圈2：p1.val(2) == p2.val(2) ✓
 *   p2 → NULL → 結束，result = true
 *
 *  Step 4  復原：reverseList(secondHalfStart) 重新接回
 *
 *   [1] → [2] → [2] → [1] → NULL  ← 與輸入相同 ✓
 *
 *  Step 5  return result
 *
 * ═══════════════════════════════════════════════════════════
 *  奇數長度補充（[1 → 2 → 3 → 2 → 1]）
 * ═══════════════════════════════════════════════════════════
 *
 *   endOfFirstHalf 回傳 index2 (val=3，中間節點)
 *   後半段：[2] → [1]   反轉後：[1] → [2]
 *   比較：1==1 ✓, 2==2 ✓，p2→NULL 停止（中間節點被正確忽略）
 *
 * ═══════════════════════════════════════════════════════════
 *  reverseList 三指標滾動示意
 * ═══════════════════════════════════════════════════════════
 *
 *  輸入：[2] → [1] → NULL
 *
 *  初始：  prev=NULL  curr=[2]  next=?
 *
 *  圈1：next=[1]    curr->[2]→NULL 改為 [2]→prev(NULL)
 *       prev=[2]   curr=[1]
 *
 *       NULL ← [2]    [1] → NULL
 *               ↑      ↑
 *              prev   curr
 *
 *  圈2：next=NULL   curr->[1] 改為 [1]→prev([2])
 *       prev=[1]   curr=NULL
 *
 *       NULL ← [2] ← [1]
 *                      ↑
 *                     prev  ← 新頭，回傳它
 */

#include <stdbool.h>

/* 節點定義 */
struct ListNode {
    int val;                  /* 節點儲存的整數值               */
    struct ListNode *next;    /* 指向下一個節點；尾端節點為 NULL */
};

/*
 * endOfFirstHalf — 快慢指標，回傳前半段的最後一個節點
 *
 * 規律：
 *   偶數 n=2k → slow 停在 index k-1（前半段末尾）
 *   奇數 n=2k+1 → slow 停在 index k（中間節點，留在前半段）
 */
static struct ListNode *endOfFirstHalf(struct ListNode *head) {
    struct ListNode *fast = head; /* fast 一次跳 2 步 */
    struct ListNode *slow = head; /* slow 一次跳 1 步 */
    while (fast->next != NULL && fast->next->next != NULL) {
        /* fast->next != NULL        → fast 後面至少還有 1 個節點 */
        /* fast->next->next != NULL  → fast 後面至少還有 2 個節點 */
        /* 兩者皆真才能安全地讓 fast 跳 2 步                       */
        fast = fast->next->next; /* fast 跳 2 步 */
        slow = slow->next;       /* slow 跳 1 步 */
    }
    return slow; /* 迴圈結束時 slow 指向前半段末尾 */
}

/*
 * reverseList — 原地反轉，回傳新的頭節點
 *
 * 每圈：先存 next，再把 curr->next 反轉指向 prev，然後兩指標各前進一步
 */
static struct ListNode *reverseList(struct ListNode *head) {
    struct ListNode *prev = NULL; /* 反轉後原來的頭節點 next 要指向 NULL */
    struct ListNode *curr = head; /* 當前處理的節點                      */
    while (curr != NULL) {
        struct ListNode *next = curr->next; /* 先暫存，否則斷鏈後找不回來 */
        curr->next = prev;                  /* 把 next 指標反轉，指向前一個節點 */
        prev = curr;                        /* prev 往前移一步 */
        curr = next;                        /* curr 往前移一步（沿原本方向）    */
    }
    /* curr == NULL 代表走完，prev 此時指向原本的最後一個節點 = 新頭 */
    return prev;
}

/*
 * isPalindrome — 主函式
 */
bool isPalindrome(struct ListNode *head) {
    if (head == NULL) return true; /* 空串列視為回文（邊界條件） */

    /* Step 1：找前半段末尾 */
    struct ListNode *firstHalfEnd = endOfFirstHalf(head);

    /* Step 2：反轉後半段，取得反轉後的新頭 */
    struct ListNode *secondHalfStart = reverseList(firstHalfEnd->next);

    /* Step 3：p1 從頭走，p2 從反轉後半段走，逐一比較值 */
    struct ListNode *p1 = head;           /* 從前半段頭部出發     */
    struct ListNode *p2 = secondHalfStart;/* 從後半段反轉頭部出發 */
    bool result = true;
    while (result && p2 != NULL) {
        /* p2 到 NULL 就停（後半段 ≤ 前半段，以較短的後半段為基準） */
        if (p1->val != p2->val) result = false; /* 有任何不等 → 非回文 */
        p1 = p1->next;
        p2 = p2->next;
    }

    /* Step 4：復原串列（再次反轉後半段，重新接回 firstHalfEnd）*/
    firstHalfEnd->next = reverseList(secondHalfStart);

    /* Step 5：回傳比較結果 */
    return result;
}
