/*
 * 347. Top K Frequent Elements
 * 難度：Medium
 * 方法：Hash Map + Min-Heap（size-k 最小堆積）
 *
 * LeetCode function signature:
 *     int* topKFrequent(int* nums, int numsSize, int k, int* returnSize)
 *
 * 時間複雜度：O(n · log k)
 *   - 計數：O(n)
 *   - 對 U 個不同元素做 heap 維護：O(U · log k) ≤ O(n · log k)
 *
 * 空間複雜度：O(n + k)
 *   - hash 表：O(n)（最多 n 個不同值）
 *   - min-heap：O(k)
 *   - 答案陣列：O(k)
 *
 * ── 核心概念 ──
 *   1. 先用 hash map 把每個數字的出現次數算出來。
 *   2. 維護一個「大小最多為 k」的 min-heap，比較依據是『出現次數』。
 *   3. 對每個不同數字：
 *        - 若 heap 未滿 (size < k) → 直接 push。
 *        - 若 heap 已滿，且該數字的頻率 > 堆頂頻率
 *          → 把堆頂換掉，再 siftDown。
 *   4. 全部處理完後，heap 裡的 k 個元素就是 top-k，把它們的 value 取出即可。
 *
 * ── 為什麼用 Min-Heap 且大小為 k？──
 *   如果用 Max-Heap，則要把全部 U 個不同元素都放進去、再 pop k 次，
 *   整體是 O(U log U + k log U)，空間是 O(U)。
 *   改用「大小為 k 的 Min-Heap」：
 *     - 堆頂是目前 top-k 候選中『頻率最小』的那個。
 *     - 新來的元素只要比堆頂更高頻，就有資格擠掉它。
 *     - 每次 push + pop 為 O(log k)，總成本 O(n log k)。
 *   當 k ≪ n 時這個優勢相當明顯，也正是面試官想看到的答案。
 *
 * ── 圖解範例（LeetCode Example 1）──
 *   nums = [1, 1, 1, 2, 2, 3], k = 2
 *
 *   Step A：計頻率
 *       value :  1   2   3
 *       count :  3   2   1
 *
 *   Step B：逐一把不同值丟進 size-k min-heap（比較 count）
 *
 *   假設走訪順序為 (1,3) → (2,2) → (3,1)：
 *
 *   push (1,3)：heap size=0 < k=2 → 直接放
 *       heap (count 為比較鍵)：
 *              (1,3)                 ← 堆頂 = 最小頻率 3
 *
 *   push (2,2)：heap size=1 < k=2 → 直接放
 *       heap：
 *                 (2,2)              ← 堆頂變成較小的頻率 2
 *                /
 *             (1,3)
 *       陣列表示：[(2,2), (1,3)]
 *
 *   push (3,1)：heap size=2 == k，且 count=1 ≤ 堆頂 count=2
 *             → 不替換，直接丟棄
 *       heap：
 *                 (2,2)              ← 堆頂
 *                /
 *             (1,3)
 *
 *   Step C：把 heap 裡所有 value 取出 → 答案 [2, 1] 或 [1, 2]（題目不要求順序）
 *
 * ── 圖解範例（LeetCode Example 2）──
 *   nums = [1], k = 1
 *       value :  1
 *       count :  1
 *   push (1,1) → heap = [(1,1)] → 取出 → 答案 [1]
 *
 * ── Heap 陣列表示法（以 count 為鍵）──
 *
 *       索引:        0        1        2        3
 *                ┌────────┬────────┬────────┬────────┐
 *       heap:    │ (v,c0) │ (v,c1) │ (v,c2) │ (v,c3) │
 *                └────────┴────────┴────────┴────────┘
 *                   ↑
 *                 堆頂 (c0 最小 → 會最先被踢掉)
 *
 *       父子索引關係（0-indexed）：
 *         parent(i) = (i - 1) / 2
 *         left(i)   = 2 * i + 1
 *         right(i)  = 2 * i + 2
 *
 * ── 與 Bucket Sort 版（同資料夾內的另一份解）的比較 ──
 *
 *   | 比較項目             | Heap 版 (此檔)              | Bucket Sort 版 |
 *   | -------------------- | --------------------------- | -------------- |
 *   | 時間複雜度           | O(n log k)                  | O(n)          |
 *   | 空間複雜度           | O(n + k)                    | O(n)          |
 *   | 面試泛用性           | 非常高（通用於任何值域）   | 僅在值域受限時漂亮 |
 *   | 可擴展為 streaming   | 可以（只留 size-k heap）   | 不行          |
 *   | 對 k 很小時          | 特別有利                    | 相同          |
 *
 *   在 Google / 大廠面試中，Heap 版本是最常被要求講解的「標準答案」，
 *   因為它對值域沒有假設、且 size-k 這個技巧經常出現在其他 top-k 題目。
 */

