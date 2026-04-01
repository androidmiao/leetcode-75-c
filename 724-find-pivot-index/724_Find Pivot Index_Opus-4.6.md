# LeetCode 724 - Find Pivot Index
## Implementation vs. Editorial Comparison

---

## 1. Implementation vs. Editorial Comparison

Both the repository solution and the editorial solution implement the same **prefix sum algorithm** with logically equivalent approaches.

### Repository Approach (Right-Sum Decrement)
```c
// First pass: compute total sum into right_sum
int right_sum = 0;
for (int i = 0; i < numsSize; i++) {
    right_sum += nums[i];
}

// Second pass: iterate while decrementing right_sum
int left_sum = 0;
for (int i = 0; i < numsSize; i++) {
    right_sum -= nums[i];
    if (left_sum == right_sum) {
        return i;
    }
    left_sum += nums[i];
}
```

**Logic**: Maintain `left_sum` (sum of elements before index i) and `right_sum` (sum of elements after index i). At each step, decrement `right_sum` by the current element and check if `left_sum == right_sum`.

### Editorial Approach (Total-Sum Tracking)
```c
// Compute total sum
int total_sum = 0;
for (int i = 0; i < numsSize; i++) {
    total_sum += nums[i];
}

// Single pass: track leftsum and derive rightsum
int leftsum = 0;
for (int i = 0; i < numsSize; i++) {
    // rightsum = total_sum - leftsum - nums[i]
    if (leftsum == total_sum - leftsum - nums[i]) {
        return i;
    }
    leftsum += nums[i];
}
```

**Logic**: Track `leftsum` (sum before index i) and derive `rightsum` by calculating `total_sum - leftsum - nums[i]`. Check if `leftsum == rightsum` before advancing.

### Equivalence
Both approaches are **logically identical**:
- Repository: Explicitly maintains `right_sum`, decrementing as it progresses
- Editorial: Computes `right_sum` on-the-fly as `total_sum - leftsum - nums[i]`

The mathematical relationship is:
```
right_sum (editorial) = total_sum - leftsum - nums[i]
                      = right_sum (repository, after decrement)
```

---

## 2. Which Approach is Better?

| Criterion | Repository (Right-Sum Decrement) | Editorial (Total-Sum Tracking) | Winner |
|-----------|----------------------------------|--------------------------------|--------|
| **Time Complexity** | O(n) - two passes | O(n) - two passes | Tie |
| **Space Complexity** | O(1) | O(1) | Tie |
| **Code Clarity** | Clear state management with explicit `left_sum` and `right_sum` variables | Slightly more abstract (derives `right_sum` on-the-fly) | Repository |
| **Variable Count** | 2 variables (`left_sum`, `right_sum`) | 2 variables (`leftsum`, `total_sum`) | Tie |
| **Interview Suitability** | Excellent - easy to explain the "left sum growing, right sum shrinking" intuition | Excellent - demonstrates mathematical optimization (avoiding variable decrement) | Tie (both strong) |
| **C Implementation Difficulty** | Straightforward; standard increment/decrement operations | Straightforward; standard arithmetic operations | Tie |
| **Physical Intuition** | Highly intuitive: walk through array while balancing left and right | Requires understanding of algebraic substitution | Repository |

### Recommendation
**The Repository approach (Right-Sum Decrement) is slightly preferable for interviews and readability:**
- The explicit `left_sum` and `right_sum` variables provide clear mental mapping
- Easier to explain: "As I move right, left sum grows and right sum shrinks"
- No need to derive the right sum algebraically—it's directly visible

Both solutions are equally efficient and are both excellent implementations of the prefix sum strategy.

---

## 3. Baseline vs. Recommended Approach

