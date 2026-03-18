The median is the middle value in an ordered integer list. If the size of the list is even, there is no middle value. So the median is the mean of the two middle values.

For examples, if arr = [2,3,4], the median is 3.
For examples, if arr = [1,2,3,4], the median is (2 + 3) / 2 = 2.5.
You are given an integer array nums and an integer k. There is a sliding window of size k which is moving from the very left of the array to the very right. You can only see the k numbers in the window. Each time the sliding window moves right by one position.

Return the median array for each window in the original array. Answers within 10-5 of the actual value will be accepted.

Example 1:

Input: nums = [1,3,-1,-3,5,3,6,7], k = 3
Output: [1.00000,-1.00000,-1.00000,3.00000,5.00000,6.00000]
Explanation:
Window position                Median
---------------                -----
[1  3  -1] -3  5  3  6  7        1
 1 [3  -1  -3] 5  3  6  7       -1
 1  3 [-1  -3  5] 3  6  7       -1
 1  3  -1 [-3  5  3] 6  7        3
 1  3  -1  -3 [5  3  6] 7        5
 1  3  -1  -3  5 [3  6  7]       6

Example 2:

Input: nums = [1,2,3,4,2,3,1,4,2], k = 3
Output: [2.00000,3.00000,3.00000,3.00000,2.00000,3.00000,2.00000]

Constraints:

1 <= k <= nums.length <= 10^5
-2^31 <= nums[i] <= 2^31 - 1

Hint:
1. The simplest of solutions comes from the basic idea of finding the median given a set of numbers. We know that by definition, a median is the center element (or an average of the two center elements). Given an unsorted list of numbers, how do we find the median element? If you know the answer to this question, can we extend this idea to every sliding window that we come across in the array?

2. Is there a better way to do what we are doing in the above hint? Don't you think there is duplication of calculation being done there? Is there some sort of optimization that we can do to achieve the same result? This approach is merely a modification of the basic approach except that it simply reduces duplication of calculations once done.

3. The third line of thought is also based on this same idea but achieving the result in a different way. We obviously need the window to be sorted for us to be able to find the median. Is there a data-structure out there that we can use (in one or more quantities) to obtain the median element extremely fast, say O(1) time while having the ability to perform the other operations fairly efficiently as well?

---
中位數是排序後整數串列中的中間值。如果串列大小是偶數，就不會有唯一的中間值，因此中位數就是中間兩個值的平均數。

例如，若 arr = [2,3,4]，則中位數為 3。
例如，若 arr = [1,2,3,4]，則中位數為 (2 + 3) / 2 = 2.5。
給定一個整數陣列 nums 和一個整數 k。有一個大小為 k 的滑動視窗，會從陣列最左邊一路移動到最右邊。你只能看到目前視窗中的 k 個數字。每次滑動視窗都會向右移動一個位置。

請回傳原始陣列中每個視窗對應的中位數陣列。若答案與實際值的誤差在 10^-5 以內，皆視為正確。

範例 1：

輸入：nums = [1,3,-1,-3,5,3,6,7], k = 3
輸出：[1.00000,-1.00000,-1.00000,3.00000,5.00000,6.00000]
說明：
視窗位置                      中位數
---------------              -----
[1  3  -1] -3  5  3  6  7        1
 1 [3  -1  -3] 5  3  6  7       -1
 1  3 [-1  -3  5] 3  6  7       -1
 1  3  -1 [-3  5  3] 6  7        3
 1  3  -1  -3 [5  3  6] 7        5
 1  3  -1  -3  5 [3  6  7]       6

範例 2：

輸入：nums = [1,2,3,4,2,3,1,4,2], k = 3
輸出：[2.00000,3.00000,3.00000,3.00000,2.00000,3.00000,2.00000]

限制條件：

1 <= k <= nums.length <= 10^5
-2^31 <= nums[i] <= 2^31 - 1

提示：
1. 最簡單的解法，來自於「給定一組數字後如何找出中位數」這個基本想法。我們知道依定義，中位數是中央元素，或中央兩個元素的平均值。若給你一個未排序的數字串列，要如何找出中位數？如果你知道這個問題的答案，能否把同樣的想法延伸到陣列中的每一個滑動視窗？

2. 有沒有比上面提示更好的作法？你不覺得那樣做會重複很多計算嗎？是否有某種最佳化方式，可以在得到相同結果的同時減少重複運算？這種方法本質上只是基本解法的改良版，重點在於把已經做過的計算盡量重複利用。

3. 第三個思路同樣建立在這個概念上，但會用不同方式達成結果。顯然，若要找出中位數，視窗內容必須保持有序。是否存在某種資料結構，可以是一個或多個，能讓我們以非常快的速度，例如 O(1) 時間取得中位數，同時也能相當有效率地完成其他必要操作？
