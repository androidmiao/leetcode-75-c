# LeetCode 480 - Sliding Window Median
## Linux Kernel 核心知識探討

---

## 1. Linux 相關原始碼位置

### 優先佇列 (Priority Queue) 與堆積 (Heap) 實作

| 核心檔案 | 路徑 | 用途說明 |
|--------|------|--------|
| `lib/min_heap.h` | `linux/lib/min_heap.h` | 泛用最小堆積實作，用於內核各子系統需要高效優先佇列的場景，如事件調度、cgroup 資源管理 |
| `lib/prio_heap.c` | `linux/lib/prio_heap.c` | 傳統優先堆積實作，用於 cgroup 限制和 perf 性能監控子系統 |
| `include/linux/prio_heap.h` | `linux/include/linux/prio_heap.h` | 優先堆積公開 API 定義，提供標準的入堆、出堆操作 |

### 滑動視窗與串流統計相關

| 核心檔案 | 路徑 | 用途說明 |
|--------|------|--------|
| `include/linux/average.h` | `linux/include/linux/average.h` | 指數加權移動平均 (EWMA) 實作，用於網路統計量、CPU 負載追蹤，體現滑動時間窗口的概念 |
| `kernel/sched/pelt.c` | `linux/kernel/sched/pelt.c` | 完全可運行時間的衰減計算，使用衰減時間窗口追蹤任務負載 |
| `net/ipv4/tcp_input.c` | `linux/net/ipv4/tcp_input.c` | TCP 同步擁塞控制演算法，維護滑動視窗統計 (RTT、丟包率等) |

### 延遲刪除 (Lazy Deletion) 相關機制

| 核心檔案 | 路徑 | 用途說明 |
|--------|------|--------|
| `kernel/time/timer.c` | `linux/kernel/time/timer.c` | 計時器輪實作，使用延遲清理過期計時器，減少即時削除的代價 |
| `kernel/rcu/tree.c` | `linux/kernel/rcu/tree.c` | Read-Copy-Update 機制，延遲回收記憶體，提供無鎖讀取與最終一致性 |
| `kernel/workqueue.c` | `linux/kernel/workqueue.c` | 延遲工作佇列，使用延遲執行模式實現非同步處理 |
| `include/linux/rcu_types.h` | `linux/include/linux/rcu_types.h` | RCU 延遲回收的型別定義，提供垃圾收集式的資源管理 |

### 雜湊表與動態追蹤

| 核心檔案 | 路徑 | 用途說明 |
|--------|------|--------|
| `include/linux/hashtable.h` | `linux/include/linux/hashtable.h` | 核心通用鏈式雜湊表巨集，用於快速元素查找與追蹤，時間複雜度 O(1) |
| `lib/rhashtable.c` | `linux/lib/rhashtable.c` | 可重新調整大小的雜湊表，動態擴展以保持負載因子，類似於 LeetCode 解法中的動態哈希追蹤 |

---

## 2. Kernel vs. LeetCode 實作對照

### LeetCode 480 解題核心策略

LeetCode 480 使用 **Dual Heap (雙堆) + Lazy Deletion (延遲刪除)** 來在 O(log n) 時間找到滑動視窗中位數：

```c
// 核心資料結構
typedef struct {
    MaxHeap* small;      // 較小的一半 (最大堆)
    MinHeap* large;      // 較大的一半 (最小堆)
    HashSet* deleted;    // 標記要刪除的元素 (延遲刪除)
    int small_size;      // small 堆的有效元素數
    int large_size;      // large 堆的有效元素數
} MedianFinder;
```

**核心操作**:
1. **插入**: 元素加入適當的堆，不立即重新平衡
2. **延遲刪除**: 將要移除的元素標記在雜湊表，不立即執行堆削除
3. **中位數查詢**: 在重新平衡後，取堆頂即為中位數

```c
// 插入示例 (O(log n))
void addNum(int num) {
    if (num <= getMax(small)) {
        heapPush(small, num);
    } else {
        heapPush(large, num);
    }
    rebalance();  // 確保 large 的大小 >= small，差異 <= 1
}

// 延遲刪除示例 (O(1) 標記，O(log n) 清理)
void removeNum(int num) {
    hashset_insert(deleted, num);
    // 堆不修改，延遲到查詢時清理
}

// 中位數查詢 (O(log n) 最壞，均攤 O(1))
double findMedian() {
    // 清理堆頂的已刪除元素
    while (!heapEmpty(small) && hashset_contains(deleted, heapTop(small))) {
        hashset_erase(deleted, heapTop(small));
        heapPop(small);
    }
    while (!heapEmpty(large) && hashset_contains(deleted, heapTop(large))) {
        hashset_erase(deleted, heapTop(large));
        heapPop(large);
    }

    return calculateMedian(small, large);
}
```

### Linux Kernel 中的對應概念

#### 1. **堆積實作對比**

**LeetCode 方案** (`480_solution.c` 中的堆):
- 使用陣列實作二叉堆，手動管理父子關係
- 支援 O(log n) 的 heappush / heappop
- 堆內元素無特定順序保證，只保證堆性質

**Linux Kernel `lib/min_heap.h`**:
```c
/* linux/lib/min_heap.h (簡化) */
#define min_heap_push(heap, elem, cmp, ...) \
    do { \
        if (unlikely(heap->nr == heap->size)) \
            /* 動態擴展 */ \
        heap->data[heap->nr++] = elem; \
        sift_up(heap, heap->nr - 1, cmp); \
    } while (0)

#define min_heap_pop(heap, cmp) \
    do { \
        heap->data[0] = heap->data[--heap->nr]; \
        sift_down(heap, 0, cmp); \
    } while (0)
```

**對應特點**:
- 核心堆邏輯完全相同 (sift_up / sift_down)
- Kernel 使用巨集實現泛型支援，LeetCode 通常使用具體實作
- Kernel 堆自動動態擴展，LeetCode 通常需要預分配

#### 2. **延遲刪除對比**

