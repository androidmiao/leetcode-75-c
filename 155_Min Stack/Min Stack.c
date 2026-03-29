/*
 * LeetCode 155. Min Stack（最小棧）
 * 難度：Medium
 * 時間複雜度：所有操作 O(1)
 * 空間複雜度：O(n)
 *
 * 演算法：輔助最小值陣列（對應 Editorial Approach 1：Value/Minimum Pairs）
 *
 * ============================================================
 *  核心觀察
 * ============================================================
 *  棧是 LIFO 結構 — 只從頂部新增 / 移除。
 *  當元素 x 位於棧中，x 下方的元素永遠不會改變。
 *  因此 x 在棧頂時的「全域最小值」只取決於：
 *      min(x, x 下方所有元素的最小值)
 *  這個值在 x 被推入時就可以算好，之後不會變。
 *
 * ============================================================
 *  資料結構設計
 * ============================================================
 *  使用兩個平行陣列：
 *    data[i]      = 第 i 層存放的實際值
 *    min_stack[i]  = 當棧高度為 i+1 時的全域最小值
 *
 *  等價於 Editorial 的 stack<pair<int,int>> 方式，
 *  只是用兩個 int[] 取代一個 pair[]。
 *
 * ============================================================
 *  範例步驟追蹤（Example 1）
 * ============================================================
 *
 *  操作            data[]          min_stack[]      top    getMin
 *  ──────────────  ──────────────  ──────────────   ───    ──────
 *  push(-2)        [-2]            [-2]              0      -2
 *  push(0)         [-2, 0]         [-2, -2]          1      -2
 *  push(-3)        [-2, 0, -3]     [-2, -2, -3]      2      -3
 *  getMin()                                                  -3
 *  pop()           [-2, 0]         [-2, -2]          1      -2
 *  top()                                             → 0
 *  getMin()                                                  -2
 *
 *  ASCII 圖解：push(-3) 之後的棧狀態
 *
 *       index    data[]    min_stack[]
 *      ┌─────┬──────────┬──────────────┐
 *   2  │  2  │   -3     │     -3       │  ← top (棧頂)
 *      ├─────┼──────────┼──────────────┤
 *   1  │  1  │    0     │     -2       │
 *      ├─────┼──────────┼──────────────┤
 *   0  │  0  │   -2     │     -2       │  ← 棧底
 *      └─────┴──────────┴──────────────┘
 *
 *  pop() 只需 top--，邏輯上移除最頂層，
 *  min_stack[top] 自動回到 -2。
 *
 * ============================================================
 *  push 時 min_stack 的更新邏輯
 * ============================================================
 *
 *  push(val):
 *     if (top == 0)                     ← 棧為空，第一個元素自己就是最小值
 *         min_stack[0] = val
 *     else
 *         min_stack[top] = min(val, min_stack[top-1])
 *                                       ← 和前一層的最小值比較
 *
 *  圖示（push(0) 時）:
 *
 *     min_stack[0] = -2   （前一層最小值）
 *                    │
 *                    ▼
 *     min(0, -2) = -2  →  min_stack[1] = -2
 *
 *  圖示（push(-3) 時）:
 *
 *     min_stack[1] = -2   （前一層最小值）
 *                    │
 *                    ▼
 *     min(-3, -2) = -3  →  min_stack[2] = -3
 */

#include <stdlib.h>
#include <limits.h>

/* MinStack 結構體：用兩個平行陣列模擬「值/最小值配對棧」 */
typedef struct {
    int* data;       /* 主資料陣列，儲存推入的值 */
    int* min_stack;  /* 輔助陣列，min_stack[i] = 棧高度為 i+1 時的全域最小值 */
    int top;         /* 棧頂索引，-1 表示空棧 */
    int capacity;    /* 目前配置的容量 */
} MinStack;

/* 建立並初始化 MinStack */
MinStack* minStackCreate() {
    MinStack* obj = (MinStack*)malloc(sizeof(MinStack));
    obj->capacity = 1000;  /* 初始容量，足夠應付多數測資（限制 3*10^4 次操作） */
    obj->data = (int*)malloc(obj->capacity * sizeof(int));
    obj->min_stack = (int*)malloc(obj->capacity * sizeof(int));
    obj->top = -1;  /* -1 代表空棧 */
    return obj;
}

/* 推入元素 val */
void minStackPush(MinStack* obj, int val) {
    obj->top++;  /* 先遞增棧頂索引 */

    /* 若超出容量，動態擴容為兩倍 */
    if (obj->top >= obj->capacity) {
        obj->capacity *= 2;
        obj->data = (int*)realloc(obj->data, obj->capacity * sizeof(int));
        obj->min_stack = (int*)realloc(obj->min_stack, obj->capacity * sizeof(int));
    }

    obj->data[obj->top] = val;  /* 將值存入主資料陣列 */

    if (obj->top == 0) {
        /* 棧內只有一個元素時，它本身就是最小值 */
        obj->min_stack[obj->top] = val;
    } else {
        /* 與前一層的最小值比較，取較小者存入 min_stack */
        int curr_min = obj->min_stack[obj->top - 1];
        obj->min_stack[obj->top] = (val < curr_min) ? val : curr_min;
    }
}

/* 移除棧頂元素 — 只需遞減 top，O(1) */
void minStackPop(MinStack* obj) {
    obj->top--;  /* 邏輯上移除，min_stack[top] 自動回到正確的最小值 */
}

/* 回傳棧頂元素的值 */
int minStackTop(MinStack* obj) {
    return obj->data[obj->top];  /* 直接讀取 data[top] */
}

/* 回傳棧中的最小元素 — O(1)，直接讀取輔助陣列 */
int minStackGetMin(MinStack* obj) {
    return obj->min_stack[obj->top];  /* min_stack[top] 永遠是當前棧的全域最小值 */
}

/* 釋放所有動態配置的記憶體 */
void minStackFree(MinStack* obj) {
    free(obj->data);       /* 釋放主資料陣列 */
    free(obj->min_stack);  /* 釋放輔助最小值陣列 */
    free(obj);             /* 釋放結構體本身 */
}
