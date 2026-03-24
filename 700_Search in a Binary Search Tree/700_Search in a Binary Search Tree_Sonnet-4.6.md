# 700. Search in a Binary Search Tree — Implementation vs. Editorial Comparison

## Implementation vs. Editorial

The `.c` solution implements **Approach 2 (Iteration)** from the editorial. It is the exact same algorithm — a simple `while` loop that walks down the BST by comparing `val` against `root->val` at each step and moving left or right accordingly.

## Which Approach is Better

| Dimension                  | Approach 1: Recursion       | Approach 2: Iteration (our `.c`) |
|----------------------------|-----------------------------|----------------------------------|
| Time complexity            | O(H)                        | O(H)                             |
| Space complexity           | O(H) (call stack)           | **O(1)**                         |
| Interview suitability      | Good — clean, readable      | **Better** — shows optimization  |
| C implementation difficulty| Easy                        | Easy                             |
| Tail-call optimization     | Depends on compiler flags   | Not needed                       |

Both approaches have identical time complexity. The iteration approach wins on space complexity (O(1) vs. O(H)) with no increase in code complexity.

## Baseline vs. Recommended

- **Baseline approach:** Recursion (Approach 1). It is the most natural and intuitive way to express BST search — directly mirrors the definition.

- **Recommended approach:** Iteration (Approach 2). For this problem the iterative version is equally simple to write and explain, but eliminates the recursion stack overhead. In an interview, starting with the recursive baseline and then optimizing to iteration demonstrates awareness of space efficiency — a small but meaningful signal.

For a Google interview specifically, this problem is too straightforward to differentiate candidates on its own. The key is to solve it quickly and cleanly, then be ready to discuss BST properties, balanced vs. unbalanced trees, and follow-up questions about tree modifications (insert, delete).

---

# 700. 搜尋二元搜尋樹 — 實作 vs. 官方題解對照

## 實作 vs. 題解

`.c` 解法實作的是**方法二（迭代）**，與官方題解完全相同的演算法 — 用一個 `while` 迴圈沿著 BST 往下走，每一步比較 `val` 與 `root->val`，決定往左或往右移動。

## 哪種方法較好

| 比較面向              | 方法一：遞迴              | 方法二：迭代（我們的 `.c`）|
|----------------------|--------------------------|---------------------------|
| 時間複雜度            | O(H)                     | O(H)                      |
| 空間複雜度            | O(H)（呼叫堆疊）          | **O(1)**                   |
| 面試適合度            | 佳 — 簡潔易讀             | **較佳** — 展現優化意識     |
| C 實作難度            | 簡單                      | 簡單                       |
| 尾遞迴優化            | 取決於編譯器旗標           | 不需要                     |

兩種方法時間複雜度相同。迭代法在空間複雜度上勝出（O(1) vs. O(H)），而且程式碼複雜度毫無增加。

## 基線方法 vs. 推薦方法

- **基線方法：** 遞迴（方法一）。最自然直覺的 BST 搜尋表達方式，直接反映 BST 定義。

- **推薦方法：** 迭代（方法二）。對這題而言，迭代版同樣簡潔好寫，但省去了遞迴堆疊的額外空間消耗。在面試中，先提出遞迴基線再優化為迭代，能展現對空間效率的敏感度 — 雖然是小細節，但能傳達有意義的訊號。

對於 Google 面試而言，這題太過基礎，不太可能單獨用來區分候選人。關鍵是快速且乾淨地解決，然後準備好討論 BST 性質、平衡 vs. 非平衡樹、以及後續的樹修改操作（插入、刪除）。
