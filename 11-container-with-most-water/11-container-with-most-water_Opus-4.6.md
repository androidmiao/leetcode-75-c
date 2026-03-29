# LeetCode 11: Container With Most Water — Solution vs. Editorial Comparison

## English

### 1. Implementation vs. Editorial Comparison

The repository's `solution.c` implementation employs the **Two Pointer approach**, which is identical in algorithm to **Editorial Approach 2**. Both solutions converge on a single pointer movement strategy: initialize pointers at opposite ends, compute the area at each step, and move the pointer pointing to the shorter line inward.

**Key Differences in Implementation Details:**

| Aspect | Repo's solution.c | Editorial Approach 2 |
|--------|------------------|----------------------|
| **Min height calculation** | `fmin(height[left], height[right])` | `min(height[left], height[right])` |
| **Max area update** | Ternary operator: `area = (area > current) ? area : current` | Function call: `area = max(area, current)` |
| **Loop condition** | `while (right - left > 0)` | `while (left < right)` |
| **Time complexity** | O(n) | O(n) |
| **Space complexity** | O(1) | O(1) |

These differences are stylistic and algorithmic-equivalent. The `fmin()` function and the ternary operator are idiomatic C choices, whereas the editorial uses language-agnostic pseudocode or abstractions. The loop conditions `right - left > 0` and `left < right` both terminate when the pointers meet.

### 2. Which Approach is Better: Brute Force vs. Two Pointer

| Criterion | Brute Force (Editorial Approach 1) | Two Pointer (Editorial Approach 2 & Repo) |
|-----------|-------------------------------------|-------------------------------------------|
| **Time Complexity** | O(n²) | O(n) |
| **Space Complexity** | O(1) | O(1) |
| **Interview Suitability** | Poor — naive solution, shows lack of optimization insight | Excellent — demonstrates greedy thinking and space-time tradeoff awareness |
| **C Implementation Difficulty** | Minimal — straightforward nested loops | Low — requires understanding of pointer movement logic |
| **Scalability** | Fails on large inputs (n > 10,000) | Handles large inputs efficiently |
| **Follow-up Potential** | Limited discussion value | Opens discussion of greedy correctness, why shorter pointer moves |

**Verdict:** The Two Pointer approach is unequivocally better. In interviews, proposing the O(n²) brute force first, then optimizing to O(n), demonstrates problem-solving maturity. However, the two-pointer solution should be your target implementation.

### 3. Baseline vs. Recommended

**Baseline Approach (Brute Force):**
- **Algorithm:** Check all pairs (i, j) where i < j, compute area for each pair, track the maximum.
- **Time Complexity:** O(n²)
- **Space Complexity:** O(1)
- **Why it works:** Exhaustive enumeration guarantees finding the maximum container.
- **Interview context:** Acceptable as an initial explanation, but expected to be optimized.

**Recommended Approach (Two Pointer):**
- **Algorithm:** Place pointers at both ends. Compute area. Move the pointer at the shorter line inward. Repeat until pointers meet.
- **Time Complexity:** O(n)
- **Space Complexity:** O(1)
- **Why it is better:** Reduces redundant pair checks. The greedy insight is that moving the taller pointer inward cannot increase area (the height is still bounded by the shorter line, and width decreases). Moving the shorter pointer inward has the potential to find a taller height that increases area.
- **Interview context:** Expected solution. Demonstrates understanding of optimization and greedy algorithm principles.

---

## 繁體中文

### 1. 實現方案 vs. 官方解法比較

倉庫中的 `solution.c` 實現採用了**雙指針方法**，在演算法上與**官方方案 2** 完全相同。兩種解法都基於同一個指針移動策略：在兩端初始化指針，計算每一步的面積，然後將指向較短線的指針向內移動。

**實現細節的關鍵差異：**

| 項目 | 倉庫 solution.c | 官方方案 2 |
|------|-----------------|----------|
| **最小高度計算** | `fmin(height[left], height[right])` | `min(height[left], height[right])` |
| **最大面積更新** | 三元運算符：`area = (area > current) ? area : current` | 函式呼叫：`area = max(area, current)` |
| **迴圈條件** | `while (right - left > 0)` | `while (left < right)` |
| **時間複雜度** | O(n) | O(n) |
| **空間複雜度** | O(1) | O(1) |

這些差異在本質上是風格化的且演算法等效。`fmin()` 函式和三元運算符是慣用的 C 語言選擇，而官方解法使用語言無關的偽碼或抽象。迴圈條件 `right - left > 0` 和 `left < right` 在指針相遇時都會終止。

### 2. 哪種方法更好：暴力法 vs. 雙指針

| 標準 | 暴力法（官方方案 1） | 雙指針（官方方案 2 & 倉庫） |
|-----|-------------------|------------------------|
| **時間複雜度** | O(n²) | O(n) |
| **空間複雜度** | O(1) | O(1) |
| **面試適用性** | 差 — 天真解法，顯示缺乏最佳化洞察 | 優秀 — 展示貪心思維和空間時間權衡意識 |
| **C 語言實現難度** | 最小 — 直觀的嵌套迴圈 | 低 — 需要理解指針移動邏輯 |
| **可擴展性** | 大輸入失敗（n > 10,000） | 高效處理大輸入 |
| **後續討論潛力** | 有限的討論價值 | 開啟貪心正確性、為何移動較短指針的討論 |

**結論：** 雙指針方法明顯更好。在面試中，先提出 O(n²) 暴力法，然後最佳化到 O(n)，展示了成熟的問題解決能力。然而，雙指針解法應該是你的目標實現。

### 3. 基準方案 vs. 推薦方案

**基準方案（暴力法）：**
- **演算法：** 檢查所有配對 (i, j)，其中 i < j，計算每個配對的面積，追蹤最大值。
- **時間複雜度：** O(n²)
- **空間複雜度：** O(1)
- **為何有效：** 窮舉列舉保證找到最大容器。
- **面試背景：** 可接受作為初始解釋，但預期會被最佳化。

**推薦方案（雙指針）：**
- **演算法：** 將指針放在兩端。計算面積。向內移動指向較短線的指針。重複直到指針相遇。
- **時間複雜度：** O(n)
- **空間複雜度：** O(1)
- **為何更好：** 減少冗餘配對檢查。貪心洞察在於，向內移動較高指針無法增加面積（高度仍然受較短線限制，寬度減少）。向內移動較短指針有可能找到更高的高度，從而增加面積。
- **面試背景：** 期望的解法。展示對最佳化和貪心演算法原則的理解。

