# Hash Table 雜湊表模式

## 概述 Overview

雜湊表是一種將鍵映射到值的數據結構，通過雜湊函數實現 O(1) 平均時間的插入、查詢和刪除操作。本模式涵蓋雜湊表的基本操作、衝突解決方法、常見應用（如頻率計數、分組、緩存）以及相關的優化技巧。

雜湊表在 LeetCode 問題中極其常見，尤其是涉及計數、去重和配對的問題。C 語言中需要手動實現或使用適當的數據結構（如陣列、鏈表）來模擬。

## 核心概念 Core Concepts

### 1. 雜湊函數和衝突解決

**簡單雜湊函數（適用於整數範圍有限的情況）：**
```c
#define HASH_SIZE 10007

int hash_function(int key) {
    return key % HASH_SIZE;
}
```

**鏈地址法解決衝突：**
```c
// 雜湊表的節點（鏈表）
typedef struct HashNode {
    int key;
    int value;
    struct HashNode* next;
} HashNode;

// 雜湊表
typedef struct {
    HashNode** buckets;
    int size;
} HashTable;

// 創建雜湊表
HashTable* create_hash_table(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->buckets = (HashNode**)calloc(size, sizeof(HashNode*));
    table->size = size;
    return table;
}

// 插入或更新
void hash_table_put(HashTable* table, int key, int value) {
    int hash = hash_function(key) % table->size;
    HashNode* node = table->buckets[hash];

    // 查找是否已存在
    while (node != NULL) {
        if (node->key == key) {
            node->value = value;
            return;
        }
        node = node->next;
    }

    // 新建節點並插入到鏈表頭
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->key = key;
    new_node->value = value;
    new_node->next = table->buckets[hash];
    table->buckets[hash] = new_node;
}

// 查詢
int hash_table_get(HashTable* table, int key, int default_value) {
    int hash = hash_function(key) % table->size;
    HashNode* node = table->buckets[hash];

    while (node != NULL) {
        if (node->key == key) {
            return node->value;
        }
        node = node->next;
    }

    return default_value;
}

// 檢查鍵是否存在
bool hash_table_contains(HashTable* table, int key) {
    int hash = hash_function(key) % table->size;
    HashNode* node = table->buckets[hash];

    while (node != NULL) {
        if (node->key == key) {
            return true;
        }
        node = node->next;
    }

    return false;
}

// 刪除
void hash_table_remove(HashTable* table, int key) {
    int hash = hash_function(key) % table->size;
    HashNode* node = table->buckets[hash];
    HashNode* prev = NULL;

    while (node != NULL) {
        if (node->key == key) {
            if (prev == NULL) {
                table->buckets[hash] = node->next;
            } else {
                prev->next = node->next;
            }
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

// 釋放雜湊表
void free_hash_table(HashTable* table) {
    for (int i = 0; i < table->size; i++) {
        HashNode* node = table->buckets[i];
        while (node != NULL) {
            HashNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}
```

### 2. 使用陣列作為簡單雜湊表

對於元素範圍有限的問題（如字元），可以用陣列替代雜湊表：

```c
// 用陣列計算字元頻率（ASCII 範圍）
int char_count[256];
memset(char_count, 0, sizeof(char_count));

for (int i = 0; i < strlen(s); i++) {
    char_count[(unsigned char)s[i]]++;
}

// 檢查字元頻率
if (char_count['a'] > 0) {
    printf("'a' 出現了 %d 次\n", char_count['a']);
}
```

### 3. 常用的集合操作

雜湊表可以用來實現集合（檢查成員資格）：

```c
// 使用陣列和布林標記實現集合
#define MAX_SIZE 1000
bool in_set[MAX_SIZE];

void add_to_set(int value) {
    if (value >= 0 && value < MAX_SIZE) {
        in_set[value] = true;
    }
}

bool is_in_set(int value) {
    return value >= 0 && value < MAX_SIZE && in_set[value];
}

void remove_from_set(int value) {
    if (value >= 0 && value < MAX_SIZE) {
        in_set[value] = false;
    }
}
```

