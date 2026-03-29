# 49. Group Anagrams

## Problem Statement

Given an array of strings `strs`, group the anagrams together. You can return the answer in **any order**.

An **Anagram** is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

## Examples

### Example 1:
```
Input: strs = ["eat","tea","tan","ate","nat","bat"]
Output: [["bat"],["nat","tan"],["ate","eat","tea"]]
```

**Explanation:**
- There is no string in strs that can be rearranged to form `"bat"`.
- The strings `"nat"` and `"tan"` are anagrams as they can be rearranged to form each other.
- The strings `"ate"`, `"eat"`, and `"tea"` are anagrams as they can be rearranged to form each other.

### Example 2:
```
Input: strs = [""]
Output: [[""]]
```

### Example 3:
```
Input: strs = ["a"]
Output: [["a"]]
```

## Constraints

- `1 <= strs.length <= 10^4`
- `0 <= strs[i].length <= 100`
- `strs[i]` consists of lowercase English letters.

---

# 49. 異位詞分組

## 題目描述

給定一個字串陣列 `strs`，將**異位詞**組合在一起。可以按任意順序返回結果。

**異位詞（Anagram）** 是指透過重新排列不同單詞或片語的字母而形成的單詞或片語，通常恰好使用所有原始字母各一次。

## 範例

### 範例 1：
```
輸入：strs = ["eat","tea","tan","ate","nat","bat"]
輸出：[["bat"],["nat","tan"],["ate","eat","tea"]]
```

**說明：**
- strs 中沒有任何字串可以重新排列形成 `"bat"`。
- 字串 `"nat"` 和 `"tan"` 是異位詞，因為它們可以互相重新排列而成。
- 字串 `"ate"`、`"eat"` 和 `"tea"` 是異位詞，因為它們可以互相重新排列而成。

### 範例 2：
```
輸入：strs = [""]
輸出：[[""]]
```

### 範例 3：
```
輸入：strs = ["a"]
輸出：[["a"]]
```

## 限制條件

- `1 <= strs.length <= 10^4`
- `0 <= strs[i].length <= 100`
- `strs[i]` 僅包含小寫英文字母。

## 關鍵觀察

1. **異位詞性質**：兩個字串是異位詞，當且僅當它們擁有相同的字元頻率，或者排序後的形式相同。
2. **排序鍵**：將每個字串排序可得到一個標準形式（canonical form），所有異位詞排序後都會得到相同的標準形式。
3. **分組策略**：具有相同排序形式的字串屬於同一個異位詞群組。
4. **時間複雜度**：單個字串排序（每個 O(m log m)）主導了整體複雜度。
5. **空間複雜度**：需要額外空間儲存所有字串及其排序副本。
