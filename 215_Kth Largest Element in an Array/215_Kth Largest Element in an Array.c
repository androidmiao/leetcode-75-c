//Heap/Priority Queue: https://leetcode.com/problems/kth-largest-element-in-an-array/description/?envType=study-plan-v2&envId=leetcode-75
/*
這份是 Quickselect，平均時間複雜度 O(n)、空間複雜度 O(1)，比直接排序更符合這題重點。核心函式是：
int findKthLargest(int* nums, int numsSize, int k)

swap 怎麼交換、partition 怎麼把大於 pivot 的值放左邊、findKthLargest 怎麼縮小搜尋區間直到找到第 k 大元素.
這份現在包含 C 程式碼、Quickselect 核心概念、partition 怎麼運作、target = k - 1 的原因、範例流程與複雜度
*/

/*
內容現在有：
partition 的逐步陣列變化圖
pivot 怎麼移動到正確位置
為什麼可以往左半邊或右半邊縮小範圍
整體 Quickselect 流程圖
*/

// 引入 rand() 所需的標頭檔。
#include <stdlib.h>

// 交換兩個整數的值。
static void swap(int* a, int* b) {
    // 暫存 a 原本的值。
    int temp = *a;
    // 把 b 的值放到 a。
    *a = *b;
    // 再把原本 a 的值放到 b，完成交換。
    *b = temp;
}

// 以 pivotIndex 指定的值作為基準，將陣列分區。
// 分區後，所有比 pivot 大的元素都會被移到左側，
// pivot 會被放到它排序後應在的位置，並回傳該位置索引。
static int partition(int* nums, int left, int right, int pivotIndex) {
    // 記錄 pivot 的值。
    int pivotValue = nums[pivotIndex];
    // storeIndex 指向下一個應該放入「比 pivot 大」元素的位置。
    int storeIndex = left;

    // 先把 pivot 移到目前區間最右邊，方便遍歷其他元素。
    swap(&nums[pivotIndex], &nums[right]);

    // 走訪 left 到 right - 1，檢查每個元素與 pivot 的大小關係。
    for (int i = left; i < right; i++) {
        // 如果目前元素比 pivot 大，就應該放到左側區域。
        if (nums[i] > pivotValue) {
            // 把目前元素交換到左側可放置的位置。
            swap(&nums[storeIndex], &nums[i]);
            // 左側區域往右擴張一格。
            storeIndex++;
        }
    }

    // 走訪完成後，把 pivot 放回 storeIndex，這就是它正確的位置。
    swap(&nums[storeIndex], &nums[right]);
    // 回傳 pivot 的最終索引。
    return storeIndex;
}

// 回傳陣列中第 k 大的元素。
int findKthLargest(int* nums, int numsSize, int k) {
    // 目前搜尋區間的左邊界。
    int left = 0;
    // 目前搜尋區間的右邊界。
    int right = numsSize - 1;
    // 第 k 大元素若用「由大到小排序後的索引」表示，目標索引是 k - 1。
    int target = k - 1;

    // 只要搜尋區間有效，就持續進行 Quickselect。
    while (left <= right) {
        // 在目前區間中隨機挑一個 pivot，避免固定輸入造成較差表現。
        int pivotIndex = left + rand() % (right - left + 1);
        // 分區後取得 pivot 的新位置。
        int newPivotIndex = partition(nums, left, right, pivotIndex);

        // 如果 pivot 剛好落在目標索引，代表找到第 k 大元素。
        if (newPivotIndex == target) {
            return nums[newPivotIndex];
        }

        // 如果 pivot 位置比目標還右邊，表示答案在左半部。
        if (newPivotIndex > target) {
            right = newPivotIndex - 1;
        } else {
            // 否則答案在右半部。
            left = newPivotIndex + 1;
        }
    }

    // 題目保證輸入有效，正常情況下不會走到這裡。
    return -1;
}



