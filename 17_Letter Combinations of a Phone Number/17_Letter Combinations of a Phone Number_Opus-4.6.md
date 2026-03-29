# LeetCode 17: Letter Combinations of a Phone Number — Solution Analysis

## English

### Implementation vs. Editorial Comparison

The provided C solution uses **backtracking** — the identical algorithmic approach as the LeetCode editorial's Approach 1. Both solutions:
- Build letter combinations recursively by mapping each digit to its corresponding phone keypad letters
- Use backtracking to explore all possible letter combinations
- Return all valid combinations for the input digits

The key implementation differences reflect C-specific optimizations:

| Aspect | Editorial (Hash Map) | C Solution (Static Array) |
|--------|----------------------|--------------------------|
| **Digit-to-Letters Mapping** | Hash map data structure | Static const array `PHONE_MAP[]` with indices 0–9 |
| **Path Management** | Explicitly remove letters after recursive calls | Implicitly handled by overwriting `path[index]` in next iteration |
| **Memory Allocation** | Output array size often not pre-calculated | Pre-calculates `totalCombinations` to malloc exact result array size |
| **Time Complexity** | O(4^N * N) where N is input length | O(4^N * N) — identical |
| **Space Complexity** | O(N) for recursion depth (excluding output) | O(N) for recursion depth (excluding output) — identical |

### Which Approach is Better?

| Criterion | Evaluation |
|-----------|-----------|
| **Time Complexity** | Both are equivalent at O(4^N * N). The C solution's static array lookup is actually faster than editorial's hash map lookup. |
| **Space Complexity** | Both are equivalent at O(N) recursion depth (excluding output). The C solution's pre-calculated array size avoids dynamic resizing. |
| **Interview Suitability** | The backtracking approach is excellent for interviews. The editorial and C solution are equally suitable — the algorithm is the same. The C solution demonstrates understanding of C-specific memory optimization. |
| **C Implementation Difficulty** | The C solution is **more elegant**: static const array lookup is simpler than implementing/managing a hash map in C. The implicit path overwriting is idiomatic C. Pre-calculating array size shows practical C knowledge. |

**Conclusion:** Since the editorial presents only one approach (backtracking) and the C solution implements that same approach, they are **algorithmically equivalent**. The C solution is the better **implementation** due to more efficient data structures (array vs. hash map) and practical memory management suited to C.

### Baseline vs. Recommended

**Baseline Approach:** Backtracking
**Recommended Approach:** Backtracking

The C solution's backtracking approach **is the only approach** presented in the editorial, and it **is also the recommended approach**. Here's why:

1. **Optimal Complexity:** Backtracking achieves O(4^N * N) time and O(N) space (excluding output), which is optimal — you must generate all combinations, and each combination has up to N letters.

2. **Natural Problem Structure:** The digit-to-letter mapping on a phone keypad naturally lends itself to recursive exploration. Backtracking directly mirrors the problem's decision tree.

3. **Interview Gold Standard:** Backtracking is a fundamental technique tested in technical interviews. This solution demonstrates:
   - Clear understanding of recursive decomposition
   - Proper handling of base cases
   - Effective path management (whether explicit or implicit)

4. **C-Specific Excellence:** The solution showcases practical C programming:
   - Efficient static array lookup instead of hash map overhead
   - Smart pre-calculation of result array size to avoid reallocation
   - Idiomatic use of array indexing for path management

5. **No Better Alternative:** For this problem, no alternative approach (iterative, dynamic programming, etc.) would be better. Backtracking is the natural and optimal fit.

**Recommendation:** Use this C backtracking solution as your model. It is algorithmically optimal, interview-ready, and demonstrates excellent C programming practices.

---

## 繁體中文

### 實現與官方編輯區解法的比較

