/*
 * LeetCode 23. 合併 k 個已排序的鏈結串列
 * 難度：Hard
 *
 * 問題描述：
 * 給定k個排序鏈結串列數組，將所有鏈結串列合併為一個排序鏈結串列。
 *
 * 演算法：分治法 (Divide and Conquer)
 *
 * 核心思想：
 * - 將k個鏈結串列分為兩組
 * - 遞迴地合併每組
 * - 合併兩個已排序的鏈結串列
 *
 * 時間複雜度：O(N log k)，其中N是所有元素總數
 * 空間複雜度：O(log k)，遞迴棧深度
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

/* 合併兩個排序鏈結串列 */
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = NULL;

    struct ListNode* current = &dummy;

    while (l1 && l2) {
        if (l1->val < l2->val) {
            current->next = l1;
            l1 = l1->next;
        } else {
            current->next = l2;
            l2 = l2->next;
        }
        current = current->next;
    }

    if (l1) current->next = l1;
    else current->next = l2;

    return dummy.next;
}

/* 遞迴合併k個鏈結串列 */
struct ListNode* mergeKListsHelper(struct ListNode** lists, int left, int right) {
    if (left == right) {
        return lists[left];
    }
    if (left > right) {
        return NULL;
    }

    int mid = left + (right - left) / 2;
    struct ListNode* l1 = mergeKListsHelper(lists, left, mid);
    struct ListNode* l2 = mergeKListsHelper(lists, mid + 1, right);

    return mergeTwoLists(l1, l2);
}

struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    if (listsSize == 0) return NULL;
    return mergeKListsHelper(lists, 0, listsSize - 1);
}