### Baseline: Brute Force O(n²)
```c
// For each index i, compute left sum and right sum separately
for (int i = 0; i < numsSize; i++) {
    int left_sum = 0;
    for (int j = 0; j < i; j++) {
        left_sum += nums[j];
    }
    int right_sum = 0;
    for (int j = i + 1; j < numsSize; j++) {
        right_sum += nums[j];
    }
    if (left_sum == right_sum) {
        return i;
    }
}
// Time: O(n²), Space: O(1)
```

**Disadvantage**: For each of the n indices, we recompute left and right sums from scratch. This results in redundant calculations—the same values are computed multiple times.

### Recommended: Prefix Sum with Single-Variable Tracking O(n)

**Both the repository and editorial solutions implement this approach.** The key insight is:
1. **Precompute total sum** in the first pass (O(n))
2. **Single pass** through the array, maintaining only `left_sum` (O(n))
3. At each index i, derive `right_sum` as `total_sum - left_sum - nums[i]` without recalculating
4. Check the pivot condition and advance

**Why This is Superior**:
- **Eliminates redundant computation**: Instead of recalculating left and right sums for each index, we build them incrementally
- **Time complexity improves from O(n²) to O(n)**: Two sequential passes are linear; nested recalculation is quadratic
- **Space complexity remains O(1)**: No auxiliary data structures needed
- **Practical efficiency**: For large arrays (n = 10⁴), O(n²) brute force becomes prohibitively slow

**The Principle**: Prefix sums transform problems requiring sum recalculation at every step into problems with direct O(1) lookup per element. By pre-computing the total and tracking partial sums, we avoid revisiting elements.

---

## Summary

| Aspect | Detail |
|--------|--------|
| **Both implementations** | Use the same prefix sum algorithm with O(n) time, O(1) space |
| **Repository advantage** | Explicit state variables (`left_sum`, `right_sum`) are more intuitive |
| **Editorial advantage** | Demonstrates algebraic optimization (avoids variable decrement) |
| **Practical recommendation** | Either approach is excellent; repository slightly favored for interview clarity |
| **Why prefix sum wins** | Reduces baseline O(n²) brute force to O(n) by eliminating redundant recalculation |

---

## English Section Complete

---

# 中文版本 (Traditional Chinese)

---

# LeetCode 724 - 尋找陣列的中心下標
## 實現與編輯方案比較

---

## 1. 實現與編輯方案比較

倉庫解法和編輯方案都實現了相同的**前綴和演算法**，方法在邏輯上等價。

### 倉庫方案（右側和遞減）
```c
// 第一遍：計算總和到 right_sum
int right_sum = 0;
for (int i = 0; i < numsSize; i++) {
    right_sum += nums[i];
}

// 第二遍：遍歷時遞減 right_sum
int left_sum = 0;
for (int i = 0; i < numsSize; i++) {
    right_sum -= nums[i];
    if (left_sum == right_sum) {
        return i;
    }
    left_sum += nums[i];
}
```

**邏輯**：維護 `left_sum`（下標 i 前的元素和）和 `right_sum`（下標 i 後的元素和）。每一步中，用當前元素遞減 `right_sum`，並檢查 `left_sum == right_sum`。

### 編輯方案（總和追蹤）
```c
// 計算總和
int total_sum = 0;
for (int i = 0; i < numsSize; i++) {
    total_sum += nums[i];
}

// 單遍：追蹤 leftsum，動態推導 rightsum
int leftsum = 0;
for (int i = 0; i < numsSize; i++) {
    // rightsum = total_sum - leftsum - nums[i]
    if (leftsum == total_sum - leftsum - nums[i]) {
        return i;
    }
    leftsum += nums[i];
}
```

**邏輯**：追蹤 `leftsum`（下標 i 前的和），並通過計算 `total_sum - leftsum - nums[i]` 動態推導 `rightsum`。在推進前檢查條件。

### 等價性
兩種方案在**邏輯上完全相同**：
- 倉庫方案：顯式維護 `right_sum`，隨著進行而遞減
- 編輯方案：動態計算 `right_sum` 為 `total_sum - leftsum - nums[i]`