/*
## 詳細說明

這題要找的是「第 `k` 大」元素，不是排序後第 `k` 個不同的值。

例如：

- `nums = [3,2,3,1,2,4,5,5,6]`
- `k = 4`

由大到小排序後會是：

`[6,5,5,4,3,3,2,2,1]`

第 `4` 大是 `4`。

這份 C 程式使用的是 `Quickselect`。

它的想法是：

1. 隨機挑一個 `pivot`
2. 把陣列分成「比 pivot 大」和「比 pivot 小或等於」兩邊
3. 看 pivot 最後落在哪個索引
4. 判斷第 `k` 大元素是在左邊、右邊，還是就是 pivot 本身
5. 持續縮小搜尋範圍，直到找到答案

這樣就不需要真的把整個陣列完整排序。

## 為什麼 `target = k - 1`

因為這份寫法是把陣列想成「如果由大到小排序後」：

- 第 `1` 大在索引 `0`
- 第 `2` 大在索引 `1`
- 第 `3` 大在索引 `2`

所以第 `k` 大元素對應的目標索引就是：

`k - 1`

## 程式逐段解釋

### 1. `swap`

```c
static void swap(int* a, int* b)
```

這個函式只做一件事：交換兩個整數的值。

例如：

- `*a = 3`
- `*b = 8`

交換後：

- `*a = 8`
- `*b = 3`

在分區時會一直用到。

### 2. `partition`

```c
static int partition(int* nums, int left, int right, int pivotIndex)
```

這個函式的工作是：

- 選定一個 pivot
- 把所有「大於 pivot」的元素移到左邊
- 最後把 pivot 放到正確位置
- 回傳 pivot 的最終索引

這裡特別注意：

這題找的是「第 k 大」，所以我們把「大的元素」往左邊放。

如果是找第 k 小，通常會把小的元素往左邊放，但這題剛好反過來。

#### `partition` 的關鍵變數

```c
int pivotValue = nums[pivotIndex];
int storeIndex = left;
```

- `pivotValue`：記住 pivot 的值
- `storeIndex`：下一個應該放「比 pivot 大」元素的位置

接著：

```c
swap(&nums[pivotIndex], &nums[right]);
```

先把 pivot 暫時移到最右邊，這樣中間掃描陣列時比較方便，不會被自己干擾。

然後遍歷：

```c
for (int i = left; i < right; i++) {
    if (nums[i] > pivotValue) {
        swap(&nums[storeIndex], &nums[i]);
        storeIndex++;
    }
}
```

意思是：

- 如果 `nums[i] > pivotValue`
- 這個值應該被放到左側
- 就把它和 `storeIndex` 的位置交換
- 然後 `storeIndex++`

最後：

```c
swap(&nums[storeIndex], &nums[right]);
return storeIndex;
```

把原本放在最右邊的 pivot 換回 `storeIndex`。

這時：

- `storeIndex` 左邊都是比 pivot 大的值
- `storeIndex` 位置就是 pivot
- `storeIndex` 右邊都是小於等於 pivot 的值

所以 `storeIndex` 就是 pivot 在「由大到小排序」後應該落的位置。

### 3. `findKthLargest`

```c
int findKthLargest(int* nums, int numsSize, int k)
```

這是 LeetCode 會呼叫的主函式。

先初始化：

```c
int left = 0;
int right = numsSize - 1;
int target = k - 1;
```

- `left`：目前搜尋範圍左邊界
- `right`：目前搜尋範圍右邊界
- `target`：第 `k` 大元素在「由大到小排序後」應該在的索引

接著進入主迴圈：

```c
while (left <= right) {
```

只要搜尋區間還有效，就繼續找。

#### 隨機選 pivot

```c
int pivotIndex = left + rand() % (right - left + 1);
```

這行表示在 `[left, right]` 範圍內隨機選一個位置當 pivot。

這樣做的目的，是降低總是選到很差 pivot 的機率，讓平均時間維持在 `O(n)`。

#### 分區

```c
int newPivotIndex = partition(nums, left, right, pivotIndex);
```

呼叫 `partition` 後，pivot 會被放到正確位置，並回傳該位置。

#### 判斷是否找到答案

```c
if (newPivotIndex == target) {
    return nums[newPivotIndex];
}
```

如果 pivot 的位置剛好等於我們要找的 `target`，那它就是第 `k` 大元素。

#### 決定往左找還是往右找

```c
if (newPivotIndex > target) {
    right = newPivotIndex - 1;
} else {
    left = newPivotIndex + 1;
}
```

分成兩種情況：

- `newPivotIndex > target`
  代表 pivot 太靠右，答案在左半邊
- `newPivotIndex < target`
  代表 pivot 太靠左，答案在右半邊

所以每次都能把搜尋範圍縮小。

最後這行：

```c
return -1;
```

理論上不會走到，因為題目保證輸入合法，一定找得到答案。

## 範例走一次

輸入：

- `nums = [3,2,1,5,6,4]`
- `k = 2`

目標：

- 找第 `2` 大
- `target = 1`

假設某次 pivot 選到 `4`。

分區後可能變成：

`[5,6,4,3,1,2]`

此時 pivot `4` 的索引是 `2`。

因為：

- `2 > 1`

表示第 `2` 大元素在左邊 `[5,6]`。

再繼續在左半部做 Quickselect。

若下一次 pivot 最後落在索引 `1`，那就找到答案，回傳該值。

## 複雜度

- 平均時間複雜度：`O(n)`
- 最壞時間複雜度：`O(n^2)`
- 空間複雜度：`O(1)`

說明：

- 平均情況下，每次 partition 都能有效縮小搜尋範圍
- 最壞情況是每次都選到很差的 pivot
- 這份寫法是原地操作，除了少數變數外沒有額外開大陣列

## 這份解法的重點

- 不是完整排序
- 只找出「第 `k` 大所在的位置」
- 利用 partition 不斷縮小範圍
- 把「比 pivot 大的元素」放左邊，這點是因為題目要找第 `k` 大
*/

