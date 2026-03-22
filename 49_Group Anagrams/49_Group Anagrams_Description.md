# 49. Group Anagrams

## Problem Statement

Given an array of strings `strs`, group the anagrams together. You can return the answer in any order.

An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

## Examples

### Example 1:
```
Input: strs = ["eat","tea","ate","eat","tan","ant"]
Output: [["bat"],["nat","tan"],["ate","eat","tea"]]
(Note: The grouping order may differ; what matters is that anagrams are grouped together)
```

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

## Key Insights

1. **Anagram Property**: Two strings are anagrams if and only if they have the same character frequencies or the same sorted form.

2. **Sorted Key**: Sorting each string gives a canonical form. All anagrams will have the same sorted form.

3. **Grouping**: Strings with the same sorted form belong to the same anagram group.

4. **Time Complexity**: The sorting of individual strings (O(m log m) per string) dominates the overall complexity.

5. **Space Complexity**: We need space to store all strings and their sorted copies.

## Approaches

1. **Hash Map + Sorting** (Recommended): Sort each string and use it as a key in a hash map. In C, we can simulate this by sorting pairs of (sorted_string, original_string) and then grouping consecutive identical sorted strings.

2. **Character Count + Hash Map**: Count character frequencies for each string and use the frequency signature as a hash key. More complex to implement in C without a proper hash table library.
