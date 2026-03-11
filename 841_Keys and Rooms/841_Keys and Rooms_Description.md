There are n rooms labeled from 0 to n - 1 and all the rooms are locked except for room 0. Your goal is to visit all the rooms. However, you cannot enter a locked room without having its key.

When you visit a room, you may find a set of distinct keys in it. Each key has a number on it, denoting which room it unlocks, and you can take all of them with you to unlock the other rooms.

Given an array rooms where rooms[i] is the set of keys that you can obtain if you visited room i, return true if you can visit all the rooms, or false otherwise.

 

Example 1:

Input: rooms = [[1],[2],[3],[]]
Output: true
Explanation: 
We visit room 0 and pick up key 1.
We then visit room 1 and pick up key 2.
We then visit room 2 and pick up key 3.
We then visit room 3.
Since we were able to visit every room, we return true.
Example 2:

Input: rooms = [[1,3],[3,0,1],[2],[0]]
Output: false
Explanation: We can not enter room number 2 since the only key that unlocks it is in that room.
 

Constraints:

n == rooms.length
2 <= n <= 1000
0 <= rooms[i].length <= 1000
1 <= sum(rooms[i].length) <= 3000
0 <= rooms[i][j] < n
All the values of rooms[i] are unique.

---

中文翻譯：

有 `n` 個房間，編號從 `0` 到 `n - 1`，除了 `0` 號房間之外，其餘房間一開始都是上鎖的。你的目標是拜訪所有房間。不過，如果沒有對應的鑰匙，你就無法進入被鎖住的房間。

當你進入某個房間時，可能會在裡面找到一組互不相同的鑰匙。每把鑰匙上都有一個數字，表示它可以打開哪一個房間，而你可以把找到的所有鑰匙都帶走，用來開啟其他房間。

給定一個陣列 `rooms`，其中 `rooms[i]` 表示當你拜訪第 `i` 個房間時可以取得的鑰匙集合。如果你能拜訪所有房間，回傳 `true`；否則回傳 `false`。

範例 1：

輸入：`rooms = [[1],[2],[3],[]]`
輸出：`true`
解釋：
我們先進入 `0` 號房間並拿到鑰匙 `1`。
接著進入 `1` 號房間並拿到鑰匙 `2`。
然後進入 `2` 號房間並拿到鑰匙 `3`。
最後進入 `3` 號房間。
因為我們成功拜訪了每一個房間，所以回傳 `true`。

範例 2：

輸入：`rooms = [[1,3],[3,0,1],[2],[0]]`
輸出：`false`
解釋：我們無法進入編號 `2` 的房間，因為唯一能打開它的鑰匙就在那個房間裡。

限制條件：

`n == rooms.length`

`2 <= n <= 1000`

`0 <= rooms[i].length <= 1000`

`1 <= sum(rooms[i].length) <= 3000`

`0 <= rooms[i][j] < n`

`rooms[i]` 中的所有值皆互不相同。
