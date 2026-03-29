# LeetCode 1. Two Sum - Solution Analysis

## English Section

### Implementation vs. Editorial Comparison

The C solution in `1_Two Sum.c` implements **Approach 3: One-pass Hash Table** from the editorial. However, there is an important caveat: **it is a variation of the canonical approach**.

**Key difference from typical implementations:**
- The editorial assumes a language with a built-in hash map (like Python's dictionary or Java's HashMap)
- The C solution implements a **hand-rolled hash table using linear probing** for open addressing
- This is necessary because C has no built-in hash map

**Correctness:** The algorithm is identical to Approach 3 in terms of logic and flow:
1. For each element, compute the complement `target - nums[i]`
2. Search the hash table for the complement
3. If found, return the indices immediately
4. If not found, insert the current element and its index

The hand-rolled implementation with linear probing achieves the same theoretical time complexity as a built-in hash map.

---

### Which Approach is Better?

Below is a comparison across all three approaches relevant to this problem:

| Aspect | Brute Force (Approach 1) | Two-pass Hash Table (Approach 2) | One-pass Hash Table (Approach 3 - C impl.) |
|--------|------------------------|----------------------------------|-------------------------------------------|
| **Time Complexity** | O(n²) | O(n) | O(n) |
| **Space Complexity** | O(1) | O(n) | O(n) |
| **Interview Suitability** | Low (naive) | High (good trade-off) | High (optimal) |
| **C Implementation Difficulty** | Trivial | Moderate (need hash table) | Moderate (need hash table) |
| **Code Readability** | Very clear | Clear | More complex (probing logic) |
| **Practical Performance on Modern Hardware** | Very slow (nested loops) | Fast | Fast (single pass) |

**Time Complexity Explanation:**
- Brute Force scans remaining elements for each element → O(n²)
- Both hash table approaches scan the array once with near-constant lookups → O(n)

**Space Complexity Explanation:**
- Brute Force uses no extra space → O(1)
- Hash table approaches store all elements in a hash map → O(n)

**C Implementation Difficulty:**
- Brute Force: Just two nested loops
- Hash Table (either approach): Requires implementing or using a hash table library
  - The C solution implements one from scratch with linear probing (handles collisions)
  - This adds complexity but demonstrates good understanding

---

### Baseline vs. Recommended Approach

| Aspect | Baseline | Recommended |
|--------|----------|-------------|
| **Name** | Brute Force (Approach 1) | One-pass Hash Table (Approach 3) |
| **Why Baseline** | Straightforward to understand and implement, requires no extra setup | Demonstrates the progression of optimization |
| **Why Recommended** | Optimal time complexity, single-pass is more elegant than two-pass in interviews, demonstrates real-world performance thinking | In interviews, one-pass shows depth of thought; in production, avoids unnecessary traversals |

**Detailed Explanation:**

**Baseline Approach (Brute Force):**
- Use nested loops to check every pair of elements
- Pros: Requires no additional space (O(1)) and is easiest to code
- Cons: O(n²) time is unacceptable for large inputs (up to 10^4 elements); interviews expect better

**Recommended Approach (One-pass Hash Table):**
- Build a hash table as you iterate through the array
- Check for the complement before inserting each element
- Why it's better:
  - **Time**: O(n) is optimal for this problem (must read every element at least once)
  - **Space trade-off**: O(n) extra space is well justified for a 100x speedup
  - **Elegance**: One pass is more efficient and elegant than two-pass
  - **Interview value**: Demonstrates understanding of hash tables, collision handling, and optimization thinking
  - **Real-world relevance**: Single-pass algorithms are preferred for streaming/online data

**For C specifically:**
The C implementation using linear probing shows additional mastery—it's not just about knowing the algorithm, but understanding how hash tables actually work under the hood. This is a significant interview advantage.

---

## 繁體中文 (Traditional Chinese Section)

### 實作與教科書比較

