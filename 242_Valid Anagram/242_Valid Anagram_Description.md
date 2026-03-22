# 242. Valid Anagram

## Problem Statement

Given two strings `s` and `t`, return `true` if `t` is an anagram of `s`, and `false` otherwise.

An Anagram is a word or phrase formed by rearranging the letters of a different word or phrase, typically using all the original letters exactly once.

## Examples

### Example 1:
```
Input: s = "anagram", t = "nagaram"
Output: true
```

### Example 2:
```
Input: s = "rat", t = "car"
Output: false
```

### Example 3:
```
Input: s = "a", t = "a"
Output: true
```

## Constraints

- `1 <= s.length, t.length <= 5 × 10^4`
- `s` and `t` consist of lowercase English letters.

## Key Insights

1. **Length Check**: Anagrams must have the same length. If lengths differ, they cannot be anagrams.

2. **Character Frequency**: Two strings are anagrams if and only if they have the same character frequencies.

3. **Counting Approach**: Use a frequency array (or hash map) to count occurrences of each character.

4. **Two-Pass Method**:
   - First pass: increment counters for characters in `s`
   - Second pass: decrement counters for characters in `t`
   - If all counters are zero, the strings are anagrams

5. **Space Optimization**: Since we only have lowercase English letters, a fixed-size array of 26 is sufficient.
