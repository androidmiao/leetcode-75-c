# Macro Tricks C 巨集習慣語

## 核心概念

C 巨集是文本替換機制，在編譯前由預處理器展開。正確使用可提高代碼可讀性，但不當使用會導致難以調試的問題。

**何時使用：**
- 定義常數（#define）
- 簡化重複代碼
- 實現泛型（多型）
- 容器操作（offset_of, container_of）

---

## 核心模式

### 1. MIN/MAX 巨集

```c
// 基礎版本（有陷阱）
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// 更安全的版本（處理重複計算）
#define MIN(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    (_a < _b) ? _a : _b; \
})

#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    (_a > _b) ? _a : _b; \
})

// C 標準版本（使用陳述式表達式）
#define MIN(a, b) \
    ({ \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b; \
    })

// 使用示例
int a = 5, b = 3;
int smaller = MIN(a, b);  // 3
int larger = MAX(a++, b++);  // 注意：a 和 b 被計算兩次（基礎版本）
```

**危險示例：**
```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 1, y = 2;
int z = MAX(x++, y++);
// 展開為：((x++) > (y++) ? (x++) : (y++))
// x++ 或 y++ 可能被計算多次！
```

---

### 2. SWAP 巨集

```c
// 交換整數
#define SWAP_INT(a, b) do { \
    int temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

// 通用交換（要求相同型別）
#define SWAP(a, b, type) do { \
    type temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

// 泛型交換（GCC 擴展）
#define SWAP_GENERIC(a, b) do { \
    __typeof__(a) temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

// 使用
int x = 10, y = 20;
SWAP(x, y, int);  // x = 20, y = 10

char c1 = 'a', c2 = 'b';
SWAP(c1, c2, char);  // c1 = 'b', c2 = 'a'
```

**為何用 do { ... } while(0)：**
```c
// 不安全的版本
#define SWAP(a, b, type) { \
    type temp = (a); \
    (a) = (b); \
    (b) = temp; \
}

// 問題：
if (condition)
    SWAP(a, b, int);  // 展開後變成多個陳述句，if 只作用於第一個
else
    doSomething();

// do { ... } while(0) 版本確保總是一個陳述句單元
```

---

### 3. Container_of（容器結構）

```c
// 自訂的 offsetof（標準庫有 offsetof，但理解原理很重要）
#define offset_of(type, member) \
    ((size_t) & ((type *)0)->member)

// container_of：從成員指標獲取容器指標
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offset_of(type, member)))

// 典型用途：侵入式列表
typedef struct {
    int val;
    struct Node *next;
} Node;

typedef struct {
    int data;
    Node node;  // 嵌入的 Node
} Container;

// 給定 Node 指標，獲取 Container 指標
Node *nodePtr = ...;
Container *cont = container_of(nodePtr, Container, node);
int data = cont->data;
```

**原理解釋：**
```c
// offset_of(Container, node) 返回 node 在 Container 中的位元組偏移
// container_of 通過減去這個偏移從成員指標得到容器指標

// 示例：若 Container 記憶體佈局為
// [Container: int data(4B) | Node node(8B)]
//               ^            ^
//            offset 0      offset 4

// 若 nodePtr 指向 node，則容器指標 = nodePtr - 4
```

---

### 4. 陣列長度巨集（Array Length Macro）

```c
// 獲取棧分配陣列的元素個數
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 使用
int arr[] = {1, 2, 3, 4, 5};
int len = ARRAY_SIZE(arr);  // 5

// 陷阱：不能用於指標參數
void process(int arr[]) {
    int len = ARRAY_SIZE(arr);  // 危險！arr 衰減為指標，sizeof(arr) = sizeof(int*)
}

// 安全做法：顯式傳遞大小
void process(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        // ...
    }
}
```

---

### 5. 雜湊表大小定義（Hash Size Definition）

