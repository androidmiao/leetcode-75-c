# LeetCode 2215: Find the Difference of Two Arrays — Implementation Analysis

## English Section

### Overview

This document compares the implementations of LeetCode problem 2215 ("Find the Difference of Two Arrays") across three C solutions and the official editorial approaches.

**Problem Statement:** Given two integer arrays `nums1` and `nums2`, return a list of two lists. The first list contains elements that are in `nums1` but not in `nums2`, and the second list contains elements that are in `nums2` but not in `nums1`. Elements in the result must be distinct.

**Constraints:** `-1000 ≤ nums[i] ≤ 1000`

---

### Solution Implementations

#### 1. Main Solution: Fixed-Range Boolean Array Approach

**File:** `2215_Find the Difference of Two Arrays.c`

**Algorithm:**
- Uses two fixed-size boolean arrays: `bool in1[2001]` and `bool in2[2001]`
- Maps the value range `[-1000, 1000]` to indices `[0, 2000]` via `index = value + 1000`
- **Phase 1:** Marks presence of values in both arrays (automatically handles duplicates)
- **Phase 2:** Scans the full value range `[-1000, 1000]` to collect set differences
- **Time Complexity:** O(n + m + 2001) = **O(n + m)** (where 2001 is constant)
- **Space Complexity:** O(2001) = **O(1)** extra space (excluding output)

**Relationship to Editorial:**
- **Does NOT match Approach 1** (Brute Force): Approach 1 uses O(N×M) nested loops; this solution is linear.
- **Matches Approach 2's core concept** (HashSet): Both pre-build membership structures and then scan to find differences. However, the data structure differs: this uses a fixed boolean array instead of a true hash set, which works because the value domain is fixed and known.

---

#### 2. Alternative Approach 1: Brute Force with Linear Duplicate Check

**File:** `2215_Find the Difference of Two Arrays_Approch1.c`

**Algorithm:**
- Implements a helper function `exists_in_array()` that performs linear search
- For each element in the source array, checks:
  1. Whether it already appears in the output (to ensure distinctness)
  2. Whether it exists in the target array
- **Time Complexity:** O(N² + M²) = **O((N + M)²)** in the worst case
  - For each of N elements in nums1, checks N elements in the result set, then M elements in nums2: O(N·M)
  - Repeated similarly for nums2: O(M·N)
  - Overall: approximately **O(N·M)**
- **Space Complexity:** O(N + M) (for the temporary output arrays)

**Relationship to Editorial:**
- **Matches Approach 1** (Brute Force) very closely
- Uses linear search instead of hash sets for both membership checks and duplicate detection
- Time complexity O(N·M) confirms alignment with the brute force editorial approach

---

#### 3. Alternative Approach 2: Custom Hash Set Implementation

**File:** `2215_Find the Difference of Two Arrays_Approch2.c`

**Algorithm:**
- Implements a custom `IntHashSet` with open addressing and linear probing
- Hash function: MurmurHash-like bit mixing
- Capacity strategy: powers of 2, sized at approximately 2×(expected_size)
- **Phase 1:** Build two hash sets from nums1 and nums2
- **Phase 2:** Iterate through nums1, check membership in set2; iterate through nums2, check membership in set1
- **Time Complexity:** O(n + m) for hashing + O(n + m) for difference collection = **O(n + m)**
- **Space Complexity:** O(n + m) for the hash sets

**Relationship to Editorial:**
- **Matches Approach 2** (HashSet) perfectly in concept
- Implements a proper hash table instead of using a built-in or assuming a fixed value domain
- Demonstrates practical hash set construction with collision handling

---

### Comparison Table

| Aspect | Main Solution (Boolean Array) | Approach 1 (Brute Force) | Approach 2 (Hash Set) |
|--------|------|------|------|
| **Algorithm** | Fixed-range boolean array | Nested linear search | Custom hash table |
| **Time Complexity** | O(n + m) | O(n·m) | O(n + m) |
| **Space Complexity** | O(1) extra | O(n + m) | O(n + m) |
| **Editorial Match** | Approach 2 (concept) | Approach 1 | Approach 2 |
| **Scales Beyond [-1000, 1000]** | No (fixed array size) | Yes | Yes |
| **Implementation Difficulty** | Very easy | Easy | Medium (hash function, open addressing) |
| **Interview Suitability** | Excellent for this specific problem | Good for initial solution, suboptimal | Excellent for general-purpose understanding |
| **Practical Use** | Perfect for constraints with fixed range | Educational, suboptimal | Robust, production-ready |

