# LeetCode 1431: Kids With the Greatest Number of Candies

## 問題概述

給定一個陣列 `candies`，其中 `candies[i]` 表示第 `i` 個小孩擁有的糖果數量，以及一個額外的糖果數 `extraCandies`。

**任務**：判斷如果我們給每個小孩額外的糖果，是否每個小孩的糖果數量都會成為（至少與目前的最大值相等或更多）。

**輸出**：返回一個布林陣列，其中第 `i` 個元素表示第 `i` 個小孩在獲得 `extraCandies` 後是否會有最多（或並列最多）的糖果。

---

## 核心演算法：二次線性掃描模式

該問題的標準解法遵循簡單而高效的模式：

### 第一次掃描：找出最大值
```c
int maxCandies = candies[0];
for (int i = 1; i < candiesSize; i++) {
    if (candies[i] > maxCandies) {
        maxCandies = candies[i];
    }
}
```

### 第二次掃描：比較與決策
```c
bool* result = (bool*)malloc(candiesSize * sizeof(bool));
for (int i = 0; i < candiesSize; i++) {
    result[i] = (candies[i] + extraCandies >= maxCandies);
}
return result;
```

**時間複雜度**：O(n)（兩次線性掃描）
**空間複雜度**：O(1)（不計算輸出陣列）

---

## Linux 相關原始碼位置

### 1. `lib/find_bit.c` 與 `include/linux/bitmap.h`
**用途**：位元圖掃描函式

這些檔案實現了核心的位元圖操作，例如 `find_first_bit()`、`find_next_bit()`。它們通過線性迭代來找到特定狀態的位元，這與我們找最大值的線性掃描完全相同：

```c
// 核心概念：遍歷陣列找符合條件的元素
unsigned long find_first_bit(const unsigned long *addr, unsigned long size);
```

**連接點**：與 LeetCode 1431 的相似性在於都是進行陣列掃描以找到特定條件的元素。

**GitHub 連結**：
- https://github.com/torvalds/linux/blob/master/lib/find_bit.c
- https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h

---

### 2. `kernel/sched/fair.c` — 排程器負載平衡
**用途**：CPU 執行佇列的負載均衡決策

核心排程器中的負載平衡演算法使用完全相同的模式：

**第一步：找出最繁忙的排程群組**（類似找最大值）
```c
// kernel/sched/fair.c 中的概念
struct sched_group *find_busiest_group(struct lb_env *env)
{
    // 掃描所有排程群組，找出最繁忙的一個
    for_each_sched_group(env->sd, group) {
        // 計算該群組的負載
        update_sg_lb_stats(env, group, ...);
        if (group->load > max_load) {
            max_load = group->load;
            busiest = group;
        }
    }
    return busiest;
}
```

**第二步：針對每個 CPU 進行比較決策**（類似比較每個元素）
```c
// 決定是否應該從繁忙群組遷移工作到目前群組
if (busiest_group->load >= threshold) {
    // 遷移工作
}
```

**演算法相似性**：
| LeetCode 1431 | Linux 排程器 |
|---------------|-------------|
| 找陣列中的最大值 | 找所有 CPU 群組中負載最大的 |
| 遍歷陣列比較每個元素 + extraCandies | 遍歷每個 CPU，判斷是否應從繁忙群組遷移 |
| O(n) 兩次掃描 | O(m) 群組掃描 + O(n) CPU 掃描 |

**GitHub 連結**：https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c

---

### 3. `include/linux/minmax.h` — 型別安全的最大值/最小值宏
**用途**：核心提供的最大值、最小值、範圍限制宏定義

```c
#define max(a, b) ({            \
    typeof(a) _max1 = (a);      \
    typeof(b) _max2 = (b);      \
    (void) (&_max1 == &_max2);  \
    _max1 > _max2 ? _max1 : _max2; })

#define min(a, b) ({            \
    typeof(a) _min1 = (a);      \
    typeof(b) _min2 = (b);      \
    (void) (&_min1 == &_min2);  \
    _min1 < _min2 ? _min1 : _min2; })

#define clamp(val, lo, hi)      \
    min(max(val, lo), hi)
```

