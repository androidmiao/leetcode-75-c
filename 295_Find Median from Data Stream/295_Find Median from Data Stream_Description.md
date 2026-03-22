# 295. Find Median from Data Stream

## Problem Statement

The median is the middle value in an ordered integer list. If the size of the list is even, there is no middle value and the median is the mean of the two middle values.

Implement the MedianFinder class:
- `MedianFinder()`: Initializes the MedianFinder object.
- `void addNum(int num)`: Adds the integer num from the data stream
- `double findMedian()`: Returns the median of all elements so far

## Examples

### Example 1:
```
Input: ["MedianFinder", "addNum", "findMedian", "addNum", "findMedian", "addNum", "findMedian"]
Output: [null, null, 1.0, null, 1.5, null, 2.0]

Explanation:
MedianFinder mf = new MedianFinder();
mf.addNum(1);    // arr = [1]
mf.findMedian(); // return 1.0
mf.addNum(2);    // arr = [1, 2]
mf.findMedian(); // return 1.5
mf.addNum(3);    // arr = [1, 2, 3]
mf.findMedian(); // return 2.0
```

## Constraints

- `-10^5 <= num <= 10^5`
- At most 5 × 10^4 calls

## Key Insights

1. **Two Heap Approach**: Max-heap for lower half, Min-heap for upper half
2. **Balance**: Keep sizes equal or max-heap size = min-heap size + 1
3. **Median**: If equal size, average of two roots. Otherwise, max-heap root
