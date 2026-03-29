# 295. Find Median from Data Stream - Implementation vs Editorial Comparison

## English Section

### 1. Implementation vs Editorial Comparison

The C implementation uses the **Two Heaps approach** (matching **Editorial Approach 3**). However, there is a subtle but important difference in the insertion strategy:

**Editorial Approach 3 (Always-Add-to-Lo-First):**
1. Always add `num` to the max-heap `lo`
2. Pop the max from `lo` to `hi` (ensures `lo` ≤ `hi`)
3. If `hi` has more elements than `lo`, pop min from `hi` back to `lo` (rebalances)

This is a straightforward, uniform strategy: always route through `lo` first.

**C Implementation (Classify-First-Then-Rebalance):**
1. Check if `num <= max_heap[0]` to decide which heap to insert into directly
2. If true, add to `max_heap`; otherwise, add to `min_heap`
3. Then check and rebalance: if `max_heap > min_heap + 1`, move max_heap top to min_heap; if `min_heap > max_heap`, move min_heap top to max_heap

This is a two-phase strategy: first classify the element based on the current max_heap top, then selectively rebalance.

**Both approaches are mathematically equivalent** — they maintain the same invariants and produce identical results. The C implementation's variant may reduce heap operations on average by avoiding unnecessary moves through the intermediate heap.

---

### 2. Approaches Comparison Table

| Approach | Time: addNum | Time: findMedian | Space | Pros | Cons | Interview Suitability |
|----------|--------------|------------------|-------|------|------|----------------------|
| **Approach 1: Simple Sorting** | O(1) amortized | **O(n log n)** | O(n) | Easy to implement; straightforward | Median query is very slow | Baseline; rarely used in interviews |
| **Approach 2: Insertion Sort** | **O(n)** | O(1) | O(n) | Sorted array access; O(1) median | Slow insertions due to shifting | Sometimes asked for variety |
| **Approach 3: Two Heaps (Recommended)** | **O(log n)** | **O(1)** | O(n) | Balanced, efficient; industry standard | Requires heap understanding | **Strongly preferred** in interviews |
| **Approach 4: Multiset + Two Pointers** | O(log n) | O(1) | O(n) | Elegant; BST properties | Hard to implement in C (no STL) | Easier in C++/Python; rare in C interviews |

---

### 3. Baseline vs Recommended

**Baseline Approach: Simple Sorting (Approach 1)**
- Store all numbers in a dynamic array; sort on each median query
- **Time:** O(1) addNum, O(n log n) findMedian
- **Interview Context:** This is rarely acceptable unless the problem specifically states that `findMedian` is called rarely. It demonstrates understanding of the problem but does not showcase optimization skills.

**Recommended Approach: Two Heaps (Approach 3)**
- Maintain one max-heap for the smaller half and one min-heap for the larger half
- **Time:** O(log n) addNum, O(1) findMedian
- **Interview Context:** This is the **gold standard** for this problem. It shows:
  - Deep understanding of heap data structures
  - Ability to maintain invariants in a non-trivial system
  - Optimal balance between insertion and query operations
  - Awareness of real-world use cases (online median in streaming data)

**Why Two Heaps is Better:**
1. **Balanced Trade-off:** O(log n) insertion and O(1) query is superior to O(1) insertion with O(n log n) queries, especially when the number of `findMedian` calls is comparable to or greater than the number of `addNum` calls.
2. **Streaming Scenarios:** In real-world data streams, the median is often queried frequently. Two Heaps excels here.
3. **Scalability:** As data grows, the cost of sorting (Approach 1) becomes prohibitive; Two Heaps scales gracefully.
4. **Interview Signal:** Demonstrates mastery of advanced data structures.

---

### 4. Minor Variation in Insertion Strategy

The C implementation and Editorial Approach 3 differ in how they route elements to the heaps:

**Editorial:** "Always add to `lo` first" is a uniform rule that simplifies the invariant proof but may perform an extra heap operation.

**C Implementation:** "Classify first by comparing with `max_heap[0]`" reduces the number of heap operations in the average case because many elements are already correctly classified on the first insertion.

**Trade-off Analysis:**
- Editorial's approach: 3 heap operations in worst case (add to lo, pop to hi, pop back to lo) but very easy to reason about.
- C's approach: 1–2 heap operations on average (add directly to the correct heap, possibly one rebalance pop-and-insert). More efficient in practice but slightly more complex to implement.

Both are valid; the C implementation optimizes for fewer operations without sacrificing correctness.

---

## Traditional Chinese Section (繁體中文)

