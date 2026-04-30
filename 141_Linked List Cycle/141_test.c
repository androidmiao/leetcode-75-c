// 本地測試檔：用來呼叫並驗證 hasCycle()
// 編譯：gcc -o 141_test 141_test.c
// 執行：./141_test

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// LeetCode 上由平台提供，本地測試需自行定義
struct ListNode {
    int val;
    struct ListNode *next;
};

// ===== 你的解法（與 141_Linked List Cycle.c 相同） =====
bool hasCycle(struct ListNode *head) {
    struct ListNode *slow = head;
    struct ListNode *fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            return true;
        }
    }
    return false;
}

// ===== Helper：建立節點 =====
static struct ListNode* newNode(int val) {
    struct ListNode *node = (struct ListNode*)malloc(sizeof(struct ListNode));
    node->val = val;
    node->next = NULL;
    return node;
}

// ===== Helper：用陣列建立 linked list =====
// pos = -1 表示沒有環；否則尾端 next 連回 index = pos 的節點
static struct ListNode* buildList(int arr[], int n, int pos) {
    if (n == 0) return NULL;

    struct ListNode *head = newNode(arr[0]);
    struct ListNode *cur = head;
    struct ListNode *cycleNode = (pos == 0) ? head : NULL;

    for (int i = 1; i < n; i++) {
        cur->next = newNode(arr[i]);
        cur = cur->next;
        if (i == pos) {
            cycleNode = cur;
        }
    }

    if (cycleNode != NULL) {
        cur->next = cycleNode;  // 製造環
    }
    return head;
}

// ===== Helper：釋放沒有環的 list =====
static void freeListNoCycle(struct ListNode *head) {
    while (head != NULL) {
        struct ListNode *tmp = head;
        head = head->next;
        free(tmp);
    }
}

// ===== 測試 =====
int main(void) {
    // Test 1：[3,2,0,-4]，pos=1 → 有環
    int a[] = {3, 2, 0, -4};
    struct ListNode *head1 = buildList(a, 4, 1);
    printf("Test 1 [3,2,0,-4] pos=1  expected=true   got=%s\n",
           hasCycle(head1) ? "true" : "false");

    // Test 2：[1,2]，pos=0 → 有環（連回 head）
    int b[] = {1, 2};
    struct ListNode *head2 = buildList(b, 2, 0);
    printf("Test 2 [1,2]      pos=0  expected=true   got=%s\n",
           hasCycle(head2) ? "true" : "false");

    // Test 3：[1]，pos=-1 → 無環
    int c[] = {1};
    struct ListNode *head3 = buildList(c, 1, -1);
    printf("Test 3 [1]        pos=-1 expected=false  got=%s\n",
           hasCycle(head3) ? "true" : "false");
    freeListNoCycle(head3);

    // Test 4：空 list
    printf("Test 4 []                expected=false  got=%s\n",
           hasCycle(NULL) ? "true" : "false");

    // Test 5：[1,2,3,4,5]，pos=-1 → 無環
    int d[] = {1, 2, 3, 4, 5};
    struct ListNode *head5 = buildList(d, 5, -1);
    printf("Test 5 [1..5]     pos=-1 expected=false  got=%s\n",
           hasCycle(head5) ? "true" : "false");
    freeListNoCycle(head5);

    // 註：Test 1、Test 2 的 list 含環，未釋放以保持簡潔
    return 0;
}
