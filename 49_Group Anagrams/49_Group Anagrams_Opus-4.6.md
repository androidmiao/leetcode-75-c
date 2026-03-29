# LeetCode 49: Group Anagrams — Solution Analysis

## English

### Implementation vs. Editorial Comparison

The provided C solution uses the **"Categorize by Sorted String"** approach, which is **identical to Editorial Approach 1**. Both implementations:

1. Sort each word by its characters to create a canonical form (sorted key)
2. Use this sorted key as a grouping mechanism
3. Pair the sorted key with the original word
4. Sort all pairs by their sorted keys to co-locate anagrams
5. Perform a linear scan to group anagrams together

The C solution leverages `qsort()` to sort the (sorted_key, original_word) pairs, then iterates linearly to identify group boundaries where the sorted key changes. This is a direct implementation of the editorial's first approach.

### Which Approach is Better?

| Criterion | Sorted String | Count |
|-----------|---------------|-------|
| **Time Complexity** | O(NK log K) | O(NK) |
| **Space Complexity** | O(NK) | O(NK) |
| **Interview Suitability** | Excellent — intuitive and easy to explain | Excellent — demonstrates optimization thinking |
| **C Implementation Difficulty** | Moderate — requires string comparison functions and qsort | Higher — requires hash table or character frequency array |

**Key Observations:**

- **Time Complexity Winner:** The Count approach is theoretically superior at O(NK) versus O(NK log K), since sorting each word takes O(K log K) while counting characters takes O(K).
- **Space Complexity:** Both use O(NK) for storing and grouping results.
- **Practical Performance:** For typical inputs where K is small (word lengths are bounded), the difference between O(NK log K) and O(NK) is marginal. The sorted string approach is simpler to implement in C.
- **C Limitations:** Implementing Approach 2 (Count) in C is significantly more complex without a built-in hash table. It would require either:
  - A custom hash table implementation
  - A character frequency array with manual grouping logic
  - Both add implementation complexity and potential for bugs

### Baseline vs. Recommended

**Baseline Approach:** Categorize by Sorted String
**Recommended Approach:** Categorize by Count

**Why the Recommended Approach is Better for Interview Discussion:**

1. **Algorithmic Insight:** Demonstrating the Count approach shows you can optimize time complexity by avoiding the sorting step (O(K log K) per word becomes O(K)).

2. **Trade-off Understanding:** You can articulate that while sorting is conceptually simpler, counting characters is more efficient. This demonstrates algorithmic maturity.

3. **Scalability Argument:** For large datasets or strict time constraints, reducing NK log K to NK provides measurable improvements.

4. **Practical Caveat:** However, you should note that in C, the implementation of a hash table or robust grouping mechanism for the Count approach is more involved. This shows you understand implementation trade-offs versus pure algorithmic superiority.

5. **Interview Strategy:** The best answer acknowledges both approaches:
   - Start with Sorted String (simpler, intuitive)
   - Explain why Count is better (lower time complexity)
   - Discuss the practical C implementation challenge
   - Decide together with the interviewer which to code (Sorted String for simplicity, Count for optimization)

This balanced discussion demonstrates both algorithmic depth and practical engineering judgment.

---

## 繁體中文

### 實現與官方題解比較

提供的 C 語言解決方案採用 **「按排序字符串分類」** 方法，**與官方題解方法 1 完全相同**。兩種實現都：

1. 將每個單詞按字符排序，生成標準形式（排序後的鑰匙）
2. 使用排序後的鑰匙作為分組機制
3. 將排序後的鑰匙與原始單詞配對
4. 按排序後的鑰匙對所有配對進行排序，使同義詞彙靠在一起
5. 執行線性掃描以識別分組邊界

C 語言解決方案利用 `qsort()` 對（排序鑰匙、原始單詞）配對進行排序，然後線性迭代以識別排序鑰匙改變時的分組邊界。這是官方題解第一種方法的直接實現。

### 哪種方法更好？

| 評估標準 | 排序字符串 | 字符計數 |
|---------|-----------|---------|
| **時間複雜度** | O(NK log K) | O(NK) |
| **空間複雜度** | O(NK) | O(NK) |
| **面試適合度** | 優秀 — 直觀且易於解釋 | 優秀 — 展示優化思維 |
| **C 語言實現難度** | 中等 — 需要字符串比較函數和 qsort | 較高 — 需要雜湊表或字符頻率陣列 |

**關鍵觀察：**

- **時間複雜度優勝者：** 字符計數方法在理論上優於排序方法，O(NK) 對比 O(NK log K)，因為對每個單詞排序需要 O(K log K)，而計數字符只需 O(K)。
- **空間複雜度：** 兩種方法都使用 O(NK) 來存儲和分組結果。
- **實際性能：** 對於典型輸入（單詞長度有限），O(NK log K) 和 O(NK) 之間的差異微乎其微。排序字符串方法在 C 中更易實現。
- **C 語言限制：** 在 C 中實現方法 2（字符計數）明顯更複雜，因為沒有內置雜湊表。實現方案包括：
  - 自定義雜湊表實現
  - 字符頻率陣列搭配手動分組邏輯
  - 兩種方式都增加了實現複雜性和潛在錯誤

### 基準方案 vs. 推薦方案

**基準方案：** 按排序字符串分類
**推薦方案：** 按字符計數分類

**推薦方案為何更適合面試討論：**

1. **算法洞察力：** 展示字符計數方法說明你能夠通過避免排序步驟來優化時間複雜度（每個單詞的 O(K log K) 變為 O(K)）。

2. **權衡理解：** 你能夠明確表達，雖然排序在概念上更簡單，但計數字符更有效率。這展示了算法成熟度。

3. **可擴展性論證：** 對於大型數據集或嚴格的時間約束，將 NK log K 減少到 NK 提供可衡量的改進。

4. **實踐注意事項：** 但你應該指出，在 C 中，為字符計數方法實現雜湊表或穩健的分組機制涉及更複雜的代碼。這表明你理解實現權衡與純粹算法優越性之間的差異。

5. **面試策略：** 最佳答案同時承認兩種方法：
   - 從排序字符串開始（更簡單、更直觀）
   - 解釋為什麼字符計數更好（更低的時間複雜度）
   - 討論實踐中 C 語言實現的挑戰
   - 與面試官共同決定要編寫的代碼（排序字符串以簡化，字符計數以優化）

這種平衡的討論展示了算法深度和實踐工程判斷力。
