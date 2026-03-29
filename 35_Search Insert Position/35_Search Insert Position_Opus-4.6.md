# LeetCode 35: Search Insert Position

## Implementation vs. Editorial Comparison

The provided `.c` solution implements **closed-interval binary search**, which is **identical to the editorial's Binary Search approach**. There is no difference between the two implementations.

### Key Algorithm Details

**Closed Interval Binary Search [left, right]:**
- Initialization: `left = 0, right = n - 1` (both inclusive)
- Loop condition: `while (left <= right)`
- Mid-point calculation: `mid = left + (right - left) / 2` (prevents integer overflow)
- Search space reduction:
  - If `nums[mid] < target`: `left = mid + 1`
  - If `nums[mid] > target`: `right = mid - 1`
  - If `nums[mid] == target`: return `mid` immediately
- Termination: When `left > right`, the loop exits
- Return value: `left` (the insertion position)

### Why Returning `left` Works

When the loop terminates, the **loop invariant** guarantees:
- `nums[right] < target` (all elements at or before `right` are less than `target`)
- `target < nums[left]` (all elements at or from `left` are greater than or equal to `target`)

Therefore, `left` is exactly where `target` should be inserted to maintain sorted order.

### Integer Overflow Prevention

The calculation `mid = left + (right - left) / 2` is critical in C/C++:
- **Naive approach:** `mid = (left + right) / 2` can overflow when `left` and `right` are large
- **Safe approach:** `mid = left + (right - left) / 2` avoids overflow by computing the offset instead

This is a subtle but important detail that demonstrates deep understanding of C/C++ implementation concerns.

---

## Approach Comparison

| Aspect | Linear Scan (Baseline) | Binary Search (Recommended) |
|--------|------------------------|----------------------------|
| **Time Complexity** | O(n) | O(log n) |
| **Space Complexity** | O(1) | O(1) |
| **Problem Requirement** | Does NOT satisfy O(log n) mandate | Satisfies O(log n) requirement |
| **Scalability** | Fails on large inputs (n > 10^6) | Handles all input sizes efficiently |
| **Interview Suitability** | Poor—overlooks problem constraint | Excellent—demonstrates algorithmic knowledge |
| **C Implementation Difficulty** | Trivial (1-2 lines) | Moderate (requires careful boundary handling) |
| **Practical Use Case** | Only acceptable for very small arrays | Industry standard for sorted searches |
| **Edge Case Handling** | Simple but inefficient | More complex logic, must handle carefully |

---

## Baseline vs. Recommended Approach

### Baseline: Linear Scan

**Approach:** Iterate from left to right, checking each position:
- If `nums[i] >= target`, return `i`
- If no position found, return `n` (append at end)

**Why it's baseline:**
- Conceptually the simplest solution
- No understanding of the problem's constraint required
- Easy to implement, but completely misses the problem's intent

**Critical Flaw:**
The problem explicitly states: *"You must write an algorithm with O(log n) runtime complexity."*

Linear scan has **O(n) time complexity**, which **violates the core requirement**. This approach would be immediately rejected in a technical interview.

### Recommended: Binary Search

**Approach:** Use closed-interval binary search to locate the insertion position in O(log n) time.

**Why it's recommended:**

1. **Satisfies the Problem Constraint:** The problem mandates O(log n) time complexity. Binary search is the only approach that achieves this requirement on a sorted array.

2. **Optimal Performance:** With O(log n) complexity, binary search handles all input sizes efficiently:
   - n = 10^5: ~17 comparisons (vs. 100,000 iterations)
   - n = 10^6: ~20 comparisons (vs. 1,000,000 iterations)

3. **Interview Excellence:** Demonstrating binary search shows:
   - You understand algorithmic constraints
   - You can implement careful boundary logic
   - You're aware of integer overflow pitfalls in C/C++

4. **C Implementation Detail:** Mentioning the overflow-safe midpoint calculation (`left + (right - left) / 2`) signals deep C/C++ expertise and attention to detail.

5. **Loop Invariant Correctness:** Understanding why `return left` works (via loop invariant analysis) demonstrates rigorous algorithmic thinking beyond simple coding.

**Conclusion:** For this problem, binary search is not just recommended—it's the **only acceptable solution** because the problem explicitly requires O(log n) time complexity.

---

---

# LeetCode 35：搜尋插入位置

## 實現方案與題目解答比較

