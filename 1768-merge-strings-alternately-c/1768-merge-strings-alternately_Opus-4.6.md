# LeetCode 1768: Merge Strings Alternately
## Implementation vs. Editorial Comparison

---

## English Section

### Overview
LeetCode problem 1768 requires merging two strings by adding characters in alternating order, starting with the first string. If one string is longer, remaining characters are appended to the end.

### Repository Implementation Analysis

The repository's C implementation (`1768-merge-strings-alternately.c`) uses the **Two Pointers approach**:

```c
char* mergeAlternately(char* word1, char* word2) {
    size_t n1 = strlen(word1);
    size_t n2 = strlen(word2);

    char *res = malloc(n1 + n2 + 1);
    if (res == NULL) return NULL;

    size_t i = 0, j = 0, k = 0;

    while (i < n1 || j < n2) {
        if (i < n1) res[k++] = word1[i++];
        if (j < n2) res[k++] = word2[j++];
    }

    res[k] = '\0';
    return res;
}
```

**Key characteristics:**
- Uses two pointers (`i`, `j`) for input strings and one pointer (`k`) for output
- Single `while` loop: `while (i < n1 || j < n2)`
- Two independent `if` statements inside the loop allow natural alternation and tail-appending
- Returns early if malloc fails
- Uses `size_t` for lengths and indices (memory-safe)

### Editorial Approaches

**Approach 1: Two Pointers** — Matches the repository implementation exactly
- Maintains two pointers for traversing both strings
- Alternately appends from each string until both are exhausted
- O(m+n) time, O(1) auxiliary space

**Approach 2: One Pointer** — Alternative approach
- Single pointer `i` iterates from 0 to max(m, n)
- At each iteration, conditionally append from both strings if indices are within bounds
- Same complexity: O(m+n) time, O(1) auxiliary space

### Comparison Table

| Criterion | Two Pointers (Repository) | One Pointer (Editorial Alt.) |
|-----------|---------------------------|------------------------------|
| **Time Complexity** | O(m+n) | O(m+n) |
| **Auxiliary Space** | O(1) | O(1) |
| **Code Clarity** | Excellent — two independent conditions make alternation explicit | Good — single loop, but requires two bounds checks per iteration |
| **Loop Structure** | `while (i < n1 \|\| j < n2)` — stops when both exhausted | `for (i = 0; i < max(m,n))` — fixed iterations, up to longer string |
| **Interview Readability** | Very intuitive: each pointer represents one string's position | Slightly less intuitive: requires understanding bounds checks |
| **C Implementation Ease** | Direct translation; no special constructs needed | Requires computing max(m,n) or nested loop bounds |
| **Memory Safety** | Easy to implement with malloc/free pattern | Same memory pattern |
| **Scalability** | Handles unequal string lengths naturally | Also natural but requires explicit bounds checks |

### Baseline vs. Recommended Approach

**Baseline Approach:** Two Pointers (Editorial Approach 1)
- This is the standard solution taught in most DSA courses
- Most intuitive for humans: "one pointer for each string"
- Repository implementation aligns perfectly with this

**Recommended Approach:** Two Pointers (Editorial Approach 1 — same as repository)
- **Why:** The repository's implementation is already optimal
- Clear and concise C code that minimizes cognitive load
- The `while (i < n1 || j < n2)` condition is a standard idiom for merging two sequences
- Memory-safe with proper malloc error handling
- Uses `size_t` (appropriate for indices and strlen return values)
- Each if-statement inside the loop is independent and easy to verify for correctness

**Alternative consideration (Approach 2):** The One Pointer method is valid but offers no advantage:
- Requires computing `max(m, n)` upfront or using a helper function
- Two bounds checks per iteration (slightly more CPU work)
- Less intuitive in most interviews

### Conclusion

The repository's standalone C implementation is **correctly aligned** with Editorial Approach 1 (Two Pointers). It is **production-ready** and represents an **optimal solution** for this problem in C. The code demonstrates good practices: proper error handling, correct types, and clean logic.

---

## 繁體中文翻譯

