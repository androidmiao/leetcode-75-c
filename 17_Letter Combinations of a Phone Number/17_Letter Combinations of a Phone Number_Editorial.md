Overview
One of the first things you should always do is look at the constraints. Quite often, you can figure out what sort of approach needs to be taken simply from looking at the input size. In an interview, asking your interviewer about the constraints will also show your attention to detail - on top of giving you information.

In this particular problem, the length of the input is extremely small, 0 <= digits.length <= 4. With such small input sizes, we can safely assume that a brute force solution in which we generate all combinations of letters will be accepted.

Whenever you have a problem where you need to generate all combinations/permutations of some group of letters/numbers, the first thought you should have is backtracking. If you're new to backtracking, check out our backtracking explore card. Backtracking algorithms can often keep the space complexity linear with the input size.


Approach 1: Backtracking
Intuition

There aren't any smart tricks needed for this problem - the hard part is just figuring out how to correctly generate all possible combinations, and to do this using a standard backtracking algorithm template. Let's break down the problem, by starting with an input that is only 1-digit long, for example digits = "2". This example is trivial - just generate all letters that correspond with digit = "2", which would be ["a", "b", "c"].

What if instead we had a 2-digit long input, digits = "23"? Imagine taking each letter of digit = "2" as a starting point. That is, lock the first letter in, and solve all the possible combinations that start with that letter. If our first letter will always be "a", then the problem is trivial again - it's the 1-digit case, and all we have to do is generate all the letters corresponding with digit = "3", and add that to "a", to get ["ad", "ae","af"]. This was easy because we ignored the first letter, and said it will always be "a". But we know how to generate all the first letters too - it's the 1-digit case which we already solved to be ["a", "b", "c"].

As you can see, solving the 1-digit case is trivial, and solving the 2-digit case is just solving the 1-digit case twice. The same reasoning can be extended to n digits. For the 3-digit case, solve the 2-digit case to generate all combinations of the first 2 letters, and then solve the 1-digit case for the final digit. Now that we know how to solve the 3-digit case, to solve the 4-digit case, solve the 3-digit case for all combinations of the first 3 letters, and then solve the 1-digit case for the final digit. We could extend this to infinity, but, don't worry, for this problem we're finished after 4.

Current

Algorithm

As mentioned previously, we need to lock-in letters when we generate new letters. The easiest way to save state like this is to use recursion. Our algorithm will be as follows:

If the input is empty, return an empty array.

Initialize a data structure (e.g. a hash map) that maps digits to their letters, for example, mapping "6" to "m", "n", and "o".

Use a backtracking function to generate all possible combinations.

The function should take 2 primary inputs: the current combination of letters we have, path, and the index we are currently checking.
As a base case, if our current combination of letters is the same length as the input digits, that means we have a complete combination. Therefore, add it to our answer, and backtrack.
Otherwise, get all the letters that correspond with the current digit we are looking at, digits[index].
Loop through these letters. For each letter, add the letter to our current path, and call backtrack again, but move on to the next digit by incrementing index by 1.
Make sure to remove the letter from path once finished with it.
Implementation


Complexity Analysis

Time complexity: O(4 
N
 ⋅N), where N is the length of digits. Note that 4 in this expression is referring to the maximum value length in the hash map, and not to the length of the input.

The worst-case is where the input consists of only 7s and 9s. In that case, we have to explore 4 additional paths for every extra digit. Then, for each combination, it costs up to N to build the combination. This problem can be generalized to a scenario where numbers correspond with up to M digits, in which case the time complexity would be O(M 
N
 ⋅N). For the problem constraints, we're given, M=4, because of digits 7 and 9 having 4 letters each.

Space complexity: O(N), where N is the length of digits.

Not counting space used for the output, the extra space we use relative to input size is the space occupied by the recursion call stack. It will only go as deep as the number of digits in the input since whenever we reach that depth, we backtrack.

As the hash map does not grow as the inputs grows, it occupies O(1) space.

---
概述
你應該做的第一件事之一就是先看限制條件。很多時候，你只要看輸入規模，就能推斷出應該採取哪一類解法。在面試中，主動詢問面試官限制條件，也能展現你對細節的重視，同時獲得有用資訊。

