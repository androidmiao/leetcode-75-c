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

---

# Linux Kernel 視角擴充（Google / 系統職缺加分）

> 上半部是**一般 C 工程師角度**的 code review；下半部是**Linux kernel 維護者／資深系統工程師**角度。
>
> 為何要分開？Google、Meta、Red Hat、AWS Kernel、Android、Qualcomm BSP 這類職缺的 code review 面試，真正的 bar 是：**「這段 C 代碼能不能直接貼進 kernel 而不會被 Linus 罵？」**
>
> 本節的每一題都附上在 Linux 主線 (mainline) 或 Android common kernel 中可以直接找到的實際例子，讓你能把 kernel 原始碼當作教材，而不是憑空想像最佳實踐。

## 6. Kernel 風格的基礎紀律

### 問題 6.1: Kernel Coding Style

面試官可能會丟一段代碼，明知縮排 / 括號 / 行寬都不符合 `Documentation/process/coding-style.rst`。

```c
/* ✗ 這段在 kernel 裡會被立刻駁回 */
int foo(int *arr, int n) {
    if(n==0){return -1;}
    for(int i=0;i<n;i++) arr[i]=arr[i]*2;
    return 0;
}

/* ✓ kernel 風格 */
int foo(int *arr, int n)
{
	int i;

	if (n == 0)
		return -1;

	for (i = 0; i < n; i++)
		arr[i] *= 2;

	return 0;
}
```

Kernel 口訣（要能背）：

- Tab = 8 字元、行寬 100（舊規範 80）。
- 函式開花括號另起一行；其他（`if`、`for`、`while`）同行。
- 單句 `if` **不加**花括號，除非另一分支有大括號。
- C99 混合宣告允許，但風格上仍把變數放函式起始。
- 運算子兩側空格；逗號後空格；`*` 緊貼變數名（`int *p` 而非 `int* p`）。

> 面試話語：「我注意到這段不符合 kernel coding style — 如果是送 upstream 的 patch，`scripts/checkpatch.pl` 會先把它擋下來。」

---

### 問題 6.2: ERR_PTR / PTR_ERR / IS_ERR — 錯誤的第三狀態

```c
/* ✗ LeetCode 風格：只有「成功指標」或「NULL」 */
struct node *lookup(int key)
{
	struct node *n = search(key);
	if (!n)
		return NULL;   /* 找不到？還是 ENOMEM？無法區分 */
	return n;
}

/* ✓ Kernel 慣例：把 errno 編碼進指標 */
struct node *lookup(int key)
{
	struct node *n = search(key);
	if (!n)
		return ERR_PTR(-ENOENT);
	if (!node_valid(n))
		return ERR_PTR(-EINVAL);
	return n;
}

/* 呼叫端 */
n = lookup(k);
if (IS_ERR(n))
	return PTR_ERR(n);   /* 把 -ENOENT 等負 errno 往上傳 */
```

考點：

- `ERR_PTR(-errno)` 利用最高 4K 虛擬位址永遠不會是合法指標，把錯誤碼塞進指標本身。
- 為什麼不用 `errno` 全域變數？**kernel 沒有 per-thread `errno`**，`errno` 是 userspace libc 機制。
- 為什麼不把錯誤碼用輸出參數回傳？kernel API 慣例是一個回傳值就搞定，介面較簡潔且 caller 容易寫 `goto`。

> 面試加分：「LeetCode 題目回傳 `NULL` 等價於 `-ENOENT`；如果這是 kernel helper，我會改用 `ERR_PTR`，讓呼叫端可以區分『找不到』與『記憶體不足』。」

---

### 問題 6.3: goto cleanup — 集中式錯誤路徑

```c
/* ✗ 常見 LeetCode 寫法：每個錯誤點重複 free */
int init(struct foo **out)
{
	struct foo *f = malloc(sizeof(*f));
	if (!f)
		return -1;
	f->buf = malloc(1024);
	if (!f->buf) {
		free(f);
		return -1;
	}
	f->tbl = malloc(4096);
	if (!f->tbl) {
		free(f->buf);
		free(f);
		return -1;
	}
	*out = f;
	return 0;
}

/* ✓ Kernel 慣例：goto 標籤 = 反向拆卸 */
int init(struct foo **out)
{
	struct foo *f;
	int ret;

	f = kzalloc(sizeof(*f), GFP_KERNEL);
	if (!f)
		return -ENOMEM;

	f->buf = kmalloc(1024, GFP_KERNEL);
	if (!f->buf) {
		ret = -ENOMEM;
		goto err_free_f;
	}

	f->tbl = kmalloc(4096, GFP_KERNEL);
	if (!f->tbl) {
		ret = -ENOMEM;
		goto err_free_buf;
	}

	*out = f;
	return 0;

err_free_buf:
	kfree(f->buf);
err_free_f:
	kfree(f);
	return ret;
}
```

為什麼這不是「可怕的 goto」：

- 只向前跳（never backward）— 沒有迴圈或控制流混亂。
- 標籤名稱說明「要 undo 哪一步」— `err_free_buf:` 表示「釋放 buf 及之後已分配的資源」。
- 比 `if / else` 巢狀 3-5 層大大可讀。Linus 本人在 2003 年 LKML 上替 goto 辯護過這個用法。

> 面試話語：「LeetCode 寫法在 2 個資源還可以，但 VMA 或 net_device 這種 10+ 資源的初始化，若不用 goto cleanup 會變成金字塔右斜塔。」

---

### 問題 6.4: 整數溢出 — kcalloc 與 overflow helpers

```c
/* ✗ 隱藏的乘法溢出 */
arr = kmalloc(n * sizeof(struct item), GFP_KERNEL);
/* 若 n 是 user-controlled，n * sizeof(item) 可能折返 */

/* ✓ kcalloc：內建 overflow check */
arr = kcalloc(n, sizeof(struct item), GFP_KERNEL);
/* 或 */
arr = kmalloc_array(n, sizeof(struct item), GFP_KERNEL);

/* ✓ 顯式 overflow check */
size_t total;
if (check_mul_overflow(n, sizeof(struct item), &total))
	return -EOVERFLOW;
arr = kmalloc(total, GFP_KERNEL);
```

考點延伸：

