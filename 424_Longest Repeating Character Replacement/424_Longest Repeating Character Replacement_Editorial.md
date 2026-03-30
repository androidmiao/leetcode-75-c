# 424. Longest Repeating Character Replacement - Editorial

## Overview

Given a string `s` and an integer `k`, find the length of the longest substring where you can make all characters the same by performing at most `k` character replacements.

**Example:**
- Input: `s = "ABAB"`, `k = 2` → Output: `4` (replace all to 'A')
- Input: `s = "ABABAB"`, `k = 2` → Output: `4` (substring "ABAB")

---

## English Solution

### Approach 1: Sliding Window + Binary Search

#### Intuition
The key observation is that the answer—the maximum length—has a monotonic property: if we can form a valid substring of length `L`, then we can also form one of length `L-1`. This suggests using binary search on the answer.

For a given candidate length `mid`, we check whether there exists a substring of exactly that length where we can make all characters identical using at most `k` replacements. We use a sliding window of fixed size `mid` to perform this check.

#### Algorithm
1. **Binary Search Setup:** Left boundary = 1, right boundary = length of string `s`.
2. **Check Function:** For a candidate length `mid`:
   - Slide a window of size `mid` across the string.
   - Track the frequency of each character in the current window.
   - Find the maximum frequency.
   - If `window_length - max_frequency <= k`, this means we need at most `k` replacements, so the substring is valid.
   - If any window position is valid, return true.
3. **Binary Search:** Adjust the search range based on whether a valid substring of length `mid` exists.

#### Complexity Analysis
- **Time:** O(n log n) – binary search iterations are O(log n), each check is O(n) with O(n) window slides.
- **Space:** O(26) = O(1) – frequency array for at most 26 lowercase letters.

#### C Code
```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool canMakeValidSubstring(const char* s, int length, int mid, int k) {
    int sLen = strlen(s);

    for (int i = 0; i <= sLen - mid; i++) {
        int freq[26] = {0};
        int maxFreq = 0;

        // Count characters in current window [i, i+mid)
        for (int j = i; j < i + mid; j++) {
            freq[s[j] - 'A']++;
            maxFreq = (freq[s[j] - 'A'] > maxFreq) ? freq[s[j] - 'A'] : maxFreq;
        }

        // Check if we need at most k replacements
        if (mid - maxFreq <= k) {
            return true;
        }
    }
    return false;
}

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int left = 1, right = sLen;
    int result = 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (canMakeValidSubstring(s, sLen, mid, k)) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}
```

---

### Approach 2: Sliding Window (Per Character)

#### Intuition
Instead of searching on the answer, we can explicitly check for each unique character whether we can form a valid substring where that character is the one we keep (and replace all others in the window).

For each character, we maintain a sliding window where the number of characters we need to replace (total characters minus count of our target character) does not exceed `k`.

#### Algorithm
1. **For each unique character** in the string:
   - Use two pointers: `left` and `right`.
   - Expand `right` to include more characters.
   - Count the frequency of the target character in [left, right].
   - While `window_size - count_of_target_char > k`, shrink from `left`.
   - Track the maximum window size across all characters.
2. **Return** the maximum length found.

#### Complexity Analysis
- **Time:** O(26n) = O(n) – at most 26 characters, and each position is visited O(n) times (once per character).
- **Space:** O(26) = O(1) – frequency tracking for at most 26 characters.

#### C Code
```c
#include <stdio.h>
#include <string.h>

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int maxLength = 0;

    // For each unique character
    for (char c = 'A'; c <= 'Z'; c++) {
        int left = 0, right = 0;
        int count = 0; // Count of character c in current window

        while (right < sLen) {
            if (s[right] == c) {
                count++;
            }

            // Window size - count of c should be <= k
            int windowSize = right - left + 1;
            if (windowSize - count > k) {
                if (s[left] == c) {
                    count--;
                }
                left++;
            }

            windowSize = right - left + 1;
            if (windowSize > maxLength) {
                maxLength = windowSize;
            }

            right++;
        }
    }

    return maxLength;
}
```

---

### Approach 3: Expanding Sliding Window (Optimal / Recommended)

#### Intuition
The most efficient approach leverages a critical insight: once we find a valid window of length `L`, we never need to examine a smaller window. Instead of shrinking and expanding repeatedly, we can use an expanding window strategy where:

1. The window size is monotonically non-decreasing (it never shrinks below its current size).
2. We track the global maximum frequency of any character seen so far.
3. Key optimization: The maximum frequency variable only increases or stays the same—it never decreases. This is valid because we're looking for the maximum length, and once we've seen a frequency of `f`, we don't care about smaller frequencies.

