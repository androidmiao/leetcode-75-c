已將 C 語言實作寫入 21_Merge Two Sorted Lists.c。內容是 LeetCode 可直接提交的迭代解法，使用 dummy 虛擬頭節點來簡化串列合併流程，並且已補上逐行中文註解。

這個解法的核心是每次比較 list1 和 list2 當前節點，把較小的節點接到結果串列尾端，直到其中一條串列走完，再把剩下那條串列直接接上去。時間複雜度是 O(n + m)，空間複雜度是 O(1)。



不完全一樣，但核心思路一致。

[`21_Merge Two Sorted Lists_Solution.md`](/Users/alex.miao/Documents/GitHub/leetcode-75-c/21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists_Solution.md) 裡有兩種解法：
1. 遞迴
2. 迭代

我寫在 [`21_Merge Two Sorted Lists.c`](/Users/alex.miao/Documents/GitHub/leetcode-75-c/21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c) 的是「迭代解法」，所以它對應的是 Solution.md 的 `Approach 2: Iteration`，不是 `Approach 1: Recursion`。

相同的地方是：
- 都是不斷比較 `list1` 和 `list2` 當前節點
- 每次把較小的節點接到結果串列後面
- 最後把剩下未處理完的串列直接接上

我這份程式也用了 `dummy`/`prehead` 虛擬頭節點，這點和 Solution.md 的迭代解法是同一類寫法，只是變數命名不同：
- Solution.md 用 `prehead`、`prev`
- 我的程式用 `dummy`、`tail`

如果你要，我可以再幫你把 [`21_Merge Two Sorted Lists_Solution.md`](/Users/alex.miao/Documents/GitHub/leetcode-75-c/21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists_Solution.md) 補一段「本專案採用的是迭代版」的中文說明。