# Stack & Queue 堆疊與佇列模式

## 簡介

堆疊（Stack）和佇列（Queue）是兩種基本的資料結構，分別遵循 LIFO（後進先出）和 FIFO（先進先出）的原則。它們在解決括號匹配、最近元素追蹤、單調性維持等問題中極為重要。更高級的變體如雙端隊列（Deque）和單調堆疊提供了解決複雜區間問題的強大能力。

## 辨識信號

使用堆疊和佇列的典型場景：

- **括號匹配**：驗證括號序列的合法性
- **遞迴轉迭代**：使用堆疊模擬遞迴呼叫棧
- **後進先出處理**：撤銷/重做、expression 計算
- **單調結構**：尋找下一個更大/更小的元素、最大/最小區間值
- **時間窗口**：追蹤最近發生的事件
- **緩衝區**：生產者-消費者模式

## 堆疊（Stack）的變體

### 1. 基本堆疊

```c
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* data;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->data = (int*)malloc(sizeof(int) * capacity);
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

void push(Stack* stack, int value) {
    if (stack->top < stack->capacity - 1) {
        stack->data[++stack->top] = value;
    }
}

int pop(Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return -1;  // 錯誤情況
}

int peek(Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top];
    }
    return -1;
}

bool isEmpty(Stack* stack) {
    return stack->top < 0;
}

void freeStack(Stack* stack) {
    free(stack->data);
    free(stack);
}
```

#### 例子：Valid Parentheses (LeetCode 20)

```c
#include <string.h>
#include <stdbool.h>

bool isValid(char* s) {
    int len = strlen(s);
    Stack* stack = createStack(len);

    for (int i = 0; i < len; i++) {
        char c = s[i];

        if (c == '(' || c == '[' || c == '{') {
            push(stack, c);
        } else {
            if (isEmpty(stack)) {
                freeStack(stack);
                return false;
            }

            char top = (char)pop(stack);
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                freeStack(stack);
                return false;
            }
        }
    }

    bool result = isEmpty(stack);
    freeStack(stack);
    return result;
}
```

---

### 2. 輔助堆疊

用於追蹤額外資訊（如最小值）。

#### 例子：Min Stack (LeetCode 155)

```c
typedef struct {
    int* data;
    int* minStack;
    int top;
    int capacity;
} MinStack;

MinStack* minStackCreate() {
    MinStack* ms = (MinStack*)malloc(sizeof(MinStack));
    ms->capacity = 10000;
    ms->data = (int*)malloc(sizeof(int) * ms->capacity);
    ms->minStack = (int*)malloc(sizeof(int) * ms->capacity);
    ms->top = -1;
    return ms;
}

void minStackPush(MinStack* obj, int val) {
    obj->data[++obj->top] = val;
    if (obj->top == 0) {
        obj->minStack[obj->top] = val;
    } else {
        int currentMin = (obj->minStack[obj->top - 1] < val) ?
                         obj->minStack[obj->top - 1] : val;
        obj->minStack[obj->top] = currentMin;
    }
}

void minStackPop(MinStack* obj) {
    if (obj->top >= 0) {
        obj->top--;
    }
}

int minStackTop(MinStack* obj) {
    if (obj->top >= 0) {
        return obj->data[obj->top];
    }
    return -1;
}

int minStackGetMin(MinStack* obj) {
    if (obj->top >= 0) {
        return obj->minStack[obj->top];
    }
    return -1;
}

void minStackFree(MinStack* obj) {
    free(obj->data);
    free(obj->minStack);
    free(obj);
}
```

---

### 3. 單調堆疊（Monotonic Stack）

維持堆疊內元素的單調遞增或遞減。

#### 模板：單調遞減堆疊（找下一個更大的元素）

```c
typedef struct {
    int index;
    int value;
} StackNode;

int* nextGreaterElement(int* nums, int numsSize, int* returnSize) {
    int* result = (int*)malloc(sizeof(int) * numsSize);
    for (int i = 0; i < numsSize; i++) {
        result[i] = -1;
    }

    StackNode* stack = (StackNode*)malloc(sizeof(StackNode) * numsSize);
    int top = -1;

    for (int i = numsSize - 1; i >= 0; i--) {
        // 移除所有小於等於當前元素的棧頂
        while (top >= 0 && stack[top].value <= nums[i]) {
            top--;
        }

        // 棧頂是下一個更大的元素
        if (top >= 0) {
            result[i] = stack[top].value;
        }

        // 推入當前元素
        stack[++top].index = i;
        stack[top].value = nums[i];
    }

    free(stack);
    *returnSize = numsSize;
    return result;
}
```

