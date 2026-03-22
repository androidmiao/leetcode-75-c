/*
 * 146. LRU 快取
 * 難度：Medium
 * 時間複雜度：get/put 均 O(1) 平攤
 * 空間複雜度：O(capacity)
 *
 * ─────────────────────────────────────────────────────────────────
 * 演算法：雙向鏈結串列 + 雜湊表
 * ─────────────────────────────────────────────────────────────────
 *
 * 核心想法
 * --------
 * LRU（最近最少使用）快取需要兩個操作各 O(1)：
 *   get(key)：查找 key 的值，同時將其移至「最近使用」端
 *   put(key, value)：插入/更新，若超出容量則淘汰最久未用的節點
 *
 * 資料結構設計：
 *   雙向鏈結串列：維護存取順序
 *     head（虛擬）← ... 較舊節點 ... ← ... 較新節點 ... → tail（虛擬）
 *   雜湊表（陣列）：key → 節點指標，O(1) 查找
 *
 * ASCII 示意圖（capacity = 2）：
 *
 *   put(1,1): head ↔ [1,1] ↔ tail
 *   put(2,2): head ↔ [1,1] ↔ [2,2] ↔ tail
 *   get(1):   head ↔ [2,2] ↔ [1,1] ↔ tail  （1 移至最近）
 *   put(3,3): 淘汰 head 後第一個（最久未用=2）
 *             head ↔ [1,1] ↔ [3,3] ↔ tail
 */

#include <stdlib.h>

#define HASH_SIZE 10007  /* 雜湊表大小（質數，覆蓋 key 範圍 0~10^4） */

typedef struct Node {
    int key;
    int val;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    int capacity;           /* 快取容量 */
    int size;               /* 目前元素數量 */
    Node *head;             /* 虛擬頭節點（緊接著最舊節點） */
    Node *tail;             /* 虛擬尾節點（緊接著最新節點） */
    Node *table[HASH_SIZE]; /* 雜湊表：key → Node* */
} LRUCache;

/* 雜湊函式：將 key 映射到 table 索引 */
static int hash(int key) {
    return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
}

/* 從鏈結串列中移除節點（不釋放記憶體） */
static void removeNode(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/* 將節點插入到尾節點之前（最近使用端） */
static void insertBeforeTail(LRUCache *obj, Node *node) {
    node->prev = obj->tail->prev;
    node->next = obj->tail;
    obj->tail->prev->next = node;
    obj->tail->prev = node;
}

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache *obj = (LRUCache*)malloc(sizeof(LRUCache));
    obj->capacity = capacity;
    obj->size = 0;
    obj->head = (Node*)malloc(sizeof(Node)); /* 虛擬頭 */
    obj->tail = (Node*)malloc(sizeof(Node)); /* 虛擬尾 */
    obj->head->prev = NULL;
    obj->head->next = obj->tail;
    obj->tail->prev = obj->head;
    obj->tail->next = NULL;
    /* 初始化雜湊表為 NULL */
    for (int i = 0; i < HASH_SIZE; i++) obj->table[i] = NULL;
    return obj;
}

int lRUCacheGet(LRUCache* obj, int key) {
    int h = hash(key);
    Node *node = obj->table[h]; /* 查找節點 */
    if (!node) return -1;       /* 不存在 */
    /* 找到後移至最近使用端 */
    removeNode(node);
    insertBeforeTail(obj, node);
    return node->val;
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    int h = hash(key);
    Node *node = obj->table[h];
    if (node) {
        /* key 已存在：更新值並移至最近端 */
        node->val = value;
        removeNode(node);
        insertBeforeTail(obj, node);
    } else {
        /* key 不存在：建立新節點 */
        Node *newNode = (Node*)malloc(sizeof(Node));
        newNode->key = key;
        newNode->val = value;
        obj->table[h] = newNode;
        insertBeforeTail(obj, newNode);
        obj->size++;
        if (obj->size > obj->capacity) {
            /* 超出容量：淘汰最久未用節點（head 後第一個） */
            Node *lru = obj->head->next;
            removeNode(lru);
            obj->table[hash(lru->key)] = NULL;
            free(lru);
            obj->size--;
        }
    }
}

void lRUCacheFree(LRUCache* obj) {
    Node *cur = obj->head->next;
    while (cur != obj->tail) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    free(obj->head);
    free(obj->tail);
    free(obj);
}
