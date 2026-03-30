# 345. Reverse Vowels of a String — Editorial

## Overview

This problem extends 344 Reverse String. Instead of reversing every character, we reverse only the vowel characters in the string while keeping all other characters in their original positions.

## Approach 1: Two Pointers

### Intuition

We initialize two pointers: `left` pointing to the beginning of the string and `right` pointing to the end. The key difference from a full string reversal is that we only want to swap vowels. So both pointers must advance until they each land on a vowel, then we swap the characters at those positions.

Specifically, `left` starts at index 0 and increments until it finds a vowel. `right` starts at the last index and decrements until it finds a vowel. When both pointers are on vowels and `left < right`, we swap and move both pointers inward. We repeat until the pointers cross.

### Algorithm

1. Initialize the left pointer `start` to `0`, and the right pointer `end` to `s.size() - 1`.
2. Keep iterating until the left pointer catches up with the right pointer:
   - a. Keep incrementing the left pointer `start` until it points to a vowel character.
   - b. Keep decrementing the right pointer `end` until it points to a vowel character.
   - c. Swap the characters at `start` and `end`.
   - d. Increment the `start` pointer and decrement the `end` pointer.
3. Return the string `s`.

### Complexity Analysis

Let N be the length of the string `s`.

**Time complexity: O(N)**

Although there appear to be nested loops, the `start` and `end` pointers each traverse the index at most once. Every element of `s` is visited at most once by either the left or right pointer. The swap itself is O(1). Hence the total time complexity is O(N).

**Space complexity: O(1) for C/C++, O(N) for Java**

In C/C++ we only need a temporary variable for the swap, so space is O(1). In Java, strings are immutable so we must convert to a char array first, which takes O(N) space. In the repo's C implementation, a new string is malloc'd (O(N)), but an in-place version would be O(1).

---

# 345. 反轉字串中的母音字母 — 題解

## 概述

本題是 344 反轉字串 的延伸。不同於反轉所有字元，我們只需反轉字串中的母音字母，其他字元保持在原來的位置不動。

## 方法一：雙指標（Two Pointers）

### 直覺思路

我們初始化兩個指標：`left` 指向字串開頭，`right` 指向字串結尾。與完整字串反轉的唯一差異在於，我們只想交換母音字母。因此兩個指標都必須持續移動直到各自指向一個母音，然後交換這兩個位置的字元。

具體來說，`left` 從索引 0 開始向右遞增，直到找到母音。`right` 從最後一個索引開始向左遞減，直到找到母音。當兩個指標都在母音上且 `left < right` 時，我們交換並將兩個指標各自向內移動。重複此過程直到指標交叉。

### 演算法步驟

1. 初始化左指標 `start` 為 `0`，右指標 `end` 為 `s.size() - 1`。
2. 持續迭代直到左指標追上右指標：
   - a. 持續遞增左指標 `start`，直到它指向一個母音字元。
   - b. 持續遞減右指標 `end`，直到它指向一個母音字元。
   - c. 交換 `start` 和 `end` 位置的字元。
   - d. 遞增 `start` 指標，遞減 `end` 指標。
3. 回傳字串 `s`。

### 複雜度分析

令 N 為字串 `s` 的長度。

**時間複雜度：O(N)**

雖然看起來有巢狀迴圈，但 `start` 和 `end` 指標各自最多只會遍歷索引一次。`s` 的每個元素最多只被左指標或右指標訪問一次。交換操作本身是 O(1)。因此總時間複雜度為 O(N)。

**空間複雜度：C/C++ 為 O(1)，Java 為 O(N)**

在 C/C++ 中，我們只需要一個暫存變數來進行交換，空間為 O(1)。在 Java 中，字串是不可變的，因此需要先轉換為字元陣列，佔用 O(N) 空間。在本 repo 的 C 實作中，malloc 了一個新字串（O(N)），但若改為原地（in-place）版本則為 O(1)。