- `size_add()`, `size_mul()`, `check_add_overflow()`, `check_mul_overflow()` 定義於 `include/linux/overflow.h`，編譯器 builtin（`__builtin_add_overflow`）。
- CVE 案例：早期 `ext4`、`drm`、`io_uring` 都曾因使用者可控的整數乘法出過 root 級漏洞。
- `array_size(a, b)`、`struct_size(p, member, n)` 是 flex-array-safe 的 helper，flex array 場景首選。

> 面試加分：「面試題的 `new int[n*m]` 在 kernel 裡等價於 `kmalloc_array()`，**從來不寫裸 `kmalloc(n * m * sizeof...)`**，這是防 CVE 的基本紀律。」

---

### 問題 6.5: GFP 旗標 — 分配上下文的隱形合約

```c
/* ✗ 在中斷處理常式裡呼叫可睡眠的分配 */
irqreturn_t my_isr(int irq, void *dev)
{
	struct pkt *p = kmalloc(sizeof(*p), GFP_KERNEL);   /* BUG：GFP_KERNEL 可睡 */
	...
}

/* ✓ 中斷 / atomic context 用 GFP_ATOMIC */
irqreturn_t my_isr(int irq, void *dev)
{
	struct pkt *p = kmalloc(sizeof(*p), GFP_ATOMIC);
	...
}
```

GFP 旗標速查：

| 旗標 | 使用場景 | 可睡嗎 |
|------|----------|--------|
| `GFP_KERNEL` | 預設。進程上下文、可 reclaim、可 I/O | 可 |
| `GFP_ATOMIC` | 中斷、spinlock 內、preempt-disabled | 不可 |
| `GFP_NOWAIT` | 低開銷 retry，失敗時直接返回 | 不可 |
| `GFP_NOFS` | 檔案系統內部避免遞迴 reclaim | 可（無 FS 遞迴） |
| `GFP_USER` | 供 userspace 使用的頁面（可 mount 至 user VMA） | 可 |
| `__GFP_ZERO` | 補旗標：配完後 memset 0（= `kzalloc`） | — |

> 面試話語：「我會先問：這個函式的呼叫上下文是什麼？能睡嗎？這決定 GFP 旗標選擇，也決定能不能 `mutex_lock` 還是只能 `spin_lock`。」

---

## 7. 侵入式資料結構與 container_of

### 問題 7.1: LeetCode 風格 vs kernel 風格的鏈表

```c
/* ✗ LeetCode 風格的 struct — 在 kernel 裡很少這樣寫 */
struct task {
	int pid;
	char name[16];
	struct task *next;
};

/* ✓ Kernel 風格：侵入式，一個 task 可在多條鏈上 */
struct task {
	int pid;
	char name[16];
	struct list_head run_list;        /* CPU run queue */
	struct list_head sibling;         /* 兄弟進程 */
	struct list_head children;        /* 子進程列表 */
	struct hlist_node pid_hash;       /* pid → task 雜湊 */
};

/* 從 list_head 反查回 task 用 container_of */
struct task *t = container_of(pos, struct task, run_list);
```

審查角度問題：

1. *為什麼 kernel 選侵入式？* — 節省記憶體、一個物件可同時在多條鏈上、快取友善（鏈結與資料相鄰）。
2. *`container_of` 有何性能成本？* — **零**。編譯時算出 offset，純位址減法。
3. *若 member 指標是 NULL，`container_of` 會壞掉嗎？* — 會回傳 `(type *)(- offset)`，即小得不像樣的位址；kernel 專門有 `list_entry_or_null()` 處理。

---

### 問題 7.2: 陣列大小巨集

```c
/* ✗ 常見但脆弱 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
/* 若 a 是指標（參數），這會回傳 1 或 2，不是元素數 */

/* ✓ Kernel 的 ARRAY_SIZE (include/linux/kernel.h) */
#define ARRAY_SIZE(arr) \
	(sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#define __must_be_array(a) \
	BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
/* 若 a 是指標，__same_type 成立，BUILD_BUG_ON_ZERO 觸發編譯錯誤 */
```

> 面試加分：「這個巨集看得多，但只有在 kernel 我才意識到它**必須阻擋指標誤用** — 用 `__builtin_types_compatible_p` 在編譯時攔下錯誤。」

---

## 8. 並發與鎖定紀律

### 問題 8.1: Spinlock 持有時能做什麼？

```c
/* ✗ Spinlock 內呼叫可睡眠函式 — kernel 最常見的 oops */
spin_lock(&my_lock);
kmalloc(sz, GFP_KERNEL);   /* GFP_KERNEL 可睡 → sleeping while atomic */
mutex_lock(&other);        /* mutex 可睡 → 同上 */
copy_to_user(...);         /* 可 page fault → 可睡 */
spin_unlock(&my_lock);

/* ✓ 正確做法 */
spin_lock(&my_lock);
/* 臨界區：只做快速、非睡眠操作 */
spin_unlock(&my_lock);

buf = kmalloc(sz, GFP_KERNEL);    /* 離開臨界區再分配 */
```

三層檢查：

1. **上下文**：函式可能從哪呼叫？中斷、softirq、還是 process context？
2. **能睡嗎**：若答案有可能是「中斷」，就不能 `mutex_lock` 也不能 `GFP_KERNEL`。
3. **Lockdep 會告狀嗎**：`CONFIG_LOCKDEP=y` 會在執行時偵測 spinlock 內的可睡眠操作 — 所以 kernel 自測期間不會漏掉。

---

### 問題 8.2: 鎖順序 — AB-BA 死鎖

```c
/* ✗ 兩個 code path 鎖定順序相反 */
thread A: spin_lock(&X);  spin_lock(&Y);  ...  spin_unlock(&Y);  spin_unlock(&X);
thread B: spin_lock(&Y);  spin_lock(&X);  ...  spin_unlock(&X);  spin_unlock(&Y);
/* A 持 X 等 Y，B 持 Y 等 X → 互等 → 死鎖 */

/* ✓ 全系統約定同一鎖順序 */
/* 例：Linux VM 訂「always mmap_lock → i_mmap_lock → page_table_lock」 */
```

考點：

- Lockdep 的核心使命就是偵測這個（`kernel/locking/lockdep.c`），BFS 遍歷鎖依賴圖。
- `spin_lock_nested()` / `mutex_lock_nested()` 給 lockdep 提示：雖然同類別的鎖，但持有順序有意義。

> 面試話語：「這段如果要審，我會問：這兩個鎖還有沒有其他地方一起被持有？若有，兩處順序必須一致 — Lockdep 平時只在這兩個順序都走過的 code path 被觸發時才會抓到。」

