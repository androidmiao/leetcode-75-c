//https://leetcode.com/problems/lru-cache/description/?envType=problem-list-v2&envId=wl94y6ih 
/*
 * 146. LRU Cache（最近最少使用快取）
 * 難度：Medium
 * 時間複雜度：get/put 均 O(1) 平攤
 * 空間複雜度：O(capacity)
 *
 * ═══════════════════════════════════════════════════════════════════
 * 演算法：雙向鏈結串列（Doubly Linked List）+ 雜湊表（Hash Map）
 * ═══════════════════════════════════════════════════════════════════
 *
 * 核心想法
 * --------
 * LRU 快取需要兩個操作各 O(1)：
 *   get(key)  ：查找 key 的值，同時將其移至「最近使用」端
 *   put(key,v)：插入/更新，若超出容量則淘汰最久未用的節點
 *
 * 資料結構設計：
 *   1. 雙向鏈結串列：維護存取順序，使用 sentinel（哨兵）節點簡化邊界處理
 *      head（哨兵）←→ 最舊節點 ←→ ... ←→ 最新節點 ←→ tail（哨兵）
 *   2. 雜湊表（鏈式雜湊 / separate chaining）：key → 節點指標，O(1) 查找
 *      使用鏈結串列處理碰撞，避免不同 key 映射到同一 bucket 時互相覆蓋
 *
 * ═══════════════════════════════════════════════════════════════════
 * ASCII 示意圖（capacity = 2）
 * ═══════════════════════════════════════════════════════════════════
 *
 * 初始狀態：
 *   head ←→ tail    （空快取，兩個哨兵互指）
 *
 * put(1,1):
 *   head ←→ [1,1] ←→ tail
 *   hash_table: bucket[h(1)] → (1,1)
 *
 * put(2,2):
 *   head ←→ [1,1] ←→ [2,2] ←→ tail
 *   hash_table: bucket[h(1)] → (1,1), bucket[h(2)] → (2,2)
 *
 * get(1) → 回傳 1，並將 [1,1] 移到最近端：
 *   head ←→ [2,2] ←→ [1,1] ←→ tail
 *   （先 remove [1,1]，再 insertBeforeTail [1,1]）
 *
 * put(3,3) → 超出容量，淘汰最舊的 [2,2]：
 *   step 1: 移除 head->next = [2,2]
 *   step 2: 從 hash_table 刪除 key=2
 *   step 3: 插入 [3,3] 到 tail 前
 *   head ←→ [1,1] ←→ [3,3] ←→ tail
 *
 * get(2) → 回傳 -1（已被淘汰）
 *
 * ═══════════════════════════════════════════════════════════════════
 * 雜湊表碰撞處理（Separate Chaining）示意：
 * ═══════════════════════════════════════════════════════════════════
 *
 *   bucket[0] → NULL
 *   bucket[1] → [HashNode key=1] → [HashNode key=10008] → NULL
 *   bucket[2] → [HashNode key=2] → NULL
 *   ...
 *
 *   每個 bucket 是一條單向鏈結串列（HashNode），
 *   存放所有 hash(key) 相同的 key-node 配對。
 *   查找、插入、刪除都在該 bucket 的短鏈上線性掃描。
 */

#include <stdlib.h>
#include <string.h>

/* 雜湊表大小（質數，降低碰撞機率；key 範圍 0~10^4，3000 capacity） */
#define HASH_SIZE 10007

/* ─── 雙向鏈結串列節點：儲存快取的 key-value ─── */
typedef struct Node {
    int key;
    int val;
    struct Node *prev;  /* 指向更舊的節點 */
    struct Node *next;  /* 指向更新的節點 */
} Node;

/* ─── 雜湊表節點：處理碰撞用的單向鏈結串列 ─── */
typedef struct HashNode {
    int key;                /* 此條目對應的 key */
    Node *node;             /* 指向雙向鏈結串列中的實際節點 */
    struct HashNode *next;  /* 同一 bucket 中的下一個條目（碰撞鏈） */
} HashNode;

typedef struct {
    int capacity;               /* 快取容量上限 */
    int size;                   /* 目前快取中的元素數量 */
    Node *head;                 /* 哨兵頭節點（head->next 是最舊節點） */
    Node *tail;                 /* 哨兵尾節點（tail->prev 是最新節點） */
    HashNode *table[HASH_SIZE]; /* 雜湊表：每個 bucket 是 HashNode 鏈結串列 */
} LRUCache;

/* ─── 雜湊函式：將 key 映射到 [0, HASH_SIZE) ─── */
static int hash(int key) {
    return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
}

/* ─── 雜湊表操作：查找 key 對應的 Node* ─── */
static Node* hashFind(LRUCache *obj, int key) {
    int h = hash(key);
    HashNode *cur = obj->table[h];  /* 從 bucket 頭開始遍歷碰撞鏈 */
    while (cur) {
        if (cur->key == key) return cur->node;  /* 找到：回傳對應的鏈結串列節點 */
        cur = cur->next;
    }
    return NULL;  /* 未找到 */
}

