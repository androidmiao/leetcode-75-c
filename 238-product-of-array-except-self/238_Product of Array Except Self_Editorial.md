## English

# Solution

From the looks of it, this seems like a simple enough problem to solve in linear time and space. We can simply take the product of all the elements in the given array and then, for each of the elements `x` of the array, we can simply find product of array except self value by dividing the product by `x`.

Doing this for each of the elements would solve the problem. However, there's a note in the problem which says that we are not allowed to use division operation. That makes solving this problem a bit harder.

## Approach 1: Left and Right product lists

It's much easier to build an intuition for solving this problem without division once you visualize how the different products except self look like for each of the elements. So, let's take a look at an example array and the different products.

Looking at the figure above we can figure another way of computing those different product values.

Instead of dividing the product of all the numbers in the array by the number at a given index to get the corresponding product, we can make use of the product of all the numbers to the left and all the numbers to the right of the index. Multiplying these two individual products would give us the desired result as well.

For every given index, `i`, we will make use of the product of all the numbers to the left of it and multiply it by the product of all the numbers to the right. This will give us the product of all the numbers except the one at the given index `i`. Let's look at a formal algorithm describing this idea more concretely.

### Algorithm

1. Initialize two empty arrays, `L` and `R` where for a given index `i`, `L[i]` would contain the product of all the numbers to the left of `i` and `R[i]` would contain the product of all the numbers to the right of `i`.
2. We would need two different loops to fill in values for the two arrays. For the array `L`, `L[0]` would be `1` since there are no elements to the left of the first element. For the rest of the elements, we simply use `L[i] = L[i - 1] * nums[i - 1]`. Remember that `L[i]` represents the product of all the elements to the left of the element at index `i`.
3. For the other array, we do the same thing but in reverse, i.e. we start with the initial value of `1` in `R[length - 1]` where `length` is the number of elements in the array, and keep updating `R[i]` in reverse. Essentially, `R[i] = R[i + 1] * nums[i + 1]`. Remember that `R[i]` represents the product of all the elements to the right of the element at index `i`.
4. Once we have the two arrays set up properly, we simply iterate over the input array one element at a time, and for each element at index `i`, we find the product except self as `L[i] * R[i]`.

Let's go over a simple run of the algorithm that clearly depicts the construction of the two intermediate arrays and finally the answer array.

For the given array `[4,5,1,8,2]`, the `L` and `R` arrays would finally be formed accordingly, and combining them element by element gives the final answer.

### Complexity analysis

Time complexity: `O(N)` where `N` represents the number of elements in the input array. We use one iteration to construct the array `L`, one to construct the array `R`, and one last to construct the answer array using `L` and `R`.

Space complexity: `O(N)` used up by the two intermediate arrays that we constructed to keep track of the product of elements to the left and right.

## Approach 2: O(1) space approach

Although the above solution is good enough to solve the problem since we are not using division anymore, there's a follow-up component as well which asks us to solve this using constant space. Understandably, the output array does not count towards the space complexity. This approach is essentially an extension of the approach above. Basically, we will be using the output array as one of `L` or `R` and we will be constructing the other one on the fly. Let's look at the algorithm based on this idea.

### Algorithm

1. Initialize the empty answer array where for a given index `i`, `answer[i]` would contain the product of all the numbers to the left of `i`.
2. We construct the answer array the same way we constructed the `L` array in the previous approach. These two algorithms are exactly the same except that we are trying to save up on space.
3. The only change in this approach is that we don't explicitly build the `R` array from before. Instead, we simply use a variable to keep track of the running product of elements to the right and we keep updating the answer array by doing `answer[i] = answer[i] * R`. For a given index `i`, `answer[i]` contains the product of all the elements to the left and `R` would contain the product of all the elements to the right. We then update `R` as `R = R * nums[i]`.

### Complexity analysis

Time complexity: `O(N)` where `N` represents the number of elements in the input array. We use one iteration to construct the left products in the answer array, and one more iteration to update the answer using the running right product.

Space complexity: `O(1)` since we don't use any additional array for our computations. The problem statement mentions that using the answer array doesn't add to the space complexity.

## 中文翻譯

# 解題說明

乍看之下，這題似乎可以很簡單地用線性時間與線性空間解決。我們可以先把整個陣列所有元素的乘積算出來，接著對於陣列中的每個元素 `x`，再用總乘積除以 `x`，就能得到「除自己以外陣列元素的乘積」。