/*
## 畫圖說明

這份程式使用 `Quickselect`。

目標不是把整個陣列排好，而是：

- 每次選一個 `pivot`
- 把它放到正確位置
- 判斷答案在左邊還是右邊
- 持續縮小範圍

---

## 1. 題目範例

```text
nums = [3, 2, 1, 5, 6, 4]
k = 2
```

第 `2` 大元素在「由大到小排序後」的索引是：

```text
target = k - 1 = 1
```

也就是要找：

```text
排序後索引 1 的值
```

---

## 2. 初始狀態

```text
index:   0  1  2  3  4  5
nums :  [3, 2, 1, 5, 6, 4]

left   = 0
right  = 5
target = 1
```

整段 `[0..5]` 都是目前搜尋範圍。

---

## 3. 假設這次選到 pivot = 4

假設隨機選到：

```text
pivotIndex = 5
pivotValue = 4
```

```text
index:   0  1  2  3  4  5
nums :  [3, 2, 1, 5, 6, 4]
                         ^
                       pivot
```

這時 `partition()` 要做的事情是：

- 把所有大於 `4` 的值放左邊
- 把 `4` 放回正確位置

---

## 4. partition 過程

### 初始

```text
index:        0  1  2  3  4  5
nums :       [3, 2, 1, 5, 6, 4]
storeIndex = 0
```

### i = 0

```text
nums[0] = 3
3 > 4 ? 否
```

不交換：

```text
index:        0  1  2  3  4  5
nums :       [3, 2, 1, 5, 6, 4]
storeIndex = 0
```

### i = 1

```text
nums[1] = 2
2 > 4 ? 否
```

不交換。

### i = 2

```text
nums[2] = 1
1 > 4 ? 否
```

不交換。

### i = 3

```text
nums[3] = 5
5 > 4 ? 是
```

交換 `nums[3]` 和 `nums[0]`：

```text
index:        0  1  2  3  4  5
nums :       [5, 2, 1, 3, 6, 4]
storeIndex = 1
```

意思是：

- 索引 `0` 這格已經確定放了一個比 pivot 大的值
- 下一個比 pivot 大的值要放到索引 `1`

### i = 4

```text
nums[4] = 6
6 > 4 ? 是
```

交換 `nums[4]` 和 `nums[1]`：

```text
index:        0  1  2  3  4  5
nums :       [5, 6, 1, 3, 2, 4]
storeIndex = 2
```

---

## 5. 把 pivot 放回去

掃描結束後，把最右邊的 pivot 放到 `storeIndex`：

```text
交換 nums[2] 和 nums[5]
```

結果：

```text
index:   0  1  2  3  4  5
nums :  [5, 6, 4, 3, 2, 1]
                ^
            newPivotIndex = 2
```

這時可以保證：

```text
index 0 ~ 1 : 都 > 4
index 2     : 就是 4
index 3 ~ 5 : 都 <= 4
```

也就是：

```text
[ 比 4 大 | 4 | 比 4 小或等於 ]
```

---

## 6. 判斷答案在左邊還是右邊

我們的目標索引是：

```text
target = 1
```

現在 pivot 的位置是：

```text
newPivotIndex = 2
```

比較後：

```text
2 > 1
```

代表第 `2` 大元素一定在左邊。

所以搜尋範圍縮小成：

```text
index:   0  1
nums :  [5, 6]

left  = 0
right = 1
```

---

## 7. 第二輪搜尋

現在只看這一小段：

```text
index:   0  1
nums :  [5, 6]
target = 1
```

假設這次 pivot 選到 `5`。

partition 後會變成：

```text
index:   0  1
nums :  [6, 5]
             ^
        newPivotIndex = 1
```

此時：

```text
newPivotIndex == target
```

所以答案就是：

```text
nums[1] = 5
```

---

## 8. 整體流程圖

```text
原始陣列
[3, 2, 1, 5, 6, 4]
 target = 1
     |
     v
選 pivot = 4
     |
     v
partition 後
[5, 6, 4, 3, 2, 1]
        ^
        2
     |
     v
因為 2 > 1
往左邊找
     |
     v
[5, 6]
     |
     v
再做 partition
     |
     v
[6, 5]
     ^
     1
     |
     v
1 == target
     |
     v
答案 = 5
```

---

## 9. 這張圖想表達的重點

- `partition` 不會把整個陣列完全排序
- 它只會把 `pivot` 放到正確位置
- `pivot` 左邊都比它大
- `pivot` 右邊都比它小或等於它
- 只要知道 `pivot` 的位置，就能判斷答案要往左找還是往右找
- 因此不需要完整排序，也能找到第 `k` 大元素

---

## 10. 對應到程式碼

### 隨機選 pivot

```c
int pivotIndex = left + rand() % (right - left + 1);
```

### 做 partition

```c
int newPivotIndex = partition(nums, left, right, pivotIndex);
```

### 找到答案

```c
if (newPivotIndex == target) {
    return nums[newPivotIndex];
}
```

### 往左邊縮

```c
if (newPivotIndex > target) {
    right = newPivotIndex - 1;
}
```

### 往右邊縮

```c
else {
    left = newPivotIndex + 1;
}
*/