---

### 問題 8.3: RCU 與 refcount

```c
/* ✗ Reader 取指標，writer 同時 free — use-after-free */
rcu_read_lock();
p = shared_ptr;
...  /* 此時其他 CPU free(p) 就會炸 */
rcu_read_unlock();

/* ✓ RCU pattern */
rcu_read_lock();
p = rcu_dereference(shared_ptr);
use(p);
rcu_read_unlock();

/* writer */
old = shared_ptr;
rcu_assign_pointer(shared_ptr, new);
synchronize_rcu();        /* 等所有 reader 離開 grace period */
kfree(old);

/* refcount 保護延伸使用 */
rcu_read_lock();
p = rcu_dereference(shared_ptr);
if (refcount_inc_not_zero(&p->ref)) {
	rcu_read_unlock();
	/* 現在可以睡、可以 mutex_lock */
	use(p);
	if (refcount_dec_and_test(&p->ref))
		kfree(p);
} else {
	rcu_read_unlock();   /* 已被刪除中 */
}
```

考點：

- `refcount_t` vs `atomic_t`：`refcount_t` 在 overflow 或 `0→inc` 時會 WARN，是專門給引用計數用的 saturating counter（`include/linux/refcount.h`）。
- 為什麼不是單純用 reader/writer 鎖？因為 RCU reader side **零成本**（無 atomic、無 cache line bounce），適合讀極多寫極少場景（dentry cache、路由表、namespace list）。

---

## 9. Kernel 維護性專用信號

### 問題 9.1: likely / unlikely

```c
/* ✓ 熱路徑提示 */
if (unlikely(!buf))
	return -ENOMEM;

if (likely(entry->valid))
	fast_path();
else
	slow_path();
```

重點：

- `likely()` / `unlikely()` 展開為 `__builtin_expect`；影響 branch predictor 佈局，在極熱路徑能差個幾 % 吞吐量。
- 濫用反而傷 icache — 只在真正 99%+ 走某分支時用。

### 問題 9.2: __must_check 與其他 annotation

```c
__must_check int get_user(T *x, const T __user *ptr);    /* 忽略回傳值會 warning */
__always_inline static void fast(void);                  /* 強制 inline */
__no_kcsan void race_tolerated_counter(void);            /* KCSAN 免檢 */
__read_mostly static struct foo config;                  /* 放進 .data..read_mostly section，避免 false sharing */
static DEFINE_PER_CPU_ALIGNED(struct q, queues);         /* cacheline 對齊避 false sharing */
```

### 問題 9.3: Cache line / false sharing

```c
/* ✗ 兩個熱變數擠在同一 cache line */
struct stats {
	u64 packets_in;    /* CPU 0 每秒寫千萬次 */
	u64 packets_out;   /* CPU 1 每秒寫千萬次 */
};
/* → CPU 0 / CPU 1 持續把 cache line 互彈 (false sharing) */

/* ✓ 拆開或對齊 */
struct stats {
	u64 packets_in ____cacheline_aligned_in_smp;
	u64 packets_out ____cacheline_aligned_in_smp;
};

/* 更好：per-CPU 計數，讀時才聚合 */
DEFINE_PER_CPU(u64, packets_in);
```

> 面試話語：「這段在 LeetCode 上不重要，但如果送到 kernel 熱路徑（packet RX、syscall fast path），光是 false sharing 就能讓吞吐量掉 30%。」

---

## 10. 自動化工具：主管會先問你用了什麼

Kernel patch 送 upstream 前，**標配**通過以下工具：

| 工具 | 抓什麼 | 開關 |
|------|--------|------|
| `scripts/checkpatch.pl` | Coding style、常見反模式 | 本地執行 |
| **KASAN** (Kernel Address Sanitizer) | Use-after-free、heap/stack OOB | `CONFIG_KASAN=y` |
| **KMSAN** (Memory Sanitizer) | 使用未初始化記憶體 | `CONFIG_KMSAN=y` |
| **KCSAN** (Concurrency Sanitizer) | Data race（未同步讀寫） | `CONFIG_KCSAN=y` |
| **KFENCE** (Kernel Electric Fence) | 低開銷的 UaF / OOB 取樣偵測 | `CONFIG_KFENCE=y` |
| **lockdep** | AB-BA、spinlock 內 sleep、錯誤 IRQ context | `CONFIG_LOCKDEP=y` |
| **sparse** | Endian / address_space 錯用、typedef 濫用 | `make C=2` |
| **smatch / coccinelle** | 靜態分析、自動 refactor | 第三方 |
| **syzkaller** | Fuzzing，找非預期 syscall 組合觸發的崩潰 | syzbot 自動化 |

> 面試加分：「若這段代碼要進 kernel，我會先跑 `./scripts/checkpatch.pl --strict`，再用 `make C=2 W=1` 讓 sparse 跑，接著 `CONFIG_KASAN=y` 做 boot test；所有 upstream patch 的標配是『syzbot 不會掛掉就 OK』。」

---

## 11. Kernel 風格 review 一句話範例

收錄可以直接說出口的話術，分情境：

**記憶體 / 錯誤**

- 「這個 `kmalloc` 可能在 atomic context 被呼叫，要考慮 `GFP_ATOMIC` 或 `GFP_NOWAIT`。」
- 「乘法直接塞進 `kmalloc` 有 CVE 風險，換 `kmalloc_array()` 或 `check_mul_overflow()`。」
- 「`NULL` 在這裡無法區分『沒找到』與『OOM』，我會改回 `ERR_PTR(-ENOENT)` / `ERR_PTR(-ENOMEM)`。」
- 「錯誤路徑有 3 處資源需要 undo，金字塔 `if` 可讀性差，改 `goto err_xxx` 標籤會更像 kernel 習慣寫法。」

**並發**

- 「持有 `spin_lock` 期間不能 `mutex_lock`、不能 `kmalloc(GFP_KERNEL)`、不能 `copy_to_user`。」
- 「這個 reader 看起來很頻繁，可以考慮 RCU — writer 吃 `synchronize_rcu()` 換 reader side 零成本。」
- 「Lockdep 會抱怨這兩個鎖順序不一致 — 需要訂全系統的鎖序文件。」
- 「共用計數器建議 per-CPU，否則多核上 cache line bounce 會吞掉吞吐量。」

**維護性**