在這一題中，輸入長度非常小，`0 <= digits.length <= 4`。由於輸入規模這麼小，我們可以合理地假設，直接暴力生成所有字母組合的解法會被接受。

每當你遇到需要產生某組字母或數字所有組合／排列的問題時，第一個應該想到的方法就是回溯法（backtracking）。如果你對回溯法還不熟，可以參考相關的 backtracking explore card。回溯演算法通常可以讓額外空間複雜度維持在線性等級。


方法一：回溯法
直覺

這題不需要什麼特別巧妙的技巧，難點只是在於如何正確地產生所有可能的組合，並套用標準的回溯演算法模板來完成。讓我們先從最簡單的情況拆解這個問題，例如輸入只有 1 個數字，`digits = "2"`。這個例子很直接，只要產生數字 `"2"` 所對應的所有字母，也就是 `["a", "b", "c"]`。

那如果輸入有 2 個數字，`digits = "23"` 呢？可以想像把數字 `"2"` 的每個字母都當成起點。也就是先固定第一個字母，再去求所有以這個字母開頭的可能組合。假設第一個字母固定為 `"a"`，那問題又變得很簡單了，因為它就退化成 1 位數的情況，我們只需要產生數字 `"3"` 對應的所有字母，再把它們接到 `"a"` 後面，就得到 `["ad", "ae","af"]`。這之所以簡單，是因為我們暫時忽略第一個字母，並假設它固定為 `"a"`。但其實我們也知道如何產生所有可能的第一個字母，那就是我們剛剛已經解決的 1 位數情況，結果是 `["a", "b", "c"]`。

如你所見，解 1 位數的情況很簡單，而解 2 位數的情況，本質上就是把 1 位數的情況解兩次。同樣的推理可以延伸到 `n` 位數。對於 3 位數的情況，先解 2 位數的情況來生成前 2 個字母的所有組合，再解最後 1 位數的情況。既然我們知道怎麼解 3 位數，那要解 4 位數時，就先解前 3 位數的所有組合，再處理最後 1 位數。這個想法其實可以無限延伸，不過別擔心，在這一題裡最多只會到 4 位數。

目前

演算法

如前所述，當我們產生新字母時，需要把已經選定的字母固定下來。保存這種狀態最簡單的方法就是使用遞迴。演算法流程如下：

如果輸入為空，回傳空陣列。

初始化一個資料結構（例如雜湊表），用來將數字對應到它們的字母，例如把 `"6"` 對應到 `"m"`、`"n"` 和 `"o"`。

使用一個回溯函式來產生所有可能的組合。

這個函式應該接收兩個主要輸入：目前已建立的字母組合 `path`，以及目前正在處理的索引值。
作為基本情況，如果目前字母組合的長度已經和輸入 `digits` 的長度相同，代表我們已經得到一個完整組合。因此把它加入答案中，然後回溯。
否則，取得目前正在查看的數字 `digits[index]` 所對應的所有字母。
遍歷這些字母。對於每個字母，先把它加入目前的 `path`，然後再次呼叫 `backtrack`，並把 `index` 加 1 以移到下一個數字。
處理完之後，記得把這個字母從 `path` 中移除。
實作


複雜度分析

時間複雜度：`O(4^N * N)`，其中 `N` 是 `digits` 的長度。注意，這裡的 4 指的是雜湊表中單一數字可能對應的最大字母數量，而不是輸入長度。

最糟情況是輸入只包含 7 和 9。這種情況下，每多一個數字，就必須額外探索 4 條分支。接著，對於每一個組合，建立該組合本身最多還需要 `N` 的成本。這題也可以推廣到每個數字最多對應 `M` 個字母的情況，此時時間複雜度會是 `O(M^N * N)`。在這題的限制中，`M = 4`，因為數字 7 和 9 各自對應 4 個字母。

空間複雜度：`O(N)`，其中 `N` 是 `digits` 的長度。

如果不計輸出結果所佔的空間，相對於輸入規模所使用的額外空間，主要就是遞迴呼叫堆疊所佔的空間。它的深度最多只會和輸入中的數字數量一樣，因為每當遞迴深度到達該層時，我們就會開始回溯。

由於雜湊表不會隨著輸入增長而擴大，因此它只佔用 `O(1)` 空間。
