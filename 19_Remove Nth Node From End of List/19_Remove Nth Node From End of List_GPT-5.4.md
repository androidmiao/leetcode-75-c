這題我放在 [19_Remove Nth Node From End of List.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/19_Remove%20Nth%20Node%20From%20End%20of%20List/19_Remove%20Nth%20Node%20From%20End%20of%20List.c) 的做法，是 `dummy + two pointers` 的一次走訪版本。

它和 [19_Remove Nth Node From End of List_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/19_Remove%20Nth%20Node%20From%20End%20of%20List/19_Remove%20Nth%20Node%20From%20End%20of%20List_Solution.md) 裡的 `Approach 2: One Pass Algorithm` 是同一個核心思路。

如果你問的是「跟整份 `Solution.md` 一不一樣」，
答案是不完全一樣。

原因是：

- `Solution.md` 收錄了兩種方法
- `Approach 1` 是先算長度，再做刪除
- `Approach 2` 是 `dummy + fast/slow` 一次走訪
- 我的 `.c` 只實作了其中的 `Approach 2`

所以更精確地說：

- 跟整份 `Solution.md` 不算完全一樣
- 跟 `Solution.md` 裡的 `Approach 2` 是一樣的

如果要問哪個比較好，也要拆開來看：

- 如果是拿我的 `.c` 跟 `Solution.md` 比，`Solution.md` 比較完整，因為它同時整理了兩種做法和複雜度分析
- 如果是拿我的 `.c` 跟 `Solution.md` 的 `Approach 1` 比，我的 `.c` 這種 `Approach 2` 通常更好

原因是：

- 同樣都是 `O(n)` 時間、`O(1)` 空間
- 但 `Approach 2` 不用先算整條串列長度
- 也更貼近題目 follow-up 想要的一次走訪
- 對這個 repo 來說，拿來當 submit-ready 解法也比較直接

所以結論可以整理成：

- 想看完整解題整理：`Solution.md` 比較好
- 想看真正提交用的版本：我的 `.c` 比較直接
- 如果只比較演算法選擇：`Approach 2` 通常比 `Approach 1` 更值得優先記

不過它和 [19_Remove Nth Node From End of List_Description.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/19_Remove%20Nth%20Node%20From%20End%20of%20List/19_Remove%20Nth%20Node%20From%20End%20of%20List_Description.md) 不一樣。

原因很簡單：

- `Description.md` 是題目敘述，內容是題目要求、範例、限制和提示
- `.c` 是具體解法，內容是如何真的把程式寫出來

所以兩者不是「誰取代誰」的關係，而是：

- `Description.md` 告訴你要做什麼
- `.c` 告訴你怎麼做

如果一定要問哪一個比較好，要看你的目的。

如果你的目的是：

- 確認題目要求和邊界條件：`Description.md` 比較重要，因為它是題目的原始需求
- 學會怎麼解題、怎麼在 LeetCode 上提交：`.c` 比較有用，因為它已經把提示落成完整演算法

若從「解題品質」來看，我這份 `.c` 會比 `Description.md` 更完整，因為：

- 題目描述本身沒有真正展開演算法
- 它只給了 hint：`Maintain two pointers and keep one of them delayed by n nodes`
- 我的 `.c` 把這個 hint 具體化成 `dummy + fast/slow + n + 1 gap`

重點不是直接找到「倒數第 `n` 個節點」本身，而是找到它的前一個節點，這樣才能做：

`slow->next = slow->next->next;`

我這份實作為什麼要加 `dummy`？

- 如果剛好要刪掉的是原本的 `head`，沒有 `dummy` 就要額外分支處理
- 有了 `dummy -> head` 之後，刪頭節點和刪中間節點可以用同一套邏輯
- 最後只要回傳 `dummy.next`

整個流程可以想成：

1. `fast` 和 `slow` 都先站在 `dummy`
2. 先讓 `fast` 多走 `n + 1` 步
3. 接著 `fast`、`slow` 一起往前走
4. 當 `fast == NULL` 時，`slow` 剛好停在目標節點前一格
5. 把 `slow->next` 跳過去，完成刪除

以 `head = [1,2,3,4,5], n = 2` 為例：

- 目標是刪掉值為 `4` 的節點
- 當雙指標同步前進到結束時，`slow` 會停在值為 `3` 的節點
- 所以做 `slow->next = slow->next->next`
- 結果就變成 `[1,2,3,5]`

這題最容易卡住的地方有兩個：

- `fast` 不是先走 `n` 步，而是先走 `n + 1` 步
- 因為我們要讓 `slow` 停在「待刪節點的前一個節點」

如果只先走 `n` 步，最後 `slow` 停的位置會不對。

另外，這題雖然 `Solution.md` 也有 `Approach 1: Two Pass Algorithm`，也就是先算長度再刪除，但在 C 裡面，`dummy + 雙指標` 版本通常更乾淨，因為：

- 不用先算整條串列長度
- 邏輯比較集中
- 也更符合題目 follow-up 想要的一次走訪

所以結論很簡單：

- 這份 `.c` 用的是官方解法的 `Approach 2`
- 它和整份 `Solution.md` 不完全一樣，因為 `Solution.md` 還包含 `Approach 1`
- 但它和 `Solution.md` 的 `Approach 2` 是同一個演算法
- 如果想看完整整理，`Solution.md` 比較好
- 如果想看可提交、可直接記住的版本，這份 `.c` 比較好
- 如果只比較演算法選擇，`Approach 2` 通常比 `Approach 1` 更值得優先學
- 它和 `Description.md` 不一樣，因為 `Description.md` 是題目不是解法
- 如果是為了解題，`.c` 比 `Description.md` 更有用
- 如果是為了確認題目需求，`Description.md` 才是基準
- 關鍵技巧是 `dummy`
- 關鍵細節是 `fast` 要先走 `n + 1` 步
- 時間複雜度 `O(n)`，空間複雜度 `O(1)`
