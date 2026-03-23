/*
 * LeetCode 207. Course Schedule （課程表）
 * 難度：Medium
 * 演算法：DFS 循環檢測（三色標記法）
 * 時間複雜度：O(V + E)，其中 V = numCourses, E = prerequisites.length
 * 空間複雜度：O(V + E)，鄰接表 + 狀態陣列 + 遞迴堆疊
 *
 * ============================================================
 * 核心思路：
 * ============================================================
 * 將課程依賴關係建模為有向圖，問題轉化為「有向圖中是否存在環」。
 * 若存在環 → 互相依賴，無法完成所有課程 → 返回 false
 * 若無環（DAG）→ 拓撲排序存在，可完成所有課程 → 返回 true
 *
 * 使用三色標記的 DFS：
 *   state[i] = 0 (WHITE)：未訪問
 *   state[i] = 1 (GRAY) ：正在訪問（在當前 DFS 遞迴堆疊中）
 *   state[i] = 2 (BLACK)：已完成訪問（所有後代已處理）
 *
 * 當 DFS 遇到 GRAY 節點 → 發現回邊（back edge）→ 有環！
 *
 * ============================================================
 * 範例演示：numCourses = 4, prerequisites = [[1,0],[2,0],[3,1],[3,2]]
 * ============================================================
 *
 *   建出的鄰接表（course → 其先修課）：
 *     0: []           （0 沒有先修課）
 *     1: [0]          （要修 1，先修 0）
 *     2: [0]          （要修 2，先修 0）
 *     3: [1, 2]       （要修 3，先修 1 和 2）
 *
 *   有向圖（箭頭表示「依賴」方向：course → prereq）：
 *
 *        3
 *       / \
 *      v   v
 *      1   2
 *       \ /
 *        v
 *        0
 *
 *   DFS 從課程 0 開始：
 *     visit(0): state[0]=1(GRAY) → 無鄰居 → state[0]=2(BLACK) ✓
 *   DFS 從課程 1 開始：
 *     visit(1): state[1]=1(GRAY) → 鄰居 0 已 BLACK → 跳過 → state[1]=2(BLACK) ✓
 *   DFS 從課程 2 開始：
 *     visit(2): state[2]=1(GRAY) → 鄰居 0 已 BLACK → 跳過 → state[2]=2(BLACK) ✓
 *   DFS 從課程 3 開始：
 *     visit(3): state[3]=1(GRAY) → 鄰居 1 已 BLACK → 跳過
 *                                 → 鄰居 2 已 BLACK → 跳過 → state[3]=2(BLACK) ✓
 *   全部無環 → 返回 true
 *
 * ============================================================
 * 有環範例：numCourses = 2, prerequisites = [[1,0],[0,1]]
 * ============================================================
 *
 *   鄰接表：
 *     0: [1]     （要修 0，先修 1）
 *     1: [0]     （要修 1，先修 0）
 *
 *   有向圖：
 *     0 ⇄ 1    （互相指向，形成環）
 *
 *   DFS 從課程 0 開始：
 *     visit(0): state[0]=1(GRAY)
 *       → 鄰居 1: state[1]=0(WHITE) → 遞迴進入
 *         visit(1): state[1]=1(GRAY)
 *           → 鄰居 0: state[0]=1(GRAY) ← 發現 GRAY！有環！返回 false
 */

#include <stdlib.h>
#include <stdbool.h>

/* DFS 遞迴函數：檢查從 course 出發是否能偵測到環
 * 返回 true 表示無環（安全），false 表示有環 */
bool dfs(int course, int** adj, int* degrees, int* state) {
    state[course] = 1;  /* 標記為 GRAY：正在此 DFS 路徑上 */

    /* 遍歷 course 的所有先修課程（鄰居） */
    for (int i = 0; i < degrees[course]; i++) {
        int prereq = adj[course][i];  /* 取得第 i 個先修課 */
        if (state[prereq] == 1) {
            /* 先修課正處於 GRAY 狀態 → 在當前遞迴堆疊中 → 回邊 → 有環！ */
            return false;
        }
        if (state[prereq] == 0) {
            /* 先修課尚未訪問（WHITE），遞迴探索 */
            if (!dfs(prereq, adj, degrees, state)) {
                return false;  /* 子樹中發現環，向上傳遞 */
            }
        }
        /* state[prereq] == 2 (BLACK)：已完成處理，跳過 */
    }

    state[course] = 2;  /* 所有後代已處理完畢，標記為 BLACK */
    return true;         /* 此子樹無環 */
}

bool canFinish(int numCourses, int** prerequisites, int prerequisitesSize, int* prerequisitesColSize) {
    /* ---- 建立鄰接表 ---- */
    /* adj[i] 存放課程 i 的所有先修課程（i 依賴哪些課程） */
    int** adj = (int**)malloc(numCourses * sizeof(int*));
    int* degrees = (int*)malloc(numCourses * sizeof(int));  /* degrees[i] = adj[i] 的長度 */

    for (int i = 0; i < numCourses; i++) {
        adj[i] = (int*)malloc(numCourses * sizeof(int));  /* 最壞情況：每門課依賴所有其他課 */
        degrees[i] = 0;
    }

    /* 讀取先修條件，建邊：prerequisites[i] = [course, prereq]
     * 意義：要修 course，必須先修 prereq
     * 所以 adj[course] 裡加入 prereq */
    for (int i = 0; i < prerequisitesSize; i++) {
        int course = prerequisites[i][0];   /* 要修的課 */
        int prereq = prerequisites[i][1];   /* 先修課 */
        adj[course][degrees[course]++] = prereq;
    }

    /* ---- DFS 環檢測（三色標記）---- */
    int* state = (int*)malloc(numCourses * sizeof(int));
    for (int i = 0; i < numCourses; i++) {
        state[i] = 0;  /* 初始狀態：所有節點為 WHITE（未訪問） */
    }

    bool has_cycle = false;

    /* 對每個未訪問的節點啟動 DFS（圖可能不連通） */
    for (int i = 0; i < numCourses && !has_cycle; i++) {
        if (state[i] == 0) {  /* 只對 WHITE 節點啟動 DFS */
            if (!dfs(i, adj, degrees, state)) {
                has_cycle = true;  /* 發現環，提前終止 */
            }
        }
    }

    /* ---- 釋放記憶體 ---- */
    for (int i = 0; i < numCourses; i++) {
        free(adj[i]);
    }
    free(adj);
    free(degrees);
    free(state);

    return !has_cycle;  /* 無環 → 可完成所有課程 */
}
