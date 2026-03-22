# 162. Find Peak Element — Editorial

Source: Live authenticated LeetCode session (Editorial tab).

---

## Approach 1: Linear Scan

**Intuition:** Traverse the array; whenever `nums[i] > nums[i+1]`, the current element `nums[i]` is a peak (because of the `-∞` boundary on both sides).

**Complexity:**
- Time: O(n)
- Space: O(1)

**Note:** Does not satisfy the O(log n) requirement. Mention briefly then optimize.

---

## Approach 2: Recursive Binary Search

**Intuition:** Divide the array at the midpoint. Compare `nums[mid]` with `nums[mid+1]`:
- If `nums[mid] > nums[mid+1]`: a peak exists in `[l, mid]` (mid could be the peak, or ascending towards it on the left).
- If `nums[mid] < nums[mid+1]`: a peak exists in `[mid+1, r]` (the right side is ascending, guaranteeing a peak).

**Key invariant:** The search space `[l, r]` always contains at least one peak.

**Complexity:**
- Time: O(log n)
- Space: O(log n) — recursion call stack depth

---

## Approach 3: Iterative Binary Search (Optimal)

**Intuition:** Same as Approach 2 but using a `while` loop instead of recursion to achieve O(1) space.

**Algorithm:**
1. Initialize `l = 0`, `r = n - 1`.
2. While `l < r`:
   - `mid = l + (r - l) / 2`
   - If `nums[mid] > nums[mid+1]`: `r = mid` (peak in left half, including mid)
   - Else: `l = mid + 1` (peak in right half, excluding mid)
3. Return `l` (at termination, `l == r`, and that element is a peak).

**Why is `l` guaranteed to be a peak when the loop ends?**
The invariant guarantees `[l, r]` always contains a peak. When `l == r`, there's only one element left, so it must be the peak.

**Why can we skip comparing `nums[mid]` with `nums[mid-1]`?**
We only reached `mid` from the left (`l = mid + 1` in the previous step), which means `nums[mid-1] < nums[mid]` was already established.

```
l = 0, r = n - 1
while l < r:
    mid = l + (r - l) / 2
    if nums[mid] > nums[mid + 1]:
        r = mid
    else:
        l = mid + 1
return l
```

**Complexity:**
- Time: O(log n)
- Space: O(1) ✓

---

---

## 中文翻譯

### 方法一：線性掃描

**直覺：** 遍歷陣列，當 `nums[i] > nums[i+1]` 時，`nums[i]` 就是峰值（邊界視為 -∞）。

**複雜度：** 時間 O(n)，空間 O(1) — 不滿足 O(log n) 要求

---

### 方法二：遞迴二分搜尋

**直覺：** 對 mid 進行比較，決定往左還是往右遞迴。不變量：搜尋空間 `[l, r]` 恆含至少一個峰值。

**複雜度：** 時間 O(log n)，空間 O(log n)（遞迴堆疊）

---

### 方法三：迭代二分搜尋（最優）

**直覺：** 與方法二相同，改用迴圈取代遞迴，節省遞迴堆疊空間。

**演算法：**
1. 初始化 `l = 0`，`r = n - 1`
2. 當 `l < r`：
   - 計算 `mid = l + (r - l) / 2`
   - 若 `nums[mid] > nums[mid+1]`：`r = mid`（峰值在左半段含 mid）
   - 否則：`l = mid + 1`（峰值在右半段不含 mid）
3. 回傳 `l`

**為何不需要和 nums[mid-1] 比較？**
我們之所以到達 mid，是因為前一步執行了 `l = mid + 1`，代表前一個元素比 mid 小，所以 `nums[mid-1] < nums[mid]` 已成立。

**複雜度：**
- 時間：O(log n)
- 空間：O(1) ✓