如果對每個元素都這樣做，問題就解完了。不過，題目特別註明不能使用除法運算，這就讓這題變得比較困難。

## 方法一：左側乘積與右側乘積陣列

如果想在不使用除法的情況下理解這題，最簡單的方式就是先把每個位置的「除了自己以外的乘積」想像出來。接著，我們來看一個範例陣列，以及這些不同位置對應的乘積結果。

觀察圖示後，我們可以發現另一種計算這些乘積值的方法。

我們不需要把整個陣列的總乘積除以某個索引上的數字，而是可以改用「該索引左邊所有數字的乘積」以及「該索引右邊所有數字的乘積」。只要把這兩個結果相乘，一樣可以得到答案。

對於每一個索引 `i`，我們取它左邊所有數字的乘積，再乘上它右邊所有數字的乘積，就能得到除了 `i` 位置本身以外所有元素的乘積。下面用更正式的演算法來描述這個想法。

### 演算法

1. 建立兩個空陣列 `L` 與 `R`。對任意索引 `i`，`L[i]` 代表 `i` 左邊所有數字的乘積，`R[i]` 代表 `i` 右邊所有數字的乘積。
2. 我們需要兩個不同的迴圈來填入這兩個陣列。對於 `L` 陣列來說，`L[0] = 1`，因為第一個元素左邊沒有任何元素。其餘位置則使用 `L[i] = L[i - 1] * nums[i - 1]`。請記住，`L[i]` 表示索引 `i` 左邊所有元素的乘積。
3. 對於另一個陣列 `R`，作法相同但方向相反。也就是從 `R[length - 1] = 1` 開始，其中 `length` 是陣列長度，然後從右往左更新 `R[i]`。本質上，`R[i] = R[i + 1] * nums[i + 1]`。請記住，`R[i]` 表示索引 `i` 右邊所有元素的乘積。
4. 當這兩個陣列都建立完成後，我們只要再走訪一次輸入陣列，對每個索引 `i` 計算 `L[i] * R[i]`，就能得到該位置的答案。

接著，我們用一個簡單範例實際跑一次演算法，清楚展示兩個中間陣列的建立過程，以及最後答案陣列如何形成。

以陣列 `[4,5,1,8,2]` 為例，最後會得到對應的 `L` 與 `R` 陣列，接著再把兩者逐項相乘，就能得到最終答案。

### 複雜度分析

時間複雜度：`O(N)`，其中 `N` 代表輸入陣列中的元素數量。我們用一次走訪建立 `L`，一次走訪建立 `R`，最後再走訪一次用 `L` 與 `R` 組合出答案。

空間複雜度：`O(N)`，因為我們建立了兩個額外的中間陣列，分別用來記錄左側乘積與右側乘積。

## 方法二：`O(1)` 額外空間作法

雖然上面的解法已經足夠解出題目，因為它不再使用除法，但題目還有一個進階要求，希望我們使用常數額外空間來完成。需要注意的是，輸出陣列本身不算在空間複雜度內。這個方法本質上是前一種方法的延伸。基本想法是：把輸出陣列當成 `L` 或 `R` 其中之一來使用，另一個則在走訪過程中動態計算。下面來看這個想法的演算法。

### 演算法

1. 建立一個空的答案陣列 `answer`，其中 `answer[i]` 用來存放索引 `i` 左邊所有數字的乘積。
2. 建立 `answer` 的方式與前一種方法建立 `L` 陣列完全相同。這兩個演算法本質上是一樣的，只是這裡的目標是節省空間。
3. 這個方法唯一的不同在於，我們不再額外建立 `R` 陣列。相反地，我們使用一個變數來追蹤「右邊元素乘積」的累積結果，並在走訪時持續更新答案陣列，也就是 `answer[i] = answer[i] * R`。對於某個索引 `i`，`answer[i]` 代表左邊所有元素的乘積，而 `R` 代表右邊所有元素的乘積。接著再更新 `R = R * nums[i]`。

### 複雜度分析

時間複雜度：`O(N)`，其中 `N` 代表輸入陣列中的元素數量。我們先走訪一次建立答案陣列中的左側乘積，再走訪一次用右側累積乘積更新答案。

空間複雜度：`O(1)`，因為計算過程中沒有使用額外的陣列。根據題目定義，答案陣列不列入額外空間計算。
