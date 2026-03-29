# LeetCode 53 — Maximum Subarray 與 Linux 核心的連結

## 問題概述

**最大子陣列問題** (Maximum Subarray Problem)：在陣列中找一個連續子陣列，使其元素總和最大，時間複雜度為 O(N)。

**Kadane's Algorithm**：動態規劃演算法，維護一個執行中的最大和，逐步掃過陣列，決定何時延續前一個子陣列或重新開始。

```c
int maxSubArray(int* nums, int numsSize) {
    int max_current = nums[0];
    int max_global = nums[0];

    for (int i = 1; i < numsSize; i++) {
        max_current = (nums[i] > max_current + nums[i]) ? nums[i] : max_current + nums[i];
        if (max_current > max_global) {
            max_global = max_current;
        }
    }

    return max_global;
}
```

---

## Linux 相關原始碼位置

### 1. `kernel/sched/pelt.c` — Per-Entity Load Tracking (PELT)

**位置**：`kernel/sched/pelt.c` (CFS Scheduler)

**核心概念**：
- Linux 的 CFS (完全公平排程器) 使用指數加權移動平均 (EWMA) 來追蹤 CPU 負載
- 每個工作實體（task entity）維護一個執行中的負載指標，類似於 Kadane's 的 `max_current`
- 定期更新時考慮衰減因素，類似於決定何時重置或延續窗口

**相關程式碼結構**：
```c
/* kernel/sched/pelt.c 中的核心邏輯 */
struct sched_avg {
    u64 last_update_time;
    u64 load_sum;
    u64 runnable_sum;
    u32 util_sum;
    u32 period_contrib;
    unsigned long load_avg;
    unsigned long util_avg;
    unsigned long runnable_avg;
};

/* 更新負載指標 (類似 Kadane's 的迭代步驟) */
void __update_load_avg_se(u64 now, struct cfs_rq *cfs_rq, struct sched_entity *se)
{
    unsigned long divider = get_pelt_divider(se_load_period_avg(se));
    long delta = (now - se->avg.last_update_time);

    se->avg.load_sum += se_weight(se) * delta;
    se->avg.load_avg = div_u64(se->avg.load_sum, divider);

    if (se->avg.load_avg > se->avg.last_max_load)
        se->avg.last_max_load = se->avg.load_avg;
}
```

**與 Kadane's 的相似之處**：
- 都是執行中的指標追蹤 (running metric)
- 都需要決定何時「重置」或「延續」
- 都使用線性掃描避免額外的記憶體

---

### 2. `net/netfilter/nf_conntrack_core.c` — Connection Tracking

**位置**：`net/netfilter/nf_conntrack_core.c` 和 `net/netfilter/nf_conntrack.h`

**核心概念**：
- `nf_conntrack` 模組追蹤網路連線狀態並維護執行中的計數器
- 每個連線追蹤器 (`struct nf_conn`) 維護統計資訊，決定何時接受/拒絕封包
- 類似於 Kadane's 決定何時延續或重啟窗口

**相關程式碼結構**：
```c
/* net/netfilter/nf_conntrack.h */
struct nf_conn {
    struct nf_conntrack_tuple_hash tuplehash[IP_CT_DIR_MAX];
    unsigned long status;
    u32 timeout;
    struct nf_ct_counter counters[IP_CT_DIR_MAX];
    unsigned long jiffies;
    spinlock_t lock;
    /* ... 其他欄位 */
};

struct nf_ct_counter {
    atomic64_t packets;  /* 執行計數器 */
    atomic64_t bytes;
};

/* 更新計數 (類似 max_current += nums[i]) */
static inline void nf_ct_acct_update(struct nf_conn *ct,
                                      enum ip_conntrack_dir dir,
                                      unsigned int len)
{
    atomic64_add(len, &ct->counters[dir].bytes);
    atomic64_inc(&ct->counters[dir].packets);
}
```

**與 Kadane's 的相似之處**：
- 執行中維護統計值
- 流式資料處理（封包流）
- 決策點：何時接受（類似延續）或重置

---

### 3. `mm/vmstat.c` — Memory Statistics

