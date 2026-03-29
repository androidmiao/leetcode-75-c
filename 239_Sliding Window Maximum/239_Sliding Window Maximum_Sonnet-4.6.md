# 239. Sliding Window Maximum — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The `.c` solution and the editorial both use the **same core algorithm**: a monotonic decreasing deque that stores indices, maintaining the invariant that `nums[deque[front]]` is always the current window's maximum.

**Key difference**: The `.c` implementation uses a **circular array** of size `k` to simulate the deque (`front`, `size`, modulo arithmetic), whereas the editorial's C++ reference code uses `std::deque<int>` backed by a dynamically resized structure. This is a minor implementation-level difference — the algorithm and logic are identical.

Both implementations follow the same steps:
1. For each new element, remove expired front element if it has left the window.
2. Pop all back elements with values ≤ current element (maintain monotonic decreasing order).
3. Push current index to back.
4. Record `nums[deque[front]]` as the window maximum once the first full window is formed.

## Which Approach is Better

| Dimension                  | Circular Array Deque (`.c`)       | std::deque (Editorial C++)         |
|----------------------------|------------------------------------|------------------------------------|
| Time complexity            | O(n)                               | O(n)                               |
| Space complexity           | O(k) — fixed allocation            | O(k) — but with dynamic overhead   |
| Memory allocations         | 1 malloc of size k                 | Multiple internal allocations       |
| Cache locality             | Excellent (contiguous k ints)      | Good but fragmented                 |
| Interview suitability (C)  | Strong — shows low-level mastery   | N/A (C++ only)                      |
| Implementation difficulty  | Moderate — modulo arithmetic       | Easy — library container            |
| Correctness risk           | Slightly higher (off-by-one in %)  | Lower (library handles edges)       |

## Baseline vs. Recommended

- **Baseline approach**: Brute force — for each of the n−k+1 windows, scan all k elements for the max. Time O(n·k), space O(1) extra. Simple but TLEs on large inputs.

- **Recommended approach**: Monotonic decreasing deque (the editorial and `.c` approach). This is the standard interview answer for this classic problem. The circular-array variant used in the `.c` solution is especially strong for a C interview because it demonstrates:
  - Understanding of the monotonic deque pattern
  - Ability to implement a deque from scratch using a circular buffer
  - Memory efficiency (only k slots allocated)
  - Familiarity with modular arithmetic for ring buffer indexing

For a Google interview, the monotonic deque approach is the expected answer. The circular buffer implementation adds extra points by showing systems-level thinking and memory-conscious design, which aligns well with Google's emphasis on efficiency.

---

# 239. 滑動視窗最大值 — 實作與官方題解比較

## 實作 vs. 官方題解比較

`.c` 解法與官方題解使用的是**完全相同的核心演算法**：以單調遞減雙端佇列儲存索引，維持 `nums[deque[front]]` 永遠是當前視窗最大值的不變式。

**主要差異**：`.c` 實作使用大小為 `k` 的**環形陣列**來模擬 deque（透過 `front`、`size` 及取模運算），而官方題解的 C++ 參考程式碼使用 `std::deque<int>`，其底層是動態調整大小的結構。這只是實作層級的差異，演算法與邏輯完全一致。

兩種實作都遵循相同的步驟：
1. 對每個新元素，若隊首索引已離開視窗範圍則移除。
2. 從隊尾彈出所有對應值 ≤ 當前元素的索引（維持單調遞減性質）。
3. 將當前索引推入隊尾。
4. 當第一個完整視窗形成後，記錄 `nums[deque[front]]` 作為視窗最大值。

## 哪種做法更好

| 面向                    | 環形陣列 Deque（`.c`）           | std::deque（官方 C++）             |
|-------------------------|-----------------------------------|------------------------------------|
| 時間複雜度              | O(n)                              | O(n)                               |
| 空間複雜度              | O(k) — 固定配置                   | O(k) — 但有動態配置開銷           |
| 記憶體配置次數          | 1 次 malloc，大小為 k             | 多次內部配置                       |
| 快取局部性              | 優秀（連續的 k 個 int）           | 尚可但較分散                       |
| 面試適用性（C 語言）    | 強 — 展現底層掌控力               | 不適用（僅限 C++）                 |
| 實作難度                | 中等 — 需要取模運算               | 簡單 — 使用標準函式庫容器          |
| 正確性風險              | 稍高（取模可能出現 off-by-one）   | 較低（函式庫處理邊界情況）         |

## 基準做法 vs. 推薦做法

- **基準做法**：暴力法 — 對每個 n−k+1 個視窗，掃描全部 k 個元素求最大值。時間 O(n·k)，額外空間 O(1)。簡單但在大測資會超時（TLE）。

- **推薦做法**：單調遞減雙端佇列（官方題解與 `.c` 使用的方法）。這是此經典題目的標準面試解法。`.c` 中使用的環形陣列變體在 C 語言面試中特別亮眼，因為它展示了：
  - 對單調 deque 模式的理解
  - 從零實作 deque（使用環形緩衝區）的能力
  - 記憶體效率（僅配置 k 個位置）
  - 對環形緩衝區索引取模運算的熟練度

在 Google 面試中，單調 deque 是預期的解法。環形緩衝區的實作方式能額外加分，展現系統層級思維與記憶體效率意識，這與 Google 對效能的重視高度契合。
