# 217. Contains Duplicate - Editorial

## Approach 1: Hash Set (Recommended)

### Algorithm

Use a hash set (hash table) to track all elements we have seen. For each element, check if it's already in the set. If yes, return true. Otherwise, add it to the set and continue.

**Steps:**
1. Initialize an empty hash set
2. Iterate through each element in the array
3. If the element is in the set, return true
4. Otherwise, add the element to the set
5. If we finish the iteration without finding a duplicate, return false

### Complexity Analysis

- **Time Complexity**: O(n), where n is the number of elements. Each insert and lookup in a hash set is O(1) on average.
- **Space Complexity**: O(min(n, m)) where m is the range of values. At most n elements are stored.

### Pseudocode

```
seen = empty set
for each num in nums:
    if num in seen:
        return true
    add num to seen
return false
```

## Approach 2: Sorting (Alternative for C)

### Algorithm

Sort the array first. If there are duplicates, they will be adjacent after sorting. Then check adjacent elements.

**Steps:**
1. Sort the input array
2. Compare each element with the next element
3. If any two adjacent elements are equal, return true
4. If no adjacent elements are equal, return false

### Complexity Analysis

- **Time Complexity**: O(n log n), dominated by the sorting operation.
- **Space Complexity**: O(1) or O(log n) depending on the sorting implementation (if we count the recursion stack).

### Code

```c
int compare(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

bool containsDuplicate(int* nums, int numsSize) {
    if (numsSize <= 1) return false;

    qsort(nums, numsSize, sizeof(int), compare);

    for (int i = 0; i < numsSize - 1; i++) {
        if (nums[i] == nums[i + 1]) {
            return true;
        }
    }

    return false;
}
```

## Approach 3: Brute Force

### Algorithm

Check every pair of elements to see if they are equal.

**Steps:**
1. For each element at index i
2. Check all elements after it (from index i+1 to end)
3. If any are equal, return true
4. If no duplicates found, return false

### Complexity Analysis

- **Time Complexity**: O(n²), checking all pairs.
- **Space Complexity**: O(1), no extra space needed.

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Hash Set | O(n) | O(n) | Optimal time | Uses extra space |
| Sorting | O(n log n) | O(1) | No extra space | Modifies input |
| Brute Force | O(n²) | O(1) | Simple logic | Too slow for large inputs |

---

# 217. 存在重复元素 - 编辑社论

## 方法1: 哈希集合 (推荐)

### 算法

使用哈希集合来跟踪所有已看到的元素。对于每个元素，检查它是否已在集合中。如果是，返回 true。否则，将其添加到集合并继续。

**步骤：**
1. 初始化一个空哈希集合
2. 遍历数组中的每个元素
3. 如果元素在集合中，返回 true
4. 否则，将元素添加到集合
5. 如果遍历完成而未找到重复，返回 false

### 复杂度分析

- **时间复杂度**: O(n)，其中 n 是元素个数。哈希集合的插入和查找平均为 O(1)。
- **空间复杂度**: O(min(n, m))，其中 m 是值的范围。最多存储 n 个元素。

## 方法2: 排序

### 算法

先对数组进行排序。如果有重复元素，排序后会相邻。然后检查相邻元素。

**步骤：**
1. 对输入数组进行排序
2. 比较每个元素与下一个元素
3. 如果任何两个相邻元素相等，返回 true
4. 如果没有相邻元素相等，返回 false

### 复杂度分析

- **时间复杂度**: O(n log n)，由排序操作主导。
- **空间复杂度**: O(1) 或 O(log n)，取决于排序实现。

## 方法3: 暴力法

### 算法

检查每一对元素是否相等。

**步骤：**
1. 对索引 i 处的每个元素
2. 检查它之后的所有元素(从索引 i+1 到结束)
3. 如果有任何相等的，返回 true
4. 如果未找到重复，返回 false

### 复杂度分析

- **时间复杂度**: O(n²)，检查所有对。
- **空间复杂度**: O(1)，不需要额外空间。

## 比较

| 方法 | 时间 | 空间 | 优点 | 缺点 |
|------|------|------|------|------|
| 哈希集合 | O(n) | O(n) | 最优时间 | 使用额外空间 |
| 排序 | O(n log n) | O(1) | 无额外空间 | 修改输入 |
| 暴力法 | O(n²) | O(1) | 逻辑简单 | 大输入时太慢 |
