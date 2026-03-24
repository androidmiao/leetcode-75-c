# 232. Implement Queue using Stacks - Editorial

> Source: LeetCode Official Editorial (live authenticated session)

## Summary

This editorial covers implementing a FIFO queue using two stacks. A queue inserts at the `rear` and removes from the `front`. A stack adds and removes from the same end, called `top`. Two stacks are needed to reverse the arrival order of elements so that the queue's FIFO property is satisfied.

## Approach #1: Two Stacks — Push O(n), Pop O(1)

### Push

Since a queue is FIFO but a stack is LIFO, the newest element must be pushed to the bottom of the stack. To achieve this:

1. Transfer all elements from `s1` to auxiliary stack `s2`.
2. Push the newly arrived element on top of `s1`.
3. Pop all elements from `s2` and push them back to `s1`.

This ensures `s1` always has the oldest element on top (queue front).

### Complexity Analysis — Push

- **Time:** O(n). Each existing element is moved twice (to `s2` then back to `s1`), plus the new element is pushed once. Total: 4n + 2 operations. Both `push` and `pop` have O(1) individual stack operations per call, but overall push is O(n).
- **Space:** O(n). Additional memory for storing queue elements.

### Pop

Simply pop from `s1`, which always has the first inserted element on top. A `front` variable tracks the queue's front element.

### Complexity Analysis — Pop

- **Time:** O(1).
- **Space:** O(1).

### Empty

Check `s1` size to determine if the queue is empty.

### Complexity Analysis — Empty

- **Time:** O(1).
- **Space:** O(1).

### Peek

The `front` element is kept in constant memory and is modified when we push or pop an element.

### Complexity Analysis — Peek

- **Time:** O(1). The `front` element has been calculated in advance and only returned in `peek` operation.
- **Space:** O(1).

---

## Approach #2: Two Stacks — Push O(1), Pop Amortized O(1)

### Push

The newly arrived element is always added on top of stack `s1`, and the first element is kept as the `front` queue element.

### Complexity Analysis — Push

- **Time:** O(1). Appending to a stack is O(1).
- **Space:** O(n). Additional memory for storing queue elements.

### Pop

We need to remove the element at the front of the queue, which is positioned at the bottom of `s1` due to LIFO. To access it, pop all elements from `s1` and push them onto `s2`, reversing their order. The bottom element of `s1` is now on top of `s2` and can be popped. Once `s2` is empty, the algorithm transfers data from `s1` to `s2` again.

### Complexity Analysis — Pop

- **Time:** Amortized O(1), Worst-case O(n). When `s2` is empty, the algorithm pops n elements from `s1` and pushes n elements to `s2`, giving 2n operations which is O(n). But when `s2` is not empty, it is O(1).
- **Space:** O(1).

### Amortized Analysis

Amortized analysis gives the average performance (over time) of each operation in the worst case. A worst case operation can alter the state so that the worst case cannot occur again for a long time, thus amortizing its cost.

Consider a sequence: push₁, push₂, ..., pushₙ, pop₁, pop₂, ..., popₙ

The worst case time complexity of a single pop operation is O(n). Since we have n pop operations, the number of times pop can be called is limited by the number of push operations before it. Although a single pop operation could be expensive, it is expensive only once per n times (queue size), when `s2` is empty and there is a need for data transfer between `s1` and `s2`. Hence the total time complexity of the sequence is: n (for push operations) + 2n (for first pop operation) + n − 1 (for pop operations) which is O(2 * n). This gives O(2n/2n) = O(1) average time per operation.

### Empty

Both stacks `s1` and `s2` contain all stack elements, so the algorithm checks `s1` and `s2` size to return if the queue is empty.

### Complexity Analysis — Empty

- **Time:** O(1).
- **Space:** O(1).

### Peek

The `front` element is kept in constant memory and is modified when we push an element. When `s2` is not empty, the front element is positioned on the top of `s2`.

### Complexity Analysis — Peek

- **Time:** O(1). The `front` element was either previously calculated or returned as the top element of stack `s2`. Therefore complexity is O(1).
- **Space:** O(1).

---

# 232. 用棧實現隊列 - 題解

> 來源：LeetCode 官方題解（透過已登入的瀏覽器會話取得）

## 概述

