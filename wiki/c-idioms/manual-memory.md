# Manual Memory Management C 習慣語

## 核心概念

C 沒有自動垃圾回收，程序員必須手動管理記憶體。選擇堆或棧、何時 malloc/free 是性能和安全的關鍵。

**記憶體區域：**
- **棧（Stack）**：自動回收，有限大小，速度快，用於局部變數
- **堆（Heap）**：手動管理，大小可變，速度較慢，用於動態資料結構

---

## 核心模式

### 1. malloc/calloc/free 的正確使用

```c
#include <stdlib.h>
#include <string.h>

// malloc：分配 n 字節，初值未定義
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    // 記憶體分配失敗
    return;
}
// 使用 arr
free(arr);
arr = NULL;  // 良好習慣：釋放後置為 NULL

// calloc：分配 n 個元素，每個 m 字節，初值為 0
int *arr = calloc(10, sizeof(int));  // 分配 10 個 int，全為 0
if (arr == NULL) {
    return;
}
free(arr);

// realloc：調整既有記憶體大小
int *arr = malloc(10 * sizeof(int));
int *newArr = realloc(arr, 20 * sizeof(int));
if (newArr == NULL) {
    // 失敗時，原指標仍有效
    free(arr);
    return;
}
arr = newArr;
free(arr);
```

**重要注意事項：**
- 總是檢查返回值是否為 NULL
- `realloc` 失敗時原指標仍有效，不可丟失
- 釋放後應置為 NULL 以避免野指標

---

### 2. 棧分配的陣列（Stack-Allocated Arrays）

```c
// 當大小已知且不太大時，用棧陣列
void processFixedSize(void) {
    int buffer[1024];  // 棧陣列，自動回收
    for (int i = 0; i < 1024; i++) {
        buffer[i] = i;
    }
    // 函數返回時自動回收
}

// 優點：無需 malloc/free，更快
// 缺點：大小必須在編譯時已知，且大小受棧限制

// 典型 LeetCode 模式
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int *result = malloc(2 * sizeof(int));  // 已知大小為 2
    result[0] = 0;
    result[1] = 1;
    *returnSize = 2;
    return result;
}
```

**何時使用棧陣列：**
- 大小固定且較小（< 1MB）
- 不需要在函數外使用
- 頻繁分配/釋放

---

### 3. VLA 的注意（Variable Length Array）

```c
// C99+ 支援 VLA，但有風險
void processArray(int size) {
    // 直接在棧上分配大小為 size 的陣列
    int arr[size];  // VLA：編譯時大小未知

    // 危險：若 size 過大導致棧溢出
    // 標準不保證 VLA 的成功分配
}

// 更安全的做法
#include <stdlib.h>

void processArray(int size) {
    // 檢查大小是否合理
    if (size > 1000000) {
        return;  // 拒絕過大申請
    }

    int *arr = malloc(size * sizeof(int));
    if (arr == NULL) {
        // 處理分配失敗
        return;
    }

    // 使用 arr
    free(arr);
}
```

**建議：**
- 避免過大的 VLA
- 對於不確定大小，使用 malloc
- 某些編譯器對 VLA 的支援不完整

---

## 常見模式

### 4. 動態結構陣列（Dynamic Array of Structures）

```c
typedef struct {
    int val;
    char *name;  // 指標：需獨立釋放
} Node;

// 分配結構陣列
Node *nodes = malloc(n * sizeof(Node));
if (nodes == NULL) return;

// 初始化結構中的指標
for (int i = 0; i < n; i++) {
    nodes[i].name = malloc(MAX_NAME);
    strcpy(nodes[i].name, "default");
}

// 釋放時需逐個釋放嵌入指標
for (int i = 0; i < n; i++) {
    free(nodes[i].name);
}
free(nodes);
```

**重點：** 嵌入的指標需獨立釋放，不會被自動回收

---

### 5. 鏈表節點分配（Linked List Node Allocation）

