# 33. Search in Rotated Sorted Array — Opus-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 2: One-Pass Binary Search**, which is the editorial's recommended approach. The implementation is essentially identical to the editorial's algorithm: at each midpoint, determine which half is sorted, then check whether the target falls within the sorted half to decide the search direction.

### Approach Comparison Table

| Criterion | Approach 1: Find Pivot + Binary Search | Approach 2: One-Pass Binary Search (Used) |
|-----------|----------------------------------------|-------------------------------------------|
| Time Complexity | O(log n) | **O(log n)** |
| Space Complexity | O(1) | **O(1)** |
| Number of passes | 2 (find pivot + search) | **1** |
| Interview Suitability | Good (clear separation of concerns) | **Better (shows deeper insight)** |
| C Implementation Difficulty | Easy–Medium | Medium |
| Edge case handling | Pivot search needs careful boundary | `<=` in `nums[l] <= nums[mid]` handles `l == mid` |
| Code length | ~30 lines | **~20 lines** |

### Baseline vs. Recommended

**Baseline approach:** Find Pivot Index + Binary Search (Approach 1)

This approach decomposes the problem into two well-understood sub-problems: (1) find the rotation point using a modified binary search comparing `nums[mid]` against `nums[n-1]`, then (2) run standard binary search on the appropriate sorted subarray. It is a natural first answer in an interview because it shows structured thinking — but it requires two passes and the pivot-finding binary search has its own edge cases (e.g., array not rotated at all).

**Recommended approach:** One-Pass Binary Search (Approach 2)

This approach exploits the key invariant: in any rotated sorted array, at least one half of any binary-search window is always sorted. The decision tree at each step is:

1. If `nums[l] <= nums[mid]` → left half `[l, mid]` is sorted
   - If `nums[l] <= target < nums[mid]` → search left (`r = mid - 1`)
   - Else → search right (`l = mid + 1`)
2. Else → right half `[mid, r]` is sorted
   - If `nums[mid] < target <= nums[r]` → search right (`l = mid + 1`)
   - Else → search left (`r = mid - 1`)

**Why the recommended approach is better for interview discussion:**

1. **Demonstrates deeper algorithmic insight** — recognizing that one half is always sorted is the "aha" moment interviewers want to see.
2. **Single pass is cleaner** — no need to explain or debug a separate pivot-finding phase.
3. **Naturally handles edge cases** — the `<=` in `nums[l] <= nums[mid]` covers the `l == mid` case (subarray of size 1–2), and the non-rotated array case requires no special handling.
4. **Extendable** — the same framework adapts to follow-up questions like "what if duplicates are allowed?" (LeetCode 81) by adding a `nums[l] == nums[mid]` case.

### Key Interview Discussion Points

1. **Why one half is always sorted** — if the pivot is in the right half, the left half is fully sorted, and vice versa. This is because rotation only introduces one "break point."
2. **Why `<=` not `<`** — when `l == mid` (window size 1 or 2), `nums[l] == nums[mid]` and the left "half" is trivially sorted (single element). Using `<` would incorrectly route to the else-branch.
3. **Strict inequality design** — `nums[l] <= target < nums[mid]` uses `<` on the right to exclude `mid` itself (already checked). Similarly `nums[mid] < target <= nums[r]` excludes `mid` on the left.

---

## 繁體中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法二：一次二分搜尋**，與 Editorial 描述的演算法本質上相同：每個中點判斷哪半有序，再決定 target 是否在有序那半。

### 基準方法 vs. 推薦方法

**基準方法：找 Pivot + 二分搜尋（方法一）**

將問題拆成兩個子問題：(1) 用改良二分搜尋找旋轉點，(2) 在正確的有序子陣列上做標準二分搜尋。面試時可以作為自然的第一個回答，展示結構化思維，但需要兩次掃描且 pivot 搜尋本身有邊界情況。

**推薦方法：一次二分搜尋（方法二）**

利用核心不變量：旋轉有序陣列中，任意二分搜尋窗口至少有一半是連續有序的。

判斷樹：
1. 若 `nums[l] <= nums[mid]` → 左半段有序
   - 若 `nums[l] <= target < nums[mid]` → 往左搜
   - 否則 → 往右搜
2. 否則 → 右半段有序
   - 若 `nums[mid] < target <= nums[r]` → 往右搜
   - 否則 → 往左搜

**為何推薦方法更適合面試討論：**

1. **展示更深的演算法洞察** — 認識到必有一半有序是面試官想看到的核心突破點
2. **單次掃描更簡潔** — 不需要額外解釋或除錯獨立的 pivot 搜尋階段
3. **自然處理邊界情況** — `<=` 處理 `l == mid` 的情況，未旋轉的陣列也不需特殊處理
4. **可擴展性** — 相同框架可適應追問（如允許重複元素的 LeetCode 81）

### 面試關鍵討論點

1. **為何必有一半有序** — 旋轉只引入一個「斷點」，pivot 在右半段則左半段完全有序，反之亦然
2. **`<=` vs `<` 的差異** — 當 `l == mid` 時（窗口大小 1 或 2），`nums[l] == nums[mid]`，左「半」段只有單一元素，視為有序。用 `<` 會錯誤地進入 else 分支
3. **嚴格不等式設計** — `nums[l] <= target < nums[mid]` 右邊用 `<` 排除已檢查的 `mid`；`nums[mid] < target <= nums[r]` 左邊用 `<` 同理
