# 232. Implement Queue using Stacks

**Difficulty:** Easy

## Problem Statement

Implement a first in first out (FIFO) queue using only two stacks. The implemented queue should support all the functions of a normal queue (`push`, `peek`, `pop`, and `empty`).

Implement the `MyQueue` class:

- `void push(int x)` Pushes element x to the back of the queue.
- `int pop()` Removes the element from the front of the queue and returns it.
- `int peek()` Returns the element at the front of the queue.
- `boolean empty()` Returns `true` if the queue is empty, `false` otherwise.

**Notes:**

- You must use **only** standard operations of a stack, which means only `push to top`, `peek/pop from top`, `size`, and `is empty` operations are valid.
- Depending on your language, the stack may not be supported natively. You may simulate a stack using a list or deque (double-ended queue) as long as you use only a stack's standard operations.

## Example

**Example 1:**

```
Input
["MyQueue", "push", "push", "peek", "pop", "empty"]
[[], [1], [2], [], [], []]
Output
[null, null, null, 1, 1, false]

Explanation
MyQueue myQueue = new MyQueue();
myQueue.push(1); // queue is: [1]
myQueue.push(2); // queue is: [1, 2] (leftmost is front of the queue)
myQueue.peek();  // return 1
myQueue.pop();   // return 1, queue is [2]
myQueue.empty(); // return false
```

## Constraints

- `1 <= x <= 9`
- At most `100` calls will be made to `push`, `pop`, `peek`, and `empty`.
- All the calls to `pop` and `peek` are valid.

## Follow-up

Can you implement the queue such that each operation is **amortized** `O(1)` time complexity? In other words, performing `n` operations will take overall `O(n)` time even if one of those operations may take longer.

---

# 232. 用棧實現隊列

**難度：** 簡單

## 題目描述

僅使用兩個棧來實現一個先進先出（FIFO）的隊列。實現的隊列應該支援一般隊列的所有功能（`push`、`peek`、`pop` 和 `empty`）。

實現 `MyQueue` 類別：

- `void push(int x)` 將元素 x 推到隊列的末尾。
- `int pop()` 從隊列的前端移除元素並返回該元素。
- `int peek()` 返回隊列前端的元素。
- `boolean empty()` 如果隊列為空則返回 `true`，否則返回 `false`。

**注意：**

- 你只能使用棧的標準操作，也就是只有 `push to top`（推到頂部）、`peek/pop from top`（從頂部查看/彈出）、`size`（大小）和 `is empty`（是否為空）操作是有效的。
- 根據你使用的程式語言，棧可能不是原生支援的。你可以使用列表（list）或雙端隊列（deque）來模擬棧，只要你只使用棧的標準操作即可。

## 範例

**範例 1：**

```
輸入
["MyQueue", "push", "push", "peek", "pop", "empty"]
[[], [1], [2], [], [], []]
輸出
[null, null, null, 1, 1, false]

說明
MyQueue myQueue = new MyQueue();
myQueue.push(1); // 隊列為：[1]
myQueue.push(2); // 隊列為：[1, 2]（最左邊是隊列前端）
myQueue.peek();  // 返回 1
myQueue.pop();   // 返回 1，隊列變為 [2]
myQueue.empty(); // 返回 false
```

## 約束條件

- `1 <= x <= 9`
- 最多對 `push`、`pop`、`peek` 和 `empty` 進行 `100` 次呼叫。
- 所有對 `pop` 和 `peek` 的呼叫都是有效的。

## 進階

你能否實現使每個操作的**攤還**時間複雜度為 `O(1)` 的隊列？換句話說，執行 `n` 個操作的總時間為 `O(n)`，即使其中某個操作可能花費較長時間。
