# Code Review Questions 代碼審查視角

## 核心概念

面試的最後階段，面試官可能從**資深工程師的角度**審視你的代碼，不只問演算法正確性，還要考慮生產級別的品質。

**審查角度：**
1. 記憶體安全
2. 錯誤處理
3. 性能和可擴展性
4. 代碼可讀性
5. 生產考慮（可維護性、可測試性）

---

## 1. 記憶體安全 (Memory Safety)

### 問題 1.1: 檢查記憶體洩漏

```c
// 被審查的代碼
int* solution(int* nums, int numsSize, int* returnSize) {
    int *result = malloc(numsSize * sizeof(int));  // ✓ 分配
    int *temp = malloc(numsSize * sizeof(int));    // ✓ 分配

    // ... 處理 ...

    free(result);  // ✗ temp 未釋放 — 記憶體洩漏！

    *returnSize = numsSize;
    return result;
}

// 審查意見：
// Q: 為什麼沒有釋放 temp？
// A: 我應該在返回前 free(temp)。

// 改進版本
int* solution(int* nums, int numsSize, int* returnSize) {
    int *result = malloc(numsSize * sizeof(int));
    int *temp = malloc(numsSize * sizeof(int));

    // ... 處理 ...

    free(temp);
    *returnSize = numsSize;
    return result;
}
```

### 問題 1.2: 檢查 NULL 檢查

```c
// 被審查的代碼
char* reverseString(char* s) {
    int len = strlen(s);  // ✗ s 可能是 NULL
    char *result = malloc(len + 1);

    // ... 反轉 ...

    return result;
}

// 改進版本
char* reverseString(char* s) {
    if (s == NULL) {
        return NULL;  // 或 malloc(1) 返回空字串
    }

    int len = strlen(s);
    char *result = malloc(len + 1);
    if (result == NULL) {  // 檢查分配失敗
        return NULL;
    }

    // ... 反轉 ...

    return result;
}
```

### 問題 1.3: 緩衝區溢出

```c
// 被審查的代碼
void copyString(char *dest, char *src, int destSize) {
    strcpy(dest, src);  // ✗ 未檢查大小，可能溢出
}

// 改進版本
void copyString(char *dest, char *src, int destSize) {
    if (src == NULL || dest == NULL) return;

    // 確保有空間放置終止符
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}
```

### 問題 1.4: 雙重釋放（Double Free）

```c
// 被審查的代碼
void cleanup(int *ptr) {
    free(ptr);
    free(ptr);  // ✗ 雙重釋放！未定義行為
}

// 改進版本
void cleanup(int *ptr) {
    if (ptr != NULL) {
        free(ptr);
        // 或者從呼叫者設為 NULL
    }
}

// 最佳實踐：使用指標的指標
void cleanup(int **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;  // 呼叫者的指標被設為 NULL
    }
}
```

---

## 2. 錯誤處理 (Error Handling)

### 問題 2.1: 檢查邊界條件

```c
// 被審查的代碼
int findMax(int* nums, int numsSize) {
    if (numsSize == 0) {
        // ✗ 未定義返回值，調用者困惑
    }

    int max = nums[0];
    // ...
    return max;
}

// 改進版本
int findMax(int* nums, int numsSize) {
    if (nums == NULL || numsSize <= 0) {
        return INT_MIN;  // 或使用輸出參數表示錯誤
    }

    int max = nums[0];
    for (int i = 1; i < numsSize; i++) {
        if (nums[i] > max) max = nums[i];
    }
    return max;
}

// 或使用錯誤碼
int findMax(int* nums, int numsSize, int* result) {
    if (nums == NULL || numsSize <= 0) {
        return -1;  // 錯誤碼
    }

    *result = nums[0];
    for (int i = 1; i < numsSize; i++) {
        if (nums[i] > *result) *result = nums[i];
    }
    return 0;  // 成功
}
```

### 問題 2.2: 處理溢出

```c
// 被審查的代碼
int compareInt(const void *a, const void *b) {
    return *(int *)a - *(int *)b;  // ✗ INT_MAX - INT_MIN 溢出
}

// 改進版本
int compareInt(const void *a, const void *b) {
    int x = *(int *)a;
    int y = *(int *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}
```

