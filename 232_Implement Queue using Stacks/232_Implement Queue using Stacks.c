/*
 * LeetCode 232. Implement Queue using Stacks（用棧實現隊列）
 * 難度：Easy
 * 演算法：兩個棧 + 延遲轉移（Lazy Transfer）
 * 時間：所有操作攤還 O(1)
 * 空間：O(n)
 *
 * ============================================================
 * 核心概念：
 * ============================================================
 * 隊列是 FIFO（先進先出），棧是 LIFO（後進先出）。
 * 使用兩個棧可以反轉元素順序，達到 FIFO 效果。
 *
 * inbox  棧：專門接收 push 進來的新元素
 * outbox 棧：專門提供 pop/peek 的元素
 *
 * 關鍵：只有當 outbox 為空時，才將 inbox 的全部元素
 *       倒入 outbox。這就是「延遲轉移」策略。
 *
 * ============================================================
 * 範例流程：push(1), push(2), peek(), pop(), push(3), pop()
 * ============================================================
 *
 * 步驟 1：push(1)
 *   inbox:  [1]      outbox: []
 *            ^top
 *
 * 步驟 2：push(2)
 *   inbox:  [1, 2]   outbox: []
 *                ^top
 *
 * 步驟 3：peek() → 需要查看隊列前端
 *   outbox 為空 → 觸發轉移！
 *   將 inbox 的元素逐一彈出並推入 outbox：
 *
 *   轉移前：
 *     inbox:  [1, 2]   outbox: []
 *                  ^top
 *   彈出 2 → 推入 outbox：
 *     inbox:  [1]      outbox: [2]
 *              ^top              ^top
 *   彈出 1 → 推入 outbox：
 *     inbox:  []       outbox: [2, 1]
 *                                  ^top
 *
 *   轉移後：
 *     inbox:  []       outbox: [2, 1]
 *                                  ^top
 *   ┌─────────────────────────────────────┐
 *   │  inbox          outbox              │
 *   │  ┌───┐          ┌───┐              │
 *   │  │   │  empty   │ 1 │ ← top (前端) │
 *   │  │   │          ├───┤              │
 *   │  │   │          │ 2 │   (後端)      │
 *   │  └───┘          └───┘              │
 *   └─────────────────────────────────────┘
 *   peek() 返回 outbox 頂部 = 1 ✓
 *
 * 步驟 4：pop() → 從 outbox 彈出
 *   outbox 非空 → 不需轉移
 *   彈出 outbox 頂部 = 1
 *     inbox:  []       outbox: [2]
 *                               ^top
 *   pop() 返回 1 ✓
 *
 * 步驟 5：push(3)
 *   inbox:  [3]       outbox: [2]
 *            ^top               ^top
 *
 * 步驟 6：pop() → 從 outbox 彈出
 *   outbox 非空 → 不需轉移
 *   彈出 outbox 頂部 = 2
 *     inbox:  [3]      outbox: []
 *              ^top
 *   pop() 返回 2 ✓
 *
 * ============================================================
 * 攤還分析：
 * ============================================================
 * 每個元素最多被推入 inbox 一次、從 inbox 彈出一次、
 * 推入 outbox 一次、從 outbox 彈出一次 = 共 4 次操作。
 * 因此 n 個元素總共 O(4n) = O(n)，每操作攤還 O(1)。
 */

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* inbox;       /* 入棧：用於接收 push 操作 */
    int* outbox;      /* 出棧：用於提供 pop/peek 操作 */
    int inbox_top;    /* inbox 的棧頂索引，-1 表示空 */
    int outbox_top;   /* outbox 的棧頂索引，-1 表示空 */
    int capacity;     /* 預分配容量（題目限制最多 100 次呼叫） */
} MyQueue;

/* 建立並初始化隊列結構 */
MyQueue* myQueueCreate() {
    MyQueue* queue = (MyQueue*)malloc(sizeof(MyQueue));
    queue->capacity = 1000;  /* 預留足夠空間，遠超題目 100 次限制 */
    queue->inbox = (int*)malloc(queue->capacity * sizeof(int));
    queue->outbox = (int*)malloc(queue->capacity * sizeof(int));
    queue->inbox_top = -1;   /* 初始為空棧 */
    queue->outbox_top = -1;  /* 初始為空棧 */
    return queue;
}

/* Push：將元素推入 inbox 棧頂，O(1) */
void myQueuePush(MyQueue* obj, int x) {
    obj->inbox[++obj->inbox_top] = x;  /* 先遞增 top 再寫入 */
}

/*
 * 延遲轉移（Lazy Transfer）：
 * 只有當 outbox 為空時，才將 inbox 全部倒入 outbox。
 * 這是攤還 O(1) 的關鍵——避免每次 pop/peek 都做轉移。
 *
 * 轉移過程示意（inbox 有 [A, B, C]，C 在頂部）：
 *
 *   inbox        outbox         inbox        outbox
 *   ┌───┐        ┌───┐         ┌───┐        ┌───┐
 *   │ C │ top    │   │  ──→    │   │ empty  │ A │ top
 *   ├───┤        │   │         │   │        ├───┤
 *   │ B │        │   │         │   │        │ B │
 *   ├───┤        │   │         │   │        ├───┤
 *   │ A │        │   │         │   │        │ C │
 *   └───┘        └───┘         └───┘        └───┘
 *
 *   C 先彈出 → 推入 outbox 底部
 *   B 再彈出 → 推入 outbox 中間
 *   A 最後彈出 → 推入 outbox 頂部
 *   現在 outbox 頂部是 A（最早進入的元素），符合 FIFO！
 */
void transfer(MyQueue* obj) {
    if (obj->outbox_top == -1) {          /* 只在 outbox 為空時轉移 */
        while (obj->inbox_top >= 0) {     /* 將 inbox 逐一搬到 outbox */
            obj->outbox[++obj->outbox_top] = obj->inbox[obj->inbox_top--];
        }
    }
}

/* Pop：確保 outbox 有元素後，從頂部彈出，攤還 O(1) */
int myQueuePop(MyQueue* obj) {
    transfer(obj);                        /* 必要時觸發轉移 */
    return obj->outbox[obj->outbox_top--]; /* 彈出 outbox 頂部（隊列前端） */
}

/* Peek：確保 outbox 有元素後，查看頂部但不移除，攤還 O(1) */
int myQueuePeek(MyQueue* obj) {
    transfer(obj);                        /* 必要時觸發轉移 */
    return obj->outbox[obj->outbox_top];  /* 只查看，不彈出 */
}

/* Empty：兩個棧都為空時隊列才為空，O(1) */
bool myQueueEmpty(MyQueue* obj) {
    return obj->inbox_top == -1 && obj->outbox_top == -1;
}

/* Free：釋放所有動態分配的記憶體 */
void myQueueFree(MyQueue* obj) {
    free(obj->inbox);   /* 釋放 inbox 陣列 */
    free(obj->outbox);  /* 釋放 outbox 陣列 */
    free(obj);          /* 釋放結構體本身 */
}
