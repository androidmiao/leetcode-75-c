# 35. Search Insert Position — Editorial

Source: Live authenticated LeetCode session (Editorial tab).

---

## Approach 1: Binary Search

**Intuition:** Since the array is sorted and distinct, we can apply binary search. The key insight is that when `target` is not found, `left` always ends up pointing to the correct insert position.

**Algorithm:**
1. Initialize `left = 0`, `right = n - 1`.
2. While `left <= right`:
   - Compute `pivot = left + (right - left) / 2`.
   - If `target == nums[pivot]`: return `pivot`.
   - If `target < nums[pivot]`: `right = pivot - 1` (search left half).
   - Else: `left = pivot + 1` (search right half).
3. Return `left`.

**Why does returning `left` work when target is not found?**
When the loop exits, `right < left`. At that point:
```
nums[right] < target < nums[left]
```
So `target` should be inserted at index `left`, shifting all elements from `left` onward to the right.

**Example (target not found):**
```
nums = [1, 3, 5, 6], target = 2

Round 1: left=0, right=3, pivot=1, nums[1]=3
         target(2) < 3  →  right = 0

Round 2: left=0, right=0, pivot=0, nums[0]=1
         target(2) > 1  →  left = 1

left(1) > right(0), loop exits  →  return left = 1
```

**Integer Overflow Note:**
Use `pivot = left + (right - left) / 2` instead of `(left + right) / 2` to avoid integer overflow when `left + right > INT_MAX` (important in C/C++/Java).

**Complexity:**
- Time: O(log n)
- Space: O(1)

---

---

## 中文翻譯

### 方法一：二分搜尋

**直覺：** 陣列已排序且無重複，可以使用二分搜尋。關鍵洞察是：當 target 不存在時，`left` 指標最終停在正確的插入位置。

**演算法：**
1. 初始化 `left = 0`，`right = n - 1`。
2. 當 `left <= right`：
   - 計算中點 `pivot = left + (right - left) / 2`
   - 若 `target == nums[pivot]`：回傳 `pivot`
   - 若 `target < nums[pivot]`：`right = pivot - 1`（搜左半段）
   - 否則：`left = pivot + 1`（搜右半段）
3. 回傳 `left`

**為何不在時回傳 left？**
迴圈結束時 `right < left`，此時：
```
nums[right] < target < nums[left]
```
target 應該插在 left 的位置，把 left 及其之後的元素向右移一位。

**整數溢位注意：**
使用 `pivot = left + (right - left) / 2` 而非 `(left + right) / 2`，以避免 `left + right` 超過 `INT_MAX`。

**複雜度：**
- 時間：O(log n)
- 空間：O(1)
