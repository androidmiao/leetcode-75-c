// https://leetcode.com/problems/merge-two-sorted-lists/description/?envType=problem-list-v2&envId=wl94y6ih

/*
題目圖解：合併兩個已排序 linked list

範例：
list1: 1 -> 2 -> 4 -> NULL
list2: 1 -> 3 -> 4 -> NULL

一開始先建立虛擬頭節點 dummy：

dummy -> NULL
tail

list1 -> 1 -> 2 -> 4 -> NULL
list2 -> 1 -> 3 -> 4 -> NULL

--------------------------------------------------
第 1 次比較：list1=1, list2=1
因為 list1->val <= list2->val，所以先接 list1

dummy -> 1 -> 2 -> 4 -> NULL
         tail

list1         -> 2 -> 4 -> NULL
list2 -> 1 -> 3 -> 4 -> NULL

--------------------------------------------------
第 2 次比較：list1=2, list2=1
這次 list2 較小，所以接上 list2

dummy -> 1 -> 1 -> 3 -> 4 -> NULL
              tail

list1 -> 2 -> 4 -> NULL
list2      -> 3 -> 4 -> NULL

--------------------------------------------------
第 3 次比較：list1=2, list2=3
這次 list1 較小，所以接上 list1

dummy -> 1 -> 1 -> 2 -> 4 -> NULL
                   tail

list1      -> 4 -> NULL
list2 -> 3 -> 4 -> NULL

--------------------------------------------------
第 4 次比較：list1=4, list2=3
這次 list2 較小，所以接上 list2

dummy -> 1 -> 1 -> 2 -> 3 -> 4 -> NULL
                        tail

list1 -> 4 -> NULL
list2      -> 4 -> NULL

--------------------------------------------------
第 5 次比較：list1=4, list2=4
因為 list1->val <= list2->val，所以先接 list1

dummy -> 1 -> 1 -> 2 -> 3 -> 4 -> NULL
                             tail

list1 -> NULL
list2 -> 4 -> NULL

--------------------------------------------------
迴圈結束：
list1 已經是 NULL，因此直接把 list2 剩下的部分接到 tail 後面

dummy -> 1 -> 1 -> 2 -> 3 -> 4 -> 4 -> NULL

最後回傳：
dummy.next

也就是：
1 -> 1 -> 2 -> 3 -> 4 -> 4 -> NULL
*/

//Definition for singly-linked list.
 struct ListNode {
     int val;
     struct ListNode *next;
};

struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2) {
    // 建立一個虛擬頭節點，方便統一處理合併流程，避免一開始就要特別判斷頭節點。
    struct ListNode dummy;

    // `tail` 會永遠指向目前合併後串列的最後一個節點。
    struct ListNode* tail = &dummy;

    // 先把虛擬頭節點的 next 設成 NULL，代表目前合併結果還是空的。
    dummy.next = NULL;

    // 當兩個串列都還有節點時，持續比較目前節點值並接上較小的那一個。
    while (list1 != NULL && list2 != NULL) {
        // 如果 list1 目前節點的值比較小或相等，就先接上 list1 的節點。
        if (list1->val <= list2->val) {
            // 把合併結果的尾端接到 list1 目前這個節點。
            tail->next = list1;

            // list1 往後移動一格，準備比較下一個節點。
            list1 = list1->next;
        } else {
            // 否則就接上 list2 目前這個較小的節點。
            tail->next = list2;

            // list2 往後移動一格，準備比較下一個節點。
            list2 = list2->next;
        }

        // 無論剛剛接的是哪個節點，tail 都要往後移到新的尾端。
        tail = tail->next;
    }

    // 走到這裡時，至少有一個串列已經走完。
    // 因為剩下的另一個串列本來就已排序，所以直接整段接到尾端即可。
    if (list1 != NULL) {
        // 如果 list1 還有剩，直接接上 list1 剩餘部分。
        tail->next = list1;
    } else {
        // 否則就接上 list2 剩餘部分，包含 list2 可能是 NULL 的情況。
        tail->next = list2;
    }

    // 回傳真正的頭節點，需跳過一開始用來輔助操作的虛擬頭節點。
    return dummy.next;
}
