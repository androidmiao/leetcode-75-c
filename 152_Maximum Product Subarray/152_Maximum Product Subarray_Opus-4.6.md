# LeetCode 152: Maximum Product Subarray

## Implementation vs. Editorial Comparison

The `.c` solution implements the **Dynamic Programming approach** (Editorial Approach 2), tracking both maximum and minimum products simultaneously as we iterate through the array. This is necessary because a negative number can flip a minimum product into a maximum product on the next iteration.

**Key Implementation Details:**
- Uses `max_prod` and `min_prod` state variables to track candidates at each position
- Employs helper functions `max3()` and `min3()` to cleanly compute new states considering three options:
  - Start a new subarray (current element alone)
  - Extend the previous max product
  - Extend the previous min product
- Crucially, uses `temp_max` to store the old `max_prod` before updating it, preventing accidental overwrites that would corrupt the calculation of new `min_prod`
- Time Complexity: O(n)
- Space Complexity: O(1)

**Bug Fix Note:** The original stub solution had a subtle bug where `min_prod` was computed using the already-modified `max_prod` instead of its original value. The corrected solution uses `temp_max` to preserve the pre-update value, ensuring correct state transitions.

---

## Approach Comparison

| Aspect | Approach 1: Brute Force | Approach 2: Dynamic Programming |
|--------|------------------------|----------------------------------|
| **Time Complexity** | O(N²) | O(N) |
| **Space Complexity** | O(1) | O(1) |
| **Interview Suitability** | Poor—naive solution, fails on large inputs | Excellent—clean DP solution demonstrating state management |
| **C Implementation Difficulty** | Very easy | Moderate—requires careful variable handling (temp_max) and helper functions |
| **Scalability** | Does not scale; times out on large arrays | Scales efficiently to any input size |
| **Algorithm Insight** | None; relies on brute enumeration | High—reveals why tracking both max and min is necessary with negative numbers |

---

## Baseline vs. Recommended Approach

**Baseline (Brute Force, O(N²)):**
The brute force approach considers every possible subarray by checking all pairs of start and end indices, computing the product for each. While conceptually simple and space-efficient, it is impractical for interviews:
- Fails on large inputs (TLE—Time Limit Exceeded)
- Does not demonstrate algorithmic insight
- Wastes interview time on an inefficient solution

**Recommended (Dynamic Programming, O(N)):**
The DP approach solves the problem in a single pass by maintaining two key insights:

1. **State Tracking with Negatives:** A minimum product can become a maximum product after multiplying by a negative number. Therefore, we must track both `max_prod` and `min_prod` at each position.

2. **Three Choices at Each Step:** At each element, the new maximum product is the best of three options:
   - Start fresh with the current element alone
   - Extend the previous maximum product by multiplying
   - Extend the previous minimum product (useful when the current number is negative)

**Why DP is Superior for Interviews:**
- **O(N) efficiency:** Demonstrates optimization skills and understanding of problem structure
- **State management mastery:** Shows you can handle complex state (max and min simultaneously) and recognize when supplementary tracking is needed
- **Clean implementation:** The helper functions make the code readable and maintainable
- **Problem-solving insight:** Reveals deep understanding of why negative numbers require special handling in product problems

The DP solution is the gold standard for this problem in technical interviews, balancing elegance, efficiency, and demonstrable algorithmic thinking.

---

---

# LeetCode 152：最大乘積子陣列

## 實現與題目編輯社論之比較

`.c` 解決方案實現了**動態規劃方法**（編輯社論方法 2），在迭代陣列時同時追蹤最大和最小乘積。這是必要的，因為負數可以在下一次迭代中將最小乘積翻轉為最大乘積。

**關鍵實現細節：**
- 使用 `max_prod` 和 `min_prod` 狀態變數，在每個位置追蹤候選值
- 採用輔助函數 `max3()` 和 `min3()`，清潔地計算新狀態，考慮三個選項：
  - 啟動新子陣列（當前元素單獨成立）
  - 擴展先前的最大乘積
  - 擴展先前的最小乘積
- 關鍵地，使用 `temp_max` 在更新前存儲舊的 `max_prod`，防止意外覆蓋導致新 `min_prod` 計算損壞
- 時間複雜度：O(n)
- 空間複雜度：O(1)

**錯誤修正備註：** 原始存根解決方案存在細微錯誤，其中 `min_prod` 使用已經修改的 `max_prod` 而不是其原始值進行計算。更正後的解決方案使用 `temp_max` 保留更新前的值，確保正確的狀態轉換。

---

## 方法比較

| 方面 | 方法 1：暴力搜尋 | 方法 2：動態規劃 |
|------|-----------------|-----------------|
| **時間複雜度** | O(N²) | O(N) |
| **空間複雜度** | O(1) | O(1) |
| **面試適用性** | 差——幼稚的解決方案，在大輸入上失敗 | 優秀——乾淨的 DP 解決方案，展現狀態管理能力 |
| **C 語言實現難度** | 非常容易 | 中等——需要謹慎的變數處理（temp_max）和輔助函數 |
| **可擴展性** | 不可擴展；在大陣列上超時 | 高效擴展至任何輸入規模 |
| **算法洞察** | 無；依賴於暴力枚舉 | 高——揭示為什麼在負數情況下必須追蹤最大和最小值 |

---

## 基礎方案 vs 推薦方案

**基礎方案（暴力搜尋，O(N²)）：**
暴力搜尋方法考慮每個可能的子陣列，通過檢查所有開始和結束索引對來計算每個子陣列的乘積。雖然概念簡單且空間高效，但在面試中不切實際：
- 在大輸入上失敗（TLE——時間超限）
- 未展現算法洞察
- 浪費面試時間在低效的解決方案上

**推薦方案（動態規劃，O(N)）：**
DP 方法通過單次遍歷解決問題，維護兩個關鍵洞察：

1. **帶有負數的狀態追蹤：** 最小乘積在乘以負數後可以變成最大乘積。因此，我們必須在每個位置追蹤 `max_prod` 和 `min_prod`。

2. **每一步的三個選擇：** 在每個元素處，新的最大乘積是三個選項中最好的：
   - 僅以當前元素啟動新陣列
   - 通過相乘擴展先前的最大乘積
   - 擴展先前的最小乘積（當當前數字為負數時有用）

**為什麼 DP 在面試中優越：**
- **O(N) 效率：** 展現優化技能和對問題結構的理解
- **狀態管理精通：** 顯示您可以同時處理複雜狀態（最大和最小）並識別何時需要補充追蹤
- **乾淨的實現：** 輔助函數使程式碼易讀且可維護
- **解決問題的洞察：** 揭示對為什麼負數需要在乘積問題中特殊處理的深入理解

DP 解決方案是此問題在技術面試中的黃金標準，平衡了優雅性、效率和可見的算法思考。
