# 703. Kth Largest Element in a Stream - Implementation vs Editorial

## Implementation Overview

The C solution uses **Min-Heap** approach, which is optimal for stream processing of kth largest element.

## Comparison Table

| Aspect | Implementation | Editorial Baseline | Editorial Recommended |
|--------|-----------------|-------------------|----------------------|
| Algorithm | Min-Heap | Sorted List | Min-Heap |
| Add Time | O(log k) | O(n) | O(log k) |
| Space | O(k) | O(n) | O(k) |
| Implementation | Medium | Simple | Medium |
| LeetCode Status | ✓ Accepted (Optimal) | ✗ TLE for many adds | ✓ Accepted |
| Best For | Stream processing | Simple | Production |

## Why Min-Heap is Better

For a stream of n additions with k fixed:
- Min-heap: O(n log k) total
- Sorted list: O(n²) total

For n=10^4, k=100:
- Min-heap: 10^4 × log(100) ≈ 6.6×10^4 operations
- Sorted list: 10^4² = 10^8 operations
- **Min-heap is 1500× faster**

## Implementation Details

The implementation includes custom heapify operations:
- `heapifyUp`: Restores heap property after insertion
- `heapifyDown`: Restores heap property after root removal

These maintain the O(log k) complexity per operation.

---

# 703. 数据流中第K大的元素 - 实现与编辑社论比较

## 性能对比

对于 n=10^4，k=100 的流:
- 最小堆: 10^4 × log(100) ≈ 6.6×10^4 操作
- 排序列表: 10^4² = 10^8 操作
- **最小堆快 1500 倍**