- 「這行 80% 走 `err` 路徑，加 `unlikely()` 讓 branch predictor 佈局快路徑。」
- 「回傳值被忽略會導致資源洩漏，函式簽名應加 `__must_check`。」
- 「這兩個熱變數同 cache line，加 `____cacheline_aligned_in_smp` 或拆檔。」
- 「`offsetof` + 手算位移錯一次就 oops，改用 `container_of` 讓編譯器算。」

---

## 12. Kernel 版實現檢查清單

基礎清單之外，kernel 層級必檢：

- [ ] 函式是否明確標示「可睡 / 不可睡」上下文？（code comment / `might_sleep()`）
- [ ] 所有記憶體分配是否用正確的 GFP 旗標？
- [ ] 錯誤路徑是否用 `goto cleanup` 統一管理？
- [ ] 回傳是否區分「找不到」與「失敗」（`ERR_PTR` 或 `-ENOENT` vs `-ENOMEM`）？
- [ ] 整數相乘是否配對 `kmalloc_array` / `kcalloc` / `check_mul_overflow`？
- [ ] 鎖持有期間只做不睡的操作？
- [ ] 與同資源的其他鎖是否訂定一致順序？
- [ ] Reader 極多寫極少時是否考慮 RCU / seqlock？
- [ ] 熱變數是否 `____cacheline_aligned_in_smp` 或改 per-CPU？
- [ ] 回傳值重要的函式簽名是否有 `__must_check`？
- [ ] 用了 `container_of` / `list_for_each_entry` 而非裸指標運算？
- [ ] 註解是否寫下 invariant、locking rule、caller 需持有的鎖？
- [ ] `./scripts/checkpatch.pl --strict` 過？
- [ ] KASAN / KMSAN / lockdep / syzkaller boot 過？

---

## 13. 參考資源

- `Documentation/process/coding-style.rst` — kernel coding style 官方文件
- `Documentation/process/submitting-patches.rst` — patch 送 upstream 的完整流程
- `Documentation/dev-tools/` — KASAN、KMSAN、KCSAN、KFENCE、lockdep 的使用說明
- `include/linux/err.h` — `ERR_PTR` / `PTR_ERR` / `IS_ERR` 的定義
- `include/linux/overflow.h` — `check_*_overflow` / `size_add` / `struct_size`
- `include/linux/gfp.h` — GFP 旗標完整定義
- `include/linux/list.h`, `include/linux/rculist.h` — 侵入式鏈表、RCU 保護版本
- `include/linux/refcount.h` — saturating 引用計數
- Bootlin: <https://bootlin.com/doc/training/linux-kernel/> — 社群訓練教材
- LWN.net: <https://lwn.net/Kernel/Index/> — kernel 新特性追蹤

---

# Firmware 子系統視角：ARM SCMI 與 Google ACPM 專題擴充

> 2026-04-20 新增。Google / Qualcomm / Samsung BSP / Android kernel 職缺的 on-site 經常挑一段 `drivers/firmware/` 的代碼做 review — 這邊的重點已經不是「記憶體 / 鎖 / GFP」基本功，而是「你是否理解一個 SoC firmware driver 的 life cycle 與跨層合約」。
>
> 本節對應 `/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/analyses/scmi-vs-google-acpm.md` 的三方對照分析（ARM SCMI vs Google ACPM vs Qualcomm RPMh）。建議搭配閱讀。

## 14. ARM SCMI 視角（`drivers/firmware/arm_scmi/`）

ARM SCMI（System Control & Management Interface）是跨 SoC 標準化的 AP↔SCP 韌體協定，10 個 protocol（Base 0x10 / Power 0x11 / System 0x12 / Perf 0x13 / Clock 0x14 / Sensor 0x15 / Reset 0x16 / Voltage 0x17 / Powercap 0x18 / Pinctrl 0x19）、4 種 transport（mailbox / SMC / VirtIO / optee）。Kernel 實作 31 檔 19,481 行，是**韌體子系統裡最複雜、最值得學的 code base**。

### 問題 14.1: Vendor protocol 註冊與生命週期

```c
/* 被審查的代碼：vendor 自定 SCMI protocol 驅動 */
static int my_vendor_probe(struct scmi_device *sdev)
{
	const struct scmi_handle *handle = sdev->handle;
	struct scmi_protocol_handle *ph;
	struct my_priv *priv;

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);   /* ✗ 未用 devm */
	if (!priv)
		return -ENOMEM;

	ph = handle->protocol_get(sdev, MY_PROTO_ID, &priv->ops);
	if (!ph)
		return -ENODEV;   /* ✗ 無法區分 ENOMEM / ENOENT */

	priv->ph = ph;
	sdev->priv = priv;   /* ✗ struct scmi_device 無 priv 欄位 */
	return 0;
}
```

Kernel 風格的 SCMI vendor driver 該長的樣子：

```c
/* 對照 drivers/firmware/arm_scmi/clock.c, perf.c */
static int my_vendor_probe(struct scmi_device *sdev)
{
	const struct scmi_handle *handle = sdev->handle;
	struct scmi_protocol_handle *ph;
	struct my_priv *priv;

	priv = devm_kzalloc(&sdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	ph = devm_scmi_protocol_acquire(&sdev->dev, handle,
					MY_PROTO_ID, &priv->ops);
	if (IS_ERR(ph))
		return PTR_ERR(ph);    /* ✓ 保留 -ENOMEM / -ENOENT / -EPROBE_DEFER */

	priv->ph = ph;
	dev_set_drvdata(&sdev->dev, priv);
	return 0;
}
```

審查要點：

1. **`devm_` 包裝** — SCMI driver 幾乎不用裸 `kmalloc` + manual remove path；`devm_kzalloc` + `devm_scmi_protocol_acquire` 把 release 交給 device model，避免 probe 半途失敗的 leak。
2. **`protocol_get` vs `protocol_acquire`** — 前者是舊 API，只回傳指標或 NULL；後者回 `ERR_PTR`，可往上傳 `-EPROBE_DEFER`（等 transport 就緒）。mainline 已統一使用後者。
3. **`scmi_protocol_handle` 是 protocol-scoped**，不要跨 protocol 重用。每個 protocol 各自有 xfer quota、event callbacks。

> 面試話語：「SCMI 的 probe 要靠 `devm_scmi_protocol_acquire` 拿 ph — 它會自動 `protocol_put` 對應 refcount；手動路徑容易漏 put 造成 protocol 無法被 unload。」

### 問題 14.2: Mailbox TX — GFP 與上下文合約

