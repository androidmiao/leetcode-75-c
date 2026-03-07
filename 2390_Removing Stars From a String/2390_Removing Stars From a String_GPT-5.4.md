不完全一樣，但核心思路有重疊。

[2390_Removing Stars From a String_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2390_Removing Stars From a String/2390_Removing Stars From a String_Solution.md) 裡面寫了三種方法：
1. `Stack`
2. `Strings`
3. `Two Pointers`

而 [2390_Removing Stars From a String.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2390_Removing Stars From a String/2390_Removing Stars From a String.c) 的寫法，本質上最接近第 3 種 `Two Pointers`，但又帶有第 1 種 `Stack` 的思維。

原因是你的 `.c` 程式：

```c
char *removeStars(char *s) {
    int w = 0;

    for (int r = 0; s[r] != '\0'; r++) {
        if (s[r] == '*') {
            if (w > 0) {
                w--;
            }
        } else {
            s[w++] = s[r];
        }
    }

    s[w] = '\0';
    return s;
}
```

這裡：
- `r` 是讀取指標
- `w` 是寫入指標
- 遇到普通字元就寫進 `s[w]`
- 遇到 `*` 就 `w--`

這和 `Solution.md` 的 `Two Pointers` 方法非常像，只是 `Solution.md` 是另外開一個 `ch` 陣列來存結果，所以額外空間是 `O(n)`；你的 `.c` 是直接在原字串 `s` 上原地覆寫，因此額外空間更好，是 `O(1)`。

所以最準確的說法是：

- 思想上：一樣，都是「保留有效字元、遇到 `*` 就回退一格」
- 實作上：不完全一樣
- 你的 `.c`：是 `Two Pointers + in-place` 的優化版本
- `Solution.md` 第 3 種：是 `Two Pointers + extra array`

如果你要，我可以直接幫你把 [2390_Removing Stars From a String_Solution.md](/Users/alex.miao/Documents/GitHub/leetcode-75-c/2390_Removing Stars From a String/2390_Removing Stars From a String_Solution.md) 補一句，明確標註「目前 `.c` 採用的是第 3 種方法的原地優化版」。