本題解介紹如何使用兩個棧來實現 FIFO 隊列。隊列從 `rear`（尾端）插入，從 `front`（前端）移除。棧則從同一端（稱為 `top`，頂部）進行新增和移除。為了滿足隊列的 FIFO 特性，需要兩個棧來反轉元素的到達順序。

## 方法一：兩個棧 — Push O(n)，Pop O(1)

### Push（推入）

由於隊列是 FIFO 而棧是 LIFO，新元素必須推到棧的底部。為此：

1. 將 `s1` 中的所有元素轉移到輔助棧 `s2`。
2. 將新到達的元素推入 `s1` 的頂部。
3. 將 `s2` 中的所有元素彈出並推回 `s1`。

這確保了 `s1` 始終將最早的元素放在頂部（隊列前端）。

### 複雜度分析 — Push

- **時間：** O(n)。每個現有元素被移動兩次（到 `s2` 再回到 `s1`），加上新元素推入一次。總計：4n + 2 次操作。
- **空間：** O(n)。需要額外記憶體來存儲隊列元素。

### Pop（彈出）

直接從 `s1` 彈出即可，因為 `s1` 始終將最先插入的元素放在頂部。使用 `front` 變數追蹤隊列的前端元素。

### 複雜度分析 — Pop

- **時間：** O(1)。
- **空間：** O(1)。

### Empty（判空）

檢查 `s1` 的大小來判斷隊列是否為空。

### 複雜度分析 — Empty

- **時間：** O(1)。
- **空間：** O(1)。

### Peek（查看前端）

`front` 元素保存在常數記憶體中，在推入或彈出元素時被修改。

### 複雜度分析 — Peek

- **時間：** O(1)。`front` 元素已預先計算，`peek` 操作只需返回該值。
- **空間：** O(1)。

---

## 方法二：兩個棧 — Push O(1)，Pop 攤還 O(1)

### Push（推入）

新到達的元素總是被推入棧 `s1` 的頂部，並且第一個元素被記錄為 `front` 隊列元素。

### 複雜度分析 — Push

- **時間：** O(1)。追加到棧是 O(1) 操作。
- **空間：** O(n)。需要額外記憶體來存儲隊列元素。

### Pop（彈出）

我們需要移除隊列前端的元素，該元素由於 LIFO 特性位於 `s1` 的底部。要存取它，將 `s1` 的所有元素彈出並推入 `s2`，反轉它們的順序。`s1` 的底部元素現在位於 `s2` 的頂部，可以直接彈出。一旦 `s2` 為空，演算法再次將資料從 `s1` 轉移到 `s2`。

### 複雜度分析 — Pop

- **時間：** 攤還 O(1)，最壞情況 O(n)。當 `s2` 為空時，演算法從 `s1` 彈出 n 個元素並推入 `s2`，共 2n 次操作，即 O(n)。但當 `s2` 非空時，為 O(1)。
- **空間：** O(1)。

### 攤還分析

攤還分析給出最壞情況下每個操作的平均效能。一個最壞情況的操作可以改變狀態，使得最壞情況在很長時間內不會再次發生，從而攤還其成本。

考慮一個序列：push₁, push₂, ..., pushₙ, pop₁, pop₂, ..., popₙ

單次 pop 操作的最壞情況時間複雜度是 O(n)。但 pop 被調用為昂貴操作的次數受限於它之前的 push 操作次數。雖然單次 pop 可能昂貴，但它每 n 次（隊列大小）才昂貴一次——當 `s2` 為空且需要在 `s1` 和 `s2` 之間轉移資料時。因此整個序列的總時間複雜度為：n（push 操作）+ 2n（第一次 pop 操作）+ n − 1（其餘 pop 操作）= O(2 * n)。這給出 O(2n/2n) = O(1) 的每操作平均時間。

### Empty（判空）

`s1` 和 `s2` 共同包含所有棧元素，因此演算法檢查 `s1` 和 `s2` 的大小來判斷隊列是否為空。

### 複雜度分析 — Empty

- **時間：** O(1)。
- **空間：** O(1)。

### Peek（查看前端）

`front` 元素保存在常數記憶體中，在推入元素時被修改。當 `s2` 非空時，前端元素位於 `s2` 的頂部。

### 複雜度分析 — Peek

- **時間：** O(1)。`front` 元素要麼已預先計算，要麼作為棧 `s2` 的頂部元素返回。因此複雜度為 O(1)。
- **空間：** O(1)。
