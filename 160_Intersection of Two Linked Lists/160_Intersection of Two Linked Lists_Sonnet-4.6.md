# 160. Intersection of Two Linked Lists — Sonnet-4.6 Notes

## Implementation vs. Editorial Comparison

The `.c` solution implements **Approach 3: Two Pointers (交叉走法)** from the editorial.

This is exactly the same algorithm as the editorial's optimal approach: redirect each pointer to the other list's head upon reaching `null`, so both pointers travel the same total distance `a + b + c` and meet at the intersection (or both reach `null` if no intersection).

### Approach Comparison Table

| Criterion | Approach 1: Brute Force | Approach 2: Hash Set | Approach 3: Two Pointers |
|-----------|------------------------|---------------------|--------------------------|
| Time Complexity | O(N × M) | O(N + M) | O(N + M) |
| Space Complexity | O(1) | O(M) | **O(1)** |
| Interview Suitability | Poor (mention briefly only) | Good (acceptable) | **Best** |
| C Implementation Difficulty | Easy | Medium (need hash table) | **Easy** |
| Meets Follow-up Constraint | No | No | **Yes** |

### Baseline vs. Recommended

**Baseline approach:** Approach 2 — Hash Set
- Easy to understand and implement
- O(N + M) time, O(M) space
- A reasonable first-pass answer in an interview

**Recommended approach:** Approach 3 — Two Pointers
- O(N + M) time, **O(1)** space — satisfies the follow-up constraint
- The key mathematical insight (both pointers walk `a + b + c` steps) is elegant and memorable
- In C, the implementation is only 5 lines of logic:
  ```c
  while (pA != pB) {
      pA = (pA == NULL) ? headB : pA->next;
      pB = (pB == NULL) ? headA : pB->next;
  }
  return pA;
  ```
- This is the expected "aha" answer for a Google interview — the interviewer will almost certainly ask for O(1) space after you mention the hash set approach

**Why Two Pointers is better for interview discussion:**
1. It demonstrates mathematical reasoning, not just coding.
2. The O(1) space satisfies the explicit follow-up constraint.
3. The code is concise and explainable in under 30 seconds.
4. It handles the "no intersection" edge case naturally (both pointers become `null` simultaneously).

---

## 中文翻譯

### 實作 vs. Editorial 對比

`.c` 解法實作的是 Editorial 中的**方法三：雙指標交叉走法**，與最優方案完全一致。

### 基準方法 vs. 推薦方法

**基準方法：方法二（雜湊集合）**
- 容易理解，O(N + M) 時間、O(M) 空間
- 面試中作為初步答案是可以接受的

**推薦方法：方法三（雙指標）**
- O(N + M) 時間、O(1) 空間，滿足 Follow-up 要求
- 數學洞察優雅：兩個指標都走 a + b + c 步後必然相遇
- C 語言實作極為精簡，易於在面試中當場寫出並解釋
- Google 面試中，面試官幾乎一定會追問 O(1) 空間的解法，雙指標是標準期望答案

**推薦原因總結：**
1. 展現了數學推理能力，不只是暴力編碼
2. 明確滿足 Follow-up 的空間限制
3. 程式碼簡潔，可在 30 秒內完整解釋
4. 無交叉情況的邊界條件自然處理（兩指標同時變為 NULL）
