# LeetCode 2390: Removing Stars From a String
## Implementation vs. Editorial Analysis

---

## English Version

### Overview

This document analyzes the C solution for LeetCode problem 2390 "Removing Stars From a String" and compares it against the three approaches presented in the Editorial.

**Problem Statement:** Given a string `s` containing lowercase English letters and stars (`*`), remove each star along with its closest non-star character to the left. Return the resulting string.

---

### 1. Implementation vs. Editorial Comparison

#### The C Solution (Implemented)

The C implementation uses a **stack-thinking two-pointer in-place approach** that directly modifies the input string `s` without allocating extra data structures.

**Key Characteristics:**
- **Two pointers:** `r` (read index) scans the original string left-to-right
- **Write pointer:** `w` (write index) maintains the position for writing valid characters
- **In-place modification:** The string `s[0..w-1]` always represents the current valid result
- **Space optimization:** No separate array or stack container; `s` itself serves as the working space
- **Logic:**
  - When encountering a non-star character: write it to `s[w]` and increment `w`
  - When encountering a star: decrement `w` (conceptually removing the last valid character)
  - Finalize with null terminator at `s[w]`

**Relationship to Editorial Approaches:**

| Aspect | Editorial Approach 3 (Two Pointers) | C Solution |
|--------|--------------------------------------|-----------|
| **Core Logic** | Same: two-pointer tracking | Identical core algorithm |
| **Data Structure** | Creates separate array `ch[]` | Reuses original string `s` |
| **Extra Space** | O(n) for `ch[]` array | O(1) - no extra allocation |
| **Implementation** | Multi-language friendly | C-optimized in-place variant |
| **Conceptual Basis** | Stack thinking via two pointers | Stack thinking via two pointers |

**The C solution is an optimized variant of Approach 3 that combines:**
1. The two-pointer logic from Editorial Approach 3
2. The stack-based thinking from Editorial Approach 1
3. In-place string modification for O(1) extra space (C-specific advantage)

---

### 2. Approach Comparison Table

| Criterion | Approach 1: Stack | Approach 2: String | Approach 3: Two Pointers | **C Solution (In-Place)** |
|-----------|-------------------|-------------------|--------------------------|--------------------------|
| **Time Complexity** | O(n) | O(n) | O(n) | **O(n)** |
| **Space Complexity** | O(n) | O(n) | O(n) | **O(1) extra space** |
| **Extra Space (Details)** | Stack container | String/StringBuilder | Separate array `ch[]` | None (modifies `s`) |
| **Conceptual Clarity** | High (explicit stack) | Very high (intuitive) | Medium (pointer mechanics) | Medium (pointer mechanics) |
| **Interview Suitability** | Excellent | Excellent (language-dependent) | Good | Excellent (C context) |
| **C Implementation Difficulty** | Medium | Medium | Medium | **Low (straightforward)** |
| **Real-world Performance** | Good | Fair/Good (depends on language) | Good | **Excellent (no allocations)** |
| **Memory Efficiency** | Poor (O(n) overhead) | Poor (O(n) overhead) | Poor (O(n) overhead) | **Excellent (O(1) overhead)** |

**Winner by Category:**
- **Best Overall (Interview):** Approach 2 (String) - most intuitive, but requires language support
- **Best for C:** C Solution (In-Place) - optimal space complexity for C environment
- **Best for Teaching:** Approach 1 (Stack) - clearest conceptual model
- **Best Space Efficiency:** C Solution (In-Place) - only O(1) extra space

---

### 3. Detailed Comparison

#### Approach 1: Stack (Editorial)
**How it works:**
- Push non-star characters onto a stack
- Pop the stack when encountering a star
- After processing, reverse the stack contents to form the answer

**Pros:**
- Most intuitive and pedagogically clear
- Directly models the problem semantics
- Easy to implement in any language with stack support

**Cons:**
- Always allocates O(n) space for the stack
- Requires reversal step at the end
- Two passes of data (one for stack operations, one for reversal)

---

#### Approach 2: String (Editorial)
**How it works:**
- Build a result string incrementally
- Append non-star characters
- Remove the last character when encountering a star