#### 例子：Daily Temperatures (LeetCode 739)

```c
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    int* result = (int*)malloc(sizeof(int) * temperaturesSize);
    memset(result, 0, sizeof(int) * temperaturesSize);

    int* stack = (int*)malloc(sizeof(int) * temperaturesSize);
    int top = -1;

    for (int i = temperaturesSize - 1; i >= 0; i--) {
        // 移除所有溫度小於等於當前的棧頂
        while (top >= 0 && temperatures[stack[top]] <= temperatures[i]) {
            top--;
        }

        // 棧頂的索引距離
        if (top >= 0) {
            result[i] = stack[top] - i;
        }

        // 推入當前索引
        stack[++top] = i;
    }

    free(stack);
    *returnSize = temperaturesSize;
    return result;
}
```

---

#### 例子：Largest Rectangle in Histogram (LeetCode 84)

```c
int largestRectangleArea(int* heights, int heightsSize) {
    int maxArea = 0;
    int* stack = (int*)malloc(sizeof(int) * heightsSize);
    int top = -1;

    for (int i = 0; i < heightsSize; i++) {
        // 當前高度小於棧頂時，計算矩形面積
        while (top >= 0 && heights[stack[top]] > heights[i]) {
            int h = heights[stack[top--]];
            int w = (top >= 0) ? (i - stack[top] - 1) : i;
            int area = h * w;
            if (area > maxArea) {
                maxArea = area;
            }
        }

        stack[++top] = i;
    }

    // 處理剩余的棧中元素
    while (top >= 0) {
        int h = heights[stack[top--]];
        int w = (top >= 0) ? (heightsSize - stack[top] - 1) : heightsSize;
        int area = h * w;
        if (area > maxArea) {
            maxArea = area;
        }
    }

    free(stack);
    return maxArea;
}
```

---

## 佇列（Queue）的變體

### 1. 基本佇列

```c
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* data;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->data = (int*)malloc(sizeof(int) * capacity);
    queue->front = 0;
    queue->rear = -1;
    queue->capacity = capacity;
    return queue;
}

void enqueue(Queue* queue, int value) {
    if (queue->rear < queue->capacity - 1) {
        queue->data[++queue->rear] = value;
    }
}

int dequeue(Queue* queue) {
    if (queue->front <= queue->rear) {
        return queue->data[queue->front++];
    }
    return -1;
}

bool isQueueEmpty(Queue* queue) {
    return queue->front > queue->rear;
}

void freeQueue(Queue* queue) {
    free(queue->data);
    free(queue);
}
```

---

### 2. 雙端隊列（Deque）

支持在兩端進行插入和刪除操作。

```c
typedef struct {
    int* data;
    int front;
    int rear;
    int capacity;
    int count;
} Deque;

Deque* createDeque(int capacity) {
    Deque* deque = (Deque*)malloc(sizeof(Deque));
    deque->data = (int*)malloc(sizeof(int) * capacity);
    deque->front = 0;
    deque->rear = capacity - 1;
    deque->capacity = capacity;
    deque->count = 0;
    return deque;
}

void insertFront(Deque* deque, int value) {
    if (deque->count < deque->capacity) {
        deque->front = (deque->front - 1 + deque->capacity) % deque->capacity;
        deque->data[deque->front] = value;
        deque->count++;
    }
}

void insertRear(Deque* deque, int value) {
    if (deque->count < deque->capacity) {
        deque->rear = (deque->rear + 1) % deque->capacity;
        deque->data[deque->rear] = value;
        deque->count++;
    }
}

int deleteFront(Deque* deque) {
    if (deque->count > 0) {
        int value = deque->data[deque->front];
        deque->front = (deque->front + 1) % deque->capacity;
        deque->count--;
        return value;
    }
    return -1;
}

int deleteRear(Deque* deque) {
    if (deque->count > 0) {
        int value = deque->data[deque->rear];
        deque->rear = (deque->rear - 1 + deque->capacity) % deque->capacity;
        deque->count--;
        return value;
    }
    return -1;
}
```

#### 例子：Sliding Window Maximum (LeetCode 239)

```c
int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    int* result = (int*)malloc(sizeof(int) * (numsSize - k + 1));
    *returnSize = 0;

    Deque* deque = createDeque(numsSize);

    for (int i = 0; i < numsSize; i++) {
        // 移除超出窗口的元素
        if (!isDequeEmpty(deque) && deque->front <= i - k) {
            deleteFront(deque);
        }

        // 維持遞減順序：移除所有小於當前元素的元素
        while (!isDequeEmpty(deque) &&
               nums[deque->rear] < nums[i]) {
            deleteRear(deque);
        }

        insertRear(deque, i);

        // 當窗口滿時，記錄最大值
        if (i >= k - 1) {
            result[(*returnSize)++] = nums[deque->front];
        }
    }

    free(deque->data);
    free(deque);
    return result;
}
```