```c
/* 被審查的代碼：protocol 驅動送命令 */
int my_get_value(const struct scmi_protocol_handle *ph, u32 *val)
{
	struct scmi_xfer *t;
	int ret;

	ret = ph->xops->xfer_get_init(ph, MY_CMD_GET, 0, sizeof(*val), &t);
	if (ret)
		return ret;

	ret = ph->xops->do_xfer(ph, t);
	if (!ret)
		*val = le32_to_cpu(*(__le32 *)t->rx.buf);   /* ✗ 未檢查 rx.len */

	ph->xops->xfer_put(ph, t);
	return ret;
}
```

審查要點：

1. **`xfer_get_init` 會 sleep**（等 xfer slot），不能在 atomic 或 irq context 呼叫 — 這和 SCMI 底層 mailbox + `wait_for_completion_timeout` 的設計一致。
2. **`rx.len` 必須檢查**：SCP 回傳長度可能比預期短（protocol version 差異）、或長（惡意/異常韌體）。不檢查就 `le32_to_cpu` 會讀到未初始化或越界記憶體 — 這在 Android security hardening review 會被立刻擋下。
3. **Endianness** — SCMI 規範 payload 都是 little-endian，要用 `le32_to_cpu` / `put_unaligned_le32`，不能用 `memcpy` + host order 讀取。
4. **`do_xfer` 的回傳值含 transport 錯誤（-ETIMEDOUT/-EIO）和 protocol 錯誤（-EINVAL 等）** — 兩者路徑要一致 undo。
5. **保留 `xfer_put` 在所有路徑** — 用 `goto out_put` + 單一出口比較安全。

改進版：

```c
int my_get_value(const struct scmi_protocol_handle *ph, u32 *val)
{
	struct scmi_xfer *t;
	int ret;

	ret = ph->xops->xfer_get_init(ph, MY_CMD_GET, 0, sizeof(__le32), &t);
	if (ret)
		return ret;

	ret = ph->xops->do_xfer(ph, t);
	if (ret)
		goto out;

	if (t->rx.len < sizeof(__le32)) {
		ret = -EPROTO;
		goto out;
	}

	*val = get_unaligned_le32(t->rx.buf);
out:
	ph->xops->xfer_put(ph, t);
	return ret;
}
```

### 問題 14.3: Notification（RX）— IRQ context 與 GFP_ATOMIC

```c
/* 被審查的代碼：SCMI notification handler */
static int my_notify_cb(struct notifier_block *nb,
			unsigned long action, void *data)
{
	struct my_event *ev;

	ev = kmalloc(sizeof(*ev), GFP_KERNEL);   /* ✗ 若 cb 在 RX softirq 中呼叫 */
	if (!ev)
		return NOTIFY_BAD;

	memcpy(ev, data, sizeof(*ev));
	queue_work(my_wq, &ev->work);
	return NOTIFY_OK;
}
```

審查要點：

1. **SCMI RX 路徑上下文**：`scmi_rx_callback` 可能從 mailbox IRQ handler 回調（看 transport：`mailbox.c` vs `smc.c` vs `virtio.c`）— SMC transport 在 process context、mailbox 在 softirq、virtio 在 threaded IRQ。**最保守的假設是 atomic context**。
2. **改用 `GFP_ATOMIC` 或 `GFP_NOWAIT`**；若能承受失敗就改預先配好的 per-cpu pool。
3. **`NOTIFY_BAD` 會中斷整條 notifier chain** — 通常要改 `NOTIFY_DONE` 讓其他 listener 繼續。
4. **`queue_work` 本身可在 atomic 呼叫**（內部 spin_lock），但別忘 release：`work` 生命週期要和 `ev` 一致。

### 問題 14.4: Timeout / Completion 的陷阱

```c
/* 被審查的代碼：polling + completion 混用 */
ret = wait_for_completion_timeout(&xfer->done, msecs_to_jiffies(30));
if (!ret)
	return -ETIMEDOUT;   /* ✗ 未取消 in-flight xfer，token 洩漏 */
```

審查要點：

1. **SCMI xfer token 是 10-bit（1024 in-flight）**，timeout 不做 `mark_xfer_as_aborted` 就無法回收；連續 timeout 會耗盡 token pool，整個協定卡死。
2. **正確做法**：參考 `drivers/firmware/arm_scmi/driver.c` 的 `scmi_xfer_done_no_timeout` 與 `scmi_abort_match_transfer` — 超時後要發 `abort` 或至少把 xfer 標為 aborted，等 SCP 遲到回應時能丟棄。
3. **Timeout 值** — SCMI spec 建議 30 ms 基線；ACPM on GS101 因 APM 本身很忙可能要放寬到 200 ms。硬編碼 30 ms 在某些 SoC 會誤觸 timeout。

### 問題 14.5: DT binding 與 `of_device_id`

```c
/* ✗ 只匹配一個 compatible，之後換 SoC 又要改一輪 */
static const struct of_device_id scmi_of_match[] = {
	{ .compatible = "arm,scmi" },
	{}
};

/* ✓ ARM SCMI 主幹的 match table */
static const struct of_device_id scmi_of_match[] = {
	{ .compatible = "arm,scmi" },       /* mailbox transport */
	{ .compatible = "arm,scmi-smc" },   /* SMC transport */
	{ .compatible = "arm,scmi-virtio" },/* VirtIO transport */
	{}
};
```

審查要點：

1. **DT binding 的命名規則** — vendor prefix (`arm,`、`google,`、`qcom,`) 必須在 `Documentation/devicetree/bindings/vendor-prefixes.yaml` 裡登錄；沒登錄 checkpatch 擋。
2. **Vendor 特化的 `compatible`**：i.MX95 有 `arm,scmi-imx` 等衍生；加新的 vendor protocol 要同時改 binding schema、driver match、`Documentation/devicetree/bindings/firmware/arm,scmi.yaml`。
3. **`MODULE_DEVICE_TABLE(of, ...)`** 不要忘 — 沒這行 `modprobe` 無法自動載入模組。

### 問題 14.6: 跨 clock / perf / regulator provider 的生命週期

```c
/* ✗ SCMI clock provider 的 error path 有 leak */
static int scmi_clk_probe(struct scmi_device *sdev)
{
	struct clk_hw_onecell_data *clk_data;
	int i, ret;

	clk_data = devm_kzalloc(&sdev->dev, struct_size(clk_data, hws, count),
				GFP_KERNEL);
	...
	for (i = 0; i < count; i++) {
		ret = clk_hw_register(&sdev->dev, &clks[i].hw);
		if (ret)
			return ret;   /* ✗ 前面 i-1 個已 register 沒 unregister */
	}
	...
}
```

