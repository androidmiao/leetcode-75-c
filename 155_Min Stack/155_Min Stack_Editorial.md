# 155. Min Stack - Editorial

## Overview

The key insight is that a stack only adds and removes from the top. When element `x` is on the stack, the elements below it never change. Therefore, the minimum for every prefix of the stack can be precomputed and stored alongside each element.

Important observations:
- `getMin()` only returns the minimum — it does not remove it.
- All operations must run in O(1) time, ruling out Binary Search Tree or Heap (whose core operations are O(log n)).
- Test cases are always valid (pop/top/getMin on non-empty stacks).

---

## Approach 1: Stack of Value/Minimum Pairs

### Intuition

Store each element as a pair `(value, current_minimum)` in a single stack. When pushing a new value, compute the new minimum as `min(value, previous_top_minimum)`. The `getMin()` operation simply reads the second field of the top pair.

### Algorithm

1. **push(x):** If the stack is empty, push `(x, x)`. Otherwise, push `(x, min(x, stack.top().min))`.
2. **pop():** Pop the top pair.
3. **top():** Return the first field of the top pair.
4. **getMin():** Return the second field of the top pair.

### Complexity

- **Time:** O(1) for all operations.
- **Space:** O(n) — each element stores a value and a minimum, so worst case (all pushes) is O(2n) = O(n).

---

## Approach 2: Two Stacks

### Intuition

Instead of storing pairs, use two separate stacks: a main data stack and a min-tracker stack. The min-tracker stack only gets a new entry pushed when the incoming value is less than or equal to the current minimum.

### Algorithm

1. **push(x):** Always push `x` onto the main stack. If the min-tracker stack is empty or `x <= minStack.top()`, also push `x` onto the min-tracker stack.
2. **pop():** Pop from the main stack. If the popped value equals `minStack.top()`, also pop the min-tracker stack.
3. **top():** Return the top of the main stack.
4. **getMin():** Return the top of the min-tracker stack.

### Key Detail — Why `<=` Not `<`

Using strict `<` causes a bug when duplicate minimums are pushed. For example, pushing `[12, 7, 6, 45, 6]` — if we only push to min-tracker on strict `<`, the second `6` is not pushed. When the first `6` is popped, the min-tracker's `6` is also removed, even though a `6` still exists in the main stack.

Using `<=` ensures duplicates of the current minimum are tracked correctly.

### Complexity

- **Time:** O(1) for all operations.
- **Space:** O(n) — in the worst case (strictly decreasing sequence), the min-tracker stack has the same size as the main stack.

---

## Approach 3: Improved Two Stacks (with Count)

### Intuition

When the same minimum value is pushed repeatedly, Approach 2 stores redundant copies on the min-tracker stack. An improvement is to store `(value, count)` pairs on the min-tracker stack. When a duplicate minimum arrives, increment the count instead of pushing a new entry.

### Algorithm

1. **push(x):** Push `x` onto the main stack. If `x < minStack.top().value`, push `(x, 1)` onto the min-tracker stack. If `x == minStack.top().value`, increment `minStack.top().count`.
2. **pop():** Pop from the main stack. If the popped value equals `minStack.top().value`, decrement the count. If the count reaches 0, pop the min-tracker stack.
3. **top():** Return the top of the main stack.
4. **getMin():** Return `minStack.top().value`.

### Complexity

- **Time:** O(1) for all operations.
- **Space:** O(n) — asymptotically the same, but in practice uses less space when there are many duplicate minimums.

---

# 155. 最小棧 - 題解

## 概覽

核心洞見：棧只從頂部新增和移除元素。當元素 `x` 在棧中時，`x` 下方的元素永遠不會改變。因此，棧的每個前綴的最小值都可以預先計算並儲存在每個元素旁邊。

重要觀察：
- `getMin()` 只回傳最小值，不會移除它。
- 所有操作必須是 O(1)，排除了二元搜尋樹或堆積（核心操作是 O(log n)）。
- 測試案例永遠合法（pop/top/getMin 只在非空棧上呼叫）。

---

## 方法一：值/最小值配對棧

### 直覺

在單一棧中將每個元素存為配對 `(value, current_minimum)`。推入新值時，計算新的最小值為 `min(value, 前一個棧頂的最小值)`。`getMin()` 直接讀取棧頂配對的第二個欄位。

### 演算法

1. **push(x)：** 若棧為空，推入 `(x, x)`。否則推入 `(x, min(x, stack.top().min))`。
2. **pop()：** 彈出棧頂配對。
3. **top()：** 回傳棧頂配對的第一個欄位。
4. **getMin()：** 回傳棧頂配對的第二個欄位。

### 複雜度

- **時間：** 所有操作 O(1)。
- **空間：** O(n) — 每個元素儲存一個值和一個最小值，最壞情況（全部 push）為 O(2n) = O(n)。

---

## 方法二：兩個棧

### 直覺

不儲存配對，改用兩個獨立的棧：一個主資料棧和一個最小值追蹤棧。最小值追蹤棧只在新值小於等於當前最小值時才推入新條目。

### 演算法

1. **push(x)：** 永遠將 `x` 推入主棧。若最小值追蹤棧為空或 `x <= minStack.top()`，也將 `x` 推入最小值追蹤棧。
2. **pop()：** 從主棧彈出。若彈出的值等於 `minStack.top()`，也彈出最小值追蹤棧。
3. **top()：** 回傳主棧的棧頂。
4. **getMin()：** 回傳最小值追蹤棧的棧頂。

### 關鍵細節 — 為什麼用 `<=` 而不是 `<`

使用嚴格 `<` 會在有重複最小值時產生 bug。例如推入 `[12, 7, 6, 45, 6]` — 若只在嚴格 `<` 時才推入追蹤棧，第二個 `6` 不會被推入。當第一個 `6` 被彈出時，追蹤棧的 `6` 也被移除，但主棧中仍存在一個 `6`。

使用 `<=` 確保重複的最小值能被正確追蹤。

### 複雜度

- **時間：** 所有操作 O(1)。
- **空間：** O(n) — 最壞情況（嚴格遞減序列），追蹤棧與主棧大小相同。

---

## 方法三：改進的兩個棧（帶計數）

### 直覺

當相同的最小值被重複推入時，方法二會在追蹤棧上儲存冗餘副本。改進方式是在追蹤棧上儲存 `(value, count)` 配對。當重複最小值到來時，遞增計數而非推入新條目。

### 演算法

1. **push(x)：** 將 `x` 推入主棧。若 `x < minStack.top().value`，推入 `(x, 1)` 到追蹤棧。若 `x == minStack.top().value`，遞增 `minStack.top().count`。
2. **pop()：** 從主棧彈出。若彈出的值等於 `minStack.top().value`，遞減計數。若計數歸零，彈出追蹤棧。
3. **top()：** 回傳主棧的棧頂。
4. **getMin()：** 回傳 `minStack.top().value`。

### 複雜度

- **時間：** 所有操作 O(1)。
- **空間：** O(n) — 漸近相同，但實際上在有大量重複最小值時使用更少空間。

---

*內容來源：LeetCode 155 Editorial 頁面（已登入的即時瀏覽器工作階段）。*
