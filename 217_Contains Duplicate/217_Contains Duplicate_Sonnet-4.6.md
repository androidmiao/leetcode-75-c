# 217. Contains Duplicate - Implementation vs Editorial

## Implementation Overview

The provided C solution uses **Sorting** approach, which is practical for C when a proper hash set library isn't available.

## Comparison Table

| Aspect | Implementation | Editorial Baseline | Editorial Recommended |
|--------|-----------------|-------------------|----------------------|
| Algorithm | Sorting | Brute Force | Hash Set |
| Time Complexity | O(n log n) | O(n²) | O(n) |
| Space Complexity | O(1) | O(1) | O(n) |
| Implementation Complexity | Low | Very Low | Medium (needs HashMap) |
| LeetCode Status | ✓ Accepted | ✗ TLE on large inputs | ✓ Accepted (Optimal) |
| Code Clarity | Clear | Simple | Clear |
| Best For | C without HashMap | Simple understanding | Production use |

## Baseline vs Recommended Approach

### Baseline Approach (Brute Force)
```
Nested loops checking every pair:
- Outer loop: for each element i
- Inner loop: for each element j after i
  - Compare nums[i] with nums[j]

Result: O(n²) - fails for n ≥ 10^4
```

### Recommended Approach (Hash Set)
```
Single pass with hash set:
- For each element:
  - Check if in set (O(1))
  - If yes, return true
  - Else add to set

Result: O(n) - optimal for all sizes
```

## Why Implementation Uses Sorting

In C, implementing a hash set requires either:
1. Complex code using chaining or open addressing
2. External libraries (not available in LeetCode)
3. Assuming a specific range for values (not general)

**Sorting approach**:
- Uses built-in `qsort`
- Clear logic: duplicates are adjacent after sorting
- O(n log n) is acceptable for n ≤ 10^5
- Simpler to implement and understand

## Practical Performance

For n = 10^5:
- Brute force: 10^10 operations → TLE (2-5 seconds allowed, this takes 20+ seconds)
- Sorting: 10^5 * log(10^5) ≈ 10^5 * 17 = 1.7 × 10^6 operations → ~5ms ✓
- Hash set: 10^5 operations → ~1ms ✓

So while hash set is theoretically better, sorting is much better than brute force.

## Edge Cases Handled

| Case | Input | Output | Explanation |
|------|-------|--------|-------------|
| Single element | [1] | false | Only one element, can't have duplicate |
| Two duplicates | [1,1] | true | Obvious duplicate |
| All same | [1,1,1,1] | true | Finds first duplicate immediately |
| All different | [1,2,3,4,5] | false | No duplicates |
| Negative numbers | [-1,-1,0] | true | Works with negative values |

## Key Insight in Implementation

```c
// Sort array - duplicates will be adjacent
qsort(nums, numsSize, sizeof(int), compare);

// Simple check: if adjacent elements equal, found duplicate
for (int i = 0; i < numsSize - 1; i++) {
    if (nums[i] == nums[i + 1]) return true;
}
```

This is much simpler than implementing a hash table in C, making it ideal for interviews or competitive programming.

---

# 217. 存在重复元素 - 实现与编辑社论比较

## 实现概述

提供的 C 解决方案使用 **排序** 方法，这在没有适当哈希集库的 C 中是实用的。

## 比较表

| 方面 | 实现 | 编辑基线 | 编辑推荐 |
|------|------|--------|---------|
| 算法 | 排序 | 暴力法 | 哈希集合 |
| 时间复杂度 | O(n log n) | O(n²) | O(n) |
| 空间复杂度 | O(1) | O(1) | O(n) |
| 实现复杂性 | 低 | 非常低 | 中等(需要HashMap) |
| LeetCode状态 | ✓ 接受 | ✗ 大输入超时 | ✓ 接受(最优) |
| 代码清晰度 | 清晰 | 简单 | 清晰 |
| 最适用于 | 无HashMap的C | 简单理解 | 生产环境 |

## 基线方法与推荐方法对比

### 基线方法 (暴力法)
```
嵌套循环检查每一对:
- 外层循环: 对每个元素 i
- 内层循环: 对 i 之后的每个元素 j
  - 比较 nums[i] 与 nums[j]

结果: O(n²) - n ≥ 10^4 时失败
```

### 推荐方法 (哈希集合)
```
单次遍历，使用哈希集合:
- 对于每个元素:
  - 检查是否在集合中 (O(1))
  - 如果是，返回 true
  - 否则添加到集合

结果: O(n) - 对所有大小都最优
```

## 为什么实现使用排序

在 C 中，实现哈希集合需要:
1. 复杂的代码使用链接法或开放寻址
2. 外部库(LeetCode 不可用)
3. 假设值的特定范围(不通用)

**排序方法**:
- 使用内置 `qsort`
- 逻辑清晰: 排序后重复元素相邻
- O(n log n) 对 n ≤ 10^5 可接受
- 更简单实现和理解

## 实际性能对比

对于 n = 10^5:
- 暴力法: 10^10 操作 → 超时(允许2-5秒，需要20+ 秒)
- 排序: 10^5 * log(10^5) ≈ 1.7 × 10^6 操作 → ~5ms ✓
- 哈希集合: 10^5 操作 → ~1ms ✓

虽然哈希集合理论上更好，但排序比暴力法好得多。

## 处理的边界情况

| 情况 | 输入 | 输出 | 解释 |
|------|------|------|------|
| 单个元素 | [1] | false | 只有一个，无法有重复 |
| 两个重复 | [1,1] | true | 明显重复 |
| 全部相同 | [1,1,1,1] | true | 立即找到重复 |
| 全不相同 | [1,2,3,4,5] | false | 无重复 |
| 负数 | [-1,-1,0] | true | 适用于负值 |
