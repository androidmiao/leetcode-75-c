//Two Pointers: https://leetcode.com/problems/move-zeroes/description/?envType=study-plan-v2&envId=leetcode-75
//給定一個整數陣列 `nums`，請將所有 `0` 移到陣列末尾，同時保持所有非零元素的相對順序不變; 請注意，你必須在**原地（in-place）**完成此操作，不能另外複製一份陣列。

/*
範例走一次 nums = [0,1,0,3,12]：
掃描後前段會變成 [1,3,12,?,?]，此時 insertPos = 3
從 index 3 開始補 0
最後 [1,3,12,0,0]
*/

/*
可以，用 `nums = [0,1,0,3,12]` 畫「陣列索引 + 記憶體位址（示意）」如下。

```text
假設 nums 起始位址是 0x1000，int = 4 bytes

索引 index:     0        1        2        3        4
位址 address: 0x1000   0x1004   0x1008   0x100C   0x1010
值 value:        0        1        0        3       12
```

`insertPos = 0`，`i` 從左到右掃：

### Step 1: i=0, nums[0]=0
```text
insertPos=0 不動
[0, 1, 0, 3, 12]
```

### Step 2: i=1, nums[1]=1 (非0，搬到 nums[insertPos=0])
```text
寫入: nums[0] = nums[1] = 1
insertPos -> 1

索引:  0  1  2  3  4
值:   [1, 1, 0, 3,12]
```

### Step 3: i=2, nums[2]=0
```text
insertPos=1 不動
[1, 1, 0, 3, 12]
```

### Step 4: i=3, nums[3]=3 (非0，搬到 nums[1])
```text
寫入: nums[1] = nums[3] = 3
insertPos -> 2

[1, 3, 0, 3,12]
```

### Step 5: i=4, nums[4]=12 (非0，搬到 nums[2])
```text
寫入: nums[2] = nums[4] = 12
insertPos -> 3

[1, 3,12, 3,12]
```

第一輪結束後，前 `insertPos=3` 格已是正確非0序列：`[1,3,12,...]`

---

第二輪：從 `insertPos=3` 開始補 0

### Fill 0 at index 3
```text
nums[3] = 0 -> [1,3,12,0,12]
insertPos -> 4
```

### Fill 0 at index 4
```text
nums[4] = 0 -> [1,3,12,0,0]
insertPos -> 5 結束
```

最終：
```text
索引:  0  1  2  3  4
位址:1000 1004 1008 100C 1010
值:  [1, 3,12, 0, 0]
```
*/

/*
畫成「雙指標箭頭版（i / insertPos 同步移動圖）」:
題目：nums = [0,1,0,3,12]
規則：
- i：掃描指標（每步都往右）
- insertPos：下一個「非0要放的位置」（只有遇到非0才往右）

初始
index:      0   1   2   3   4
nums:      [0,  1,  0,  3, 12]
            ^               
            i
            ^
         insertPos=0
```

```text
Step 1: i=0, nums[i]=0  -> 跳過
index:      0   1   2   3   4
nums:      [0,  1,  0,  3, 12]
                ^           
                i
            ^
         insertPos=0
```

```text
Step 2: i=1, nums[i]=1 (非0)
動作: nums[insertPos] = nums[i] => nums[0]=1, insertPos++

index:      0   1   2   3   4
nums:      [1,  1,  0,  3, 12]
                    ^       
                    i
                ^
             insertPos=1
```

```text
Step 3: i=2, nums[i]=0 -> 跳過
index:      0   1   2   3   4
nums:      [1,  1,  0,  3, 12]
                        ^   
                        i
                ^
             insertPos=1
```

```text
Step 4: i=3, nums[i]=3 (非0)
動作: nums[1]=3, insertPos++

index:      0   1   2   3   4
nums:      [1,  3,  0,  3, 12]
                            ^
                            i
                    ^
                 insertPos=2
```

```text
Step 5: i=4, nums[i]=12 (非0)
動作: nums[2]=12, insertPos++

index:      0   1   2   3   4
nums:      [1,  3, 12,  3, 12]
(i 掃描結束)
                        ^
                     insertPos=3
```

```text
第一輪結束：前 0..2 已是正確非0序列 [1,3,12]
第二輪：從 insertPos=3 開始補0
nums[3]=0, nums[4]=0
```

```text
最終
index:      0   1   2   3   4
nums:      [1,  3, 12,  0,  0]
```

一句話記憶：`i` 負責「找非0」，`insertPos` 負責「放非0」。
*/

/*
時間複雜度 `O(n)`: 最多兩次線性掃描:
第一次：for 迴圈掃完整個陣列，把非 0 往前放。
第二次：while 迴圈只掃後半段補 0，最壞情況（全都非 0）會跑 0 次；最好情況（全都 0）會跑 n 次。
所以總操作量是 n + (n - 非0個數)，上界約 2n，時間複雜度仍是 O(n)。

額外空間 `O(1)`
*/

//補充：這版不是「最少寫入次數」版本（因為補 0 會寫很多次），但簡潔、穩定、LeetCode 可直接 AC。


/*
這支程式是標準的「雙指標（Two Pointers）」解法，核心目標是：
- 把所有非 0 元素依序搬到前面
- 後面剩下的位置全部補 0
@一句話記憶：`i` 負責「找非0」，`insertPos` 負責「放非0」。
*/
void moveZeroes(int* nums, int numsSize) {
    int insertPos = 0; //insertPos 代表「下一個非 0 元素要放的位置」

    // 先把所有非 0 元素往前放 
    for (int i = 0; i < numsSize; i++) { //for 迴圈（i 從 0 到 numsSize-1）, 逐一檢查每個元素
        if (nums[i] != 0) {
            nums[insertPos++] = nums[i]; //如果 nums[i] != 0，就把它放到 nums[insertPos], 然後 insertPos++, 這樣會保留原本非 0 元素的相對順序（穩定）
        }
    }

    // 剩下位置補 0
    while (insertPos < numsSize) { //當第一段結束後，insertPos 之後的區間都應該是 0，直接一路補到陣列尾端
        nums[insertPos++] = 0;
    }
}