#include <stdlib.h>
#include <string.h>

/* ──────────────────────────────────────────────────────────────
 * 1) 小型開放定址 (open addressing) hash map：value -> count
 *    容量是 2 的冪，取模改成 &(CAP - 1)，省掉除法。
 *    nums[i] 範圍 [-10^4, 10^4]，最多 20001 個不同值，
 *    用 32768 (2^15) 容量，負載率最壞也只有 ~0.61。
 * ────────────────────────────────────────────────────────────── */
enum { HASH_CAP = 1 << 15 };          /* 32768，須為 2 的冪 */
enum { HASH_MASK = HASH_CAP - 1 };

/* 每個 slot 存一組 (key, count)，used=1 表示已經被佔用 */
typedef struct {
    int key;        /* 數字本體 */
    int count;      /* 出現次數 */
    int used;       /* 是否為有效 entry */
} HashEntry;

/*
 * hashMix：把任意整數（含負數）打散，避免退化成連續碰撞。
 * 用 Knuth multiplicative hash 的常數 2654435761（黃金比例 × 2^32）。
 */
static unsigned hashMix(int x) {
    /* 轉成 unsigned 才能安全做乘法溢位 */
    return (unsigned)x * 2654435761u;
}

/*
 * hashLookupOrInsert：
 *   - 找到 key 所在 slot 就回傳。
 *   - 找不到則佔用第一個空 slot、回傳它（count 預設為 0）。
 * 這樣呼叫端只需 `entry->count++` 即可完成計數。
 */
static HashEntry* hashLookupOrInsert(HashEntry* table, int key) {
    /* 初始 index 由 hashMix 決定，再用 MASK 限制在容量內 */
    unsigned idx = hashMix(key) & HASH_MASK;

    /* 線性探測：若目前 slot 已被別的 key 佔用就往右跳 */
    while (table[idx].used && table[idx].key != key) {
        idx = (idx + 1) & HASH_MASK;
    }

    /* 若是全新 key，初始化 slot */
    if (!table[idx].used) {
        table[idx].used = 1;
        table[idx].key = key;
        table[idx].count = 0;
    }
    return &table[idx];
}

/* ──────────────────────────────────────────────────────────────
 * 2) Min-Heap：以『出現次數』為比較鍵，存 (value, count)
 *    維持 size ≤ k，堆頂永遠是目前 top-k 候選裡最不夠高的那個。
 * ────────────────────────────────────────────────────────────── */
typedef struct {
    int value;      /* 原始數字 */
    int count;      /* 其出現次數（比較鍵）*/
} HeapItem;

