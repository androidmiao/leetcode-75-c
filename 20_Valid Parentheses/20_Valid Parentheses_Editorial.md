# 20. Valid Parentheses — Editorial

## Intuition

The problem is analogous to a compiler checking whether parentheses in source code are balanced. For a simplified version with only one type of bracket (e.g. `()`), a simple counter suffices: increment on `(`, decrement on `)`, and reject if the counter ever goes negative or is nonzero at the end.

However, with three types of brackets, a counter alone fails because it cannot enforce *ordering*. For instance, `[{]` has matching counts but is invalid — the closest unmatched opening bracket when we see `]` is `{`, not `[`. We need a data structure that remembers the *order* of opening brackets, and a stack provides exactly this LIFO property.

## Approach 1: Stacks

### Key Observation

A valid parentheses expression has a recursive sub-expression property: any sub-expression enclosed by a matching pair is itself valid. When we encounter a closing bracket, the most recent unmatched opening bracket (top of stack) must be the one that matches it.

### Algorithm

1. Initialize an empty stack.
2. Process the string character by character from left to right.
3. If we encounter an opening bracket (`(`, `[`, `{`), push it onto the stack.
4. If we encounter a closing bracket (`)`, `]`, `}`):
   - If the stack is empty, return `false` (no matching opener).
   - Pop the top element. If it is not the matching opening bracket for this closing bracket, return `false`.
5. After processing the entire string, if the stack is empty, the expression is valid; otherwise it is not.

### Complexity Analysis

- **Time complexity:** O(n) — we traverse the string once; each push and pop is O(1).
- **Space complexity:** O(n) — in the worst case (e.g. `((((((((`), all characters are pushed onto the stack.

### Early Exit Optimization

If the string length is odd, it cannot be valid — return `false` immediately.

---

# 20. 有效的括號 — 題解

## 直覺

此問題等同於編譯器檢查原始碼中的括號是否平衡。若只有一種括號（如 `()`），用一個計數器即可解決：遇到 `(` 加一，遇到 `)` 減一，若計數器曾變為負數或最終不為零則判定無效。

但在有三種括號的情況下，單純的計數器無法處理**順序**問題。例如 `[{]` 的各類括號數量相同，但它是無效的——遇到 `]` 時，最近未匹配的開括號是 `{` 而非 `[`。我們需要一個能記住開括號**順序**的資料結構，而堆疊（Stack）正好提供了這種後進先出（LIFO）的特性。

## 方法一：堆疊（Stack）

### 核心觀察

有效的括號表達式具有遞迴子表達式性質：任何被一對匹配括號包圍的子表達式本身也是有效的。當我們遇到一個閉括號時，最近未匹配的開括號（堆疊頂部）必須與其配對。

### 演算法步驟

1. 初始化一個空堆疊。
2. 從左到右逐字元處理字串。
3. 若遇到開括號（`(`、`[`、`{`），將其推入堆疊。
4. 若遇到閉括號（`)`、`]`、`}`）：
   - 若堆疊為空，回傳 `false`（無對應的開括號）。
   - 彈出堆疊頂部元素。若該元素不是此閉括號對應的開括號，回傳 `false`。
5. 處理完整個字串後，若堆疊為空則表達式有效，否則無效。

### 複雜度分析

- **時間複雜度：** O(n) — 遍歷字串一次，每次推入和彈出都是 O(1)。
- **空間複雜度：** O(n) — 最壞情況（如 `((((((((`）所有字元都被推入堆疊。

### 提前退出優化

若字串長度為奇數，則不可能有效——直接回傳 `false`。