數學關係為：
```
right_sum (編輯方案) = total_sum - leftsum - nums[i]
                    = right_sum (倉庫方案，遞減後)
```

---

## 2. 哪種方案更好？

| 評估項目 | 倉庫方案（右側和遞減） | 編輯方案（總和追蹤） | 勝者 |
|---------|----------------------|----------------------|------|
| **時間複雜度** | O(n) - 兩遍 | O(n) - 兩遍 | 平手 |
| **空間複雜度** | O(1) | O(1) | 平手 |
| **程式碼清晰度** | 變數狀態明確，易於理解 | 動態推導，稍微抽象 | 倉庫方案 |
| **變數數量** | 2 個（`left_sum`, `right_sum`） | 2 個（`leftsum`, `total_sum`） | 平手 |
| **面試適合度** | 優秀 - 直觀解釋 | 優秀 - 展示數學最佳化 | 平手 |
| **C 語言實現難度** | 直接簡單 | 直接簡單 | 平手 |
| **物理直覺性** | 高度直觀：左和增長，右和收縮 | 需要代數替換理解 | 倉庫方案 |

### 建議
**倉庫方案（右側和遞減）在面試和可讀性上略佔優勢：**
- 顯式的 `left_sum` 和 `right_sum` 變數提供清晰的心理映射
- 更易解釋："隨著向右移動，左側和增長，右側和收縮"
- 無需代數推導；右側和直接可見

兩個解法在效率上完全相同，都是前綴和策略的優秀實現。

---

## 3. 基線方案與推薦方案

### 基線：蠻力 O(n²)
```c
// 對每個下標 i，分別計算左側和與右側和
for (int i = 0; i < numsSize; i++) {
    int left_sum = 0;
    for (int j = 0; j < i; j++) {
        left_sum += nums[j];
    }
    int right_sum = 0;
    for (int j = i + 1; j < numsSize; j++) {
        right_sum += nums[j];
    }
    if (left_sum == right_sum) {
        return i;
    }
}
// 時間：O(n²)，空間：O(1)
```

**缺點**：對於每個 n 個下標中的每一個，我們從零開始重新計算左側和與右側和。這導致大量重複計算——同一些值被多次計算。

### 推薦：單變數前綴和追蹤 O(n)

**倉庫方案和編輯方案都實現了這種方法。** 核心洞察是：
1. **預計算總和**，第一遍 (O(n))
2. **單遍遍歷**陣列，僅維護 `left_sum` (O(n))
3. 在每個下標 i，推導 `right_sum` 為 `total_sum - left_sum - nums[i]`，無需重新計算
4. 檢查中心條件並推進

**為什麼這更優越**：
- **消除重複計算**：無需為每個下標重新計算左側和與右側和，我們增量式構建它們
- **時間複雜度從 O(n²) 改進至 O(n)**：兩次連續遍歷是線性的；嵌套重新計算是二次的
- **空間複雜度保持 O(1)**：無需輔助資料結構
- **實踐效率**：對於大陣列（n = 10⁴），O(n²) 蠻力變得非常緩慢

**原理**：前綴和將需要在每一步重新計算和的問題轉變為允許直接 O(1) 查詢的問題。通過預計算總和和追蹤部分和，我們避免重新訪問元素。

---

## 總結

| 方面 | 細節 |
|------|------|
| **兩個實現** | 使用相同的前綴和演算法，時間 O(n)，空間 O(1) |
| **倉庫方案優勢** | 顯式狀態變數（`left_sum`, `right_sum`）更直觀 |
| **編輯方案優勢** | 展示代數最佳化（避免變數遞減） |
| **實踐建議** | 任一方案都優秀；倉庫方案在面試清晰度上略佔上風 |
| **為何前綴和勝出** | 通過消除重複重新計算，將基線 O(n²) 蠻力降至 O(n) |

---

## 中文部分完成