**Pros:**
- Extremely intuitive and simple
- Single forward pass
- Works well in languages with mutable strings (C, C++)

**Cons:**
- Always allocates O(n) space for the result
- String deletion operations can be expensive in some languages
- Doesn't work efficiently in Python (O(n²) due to immutable strings)

---

#### Approach 3: Two Pointers with Separate Array (Editorial)
**How it works:**
- Create a separate character array `ch[]` of size n
- Use `i` to read from original string
- Use `j` to write/manage positions in `ch[]`
- When encountering a star, decrement `j` (removes last character)
- Return substring `ch[0..j-1]`

**Pros:**
- More sophisticated understanding of pointer mechanics
- Avoids stack reversal overhead
- Clear separation of input and output

**Cons:**
- Always allocates O(n) space for `ch[]`
- Requires another pass to construct final string from `ch[]`
- Less intuitive than string or stack approaches

---

#### C Solution (In-Place Variant)
**How it works:**
- Combines Approach 3 logic with in-place modification
- Reuses the input string `s` instead of allocating `ch[]`
- `w` pointer serves dual purpose: write position AND stack size indicator
- Invariant: `s[0..w-1]` always contains current valid result
- Final step: null-terminate at `s[w]`

**Pros:**
- **Only O(1) extra space** (no separate array needed)
- Eliminates allocation overhead
- Maintains all algorithm clarity of Approach 3
- Ideal for C environments where in-place modification is expected
- Single pass through string
- Direct modification makes it cache-friendly

**Cons:**
- Modifies input string (may not be acceptable in all problems)
- Slightly more advanced pointer manipulation
- Less portable across languages

---

### 4. Baseline vs. Recommended

#### Baseline Approach: Approach 1 (Stack)

**Why Baseline:**
- Most straightforward conceptually
- Requires no advanced pointer understanding
- Recommended as first-pass solution in interviews
- Clear mental model: "What would a stack do?"

**When to Use:**
- First attempt in interview
- Teaching/learning phase
- When code clarity is paramount
- Languages without good string support

**Example Mindset:** "Each star pops the stack. Non-stars push to stack. Reverse at end."

---

#### Recommended Approach: C Solution (In-Place Two Pointers)

**Why Recommended:**

1. **Optimal Space Complexity:** O(1) extra space vs. O(n) for all editorial approaches
2. **Best for C:** Leverages C's capability for direct memory manipulation
3. **Interview Excellence:** Shows understanding of:
   - Stack-based thinking
   - Two-pointer technique
   - In-place algorithm optimization
   - Memory efficiency awareness
4. **Production Code:** Most efficient implementation possible
5. **Performance:** No heap allocations, better cache locality

**When to Use:**
- Production C code where efficiency matters
- Advanced interviews (especially with language specification)
- Space-constrained environments
- When interviewer asks "Can you optimize further?"

**Example Mindset:** "Stack thinking, but use the input string itself as our workspace. `w` is both write position and stack depth. Works perfectly in C."

---

### 5. Step-by-Step Walkthrough: C Solution

**Example:** `s = "leet**cod*e"` should produce `"lecoe"`

| Step | r | Character | Action | w | Valid Range | Result |
|------|---|-----------|--------|---|-------------|--------|
| 0 | 0 | 'l' | Write to s[0], w++ | 1 | s[0..0] | "l" |
| 1 | 1 | 'e' | Write to s[1], w++ | 2 | s[0..1] | "le" |
| 2 | 2 | 'e' | Write to s[2], w++ | 3 | s[0..2] | "lee" |
| 3 | 3 | 't' | Write to s[3], w++ | 4 | s[0..3] | "leet" |
| 4 | 4 | '*' | w-- | 3 | s[0..2] | "lee" |
| 5 | 5 | '*' | w-- | 2 | s[0..1] | "le" |
| 6 | 6 | 'c' | Write to s[2], w++ | 3 | s[0..2] | "lec" |
| 7 | 7 | 'o' | Write to s[3], w++ | 4 | s[0..3] | "leco" |
| 8 | 8 | 'd' | Write to s[4], w++ | 5 | s[0..4] | "lecod" |
| 9 | 9 | '*' | w-- | 4 | s[0..3] | "leco" |
| 10 | 10 | 'e' | Write to s[4], w++ | 5 | s[0..4] | "lecoe" |
| Final | — | — | s[5] = '\0' | 5 | s[0..4] | "lecoe" |