**位置**：`mm/vmstat.c` 和 `include/linux/vmstat.h`

**核心概念**：
- 核心維護各個記憶體區域 (zone) 的統計資訊
- 使用執行和與重置模式來追蹤分配/解放事件
- Per-CPU 統計以降低競爭

**相關程式碼結構**：
```c
/* mm/vmstat.c */
struct vm_event_state {
    unsigned long event[NR_VM_EVENT_ITEMS];
};

static DEFINE_PER_CPU(struct vm_event_state, vm_event_states) = {{0}};

/* 累加統計 (類似 max_current += nums[i]) */
static void __count_vm_event(enum vm_event_item item)
{
    __this_cpu_inc(vm_event_states.event[item]);
}

/* 定期同步全域統計 (決定何時「重置」本地計數) */
void all_vm_events(unsigned long *ret)
{
    int cpu;
    int i;

    memset(ret, 0, NR_VM_EVENT_ITEMS * sizeof(unsigned long));

    for_each_online_cpu(cpu) {
        struct vm_event_state *this = &per_cpu(vm_event_states, cpu);

        for (i = 0; i < NR_VM_EVENT_ITEMS; i++)
            ret[i] += this->event[i];
    }
}
```

**與 Kadane's 的相似之處**：
- Per-CPU 累加器類似於 `max_current`
- 全域同步類似於 `max_global = max(max_global, max_current)`

---

### 4. `lib/` — Running Statistics Utilities

**位置**：`lib/math/` 和 `lib/`

**核心概念**：
- Linux 提供了執行統計的工具函式
- 例如 `ewma.c` (指數加權移動平均)，`reciprocal_div.c` (最優化除法)

**相關程式碼位置**：
```c
/* lib/math/reciprocal_div.c - 用於快速除法 (最優化常數計算) */
u32 reciprocal_scale(u32 val, u32 ep_ro)
{
    return (u64)val * ep_ro >> 32;
}

/* lib/ewma.c - 指數加權移動平均 */
struct ewma {
    unsigned long internal;
};

void ewma_add(struct ewma *avg, unsigned long val)
{
    avg->internal = avg->internal * 15 / 16 + val;
}
```

**與 Kadane's 的相似之處**：
- 都是執行統計的核心模式
- 最優化除法以應對高頻率更新

---

## Kernel vs. LeetCode 實作對照

| 面向 | Kadane's Algorithm | Linux Kernel |
|------|-------------------|--------------|
| **執行指標** | `max_current` | `load_avg`, `counters`, `event` |
| **重置邏輯** | `max_current = nums[i]` | Per-CPU reset, timeout expiry |
| **全域追蹤** | `max_global` | Per-CPU aggregation |
| **資料流** | 陣列掃描 | 連續事件（CPU 時間、封包、記憶體操作） |
| **最優化** | 分支預測 (cache locality) | 原子操作、per-CPU 鎖定 |
| **整數溢出** | 需要檢查 (INT_MIN case) | 使用 `atomic64_t` 或模組運算 |

### 實作細節對比

**Kadane's 的決策點**：
```c
max_current = (nums[i] > max_current + nums[i]) ? nums[i] : max_current + nums[i];
```
決策：**該元素是否應該開始新的子陣列？**

**PELT 的類似決策**：
```c
/* 衰減舊負載，累加新貢獻 */
se->avg.load_sum += se_weight(se) * delta;
if (se->avg.load_avg > last_max_load)  /* 追蹤最大值 */
    se->avg.last_max_load = se->avg.load_avg;
```

**Connection Tracking 的類似決策**：
```c
/* 累加計數器，決定連線是否應重置 */
atomic64_add(len, &ct->counters[dir].bytes);
if (ct->counters[dir].packets > threshold)  /* 決策點 */
    reset_conntrack(ct);
```

---

## 主管 Code Review 角度考題

### 1. **快取局部性 (Cache Locality)**

**問題**：
- Kadane's 演算法只掃描一次陣列，實現理想的順序存取模式
- Linux 核心的 PELT 和 vmstat 也遵循相同原則
- 您的實作是否充分利用了快取行 (cache line)?