**連接點**：
- LeetCode 1431 的第一次掃描本質上就是在重複呼叫 `max()` 巨集
- 核心的 `max()` 和 `min()` 實現了型別檢查和型別安全
- 我們的簡單迴圈可以用核心提供的工具改寫

**GitHub 連結**：https://github.com/torvalds/linux/blob/master/include/linux/minmax.h

---

### 4. `lib/sort.c` — 陣列排序實現
**用途**：快速排序實現（quicksort）

雖然 LeetCode 1431 不需要排序，但排序是找最大值的另一種方法：

```c
// 排序後，最大值就在陣列末尾
sort(candies, n);
int maxCandies = candies[n-1];
```

**效能比較**：
| 方法 | 時間複雜度 | 說明 |
|------|-----------|------|
| 線性掃描（我們的做法） | O(n) | 最優 |
| 排序後取末尾 | O(n log n) | 不必要的開銷 |
| 堆積（Heap） | O(n) 建立 + O(1) 取最大 | 更複雜 |

**GitHub 連結**：https://github.com/torvalds/linux/blob/master/lib/sort.c

---

## Kernel vs. LeetCode 實作對照

### 場景 1：位元圖掃描 vs. 陣列掃描

**LeetCode 1431：**
```c
int maxCandies = candies[0];
for (int i = 1; i < candiesSize; i++) {
    if (candies[i] > maxCandies) {
        maxCandies = candies[i];
    }
}
```

**Kernel `find_bit.c`：**
```c
// 掃描位元圖找第一個設定位
unsigned long find_first_bit(const unsigned long *addr, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        if (addr[i] != 0) {
            return __ffs(addr[i]) + i * BITS_PER_LONG;
        }
    }
    return size;
}
```

**相似之處**：都是從左到右線性掃描，找到滿足條件的第一個位置。

---

### 場景 2：決策邏輯的對照

**LeetCode 1431（決策層）：**
```c
for (int i = 0; i < candiesSize; i++) {
    result[i] = (candies[i] + extraCandies >= maxCandies);
}
```

**Kernel `sched/fair.c`（排程決策）：**
```c
// 簡化版本
for_each_cpu_in_group(cpu, group) {
    cpu_load[cpu] = get_cpu_load(cpu);
    should_migrate[cpu] = (cpu_load[cpu] < average_load);
}
```

**相似之處**：
- 兩者都是 O(n) 的第二次遍歷
- 都基於第一次掃描得出的聚合結果（最大值、平均負載）
- 都是「找出符合條件的候選對象」的模式

---

### 場景 3：型別考量

**LeetCode 1431：**
```c
// C 語言需要手動處理型別
bool* result = (bool*)malloc(candiesSize * sizeof(bool));
```

**Kernel `minmax.h`：**
```c
#define max(a, b) ({            \
    typeof(a) _max1 = (a);      \  // 型別檢查！
    typeof(b) _max2 = (b);      \
    (void) (&_max1 == &_max2);  \  // 編譯時檢查
    _max1 > _max2 ? _max1 : _max2; })
```

**差異**：
- Kernel 通過 `typeof()` 提供編譯時型別安全檢查
- LeetCode 通常沒有這類檢查
- 產業級程式碼應該考慮型別安全

---

## 主管 Code Review 角度考題

### 問題 1：時間複雜度的權衡

**Reviewer 提問**：
> 「我注意到你用了兩次線性掃描。有沒有考慮過只掃描一次？比如說在單次掃描中同時追蹤最大值和結果？」

**預期回答方向**：
- 在單次掃描中無法計算結果（因為要和最大值比較，而最大值要掃描完才知道）
- 必須先知道全域最大值才能做出決策
- 這是資料依賴性的問題，不可避免

**追問**（測試深度理解）：
- 「如果 extraCandies 是未知的呢？」（答：仍然需要兩次掃描）
- 「能否用分治法或平行化來改善？」（答：可以平行化第一次掃描找最大值，但整體仍是 O(n)）

**考驗重點**：是否理解演算法的基本限制和資料依賴性

---

### 問題 2：記憶體管理與資源洩漏

