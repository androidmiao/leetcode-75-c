# LeetCode 104: Maximum Depth of Binary Tree - Implementation vs Editorial

## English Version

### Implementation vs. Editorial Comparison

The provided `.c` solution implements **Approach 1: Recursion (DFS)** from the editorial, which is the foundational depth-first search approach. The implementation matches the editorial approach exactly:

- Returns 0 for NULL nodes (base case)
- Recursively calculates the depth of left and right subtrees
- Returns the maximum of the two depths plus 1
- Time Complexity: O(n) where n is the number of nodes
- Space Complexity: O(h) where h is the height of the tree (recursion call stack)

### Approach Comparison

| Aspect | Approach 1: Recursion (DFS) | Approach 2: BFS with Queue | Approach 3: Iteration with Stack |
|--------|-----|-----|-----|
| **Time Complexity** | O(n) | O(n) | O(n) |
| **Space Complexity** | O(h) | O(n) | O(h) |
| **Best Case Space** | O(1) - balanced tree | O(n) - all levels | O(1) - linear tree |
| **Worst Case Space** | O(n) - skewed tree | O(n) - always | O(n) - skewed tree |
| **Code Clarity** | Excellent | Good | Moderate |
| **Interview Suitability** | Excellent | Good | Moderate |
| **Implementation Difficulty in C** | Very Easy | Moderate | Moderate |
| **Explanation Time** | 1-2 minutes | 2-3 minutes | 2-3 minutes |

### Which Approach is Better?

**For this problem (Easy-level):**

1. **Approach 1 (Recursion)** is the **clear winner** for interviews:
   - Simplest to implement and understand
   - Most elegant code
   - Matches common recursion patterns taught in algorithms courses
   - O(h) space is excellent for balanced trees and acceptable for most cases
   - Can be explained in seconds

2. **Approach 2 (BFS with Queue)** is useful for learning:
   - Better space efficiency when tree is highly skewed (O(n) for both recursion and queue)
   - But actually uses MORE space in typical cases due to queue overhead
   - Better for problems where level-order matters
   - Not necessary for just calculating depth

3. **Approach 3 (Iteration with Stack)** is rarely needed:
   - Adds unnecessary complexity
   - Explicit stack has same space characteristics as recursion
   - Not clearer than recursion for this problem
   - Only useful to demonstrate recursion mimicking without using call stack

### Baseline vs. Recommended

| Category | Approach |
|----------|----------|
| **Baseline (Minimum Expected)** | Approach 1: Recursion (DFS) |
| **Recommended for Interview** | Approach 1: Recursion (DFS) |
| **Advanced Alternative** | Approach 2: BFS with Queue (if interviewer asks about level-order traversal) |

**Why Approach 1 is Recommended:**
- Perfect for Easy-level tree problems
- Shows understanding of fundamental recursion and tree traversal
- Clean, maintainable code that passes all test cases
- Optimal balance of simplicity and efficiency
- Interview examiners expect and value this approach

---

## 繁體中文版本

### 實現與編輯部比較

提供的 `.c` 解決方案實現了編輯部中的**方法 1：遞歸（DFS）**，這是深度優先搜索的基礎方法。該實現完全符合編輯部方法：

- NULL 節點返回 0（基本情況）
- 遞歸計算左右子樹的深度
- 返回兩個深度的最大值加 1
- 時間複雜度：O(n)，其中 n 是節點數
- 空間複雜度：O(h)，其中 h 是樹的高度（遞歸調用棧）

### 方法比較

| 項目 | 方法 1：遞歸（DFS） | 方法 2：隊列 BFS | 方法 3：棧遞歸迭代 |
|--------|-----|-----|-----|
| **時間複雜度** | O(n) | O(n) | O(n) |
| **空間複雜度** | O(h) | O(n) | O(h) |
| **最佳情況空間** | O(1) - 平衡樹 | O(n) - 所有層 | O(1) - 線性樹 |
| **最壞情況空間** | O(n) - 傾斜樹 | O(n) - 始終 | O(n) - 傾斜樹 |
| **代碼清晰度** | 優秀 | 良好 | 中等 |
| **面試適用性** | 優秀 | 良好 | 中等 |
| **C 語言實現難度** | 非常簡單 | 中等 | 中等 |
| **解釋時間** | 1-2 分鐘 | 2-3 分鐘 | 2-3 分鐘 |

### 哪種方法更好？

**針對此問題（簡單難度）：**

1. **方法 1（遞歸）**是**面試的明確優選**：
   - 最簡單易懂的實現
   - 代碼最優雅
   - 符合教科書中教授的常見遞歸模式
   - 對於平衡樹 O(h) 空間表現優異，對於大多數情況可接受
   - 可以在數秒內解釋

2. **方法 2（隊列 BFS）**適合學習：
   - 在樹高度傾斜時空間效率更好（遞歸和隊列均為 O(n)）
   - 但在典型情況下，隊列開銷使其使用**更多**空間
   - 在層序遍歷重要的問題中更有用
   - 單純計算深度無需使用此方法

3. **方法 3（棧遞歸迭代）**很少需要：
   - 增加不必要的複雜性
   - 顯式棧具有與遞歸相同的空間特性
   - 對此問題不比遞歸清晰
   - 僅用於演示遞歸模仿而不使用調用棧

### 基線 vs. 推薦

| 分類 | 方法 |
|----------|----------|
| **基線（最低期望）** | 方法 1：遞歸（DFS） |
| **面試推薦** | 方法 1：遞歸（DFS） |
| **進階替代方案** | 方法 2：隊列 BFS（如果面試官問及層序遍歷） |

**為什麼推薦方法 1：**
- 完美適合簡單難度的樹問題
- 展示對基礎遞歸和樹遍歷的理解
- 清晰、可維護的代碼，通過所有測試用例
- 簡潔性和效率的最優平衡
- 面試官期望並重視此方法
