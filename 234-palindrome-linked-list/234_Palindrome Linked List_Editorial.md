# 234. Palindrome Linked List — Editorial

## Approach 1: Copy into Array List and then Use Two Pointer Technique

**Intuition**

A Linked List does not support O(1) random access the way an Array List does. The simplest path to a palindrome check is to copy all values into an array, then apply the two-pointer technique (one pointer from each end, walking inward) to verify symmetry.

**Algorithm**

1. Traverse the list and append each `val` to an array.
2. Use two pointers `front = 0` and `back = len - 1`. While `front < back`, compare `arr[front]` and `arr[back]`. If they ever differ, return `false`. Otherwise advance both pointers inward.
3. Return `true`.

**Complexity**

- Time: O(n) — one pass to copy, one pass (at most half) to compare.
- Space: O(n) — the array holds all n values.

---

## Approach 2: Recursive (Advanced)

**Intuition**

Recursion can traverse a linked list in reverse order for free (values are visited on the way back up the call stack). If we keep a `frontPointer` that starts at `head` and moves forward by one step each time a recursive frame returns, we can compare the front and back in one shot.

**Algorithm**

```
frontPointer = head

recursivelyCheck(currentNode):
    if currentNode is NOT null:
        if NOT recursivelyCheck(currentNode.next):
            return false
        if currentNode.val != frontPointer.val:
            return false
        frontPointer = frontPointer.next
    return true

isPalindrome(head):
    return recursivelyCheck(head)
```

Each recursive frame processes its node on the way back up, so `currentNode` effectively walks backwards while `frontPointer` walks forward.

**Complexity**

- Time: O(n) — one recursive call per node.
- Space: O(n) — the call stack depth equals the number of nodes.

---

## Approach 3: Reverse Second Half In-Place ✓ (Optimal)

**Intuition**

To achieve O(1) space we must not copy values or use the call stack. The key insight: if we reverse the second half of the list in place, we can walk two pointers simultaneously from each end toward the middle and compare values directly.

After the comparison, we restore the second half so the list is left undisturbed.

**Algorithm**

1. **Find the end of the first half** — use the fast/slow pointer trick. `slow` advances one step and `fast` advances two steps per iteration. When `fast` can no longer advance two steps, `slow` is at the last node of the first half. For an odd-length list the middle node stays attached to the first half and is correctly ignored in the comparison.
2. **Reverse the second half** — call a standard in-place list-reversal starting from `firstHalfEnd->next`. Save the new head of the reversed second half as `secondHalfStart`.
3. **Compare** — walk `p1` from `head` and `p2` from `secondHalfStart` simultaneously. If any `p1->val != p2->val`, record `false`. Stop when `p2` reaches `NULL` (the second half is always ≤ the first half in length).
4. **Restore** — reverse the second half again and reconnect it to `firstHalfEnd->next`.
5. Return the saved result.

**Complexity**

- Time: O(n) — finding the middle, reversing, comparing, and restoring are all O(n).
- Space: O(1) — only a constant number of pointers are allocated; the reversal reuses existing nodes in place.

> Note: The in-place mutation briefly breaks the list structure. In a concurrent environment the list would need to be locked for the duration of the call. This is a common limitation of in-place algorithms.

---

## 中文翻譯

## 方法一：複製到陣列後使用雙指標

**思路**

Linked list 無法像陣列一樣 O(1) 隨機存取，所以最簡單的做法就是把所有值複製到陣列中，再用雙指標（一頭一尾往中間走）來判斷是否回文。

**演算法**

1. 走訪 linked list，把每個 `val` 加入陣列。
2. 設 `front = 0`、`back = len - 1`，只要 `front < back` 就比較兩端的值，若不等則回傳 `false`，否則兩指標各往中間移一步。
3. 回傳 `true`。

**複雜度**

- 時間：O(n)。
- 空間：O(n)（陣列儲存 n 個值）。

---

## 方法二：遞迴（進階）

**思路**

遞迴可以讓 linked list 從後往前走：在回傳途中（unwinding 階段），每個呼叫框架都能拿到「比自己更後面」的節點的值。搭配一個從頭往前走的 `frontPointer`，就能同時比較頭尾兩端。

**演算法**

每個遞迴框架先遞迴到底，然後在回傳時比較 `currentNode.val` 與 `frontPointer.val`，不等就回傳 `false`，否則 `frontPointer` 往前一步。

**複雜度**

- 時間：O(n)。
- 空間：O(n)（呼叫堆疊深度等於節點數）。

---

## 方法三：原地反轉後半段 ✓（最優）

**思路**

要達到 O(1) 空間，不能複製值也不能用呼叫堆疊。核心想法：把後半段原地反轉後，從兩端同時往中間比較；比較完再把後半段復原。

**演算法**

1. **找到前半段的末尾**：用快慢指標，`slow` 每次走一步，`fast` 每次走兩步。`fast` 無法再走兩步時，`slow` 就指向前半段末尾。奇數長度時，中間那個節點留在前半段，比較時會被忽略（正確行為）。
2. **反轉後半段**：從 `firstHalfEnd->next` 開始原地反轉，記下新頭 `secondHalfStart`。
3. **比較**：`p1` 從 `head` 出發，`p2` 從 `secondHalfStart` 出發，同步往前走，若 `p1->val != p2->val` 記錄 `false`，直到 `p2` 為 `NULL` 為止。
4. **復原**：再次反轉後半段，重新接回 `firstHalfEnd->next`。
5. 回傳比較結果。

**複雜度**

- 時間：O(n)。
- 空間：O(1)（只用常數個指標，反轉是原地操作）。

> 注意：原地操作期間 linked list 結構暫時被破壞；若在多執行緒環境使用，必須在執行期間鎖定串列。
