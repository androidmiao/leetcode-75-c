/*
 * LeetCode 210. Course Schedule II （課程表 II）
 * 難度：Medium
 * 演算法：DFS 後序拓撲排序（三色標記法）
 * 時間複雜度：O(V + E)，其中 V = numCourses, E = prerequisitesSize
 * 空間複雜度：O(V + E)，鄰接表 + 狀態陣列 + 遞迴堆疊 + 結果陣列
 *
 * ============================================================
 * 核心思路：
 * ============================================================
 * 此題是 207. Course Schedule 的延伸：不僅要判斷能否完成所有課程，
 * 還要返回一個合法的修課順序（拓撲排序）。
 *
 * 將課程依賴關係建模為有向圖：
 *   prerequisites[i] = [a, b] → 邊 a → b（a 依賴 b）
 *   即 adj[a] 中存放 b
 *
 * DFS 後序拓撲排序：
 *   1. 對每個未訪問節點啟動 DFS
 *   2. DFS 先遞迴處理所有先修課（鄰居）
 *   3. 當一個節點的所有先修課都處理完畢，將它加入結果陣列
 *   4. 由於先修課先被加入，結果陣列天然滿足拓撲順序
 *
 * 環偵測（三色標記）：
 *   state[i] = 0 (WHITE)：未訪問
 *   state[i] = 1 (GRAY) ：正在訪問（在當前 DFS 遞迴堆疊中）
 *   state[i] = 2 (BLACK)：已完成訪問
 *   若 DFS 遇到 GRAY 節點 → 回邊 → 有環 → 返回空陣列
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
 *   有向圖（箭頭方向：course → prereq，「依賴」方向）：
 *
 *        3
 *       / \
 *      v   v
 *      1   2
 *       \ /
 *        v
 *        0
 *
 *   DFS 過程與結果陣列的建構：
 *
 *   步驟 1：從課程 0 開始 DFS
 *     visit(0): state[0]=GRAY → 無鄰居
 *               → state[0]=BLACK → result[0] = 0
 *     result: [0, _, _, _]
 *
 *   步驟 2：從課程 1 開始 DFS
 *     visit(1): state[1]=GRAY → 鄰居 0 已 BLACK → 跳過
 *               → state[1]=BLACK → result[1] = 1
 *     result: [0, 1, _, _]
 *
 *   步驟 3：從課程 2 開始 DFS
 *     visit(2): state[2]=GRAY → 鄰居 0 已 BLACK → 跳過
 *               → state[2]=BLACK → result[2] = 2
 *     result: [0, 1, 2, _]
 *
 *   步驟 4：從課程 3 開始 DFS
 *     visit(3): state[3]=GRAY → 鄰居 1 已 BLACK → 跳過
 *                              → 鄰居 2 已 BLACK → 跳過
 *               → state[3]=BLACK → result[3] = 3
 *     result: [0, 1, 2, 3]  ← 合法拓撲排序 ✓
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
 *     visit(0): state[0]=GRAY
 *       → 鄰居 1: state[1]=WHITE → 遞迴進入
 *         visit(1): state[1]=GRAY
 *           → 鄰居 0: state[0]=GRAY ← 發現回邊！有環！
 *           → has_cycle = true → 返回空陣列
 */

#include <stdlib.h>
#include <stdbool.h>

/* has_cycle 全域旗標：一旦發現環，所有遞迴立即結束 */
static bool has_cycle;

/*
 * DFS 遞迴函數：從 course 出發探索所有先修課
 *
 * 功能：
 *   1. 環偵測：遇到 GRAY 節點 → 設定 has_cycle = true
 *   2. 拓撲排序：後序加入結果陣列（先修課先加入）
 *
 * 參數：
 *   course      - 當前要處理的課程
 *   adj         - 鄰接表（adj[i] 存放課程 i 的先修課列表）
 *   adj_sizes   - adj_sizes[i] = adj[i] 中的元素數
 *   state       - 三色標記陣列
 *   result      - 拓撲排序結果陣列
 *   idx         - 結果陣列的當前寫入位置（指標）
 */