**Key Insight:** The write pointer `w` never advances beyond `r`, so earlier positions that are "overwritten" are never read again. This guarantees correctness.

---

### 6. Complexity Analysis

#### Time Complexity: O(n)
All approaches process each character exactly once:
- Single pass through the string
- Each character either pushes, appends, or triggers a removal (all O(1))
- Total: O(n)

#### Space Complexity Comparison:

| Approach | Extra Space | Explanation |
|----------|------------|-------------|
| Stack | O(n) | Stack container holds up to n characters |
| String | O(n) | Result string grows to up to n characters |
| Two Pointers (Editorial) | O(n) | Separate array `ch[]` of size n |
| **C In-Place** | **O(1)** | Only pointers `r` and `w`; no separate structure |

The C solution achieves O(1) extra space by reusing the input string, which is a significant optimization impossible in other editorial approaches without modification.

---

### 7. Implementation Considerations

#### Why In-Place Works in C

1. **String Mutability:** C strings are arrays of characters, fully mutable
2. **Memory Reuse:** The algorithm never reads from positions beyond the current write position
3. **Correctness:** Since `w` ≤ `r` always, overwritten characters are never accessed again
4. **Null Terminator:** Properly placing `s[w] = '\0'` finalizes the string

#### Potential Concerns

- **Input Modification:** Problem statement typically allows string modification in C
- **Thread Safety:** If `s` is shared, this approach creates synchronization issues
- **Aliasing:** Works correctly due to the `w ≤ r` invariant

---

### 8. Interview Approach Strategy

**If asked this problem in an interview:**

1. **First Response (Safest):** Explain Approach 1 (Stack)
   - Clear, correct, easy to code
   - Demonstrates fundamental problem understanding

2. **Follow-up (Show Optimization):** Suggest Approach 2 (String) or 3 (Two Pointers)
   - Shows awareness of alternative techniques

3. **If Interviewer Says "Can you optimize space?"**
   - Explain the in-place C solution
   - Shows deep understanding of the problem
   - Demonstrates C-specific optimization skills

**Code Quality Order (for interview):**
1. Get working (Stack/String) → 5-10 min
2. Discuss tradeoffs → 2-3 min
3. Optimize to in-place if time permits → 5 min

---

### 9. Real-World Usage

| Scenario | Recommended Approach |
|----------|---------------------|
| **Quick coding challenge** | Stack (clarity) |
| **Production C code** | In-place Two Pointers (efficiency) |
| **Embedded systems** | In-place Two Pointers (minimal memory) |
| **Teaching/learning** | Stack (intuitive) |
| **Python/Java/JavaScript** | Stack or String (language-appropriate) |
| **Competitive programming** | In-place (time/space optimization) |

---

### 10. Key Takeaways

1. **Same Core Logic:** All approaches use the same fundamental idea (remove pair when star encountered)
2. **Space Optimization:** The C solution achieves O(1) by reusing input; others need O(n)
3. **Implementation Matters:** Language choice affects which approach is optimal
4. **Interview Strategy:** Start simple (Stack), show alternatives, optimize if prompted
5. **Production Code:** In-place is superior in C for efficiency
6. **Correctness:** The `w ≤ r` invariant ensures all approaches are correct

---

---

## Traditional Chinese Version (繁體中文)

### 概述

本文檔分析 LeetCode 問題 2390 「移除字串中的星號」的 C 實現，並與 Editorial 中的三種方法進行比較。

**問題陳述：** 給定一個包含小寫英文字母和星號（`*`）的字串 `s`，移除每個星號及其左邊最近的非星號字元。回傳處理後的字串。

---

### 1. 實現與 Editorial 的比較

#### C 實現方案（已實現）

