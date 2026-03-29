# 133. Clone Graph — Implementation vs. Editorial Comparison

## 1. Implementation vs. Editorial Comparison

The .c solution uses DFS (same as Editorial Approach 1). The key difference is the hash map implementation:
- **Editorial (C version)**: Uses uthash (a C hash table library) with `struct Node*` as key
- **Our implementation**: Uses a simple array `visited[101]` indexed by `node->val`, leveraging the constraint that `Node.val` is unique and in [1, 100]

Both are functionally identical DFS algorithms — the only difference is the hash map backing.

## 2. Which Approach Is Better

| Dimension | DFS (Array map) | DFS (uthash) | BFS (Array map) |
|-----------|-----------------|--------------|-----------------|
| Time Complexity | O(V+E) | O(V+E) | O(V+E) |
| Space Complexity | O(V) | O(V) | O(V) |
| Interview Suitability | ★★★★★ | ★★★☆☆ | ★★★★☆ |
| C Implementation Difficulty | Easy | Medium (uthash boilerplate) | Medium (queue management) |
| Recursion Stack Risk | Yes (O(H)) | Yes (O(H)) | No |

## 3. Baseline vs. Recommended

- **Baseline**: BFS with array map — straightforward, no recursion stack risk
- **Recommended (Google interview)**: DFS with array map — cleanest code, easiest to explain, exploits the val-uniqueness constraint elegantly. The recursion depth is bounded by 100 nodes max so stack overflow is not a concern.

The DFS approach is recommended because:
1. Shortest, most readable code
2. The val-indexed array trick shows constraint awareness
3. Easy to walk through with an interviewer
4. Natural recursive structure matches the graph's recursive nature

---

# 133. 克隆圖 — 實現方案與官方解答比較

## 1. 實現方案與官方解答比較

.c 解決方案採用 DFS（與官方解答方案 1 相同）。主要區別在於雜湊表的實現方式：
- **官方解答 (C 版本)**：使用 uthash（C 雜湊表庫），以 `struct Node*` 作為鍵
- **我們的實現**：使用簡單的陣列 `visited[101]`，按 `node->val` 索引，利用 `Node.val` 唯一且在 [1, 100] 範圍內的約束

兩者在功能上是完全相同的 DFS 演算法 — 唯一的區別是雜湊表的底層實現方式。

## 2. 哪種方案更好

| 維度 | DFS (陣列對映) | DFS (uthash) | BFS (陣列對映) |
|------|-----------------|--------------|-----------------|
| 時間複雜度 | O(V+E) | O(V+E) | O(V+E) |
| 空間複雜度 | O(V) | O(V) | O(V) |
| 面試適用性 | ★★★★★ | ★★★☆☆ | ★★★★☆ |
| C 實現難度 | 簡單 | 中等 (uthash 樣板程式碼) | 中等 (佇列管理) |
| 遞迴堆疊風險 | 有 (O(H)) | 有 (O(H)) | 無 |

## 3. 基準方案 vs. 推薦方案

- **基準方案**：BFS 搭配陣列對映 — 直接明確，無遞迴堆疊風險
- **推薦方案 (Google 面試)**：DFS 搭配陣列對映 — 程式碼最簡潔、最容易解釋，優雅地利用了 val 唯一性約束。遞迴深度受限於最多 100 個節點，因此無堆疊溢位風險。

推薦 DFS 方案的理由：
1. 程式碼最簡短、最易讀
2. 以值為索引的陣列技巧展示了對約束條件的掌握
3. 便於與面試官逐步分析
4. 自然的遞迴結構與圖的遞迴特性相符合