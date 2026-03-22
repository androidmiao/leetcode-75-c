# 242. Valid Anagram - Editorial

## Approach 1: Character Frequency Count (Recommended)

### Algorithm

Count the frequency of each character in both strings. Two strings are anagrams if and only if they have the same character frequencies.

**Steps:**
1. Check if the strings have the same length (anagrams must have equal length)
2. Create a frequency array for 26 lowercase English letters
3. Traverse string `s` and increment the frequency count for each character
4. Traverse string `t` and decrement the frequency count for each character
5. Check if all frequency counts are zero
6. If all counts are zero, return true; otherwise return false

### Complexity Analysis

- **Time Complexity**: O(n), where n is the length of the strings. We traverse both strings once and check the frequency array once.
- **Space Complexity**: O(1), as the frequency array has a fixed size of 26.

### Pseudocode

```
if len(s) != len(t):
    return false

freq = array of 26 zeros

for each char c in s:
    freq[c - 'a'] += 1

for each char c in t:
    freq[c - 'a'] -= 1

for i in 0 to 25:
    if freq[i] != 0:
        return false

return true
```

## Approach 2: Sorting

### Algorithm

Sort both strings. If they are anagrams, the sorted strings will be identical.

**Steps:**
1. Convert both strings to character arrays
2. Sort both arrays
3. Compare the sorted arrays
4. If they are equal, return true; otherwise return false

### Complexity Analysis

- **Time Complexity**: O(n log n), dominated by the sorting operation.
- **Space Complexity**: O(n) for the sorted arrays (or O(log n) if we count only the recursion stack for in-place sorting).

### Code Concept

```c
char* sorted_s = strdup(s);
char* sorted_t = strdup(t);

qsort(sorted_s, strlen(sorted_s), sizeof(char), compare);
qsort(sorted_t, strlen(sorted_t), sizeof(char), compare);

bool result = strcmp(sorted_s, sorted_t) == 0;

free(sorted_s);
free(sorted_t);
return result;
```

## Approach 3: Hash Map (Alternative)

### Algorithm

Use a hash map (hash table) to count character frequencies.

**Steps:**
1. Check if lengths are equal
2. Create a hash map
3. For each character in `s`, increment its count
4. For each character in `t`, decrement its count
5. Check if all counts are zero

### Complexity Analysis

- **Time Complexity**: O(n), same as frequency count approach.
- **Space Complexity**: O(k), where k is the number of distinct characters (at most 26 for lowercase English).

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Frequency Count | O(n) | O(1) | Simple, optimal | Need 26-size array |
| Sorting | O(n log n) | O(n) | No extra data structure | Slower |
| Hash Map | O(n) | O(k) | Flexible for any charset | Extra overhead |

---

# 242. 有效的异位词 - 编辑社论

## 方法1: 字符频率计数 (推荐)

### 算法

计算两个字符串中每个字符的频率。两个字符串是异位词当且仅当它们具有相同的字符频率。

**步骤：**
1. 检查字符串的长度是否相同(异位词必须等长)
2. 创建26个小写英文字母的频率数组
3. 遍历字符串 `s`，增加每个字符的频率计数
4. 遍历字符串 `t`，减少每个字符的频率计数
5. 检查所有频率计数是否都为零
6. 如果所有计数都为零，返回 true；否则返回 false

### 复杂度分析

- **时间复杂度**: O(n)，其中 n 是字符串长度。遍历两个字符串一次，检查频率数组一次。
- **空间复杂度**: O(1)，频率数组的大小固定为26。

## 方法2: 排序

### 算法

对两个字符串进行排序。如果它们是异位词，排序后的字符串将相同。

**步骤：**
1. 将两个字符串转换为字符数组
2. 对两个数组进行排序
3. 比较排序后的数组
4. 如果相等，返回 true；否则返回 false

### 复杂度分析

- **时间复杂度**: O(n log n)，由排序操作主导。
- **空间复杂度**: O(n) 用于排序数组(或 O(log n) 如果只计递归栈)。

## 方法3: 哈希表

### 算法

使用哈希表计数字符频率。

**步骤：**
1. 检查长度是否相等
2. 创建哈希表
3. 对于 `s` 中的每个字符，增加其计数
4. 对于 `t` 中的每个字符，减少其计数
5. 检查所有计数是否都为零

### 复杂度分析

- **时间复杂度**: O(n)，与频率计数方法相同。
- **空间复杂度**: O(k)，其中 k 是不同字符的个数(最多26个小写英文字母)。

## 比较

| 方法 | 时间 | 空间 | 优点 | 缺点 |
|------|------|------|------|------|
| 字符频率计数 | O(n) | O(1) | 简单，最优 | 需要26元素数组 |
| 排序 | O(n log n) | O(n) | 无额外数据结构 | 较慢 |
| 哈希表 | O(n) | O(k) | 灵活适应任何字符集 | 额外开销 |