提供的 C 語言解決方案使用 **回溯法 (Backtracking)** — 與 LeetCode 官方編輯區方案 1 完全相同的算法方法。兩種解決方案都：
- 通過遞迴映射每個數字到對應的電話鍵盤字母來構建字母組合
- 使用回溯法探索所有可能的字母組合
- 返回輸入數字的所有有效組合

主要的實現差異反映了 C 語言特有的優化：

| 方面 | 官方編輯區 (雜湊表) | C 語言解決方案 (靜態陣列) |
|------|---------------------|-------------------------|
| **數字到字母映射** | 雜湊表資料結構 | 靜態 const 陣列 `PHONE_MAP[]`，索引 0–9 |
| **路徑管理** | 在遞迴呼叫後明確移除字母 | 通過在下一次迭代中覆寫 `path[index]` 來隱含處理 |
| **記憶體配置** | 輸出陣列大小通常不預先計算 | 預先計算 `totalCombinations` 以 malloc 精確大小的結果陣列 |
| **時間複雜度** | O(4^N * N)，其中 N 為輸入長度 | O(4^N * N) — 相同 |
| **空間複雜度** | O(N) 用於遞迴深度 (不含輸出) | O(N) 用於遞迴深度 (不含輸出) — 相同 |

### 哪種方法更好？

| 評估標準 | 評價 |
|---------|------|
| **時間複雜度** | 兩者相同，均為 O(4^N * N)。C 語言解決方案的靜態陣列查詢實際上比官方編輯區的雜湊表查詢更快。 |
| **空間複雜度** | 兩者相同，均為 O(N) 遞迴深度 (不含輸出)。C 語言解決方案預先計算的陣列大小避免了動態調整大小。 |
| **面試適配性** | 回溯法方法非常適合面試。官方編輯區和 C 語言解決方案同樣適合 — 算法完全相同。C 語言解決方案展示了對 C 語言特有記憶體優化的理解。 |
| **C 語言實現難度** | C 語言解決方案 **更優雅**：靜態 const 陣列查詢比在 C 語言中實現/管理雜湊表更簡單。隱含的路徑覆寫是慣用的 C 語言寫法。預先計算陣列大小體現了實用的 C 語言知識。 |

**結論：** 由於官方編輯區僅呈現一種方法 (回溯法)，且 C 語言解決方案實現了該相同方法，它們在 **算法上是等價的**。C 語言解決方案是更好的 **實現**，因為它使用了更高效的資料結構 (陣列 vs. 雜湊表) 以及適合 C 語言的實用記憶體管理。

### 基礎方法 vs. 推薦方法

**基礎方法：** 回溯法
**推薦方法：** 回溯法

C 語言解決方案的回溯法方法 **是官方編輯區唯一呈現的方法**，也 **是推薦的方法**。以下是原因：

1. **最優複雜度：** 回溯法達到 O(4^N * N) 時間和 O(N) 空間 (不含輸出)，這是最優的 — 您必須生成所有組合，且每個組合最多包含 N 個字母。

2. **自然的問題結構：** 電話鍵盤上的數字到字母映射自然地適合遞迴探索。回溯法直接反映了問題的決策樹。

3. **面試黃金標準：** 回溯法是技術面試中測試的基礎技術。此解決方案展示了：
   - 對遞迴分解的清晰理解
   - 正確處理基本情況
   - 有效的路徑管理 (無論是明確還是隱含)

4. **C 語言特有優勢：** 該解決方案展示了實用的 C 語言程式設計：
   - 高效的靜態陣列查詢而非雜湊表開銷
   - 智慧地預先計算結果陣列大小以避免重新配置
   - 慣用的陣列索引用於路徑管理

5. **沒有更好的替代方案：** 對於此問題，沒有替代方法 (迭代法、動態規劃等) 會更好。回溯法是自然且最優的選擇。

**推薦：** 將此 C 語言回溯法解決方案作為您的模型。它在算法上是最優的、適合面試的，並展示了卓越的 C 語言程式設計實踐。
