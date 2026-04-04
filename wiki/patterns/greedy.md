# Greedy 貪心演算法模式

## 核心概念

貪心演算法在每一步都做出**局部最優選擇**，希望最終能得到全局最優解。與動態規劃不同，貪心不會回溯或考慮所有可能。

**何時使用：**
- 問題具有「貪心選擇性質」：局部最優選擇能保證全局最優
- 問題具有「最優子結構」：子問題的最優解組合成大問題的最優解
- 通常用於分割、選擇、排序等問題

## 核心模式

### 1. 短邊收縮（Two Pointers with Greedy Shrinking）

```c
// 11. Container With Most Water
// 維護最大面積，在較短邊處收縮
int maxArea(int* height, int heightSize) {
    int left = 0, right = heightSize - 1;
    int maxA = 0;

    while (left < right) {
        int h = (height[left] < height[right]) ? height[left] : height[right];
        int area = h * (right - left);
        maxA = (area > maxA) ? area : maxA;

        // 貪心：總是在較短的邊移動指針
        // 因為寬度已經減少了，只有升高短邊才能得到更大面積
        if (height[left] < height[right]) {
            left++;
        } else {
            right--;
        }
    }
    return maxA;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**貪心選擇：** 在較短邊處移動 → 縮小寬度的同時尋求高度補償

---

### 2. 追蹤最小值（Track Minimum for Best Profit）

```c
// 121. Best Time to Buy and Sell Stock (Easy)
// 追蹤遇過的最低價格，每天計算 profit
int maxProfit(int* prices, int pricesSize) {
    if (pricesSize < 2) return 0;

    int minPrice = prices[0];
    int maxProfit = 0;

    for (int i = 1; i < pricesSize; i++) {
        int profit = prices[i] - minPrice;
        maxProfit = (profit > maxProfit) ? profit : maxProfit;
        minPrice = (prices[i] < minPrice) ? prices[i] : minPrice;
    }
    return maxProfit;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**貪心選擇：** 每次都從遇到的最小價買入，記錄最大利潤

---

### 3. 累計所有正差（Accumulate All Positive Deltas）

```c
// 122. Best Time to Buy and Sell Stock II (Medium)
// 可多次買賣，累計所有上升差
int maxProfit(int* prices, int pricesSize) {
    if (pricesSize < 2) return 0;

    int profit = 0;
    for (int i = 1; i < pricesSize; i++) {
        int delta = prices[i] - prices[i-1];
        if (delta > 0) {
            profit += delta;  // 貪心：累計所有正差
        }
    }
    return profit;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**貪心選擇：** 每次只要有上升就買賣，效果等於一次大的上升

---

### 4. 維護最小和次小（Track Two Minimums）

```c
// 334. Increasing Triplet Subsequence (Medium)
// 維護最小和次小的兩個值，找是否存在第三個更大的值
bool increasingTriplet(int* nums, int numsSize) {
    if (numsSize < 3) return false;

    int first = INT_MAX;   // 最小值
    int second = INT_MAX;  // 次小值

    for (int i = 0; i < numsSize; i++) {
        if (nums[i] <= first) {
            first = nums[i];  // 更新最小值
        } else if (nums[i] <= second) {
            second = nums[i];  // 更新次小值
        } else {
            return true;  // 找到更大的，形成遞增三元組
        }
    }
    return false;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**貪心選擇：** 維護最小和次小，確保能在後續找到最大的三元組

---

### 5. 貪心局部決策（Greedy Local Decisions）

```c
// 605. Can Place Flowers (Easy)
// 花園問題：貪心地放置每一朵花（如果可以的話）
bool canPlaceFlowers(int* flowerbed, int flowerbedSize, int n) {
    int count = 0;
    for (int i = 0; i < flowerbedSize; i++) {
        if (flowerbed[i] == 0) {
            // 檢查左右是否都為空
            bool emptyLeft = (i == 0) || (flowerbed[i-1] == 0);
            bool emptyRight = (i == flowerbedSize - 1) || (flowerbed[i+1] == 0);

            if (emptyLeft && emptyRight) {
                flowerbed[i] = 1;  // 貪心：只要可以就放
                count++;
            }
        }
    }
    return count >= n;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**貪心選擇：** 一旦可以放花就立即放置，不會考慮先放置其他位置

---

### 6. GCD 除數驗證（GCD Divisor Validation）

```c
// 1071. Greatest Common Divisor of Strings (Easy)
// 貪心地從最長候選開始，逐步縮短長度直到找到整除兩個長度的數
char *gcdOfStrings(char *str1, char *str2) {
    size_t len1 = strlen(str1), len2 = strlen(str2);

    // 確保 str1 是較長的字串
    if (len1 < len2) return gcdOfStrings(str2, str1);

    size_t len = len2;
    while (len1 % len != 0 || len2 % len != 0) {
        len--;  // 貪心：逐步縮短
    }

    char *ret = malloc(len + 1);
    strncpy(ret, str2, len);
    ret[len] = '\0';

    // 驗證 ret 能否重複覆蓋 str1 和 str2
    // ... (驗證邏輯)

    return ret;
}
```

**複雜度：** O(min(m,n) * (m+n)) 時間，O(n) 空間
**貪心選擇：** 從最長候選開始向下嘗試，因為最長候選最有可能

---

### 7. 最大值追蹤（Track Maximum Value）

```c
// 1431. Kids With the Greatest Number of Candies (Easy)
// 追蹤最大值，比較每個元素是否能達到或超過最大值
bool* kidsWithCandies(int* candies, int candiesSize, int extraCandies,
                      int* returnSize) {
    // 找最大糖果數
    int maxCandies = candies[0];
    for (int i = 1; i < candiesSize; i++) {
        if (candies[i] > maxCandies) {
            maxCandies = candies[i];
        }
    }

    bool *result = malloc(candiesSize * sizeof(bool));
    *returnSize = candiesSize;

    // 貪心：每個孩子加上額外糖果後，檢查是否最多
    for (int i = 0; i < candiesSize; i++) {
        result[i] = (candies[i] + extraCandies >= maxCandies);
    }

    return result;
}
```

**複雜度：** O(n) 時間，O(n) 空間
**貪心選擇：** 一次遍歷即可確定每個孩子的答案

---

## 設計貪心演算法的步驟

1. **證明貪心選擇性質**：證明局部最優能導向全局最優
2. **定義貪心選擇**：決定每一步的最優決策標準
3. **實現貪心選擇**：簡潔高效的代碼實現
4. **驗證正確性**：用反例檢驗，或數學證明

## 常見陷阱

- **誤以為所有問題都能用貪心**：如多背包問題不能貪心
- **貪心選擇不正確**：需要仔細驗證
- **邊界條件處理**：空集合、單一元素等

## 相關 LeetCode 題目

| # | 題名 | 難度 | 貪心策略 |
|---|------|------|---------|
| 11 | Container With Most Water | Med | 短邊收縮 |
| 121 | Best Time to Buy and Sell Stock | Easy | 追蹤最小 |
| 122 | Best Time to Buy and Sell Stock II | Med | 累計正差 |
| 334 | Increasing Triplet Subsequence | Med | 維護兩個最小值 |
| 605 | Can Place Flowers | Easy | 局部決策 |
| 1071 | Greatest Common Divisor of Strings | Easy | 除數驗證 |
| 1431 | Kids With the Greatest Number of Candies | Easy | 追蹤最大值 |