**LeetCode 480 的延遲刪除**:
```c
HashSet deleted;  // 標記已刪除的元素
// 刪除時: hashset_insert(deleted, num) 標記，O(1)
// 查詢時: 清理堆頂已刪除元素，O(log n) 最壞
```

**Linux Kernel 的延遲刪除模式 - RCU (Read-Copy-Update)**:
```c
/* kernel/rcu/tree.c (概念示例) */
struct rcu_node {
    spinlock_t lock;
    struct rcu_data *rcu_data;
    // ... 其他欄位
};

// RCU 讀端：無需鎖定，快速路徑
rcu_read_lock();
ptr = rcu_dereference(gp);  // 讀取
// ... 使用 ptr
rcu_read_unlock();

// RCU 寫端：標記舊版本為刪除，不立即回收
rcu_assign_pointer(gp, new_data);  // 發佈新版本
call_rcu(&old_data->rcu_head, free_callback);  // 延遲回收
// free_callback 在所有讀取者都離開後才執行
```

**延遲刪除對比表**:

| 方面 | LeetCode 480 (HashSet) | Linux Kernel RCU | 共同特性 |
|------|----------------------|-----------------|--------|
| **標記方式** | hashset_insert(deleted, num) | rcu_assign_pointer() | 都是 O(1) 標記 |
| **清理時機** | 懶加載 (查詢時) | 寬限期後 (Grace Period) | 都延遲實際刪除 |
| **讀寫競爭** | 查詢時掃描 deleted 集合 | RCU 讀鎖無競爭 | 都減少同步開銷 |
| **回收開銷** | O(log n) 最壞，均攤低 | O(1)，但全域同步 | 都攤銷刪除成本 |

#### 3. **滑動視窗對比**

**LeetCode 480 的滑動視窗**:
```c
// 窗口大小為 k，逐次移動
for (int i = 0; i < numsSize; i++) {
    addNum(nums[i]);
    if (i >= k - 1) {
        median[result_idx++] = findMedian();
        if (i >= k - 1) {
            removeNum(nums[i - k + 1]);
        }
    }
}
```

**Linux Kernel 的滑動視窗 - TCP 擁塞控制**:
```c
/* net/ipv4/tcp_input.c (概念) */
struct tcp_sock {
    u32 snd_una;        // 最小未確認序列號 (視窗左端)
    u32 snd_nxt;        // 下一個要發送的序列號 (視窗右端)
    u32 snd_wnd;        // 發送視窗大小
    // ...
};

// 接收到 ACK，視窗滑動
void tcp_ack(struct sock *sk, ...) {
    // snd_una 向前移動，視窗滑動
    // 新資料進入視窗，舊資料離開視窗
}
```

**滑動視窗對比表**:

| 方面 | LeetCode 480 | Linux TCP | 共同特性 |
|------|-------------|-----------|--------|
| **視窗定義** | 最新 k 個元素 | 序列號範圍 [snd_una, snd_nxt) | 都維護固定大小窗口 |
| **進入機制** | addNum(nums[i]) | 新 TCP 段進入發送緩衝區 | 都 O(1) 新增 |
| **離開機制** | removeNum(nums[i-k]) | ACK 確認使 snd_una 前進 | 都批次更新邊界 |
| **查詢開銷** | O(log n) 最壞，均攤低 | 統計計算 O(1) | 都支援高效查詢 |

#### 4. **雜湊表對比**

**LeetCode 480 中的 HashSet 標記已刪除元素**:
```c
// 簡化實作
typedef struct {
    int* keys;
    bool* present;
    int capacity;
} HashSet;

void hashset_insert(HashSet* set, int key) {
    int idx = hash(key) % set->capacity;
    set->present[idx] = true;  // O(1)
}

bool hashset_contains(HashSet* set, int key) {
    int idx = hash(key) % set->capacity;
    return set->present[idx];  // O(1)
}
```

**Linux Kernel 的通用雜湊表 `include/linux/hashtable.h`**:
```c
/* include/linux/hashtable.h (簡化) */
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)] = { }

#define hash_add(table, node, key) \
    hlist_add_head(node, &table[hash_key(key, HASH_BITS(table))])

#define hash_for_each_possible(table, obj, member, key) \
    hlist_for_each_entry(obj, \
        &table[hash_key(key, HASH_BITS(table))], member)

/* Resizable Hash Table - lib/rhashtable.c */
struct rhashtable {
    struct bucket_table *tbl;
    struct bucket_table *future_tbl;  // 擴展時的新表
    size_t cur_size;
    size_t max_size;
    // ...
};

int rhashtable_insert_fast(struct rhashtable *ht, ...) {
    // 檢查負載因子，必要時自動重新雜湊
    if (rht_grow_above_75(ht))
        rhashtable_rehash_table(ht);
    // ...
}
```

**雜湊表對比表**:

| 方面 | LeetCode 480 (簡單 HashSet) | Linux Kernel `rhashtable` | 共同特性 |
|------|---------------------------|------------------------|--------|
| **衝突處理** | 開放定址 (本例簡化) | 鏈式 (hlist) | 都支援 O(1) 查詢 |
| **動態擴展** | 需手動管理 | 自動當負載因子 > 75% | 都控制效率 |
| **記憶體開銷** | 固定大小 | 漸進式重新雜湊 | Kernel 版本更靈活 |
| **併發支援** | 通常不考慮 | RCU 讀鎖無競爭寫 | Kernel 版本支援無鎖讀 |

---

## 3. 主管 Code Review 角度考題

### 問題 1: 堆積重新平衡的穩定性與複雜度保證

**主管提問**:
> "在 LeetCode 480 中，你在每次 addNum 後呼叫 rebalance() 來確保兩個堆的大小差異不超過 1。在 Linux Kernel 的 `lib/min_heap.h` 實作中，我沒看到顯式的重新平衡邏輯。這如何影響實時性能保證？"

**預期解答**:

LeetCode 480 中的 rebalance 是必需的，因為中位數定義要求知道較小的一半和較大的一半。這通常需要額外的 O(log n) 平衡操作。

