# Dynamic Programming 動態規劃模式

## 概述 Overview

動態規劃（DP）是一種通過將問題分解為重疊的子問題並存儲中間結果來解決複雜問題的方法。DP 遵循兩個原則：最優子結構和子問題重疊。本模式涵蓋一維 DP、狀態機 DP 和 Kadane 算法等多種變體。

動態規劃可以是自上而下（記憶化）或自下而上（制錶法）的方式實現。對於簡單問題，常使用滾動數組優化空間。

## 核心概念 Core Concepts

### 1. 最大子陣列和 - Kadane 算法 (Problem 53)

Kadane 算法是解決最大子陣列和的經典貪心 + DP 算法。維護到當前位置為止的最大和。

```c
int maxSubArray(int* nums, int numsSize) {
    int max_current = nums[0];  // 以當前位置結尾的最大和
    int max_global = nums[0];   // 全局最大和

    for (int i = 1; i < numsSize; i++) {
        // 要麼開始新序列，要麼繼續前序列
        max_current = (nums[i] > max_current + nums[i]) ?
                      nums[i] : max_current + nums[i];

        // 更新全局最大值
        if (max_current > max_global) {
            max_global = max_current;
        }
    }

    return max_global;
}
```

**邏輯解釋：**
- max_current 追蹤以當前位置結尾的最大子陣列和
- 在每個位置，選擇是開始新序列（當前元素）還是延續前序列（加上當前元素）
- 選擇哪個產生更大的和

### 2. 狀態機 DP - 股票交易

股票買賣問題可以視為狀態機，狀態是「持有」或「不持有」股票。

**單次交易（Problem 121）：**
```c
int maxProfit(int* prices, int pricesSize) {
    int min_price = prices[0];
    int max_profit = 0;

    for (int i = 1; i < pricesSize; i++) {
        int profit = prices[i] - min_price;
        if (profit > max_profit) {
            max_profit = profit;
        }
        if (prices[i] < min_price) {
            min_price = prices[i];
        }
    }

    return max_profit;
}
```

**無限次交易（Problem 122）：**
峰谷法則：只要相鄰兩天價格上升，就進行交易。

```c
int maxProfit_unlimited(int* prices, int pricesSize) {
    int max_profit = 0;

    for (int i = 1; i < pricesSize; i++) {
        if (prices[i] > prices[i - 1]) {
            max_profit += prices[i] - prices[i - 1];
        }
    }

    return max_profit;
}
```

### 3. 最大乘積子陣列 (Problem 152)

需要追蹤最大和最小乘積（因為負數相乘會改變大小關係）。

```c
int maxProduct(int* nums, int numsSize) {
    if (numsSize == 0) return 0;

    int max_prod = nums[0];  // 以當前位置結尾的最大乘積
    int min_prod = nums[0];  // 以當前位置結尾的最小乘積
    int result = nums[0];

    for (int i = 1; i < numsSize; i++) {
        int temp_max = max_prod;

        // 當前位置最大乘積可能來自：
        // 1. 當前數字本身
        // 2. 最大乘積 * 當前數字
        // 3. 最小乘積 * 當前數字（負數使小變大）
        max_prod = nums[i];
        if (temp_max * nums[i] > max_prod) {
            max_prod = temp_max * nums[i];
        }
        if (min_prod * nums[i] > max_prod) {
            max_prod = min_prod * nums[i];
        }

        // 同樣計算最小乘積
        min_prod = nums[i];
        if (temp_max * nums[i] < min_prod) {
            min_prod = temp_max * nums[i];
        }
        if (min_prod * nums[i] < min_prod) {
            min_prod = min_prod * nums[i];
        }

        // 更新全局最大值
        if (max_prod > result) {
            result = max_prod;
        }
    }

    return result;
}
```

## DP 問題解決步驟 DP Problem-Solving Steps

### 步驟 1：定義狀態
確定 DP 數組的含義。例如：
- dp[i] = 以第 i 個元素結尾的子陣列最大和
- dp[i] = 到第 i 天的最大利潤
- dp[i][j] = 前 i 個物品，容量為 j 的背包最大價值

### 步驟 2：確定轉移方程
找出當前狀態與前面狀態的關係。例如：
- dp[i] = max(nums[i], dp[i-1] + nums[i])
- dp[i] = max(dp[i-1], prices[i] - min_price)

### 步驟 3：初始化基本情況
確定 dp[0] 或 dp[0][0] 等基本情況的值。

### 步驟 4：確定計算順序
確定是自上而下（遞迴 + 記憶化）還是自下而上（迴圈）。

### 步驟 5：優化空間
如果只依賴前幾個狀態，可以使用滾動數組減少空間。

## 常見變體 Common Variants

### 變體 1：1D DP 數組

簡單的一維 DP，狀態只取決於前一個或幾個位置。

```c
// 通用模板
int* dp = (int*)malloc(n * sizeof(int));
dp[0] = base_case;

for (int i = 1; i < n; i++) {
    dp[i] = compute(dp[i-1], dp[i-2], ...);
}

int result = dp[n-1];
free(dp);
return result;
```

