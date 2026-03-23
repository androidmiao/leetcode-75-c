# 206. Reverse Linked List — Editorial

Source: LeetCode Official Editorial (accessed via live authenticated session)

---

## Approach 1: Iterative

### Intuition

Given a linked list such as `1 → 2 → 3 → ∅`, the goal is to transform it into `∅ ← 1 ← 2 ← 3`.

While traversing the list, change each node's `next` pointer to point to its previous element. Since a singly-linked node has no reference to its predecessor, we must store the previous node beforehand. We also need a temporary pointer to save the next node before overwriting the `next` reference. After the traversal completes, return the new head (the last node visited).

### Algorithm

1. Initialize `prev = NULL` and `curr = head`.
2. While `curr` is not NULL:
   - Save `curr->next` into `nextTemp`.
   - Reverse the pointer: `curr->next = prev`.
   - Advance `prev` to `curr`.
   - Advance `curr` to `nextTemp`.
3. Return `prev` (the new head).

### Complexity Analysis

- **Time complexity:** O(n) — each node is visited exactly once.
- **Space complexity:** O(1) — only a constant number of extra pointers are used.

---

## Approach 2: Recursive

### Intuition

The recursive approach works backwards. Assume the rest of the list (from `head->next` onward) has already been reversed. Given the list:

```
n₁ → … → nₖ₋₁ → nₖ → nₖ₊₁ ← … ← nₘ
```

If nodes from `nₖ₊₁` to `nₘ` are already reversed and we are currently at node `nₖ`, we need `nₖ₊₁` to point back to `nₖ`. This is achieved by `nₖ.next.next = nₖ`.

Critical detail: the original first node `n₁` must have its `next` set to `NULL` to avoid creating a cycle. Forgetting this is a common bug, detectable by testing with a two-element list.

### Algorithm

1. Base case: if `head == NULL` or `head->next == NULL`, return `head`.
2. Recursively reverse the sublist starting at `head->next`, obtaining the new head `p`.
3. Set `head->next->next = head` (make the next node point back to the current node).
4. Set `head->next = NULL` (break the original forward link).
5. Return `p` (the new head propagated up through all recursive calls).

### Complexity Analysis

- **Time complexity:** O(n) — each node is visited exactly once.
- **Space complexity:** O(n) — the implicit call stack uses up to n frames.

---

---

# 206. Reverse Linked List — 官方題解

來源：LeetCode 官方 Editorial（透過已登入的瀏覽器 session 存取）

---

## 方法一：迭代法（Iterative）

### 直覺

給定鏈結串列 `1 → 2 → 3 → ∅`，目標是將其轉換為 `∅ ← 1 ← 2 ← 3`。

遍歷串列時，將每個節點的 `next` 指標改為指向前一個元素。由於單向鏈結串列的節點沒有指向前驅的指標，我們必須事先儲存前一個節點。同時需要一個暫時指標在覆寫 `next` 之前保存下一個節點。遍歷結束後，返回新的頭節點（即最後訪問的節點）。

### 演算法

1. 初始化 `prev = NULL`、`curr = head`。
2. 當 `curr` 不為 NULL 時：
   - 將 `curr->next` 存入 `nextTemp`。
   - 反轉指標：`curr->next = prev`。
   - 將 `prev` 前移到 `curr`。
   - 將 `curr` 前移到 `nextTemp`。
3. 返回 `prev`（新的頭節點）。

### 複雜度分析

- **時間複雜度：** O(n) — 每個節點恰好被訪問一次。
- **空間複雜度：** O(1) — 只使用常數個額外指標。

---

## 方法二：遞迴法（Recursive）

### 直覺

遞迴方法的關鍵是從後向前處理。假設從 `head->next` 開始的子串列已經被反轉。串列狀態如下：

```
n₁ → … → nₖ₋₁ → nₖ → nₖ₊₁ ← … ← nₘ
```

如果從 `nₖ₊₁` 到 `nₘ` 已經反轉完成，且我們目前在節點 `nₖ`，則需要讓 `nₖ₊₁` 指回 `nₖ`。透過 `nₖ.next.next = nₖ` 即可達成。

關鍵細節：原始的第一個節點 `n₁` 必須將其 `next` 設為 `NULL`，否則串列會形成環。這是常見的錯誤，可以用長度為 2 的串列來檢測。

### 演算法

1. 基本情況：若 `head == NULL` 或 `head->next == NULL`，直接返回 `head`。
2. 遞迴反轉從 `head->next` 開始的子串列，取得新的頭節點 `p`。
3. 設定 `head->next->next = head`（讓下一個節點指回當前節點）。
4. 設定 `head->next = NULL`（斷開原本的正向連結）。
5. 返回 `p`（新的頭節點，透過所有遞迴呼叫層層向上傳遞）。

### 複雜度分析

- **時間複雜度：** O(n) — 每個節點恰好被訪問一次。
- **空間複雜度：** O(n) — 隱含的呼叫堆疊最多使用 n 層。
