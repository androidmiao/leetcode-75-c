# Follow-up Patterns 常見後續問題模式

## 核心概念

面試官常在你解完問題後提出後續問題，來測試你的思考深度和優化能力。

**常見後續問題類型：**
1. 空間優化
2. 時間優化
3. 應用場景擴展
4. 系統設計級別的考量
5. 並發/多線程處理

---

## 模式 1: 能在 O(1) 空間內完成嗎？(Can you do it in O(1) space?)

### 什麼時候會問

- 原始解法使用額外陣列或堆疊
- 問題涉及陣列修改或重排

### 應對策略

```c
// 原始解法：O(n) 空間
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int *result = malloc(2 * sizeof(int));  // O(n) 空間用於雜湊表
    // ... 建立雜湊表 ...
    *returnSize = 2;
    return result;
}

// 後續問題：若陣列已排序，能 O(1) 空間嗎？
// 答：是的，使用雙指標
int* twoSum_Sorted(int* nums, int numsSize, int target, int* returnSize) {
    int *result = malloc(2 * sizeof(int));
    int left = 0, right = numsSize - 1;

    // 只使用常數額外空間
    while (left < right) {
        int sum = nums[left] + nums[right];
        if (sum == target) {
            result[0] = left + 1;  // 1-indexed
            result[1] = right + 1;
            *returnSize = 2;
            return result;
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }

    *returnSize = 0;
    return result;
}
```

### 適用問題

| 問題 | 原始空間 | 優化空間 | 方法 |
|------|--------|--------|------|
| 反轉陣列 | O(n) | O(1) | 原地交換 |
| 反轉字串 | O(n) | O(1) | 雙指標 |
| 移除重複 | O(n) | O(1) | 雙指標 + 覆蓋 |
| 旋轉陣列 | O(n) | O(1) | 多次反轉 |

---

## 模式 2: 如果輸入太大無法放入記憶體呢？(What if input is too large for memory?)

### 什麼時候會問

- 涉及陣列、字串等大資料結構
- 測試流式處理能力

### 應對策略

```c
// 場景：計算 10GB 大檔案中的最大值
// 原始想法：全部讀入陣列 — 不可行

// 優化方案：流式處理
int findMaxFromStream(FILE *file) {
    int max = INT_MIN;
    int value;

    // 逐個讀取，維護最大值
    while (fscanf(file, "%d", &value) == 1) {
        if (value > max) {
            max = value;
        }
    }

    return max;
}

// 應用到 LeetCode 問題
// 原始：在陣列中找第 k 大的數
// 改進：若陣列來自外部流呢？
typedef struct {
    int *heap;
    int heapSize;
    int heapCapacity;
    int k;
} KthLargestStream;

void heapPushMin(KthLargestStream *obj, int val);
int addToStream(KthLargestStream *obj, int val) {
    if (obj->heapSize < obj->k) {
        heapPushMin(obj, val);
    } else if (val > obj->heap[0]) {
        obj->heap[0] = val;
        // 重新堆積化
    }
    return obj->heap[0];
}
```

### 應對要點

1. **流式處理**：逐個讀取，持續更新答案
2. **維護必要資訊**：只保留需要的狀態
3. **時間複雜度**：通常 O(n) 時間，O(k) 或 O(log k) 空間

### 適用問題

| 問題 | 流式方案 | 空間 |
|------|--------|------|
| 計算平均值 | 累加和計數 | O(1) |
| 找 k 大 | 最小堆 | O(k) |
| 中位數 | 兩個堆（最大堆+最小堆） | O(1) |
| 眾數 | 雜湊表 | O(unique) |

---

## 模式 3: 能一次遍歷完成嗎？(Can you do it in one pass?)

### 什麼時候會問

- 你的解法需要多次掃描陣列
- 測試最優化能力

### 應對策略

```c
// 不優的方案：多次遍歷
int* productExceptSelf_TwoPass(int* nums, int numsSize, int* returnSize) {
    int *result = malloc(numsSize * sizeof(int));

    // 第一遍：計算左邊乘積
    for (int i = 0; i < numsSize; i++) {
        result[i] = (i == 0) ? 1 : result[i-1] * nums[i-1];
    }

    // 第二遍：計算右邊乘積
    int rightProduct = 1;
    for (int i = numsSize - 1; i >= 0; i--) {
        result[i] *= rightProduct;
        rightProduct *= nums[i];
    }

    *returnSize = numsSize;
    return result;
}

// 實際上，上述已經是兩遍優化版本（不需第三遍）
// 若要進一步優化到「真一遍」可能不可行，因為：
// - 左邊乘積需累積自左
// - 右邊乘積需累積自右
// 無法同時進行單遍掃描
```

### 適用場景

| 原始方案 | 遍歷次數 | 單遍可行性 | 注意 |
|----------|--------|----------|------|
| 反轉字串 | 2 | 否 | 需同時掃描兩端 |
| 移除元素 | 2 | 是 | 雙指標單遍 |
| 尋找最大 | 1 | 是 | 已最優 |
| 計算前綴和 | 1 | 是 | 邊計算邊輸出 |

---

## 模式 4: 如何處理重複元素？(How to handle duplicates?)

### 什麼時候會問

- 原始問題未明確處理重複
- 測試算法的魯棒性

### 應對策略