### 問題 2.3: 檢查分配失敗

```c
// 被審查的代碼
int* allocate(int size) {
    return malloc(size * sizeof(int));  // ✗ 未檢查 NULL
}

// 改進版本
int* allocate(int size) {
    if (size <= 0) return NULL;

    int *ptr = malloc(size * sizeof(int));
    if (ptr == NULL) {
        // 日誌記錄、返回錯誤代碼等
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    return ptr;
}
```

---

## 3. 性能和可擴展性 (Performance)

### 問題 3.1: 不必要的複製

```c
// 被審查的代碼：低效
char* processString(char* s) {
    char *temp = malloc(strlen(s) + 1);
    strcpy(temp, s);  // ✗ 不必要的複製

    char *result = malloc(strlen(temp) + 1);
    strcpy(result, temp);
    free(temp);

    return result;
}

// 改進版本
char* processString(char* s) {
    if (s == NULL) return NULL;

    // 直接分配 + 複製一次
    char *result = malloc(strlen(s) + 1);
    if (result == NULL) return NULL;

    strcpy(result, s);
    return result;
}
```

### 問題 3.2: 重複計算

```c
// 被審查的代碼
int sumDigits(int n) {
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n = n / 10;
    }
    return sum;
}

int checkAllNumbers(int* nums, int numsSize) {
    for (int i = 0; i < numsSize; i++) {
        int sum = sumDigits(nums[i]);  // ✗ 可快取結果
        // ...
    }
}

// 改進版本：若需頻繁計算，考慮備忘錄
#define CACHE_SIZE 1000
static int digitSumCache[CACHE_SIZE] = {0};

int sumDigits_Cached(int n) {
    if (n < CACHE_SIZE && digitSumCache[n] != 0) {
        return digitSumCache[n];
    }

    int sum = 0;
    int temp = n;
    while (temp > 0) {
        sum += temp % 10;
        temp = temp / 10;
    }

    if (n < CACHE_SIZE) {
        digitSumCache[n] = sum;
    }

    return sum;
}
```

### 問題 3.3: 演算法選擇

```c
// 被審查的代碼：O(n²)
int hasDuplicate(int* nums, int numsSize) {
    for (int i = 0; i < numsSize; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if (nums[i] == nums[j]) return 1;
        }
    }
    return 0;
}

// 改進版本：O(n) 雜湊表（或排序 O(n log n)）
// 使用簡單雜湊集（假設範圍有限）
int hasDuplicate_Fast(int* nums, int numsSize) {
    if (numsSize > 100000) return 0;  // 假設範圍限制

    bool seen[100000] = {0};

    for (int i = 0; i < numsSize; i++) {
        if (nums[i] >= 0 && nums[i] < 100000) {
            if (seen[nums[i]]) return 1;
            seen[nums[i]] = true;
        }
    }
    return 0;
}
```

---

## 4. 可讀性 (Readability)

### 問題 4.1: 變數命名

```c
// 被審查的代碼：不清楚
int f(int* a, int b) {
    int c = 0;
    for (int i = 0; i < b; i++) {
        c += a[i];
    }
    return c;
}

// 改進版本
int arraySum(int* arr, int arrSize) {
    int sum = 0;
    for (int i = 0; i < arrSize; i++) {
        sum += arr[i];
    }
    return sum;
}
```

### 問題 4.2: 過度複雜的邏輯

```c
// 被審查的代碼：一行太多邏輯
return (ptr && ptr->next && ptr->next->val > 5) ? process(ptr->next) : NULL;

// 改進版本：拆分為多步
if (ptr == NULL || ptr->next == NULL) {
    return NULL;
}

if (ptr->next->val > 5) {
    return process(ptr->next);
}

return NULL;
```

### 問題 4.3: 缺乏註釋

```c
// 被審查的代碼：沒有解釋為什麼
int result = (a & (a - 1)) == 0;

// 改進版本
// 檢查 a 是否是 2 的冪次：若是，a & (a-1) == 0
int isPowerOfTwo = (a > 0) && ((a & (a - 1)) == 0);
```

---

## 5. 生產考慮 (Production Readiness)

### 問題 5.1: 防禦性編程