#### Algorithm
1. **Initialize:** `left = 0`, `right = 0`, `maxFreq = 0`, `result = 0`, and frequency array.
2. **Expand Window:** Move `right` pointer and update the frequency of `s[right]`.
3. **Update Global Max Frequency:** Update `maxFreq` (never decrease it).
4. **Check Validity:** If `window_size - maxFreq > k`:
   - Move `left` forward by 1 (shrink window by 1).
   - The window size shrinks by 1, but `right` also advances by 1 in the next iteration, so the net effect is a "sliding" behavior where window size is maintained or grows.
5. **Update Result:** Track the maximum window size.

#### Complexity Analysis
- **Time:** O(n) – single pass with two pointers, each position visited at most twice.
- **Space:** O(1) – frequency array of fixed size 26.

#### C Code
```c
#include <stdio.h>
#include <string.h>

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int freq[26] = {0};
    int left = 0, maxFreq = 0, result = 0;

    for (int right = 0; right < sLen; right++) {
        // Update frequency of current character
        freq[s[right] - 'A']++;
        maxFreq = (freq[s[right] - 'A'] > maxFreq) ? freq[s[right] - 'A'] : maxFreq;

        // Current window size
        int windowSize = right - left + 1;

        // If window is invalid, shrink from left
        if (windowSize - maxFreq > k) {
            freq[s[left] - 'A']--;
            left++;
        }

        // Update result (window size stays same or grows)
        windowSize = right - left + 1;
        result = (windowSize > result) ? windowSize : result;
    }

    return result;
}
```

---

### Comparison Table

| Approach | Time Complexity | Space Complexity | Advantage | Disadvantage |
|----------|-----------------|------------------|-----------|--------------|
| **Approach 1: Binary Search** | O(n log n) | O(1) | Clear logic, easy to understand | Slower due to log n factor |
| **Approach 2: Per Character** | O(26n) = O(n) | O(1) | Direct character-by-character logic | Requires 26 passes (constant but not elegant) |
| **Approach 3: Expanding Window** | O(n) | O(1) | Optimal, single pass, elegant | Requires understanding max frequency property |

**Recommendation:** Use Approach 3 for interviews and production code. It is the most efficient and elegant solution.

---

## 繁體中文解法

### 方法 1: 滑動視窗 + 二分搜尋

#### 直覺
答案——最大長度——具有單調性：如果我們能形成長度 `L` 的有效子字符串，那麼也能形成長度 `L-1` 的。這提示我們可以對答案進行二分搜尋。

對於候選長度 `mid`，我們檢查是否存在恰好 `mid` 長度的子字符串，其中我們最多使用 `k` 次替換就能使所有字符相同。使用固定大小 `mid` 的滑動視窗進行檢查。

#### 演算法
1. **二分搜尋設置：** 左邊界 = 1，右邊界 = 字符串長度。
2. **檢查函數：** 對於候選長度 `mid`：
   - 在字符串上滑動大小為 `mid` 的視窗。
   - 追蹤當前視窗中每個字符的頻率。
   - 找出最大頻率。
   - 如果 `視窗長度 - 最大頻率 <= k`，則需要最多 `k` 次替換，子字符串有效。
   - 若任何視窗位置有效，返回 true。
3. **二分搜尋：** 根據長度 `mid` 的有效子字符串是否存在調整搜尋範圍。

#### 複雜度分析
- **時間：** O(n log n) – 二分搜尋迭代 O(log n)，每次檢查 O(n)。
- **空間：** O(26) = O(1) – 最多 26 個小寫字母的頻率陣列。

#### C 程式碼
```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool canMakeValidSubstring(const char* s, int length, int mid, int k) {
    int sLen = strlen(s);

    for (int i = 0; i <= sLen - mid; i++) {
        int freq[26] = {0};
        int maxFreq = 0;

        // 計算當前視窗 [i, i+mid) 中的字符
        for (int j = i; j < i + mid; j++) {
            freq[s[j] - 'A']++;
            maxFreq = (freq[s[j] - 'A'] > maxFreq) ? freq[s[j] - 'A'] : maxFreq;
        }

        // 檢查是否需要最多 k 次替換
        if (mid - maxFreq <= k) {
            return true;
        }
    }
    return false;
}

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int left = 1, right = sLen;
    int result = 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (canMakeValidSubstring(s, sLen, mid, k)) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}
```

---

### 方法 2: 滑動視窗（逐字符）

#### 直覺
我們不是搜尋答案，而是明確地為每個唯一字符檢查是否能形成有效子字符串，其中該字符保留而其他字符被替換。

