# LeetCode Problem 3: Longest Substring Without Repeating Characters (Medium)

## Implementation vs. Editorial Comparison

The `.c` solution in the repository implements **Approach 3: Sliding Window Optimized** from the Editorial. Instead of using a HashMap as suggested in the Editorial, the C implementation uses a direct-access table—an `int lastSeen[128]` array—to store the last-seen index of each character. This is a more efficient and idiomatic approach in C, leveraging the fact that ASCII characters are limited to 128 values.

Both approaches follow the same core algorithm:
1. Maintain a sliding window with `left` and `right` pointers
2. Expand the window by moving `right` pointer
3. When a duplicate character is found within the current window, jump the `left` pointer directly to `lastSeen[char] + 1`
4. Track the maximum window size encountered

The key difference is the data structure: HashMap in Editorial vs. fixed-size integer array in the C solution.

---

## Approach Comparison

| Aspect | Approach 1: Brute Force | Approach 2: Sliding Window (HashSet) | Approach 3: Optimized (Direct Access) |
|--------|------------------------|--------------------------------------|---------------------------------------|
| **Time Complexity** | O(n³) | O(2n) = O(n) | O(n) |
| **Space Complexity** | O(min(n, m)) | O(min(n, m)) | O(1) |
| **Practical Steps** | Check all substrings | May shrink window one by one | Jump left pointer directly |
| **Interview Suitability** | Poor | Good | Excellent |
| **C Implementation Difficulty** | Easy | Medium | Easy |
| **Demonstrates Optimization** | No | No | Yes (charset optimization) |

**Detailed Explanation:**

- **Approach 1 (Brute Force):** Checks every possible substring to find the longest one without repeating characters. Inefficient and rarely used in real interviews.

- **Approach 2 (Sliding Window with HashSet):** Uses a hash set to track characters in the current window. When a duplicate is found, shrinks the window from the left one character at a time until the duplicate is removed. Correct and acceptable in interviews, but wastes steps when the duplicate is far to the left.

- **Approach 3 (Optimized Sliding Window):** Uses a direct-access table (HashMap or array) to store the last-seen index of each character. When a duplicate is found, jumps the `left` pointer directly to `lastSeen[char] + 1`. This eliminates redundant steps and is optimal.

---

## Baseline vs. Recommended Approach

**Baseline Approach:** Approach 2 (Sliding Window with HashSet)
- Easy to understand and implement
- Correct time complexity of O(n)
- However, in worst case, the left pointer moves through all n characters (e.g., "abcdefg" has no duplicates, but for each character added, the algorithm checks removals)
- Acceptable for most interviews

**Recommended Approach for Google Interview:** Approach 3 (Optimized Sliding Window with Direct-Access Table)
- Reduces worst-case from 2n to n pointer movements
- Demonstrates deeper understanding of the problem and optimization techniques
- Simpler to implement in C using a fixed-size array (e.g., `int lastSeen[128]`)
- Shows knowledge of charset constraints—leveraging the fact that ASCII has only 128 characters, so space is constant O(1) regardless of input size
- Impresses interviewers by proving you think about real-world optimizations

**Why Recommended is Better:**
1. **True O(n) guarantee:** Each character is visited at most once by the `right` pointer and the `left` pointer never backtracks—guaranteed n operations total.
2. **C-friendly:** Arrays are faster and simpler than hash tables in C; no need for hash function or collision handling.
3. **Interview edge case:** When interviewer asks "Can you optimize further?", you can explain the direct-access technique instead of being stuck.
4. **Professional signal:** Shows you understand trade-offs between generality (HashMap) and specificity (fixed array for ASCII).

---

# LeetCode 問題 3：最長無重複字符子字符串（中等難度）

## 實現與官方解答比較

倉庫中的 `.c` 解決方案實現了官方解答的 **方法 3：優化滑動窗口**。與官方解答中建議的 HashMap 不同，C 實現使用直接訪問表—— `int lastSeen[128]` 數組——來存儲每個字符最後一次出現的索引。這是 C 中更高效且更慣用的方法，利用 ASCII 字符限於 128 個值的事實。

兩種方法都遵循相同的核心算法：
1. 用 `left` 和 `right` 指針維護一個滑動窗口
2. 通過移動 `right` 指針擴展窗口
3. 當在當前窗口內找到重複字符時，直接跳躍 `left` 指針至 `lastSeen[char] + 1`
4. 跟蹤遇到的最大窗口大小

關鍵區別在於數據結構：官方解答中的 HashMap 對比 C 解決方案中的固定大小整數數組。

---

## 方法比較

| 面向 | 方法 1：暴力法 | 方法 2：滑動窗口（HashSet） | 方法 3：優化（直接訪問） |
|------|------------------------|--------------------------------------|---------------------------------------|
| **時間複雜度** | O(n³) | O(2n) = O(n) | O(n) |
| **空間複雜度** | O(min(n, m)) | O(min(n, m)) | O(1) |
| **實際步數** | 檢查所有子字符串 | 可能逐個縮小窗口 | 直接跳躍左指針 |
| **面試適用性** | 差 | 好 | 優秀 |
| **C 實現難度** | 簡單 | 中等 | 簡單 |
| **展示優化能力** | 否 | 否 | 是（字符集優化） |

**詳細說明：**

- **方法 1（暴力法）：** 檢查每個可能的子字符串以找到最長的無重複字符子字符串。低效，面試中很少使用。

- **方法 2（含 HashSet 的滑動窗口）：** 使用哈希集來跟蹤當前窗口中的字符。找到重複字符時，從左邊逐個縮小窗口，直到重複字符被移除。正確且大多數面試都能接受，但當重複字符距離較遠時會浪費步驟。

- **方法 3（優化滑動窗口）：** 使用直接訪問表（HashMap 或數組）存儲每個字符最後一次出現的索引。找到重複字符時，直接跳躍 `left` 指針至 `lastSeen[char] + 1`。這消除了冗餘步驟並且是最優的。

---

## 基線方法 vs. 推薦方法

**基線方法：** 方法 2（含 HashSet 的滑動窗口）
- 易於理解和實現
- 時間複雜度為 O(n)
- 然而，最壞情況下，左指針移動所有 n 個字符（例如「abcdefg」沒有重複，但對於每個添加的字符，算法檢查移除）
- 大多數面試都可以接受

**Google 面試推薦方法：** 方法 3（含直接訪問表的優化滑動窗口）
- 將最壞情況從 2n 減少到 n 個指針移動
- 展示對問題和優化技術的深入理解
- 在 C 中使用固定大小數組（例如 `int lastSeen[128]`）更簡單
- 展示字符集約束的知識——利用 ASCII 只有 128 個字符的事實，空間始終為常數 O(1)，與輸入大小無關
- 通過證明你考慮真實世界的優化給面試官留下深刻印象

**為什麼推薦方法更好：**
1. **真正的 O(n) 保證：** 每個字符最多被右指針訪問一次，左指針永不回溯——保證總共 n 次操作。
2. **C 友好：** 在 C 中，數組比哈希表更快更簡單；無需哈希函數或碰撞處理。
3. **面試邊界情況：** 當面試官問「還能進一步優化嗎？」時，你可以解釋直接訪問技術，而不是被卡住。
4. **專業信號：** 顯示你理解通用性（HashMap）和特異性（ASCII 固定數組）之間的權衡。
