# 567. Permutation in String — Sonnet-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 4: Sliding Window with `matches` Counter**, which is the optimal approach from the editorial.

The implementation matches the editorial's optimal approach exactly, using a fixed-size window of `len(s1)` with O(1) per-step updates via the `matches` counter.

### Approach Comparison Table

| Criterion | Brute Force | Sorting | Hash Map Sliding | matches Counter |
|-----------|-------------|---------|-----------------|-----------------|
| Time Complexity | O(l1! * l2) | O(l1 l2 log l1) | O((l1+l2)*26) | **O(l1 + l2)** |
| Space Complexity | O(l1!) | O(l1) | O(1) | **O(1)** |
| Interview Suitability | None | Poor | Good | **Best** |
| C Implementation Difficulty | Hard | Medium | Easy | **Easy** |

### Baseline vs. Recommended

**Baseline approach:** Hash Map / Frequency Array Sliding Window (Approach 3)
- Compare `count1` vs `count2` at each step with O(26) comparison per step.
- O(26 * l2) = O(l2) effectively — acceptable but has larger constant.
- Fine to describe first in an interview, then optimize to `matches` counter.

**Recommended approach:** Sliding Window with `matches` Counter (Approach 4)
- O(l1 + l2) with truly O(1) per step — no 26-element comparison each time.
- The `matches` counter tracks how many of 26 characters currently have equal frequency.
- Two key update rules when modifying `count2[c]`:
  - **After increment:** if `count2[c] == count1[c]` → `matches++`; if `count2[c] == count1[c]+1` → `matches--`
  - **After decrement:** if `count2[c] == count1[c]` → `matches++`; if `count2[c] == count1[c]-1` → `matches--`
- Return `true` immediately when `matches == 26` (don't wait until end of loop).

**Key interview points:**
1. The fixed window size is `len(s1)` — this is what makes it "fixed" sliding window vs. variable.
2. The `matches` counter optimization reduces O(26) comparison per step to O(1).
3. Handle the last window check: the `for` loop checks `matches == 26` at start of each iteration except the very last window, so we check once more after the loop.

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 的**方法四：matches 計數器滑動視窗**，是最優方案。

### 基準方法 vs. 推薦方法

**基準方法：頻率陣列比較（方法三）**
- 每步視窗移動後，比較全部 26 個字元頻率
- O(26 * l2) — 有較大常數，但仍可接受作為面試的初步答案

**推薦方法：matches 計數器（方法四）**
- 每步只做 O(1) 的 matches 更新，完全避免了每步 26 次比較
- 核心不變量：`matches` 記錄 `count1[i] == count2[i]` 的字元種數
- `matches == 26` 時立即回傳 true

**更新邏輯（加入/移出字元時）：**
- 更新後 `count2[c] == count1[c]`：matches++（剛好相等）
- 加入後 `count2[c] == count1[c] + 1`：matches--（多了一個，之前相等現在不等）
- 移出後 `count2[c] == count1[c] - 1`：matches--（少了一個，之前相等現在不等）
