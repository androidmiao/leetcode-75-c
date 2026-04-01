# LeetCode 2130: Maximum Twin Sum of a Linked List - Editorial

---

## English Section

### Overview

In this problem, we are given the head of a linked list that always contains an even number of nodes. For any node at position i (counting from 0), its "twin" is defined as the node at position (n-1-i), where n is the total number of nodes. The task is to find the maximum sum formed by any node and its corresponding twin.

For example, in a list of 4 nodes, node 0 twins with node 3, and node 1 twins with node 2. We need to compute all twin sums and return the maximum value.

### Approach 1: Using List of Integers

#### Intuition

The key observation is that the i-th node from the start is the twin of the i-th node from the end. If we convert the linked list into an array or list data structure, we can directly access elements by index. We can then use the two-pointer technique: initialize one pointer at the beginning and another at the end. Each iteration computes the sum of the elements at these two positions, tracks the maximum sum, and moves the pointers toward the center.

#### Algorithm

1. **Traverse the linked list**: Start from the head and visit each node in order.
2. **Build the array**: Store all node values in an array (or list).
3. **Initialize two pointers**: Set i = 0 (start) and j = size - 1 (end).
4. **Two-pointer iteration**: While i < j:
   - Compute the current twin sum: sum = values[i] + values[j]
   - Update the maximum sum: max_sum = max(max_sum, sum)
   - Move the pointers closer: i += 1, j -= 1
5. **Return**: Return the maximum twin sum found.

#### Complexity Analysis

- **Time Complexity**: O(n), where n is the number of nodes. We traverse the list once to build the array, then perform at most n/2 iterations of the two-pointer loop.
- **Space Complexity**: O(n), because we store all node values in an auxiliary array.

---

### Approach 2: Using Stack

#### Intuition

A stack naturally reverses the order of elements. We can push all node values onto a stack, which effectively stores them in reverse order. Then, we iterate through the original list from the head with a counter to track which half we are in. For each node in the first half, we pop from the stack to retrieve the corresponding twin value from the second half (in reverse order). This approach avoids computing a middle pointer and works intuitively with the stack's LIFO nature.

#### Algorithm

1. **Traverse and push to stack**: Iterate through all nodes, pushing each node's value onto a stack.
2. **Reset to head**: Set current = head to start iterating from the beginning again.
3. **Initialize counter and size**: Set size = stack.size() and count = 1 to track position in the first half.
4. **Iterate through first half**: While count <= size / 2:
   - Retrieve the twin: twin_val = stack.top()
   - Compute twin sum: sum = current.val + twin_val
   - Update maximum: max_sum = max(max_sum, sum)
   - Move forward in list: current = current.next
   - Pop from stack and increment counter: stack.pop(), count += 1
5. **Return**: Return the maximum twin sum.

#### Complexity Analysis

- **Time Complexity**: O(n), where n is the number of nodes. We traverse the list once to fill the stack, then iterate through the first half of the list.
- **Space Complexity**: O(n), because the stack stores all node values.

---

### Approach 3: Reverse Second Half In Place

#### Intuition

This approach is optimized for space efficiency. We use the slow and fast pointer technique to locate the middle of the list. Once we find the middle, we reverse the second half of the list in place using standard reversal techniques. After reversal, we can use two pointers—one starting from the head and another from the reversed second half—to iterate and compute twin sums without requiring extra space for storage.

#### Algorithm

1. **Find the middle using slow and fast pointers**:
   - Initialize slow = head, fast = head
   - While fast is not null and fast.next is not null:
     - fast = fast.next.next (move 2 steps)
     - slow = slow.next (move 1 step)
   - slow now points to the start of the second half

2. **Reverse the second half**:
   - Initialize prev = null
   - While slow is not null:
     - nextNode = slow.next (save next pointer)
     - slow.next = prev (reverse the link)
     - prev = slow (move prev forward)
     - slow = nextNode (move to next node)
   - prev now points to the head of the reversed second half

3. **Compute twin sums with two pointers**:
   - Initialize start = head (beginning of first half)
   - While prev is not null (prev points to reversed second half):
     - Compute twin sum: sum = start.val + prev.val
     - Update maximum: max_sum = max(max_sum, sum)
     - Advance both pointers: start = start.next, prev = prev.next
   - The two pointers meet at the middle

4. **Return**: Return the maximum twin sum.

#### Complexity Analysis

- **Time Complexity**: O(n), where n is the number of nodes. We perform one pass to find the middle, one pass to reverse the second half, and one pass to compute sums.
- **Space Complexity**: O(1), only constant extra space for pointers (prev, nextNode, slow, fast, start). We modify the list structure in place but do not use an auxiliary data structure.

---

## 繁體中文 段落

### 概述

在本題中，我們得到一個鏈表的頭節點，該鏈表始終包含偶數個節點。對於位置 i（從 0 計數）的任何節點，其「孿生節點」定義為位置 (n-1-i) 的節點，其中 n 是節點的總數。任務是找到由任何節點及其對應孿生節點形成的最大和。

