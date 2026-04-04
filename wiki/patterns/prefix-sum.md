# Prefix Sum 前綴和模式

## 核心概念

前綴和是一種預處理技術，將陣列轉換為累積和陣列，使得**查詢任意區間的和可在 O(1) 時間內完成**。

**何時使用：**
- 多次查詢陣列中區間的和
- 需要快速計算某個位置左側或右側的和
- 分割點問題（如尋找平衡點）

## 核心模式

### 1. 左右累積乘積（Left-Right Product Arrays）

```c
// 238. Product of Array Except Self (Medium)
// 不用除法，計算每個位置除了自己以外的乘積
int* productExceptSelf(int* nums, int numsSize, int* returnSize) {
    int *result = malloc(numsSize * sizeof(int));
    int *left = malloc(numsSize * sizeof(int));
    int *right = malloc(numsSize * sizeof(int));

    // 構建左邊界乘積：left[i] = nums[0] * nums[1] * ... * nums[i-1]
    left[0] = 1;
    for (int i = 1; i < numsSize; i++) {
        left[i] = left[i-1] * nums[i-1];
    }

    // 構建右邊界乘積：right[i] = nums[i+1] * nums[i+2] * ... * nums[n-1]
    right[numsSize - 1] = 1;
    for (int i = numsSize - 2; i >= 0; i--) {
        right[i] = right[i+1] * nums[i+1];
    }

    // 結果 = 左邊界 × 右邊界
    for (int i = 0; i < numsSize; i++) {
        result[i] = left[i] * right[i];
    }

    free(left);
    free(right);
    *returnSize = numsSize;
    return result;
}
```

**複雜度：** O(n) 時間，O(n) 空間（3 個陣列）
**關鍵洞察：** 每個位置的答案 = 左邊所有元素的乘積 × 右邊所有元素的乘積

---

### 2. 左右累積和（Left-Right Sum Arrays）

```c
// 724. Find Pivot Index (Easy)
// 找「支點」使得左邊和 = 右邊和
int pivotIndex(int* nums, int numsSize) {
    // 計算總和
    int totalSum = 0;
    for (int i = 0; i < numsSize; i++) {
        totalSum += nums[i];
    }

    int leftSum = 0;
    for (int i = 0; i < numsSize; i++) {
        // rightSum = totalSum - leftSum - nums[i]
        // 如果 leftSum == rightSum，則：
        // leftSum = totalSum - leftSum - nums[i]
        // 2 * leftSum + nums[i] = totalSum
        int rightSum = totalSum - leftSum - nums[i];

        if (leftSum == rightSum) {
            return i;
        }
        leftSum += nums[i];
    }
    return -1;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**技巧：** 不需要額外陣列，動態計算左和與右和

---

### 3. 累積和追蹤最大值（Cumulative Sum Tracking Maximum）

```c
// 1732. Find the Highest Altitude (Easy)
// 追蹤累積海拔，找最高點
int largestAltitude(int* gain, int gainSize) {
    int currentAltitude = 0;
    int maxAltitude = 0;

    for (int i = 0; i < gainSize; i++) {
        currentAltitude += gain[i];  // 累積和
        if (currentAltitude > maxAltitude) {
            maxAltitude = currentAltitude;
        }
    }

    return maxAltitude;
}
```

**複雜度：** O(n) 時間，O(1) 空間
**使用場景：** 一次遍歷既要累積又要追蹤最大值

---

## 進階應用

### 區間查詢優化

```c
// 通用前綴和模式：預處理 O(n)，查詢 O(1)
typedef struct {
    int *prefixSum;  // prefixSum[i] = nums[0] + ... + nums[i-1]
    int size;
} NumArray;

NumArray* numArrayCreate(int* nums, int numsSize) {
    NumArray *obj = malloc(sizeof(NumArray));
    obj->prefixSum = malloc((numsSize + 1) * sizeof(int));
    obj->size = numsSize;

    obj->prefixSum[0] = 0;
    for (int i = 0; i < numsSize; i++) {
        obj->prefixSum[i+1] = obj->prefixSum[i] + nums[i];
    }
    return obj;
}

// 查詢 [left, right] 的和
int sumRange(NumArray* obj, int left, int right) {
    return obj->prefixSum[right + 1] - obj->prefixSum[left];
}

void numArrayFree(NumArray* obj) {
    free(obj->prefixSum);
    free(obj);
}
```

---

## 前綴和 vs. 其他技術

| 技術 | 時間（查詢） | 空間 | 適用場景 |
|------|-------------|------|----------|
| 前綴和 | O(1) | O(n) | 多次查詢區間和 |
| 暴力掃描 | O(n) | O(1) | 單次或少數查詢 |
| 線段樹 | O(log n) | O(n) | 範圍查詢 + 動態更新 |
| 樹狀陣列 | O(log n) | O(n) | 動態更新 + 範圍查詢 |

---

## 常見變種

1. **二維前綴和**（影像區域查詢）
   - `prefixSum[i][j]` = 左上角到 (i,j) 的和
   - 查詢 O(1)，構建 O(n²)

2. **模運算前綴和**（處理溢出）
   - 每次累加後取模
   - 解決範圍溢出問題

3. **差分陣列**（區間更新優化）
   - 與前綴和互為逆運算
   - 區間更新 O(1)，查詢需重構

---

## 相關 LeetCode 題目

| # | 題名 | 難度 | 前綴和應用 |
|---|------|------|-----------|
| 238 | Product of Array Except Self | Med | 左右累積乘積 |
| 724 | Find Pivot Index | Easy | 左右累積和 |
| 1732 | Find the Highest Altitude | Easy | 累積和追蹤最大值 |

---

## 設計技巧

1. **明確邊界**：前綴和陣列通常比原陣列長 1
2. **簡化查詢**：利用 `sum[right+1] - sum[left]` 公式
3. **處理負數**：前綴和對負數也適用
4. **空間優化**：某些情況可原地修改（如果允許）

## 面試提示

- 解釋為何選擇前綴和而非其他方法
- 強調預處理與查詢的時間權衡
- 討論如何推廣到二維或更高維