---

### 3. 使用兩個堆疊實現佇列

```c
typedef struct {
    Stack* inStack;
    Stack* outStack;
} Queue_DoubleStack;

Queue_DoubleStack* create() {
    Queue_DoubleStack* queue = (Queue_DoubleStack*)malloc(sizeof(Queue_DoubleStack));
    queue->inStack = createStack(1000);
    queue->outStack = createStack(1000);
    return queue;
}

void push(Queue_DoubleStack* obj, int x) {
    push(obj->inStack, x);
}

int pop(Queue_DoubleStack* obj) {
    if (isEmpty(obj->outStack)) {
        while (!isEmpty(obj->inStack)) {
            push(obj->outStack, pop(obj->inStack));
        }
    }
    return pop(obj->outStack);
}

int peek(Queue_DoubleStack* obj) {
    if (isEmpty(obj->outStack)) {
        while (!isEmpty(obj->inStack)) {
            push(obj->outStack, pop(obj->inStack));
        }
    }
    return peek(obj->outStack);
}

bool empty(Queue_DoubleStack* obj) {
    return isEmpty(obj->inStack) && isEmpty(obj->outStack);
}
```

---

## 常見問題及其解決方案

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 20 | Valid Parentheses | Easy | 堆疊匹配括號 |
| 84 | Largest Rectangle in Histogram | Hard | 單調遞增堆疊 |
| 155 | Min Stack | Medium | 輔助堆疊追蹤最小值 |
| 232 | Implement Queue using Stacks | Easy | 雙堆疊延遲翻轉 |
| 239 | Sliding Window Maximum | Hard | 單調遞減雙端隊列 |
| 739 | Daily Temperatures | Medium | 單調遞減堆疊 |
| 933 | Number of Recent Calls | Easy | 時間窗口佇列 |
| 2390 | Removing Stars From a String | Medium | 堆疊模擬 |

---

## 面試講解

### 30 秒版本

「堆疊和佇列是基本的資料結構。堆疊（LIFO）用於括號匹配和遞迴模擬，單調堆疊解決『下一個更大元素』類問題。佇列（FIFO）用於層序遍歷和時間窗口。雙端隊列結合兩者的優勢，在滑動窗口最大值等問題中高效。」

### 2 分鐘版本

「堆疊和佇列的關鍵在於理解它們的順序性質。

**基本堆疊**最直接的應用是括號匹配。遍歷字符串，開括號進堆，閉括號檢查棧頂是否匹配。時間複雜度 O(n)，空間 O(n)。

**單調堆疊**是一個更巧妙的技巧。要找『下一個更大元素』，我們從右向左遍歷，維持堆疊為遞減序列。當前元素大於棧頂時，棧頂的『下一個更大元素』就是當前元素。這避免了 O(n²) 的巢狀迴圈。在 Largest Rectangle in Histogram 中，我們用相同思想找到每個高度能延伸的最寬範圍。

**輔助堆疊**用於同時追蹤主要資訊和元資料。Min Stack 維持兩個堆疊：一個存值，一個存對應的最小值。查詢最小值變成 O(1) 而非 O(n)。

**雙端隊列**結合堆疊和佇列的靈活性。在 Sliding Window Maximum 中，我們維持一個遞減的索引隊列。隊列前端的索引對應當前窗口的最大值。移除超出窗口的索引，並維持隊列遞減性。

**兩堆疊實現佇列**展示了資料結構的組合設計。輸入堆疊負責收集元素，輸出堆疊負責提供它們。當輸出堆疊為空時，將輸入堆疊翻轉到輸出堆疊，實現『懶翻轉』的延遲評估。」

---

## 常見 Follow-up 問題

1. **「Min Stack 能以 O(1) 空間做到嗎？」**
   - 困難。因為需要在每一層都追蹤最小值。可以用編碼技巧存儲舊最小值，但仍需額外空間。

2. **「為什麼單調堆疊在歷史元素上有效？」**
   - 因為它們已經被『詢問』過了。如果它們沒有找到答案，當前元素才可能是答案。

3. **「Largest Rectangle in Histogram 為什麼不用貪心？」**
   - 貪心選擇最高的柱子不一定給出最大面積。需要考慮寬度權衡。單調堆疊通過詢問『每個高度能延伸多遠』來解決。

4. **「Sliding Window Maximum 中為什麼用索引而不是值？」**
   - 因為需要知道元素是否超出窗口。值本身無法告訴我們位置資訊。

