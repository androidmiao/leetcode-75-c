# C 指標操作習慣語 (Pointer Manipulation Idioms)

## 核心概念

指標是 C 的核心機制，掌握指標操作習慣語對寫出高效、簡潔的代碼至關重要。

**關鍵概念：**
- 單層指標 (`int *ptr`)：指向一個值
- 雙層指標 (`int **ptr`)：指向指標本身，常用於修改指標或動態陣列
- 指標運算：加減、比較、取值
- 記憶體佈局

---

## 核心模式

### 1. 雙層指標用於修改指標本身（Pointer-to-Pointer for Reassignment）

```c
// 鏈表頭節點修改時需要雙層指標
typedef struct {
    int val;
    struct ListNode *next;
} ListNode;

// 正確方式：使用雙層指標
void deleteNode(ListNode **head, int val) {
    while (*head && (*head)->val != val) {
        head = &((*head)->next);  // 前進指標
    }

    if (*head) {
        ListNode *temp = *head;
        *head = (*head)->next;  // 修改前驅指標
        free(temp);
    }
}

// 使用示例
ListNode *head = ...;
deleteNode(&head, 5);  // 傳遞 &head（雙層指標）
```

**何時使用：**
- 需要修改函數外部的指標本身
- 鏈表操作（刪除頭節點、修改指標連接）
- 動態陣列的增長/縮小

---

### 2. 哨兵/虛擬節點簡化邊界（Sentinel/Dummy Node）

```c
// 使用虛擬節點避免對頭節點的特殊處理
typedef struct {
    int val;
    struct ListNode *next;
} ListNode;

// 刪除鏈表中所有值為 val 的節點
ListNode* removeElements(ListNode* head, int val) {
    // 創建虛擬節點，指向真實頭節點
    ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    ListNode *prev = &dummy;  // 使用虛擬節點作為前驅

    while (prev->next) {
        if (prev->next->val == val) {
            ListNode *temp = prev->next;
            prev->next = prev->next->next;
            free(temp);
        } else {
            prev = prev->next;
        }
    }

    ListNode *newHead = dummy.next;
    // 注意：不 free dummy，因為它在棧上
    return newHead;
}
```

**好處：**
- 統一對頭節點和其他節點的處理
- 避免 `if (head == NULL)` 的特殊情況
- 代碼更簡潔、不易出錯

---

### 3. 快慢指標模式（Fast/Slow Pointer Pattern）

```c
// 檢測鏈表是否有環
typedef struct {
    int val;
    struct ListNode *next;
} ListNode;

bool hasCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;           // 每次前進 1 步
        fast = fast->next->next;     // 每次前進 2 步

        // 若有環，fast 遲早會追上 slow
        if (slow == fast) {
            return true;
        }
    }
    return false;
}
```

**應用場景：**
- 檢測環（Floyd 循環檢測算法）
- 找鏈表中點
- 找倒數第 k 個節點

**時間複雜度：** O(n)
**空間複雜度：** O(1)

---

### 4. 指標交換（Pointer Swapping）

```c
// 兩個指標互換所指向的值
#define SWAP(a, b, type) do { \
    type temp = *(a); \
    *(a) = *(b); \
    *(b) = temp; \
} while(0)

void example(void) {
    int x = 10, y = 20;
    int *px = &x, *py = &y;
    SWAP(px, py, int);
    // 現在 x = 20, y = 10
}

// 或者不用宏
void swapPointers(int **p1, int **p2) {
    int *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}
```

---

### 5. 指標運算（Pointer Arithmetic）

```c
// 遍歷陣列的多種方式
void traverseArray(int *arr, int size) {
    // 方式 1：下標法
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }

    // 方式 2：指標遞增
    for (int *p = arr; p < arr + size; p++) {
        printf("%d ", *p);
    }

    // 方式 3：指標和索引混合
    int *end = arr + size;
    for (int *p = arr; p != end; p++) {
        printf("%d ", *p);
    }
}
```

