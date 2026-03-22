# 703. Kth Largest Element in a Stream - Editorial

## Approach 1: Min-Heap (Recommended)

### Algorithm

Maintain a min-heap of size k containing the k largest elements. The root of this heap is always the kth largest element.

**Steps:**
1. Initialize a min-heap of capacity k+1
2. Add all initial nums: if heap size < k, add directly; otherwise, if num > root, replace root
3. For each new value added, apply the same logic

### Complexity Analysis

- **Constructor Time**: O(n log k)
- **Add Time**: O(log k)
- **Space**: O(k)

### Pseudocode

```
min_heap = empty heap of size k
for each num in nums:
    if heap.size < k:
        heap.add(num)
    else if num > heap.root():
        heap.replaceRoot(num)

add(val):
    if heap.size < k:
        heap.add(val)
    else if val > heap.root():
        heap.replaceRoot(val)
    return heap.root()
```

## Approach 2: Sorted List / Vector

### Algorithm

Keep all elements in a sorted data structure. For each new element, insert it in sorted order. Return the kth largest.

### Complexity Analysis

- **Add Time**: O(n) for insertion in sorted array
- **Space**: O(n)

## Comparison

| Approach | Add Time | Space | Pros | Cons |
|----------|----------|-------|------|------|
| Min-Heap | O(log k) | O(k) | Optimal time and space | Requires heap implementation |
| Sorted List | O(n) | O(n) | Simpler logic | Slower for large streams |

---

# 703. 数据流中第K大的元素 - 编辑社论

## 方法1: 最小堆 (推荐)

### 算法

维护一个大小为 k 的最小堆，包含 k 个最大的元素。此堆的根始终是第 k 大的元素。

### 复杂度分析

- **构造函数时间**: O(n log k)
- **添加时间**: O(log k)
- **空间**: O(k)

## 方法2: 排序列表

### 算法

将所有元素保持在排序的数据结构中。对于每个新元素，按排序顺序插入。

### 复杂度分析

- **添加时间**: O(n)
- **空间**: O(n)
