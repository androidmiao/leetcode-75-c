# 151. Reverse Words in a String — Editorial

> Source: LeetCode Official Editorial (live authenticated session)

## Overview

The task of reversing words in a string is a classic problem that showcases different algorithmic approaches and trade-offs between simplicity and space efficiency. Different interviewers expect different solutions—some appreciate the elegance of built-in methods, while others want to see how you can solve this with minimal extra space.

This editorial presents three approaches, each with linear time complexity but varying space requirements:

1. **Built-in Split and Reverse**: The simplest and most readable approach using language built-ins
2. **Reverse the Whole String, Then Reverse Each Word**: An optimized approach that can achieve O(1) space for mutable strings
3. **Deque-Based Approach**: A middle-ground solution that processes words in reverse order while handling spaces elegantly

---

## Approach 1: Built-in Split + Reverse

### Intuition

The most straightforward approach leverages the language's built-in string processing capabilities. The core idea is:
- Split the string by spaces to extract individual words
- Reverse the resulting list of words
- Join them back together with single spaces

This approach is clean, concise, and easy to understand. While it uses extra space proportional to the input size, it's often the preferred solution in real-world applications due to its readability and maintainability.

### Algorithm

1. Split the input string by whitespace (handling multiple consecutive spaces automatically)
2. Reverse the list of words using the language's built-in reverse function
3. Join the reversed list with single spaces to reconstruct the result string

**Pseudocode:**
```
function reverseWords(s):
    words = split(s) by whitespace
    reverse(words)
    return join(words, ' ')
```

### Complexity Analysis

- **Time Complexity**: O(N), where N is the length of the input string. We traverse the string once to split it, once to reverse it, and once to join it.
- **Space Complexity**: O(N) for storing the split words and the result string. This is unavoidable since the output itself is O(N) in size.

---

## Approach 2: Reverse the Whole String and Then Reverse Each Word

### Intuition

This approach is more space-efficient for mutable strings (like character arrays in C or C++). The key insight is:
- Reversing the entire string puts all words in reverse order, but each word itself is also reversed
- By reversing each individual word after reversing the entire string, we get the correct result
- Additionally, we can trim spaces during the reversal process to achieve O(1) space

The algorithm:
1. First pass: Reverse the entire string to invert word order
2. Second pass: Reverse each individual word while compacting extra spaces
3. Final step: Trim trailing characters to get the final result

This approach is particularly elegant for languages where strings are mutable (like character arrays in C).

### Algorithm

**For mutable strings (e.g., C with character arrays):**

1. **Reverse the whole string**: Use two pointers to reverse all characters
2. **Reverse each word individually**:
   - Iterate through the reversed string
   - Identify word boundaries
   - Reverse each word in place
   - Track an output index `idx` to compact the result (remove extra spaces)
3. **Trim and finalize**:
   - Erase characters beyond the final output index to remove trailing spaces
   - Return the compacted result

**Step-by-step example:**
- Input: `"  hello   world  "`
- After reversing whole string: `"  dlrow   olleh  "`
- After reversing each word (with compacting): `"world hello"`
- Final result: `"world hello"`

**Pseudocode:**
```
function reverseWords(s):
    // Reverse the entire string
    reverse(s, 0, len(s) - 1)

    // Reverse each word and compact spaces
    idx = 0
    for i = 0 to len(s) - 1:
        if s[i] != ' ':
            // Add space before word (if not first word)
            if idx > 0:
                s[idx] = ' '
                idx++

            // Reverse the word
            j = i
            while j < len(s) and s[j] != ' ':
                j++
            reverse(s, i, j - 1)

            // Move characters to output position
            while i < j:
                s[idx] = s[i]
                idx++
                i++
            i--  // Adjust for loop increment

    // Trim to final length
    return s[0:idx]
```

### Complexity Analysis

- **Time Complexity**: O(N), where N is the length of the input string. We perform two passes through the string: one full reversal and one to reverse words and compact spaces.
- **Space Complexity**: O(1) for mutable strings, as we only use a constant amount of extra space for pointers and indices. (For immutable strings, conversion to mutable structure requires O(N) space.)

---

## Approach 3: Deque of Words

### Intuition

This approach uses a deque (double-ended queue) to elegantly handle the word reversal. The key insight is:
- Instead of reversing the entire string, we can iterate through words and push each word to the **front** of the deque
- This naturally reverses the order of words
- Finally, we convert the deque back to a string

This approach separates the concerns of parsing and ordering, making it intuitive and readable. It handles multiple spaces naturally without explicit trimming logic.

### Algorithm

1. **Trim and initialize**: Create an empty deque to store words
2. **Parse words**: Iterate through the string character by character
   - Skip leading spaces
   - Accumulate characters to form words
   - When a space is encountered (after a word), push the word to the front of the deque