---

### Detailed Comparison Analysis

#### Time Complexity Analysis

- **Main Solution:** O(n + m + 2001)
  The constant 2001 comes from scanning the entire value range once. Since 2001 is fixed regardless of input size, this simplifies to O(n + m).

- **Approach 1:** O(n·m) at worst
  Each of n elements in nums1 is checked against up to m elements in nums2 (and against n elements already collected for distinctness). Similarly for nums2 against nums1.

- **Approach 2:** O(n + m)
  Hash set insertion and lookup are O(1) average case. The dominant operations are: iterate through nums1 (O(n)), iterate through nums2 (O(m)), hash set operations (O(n + m) total).

#### Space Complexity Analysis

- **Main Solution:** O(1) extra space
  Only the two fixed 2001-element boolean arrays, which consume constant space regardless of input size.

- **Approach 1:** O(n + m)
  Temporary arrays for the two result sets can grow up to size n and m respectively.

- **Approach 2:** O(n + m)
  Two hash sets, each storing up to n and m distinct elements respectively.

---

### Interview Suitability

**Best for Interview: Main Solution (Fixed-Range Boolean Array)**

| Criterion | Assessment |
|-----------|------------|
| **Ease of Explanation** | Excellent — straightforward: "mark presence, then scan range" |
| **Code Clarity** | Excellent — minimal helper functions, linear flow |
| **Time Complexity** | Optimal — O(n + m) |
| **Space Complexity** | Best-in-class — O(1) extra space |
| **Adaptability** | Limited — only works when value domain is fixed and small |
| **Follow-up Readiness** | Good — can pivot to Approach 2 if domain constraints change |

**Secondary Choice: Approach 2 (Hash Set)**

| Criterion | Assessment |
|-----------|------------|
| **Ease of Explanation** | Very Good — "build set, then check membership" |
| **Code Clarity** | Good — requires understanding of hash tables |
| **Time Complexity** | Optimal — O(n + m) |
| **Space Complexity** | O(n + m) — necessary for general inputs |
| **Adaptability** | Excellent — works for any value range |
| **Follow-up Readiness** | Excellent — can extend to larger problem domains |

**Not Recommended: Approach 1 (Brute Force)**

| Criterion | Assessment |
|-----------|------------|
| **Ease of Explanation** | Good — intuitive nested loops |
| **Time Complexity** | Suboptimal — O(n·m) |
| **Use Case** | Educational only; demonstrates why optimization is needed |

---

### Baseline vs. Recommended

**Baseline Approach:** Approach 1 (Brute Force with Linear Search)
**Reasoning:** It directly implements the problem statement without optimization. Every element is explicitly checked for membership via linear search. This is the intuitive first solution any programmer might write.

**Recommended Approach:** Main Solution (Fixed-Range Boolean Array)
**Reasoning:** Given the problem's constraint that all values lie in `[-1000, 1000]`, this approach is optimal:
- Achieves O(n + m) time (same as a general hash set, but more efficiently)
- Uses only O(1) extra space (compared to O(n + m) for hash sets)
- Code is simple and cache-friendly (boolean array access is highly efficient)
- Demonstrates constraint-aware problem-solving, a valued interview skill

**When to Pivot to Approach 2:**
- If the interviewer removes the fixed value domain constraint
- If you need to demonstrate understanding of hash tables
- If you want to generalize the solution beyond this specific problem

---

### Implementation Difficulty in C

1. **Main Solution:** Very Easy
   - No complex data structures needed
   - No dynamic memory allocation of variable size (arrays are fixed)
   - Simple index mapping and boolean flags

2. **Approach 1:** Easy
   - Straightforward loops and conditionals
   - Linear search is simple to implement
   - Main challenge: avoiding duplicate insertion into output