## 常見應用 Common Applications

### 應用 1：兩數之和 (Problem 1)

使用雜湊表存儲已見過的數字，實現 O(n) 時間複雜度。

```c
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int* result = (int*)malloc(2 * sizeof(int));
    HashTable* seen = create_hash_table(numsSize);

    for (int i = 0; i < numsSize; i++) {
        int complement = target - nums[i];

        if (hash_table_contains(seen, complement)) {
            result[0] = hash_table_get(seen, complement, -1);
            result[1] = i;
            *returnSize = 2;
            free_hash_table(seen);
            return result;
        }

        hash_table_put(seen, nums[i], i);
    }

    *returnSize = 0;
    free_hash_table(seen);
    return result;
}
```

### 應用 2：字元頻率和滑動窗口 (Problem 3)

計算字元頻率，利用滑動窗口找最長不重複子字符串。

```c
int lengthOfLongestSubstring(char* s) {
    int char_count[256];
    memset(char_count, 0, sizeof(char_count));

    int max_len = 0;
    int left = 0;

    for (int right = 0; s[right] != '\0'; right++) {
        char_count[(unsigned char)s[right]]++;

        // 如果有重複字元，縮小窗口
        while (char_count[(unsigned char)s[right]] > 1) {
            char_count[(unsigned char)s[left]]--;
            left++;
        }

        int current_len = right - left + 1;
        if (current_len > max_len) {
            max_len = current_len;
        }
    }

    return max_len;
}
```

### 應用 3：分組同字母異序詞 (Problem 49)

使用排序後的字符串作為鍵進行分組。

```c
// 比較函數
int compare_strings(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

char*** groupAnagrams(char** strs, int strsSize, int* returnSize,
                     int** returnColumnSizes) {
    // 1. 為每個字符串排序作為鍵
    // 2. 使用雜湊表（或陣列）分組
    // 3. 構建結果

    char*** result = (char***)malloc(strsSize * sizeof(char**));
    *returnColumnSizes = (int*)malloc(strsSize * sizeof(int));
    *returnSize = 0;

    // 簡化實現：使用陣列和排序
    // ...
    return result;
}
```

### 應用 4：LRU 緩存 (Problem 146)

結合雜湊表和雙向鏈表實現 LRU 緩存。

```c
// 雙向鏈表節點
typedef struct LRUNode {
    int key;
    int value;
    struct LRUNode* prev;
    struct LRUNode* next;
} LRUNode;

// LRU 緩存
typedef struct {
    HashTable* cache;           // 鍵 -> 節點的映射
    LRUNode* head;              // 虛擬頭節點
    LRUNode* tail;              // 虛擬尾節點
    int capacity;
    int size;
} LRUCache;

// 創建 LRU 緩存
LRUCache* lRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->cache = create_hash_table(capacity);
    cache->capacity = capacity;
    cache->size = 0;

    // 虛擬頭尾節點
    cache->head = (LRUNode*)malloc(sizeof(LRUNode));
    cache->tail = (LRUNode*)malloc(sizeof(LRUNode));
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;

    return cache;
}

// 從雙向鏈表移除節點
void remove_node(LRUNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// 將節點移至鏈表頭（最近使用）
void add_to_head(LRUCache* cache, LRUNode* node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

// 獲取值
int lRUCacheGet(LRUCache* cache, int key) {
    // 如果存在，移至頭部並返回值
    // 否則返回 -1
    return -1;  // 簡化
}

// 放入鍵值對
void lRUCachePut(LRUCache* cache, int key, int value) {
    // 如果鍵已存在，更新值並移至頭部
    // 如果鍵不存在且未滿，插入
    // 如果鍵不存在且已滿，刪除尾部節點，插入新節點
}
```

### 應用 5：包含重複 (Problem 217, 219)

簡單的成員資格檢查。

