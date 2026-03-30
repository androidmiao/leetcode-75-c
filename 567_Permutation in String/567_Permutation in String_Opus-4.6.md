# 567. Permutation in String — Opus-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 4: Sliding Window with `matches` Counter**, which is the optimal approach from the editorial.

The implementation matches the editorial's optimal approach exactly, using a fixed-size window of `len(s1)` with O(1) per-step updates via the `matches` counter.

### Approach Comparison Table

| Criterion | Brute Force | Sorting | Hash Map Sliding | matches Counter |
|-----------|-------------|---------|-----------------|-----------------|
| Time Complexity | O(l1! × l2) | O(l1 l2 log l1) | O((l1+l2)×26) | **O(l1 + l2)** |
| Space Complexity | O(l1!) | O(l1) | O(1) | **O(1)** |
| Interview Suitability | None | Poor | Good | **Best** |
| C Implementation Difficulty | Hard | Medium | Easy | **Easy** |

### Baseline vs. Recommended

**Baseline approach:** Hash Map / Frequency Array Sliding Window (Approach 3)
- Compare `count1` vs `count2` at each step with O(26) comparison per step.
- O(26 × l2) = O(l2) effectively — acceptable but has larger constant.
- Fine to describe first in an interview, then optimize to `matches` counter.

**Recommended approach:** Sliding Window with `matches` Counter (Approach 4)
- O(l1 + l2) with truly O(1) per step — no 26-element comparison each time.
- The `matches` counter tracks how many of 26 characters currently have equal frequency.
- Two key update rules when modifying `count2[c]`:
  - **After increment:** if `count2[c] == count1[c]` → `matches++`; if `count2[c] == count1[c]+1` → `matches--`
  - **After decrement:** if `count2[c] == count1[c]` → `matches++`; if `count2[c] == count1[c]-1` → `matches--`
- Return `true` immediately when `matches == 26` (don't wait until end of loop).

### Which Approach Is Better

The `matches` counter (Approach 4) is strictly better than the frequency comparison (Approach 3) in all dimensions:

1. **Time:** Both are O(l1 + l2) asymptotically, but Approach 4 avoids 26 comparisons per step, making it faster in practice.
2. **Space:** Both use O(1) — two arrays of size 26.
3. **Interview:** Approach 4 demonstrates deeper understanding. The interviewer can see that you recognized the redundancy in Approach 3's full-array comparison and optimized it to incremental tracking.
4. **C Implementation:** Both are equally easy in C. The `matches` counter adds ~10 lines but no extra complexity.

**Key interview points:**
1. The fixed window size is `len(s1)` — this is what makes it "fixed" sliding window vs. variable.
2. The `matches` counter optimization reduces O(26) comparison per step to O(1).
3. Handle the last window check: the `for` loop checks `matches == 26` at start of each iteration except the very last window, so we check once more after the loop.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法四：matches 計數器滑動視窗**，是最優方案。

### 方法比較表

| 準則 | 暴力法 | 排序法 | 頻率表滑窗 | matches 計數器 |
|------|--------|--------|-----------|---------------|
| 時間複雜度 | O(l1! × l2) | O(l1 l2 log l1) | O((l1+l2)×26) | **O(l1 + l2)** |
| 空間複雜度 | O(l1!) | O(l1) | O(1) | **O(1)** |
| 面試適合度 | 無 | 差 | 好 | **最佳** |
| C 實作難度 | 難 | 中 | 易 | **易** |

### 基準方法 vs. 推薦方法

**基準方法：頻率陣列比較（方法三）**
- 每步視窗移動後，比較全部 26 個字元頻率
- O(26 × l2) — 有較大常數，但仍可接受作為面試的初步答案

**推薦方法：matches 計數器（方法四）**
- 每步只做 O(1) 的 matches 更新，完全避免了每步 26 次比較
- 核心不變量：`matches` 記錄 `count1[i] == count2[i]` 的字元種數
- `matches == 26` 時立即回傳 true

**更新邏輯（加入/移出字元時）：**
- 更新後 `count2[c] == count1[c]`：matches++（剛好相等）
- 加入後 `count2[c] == count1[c] + 1`：matches--（多了一個，之前相等現在不等）
- 移出後 `count2[c] == count1[c] - 1`：matches--（少了一個，之前相等現在不等）

### 哪個方法更好

matches 計數器（方法四）在所有維度都嚴格優於頻率比較（方法三）：

1. **時間：** 漸進複雜度相同都是 O(l1 + l2)，但方法四避免了每步 26 次比較，實際更快。
2. **空間：** 相同 O(1) — 兩個大小 26 的陣列。
3. **面試：** 方法四展現更深入的理解。面試官能看出你發現了方法三全陣列比較的冗餘，並優化為增量追蹤。
4. **C 實作：** 兩者同樣簡單。matches 計數器多約 10 行程式碼但不增加複雜性。

**面試重點：**
1. 固定視窗大小為 `len(s1)` — 這是「固定」滑動視窗與「可變」滑動視窗的區別。
2. matches 計數器優化將每步 O(26) 比較降為 O(1)。
3. 處理最後一個視窗：for 迴圈在每次迭代開頭檢查 `matches == 26`，但最後一個視窗不會進入迴圈，所以迴圈後要再檢查一次。
