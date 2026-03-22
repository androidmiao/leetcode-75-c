# 424. Longest Repeating Character Replacement - Editorial

## Approach 1: Sliding Window (Recommended)

### Algorithm

The key insight is that we want to find the longest window where we can make all characters the same by changing at most `k` characters.

For any window, the number of characters we need to change is: `window_length - max_frequency_in_window`

If this value is ≤ `k`, the window is valid.

**Steps:**
1. Use two pointers `left` and `right` to maintain a sliding window
2. Expand the window by moving `right` and update character frequencies
3. Track the maximum frequency character in the current window
4. When `window_length - max_frequency > k`, shrink the window from the left
5. Keep track of the maximum window size seen

### Complexity Analysis

- **Time Complexity**: O(n), where n is the length of the string. Each character is visited at most twice (once by right pointer, once by left pointer).
- **Space Complexity**: O(1), as we only use a fixed-size array of 26 letters.

### Code

```c
int characterReplacement(char* s, int k) {
    int freq[26] = {0};
    int left = 0;
    int max_freq = 0;
    int max_len = 0;

    for (int right = 0; s[right] != '\0'; right++) {
        freq[s[right] - 'A']++;

        if (freq[s[right] - 'A'] > max_freq) {
            max_freq = freq[s[right] - 'A'];
        }

        int window_len = right - left + 1;
        if (window_len - max_freq > k) {
            freq[s[left] - 'A']--;
            left++;
        }

        max_len = (right - left + 1 > max_len) ? (right - left + 1) : max_len;
    }

    return max_len;
}
```

## Approach 2: Brute Force

### Algorithm

For each possible substring, check if it can be made into repeating characters with at most `k` changes.

**Steps:**
1. For each starting position `i`:
   - For each ending position `j` (from `i` to end):
     - Count frequencies of all characters in `s[i:j+1]`
     - Find the max frequency
     - Check if `length - max_frequency <= k`
     - Update the maximum length found

### Complexity Analysis

- **Time Complexity**: O(n² × 26) = O(n²), where we check all substrings and count frequencies in each.
- **Space Complexity**: O(1), only using a 26-element frequency array.

### Code

```c
int characterReplacement(char* s, int k) {
    int max_len = 0;
    int len = strlen(s);

    for (int i = 0; i < len; i++) {
        int freq[26] = {0};
        int max_freq = 0;

        for (int j = i; j < len; j++) {
            freq[s[j] - 'A']++;

            if (freq[s[j] - 'A'] > max_freq) {
                max_freq = freq[s[j] - 'A'];
            }

            int window_len = j - i + 1;
            if (window_len - max_freq <= k) {
                max_len = (window_len > max_len) ? window_len : max_len;
            }
        }
    }

    return max_len;
}
```

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Sliding Window | O(n) | O(1) | Optimal time complexity | Requires careful window management |
| Brute Force | O(n²) | O(1) | Simple logic | Too slow for large inputs |

---

# 424. 最长重复字符替换 - 编辑社论

## 方法1: 滑动窗口 (推荐)

### 算法

核心思想是找到最长的窗口，使得我们可以通过改变最多 `k` 个字符来使所有字符相同。

对于任意窗口，需要改变的字符数 = `窗口长度 - 窗口内最大频率字符数`

如果这个值 ≤ `k`，则窗口是有效的。

**步骤：**
1. 使用左右指针维护滑动窗口
2. 右指针扩展窗口并更新字符频率
3. 追踪当前窗口内最大频率的字符
4. 当 `窗口长度 - 最大频率 > k` 时，从左边收缩窗口
5. 记录看到的最大窗口大小

### 复杂度分析

- **时间复杂度**: O(n)，其中 n 是字符串长度。每个字符最多被访问两次。
- **空间复杂度**: O(1)，仅使用固定大小的26个字母数组。

## 方法2: 暴力法

### 算法

对于每个可能的子字符串，检查是否可以通过最多 k 次改变变成重复字符。

**步骤：**
1. 对每个起始位置 `i`：
   - 对每个结束位置 `j` (从 `i` 到结束)：
     - 计数 `s[i:j+1]` 中所有字符的频率
     - 找到最大频率
     - 检查 `长度 - 最大频率 <= k`
     - 更新找到的最大长度

### 复杂度分析

- **时间复杂度**: O(n²)，检查所有子字符串并计数频率。
- **空间复杂度**: O(1)，仅使用26元素的频率数组。

## 比较

| 方法 | 时间 | 空间 | 优点 | 缺点 |
|------|------|------|------|------|
| 滑动窗口 | O(n) | O(1) | 最优时间复杂度 | 需要仔细管理窗口 |
| 暴力法 | O(n²) | O(1) | 逻辑简单 | 大输入时太慢 |
