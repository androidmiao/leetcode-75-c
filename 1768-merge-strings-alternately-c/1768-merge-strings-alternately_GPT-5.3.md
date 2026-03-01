@what is the answer using C?


Use this C solution for LeetCode 1768 (`Merge Strings Alternately`):

```c
#include <stdlib.h>
#include <string.h>

char* mergeAlternately(char* word1, char* word2) {
    int n1 = (int)strlen(word1);
    int n2 = (int)strlen(word2);

    char *res = (char *)malloc(n1 + n2 + 1);
    int i = 0, j = 0, k = 0;

    while (i < n1 || j < n2) {
        if (i < n1) res[k++] = word1[i++];
        if (j < n2) res[k++] = word2[j++];
    }

    res[k] = '\0';
    return res;
}
```

Time: `O(n1 + n2)`  
Space: `O(n1 + n2)` (for output string)


---
結論：兩個都正確、複雜度也一樣，**我那版稍微更精簡**，`Claude 4.5 Haiku` 那版**可讀性稍微更直觀**。

- `Claude 4.5 Haiku`（[Claude 4.5 Haiku.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/Claude%204.5%20Haiku.md:24)）  
  先交錯合併、再補剩餘字元，流程分段清楚。
- 我的版本（[GPT-5.3.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/GPT-5.3.md:14)）  
  一個 `while` + 兩個 `if` 就完成，程式更短。

如果你重視「面試時快速寫對」：用我的。  
如果你重視「教學/初學者好懂」：用 Claude 那版。  

補充：兩版都可以再加 `malloc` 失敗檢查，會更完整。


---
大方向一樣，但不是「完全同一寫法」。

- `Solution.md` 的方法一（雙指標）和我寫的解法邏輯一致：都是交替取字元、剩餘字元補尾端，時間 `O(m+n)`（[Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/Solution.md:22)）。
- 我那版是每輪 `while` 裡直接兩個 `if`（先放 `word1` 再放 `word2`），程式更精簡。
- 你目前 `solution.c` 用 `flop + switch` 控制輪替，並用 `continue` 處理某字串已耗盡的情況（[solution.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/solution.c:16)）；結果正確，但寫法比我那版繞一點。

另外 `Solution.md` 還有方法二（單指標），那個就和我那版不同路徑（[Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1768-merge-strings-alternately-c/Solution.md:48)）。