```c
// 定義預設雜湊表大小
#define DEFAULT_HASH_SIZE 1024
#define HASH_MASK (DEFAULT_HASH_SIZE - 1)  // 2 的冪次，用於取模

// 快速模運算
#define HASH_INDEX(key) ((key) & HASH_MASK)  // 等同 (key) % DEFAULT_HASH_SIZE

// 典型應用
typedef struct {
    char *key;
    int val;
} Entry;

typedef struct {
    Entry *table[DEFAULT_HASH_SIZE];
} HashTable;

int hash_func(const char *key) {
    int hash = 0;
    for (const char *p = key; *p; p++) {
        hash = (hash << 5) + hash + *p;  // hash * 33 + c
    }
    return HASH_INDEX(hash);
}
```

---

### 6. 強制型別檢查（Type Safety）

```c
// 編譯期型別檢查巨集（GCC 擴展）
#define ENSURE_TYPE(var, type) \
    do { \
        if (0) { \
            type _dummy __attribute__((unused)) = (var); \
        } \
    } while(0)

// 使用
int x = 5;
ENSURE_TYPE(x, int);  // 通過
ENSURE_TYPE(x, float);  // 編譯警告

// 或使用 _Generic（C11）進行多態分發
#define ABS(x) _Generic((x), \
    int: abs, \
    long: labs, \
    double: fabs \
)(x)
```

---

## 常見陷阱

### 1. 參數未加括號

```c
// 危險
#define SQUARE(x) x * x

int result = SQUARE(2 + 3);  // (2 + 3 * 2 + 3) = 11，而非 25

// 安全
#define SQUARE(x) ((x) * (x))

int result = SQUARE(2 + 3);  // (2 + 3) * (2 + 3) = 25
```

### 2. 副作用重複執行

```c
// 危險
#define DOUBLE(x) ((x) + (x))

int a = 5;
int result = DOUBLE(a++);  // a++ 被計算兩次，a 變成 7，result = 12
// 應該是 result = 10

// 解決：使用臨時變數
#define DOUBLE(x) ({ \
    __typeof__(x) _x = (x); \
    (_x) + (_x); \
})
```

### 3. 返回值問題

```c
// 危險
#define GET_VALUE(ptr) ptr->value

if (!GET_VALUE(myPtr)) {  // 若 value 是指標，行為未定義
    // ...
}

// 應該明確轉換型別或使用安全的寫法
#define GET_VALUE(ptr) (ptr)->value
```

---

## 調試巨集

### 查看展開結果

```bash
# GCC 預處理器輸出
gcc -E -P myfile.c | head -50

# 或使用調試器逐步執行
```

### 巨集追蹤

```c
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

// 使用
DEBUG_PRINT("Value: %d\n", x);
```

---

## 最佳實踐

1. **參數總是加括號**
   ```c
   #define SAFE(x) ((x))
   #define UNSAFE(x) x
   ```

2. **多行巨集用 do { ... } while(0)**
   ```c
   #define INIT(x) do { \
       setup(x); \
       configure(x); \
   } while(0)
   ```

3. **避免巨集內的副作用**
   ```c
   // 不好
   #define PROCESS(x) (func1(x), func2(x), func3(x))

   // 好
   void process(int x) {
       func1(x);
       func2(x);
       func3(x);
   }
   ```

4. **使用 #undef 限制作用域**
   ```c
   #define TEMP_MACRO 1
   // ... 使用 ...
   #undef TEMP_MACRO
   ```

5. **用常數函數替代簡單巨集**
   ```c
   // 舊方法
   #define MAX_SIZE 1024

   // 現代方法
   static const int MAX_SIZE = 1024;
   // 或（C99）
   const int MAX_SIZE = 1024;
   ```

---

## 面試要點

1. **解釋巨集的預處理時機和代價**
2. **討論 do { ... } while(0) 的用途**
3. **展示參數括號的重要性**
4. **比較巨集與內聯函數的優缺點**
5. **討論型別安全和調試困難**

## 實現檢查清單

- [ ] 巨集參數加括號
- [ ] 多行巨集用 do { ... } while(0)
- [ ] 沒有隱含的副作用
- [ ] 返回值清楚定義
- [ ] 命名規範（大寫）
- [ ] 文檔化巨集的目的
- [ ] 在 IDE 中能正確展開
- [ ] 測試邊界情況
