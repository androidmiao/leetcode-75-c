# 392. Is Subsequence - Editorial

## Problem Statement
Determine whether string `s` is a subsequence of string `t`. A subsequence is a sequence that can be derived from another sequence by deleting some (or no) elements without changing the order of the remaining elements.

---

## Approach 1: Divide and Conquer with Greedy

### Intuition
This approach uses a divide-and-conquer strategy combined with greedy selection. The key insight is that when we encounter matching characters, we should consume them immediately rather than skip them. This is optimal because we only need to find one valid subsequence match.

At each step, we compare the first character of the source string with the first character of the target string:
- **Case 1:** Characters match → consume both and solve the smaller subproblem
- **Case 2:** Characters don't match → skip the target character and try again

The base cases terminate the recursion:
- If source is exhausted (all characters matched) → return true
- If target is exhausted but source still has unmatched characters → return false

### Algorithm
We implement this using indices rather than string slicing:
- `leftIndex` tracks position in source
- `rightIndex` tracks position in target
- `leftBound` = length of source
- `rightBound` = length of target

**Recursive logic:**
```
if leftIndex == leftBound:
    return true  // all source chars matched
if rightIndex == rightBound:
    return false  // target exhausted, source incomplete

if source[leftIndex] == target[rightIndex]:
    return isSubsequence(leftIndex + 1, rightIndex + 1)
else:
    return isSubsequence(leftIndex, rightIndex + 1)
```

**Greedy property:** When characters match, consuming them immediately is always optimal. Deferring the match cannot lead to a better outcome; the only question is whether a complete match exists, not which match is "best."

### Complexity Analysis
- **Time Complexity:** O(|T|) where |T| is the length of target. In worst case, we examine every character of target once.
- **Space Complexity:** O(|T|) due to the recursion call stack depth, which is bounded by target length.

---

## Approach 2: Two-Pointers

### Intuition
We convert the recursive approach into an iterative solution by simulating the recursion logic with two pointers. This eliminates the function call overhead and recursion stack space.

The algorithm is straightforward:
- Advance the source pointer only when characters match
- Always advance the target pointer
- If source pointer reaches the end, all characters were matched in order

### Algorithm
```
leftP = 0  // pointer for source
rightP = 0  // pointer for target

while leftP < len(source) and rightP < len(target):
    if source[leftP] == target[rightP]:
        leftP += 1
    rightP += 1

return leftP == len(source)
```

This directly mirrors the recursive approach:
- The inner loop substitutes the recursion
- The final check validates that all source characters were matched

### Complexity Analysis
- **Time Complexity:** O(|T|) where |T| is the length of target. Each character of target is visited exactly once.
- **Space Complexity:** O(1) constant space, no recursion or auxiliary data structures.

---

## Approach 3: Greedy Match with Character Indices Hashmap

### Intuition
This approach addresses the follow-up question: "What if there are lots of incoming source strings and we want to check multiple times?"

The key optimization is preprocessing: build a hashmap where each character in target maps to a sorted list of all indices where it appears. Then, for each character in source, we use binary search to find the next valid occurrence in target.

This is efficient when:
- Target string is fixed and checked against many source strings
- We want to avoid redundant scanning of target for each query

### Algorithm
**Preprocessing step:**
```
charToIndices = {}
for i, char in enumerate(target):
    if char not in charToIndices:
        charToIndices[char] = []
    charToIndices[char].append(i)
```

**Matching step:**
```
currentPosition = 0
for char in source:
    if char not in charToIndices:
        return false

    indices = charToIndices[char]
    nextIndex = binarySearch(indices, currentPosition)

    if nextIndex == not_found:
        return false  // no valid index greater than currentPosition

    currentPosition = nextIndex + 1
```

The binary search finds the smallest index in the list that is strictly greater than `currentPosition`. This ensures we don't reuse the same index and maintain the subsequence property.

### Complexity Analysis
- **Time Complexity:** O(|T| + |S|·log|T|)
  - O(|T|) to build the hashmap
  - O(|S|·log|T|) for S iterations of binary search on lists of size ≤ |T|
- **Space Complexity:** O(|T|) to store the character-to-indices mapping

**Trade-off:** Higher preprocessing cost but better amortized cost when checking multiple source strings against the same target.

---

## Approach 4: Dynamic Programming

### Intuition
This approach models the problem as finding the longest matching subsequence using dynamic programming, similar to the Edit Distance problem.

We construct a matrix `dp[row][col]` where:
- Rows represent characters of target (0 to |T|)
- Columns represent characters of source (0 to |S|)
- Each cell `dp[row][col]` represents the **maximum number of matched characters** from source[0:col] that can be found as a subsequence in target[0:row]

