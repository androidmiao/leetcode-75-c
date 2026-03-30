# 692. Top K Frequent Words — Opus-4.6 Implementation vs. Editorial Comparison

---

## English Version

### Implementation vs. Editorial Comparison

The current `.c` solution implements **Approach 3: Min Heap** from the editorial. It is the same algorithm, not merely a variation.

Both the editorial description and the C implementation follow this exact workflow:

1. Count word frequencies using a hash map.
2. Iterate through all unique words.
3. Maintain a min heap of size at most `k`, where the heap root is the "worst" among the current top `k` candidates.
4. When a new candidate is better than the heap root, replace the root and sift down.
5. After processing all words, pop the heap to reconstruct the answer in the correct order.

The C implementation fills in the concrete details that the editorial leaves abstract: a djb2-based hash table with separate chaining, explicit `siftUp`/`siftDown` on a pointer array, and a reverse-fill loop to convert the min-heap pop order into the required descending-frequency / ascending-lexicographic output.

### Which Approach Is Better

| Dimension | Approach 1: Brute Force | Approach 3: Min Heap (current) | Approach 4: Bucket + Trie |
|---|---|---|---|
| Time complexity | O(N log N) | O(N log k) | O(N) |
| Space complexity | O(N) | O(N) | O(N) |
| Interview suitability | Good baseline | Best overall | Too complex for whiteboard |
| C implementation difficulty | Low (qsort) | Medium (manual heap) | High (trie + DFS) |

### Baseline vs. Recommended

**Baseline approach**: Approach 1 (Brute Force) — count frequencies, sort, take first `k`. Simple to explain and code, runs in O(N log N).

**Recommended approach**: Approach 3 (Min Heap) — maintains a size-`k` min heap for O(N log k) time. This is the recommended Google interview approach because:

1. It directly satisfies the follow-up requirement of O(n log k) time.
2. Size-`k` min heap is the standard pattern interviewers expect for top-k problems.
3. The natural narrative is: explain brute force first, then optimize to min heap — this demonstrates tradeoff awareness.
4. It is more practical to implement correctly on a whiteboard than Approach 4 (Bucket + Trie), which has O(N) time but large constant factors and heavy implementation burden.

---

## 中文翻譯

### 實作 vs. Editorial 對照

目前的 `.c` 解法實作的是 editorial 的 **Approach 3: Min Heap**，兩者是相同的演算法，不只是變體。

Editorial 描述與 C 實作都遵循完全相同的流程：

1. 用 hash map 統計每個單字的頻率。
2. 遍歷所有不同單字。
3. 維護一個大小最多為 `k` 的 min heap，heap 頂端放的是目前 top `k` 裡最差的候選。
4. 當新候選比 heap 頂端更好時，替換頂端並做 sift down。
5. 處理完所有單字後，從 heap 依序 pop 出來，重建成正確的輸出順序。

C 實作把 editorial 留白的細節補齊了：使用 djb2 hash 搭配 separate chaining 的 hash table、在指標陣列上實作明確的 `siftUp`/`siftDown`、以及用反向填充迴圈把 min heap 的 pop 順序轉成題目要求的「頻率遞減 / 字典序遞增」輸出。

### 哪個 Approach 比較好

| 面向 | Approach 1: Brute Force | Approach 3: Min Heap（目前版本） | Approach 4: Bucket + Trie |
|---|---|---|---|
| 時間複雜度 | O(N log N) | O(N log k) | O(N) |
| 空間複雜度 | O(N) | O(N) | O(N) |
| 面試適合度 | 適合當 baseline | 整體最推薦 | 白板上太複雜 |
| C 實作難度 | 低（用 qsort） | 中等（手寫 heap） | 高（trie + DFS） |

### Baseline vs. 推薦方案

**Baseline 方案**：Approach 1（Brute Force）— 計數、排序、取前 `k` 個。簡單好講、好寫，時間複雜度 O(N log N)。

**推薦方案**：Approach 3（Min Heap）— 維護大小為 `k` 的 min heap，時間複雜度 O(N log k)。這是 Google 面試推薦的做法，原因如下：

1. 直接滿足 follow-up 要求的 O(n log k) 時間複雜度。
2. Size-`k` min heap 是面試官對 top-k 題型期待看到的標準模式。
3. 自然的敘事方式：先講 brute force，再優化到 min heap — 展現 tradeoff 思考能力。
4. 比 Approach 4（Bucket + Trie）更穩、更容易在白板或編輯器中正確實作。Approach 4 雖然漸進複雜度是 O(N)，但常數因子大、實作負擔重。
