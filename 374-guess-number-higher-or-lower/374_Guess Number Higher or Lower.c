//Binary Search: https://leetcode.com/problems/guess-number-higher-or-lower/description/?envType=study-plan-v2&envId=leetcode-75

// 解法：Binary Search（二分搜尋）
// 時間複雜度：O(log n)
// 空間複雜度：O(1)

 /*
 這題標準解是二分搜尋：
 重點是用 mid = left + (right - left) / 2，可以避免 left + right 溢位。時間複雜度 O(log n)，空間複雜度 O(1)。

 用 left 和 right 表示目前可能答案的範圍
 每次取中間值 mid
 用 guess(mid) 判斷是猜中、猜太大、還是猜太小
 再把搜尋範圍縮到左半邊或右半邊
 */

/**
 * LeetCode 會預先提供這個 API，不需要自己實作。
 * 你只要呼叫它來判斷目前猜的數字和答案的關係。
 *
 * @param num 你目前猜的數字
 * @return
 *   -1：代表你猜太大，num > pick
 *    1：代表你猜太小，num < pick
 *    0：代表猜中，num == pick
 *
 * int guess(int num);
 */

// 題目要你從 1 到 n 中找出正確答案。
// 因為每次都能知道答案在「左半邊」還是「右半邊」，
// 所以最適合用二分搜尋，把搜尋範圍不斷縮小。
int guessNumber(int n) {
    // left 表示目前搜尋區間的左邊界，初始為 1。
    int left = 1;

    // right 表示目前搜尋區間的右邊界，初始為 n。
    int right = n;

    // 只要搜尋區間還有效（left 沒有超過 right），就持續搜尋。
    while (left <= right) {
        // 取中間值 mid。
        // 這裡不用 (left + right) / 2，
        // 而是用 left + (right - left) / 2 來避免整數溢位。
        int mid = left + (right - left) / 2;

        // 呼叫題目提供的 guess() API，
        // 判斷 mid 和真正答案 pick 的大小關係。
        int result = guess(mid);

        // 如果回傳 0，表示 mid 就是答案，直接回傳。
        if (result == 0) {
            return mid;
        }

        // 如果 result < 0，代表 mid 太大了。
        // 真正答案一定在左半邊，所以把右邊界縮成 mid - 1。
        if (result < 0) {
            right = mid - 1;
        } else {
            // 否則 result > 0，代表 mid 太小了。
            // 真正答案一定在右半邊，所以把左邊界改成 mid + 1。
            left = mid + 1;
        }
    }

    // 正常情況下一定會在迴圈中找到答案。
    // 這個 return -1 只是保底寫法，避免函式沒有回傳值。
    return -1;
}


/*
**流程圖**

```text
開始
  |
  v
left = 1, right = n
  |
  v
left <= right ?
  |
  +-- 否 --> 結束，回傳 -1（理論上不會發生）
  |
  +-- 是 -->
          mid = left + (right - left) / 2
          result = guess(mid)
              |
              v
         result == 0 ?
              |
         +----+----+
         |         |
        是         否
         |         |
         v         v
    回傳 mid   result < 0 ?
                   |
              +----+----+
              |         |
             是         否
              |         |
              v         v
     right = mid - 1   left = mid + 1
              |         |
              +---- 回到 while ----+
```

**範圍變化圖**

假設 `n = 10`，答案 `pick = 6`

```text
初始範圍：
[left........................right]
  1                            10

第 1 次：
left = 1, right = 10
mid = 5
guess(5) = 1  -> 代表猜太小，答案在右邊

新的範圍：
            [left........right]
              6            10


第 2 次：
left = 6, right = 10
mid = 8
guess(8) = -1 -> 代表猜太大，答案在左邊

新的範圍：
            [left..right]
              6     7


第 3 次：
left = 6, right = 7
mid = 6
guess(6) = 0 -> 猜中

回傳 6
```

**陣列範圍感覺圖**

雖然這題不是直接搜尋陣列，但你可以把 `1 ~ n` 想成一排已排序的數字：

```text
1  2  3  4  5  6  7  8  9  10
^                           ^
left                      right
```

第一次取中間：

```text
1  2  3  4  5  6  7  8  9  10
^           ^               ^
left       mid            right
            5
```

如果 `5` 太小，就丟掉左半邊：

```text
1  2  3  4  5  6  7  8  9  10
               ^           ^
              left       right
```

如果 `8` 太大，就丟掉右半邊：

```text
1  2  3  4  5  6  7  8  9  10
               ^     ^
              left right
```

**核心觀念**

每次不是只排除一個數字，而是排除一半範圍，所以時間複雜度是 `O(log n)`。

如果你要，我可以下一步再幫你畫成「記憶體位址 / 索引指標移動版」ASCII 圖。
*/

