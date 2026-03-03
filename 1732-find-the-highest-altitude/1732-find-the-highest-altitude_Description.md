There is a biker going on a road trip. The road trip consists of n + 1 points at different altitudes. The biker starts his trip on point 0 with altitude equal 0.

You are given an integer array gain of length n where gain[i] is the net gain in altitude between points i​​​​​​ and i + 1 for all (0 <= i < n). Return the highest altitude of a point.

 

Example 1:

Input: gain = [-5,1,5,0,-7]
Output: 1
Explanation: The altitudes are [0,-5,-4,1,1,-6]. The highest is 1.
Example 2:

Input: gain = [-4,-3,-2,-1,4,3,2]
Output: 0
Explanation: The altitudes are [0,-4,-7,-9,-10,-6,-3,-1]. The highest is 0.
 

Constraints:

n == gain.length
1 <= n <= 100
-100 <= gain[i] <= 100

---
有一位騎士正在進行公路旅行。這趟旅行由 `n + 1` 個不同海拔的點組成。騎士從第 `0` 個點出發，該點海拔為 `0`。

給你一個長度為 `n` 的整數陣列 `gain`，其中 `gain[i]` 表示點 `i` 與點 `i + 1` 之間的海拔淨變化（`0 <= i < n`）。請回傳所有點中的最高海拔。

 

範例 1：

輸入：`gain = [-5,1,5,0,-7]`
輸出：`1`
說明：海拔依序為 `[0,-5,-4,1,1,-6]`，最高海拔是 `1`。
範例 2：

輸入：`gain = [-4,-3,-2,-1,4,3,2]`
輸出：`0`
說明：海拔依序為 `[0,-4,-7,-9,-10,-6,-3,-1]`，最高海拔是 `0`。

 

限制條件：

`n == gain.length`
`1 <= n <= 100`
`-100 <= gain[i] <= 100`


---
