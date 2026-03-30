# 345. Reverse Vowels of a String — Opus-4.6 Notes

## Implementation vs. Editorial Comparison

The repo's `solution.c` implements the **same algorithm** as the editorial's Approach 1 (Two Pointers). Both use a left pointer and a right pointer that converge toward the center, skipping non-vowel characters and swapping when both pointers land on vowels.

### Differences in detail

| Aspect | Editorial (C++) | Repo `solution.c` |
|---|---|---|
| Vowel check | `||` chain of comparisons | `switch` statement |
| Swap method | `std::swap` / temp variable | XOR swap (`swap_xor`) |
| In-place? | Yes (modifies `s` directly) | No (malloc + strcpy, returns new string) |
| Boundary guard | `start < s.size()` and `end >= 0` in inner loops | Only outer `s_lptr < s_rptr`; inner loop lacks explicit guard |
| Pointer move | Both `start++` and `end--` after swap | `s_rptr--` after swap, `s_lptr++` unconditionally |

### Which approach is better?

| Criterion | Editorial | Repo |
|---|---|---|
| Time complexity | O(N) | O(N) |
| Space complexity | O(1) in C++ | O(N) due to malloc |
| Interview suitability | Slightly better — cleaner boundary checks | Good — switch-based vowel check is clear |
| C implementation difficulty | Low | Low (XOR swap is a nice talking point but unnecessary) |

### Baseline vs. Recommended

- **Baseline approach:** Two Pointers (the only approach for this problem).
- **Recommended approach:** Two Pointers with in-place modification and a simple temp-variable swap. The XOR swap is a fun trick but adds complexity without benefit in an interview — a temp variable is clearer and less error-prone. The editorial's explicit boundary checks in the inner loops (`start < s.size()` and `end >= 0`) are safer and should be adopted.

### Potential improvement for the repo

The repo's inner `while (!isVowel(*s_rptr))` loop has no guard against `s_rptr` underflowing past the string start when the string contains no vowels to the right of `s_lptr`. The editorial guards against this. Consider adding `s_lptr < s_rptr` to the inner loop condition.

---

# 345. 反轉字串中的母音字母 — Opus-4.6 筆記

## 實作 vs. 題解比較

repo 的 `solution.c` 與題解的方法一（雙指標）使用**相同的演算法**。兩者都用一個左指標和一個右指標向中間收斂，跳過非母音字元，當兩個指標都指向母音時進行交換。

### 細節差異

| 面向 | 題解 (C++) | Repo `solution.c` |
|---|---|---|
| 母音判斷 | `\|\|` 串接比較 | `switch` 語句 |
| 交換方式 | `std::swap` / 暫存變數 | XOR 交換 (`swap_xor`) |
| 是否原地修改？ | 是（直接修改 `s`） | 否（malloc + strcpy，回傳新字串） |
| 邊界保護 | 內迴圈有 `start < s.size()` 和 `end >= 0` | 僅外迴圈有 `s_lptr < s_rptr`；內迴圈缺少明確防護 |
| 指標移動 | 交換後 `start++` 和 `end--` | 交換後 `s_rptr--`，`s_lptr++` 無條件執行 |

### 哪種方法較好？

| 評估標準 | 題解 | Repo |
|---|---|---|
| 時間複雜度 | O(N) | O(N) |
| 空間複雜度 | C++ 為 O(1) | O(N)，因為 malloc |
| 面試適用性 | 略好——邊界檢查更清晰 | 好——switch 判斷母音很清楚 |
| C 語言實作難度 | 低 | 低（XOR 交換是有趣的話題但非必要） |

### 基線 vs. 建議方法

- **基線方法：** 雙指標（本題唯一的做法）。
- **建議方法：** 雙指標 + 原地修改 + 簡單暫存變數交換。XOR 交換雖然有趣，但在面試中增加了不必要的複雜度——暫存變數更清晰、不易出錯。題解在內迴圈中的明確邊界檢查（`start < s.size()` 和 `end >= 0`）更安全，應當採用。

### repo 可改進之處

repo 的內迴圈 `while (!isVowel(*s_rptr))` 沒有防護 `s_rptr` 在字串中沒有更多右側母音時越界到字串開頭之前的情況。題解有對此做防護。建議在內迴圈條件中加入 `s_lptr < s_rptr`。