5. **「雙堆疊實現佇列的時間複雜度？」**
   - 平攤 O(1)。每個元素最多被推入和彈出一次，所以總操作是 O(n)。

---

## 和 Kernel 的關聯

在 Linux 核心中，堆疊和佇列有多種應用：

### Kernel Ring Buffer

```c
// Kernel 中的 kfifo 是一個環形緩衝區
struct kfifo {
    unsigned char *buffer;
    unsigned int size;
    unsigned int in;
    unsigned int out;
};

// 寫入
void kfifo_put(struct kfifo *fifo, unsigned char byte) {
    fifo->buffer[fifo->in % fifo->size] = byte;
    fifo->in++;
}

// 讀取
unsigned char kfifo_get(struct kfifo *fifo) {
    unsigned char byte = fifo->buffer[fifo->out % fifo->size];
    fifo->out++;
    return byte;
}
```

### 應用場景

- **中斷處理**：使用佇列存儲待處理的事件，避免棧溢出
- **工作佇列**：內核工作佇列（workqueue）用於延遲工作
- **任務堆疊**：函數呼叫棧追蹤，用於 stack unwinding
- **環形日誌**：內核日誌環形緩衝區使用 FIFO 原則

---

## 常見陷阱

### 1. **堆疊溢出**

```c
// 錯誤：沒有容量檢查
void push(Stack* stack, int value) {
    stack->data[++stack->top] = value;  // 可能超界
}

// 正確
void push(Stack* stack, int value) {
    if (stack->top < stack->capacity - 1) {
        stack->data[++stack->top] = value;
    }
}
```

### 2. **空堆疊/佇列存取**

```c
// 錯誤：沒有檢查空狀態
int value = pop(stack);  // 可能返回垃圾值

// 正確
if (!isEmpty(stack)) {
    int value = pop(stack);
}
```

### 3. **環形陣列索引錯誤**

```c
// 錯誤：直接加減可能超界
deque->rear = deque->rear + 1;

// 正確：使用模運算
deque->rear = (deque->rear + 1) % deque->capacity;
```

### 4. **單調堆疊邏輯反向**

```c
// 錯誤：不清楚是遞增還是遞減
while (top >= 0 && stack[top] > nums[i]) {
    // 應該是什麼操作？不清楚
}

// 正確：明確註釋
while (top >= 0 && stack[top].value < nums[i]) {
    // 維持遞減序列，移除較小的元素
    top--;
}
```

### 5. **忘記初始化**

```c
// 錯誤：佇列結構未初始化
Queue* queue;
enqueue(queue, 5);  // 崩潰！

// 正確
Queue* queue = createQueue(100);
enqueue(queue, 5);
```

---

## 複雜度分析

| 資料結構 | 操作 | 時間複雜度 | 空間複雜度 |
|---------|------|----------|----------|
| Stack | push/pop/peek | O(1) | O(n) |
| Queue | enqueue/dequeue | O(1) | O(n) |
| Deque | insert/delete (兩端) | O(1) | O(n) |
| Monotonic Stack | 維持+操作 | O(n) 平均 | O(n) |

---

## 進階技巧

### 堆疊與 DFS

```c
// 遞迴 DFS
void dfsDFS(TreeNode* root) {
    if (!root) return;
    visit(root);
    dfsRecursive(root->left);
    dfsRecursive(root->right);
}

// 迭代 DFS（使用堆疊）
void dfsIterative(TreeNode* root) {
    Stack* stack = createStack(1000);
    push(stack, root);

    while (!isEmpty(stack)) {
        TreeNode* node = (TreeNode*)pop(stack);
        visit(node);

        if (node->right) push(stack, node->right);
        if (node->left) push(stack, node->left);
    }

    freeStack(stack);
}
```

### 佇列與 BFS

```c
// BFS 使用佇列
void bfs(TreeNode* root) {
    Queue* queue = createQueue(1000);
    enqueue(queue, root);

    while (!isQueueEmpty(queue)) {
        TreeNode* node = (TreeNode*)dequeue(queue);
        visit(node);

        if (node->left) enqueue(queue, node->left);
        if (node->right) enqueue(queue, node->right);
    }

    freeQueue(queue);
}
```

---

## 練習建議

1. 從基本堆疊開始（Valid Parentheses）
2. 學習輔助堆疊（Min Stack）
3. 掌握單調堆疊（Daily Temperatures）
4. 進階到複雜堆疊應用（Largest Rectangle）
5. 學習雙端隊列（Sliding Window Maximum）
6. 結合多種資料結構（LRU Cache）
