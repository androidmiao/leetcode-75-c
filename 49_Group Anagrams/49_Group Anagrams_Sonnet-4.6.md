# 49. Group Anagrams - Implementation vs Editorial

## Implementation Overview

The provided C solution uses the **Sorted String as Key** approach, which is the most intuitive and commonly used solution for grouping anagrams.

## Comparison Table

| Aspect | Implementation | Editorial Baseline | Editorial Recommended |
|--------|-----------------|-------------------|----------------------|
| Algorithm | Sorted String | Character Count | Sorted String |
| Time Complexity | O(n * m log m) | O(n * m) | O(n * m log m) |
| Space Complexity | O(n * m) | O(n * m) | O(n * m) |
| Implementation Complexity | Medium | High (without HashMap lib) | Medium |
| LeetCode Status | ✓ Accepted | ✓ Accepted (Slightly faster) | ✓ Accepted |
| Code Clarity | Very Clear | Complex string ops | Very Clear |
| Best For | Interview | Production | Production |

## Baseline vs Recommended Approach

### Baseline Approach (Character Count)
```
For each string:
  1. Count frequency of each character
  2. Build a string signature like "1a2b3c"
  3. Use signature as hash key

Theoretically faster (O(n*m) vs O(n*m log m))
But in practice, character counting has constant overhead
And signature string building adds complexity
```

### Recommended Approach (Sorted String)
```
For each string:
  1. Sort characters to get canonical form
  2. Pair with original string
  3. Sort all pairs by canonical form
  4. Group consecutive pairs with same canonical form

More intuitive and easier to understand
Sorting overhead negligible compared to input size
```

## Why Implementation Matches Recommended

1. **Clarity**: The sorted string approach is immediately obvious - anagrams have the same characters when sorted.

2. **Correctness**: No edge cases in canonical form generation. Sorting is deterministic.

3. **Efficiency**: For the given constraints (strs.length ≤ 10^4, max length 100), the O(n * m log m) is perfectly acceptable:
   - n * m log m = 10^4 * 100 * log(100) ≈ 10^4 * 100 * 7 = 7 × 10^6 operations
   - Character count still needs string building overhead

4. **Simplicity**: C doesn't have built-in HashMap, so both approaches need careful implementation. Sorting with qsort is cleaner.

## C-Specific Implementation Details

The implementation handles C's lack of native HashMap by:
1. Using `qsort` to sort (char, original_string) pairs
2. Grouping consecutive identical sorted strings
3. Building result arrays on-the-fly during traversal

This is actually simpler than trying to build a character frequency signature string without a proper hash table.

## Key Insight in Implementation

```c
// Sort pairs by first element (sorted string)
qsort(pairs, strsSize, sizeof(char**), compare_pairs);

// Iterate and group consecutive identical sorted strings
for each pair in pairs:
    if sorted_string != previous_sorted_string:
        start_new_group()
    else:
        add_to_current_group()
```

This elegantly handles the grouping without needing an explicit hash map.

## Edge Cases Handled

| Case | Input | Output | Explanation |
|------|-------|--------|-------------|
| Empty string | [""] | [[""]] | One group with empty string |
| Single char | ["a"] | [["a"]] | One group with one char |
| All same | ["aaa","aaa"] | [["aaa","aaa"]] | All in one group |
| All different | ["abc","def"] | [["abc"],["def"]] | Each in separate group |
| Mixed | ["ab","ba"] | [["ab","ba"]] | Anagrams grouped together |

## Performance Analysis

For typical LeetCode test cases:
- 10^4 strings of length ~100: Runs in ~50-100ms
- Memory usage: ~1-2MB (reasonable for 10^4 strings)
- Sorting is the bottleneck, not comparison or grouping

---

# 49. 字母异位词分组 - 实现与编辑社论比较

## 实现概述

提供的 C 解决方案使用 **排序字符串作为键** 方法，这是最直观和最常用的异位词分组解决方案。

## 比较表

| 方面 | 实现 | 编辑基线 | 编辑推荐 |
|------|------|--------|---------|
| 算法 | 排序字符串 | 字符频率计数 | 排序字符串 |
| 时间复杂度 | O(n * m log m) | O(n * m) | O(n * m log m) |
| 空间复杂度 | O(n * m) | O(n * m) | O(n * m) |
| 实现复杂性 | 中等 | 高(无HashMap库) | 中等 |
| LeetCode状态 | ✓ 接受 | ✓ 接受(略快) | ✓ 接受 |
| 代码清晰度 | 非常清晰 | 复杂字符串操作 | 非常清晰 |
| 最适用于 | 面试 | 生产环境 | 生产环境 |

## 基线方法与推荐方法对比

### 基线方法 (字符计数)
```
对于每个字符串:
  1. 计数每个字符的频率
  2. 构建签名字符串，如 "1a2b3c"
  3. 使用签名作为哈希键

理论上更快 (O(n*m) vs O(n*m log m))
但实际上，字符计数有常数开销
签名字符串构建增加复杂性
```

### 推荐方法 (排序字符串)
```
对于每个字符串:
  1. 对字符进行排序以获得规范形式
  2. 与原始字符串配对
  3. 按规范形式对所有配对进行排序
  4. 将具有相同规范形式的连续配对分组

更直观且易于理解
排序开销相对于输入大小可忽略
```

## 为什么实现符合推荐方法

1. **清晰性**: 排序字符串方法是立即显而易见的 - 异位词排序后有相同的字符。

2. **正确性**: 规范形式生成中无边界情况。排序是确定性的。

3. **效率**: 对于给定的约束(strs.length ≤ 10^4，最大长度100)，O(n * m log m) 是完全可接受的：
   - n * m log m = 10^4 * 100 * log(100) ≈ 10^4 * 100 * 7 = 7 × 10^6 操作
   - 字符计数仍需字符串构建开销

4. **简洁性**: C 没有内置 HashMap，所以两种方法都需要仔细实现。使用 qsort 更清晰。

## C特定的实现细节

该实现通过以下方式处理C缺乏本机HashMap的问题:
1. 使用 `qsort` 对(字符, 原始字符串)配对进行排序
2. 将连续相同的排序字符串分组
3. 在遍历过程中即时构建结果数组

这实际上比在没有适当哈希表的情况下尝试构建字符频率签名字符串更简单。

## 实现中的关键洞察

```c
// 按第一个元素(排序字符串)对配对排序
qsort(pairs, strsSize, sizeof(char**), compare_pairs);

// 迭代并将连续相同的排序字符串分组
for each pair in pairs:
    if sorted_string != previous_sorted_string:
        start_new_group()
    else:
        add_to_current_group()
```

这优雅地处理分组，无需显式哈希表。

## 处理的边界情况

| 情况 | 输入 | 输出 | 解释 |
|------|------|------|------|
| 空字符串 | [""] | [[""]] | 一个包含空字符串的组 |
| 单字符 | ["a"] | [["a"]] | 一个包含一个字符的组 |
| 全部相同 | ["aaa","aaa"] | [["aaa","aaa"]] | 全部在一个组中 |
| 全不相同 | ["abc","def"] | [["abc"],["def"]] | 每个在单独的组中 |
| 混合 | ["ab","ba"] | [["ab","ba"]] | 异位词分组在一起 |
