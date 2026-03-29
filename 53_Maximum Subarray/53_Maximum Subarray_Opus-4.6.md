# LeetCode 53: Maximum Subarray — Implementation vs. Editorial Comparison

## Overview
The C implementation for LeetCode 53 uses **Kadane's Algorithm**, which corresponds to **Approach 2** in the official editorial. This document compares the implementation against the editorial's three approaches.

---

## Editorial Approaches Summary

The LeetCode editorial presents three approaches to solve the Maximum Subarray problem:

1. **Approach 1: Optimized Brute Force**
   - Time Complexity: O(N²)
   - Space Complexity: O(1)
   - Method: Check all possible subarrays and track the maximum sum

2. **Approach 2: Kadane's Algorithm (Dynamic Programming)**
   - Time Complexity: O(N)
   - Space Complexity: O(1)
   - Method: Maintain running sum and reset when it becomes negative
   - **Status: RECOMMENDED by editorial**

3. **Approach 3: Divide and Conquer**
   - Time Complexity: O(N log N)
   - Space Complexity: O(log N)
   - Method: Recursively divide array and merge results

---

## Implementation Analysis

The provided C implementation uses **Kadane's Algorithm (Approach 2)**, which is the optimal and recommended solution.

### Algorithm Correctness
- The implementation correctly implements Kadane's Algorithm
- It maintains `max_sum` as the global maximum subarray sum
- It maintains `current_sum` as the maximum sum ending at the current position
- Core logic: `current_sum = max(num[i], current_sum + num[i])`
- The algorithm correctly handles all edge cases including all-negative arrays

### Implementation Quality
- Uses standard C constructs (no complex data structures)
- Single pass through the array (O(N) time)
- Constant space (O(1) space)
- Clear variable naming and logic flow
- Suitable for interview environments

---

## Comparison: Three Approaches

| Metric | Approach 1: Brute Force | Approach 2: Kadane's | Approach 3: Divide & Conquer |
|--------|------------------------|---------------------|------------------------------|
| **Time Complexity** | O(N²) | O(N) ✓ | O(N log N) |
| **Space Complexity** | O(1) | O(1) ✓ | O(log N) |
| **Interview Suitability** | Poor (too slow) | Excellent ✓ | Good (follow-up) |
| **C Implementation Difficulty** | Very Easy | Easy ✓ | Moderate |
| **Explanation Difficulty** | Easy | Very Easy ✓ | Moderate |
| **Code Length** | Short | Short ✓ | Longer |
| **Practical Use** | No | Yes ✓ | Rare |

---

## Baseline vs. Recommended

### Baseline: Optimized Brute Force (Approach 1)
- **When to use**: Initial discussion or learning the problem
- **Advantage**: Intuitive logic—simply check all subarrays
- **Disadvantage**: O(N²) time makes it unsuitable for large inputs or interviews
- **Code pattern**: Two nested loops to enumerate all subarrays

### Recommended: Kadane's Algorithm (Approach 2)
- **Why recommended**:
  1. **Optimal Time Complexity**: O(N) is the best possible for this problem
  2. **Simple to Implement in C**: No complex data structures needed
  3. **Interview-Friendly**: Easy to code, explain, and defend
  4. **Space Efficient**: Uses only O(1) extra space
  5. **Industry Standard**: The go-to solution for this problem

- **Code pattern**: Single pass with running sum tracking

### Follow-up: Divide and Conquer (Approach 3)
- **When to mention**: If interviewer asks for alternative approaches
- **Advantage**: Demonstrates deeper algorithmic thinking
- **Disadvantage**: O(N log N) is suboptimal compared to Kadane's
- **Use case**: Interview impression or academic completeness
- **Note**: In C, the recursion and managing subarrays adds complexity

---

## Recommendation Summary

✓ **Use Kadane's Algorithm (Approach 2)** for LeetCode 53 because:
- It is the optimal O(N) time solution
- It is the easiest to implement correctly in C
- It is the easiest to explain in an interview
- It uses constant O(1) space
- It is the editorial's recommended approach

