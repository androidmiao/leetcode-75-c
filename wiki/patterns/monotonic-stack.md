# Monotonic Stack 單調堆疊模式

## 核心概念

單調堆疊是一種特殊堆疊，其中元素**始終保持遞增或遞減的有序性**。

**何時使用：**
- 找**下一個更大/更小的元素**
- 計算**矩形面積問題**
- 移除**重複字元並保持字典序**最小

## 核心模式

### 1. 單調遞減堆疊 + 下一個更大元素（Monotonic Decreasing + Next Greater Element）

```c
// 739. Daily Temperatures (Medium)
// 找每天後面第一個更暖的天
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    int *result = malloc(temperaturesSize * sizeof(int));
    for (int i = 0; i < temperaturesSize; i++) {
        result[i] = 0;  // 初始化
    }

    int *stack = malloc(temperaturesSize * sizeof(int));  // 存儲索引
    int stackTop = 0;

    for (int i = 0; i < temperaturesSize; i++) {
        // 當前溫度高於棧頂的溫度時，棧頂找到答案
        while (stackTop > 0 && temperatures[i] > temperatures[stack[stackTop - 1]]) {
            int prevIdx = stack[stackTop - 1];
            stackTop--;
            result[prevIdx] = i - prevIdx;  // 計算天數差
        }
        stack[stackTop++] = i;  // 當前索引入棧
    }

    free(stack);
    *returnSize = temperaturesSize;
    return result;
}
```

**複雜度：** O(n) 時間，O(n) 空間（堆疊最多 n 個元素）
**模式特點：** 堆疊存儲索引（不是值），便於計算距離

---

### 2. 單調遞減雙端佇列（Monotonic Deque for Sliding Window Maximum）

```c
// 239. Sliding Window Maximum (Hard)
// 找每個滑動窗口的最大值
int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    int *result = malloc((numsSize - k + 1) * sizeof(int));
    int resultIdx = 0;

    // 雙端佇列存儲索引
    int *deque = malloc(numsSize * sizeof(int));
    int front = 0, rear = 0;

    for (int i = 0; i < numsSize; i++) {
        // 移除超出窗口的元素
        while (front < rear && deque[front] < i - k + 1) {
            front++;
        }

        // 移除比當前元素小的元素（保持遞減）
        while (front < rear && nums[deque[rear - 1]] < nums[i]) {
            rear--;
        }

        deque[rear++] = i;  // 當前索引入隊

        // 窗口已滿，記錄最大值
        if (i >= k - 1) {
            result[resultIdx++] = nums[deque[front]];
        }
    }

    free(deque);
    *returnSize = resultIdx;
    return result;
}
```

**複雜度：** O(n) 時間，O(min(n, k)) 空間
**關鍵點：** 雙端佇列允許從兩端移除元素，而堆疊不行

---

### 3. 單調遞增堆疊 + 面積計算（Monotonic Increasing + Area Calculation）

```c
// 84. Largest Rectangle in Histogram (Hard)
// 找柱狀圖中最大的矩形面積
int largestRectangleArea(int* heights, int heightsSize) {
    int *stack = malloc((heightsSize + 1) * sizeof(int));
    int stackTop = 0;
    int maxArea = 0;

    stack[stackTop++] = 0;  // 哨兵：索引 0

    for (int i = 1; i <= heightsSize; i++) {
        int h = (i < heightsSize) ? heights[i] : 0;  // 最後添加哨兵 0

        // 當遇到更矮的柱子時，計算出棧元素的面積
        while (stackTop > 0 && (i < heightsSize ? heights[stack[stackTop - 1]] : 0) > h) {
            int idx = stack[--stackTop];
            int height = heights[idx];
            // 寬度 = 當前位置 - 新棧頂位置 - 1
            int width = i - (stackTop > 0 ? stack[stackTop - 1] : 0) - 1;
            int area = height * width;
            maxArea = (area > maxArea) ? area : maxArea;
        }

        if (i < heightsSize) {
            stack[stackTop++] = i;  // 索引入棧
        }
    }

    free(stack);
    return maxArea;
}
```

**複雜度：** O(n) 時間，O(n) 空間
**難點：** 計算寬度時需要考慮棧的狀態

---

## 單調堆疊的構建步驟

### 遞減堆疊（尋找 Next Greater Element）

1. 為每個新元素 x：
   - **While** 堆疊不空 **And** 棧頂 < x
     - 棧頂找到答案：x（下一個更大的）
     - 出棧
   - x 入棧

2. 出棧後無答案的元素 → -1 或 0

### 遞增堆疊（矩形面積）

1. 維持堆疊遞增
2. 遇到更小元素時，出棧並計算寬度
3. 使用哨兵避免最後一個元素遺漏

---

## 常用場景對應表

| 問題 | 堆疊類型 | 出棧時機 | 記錄信息 |
|------|--------|--------|---------|
| Next Greater | 遞減 | 遇到更大 | 當前元素 |
| Next Smaller | 遞增 | 遇到更小 | 當前元素 |
| 矩形面積 | 遞增 | 遇到更小 | 寬度和高度 |
| Sliding Max | 雙端+遞減 | 出界/遇到更大 | 佇列頂 |

---

## 實現技巧

### 使用哨兵簡化邊界

```c
// 方法 1：添加哨兵元素
stack[stackTop++] = 0;  // 起始哨兵
// ... 主循環 ...
// 最後添加哨兵 INT_MIN，確保所有元素出棧
```

### 避免重複計算

```c
// 在 while 迴圈中計算面積，而不是在 if 中
while (...) {
    // 計算並記錄面積
    // 只在需要時更新結果
}
```

---

## 常見陷阱

1. **忘記哨兵**：最後的元素可能無法正確出棧
2. **寬度計算錯誤**：需要理解「有效範圍」的邏輯
3. **棧為空時的訪問**：always check `stackTop > 0`
4. **索引 vs. 值的混淆**：確認堆疊存的是索引還是值

---

## 相關 LeetCode 題目

| # | 題名 | 難度 | 堆疊應用 |
|---|------|------|----------|
| 84 | Largest Rectangle in Histogram | Hard | 遞增堆疊+面積計算 |
| 239 | Sliding Window Maximum | Hard | 遞減雙端佇列 |
| 739 | Daily Temperatures | Med | 遞減堆疊+下一個更大 |

---

## 面試要點

1. **清晰解釋堆疊的序列性質**：為何遞減或遞增
2. **哨兵的作用**：簡化邊界條件
3. **時間複雜度分析**：每個元素 push/pop 各最多一次 = O(n)
4. **與其他結構對比**：為何不用優先佇列或二叉搜尋樹
5. **實現細節**：索引 vs. 值，寬度計算公式
