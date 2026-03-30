# LeetCode 480 – Sliding Window Median (Hard)

## Implementation vs. Editorial Comparison

The `.c` solution **is an implementation of Editorial Approach 2: Two Heaps with Lazy Deletion**. The editorial describes the conceptual algorithm; the `.c` code realizes it with concrete C data structures:
- **Max-heap** for the smaller half of elements
- **Min-heap** for the larger half of elements
- **Hash map with open addressing** for tracking delayed deletions instead of actually removing elements from heaps

The `.c` solution adds **dynamic heap resizing** (growing the heap array as needed), which the editorial mentions theoretically (heaps can grow to O(n) size) but does not show explicit implementation for.

---

## Approach Comparison

| Aspect | Approach 1: Simple Sorting | Approach 2: Two Heaps (Our .c) | Approach 3: Two Multisets | Approach 4: Multiset + Iterator |
|--------|---------------------------|--------------------------------|---------------------------|--------------------------------|
| **Time Complexity** | O(n·k) | O(n log n) | O(n log k) | O(n log k) |
| **Space Complexity** | O(k) | O(n) | O(k) | O(k) |
| **Conceptual Clarity** | Very clear | Moderate; requires heap balance logic | Clear (for C++/Java) | Most elegant (for C++/Java) |
| **C Impl. Difficulty** | Low | High | Not directly applicable | Not directly applicable |
| **Interview Readiness** | Baseline; shows sorting | Recommended; demonstrates heap + hash table mastery | Requires balanced BST or custom sorted array | Requires C++ STL or equivalent |
| **Worst-Case Behavior** | Degrades badly with large k | Consistent, predictable | Degrades with large k | Degrades with large k |

---

## Baseline vs. Recommended Approach

**Baseline:** Approach 1 (Simple Sorting)
**Recommended:** Approach 2 (Two Heaps with Lazy Deletion)

### Why Approach 2 is Better for Interviews (Especially in C)

1. **Time Complexity:** O(n log n) vs. O(n·k) provides significant speedup, especially when k is large.
2. **Skill Demonstration:**
   - Heap data structure mastery (building, balancing, and maintaining invariants)
   - Hash map implementation (open addressing for lazy deletion tracking)
   - Dynamic memory management in C
3. **Portability:** The two-heap strategy is language-agnostic and works in C++, Java, Python, and Go without relying on language-specific containers.
4. **Lazy Deletion Insight:** The trick of deferring removals rather than eagerly extracting from heaps is elegant and shows problem-solving maturity.
5. **Real-World Relevance:** Two heaps plus lazy deletion is used in many online problems (task scheduler, IPO, etc.) and in production systems.

### Why Not Approaches 3 and 4?

**In C:**
- Approach 3 (Two Multisets) and Approach 4 (Multiset + Iterator) rely on balanced tree containers native to C++ (`std::multiset`), Java, or Python.
- In standalone C, you would need to implement a **balanced binary search tree** (AVL, Red-Black, or similar) from scratch, which is complex, error-prone, and rarely expected in time-limited interviews.
- If you use a **sorted array with binary search**, you still pay O(k) or O(n) for insertion/deletion, negating the O(log k) advantage.

**In C++/Java:**
- Approach 4 is theoretically the most elegant and efficient overall (O(n log k) time, O(k) space).
- However, it requires iterators and bidirectional container operations; fewer candidates master this level of STL usage.

### Summary

For a **C interview**, implement Approach 2 (Two Heaps). It shows you understand:
- How to build and maintain custom heap structures
- How to use a hash map for deferred deletions
- How to balance and reheapify when the window shrinks

For **C++ / Java interviews**, Approach 4 is superior if you're fluent with multiset and bidirectional iterators. Otherwise, Approach 2 remains a solid, language-neutral choice.

---

---

# LeetCode 480 – 滑動視窗中位數 (困難)

## 實現與官方解法的比較