3. **Approach 2:** Medium
   - Requires understanding of hash function design
   - Needs collision handling (linear probing)
   - More subtle: capacity management, mask computation for modulo
   - Involves more dynamic memory allocation

---

### Summary Table: All Three Approaches

| Property | Editorial Approach 1 | Editorial Approach 2 | Main .c Solution | Approch1.c | Approch2.c |
|----------|:---:|:---:|:---:|:---:|:---:|
| Time | O(N×M) | O(N+M) | O(N+M) | O(N×M) | O(N+M) |
| Space | O(N+M) | O(N+M) | O(1)* | O(N+M) | O(N+M) |
| Implementation | N/A (editorial pseudocode) | N/A (editorial pseudocode) | Fixed boolean array | Linear search loops | Custom hash set |
| Matches Editorial | Matching | Conceptual match | — | Exact match | Exact match |

*Excluding output; fixed domain assumption

---

### Key Insights

1. **Constraint-Aware Optimization:** The main solution exploits the fixed value domain `[-1000, 1000]` to achieve better space efficiency than general hash-based approaches.

2. **Conceptual Equivalence:** Despite using different data structures, the main solution and Approach 2 both follow the same two-phase strategy: (1) Build presence information, (2) Scan and collect differences.

3. **C Implementation Challenges:**
   - Approach 1 is simple but inefficient
   - Approach 2 is efficient but requires careful hash table implementation
   - Main solution is a sweet spot: optimal complexity with minimal implementation complexity

4. **Learning Value:**
   - Understanding why Approach 1 is slow (O(N×M)) helps appreciate optimization
   - Implementing Approach 2 demonstrates hash table mastery
   - Recognizing the main solution shows problem-solving insight and constraint awareness

---

---

## 繁體中文翻譯 (Traditional Chinese Section)

---

# LeetCode 2215：求兩個陣列的差集 — 實作分析

## 中文部分

### 概觀

本文件比較了 LeetCode 第 2215 題「求兩個陣列的差集」(Find the Difference of Two Arrays) 的三份 C 語言解法實作與官方編輯社論中的方法。

**題目敘述:** 給定兩個整數陣列 `nums1` 和 `nums2`，回傳一個包含兩個列表的結果。第一個列表包含在 `nums1` 但不在 `nums2` 的元素，第二個列表包含在 `nums2` 但不在 `nums1` 的元素。結果中的元素必須是不重複的。

**約束條件:** `-1000 ≤ nums[i] ≤ 1000`

---

### 解法實作

#### 1. 主解法：固定值域布林陣列法

**檔案:** `2215_Find the Difference of Two Arrays.c`

**演算法:**
- 使用兩個固定大小的布林陣列：`bool in1[2001]` 和 `bool in2[2001]`
- 透過 `index = value + 1000` 將值域 `[-1000, 1000]` 映射到索引 `[0, 2000]`
- **第一階段:** 標記兩個陣列中值的存在與否（自動處理重複值）
- **第二階段:** 掃描完整值域 `[-1000, 1000]` 以收集差集
- **時間複雜度:** O(n + m + 2001) = **O(n + m)**（2001 是常數）
- **空間複雜度:** O(2001) = **O(1)** 額外空間（不含輸出）

**與編輯社論的關係:**
- **不符合方法一** (暴力法)：方法一使用 O(N×M) 的巢狀迴圈；本解法是線性時間。
- **符合方法二的核心概念** (雜湊集合)：兩者都先建構成員資格結構，再掃描以找出差集。但資料結構不同：本解法使用固定布林陣列，而不是真正的雜湊表，這是因為值域是固定且已知的。

---

#### 2. 替代方法一：含線性重複檢查的暴力法

**檔案:** `2215_Find the Difference of Two Arrays_Approch1.c`

**演算法:**
- 實作輔助函數 `exists_in_array()` 進行線性搜尋
- 對源陣列中的每個元素進行檢查：
  1. 是否已經出現在輸出結果中（確保不重複）
  2. 是否存在於目標陣列中
