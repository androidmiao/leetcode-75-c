# 295. Find Median from Data Stream - Editorial

## Approach 1: Two Heaps (Recommended)

### Algorithm

Maintain two heaps: max-heap for lower half, min-heap for upper half.

**Steps:**
1. Add number to appropriate heap
2. Balance heaps: max_size == min_size or max_size == min_size + 1
3. Median = (max_root + min_root) / 2 if equal size, else max_root

### Complexity

- **Add**: O(log n)
- **Find Median**: O(1)
- **Space**: O(n)

---

# 295. 数据流的中位数 - 编辑社论

## 方法1: 两个堆 (推荐)

维护最大堆和最小堆以动态找到中位数。

### 复杂度

- **添加**: O(log n)
- **查找中位数**: O(1)
- **空间**: O(n)
