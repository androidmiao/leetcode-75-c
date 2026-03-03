//Prefix Sum: https://leetcode.com/problems/find-the-highest-altitude/description/?envType=study-plan-v2&envId=leetcode-75
//這份程式是在解 LeetCode 1732「找最高海拔」，核心想法是「一路累加海拔變化，並同步記錄最大值」
/*
Hint1: Let's note that the altitude of an element is the sum of gains of all the elements behind it
Hint2: Getting the altitudes can be done by getting the prefix sum array of the given array
--> 這題的「海拔變化」陣列 `gain` 就像是「前綴和」的增量，從起點 0 開始，每走一步就加上 `gain[i]`，同時更新最高海拔。
*/

/*
Time complexity: `O(n)`  
Space complexity: `O(1)`
*/

/*
1. 函式定義  
`int largestAltitude(int* gain, int gainSize)`  
`gain` 是海拔差陣列，`gainSize` 是長度。  
題意中起點海拔固定是 `0`，`gain[i]` 代表從第 `i` 點到第 `i+1` 點的淨變化。

2. 變數初始化  
[1732-find-the-highest-altitude.c:8](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:8)  
`current = 0`：目前走到的海拔。  
[1732-find-the-highest-altitude.c:9](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:9)  
`highest = 0`：目前遇過的最高海拔。  
一開始都設成 `0`，因為起點海拔就是 0，而且最高點至少不會低於起點（若一路下降，答案仍是 0）。

3. 主迴圈：逐步更新海拔  
[1732-find-the-highest-altitude.c:11](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:11)  
用 `for` 從 `i = 0` 走到 `gainSize - 1`，每一步處理一個海拔變化。  
[1732-find-the-highest-altitude.c:12](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:12)  
`current += gain[i];`：把這一步的升降加到目前海拔。  
[1732-find-the-highest-altitude.c:13](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:13)  
如果 `current > highest`，就更新 `highest`。  
這等同於在跑「前綴和」時，同時取最大前綴和（再加上起點 0 的考量）。

4. 回傳結果  
[1732-find-the-highest-altitude.c:18](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:18)  
`return highest;` 回傳旅程中的最高海拔。

5. 複雜度  
[1732-find-the-highest-altitude.c:3](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:3)  
時間複雜度 `O(n)`：只掃過陣列一次。  
[1732-find-the-highest-altitude.c:4](/Users/alex.miao/Documents/GitHub/leetcode-75-c/1732-find-the-highest-altitude/1732-find-the-highest-altitude.c:4)  
空間複雜度 `O(1)`：只用了 `current`、`highest`、`i` 這幾個固定變數。

6. 小範例走一次  
若 `gain = [-5, 1, 5, 0, -7]`：  
起點：`current=0, highest=0`  
第1步：`current=-5, highest=0`  
第2步：`current=-4, highest=0`  
第3步：`current=1, highest=1`  
第4步：`current=1, highest=1`  
第5步：`current=-6, highest=1`  
最後回傳 `1`。 
*/

/*
用你的這支 `largestAltitude`，示範 `gain = [-5, 1, 5, 0, -7]` 的「陣列索引 + 記憶體位址」圖。

```text
假設:
- int = 4 bytes
- gain 起始位址 = 0x2000
- &gain[i] = 0x2000 + 4*i
- current, highest 在 stack 上（位址用示意）
  current @ 0x7FFE_EA10
  highest @ 0x7FFE_EA14
```

```text
初始 (進入函式後)
gain:
idx:        0       1       2       3       4
addr:    2000    2004    2008    200C    2010
value:    -5       1       5       0      -7

stack:
[current @ EA10] = 0
[highest @ EA14] = 0
```

```text
i = 0
讀取 gain[0] (0x2000) = -5
current = 0 + (-5) = -5
highest = max(0, -5) = 0
```

```text
i = 1
讀取 gain[1] (0x2004) = 1
current = -5 + 1 = -4
highest = max(0, -4) = 0
```

```text
i = 2
讀取 gain[2] (0x2008) = 5
current = -4 + 5 = 1
highest = max(0, 1) = 1
```

```text
i = 3
讀取 gain[3] (0x200C) = 0
current = 1 + 0 = 1
highest = max(1, 1) = 1
```

```text
i = 4
讀取 gain[4] (0x2010) = -7
current = 1 + (-7) = -6
highest = max(1, -6) = 1
```

```text
迴圈結束 -> return highest = 1
```

@對應到程式就是：每次用 `i` 取 `gain[i]`（連續位址），累加到 `current`，再用 `highest` 記錄目前最大海拔。
*/

int largestAltitude(int* gain, int gainSize) {
    int current = 0;
    int highest = 0;  // starting altitude is 0

    for (int i = 0; i < gainSize; i++) {
        current += gain[i];
        if (current > highest) {
            highest = current;
        }
    }

    return highest;
}