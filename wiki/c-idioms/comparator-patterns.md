# Comparator Patterns 比較函數習慣語

## 核心概念

比較函數是排序、查詢、優先佇列等演算法的核心。在 C 中，通過函數指標傳遞自訂比較邏輯。

**標準簽名：**
```c
int compare(const void *a, const void *b);
// 返回：< 0 (a < b), 0 (a == b), > 0 (a > b)
```

---

## 核心模式

### 1. qsort 比較函數（qsort Comparator）

```c
#include <stdlib.h>

// 場景 1：整數升序
int compareAscending(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

// 場景 2：整數降序
int compareDescending(const void *a, const void *b) {
    return *(int *)b - *(int *)a;
}

// 場景 3：浮點數（需特殊處理避免下溢）
int compareFloat(const void *a, const void *b) {
    double diff = *(double *)a - *(double *)b;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

// 場景 4：字串字典序
int compareString(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// 使用
int main(void) {
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
    int size = sizeof(arr) / sizeof(arr[0]);

    qsort(arr, size, sizeof(int), compareAscending);
    // arr 現在是 {1, 1, 2, 3, 4, 5, 6, 9}

    return 0;
}
```

---

### 2. 結構體排序（Sorting Structures）

```c
typedef struct {
    char name[50];
    int age;
    double score;
} Student;

// 按年齡升序
int compareByAge(const void *a, const void *b) {
    Student *s1 = (Student *)a;
    Student *s2 = (Student *)b;
    return s1->age - s2->age;
}

// 按分數降序
int compareByScore(const void *a, const void *b) {
    Student *s1 = (Student *)a;
    Student *s2 = (Student *)b;
    double diff = s2->score - s1->score;  // 降序
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

// 按名字字典序，次按年齡
int compareByNameThenAge(const void *a, const void *b) {
    Student *s1 = (Student *)a;
    Student *s2 = (Student *)b;

    int nameComp = strcmp(s1->name, s2->name);
    if (nameComp != 0) return nameComp;

    // 名字相同，按年齡排
    return s1->age - s2->age;
}

// 使用
int main(void) {
    Student students[] = {
        {"Alice", 20, 85.5},
        {"Bob", 19, 92.3},
        {"Alice", 21, 88.0}
    };
    int size = sizeof(students) / sizeof(students[0]);

    qsort(students, size, sizeof(Student), compareByNameThenAge);

    for (int i = 0; i < size; i++) {
        printf("%s, %d, %.1f\n", students[i].name, students[i].age, students[i].score);
    }
    // 輸出：
    // Alice, 20, 85.5
    // Alice, 21, 88.0
    // Bob, 19, 92.3

    return 0;
}
```

---

### 3. 指標陣列排序（Sorting Pointers）

```c
// 字串陣列排序（指標的指標）
int compareStrings(const void *a, const void *b) {
    // a 和 b 是 char* 的指標，即 char**
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(void) {
    const char *words[] = {"zebra", "apple", "banana", "cherry"};
    int count = sizeof(words) / sizeof(words[0]);

    qsort(words, count, sizeof(const char *), compareStrings);

    for (int i = 0; i < count; i++) {
        printf("%s\n", words[i]);
    }
    // 輸出：apple, banana, cherry, zebra

    return 0;
}
```

---

### 4. 堆比較函數（Heap Comparator）

```c
// 最大堆：父節點 >= 子節點
int compareMaxHeap(const void *a, const void *b) {
    return *(int *)b - *(int *)a;  // 降序 = 最大堆
}

// 最小堆：父節點 <= 子節點
int compareMinHeap(const void *a, const void *b) {
    return *(int *)a - *(int *)b;  // 升序 = 最小堆
}

// 自訂堆（按優先級，優先級高的在前）
typedef struct {
    int val;
    int priority;
} Item;

int comparePriority(const void *a, const void *b) {
    Item *x = (Item *)a;
    Item *y = (Item *)b;
    // 優先級高的排在前面
    return y->priority - x->priority;
}
```