```c
// LeetCode 480 中的顯式平衡
void rebalance(MedianFinder* obj) {
    if (obj->small_size > obj->large_size + 1) {
        // small 太大，移動最大元素到 large
        heapPush(obj->large, heapTop(obj->small));
        heapPop(obj->small);
        obj->small_size--;
        obj->large_size++;
    } else if (obj->large_size > obj->small_size) {
        // large 太大，移動最小元素到 small
        heapPush(obj->small, heapTop(obj->large));
        heapPop(obj->large);
        obj->small_size++;
        obj->large_size--;
    }
}
```

Linux Kernel 的 `lib/min_heap.h` 則只維護最小堆性質，無額外平衡邏輯，因為它的用途（優先佇列調度）不需要"中位數"概念。

```c
/* linux/lib/min_heap.h - 純粹的堆操作，無應用層平衡 */
#define min_heap_push(heap, elem, cmp, ...) \
    do { \
        if (unlikely(heap->nr == heap->size)) { /* 擴展 */ } \
        heap->data[heap->nr++] = elem; \
        sift_up(heap, heap->nr - 1, cmp); \
    } while (0)
```

**重點**: LeetCode 解法加入了應用層邏輯 (balance check)，Kernel 保持底層堆的純粹性，讓上層應用自決策平衡策略。

**進階探問 1-A**:
> "如果視窗大小 k 很大（例如 10^6），rebalance 的 O(log n) 成本是否仍可接受？如何在實時系統中改善這一點？"

**進階解答**:
- 時間複雜度仍為 O(log n) 對數，即使 n = 10^6，也只需約 20 次堆操作
- 如果需要進一步優化，可考慮**懶平衡**: 只在中位數查詢時檢查平衡，而非每次插入都平衡
- 這類似 Linux Kernel 中的**延遲工作佇列** (`kernel/workqueue.c`)，將平衡工作延遲到必要時刻

**進階探問 1-B**:
> "Kernel 中的 `lib/prio_heap.c` 與 `lib/min_heap.h` 有什麼區別？為什麼 Kernel 需要兩種堆實作？"

**進階解答**:
- `lib/prio_heap.c`: 傳統優先級堆，元素有顯式優先級欄位，用於 cgroup 和 perf 子系統
- `lib/min_heap.h`: 泛型最小堆，通過巨集支援任意可比較型別，更靈活
- LeetCode 480 實際上需要**雙堆** (max-heap + min-heap)，類似於 Kernel 中多堆協作的場景，但 Kernel 通常使用單一堆配合不同的比較器

**測試技能**:
- 理解堆性質與應用層需求的分離
- 認識時間複雜度分析中的大 O 常數影響
- 延遲優化與實時性權衡

---

### 問題 2: 延遲刪除的正確性與記憶體回收保證

**主管提問**:
> "你在 LeetCode 480 中使用 HashSet 標記已刪除元素，延遲到查詢時清理。這種方式在多執行緒環境下如何保證正確性？Linux Kernel 使用 RCU 實現類似的延遲刪除，它如何解決競爭條件？"

**預期解答**:

LeetCode 480 (單執行緒):
```c
void removeNum(int num) {
    hashset_insert(obj->deleted, num);  // O(1) 標記，不立即削除
}

double findMedian(MedianFinder* obj) {
    // 清理堆頂已刪除元素
    while (!heapEmpty(obj->small) && hashset_contains(obj->deleted, heapTop(obj->small))) {
        hashset_erase(obj->deleted, heapTop(obj->small));
        heapPop(obj->small);
        obj->small_size--;
    }
    // 計算中位數
    // ...
}
```

**單執行緒正確性**:
- 由於只有一個執行緒，標記和清理之間無競爭
- 至多一個執行緒訪問 deleted HashSet，無需同步

**多執行緒場景下的問題**:
```c
// 執行緒 1
removeNum(5);  // 標記 5 為已刪除

// 執行緒 2 (並行)
findMedian();  // 開始查詢中位數，此時 5 可能尚未清理出堆

// 競爭條件: 執行緒 2 的 findMedian() 可能在清理不完整的狀態下計算中位數
```

**Linux Kernel RCU 解決方案**:
```c
/* kernel/rcu/tree.c (概念) */

// 讀端: 無需鎖定 (RCU 讀鎖實際上只是禁止搶佔)
rcu_read_lock();
struct task *t = rcu_dereference(task_ptr);
// ... 讀取 t 的資料
rcu_read_unlock();

// 寫端: 標記舊版本並延遲回收
struct task *old_task = task_ptr;
task_ptr = new_task;  // 發佈新指標
synchronize_rcu();    // 等待所有讀取者離開 (寬限期)
kfree(old_task);      // 安全回收舊物件
```

**RCU 的三步舞**:
1. **移除 (Remove)**: 從資料結構中移除指標，新讀取者無法訪問
2. **寬限期 (Grace Period)**: 等待現有讀取者完成
3. **回收 (Reclaim)**: 確保所有讀取者已完成，安全回收記憶體

**對比分析**:

| 方面 | LeetCode 480 | Linux RCU | 差異 |
|------|------------|-----------|------|
| **同步機制** | 單執行緒，無同步 | RCU 寬限期（全域同步檢查點） | Kernel 版本支援高度併發 |
| **讀端開銷** | 無，直接訪問 | 最小 (禁止搶佔)，無鎖 | RCU 讀端快速路徑 |
| **寫端開銷** | O(log n) (清理) | O(n) 等待寬限期，但批次化回收 | 取捨：延遲批次化減少全域同步 |
| **記憶體回收** | 漸進式清理 | 批次化回收 | RCU 更激進的批次化 |
| **複雜度** | 簡單 | 複雜，但提供無鎖保證 | Kernel 為併發換複雜度 |

**進階探問 2-A**:
> "如果 LeetCode 480 需要支援多執行緒，應如何改進？是否可使用自旋鎖或讀寫鎖？"

