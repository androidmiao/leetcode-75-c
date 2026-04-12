/*
 * 215. Kth Largest Element in an Array
 * 難度：Medium
 * 方法：Min-Heap（最小堆積）
 *
 * 時間複雜度：O(n·logk)
 * 空間複雜度：O(k)
 *
 * ── 核心概念 ──
 * 維護一個大小最多為 k 的最小堆積（min-heap）。
 * 遍歷陣列中的每個元素，將它放入 heap：
 *   - 如果 heap 大小超過 k，就彈出堆頂（目前最小值）
 *   - 這樣 heap 永遠只保留目前看過的「最大 k 個元素」
 * 全部遍歷完後，堆頂就是第 k 大元素。
 *
 * ── 為什麼堆頂是答案？ ──
 * heap 裡存的是最大的 k 個元素，而 min-heap 的堆頂是這 k 個中最小的，
 * 也就是「第 k 大」。
 *
 * ── 範例走一次（Example 1）──
 * nums = [3, 2, 1, 5, 6, 4], k = 2
 *
 * Step 1: push 3  → heap = [3]           size=1 ≤ 2, 不彈出
 * Step 2: push 2  → heap = [2, 3]        size=2 ≤ 2, 不彈出
 * Step 3: push 1  → heap = [1, 3, 2]     size=3 > 2, 彈出最小值 1
 *                 → heap = [2, 3]
 * Step 4: push 5  → heap = [2, 5, 3]     size=3 > 2, 彈出最小值 2
 *                 → heap = [3, 5]
 * Step 5: push 6  → heap = [3, 6, 5]     size=3 > 2, 彈出最小值 3
 *                 → heap = [5, 6]
 * Step 6: push 4  → heap = [4, 6, 5]     size=3 > 2, 彈出最小值 4
 *                 → heap = [5, 6]
 *
 * 最終堆頂 = 5 → 答案 ✓
 *
 * ── Min-Heap 陣列表示法 ──
 *
 *    索引:     0     1     2
 *            ┌─────┬─────┬─────┐
 *    heap:   │  5  │  6  │     │
 *            └─────┴─────┴─────┘
 *              ↑
 *            堆頂（最小值）= 第 k 大
 *
 *    樹狀結構:
 *         5          ← 堆頂 = 答案
 *        /
 *       6
 *
 *    父子關係（0-indexed）:
 *      parent(i) = (i - 1) / 2
 *      left(i)   = 2 * i + 1
 *      right(i)  = 2 * i + 2
 *
 * ── 範例走一次（Example 2）──
 * nums = [3, 2, 3, 1, 2, 4, 5, 5, 6], k = 4
 *
 * Step 1: push 3 → heap = [3]                         size=1 ≤ 4
 * Step 2: push 2 → heap = [2, 3]                      size=2 ≤ 4
 * Step 3: push 3 → heap = [2, 3, 3]                   size=3 ≤ 4
 * Step 4: push 1 → heap = [1, 2, 3, 3]                size=4 ≤ 4
 * Step 5: push 2 → heap = [1, 2, 3, 3, 2]             size=5 > 4
 *                → 彈出 1 → heap = [2, 2, 3, 3]
 * Step 6: push 4 → heap = [2, 2, 3, 3, 4]             size=5 > 4
 *                → 彈出 2 → heap = [2, 3, 3, 4]
 * Step 7: push 5 → heap = [2, 3, 3, 4, 5]             size=5 > 4
 *                → 彈出 2 → heap = [3, 4, 3, 5]
 * Step 8: push 5 → heap = [3, 4, 3, 5, 5]             size=5 > 4
 *                → 彈出 3 → heap = [3, 4, 5, 5]
 * Step 9: push 6 → heap = [3, 4, 5, 5, 6]             size=5 > 4
 *                → 彈出 3 → heap = [4, 5, 5, 6]
 *
 * 最終堆頂 = 4 → 答案 ✓
 */

/* ── Min-Heap 操作函式 ── */

/* 交換兩個整數的值 */
static void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * siftUp：將新插入的元素往上浮（bubble up）。
 * 當子節點比父節點小時，交換兩者，維持 min-heap 性質。
 */
static void siftUp(int* heap, int idx) {
    /* 只要還沒到根節點，且目前節點比父節點小，就繼續往上交換 */
    while (idx > 0) {
        int parent = (idx - 1) / 2;  /* 父節點索引 */
        if (heap[idx] < heap[parent]) {
            swap(&heap[idx], &heap[parent]);
            idx = parent;  /* 移動到父節點位置，繼續檢查 */
        } else {
            break;  /* 已滿足 min-heap 性質，停止 */
        }
    }
}

/*
 * siftDown：將堆頂元素往下沉（bubble down）。
 * 當父節點比最小的子節點大時，交換兩者，維持 min-heap 性質。
 */
static void siftDown(int* heap, int size, int idx) {
    while (1) {
        int smallest = idx;            /* 假設目前節點最小 */
        int left  = 2 * idx + 1;      /* 左子節點索引 */
        int right = 2 * idx + 2;      /* 右子節點索引 */

        /* 如果左子節點存在，且比目前最小值還小 */
        if (left < size && heap[left] < heap[smallest]) {
            smallest = left;
        }

        /* 如果右子節點存在，且比目前最小值還小 */
        if (right < size && heap[right] < heap[smallest]) {
            smallest = right;
        }

        /* 如果最小值不是自己，就交換並繼續往下沉 */
        if (smallest != idx) {
            swap(&heap[idx], &heap[smallest]);
            idx = smallest;
        } else {
            break;  /* 已滿足 min-heap 性質，停止 */
        }
    }
}

