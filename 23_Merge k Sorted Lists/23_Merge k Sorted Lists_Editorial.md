# 23. Merge k Sorted Lists - Editorial

## Approach 1: Divide and Conquer (Recommended)

### Algorithm

Recursively divide the k lists into two halves, merge the results of each half, then merge the two resulting lists.

**Steps:**
1. Base case: if left == right, return lists[left]
2. Find middle index
3. Recursively merge left half: lists[0..mid]
4. Recursively merge right half: lists[mid+1..k-1]
5. Merge the two results using mergeTwoLists

### Complexity Analysis

- **Time**: O(N log k), where N is total nodes
- **Space**: O(log k) for recursion stack

## Approach 2: Min-Heap

### Algorithm

Use a min-heap to always get the smallest next node from all k lists.

### Complexity Analysis

- **Time**: O(N log k)
- **Space**: O(k)

## Comparison

| Approach | Time | Space | Pros |
|----------|------|-------|------|
| Divide & Conquer | O(N log k) | O(log k) | Optimal |
| Min-Heap | O(N log k) | O(k) | Intuitive |

---

# 23. 合并K个排序链表 - 编辑社论

## 方法1: 分治法 (推荐)

### 算法

递归地将 k 个链表分为两半，合并每一半的结果，然后合并两个结果链表。

### 复杂度分析

- **时间**: O(N log k)，其中 N 是总节点数
- **空间**: O(log k) 递归栈

## 方法2: 最小堆

### 算法

使用最小堆始终从所有 k 个链表中获取最小的下一个节点。

### 复杂度分析

- **时间**: O(N log k)
- **空间**: O(k)