### 1. 實作與題解對比

C 實作採用**兩個堆方法**（符合**題解方法三**）。然而，在插入策略上存在一個微妙但重要的差異：

**題解方法三（總是先加到 lo）：**
1. 將 `num` 總是加入最大堆 `lo`
2. 將 `lo` 的最大元素彈出到 `hi`（確保 `lo` ≤ `hi`）
3. 若 `hi` 的元素比 `lo` 多，將 `hi` 的最小元素彈回 `lo`（重新平衡）

這是一個直白統一的策略：總是先經過 `lo`。

**C 實作（先分類再平衡）：**
1. 檢查 `num <= max_heap[0]` 以決定直接放入哪個堆
2. 若為真，加入 `max_heap`；否則加入 `min_heap`
3. 然後檢查並平衡：若 `max_heap > min_heap + 1`，移出 max_heap 堆頂到 min_heap；若 `min_heap > max_heap`，移出 min_heap 堆頂到 max_heap

這是一個兩階段的策略：先根據當前 max_heap 堆頂分類元素，再有選擇性地重新平衡。

**兩種方法在數學上是等價的** — 它們維護相同的不變式，產生相同的結果。C 實作的變異可能透過避免不必要的中間堆移動，來減少平均情況下的堆操作次數。

---

### 2. 方法對比表

| 方法 | 時間: addNum | 時間: findMedian | 空間 | 優點 | 缺點 | 面試適合度 |
|------|--------------|------------------|------|------|------|-----------|
| **方法一：簡單排序** | O(1) 攤銷 | **O(n log n)** | O(n) | 易於實作；直觀 | 中位數查詢極慢 | 基準方案；面試罕用 |
| **方法二：插入排序** | **O(n)** | O(1) | O(n) | 有序陣列存取；O(1) 中位數 | 插入因移位而緩慢 | 偶爾為了多樣性詢問 |
| **方法三：兩個堆（推薦）** | **O(log n)** | **O(1)** | O(n) | 均衡、高效；業界標準 | 需要堆知識 | **強烈傾向**於面試 |
| **方法四：多重集合與雙指標** | O(log n) | O(1) | O(n) | 優雅；BST 性質 | C 中難以實作（無 STL） | C++ 和 Python 更簡單；C 面試罕見 |

---

### 3. 基準方案 vs 推薦方案

**基準方案：簡單排序（方法一）**
- 將所有數字儲存在動態陣列中；每次查詢中位數時排序
- **時間：** O(1) addNum，O(n log n) findMedian
- **面試背景：** 除非題目特別指出 `findMedian` 很少被呼叫，否則此方案幾乎不被接受。它展示了對問題的理解，但沒有展現最佳化技能。

**推薦方案：兩個堆（方法三）**
- 為較小的一半維護一個最大堆，為較大的一半維護一個最小堆
- **時間：** O(log n) addNum，O(1) findMedian
- **面試背景：** 這是**此問題的黃金標準**。它展示了：
  - 對堆資料結構的深入理解
  - 在非平凡系統中維護不變式的能力
  - 在插入與查詢操作之間的最優平衡
  - 對真實應用場景的認知（串流資料中的線上中位數）

**為什麼兩個堆更好：**
1. **均衡的權衡：** O(log n) 插入和 O(1) 查詢優於 O(1) 插入加 O(n log n) 查詢，特別是當 `findMedian` 呼叫次數與 `addNum` 次數相當或更多時。
2. **串流場景：** 在真實資料串流中，中位數經常被查詢。兩個堆在此表現卓越。
3. **可擴展性：** 隨著資料增長，排序（方法一）的成本變得難以接受；兩個堆可以優雅地擴展。
4. **面試信號：** 展示對進階資料結構的掌握。

---

### 4. 插入策略中的微妙差異

C 實作與題解方法三在如何將元素路由到堆中有所不同：

**題解：** "總是先加到 `lo`" 是一個統一的規則，簡化了不變式的證明，但可能執行額外的堆操作。

**C 實作：** "透過與 `max_heap[0]` 比較先分類" 可以減少平均情況下的堆操作，因為許多元素在第一次插入時就已被正確分類。

**權衡分析：**
- 題解的方法：最壞情況下 3 次堆操作（加到 lo、彈到 hi、彈回 lo），但非常易於推理。
- C 的方法：平均情況下 1–2 次堆操作（直接加到正確的堆，可能一次重新平衡的彈出插入）。實務上更高效，但實作略微複雜。

兩者都有效；C 實作在不犧牲正確性的情況下，最佳化了操作次數。

---

