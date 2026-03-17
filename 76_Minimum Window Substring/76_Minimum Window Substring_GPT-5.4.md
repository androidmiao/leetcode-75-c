已把 C 實作寫到 76_Minimum Window Substring.c，內容是 LeetCode 可提交的 minWindow，採用 sliding window + 計數陣列，並加上逐行中文註解。

這個版本的重點是用 need[128] 記錄還缺哪些字元，right 先擴張把需求補齊，left 再收縮找最短合法視窗，所以時間複雜度是 O(m + n)，空間複雜度是 O(1)（固定大小陣列）。

我也做了編譯檢查，語法正常。


已把圖解說明直接記錄到 76_Minimum Window Substring.c 裡，放在函式前面的多行註解中。

內容包含 left/right 指標怎麼移動、required 何時變成 0、為什麼能開始收縮，以及 need[] 陣列在每一步代表什麼。也重新做過編譯檢查，檔案仍可正常編譯。




不完全一樣。

我寫的程式在 [76_Minimum Window Substring.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring.c) 本質上是 `Approach 1: Sliding Window`，也就是標準滑動視窗：`right` 擴張、`left` 收縮、用計數陣列追蹤需求。這和 [76_Minimum Window Substring_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring_Solution.md) 的第一種方法一致，但不是第二種 `Optimized Sliding Window`。

差異主要有兩個：

- 文件的 `Approach 2` 會先把 `S` 過濾成只保留 `T` 內可能有用的字元，再做滑動視窗。
- 我的程式沒有先過濾，而是直接在原字串上滑動；但我用了固定大小 `need[128]` 陣列，所以在這題「只有英文字母」的前提下，空間其實是 `O(1)`，而且實作更簡潔。

哪一個比較好，要看你重視什麼：

- 如果重視「好寫、好懂、面試穩定、LeetCode 好提交」，我現在這份比較好。
- 如果重視「當 `S` 很長、而且大多數字元都和 `T` 無關時的常數效能」，文件的 `Approach 2` 可能更好。

就這題的 C 實作來說，我會選現在這份。原因是它已經是 `O(m + n)`，而且更短、更直觀、出錯機率更低。`Approach 2` 理論上更優化，但程式複雜度也更高，實務上未必值得。