**進階解答**:
```c
// 方案 1: 粗粒度互斥鎖 (簡單但低效)
pthread_mutex_lock(&obj->lock);
removeNum(num);
findMedian();
pthread_mutex_unlock(&obj->lock);

// 方案 2: 讀寫鎖 (更好，讀取者無競爭)
pthread_rwlock_rdlock(&obj->lock);
double median = findMedian();  // 多個讀取者可並行
pthread_rwlock_unlock(&obj->lock);

pthread_rwlock_wrlock(&obj->lock);
removeNum(num);  // 獨佔寫入
pthread_rwlock_unlock(&obj->lock);

// 方案 3: 無鎖設計 (最優但複雜)
// 使用 lock-free 堆或 RCU，如 Kernel 中的做法
```

Kernel 選擇 RCU 的原因：讀寫比例通常是 99:1，RCU 消除了讀端鎖定開銷。

**進階探問 2-B**:
> "在 Linux Timer Wheel (`kernel/time/timer.c`) 中也使用延遲刪除。它與你的 HashSet 延遲刪除有何異同？"

**進階解答**:

Timer Wheel 的延遲刪除:
```c
/* kernel/time/timer.c (簡化) */
struct timer_list {
    unsigned long expires;
    void (*function)(unsigned long);
    // ...
};

// 刪除計時器 (延遲清理)
int del_timer(struct timer_list *timer) {
    // 標記為已刪除，不從輪中移除
    timer->expires = 0;
    return 1;
}

// 檢查計時器是否過期 (懶加載清理)
static inline int timer_pending(const struct timer_list *timer) {
    if (timer->expires != 0 && !timer_del_pending(timer)) {
        return 1;
    }
    return 0;
}

// 計時器輪中斷處理
void run_timer_softirq(void) {
    while (timer_pending(timer)) {
        if (timer->expires <= jiffies) {
            timer->function(timer->data);
        }
        // 跳過已標記刪除的計時器
    }
}
```

**對比 LeetCode 480 與 Timer Wheel**:

| 方面 | LeetCode 480 | Timer Wheel | 共性 |
|------|------------|------------|------|
| **標記方式** | hashset_insert(deleted, num) | timer->expires = 0 | 都 O(1) 標記 |
| **清理契機** | findMedian() 時 | 輪盤中斷觸發時 | 都是懶加載 |
| **清理成本** | O(log n) 最壞 | O(1) 若標記正確 | Timer Wheel 更激進 |
| **批次化** | 只清理堆頂 | 批次清理整個桶 | Timer Wheel 更高效 |

**測試技能**:
- 理解延遲刪除在單執行緒與多執行緒的差異
- 認識 RCU、讀寫鎖等同步機制的權衡
- 延遲刪除在不同 Kernel 子系統的應用 (RCU、Timer Wheel)

---

### 問題 3: 視窗滑動與增量計算的最適化

**主管提問**:
> "LeetCode 480 在每次滑動視窗後重新計算中位數，涉及堆操作。在 Linux 網路棧中，TCP 擁塞控制也維護滑動視窗，但它直接計算統計量而非使用堆。如何在增量更新與完整重計算之間做出選擇？"

**預期解答**:

LeetCode 480 的增量更新:
```c
// 每次視窗滑動，中位數通過堆頂直接取得，O(log n) 更新
void getMedians(int* nums, int numsSize, int k, double* result) {
    MedianFinder* finder = medianFinderCreate();
    for (int i = 0; i < numsSize; i++) {
        medianFinderAddNum(finder, nums[i]);
        if (i >= k - 1) {
            result[i - k + 1] = medianFinderFindMedian(finder);
            medianFinderDeleteNum(finder, nums[i - k + 1]);
        }
    }
}
```

**時間複雜度**: O(numsSize * log k) — 每次操作 O(log k)

**TCP 視窗統計的增量計算**:
```c
/* net/ipv4/tcp_input.c (概念) */
struct tcp_sock {
    u32 srtt;           // 平滑 RTT (指數加權移動平均)
    u32 rttvar;         // RTT 變異
    u32 snd_una;        // 確認視窗左端
    u32 snd_nxt;        // 視窗右端
};

void tcp_update_rtt(struct tcp_sock *tp, long mrtt) {
    // 增量更新: 新樣本加權，舊樣本衰減
    // 類似 include/linux/average.h 中的 EWMA
    if (tp->srtt) {
        long m = mrtt - (tp->srtt >> 3);
        tp->srtt += m;           // srtt = (7 * srtt + rtt) / 8
        if (m < 0) m = -m;
        m -= (tp->rttvar >> 2);
        tp->rttvar += m;         // rttvar = (3 * rttvar + |m|) / 4
    } else {
        tp->srtt = mrtt << 3;
        tp->rttvar = mrtt << 1;
    }
}
```

**時間複雜度**: O(1) — 常數操作，無堆開銷

**何時選擇堆（LeetCode 方式）v.s. 增量統計（TCP 方式）**:

| 場景 | 堆方式 (LeetCode 480) | 增量統計 (TCP) | 最適化選擇 |
|-----|----------------------|--------------|---------|
| **中位數查詢** | O(log n) 但精確 | 無法快速計算 | 使用堆 |
| **平均值** | O(log n) 若使用堆 | O(1) 遞推 | 使用增量統計 |
| **指數加權移動平均** | O(log n) 浪費 | O(1) 完美配合 | 使用 EWMA |
| **百分位數** | O(log n) 堆方式 | 無好辦法 | 使用堆或分位數素描 |
| **記憶體成本** | O(k) 保存全部元素 | O(1) 只保存統計量 | TCP 的激進優化 |
| **視窗內修改** | 難以支援 | 可支援 (重新計算) | TCP 視窗淨移動 |

**Linux EWMA 對標 LeetCode 方式的優勢**:
```c
/* include/linux/average.h (簡化) */
#define DECLARE_EWMA(name, _prec, _scale) \
struct ewma_##name { \
    unsigned long internal; \
}; \
\
static inline void ewma_##name##_init(struct ewma_##name *e) { \
    e->internal = 0; \
} \
\
static inline unsigned long ewma_##name##_read(struct ewma_##name *e) { \
    return e->internal >> (_prec); \
} \
\
static inline void ewma_##name##_add(struct ewma_##name *e, unsigned long val) { \
    e->internal = e->internal * (_scale - 1) / _scale + val; \
}

DECLARE_EWMA(load, 10, 8);  // 10 位精度，權重 7:1

// 使用示例 (O(1))
ewma_load_add(&task->load, new_load);
long avg = ewma_load_read(&task->load);
```

