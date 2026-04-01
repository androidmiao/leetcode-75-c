# 703. Kth Largest Element in a Stream

## English

### Problem Statement

You are part of a university admissions office and need to keep track of the kth highest test score from applicants in real-time. This helps to determine cut-off marks for interviews and admissions dynamically as new applicants submit their scores.

You are tasked to implement a class which, for a given integer k, maintains a stream of test scores and continuously returns the kth highest test score after a new score has been submitted. More specifically, we are looking for the kth highest score in the sorted list of all scores.

### Implementation Requirements

Implement the `KthLargest` class:

- `KthLargest(int k, int[] nums)` - Initializes the object with the integer k and the stream of test scores nums.
- `int add(int val)` - Adds a new test score val to the stream and returns the element representing the kth largest element in the pool of test scores so far.

### Examples

**Example 1:**

```
Input: ["KthLargest", "add", "add", "add", "add", "add"]
       [[3, [4, 5, 8, 2]], [3], [5], [10], [9], [4]]
Output: [null, 4, 5, 5, 8, 8]
```

Explanation:
```
KthLargest kthLargest = new KthLargest(3, [4, 5, 8, 2]);
kthLargest.add(3);   // return 4
kthLargest.add(5);   // return 5
kthLargest.add(10);  // return 5
kthLargest.add(9);   // return 8
kthLargest.add(4);   // return 8
```

**Example 2:**

```
Input: ["KthLargest", "add", "add", "add", "add"]
       [[4, [7, 7, 7, 7, 8, 3]], [2], [10], [9], [9]]
Output: [null, 7, 7, 7, 8]
```

Explanation:
```
KthLargest kthLargest = new KthLargest(4, [7, 7, 7, 7, 8, 3]);
kthLargest.add(2);   // return 7
kthLargest.add(10);  // return 7
kthLargest.add(9);   // return 7
kthLargest.add(9);   // return 8
```

### Constraints

- `0 <= nums.length <= 10^4`
- `1 <= k <= nums.length + 1`
- `-10^4 <= nums[i] <= 10^4`
- `-10^4 <= val <= 10^4`
- At most `10^4` calls will be made to `add`.

---

## 繁體中文

### 問題陳述

你是大學招生辦公室的一員，需要實時追蹤申請者的第k高測試分數。這有助於在新申請者提交分數時，動態地確定面試和錄取的分數線。

你的任務是實現一個類別，該類別針對給定的整數k，維護測試分數流，並在新分數提交後持續返回第k高的測試分數。更具體地說，我們要找的是所有分數排序列表中的第k高分數。

### 實現要求

實現 `KthLargest` 類別：

- `KthLargest(int k, int[] nums)` - 用整數k和測試分數流nums初始化對象。
- `int add(int val)` - 將新的測試分數val添加到流中，並返回代表目前測試分數池中第k大元素的值。

### 範例

**範例1：**

```
輸入: ["KthLargest", "add", "add", "add", "add", "add"]
      [[3, [4, 5, 8, 2]], [3], [5], [10], [9], [4]]
輸出: [null, 4, 5, 5, 8, 8]
```

說明：
```
KthLargest kthLargest = new KthLargest(3, [4, 5, 8, 2]);
kthLargest.add(3);   // 返回 4
kthLargest.add(5);   // 返回 5
kthLargest.add(10);  // 返回 5
kthLargest.add(9);   // 返回 8
kthLargest.add(4);   // 返回 8
```

**範例2：**

```
輸入: ["KthLargest", "add", "add", "add", "add"]
      [[4, [7, 7, 7, 7, 8, 3]], [2], [10], [9], [9]]
輸出: [null, 7, 7, 7, 8]
```

說明：
```
KthLargest kthLargest = new KthLargest(4, [7, 7, 7, 7, 8, 3]);
kthLargest.add(2);   // 返回 7
kthLargest.add(10);  // 返回 7
kthLargest.add(9);   // 返回 7
kthLargest.add(9);   // 返回 8
```

### 限制條件

- `0 <= nums.length <= 10^4`
- `1 <= k <= nums.length + 1`
- `-10^4 <= nums[i] <= 10^4`
- `-10^4 <= val <= 10^4`
- 最多將有 `10^4` 次對 `add` 的調用。