---

### 5. 詞法比較（Lexicographic Comparison）

```c
// 數字字串比較：將 "3" 看作數字而非字符
int compareNumerically(const void *a, const void *b) {
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;

    int len1 = strlen(s1);
    int len2 = strlen(s2);

    // 先按長度比較
    if (len1 != len2) {
        return len1 - len2;
    }

    // 再按字典序
    return strcmp(s1, s2);
}

// 版本號比較："1.10" > "1.9"
int compareVersions(const void *a, const void *b) {
    const char *v1 = *(const char **)a;
    const char *v2 = *(const char **)b;

    int major1, minor1, major2, minor2;
    sscanf(v1, "%d.%d", &major1, &minor1);
    sscanf(v2, "%d.%d", &major2, &minor2);

    if (major1 != major2) return major1 - major2;
    return minor1 - minor2;
}
```

---

## 常見陷阱

### 1. 整數溢出

```c
// 危險：可能溢出
int compare(const void *a, const void *b) {
    return *(int *)a - *(int *)b;  // 若 a=INT_MAX, b=INT_MIN，溢出
}

// 安全
int compare(const void *a, const void *b) {
    int x = *(int *)a;
    int y = *(int *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}
```

### 2. 浮點比較

```c
// 不要直接相減
int compareFloat(const void *a, const void *b) {
    return *(float *)a - *(float *)b;  // 精度問題
}

// 應該分情況判斷
int compareFloat(const void *a, const void *b) {
    double diff = *(double *)a - *(double *)b;
    const double EPSILON = 1e-9;
    if (diff < -EPSILON) return -1;
    if (diff > EPSILON) return 1;
    return 0;
}
```

### 3. 型別轉換錯誤

```c
// 錯誤：直接轉換整數指標
int compare(const void *a, const void *b) {
    return (int)a - (int)b;  // 比較指標地址，不是值！
}

// 正確
int compare(const void *a, const void *b) {
    return *(int *)a - *(int *)b;  // 先取值再比較
}
```

---

## 比較函數設計原則

### 1. 確保一致性

```c
// 不一致的比較會導致 qsort 結果未定義
int badCompare(const void *a, const void *b) {
    static int call_count = 0;
    call_count++;
    // 返回值随機依賴外部狀態 — 危險！
    return call_count % 2;
}
```

### 2. 滿足傳遞性

```c
// 若 a < b 且 b < c，則 a < c
// 違反會導致 qsort 效率低下或結果不符預期
```

### 3. 自反性

```c
// a 與自己的比較應返回 0
int compare(const void *a, const void *b) {
    if (a == b) return 0;  // 同一物件
    // ...
}
```

---

## 複雜排序場景

### 按多個鍵排序

```c
typedef struct {
    char category[20];
    int priority;
    char name[50];
} Task;

int compareTask(const void *a, const void *b) {
    Task *t1 = (Task *)a;
    Task *t2 = (Task *)b;

    // 優先按分類
    int catComp = strcmp(t1->category, t2->category);
    if (catComp != 0) return catComp;

    // 再按優先級（降序）
    int priorityComp = t2->priority - t1->priority;
    if (priorityComp != 0) return priorityComp;

    // 最後按名字
    return strcmp(t1->name, t2->name);
}
```

---

## 面試要點

1. **清晰解釋比較函數的三種返回值**
2. **討論整數溢出和浮點精度的問題**
3. **展示多鍵排序的實現**
4. **比較 qsort vs. 其他排序方法**
5. **時間複雜度分析（通常 O(n log n)）**

## 實現檢查清單

- [ ] 比較函數簽名正確（返回 int，參數為 const void*）
- [ ] 返回值符合約定（負/零/正）
- [ ] 沒有整數溢出
- [ ] 浮點比較使用精度範圍
- [ ] 指標轉換正確（*(type *)a）
- [ ] 比較邏輯一致且傳遞
- [ ] 測試相等、昇序、降序情況
- [ ] 處理重複元素