/* 交換兩個 HeapItem 的內容（值傳遞，不涉及 aliasing 問題） */
static void swapItem(HeapItem* a, HeapItem* b) {
    HeapItem tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * siftUp：剛插入的節點在陣列末端，往上浮到正確位置。
 * 維持 min-heap 不變量：父節點的 count ≤ 子節點的 count。
 */
static void siftUp(HeapItem* heap, int idx) {
    /* 尚未到 root 就繼續檢查 */
    while (idx > 0) {
        int parent = (idx - 1) / 2;                 /* 父節點索引 */
        if (heap[idx].count >= heap[parent].count)  /* 已滿足 min-heap 性質 */
            break;
        swapItem(&heap[idx], &heap[parent]);        /* 違反就交換 */
        idx = parent;                               /* 繼續往上 */
    }
}

/*
 * siftDown：從 idx 向下沉到正確位置。
 * 每一步和『兩個子節點中較小者』比較、需要就交換。
 */
static void siftDown(HeapItem* heap, int size, int idx) {
    for (;;) {
        int smallest = idx;                /* 先假設自己最小 */
        int left  = 2 * idx + 1;           /* 左子節點索引 */
        int right = 2 * idx + 2;           /* 右子節點索引 */

        /* 左子存在且比當前 smallest 還小，就更新 */
        if (left < size && heap[left].count < heap[smallest].count) {
            smallest = left;
        }
        /* 右子存在且比當前 smallest 還小，就更新 */
        if (right < size && heap[right].count < heap[smallest].count) {
            smallest = right;
        }
        /* 若自己就是最小，已滿足 min-heap 性質，停止 */
        if (smallest == idx) break;

        /* 否則交換並繼續往下沉 */
        swapItem(&heap[idx], &heap[smallest]);
        idx = smallest;
    }
}

/* 將 item 推入 heap；呼叫端需確保尚有容量（size < capacity） */
static void heapPush(HeapItem* heap, int* size, HeapItem item) {
    heap[*size] = item;        /* 放到陣列末端 */
    siftUp(heap, *size);       /* 往上浮 */
    (*size)++;                 /* size 加 1 */
}

/* ──────────────────────────────────────────────────────────────
 * 3) 主函式：topKFrequent
 *    LeetCode 要求我們回傳一個長度為 k 的新陣列，並寫回 *returnSize。
 * ────────────────────────────────────────────────────────────── */
int* topKFrequent(int* nums, int numsSize, int k, int* returnSize) {
    /* ── Step 1：用 hash map 計頻率 ── */

    /* calloc 會把 used / count 都歸零，拿來當初始化 hash 表剛剛好 */
    HashEntry* table = (HashEntry*)calloc(HASH_CAP, sizeof(HashEntry));

    /* 一次掃描原始陣列，對應 slot 的 count 加一 */
    for (int i = 0; i < numsSize; i++) {
        HashEntry* e = hashLookupOrInsert(table, nums[i]);
        e->count++;
    }

    /* ── Step 2：建立一個 size 最多為 k 的 min-heap ── */

    /* 配置 k+1 是為了允許「先 push 再判斷溢位」的寫法更彈性 */
    HeapItem* heap = (HeapItem*)malloc(sizeof(HeapItem) * (size_t)(k + 1));
    int heapSize = 0;

    /*
     * 掃過整張 hash 表，只處理 used==1 的 entry。
     * 這就相當於走過所有不同數字。
     */
    for (int i = 0; i < HASH_CAP; i++) {
        if (!table[i].used) continue;

        /* 把這個不同數字包成 heap 節點 */
        HeapItem item;
        item.value = table[i].key;
        item.count = table[i].count;

        if (heapSize < k) {
            /* heap 還沒滿 → 直接 push */
            heapPush(heap, &heapSize, item);
        } else if (item.count > heap[0].count) {
            /*
             * heap 滿了，但新元素的頻率嚴格大於堆頂。
             * 直接覆蓋堆頂再 siftDown，就等同於 pop + push
             * 但少一次 siftUp，常數因子略優。
             */
            heap[0] = item;
            siftDown(heap, heapSize, 0);
        }
        /* 其餘情形（item.count <= 堆頂）：這個元素不可能進 top-k，略過 */
    }

    /* ── Step 3：把 heap 裡的 k 個 value 倒出來 ── */

    int* answer = (int*)malloc(sizeof(int) * (size_t)k);

    /*
     * 題目不要求順序；這裡直接線性讀 heap 陣列。
     * 若要從最小頻率往最大頻率輸出，可改成連續 heapPop。
     */
    for (int i = 0; i < k; i++) {
        answer[i] = heap[i].value;
    }

    /* LeetCode 需要把答案長度寫回 returnSize */
    if (returnSize != NULL) {
        *returnSize = k;
    }

    /* 釋放中繼資源 */
    free(heap);
    free(table);

    return answer;
}
