# 703. Kth Largest Element in a Stream

## Problem Statement

Design a class to find the `k`th largest element in a stream. Note that it is the `k`th largest element in the sorted order, not the `k`th distinct element.

Implement the `KthLargest` class:
- `KthLargest(int k, int[] nums)`: Initializes the object with the integer k and the stream of integers nums.
- `int add(int val)`: Adds the integer val to the stream and returns the element representing the kth largest element in the stream.

## Examples

### Example 1:
```
Input
["KthLargest", "add", "add", "add", "add", "add"]
[[3, [4, 5, 8, 2]], [3], [5], [10], [9], [4]]

Output
[null, 4, 5, 5, 8, 8]

Explanation
KthLargest kthLargest = new KthLargest(3, [4, 5, 8, 2]);
kthLargest.add(3);   // return 4
kthLargest.add(5);   // return 5
kthLargest.add(10);  // return 5
kthLargest.add(9);   // return 8
kthLargest.add(4);   // return 8
```

## Constraints

- `1 <= k <= 10^4`
- `0 <= nums.length <= 10^4`
- `-10^4 <= nums[i] <= 10^4`
- `-10^4 <= val <= 10^4`
- At most 10^4 calls will be made to add.

## Key Insights

1. **Min-Heap Strategy**: Maintain a min-heap of size k. The root (top) of the heap is always the kth largest element.

2. **Why Min-Heap**: By keeping only the k largest elements in a min-heap, the smallest among them (at the root) is the kth largest overall.

3. **Adding Elements**: When adding a new element, if it's larger than the heap's root, remove the root and insert the new element.

4. **Efficiency**: Each add operation is O(log k), which is much better than sorting the entire stream.
