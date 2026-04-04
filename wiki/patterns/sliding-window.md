# Sliding Window 滑動窗口模式

## 簡介

滑動窗口是一種針對陣列或字串的子序列問題的優化技巧。透過維護一個「窗口」並根據條件動態調整其大小，我們可以將時間複雜度從 O(n²) 或 O(n³) 減少到 O(n)。這種方法特別適用於「連續子陣列/子字串」問題。

## 辨識信號

使用滑動窗口的典型場景：

- **連續子序列問題**：尋找滿足條件的最短/最長子字串或子陣列
- **視窗大小變化**：最短長度取決於內容，而非固定值
- **頻率計數或累加和**：需要追蹤窗口內的統計資訊
- **字元或元素重複檢測**：尋找無重複字元的最長子字串
- **兩指標配合**：左右指標分別代表窗口的邊界

## 變體分類

### 1. 固定窗口（Fixed-Size Window）

窗口大小固定，只需向右滑動一次。適用於固定長度的子序列問題。

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 219 | Contains Duplicate II | Easy | 固定窗口+HashSet 檢查 |
| 643 | Maximum Average Subarray I | Easy | 固定窗口+累加和 |
| 1456 | Maximum Number of Vowels | Med | 固定窗口+計數 |

#### 模板：固定窗口

```c
#include <string.h>
#include <limits.h>

int maxSumFixedWindow(int* arr, int arrSize, int windowSize) {
    if (arrSize < windowSize) return INT_MIN;

    // 計算初始窗口的和
    int currentSum = 0;
    for (int i = 0; i < windowSize; i++) {
        currentSum += arr[i];
    }
    int maxSum = currentSum;

    // 滑動窗口：移除左端，添加右端
    for (int i = windowSize; i < arrSize; i++) {
        currentSum = currentSum - arr[i - windowSize] + arr[i];
        if (currentSum > maxSum) {
            maxSum = currentSum;
        }
    }

    return maxSum;
}
```

#### 例子：Contains Duplicate II (LeetCode 219)

```c
#include <stdbool.h>

typedef struct {
    int key;
    int value;
} HashEntry;

bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
    if (k >= numsSize) k = numsSize - 1;
    if (k <= 0) return false;

    // 使用簡單的陣列來模擬 HashSet（假設數值在小範圍內）
    int* seen = (int*)malloc(sizeof(int) * (k + 1));
    memset(seen, -1, sizeof(int) * (k + 1));

    for (int i = 0; i < numsSize; i++) {
        // 檢查當前元素是否在窗口內
        int idx = nums[i] % (k + 1);
        if (seen[idx] == nums[i] && i - nums[i] <= k) {
            free(seen);
            return true;
        }
        seen[idx] = nums[i];

        // 移除超出窗口的元素
        if (i >= k) {
            int leftIdx = nums[i - k] % (k + 1);
            seen[leftIdx] = -1;
        }
    }

    free(seen);
    return false;
}
```

---

### 2. 變動窗口（Variable-Size Window）

窗口大小根據條件動態變化。使用左右雙指標，根據條件移動左或右指標。

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 3 | Longest Substring Without Repeating Characters | Medium | 變動窗口+HashSet |
| 76 | Minimum Window Substring | Hard | 變動窗口+頻率計數 |
| 424 | Longest Repeating Character Replacement | Medium | 變動窗口+最大頻率 |
| 567 | Permutation in String | Medium | 固定窗口+頻率匹配 |

#### 模板：變動窗口（擴展型）

```c
#include <string.h>

int lengthOfLongestSubstring(char* s) {
    int maxLen = 0;
    int left = 0;
    int charIndex[256];
    memset(charIndex, -1, sizeof(charIndex));

    for (int right = 0; s[right] != '\0'; right++) {
        unsigned char c = (unsigned char)s[right];

        // 如果字元已在窗口內，移動 left
        if (charIndex[c] >= left) {
            left = charIndex[c] + 1;
        }

        // 更新字元的最新位置
        charIndex[c] = right;

        // 計算當前窗口大小
        int currentLen = right - left + 1;
        if (currentLen > maxLen) {
            maxLen = currentLen;
        }
    }

    return maxLen;
}
```

#### 模板：變動窗口（收縮型）

```c
#include <string.h>

char* minWindow(char* s, char* t) {
    int sLen = strlen(s);
    int tLen = strlen(t);

    // 建立 t 的頻率表
    int tFreq[256] = {0};
    for (int i = 0; i < tLen; i++) {
        tFreq[(unsigned char)t[i]]++;
    }

    int required = 0;
    for (int i = 0; i < 256; i++) {
        if (tFreq[i] > 0) required++;
    }

    int left = 0, minLen = INT_MAX, minStart = 0;
    int windowFreq[256] = {0};
    int formed = 0;  // 窗口內滿足的唯一字元數

    for (int right = 0; right < sLen; right++) {
        unsigned char c = (unsigned char)s[right];
        windowFreq[c]++;

        // 如果當前字元的頻率達到 t 中的要求
        if (tFreq[c] > 0 && windowFreq[c] == tFreq[c]) {
            formed++;
        }

        // 嘗試收縮窗口
        while (left <= right && formed == required) {
            if (right - left + 1 < minLen) {
                minLen = right - left + 1;
                minStart = left;
            }

            unsigned char leftChar = (unsigned char)s[left];
            windowFreq[leftChar]--;
            if (tFreq[leftChar] > 0 && windowFreq[leftChar] < tFreq[leftChar]) {
                formed--;
            }
            left++;
        }
    }

    if (minLen == INT_MAX) {
        char* result = (char*)malloc(1);
        result[0] = '\0';
        return result;
    }

    char* result = (char*)malloc(minLen + 1);
    strncpy(result, s + minStart, minLen);
    result[minLen] = '\0';
    return result;
}
```