**Reviewer 提問**：
> 「你的 C 實作中使用了 `malloc()`。在核心程式中，我們通常避免執行時記憶體分配。你如何看待動態 vs. 靜態分配的權衡？」

**預期回答方向**：
- LeetCode 環境中 `malloc()` 是標準做法
- Kernel 中傾向於靜態分配或棧上分配（避免 OOM）
- 需要清楚地呼叫 `free()` 防止洩漏
- 在產業程式碼中應該使用 RAII 或自動資源管理

**程式碼範例**（Kernel 風格）：
```c
// 不推薦：動態分配
bool *result = kmalloc(candiesSize * sizeof(bool), GFP_KERNEL);

// 推薦：棧分配（如果大小已知）
bool result[MAX_KIDS];

// 或使用 alloca()（臨時分配）
bool *result = alloca(candiesSize * sizeof(bool));
```

**追問**（測試系統設計思維）：
- 「如果 candiesSize 非常大怎麼辦？」（答：需要考慮棧溢出，可能需要分批處理）
- 「kernel 中如何確保沒有記憶體洩漏？」（答：使用靜態分析工具、代碼審查、測試）

**考驗重點**：系統級別的資源管理思維

---

### 問題 3：邊界條件與例外處理

**Reviewer 提問**：
> 「代碼中沒有檢查空陣列或 NULL 指標。在核心程式碼中，我們如何處理這類邊界情況？」

**預期回答方向**：
- LeetCode 通常假設有效輸入（`1 <= n <= 100`）
- 核心程式碼必須假設輸入可能無效
- 應添加防禦性檢查和錯誤碼返回

**Kernel 風格的實作**：
```c
// 防禦性檢查
if (unlikely(!candies || candiesSize == 0)) {
    return -EINVAL;  // 或使用 ERR_PTR()
}

// 確保 maxCandies 初始化正確
if (candiesSize == 0) {
    return NULL;
}
```

**追問**（測試健壯性思維）：
- 「如果 extraCandies 是負數呢？」（答：根據規約判斷是否允許）
- 「整數溢出怎麼辦？」（答：應該使用 checked arithmetic 或驗證範圍）

**考驗重點**：防禦性程式設計和例外處理的意識

---

### 問題 4：演算法的變體與最佳化

**Reviewer 提問**：
> 「假設我們要在同一陣列中多次執行這個操作（固定陣列，但 extraCandies 不同）。你會如何設計？」

**預期回答方向**：
- 預先計算並快取最大值（O(1) 後續查詢）
- 類似於 Kernel 排程器的「定期更新」策略
- 權衡重新計算 vs. 快取的成本

**優化設計**：
```c
// 預計算結構
struct CandyContext {
    int *candies;
    int candiesSize;
    int maxCandies;  // 快取
    int lastExtraCandies;
    bool *lastResult;  // 快取
};

// 只在需要時重新計算
bool* processIfNeeded(struct CandyContext *ctx, int extraCandies) {
    if (ctx->maxCandies == -1) {
        // 第一次：計算最大值
        ctx->maxCandies = findMax(ctx->candies, ctx->candiesSize);
    }

    // 重複使用 maxCandies
    return compareEach(ctx->candies, ctx->candiesSize,
                       ctx->maxCandies, extraCandies);
}
```

**追問**（測試系統設計思維）：
- 「快取何時失效？」（答：當陣列改變時）
- 「多執行緒環境下如何同步？」（答：使用互斥鎖或原子操作，如同 Kernel）

**考驗重點**：系統設計、快取策略、並髮安全性

---

### 問題 5：與 Kernel 概念的類比

**Reviewer 提問**：
> 「這個演算法在 Linux kernel 中有類似的應用嗎？你能舉例說明相似的模式嗎？」

**預期回答方向**：
- Kernel 排程器中的「找最繁忙 CPU」+ 「決定遷移」
- 記憶體管理中的「找最大空閒區塊」+ 「分配決策」
- 中斷處理中的「找最高優先級」+ 「調度決策」

