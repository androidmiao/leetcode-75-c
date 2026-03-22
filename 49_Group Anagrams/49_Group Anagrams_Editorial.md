# 49. Group Anagrams - Editorial

## Approach 1: Sorted String as Key (Recommended)

### Algorithm

The key insight is that if we sort the characters in each string, all anagrams will produce the same sorted string. We can use this sorted string as a key to group anagrams together.

**Steps:**
1. Create a hash map where keys are sorted strings and values are lists of original strings
2. For each string in the input:
   - Sort the string to get a canonical form
   - Use this sorted string as a key
   - Add the original string to the list of values for this key
3. Return all the lists of grouped anagrams

### Complexity Analysis

- **Time Complexity**: O(n * m log m) where n is the number of strings and m is the maximum length of a string. Sorting each string takes O(m log m).
- **Space Complexity**: O(n * m) to store all the strings and their sorted copies in the hash map.

### Pseudocode

```
map = {}
for each string s in strs:
    sorted_s = sort(s)
    if sorted_s not in map:
        map[sorted_s] = []
    map[sorted_s].append(s)

return values of map
```

### Code (Conceptual - In C, we use qsort approach)

```c
// Sort pairs of (sorted_string, original_string)
// Then group consecutive identical sorted strings
for i = 0 to n-1:
    sorted[i] = sort(strs[i])
    pair[i] = (sorted[i], strs[i])

sort(pair) by sorted_string

// Group consecutive pairs with same sorted string
result = []
current_group = []
for i = 0 to n-1:
    if i == 0 or pair[i].sorted != pair[i-1].sorted:
        if current_group is not empty:
            result.append(current_group)
        current_group = [pair[i].original]
    else:
        current_group.append(pair[i].original)
result.append(current_group)

return result
```

## Approach 2: Character Frequency Count

### Algorithm

Instead of sorting, we can count the frequency of each character and use the frequency signature as a key.

**Steps:**
1. For each string, count the frequency of each character
2. Create a string representation of the frequency (e.g., "1a2b3c..." for "abc abc abc")
3. Use this frequency signature as a key in a hash map
4. Group strings with the same frequency signature

### Complexity Analysis

- **Time Complexity**: O(n * m) where n is the number of strings and m is the maximum length. Counting frequencies is O(m) per string.
- **Space Complexity**: O(n * m) for storing all strings and their frequency signatures.

### Code Concept

```
map = {}
for each string s in strs:
    freq = count_frequencies(s)
    freq_signature = create_signature(freq)  // e.g., "1a1b1c"
    if freq_signature not in map:
        map[freq_signature] = []
    map[freq_signature].append(s)

return values of map
```

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Sorted String | O(n * m log m) | O(n * m) | Simple, intuitive | Sorting overhead |
| Character Frequency | O(n * m) | O(n * m) | Slightly faster | More complex implementation |

---

# 49. 字母异位词分组 - 编辑社论

## 方法1: 排序字符串作为键 (推荐)

### 算法

关键洞察是：如果我们对每个字符串的字符进行排序，所有异位词都会产生相同的排序字符串。我们可以将这个排序字符串用作键来分组异位词。

**步骤：**
1. 创建一个哈希表，其中键是排序后的字符串，值是原始字符串列表
2. 对于输入中的每个字符串：
   - 对字符串进行排序以获得规范形式
   - 使用此排序字符串作为键
   - 将原始字符串添加到此键对应的值列表中
3. 返回所有分组的异位词列表

### 复杂度分析

- **时间复杂度**: O(n * m log m)，其中 n 是字符串个数，m 是最长字符串长度。排序每个字符串需要 O(m log m)。
- **空间复杂度**: O(n * m) 以存储哈希表中的所有字符串及其排序副本。

## 方法2: 字符频率计数

### 算法

我们可以计数每个字符的出现次数，并使用频率签名作为键。

**步骤：**
1. 对于每个字符串，计数每个字符的频率
2. 创建频率的字符串表示形式(例如 "1a2b3c..." 表示"abc abc abc")
3. 使用此频率签名作为哈希表中的键
4. 将具有相同频率签名的字符串分组

### 复杂度分析

- **时间复杂度**: O(n * m)，其中 n 是字符串个数，m 是最长字符串长度。计数频率是 O(m)。
- **空间复杂度**: O(n * m) 用于存储所有字符串及其频率签名。

## 比较

| 方法 | 时间 | 空间 | 优点 | 缺点 |
|------|------|------|------|------|
| 排序字符串 | O(n * m log m) | O(n * m) | 简单直观 | 排序开销 |
| 字符频率 | O(n * m) | O(n * m) | 略快 | 实现更复杂 |