3. **Handle trailing word**: Don't forget the last word after the loop
4. **Join result**: Convert the deque back to a space-separated string

**Pseudocode:**
```
function reverseWords(s):
    deque = new Deque()
    word = ""

    for each character c in s:
        if c == ' ':
            if word is not empty:
                deque.pushFront(word)
                word = ""
        else:
            word += c

    // Add the last word if it exists
    if word is not empty:
        deque.pushFront(word)

    // Join deque elements with spaces
    return join(deque, ' ')
```

**Step-by-step example:**
- Input: `"the quick brown fox"`
- Process 'the': deque = ["the"]
- Process 'quick': deque = ["quick", "the"]
- Process 'brown': deque = ["brown", "quick", "the"]
- Process 'fox': deque = ["fox", "brown", "quick", "the"]
- Result: `"fox brown quick the"`

### Complexity Analysis

- **Time Complexity**: O(N), where N is the length of the input string. We traverse the string once to extract words and perform deque operations.
- **Space Complexity**: O(N) for storing all words in the deque and the result string. This is unavoidable since the output itself is O(N) in size.

---

## Summary and Recommendations

| Approach | Time | Space | Advantages | Disadvantages |
|----------|------|-------|------------|---------------|
| **Split + Reverse** | O(N) | O(N) | Simplest, most readable, language-idiomatic | Uses extra space proportional to input |
| **Reverse String then Words** | O(N) | O(1)* | Most space-efficient for mutable strings | More complex logic, requires mutable structure |
| **Deque of Words** | O(N) | O(N) | Clear separation of parsing and ordering | Uses extra space like Approach 1 |

*O(1) space for mutable strings; O(N) for immutable strings after conversion

**Recommendation:**
- Use **Approach 1** in production code for clarity and maintainability
- Use **Approach 2** if space optimization is critical and your language supports mutable strings
- Use **Approach 3** as a middle ground when you want readable code with moderate space complexity

---

# 繁體中文翻譯

# 151. 反轉字符串中的單詞 — 編輯詳解

> 來源：LeetCode 官方編輯詳解（實時已認證會話）

## 概述

在字符串中反轉單詞是一個經典問題，展示了不同的算法方法以及簡潔性和空間效率之間的取捨。不同的面試官期望不同的解決方案——有些人欣賞使用內置方法的優雅性，而其他人則希望看到如何以最少額外空間解決此問題。

本編輯詳解介紹了三種方法，每種方法都具有線性時間複雜度，但空間要求不同：

1. **內置分割和反轉**：使用語言內置功能的最簡單、最易讀的方法
2. **先反轉整個字符串，再反轉每個單詞**：一種優化方法，可對可變字符串實現 O(1) 空間
3. **基於雙端隊列的方法**：一種折中方案，在反向處理單詞時優雅地處理空格

---

## 方法 1：內置分割 + 反轉

### 直觀理解

最直接的方法利用語言的內置字符串處理功能。核心思想是：
- 按空格分割字符串以提取單個單詞
- 反轉得到的單詞列表
- 用單個空格將它們重新連接起來

這種方法簡潔易懂，代碼清晰。雖然它使用與輸入大小成比例的額外空間，但由於其可讀性和可維護性，它通常是實際應用中的首選解決方案。

### 算法

1. 按空格分割輸入字符串（自動處理多個連續空格）
2. 使用語言的內置反轉函數反轉單詞列表
3. 用單個空格連接反轉後的列表以重構結果字符串

**偽代碼：**
```
function reverseWords(s):
    words = 按空格分割(s)
    反轉(words)
    return 連接(words, ' ')
```

### 複雜性分析

- **時間複雜度**：O(N)，其中 N 是輸入字符串的長度。我們遍歷字符串一次來分割它，一次來反轉它，一次來連接它。
- **空間複雜度**：O(N)，用於存儲分割後的單詞和結果字符串。由於輸出本身的大小就是 O(N)，這是不可避免的。

---

## 方法 2：先反轉整個字符串，再反轉每個單詞

### 直觀理解

此方法對可變字符串（如 C 或 C++ 中的字符數組）更節省空間。關鍵洞察是：
- 反轉整個字符串會反轉所有單詞的順序，但每個單詞本身也被反轉了
- 通過在反轉整個字符串後再反轉每個單詞，我們得到正確的結果
- 此外，我們可以在反轉過程中修剪空格，以實現 O(1) 的空間複雜度

算法思路：
1. 第一次遍歷：反轉整個字符串以反轉單詞順序
2. 第二次遍歷：在反轉每個單詞的同時壓縮多余空格
3. 最後步驟：修剪尾部字符以獲得最終結果

此方法對於字符串可變的語言（如 C 中的字符數組）特別優雅。

### 算法

**對於可變字符串（例如 C 的字符數組）：**