/*
可以，把這題想成「雖然沒有真的陣列，但我們用陣列索引的方式理解 `left / mid / right`」。

**索引指標移動版**

假設：

- `n = 10`
- `pick = 6`

把 `1 ~ 10` 想成一個已排序陣列：

```text
索引:   0  1  2  3  4  5  6  7  8  9
數值:   1  2  3  4  5  6  7  8  9  10
```

但這題程式實際用的是「數值範圍」：
- `left = 1`
- `right = 10`

你可以把它類比成：
- `left` 指到最左邊可能答案
- `right` 指到最右邊可能答案
- `mid` 指到中間位置

---

**第 1 輪**

```text
數值:   1  2  3  4  5  6  7  8  9  10
        ^           ^                 ^
      left         mid              right
        1           5                10
```

程式：

```c
mid = left + (right - left) / 2;
mid = 1 + (10 - 1) / 2 = 5;
```

呼叫：

```c
guess(5)
```

結果：
- 回傳 `1`
- 代表 `5` 太小
- 答案在右半邊

所以左指標右移：

```c
left = mid + 1;   // left = 6
```

---

**第 2 輪**

```text
數值:   1  2  3  4  5  6  7  8  9  10
                          ^     ^     ^
                        left   mid  right
                          6     8    10
```

程式：

```c
mid = 6 + (10 - 6) / 2 = 8;
```

呼叫：

```c
guess(8)
```

結果：
- 回傳 `-1`
- 代表 `8` 太大
- 答案在左半邊

所以右指標左移：

```c
right = mid - 1;   // right = 7
```

---

**第 3 輪**

```text
數值:   1  2  3  4  5  6  7  8  9  10
                          ^  ^  ^
                        left mid right
                          6   6   7
```

程式：

```c
mid = 6 + (7 - 6) / 2 = 6;
```

呼叫：

```c
guess(6)
```

結果：
- 回傳 `0`
- 猜中

```c
return 6;
```

---

**指標移動總結圖**

```text
初始:
[left........................right]
  1                            10

mid = 5, 太小
            left 右移

      [left................right]
        6                    10

mid = 8, 太大
            right 左移

      [left....right]
        6        7

mid = 6, 猜中
```

**如果用「記憶體位址感」來理解**

這題其實沒有真的操作陣列記憶體，也沒有 pointer 算術；
`left`、`mid`、`right` 只是三個整數變數，用來表示「目前搜尋區間的邊界」。

所以更精準地說，這題是：

- 不是在移動真正的記憶體位址
- 而是在移動「邏輯上的搜尋範圍指標」

你可以把它想成：

```text
left  -> 目前可能答案的最左界
mid   -> 目前拿來試猜的中間值
right -> 目前可能答案的最右界
```

如果你要，我可以下一步直接幫你畫成更像你之前那種的：
`left / mid / right` 橫向對齊 ASCII 圖，配合 [374_Guess Number Higher or Lower.c](/Users/alex.miao/Documents/GitHub/leetcode-75-c/374-guess-number-higher-or-lower/374_Guess%20Number%20Higher%20or%20Lower.c) 每一行一起對照。
*/