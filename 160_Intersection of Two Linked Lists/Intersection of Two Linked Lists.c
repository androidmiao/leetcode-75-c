/*
 * 160. Intersection of Two Linked Lists
 * Difficulty: Easy
 * Time Complexity:  O(M + N)  — 各走一次兩條鏈表
 * Space Complexity: O(1)      — 只用兩個指標
 *
 * ─────────────────────────────────────────────────────────────────
 * 演算法：Two Pointers（雙指標交叉走法）
 * ─────────────────────────────────────────────────────────────────
 *
 * 核心想法
 * --------
 * 設 list A 的「私有段」長度為 a，list B 的「私有段」長度為 b，
 * 共同段（intersection 起點到尾端）長度為 c。
 *
 *   pA 路徑：[a 個節點] + [c 個節點] → 跳到 headB → [b 個節點] + [c 個節點]
 *   pB 路徑：[b 個節點] + [c 個節點] → 跳到 headA → [a 個節點] + [c 個節點]
 *
 *   兩者都走了 a + b + c 步後同時抵達交叉點（若有交叉），
 *   或同時抵達 NULL（若無交叉，此時 a + b + c = a + b）。
 *
 * ASCII 示意圖
 * -----------
 * 例：intersectVal = 8
 *
 *   headA → [4] → [1] → [8] → [4] → [5] → NULL
 *                          ↑
 *   headB → [5] → [6] → [1] ──┘   (交叉從 val=8 的節點開始)
 *
 *   a = 2  (A 的私有段：4→1)
 *   b = 3  (B 的私有段：5→6→1)
 *   c = 3  (共同段：8→4→5)
 *
 * 走法追蹤：
 *   步驟   pA          pB
 *   0      A:4         B:5
 *   1      A:1         B:6
 *   2      A:8         B:1
 *   3      A:4         B:8   ← 到達交叉點
 *   ... (pA 還沒交叉，繼續)
 *   2      A:8         B:1(from A)  ← pA 跳至 headB 後再走 b 步
 *
 * 更精確追蹤（pA 先到 NULL 跳至 headB）：
 *   pA: 4→1→8→4→5→NULL → 5→6→1→[8]  (a+c + b = 2+3+3 = 8 步後到 8)
 *   pB: 5→6→1→8→4→5→NULL → 4→1→[8]  (b+c + a = 3+3+2 = 8 步後到 8)
 *   兩者第 8 步同時指向 val=8 的節點 → 找到交叉點
 *
 * ─────────────────────────────────────────────────────────────────
 * 無交叉情況：
 *   a = lenA, b = lenB, c = 0
 *   pA 走 a+b 步後為 NULL，pB 走 b+a 步後也為 NULL
 *   → 兩者同時為 NULL，迴圈結束，回傳 NULL
 * ─────────────────────────────────────────────────────────────────
 */

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode *getIntersectionNode(struct ListNode *headA, struct ListNode *headB) {
    /* 任一串列為空則必無交叉 */
    if (!headA || !headB) return NULL;

    struct ListNode *pA = headA; /* 指標 pA 從 A 的頭出發 */
    struct ListNode *pB = headB; /* 指標 pB 從 B 的頭出發 */

    /*
     * 迴圈不變量：pA != pB 時繼續走
     * 若有交叉：兩者在走 a+b+c 步後同時到達交叉節點
     * 若無交叉：兩者在走 a+b 步後同時到達 NULL
     */
    while (pA != pB) {
        /* pA 到達末尾就跳到 headB，否則前進一步 */
        pA = (pA == NULL) ? headB : pA->next;

        /* pB 到達末尾就跳到 headA，否則前進一步 */
        pB = (pB == NULL) ? headA : pB->next;
    }

    /*
     * 退出迴圈時 pA == pB：
     *   若有交叉 → pA/pB 指向交叉節點
     *   若無交叉 → pA/pB 均為 NULL
     */
    return pA;
}