The provided C implementation correctly implements this recommended approach.

---

---

# LeetCode 53：最大子陣列和 — 實現與編者詳解對比

## 概述
LeetCode 53 的 C 實現使用 **Kadane 演算法**，對應官方編者詳解中的 **方法 2**。本文件將實現與編者詳解的三種方法進行比較。

---

## 編者詳解方法總結

LeetCode 編者詳解呈現了三種解決最大子陣列和問題的方法：

1. **方法 1：最佳化暴力法**
   - 時間複雜度：O(N²)
   - 空間複雜度：O(1)
   - 方法：檢查所有可能的子陣列並追蹤最大和

2. **方法 2：Kadane 演算法（動態規劃）**
   - 時間複雜度：O(N)
   - 空間複雜度：O(1)
   - 方法：維護執行中的和，當其變為負數時重置
   - **狀態：編者推薦**

3. **方法 3：分治法**
   - 時間複雜度：O(N log N)
   - 空間複雜度：O(log N)
   - 方法：遞迴地分割陣列並合併結果

---

## 實現分析

提供的 C 實現使用 **Kadane 演算法（方法 2）**，這是最優且推薦的解決方案。

### 演算法正確性
- 實現正確地執行 Kadane 演算法
- 維護 `max_sum` 作為全域最大子陣列和
- 維護 `current_sum` 作為以當前位置結尾的最大和
- 核心邏輯：`current_sum = max(num[i], current_sum + num[i])`
- 演算法正確處理所有邊界情況，包括全負陣列

### 實現品質
- 使用標準 C 結構（無複雜資料結構）
- 單次遍歷陣列（O(N) 時間）
- 常數空間（O(1) 空間）
- 清晰的變數命名和邏輯流程
- 適合面試環境

---

## 三種方法的比較

| 指標 | 方法 1：暴力法 | 方法 2：Kadane | 方法 3：分治法 |
|------|------------------|---------------------|------------------------|
| **時間複雜度** | O(N²) | O(N) ✓ | O(N log N) |
| **空間複雜度** | O(1) | O(1) ✓ | O(log N) |
| **面試適用性** | 差（太慢） | 優秀 ✓ | 良好（後續問題） |
| **C 實現難度** | 非常簡單 | 簡單 ✓ | 中等 |
| **說明難度** | 簡單 | 非常簡單 ✓ | 中等 |
| **程式碼長度** | 短 | 短 ✓ | 較長 |
| **實際應用** | 否 | 是 ✓ | 罕見 |

---

## 基準方法 vs 推薦方法

### 基準方法：最佳化暴力法（方法 1）
- **何時使用**：初期討論或學習該問題
- **優點**：邏輯直觀——只需檢查所有子陣列
- **缺點**：O(N²) 時間複雜度使其不適合大輸入或面試
- **程式碼模式**：使用兩個嵌套迴圈列舉所有子陣列

### 推薦方法：Kadane 演算法（方法 2）
- **推薦原因**：
  1. **最優時間複雜度**：O(N) 是該問題的最佳可能複雜度
  2. **在 C 中簡單實現**：不需要複雜資料結構
  3. **面試友善**：易於編寫、解釋和辯護
  4. **空間有效**：僅使用 O(1) 額外空間
  5. **業界標準**：該問題的標準解決方案

- **程式碼模式**：單次遍歷，追蹤執行中的和

### 後續問題：分治法（方法 3）
- **何時提及**：若面試官要求替代方法
- **優點**：展示更深層次的演算法思維
- **缺點**：O(N log N) 相比 Kadane 不是最優的
- **使用情境**：面試印象或學術完整性
- **說明**：在 C 中，遞迴和管理子陣列增加了複雜度

---

## 推薦總結

✓ **對 LeetCode 53 使用 Kadane 演算法（方法 2）**，原因如下：
- 它是最優的 O(N) 時間解決方案
- 在 C 中最容易正確實現
- 在面試中最容易解釋
- 使用常數 O(1) 空間
- 它是編者詳解推薦的方法

提供的 C 實現正確地實現了此推薦方法。
