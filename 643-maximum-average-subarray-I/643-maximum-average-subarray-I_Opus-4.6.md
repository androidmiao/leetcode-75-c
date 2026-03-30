# 643. Maximum Average Subarray I — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The `.c` solution in `643-maximum-average-subarray-I.c` implements **Approach #2: Sliding Window** from the editorial.

The algorithm is identical:

1. Compute the sum of the first `k` elements as the initial window sum.
2. Slide the window one position at a time: add the new right element, subtract the departing left element.
3. Track the maximum window sum seen.
4. Return `maxSum / k` as a `double`.

The only minor difference is that the repo's C implementation uses `long long` for `windowSum` and `maxSum` to guard against integer overflow when element values and `k` are large, whereas the editorial's Java code uses `double` accumulation and the Python code uses native arbitrary-precision integers. This is a practical C-specific defensive choice and does not change the algorithm.

## Which Approach Is Better

| Dimension | Approach #1: Cumulative Sum | Approach #2: Sliding Window |
|---|---|---|
| Time complexity | O(n) | O(n) |
| Space complexity | O(n) — prefix sum array | O(1) — constant extra space |
| Interview suitability | Good as a stepping stone | Preferred — cleaner and more space-efficient |
| C implementation difficulty | Easy — one extra array | Easy — two variables |

Both approaches run in O(n) time, but the sliding window approach is strictly better in space (O(1) vs. O(n)) and is the natural follow-up optimization that interviewers expect to see.

## Baseline vs. Recommended

- **Baseline**: Approach #1 (Cumulative Sum) — straightforward prefix-sum technique, easy to explain, but uses O(n) extra space.
- **Recommended**: Approach #2 (Sliding Window) — same time complexity, O(1) space, and demonstrates mastery of the sliding window pattern which is a recurring theme across many interview problems (e.g., LeetCode 3, 76, 209, 239).

The sliding window approach is better for interview discussion because it shows the candidate can optimize space, and the pattern generalizes to harder problems. For this easy-level problem, either approach is acceptable, but the sliding window is the expected answer.

---

# 643. Maximum Average Subarray I — 實作與 Editorial 比較

## 實作 vs. Editorial 比較

`.c` 解法實作的是 Editorial 中的**方法二：滑動視窗（Sliding Window）**。

演算法完全相同：

1. 先計算前 `k` 個元素的總和作為初始視窗總和。
2. 每次將視窗往右滑動一格：加入新的右側元素，減去離開的左側元素。
3. 持續追蹤所見過的最大視窗總和。
4. 回傳 `maxSum / k` 作為 `double` 型別的結果。

唯一的小差異在於本 repo 的 C 實作使用 `long long` 儲存 `windowSum` 和 `maxSum`，以防止元素值與 `k` 較大時發生整數溢位；而 Editorial 的 Java 程式碼使用 `double` 累加，Python 則使用原生的任意精度整數。這是 C 語言特有的防禦性設計，不影響演算法本身。

## 哪種方法更好

| 面向 | 方法一：前綴和 | 方法二：滑動視窗 |
|---|---|---|
| 時間複雜度 | O(n) | O(n) |
| 空間複雜度 | O(n) — 需要前綴和陣列 | O(1) — 只用常數額外空間 |
| 面試適合度 | 適合作為起步解法 | 較佳 — 更簡潔且更省空間 |
| C 實作難度 | 簡單 — 多一個陣列 | 簡單 — 兩個變數 |

兩種方法的時間複雜度都是 O(n)，但滑動視窗在空間上嚴格優於前綴和（O(1) vs. O(n)），也是面試官期望看到的最佳化方向。

## 基礎方法 vs. 建議方法

- **基礎方法**：方法一（前綴和）— 直觀的前綴和技巧，容易解釋，但需要 O(n) 額外空間。
- **建議方法**：方法二（滑動視窗）— 時間複雜度相同，O(1) 空間，並展示了滑動視窗模式的掌握能力。此模式在許多面試題中反覆出現（如 LeetCode 3、76、209、239）。

滑動視窗方法更適合面試討論，因為它展示了候選人能夠最佳化空間，且此模式可推廣到更難的問題。對於這道 Easy 等級的題目，兩種方法都可以接受，但滑動視窗是面試官期望的標準答案。
