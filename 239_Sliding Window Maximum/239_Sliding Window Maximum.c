//https://leetcode.com/problems/sliding-window-maximum/description/?envType=problem-list-v2&envId=wl94y6ih

// 使用單調遞減雙端佇列（deque）解這題
// 時間複雜度：O(n)
// 空間複雜度：O(k)
//
// 圖解觀念：
// 1. deque 裡面存「索引」，不是直接存數值
// 2. deque 對應的 nums 值，會維持「由大到小」排列
// 3. deque 的隊首，永遠是目前視窗的最大值索引
// 4. 這裡用的是「環形陣列 deque」，因此只需要配置 k 個位置即可
//
// ------------------------------------------------------------
// 範例：
// nums = [1, 3, -1, -3, 5, 3, 6, 7], k = 3
// index   0  1   2   3  4  5  6  7
// value   1  3  -1  -3  5  3  6  7
// ------------------------------------------------------------
//
// Step 1: i = 0, nums[i] = 1
// 視窗還沒形成，只先放入 deque
// deque(索引): [0]
// deque(數值): [1]
//
// Step 2: i = 1, nums[i] = 3
// 3 >= 1，所以索引 0 不可能再成為最大值，移除
// deque(索引): [1]
// deque(數值): [3]
//
// Step 3: i = 2, nums[i] = -1
// -1 比隊尾對應值 3 小，所以直接加入
// deque(索引): [1, 2]
// deque(數值): [3, -1]
// 這時第一個完整視窗是 [1, 3, -1]
// 最大值就是隊首 nums[1] = 3
//
// Step 4: i = 3, nums[i] = -3
// 先檢查隊首索引 1 還在不在視窗內：還在
// -3 比隊尾對應值 -1 小，所以直接加入
// deque(索引): [1, 2, 3]
// deque(數值): [3, -1, -3]
// 目前視窗是 [3, -1, -3]
// 最大值仍然是 nums[1] = 3
//
// Step 5: i = 4, nums[i] = 5
// 先檢查過期元素：索引 1 已經滑出視窗，移除
// 再從隊尾開始比較：
// 5 >= -3，移除索引 3
// 5 >= -1，移除索引 2
// 最後放入索引 4
// deque(索引): [4]
// deque(數值): [5]
// 目前視窗是 [-1, -3, 5]
// 最大值是 nums[4] = 5
//
// Step 6: i = 5, nums[i] = 3
// 3 比隊尾對應值 5 小，所以直接加入
// deque(索引): [4, 5]
// deque(數值): [5, 3]
// 目前視窗是 [-3, 5, 3]
// 最大值是 nums[4] = 5
//
// Step 7: i = 6, nums[i] = 6
// 6 >= 3，移除索引 5
// 6 >= 5，移除索引 4
// 再放入索引 6
// deque(索引): [6]
// deque(數值): [6]
// 目前視窗是 [5, 3, 6]
// 最大值是 nums[6] = 6
//
// Step 8: i = 7, nums[i] = 7
// 7 >= 6，移除索引 6
// 再放入索引 7
// deque(索引): [7]
// deque(數值): [7]
// 目前視窗是 [3, 6, 7]
// 最大值是 nums[7] = 7
//
// 最後答案：
// [3, 3, 5, 5, 6, 7]
//
// ------------------------------------------------------------
// 為什麼要移除隊尾較小元素？
// 假設目前新元素比較大，那麼前面那些比較小的元素：
// 1. 之後不可能比新元素更早成為最大值
// 2. 也不可能在新元素還留在視窗內時打敗新元素
// 所以它們已經沒有保留價值，可以直接刪掉
// ------------------------------------------------------------

#include <stdlib.h>

int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    // 總共有 numsSize - k + 1 個視窗，因此答案陣列大小也是這個數量
    *returnSize = numsSize - k + 1;

    // 配置答案陣列，用來存放每個滑動視窗的最大值
    int* result = (int*)malloc(sizeof(int) * (*returnSize));

    // 配置環形 deque，裡面存的是「索引」而不是數值
    // 因為 deque 的有效大小永遠不會超過 k，所以只需要配置 k 個位置
    int* deque = (int*)malloc(sizeof(int) * k);

    // front 指向隊首元素的位置
    // size 表示目前 deque 中有幾個有效元素
    int front = 0;
    int size = 0;

    // resultIndex 用來記錄目前要把最大值寫到 result 的哪個位置
    int resultIndex = 0;

    // 逐一處理每個元素，i 代表目前走到的索引
    for (int i = 0; i < numsSize; i++) {
        // backIndex 是目前隊尾元素所在的位置
        // insertIndex 是下一個新元素要放入的位置
        int backIndex;
        int insertIndex;

        // 如果隊首索引已經離開目前視窗範圍，就把它移除
        // 目前視窗左邊界是 i - k + 1，所以小於等於 i - k 的都要移掉
        if (size > 0 && deque[front] <= i - k) {
            front = (front + 1) % k;
            size--;
        }

        // 維持 deque 對應的數值為「遞減」
        // 若隊尾對應的數值小於等於目前數值，就不可能再成為未來視窗最大值
        // 因此把這些較小或相等的索引全部移除
        while (size > 0) {
            backIndex = (front + size - 1) % k;

            if (nums[deque[backIndex]] > nums[i]) {
                break;
            }

            size--;
        }

        // 把目前索引放到 deque 隊尾
        insertIndex = (front + size) % k;
        deque[insertIndex] = i;
        size++;

        // 當 i >= k - 1 時，代表第一個完整視窗已經形成
        // 此時 deque 隊首就是目前視窗中的最大值索引
        if (i >= k - 1) {
            result[resultIndex] = nums[deque[front]];
            resultIndex++;
        }
    }

    // deque 只是輔助資料結構，用完後釋放記憶體
    free(deque);

    // 回傳每個滑動視窗的最大值陣列
    return result;
}