**進階探問 3-A**:
> "在 LeetCode 480 中，如果視窗 k 非常大（例如 10^9），而陣列大小 numsSize 相對小（例如 10^3），如何優化？"

**進階解答**:
```c
// 問題: k 可能 > numsSize，無法形成滑動視窗
if (k > numsSize) {
    // 邊界情況: 整個陣列是視窗
    // 一次計算中位數，複雜度 O(n log n) 排序或 O(n) Quickselect
    // 無需維護堆
}

// 最適化: 如果 k >> numsSize，用 Quickselect 一次性計算中位數
double findMedianLarge(int* nums, int numsSize) {
    return quickselect_median(nums, numsSize);  // O(n)
}
```

**進階探問 3-B**:
> "TCP 的 SRTT 更新使用 EWMA，但中位數是什麼？是否存在"加權中位數"的概念，可適用於 TCP 的 RTT 估計？"

**進階解答**:
```c
// 加權中位數: 在原始中位數基礎上，靠近最新樣本
// 例如: [100ms, 150ms, 200ms] 的加權中位數傾向於較新的樣本

// TCP 實際上使用更簡單的策略: EWMA (指數加權)
// 新樣本權重高: SRTT = (7 * SRTT_old + SRTT_new) / 8
// 這在網路統計學中比中位數更合適，因為:
// 1. 最新的 RTT 最相關
// 2. O(1) 計算，適合高速路徑
// 3. 視窗大小動態，不固定

// 如果需要加權中位數，可使用加權堆:
typedef struct {
    int value;
    double weight;
} WeightedElement;

// 維護加權堆，查詢時累加權重至 50%
```

**測試技能**:
- 算法選擇與場景的匹配
- 時間複雜度 O(1) v.s. O(log n) 的實際意義
- Kernel 中激進的記憶體節省策略 (EWMA 只用 O(1) 空間)
- 邊界情況與參數約束

---

### 問題 4: 動態重新雜湊與負載因子管理

**主管提問**:
> "你在 LeetCode 480 中使用簡單的 HashSet 追蹤已刪除元素。Kernel 的 `lib/rhashtable.c` 實現自動動態重新雜湊以維持 O(1) 查詢。如何在 LeetCode 解法中實現類似的自動擴展，而不影響中位數查詢的時間複雜度保證？"

**預期解答**:

LeetCode 480 簡單 HashSet（固定大小，可能退化）:
```c
typedef struct {
    int* keys;
    bool* present;
    int capacity;           // 固定大小，無動態擴展
} HashSet;

void hashset_insert(HashSet* set, int key) {
    int idx = key % set->capacity;  // 模運算碰撞
    set->present[idx] = true;
    // 若衝突多，查詢變成 O(衝突鏈長度) — 退化為 O(n)！
}
```

**Linux Kernel rhashtable 自動擴展**:
```c
/* lib/rhashtable.c (簡化) */
struct rhashtable {
    struct bucket_table *tbl;
    struct bucket_table *future_tbl;  // 漸進式重新雜湊用
    size_t cur_size;
    size_t max_size;
    u32 rehash_count;
};

#define RHT_LOAD_MAX(ht) ((ht)->p.max_shift ? 1U << (ht)->p.max_shift : 0)
#define RHT_GROW_ABOVE_75(ht) \
    (atomic_read(&(ht)->nelems) > RHT_LOAD_MAX(ht) * 3 / 4)

int rhashtable_insert_fast(struct rhashtable *ht,
                          struct rhash_head *obj,
                          const struct rhashtable_params *params) {
    // 檢查是否需要擴展
    if (rht_grow_above_75(ht)) {
        schedule_work(&ht->run_work);  // 異步重新雜湊
    }

    // 邊插入邊重新雜湊 (漸進式)
    hash_head = rht_bucket_insert(ht, obj, params);

    return 0;
}

void rhashtable_rehash_table(struct rhashtable *ht) {
    // 漸進式重新雜湊: 一次遷移一個舊桶到新表
    struct bucket_table *old_tbl = ht->tbl;
    struct bucket_table *new_tbl = ht->future_tbl;

    for (int i = 0; i < old_tbl->size && !need_resched(); i++) {
        rhashtable_rehash_chain(ht, i);  // 一次遷移一個桶
    }

    ht->tbl = new_tbl;
}
```

**Kernel 動態擴展的優勢**:
1. **自動化**: 無需應用層干預
2. **漸進式**: 在 worker thread 中異步重新雜湊，不阻塞查詢
3. **負載因子控制**: 維持 < 75% 負載，保證 O(1) 查詢
4. **無停頓**: 舊表和新表並存，查詢時同時檢查兩者

**如何在 LeetCode 480 中改進**:

```c
// 改進方案: 手動檢查負載因子
typedef struct {
    int* keys;
    bool* present;
    int size;        // 當前元素數
    int capacity;    // 目前容量
} HashSet;

void hashset_insert(HashSet* set, int key) {
    int idx = key % set->capacity;
    set->present[idx] = true;
    set->size++;

    // 檢查負載因子 (類似 Kernel)
    if (set->size > set->capacity * 3 / 4) {
        hashset_expand(set);  // 擴展到 2 倍容量
    }
}

void hashset_expand(HashSet* set) {
    int new_capacity = set->capacity * 2;
    bool* new_present = calloc(new_capacity, sizeof(bool));

    // 重新雜湊所有元素
    for (int i = 0; i < set->capacity; i++) {
        if (set->present[i]) {
            // 粗略恢復鍵值（簡化假設 i == key）
            int new_idx = i % new_capacity;
            new_present[new_idx] = true;
        }
    }

    free(set->present);
    set->present = new_present;
    set->capacity = new_capacity;
}
```