- **時間複雜度:** O(N² + M²) = **O((N + M)²)** 最壞情況
  - nums1 中每個 N 個元素，檢查結果集中的 N 個元素，再檢查 nums2 中的 M 個元素：O(N·M)
  - 類似地對 nums2 重複：O(M·N)
  - 總體：約 **O(N·M)**
- **空間複雜度:** O(N + M)（用於暫存輸出陣列）

**與編輯社論的關係:**
- **非常接近方法一** (暴力法)
- 對成員資格檢查和重複偵測都使用線性搜尋，而不是雜湊集合
- 時間複雜度 O(N·M) 確認與暴力法編輯方法一致

---

#### 3. 替代方法二：自訂雜湊集合實作

**檔案:** `2215_Find the Difference of Two Arrays_Approch2.c`

**演算法:**
- 實作自訂 `IntHashSet`，使用開放尋址和線性探測
- 雜湊函數：類似 MurmurHash 的位元混合
- 容量策略：2 的冪次，大約是預期大小的 2 倍
- **第一階段:** 從 nums1 和 nums2 建構兩個雜湊集合
- **第二階段:** 遍歷 nums1，檢查 set2 中的成員資格；遍歷 nums2，檢查 set1 中的成員資格
- **時間複雜度:** O(n + m) 的雜湊化 + O(n + m) 的差集收集 = **O(n + m)**
- **空間複雜度:** O(n + m) 用於雜湊集合

**與編輯社論的關係:**
- **完全符合方法二** (雜湊集合) 的概念
- 實作真正的雜湊表，而不是使用內建或假設固定值域
- 展示了具有碰撞處理的實際雜湊集合建構

---

### 比較表格

| 面向 | 主解法（布林陣列） | 方法一（暴力法） | 方法二（雜湊集合） |
|--------|------|------|------|
| **演算法** | 固定值域布林陣列 | 巢狀線性搜尋 | 自訂雜湊表 |
| **時間複雜度** | O(n + m) | O(n·m) | O(n + m) |
| **空間複雜度** | O(1) 額外 | O(n + m) | O(n + m) |
| **編輯社論符合** | 方法二（概念） | 方法一 | 方法二 |
| **超越 [-1000, 1000] 的可擴展性** | 否（陣列大小固定） | 是 | 是 |
| **實作難度** | 非常簡單 | 簡單 | 中等（雜湊函數、開放尋址） |
| **面試適合度** | 本題目的優秀選擇 | 適合初始解法，但非最優 | 適合展現通用理解 |
| **實務應用** | 約束值域的完美選擇 | 教學用途、非最優 | 強大、可用於生產環境 |

---

### 詳細比較分析

#### 時間複雜度分析

- **主解法:** O(n + m + 2001)
  常數 2001 來自於掃描一次完整值域。由於 2001 無論輸入大小如何都是固定的，因此簡化為 O(n + m)。

- **方法一:** 最壞情況下 O(n·m)
  nums1 中的每個 n 個元素都要檢查 nums2 中最多 m 個元素（以及已收集的 n 個元素以確保不重複）。同樣地 nums2 也要檢查 nums1。

- **方法二:** O(n + m)
  雜湊集合插入和查找平均為 O(1)。主要操作是：遍歷 nums1 (O(n))、遍歷 nums2 (O(m))、雜湊集合操作 (O(n + m) 總計)。

#### 空間複雜度分析

- **主解法:** O(1) 額外空間
  只有兩個固定 2001 元素的布林陣列，無論輸入大小如何都消耗常數空間。

- **方法一:** O(n + m)
  兩個結果集的暫存陣列最多可增長到 n 和 m。

- **方法二:** O(n + m)
  兩個雜湊集合，各儲存最多 n 和 m 個不重複元素。

---

### 面試適合度

**面試最佳選擇：主解法（固定值域布林陣列）**

| 評斷標準 | 評估 |
|----------|------|
| **解釋難度** | 優秀 — 直白：「標記存在，掃描值域」 |
| **代碼清晰度** | 優秀 — 最少輔助函數，流程線性 |
| **時間複雜度** | 最優 — O(n + m) |
| **空間複雜度** | 業界最佳 — O(1) 額外空間 |
| **適應性** | 受限 — 只適用於值域固定且較小的情況 |
| **後續延伸準備** | 良好 — 如果約束改變，可轉向方法二 |

