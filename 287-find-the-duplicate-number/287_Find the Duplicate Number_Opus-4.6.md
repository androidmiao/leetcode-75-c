# 287. Find the Duplicate Number — Opus-4.6 Notes

---

## English

### Implementation vs. Editorial Comparison

The repository's `solution.c` implements **Approach 7: Floyd's Tortoise and Hare (Cycle Detection)** from the editorial. The algorithm is identical in structure:

1. Phase 1 — fast/slow pointer to find the intersection point inside the cycle.
2. Phase 2 — reset one pointer to the start and advance both at the same speed to find the cycle entrance (the duplicate number).

**Minor implementation difference:** The repo code initialises the pointers one step ahead of the canonical form:

```c
int tortoise = nums[nums[0]];        // already 1 step in
int hare     = nums[nums[nums[0]]];  // already 2 steps in
```

The editorial's canonical form starts both pointers at `nums[0]` and enters a `do … while` loop. The two forms are mathematically equivalent — the repo version simply folds the first iteration into the initialisation, converting the `do … while` into a plain `while`.

### Which Approach Is Better?

| Dimension | Approach 7 (Floyd's) | Approach 5 (Binary Search on Value Range) | Approach 4.2 (Cyclic Sort) |
|---|---|---|---|
| Time complexity | O(n) | O(n log n) | O(n) |
| Space complexity | O(1) | O(1) | O(1) |
| Modifies array? | No | No | Yes |
| Interview suitability | High — elegant and demonstrates linked-list cycle knowledge | Medium — solid but slower | Medium — fast but violates constraints |
| C implementation difficulty | Low — two while loops, no extra allocation | Low — nested loop with count | Low — single while with swap |

### Baseline vs. Recommended

- **Baseline approach:** Approach 2 (HashSet) — O(n) time, O(n) space. Easiest to code and explain; the first approach most candidates reach for. Demonstrates understanding of the duplicate-detection problem but does not satisfy the O(1) space constraint.

- **Recommended approach:** Approach 7 (Floyd's Tortoise and Hare) — O(n) time, O(1) space, no array modification. This is the optimal solution that satisfies all problem constraints. It is the best approach for interview discussion because:
  1. It demonstrates knowledge of the cycle-detection paradigm applicable to many problems (linked list cycle, happy number, etc.).
  2. The mathematical proof (F = nC − a) is a strong talking point showing depth of understanding.
  3. The insight of mapping an array to an implicit linked list is non-obvious and impressive.
  4. It naturally leads to follow-up discussions about the pigeonhole principle and why the cycle must exist.

---

## 繁體中文

### 實作與 Editorial 對照

倉庫中的 `solution.c` 實作了 Editorial 的**方法 7：Floyd 龜兔賽跑（循環檢測）**。演算法結構完全相同：

1. 第一階段 — 快慢指標在循環內找到交點。
2. 第二階段 — 將一個指標重置到起點，兩者同速前進，找到循環入口（即重複數字）。

**細微實作差異：** 倉庫的程式碼在初始化時就預先走了一步：

```c
int tortoise = nums[nums[0]];        // 已走 1 步
int hare     = nums[nums[nums[0]]];  // 已走 2 步
```

Editorial 的標準寫法是兩個指標都從 `nums[0]` 出發，進入 `do … while` 迴圈。兩種寫法在數學上等價——倉庫版本只是把第一次迭代折入初始化，將 `do … while` 轉成普通 `while`。

### 哪個方法更好？

| 維度 | 方法 7（Floyd） | 方法 5（值範圍二分搜索） | 方法 4.2（循環排序） |
|---|---|---|---|
| 時間複雜度 | O(n) | O(n log n) | O(n) |
| 空間複雜度 | O(1) | O(1) | O(1) |
| 是否修改陣列？ | 否 | 否 | 是 |
| 面試適合度 | 高——優雅且展示鏈表循環知識 | 中——穩健但較慢 | 中——快速但違反約束 |
| C 實作難度 | 低——兩個 while 迴圈，無額外分配 | 低——巢狀迴圈加計數 | 低——單 while 加交換 |

### 基線方法 vs. 推薦方法

- **基線方法：** 方法 2（雜湊集合）— O(n) 時間，O(n) 空間。最容易撰寫和解釋；是大多數候選人首先想到的方法。能展示對重複檢測問題的理解，但不滿足 O(1) 空間約束。

- **推薦方法：** 方法 7（Floyd 龜兔賽跑）— O(n) 時間，O(1) 空間，不修改陣列。這是滿足所有問題約束的最優解。它是面試討論的最佳方法，因為：
  1. 展示了循環檢測範式的知識，適用於許多問題（鏈表循環、快樂數等）。
  2. 數學證明（F = nC − a）是一個有力的討論點，展現了深度理解。
  3. 將陣列映射到隱式鏈表的洞察力並非顯而易見，令人印象深刻。
  4. 自然引出關於鴿籠原理以及為什麼循環必定存在的後續討論。