C 實現採用**棧思想 + 雙指標原地修改方法**，直接修改輸入字串 `s`，不分配額外的資料結構。

**核心特性：**
- **雙指標：** `r`（讀取索引）從左到右掃描原字串
- **寫入指標：** `w`（寫入索引）維護寫入有效字元的位置
- **原地修改：** 字串 `s[0..w-1]` 始終表示當前有效結果
- **空間最佳化：** 不需要單獨陣列或棧容器；`s` 本身充當工作空間
- **邏輯：**
  - 遇到非星號字元：寫入 `s[w]` 並增加 `w`
  - 遇到星號：減少 `w`（概念上移除最後一個有效字元）
  - 最後在 `s[w]` 加上空字元終止符

**與 Editorial 方法的關係：**

| 方面 | Editorial 方法三（雙指標） | C 實現 |
|------|--------------------------|-------|
| **核心邏輯** | 相同：雙指標追蹤 | 相同的核心演算法 |
| **資料結構** | 建立單獨陣列 `ch[]` | 重用原字串 `s` |
| **額外空間** | O(n) 用於 `ch[]` | O(1) - 無額外分配 |
| **實現** | 多語言友善 | C 最佳化原地變體 |
| **概念基礎** | 棧思想透過雙指標 | 棧思想透過雙指標 |

**C 實現是結合以下特性的最佳化變體：**
1. Editorial 方法三的雙指標邏輯
2. Editorial 方法一的棧基礎思想
3. 原地字串修改實現 O(1) 額外空間（C 特定優勢）

---

### 2. 方法比較表

| 評估項目 | 方法一：棧 | 方法二：字串 | 方法三：雙指標 | **C 實現（原地）** |
|--------|----------|-----------|-------------|----------------|
| **時間複雜度** | O(n) | O(n) | O(n) | **O(n)** |
| **空間複雜度** | O(n) | O(n) | O(n) | **O(1) 額外空間** |
| **額外空間詳情** | 棧容器 | 字串/StringBuilder | 單獨陣列 `ch[]` | 無（修改 `s`） |
| **概念清晰度** | 高（顯式棧） | 非常高（直觀） | 中等（指標機制） | 中等（指標機制） |
| **面試適合度** | 優秀 | 優秀（語言相依） | 良好 | **優秀（C 環境）** |
| **C 實現難度** | 中等 | 中等 | 中等 | **低（直接）** |
| **實務性能** | 良好 | 一般/良好 | 良好 | **優秀（無分配）** |
| **記憶體效率** | 差（O(n) 開銷） | 差（O(n) 開銷） | 差（O(n) 開銷） | **優秀（O(1) 開銷）** |

**各類別優勝者：**
- **整體最佳（面試）：** 方法二（字串）- 最直觀，但需要語言支援
- **C 最佳：** C 實現（原地）- 空間複雜度對 C 環境最優
- **教學最佳：** 方法一（棧）- 概念模型最清晰
- **空間效率最佳：** C 實現（原地）- 唯一 O(1) 額外空間

---

### 3. 詳細比較

#### 方法一：棧（Editorial）
**運作原理：**
- 非星號字元 push 進棧
- 遇到星號時 pop 棧
- 處理完後，反轉棧內容組成答案

**優點：**
- 最直觀且教學效果最佳
- 直接建模問題語義
- 任何有棧支援的語言都容易實現

**缺點：**
- 總是分配 O(n) 空間用於棧
- 末尾需要反轉步驟
- 資料兩次通過（棧操作 + 反轉）

---

#### 方法二：字串（Editorial）
**運作原理：**
- 漸進式建構結果字串
- 追加非星號字元
- 遇到星號時移除最後一個字元

**優點：**
- 極其直觀和簡單
- 單向通過
- 在可變字串的語言中表現良好（C、C++）

**缺點：**
- 總是為結果分配 O(n) 空間
- 字串刪除操作在某些語言可能很昂貴
- 在 Python 中效率低（因不可變字串，O(n²)）

---

