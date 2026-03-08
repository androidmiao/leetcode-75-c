#include <stdlib.h>

// 每個節點代表一次 ping 請求，裡面記錄該次請求的時間。
typedef struct Node {
    int time;          // 這筆請求發生的時間 t
    struct Node* next; // 指向下一個節點
} Node;

// RecentCounter 用 linked list 來模擬 queue。
// head 是前端，tail 是尾端，size 記錄目前有效請求數量。
typedef struct {
    Node* head;  // 指向目前 queue 最前面的節點
    Node* tail;  // 指向目前 queue 最後面的節點
    int size;    // 目前 queue 中有效節點的數量
} RecentCounter;

RecentCounter* recentCounterCreate() {
    // 配置一個 RecentCounter 結構體的記憶體空間
    RecentCounter* obj = (RecentCounter*)malloc(sizeof(RecentCounter));

    // 一開始 queue 是空的，所以 head 和 tail 都設成 NULL
    obj->head = NULL;
    obj->tail = NULL;

    // 一開始沒有任何請求，因此大小為 0
    obj->size = 0;

    // 回傳建立好的物件
    return obj;
}

int recentCounterPing(RecentCounter* obj, int t) {
    // 為這次新的 ping 建立一個節點
    Node* node = (Node*)malloc(sizeof(Node));

    // 把目前請求時間存進節點
    node->time = t;

    // 新節點會放在 linked list 的尾端，所以 next 先設成 NULL
    node->next = NULL;

    // 如果目前 queue 是空的，代表這是第一個節點
    if (obj->tail == NULL) {
        // 第一個節點同時是 head 和 tail
        obj->head = node;
        obj->tail = node;
    } else {
        // 否則就把新節點接到目前 tail 的後面
        obj->tail->next = node;

        // 然後更新 tail，讓它指向新的最後一個節點
        obj->tail = node;
    }

    // 新增一筆有效請求後，數量加 1
    obj->size++;

    // 把所有不在 [t - 3000, t] 範圍內的舊節點移除
    // 題目保證 t 會遞增，所以過期節點一定只會出現在 head 端
    while (obj->head != NULL && obj->head->time < t - 3000) {
        // 先記住目前要刪除的舊節點
        Node* expired = obj->head;

        // 把 head 往後移一格，表示移除最前面的節點
        obj->head = obj->head->next;

        // 釋放已過期節點的記憶體，避免 memory leak
        free(expired);

        // 有效請求數量減 1
        obj->size--;
    }

    // 如果移除後 head 變成 NULL，代表整個 queue 已空
    if (obj->head == NULL) {
        // 此時 tail 也要一起設成 NULL，保持結構一致
        obj->tail = NULL;
    }

    // 回傳目前視窗內有效請求的數量
    return obj->size;
}

void recentCounterFree(RecentCounter* obj) {
    // 從 linked list 的頭開始逐一釋放所有剩餘節點
    Node* current = obj->head;

    // 只要還有節點，就持續往後釋放
    while (current != NULL) {
        // 先記住下一個節點，避免 free 後遺失鏈結
        Node* next = current->next;

        // 釋放目前節點
        free(current);

        // 繼續處理下一個節點
        current = next;
    }

    // 最後再釋放 RecentCounter 本身
    free(obj);
}