**對中位數查詢複雜度的影響**:

| 操作 | 時間複雜度 | 說明 |
|------|----------|------|
| findMedian（無擴展） | O(log k) | 堆操作，不涉及 HashSet |
| findMedian（遇到擴展） | O(log k + capacity) | 最壞情況，但 capacity 增長對數 |
| **攤銷複雜度** | O(log k) | 擴展頻率 O(1/log n)，成本攤銷 |

**進階探問 4-A**:
> "Kernel 的 rhashtable 使用漸進式重新雜湊（一次遷移一個舊桶），LeetCode 能否採用相同策略？"

**進階解答**:
```c
// 漸進式重新雜湊 (類似 Kernel rhashtable)
typedef struct {
    int* old_present;
    int* new_present;
    int old_capacity;
    int new_capacity;
    int rehash_idx;     // 當前重新雜湊進度
    bool rehashing;
} HashSet;

void hashset_insert(HashSet* set, int key) {
    // 如果正在重新雜湊，先完成一部分
    if (set->rehashing) {
        hashset_rehash_step(set);  // 遷移一個舊桶
    }

    // 決定插入到舊表或新表
    int tbl = set->rehashing && (key % set->old_capacity >= set->rehash_idx)
              ? 1 : 0;  // 0: 舊表，1: 新表

    if (tbl == 0) {
        set->old_present[key % set->old_capacity] = true;
    } else {
        set->new_present[key % set->new_capacity] = true;
    }
}

void hashset_rehash_step(HashSet* set) {
    // 遷移一個舊桶的所有元素到新表
    int idx = set->rehash_idx++;
    for (int i = 0; i < set->old_capacity; i++) {
        if (set->old_present[i] && i % set->old_capacity == idx) {
            int new_idx = i % set->new_capacity;
            set->new_present[new_idx] = true;
        }
    }

    // 完成所有重新雜湊
    if (set->rehash_idx >= set->old_capacity) {
        free(set->old_present);
        set->old_present = set->new_present;
        set->old_capacity = set->new_capacity;
        set->rehashing = false;
    }
}
```

**優勢**: 避免單次大規模重新雜湊堵塞，查詢時最多涉及兩個表。

**進階探問 4-B**:
> "Kernel rhashtable 支援 RCU 讀鎖無競爭查詢，同時執行漸進式重新雜湊。LeetCode 多執行緒版本能否也採用 RCU 或類似機制？"

**進階解答**:
```c
// 簡化的 RCU 風格實作（多執行緒 LeetCode 480）
struct rcu_hashtable {
    struct rcu_head rcu;
    bool* present;
    int capacity;
};

// 讀端: RCU 讀鎖無競爭
void findMedian_rcu(MedianFinder* finder) {
    rcu_read_lock();

    // 直接訪問 deleted 表，無需鎖定
    // 因為 RCU 讀者無法看到舊表被釋放
    while (!heapEmpty(finder->small)) {
        int top = heapTop(finder->small);
        // 下面訪問 finder->deleted 是無鎖的
        if (rcu_dereference(finder->deleted)->present[top % capacity]) {
            heapPop(finder->small);
        } else {
            break;
        }
    }

    rcu_read_unlock();
}

// 寫端: 新增 deleted 表版本，延遲回收舊版本
void removeNum_rcu(MedianFinder* finder, int num) {
    struct rcu_hashtable* new_deleted = rcu_assign_pointer(
        finder->deleted,
        rhashtable_expand(finder->deleted, num)
    );

    // 延遲回收舊 deleted 表
    call_rcu(&old_deleted->rcu, free_deleted_callback);
}
```

**複雜度影響**:
- 讀端 (findMedian): O(log k) + O(1) RCU 鎖 = O(log k)
- 寫端 (removeNum): O(log k) + O(全域同步) = O(log k) + O(微秒級別)

**測試技能**:
- 動態擴展與時間複雜度的互動
- 漸進式重新雜湊算法
- RCU 與多執行緒 (Advanced)

---

### 問題 5: 堆性質的驗證與單元測試

**主管提問**:
> "在 Code Review 中，我需要驗證你的堆實作正確地保持了堆性質（父 >= 子或父 <= 子）。如何設計單元測試？Linux Kernel 中的 `lib/min_heap.h` 有對應的測試機制嗎？"

**預期解答**:

LeetCode 480 堆驗證:
```c
// 堆驗證函數
bool isValidHeap(int* heap, int size, bool isMaxHeap) {
    for (int i = 1; i < size; i++) {
        int parent = (i - 1) / 2;
        if (isMaxHeap) {
            // 最大堆: 父 >= 子
            if (heap[parent] < heap[i]) {
                return false;
            }
        } else {
            // 最小堆: 父 <= 子
            if (heap[parent] > heap[i]) {
                return false;
            }
        }
    }
    return true;
}

// 單元測試
void test_heap_property() {
    int nums[] = {10, 5, 8, 3, 2};

    // 建堆
    for (int i = 0; i < 5; i++) {
        heapPush(heap, nums[i]);
    }

    // 驗證堆性質
    assert(isValidHeap(heap->data, heap->size, true));  // MaxHeap

    // 隨機操作
    heapPop(heap);
    heapPush(heap, 7);

    assert(isValidHeap(heap->data, heap->size, true));
}

// 統計測試
void test_heap_median() {
    MedianFinder* finder = medianFinderCreate();
    int nums[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        medianFinderAddNum(finder, nums[i]);
    }

    double median = medianFinderFindMedian(finder);
    assert(median == 3.0);  // [1,2,3,4,5] 的中位數
}
```

**Linux Kernel 的堆測試**:

在 `tools/testing/selftests/` 下有 Kernel 子系統的測試，但 `lib/min_heap.h` 作為通用庫，測試通常由使用者自行設計。例如，perf 和 cgroup 的測試會隱含測試堆性質。

```bash
# 在 Kernel 原始碼中查找 min_heap 的使用
$ grep -r "min_heap_push\|min_heap_pop" linux/

# Linux 6.0+ 中，min_heap 用於跟蹤進程優先級等
```

