# 841. Keys and Rooms — Editorial

> Source: LeetCode Official Editorial (live authenticated session, 2026-04-01)

## Approach #1: Depth-First Search [Accepted]

### Intuition and Algorithm

When visiting a room for the first time, look at all the keys in that room. For any key that hasn't been used yet, add it to the todo list (`stack`) for it to be used.

The algorithm maintains two structures:

- `seen` (or `visited`): a boolean array tracking which rooms have been entered.
- `stack`: a list of rooms still to be processed (keys collected but room not yet explored).

Starting from room 0, the DFS repeatedly pops a room from the stack, iterates over the keys inside, and pushes any unvisited room onto the stack. After the stack is exhausted, the answer is whether every entry in `seen` is `true`.

### Java

```java
class Solution {
    public boolean canVisitAllRooms(List<List<Integer>> rooms) {
        boolean[] seen = new boolean[rooms.size()];
        seen[0] = true;
        Stack<Integer> stack = new Stack();
        stack.push(0);

        //At the beginning, we have a todo list "stack" of keys to use.
        //'seen' represents at some point we have entered this room.
        while (!stack.isEmpty()) { // While we have keys...
            int node = stack.pop(); // Get the next key 'node'
            for (int nei: rooms.get(node)) // For every key in room # 'node'...
                if (!seen[nei]) { // ...that hasn't been used yet
                    seen[nei] = true; // mark that we've entered the room
                    stack.push(nei); // add the key to the todo list
                }
        }

        for (boolean v: seen)  // if any room hasn't been visited, return false
            if (!v) return false;
        return true;
    }
}
```

### Python

```python
class Solution(object):
    def canVisitAllRooms(self, rooms):
        seen = [False] * len(rooms)
        seen[0] = True
        stack = [0]
        # At the beginning, we have a todo list "stack" of keys to use.
        # 'seen' represents at some point we have entered this room.
        while stack:           # While we have keys...
            node = stack.pop() # get the next key 'node'
            for nei in rooms[node]:   # For every key in room # 'node'...
                if not seen[nei]:     # ... that hasn't been used yet
                    seen[nei] = True  # mark that we've entered the room
                    stack.append(nei) # add the key to the todo list
        return all(seen)       # Return true iff we've visited every room
```

### Complexity Analysis

- **Time Complexity:** O(N + E), where N is the number of rooms, and E is the total number of keys.
- **Space Complexity:** O(N) in additional space complexity, to store `stack` and `seen`.

---

# 繁體中文翻譯

## 方法一：深度優先搜尋（Depth-First Search, DFS）[已通過]

### 直覺與演算法

當你第一次拜訪某個房間時，查看該房間中的所有鑰匙。對於任何一把尚未使用過的鑰匙，就把它加入待辦清單（`stack`）中，之後再使用。

演算法維護兩個結構：

- `seen`（或 `visited`）：一個布林陣列，追蹤哪些房間已被進入。
- `stack`：一個待處理房間列表（已收集鑰匙但尚未探索的房間）。

從 0 號房間出發，DFS 反覆從 stack 中取出一個房間，遍歷裡面的鑰匙，並將所有尚未拜訪的房間推入 stack。當 stack 清空後，答案就是 `seen` 陣列中的每個值是否都為 `true`。

### 複雜度分析

- **時間複雜度：** O(N + E)，其中 N 是房間數量，E 是所有鑰匙的總數。
- **空間複雜度：** O(N) 的額外空間，用來儲存 `stack` 和 `seen`。