#### 方法三：雙指標與單獨陣列（Editorial）
**運作原理：**
- 建立大小為 n 的單獨字元陣列 `ch[]`
- 用 `i` 讀取原字串
- 用 `j` 在 `ch[]` 中寫入/管理位置
- 遇到星號時，減少 `j`（移除最後一個字元）
- 回傳子字串 `ch[0..j-1]`

**優點：**
- 更進階的指標機制理解
- 避免棧反轉開銷
- 輸入和輸出清晰分離

**缺點：**
- 總是為 `ch[]` 分配 O(n) 空間
- 需要另外通過構建 `ch[]` 的最終字串
- 不如字串或棧方法直觀

---

#### C 實現（原地變體）
**運作原理：**
- 結合方法三的邏輯與原地修改
- 重用輸入字串 `s` 而非分配 `ch[]`
- `w` 指標雙重用途：寫入位置 AND 棧大小指示器
- 不變性：`s[0..w-1]` 始終包含當前有效結果
- 最後步驟：在 `s[w]` 處空字元終止

**優點：**
- **僅需 O(1) 額外空間**（無需單獨陣列）
- 消除分配開銷
- 保持方法三的所有演算法清晰度
- C 環境中理想（原地修改是預期行為）
- 單次通過字串
- 直接修改提供更好的快取友善性

**缺點：**
- 修改輸入字串（在某些問題中可能不被接受）
- 指標操作略微進階
- 語言可攜性較低

---

### 4. 基線 vs 推薦方案

#### 基線方案：方法一（棧）

**為何為基線：**
- 概念上最直接
- 無需進階指標理解
- 面試中推薦首次嘗試
- 清晰的心智模型：「棧會做什麼？」

**何時使用：**
- 面試首次嘗試
- 教學/學習階段
- 程式碼清晰度至上
- 沒有良好字串支援的語言

**思維方式：** 「每個星號都 pop 棧。非星號 push 進棧。最後反轉。」

---

#### 推薦方案：C 實現（原地雙指標）

**為何推薦：**

1. **最佳空間複雜度：** O(1) 額外空間，vs 所有 Editorial 方法的 O(n)
2. **最適合 C：** 充分利用 C 的直接記憶體操作能力
3. **面試優秀：** 展示對以下的理解：
   - 棧思想
   - 雙指標技術
   - 原地演算法最佳化
   - 記憶體效率認知
4. **生產代碼：** 可能的最有效實現
5. **性能：** 無堆分配，更好的快取局部性

**何時使用：**
- 生產 C 代碼（效率重要）
- 進階面試（特別是指定語言）
- 空間受限環境
- 面試官問「能進一步最佳化嗎？」

**思維方式：** 「棧思想，但把輸入字串本身當作工作空間。`w` 既是寫入位置又是棧深度。在 C 中完美運作。」

---

### 5. 步驟詳解：C 實現

**範例：** `s = "leet**cod*e"` 應產生 `"lecoe"`

| 步驟 | r | 字元 | 操作 | w | 有效範圍 | 結果 |
|------|---|------|------|---|---------|------|
| 0 | 0 | 'l' | 寫入 s[0]，w++ | 1 | s[0..0] | "l" |
| 1 | 1 | 'e' | 寫入 s[1]，w++ | 2 | s[0..1] | "le" |
| 2 | 2 | 'e' | 寫入 s[2]，w++ | 3 | s[0..2] | "lee" |
| 3 | 3 | 't' | 寫入 s[3]，w++ | 4 | s[0..3] | "leet" |
| 4 | 4 | '*' | w-- | 3 | s[0..2] | "lee" |
| 5 | 5 | '*' | w-- | 2 | s[0..1] | "le" |
| 6 | 6 | 'c' | 寫入 s[2]，w++ | 3 | s[0..2] | "lec" |
| 7 | 7 | 'o' | 寫入 s[3]，w++ | 4 | s[0..3] | "leco" |
| 8 | 8 | 'd' | 寫入 s[4]，w++ | 5 | s[0..4] | "lecod" |
| 9 | 9 | '*' | w-- | 4 | s[0..3] | "leco" |
| 10 | 10 | 'e' | 寫入 s[4]，w++ | 5 | s[0..4] | "lecoe" |
| 最後 | — | — | s[5] = '\0' | 5 | s[0..4] | "lecoe" |

