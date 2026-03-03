Approach: Prefix Sum
做法：前綴和（Prefix Sum）

Intuition
直覺

We start from the altitude 0 and we have a list of N integers, where each integer represents the gain in altitude at each step (it could be negative as well, which implies a fall in altitude) a biker takes. We need to return the highest altitude of the biker in the complete journey, including the starting point at 0.
我們從海拔 0 開始，並有一個長度為 N 的整數串列，其中每個整數代表騎士每一步的海拔變化（也可能是負數，表示海拔下降）。我們需要回傳騎士在整段旅程中的最高海拔，包含起點 0。

This can be solved by taking the maximum altitudes at each step in the journey. The altitude at a step can be determined as the altitude at the previous step plus the gain at the current step. Hence, we will start from 0 and keep adding the gain in altitude to it at each step, and after each addition, we will update the maximum altitude we have seen so far.
這個問題可以透過在旅程每一步追蹤最大海拔來解決。某一步的海拔可由「前一步海拔 + 目前步驟的海拔變化」得到。因此，我們從 0 開始，每一步把海拔變化加上去，並在每次相加後更新目前為止看過的最高海拔。

fig
圖示

If we observe closely, the altitude at a point is the sum of gains on the left of it, which is nothing but the prefix sum at this index. Therefore, we can find the prefix sum and return the maximum as the highest reached altitude.
仔細觀察可知，某一點的海拔就是它左側所有海拔變化的總和，也就是該索引位置的前綴和。因此，我們可以計算前綴和，並回傳其中最大值作為最高海拔。

Algorithm
演算法

Initialize the variable currentAltitude to 0; this is the current altitude of the biker.
將變數 currentAltitude 初始化為 0；這是騎士目前的海拔。

Initialize the variable highestPoint to currentAltitude, as the highest altitude we have seen is 0.
將變數 highestPoint 初始化為 currentAltitude，因為目前看到的最高海拔是 0。

Iterate over the gain in altitude in the list gain and add the current gain altitudeGain to the variable currentAltitude.
走訪串列 gain 中的每個海拔變化，將目前的變化值 altitudeGain 加到 currentAltitude。

Update the variable highestPoint as necessary.
視需要更新變數 highestPoint。

Return highestPoint.
回傳 highestPoint。

Implementation
實作


Complexity Analysis
複雜度分析

Here, N is the number of integers in the list gain.
這裡的 N 是串列 gain 中整數的數量。

Time complexity: O(N).
時間複雜度：O(N)。

We iterate over every integer in the list gain only once, and hence the total time complexity is equal to O(N).
我們只需走訪串列 gain 一次，因此總時間複雜度為 O(N)。

Space complexity: O(1).
空間複雜度：O(1)。

We only need two variables, currentAltitude andhighestPoint; hence the space complexity is constant.
我們只需要兩個變數 `currentAltitude` 和 `highestPoint`，因此空間複雜度是常數。


---