```c
// 原始：移除陣列中的重複元素（已排序）
int removeDuplicates(int* nums, int numsSize) {
    if (numsSize == 0) return 0;

    int i = 0;
    for (int j = 1; j < numsSize; j++) {
        if (nums[j] != nums[i]) {
            i++;
            nums[i] = nums[j];
        }
    }
    return i + 1;
}

// 後續問題 1：允許最多 k 個重複？
int removeDuplicates_AtMostK(int* nums, int numsSize, int k) {
    if (numsSize <= k) return numsSize;

    int i = 0;
    for (int j = 1; j < numsSize; j++) {
        // 若當前元素等於前面第 i-k 個元素，則是第 k+1 個重複，跳過
        if (i < k || nums[j] != nums[i - k]) {
            nums[i++] = nums[j];
        }
    }
    return i;
}

// 後續問題 2：未排序陣列中的重複？
// 使用雜湊表追蹤已見元素
typedef struct {
    int *table;  // 值 => 計數
    int size;
} HashTable;

int countDuplicates(int* nums, int numsSize) {
    int duplicateCount = 0;
    // 用雜湊表或排序
    qsort(nums, numsSize, sizeof(int), compareInt);

    for (int i = 1; i < numsSize; i++) {
        if (nums[i] == nums[i-1]) {
            duplicateCount++;
        }
    }
    return duplicateCount;
}
```

---

## 模式 5: 如何支援並發訪問？(Concurrent access / Thread-safe?)

### 什麼時候會問

- 從單線程升級到多線程
- 考察系統設計能力

### 應對策略

```c
// 原始：簡單的計數器
typedef struct {
    int count;
} Counter;

void increment(Counter *c) {
    c->count++;  // 非原子操作，線程不安全
}

// 改進 1：加鎖（Mutex）
#include <pthread.h>

typedef struct {
    int count;
    pthread_mutex_t lock;
} ThreadSafeCounter;

void increment_locked(ThreadSafeCounter *c) {
    pthread_mutex_lock(&c->lock);
    c->count++;
    pthread_mutex_unlock(&c->lock);
}

// 改進 2：原子操作（若環境支援）
#include <stdatomic.h>

typedef struct {
    _Atomic(int) count;
} AtomicCounter;

void increment_atomic(AtomicCounter *c) {
    atomic_fetch_add(&c->count, 1);
}

// 改進 3：無鎖數據結構（高效但複雜）
// 通常使用 CAS (Compare-And-Swap) 指令
```

### 考慮要點

1. **共享資源的保護**：使用鎖或原子操作
2. **避免死鎖**：鎖的順序、超時
3. **性能**：鎖的粒度（粗粒度vs細粒度）
4. **可見性**：記憶體屏障

---

## 模式 6: 能用分治 / 動態規劃 / 貪心優化嗎？

### 什麼時候會問

- 你的解法效率不夠
- 測試演算法思想

### 應對策略

```c
// 原始：O(n²) 暴力
int maxProfit_Brute(int* prices, int pricesSize) {
    int maxP = 0;
    for (int i = 0; i < pricesSize; i++) {
        for (int j = i + 1; j < pricesSize; j++) {
            int profit = prices[j] - prices[i];
            if (profit > maxP) maxP = profit;
        }
    }
    return maxP;
}

// 優化為 O(n) 貪心
int maxProfit_Greedy(int* prices, int pricesSize) {
    if (pricesSize < 2) return 0;

    int minPrice = prices[0];
    int maxP = 0;

    for (int i = 1; i < pricesSize; i++) {
        int profit = prices[i] - minPrice;
        if (profit > maxP) maxP = profit;
        if (prices[i] < minPrice) minPrice = prices[i];
    }
    return maxP;
}

// 為什麼貪心有效？
// 「總是從遇到的最低價買入」保證了最優解
// 因為任何更晚的最低價不會給出更好結果（如果它更低，我們已更新了最小值）
```

---

## 常見後續問題總結表

| 後續問題 | 典型回應 | 涉及問題 |
|---------|--------|--------|
| O(1) 空間？ | 雙指標、原地修改 | 陣列反轉、移除元素 |
| 太大無法放入記憶體？ | 流式處理、堆 | 找 k 大、計算中位數 |
| 一次遍歷？ | 仔細分析依賴性 | 前綴和、乘積 |
| 處理重複？ | 雙指標或雜湊表 | 移除重複、計數 |
| 並發訪問？ | 加鎖或原子操作 | 系統設計 |
| 更優演算法？ | DP / 貪心 / 分治 | 複雜最優化問題 |

---

## 面試話語範例

> "我目前的解法是 O(n²)。後續可以用雙指標優化到 O(n)，因為如果較短邊移動，寬度必然減少，只能期望更高的邊來補償。"

> "這裡我用了雜湊表，所以空間是 O(n)。如果要求 O(1) 空間，而陣列已排序的話，可以用雙指標代替。"

> "對於流式輸入，無法一次性載入，我會改用堆來維護 top-k，空間只需 O(k)。"

> "關於重複元素，我的雙指標方法能正確處理，因為比較邏輯是『當前元素不等於上一個』，自動忽略重複。"

---

## 面試準備建議

1. **事先準備常見變種**：為每個問題想好 2-3 個後續
2. **從簡到難思考**：時間 → 空間 → 應用場景
3. **主動提及權衡**：「這個解法的權衡是...」
4. **展示靈活思維**：證明你能從不同角度思考
5. **時間管理**：面試時留下時間討論後續

---

## 實現檢查清單

- [ ] 原始解法清晰、正確
- [ ] 複雜度分析完整
- [ ] 主動提出至少一個改進方向
- [ ] 對常見後續問題有準備
- [ ] 能清楚解釋「為什麼這樣優化」
- [ ] 權衡利弊（時間vs空間）
- [ ] 考慮邊界情況和輸入限制
