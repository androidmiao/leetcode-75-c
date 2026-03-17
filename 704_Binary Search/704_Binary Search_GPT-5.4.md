已把題目的 C 語言實作寫進 [704_Binary Search.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search.c)，而且是可直接提交的 LeetCode 版本，內含逐行中文註解。

這份實作使用標準二分搜尋：
- 有序陣列下，每次看中間值
- 比 `target` 小就往右找
- 比 `target` 大就往左找
- 找到就回傳索引，否則回傳 `-1`

我也已在註解中補上 `mid = left + (right - left) / 2` 的原因，避免直接寫 `left + right` 可能帶來的整數溢位風險。  



已把圖解說明直接記錄進 [704_Binary Search.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search.c)。

現在這個檔案包含：

可提交的 C 程式
逐行中文註解
二分搜尋的 ASCII 圖解
target = 9 的實際搜尋流程示範



這份內容比較長，我已維持原本段落順序翻譯，包括：

Overview
Approach 1 ~ 4
Intuition
Algorithm
Complexity Analysis



有，一樣，但對應的是 `Approach 1: Find the Exact Value`。

理由很直接：
- 你的程式在 [704_Binary Search.c:49](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search.c#L49) 到 [704_Binary Search.c:55](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search.c#L55) 用的是 `left = 0`、`right = numsSize - 1`、`while (left <= right)`。
- 這和 [704_Binary Search_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search_Solution.md) 裡 `Approach 1` 的描述一致：搜尋區間是包含端點的 `[left, right]`，找到 `nums[mid] == target` 就直接回傳。

所以結論是：
- 跟文件的 `Approach 1` 一樣
- 不同於 `Approach 2` 的 `upper bound`
- 也不同於 `Approach 3` 的 `lower bound`
- 更不是 `Approach 4` 的內建工具寫法

補一句精準一點的說法：邏輯本質相同，只有我在 [704_Binary Search.c:58](/Users/alex.miao/Documents/GitHub/leetcode-75-c/704_Binary Search/704_Binary Search.c#L58) 用 `left + (right - left) / 2` 來避免溢位，這比直接寫 `(left + right) / 2` 更穩。