/*
已補上「記憶體位址 / 陣列索引版」圖解，在 215_Kth Largest Element in an Array_GPT-5.4.md。

這段現在會從 C 的角度解釋：
nums[i] 和 &nums[i] 的差別
swap(&nums[x], &nums[y]) 實際交換的是哪兩格
partition 期間 i、storeIndex、pivotIndex 怎麼移動
為什麼 newPivotIndex 能決定下一步往左還是往右找


## 11. 記憶體位址 / 陣列索引版

這一版專門用 C 的角度來看：

- `nums` 是一個整數陣列
- `nums[i]` 是第 `i` 格的值
- `&nums[i]` 是第 `i` 格的記憶體位址
- `swap(&nums[x], &nums[y])` 代表交換兩個位置上的內容

---

## 12. 陣列在記憶體中的樣子

假設：

```c
int nums[] = {3, 2, 1, 5, 6, 4};
```

可以把它想成：

```text
索引 index
            0        1        2        3        4        5
            |        |        |        |        |        |
值 value    3        2        1        5        6        4
            |        |        |        |        |        |
位址 addr   A0       A1       A2       A3       A4       A5
```

如果用 C 的寫法表示：

```text
nums[0] = 3      &nums[0] = A0
nums[1] = 2      &nums[1] = A1
nums[2] = 1      &nums[2] = A2
nums[3] = 5      &nums[3] = A3
nums[4] = 6      &nums[4] = A4
nums[5] = 4      &nums[5] = A5
```

這裡的 `A0 ~ A5` 只是示意，不是真實位址。

---

## 13. `swap(&nums[i], &nums[j])` 到底在做什麼

例如這行：

```c
swap(&nums[3], &nums[0]);
```

傳進去的是：

```text
a = &nums[3] = A3
b = &nums[0] = A0
```

也就是：

```text
a 指向索引 3
b 指向索引 0
```

交換前：

```text
索引       0        1        2        3        4        5
值         3        2        1        5        6        4
位址       A0       A1       A2       A3       A4       A5
```

`swap()` 裡面做的是：

```c
int temp = *a;
*a = *b;
*b = temp;
```

代入後可看成：

```c
int temp = nums[3];   // temp = 5
nums[3] = nums[0];    // nums[3] = 3
nums[0] = temp;       // nums[0] = 5
```

交換後：

```text
索引       0        1        2        3        4        5
值         5        2        1        3        6        4
位址       A0       A1       A2       A3       A4       A5
```

重點是：

- 記憶體格子的位置沒有變
- 改變的是那些位址裡面存的值

---

## 14. `partition()` 的索引移動圖

先看初始狀態：

```text
index:        0        1        2        3        4        5
addr :       A0       A1       A2       A3       A4       A5
nums :       3        2        1        5        6        4

left       = 0
right      = 5
pivotIndex = 5
pivotValue = 4
storeIndex = 0
```

這代表：

- pivot 在索引 `5`
- pivot 的位址是 `A5`
- pivot 的值是 `4`

---

## 15. 掃描時 `i` 和 `storeIndex` 的關係

### 一開始

```text
index:        0        1        2        3        4        5
nums :       3        2        1        5        6        4
             ^
             i
             ^
        storeIndex
```

此時：

- `i = 0`
- `storeIndex = 0`

因為 `nums[0] = 3` 沒有大於 `4`，所以不動。

---

### 掃到 `i = 3`

```text
index:        0        1        2        3        4        5
nums :       3        2        1        5        6        4
                                  ^
                                  i
             ^
        storeIndex
```

此時：

```text
nums[3] = 5 > 4
```

所以做：

```c
swap(&nums[storeIndex], &nums[i]);
swap(&nums[0], &nums[3]);
```

交換後：

```text
index:        0        1        2        3        4        5
nums :       5        2        1        3        6        4
                      ^
                 storeIndex
```

然後：

```c
storeIndex++;
```

變成：

```text
storeIndex = 1
```

意思就是：

- 索引 `0` 已經被確定放了「比 pivot 大」的值
- 下一個符合條件的值，要放到索引 `1`

---

### 掃到 `i = 4`

```text
index:        0        1        2        3        4        5
nums :       5        2        1        3        6        4
                                           ^
                                           i
                      ^
                 storeIndex
```

此時：

```text
nums[4] = 6 > 4
```

所以做：

```c
swap(&nums[1], &nums[4]);
```

交換後：

```text
index:        0        1        2        3        4        5
nums :       5        6        1        3        2        4
                               ^
                          storeIndex
```

接著：

```text
storeIndex = 2
```

---

## 16. 把 pivot 放回正確索引

掃描完成後做：

```c
swap(&nums[storeIndex], &nums[right]);
swap(&nums[2], &nums[5]);
```

交換前：

```text
index:        0        1        2        3        4        5
addr :       A0       A1       A2       A3       A4       A5
nums :       5        6        1        3        2        4
                               ^                          ^
                          storeIndex                    right
```

交換後：

```text
index:        0        1        2        3        4        5
addr :       A0       A1       A2       A3       A4       A5
nums :       5        6        4        3        2        1
                               ^
                          newPivotIndex
```

這代表：

- `A2` 這格現在放的是 pivot `4`
- pivot 已經被放到它正確的位置

---

## 17. 為什麼 `newPivotIndex = 2` 很重要

當 `partition()` 回傳 `2` 時，表示：

```text
nums[2] 已經是最終正確位置
```

也就是：

```text
比 nums[2] 大的值都在左邊
比 nums[2] 小或等於的值都在右邊
```

如果我們要找的是：

```text
target = 1
```

那就表示答案一定不在右邊，因為：

```text
索引 2 已經比 target 更右
```

所以直接縮小到：

```c
right = newPivotIndex - 1;
```

---

## 18. 用指標語言重新理解這題

這份 C 程式本質上是在做兩件事：

1. 用索引決定要處理哪一格
2. 用位址把兩格裡的值交換

也就是：

- `i`、`left`、`right`、`storeIndex` 控制「看哪一格」
- `&nums[x]` 控制「改哪一格的內容」

所以你可以把它想成：

```text
索引負責定位
位址負責修改
```

---

## 19. 這樣看程式碼會更直觀

### 這行是在選一個要觀察的格子

```c
int pivotIndex = left + rand() % (right - left + 1);
```

### 這行是在把兩個記憶體位置裡的值交換

```c
swap(&nums[storeIndex], &nums[i]);
```

### 這行是在把 pivot 放回正確位置

```c
swap(&nums[storeIndex], &nums[right]);
```

### 這行是在縮小之後還要搜尋的索引範圍

```c
right = newPivotIndex - 1;
```

或

```c
left = newPivotIndex + 1;
```
*/