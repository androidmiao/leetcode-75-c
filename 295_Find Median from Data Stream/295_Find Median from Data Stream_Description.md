# 295. Find Median from Data Stream

**Difficulty:** Hard

## Problem Statement

The **median** is the middle value in an ordered integer list. If the size of the list is even, there is no middle value, and the median is the mean of the two middle values.

Implement the `MedianFinder` class:

- `MedianFinder()` initializes the `MedianFinder` object.
- `void addNum(int num)` adds the integer `num` from the data stream to the data structure.
- `double findMedian()` returns the median of all elements so far. Answers within 10⁻⁵ of the actual answer will be accepted.

## Examples

### Example 1

```
Input
["MedianFinder", "addNum", "addNum", "findMedian", "addNum", "findMedian"]
[[], [1], [2], [], [3], []]

Output
[null, null, null, 1.5, null, 2.0]

Explanation
MedianFinder medianFinder = new MedianFinder();
medianFinder.addNum(1);    // arr = [1]
medianFinder.addNum(2);    // arr = [1, 2]
medianFinder.findMedian(); // return 1.5 (i.e., (1 + 2) / 2)
medianFinder.addNum(3);    // arr = [1, 2, 3]
medianFinder.findMedian(); // return 2.0
```

## Constraints

- `-10⁵ <= num <= 10⁵`
- There will be at least one element in the data structure before calling `findMedian`.
- At most `5 * 10⁴` calls will be made to `addNum` and `findMedian`.

## Follow Up

1. If all integer numbers from the stream are in the range `[0, 100]`, how would you optimize your solution?
2. If `99%` of all integer numbers from the stream are in the range `[0, 100]`, how would you optimize your solution?

---

# 295. 求資料流的中位數

**難度：** 困難

## 題目描述

**中位數**是有序整數列表中的中間值。如果列表大小為偶數，則沒有中間值，中位數是兩個中間值的平均值。

例如，`arr = [2,3,4]` 的中位數是 `3`。
例如，`arr = [2,3]` 的中位數是 `(2 + 3) / 2 = 2.5`。

實作 `MedianFinder` 類別：

- `MedianFinder()` 初始化 `MedianFinder` 物件。
- `void addNum(int num)` 將資料流中的整數 `num` 加入資料結構中。
- `double findMedian()` 回傳目前所有元素的中位數。與實際答案的誤差在 10⁻⁵ 以內的答案將被接受。

## 範例

### 範例 1

```
輸入
["MedianFinder", "addNum", "addNum", "findMedian", "addNum", "findMedian"]
[[], [1], [2], [], [3], []]

輸出
[null, null, null, 1.5, null, 2.0]

說明
MedianFinder medianFinder = new MedianFinder();
medianFinder.addNum(1);    // arr = [1]
medianFinder.addNum(2);    // arr = [1, 2]
medianFinder.findMedian(); // 回傳 1.5（即 (1 + 2) / 2）
medianFinder.addNum(3);    // arr = [1, 2, 3]
medianFinder.findMedian(); // 回傳 2.0
```

## 限制條件

- `-10⁵ <= num <= 10⁵`
- 呼叫 `findMedian` 前，資料結構中至少有一個元素。
- `addNum` 和 `findMedian` 的總呼叫次數最多為 `5 * 10⁴`。

## 進階問題

1. 如果資料流中所有整數都在 `[0, 100]` 範圍內，你會如何優化你的解法？
   - 可以使用計數排序（counting sort）：維護一個大小為 101 的計數陣列，`addNum` 為 O(1)，`findMedian` 為 O(100) = O(1)。
2. 如果資料流中 `99%` 的整數都在 `[0, 100]` 範圍內，你會如何優化？
   - 仍然使用計數陣列處理 `[0, 100]` 範圍的數字，另外追蹤小於 0 和大於 100 的數字數量。中位數大機率落在 `[0, 100]` 範圍內，可以用計數陣列快速找到。