If any cell equals |S|, we have matched the entire source string.

### Algorithm
**DP table setup:**
- Create a matrix of size (|T| + 1) × (|S| + 1), initialized to 0
- Row 0: always 0 (empty target, no matches possible)
- Column 0: always 0 (empty source, 0 matches needed)

**Recurrence relation:**
```
for row in 1 to |T|:
    for col in 1 to |S|:
        if source[col-1] == target[row-1]:
            dp[row][col] = dp[row-1][col-1] + 1
        else:
            dp[row][col] = dp[row-1][col]
```

**Why this works:**
- If characters match: we increment the match count from the previous state (diagonal)
- If characters don't match: we carry forward the match count from the row above (skip current target character)
- When `dp[row][col] == |S|`, all source characters have been matched

**Return:** Check if any cell equals |S|, or equivalently check `dp[|T|][|S|] == |S|`.

### Complexity Analysis
- **Time Complexity:** O(|S|·|T|) to fill the entire DP table
- **Space Complexity:** O(|S|·|T|) to store the DP table

**Space optimization note:** We only need the previous row to compute the current row, so space can be optimized to O(min(|S|, |T|)).

---

## Comparison Summary

| Approach | Time | Space | Pros | Cons |
|----------|------|-------|------|------|
| 1. Recursion | O(\|T\|) | O(\|T\|) | Intuitive logic | Stack overhead |
| 2. Two-Pointers | O(\|T\|) | O(1) | Optimal for single query | N/A |
| 3. Hashmap + Binary Search | O(\|T\| + \|S\|·log\|T\|) | O(\|T\|) | Best for multiple queries | Preprocessing cost |
| 4. Dynamic Programming | O(\|S\|·\|T\|) | O(\|S\|·\|T\|) | Intuitive matrix building | More space usage |

**Recommendation:** Use Approach 2 (Two-Pointers) for the basic problem due to optimal time and space. Use Approach 3 for the follow-up with multiple source strings.

---

---

---

# 392. 是子序列 - 編輯指南

## 問題陳述
判斷字符串 `s` 是否為字符串 `t` 的子序列。子序列是指從另一個序列派生的序列，通過刪除某些（或不刪除）元素而不改變其餘元素的順序。

---

## 方法 1: 分治法與貪心策略

### 直覺
該方法採用分治思想結合貪心選擇。核心洞察是：當我們遇到匹配的字符時，應該立即消耗它們，而不是跳過。這是最優的，因為我們只需要找到一個有效的子序列匹配。

在每一步，我們比較源字符串的第一個字符與目標字符串的第一個字符：
- **情況 1:** 字符匹配 → 同時消耗兩者並解決較小的子問題
- **情況 2:** 字符不匹配 → 跳過目標字符並重試

遞歸的基本情況終止遞歸：
- 源字符串被消耗完（所有字符都已匹配） → 返回真
- 目標字符串被消耗完但源字符串仍有未匹配的字符 → 返回假

### 演算法
我們使用索引而不是字符串切片來實現：
- `leftIndex` 跟蹤源字符串中的位置
- `rightIndex` 跟蹤目標字符串中的位置
- `leftBound` = 源字符串的長度
- `rightBound` = 目標字符串的長度

**遞歸邏輯：**
```
if leftIndex == leftBound:
    return true  // 所有源字符都已匹配
if rightIndex == rightBound:
    return false  // 目標字符串已用完，源字符串未完全匹配

if source[leftIndex] == target[rightIndex]:
    return isSubsequence(leftIndex + 1, rightIndex + 1)
else:
    return isSubsequence(leftIndex, rightIndex + 1)
```

**貪心性質：** 當字符匹配時，立即消耗它們始終是最優的。延遲匹配無法帶來更好的結果；唯一的問題是是否存在完整匹配，而不是哪個匹配「最好」。

### 複雜度分析
- **時間複雜度：** O(|T|)，其中 |T| 是目標字符串的長度。在最壞情況下，我們檢查目標字符串的每個字符一次。
- **空間複雜度：** O(|T|)，由於遞歸調用棧的深度受目標字符串長度限制。

---

## 方法 2: 雙指針

### 直覺
我們將遞歸方法轉換為迭代解決方案，通過使用兩個指針來模擬遞歸邏輯。這消除了函數調用開銷和遞歸棧空間。

演算法很直接：
- 僅當字符匹配時才推進源指針
- 始終推進目標指針
- 如果源指針到達末尾，則所有字符都按順序匹配

### 演算法
```
leftP = 0  // 源字符串指針
rightP = 0  // 目標字符串指針

while leftP < len(source) and rightP < len(target):
    if source[leftP] == target[rightP]:
        leftP += 1
    rightP += 1

return leftP == len(source)
```

