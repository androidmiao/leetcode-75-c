> **Source:** Synced from the live authenticated LeetCode session (leetcode.com/problems/middle-of-the-linked-list/editorial), March 2026.

---

## Approach 1: Output to Array

### Intuition and Algorithm

Put every node into an array `A` in order. Then the middle node is just `A[A.length / 2]`, since we can retrieve each node by index.

We can initialize the array to be of length 100, as the problem description states the input has at most 100 nodes.

```cpp
// C++ (from LeetCode editorial)
ListNode* middleNode(ListNode* head) {
    vector<ListNode*> A = {head};
    while (A.back()->next != NULL)
        A.push_back(A.back()->next);
    return A[A.size() / 2];
}
```

### Complexity Analysis

- **Time Complexity:** O(N), where N is the number of nodes in the given list.
- **Space Complexity:** O(N), the space used by `A`.

---

## Approach 2: Fast and Slow Pointer

### Intuition and Algorithm

When traversing the list with a pointer `slow`, make another pointer `fast` that traverses twice as fast. When `fast` reaches the end of the list, `slow` must be in the middle.

```cpp
// C++ (from LeetCode editorial)
class Solution {
public:
    ListNode* middleNode(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head;
        while (fast != NULL && fast->next != NULL) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }
};
```

### Complexity Analysis

- **Time Complexity:** O(N), where N is the number of nodes in the given list.
- **Space Complexity:** O(1), the space used by `slow` and `fast`.

---

---

> **資料來源：** 已透過已登入的 LeetCode Chrome session 即時同步（leetcode.com/problems/middle-of-the-linked-list/editorial），2026 年 3 月。

---

## 方法 1：輸出成陣列

### 直覺與演算法

依序將每個節點放入陣列 `A`，中間節點就是 `A[A.length / 2]`，因為我們可以透過索引直接取得任何節點。

由於題目說明輸入最多只有 100 個節點，陣列大小可初始化為 100。

### 複雜度分析

- **時間複雜度：** O(N)，N 為串列中的節點數量。
- **空間複雜度：** O(N)，陣列 `A` 所使用的空間。

---

## 方法 2：快慢指標

### 直覺與演算法

用指標 `slow` 走訪串列時，同時讓另一個指標 `fast` 以兩倍速前進。當 `fast` 到達串列尾端時，`slow` 必定停在中間位置。

### 複雜度分析

- **時間複雜度：** O(N)，N 為串列中的節點數量。
- **空間複雜度：** O(1)，只使用 `slow` 和 `fast` 兩個指標的空間。
