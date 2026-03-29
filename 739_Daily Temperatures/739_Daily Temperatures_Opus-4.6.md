# 739. Daily Temperatures — Implementation vs. Editorial Comparison

## English

### Implementation vs. Editorial Comparison

The `.c` solution in this folder (`Daily Temperatures.c`) implements **Approach 1: Monotonic Stack** from the editorial. The algorithm is identical: iterate forward through the array, maintain a monotonic decreasing stack of indices, and when a warmer temperature is found, pop all cooler days from the stack and record the distance.

The editorial also presents **Approach 2: Array, Optimized Space**, which iterates backwards and uses the `answer` array itself to "jump" forward, eliminating the need for a separate stack. This approach achieves O(1) extra space.

### Which Approach Is Better

| Dimension | Approach 1: Monotonic Stack | Approach 2: Array, Optimized Space |
|-----------|----------------------------|-------------------------------------|
| Time Complexity | O(n) | O(n) |
| Space Complexity | O(n) (stack array) | O(1) (no extra beyond output) |
| Interview Suitability | **Excellent** — classic, well-known pattern | Good — clever but harder to explain |
| C Implementation Difficulty | Low — straightforward stack with array | Low-Medium — backward iteration + jumping logic |
| Code Clarity | High — direct push/pop semantics | Medium — the "jump" trick requires careful reasoning |
| Correctness Proof | Easy — each element pushed/popped at most once | Moderate — must argue jumps don't skip valid answers |

### Baseline vs. Recommended

**Baseline approach:** Approach 1 (Monotonic Stack). This is the standard, widely-taught solution for "next greater element" type problems. It directly demonstrates understanding of the monotonic stack pattern, which appears frequently in interviews.

**Recommended approach for interview:** Approach 1 (Monotonic Stack). Although Approach 2 saves space, the monotonic stack is the canonical pattern that interviewers expect candidates to know. It generalizes cleanly to related problems (Next Greater Element I/II/III, Stock Span, Largest Rectangle in Histogram). The O(n) space cost is negligible given that the output array already requires O(n), and the stack approach is significantly easier to explain and prove correct under interview pressure.

Approach 2 is worth mentioning as a follow-up optimization if the interviewer asks about reducing space, demonstrating awareness of the tradeoff.

---

## 繁體中文

### 實作與 Editorial 對照

資料夾中的 `.c` 解法（`Daily Temperatures.c`）實作的是 Editorial 中的**方法 1：單調棧**。演算法完全一致：從前往後遍歷陣列，維護一個索引的單調遞減棧，當遇到更暖的溫度時，將棧中所有較冷的天彈出並記錄距離。

Editorial 另外提出了**方法 2：陣列、空間優化**，從後往前遍歷，利用 `answer` 陣列本身來「跳躍」前進，省去額外棧的空間，達到 O(1) 額外空間。

### 哪個方法更好

| 面向 | 方法 1：單調棧 | 方法 2：陣列、空間優化 |
|------|---------------|----------------------|
| 時間複雜度 | O(n) | O(n) |
| 空間複雜度 | O(n)（棧陣列） | O(1)（輸出之外無額外空間） |
| 面試適合度 | **極佳** — 經典、廣為人知的模式 | 良好 — 巧妙但較難解釋 |
| C 實作難度 | 低 — 直觀的陣列棧 push/pop | 低-中 — 反向遍歷 + 跳躍邏輯 |
| 程式碼清晰度 | 高 — 直接的 push/pop 語義 | 中 — 「跳躍」技巧需要仔細推理 |
| 正確性證明 | 簡單 — 每個元素最多被 push/pop 各一次 | 中等 — 需論證跳躍不會漏掉有效答案 |

### 基線方法 vs. 推薦方法

**基線方法：** 方法 1（單調棧）。這是「下一個更大元素」類型問題的標準教科書解法。它直接展現對單調棧模式的理解，此模式在面試中頻繁出現。

**面試推薦方法：** 方法 1（單調棧）。雖然方法 2 節省空間，但單調棧是面試官期望候選人掌握的經典模式。它能乾淨地推廣到相關問題（Next Greater Element I/II/III、Stock Span、Largest Rectangle in Histogram）。O(n) 的空間成本微不足道（輸出陣列本身已需要 O(n)），而且棧方法在面試壓力下顯著更容易解釋和證明正確性。

方法 2 值得作為後續優化提及——如果面試官詢問如何減少空間使用，展示你對這個權衡的認識。
