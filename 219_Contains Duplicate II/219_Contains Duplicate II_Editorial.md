# 219. Contains Duplicate II - Editorial

## Approach 1: Hash Map with Sliding Window (Recommended)

### Algorithm

Maintain a sliding window of at most `k` elements using a hash map. For each element, check if it already exists in the window. If yes, return true (the distance is automatically <= k because the window size is <= k). Otherwise, add it and remove the oldest element if the window exceeds size k.

**Steps:**
1. Create a hash map to store values and their most recent indices
2. Initialize two pointers: left = 0, right = 0 (or just use a single pointer with index tracking)
3. For each element at index i:
   - Check if nums[i] exists in the hash map
   - If yes and the last occurrence is within distance k, return true
   - Add nums[i] with index i to the hash map
   - If the map size exceeds k, remove the element at index i-k

### Complexity Analysis

- **Time Complexity**: O(n), where n is the length of the array. Each element is processed once.
- **Space Complexity**: O(min(n, k)), the hash map contains at most k elements.

### Pseudocode

```
map = {}
for i = 0 to n-1:
    if nums[i] in map and i - map[nums[i]] <= k:
        return true
    map[nums[i]] = i

    // Keep only k most recent elements
    if i >= k:
        remove nums[i-k] from map

return false
```

## Approach 2: Set with Sliding Window

### Algorithm

Use a set instead of a hash map to store only values (not indices). Maintain the window size at <= k.

**Steps:**
1. Create a set
2. For each element at index i:
   - If the element is in the set, return true
   - Add the element to the set
   - If the set size exceeds k, remove the oldest element (nums[i-k])

### Complexity Analysis

- **Time Complexity**: O(n)
- **Space Complexity**: O(min(n, k))

### Pseudocode

```
set = {}
for i = 0 to n-1:
    if nums[i] in set:
        return true
    set.add(nums[i])

    if i >= k:
        set.remove(nums[i-k])

return false
```

## Approach 3: Brute Force

### Algorithm

For each element, check all elements within distance k to see if there's a duplicate.

**Steps:**
1. For each index i:
   - Check all indices from max(0, i-k) to i-1
   - If any nums[j] == nums[i], return true

### Complexity Analysis

- **Time Complexity**: O(n × k), checking k elements for each of n elements.
- **Space Complexity**: O(1)

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Hash Map | O(n) | O(min(n,k)) | Optimal, elegant | Requires hash map |
| Set | O(n) | O(min(n,k)) | Simple, clean | Same as hash map for this problem |
| Brute Force | O(n × k) | O(1) | No extra space | Slow for large k |

---

# 219. 存在重复元素 II - 编辑社论

## 方法1: 哈希表与滑动窗口 (推荐)

### 算法

使用哈希表维护一个大小至多为 k 的滑动窗口。对于每个元素，检查它是否已在窗口中。如果是，返回 true (距离自动 <= k，因为窗口大小 <= k)。否则，添加它，并在窗口超过大小 k 时移除最旧的元素。

**步骤：**
1. 创建哈希表存储值及其最近的索引
2. 初始化指针: left = 0, right = 0
3. 对于索引 i 处的每个元素：
   - 检查 nums[i] 是否在哈希表中
   - 如果是且最后出现的索引在距离 k 内，返回 true
   - 将 nums[i] 与索引 i 添加到哈希表
   - 如果表大小超过 k，移除索引 i-k 处的元素

### 复杂度分析

- **时间复杂度**: O(n)，其中 n 是数组长度。每个元素被处理一次。
- **空间复杂度**: O(min(n, k))，哈希表最多包含 k 个元素。

## 方法2: 集合与滑动窗口

### 算法

使用集合而不是哈希表，仅存储值(不存储索引)。维护窗口大小 <= k。

**步骤：**
1. 创建集合
2. 对于索引 i 处的每个元素：
   - 如果元素在集合中，返回 true
   - 将元素添加到集合
   - 如果集合大小超过 k，移除最旧的元素(nums[i-k])

### 复杂度分析

- **时间复杂度**: O(n)
- **空间复杂度**: O(min(n, k))

## 方法3: 暴力法

### 算法

对于每个元素，检查距离 k 内的所有元素是否有重复。

**步骤：**
1. 对于索引 i：
   - 检查索引从 max(0, i-k) 到 i-1 的所有元素
   - 如果任何 nums[j] == nums[i]，返回 true

### 复杂度分析

- **时间复杂度**: O(n × k)
- **空间复杂度**: O(1)

## 比较

| 方法 | 时间 | 空间 | 优点 | 缺点 |
|------|------|------|------|------|
| 哈希表 | O(n) | O(min(n,k)) | 最优，优雅 | 需要哈希表 |
| 集合 | O(n) | O(min(n,k)) | 简单清晰 | 与哈希表相同 |
| 暴力法 | O(n × k) | O(1) | 无额外空间 | 大k时缓慢 |
