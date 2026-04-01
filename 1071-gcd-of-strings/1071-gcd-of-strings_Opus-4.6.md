# LeetCode 1071: Greatest Common Divisor of Strings — Solution vs. Editorial Comparison

## English

### 1. Implementation vs. Editorial Comparison

The repository's `solution.c` implements a **Brute Force Prefix Check** approach, which is a variation of **Editorial Approach 1 (Brute Force)**. Both share the same core idea: iterate through candidate prefix lengths from longest to shortest, and verify if the candidate can tile both strings.

**Key Differences in Implementation Details:**

| Aspect | Repo's solution.c | Editorial Approach 1 |
|--------|------------------|----------------------|
| **Candidate shrinking** | Finds first `len` divisible by both lengths, then verifies character-by-character with inline shrinking | Iterates `len` from `min(m,n)` down to 1, checks full-string equality per candidate |
| **Verification method** | Manual pass-by-pass character comparison with `goto` on mismatch | Builds repeated string and compares with original |
| **Early termination** | `goto terminate` sets `len=0` on second-pass mismatch | Returns `""` after exhausting all candidates |
| **Swap logic** | Recursive call to ensure `str1` is longer | Editorial assumes shorter string WLOG |
| **Time complexity** | O(m+n) best, O(min(m,n)·(m+n)) worst | O(min(m,n)·(m+n)) |
| **Space complexity** | O(n) — `malloc` + `realloc` for `ret` | O(min(m,n)) — copy of `base` |

The repo does **not** implement Editorial Approach 2 (GCD of Lengths), which is the mathematically elegant O(m+n) solution using `str1+str2 == str2+str1` as the existence check.

### 2. Which Approach is Better

| Criterion | Brute Force (Repo & Editorial Approach 1) | GCD of Lengths (Editorial Approach 2) |
|-----------|-------------------------------------------|---------------------------------------|
| **Time Complexity** | O(min(m,n)·(m+n)) worst case | O(m+n) |
| **Space Complexity** | O(min(m,n)) | O(m+n) for concatenation check |
| **Interview Suitability** | Good as initial approach, shows iteration skills | Excellent — demonstrates mathematical insight |
| **C Implementation Difficulty** | Moderate — manual memory management, pointer arithmetic | Low — `strcmp` on concatenated strings + GCD function |
| **Correctness Confidence** | Complex control flow with `goto` and inline shrinking may introduce subtle bugs | Simple, provably correct |
| **Follow-up Potential** | Limited | Opens discussion of Euclidean algorithm, string periodicity |

### 3. Baseline vs. Recommended

**Baseline Approach (Brute Force — current repo implementation):**
- **Algorithm:** Copy shorter string as candidate. Find largest `len` divisible by both lengths. Verify candidate tiles both strings pass-by-pass with inline shrinking.
- **Time Complexity:** O(min(m,n)·(m+n)) worst case
- **Space Complexity:** O(n)
- **Interview context:** Acceptable as an initial solution. Shows understanding of the divisibility concept. However, the `goto`-based control flow and inline shrinking logic may be hard to explain under interview pressure.

**Recommended Approach (GCD of Lengths):**
- **Algorithm:** (1) Check if `str1+str2 == str2+str1`. If not, return `""`. (2) Compute `g = gcd(len(str1), len(str2))`. (3) Return `str1[0..g-1]`.
- **Time Complexity:** O(m+n)
- **Space Complexity:** O(m+n)
- **Why it is better:** The concatenation check elegantly handles the existence question. The GCD computation directly yields the answer length. The code is short, easy to explain, and provably correct. In an interview, this demonstrates mathematical reasoning and clean problem decomposition.

---

## 繁體中文

### 1. 實現方案 vs. 官方解法比較

倉庫中的 `solution.c` 實現了一種**暴力前綴檢查**方法，是**官方方案 1（暴力法）**的變體。兩者共享相同的核心思想：從最長到最短遍歷候選前綴長度，驗證候選能否鋪滿兩個字串。

**實現細節的關鍵差異：**

| 項目 | 倉庫 solution.c | 官方方案 1 |
|------|-----------------|----------|
| **候選縮短方式** | 先找到能同時整除兩個長度的 `len`，再逐字元驗證並行內縮短 | 從 `min(m,n)` 向下遍歷到 1，每個候選做完整字串比對 |
| **驗證方法** | 手動逐輪逐字元比較，不匹配時 `goto` | 建立重複字串後與原字串比較 |
| **提前終止** | `goto terminate` 在第二輪不匹配時設 `len=0` | 窮盡所有候選後回傳 `""` |
| **交換邏輯** | 遞迴呼叫確保 `str1` 較長 | 官方假設較短字串 WLOG |
| **時間複雜度** | 最佳 O(m+n)，最差 O(min(m,n)·(m+n)) | O(min(m,n)·(m+n)) |
| **空間複雜度** | O(n) — `malloc` + `realloc` 管理 `ret` | O(min(m,n)) — `base` 的副本 |

倉庫**未實現**官方方案 2（長度的 GCD），即利用 `str1+str2 == str2+str1` 作為存在性檢查的數學方法，時間複雜度為 O(m+n)。

### 2. 哪種方法更好

| 標準 | 暴力法（倉庫 & 官方方案 1） | 長度的 GCD（官方方案 2） |
|-----|---------------------------|----------------------|
| **時間複雜度** | 最差 O(min(m,n)·(m+n)) | O(m+n) |
| **空間複雜度** | O(min(m,n)) | O(m+n)（串接檢查） |
| **面試適用性** | 適合作為初始方法，展示迭代能力 | 優秀 — 展示數學洞察力 |
| **C 語言實現難度** | 中等 — 手動記憶體管理、指標算術 | 低 — 串接後 `strcmp` + GCD 函式 |
| **正確性信心** | 複雜的控制流搭配 `goto` 和行內縮短可能引入微妙的 bug | 簡單，可證明正確 |
| **後續討論潛力** | 有限 | 開啟歐幾里得演算法、字串週期性的討論 |

### 3. 基準方案 vs. 推薦方案

**基準方案（暴力法 — 目前倉庫實現）：**
- **演算法：** 複製較短字串作為候選。找到能同時整除兩個長度的最大 `len`。逐輪驗證候選能否鋪滿兩個字串，行內縮短不匹配的候選。
- **時間複雜度：** 最差 O(min(m,n)·(m+n))
- **空間複雜度：** O(n)
- **面試背景：** 可接受作為初始解法。展示對整除概念的理解。然而，`goto` 控制流和行內縮短邏輯在面試壓力下可能難以解釋。

**推薦方案（長度的 GCD）：**
- **演算法：** (1) 檢查 `str1+str2 == str2+str1`，若不同回傳 `""`。(2) 計算 `g = gcd(len(str1), len(str2))`。(3) 回傳 `str1[0..g-1]`。
- **時間複雜度：** O(m+n)
- **空間複雜度：** O(m+n)
- **為何更好：** 串接檢查優雅地解決了存在性問題。GCD 計算直接得出答案長度。程式碼簡短、易於解釋且可證明正確。在面試中，這展示了數學推理能力和清晰的問題分解能力。