**指標運算規則：**
- `ptr + n` = 向前 n 個元素
- `ptr - n` = 向後 n 個元素
- `ptr1 - ptr2` = 指標間隔的元素個數
- 指標比較：`<`, `<=`, `>`, `>=`, `==`, `!=`

---

### 6. 指標轉換（Pointer Casting）

```c
// 不同類型指標的轉換
int x = 0x12345678;

// 方式 1：void 指標作為中介
void *vptr = &x;
int *iptr = (int *)vptr;

// 方式 2：直接轉換
char *cptr = (char *)&x;  // 按位元組存取
short *sptr = (short *)&x;  // 按雙位元組存取

// 方式 3：在 union 中混合使用
union {
    int i;
    char c[4];
} data;
data.i = 0x12345678;
printf("%02x ", data.c[0]);  // 存取最低位元組
```

---

## 常見陷阱

### 1. 野指標（Dangling Pointer）

```c
// 錯誤：指標指向已釋放的記憶體
int *getPointer(void) {
    int x = 10;
    return &x;  // 危險！x 的生命週期結束
}

// 正確：傳回分配的記憶體
int *getPointer(void) {
    int *p = malloc(sizeof(int));
    *p = 10;
    return p;  // 呼叫者需 free
}
```

### 2. 空指標取值

```c
// 錯誤
int *ptr = NULL;
printf("%d\n", *ptr);  // 段錯誤！

// 正確
if (ptr != NULL) {
    printf("%d\n", *ptr);
}
```

### 3. 緩衝區溢出

```c
// 錯誤
char buffer[10];
char *p = buffer;
p += 20;  // 超出邊界
*p = 'x';  // 未定義行為

// 正確
char buffer[10];
for (char *p = buffer; p < buffer + 10; p++) {
    *p = 'x';
}
```

---

## 指標和陣列的關係

```c
// 陣列衰減為指標
int arr[5] = {1, 2, 3, 4, 5};
int *ptr = arr;  // 等於 &arr[0]

// 等價的存取方式
arr[2] == *(ptr + 2) == *(arr + 2)

// 函數參數中陣列衰減為指標
void printArray(int arr[], int size) {
    // arr 實際是 int *arr
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
}
```

---

## 動態陣列與 2D 陣列

```c
// 1D 動態陣列
int *arr1d = malloc(n * sizeof(int));
arr1d[i] == *(arr1d + i)

// 2D 動態陣列（陣列的陣列）
int **arr2d = malloc(rows * sizeof(int *));
for (int i = 0; i < rows; i++) {
    arr2d[i] = malloc(cols * sizeof(int));
}

// 存取與釋放
arr2d[i][j] == *(*(arr2d + i) + j)

for (int i = 0; i < rows; i++) {
    free(arr2d[i]);
}
free(arr2d);
```

---

## 指標型別與 typedef

```c
// 清晰的指標定義
typedef int *IntPtr;
typedef struct Node {
    int val;
    struct Node *next;
} Node;

typedef Node *NodePtr;  // 比 Node *ptr 更清晰

// 函數指標
typedef int (*Comparator)(const void *, const void *);

int compare(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

qsort(arr, size, sizeof(int), (Comparator)compare);
```

---

## 面試要點

1. **清晰解釋雙層指標的用途**
2. **討論野指標和記憶體洩漏的風險**
3. **展示快慢指標的應用**
4. **解釋指標運算的底層機制**
5. **比較不同的鏈表操作方式（虛擬節點 vs. 雙層指標）**

## 實現檢查清單

- [ ] 所有指標初始化（或檢查 NULL）
- [ ] 指標運算不超出邊界
- [ ] 動態分配的記憶體正確釋放
- [ ] 沒有野指標（指向已釋放的記憶體）
- [ ] 正確使用 &（取址）和 *（取值）
- [ ] 雙層指標的目的明確
- [ ] 虛擬節點不會被誤 free（確認其分配方式）