---

### 3. 單調結構窗口（Monotonic Window）

結合單調棧或雙端隊列，以在 O(n) 時間內處理區間最大值等問題。

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 239 | Sliding Window Maximum | Hard | 單調遞減雙端佇列 |
| 480 | Sliding Window Median | Hard | 雙堆+延遲刪除 |

#### 模板：單調雙端隊列（最大值）

```c
#include <stdio.h>

typedef struct {
    int* deque;
    int front, rear, size, capacity;
} Deque;

Deque* createDeque(int capacity) {
    Deque* d = (Deque*)malloc(sizeof(Deque));
    d->deque = (int*)malloc(sizeof(int) * capacity);
    d->front = 0;
    d->rear = -1;
    d->size = 0;
    d->capacity = capacity;
    return d;
}

void pushBack(Deque* d, int val, int* arr) {
    while (d->size > 0 && arr[d->deque[d->rear]] <= arr[val]) {
        d->rear--;
        d->size--;
    }
    d->deque[++d->rear] = val;
    d->size++;
}

void popFront(Deque* d, int index) {
    if (d->size > 0 && d->deque[d->front] == index) {
        d->front++;
        d->size--;
    }
}

int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    int* result = (int*)malloc(sizeof(int) * (numsSize - k + 1));
    *returnSize = 0;

    Deque* dq = createDeque(numsSize);

    for (int i = 0; i < numsSize; i++) {
        // 移除超出窗口的元素
        popFront(dq, i - k);

        // 維持單調遞減
        pushBack(dq, i, nums);

        // 當窗口滿時，記錄最大值
        if (i >= k - 1) {
            result[(*returnSize)++] = nums[dq->deque[dq->front]];
        }
    }

    free(dq->deque);
    free(dq);
    return result;
}
```

---

## 面試講解

### 30 秒版本

「滑動窗口是一種對連續子序列問題的優化技巧。透過維護一個視窗並根據條件動態調整，我們可以將巢狀迴圈減少為單次遍歷，複雜度從 O(n²) 降至 O(n)。有固定大小和變動大小兩種主要變體。」

### 2 分鐘版本

「滑動窗口基於一個核心觀察：當我們找到一個滿足條件的視窗時，不必重新檢查所有之前的元素。相反，我們只需調整窗口邊界。

**固定窗口**適用於『找長度為 k 的子陣列』這類問題。我們先計算前 k 個元素，然後每次移動右指標時，同時移除左端的元素。這樣累加和只需一次減法和一次加法，避免了重複計算。

**變動窗口**更複雜，適用於『最長無重複字符子字串』等問題。我們維持一個包含所有滿足條件元素的窗口，當發現違反條件的元素時，收縮左邊界直到恢復條件。這通常涉及雜湊表或頻率陣列來追蹤窗口內的狀態。

關鍵洞察是：每個元素最多被添加和移除一次，所以時間複雜度總是 O(n)。對於空間複雜度，我們通常需要 O(1) 到 O(字母表大小) 的額外空間。

在 Minimum Window Substring 這樣的困難問題中，我們需要精心追蹤『形成』的條件——即窗口何時包含所有必要的字元。這種技巧結合雙指標和頻率追蹤，展示了滑動窗口的強大能力。」

---

## 常見 Follow-up 問題

1. **「如果要找最短的，能改成找最長的嗎？」**
   - 可以，只需改變比較條件和更新邏輯。對於 Minimum Window Substring，改成 Maximum Window 時，收縮條件會反轉。

2. **「為什麼固定窗口只需要一次遍歷？」**
   - 因為窗口大小已知，我們只需「滑動」這個固定大小的框，無需調整其大小。

3. **「Sliding Window Maximum 如何在 O(n) 時間內解決？」**
   - 使用單調遞減的雙端隊列。隊列的前端始終是當前窗口的最大值，移除不在窗口內的元素，並維持隊列的遞減性質。

4. **「如何處理有多個答案的情況？」**
   - 記錄第一次找到的（最短視窗）或根據題目要求選擇。在 LeetCode 76 中，如果有多個最小窗口，返回任意一個。

5. **「變動窗口中，為什麼先添加右端再移動左端？」**
   - 因為我們需要檢查『是否滿足條件』。添加元素後，我們才知道是否違反了條件，進而決定是否需要收縮。