```c
// 被審查的代碼：假設輸入總是合法
void divide(int a, int b) {
    printf("%d\n", a / b);  // ✗ 若 b == 0 則崩潰
}

// 改進版本
int divide(int a, int b, int* result) {
    if (b == 0) {
        return -1;  // 或返回錯誤代碼
    }

    *result = a / b;
    return 0;
}
```

### 問題 5.2: 可測試性

```c
// 被審查的代碼：與 I/O 強耦合，難以單元測試
void processUserInput(void) {
    char buffer[100];
    scanf("%s", buffer);  // ✗ 從 stdin 讀取，不可單元測試
    printf("%s\n", process(buffer));
}

// 改進版本：分離邏輯和 I/O
char* process(const char* input) {
    // 純邏輯，易於測試
    // ...
}

void processUserInput(void) {
    char buffer[100];
    if (scanf("%s", buffer) == 1) {
        char* result = process(buffer);
        printf("%s\n", result);
        free(result);
    }
}
```

### 問題 5.3: 常數和魔數

```c
// 被審查的代碼
if (size > 1024) {  // ✗ 魔數，沒有解釋

// 改進版本
#define MAX_BUFFER_SIZE 1024

if (size > MAX_BUFFER_SIZE) {
    // 或用枚舉、結構體字段等
}
```

---

## 常見審查項目檢查清單

| 類別 | 檢查項 | 是否通過 |
|------|--------|--------|
| 記憶體 | 檢查所有 malloc 的 NULL | ☐ |
| 記憶體 | 每個 malloc 都有 free | ☐ |
| 記憶體 | 無緩衝區溢出 | ☐ |
| 記憶體 | 無雙重釋放 | ☐ |
| 錯誤 | 邊界條件處理 | ☐ |
| 錯誤 | 整數溢出檢查 | ☐ |
| 錯誤 | 分配失敗處理 | ☐ |
| 性能 | 複雜度合理 | ☐ |
| 性能 | 無不必要複製 | ☐ |
| 性能 | 無重複計算 | ☐ |
| 可讀性 | 變數命名清晰 | ☐ |
| 可讀性 | 邏輯易於理解 | ☐ |
| 可讀性 | 關鍵部分有註釋 | ☐ |
| 生產 | 防禦性邊界檢查 | ☐ |
| 生產 | 易於單元測試 | ☐ |
| 生產 | 無魔數 | ☐ |

---

## 面試話語範例

> "我意識到沒有檢查 malloc 的返回值。應該加上 if (ptr == NULL) 的檢查。"

> "這裡有個潛在的整數溢出，若 a 和 b 都是 INT_MAX。我應該改為比較而非相減。"

> "我的原始解法是 O(n²)，可以優化到 O(n)，但需要額外 O(n) 空間用於雜湊表。面試中，O(n log n) 排序解決方案也可以接受。"

> "關於可讀性，我應該將比較邏輯提取為獨立函數，而不是在迴圈中內聯一個複雜表達式。"

> "考慮生產環境，我需要添加更多的錯誤檢查和日誌記錄，即使題目沒有明確要求。"

---

## 面試準備建議

1. **自我審視你的代碼**
   - 閱讀代碼就像讀者一樣
   - 問自己：「有什麼可以改進的？」

2. **預留時間進行代碼審查**
   - 面試最後幾分鐘回頭檢查
   - 主動說出發現的問題

3. **多角度思考**
   - 功能正確性（首要）
   - 記憶體安全（重要）
   - 性能（次要，除非明確要求）
   - 可讀性（長期價值）

4. **展示成長心態**
   - 承認潛在缺陷
   - 解釋改進方案
   - 說明為什麼新方案更好

---

## 實現檢查清單

- [ ] 所有 malloc 都檢查 NULL
- [ ] 每個 malloc 都有配對的 free
- [ ] 無緩衝區溢出、無雙重釋放
- [ ] 邊界條件正確處理
- [ ] 整數溢出預防
- [ ] 複雜度符合要求
- [ ] 變數命名清晰有意義
- [ ] 複雜邏輯有註釋
- [ ] 防禦性檢查（NULL、範圍）
- [ ] 無魔數（使用 #define 或常數）