/*
 * heapPush：將 val 插入 min-heap。
 * 先放到陣列末尾，再 siftUp 維持堆序。
 */
static void heapPush(int* heap, int* size, int val) {
    heap[*size] = val;   /* 放到最後一個位置 */
    (*size)++;           /* heap 大小加 1 */
    siftUp(heap, *size - 1);  /* 從最後一個位置往上浮 */
}

/*
 * heapPop：移除並回傳 min-heap 的堆頂（最小值）。
 * 把最後一個元素搬到堆頂，再 siftDown 維持堆序。
 */
static int heapPop(int* heap, int* size) {
    int top = heap[0];          /* 記錄堆頂值 */
    (*size)--;                  /* heap 大小減 1 */
    heap[0] = heap[*size];     /* 把最後一個元素放到堆頂 */
    siftDown(heap, *size, 0);  /* 從堆頂往下沉 */
    return top;
}

/* ── 主函式 ── */

/*
 * findKthLargest：回傳陣列中第 k 大的元素。
 *
 * 策略：維護一個大小為 k 的 min-heap。
 * 遍歷完所有元素後，堆頂就是第 k 大。
 */
int findKthLargest(int* nums, int numsSize, int k) {
    /* 配置 heap 陣列，最多只需要 k+1 格（push 後可能暫時到 k+1，馬上 pop） */
    int heap[k + 1];
    int heapSize = 0;  /* 目前 heap 中的元素數量 */

    /* 遍歷陣列中的每個元素 */
    for (int i = 0; i < numsSize; i++) {
        /* 將目前元素放入 heap */
        heapPush(heap, &heapSize, nums[i]);

        /* 如果 heap 大小超過 k，就移除堆頂（最小值） */
        /* 這確保 heap 中永遠只保留最大的 k 個元素 */
        if (heapSize > k) {
            heapPop(heap, &heapSize);
        }
    }

    /* 遍歷結束後，heap 中有最大的 k 個元素 */
    /* 堆頂是這 k 個中最小的，也就是第 k 大元素 */
    return heap[0];
}


/*
## 詳細說明

### 為什麼用 Min-Heap 而不是 Max-Heap？

如果用 max-heap，我們需要把全部 n 個元素放進去，
然後連續 pop k 次才能拿到第 k 大。
時間複雜度是 O(n + k·logn)。

而用 min-heap 且限制大小為 k：
- 每次 push + 可能的 pop 成本是 O(logk)
- 總共做 n 次，所以是 O(n·logk)

當 k 遠小於 n 時，O(n·logk) 比 O(n·logn) 好得多。

### Heap 的陣列表示法

min-heap 用陣列實作，父子關係用索引計算：

```
          heap[0]           ← 堆頂（最小值）
         /       \
    heap[1]     heap[2]
    /    \      /    \
heap[3] heap[4] heap[5] heap[6]
```

- parent(i)     = (i - 1) / 2
- leftChild(i)  = 2 * i + 1
- rightChild(i) = 2 * i + 2

### siftUp 的作用

當新元素被放到陣列末尾時，它可能比父節點小，
違反 min-heap 性質。siftUp 不斷把它和父節點交換，
直到它找到正確位置。

```
插入 2 到 heap = [3, 5, 4]:

Step 0: [3, 5, 4, 2]      ← 2 放在末尾 (idx=3)
        parent(3) = 1, heap[1]=5, 2 < 5 → swap
Step 1: [3, 2, 4, 5]      ← 2 往上到 idx=1
        parent(1) = 0, heap[0]=3, 2 < 3 → swap
Step 2: [2, 3, 4, 5]      ← 2 到堆頂，完成
```

### siftDown 的作用

當堆頂被移除後，最後一個元素被搬到堆頂，
它可能比子節點大，違反 min-heap 性質。
siftDown 不斷把它和最小的子節點交換，直到正確位置。

```
移除堆頂 2，把末尾 5 搬到堆頂:

Step 0: [5, 3, 4]        ← 5 在堆頂 (idx=0)
        left=1(3), right=2(4), smallest=1
        5 > 3 → swap
Step 1: [3, 5, 4]        ← 5 下沉到 idx=1
        left=3 (不存在) → 停止
```

### 複雜度分析

時間複雜度：O(n·logk)
- 遍歷 n 個元素
- 每個元素做 push（O(logk)）和可能的 pop（O(logk)）

空間複雜度：O(k)
- heap 陣列大小為 k+1

### 與 Quickselect 的比較

| 比較項目       | Min-Heap        | Quickselect       |
|----------------|-----------------|-------------------|
| 平均時間       | O(n·logk)       | O(n)              |
| 最壞時間       | O(n·logk)       | O(n²)             |
| 額外空間       | O(k)            | O(1)              |
| 是否修改原陣列 | 不會            | 會                |
| 面試適合度     | 非常好（好寫）  | 好（但較難寫）    |
| 穩定性         | 永遠 O(n·logk)  | 依 pivot 選擇     |

面試中 Min-Heap 是最推薦的做法：
- 時間複雜度穩定，沒有最壞 O(n²) 的風險
- 實作直觀，容易解釋
- 不修改輸入陣列
- 可以輕鬆擴展為串流（streaming）版本
*/
