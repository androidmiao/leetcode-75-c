Approach #1: Depth-First Search [Accepted]
Intuition and Algorithm

When visiting a room for the first time, look at all the keys in that room. For any key that hasn't been used yet, add it to the todo list (stack) for it to be used.

See the comments of the code for more details.

//Python
class Solution(object):
    def canVisitAllRooms(self, rooms):
        seen = [False] * len(rooms)
        seen[0] = True
        stack = [0]
        #At the beginning, we have a todo list "stack" of keys to use.
        #'seen' represents at some point we have entered this room.
        while stack:  #While we have keys...
            node = stack.pop() # get the next key 'node'
            for nei in rooms[node]: # For every key in room # 'node'...
                if not seen[nei]: # ... that hasn't been used yet
                    seen[nei] = True # mark that we've entered the room
                    stack.append(nei) # add the key to the todo list
        return all(seen) # Return true iff we've visited every room


Complexity Analysis

Time Complexity: O(N+E), where N is the number of rooms, and E is the total number of keys.

Space Complexity: O(N) in additional space complexity, to store stack and seen.

---

中文翻譯：

方法一：深度優先搜尋（Depth-First Search, DFS）[已通過]

直覺與演算法

當你第一次拜訪某個房間時，查看該房間中的所有鑰匙。對於任何一把尚未使用過的鑰匙，就把它加入待辦清單（stack）中，之後再使用。

更多細節可參考程式碼中的註解。

// Python
```python
class Solution(object):
    def canVisitAllRooms(self, rooms):
        seen = [False] * len(rooms)
        seen[0] = True
        stack = [0]
        # 一開始，我們有一個待處理的鑰匙清單 "stack"。
        # "seen" 代表這個房間是否曾經被進入過。
        while stack:  # 只要還有鑰匙可用
            node = stack.pop()  # 取出下一把鑰匙對應的房間 node
            for nei in rooms[node]:  # 逐一查看 room node 中的每把鑰匙
                if not seen[nei]:  # 如果這把鑰匙對應的房間還沒進去過
                    seen[nei] = True  # 標記該房間已進入
                    stack.append(nei)  # 把這把鑰匙加入待辦清單
        return all(seen)  # 只有所有房間都拜訪過時才回傳 true
```

複雜度分析

時間複雜度：`O(N + E)`，其中 `N` 是房間數量，`E` 是所有鑰匙的總數。

空間複雜度：`O(N)` 的額外空間，用來儲存 `stack` 和 `seen`。