### 變體 2：2D DP 數組

適用於兩個維度的問題，如背包問題。

```c
// 背包問題模板
int** dp = (int**)malloc((n + 1) * sizeof(int*));
for (int i = 0; i <= n; i++) {
    dp[i] = (int*)calloc(capacity + 1, sizeof(int));
}

for (int i = 1; i <= n; i++) {
    for (int j = 0; j <= capacity; j++) {
        // 不選物品 i
        dp[i][j] = dp[i-1][j];
        // 選物品 i（如果容量允許）
        if (weight[i-1] <= j) {
            int with_item = dp[i-1][j - weight[i-1]] + value[i-1];
            if (with_item > dp[i][j]) {
                dp[i][j] = with_item;
            }
        }
    }
}
```

### 變體 3：滾動數組優化

對於只依賴前一行或前幾個元素的情況，使用滾動數組節省空間。

```c
// 使用兩個數組替代 2D 數組
int* prev = (int*)calloc(capacity + 1, sizeof(int));
int* curr = (int*)calloc(capacity + 1, sizeof(int));

for (int i = 0; i < n; i++) {
    for (int j = 0; j <= capacity; j++) {
        curr[j] = prev[j];  // 不選
        if (weight[i] <= j) {
            int with_item = prev[j - weight[i]] + value[i];
            if (with_item > curr[j]) {
                curr[j] = with_item;
            }
        }
    }
    // 交換指針
    int* temp = prev;
    prev = curr;
    curr = temp;
}

free(prev);
free(curr);
```

### 變體 4：記憶化搜尋

自上而下的 DP 方法，用遞迴 + 哈希表存儲已計算的結果。

```c
// 記憶化搜尋模板
#define MEMO_SIZE 1000
int memo[MEMO_SIZE];
bool computed[MEMO_SIZE];

int dp_recursive(int state) {
    if (computed[state]) {
        return memo[state];
    }

    int result;
    if (base_case(state)) {
        result = base_value(state);
    } else {
        result = compute(dp_recursive(state1), dp_recursive(state2), ...);
    }

    computed[state] = true;
    memo[state] = result;
    return result;
}
```

## 相關問題 Related Problems

| 問題 ID | 名稱 | 難度 | 描述 |
|---------|------|------|------|
| 53 | Maximum Subarray | Med | Kadane 算法 |
| 122 | Best Time to Buy and Sell Stock II | Med | 峰谷累計 |
| 152 | Maximum Product Subarray | Med | 追蹤最大最小乘積 |

## 內核連接 Kernel Connection

Linux 內核在以下地方使用 DP 或類似思想：

- **PELT 負載追蹤**：調度器使用 exponential moving average 追蹤 CPU 負載
- **連接狀態機**：網絡協議棧使用狀態機管理連接狀態
- **內存頁面回收**：使用貪心策略（類似 DP）決定哪些頁面應該被回收

## 時間和空間複雜度

- **Kadane 算法**：
  - 時間複雜度：O(n)
  - 空間複雜度：O(1)

- **1D DP**：
  - 時間複雜度：O(n)
  - 空間複雜度：O(n)（或 O(1) 如果只需前幾個值）

- **2D DP**：
  - 時間複雜度：O(n * m)
  - 空間複雜度：O(n * m)（或 O(m) 使用滾動數組）

- **背包問題**：
  - 時間複雜度：O(n * W)（n 是物品數，W 是背包容量）
  - 空間複雜度：O(W)（使用滾動數組）

## 除錯技巧 Debugging Tips

1. **驗證基本情況**：確保 dp[0] 或 dp[0][0] 計算正確
2. **手工執行小例子**：在紙上手工計算 DP 數組的演變過程
3. **列印 DP 數組**：在計算過程中列印 DP 數組，查看值是否合理
4. **驗證轉移方程**：確保轉移方程邏輯正確
5. **檢查邊界條件**：確保數組訪問不超出範圍
6. **驗證時間複雜度**：如果超時，考慮是否有更優的轉移方程

## 實踐建議 Practice Tips

1. 從簡單的 1D DP 開始，如 Kadane 算法
2. 理解狀態的含義，這是 DP 的關鍵
3. 先寫出轉移方程，再編寫代碼
4. 練習識別 DP 問題的特徵（最優子結構、子問題重疊）
5. 掌握滾動數組優化，節省空間
6. 對於複雜問題，先用 2D DP，確認邏輯無誤後再優化
7. 學習記憶化搜尋，理解自上而下和自下而上的區別

## 常見錯誤 Common Mistakes

1. **狀態定義不清**：導致轉移方程混亂
2. **初始化錯誤**：基本情況計算不正確
3. **轉移方程錯誤**：邏輯不正確，導致結果錯誤
4. **邊界訪問越界**：數組索引計算不當
5. **空間優化不當**：使用滾動數組時，新舊值混淆
6. **遞迴深度過深**：自上而下方法沒有適當的記憶化，導致重複計算或棧溢位
7. **整數溢位**：在累加或乘法時發生溢位