審查要點：

1. **clock_hw / PM domain / regulator provider 都是「註冊 N 個，失敗要 undo 前 i-1 個」模式** — 要用 `goto err_unreg;` 或 `devm_clk_hw_register`（`devres` 版自帶 release）。
2. **SCMI clock / perf protocol ↔ clk / cpufreq framework 的生命週期對齊**：若 SCMI transport 在 runtime 被拔除（例：virtio disconnect），底層 `scmi_protocol_handle` 失效，consumer driver 需要感知 — 透過 `scmi_device_remove()` 回呼清理。
3. **`of_clk_add_provider` vs `of_clk_add_hw_provider`** — 後者支援 `clk_hw` 新 API，新驅動一律用 hw 版；兩者 release 路徑不同別混。

### 問題 14.7: Concurrency — xfer 池的鎖爭用

```c
/* 熱路徑：cpufreq governor 每 10 ms call 一次 perf_set_level */
ret = scmi_set_level(ph, domain, level);
/* ✗ 若 domain × CPU 組合讓 xfer 池打滿，governor 會 spin 等 slot */
```

審查要點：

1. SCMI xfer 池每 protocol 預設 20 slot（`scmi.max_msg = 20`），**多 cluster DVFS 同時切會搶光** — 熱路徑要避免 synchronous xfer，改用 `scmi_perf_level_set_delayed` / fastchannel。
2. **Fastchannel**：SCMI spec 1.1+ 允許 perf/power protocol 在 shared mem 暴露 doorbell-less 頻道，CPU 直寫 mem 就等於下指令；`drivers/firmware/arm_scmi/perf.c` 的 `scmi_perf_fc_ring_db()` 是典型實作。沒 fastchannel 的 SoC 熱路徑會被 mailbox 卡。
3. **Lockdep 視角**：SCMI xfer 內部有 `scmi_info->tx_lock` + `xfer->done` completion + protocol 自有 mutex — 三層鎖順序要一致，不要在 protocol mutex 持有時呼叫 `do_xfer`（會 ABBA 反向）。

### 問題 14.8: Security — userspace-trigger DoS

```c
/* ✗ ioctl 直接接 user 呼叫 SCMI */
case MY_IOCTL_SET_PERF:
	ret = scmi_set_level(ph, arg.domain, arg.level);
```

審查要點：

1. **SCMI xfer 是有限資源，userspace 若能無限 trigger，整顆 SCP 會被拖垮**；要加 rate-limit、capability check、or at minimum CAP_SYS_ADMIN。
2. **`arg.domain`、`arg.level` 必須範圍檢查** — SCP 收到 out-of-range 可能 hang（vendor SCP 韌體品質良莠不齊）。
3. **Android CTS / vts 範疇**：任何由 user-trigger 的 SCMI RPC 都要 SELinux policy 保護；沒 policy 的 sysfs/ioctl 會被 CTS flag。

### 問題 14.9: VirtIO SCMI — Guest 視角特殊點

```c
/* VirtIO transport 的 xfer — guest 跑在 VM 內 */
ret = virtqueue_add_sgs(vq, sgs, out_sgs, in_sgs, xfer, GFP_KERNEL);
```

審查要點：

1. **Guest 不能假設 host 會回覆** — timeout 要更保守；host 若 hang，guest 不能跟著 hang。
2. **SCMI VirtIO device id = 32**（`VIRTIO_ID_SCMI`）— binding 要明寫。
3. **KVM / confidential compute 場景**：shared buffer 要走 swiotlb 或 SEV-encrypted 頁，`virtio-scmi` 底層 buffer allocation 要用 `dma_alloc_coherent` 而非 `kmalloc`（否則 bounce buffer 失效）。

### 問題 14.10: SCMI 一句話 review 話術

- 「Protocol driver 的 probe 用 `devm_scmi_protocol_acquire` 不用 `protocol_get` — 自動 refcount。」
- 「RX callback 一律假設 atomic context，`GFP_ATOMIC`；會動的實作移到 workqueue。」
- 「Timeout 要配對 `scmi_abort_match_transfer` 回收 token，否則池用光協定就死。」
- 「Payload 讀寫用 `get_unaligned_le32`，不假設對齊與 host endian。」
- 「熱路徑 DVFS 永遠優先 fastchannel，不要同步 xfer 每 10 ms call。」
- 「跨 transport 的 binding 要同時更新 yaml schema 與 `of_device_id`。」

---

## 15. Google ACPM 視角（`drivers/firmware/samsung/exynos-acpm.c`）

Google Tensor GS101 系列（Pixel 6/7/8）與 Samsung Exynos 從 E9825 起**不走 SCMI**，而是使用 Samsung 自家的 ACPM（Alive Clock and Power Manager）協定。**Linaro 直到 2024 年才把 ACPM upstream**（1,111 行單一 driver），在此之前完全 live 在 Google vendor tree。Code review 的角度與 SCMI 有幾個關鍵差異。

### 問題 15.1: Compatible / Binding — ACPM 不是 SCMI

```c
/* ✓ GS101 DT 片段（arch/arm64/boot/dts/exynos/google/gs101.dtsi:486） */
firmware {
	acpm_ipc: power-management {
		compatible = "google,gs101-acpm-ipc";
		#clock-cells = <1>;
		mboxes = <&ap2apm_mailbox>;
		shmem = <&apm_sram>;
	};
};

/* driver 端的 match table（drivers/firmware/samsung/exynos-acpm.c:772） */
static const struct of_device_id acpm_match[] = {
	{ .compatible = "google,gs101-acpm-ipc", .data = &acpm_gs101 },
	{},
};
```

審查要點：

1. **`compatible = "google,gs101-acpm-ipc"` 不是 `arm,scmi`** — 兩者的訊息格式、handshake、channel ID 空間都不同，不能混用 binding。
2. **binding schema** 在 `Documentation/devicetree/bindings/firmware/google,gs101-acpm-ipc.yaml`，與 `arm,scmi.yaml` 並列但互不相容。
3. **vendor prefix `google,`** 已在 vendor-prefixes.yaml 登錄，但 `samsung,` 早期 ACPM DT 亦存在 — GS101 path 下要特別小心別改成 samsung 命名空間。

