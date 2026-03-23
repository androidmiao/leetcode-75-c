# 210. Course Schedule II — Implementation vs Editorial Comparison

## Implementation vs. Editorial Comparison

The current `.c` solution uses **DFS post-order with 3-color marking** (WHITE/GRAY/BLACK) to produce a topological ordering while simultaneously detecting cycles. This corresponds directly to **Editorial Approach 1: Using Depth First Search**.

Key mapping between our implementation and the editorial:

- The editorial describes pushing finished nodes onto a stack; our implementation writes them sequentially into `result[]` in post-order. Since the adjacency list stores `course → prereq` edges (i.e., we DFS into prerequisites first), the post-order naturally yields a valid topological order without needing to reverse.
- The editorial uses the term "stack" conceptually; our flat array achieves the same effect because prerequisites are explored before the dependent course is appended.
- Cycle detection uses a `static bool has_cycle` flag with early termination (pruning) — once a back edge is found, all recursive calls return immediately. The editorial describes the same GRAY-node detection but doesn't emphasize the pruning optimization.

The editorial also presents **Approach 2: Using Node Indegree (Kahn's Algorithm)**, which is a BFS-based iterative method that naturally produces topological order by repeatedly dequeuing nodes with in-degree 0.

## Which Approach Is Better

| Criterion                  | DFS Post-order (our impl)     | Kahn's Algorithm (BFS)       |
|----------------------------|-------------------------------|------------------------------|
| Time complexity            | O(V + E)                      | O(V + E)                     |
| Space complexity           | O(V + E)                      | O(V + E)                     |
| Interview suitability      | High                          | High                         |
| C implementation difficulty| Easy                          | Easy                         |
| Produces topological order | Yes (post-order)              | Yes (dequeue order)          |
| Cycle detection            | Natural (GRAY back edge)      | Implicit (count < V)         |
| Recursion depth concern    | Yes (V up to 2000)            | No (iterative)               |
| Extends from 207 naturally | Yes (add result array)        | Yes (collect dequeued nodes) |

Both approaches are equally efficient. The key trade-offs:

- **DFS post-order** is a natural extension of the 207 solution — just add a result array to collect nodes in post-order. If you already implemented 207 with DFS, minimal changes are needed. The 3-color marking pattern is a fundamental graph algorithm that interviewers expect.
- **Kahn's Algorithm** is iterative (no stack overflow risk), arguably easier to explain step-by-step, and the "peel off in-degree-0 nodes" intuition is very visual. It also naturally handles follow-up questions like "what if I need to find the order level by level?" (just track queue levels).

## Baseline vs. Recommended

- **Baseline approach:** DFS post-order with 3-color marking (current implementation) — direct extension from 207, familiar DFS pattern, single `result[]` array addition.
- **Recommended approach for interview:** **Kahn's Algorithm (BFS)** — iterative (avoids recursion depth issues), the dequeue order *is* the topological order (no stack reversal confusion), and the in-degree concept is easy to explain on a whiteboard. When an interviewer asks "walk me through how this works," you can literally draw nodes being removed one by one as their in-degrees drop to zero — very intuitive.

For this specific problem, our DFS implementation is clean and correct. But if starting fresh in an interview, Kahn's gives a slight edge in clarity and extensibility.

---

## 中文翻譯

## 實作 vs. Editorial 比較

目前的 `.c` 解法使用 **DFS 後序 + 三色標記法**（WHITE/GRAY/BLACK）來產生拓撲排序，同時偵測環。這直接對應 **Editorial 方法一：深度優先搜尋**。

我們的實作與 Editorial 的關鍵對應：

- Editorial 描述將完成的節點推入堆疊；我們的實作在後序中依序寫入 `result[]`。由於鄰接表存放的是 `course → prereq` 的邊（即 DFS 先進入先修課），後序天然產出合法拓撲順序，無需反轉。
- Editorial 概念上使用「堆疊」；我們用平坦陣列達到相同效果。
- 環偵測使用 `static bool has_cycle` 旗標搭配提前終止（剪枝）— 一旦發現回邊，所有遞迴呼叫立即返回。

Editorial 也介紹了**方法二：入度法（Kahn 演算法）**，這是基於 BFS 的迭代式方法，透過反覆取出入度為 0 的節點來自然產生拓撲排序。

## 哪個方法更好

| 比較項目            | DFS 後序（我們的實作）         | Kahn 演算法（BFS）            |
|---------------------|-------------------------------|-------------------------------|
| 時間複雜度          | O(V + E)                      | O(V + E)                      |
| 空間複雜度          | O(V + E)                      | O(V + E)                      |
| 面試適用性          | 高                            | 高                            |
| C 語言實作難度      | 簡單                          | 簡單                          |
| 產生拓撲排序        | 是（後序）                    | 是（出佇列順序）              |
| 環偵測              | 天然支援（GRAY 回邊）          | 隱式（計數 < V）              |
| 遞迴深度風險        | 有（V 最多 2000）              | 無（迭代式）                  |
| 從 207 自然延伸     | 是（加結果陣列）               | 是（收集出佇列節點）          |

兩者效率相同。主要取捨：

- **DFS 後序** 是 207 解法的自然延伸 — 只需加一個結果陣列收集後序節點。若 207 已用 DFS 實作，改動極小。三色標記是面試官期望掌握的基礎圖演算法。
- **Kahn 演算法** 是迭代式的（無堆疊溢出風險），逐步解釋更容易，「剝離入度為 0 的節點」直覺上非常視覺化。對後續問題也有延伸優勢，例如「如果需要逐層返回順序？」（追蹤佇列層級即可）。

## 基線 vs. 建議方案

- **基線方案：** DFS 後序 + 三色標記（目前的實作）— 從 207 直接延伸，熟悉的 DFS 模式，僅增加 `result[]` 陣列。
- **面試建議方案：** **Kahn 演算法（BFS）** — 迭代式（避免遞迴深度問題），出佇列順序就是拓撲排序（無需堆疊反轉的混淆），入度概念在白板上容易解釋。面試官問「帶我走一遍流程」時，可以直接畫出節點隨入度降至零而逐個被移除的過程 — 非常直覺。

就本題而言，我們的 DFS 實作乾淨正確。但若在面試中從零開始寫，Kahn's 在清晰度和可延伸性上稍佔優勢。
