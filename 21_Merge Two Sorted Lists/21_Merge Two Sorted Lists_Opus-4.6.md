# LeetCode 21 - Merge Two Sorted Lists

## Implementation vs. Editorial Comparison

The .c solution implements **Approach 2: Iteration** from the Editorial. Both use the same core algorithm:

- Create a dummy head node to simplify edge cases
- Use a pointer (tail/prev) to track the current end of the merged list
- Iterate through both input lists, comparing values and appending the smaller node
- Attach any remaining nodes from the non-empty list
- Return the merged list starting from dummy->next

The only difference is terminology: the .c solution uses `dummy` and `tail`, while the Editorial uses `prehead` and `prev`. The logic and approach are identical.

## Which Approach is Better?

| Aspect | Approach 1: Recursion | Approach 2: Iteration (.c solution) |
|--------|----------------------|-------------------------------------|
| **Time Complexity** | O(n+m) | O(n+m) |
| **Space Complexity** | O(n+m) due to call stack | O(1) - only uses pointers |
| **Interview Suitability** | Good - shows recursive thinking | Better - demonstrates optimization awareness |
| **C Implementation Difficulty** | Straightforward | Straightforward - slightly easier in C |
| **Code Clarity** | Very clean, intuitive | Slightly more verbose but explicit |
| **Production Code** | Risky for large inputs (stack overflow) | Safe for all input sizes |

**Why Iteration is Superior:**
The iterative approach is better because it avoids the O(n+m) space complexity from the recursion stack. For an interview, this demonstrates that you can recognize and optimize away unnecessary space usage. Since the recursive solution doesn't gain anything in time complexity, choosing iteration shows strong algorithmic thinking.

## Baseline vs. Recommended

**Baseline Approach:** Recursion (Approach 1)
- Intuitive and elegant
- Natural way to think about "merge the rest"
- But uses extra space for the call stack

**Recommended Approach:** Iteration (Approach 2) — *This is what the .c solution uses*
- Achieves the same O(n+m) time complexity
- Reduces space complexity from O(n+m) to O(1)
- More scalable for production systems
- Perfect for demonstrating optimization skills in interviews

**Why Iteration is Better for Interview Discussion:**
In an interview, you would ideally present the recursive solution first (to show you can think recursively), then explain that while it's correct and elegant, the call stack adds unnecessary space. Then pivot to the iterative solution as the optimized version. This shows you understand both approaches and can recognize when to optimize.

---

# LeetCode 21 - 合併兩個已排序的列表

## 實現與編輯部版本的比較

.c 解決方案實現了**編輯部的方法 2：迭代**。兩者使用相同的核心演算法：

- 建立一個虛擬頭節點來簡化邊界情況
- 使用指針（tail/prev）追蹤合併列表的當前末端
- 遍歷兩個輸入列表，比較值並附加較小的節點
- 將任何剩餘節點從非空列表附加到結果中
- 返回從 dummy->next 開始的合併列表

唯一的區別是術語：.c 解決方案使用 `dummy` 和 `tail`，而編輯部版本使用 `prehead` 和 `prev`。邏輯和方法完全相同。

## 哪個方法更好？

| 方面 | 方法 1：遞迴 | 方法 2：迭代（.c 解決方案） |
|------|------------|--------------------------|
| **時間複雜度** | O(n+m) | O(n+m) |
| **空間複雜度** | O(n+m)（由於調用堆棧） | O(1)（僅使用指針） |
| **面試適用性** | 良好 - 展示遞迴思維 | 更好 - 展示優化意識 |
| **C 實現難度** | 直接 | 直接 - 在 C 中稍容易 |
| **代碼清晰度** | 非常清晰，直觀 | 稍微冗長但明確 |
| **生產代碼** | 風險大（大輸入會導致堆棧溢出） | 安全（適用於所有輸入大小） |

**為什麼迭代更優越：**
迭代方法更好，因為它避免了遞迴調用堆棧帶來的 O(n+m) 空間複雜度。在面試中，這表明你能夠識別並優化不必要的空間使用。由於遞迴解決方案在時間複雜度上沒有優勢，選擇迭代展示了強大的演算法思維。

## 基準方法 vs. 建議方法

**基準方法：** 遞迴（方法 1）
- 直觀且優雅
- 自然的思考方式：「合併其餘部分」
- 但調用堆棧佔用額外空間

**建議方法：** 迭代（方法 2）— *這就是 .c 解決方案所使用的*
- 達成相同的 O(n+m) 時間複雜度
- 將空間複雜度從 O(n+m) 降低到 O(1)
- 更適合生產系統的可擴展性
- 完美展示面試中的優化能力

**為什麼迭代對面試討論更好：**
在面試中，理想的做法是首先提出遞迴解決方案（展示你的遞迴思維能力），然後解釋雖然它正確且優雅，但調用堆棧增加了不必要的空間。然後轉向迭代解決方案作為優化版本。這表明你理解兩種方法，並能識別何時進行優化。