1. **反轉整個字符串**：使用雙指針反轉所有字符
2. **分別反轉每個單詞**：
   - 遍歷反轉後的字符串
   - 識別單詞邊界
   - 原地反轉每個單詞
   - 跟蹤輸出索引 `idx` 以壓縮結果（移除多余空格）
3. **修剪並完成**：
   - 擦除最終輸出索引之外的字符以移除尾部空格
   - 返回壓縮後的結果

**逐步示例：**
- 輸入：`"  hello   world  "`
- 反轉整個字符串後：`"  dlrow   olleh  "`
- 反轉每個單詞（壓縮空格）後：`"world hello"`
- 最終結果：`"world hello"`

**偽代碼：**
```
function reverseWords(s):
    // 反轉整個字符串
    反轉(s, 0, len(s) - 1)

    // 反轉每個單詞並壓縮空格
    idx = 0
    for i = 0 to len(s) - 1:
        if s[i] != ' ':
            // 在單詞前添加空格（如果不是第一個單詞）
            if idx > 0:
                s[idx] = ' '
                idx++

            // 反轉該單詞
            j = i
            while j < len(s) and s[j] != ' ':
                j++
            反轉(s, i, j - 1)

            // 移動字符到輸出位置
            while i < j:
                s[idx] = s[i]
                idx++
                i++
            i--  // 調整循環增量

    // 修剪到最終長度
    return s[0:idx]
```

### 複雜性分析

- **時間複雜度**：O(N)，其中 N 是輸入字符串的長度。我們對字符串進行兩次遍歷：一次完整反轉，一次反轉單詞和壓縮空格。
- **空間複雜度**：O(1)，用於可變字符串，因為我們只使用常數個額外空間來存儲指針和索引。（對於不可變字符串，轉換為可變結構需要 O(N) 空間。）

---

## 方法 3：雙端隊列處理單詞

### 直觀理解

此方法使用雙端隊列（deque）來優雅地處理單詞反轉。關鍵洞察是：
- 我們可以遍歷單詞，並將每個單詞推送到雙端隊列的**前端**，而不是反轉整個字符串
- 這自然地反轉了單詞的順序
- 最後，我們將雙端隊列轉換回字符串

此方法分離了解析和排序的關注點，使其直觀易讀。它自然地處理多個空格，無需顯式修剪邏輯。

### 算法

1. **修剪並初始化**：創建一個空雙端隊列來存儲單詞
2. **解析單詞**：逐個字符遍歷字符串
   - 跳過前導空格
   - 累積字符以形成單詞
   - 遇到空格時（在單詞之後），將單詞推送到雙端隊列的前端
3. **處理尾部單詞**：不要忘記循環後的最後一個單詞
4. **連接結果**：將雙端隊列轉換回空格分隔的字符串

**偽代碼：**
```
function reverseWords(s):
    deque = new 雙端隊列()
    word = ""

    for each character c in s:
        if c == ' ':
            if word 不為空:
                deque.pushFront(word)
                word = ""
        else:
            word += c

    // 如果最後的單詞存在，將其添加
    if word 不為空:
        deque.pushFront(word)

    // 用空格連接雙端隊列元素
    return 連接(deque, ' ')
```

**逐步示例：**
- 輸入：`"the quick brown fox"`
- 處理 'the'：deque = ["the"]
- 處理 'quick'：deque = ["quick", "the"]
- 處理 'brown'：deque = ["brown", "quick", "the"]
- 處理 'fox'：deque = ["fox", "brown", "quick", "the"]
- 結果：`"fox brown quick the"`

### 複雜性分析

- **時間複雜度**：O(N)，其中 N 是輸入字符串的長度。我們遍歷字符串一次以提取單詞並執行雙端隊列操作。
- **空間複雜度**：O(N)，用於在雙端隊列中存儲所有單詞和結果字符串。由於輸出本身的大小就是 O(N)，這是不可避免的。

---

## 總結和建議

| 方法 | 時間 | 空間 | 優點 | 缺點 |
|------|------|------|------|------|
| **分割 + 反轉** | O(N) | O(N) | 最簡單，最易讀，符合語言習慣 | 使用與輸入成比例的額外空間 |
| **反轉字符串再反轉單詞** | O(N) | O(1)* | 對可變字符串最節省空間 | 邏輯更複雜，需要可變結構 |
| **雙端隊列處理單詞** | O(N) | O(N) | 清晰的解析和排序分離 | 使用與方法 1 相同的額外空間 |

*O(1) 空間用於可變字符串；O(N) 用於轉換為可變結構後的不可變字符串

**建議：**
- 在生產代碼中使用**方法 1** 以獲得清晰度和可維護性
- 如果空間優化至關重要且語言支持可變字符串，請使用**方法 2**
- 當您希望代碼具有可讀性和適中空間複雜度時，使用**方法 3** 作為折中方案
