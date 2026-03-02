//Sliding Window (滑動視窗): https://leetcode.com/problems/maximum-average-subarray-i/description/?envType=study-plan-v2&envId=leetcode-75
//給定一個整數陣列 `nums` 和一個整數 `k`，請找出平均值最大且長度為 `k` 的子陣列，並回傳該最大平均值。
//這支程式在做的是：找出「長度固定為 k 的連續子陣列」中，平均值最大的那一段，並回傳最大平均值

/*
用 `nums = [1,12,-5,-6,50,3], k = 4` 畫這題的「記憶體位址 / 陣列索引版」如下。

```text
假設 int = 4 bytes
nums 起始位址 = 0x1000

索引 idx:      0        1        2        3        4        5
位址 &nums[idx]:
             0x1000   0x1004   0x1008   0x100C   0x1010   0x1014
值 nums[idx]:    1       12       -5       -6       50        3
```

初始視窗（idx 0~3）：
```text
windowSum = nums[0]+nums[1]+nums[2]+nums[3]
          = 1 + 12 + (-5) + (-6) = 2
maxSum = 2
```

滑動一次（i=4）：
```text
新進來: nums[4]  (位址 0x1010, 值 50)
移出去: nums[0]  (位址 0x1000, 值 1)

windowSum = 2 + 50 - 1 = 51
maxSum = 51
目前視窗 idx 1~4 => [12, -5, -6, 50]
```

再滑一次（i=5）：
```text
新進來: nums[5]  (位址 0x1014, 值 3)
移出去: nums[1]  (位址 0x1004, 值 12)

windowSum = 51 + 3 - 12 = 42
maxSum 仍是 51
目前視窗 idx 2~5 => [-5, -6, 50, 3]
```

最後回傳：
max average = (double)maxSum / k = 51 / 4 = 12.75
*/

/*
This is the standard sliding-window solution: `O(n)` time, `O(1)` extra space.
- 時間：O(n)，每個元素最多被加入/移出一次

- 空間：O(1)，只用固定數量變數
`O(1)` 空間的意思是：  
不管輸入陣列多大（`n` 變多大），額外使用的記憶體都不會跟著成長。
這段程式只用了固定幾個變數：
- `windowSum`
- `maxSum`
- 迴圈索引 `i`
沒有建立像「長度為 `n` 的新陣列」這種結構，所以額外空間是常數級，記作 `O(1)`。（輸入的 `nums` 本身不算在額外空間內。）
*/

/*
核心方法：Sliding Window（滑動視窗）
先算第一個長度 k 視窗的總和
再把視窗每次往右移 1 格：
加入新的右邊元素, 移除舊的左邊元素
這樣每次都能 O(1) 更新總和，不用重算整段
平均值 = 總和 / 固定常數 k，所以最大平均值必定來自最大總和
*/
double findMaxAverage(int* nums, int numsSize, int k) { //輸入整數陣列 `nums`、陣列大小 `numsSize` = n、子陣列長度 `k`，回傳最大平均值（double）
    long long windowSum = 0; //用來存「目前視窗」的總和。用 long long 是為了降低整數加總溢位風險

    for (int i = 0; i < k; i++) {
        windowSum += nums[i]; //先把前 k 個元素加起來，得到第一個視窗總和
    }

    long long maxSum = windowSum; //先把第一個視窗當成目前最大總和

    for (int i = k; i < numsSize; i++) {
        windowSum += nums[i] - nums[i - k]; //加上新進來的 nums[i], 減去被移出視窗的 nums[i-k]
        if (windowSum > maxSum) { //若新視窗總和更大，就更新 maxSum
            maxSum = windowSum;
        }
    }

    return (double)maxSum / k; //把最大總和除以 k 得到最大平均值，強制轉成 double 避免整數除法。
}

/*
`(double)maxSum / k` 的重點是把運算改成浮點除法：
- 若寫成 `maxSum / k`（兩者都是整數型別），會先做整數除法，小數被截掉。
- 加上 `(double)` 後，會變成浮點除法，保留小數，符合題目要的平均值精度。
(是，意思是「結果要有小數精度」。題目接受誤差小於 `1e-5` = 10^-5 = 0.00001 的答案，所以不能用整數除法（會把小數截掉），要用 `double` 做除法，才能得到像 `12.75` 這種精確平均值。)
(你的答案和正確答案的誤差只要小於 0.00001 就會被接受。)

例子：
- `51 / 4`（整數除法）結果是 `12`
- `(double)51 / 4` 結果是 `12.75`
*/