提供的 `.c` 解決方案實現了**閉區間二分搜尋**，與題目編輯的二分搜尋方案**完全相同**。兩者沒有任何實現上的差異。

### 關鍵演算法細節

**閉區間二分搜尋 [left, right]:**
- 初始化：`left = 0, right = n - 1`（包含兩端）
- 迴圈條件：`while (left <= right)`
- 中點計算：`mid = left + (right - left) / 2`（防止整數溢位）
- 搜尋空間縮減：
  - 若 `nums[mid] < target`：`left = mid + 1`
  - 若 `nums[mid] > target`：`right = mid - 1`
  - 若 `nums[mid] == target`：立即返回 `mid`
- 迴圈終止：當 `left > right` 時，迴圈退出
- 返回值：`left`（插入位置）

### 為什麼返回 `left` 是正確的

迴圈終止時，**迴圈不變式**保證：
- `nums[right] < target`（所有 `right` 位置及之前的元素都小於 `target`）
- `target <= nums[left]`（所有 `left` 位置及之後的元素都大於或等於 `target`）

因此，`left` 就是 `target` 在已排序陣列中應插入的位置。

### 整數溢位防護

計算 `mid = left + (right - left) / 2` 在 C/C++ 中至關重要：
- **素樸方法：** `mid = (left + right) / 2` 當 `left` 和 `right` 較大時會發生溢位
- **安全方法：** `mid = left + (right - left) / 2` 透過計算偏移量而非直接相加，避免溢位

這是一個細微但重要的細節，展示了對 C/C++ 實現細節的深刻理解。

---

## 方案比較

| 面向 | 線性掃描（基準） | 二分搜尋（推薦） |
|------|-----------------|-----------------|
| **時間複雜度** | O(n) | O(log n) |
| **空間複雜度** | O(1) | O(1) |
| **題目要求** | 不滿足 O(log n) 要求 | 完全滿足 O(log n) 要求 |
| **可擴展性** | 在大規模輸入上失效 (n > 10^6) | 有效處理所有輸入規模 |
| **面試適合度** | 差—忽視題目限制 | 優秀—展示演算法知識 |
| **C 實現難度** | 平凡（1-2 行代碼） | 中等（需要謹慎的邊界處理） |
| **實務應用** | 僅在極小陣列上可接受 | 業界標準的有序搜尋方法 |
| **邊界情況處理** | 簡單但效率低 | 邏輯更複雜，須小心處理 |

---

## 基準方案 vs. 推薦方案

### 基準方案：線性掃描

**方法：** 從左至右逐一檢查每個位置：
- 若 `nums[i] >= target`，返回 `i`
- 若未找到，返回 `n`（在末尾插入）

**為什麼是基準方案：**
- 概念上最簡單
- 無需理解題目約束條件
- 實現容易，但完全忽視題目的本質要求

**致命缺陷：**
題目明確指出：*「您必須編寫時間複雜度為 O(log n) 的演算法。」*

線性掃描的**時間複雜度為 O(n)**，**違反了核心要求**。在技術面試中，此方案會被立即否決。

### 推薦方案：二分搜尋

**方法：** 使用閉區間二分搜尋在 O(log n) 時間內找到插入位置。

**為什麼推薦此方案：**

1. **滿足題目限制：** 題目明確要求 O(log n) 時間複雜度。二分搜尋是在有序陣列上達成此要求的唯一方案。

2. **性能最優：** 以 O(log n) 複雜度，二分搜尋高效處理所有輸入規模：
   - n = 10^5：約 17 次比較（vs. 100,000 次迭代）
   - n = 10^6：約 20 次比較（vs. 1,000,000 次迭代）

3. **面試優勢：** 展示二分搜尋說明：
   - 你理解演算法的時間複雜度限制
   - 你能實現細緻的邊界邏輯
   - 你留意 C/C++ 中的整數溢位問題

4. **C 實現細節：** 提及溢位安全的中點計算 (`left + (right - left) / 2`) 展示了深厚的 C/C++ 專業知識和對細節的關注。

5. **迴圈不變式正確性：** 理解為什麼 `return left` 成立（透過迴圈不變式分析）展示了超越簡單編碼的嚴謹演算法思維。

**結論：** 對於此問題，二分搜尋不僅是推薦方案—它是**唯一可接受的解決方案**，因為題目明確要求 O(log n) 時間複雜度。