`1_Two Sum.c` 中的 C 解法實現了教科書中的**解法三：一次走訪雜湊表（Approach 3: One-pass Hash Table）**。不過有一個重要的細節：**這是經過調整的變體實作**。

**與典型實作的主要差異：**
- 教科書假設使用內建雜湊表的語言（如 Python 的字典或 Java 的 HashMap）
- C 解法實現了**手工打造的雜湊表，採用線性探測（linear probing）的開放位址法**
- 這是必要的，因為 C 沒有內建的雜湊表

**正確性：** 從演算邏輯和流程來看，完全等同於教科書的解法三：
1. 對每個元素計算補數 `complement = target - nums[i]`
2. 在雜湊表中搜尋補數
3. 如果找到，立刻回傳索引
4. 如果沒找到，將目前元素及其索引存入雜湊表

手工實作的線性探測雜湊表能達到與內建雜湊表相同的理論時間複雜度。

---

### 何者更優？

以下是三種解法在各重要面向的比較：

| 面向 | 暴力法（解法一） | 兩次走訪雜湊表（解法二） | 一次走訪雜湊表（解法三 - C 實作） |
|------|-----------------|------------------------|--------------------------------|
| **時間複雜度** | O(n²) | O(n) | O(n) |
| **空間複雜度** | O(1) | O(n) | O(n) |
| **面試適合度** | 低（過於單純） | 高（很好的權衡） | 高（最優解） |
| **C 語言實作難度** | 微不足道 | 中等（需實作雜湊表） | 中等（需實作雜湊表） |
| **程式碼易讀性** | 非常清晰 | 清晰 | 較複雜（有探測邏輯） |
| **現代硬體實務表現** | 極慢（嵌套迴圈） | 快速 | 快速（單次掃描） |

**時間複雜度說明：**
- 暴力法對每個元素再掃描剩餘元素 → O(n²)
- 兩種雜湊表解法都掃描陣列一次，查詢時間近似常數 → O(n)

**空間複雜度說明：**
- 暴力法不使用額外空間 → O(1)
- 雜湊表解法需存放陣列中的所有元素 → O(n)

**C 語言實作難度：**
- 暴力法：僅需兩層迴圈
- 雜湊表（任一解法）：需自己實作或引入雜湊表程式庫
  - 本解中自行實作，使用線性探測處理碰撞
  - 雖然增加複雜度，但展現了深入的理解

---

### 基準解法 vs. 推薦解法

| 面向 | 基準解法 | 推薦解法 |
|------|----------|---------|
| **名稱** | 暴力法（解法一） | 一次走訪雜湊表（解法三） |
| **為何為基準** | 最直觀易懂，無需額外設定或複雜資料結構 | 體現優化的思路進展 |
| **為何推薦** | 時間複雜度最優，單次掃描比雙次掃描更優雅，展現實務思考 | 面試中展現深度的思考；生產環境中避免不必要的遍歷 |

**詳細說明：**

**基準解法（暴力法）：**
- 使用嵌套迴圈檢查每一對元素
- 優點：不需額外空間（O(1)），最簡單易寫
- 缺點：O(n²) 時間複雜度對大規模輸入（最多 10^4 個元素）難以接受；面試官期望更好的解法

**推薦解法（一次走訪雜湊表）：**
- 在遍歷陣列時同步建立雜湊表
- 每插入元素前先檢查補數是否存在
- 為何更優：
  - **時間**：O(n) 是此問題的最優（至少要讀一次所有元素）
  - **空間權衡**：O(n) 的額外空間相對於 100 倍的時間加速，非常值得
  - **優雅性**：單次掃描比雙次掃描更有效率且更精簡
  - **面試價值**：展現對雜湊表、碰撞處理、優化思路的理解
  - **實務意義**：單次遍歷演算法在串流/線上資料處理中更受青睞

**特別針對 C 語言：**
本實作所使用的線性探測手工雜湊表展現了額外的掌握—不僅知道演算法，更理解雜湊表在低層是如何運作的。在面試中這是重大的優勢加分。
