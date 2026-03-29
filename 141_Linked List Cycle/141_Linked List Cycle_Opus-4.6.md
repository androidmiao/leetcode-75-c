# LeetCode 141: Linked List Cycle - Implementation Analysis

## English Section

### Implementation vs. Editorial Comparison

The `.c` solution uses **Floyd's Cycle Finding Algorithm** (fast/slow pointers), which is **identical to Approach 2 from the Editorial**. The implementation is not a variation or different approach—it directly implements the two-pointer technique described in the editorial.

**Key evidence:**
- Both use a slow pointer that moves 1 step and a fast pointer that moves 2 steps per iteration
- Both detect cycles by checking if the pointers meet at the same node
- Both handle the no-cycle case by checking if `fast` or `fast->next` becomes NULL
- Time and space complexity match exactly: O(n) time, O(1) space

The only difference is presentation: the editorial explains the algorithm conceptually, while the `.c` file provides a production-ready implementation with detailed Chinese comments and ASCII diagrams.

---

### Which Approach is Better?

| Criterion | Hash Table (Approach 1) | Floyd's Algorithm (Approach 2) |
|-----------|------------------------|--------------------------------|
| **Time Complexity** | O(n) | O(n) |
| **Space Complexity** | **O(n)** – stores visited nodes | **O(1)** – only two pointers |
| **Intuition** | Track visited nodes; cycle if revisit | Two runners at different speeds; they collide in a cycle |
| **Interview Suitability** | Good for explaining basic logic; easier to understand initially | Excellent; demonstrates algorithmic insight and space optimization |
| **C Implementation Difficulty** | Moderate – requires hash table implementation (not trivial in C) | Very easy – simple pointer arithmetic; no extra data structures |
| **Space Trade-off** | Straightforward but uses O(n) memory | Elegant and memory-efficient |

**Key Insight:** Both achieve O(n) time complexity, but Floyd's Algorithm is superior because it eliminates the space overhead entirely while demonstrating deeper understanding of cycle detection mechanics.

---

### Baseline vs. Recommended Approach

**Baseline Approach:** Hash Table (Approach 1)
- **Why it's baseline:** It's conceptually simpler and mirrors a natural problem-solving process—record what you've seen, detect when you see something again.
- **Interview context:** Good starting point for candidates unfamiliar with advanced techniques.

**Recommended Approach:** Floyd's Cycle Finding Algorithm (Approach 2)
- **Why it's recommended for interviews:**
  1. **Space Efficiency:** O(1) space vs. O(n)—a significant advantage that distinguishes stronger candidates from average ones.
  2. **Algorithmic Elegance:** Shows understanding of pointer manipulation and mathematical insight (the fast/slow runner metaphor is memorable and demonstrates depth).
  3. **Practical Advantage:** In resource-constrained environments (embedded systems, real-time processing), O(1) space is crucial.
  4. **Scalability:** For very large linked lists, the hash table approach consumes significant memory; Floyd's Algorithm scales perfectly.
  5. **Technical Interview Appeal:** Interviewers specifically look for this answer as it tests understanding beyond "the obvious solution."

**Interview Discussion Recommendation:**
Start by explaining the hash table approach briefly to show baseline understanding, then pivot to Floyd's Algorithm while highlighting the O(1) space breakthrough. This demonstrates both foundational knowledge and advanced problem-solving ability.

---

## 繁體中文部分

### 實作與題解比較

`.c` 解法使用的是 **Floyd 快慢指標演算法**，與題解中的**第 2 種方法完全一致**。這不是變化或不同的做法，而是直接實作題解中所描述的雙指標技術。

**關鍵證據：**
- 都使用慢指標（每次走 1 步）和快指標（每次走 2 步）
- 都透過檢查指標是否指向同一個節點來偵測環
- 都透過檢查 `fast` 或 `fast->next` 是否為 NULL 來處理無環的情況
- 時間和空間複雜度完全相同：O(n) 時間，O(1) 空間

唯一的差異在於呈現方式：題解以概念說明為主，而 `.c` 檔案提供可直接提交的實作版本，並附上詳細的中文註解和 ASCII 圖解。

---

### 哪種方法更好？

| 評判標準 | Hash Table（方法 1） | Floyd 演算法（方法 2） |
|---------|--------------------|--------------------|
| **時間複雜度** | O(n) | O(n) |
| **空間複雜度** | **O(n)** – 儲存已訪問的節點 | **O(1)** – 只需要兩個指標 |
| **直覺理解** | 追蹤已訪問的節點；重複訪問時偵測環 | 兩位跑者以不同速度奔跑；在圓形跑道上必相遇 |
| **面試合適度** | 適合解釋基礎邏輯；初期理解較容易 | 優秀；展現演算法洞察力和空間最佳化 |
| **C 語言實作難度** | 中等 – 需自行實作雜湊表（在 C 中非平凡） | 非常簡單 – 簡單的指標運算；無需額外資料結構 |
| **空間權衡** | 直接但使用 O(n) 記憶體 | 優雅且記憶體高效 |

**核心洞察：** 兩種方法都達到 O(n) 時間複雜度，但 Floyd 演算法更優越，因為它完全消除了空間額外開銷，同時展現對環偵測機制更深層的理解。

---

### 基礎解法 vs. 推薦解法

**基礎解法：** Hash Table（方法 1）
- **為何是基礎方法：** 概念上更簡單，符合自然的問題解決過程——記錄已見過的內容，在重複訪問時偵測。
- **面試角度：** 對於不熟悉進階技巧的應試者是很好的起點。

**推薦解法：** Floyd 快慢指標演算法（方法 2）
- **為何推薦用於面試：**
  1. **空間效率：** O(1) 空間 vs. O(n) 空間——這個優勢能區分優秀的應試者和普通應試者。
  2. **演算法優雅性：** 展現對指標操作和數學洞察的理解；「快慢跑者」比喻容易記憶，展現深度思考。
  3. **實務優勢：** 在資源受限的環境（嵌入式系統、即時處理），O(1) 空間至關重要。
  4. **可擴展性：** 處理超大型鏈結串列時，Hash Table 方法會耗費大量記憶體；Floyd 演算法完美擴展。
  5. **面試官期望：** 面試官特別看重這個答案，因為它測試的是超越「顯而易見解法」的理解能力。

**面試討論建議：**
先簡要說明 Hash Table 方法以展現基礎理解，然後轉向 Floyd 演算法，並著重強調 O(1) 空間的突破。這樣既展現了基礎知識，也展現了進階問題解決能力。

---
