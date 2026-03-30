# Odd Even Linked List - Editorial

## Solution

### Intuition
The core idea is to separate nodes at odd positions from those at even positions into two distinct linked lists. Once separated, we concatenate the even-position list to the end of the odd-position list.

### Algorithm
While conceptually straightforward, implementing this efficiently requires careful pointer management to avoid bugs.

A proper linked list implementation requires head and tail pointers to support efficient operations at both ends. In this solution:
- The `head` and `odd` pointers track the head and tail of the oddList
- The `evenHead` and `even` pointers track the head and tail of the evenList

As we traverse the original list, we route odd-indexed nodes to oddList and even-indexed nodes to evenList. The pointers `odd` and `even` serve dual purposes: they mark the tail of their respective lists while simultaneously acting as traversal iterators through the original sequence.

The algorithm proceeds as follows:

- **STEP 0:** Set odd = head and even = head.next, with evenHead = even. This positions the odd pointer at position 1 and the even pointer at position 2.

- **STEP 1:** Perform odd.next = even.next to skip node 2 and connect node 1 directly to node 3. Then advance odd to this next position with odd = odd.next. Similarly, set even.next = odd.next to connect node 2 to node 4, then advance even with even = even.next.

- **STEP 2:** Continue iterating: odd.next = even.next connects node 3 to node 5, followed by odd = odd.next. Then even.next = odd.next connects node 4 to null, and the loop terminates since even.next becomes null.

- **FINAL:** Execute odd.next = evenHead to attach the entire evenList to the end of oddList, resulting in the sequence: 1→3→5→2→4.

The loop continues while `even != null && even.next != null`.

Reference Java implementation:
```java
public class Solution {
    public ListNode oddEvenList(ListNode head) {
        if (head == null) return null;
        ListNode odd = head, even = head.next, evenHead = even;
        while (even != null && even.next != null) {
            odd.next = even.next;
            odd = odd.next;
            even.next = odd.next;
            even = even.next;
        }
        odd.next = evenHead;
        return head;
    }
}
```

### Complexity Analysis
- **Time Complexity:** O(n), where n is the total number of nodes. Each node is visited exactly once during traversal.
- **Space Complexity:** O(1). The algorithm uses only a constant number of pointers regardless of input size.

---

# 奇偶鏈表 - 編輯方案

## 解法

### 直觀想法
基本策略是將位於奇數位置的節點與位於偶數位置的節點分別收集到兩個獨立的鏈表中。完成分離後，我們將偶數位置鏈表連接到奇數位置鏈表的末尾。

### 演算法
儘管在概念上相對直接，但要實現一個高效且無錯誤的版本需要精妙的指標管理。

一個設計良好的鏈表通常需要頭尾指標來支援在兩端進行高效操作。在本解法中：
- `head` 和 `odd` 指標分別代表奇數位置鏈表的頭和尾
- `evenHead` 和 `even` 指標分別代表偶數位置鏈表的頭和尾

當我們遍歷原始鏈表時，將奇數索引的節點導向奇數鏈表，偶數索引的節點導向偶數鏈表。指標 `odd` 和 `even` 具有雙重職責：它們既標記各自鏈表的尾部，同時也充當對原始序列的遍歷指標。

演算法按以下步驟進行：

- **第0步：** 初始化 odd = head 且 even = head.next，並設置 evenHead = even。此時 odd 指標位於第1個節點，even 指標位於第2個節點。

- **第1步：** 執行 odd.next = even.next 以跳過第2個節點並直接連接第1個節點至第3個節點。然後透過 odd = odd.next 將 odd 前進到新位置。類似地，設置 even.next = odd.next 將第2個節點連接至第4個節點，再透過 even = even.next 將 even 前進。

- **第2步：** 繼續迭代：odd.next = even.next 連接第3個節點至第5個節點，隨後 odd = odd.next。然後 even.next = odd.next 連接第4個節點至空值，由於 even.next 變為空值，迴圈終止。

- **最終步驟：** 執行 odd.next = evenHead 將整個偶數鏈表附加到奇數鏈表的末尾，得到序列：1→3→5→2→4。

迴圈條件為 `even != null && even.next != null`。

Java 參考實現：
```java
public class Solution {
    public ListNode oddEvenList(ListNode head) {
        if (head == null) return null;
        ListNode odd = head, even = head.next, evenHead = even;
        while (even != null && even.next != null) {
            odd.next = even.next;
            odd = odd.next;
            even.next = odd.next;
            even = even.next;
        }
        odd.next = evenHead;
        return head;
    }
}
```

### 複雜度分析
- **時間複雜度：** O(n)，其中 n 為節點總數。在遍歷過程中，每個節點恰好被訪問一次。
- **空間複雜度：** O(1)。無論輸入規模如何，演算法僅使用常數個指標。