/* ─── 雜湊表操作：插入 key → node 映射 ─── */
static void hashInsert(LRUCache *obj, int key, Node *node) {
    int h = hash(key);
    HashNode *entry = (HashNode*)malloc(sizeof(HashNode));
    entry->key = key;
    entry->node = node;
    entry->next = obj->table[h];  /* 新條目插入到 bucket 鏈頭（O(1)） */
    obj->table[h] = entry;
}

/* ─── 雜湊表操作：刪除 key 對應的條目 ─── */
static void hashRemove(LRUCache *obj, int key) {
    int h = hash(key);
    HashNode *cur = obj->table[h];
    HashNode *prev = NULL;
    while (cur) {
        if (cur->key == key) {
            /* 找到目標：從碰撞鏈中摘除 */
            if (prev) prev->next = cur->next;
            else obj->table[h] = cur->next;  /* 目標是鏈頭 */
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

/* ─── 鏈結串列操作：從雙向鏈結串列中移除節點（不釋放記憶體） ─── */
static void removeNode(Node *node) {
    node->prev->next = node->next;  /* 前驅跳過自己，指向後繼 */
    node->next->prev = node->prev;  /* 後繼跳過自己，指向前驅 */
}

/* ─── 鏈結串列操作：將節點插入到 tail 哨兵之前（標記為「最近使用」） ─── */
static void insertBeforeTail(LRUCache *obj, Node *node) {
    node->prev = obj->tail->prev;   /* 新節點的前驅 = 原本的最新節點 */
    node->next = obj->tail;         /* 新節點的後繼 = tail 哨兵 */
    obj->tail->prev->next = node;   /* 原最新節點的後繼改指向新節點 */
    obj->tail->prev = node;         /* tail 哨兵的前驅改指向新節點 */
}

/* ═══ LRUCache 建構函式 ═══ */
LRUCache* lRUCacheCreate(int capacity) {
    LRUCache *obj = (LRUCache*)malloc(sizeof(LRUCache));
    obj->capacity = capacity;
    obj->size = 0;

    /* 建立哨兵節點，簡化邊界處理（不需特判空串列） */
    obj->head = (Node*)malloc(sizeof(Node));
    obj->tail = (Node*)malloc(sizeof(Node));
    obj->head->prev = NULL;
    obj->head->next = obj->tail;  /* head → tail：初始為空 */
    obj->tail->prev = obj->head;  /* tail → head */
    obj->tail->next = NULL;

    /* 初始化雜湊表所有 bucket 為 NULL */
    memset(obj->table, 0, sizeof(obj->table));
    return obj;
}

/* ═══ get 操作 ═══ */
int lRUCacheGet(LRUCache* obj, int key) {
    Node *node = hashFind(obj, key);  /* O(1) 查找 */
    if (!node) return -1;             /* key 不存在，回傳 -1 */

    /* key 存在：移至最近使用端（先移除再插入到 tail 前） */
    removeNode(node);
    insertBeforeTail(obj, node);
    return node->val;
}

/* ═══ put 操作 ═══ */
void lRUCachePut(LRUCache* obj, int key, int value) {
    Node *node = hashFind(obj, key);

    if (node) {
        /* key 已存在：更新值，並移至最近使用端 */
        node->val = value;
        removeNode(node);
        insertBeforeTail(obj, node);
    } else {
        /* key 不存在：建立新節點並插入 */
        Node *newNode = (Node*)malloc(sizeof(Node));
        newNode->key = key;
        newNode->val = value;

        hashInsert(obj, key, newNode);     /* 加入雜湊表 */
        insertBeforeTail(obj, newNode);    /* 加入鏈結串列尾端（最近使用） */
        obj->size++;

        /* 若超出容量：淘汰最久未用的節點（head 哨兵後的第一個節點） */
        if (obj->size > obj->capacity) {
            Node *lru = obj->head->next;   /* 最舊節點 = head->next */
            removeNode(lru);               /* 從鏈結串列移除 */
            hashRemove(obj, lru->key);     /* 從雜湊表移除 */
            free(lru);                     /* 釋放記憶體 */
            obj->size--;
        }
    }
}

/* ═══ 解構函式：釋放所有記憶體 ═══ */
void lRUCacheFree(LRUCache* obj) {
    /* 釋放雙向鏈結串列中所有資料節點 */
    Node *cur = obj->head->next;
    while (cur != obj->tail) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    /* 釋放哨兵節點 */
    free(obj->head);
    free(obj->tail);

    /* 釋放雜湊表中所有 HashNode */
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *entry = obj->table[i];
        while (entry) {
            HashNode *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(obj);
}