**考慮因素**：
```c
/* 好的做法 - 順序掃描 */
for (int i = 0; i < numsSize; i++) {
    max_current = max(nums[i], max_current + nums[i]);
}

/* 不好的做法 - 隨機存取 */
for (int i = 0; i < numsSize; i += stride) {
    // 快取不友善的存取模式
}
```

### 2. **分支預測 (Branch Prediction)**

**問題**：
- Kadane's 的 `max_current + nums[i]` vs `nums[i]` 決策可能造成分支預測失敗
- 現代 CPU 在難以預測的分支上表現不佳
- 核心程式碼也使用類似的微最適化技巧

**考慮因素**：
```c
/* 分支預測友善的版本 - 避免明確的 if */
max_current = max_current + nums[i];
max_current = (nums[i] > max_current) ? nums[i] : max_current;

/* 或使用無分支版本 */
int diff = nums[i] - max_current;
max_current += (diff > 0) ? diff : 0;
```

### 3. **整數溢出 (Integer Overflow)**

**問題**：
- 若陣列總和超過 `INT_MAX`，演算法會溢出
- LeetCode 測試案例可能隱含大數值
- Linux 核心使用 `s64`, `atomic64_t` 和檢查

**考慮因素**：
```c
/* 不安全 - 可能溢出 */
int max_current = nums[0];
max_current += nums[i];  // 可能溢出

/* 安全版本 - 使用長整數 */
long max_current = nums[0];
long max_global = nums[0];
for (int i = 1; i < numsSize; i++) {
    max_current = (nums[i] > max_current + nums[i]) ? nums[i] : max_current + nums[i];
    max_global = max(max_global, max_current);
}
```

### 4. **SIMD 潛力 (SIMD Potential)**

**問題**：
- 單純的 Kadane's 難以向量化（資料相依性強）
- Linux 核心在某些統計操作上使用 SIMD
- 是否可能使用向量指令？

**考慮因素**：
- Kadane's 本質上是序列相依的 (`max_current` 依賴前一步驟)
- 可以並行化段落掃描，但需謹慎合併結果

### 5. **並發性與原子性 (Concurrency & Atomicity)**

**問題**：
- 在多執行緒環境下，執行中的統計需要同步機制
- Linux 核心在 per-CPU 統計中解決此問題
- 您的演算法在並發環境中表現如何？

**考慮因素**：
```c
/* 單執行緒 - 安全 */
max_current += nums[i];

/* 多執行緒 - 需要保護 */
pthread_mutex_lock(&mutex);
max_current += nums[i];
pthread_mutex_unlock(&mutex);

/* Linux 模式 - per-CPU 無鎖 */
__this_cpu_add(local_max, nums[i]);
```

### 6. **邊界條件 (Edge Cases)**

**問題**：
- 全負數陣列
- 單一元素陣列
- 溢出情況

**Linux 核心檢查**：
```c
/* 確保初始化 */
if (numsSize == 0) return INT_MIN;  /* 或定義的結果 */
if (numsSize == 1) return nums[0];
```

---

## 面試加分總結

### 核心連結論點

1. **執行統計模式相通**
   - Kadane's 的 `max_current` 與 Linux PELT 的 `load_avg` 解決類似問題
   - 都是在流式資料上維護單一執行指標
   - 面試官會欣賞認識到這些一般化模式

2. **決策點識別**
   - Kadane's 在每步決定是否延續或重啟窗口
   - PELT 和 connection tracking 也有類似的決策邏輯
   - 展示理解演算法的本質，而非只背題目

3. **Linux 最適化技巧**
   - Per-CPU 統計避免鎖定爭競
   - 指數加權平均以平衡精確度和性能
   - 引用核心程式碼顯示深度知識

4. **性能考量**
   - 快取局部性：O(N) 線性掃描與之相容
   - 分支預測：三元運算子 vs. `max()` 巨集
   - 整數溢出：使用 `long` 或檢查
   - 並發：在多執行緒版本中論述 per-CPU 策略