```c
// Problem 217: 包含重複
bool containsDuplicate(int* nums, int numsSize) {
    if (numsSize <= 1) return false;

    // 排序方式：O(n log n) 時間，O(1) 額外空間
    qsort(nums, numsSize, sizeof(int),
          (int (*)(const void*, const void*))compare);

    for (int i = 1; i < numsSize; i++) {
        if (nums[i] == nums[i - 1]) {
            return true;
        }
    }
    return false;
}

// Problem 219: 包含重複 II（檢查 k 距離內的重複）
bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    // 使用滑動窗口和集合
    #define MAX_K 10000
    bool in_window[MAX_K + 1];
    memset(in_window, false, sizeof(in_window));

    for (int i = 0; i < numsSize; i++) {
        if (i > k) {
            in_window[nums[i - k - 1] % (k + 1)] = false;
        }

        int hash = nums[i] % (k + 1);
        if (in_window[hash]) {
            return true;
        }
        in_window[hash] = true;
    }
    return false;
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 1 | Two Sum | Easy | 互補查找 |
| 3 | Longest Substring Without Repeating Characters | Med | 字元頻率陣列 |
| 49 | Group Anagrams | Med | 排序鍵分組 |
| 133 | Clone Graph | Med | 地址映射 |
| 146 | LRU Cache | Med | Hash Map + 雙向鏈結串列 |
| 217 | Contains Duplicate | Easy | 排序+相鄰檢查 |
| 219 | Contains Duplicate II | Easy | 滑動窗口集合 |
| 242 | Valid Anagram | Easy | 字元頻率計數 |
| 347 | Top K Frequent Elements | Med | 頻率計數+桶排序 |
| 692 | Top K Frequent Words | Med | 頻率計數+堆積 |
| 2215 | Find the Difference of Two Arrays | Easy | 布林陣列集合 |

## 內核連接 Kernel Connection

Linux 內核在以下地方使用雜湊表：

- **hlist**：內核的內部哈希鏈表實現，用於各種緩存和查找
- **hashtable.h**：通用的雜湊表實現
- **GOLDEN_RATIO 雜湊**：內核使用黃金比例進行雜湊計算
- **rhashtable**：可調整大小的雜湊表，支持動態擴展
- **進程調度**：使用雜湊表管理進程描述符
- **網絡協議棧**：使用雜湊表存儲連接狀態

## 時間和空間複雜度

- **插入/查詢/刪除**：
  - 平均情況：O(1)
  - 最壞情況：O(n)（所有元素雜湊到同一桶）

- **雙向鏈表 + 雜湊表**（如 LRU）：
  - 所有操作：O(1)

- **頻率計數 + 排序/桶排序**：
  - 時間複雜度：O(n) 到 O(n log n)
  - 空間複雜度：O(n)

## 除錯技巧 Debugging Tips

1. **驗證雜湊函數**：確保分佈均勻，減少衝突
2. **列印雜湊表狀態**：調試時列印所有鏈表內容
3. **測試邊界情況**：空表、單元素、滿表等
4. **檢查記憶體洩漏**：確保正確釋放所有節點
5. **驗證操作順序**：特別是在 LRU 中，添加和刪除的順序很重要
6. **測試衝突情況**：製造多個元素雜湊到同一桶

## 實踐建議 Practice Tips

1. 理解雜湊表的基本操作（插入、查詢、刪除）
2. 掌握使用陣列作為簡單雜湊表的技巧
3. 練習字元頻率計數的常見模式
4. 理解滑動窗口結合雜湊表的應用
5. 學習 LRU 緩存的雙向鏈表實現
6. 認識何時應該使用雜湊表而不是排序
7. 理解時空權衡（雜湊表 O(n) 空間 vs 排序 O(1) 空間）

## 常見錯誤 Common Mistakes

1. **雜湊函數不當**：導致過多衝突或索引溢位
2. **初始化不完整**：未正確初始化陣列或雜湊表
3. **邊界檢查不足**：訪問超出範圍的索引
4. **忘記更新而不是插入**：在 put 時應該檢查鍵是否已存在
5. **記憶體洩漏**：未正確釋放衝突鏈表中的節點
6. **LRU 中的順序錯誤**：添加/刪除和移至頭部的順序不對
7. **字元編碼問題**：在使用字元作為索引時沒有考慮 unsigned char