**關鍵洞察：** 寫入指標 `w` 永不超越 `r`，因此「覆寫」的早期位置永不再被讀取。這保證了正確性。

---

### 6. 複雜度分析

#### 時間複雜度：O(n)
所有方法都精確處理每個字元一次：
- 通過字串的單次通過
- 每個字元要麼 push、追加或觸發移除（都是 O(1)）
- 總計：O(n)

#### 空間複雜度比較：

| 方法 | 額外空間 | 解釋 |
|------|--------|------|
| 棧 | O(n) | 棧容器最多容納 n 個字元 |
| 字串 | O(n) | 結果字串最多成長到 n 個字元 |
| 雙指標（Editorial） | O(n) | 大小為 n 的單獨陣列 `ch[]` |
| **C 原地** | **O(1)** | 僅指標 `r` 和 `w`；無單獨結構 |

C 實現透過重用輸入字串實現 O(1) 額外空間，這是其他 Editorial 方法無法修改實現的重大最佳化。

---

### 7. 實現考量

#### 為何原地在 C 中有效

1. **字串可變性：** C 字串是字元陣列，完全可變
2. **記憶體重用：** 演算法永不讀取當前寫入位置之外
3. **正確性：** 因為 `w` ≤ `r` 始終成立，覆寫的字元永不再被訪問
4. **空字元終止：** 正確放置 `s[w] = '\0'` 完成字串

#### 潛在顧慮

- **輸入修改：** 問題陳述通常允許 C 中的字串修改
- **執行緒安全：** 如果 `s` 被共用，此方法建立同步問題
- **別名化：** 因 `w ≤ `r` 不變性而正確運作

---

### 8. 面試方法策略

**如果在面試中遇到此問題：**

1. **首次回應（最安全）：** 解釋方法一（棧）
   - 清晰、正確、易於編碼
   - 展示基本問題理解

2. **後續（顯示最佳化）：** 建議方法二（字串）或方法三（雙指標）
   - 展示替代技術意識

3. **如面試官說「能最佳化空間嗎？」**
   - 解釋原地 C 實現
   - 顯示深刻的問題理解
   - 展示 C 特定最佳化技能

**面試代碼品質順序：**
1. 使其工作（棧/字串）→ 5-10 分鐘
2. 討論權衡 → 2-3 分鐘
3. 如有時間最佳化為原地 → 5 分鐘

---

### 9. 實務應用

| 情境 | 推薦方案 |
|------|--------|
| **快速編碼挑戰** | 棧（清晰度） |
| **生產 C 代碼** | 原地雙指標（效率） |
| **嵌入式系統** | 原地雙指標（最小記憶體） |
| **教學/學習** | 棧（直觀） |
| **Python/Java/JavaScript** | 棧或字串（語言適當） |
| **競賽編程** | 原地（時間/空間最佳化） |

---

### 10. 要點總結

1. **相同核心邏輯：** 所有方法使用相同基本思想（星號時移除配對）
2. **空間最佳化：** C 實現透過重用輸入實現 O(1)；其他需要 O(n)
3. **實現重要：** 語言選擇影響哪種方法最優
4. **面試策略：** 先簡單（棧），展示替代方案，提示時最佳化
5. **生產代碼：** 原地在 C 中效率上優越
6. **正確性：** `w ≤ r` 不變性確保所有方法正確

---

## Summary

This document provides a comprehensive analysis of LeetCode 2390 with three key insights:

1. **The C solution is an optimized in-place variant of Editorial Approach 3**, combining stack thinking with two-pointer technique while achieving O(1) extra space.

2. **Approach 1 (Stack) is the baseline** for clarity and ease of understanding, making it ideal for initial interview responses.

3. **The C in-place solution is the recommended approach** for production code and advanced interviews, as it demonstrates superior space efficiency and understanding of C-specific optimizations.

The comparison table and walkthroughs provide clear guidance for choosing the right approach based on context, language, and problem constraints.