void dfs(int course, int** adj, int* adj_sizes, int* state,
         int* result, int* idx) {
    if (has_cycle) return;          /* 已偵測到環，直接剪枝返回 */

    state[course] = 1;              /* 標記為 GRAY：進入遞迴堆疊 */

    /* 遍歷 course 的所有先修課程（鄰居） */
    for (int i = 0; i < adj_sizes[course]; i++) {
        int prereq = adj[course][i];  /* 取得第 i 個先修課 */

        if (state[prereq] == 1) {
            /* 先修課正處於 GRAY → 在當前遞迴路徑上 → 回邊 → 有環！ */
            has_cycle = true;
            return;
        }
        if (state[prereq] == 0) {
            /* 先修課尚未訪問（WHITE），遞迴探索 */
            dfs(prereq, adj, adj_sizes, state, result, idx);
            if (has_cycle) return;  /* 子樹中發現環，立即向上傳遞 */
        }
        /* state[prereq] == 2 (BLACK)：已完成處理，安全跳過 */
    }

    /* 所有先修課已處理完畢 → 此課程可以修了 → 加入結果 */
    state[course] = 2;              /* 標記為 BLACK：完成 */
    result[(*idx)++] = course;      /* 後序加入：先修課已在前面 */
}

/*
 * findOrder - LeetCode 提交入口
 *
 * 返回一個合法的課程修習順序，若有環則返回空陣列。
 * returnSize 設定返回陣列的長度。
 */
int* findOrder(int numCourses, int** prerequisites, int prerequisitesSize,
               int* prerequisitesColSize, int* returnSize) {
    /* ---- 建立鄰接表 ---- */
    /* adj[i] 存放課程 i 的所有先修課程（i 依賴哪些課程） */
    int** adj = (int**)malloc(numCourses * sizeof(int*));
    int* adj_sizes = (int*)calloc(numCourses, sizeof(int)); /* 每門課的先修課數量 */

    for (int i = 0; i < numCourses; i++) {
        adj[i] = (int*)malloc(numCourses * sizeof(int));  /* 最壞情況配置 */
    }

    /* 讀取先修條件，建邊：prerequisites[i] = [course, prereq]
     * 意義：要修 course，必須先修 prereq
     * 所以在 adj[course] 中加入 prereq（course → prereq 方向） */
    for (int i = 0; i < prerequisitesSize; i++) {
        int course = prerequisites[i][0];   /* 要修的課 */
        int prereq = prerequisites[i][1];   /* 先修課 */
        adj[course][adj_sizes[course]++] = prereq;
    }

    /* ---- DFS 拓撲排序 + 環偵測（三色標記）---- */
    int* state = (int*)calloc(numCourses, sizeof(int));  /* 全部初始為 0 (WHITE) */
    int* result = (int*)malloc(numCourses * sizeof(int));
    int idx = 0;                    /* 結果陣列的寫入游標 */
    has_cycle = false;              /* 重置環偵測旗標 */

    /* 對每個未訪問的節點啟動 DFS（圖可能不連通） */
    for (int i = 0; i < numCourses && !has_cycle; i++) {
        if (state[i] == 0) {       /* 只對 WHITE 節點啟動 DFS */
            dfs(i, adj, adj_sizes, state, result, &idx);
        }
    }

    /* ---- 釋放輔助記憶體 ---- */
    for (int i = 0; i < numCourses; i++) {
        free(adj[i]);
    }
    free(adj);
    free(adj_sizes);
    free(state);

    /* ---- 返回結果 ---- */
    if (has_cycle) {
        *returnSize = 0;            /* 有環 → 無法完成 → 空陣列 */
    } else {
        *returnSize = numCourses;   /* 無環 → 返回完整拓撲排序 */
    }

    return result;
}
