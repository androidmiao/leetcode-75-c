# 23. Merge k Sorted Lists - Implementation vs Editorial

## Comparison Table

| Aspect | Implementation | Editorial Baseline | Editorial Recommended |
|--------|-----------------|-------------------|----------------------|
| Algorithm | Divide & Conquer | Min-Heap | Divide & Conquer |
| Time | O(N log k) | O(N log k) | O(N log k) |
| Space | O(log k) | O(k) | O(log k) |
| Implementation | Medium | Complex | Medium |
| LeetCode Status | ✓ Accepted | ✓ Accepted | ✓ Accepted |

## Why Divide & Conquer

Both approaches have O(N log k) time complexity, but divide and conquer:
- Uses less space: O(log k) vs O(k)
- More elegant and easier to understand
- No need for heap implementation

For k=10^4, lists, space savings: 10^4 vs 14 integers
