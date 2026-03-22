# 160. Intersection of Two Linked Lists — Editorial

Source: Live authenticated LeetCode session (Editorial tab).

---

## Approach 1: Brute Force

**Intuition:** For each node in list A, traverse all of list B and check whether any node in B is the same object (same pointer, not just same value).

**Algorithm:**
1. For each node `nodeA` in list A:
   - Traverse all nodes `nodeB` in list B.
   - If `nodeA == nodeB` (pointer equality), return `nodeA`.
2. If no match found, return `null`.

**Note:** This is a useful starting point to mention in an interview, but it should be quickly optimized away.

**Complexity:**
- Time: O(N × M)
- Space: O(1)

---

## Approach 2: Hash Set

**Intuition:** Store all nodes of list B in a hash set, then traverse list A and check if any node is already in the set.

**Algorithm:**
1. Traverse list B, insert every node pointer into a hash set.
2. Traverse list A; for each node, check if it exists in the hash set.
3. Return the first matching node, or `null` if none.

**Complexity:**
- Time: O(N + M) — building the set costs O(M), traversal of A costs O(N).
- Space: O(M) — storing all nodes of list B.

---

## Approach 3: Two Pointers (Optimal)

**Intuition:** If we let pointer `pA` start at `headA` and `pB` start at `headB`, and redirect each pointer to the *other* list's head when it reaches `null`, then:

- Let `a` = length of A's exclusive part, `b` = length of B's exclusive part, `c` = length of shared tail.
- `pA` travels: `a + c` steps on A, then `b + c` steps on B → total `a + b + c`.
- `pB` travels: `b + c` steps on B, then `a + c` steps on A → total `a + b + c`.
- Both arrive at the intersection node at the same step → `pA == pB`.

If there is no intersection (`c = 0`), both pointers arrive at `null` simultaneously after `a + b` steps, so the loop terminates correctly.

```
List A:  [a1] → [a2] → [c1] → [c2] → [c3] → NULL
                           ↑
List B:  [b1] → [b2] → [b3] ┘

a = 2, b = 3, c = 3
pA path: a1 → a2 → c1 → c2 → c3 → NULL → b1 → b2 → b3 → [c1] ← meets pB here
pB path: b1 → b2 → b3 → c1 → c2 → c3 → NULL → a1 → a2 → [c1] ← meets pA here
Both arrive at c1 after 2 + 3 + 3 = 8 steps.
```

**Algorithm:**
1. Set `pA = headA`, `pB = headB`.
2. While `pA != pB`:
   - `pA = (pA == NULL) ? headB : pA->next`
   - `pB = (pB == NULL) ? headA : pB->next`
3. Return `pA` (either the intersection node or `null`).

**Complexity:**
- Time: O(N + M)
- Space: O(1) ✓ — satisfies the follow-up constraint.

---

---

## 中文翻譯

### 方法一：暴力解法

**直覺：** 對 A 中每個節點，遍歷整個 B，比較指標是否相同（注意比較的是節點本身的記憶體位址，而非節點的值）。

**複雜度：**
- 時間：O(N × M)
- 空間：O(1)

---

### 方法二：雜湊集合

**直覺：** 把 B 所有節點的指標存入 hash set，再遍歷 A，找到第一個在 hash set 中的節點即為交叉點。

**複雜度：**
- 時間：O(N + M)
- 空間：O(M)

---

### 方法三：雙指標交叉走（最優）

**核心想法：**
設 A 私有段長度 `a`，B 私有段長度 `b`，共同段長度 `c`。

- `pA` 走路徑：A 的 a 個節點 + c 個共同節點，抵達 NULL 後跳至 headB，再走 b + c 個節點 → 共 **a + b + c** 步
- `pB` 走路徑：B 的 b 個節點 + c 個共同節點，抵達 NULL 後跳至 headA，再走 a + c 個節點 → 共 **a + b + c** 步

兩者恰好在第 `a + b + c` 步同時抵達交叉節點（若有交叉），或同時為 NULL（若無交叉）。

**無交叉情況（c = 0）：**
- pA 走 a + b 步後為 NULL
- pB 走 b + a 步後也為 NULL
- 兩者同時為 NULL，`pA == pB`，迴圈退出，回傳 NULL

**複雜度：**
- 時間：O(N + M)
- 空間：O(1) ✓ — 滿足 Follow-up 要求
