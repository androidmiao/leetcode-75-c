// https://leetcode.com/problems/product-of-array-except-self/description/?envType=problem-list-v2&envId=wl94y6ih

#include <stdlib.h>

/*
題目要求：
不能使用除法，並且要在 O(n) 時間內算出每個位置「除了自己以外」的乘積。

核心概念：
answer[i] = 左邊所有數字的乘積 * 右邊所有數字的乘積

--------------------------------------------------
圖解範例：nums = [1, 2, 3, 4]
--------------------------------------------------

索引:        0   1   2   3
nums:        1   2   3   4

Step 1. 先由左到右，建立「左側乘積」

left_product 初始為 1

i = 0
answer[0] = 1
left_product = 1 * nums[0] = 1

i = 1
answer[1] = 1
left_product = 1 * nums[1] = 2

i = 2
answer[2] = 2
left_product = 2 * nums[2] = 6

i = 3
answer[3] = 6
left_product = 6 * nums[3] = 24

此時 answer 內容為：
answer:      1   1   2   6

意思是：
answer[0] = 1                 -> 0 左邊沒有元素
answer[1] = 1                 -> 1 左邊只有 1
answer[2] = 1 * 2 = 2
answer[3] = 1 * 2 * 3 = 6

--------------------------------------------------
Step 2. 再由右到左，把「右側乘積」乘回 answer
--------------------------------------------------

right_product 初始為 1

i = 3
answer[3] = 6 * 1 = 6
right_product = 1 * nums[3] = 4

i = 2
answer[2] = 2 * 4 = 8
right_product = 4 * nums[2] = 12

i = 1
answer[1] = 1 * 12 = 12
right_product = 12 * nums[1] = 24

i = 0
answer[0] = 1 * 24 = 24
right_product = 24 * nums[0] = 24

最後 answer 變成：
answer:     24  12   8   6

也就是：
answer[0] = 2 * 3 * 4 = 24
answer[1] = 1 * 3 * 4 = 12
answer[2] = 1 * 2 * 4 = 8
answer[3] = 1 * 2 * 3 = 6

優點：
1. 只需要兩次線性掃描。
2. 不使用除法。
3. 除了輸出陣列 answer 外，只用到兩個變數 left_product 與 right_product。
*/
int* productExceptSelf(int* nums, int nums_size, int* return_size) {
    // 將回傳陣列的大小設為輸入陣列大小，符合 LeetCode 題目的函式規格。
    *return_size = nums_size;

    // 配置一塊新的記憶體來存放答案陣列，呼叫端在使用完畢後需要負責釋放。
    int* answer = (int*)malloc(sizeof(int) * nums_size);

    // 如果記憶體配置失敗，回傳 NULL，避免後續存取無效指標。
    if (answer == NULL) {
        // 配置失敗時，將回傳大小設為 0，讓呼叫端知道沒有有效結果。
        *return_size = 0;
        // 直接回傳 NULL，表示無法完成計算。
        return NULL;
    }

    // left_product 用來記錄目前索引左側所有元素的乘積，初始值為 1。
    int left_product = 1;

    // 第一趟走訪：先把每個位置左邊所有元素的乘積存進 answer[i]。
    for (int i = 0; i < nums_size; i++) {
        // 在位置 i 先存入「i 左邊所有元素的乘積」。
        answer[i] = left_product;
        // 再把目前的 nums[i] 乘進 left_product，供下一個位置使用。
        left_product *= nums[i];
    }

    // right_product 用來記錄目前索引右側所有元素的乘積，初始值也是 1。
    int right_product = 1;

    // 第二趟反向走訪：把每個位置右邊所有元素的乘積再乘進 answer[i]。
    for (int i = nums_size - 1; i >= 0; i--) {
        // 將右側乘積乘到目前答案上，得到「除了自己以外所有元素的乘積」。
        answer[i] *= right_product;
        // 更新 right_product，讓左邊一格可以使用目前元素作為其右側乘積的一部分。
        right_product *= nums[i];
    }

    // 回傳已配置且已填好結果的答案陣列。
    return answer;
}
