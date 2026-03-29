# 49. Group Anagrams - Editorial

*Content synced from the live authenticated LeetCode Editorial page.*

## Approach 1: Categorize by Sorted String

### Intuition

Two strings are anagrams if and only if their sorted strings are equal.

### Algorithm

Maintain a map `ans: {String -> List}` where each key K is a sorted string, and each value is the list of strings from the initial input that when sorted, are equal to K.

In Java, we store the key as a string, e.g. `code`. In Python, we store the key as a hashable tuple, e.g. `('c', 'o', 'd', 'e')`.

**Example:**
```
strs = ["are", "bat", "ear", "code", "tab", "era"]

Java:  ans = {"aer": ["are", "ear", "era"], "abt": ["bat", "tab"], "cdeo": ["code"]}
Python: ans = {('a','e','r'): ["are","ear","era"], ('a','b','t'): ["bat","tab"], ('c','d','e','o'): ["code"]}
```

### Complexity Analysis

- **Time Complexity**: O(NK log K), where N is the length of `strs`, and K is the maximum length of a string in `strs`. The outer loop has complexity O(N) as we iterate through each string. Then, we sort each string in O(K log K) time.
- **Space Complexity**: O(NK), the total information content stored in `ans`.

## Approach 2: Categorize by Count

### Intuition

Two strings are anagrams if and only if their character counts (respective number of occurrences of each character) are the same.

### Algorithm

We can transform each string s into a character count, `count`, consisting of 26 non-negative integers representing the number of a's, b's, c's, etc. We use these counts as the basis for our hash map.

**Example:**
```
strs = ["aab", "aba", "baa", "abbccc"]

Java:  ans = {"#2#1#0#0...#0": ["aab", "aba", "baa"], "#1#2#3#0#0#0...#0": ["abbccc"]}
                   (26 total entries)
Python: ans = {(2, 1, 0, 0, ..., 0): ["aab", "aba", "baa"], (1, 2, 3, 0, 0, ..., 0): ["abbccc"]}
                   (26 total entries)
```

### Complexity Analysis

- **Time Complexity**: O(NK), where N is the length of `strs`, and K is the maximum length of a string in `strs`. Counting each string is linear in the size of the string, and we count every string.
- **Space Complexity**: O(NK), the total information content stored in `ans`.

## Comparison

| Approach | Time | Space | Key Idea |
|----------|------|-------|----------|
| Sorted String | O(NK log K) | O(NK) | Sort each string as canonical key |
| Character Count | O(NK) | O(NK) | Use 26-int frequency vector as key |

---

# 49. 異位詞分組 - 編輯社論

*內容取自 LeetCode 官方 Editorial 頁面（已登入之認證瀏覽器工作階段）。*

## 方法一：依排序字串分類（Categorize by Sorted String）

### 直覺

兩個字串是異位詞，當且僅當它們排序後的字串相等。

### 演算法

維護一個映射 `ans: {String -> List}`，其中每個鍵 K 是排序後的字串，每個值是初始輸入中排序後等於 K 的所有字串列表。

在 Java 中，我們將鍵儲存為字串，例如 `code`。在 Python 中，我們將鍵儲存為可雜湊的元組，例如 `('c', 'o', 'd', 'e')`。

**範例：**
```
strs = ["are", "bat", "ear", "code", "tab", "era"]

Java:  ans = {"aer": ["are", "ear", "era"], "abt": ["bat", "tab"], "cdeo": ["code"]}
Python: ans = {('a','e','r'): ["are","ear","era"], ('a','b','t'): ["bat","tab"], ('c','d','e','o'): ["code"]}
```

### 複雜度分析

- **時間複雜度**：O(NK log K)，其中 N 是 `strs` 的長度，K 是 `strs` 中最長字串的長度。外層迴圈複雜度為 O(N)，每個字串排序需要 O(K log K)。
- **空間複雜度**：O(NK)，即 `ans` 中儲存的全部資訊量。

## 方法二：依字元計數分類（Categorize by Count）

### 直覺

兩個字串是異位詞，當且僅當它們的字元計數（每個字元各自出現的次數）相同。

### 演算法

我們可以將每個字串 s 轉換為一個字元計數 `count`，由 26 個非負整數組成，分別代表 a、b、c 等字母的出現次數。我們以這些計數作為雜湊映射的鍵。

**範例：**
```
strs = ["aab", "aba", "baa", "abbccc"]

Java:  ans = {"#2#1#0#0...#0": ["aab", "aba", "baa"], "#1#2#3#0#0#0...#0": ["abbccc"]}
                   （共 26 項）
Python: ans = {(2, 1, 0, 0, ..., 0): ["aab", "aba", "baa"], (1, 2, 3, 0, 0, ..., 0): ["abbccc"]}
                   （共 26 項）
```

### 複雜度分析

- **時間複雜度**：O(NK)，其中 N 是 `strs` 的長度，K 是 `strs` 中最長字串的長度。計數每個字串與字串大小成線性關係，且我們對每個字串都要計數。
- **空間複雜度**：O(NK)，即 `ans` 中儲存的全部資訊量。

## 比較

| 方法 | 時間 | 空間 | 核心思想 |
|------|------|------|----------|
| 排序字串 | O(NK log K) | O(NK) | 排序每個字串作為標準鍵 |
| 字元計數 | O(NK) | O(NK) | 使用 26 個整數的頻率向量作為鍵 |