對於每個字符，維護一個滑動視窗，使得需要替換的字符數（視窗中的總字符數減去目標字符計數）不超過 `k`。

#### 演算法
1. **對於每個唯一字符**：
   - 使用兩個指針：`left` 和 `right`。
   - 擴展 `right` 來包含更多字符。
   - 計算 [left, right] 中目標字符的頻率。
   - 當 `視窗大小 - 目標字符計數 > k` 時，從 `left` 收縮。
   - 追蹤所有字符中的最大視窗大小。
2. **返回** 找到的最大長度。

#### 複雜度分析
- **時間：** O(26n) = O(n) – 最多 26 個字符，每個位置被訪問 O(n) 次。
- **空間：** O(26) = O(1) – 最多 26 個字符的頻率追蹤。

#### C 程式碼
```c
#include <stdio.h>
#include <string.h>

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int maxLength = 0;

    // 對於每個唯一字符
    for (char c = 'A'; c <= 'Z'; c++) {
        int left = 0, right = 0;
        int count = 0; // 當前視窗中字符 c 的計數

        while (right < sLen) {
            if (s[right] == c) {
                count++;
            }

            // 視窗大小 - c 的計數應該 <= k
            int windowSize = right - left + 1;
            if (windowSize - count > k) {
                if (s[left] == c) {
                    count--;
                }
                left++;
            }

            windowSize = right - left + 1;
            if (windowSize > maxLength) {
                maxLength = windowSize;
            }

            right++;
        }
    }

    return maxLength;
}
```

---

### 方法 3: 擴展滑動視窗（最優 / 推薦）

#### 直覺
最高效的方法利用一個關鍵洞察：一旦我們找到長度 `L` 的有效視窗，就無需檢查更小的視窗。我們使用擴展視窗策略：

1. 視窗大小單調非遞減（永不縮小到當前大小以下）。
2. 追蹤迄今為止看過的任何字符的全局最大頻率。
3. 關鍵優化：最大頻率變數只增加或保持不變——永不減少。這是有效的，因為我們尋找最大長度，一旦看過頻率 `f`，就不關心更小的頻率。

#### 演算法
1. **初始化：** `left = 0`、`right = 0`、`maxFreq = 0`、`result = 0` 和頻率陣列。
2. **擴展視窗：** 移動 `right` 指針並更新 `s[right]` 的頻率。
3. **更新全局最大頻率：** 更新 `maxFreq`（永不減少）。
4. **檢查有效性：** 如果 `視窗大小 - maxFreq > k`：
   - 將 `left` 前進 1（視窗縮小 1）。
   - 視窗縮小 1，但下一次迭代 `right` 也前進 1，所以淨效果是"滑動"，視窗大小保持或增長。
5. **更新結果：** 追蹤最大視窗大小。

#### 複雜度分析
- **時間：** O(n) – 單次遍歷，兩個指針，每個位置最多訪問兩次。
- **空間：** O(1) – 固定大小 26 的頻率陣列。

#### C 程式碼
```c
#include <stdio.h>
#include <string.h>

int characterReplacement(char* s, int k) {
    int sLen = strlen(s);
    int freq[26] = {0};
    int left = 0, maxFreq = 0, result = 0;

    for (int right = 0; right < sLen; right++) {
        // 更新當前字符的頻率
        freq[s[right] - 'A']++;
        maxFreq = (freq[s[right] - 'A'] > maxFreq) ? freq[s[right] - 'A'] : maxFreq;

        // 當前視窗大小
        int windowSize = right - left + 1;

        // 如果視窗無效，從左側收縮
        if (windowSize - maxFreq > k) {
            freq[s[left] - 'A']--;
            left++;
        }

        // 更新結果（視窗大小保持相同或增長）
        windowSize = right - left + 1;
        result = (windowSize > result) ? windowSize : result;
    }

    return result;
}
```

---

### 比較表格

| 方法 | 時間複雜度 | 空間複雜度 | 優勢 | 劣勢 |
|------|----------|----------|------|------|
| **方法 1: 二分搜尋** | O(n log n) | O(1) | 邏輯清晰，易於理解 | 因 log n 因子速度較慢 |
| **方法 2: 逐字符** | O(26n) = O(n) | O(1) | 直接的字符邏輯 | 需要 26 次遍歷（雖然是常數但不優雅） |
| **方法 3: 擴展視窗** | O(n) | O(1) | 最優，單次遍歷，優雅 | 需要理解最大頻率性質 |

**推薦：** 面試和生產代碼中使用方法 3。它是最高效和最優雅的解法。
