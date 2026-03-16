@17_Letter Combinations of a Phone Number_Description.md 請使用Ｃ語言來實作並把結果存到 17_Letter Combinations of a Phone Number.c 請詳細說明內容, 逐行中文註解

已實作完成，程式已寫入 17_Letter Combinations of a Phone Number.c，採用回溯法，並加上逐行中文註解，符合 LeetCode 的 C 函式格式。

這份解法的核心是：
先用 PHONE_MAP 建立數字到字母的對應，再用 backtrack(...) 逐位遞迴選字母；當選滿 digits 長度時，就把目前組合複製到結果陣列。因為題目長度最多只有 4，所以這種做法直接、穩定，也最符合題意。

我另外做了語法檢查，clang -fsyntax-only 已通過。如果你要，我下一步可以再幫你把這份 .c 對應整理成一份 GPT-5.4.md，用「完整英文說明 + 完整中文詳解 + 程式逐段解析」的格式。


@17_Letter Combinations of a Phone Number.c 請畫圖說明並記錄到這個檔案中


@[17_Letter Combinations of a Phone Number_Solution.md](17_Letter Combinations of a Phone Number/17_Letter Combinations of a Phone Number_Solution.md) 跟你寫的解法有一樣嗎？

有，核心解法是一樣的，都是「回溯法 + 遞迴逐位展開」。

`Solution.md` 在 [17_Letter Combinations of a Phone Number_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number_Solution.md#L22) 到 [17_Letter Combinations of a Phone Number_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number_Solution.md#L34) 說的流程是：
先建立數字對字母的對應、用 `path` 記錄目前組合、當長度等於 `digits` 長度就加入答案，否則繼續處理下一位。這和我在 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L74) 到 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L105) 的 `backtrack(...)` 完全對應。

差別在實作細節，不在思路：
- `Solution.md` 是語言無關的演算法描述，我的是 C 語言具體實作。
- 它說用 hash map，我在 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L52) 用靜態陣列 `PHONE_MAP` 取代，效果相同。
- 它提到「回溯後要把字母移除」，我在 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L100) 到 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L104) 是靠下一次覆寫 `path[index]` 來達成，不需要真的 `pop`。
- 我另外在 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L155) 到 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L166) 先計算總組合數並配置記憶體，這是 C 實作必須處理的部分，`Solution.md` 沒展開寫。

補一句，`Solution.md` 把空輸入當成可能情況處理，我的程式在 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L144) 到 [17_Letter Combinations of a Phone Number.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c#L148) 也有處理，所以這點也一致。

如果你要，我可以下一步把 `Solution.md` 裡每一條演算法步驟，逐條對應到 `.c` 裡的實際行數給你。