### 概覽
LeetCode 1768 題要求將兩個字串按交替順序合併字元，從第一個字串開始。若其中一個字串較長，剩餘字元就附加在結果的末尾。

### 儲存庫實作分析

儲存庫的 C 實作（`1768-merge-strings-alternately.c`）採用**雙指標方法**：

```c
char* mergeAlternately(char* word1, char* word2) {
    size_t n1 = strlen(word1);
    size_t n2 = strlen(word2);

    char *res = malloc(n1 + n2 + 1);
    if (res == NULL) return NULL;

    size_t i = 0, j = 0, k = 0;

    while (i < n1 || j < n2) {
        if (i < n1) res[k++] = word1[i++];
        if (j < n2) res[k++] = word2[j++];
    }

    res[k] = '\0';
    return res;
}
```

**主要特點：**
- 使用兩個指標（`i`、`j`）走訪輸入字串，一個指標（`k`）走訪輸出
- 單一 `while` 迴圈：`while (i < n1 || j < n2)`
- 迴圈內兩個獨立的 `if` 陳述句實現自然的交替和尾部附加
- 若 malloc 失敗則提早回傳
- 使用 `size_t` 作為長度和索引（記憶體安全）

### 官方編輯方法

**方法一：雙指標** — 與儲存庫實作完全相同
- 維護兩個指標走訪兩個字串
- 交替從每個字串附加字元，直到兩者都耗盡
- 時間 O(m+n)，輔助空間 O(1)

**方法二：單指標** — 替代方法
- 單一指標 `i` 從 0 迭代到 max(m, n)
- 每次迭代時，若索引在邊界內就從兩個字串條件式地附加字元
- 複雜度相同：時間 O(m+n)，輔助空間 O(1)

### 比較表

| 評估項目 | 雙指標（儲存庫） | 單指標（官方替代） |
|--------|-----------------|-----------------|
| **時間複雜度** | O(m+n) | O(m+n) |
| **輔助空間** | O(1) | O(1) |
| **程式碼清晰度** | 優秀 — 兩個獨立條件明確表達交替邏輯 | 良好 — 單一迴圈，但每次迭代需兩次邊界檢查 |
| **迴圈結構** | `while (i < n1 \|\| j < n2)` — 兩者皆耗盡時停止 | `for (i = 0; i < max(m,n))` — 固定迭代至較長字串長度 |
| **面試可讀性** | 非常直觀：每個指標代表一個字串位置 | 略微不直觀：需理解邊界檢查邏輯 |
| **C 實作難度** | 直接翻譯；不需特殊構造 | 需要計算 max(m,n) 或調整迴圈邊界 |
| **記憶體安全性** | 易於實作 malloc/free 模式 | 相同的記憶體模式 |
| **可擴展性** | 自然處理不等長字串 | 同樣自然但需明確邊界檢查 |

### 基準方法 vs. 建議方法

**基準方法：** 雙指標（官方方法一）
- 這是大多數 DSA 課程教授的標準解法
- 人類最直觀的理解：「每個字串一個指標」
- 儲存庫實作完全符合此方法

**建議方法：** 雙指標（官方方法一 — 同儲存庫）
- **理由：** 儲存庫實作已經最優
- 清潔簡潔的 C 程式碼，認知負荷最小
- `while (i < n1 || j < n2)` 條件是合併兩個序列的標準語法習慣
- 透過適當的 malloc 錯誤處理實現記憶體安全
- 使用 `size_t`（適合索引和 strlen 回傳值）
- 迴圈內每個 if 陳述句獨立，易於驗證正確性

**方法二的考量：** 單指標方法有效但無優勢
- 需事先計算 `max(m, n)` 或使用輔助函式
- 每次迭代兩次邊界檢查（稍微更多 CPU 工作）
- 在多數面試中較不直觀

### 結論

儲存庫的獨立 C 實作**正確對應**官方方法一（雙指標）。它**已可用於生產環境**，代表此題在 C 中的**最優解**。程式碼展現優良實務：妥善的錯誤處理、正確的資料型別，以及清晰的邏輯。