例如，在包含 4 個節點的列表中，節點 0 與節點 3 配對，節點 1 與節點 2 配對。我們需要計算所有孿生和並返回最大值。

### 方法 1：使用整數列表

#### 直覺

關鍵觀察是，從開始的第 i 個節點是從末尾的第 i 個節點的孿生節點。如果我們將鏈表轉換為陣列或列表數據結構，我們可以直接按索引訪問元素。然後，我們可以使用雙指針技巧：初始化一個指針在開始，另一個指針在末尾。每次迭代計算這兩個位置元素的和，跟踪最大和，並將指針向中心移動。

#### 算法

1. **遍歷鏈表**：從頭開始，依次訪問每個節點。
2. **構建陣列**：將所有節點值存儲在陣列（或列表）中。
3. **初始化雙指針**：設置 i = 0（開始）和 j = size - 1（末尾）。
4. **雙指針迭代**：當 i < j 時：
   - 計算當前孿生和：sum = values[i] + values[j]
   - 更新最大和：max_sum = max(max_sum, sum)
   - 移動指針靠近：i += 1，j -= 1
5. **返回**：返回找到的最大孿生和。

#### 複雜度分析

- **時間複雜度**：O(n)，其中 n 是節點數。我們遍歷列表一次以構建陣列，然後執行最多 n/2 次雙指針循環迭代。
- **空間複雜度**：O(n)，因為我們在輔助陣列中存儲所有節點值。

---

### 方法 2：使用堆棧

#### 直覺

堆棧自然地反轉元素的順序。我們可以將所有節點值壓入堆棧，這有效地以相反的順序存儲它們。然後，我們從頭開始遍歷原始列表，使用計數器跟踪我們在哪一半中。對於第一半中的每個節點，我們從堆棧中彈出以檢索第二半中的對應孿生值（以相反的順序）。這種方法避免了計算中間指針，並與堆棧的後進先出特性直觀地協作。

#### 算法

1. **遍歷並壓入堆棧**：遍歷所有節點，將每個節點的值壓入堆棧。
2. **重置到頭**：設置 current = head 以從開始重新開始遍歷。
3. **初始化計數器和大小**：設置 size = stack.size() 和 count = 1 以跟踪第一半中的位置。
4. **遍歷第一半**：當 count <= size / 2 時：
   - 檢索孿生值：twin_val = stack.top()
   - 計算孿生和：sum = current.val + twin_val
   - 更新最大值：max_sum = max(max_sum, sum)
   - 在列表中前進：current = current.next
   - 從堆棧彈出並增加計數器：stack.pop()，count += 1
5. **返回**：返回最大孿生和。

#### 複雜度分析

- **時間複雜度**：O(n)，其中 n 是節點數。我們遍歷列表一次以填充堆棧，然後遍歷列表的前半部分。
- **空間複雜度**：O(n)，因為堆棧存儲所有節點值。

---

### 方法 3：原地反轉第二半部分

#### 直覺

此方法針對空間效率進行了優化。我們使用慢指針和快指針技巧來定位列表的中點。找到中點後，我們使用標準反轉技術原地反轉列表的第二半。反轉後，我們可以使用雙指針——一個從頭開始，另一個從反轉的第二半開始——進行迭代和計算孿生和，而無需為存儲分配額外空間。

#### 算法

1. **使用慢指針和快指針找到中點**：
   - 初始化 slow = head，fast = head
   - 當 fast 不為空且 fast.next 不為空時：
     - fast = fast.next.next（移動 2 步）
     - slow = slow.next（移動 1 步）
   - slow 現在指向第二半的開始

2. **反轉第二半部分**：
   - 初始化 prev = null
   - 當 slow 不為空時：
     - nextNode = slow.next（保存下一個指針）
     - slow.next = prev（反轉鏈接）
     - prev = slow（向前移動 prev）
     - slow = nextNode（移動到下一個節點）
   - prev 現在指向反轉第二半的頭

3. **使用雙指針計算孿生和**：
   - 初始化 start = head（第一半的開始）
   - 當 prev 不為空時（prev 指向反轉的第二半）：
     - 計算孿生和：sum = start.val + prev.val
     - 更新最大值：max_sum = max(max_sum, sum)
     - 同時推進兩個指針：start = start.next，prev = prev.next
   - 兩個指針在中點相遇

4. **返回**：返回最大孿生和。

#### 複雜度分析

- **時間複雜度**：O(n)，其中 n 是節點數。我們進行一次遍歷以找到中點，一次遍歷以反轉第二半，以及一次遍歷以計算和。
- **空間複雜度**：O(1)，僅使用指針常數額外空間（prev、nextNode、slow、fast、start）。我們原地修改列表結構，但不使用輔助數據結構。

---