**Kernel 隱含的堆測試** (概念):
```c
/* kernel/sched/pelt.c 使用 min_heap 管理進程優先級 */

// 初始化堆
min_heap_init(&heap, GFP_KERNEL);

// 批量插入
for_each_process(p) {
    min_heap_push(&heap, p, prio_cmp, NULL);
}

// 驗證: 最低優先級進程應在堆頂
struct task_struct *lowest = heap.data[0];
assert(lowest->prio >= all_tasks[i].prio);

// 出堆
struct task_struct *p = heap.data[0];
min_heap_pop(&heap, prio_cmp, NULL);

// 重新檢驗堆性質
for (int i = 1; i < heap.nr; i++) {
    int parent = (i - 1) / 2;
    assert(heap.data[parent]->prio <= heap.data[i]->prio);
}
```

**進階探問 5-A**:
> "如何測試延遲刪除的正確性？特別是在堆中多次標記刪除後，中位數計算的準確性？"

**進階解答**:
```c
void test_lazy_deletion_correctness() {
    MedianFinder* finder = medianFinderCreate();

    // 插入 [1, 2, 3, 4, 5]
    int nums[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        medianFinderAddNum(finder, nums[i]);
    }

    // 中位數應為 3
    assert(medianFinderFindMedian(finder) == 3.0);

    // 標記刪除 2 和 4（不立即刪除）
    medianFinderDeleteNum(finder, 2);
    medianFinderDeleteNum(finder, 4);

    // 有效元素: [1, 3, 5]，中位數應為 3
    assert(medianFinderFindMedian(finder) == 3.0);

    // 再標記刪除 1
    medianFinderDeleteNum(finder, 1);

    // 有效元素: [3, 5]，中位數應為 4.0
    assert(medianFinderFindMedian(finder) == 4.0);

    // 邊界: 刪除所有元素
    medianFinderDeleteNum(finder, 3);
    medianFinderDeleteNum(finder, 5);

    // 應處理空堆邊界情況
    // assert(medianFinderFindMedian(finder) 的定義行為)
}

// 壓力測試: 大量插入和刪除
void test_lazy_deletion_stress() {
    MedianFinder* finder = medianFinderCreate();
    int n = 10000;

    for (int i = 0; i < n; i++) {
        medianFinderAddNum(finder, rand() % 1000);
    }

    // 隨機刪除 50% 的元素
    for (int i = 0; i < n / 2; i++) {
        medianFinderDeleteNum(finder, rand() % 1000);
    }

    // 中位數應仍然有效
    double median = medianFinderFindMedian(finder);
    assert(median >= 0 && median < 1000);
}
```

**進階探問 5-B**:
> "Kernel 的 RCU 機制如何測試其延遲回收保證？能否應用類似的思想到 LeetCode 480？"

**進階解答**:

RCU 測試 (Kernel):
```bash
# Kernel RCU 自測機制
$ grep -r "rcu_torture" linux/kernel/rcu/

# rcutorture 模塊會並行進行讀寫操作，驗證 RCU 的延遲回收保證
```

RCU 測試的核心思想:
```c
/* 簡化的 RCU 壓力測試邏輯 */

// 讀者執行緒 (多個)
void* rcu_reader_thread(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        rcu_read_lock();
        struct data* d = rcu_dereference(global_data);
        // 驗證 d 的一致性
        assert(d->magic == MAGIC);
        rcu_read_unlock();
    }
}

// 寫者執行緒
void* rcu_writer_thread(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        struct data* new_data = alloc_data();
        struct data* old_data = global_data;

        rcu_assign_pointer(global_data, new_data);
        synchronize_rcu();  // 等待寬限期

        free(old_data);  // 安全回收
    }
}

// 驗證: 沒有讀者看到被釋放的資料
// 沒有 use-after-free 或記憶體洩漏
```

**在 LeetCode 480 多執行緒版本中應用**:
```c
// 多執行緒 LeetCode 480 測試
void test_concurrent_deletion() {
    MedianFinder* finder = medianFinderCreate();
    pthread_t reader, writer;

    // 寫者執行緒: 持續添加和刪除元素
    pthread_create(&writer, NULL, writer_thread, finder);

    // 讀者執行緒: 持續查詢中位數
    pthread_create(&reader, NULL, reader_thread, finder);

    // 驗證: 查詢應總是返回有效的中位數
    // 無記憶體洩漏（使用 Valgrind）
    // 無資料競爭（使用 ThreadSanitizer）

    pthread_join(reader, NULL);
    pthread_join(writer, NULL);
}
```

**測試技能**:
- 堆性質驗證演算法
- 延遲刪除的統計測試
- 並發正確性驗證
- 邊界情況覆蓋

---

## 4. 面試加分總結

### 核心要點

- **堆與優先佇列**:
  - 理解堆的 sift_up / sift_down 操作對應 Linux Kernel `lib/min_heap.h` 的實作
  - 解釋時間複雜度 O(log n) 如何源自樹的高度
  - 對比二叉堆、斐波那契堆等變種

- **雙堆策略**:
  - 解釋為何需要 max-heap + min-heap 來找中位數（分割問題）
  - 展示堆的重新平衡邏輯 (balance check)
  - 對比 Kernel 中使用單一優先佇列的場景

- **延遲刪除模式**:
  - 描述 HashSet 標記已刪除元素的 O(1) 成本
  - 解釋懶加載清理如何在多執行緒環境中使用 RCU
  - 對比 Linux Timer Wheel、RCU 等內核機制的延遲回收策略
  - 在多執行緒版本中使用讀寫鎖或 RCU 提升併發度

- **滑動視窗優化**:
  - 在堆方式 (精確中位數) 與 EWMA 增量統計 (高效但近似) 間權衡
  - 舉例 Linux TCP 擁塞控制使用 EWMA 而非堆的原因
  - 邊界情況: 視窗大小 k 超過陣列大小 numsSize