該 `.c` 解決方案**是官方解法 2 的實現：雙堆搭配延遲刪除（Two Heaps with Lazy Deletion）**。官方解法描述了概念演算法；`.c` 程式碼用具體的 C 資料結構實現它：
- **最大堆**用於較小一半的元素
- **最小堆**用於較大一半的元素
- **雜湊映射（開放定址法）**追蹤延遲刪除，而不是立即從堆中移除元素

該 `.c` 解決方案添加了**動態堆大小調整**（在需要時增長堆陣列），官方解法理論上提及過（堆可能增長到 O(n) 大小），但沒有顯示明確的實現細節。

---

## 四種方法的比較

| 面向 | 方法 1：簡單排序 | 方法 2：雙堆（我們的 .c） | 方法 3：雙重集 | 方法 4：重集 + 迭代器 |
|------|------------------|--------------------------|------------------|----------------------|
| **時間複雜度** | O(n·k) | O(n log n) | O(n log k) | O(n log k) |
| **空間複雜度** | O(k) | O(n) | O(k) | O(k) |
| **概念清晰度** | 非常清楚 | 中等；需要堆平衡邏輯 | 清楚（對 C++/Java） | 最優雅（對 C++/Java） |
| **C 實現難度** | 低 | 高 | 不直接適用 | 不直接適用 |
| **面試準備度** | 基礎；展示排序 | 推薦；展示堆 + 雜湊表掌握 | 需要平衡二叉樹或自訂排序陣列 | 需要 C++ STL 或等效品 |
| **最壞情況行為** | 隨著 k 增大而退化 | 一致、可預測 | 隨著 k 增大而退化 | 隨著 k 增大而退化 |

---

## 基礎與推薦方法

**基礎：** 方法 1（簡單排序）
**推薦：** 方法 2（雙堆搭配延遲刪除）

### 為什麼方法 2 對面試更好（尤其是在 C 中）

1. **時間複雜度：** O(n log n) vs. O(n·k) 提供顯著加速，特別是當 k 較大時。
2. **技能展示：**
   - 堆資料結構掌握（構建、平衡、維持不變量）
   - 雜湊映射實現（開放定址法用於延遲刪除追蹤）
   - C 中的動態記憶體管理
3. **可移植性：** 雙堆策略與語言無關，可在 C++、Java、Python 和 Go 中工作，而不依賴語言特定的容器。
4. **延遲刪除洞察：** 延遲移除而非立即從堆中提取的技巧既優雅，又展示問題解決的成熟度。
5. **現實世界相關性：** 雙堆加延遲刪除用於許多線上題目（任務排程器、IPO 等）和生產系統。

### 為什麼不選方法 3 和方法 4？

**在 C 中：**
- 方法 3（雙重集）和方法 4（重集 + 迭代器）依賴於 C++ 原生平衡樹容器（`std::multiset`）、Java 或 Python。
- 在獨立 C 中，你需要從頭實現**平衡二叉搜尋樹**（AVL、紅黑樹等），這很複雜、容易出錯，且很少在時間限制的面試中要求。
- 如果使用**排序陣列搭配二分搜尋**，插入/刪除仍需 O(k) 或 O(n)，抵消了 O(log k) 的優勢。

**在 C++/Java 中：**
- 方法 4 在理論上最優雅且整體效率最高（O(n log k) 時間，O(k) 空間）。
- 然而，它需要迭代器和雙向容器操作；很少有候選人掌握此級別的 STL 使用。

### 總結

對於 **C 面試**，實現方法 2（雙堆）。它展示你理解：
- 如何構建和維護自訂堆結構
- 如何使用雜湊映射進行延遲刪除
- 當視窗縮小時如何平衡和重新堆化

對於 **C++ / Java 面試**，如果你熟悉重集和雙向迭代器，方法 4 更優。否則，方法 2 仍然是一個穩健、語言無關的選擇。
