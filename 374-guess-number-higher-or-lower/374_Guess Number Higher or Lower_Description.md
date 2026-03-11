We are playing the Guess Game. The game is as follows:

I pick a number from 1 to n. You have to guess which number I picked (the number I picked stays the same throughout the game).

Every time you guess wrong, I will tell you whether the number I picked is higher or lower than your guess.

You call a pre-defined API int guess(int num), which returns three possible results:

-1: Your guess is higher than the number I picked (i.e. num > pick).
1: Your guess is lower than the number I picked (i.e. num < pick).
0: your guess is equal to the number I picked (i.e. num == pick).
Return the number that I picked.

 

Example 1:

Input: n = 10, pick = 6
Output: 6
Example 2:

Input: n = 1, pick = 1
Output: 1
Example 3:

Input: n = 2, pick = 1
Output: 1
 

Constraints:

1 <= n <= 231 - 1
1 <= pick <= n

---

中文翻譯：

我們正在玩「猜數字遊戲（Guess Game）」。遊戲規則如下：

我會從 1 到 n 之間選一個數字。你必須猜出我選的是哪一個數字（在整個遊戲過程中，我選的數字都不會改變）。

每當你猜錯時，我會告訴你，我選的數字是比你猜的數字更大，還是更小。

你可以呼叫一個預先定義好的 API `int guess(int num)`，它會回傳以下三種結果之一：

- `-1`：你猜的數字比我選的數字大（也就是 `num > pick`）。
- `1`：你猜的數字比我選的數字小（也就是 `num < pick`）。
- `0`：你猜的數字剛好等於我選的數字（也就是 `num == pick`）。

請回傳我選的那個數字。

範例 1：

輸入：`n = 10, pick = 6`  
輸出：`6`

範例 2：

輸入：`n = 1, pick = 1`  
輸出：`1`

範例 3：

輸入：`n = 2, pick = 1`  
輸出：`1`

限制條件：

- `1 <= n <= 2^31 - 1`
- `1 <= pick <= n`
