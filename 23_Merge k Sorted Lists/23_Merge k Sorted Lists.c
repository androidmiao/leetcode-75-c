/*
 * LeetCode 23. Merge k Sorted Lists (合併 K 個升序鏈結串列)
 * 難度：Hard
 * 時間複雜度：O(N log k)，其中 N 是所有節點總數，k 是鏈結串列個數
 * 空間複雜度：O(log k)，遞迴堆疊深度
 *
 * 演算法：分治法 (Divide and Conquer) -- 對應 Editorial Approach 5
 *
 * 核心思想：
 *   將 k 個鏈結串列遞迴地兩兩配對合併，形成類似歸併排序的二元樹結構。
 *   每一層合併的總工作量為 O(N)，共 log k 層，因此總時間為 O(N log k)。
 *
 * 演算法步驟（以 Example 1 為例）：
 *   輸入: lists = [[1,4,5], [1,3,4], [2,6]]  (k=3)
 *
 *   Step 1: 遞迴拆分
 *     mergeKListsHelper(lists, 0, 2)
 *       +-- mergeKListsHelper(lists, 0, 1)     [左半部分]
 *       |     +-- lists[0]: 1->4->5            [left==mid, 直接回傳]
 *       |     +-- lists[1]: 1->3->4            [mid+1==right, 直接回傳]
 *       |     +-- mergeTwoLists(1->4->5, 1->3->4)
 *       |           = 1->1->3->4->4->5
 *       +-- mergeKListsHelper(lists, 2, 2)     [右半部分]
 *             +-- lists[2]: 2->6               [left==right, 直接回傳]
 *
 *   Step 2: 最終合併
 *     mergeTwoLists(1->1->3->4->4->5, 2->6)
 *       = 1->1->2->3->4->4->5->6
 *
 * ASCII 圖解 -- 分治合併過程：
 *
 *   初始狀態 (k=3 個鏈結串列):
 *     lists[0]: 1 -> 4 -> 5
 *     lists[1]: 1 -> 3 -> 4
 *     lists[2]: 2 -> 6
 *
 *   分治樹：
 *                    merge(0,2)
 *                   /          \
 *            merge(0,1)      merge(2,2)
 *            /       \           |
 *       lists[0]  lists[1]   lists[2]
 *       1->4->5   1->3->4    2->6
 *
 *   第一層合併 -- mergeTwoLists(lists[0], lists[1]):
 *
 *     l1: 1 -> 4 -> 5       l2: 1 -> 3 -> 4
 *         ^                      ^
 *     dummy -> [ ]
 *               ^curr
 *
 *     比較 l1(1) vs l2(1): l1 < l2? 否, 取 l2
 *     dummy -> [1] -> [ ]
 *     l1: 1->4->5   l2: 3->4
 *         ^               ^
 *
 *     比較 l1(1) vs l2(3): 取 l1
 *     dummy -> [1] -> [1] -> [ ]
 *     l1: 4->5   l2: 3->4
 *         ^            ^
 *
 *     比較 l1(4) vs l2(3): 取 l2
 *     dummy -> [1] -> [1] -> [3] -> [ ]
 *     l1: 4->5   l2: 4
 *         ^            ^
 *
 *     比較 l1(4) vs l2(4): 取 l2
 *     dummy -> [1] -> [1] -> [3] -> [4] -> [ ]
 *     l1: 4->5   l2: (nil)
 *         ^
 *
 *     l2 為空, 接上 l1 剩餘部分
 *     結果: 1 -> 1 -> 3 -> 4 -> 4 -> 5
 *
 *   第二層合併 -- mergeTwoLists(上面結果, lists[2]):
 *
 *     l1: 1->1->3->4->4->5    l2: 2->6
 *
 *     逐步取最小: 1, 1, 2, 3, 4, 4, 5, 6
 *     最終結果: 1 -> 1 -> 2 -> 3 -> 4 -> 4 -> 5 -> 6
 */

#include <stdlib.h>

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

/* mergeTwoLists: 合併兩個已排序鏈結串列，使用 dummy head 技巧簡化邊界處理 */
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode dummy;      /* 哨兵節點，避免處理頭節點的特殊情況 */
    dummy.val = 0;
    dummy.next = NULL;

    struct ListNode* current = &dummy;  /* current 指向已合併鏈結串列的尾部 */

    while (l1 && l2) {
        if (l1->val < l2->val) {        /* l1 當前值較小，接到結果鏈結串列 */
            current->next = l1;
            l1 = l1->next;              /* l1 前進一步 */
        } else {                        /* l2 當前值 <= l1，接到結果鏈結串列 */
            current->next = l2;
            l2 = l2->next;              /* l2 前進一步 */
        }
        current = current->next;        /* 結果鏈結串列尾指標前進 */
    }

    /* 其中一個鏈結串列已遍歷完，將另一個的剩餘部分直接接到結果末尾 */
    if (l1) current->next = l1;
    else current->next = l2;

    return dummy.next;                  /* dummy.next 即為合併後鏈結串列的真正頭節點 */
}

/*
 * mergeKListsHelper: 遞迴分治合併
 *   將 lists[left..right] 範圍的鏈結串列遞迴地分成兩半，
 *   分別合併後再將兩個結果合併。
 *
 * 不變量：
 *   - left <= right 時，lists[left..right] 中至少有一個鏈結串列
 *   - left == right 時，只有一個鏈結串列，直接回傳（遞迴基底）
 *   - left > right 時，範圍為空，回傳 NULL
 */
struct ListNode* mergeKListsHelper(struct ListNode** lists, int left, int right) {
    if (left == right) {                /* 遞迴基底：只有一個鏈結串列，直接回傳 */
        return lists[left];
    }
    if (left > right) {                 /* 無效範圍，回傳空 */
        return NULL;
    }

    int mid = left + (right - left) / 2;  /* 防溢位的中點計算 */

    /* 遞迴合併左半部分 lists[left..mid] */
    struct ListNode* l1 = mergeKListsHelper(lists, left, mid);
    /* 遞迴合併右半部分 lists[mid+1..right] */
    struct ListNode* l2 = mergeKListsHelper(lists, mid + 1, right);

    /* 將左右兩部分的合併結果再合併為一個有序鏈結串列 */
    return mergeTwoLists(l1, l2);
}

/* mergeKLists: 主函式入口，處理 k=0 的邊界情況後呼叫分治輔助函式 */
struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    if (listsSize == 0) return NULL;    /* 空陣列，直接回傳 NULL */
    return mergeKListsHelper(lists, 0, listsSize - 1);
}