### 問題 15.2: Shared SRAM ownership — 冷啟動競態

```c
/* ✗ ACPM probe 未驗證 APM 是否 ready */
ret = of_reserved_mem_device_init(dev);
shmem_base = ioremap(res.start, size);
/* 直接開始讀 channel descriptor — 若 APM 韌體還沒初始化 SRAM，讀到 garbage */
acpm->channels = shmem_base + desc->ch_offset;
```

審查要點：

1. **ACPM 的 shmem 是 AP 和 APM 小核共享的 SRAM**。Cold boot 時 BL1 載入 APM 韌體後，APM 會在 SRAM 特定位置寫 magic（類似 "ACPM"）；AP 必須 poll 這個 magic 才能確認 APM 已 ready。
2. **錯過 handshake 的後果**：讀到 garbage channel descriptor → ipc send 寫到錯誤位址 → APM panic 或 system hang。Google vendor tree 的 ACPM driver 有一段 `acpm_ipc_wait_for_init` 專門處理。
3. 對比 SCMI：`arm,scmi-shmem` 由 ATF/SCP 在 cold boot 前就分配好 descriptor，AP 幾乎總是後到，不需 handshake。

### 問題 15.3: Mailbox + shmem 的半雙工模式

```c
/* ✗ 假設 mailbox 是全雙工，tx/rx 各自獨立 channel */
acpm_send(msg, MBOX_CHAN_TX);
/* 等回覆 */
acpm_recv(resp, MBOX_CHAN_RX);
```

審查要點：

1. **ACPM mailbox 常是半雙工**（一條 mbox + 數個 shmem slot）：TX 寫 slot + ring doorbell，APM 改同一 slot 的狀態位並 ring back。不是 SCMI 那種「TX channel 一條 / RX notification 另一條」。
2. **slot 的生命週期**：AP 寫入 → doorbell → busy-wait 或 wait_for_completion → APM 寫回 → AP 讀出 → slot 釋放。必須在 slot 釋放前**禁止**其他 AP thread 用同一 slot — 要用 per-channel mutex 或 bit-lock。
3. **對比 SCMI xfer**：SCMI 的 xfer token 一個 xfer 只用一次，token 空間大（10-bit）、可以 concurrent 多筆 in-flight；ACPM 常見 channel 只 4-8 slot，高度爭用。

### 問題 15.4: ACPM 作為 clock provider — DVFS 特化

```c
/* GS101 CPU node */
cpu0: cpu@0 {
	...
	clocks = <&acpm_ipc GS101_CLK_ACPM_DVFS_CPUCL0>;
};
```

審查要點：

1. **ACPM 同時身兼 firmware IPC 和 clock provider**：`#clock-cells = <1>` 讓其他 node 直接 `<&acpm_ipc CLK_ID>` 引用。ID 空間定義在 `include/dt-bindings/clock/google,gs101-acpm.h`，完全私有。
2. **與 `drivers/clk/samsung/clk-gs101.c` 的分工**：CMU（Clock Management Unit，週邊 clock）走 `clk-gs101.c` 直接寫暫存器；CPU / GPU / Memory DVFS 走 ACPM。code review 要確認「改的是哪一類 clock」，直寫 CMU 與跨 IPC 到 ACPM 的錯誤模式、延遲特性不同。
3. **`clk_ops.set_rate` 在 ACPM 是非同步**：`set_rate` 送 IPC 不代表 frequency 已切完，consumer（cpufreq）需要 `clk_notifier` 或 polling 確認 — 這是 DVFS bug 的常見源頭。

### 問題 15.5: ACPM-PMIC — 走韌體的 regulator

```c
/* S2MPG10 regulator 透過 ACPM 控制 */
ret = acpm_pmic_bulk_read(ACPM_PMIC_BUS, reg, val, len);
```

審查要點：

1. **PMIC I2C bus 物理上由 APM 掌控，AP 不直接觸 PMIC** — 這和一般 I2C regulator 的 `regmap` 路徑**完全不同**。
2. **ACPM 轉發所有 PMIC read/write**：code review 時若看到 `i2c_smbus_read_*` 操作 S2MPG10，幾乎肯定是 bug（bypass ACPM 會和 APM 同時踩 I2C master 造成 hang）。
3. **PMIC 關鍵暫存器的 sequence**：APM 韌體會在開機流程管理 power-on sequence，AP 做 regulator_enable 時只是下 IPC；若 APM 韌體版本與 DT 的 `opp-table` 不一致，可能成功送 IPC 但沒真的 enable — 需要 `acpm_pmic_probe` 比對 firmware version 做版本 gate。

### 問題 15.6: 缺乏 notify framework — 主動訊息的處理

```c
/* ✗ 把 SCMI notify 習慣帶到 ACPM */
scmi_register_notifier(ph, EVENT, &nb);   /* ACPM 沒這個 API */
```

審查要點：

1. **ACPM 沒有像 SCMI 那樣的 notification protocol framework**。APM 主動上送的 thermal event / OCP event 等，走的是特定 channel + IRQ + 驅動私有 dispatch — review 時要確認 event flow 走過哪些檔案（`exynos-acpm.c` → 個別 `exynos-*` thermal/OCP driver）。
2. **Event 丟失風險**：若 AP IRQ 被延遲、channel slot 被 overwrite，APM 送的 thermal trip 點可能消失 → 熱失控。Vendor tree 有「APM 重送 N 次」的 retry 邏輯；upstream 版本要 review 有沒有 ported。
3. **改走 SCMI Base protocol 的 Notify 則可用 `scmi_notifier_register`** — 這是 ACPM → SCMI 遷移時第一個要對的 API gap。

### 問題 15.7: Module ownership / copyright header

```c
/*
 * ✓ exynos-acpm.c 的 header 長這樣
 *
 * Copyright 2020-2024 Google LLC
 * Copyright 2020-2024 Samsung Electronics
 * Copyright 2024 Linaro Ltd.
 *
 * Authors: ...
 */
MODULE_AUTHOR("Samsung / Google / Linaro");
MODULE_DESCRIPTION("Samsung Exynos ACPM mailbox protocol driver");
MODULE_LICENSE("GPL v2");
```

審查要點：