**具體例子**：
```c
// kernel/sched/fair.c 的類比

// 步驟 1：找最繁忙的排程群組（如同找 max）
int max_load = -1;
struct sched_group *busiest_group = NULL;
for (int i = 0; i < num_groups; i++) {
    if (groups[i]->load > max_load) {
        max_load = groups[i]->load;
        busiest_group = groups[i];
    }
}

// 步驟 2：為每個 CPU 決定是否遷移（如同比較 + extraCandies）
bool *should_pull = malloc(num_cpus * sizeof(bool));
for (int i = 0; i < num_cpus; i++) {
    should_pull[i] = (cpus[i]->load < max_load - THRESHOLD);
}
```

**追問**（測試知識整合）：
- 「排程器為何不直接用排序？」（答：O(n) 比 O(n log n) 快）
- 「如何避免排程抖動（thrashing）？」（答：添加遲滯（hysteresis）或最小遷移門檻）

**考驗重點**：跨領域知識整合與系統理解

---

## 面試加分總結

### 1. 演算法基礎
- ✅ 理解二次線性掃描的必要性（資料依賴性）
- ✅ 時間複雜度 O(n)、空間複雜度 O(1) 的清晰分析
- ✅ 能夠解釋為何單次掃描不可行

### 2. 系統級思維
- ✅ 認識到該演算法在 Kernel 中有對應應用（排程、記憶體管理）
- ✅ 理解「找最大/最小值」的通用模式在系統設計中的重要性
- ✅ 能夠類比 Kernel 排程器的負載平衡邏輯

### 3. 程式碼品質
- ✅ 正確處理記憶體分配與釋放（no memory leak）
- ✅ 添加邊界條件檢查（even though LeetCode doesn't require）
- ✅ 使用型別安全的做法（參考 Kernel 的 `typeof()` 宏）

### 4. 深度理解
- ✅ 能夠解釋為什麼 Kernel 排程器採用相同的模式
- ✅ 理解快取策略與多次查詢的最佳化
- ✅ 討論並髮安全性和多執行緒的考量

### 5. 面試表現方向
```
「這個看似簡單的問題涉及了幾個深層概念：

首先，兩次掃描不是無意義的重複——它反映了資料依賴性。
我們必須先掃描一遍知道全域最大值，才能進行決策。

這個模式在 Linux kernel 中隨處可見。以排程器為例：
- 第一遍掃描找出負載最高的 CPU 群組（類似找 max）
- 第二遍掃描決定哪些 CPU 應該從該群組拉取工作（類似比較）

相比直接排序（O(n log n)），線性掃描（O(n)）更有效率——
這對 Kernel 至關重要，因為排程決策必須頻繁執行。

在產業程式碼中，我還會考慮：
1. 記憶體管理：使用棧或預分配，避免執行時 malloc
2. 快取策略：如果需要多次查詢，預先計算最大值
3. 邊界檢查：防禦式程式設計
4. 並髮安全：多執行緒環境下的同步

這個簡單問題實際上反映了系統設計的核心原則。」
```

---

## Sources

### LeetCode 問題
- Problem 1431: https://leetcode.com/problems/kids-with-the-greatest-number-of-candies/

### Linux Kernel 相關源代碼
- Linux Kernel 主倉庫: https://github.com/torvalds/linux

#### 具體文件連結
1. **find_bit.c** - 位元圖掃描
   - https://github.com/torvalds/linux/blob/master/lib/find_bit.c
   - 函式：`find_first_bit()`, `find_next_bit()`

2. **fair.c** - 排程器負載平衡
   - https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c
   - 函式：`find_busiest_group()`, `update_sg_lb_stats()`

3. **minmax.h** - 型別安全的最大值/最小值宏
   - https://github.com/torvalds/linux/blob/master/include/linux/minmax.h
   - 宏：`max()`, `min()`, `clamp()`

4. **sort.c** - 陣列排序實現
   - https://github.com/torvalds/linux/blob/master/lib/sort.c
   - 函式：`sort()`

5. **bitmap.h** - 位元圖定義和函式
   - https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h

### 參考資料
- Linux Kernel 開發郵件列表：https://lkml.org
- Kernel Newbies：https://kernelnewbies.org
- LWN.net - Linux 核心及相關技術新聞：https://lwn.net
