# 242. Valid Anagram - Editorial

## Approach 1: Sorting

### Algorithm

An anagram is produced by rearranging the letters of one string into another. Therefore, if `t` is an anagram of `s`, sorting both strings will result in two identical strings. Furthermore, if `s` and `t` have different lengths, `t` cannot be an anagram of `s` and we can return early.

**Steps:**
1. If lengths differ, return false immediately
2. Convert both strings to character arrays
3. Sort both arrays
4. Compare the sorted arrays — if equal, return true; otherwise return false

### Complexity Analysis

- **Time Complexity**: O(n log n), dominated by the sorting step. Comparing two sorted strings costs O(n).
- **Space Complexity**: O(1) if using heapsort in-place. In Java, `toCharArray()` makes a copy costing O(n) extra space, but this is language-dependent. In C, `qsort` sorts in-place on a duplicated buffer — O(n) for the duplicate.

### Code Concept (C)

```c
int cmp(const void *a, const void *b) {
    return *(char *)a - *(char *)b;
}

bool isAnagram(char *s, char *t) {
    int sn = strlen(s), tn = strlen(t);
    if (sn != tn) return false;

    char *ss = strdup(s);
    char *tt = strdup(t);
    qsort(ss, sn, sizeof(char), cmp);
    qsort(tt, tn, sizeof(char), cmp);
    bool res = strcmp(ss, tt) == 0;
    free(ss);
    free(tt);
    return res;
}
```

## Approach 2: Frequency Counter (Recommended)

### Algorithm

Count occurrences of each letter in the two strings and compare them. Since both `s` and `t` contain only lowercase letters `a`–`z`, a simple array of size 26 will suffice. We can increment the count for each letter in `s` and decrement for each letter in `t`, then check whether every count is zero.

**Variant A — single loop (same-length only):**
```
for i in 0..n-1:
    counter[s[i] - 'a']++
    counter[t[i] - 'a']--
```

**Variant B — two separate loops with early exit:**
Increment counts for `s` first, then decrement for `t`. If at any point a count drops below zero, `t` contains an extra letter not in `s` — return false immediately.

**Steps (Variant B):**
1. Check if `len(s) != len(t)` → return false
2. Create `freq[26] = {0}`
3. For each character in `s`: `freq[c - 'a']++`
4. For each character in `t`: `freq[c - 'a']--`; if `freq[c - 'a'] < 0` → return false
5. Return true

### Complexity Analysis

- **Time Complexity**: O(n). Accessing the counter table is O(1) per character.
- **Space Complexity**: O(1). The counter table has fixed size 26, constant regardless of n.

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

## Follow Up: Unicode Characters

Use a hash table instead of a fixed-size counter. A fixed-size array covering the entire Unicode range (over 1 million code points) would waste too much memory. A hash table adapts to any range of characters dynamically.

## Comparison

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| Sorting | O(n log n) | O(1)–O(n) | No extra data structure | Slower |
| Frequency Counter | O(n) | O(1) | Simple, optimal | Fixed to known charset |
| Hash Map (Unicode) | O(n) | O(k) | Flexible for any charset | Hash overhead |

---

# 242. 有效的異位詞 — 題解

## 方法一：排序

### 演算法

異位詞是透過重新排列一個字串的字母而得到另一個字串。因此，若 `t` 是 `s` 的異位詞，將兩個字串排序後必定產生相同的結果。此外，若 `s` 和 `t` 的長度不同，`t` 不可能是 `s` 的異位詞，可以提前返回。

**步驟：**
1. 若長度不同，立即回傳 false
2. 將兩個字串轉為字元陣列
3. 分別排序
4. 比較排序後的陣列——若相同則回傳 true，否則 false

### 複雜度分析

- **時間複雜度**：O(n log n)，由排序步驟主導。比較兩個已排序字串需 O(n)。
- **空間複雜度**：若使用堆積排序可達 O(1)。在 C 中需先複製字串再排序，故為 O(n)。

## 方法二：頻率計數（推薦）

### 演算法

統計兩個字串中每個字母的出現次數並加以比較。由於 `s` 和 `t` 只包含小寫字母 `a`–`z`，使用大小為 26 的簡單陣列即可。對 `s` 中每個字母遞增計數、對 `t` 中每個字母遞減計數，最後檢查所有計數是否皆為零。

**變體 A — 單迴圈（僅適用等長）：**
在同一迴圈中同時遞增 `s` 的計數與遞減 `t` 的計數。

**變體 B — 兩個獨立迴圈加提前退出：**
先遞增 `s` 的計數，再遞減 `t` 的計數。若某個計數降至零以下，表示 `t` 含有 `s` 中不存在的額外字母，可立即回傳 false。

### 複雜度分析

- **時間複雜度**：O(n)。存取計數陣列為每字元 O(1)。
- **空間複雜度**：O(1)。計數陣列大小固定為 26，不隨 n 變化。

## 進階：Unicode 字元

使用雜湊表取代固定大小的計數陣列。Unicode 字元範圍超過一百萬個碼位，使用固定大小陣列將浪費大量記憶體。雜湊表可動態適應任何字元範圍。

## 比較

| 方法 | 時間 | 空間 | 優點 | 缺點 |
|------|------|------|------|------|
| 排序 | O(n log n) | O(1)–O(n) | 無額外資料結構 | 較慢 |
| 頻率計數 | O(n) | O(1) | 簡單、最優 | 限定已知字元集 |
| 雜湊表（Unicode） | O(n) | O(k) | 適應任意字元集 | 雜湊額外開銷 |
