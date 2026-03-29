# LeetCode 151: Reverse Words in a String

## Implementation vs. Editorial Comparison

The `solution.c` implements **Approach 2 from the Editorial**: "Reverse the Whole String and Then Reverse Each Word". This is the optimal in-place algorithm for C.

### How It Works
1. Reverse the entire string
2. Reverse each word individually
3. Result: words appear in reversed order, characters within each word are correct

Example: `"the sky is blue"` → reverse all → `"eulb si yks eht"` → reverse each word → `"blue is sky the"`

### Implementation Details vs. Editorial

| Aspect | Editorial | solution.c | Notes |
|--------|-----------|------------|-------|
| **Algorithm** | Reverse whole + reverse each word | Same | Identical core logic |
| **Swap Method** | Temp variable swap | XOR swap (`^=`) | XOR avoids temporary variable; slightly more compact |
| **Index Type** | `int` | `short` | Saves memory for LeetCode constraints (string length ≤ 10⁶) |
| **Pointer Usage** | Loop indices | Write pointer `k` for compacting | Efficiently handles multiple spaces by skipping them |
| **Space Complexity** | O(1) | O(1) | Both operate in-place on input array |

The core algorithm is the same; differences are implementation optimizations for C.

---

## Approach Comparison Table

| Aspect | Approach 1: Split + Reverse | Approach 2: Reverse Whole + Reverse Each Word | Approach 3: Deque of Words |
|--------|----------------------------|---------------------------------------------|-------------------------|
| **Time Complexity** | O(N) | O(N) | O(N) |
| **Space Complexity** | O(N) — stores words in array | O(1) — in-place reversal only | O(N) — deque structure |
| **Interview Suitability** | ✓ Simple, easy to explain | ✓✓ Demonstrates pointer skills; answers follow-up | ✓ Good for dynamic languages |
| **C Implementation Difficulty** | Easy — use string library functions | Medium — pointer manipulation, XOR swap | Hard — no native deque; requires linked list or array |
| **Handles Extra Spaces** | Yes (naturally) | Yes (with write pointer) | Yes (naturally) |
| **Mutable String Required** | No (works on copies) | Yes (modifies in-place) | No |
| **Best For** | Learning; quick prototypes | Production C code; technical interviews | Higher-level languages |

---

## Baseline vs. Recommended

### Baseline Approach: Approach 1 (Split + Reverse)

```c
// Pseudocode
char** words = split(s, ' ');  // O(N) space
int count = count_words(words);
reverse_array(words, count);   // Reverse word order
join(words, s, ' ');           // Rebuild string
free(words);                   // Clean up
```

**Pros:**
- Straightforward logic: split, reverse array, join
- Easy to understand and implement
- Less error-prone pointer arithmetic

**Cons:**
- **O(N) auxiliary space** — violates the follow-up challenge
- Requires dynamic allocation/deallocation
- Requires string library functions (`strtok`, `malloc`, `free`)

### Recommended Approach: Approach 2 (Reverse Whole + Reverse Each Word)

```c
// Pseudocode
reverse_whole_string(s, end);        // O(1) space
reverse_each_word(s);                // O(1) space
compact_with_write_pointer(s);       // Remove extra spaces
```

**Pros:**
- **O(1) space complexity** — passes the follow-up challenge ("Can you do it in-place with O(1) extra space?")
- Demonstrates strong pointer and low-level string manipulation skills
- Ideal for C where strings are mutable `char` arrays
- No memory allocation/deallocation needed
- Elegant two-reversal trick

**Cons:**
- Requires careful pointer arithmetic
- Need to handle extra spaces explicitly (write pointer `k`)
- XOR swap or temp variable needed for reversal
- Less intuitive at first glance

### Why Approach 2 is Recommended

1. **Follow-up Challenge**: LeetCode's follow-up explicitly asks for O(1) space. Approach 2 is the only solution that satisfies this.

2. **C Strength**: In C, strings are mutable character arrays. Approach 2 leverages this by modifying in-place — natural for C, harder in Python/Java.

3. **Interview Value**:
   - Shows understanding of pointer manipulation
   - Demonstrates ability to optimize space under constraints
   - Reveals algorithm insight (reversal trick)
   - Rare enough to stand out

4. **Implementation Quality**: The `solution.c` code is clean and efficient:
   - XOR swap avoids temporary variable overhead
   - Write pointer `k` compacts spaces in a single pass
   - `short` indexing is sufficient for constraints

---

## Key Takeaway

| Scenario | Use |
|----------|-----|
| **Learning** | Approach 1 — builds intuition with split + reverse |
| **Production / Interview** | Approach 2 — optimal space, demonstrates mastery |
| **Other Languages (Python/Java)** | Approach 1 or 3 — strings immutable, O(1) space impossible |

---

---

# LeetCode 151：反轉字符串中的單詞