---

## 和 Kernel 的關聯

在 Linux 核心中，滑動窗口的概念應用於網路和 I/O：

- **TCP 滑動窗口**：發送方維持一個視窗，表示可以發送而未被確認的位元組範圍。類似於演算法中的窗口，它根據接收方的 ACK 動態調整。

- **網路流控制**：使用固定大小的視窗來限制未確認封包的數量，與固定窗口演算法類似。

- **環形緩衝區讀寫指標**：生產者-消費者模式中的讀寫指標類似於滑動窗口的左右邊界。

```c
// Kernel-style example: 簡單的環形緩衝區
#define BUF_SIZE 1024
char buffer[BUF_SIZE];
int readPtr = 0, writePtr = 0;

// 寫入：當 writePtr 到達邊界時重繞
void write_to_buffer(char data) {
    buffer[writePtr] = data;
    writePtr = (writePtr + 1) % BUF_SIZE;
}

// 讀取：類似的邊界檢查
char read_from_buffer(void) {
    char data = buffer[readPtr];
    readPtr = (readPtr + 1) % BUF_SIZE;
    return data;
}
```

---

## 常見陷阱

### 1. **邊界條件錯誤**

```c
// 錯誤：忽視包含右端的計算
int windowSize = right - left;  // 應該是 right - left + 1

// 正確
int windowSize = right - left + 1;
```

### 2. **資料結構選擇不當**

```c
// 如果使用陣列模擬 HashSet，可能衝突
int* seen = (int*)malloc(sizeof(int) * 256);
memset(seen, 0, sizeof(int) * 256);
// 無法區分「未見過」和「見過 0 次」

// 更好的做法：使用三態陣列或結構體
typedef struct {
    int present;
    int count;
} HashEntry;
```

### 3. **指標移動順序錯誤**

```c
// 錯誤：先移動 left，再檢查條件
left++;
if (condition) {
    // left 已經移動，條件檢查對象錯誤
}

// 正確：檢查條件再移動
while (condition) {
    left++;
}
```

### 4. **重複計算或遺漏計算**

```c
// 錯誤：每次都重新計算整個窗口的和
int sum = 0;
for (int j = left; j <= right; j++) {
    sum += arr[j];
}

// 正確：維持增量更新
sum = sum - arr[left - 1] + arr[right];
```

### 5. **字元編碼問題**

```c
// 錯誤：假設 char 總是非負
int freq[256];
char c = s[i];
freq[c]++;  // 可能的負索引！

// 正確：轉換為無符號
unsigned char c = (unsigned char)s[i];
freq[c]++;
```

---

## 複雜度分析

| 變體 | 時間複雜度 | 空間複雜度 | 應用場景 |
|------|-----------|-----------|--------|
| 固定窗口 | O(n) | O(1) 或 O(字母表) | 固定長度子陣列問題 |
| 變動窗口（雜湊表） | O(n) | O(字母表) | 子字串模式匹配、無重複字元 |
| 變動窗口（頻率） | O(n) | O(字母表) | 子字串包含特定字元的問題 |
| 單調結構 | O(n) | O(k) | 區間最大值、中位數 |

---

## 進階技巧

### 多指標滑動窗口

某些問題需要多個滑動窗口或多層邏輯：

```c
// 例子：找滿足條件的最小窗口（兩層窗口）
int minLengthForCondition(char* s, char* pattern) {
    int minLen = INT_MAX;
    int left = 0;
    int patternFreq[256] = {0};
    int windowFreq[256] = {0};

    // 建立模式的頻率表
    for (int i = 0; pattern[i] != '\0'; i++) {
        patternFreq[(unsigned char)pattern[i]]++;
    }

    for (int right = 0; s[right] != '\0'; right++) {
        unsigned char c = (unsigned char)s[right];
        windowFreq[c]++;

        // 檢查當前窗口是否滿足條件
        bool valid = true;
        for (int i = 0; i < 256; i++) {
            if (patternFreq[i] > 0 && windowFreq[i] < patternFreq[i]) {
                valid = false;
                break;
            }
        }

        // 嘗試收縮窗口
        while (valid) {
            minLen = (right - left + 1 < minLen) ? (right - left + 1) : minLen;
            unsigned char leftChar = (unsigned char)s[left];
            windowFreq[leftChar]--;
            left++;

            // 重新檢查條件
            valid = true;
            for (int i = 0; i < 256; i++) {
                if (patternFreq[i] > 0 && windowFreq[i] < patternFreq[i]) {
                    valid = false;
                    break;
                }
            }
        }
    }

    return minLen == INT_MAX ? 0 : minLen;
}
```

---

## 練習建議

1. 從固定窗口開始（如 Maximum Average Subarray）
2. 進階到簡單的變動窗口（如 Longest Substring Without Repeating Characters）
3. 學習複雜的變動窗口（如 Minimum Window Substring）
4. 挑戰單調結構問題（如 Sliding Window Maximum）
5. 嘗試結合多層邏輯的進階問題