1. **ACPM 是 Samsung + Google + Linaro 三方 copyright** — review 新 patch 時 `SPDX-License-Identifier: GPL-2.0` 必須保留；私改成 MIT/BSD 會直接被 Samsung 擋。
2. **MODULE_LICENSE("GPL v2")** 觸發 `EXPORT_SYMBOL_GPL` 的可見性 — proprietary modules 不能用。若要 export 給 Google 自家 vendor module（如 GXP、TPU driver）使用，兩邊都必須是 GPL。
3. **GS101 vs Exynos 9855 的分野**：Tensor GS101 是 Exynos 9855 衍生，很多 driver 同源；upstream patch 若單純針對 Google use case 可能破壞 Samsung 其他平台，要跑 build matrix（至少 GS101 + 最近一代 Exynos）。

### 問題 15.8: Google accelerator firmware — 不在 ACK 的現實

```c
/* ✗ 試圖 upstream 一段 call 到 "Google GXP firmware" 的代碼 */
ret = gxp_firmware_send_cmd(dev, CMD_ALLOC_CORE, 0, NULL);
```

審查要點：

1. **Pixel TPU / GXP accelerator driver 在 `private/google-modules/` 的 vendor tree，不在 ACK、不在 mainline**。Code review 時若看到 `gxp_`、`gcip_`、`edgetpu_` symbols，要先確認目標 tree 是 vendor tree 還是 mainline。
2. **Vendor tree 的 review 標準不同**：upstream 紀律（SPDX、binding、checkpatch、KASAN）在 vendor tree 常被放寬，但 Google internal review 仍要求 CL 通過 TAP presubmit + style check。
3. **SCMI vendor namespace 的可能性**：若日後 Google 要把 accelerator 控制面 upstream，合理做法是在 `drivers/firmware/arm_scmi/vendors/google/` 建立 vendor protocol（類似 `vendors/imx/`）— 目前此目錄不存在，僅有 `vendors/imx/`。

### 問題 15.9: ACPM 一句話 review 話術

- 「ACPM 不是 SCMI — binding `compatible` 是 `google,gs101-acpm-ipc`，訊息格式私有。」
- 「Shmem 冷啟要 handshake，不能 ioremap 完就讀 descriptor。」
- 「ACPM mailbox 半雙工，slot 持有期間要互斥，不是 SCMI 那種 10-bit token pool。」
- 「PMIC 走 ACPM IPC，任何 `i2c_smbus_*` on S2MPG10 都是 bug。」
- 「ACPM 沒有 notification framework — event dispatch 是 driver 私有路徑，review 要追 IRQ → channel → consumer driver 整串。」
- 「GS101 patch 要同時在 Exynos 家族 build matrix 過。」
- 「Pixel TPU/GXP 不在 ACK，任何 `gxp_*` / `edgetpu_*` 符號在 upstream review 要擋下。」

---

## 16. 三方比較速查（SCMI / ACPM / RPMh）review 重點

| Review 面向 | ARM SCMI | Google ACPM | Qualcomm RPMh |
|-------------|----------|-------------|---------------|
| **DT compatible** | `arm,scmi`/`arm,scmi-smc`/`arm,scmi-virtio` | `google,gs101-acpm-ipc` | `qcom,rpmh-rsc` |
| **Binding schema** | `Documentation/devicetree/bindings/firmware/arm,scmi.yaml` | `google,gs101-acpm-ipc.yaml` | `qcom,rpmh-rsc.yaml` |
| **Protocol register API** | `devm_scmi_protocol_acquire` | driver private `acpm_ipc_*` | RPMh 無「protocol」概念，直接 `rpmh_write_*` |
| **Handshake** | 透過 Base protocol discover | 冷啟 APM shmem magic 等待 | Command DB（bootloader 靜態表） |
| **TX context** | `xfer_get_init` 可睡 | IPC send 可睡 | `rpmh_write_async` 可 atomic（TCS 硬體） |
| **RX context** | 可能 softirq / threaded IRQ | Samsung 私有 IRQ 路徑 | TCS IRQ → softirq |
| **Token / slot 空間** | 10-bit (1024) per protocol | 小（4-8 slot per channel）| TCS slot 有限，vote 聚合寫 |
| **GFP 選擇** | Notify cb 用 `GFP_ATOMIC` | 同 | `rpmh_write_async` 不分配 |
| **熱路徑** | fastchannel（shared mem） | ACPM DVFS（mailbox 仍在 critical path） | OSM/EPSS（CPU 硬體 FSM 自跑） |
| **Abort/timeout recovery** | `scmi_abort_match_transfer` | driver 私有 slot 回收 | TCS 硬體 timeout（無 SW 需求） |
| **Payload endian** | little-endian（spec） | SoC host endian（ARM64 little）| Command DB encoded |
| **Security 熱點** | userspace ioctl 可觸發 → rate limit | PMIC bus 共享 → 不可 bypass | SCM/QSEECOM 入口 |
| **Upstream 狀態** | 完全 upstream，protocol 完整 | 2024 才 upstream（1,111 行） | 完全 upstream（多年積累） |
| **Main driver 行數** | 19,481（31 檔）| 1,111（單檔） | 4,447 + soc/qcom/*（分散） |
| **Review 焦點** | devm、ERR_PTR、GFP、abort、endian、fastchannel | handshake、PMIC 獨占、半雙工 slot、Google/Samsung 雙 copyright | TCS atomic、Command DB 驗證、BCM voter 層級 |

---

## 17. 參考資源（Firmware 專題補充）

- `drivers/firmware/arm_scmi/` — ARM SCMI 全套實作（`driver.c`, `mailbox.c`, `smc.c`, `virtio.c`, `perf.c`, `clock.c`, `sensors.c` ...）
- `drivers/firmware/arm_scmi/vendors/imx/` — i.MX 的 vendor protocol 範例（Google / Samsung 尚無對應）
- `drivers/firmware/samsung/exynos-acpm.c` — Linaro 2024 upstream 的 ACPM 主體
- `include/dt-bindings/clock/google,gs101-acpm.h` — ACPM DVFS clock ID 空間
- `Documentation/devicetree/bindings/firmware/arm,scmi.yaml` — SCMI binding schema
- `Documentation/devicetree/bindings/firmware/google,gs101-acpm-ipc.yaml` — ACPM binding schema
- `/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/analyses/scmi-vs-google-acpm.md` — 三方對照完整分析
- `/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/subsystems/firmware.md` — Firmware 子系統總覽
- ARM SCMI Specification（DEN0056）<https://developer.arm.com/documentation/den0056/>
- LWN — Firmware support for multiple transports: <https://lwn.net/Articles/812588/>