**次選：方法二（雜湊集合）**

| 評斷標準 | 評估 |
|----------|------|
| **解釋難度** | 非常好 — 「建構集合，檢查成員資格」 |
| **代碼清晰度** | 良好 — 需要理解雜湊表 |
| **時間複雜度** | 最優 — O(n + m) |
| **空間複雜度** | O(n + m) — 通用輸入的必要開銷 |
| **適應性** | 優秀 — 適用於任何值域範圍 |
| **後續延伸準備** | 優秀 — 可擴展至更大問題域 |

**不推薦：方法一（暴力法）**

| 評斷標準 | 評估 |
|----------|------|
| **解釋難度** | 良好 — 直覺的巢狀迴圈 |
| **時間複雜度** | 非最優 — O(n·m) |
| **用途** | 僅適合教學；展示為何需要優化 |

---

### 基準方法 vs. 推薦方法

**基準方法:** 方法一（含線性搜尋的暴力法）
**理由:** 它直接實作題目敘述，不進行優化。每個元素都透過線性搜尋明確檢查成員資格。這是任何程式設計師可能寫出的直覺解法。

**推薦方法:** 主解法（固定值域布林陣列）
**理由:** 鑑於題目約束所有值都在 `[-1000, 1000]` 範圍內，此方法是最優的：
- 達成 O(n + m) 時間複雜度（與通用雜湊集合相同，但更高效）
- 僅使用 O(1) 額外空間（相比雜湊集合的 O(n + m)）
- 代碼簡潔且快取友善（布林陣列存取效率極高）
- 展現了對約束條件的感知與問題求解能力，這在面試中備受重視

**何時轉向方法二：**
- 如果面試官移除固定值域約束
- 如果你需要展示對雜湊表的理解
- 如果你想將解法推廣至超越此特定問題的情境

---

### C 語言實作難度

1. **主解法:** 非常簡單
   - 不需要複雜資料結構
   - 無需可變大小的動態記憶體配置（陣列大小固定）
   - 簡單的索引映射和布林旗標

2. **方法一:** 簡單
   - 直白的迴圈和條件判斷
   - 線性搜尋實作簡單
   - 主要挑戰：避免重複插入輸出

3. **方法二:** 中等難度
   - 需要理解雜湊函數設計
   - 需要碰撞處理（線性探測）
   - 更微妙的地方：容量管理、模運算的遮罩計算
   - 涉及更多的動態記憶體配置

---

### 摘要表格：三種方法比較

| 性質 | 編輯社論方法一 | 編輯社論方法二 | 主 .c 解法 | Approch1.c | Approch2.c |
|----------|:---:|:---:|:---:|:---:|:---:|
| 時間複雜度 | O(N×M) | O(N+M) | O(N+M) | O(N×M) | O(N+M) |
| 空間複雜度 | O(N+M) | O(N+M) | O(1)* | O(N+M) | O(N+M) |
| 實作方式 | N/A (編輯偽代碼) | N/A (編輯偽代碼) | 固定布林陣列 | 線性搜尋迴圈 | 自訂雜湊集合 |
| 編輯社論對應 | 直接對應 | 概念對應 | — | 完全對應 | 完全對應 |

*排除輸出；假設固定值域

---

### 核心洞察

1. **約束感知的優化:** 主解法利用固定值域 `[-1000, 1000]` 達成比通用雜湊解法更好的空間效率。

2. **概念等價性:** 儘管使用不同的資料結構，主解法和方法二都遵循相同的兩階段策略：(1) 建構存在資訊，(2) 掃描並收集差集。

3. **C 語言實作挑戰:**
   - 方法一簡單但效率低
   - 方法二效率高但需要仔細的雜湊表實作
   - 主解法是折中點：最優複雜度加上最少實作複雜度

4. **學習價值:**
   - 理解為何方法一很慢 (O(N×M)) 有助於欣賞優化
   - 實作方法二展示了雜湊表掌握度
   - 認識主解法顯示了問題求解洞察力和約束感知能力

---