5. **code review 預期**
   - 提前討論溢出風險
   - 評論快取友善性
   - 論述邊界條件的完備性
   - 比較 Linux 核心的類似實作作為參考

6. **分層思考**
   - 理解演算法層 (Kadane's)
   - 硬體層 (CPU 快取、分支預測)
   - 核心層 (PELT、conntrack、vmstat)
   - 展示從不同角度思考同一問題

---

## 關鍵代碼片段對照

### LeetCode 基礎實作
```c
int maxSubArray(int* nums, int numsSize) {
    int max_current = nums[0];
    int max_global = nums[0];

    for (int i = 1; i < numsSize; i++) {
        max_current = (nums[i] > max_current + nums[i]) ? nums[i] : max_current + nums[i];
        max_global = (max_current > max_global) ? max_current : max_global;
    }

    return max_global;
}
```

### Linux 核心類似模式 (PELT 簡化)
```c
/* kernel/sched/pelt.c 邏輯簡化 */
void update_load_tracking(struct sched_entity *se, u64 now) {
    u64 delta = now - se->avg.last_update_time;

    /* 類似 max_current += nums[i] */
    se->avg.load_sum += se_weight(se) * delta;
    se->avg.load_avg = se->avg.load_sum >> PELT_SHIFT;

    /* 類似 max_global = max(max_global, max_current) */
    if (se->avg.load_avg > se->avg.last_max_load)
        se->avg.last_max_load = se->avg.load_avg;

    se->avg.last_update_time = now;
}
```

### Connection Tracking 類似模式
```c
/* net/netfilter/nf_conntrack.c 邏輯簡化 */
void track_connection_stats(struct nf_conn *ct, int bytes) {
    /* 類似 max_current += nums[i] */
    atomic64_add(bytes, &ct->counters[IP_CT_DIR_ORIGINAL].bytes);

    /* 決策點：是否應該重置或延續 */
    if (ct->counters[IP_CT_DIR_ORIGINAL].bytes > MAX_ALLOWED) {
        /* 類似決定是否開啟新子陣列 */
        reset_connection_stats(ct);
    }
}
```

---

## 深化思考：為何 Linux 核心相關？

1. **共同的演算法挑戰**
   - 流式資料處理
   - 執行中的統計追蹤
   - 最小化記憶體和時間複雜度

2. **相同的性能最適化考量**
   - 快取局部性
   - 分支預測友善性
   - 避免昂貴的同步原始

3. **面試價值**
   - 展示對系統層面理解的深度
   - 表現出能跨層思考的能力
   - 從核心程式碼提取通用模式

4. **實務應用**
   - 如果進入核心開發，Kadane's 直覺有助於理解 PELT、vmstat
   - 如果進入性能工程，快取分析直接適用
   - 如果進入網路開發，connection tracking 的思路有啟發

---

## Sources

- **Linux Kernel Source (kernel.org)**
  - `kernel/sched/pelt.c` — Per-Entity Load Tracking
    https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/sched/pelt.c

  - `net/netfilter/nf_conntrack_core.c` — Connection Tracking
    https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/net/netfilter/nf_conntrack_core.c

  - `mm/vmstat.c` — Memory Statistics
    https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/mm/vmstat.c

  - `lib/math/` — Math Utilities
    https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/math

- **Algorithm References**
  - "Maximum Subarray Problem" — Wikipedia
    https://en.wikipedia.org/wiki/Maximum_subarray_problem

  - "Kadane's Algorithm" — GeeksforGeeks
    https://www.geeksforgeeks.org/largest-sum-contiguous-subarray/

- **Performance Analysis**
  - "CPU Caches and Why You Care" — Scott Medin
  - "Branch Prediction" — CPU Design Tutorial
    https://en.wikichip.org/wiki/branch_prediction

- **Linux Scheduler Deep Dive**
  - "Understanding the Linux Scheduler" — kernel.org Documentation
    https://www.kernel.org/doc/html/latest/scheduler/index.html

---

**撰寫日期**：2026年3月29日
**難度**：LeetCode 簡單 (Easy) / 面試深化：進階
**建議準備時間**：30 分鐘基礎理解 + 1 小時核心連結研究
