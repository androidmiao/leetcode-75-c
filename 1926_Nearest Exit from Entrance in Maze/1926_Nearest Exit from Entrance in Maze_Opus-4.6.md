# LeetCode 1926: Nearest Exit from Entrance in Maze
## Implementation vs. Editorial Analysis

---

## English

### Implementation vs. Editorial Comparison

The C solution and the LeetCode editorial both implement the **Breadth-First Search (BFS)** algorithm to find the shortest path to the nearest exit. They are **fundamentally the same algorithm** with minor implementation differences.

**Core similarities:**
- Both use BFS to explore cells level-by-level by distance from the entrance
- Both mark visited cells to prevent revisits (using `'+'` in the C version)
- Both return immediately upon discovering an exit cell at the border
- Both return `-1` if no exit is reachable

**Implementation differences:**

1. **Queue storage approach:**
   - **Editorial description:** Suggests storing `(coordinate, distance)` tuples in the queue, explicitly tracking the distance for each cell.
   - **C implementation:** Uses level-by-level BFS with a separate `steps` counter and `levelSize = rear - front` to process all cells of the same distance together. This avoids storing redundant distance information per cell.

2. **Space complexity analysis:**
   - **Editorial claim:** `O(max(m,n))` or `O(m+n)` for queue space.
   - **Conservative analysis:** In the worst case (e.g., a spiral or snake pattern filling the grid), the queue can contain `O(m*n)` cells, making the actual worst-case space complexity `O(m*n)`.

### Approach Comparison

| Criterion | Editorial Approach | C Implementation |
|-----------|-------------------|------------------|
| **Algorithm Type** | BFS (level-order traversal) | BFS (level-order traversal) |
| **Time Complexity** | O(m*n) | O(m*n) |
| **Space Complexity (Best Case)** | O(m+n) | O(m+n) |
| **Space Complexity (Worst Case)** | O(m*n) | O(m*n) |
| **Queue Storage** | Stores (coordinate, distance) | Two parallel arrays + steps counter |
| **Memory Efficiency in C** | Wastes space storing distance per cell | More efficient: no per-cell distance storage |
| **Code Clarity** | Clear intent, but verbose | Compact, level-by-level tracking is idiomatic |
| **Interview Suitability** | High—easier to explain to interviewers | High—shows deep understanding of BFS mechanics |
| **C Implementation Difficulty** | Moderate—need to handle queue of structs | Easier—simple arrays and pointers |

### Baseline vs. Recommended Approach

**Baseline:** Editorial approach (tuple-based storage)
- Represents the standard, textbook BFS method
- Easier to understand for beginners
- Works well in most languages (especially Java, Python)
- Slightly less memory-efficient in C due to storing redundant distance per cell

**Recommended:** C implementation (level-by-level with steps counter)
- Better memory utilization in C
- Avoids struct overhead or distance field duplication
- Demonstrates mastery of BFS internals
- Still maintains O(m*n) worst-case complexity but typically uses less memory in practice
- Idiomatic for C programmers—natural use of front/rear pointers and level tracking

**Why recommended?** The C implementation is optimized for the constraints of the C language (no dynamic structures, explicit memory management). It preserves the correctness and clarity of BFS while being more efficient than blindly translating the editorial approach into C.

---

## 繁體中文

### 實作 vs. 編輯方案比較

C 解法與 LeetCode 編輯方案都採用**廣度優先搜尋（BFS）**演算法來尋找最短路徑到最近的出口。它們在本質上是**相同的演算法**，只有些微的實作細節差異。

**核心相似之處：**
- 兩者都用 BFS 依距離逐層探索入口周圍的格子
- 兩者都標記已拜訪的格子以防重複造訪（C 版本使用 `'+'`）
- 兩者都在發現邊界空格出口時立即返回
- 兩者都在找不到可達出口時返回 `-1`

**實作差異：**

1. **佇列儲存方式：**
   - **編輯方案描述：** 建議在佇列中儲存 `(座標, 距離)` 元組，明確追蹤每個格子的距離。
   - **C 實作：** 使用逐層 BFS，配合獨立的 `steps` 計數器和 `levelSize = rear - front` 來一次處理同距離的所有格子。這避免了為每個格子重複儲存距離資訊。

2. **空間複雜度分析：**
   - **編輯方案聲稱：** 佇列空間為 `O(max(m,n))` 或 `O(m+n)`。
   - **保守分析：** 在最壞情況下（例如螺旋形或蛇形填滿網格），佇列可能包含 `O(m*n)` 個格子，使實際最壞情況空間複雜度為 `O(m*n)`。

### 方案比較表

| 比較項目 | 編輯方案 | C 實作 |
|---------|---------|-------|
| **演算法類型** | BFS（層序走訪） | BFS（層序走訪） |
| **時間複雜度** | O(m*n) | O(m*n) |
| **空間複雜度（最佳情況）** | O(m+n) | O(m+n) |
| **空間複雜度（最壞情況）** | O(m*n) | O(m*n) |
| **佇列儲存方式** | 儲存 (座標, 距離) | 兩個平行陣列 + steps 計數器 |
| **C 中的記憶體效率** | 浪費空間儲存每格的距離 | 更有效率：無需逐格儲存距離 |
| **程式碼清晰度** | 意圖明確，但較冗長 | 緊湊，逐層追蹤是 C 的慣用做法 |
| **面試適合度** | 高—容易向面試官解釋 | 高—展現對 BFS 內部機制的深度理解 |
| **C 實作難度** | 中等—需處理結構體佇列 | 更簡單—單純陣列與指標 |

### 基準 vs. 推薦方案

**基準：** 編輯方案（元組式儲存）
- 代表標準的教科書 BFS 方法
- 對初學者更容易理解
- 在大多數語言中都適用（尤其是 Java、Python）
- 在 C 中因逐格儲存距離而略顯記憶體低效

**推薦：** C 實作（逐層搭配 steps 計數器）
- 更好的 C 語言記憶體利用
- 避免結構體開銷或距離欄位重複
- 展現對 BFS 內部機制的掌握
- 仍保持 O(m*n) 最壞情況複雜度，但實際執行時記憶體使用通常更少
- 適合 C 程式設計師—前驅指標與層級追蹤的自然用法

**為何推薦？** C 實作針對 C 語言的限制進行了優化（無動態結構體、顯式記憶體管理）。它保留了 BFS 的正確性與清晰性，同時相比盲目將編輯方案翻譯成 C 更具效率。