- **動態雜湊擴展**:
  - 實現自動負載因子檢查 (如 Kernel rhashtable 的 75% 閾值)
  - 漸進式重新雜湊以避免停頓
  - 在多執行緒中搭配 RCU 實現無鎖讀取

- **測試與驗證**:
  - 堆性質檢驗演算法 (遍歷檢查父子關係)
  - 延遲刪除的正確性測試 (插入、刪除、查詢的排列組合)
  - 併發測試 (多執行緒下的資料一致性)
  - 邊界情況 (空堆、單元素、重複元素)

### 面試語境連結

1. **系統設計題**:
   - "設計一個線上數據流的即時中位數計算系統"
   - 使用雙堆 + 延遲刪除，並解釋為何優於排序方案
   - 討論分佈式版本中的聚合策略 (各節點本地堆，匯總層合併)

2. **效能優化**:
   - "我們的監控系統每秒需要計算 100 萬次滑動視窗的統計量"
   - 提議使用 EWMA 而非堆，時間複雜度從 O(log k) 降至 O(1)
   - 說明 Kernel 中 TCP 擁塞控制採用相同策略的原因

3. **多執行緒設計**:
   - "實現執行緒安全的中位數計算器，支援並行讀寫"
   - 方案 1: 互斥鎖 (簡單但低效)
   - 方案 2: 讀寫鎖 (分離讀寫競爭)
   - 方案 3: RCU (最優，但需深入理解 Kernel 概念)

4. **記憶體管理**:
   - "我們的嵌入式系統只有 1MB 記憶體，無法保存所有視窗元素"
   - 提議改用 EWMA，記憶體從 O(k) 降至 O(1)
   - 討論精度與資源的權衡 (近似 v.s. 精確)

5. **Kernel 知識加分**:
   - 主動提及 Linux 內核在類似場景的解決方案
   - "你的堆實作類似於 Kernel `lib/min_heap.h` 的設計"
   - "延遲刪除模式在 RCU 中應用得更激進，支援無鎖讀取"
   - "TCP 擁塞控制中的 EWMA 是 `include/linux/average.h` 的實際應用"

### 高級話題 (進一步深入)

- **無鎖資料結構**:
  - Kernel 中的 RCU、seqlock、atomic 操作
  - 在 LeetCode 480 中應用 atomic CAS 實現無鎖堆 (概念性)

- **硬體緩存親和性**:
  - 堆的陣列實作對 CPU 快取友好 (局域性)
  - vs. 鏈式堆的快取失敗

- **實時保證**:
  - Kernel 中的 O(log n) 操作適合實時系統 (有界延遲)
  - vs. 動態雜湊擴展的不可預測成本

- **數值穩定性**:
  - EWMA 的數值精度問題 (權重衰減與誤差累積)
  - Kernel 中如何使用定點運算而非浮點

---

## 5. Sources

### Linux Kernel 原始碼

- **Min Heap Implementation**:
  - [lib/min_heap.h (Linux GitHub)](https://github.com/torvalds/linux/blob/master/lib/min_heap.h)
  - [lib/prio_heap.c (Linux GitHub)](https://github.com/torvalds/linux/blob/master/lib/prio_heap.c)

- **Average / EWMA**:
  - [include/linux/average.h (Linux GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/average.h)
  - [kernel/sched/pelt.c - Load Tracking (Linux GitHub)](https://github.com/torvalds/linux/blob/master/kernel/sched/pelt.c)

- **Hash Tables**:
  - [include/linux/hashtable.h (Linux GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h)
  - [lib/rhashtable.c (Linux GitHub)](https://github.com/torvalds/linux/blob/master/lib/rhashtable.c)

- **RCU (Read-Copy-Update)**:
  - [kernel/rcu/tree.c (Linux GitHub)](https://github.com/torvalds/linux/blob/master/kernel/rcu/tree.c)
  - [include/linux/rcu_types.h (Linux GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/rcu_types.h)

- **Timer Wheel**:
  - [kernel/time/timer.c (Linux GitHub)](https://github.com/torvalds/linux/blob/master/kernel/time/timer.c)

- **TCP Congestion Control**:
  - [net/ipv4/tcp_input.c (Linux GitHub)](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c)
  - [include/net/tcp.h (Linux GitHub)](https://github.com/torvalds/linux/blob/master/include/net/tcp.h)

### LWN (Linux Weekly News) 文章

- **RCU Introduction**:
  - [What is RCU? (LWN)](https://lwn.net/Articles/262464/)
  - [RCU Usage Patterns (LWN)](https://lwn.net/Articles/573424/)

- **Kernel Data Structures**:
  - [Hash Tables in the Linux Kernel (LWN)](https://lwn.net/Articles/570970/)
  - [Priority Queues (LWN)](https://lwn.net/Kernel/Index#Priority_queues)

- **Performance & Scalability**:
  - [Lockless Synchronization (LWN)](https://lwn.net/Articles/820556/)
  - [CPU Cache and Performance (LWN)](https://lwn.net/Articles/252125/)

### 相關技術文章

- **Exponential Weighted Moving Average**:
  - [EWMA in Networking (Wikipedia)](https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average)
  - [TCP Congestion Control (RFC 5681)](https://tools.ietf.org/html/rfc5681)

- **堆演算法**:
  - [Binary Heap Implementation (GeeksforGeeks)](https://www.geeksforgeeks.org/binary-heap/)
  - [Heapsort Analysis (Khan Academy)](https://www.khanacademy.org/computing/computer-science/algorithms/sorting-algorithms/a/heapsort)

- **Sliding Window / Streaming Algorithms**:
  - [Streaming Algorithms Overview (Stanford CS166)](https://web.stanford.edu/class/cs166/)
  - [Median Finding in Data Streams (LeetCode Discuss)](https://leetcode.com/problems/find-median-from-data-stream/)

---

**檔案建立日期**: 2026-03-30
**適用 LeetCode 問題**: 480 - Sliding Window Median
**Kernel 版本**: Linux 5.0+
**語言**: 繁體中文 (Traditional Chinese) with English identifiers and code snippets
