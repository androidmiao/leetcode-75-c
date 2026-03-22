# 23. Merge k Sorted Lists

## Problem Statement

You are given an array of `k` linked-lists `lists`, each linked-list is sorted in ascending order.

Merge all the linked-lists into one sorted linked-list and return it.

## Examples

### Example 1:
```
Input: lists = [[1,4,5],[1,3,4],[2,6]]
Output: [1,1,2,1,3,4,4,5,6]
```

### Example 2:
```
Input: lists = []
Output: []
```

### Example 3:
```
Input: lists = [[]]
Output: []
```

## Constraints

- `0 <= k <= 10^4`
- `0 <= lists[i].length <= 500`
- `-10^4 <= lists[i][j] <= 10^4`

## Key Insights

1. **Divide and Conquer**: Recursively merge pairs of lists until one remains.

2. **Merge Two Lists**: Base case is efficiently merging two sorted lists in O(n+m) time.

3. **Complexity**: Total time is O(N log k) where N is total number of nodes.

4. **Space**: O(log k) for recursion stack depth.