```c
typedef struct Node {
    int val;
    struct Node *next;
} Node;

// 創建新節點
Node *createNode(int val) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->val = val;
    node->next = NULL;
    return node;
}

// 釋放鏈表
void freeList(Node *head) {
    while (head) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

// 使用
Node *head = NULL;
head = createNode(1);
head->next = createNode(2);
head->next->next = createNode(3);
freeList(head);
```

---

## 記憶體洩漏檢測

### 常見洩漏模式

```c
// 洩漏 1：忘記釋放
char *s = malloc(100);
strcpy(s, "hello");
return;  // 未 free(s) — 洩漏！

// 洩漏 2：過早返回
int *arr = malloc(100 * sizeof(int));
if (arr == NULL) {
    return -1;  // 未釋放已分配的資源
}

// 洩漏 3：例外路徑
int *a = malloc(100);
int *b = malloc(100);
if (someError) {
    // 只釋放 a，未釋放 b — 洩漏！
    free(a);
    return;
}
free(a);
free(b);

// 洩漏 4：循環中的分配
for (int i = 0; i < n; i++) {
    char *tmp = malloc(100);
    // 使用 tmp
    // 未 free(tmp) — 迴圈每次都洩漏！
}
```

### 避免洩漏的策略

```c
// 策略 1：配對分配和釋放
int *arr = malloc(size * sizeof(int));
// ... 使用 ...
free(arr);

// 策略 2：使用 goto 確保清理
int *a = malloc(100);
int *b = malloc(100);
int *c = malloc(100);

if (error1) goto cleanup;
if (error2) goto cleanup;

// 使用 a, b, c

cleanup:
    free(c);
    free(b);
    free(a);
    return;

// 策略 3：明確的清理函數
typedef struct {
    int *data;
    int size;
} Array;

Array* createArray(int size) {
    Array *arr = malloc(sizeof(Array));
    arr->data = malloc(size * sizeof(int));
    arr->size = size;
    return arr;
}

void destroyArray(Array *arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}
```

---

## 堆 vs. 棧的選擇

| 特性 | 堆 | 棧 |
|------|-----|-----|
| 大小限制 | 系統可用記憶體 | OS 限制（通常 < 10MB） |
| 分配速度 | 較慢 | 很快（只移動指標） |
| 回收 | 手動 (free) | 自動 |
| 適用 | 動態大小、長壽期 | 固定大小、短壽期 |
| 碎片化 | 可能 | 不會 |
| 典型大小 | 數 MB 到 GB | KB 到 MB |

---

## 最佳實踐

1. **總是檢查 malloc 返回值**
   ```c
   int *p = malloc(size * sizeof(int));
   if (p == NULL) { /* 處理錯誤 */ }
   ```

2. **釋放後置為 NULL**
   ```c
   free(ptr);
   ptr = NULL;
   ```

3. **避免野指標**
   ```c
   // 錯誤：多次 free
   free(ptr);
   free(ptr);  // 未定義行為

   // 正確
   free(ptr);
   ptr = NULL;
   if (ptr) free(ptr);  // 安全，因為 free(NULL) 無害
   ```

4. **使用 sizeof(type)，不用硬編碼**
   ```c
   // 好
   int *p = malloc(n * sizeof(int));

   // 不好
   int *p = malloc(n * 4);  // 假設 int 是 4 字節
   ```

5. **動態陣列用 malloc，已知大小用棧**
   ```c
   int *arr = malloc(runtime_size * sizeof(int));
   int buffer[1024];  // 編譯時大小已知
   ```

---

## 面試要點

1. **解釋棧和堆的區別以及選擇標準**
2. **展示完整的分配和釋放序列**
3. **討論記憶體洩漏的檢測和預防**
4. **說明何時使用 malloc vs. 棧陣列**
5. **考慮動態結構中的嵌入指標**

## 實現檢查清單

- [ ] 所有 malloc 都檢查 NULL
- [ ] 每個 malloc 都有對應的 free
- [ ] 釋放後指標置為 NULL
- [ ] 嵌入指標被正確釋放
- [ ] 無雙重 free
- [ ] 無野指標訪問
- [ ] 用 valgrind 或類似工具檢測洩漏
- [ ] 符合 RAII 概念（資源分配即初始化）
