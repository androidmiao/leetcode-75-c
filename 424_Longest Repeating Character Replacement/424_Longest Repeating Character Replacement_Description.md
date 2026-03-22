# 424. Longest Repeating Character Replacement

## Problem Statement

You are given a string `s` and an integer `k`. You can choose any character of the string and change it to any other uppercase English character. You can perform this operation at most `k` times.

Return the length of the longest substring containing the same letter you can get after performing the above operations.

## Examples

### Example 1:
```
Input: s = "ABAB", k = 2
Output: 4
Explanation: Replace the two 'A's with two 'B's or vice versa.
```

### Example 2:
```
Input: s = "ABCABCABCC", k = 2
Output: 4
Explanation: Replace the two 'A's with two 'B's or the two 'B's with two 'A's.
```

### Example 3:
```
Input: s = "A", k = 0
Output: 1
```

## Constraints

- `1 <= s.length <= 10^5`
- `s` consists of only uppercase English letters.
- `0 <= k <= s.length`

## Key Insights

1. **Sliding Window Approach**: This is a classic sliding window problem where we maintain a window of characters that can be made the same with at most `k` changes.

2. **Valid Window Condition**: A window is valid if `(window_length - max_frequency) <= k`, meaning the number of characters we need to change is at most `k`.

3. **Two-Pointer Technique**: We expand the window by moving the right pointer and contract it by moving the left pointer when the condition is violated.

4. **Character Frequency**: We track the frequency of each character in the current window using a 26-element array (for uppercase English letters).

5. **Maximum Frequency**: We maintain the maximum frequency of any character in the current window to determine how many changes are needed.
