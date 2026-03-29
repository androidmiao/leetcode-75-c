# 155. Min Stack

**Difficulty:** Medium

## Problem Statement

Design a stack that supports push, pop, top, and retrieving the minimum element in constant time.

Implement the `MinStack` class:

- `MinStack()` initializes the stack object.
- `void push(int val)` pushes the element `val` onto the stack.
- `void pop()` removes the element on the top of the stack.
- `int top()` gets the top element of the stack.
- `int getMin()` retrieves the minimum element in the stack.

You must implement a solution with `O(1)` time complexity for each function.

## Example 1

```
Input
["MinStack","push","push","push","getMin","pop","top","getMin"]
[[],[-2],[0],[-3],[],[],[],[]]

Output
[null,null,null,null,-3,null,0,-2]

Explanation
MinStack minStack = new MinStack();
minStack.push(-2);
minStack.push(0);
minStack.push(-3);
minStack.getMin(); // return -3
minStack.pop();
minStack.top();    // return 0
minStack.getMin(); // return -2
```

## Constraints

- `-2^31 <= val <= 2^31 - 1`
- Methods `pop`, `top` and `getMin` operations will always be called on **non-empty** stacks.
- At most `3 * 10^4` calls will be made to `push`, `pop`, `top`, and `getMin`.

---

# 155. 最小棧（Min Stack）

**難度：** Medium

## 題目描述

設計一個支援 push、pop、top 以及在常數時間內取得最小元素的棧。

實作 `MinStack` 類別：

- `MinStack()` 初始化棧物件。
- `void push(int val)` 將元素 `val` 推入棧中。
- `void pop()` 移除棧頂元素。
- `int top()` 取得棧頂元素。
- `int getMin()` 取得棧中的最小元素。

你必須實作一個每個函式時間複雜度皆為 `O(1)` 的解法。

## 範例 1

```
輸入
["MinStack","push","push","push","getMin","pop","top","getMin"]
[[],[-2],[0],[-3],[],[],[],[]]

輸出
[null,null,null,null,-3,null,0,-2]

說明
MinStack minStack = new MinStack();
minStack.push(-2);    // 棧：[-2]，最小值：-2
minStack.push(0);     // 棧：[-2, 0]，最小值：-2
minStack.push(-3);    // 棧：[-2, 0, -3]，最小值：-3
minStack.getMin();    // 回傳 -3
minStack.pop();       // 移除 -3，棧：[-2, 0]，最小值：-2
minStack.top();       // 回傳 0
minStack.getMin();    // 回傳 -2
```

## 限制條件

- `-2^31 <= val <= 2^31 - 1`
- `pop`、`top` 和 `getMin` 操作只會在**非空**棧上呼叫。
- `push`、`pop`、`top` 和 `getMin` 最多被呼叫 `3 * 10^4` 次。

## 關鍵觀察

這題的核心挑戰在於 `getMin()` 必須是 O(1)。普通的棧只能 O(1) 存取棧頂，若要找最小值通常需要 O(n) 遍歷。解決方案是利用棧的 LIFO 特性：當某個元素 x 在棧中時，x 下方的所有元素不會改變，因此 x 對應的最小值也不會改變。這讓我們可以在 push 時預先計算並儲存每個位置的最小值。