## 實現與官方題解的比較

`solution.c` 實現了**官方題解的方法 2**：「先反轉整個字符串，再反轉每個單詞」。這是 C 語言中最優的原地算法。

### 工作原理
1. 反轉整個字符串
2. 反轉每個單詞
3. 結果：單詞順序反轉，單詞內字符正確

示例：`"the sky is blue"` → 反轉全部 → `"eulb si yks eht"` → 反轉每個單詞 → `"blue is sky the"`

### 實現與題解的細節對比

| 方面 | 官方題解 | solution.c | 說明 |
|------|---------|----------|------|
| **算法** | 反轉整個 + 反轉每個單詞 | 相同 | 核心邏輯完全相同 |
| **交換方法** | 臨時變量交換 | XOR 交換 (`^=`) | XOR 避免臨時變量，更緊湊 |
| **索引類型** | `int` | `short` | 節省內存（LeetCode 限制：字符串長度 ≤ 10⁶） |
| **指針使用** | 循環索引 | 寫指針 `k` 壓縮單詞 | 高效跳過多個空格 |
| **空間複雜度** | O(1) | O(1) | 都是原地修改輸入數組 |

核心算法相同；差異是 C 語言實現的優化。

---

## 三種方法對比表

| 方面 | 方法 1：分割 + 反轉 | 方法 2：反轉整體 + 反轉單詞 | 方法 3：雙端隊列 |
|------|-------------------|----------------------|------------|
| **時間複雜度** | O(N) | O(N) | O(N) |
| **空間複雜度** | O(N) — 存儲單詞數組 | O(1) — 原地反轉 | O(N) — 雙端隊列結構 |
| **面試適合度** | ✓ 簡單易解釋 | ✓✓ 展現指針技能；滿足後續挑戰 | ✓ 適合動態語言 |
| **C 實現難度** | 簡單 — 使用字符串庫函數 | 中等 — 指針操作、XOR 交換 | 困難 — 無原生雙端隊列；需鏈表或數組 |
| **處理多個空格** | 是（自然支持） | 是（使用寫指針） | 是（自然支持） |
| **需要可變字符串** | 否（副本上操作） | 是（原地修改） | 否 |
| **最適合** | 學習；快速原型 | 生產 C 代碼；技術面試 | 高級語言 |

---

## 基礎方法 vs. 推薦方法

### 基礎方法：方法 1（分割 + 反轉）

```c
// 偽代碼
char** words = split(s, ' ');  // O(N) 空間
int count = count_words(words);
reverse_array(words, count);   // 反轉單詞順序
join(words, s, ' ');           // 重建字符串
free(words);                   // 清理
```

**優點：**
- 邏輯直觀：分割、反轉數組、合併
- 易於理解和實現
- 指針算術錯誤較少

**缺點：**
- **O(N) 輔助空間** — 不滿足後續挑戰
- 需要動態分配/釋放
- 需要字符串庫函數（`strtok`、`malloc`、`free`）

### 推薦方法：方法 2（反轉整體 + 反轉單詞）

```c
// 偽代碼
reverse_whole_string(s, end);        // O(1) 空間
reverse_each_word(s);                // O(1) 空間
compact_with_write_pointer(s);       // 移除多餘空格
```

**優點：**
- **O(1) 空間複雜度** — 通過後續挑戰（「能否原地 O(1) 額外空間完成？」）
- 展現強大的指針和低級字符串操作技能
- 最適合 C 語言中的可變 `char` 數組
- 無需內存分配/釋放
- 優雅的雙重反轉技巧

**缺點：**
- 需要謹慎的指針算術
- 需要顯式處理多餘空格（寫指針 `k`）
- 反轉時需要 XOR 交換或臨時變量
- 初次理解不直觀

### 為什麼推薦方法 2

1. **後續挑戰**：LeetCode 的後續挑戰明確要求 O(1) 空間。方法 2 是唯一滿足此要求的解決方案。

2. **C 語言優勢**：在 C 中，字符串是可變字符數組。方法 2 通過原地修改充分利用這一特性 — 在 C 中自然，在 Python/Java 中困難。

3. **面試價值**：
   - 展現指針操作理解
   - 展現在約束下優化空間的能力
   - 展現算法洞察（反轉技巧）
   - 足夠罕見以脫穎而出

4. **實現質量**：`solution.c` 代碼簡潔高效：
   - XOR 交換避免臨時變量開銷
   - 寫指針 `k` 單次遍歷壓縮空格
   - `short` 索引對約束足夠

---

## 核心要點

| 場景 | 使用 |
|------|-----|
| **學習** | 方法 1 — 通過分割 + 反轉構建直覺 |
| **生產 / 面試** | 方法 2 — 最優空間，展現掌握 |
| **其他語言（Python/Java）** | 方法 1 或 3 — 字符串不可變，O(1) 空間不可行 |
