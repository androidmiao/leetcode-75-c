# 567. Permutation in String — Editorial

Source: Live authenticated LeetCode session (Editorial tab).

---

## Approach 1: Brute Force

**Idea:** Generate all permutations of `s1`, check if any is a substring of `s2`.

**Complexity:** O(l1! * l2) — too slow, will TLE.

---

## Approach 2: Sorting

**Idea:** Sort `s1`. Slide a window of size `l1` over `s2`, sort each window, compare with sorted `s1`.

**Complexity:**
- Time: O(l1 log l1 + l2 * l1 log l1)
- Space: O(l1)

---

## Approach 3: Hash Map Sliding Window

**Idea:** Compare character frequency maps. A permutation has the same frequency map as the original string.

**Complexity:**
- Time: O(l1 + l2 * 26) — 26 comparisons per window step
- Space: O(1) — fixed-size frequency arrays

---

## Approach 4: Sliding Window with `matches` Counter (Optimal)

**Intuition:** Instead of comparing the entire frequency array at each step, maintain a `matches` counter — the number of characters where `count1[i] == count2[i]`. When `matches == 26`, the window is a permutation.

**Algorithm:**
1. Build `count1[26]` from `s1`.
2. Initialize the first window of size `l1` in `s2`, build `count2[26]`.
3. Count initial `matches`.
4. Slide the window: for each new character entering and old character leaving:
   - Update `count2`
   - Update `matches` accordingly (+1 if now equal, -1 if was equal and now differ)
5. If at any point `matches == 26`, return `true`.

**Updating `matches` when adding character `c`:**
```
count2[c]++
if count2[c] == count1[c]:       matches++   (now equal)
elif count2[c] == count1[c] + 1: matches--   (was equal, now one too many)
```

**Complexity:**
- Time: O(l1 + l2) — constant work per window step
- Space: O(1) — two arrays of size 26

---

---

## 中文翻譯

### 方法四：滑動視窗 + matches 計數器（最優）

**核心想法：** 排列（Permutation）的字元頻率和原字串相同。用固定大小為 `len(s1)` 的視窗滑過 `s2`，當視窗的字元頻率與 `s1` 的頻率完全吻合時，回傳 true。

**效率優化：** 不是每步都比較全部 26 個字元，而是維護一個 `matches` 計數器，記錄有多少個字元種類的頻率已匹配。`matches == 26` 表示完全匹配。

**演算法步驟：**
1. 建立 s1 的頻率表 `count1[26]`
2. 初始化視窗（s2 的前 l1 個字元）的頻率表 `count2[26]`
3. 計算初始 matches 值
4. 每次右移視窗：
   - 加入新字元 `in`：count2[in]++，根據變化更新 matches
   - 移出舊字元 `out`：count2[out]--，根據變化更新 matches
   - 若 matches == 26 → 回傳 true
5. 處理最後一個視窗後回傳 `matches == 26`

**matches 更新邏輯（加入字元 c）：**
- `count2[c]++` 後若 `count2[c] == count1[c]`：matches++（剛好相等）
- `count2[c]++` 後若 `count2[c] == count1[c] + 1`：matches--（之前相等，現在多一個）

**複雜度：**
- 時間：O(l1 + l2) — 每步視窗移動只做常數量的工作
- 空間：O(1) — 兩個大小為 26 的陣列