這直接反映遞歸方法：
- 循環代替遞歸
- 最終檢查驗證所有源字符都已匹配

### 複雜度分析
- **時間複雜度：** O(|T|)，其中 |T| 是目標字符串的長度。目標字符串的每個字符恰好被訪問一次。
- **空間複雜度：** O(1) 常數空間，無遞歸或輔助數據結構。

---

## 方法 3: 貪心匹配與字符索引哈希表

### 直覺
該方法處理進階問題：「如果有許多傳入的源字符串，我們想多次檢查怎麼辦？」

關鍵優化是預處理：構建一個哈希表，其中目標字符串中的每個字符映射到它出現位置的排序索引列表。然後，對於源字符串中的每個字符，我們使用二分查找找到目標字符串中的下一個有效出現位置。

此方法適用於：
- 目標字符串固定且需要與多個源字符串檢查
- 我們希望避免對每個查詢重複掃描目標字符串

### 演算法
**預處理步驟：**
```
charToIndices = {}
for i, char in enumerate(target):
    if char not in charToIndices:
        charToIndices[char] = []
    charToIndices[char].append(i)
```

**匹配步驟：**
```
currentPosition = 0
for char in source:
    if char not in charToIndices:
        return false

    indices = charToIndices[char]
    nextIndex = binarySearch(indices, currentPosition)

    if nextIndex == not_found:
        return false  // 找不到大於 currentPosition 的有效索引

    currentPosition = nextIndex + 1
```

二分查找找到列表中第一個嚴格大於 `currentPosition` 的索引。這確保我們不會重複使用同一索引並維持子序列性質。

### 複雜度分析
- **時間複雜度：** O(|T| + |S|·log|T|)
  - O(|T|) 構建哈希表
  - O(|S|·log|T|) 執行 S 次二分查找，每次在大小 ≤ |T| 的列表上
- **空間複雜度：** O(|T|) 存儲字符到索引的映射

**權衡：** 預處理成本更高，但在對同一目標字符串檢查多個源字符串時，攤銷成本更優。

---

## 方法 4: 動態規劃

### 直覺
該方法將問題建模為使用動態規劃找到最長匹配子序列，類似於編輯距離問題。

我們構造一個矩陣 `dp[row][col]`，其中：
- 行代表目標字符串的字符（0 至 |T|）
- 列代表源字符串的字符（0 至 |S|）
- 每個單元格 `dp[row][col]` 表示源字符串 source[0:col] 中可以在目標字符串 target[0:row] 中找到的**最大匹配字符數**

如果任何單元格等於 |S|，我們就已匹配整個源字符串。

### 演算法
**DP 表初始化：**
- 創建大小為 (|T| + 1) × (|S| + 1) 的矩陣，初始化為 0
- 第 0 行：始終為 0（空目標字符串，無可能的匹配）
- 第 0 列：始終為 0（空源字符串，無需匹配）

**遞歸關係：**
```
for row in 1 to |T|:
    for col in 1 to |S|:
        if source[col-1] == target[row-1]:
            dp[row][col] = dp[row-1][col-1] + 1
        else:
            dp[row][col] = dp[row-1][col]
```

**為什麼有效：**
- 如果字符匹配：我們從前一狀態（對角線）增加匹配計數
- 如果字符不匹配：我們從上一行轉移匹配計數（跳過當前目標字符）
- 當 `dp[row][col] == |S|` 時，所有源字符都已匹配

**返回：** 檢查任何單元格是否等於 |S|，或等價地檢查 `dp[|T|][|S|] == |S|`。

### 複雜度分析
- **時間複雜度：** O(|S|·|T|) 填充整個 DP 表
- **空間複雜度：** O(|S|·|T|) 存儲 DP 表

**空間優化提示：** 我們只需要前一行來計算當前行，因此空間可優化至 O(min(|S|, |T|))。

---

## 比較總結

| 方法 | 時間 | 空間 | 優點 | 缺點 |
|------|------|------|------|------|
| 1. 遞歸 | O(\|T\|) | O(\|T\|) | 邏輯直觀 | 棧開銷 |
| 2. 雙指針 | O(\|T\|) | O(1) | 單次查詢最優 | 無 |
| 3. 哈希表 + 二分查找 | O(\|T\| + \|S\|·log\|T\|) | O(\|T\|) | 多次查詢最佳 | 預處理成本 |
| 4. 動態規劃 | O(\|S\|·\|T\|) | O(\|S\|·\|T\|) | 矩陣構建直觀 | 空間使用量更多 |

**建議：** 由於時間和空間都最優，基本問題使用方法 2（雙指針）。針對包含多個源字符串的進階問題，使用方法 3。
