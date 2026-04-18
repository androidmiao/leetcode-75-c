# LeetCode 75 C — 雙 Sprint 衝刺計畫（14 天）

> Google 面試 × Linux Kernel 工程師角度，兩個 Sprint 複習完全部 73 題。
> Sprint 1：打穩 Kernel 核心 + Google 高頻基礎（Day 1–7）
> Sprint 2：進階題型 + 模式整合 + 模擬面試（Day 8–14）
>
> 🔗 **連結說明**：表格中 **題號** 直接開啟本地 C 實作檔案；**題目名稱** 連到 LeetCode 原題頁。
> （優先連到題目命名的 `.c` 檔；若資料夾只有 `main.c`+`solution.c` 結構，則連到 `main.c`，可從 `main.c` 的 `#include "solution.h"` 跳到實際解題 code）

---

## 一、全部題目分類 & Linux Kernel 相關性總覽

### 🔴 Tier 1：Kernel 核心機制直接對應（9 題）— 必須滾瓜爛熟

| #   | 題目                     |  難度  | 分類          | Kernel 對應                                    | 為什麼是 Tier 1                         |
| --- | ---------------------- | :--: | ----------- | -------------------------------------------- | ----------------------------------- |
| [146](./146_LRU%20Cache/146_LRU%20Cache.c) | [LRU Cache](https://leetcode.com/problems/lru-cache/)              | Med  | Cache       | page cache reclaim, dentry LRU, slab reclaim | kernel 記憶體管理的靈魂，面試幾乎必考              |
| [206](./206_Reverse%20Linked%20List/206_Reverse%20Linked%20List.c) | [Reverse Linked List](https://leetcode.com/problems/reverse-linked-list/)    | Easy | Linked List | `list_head` 操作                               | kernel 最基本的資料結構操作                   |
| [21](./21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c)  | [Merge Two Sorted Lists](https://leetcode.com/problems/merge-two-sorted-lists/) | Easy | Linked List | `list_sort()` merge 階段                       | `lib/list_sort.c` 直接使用              |
| [141](./141_Linked%20List%20Cycle/141_Linked%20List%20Cycle.c) | [Linked List Cycle](https://leetcode.com/problems/linked-list-cycle/)      | Easy | Linked List | lockdep 死鎖偵測                                 | Floyd 環偵測 = 依賴圖環檢查的核心思維             |
| [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c)  | [Merge k Sorted Lists](https://leetcode.com/problems/merge-k-sorted-lists/)   | Hard | Heap + LL   | `list_sort()` bottom-up merge                | kernel 的 list_sort 就是 merge sort 變體 |
| [207](./207_Course%20Schedule/207_Course%20Schedule.c) | [Course Schedule](https://leetcode.com/problems/course-schedule/)        | Med  | Graph       | module dependency 環偵測                        | `depmod` / lockdep 的基礎              |
| [210](./210_Course%20Schedule%20II/210_Course%20Schedule%20II.c) | [Course Schedule II](https://leetcode.com/problems/course-schedule-ii/)     | Med  | Graph       | `depmod` 拓撲排序                                | systemd 服務啟動順序、module 載入順序          |
| [232](./232_Implement%20Queue%20using%20Stacks/232_Implement%20Queue%20using%20Stacks.c) | [Queue using Stacks](https://leetcode.com/problems/implement-queue-using-stacks/)     | Easy | Stack/Queue | kfifo, workqueue 概念                          | 理解 kernel 的工作佇列機制                   |
| [700](./700_Search%20in%20a%20Binary%20Search%20Tree/700_Search%20in%20a%20Binary%20Search%20Tree.c) | [Search in a BST](https://leetcode.com/problems/search-in-a-binary-search-tree/)        | Easy | Tree        | rb-tree 操作前提                                 | `rbtree.h` 是 kernel 最常用平衡樹          |

### 🟡 Tier 2：Kernel 概念高度相關（11 題）— 優先準備

| # | 題目 | 難度 | 分類 | Kernel 對應 |
|---|------|:----:|------|------------|
| [160](./160_Intersection%20of%20Two%20Linked%20Lists/160_Intersection%20of%20Two%20Linked%20Lists.c) | [Intersection of Two Linked Lists](https://leetcode.com/problems/intersection-of-two-linked-lists/) | Easy | Linked List | list splicing / 合併操作 |
| [876](./876_Middle%20of%20the%20Linked%20List/876_Middle%20of%20the%20Linked%20List.c) | [Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/) | Easy | Linked List | 快慢指標是 list 操作基本功 |
| [704](./704_Binary%20Search/704_Binary%20Search.c) | [Binary Search](https://leetcode.com/problems/binary-search/) | Easy | Binary Search | `bsearch()` 直接存在於 `<linux/bsearch.h>` |
| [35](./35_Search%20Insert%20Position/Search%20Insert%20Position.c) | [Search Insert Position](https://leetcode.com/problems/search-insert-position/) | Easy | Binary Search | 左邊界搜尋，page table lookup 思維 |
| [1](./1_Two%20Sum/1_Two%20Sum.c) | [Two Sum](https://leetcode.com/problems/two-sum/) | Easy | Hash Table | pid hash, inode hash, dentry cache |
| [217](./217_Contains%20Duplicate/217_Contains%20Duplicate.c) | [Contains Duplicate](https://leetcode.com/problems/contains-duplicate/) | Easy | Hash Table | hash set 概念，重複偵測 |
| [239](./239_Sliding%20Window%20Maximum/239_Sliding%20Window%20Maximum.c) | [Sliding Window Maximum](https://leetcode.com/problems/sliding-window-maximum/) | Hard | Sliding Window | TCP window, scheduler 時間窗口 |
| [933](./933_Number%20of%20Recent%20Calls/933_Number%20of%20Recent%20Calls.c) | [Number of Recent Calls](https://leetcode.com/problems/number-of-recent-calls/) | Easy | Queue | ring buffer / `kfifo` 概念 |
| [215](./215_Kth%20Largest%20Element%20in%20an%20Array/215_Kth%20Largest%20Element%20in%20an%20Array.c) | [Kth Largest Element](https://leetcode.com/problems/kth-largest-element-in-an-array/) | Med | Heap | CFS 排程器 priority 選取 |
| [994](./994_Rotting%20Oranges/Rotting%20Oranges.c) | [Rotting Oranges](https://leetcode.com/problems/rotting-oranges/) | Med | Graph/BFS | device probe propagation |
| [287](./287-find-the-duplicate-number/main.c) | [Find the Duplicate Number](https://leetcode.com/problems/find-the-duplicate-number/) | Med | Two Pointers | Floyd 環偵測（同 141） |

### 🟢 Tier 3：Google 面試高頻但 Kernel 關聯較弱（15 題）

| # | 題目 | 難度 | 分類 | Google 面試重要度 |
|---|------|:----:|------|:-----------------:|
| [3](./3_Longest%20Substring%20Without%20Repeating%20Characters/3_Longest%20Substring%20Without%20Repeating%20Characters.c) | [Longest Substring Without Repeating](https://leetcode.com/problems/longest-substring-without-repeating-characters/) | Med | Sliding Window | ★★★ |
| [76](./76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring.c) | [Minimum Window Substring](https://leetcode.com/problems/minimum-window-substring/) | Hard | Sliding Window | ★★★ |
| [84](./84_Largest%20Rectangle%20in%20Histogram/Largest%20Rectangle%20in%20Histogram.c) | [Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/) | Hard | Monotonic Stack | ★★★ |
| [49](./49_Group%20Anagrams/Group%20Anagrams.c) | [Group Anagrams](https://leetcode.com/problems/group-anagrams/) | Med | Hash Table | ★★★ |
| [200](./200_Number%20of%20Islands/Number%20of%20Islands.c) | [Number of Islands](https://leetcode.com/problems/number-of-islands/) | Med | Graph/DFS | ★★★ |
| [295](./295_Find%20Median%20from%20Data%20Stream/Find%20Median%20from%20Data%20Stream.c) | [Find Median from Data Stream](https://leetcode.com/problems/find-median-from-data-stream/) | Hard | Heap | ★★★ |
| [347](./347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements.c) | [Top K Frequent Elements](https://leetcode.com/problems/top-k-frequent-elements/) | Med | Heap | ★★★ |
| [155](./155_Min%20Stack/Min%20Stack.c) | [Min Stack](https://leetcode.com/problems/min-stack/) | Med | Stack | ★★ |
| [739](./739_Daily%20Temperatures/Daily%20Temperatures.c) | [Daily Temperatures](https://leetcode.com/problems/daily-temperatures/) | Med | Monotonic Stack | ★★ |
| [53](./53_Maximum%20Subarray/Maximum%20Subarray.c) | [Maximum Subarray](https://leetcode.com/problems/maximum-subarray/) | Med | DP | ★★ |
| [152](./152_Maximum%20Product%20Subarray/Maximum%20Product%20Subarray.c) | [Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/) | Med | DP | ★★ |
| [11](./11-container-with-most-water/main.c) | [Container With Most Water](https://leetcode.com/problems/container-with-most-water/) | Med | Two Pointers | ★★ |
| [238](./238-product-of-array-except-self/238.%20Product%20of%20Array%20Except%20Self.c) | [Product of Array Except Self](https://leetcode.com/problems/product-of-array-except-self/) | Med | Array/Prefix | ★★ |
| [133](./133_Clone%20Graph/Clone%20Graph.c) | [Clone Graph](https://leetcode.com/problems/clone-graph/) | Med | Graph/DFS | ★★ |
| [33](./33_Search%20in%20Rotated%20Sorted%20Array/Search%20in%20Rotated%20Sorted%20Array.c) | [Search in Rotated Sorted Array](https://leetcode.com/problems/search-in-rotated-sorted-array/) | Med | Binary Search | ★★ |

### 🔵 Tier 4：基礎練習 & 暖手題（38 題）

| # | 題目 | 難度 | 分類 |
|---|------|:----:|------|
| [19](./19_Remove%20Nth%20Node%20From%20End%20of%20List/19_Remove%20Nth%20Node%20From%20End%20of%20List.c) | [Remove Nth Node From End](https://leetcode.com/problems/remove-nth-node-from-end-of-list/) | Med | Linked List |
| [234](./234-palindrome-linked-list/234_Palindrome%20Linked%20List.c) | [Palindrome Linked List](https://leetcode.com/problems/palindrome-linked-list/) | Easy | Linked List |
| [328](./328-odd-even-linked-list/main.c) | [Odd Even Linked List](https://leetcode.com/problems/odd-even-linked-list/) | Med | Linked List |
| [2095](./2095-delete-the-middle-node-of-a-linked-list/2095_Delete%20the%20Middle%20Node%20of%20a%20Linked%20List.c) | [Delete the Middle Node](https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/) | Med | Linked List |
| [2130](./2130-maximum-twin-sum-of-a-linked-list/2130_Maximum%20Twin%20Sum%20of%20a%20Linked%20List.c) | [Maximum Twin Sum](https://leetcode.com/problems/maximum-twin-sum-of-a-linked-list/) | Med | Linked List |
| [219](./219_Contains%20Duplicate%20II/Contains%20Duplicate%20II.c) | [Contains Duplicate II](https://leetcode.com/problems/contains-duplicate-ii/) | Easy | Hash Table |
| [242](./242_Valid%20Anagram/Valid%20Anagram.c) | [Valid Anagram](https://leetcode.com/problems/valid-anagram/) | Easy | Hash Table |
| [2215](./2215_Find%20the%20Difference%20of%20Two%20Arrays/2215_Find%20the%20Difference%20of%20Two%20Arrays.c) | [Find Difference of Two Arrays](https://leetcode.com/problems/find-the-difference-of-two-arrays/) | Easy | Hash Table |
| [20](./20_Valid%20Parentheses/Valid%20Parentheses.c) | [Valid Parentheses](https://leetcode.com/problems/valid-parentheses/) | Easy | Stack |
| [2390](./2390_Removing%20Stars%20From%20a%20String/2390_Removing%20Stars%20From%20a%20String.c) | [Removing Stars From a String](https://leetcode.com/problems/removing-stars-from-a-string/) | Med | Stack |
| [162](./162_Find%20Peak%20Element/Find%20Peak%20Element.c) | [Find Peak Element](https://leetcode.com/problems/find-peak-element/) | Med | Binary Search |
| [374](./374-guess-number-higher-or-lower/374_Guess%20Number%20Higher%20or%20Lower.c) | [Guess Number Higher or Lower](https://leetcode.com/problems/guess-number-higher-or-lower/) | Easy | Binary Search |
| [567](./567_Permutation%20in%20String/Permutation%20in%20String.c) | [Permutation in String](https://leetcode.com/problems/permutation-in-string/) | Med | Sliding Window |
| [424](./424_Longest%20Repeating%20Character%20Replacement/Longest%20Repeating%20Character%20Replacement.c) | [Longest Repeating Char Replacement](https://leetcode.com/problems/longest-repeating-character-replacement/) | Med | Sliding Window |
| [480](./480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c) | [Sliding Window Median](https://leetcode.com/problems/sliding-window-median/) | Hard | Sliding Window |
| [643](./643-maximum-average-subarray-I/643-maximum-average-subarray-I.c) | [Maximum Average Subarray I](https://leetcode.com/problems/maximum-average-subarray-i/) | Easy | Sliding Window |
| [703](./703_Kth%20Largest%20Element%20in%20a%20Stream/Kth%20Largest%20Element%20in%20a%20Stream.c) | [Kth Largest in a Stream](https://leetcode.com/problems/kth-largest-element-in-a-stream/) | Easy | Heap |
| [692](./692_Top%20K%20Frequent%20Words/692_Top%20K%20Frequent%20Words.c) | [Top K Frequent Words](https://leetcode.com/problems/top-k-frequent-words/) | Med | Heap |
| [104](./104_Maximum%20Depth%20of%20Binary%20Tree/104_Maximum%20Depth%20of%20Binary%20Tree.c) | [Maximum Depth of Binary Tree](https://leetcode.com/problems/maximum-depth-of-binary-tree/) | Easy | Tree |
| [199](./199_Binary%20Tree%20Right%20Side%20View/199_Binary%20Tree%20Right%20Side%20View.c) | [Binary Tree Right Side View](https://leetcode.com/problems/binary-tree-right-side-view/) | Med | Tree |
| [841](./841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c) | [Keys and Rooms](https://leetcode.com/problems/keys-and-rooms/) | Med | Graph/DFS |
| [1926](./1926_Nearest%20Exit%20from%20Entrance%20in%20Maze/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze.c) | [Nearest Exit in Maze](https://leetcode.com/problems/nearest-exit-from-entrance-in-maze/) | Med | Graph/BFS |
| [121](./121_Best%20Time%20to%20Buy%20and%20Sell%20Stock/Best%20Time%20to%20Buy%20and%20Sell%20Stock.c) | [Best Time to Buy/Sell Stock](https://leetcode.com/problems/best-time-to-buy-and-sell-stock/) | Easy | DP |
| [122](./122_Best%20Time%20to%20Buy%20and%20Sell%20Stock%20II/Best%20Time%20to%20Buy%20and%20Sell%20Stock%20II.c) | [Best Time to Buy/Sell Stock II](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/) | Med | DP |
| [283](./283-move-zeroes/283-move-zeroes.c) | [Move Zeroes](https://leetcode.com/problems/move-zeroes/) | Easy | Two Pointers |
| [334](./334-increasing-triplet-subsequence/main.c) | [Increasing Triplet Subsequence](https://leetcode.com/problems/increasing-triplet-subsequence/) | Med | Greedy |
| [345](./345-reverse-vowels-of-a-string/main.c) | [Reverse Vowels](https://leetcode.com/problems/reverse-vowels-of-a-string/) | Easy | Two Pointers |
| [392](./392-is-subsequence/main.c) | [Is Subsequence](https://leetcode.com/problems/is-subsequence/) | Easy | Two Pointers |
| [605](./605-can-place-flowers/main.c) | [Can Place Flowers](https://leetcode.com/problems/can-place-flowers/) | Easy | Greedy |
| [151](./151-reverse-words-in-a-string/main.c) | [Reverse Words in a String](https://leetcode.com/problems/reverse-words-in-a-string/) | Med | Two Pointers |
| [1768](./1768-merge-strings-alternately-c/1768-merge-strings-alternately.c) | [Merge Strings Alternately](https://leetcode.com/problems/merge-strings-alternately/) | Easy | Two Pointers |
| [1071](./1071-gcd-of-strings/main.c) | [GCD of Strings](https://leetcode.com/problems/greatest-common-divisor-of-strings/) | Easy | Math |
| [1431](./1431-kids-with-the-greatest-number-of-candies/main.c) | [Kids Greatest Candies](https://leetcode.com/problems/kids-with-the-greatest-number-of-candies/) | Easy | Array |
| [1732](./1732-find-the-highest-altitude/1732-find-the-highest-altitude.c) | [Find the Highest Altitude](https://leetcode.com/problems/find-the-highest-altitude/) | Easy | Prefix Sum |
| [724](./724-find-pivot-index/main.c) | [Find Pivot Index](https://leetcode.com/problems/find-pivot-index/) | Easy | Prefix Sum |
| [17](./17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c) | [Letter Combinations of Phone Number](https://leetcode.com/problems/letter-combinations-of-a-phone-number/) | Med | Backtracking |

---

## 二、Kernel 資料結構 ↔ LeetCode 完整映射表

> 💡 此表中的題號連到本地 C 實作檔案

| Kernel 結構/機制 | 原始碼位置 | 對應 LeetCode 題 | 面試延伸話題 |
|-----------------|-----------|:----------------:|-------------|
| **`list_head`** 雙向環形鏈結串列 | `include/linux/list.h` | [206](./206_Reverse%20Linked%20List/206_Reverse%20Linked%20List.c), [21](./21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c), [141](./141_Linked%20List%20Cycle/141_Linked%20List%20Cycle.c), [160](./160_Intersection%20of%20Two%20Linked%20Lists/160_Intersection%20of%20Two%20Linked%20Lists.c), [234](./234-palindrome-linked-list/234_Palindrome%20Linked%20List.c), [328](./328-odd-even-linked-list/main.c), [876](./876_Middle%20of%20the%20Linked%20List/876_Middle%20of%20the%20Linked%20List.c), [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c), [146](./146_LRU%20Cache/146_LRU%20Cache.c) | container_of macro、list_for_each_safe、為何用環形？ |
| **`hlist_head`** hash table chaining | `include/linux/hashtable.h` | [1](./1_Two%20Sum/1_Two%20Sum.c), [217](./217_Contains%20Duplicate/217_Contains%20Duplicate.c), [219](./219_Contains%20Duplicate%20II/Contains%20Duplicate%20II.c), [146](./146_LRU%20Cache/146_LRU%20Cache.c) | 為何 hlist 只有單向？節省 bucket 記憶體 |
| **`rb_root`/`rb_node`** 紅黑樹 | `include/linux/rbtree.h` | [700](./700_Search%20in%20a%20Binary%20Search%20Tree/700_Search%20in%20a%20Binary%20Search%20Tree.c), [215](./215_Kth%20Largest%20Element%20in%20an%20Array/215_Kth%20Largest%20Element%20in%20an%20Array.c), [295](./295_Find%20Median%20from%20Data%20Stream/Find%20Median%20from%20Data%20Stream.c) | CFS 的 vruntime、vm_area_struct 查找 |
| **`kfifo`** 環形緩衝區 | `include/linux/kfifo.h` | [232](./232_Implement%20Queue%20using%20Stacks/232_Implement%20Queue%20using%20Stacks.c), [933](./933_Number%20of%20Recent%20Calls/933_Number%20of%20Recent%20Calls.c) | power-of-2 大小、無鎖單生產者/消費者 |
| **module dependency** DAG | `kernel/module/` | [207](./207_Course%20Schedule/207_Course%20Schedule.c), [210](./210_Course%20Schedule%20II/210_Course%20Schedule%20II.c) | depmod 如何建 DAG、modprobe 載入順序 |
| **lockdep** 鎖依賴圖 | `kernel/locking/lockdep.c` | [141](./141_Linked%20List%20Cycle/141_Linked%20List%20Cycle.c), [207](./207_Course%20Schedule/207_Course%20Schedule.c) | 偵測 ABBA 死鎖、lock class 概念 |
| **page LRU lists** 頁面回收 | `mm/vmscan.c` | [146](./146_LRU%20Cache/146_LRU%20Cache.c) | active/inactive list、LRU-2Q、refault distance |
| **`bsearch()`** 二分搜尋 | `lib/bsearch.c` | [704](./704_Binary%20Search/704_Binary%20Search.c), [35](./35_Search%20Insert%20Position/Search%20Insert%20Position.c) | generic comparator 設計 |
| **TCP sliding window** | `net/ipv4/tcp_input.c` | [239](./239_Sliding%20Window%20Maximum/239_Sliding%20Window%20Maximum.c), [3](./3_Longest%20Substring%20Without%20Repeating%20Characters/3_Longest%20Substring%20Without%20Repeating%20Characters.c), [76](./76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring.c), [643](./643-maximum-average-subarray-I/643-maximum-average-subarray-I.c) | cwnd, rwnd, 流量控制 |
| **`list_sort()`** merge sort | `lib/list_sort.c` | [21](./21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c), [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c) | bottom-up 為何優於 top-down（cache + 無遞迴） |
| **buddy system** 夥伴系統 | `mm/page_alloc.c` | [283](./283-move-zeroes/283-move-zeroes.c) (compaction 思維) | 記憶體碎片整理 = move zeroes 概念 |
| **workqueue** 工作佇列 | `kernel/workqueue.c` | [232](./232_Implement%20Queue%20using%20Stacks/232_Implement%20Queue%20using%20Stacks.c), [933](./933_Number%20of%20Recent%20Calls/933_Number%20of%20Recent%20Calls.c) | CMWQ、延遲工作、priority ordered |
| **epoll rb-tree** | `fs/eventpoll.c` | [700](./700_Search%20in%20a%20Binary%20Search%20Tree/700_Search%20in%20a%20Binary%20Search%20Tree.c), [215](./215_Kth%20Largest%20Element%20in%20an%20Array/215_Kth%20Largest%20Element%20in%20an%20Array.c) | epoll 如何用 rb-tree 管理 fd |
| **radix tree / XArray** | `include/linux/xarray.h` | [704](./704_Binary%20Search/704_Binary%20Search.c) (搜尋思維) | page cache 的 index 查找 |

---

## 三、Sprint 1（Day 1–7）：Kernel 核心 + Google 基礎

> **目標**：完成所有 Tier 1 + Tier 2 + 大部分 Tier 3 題目
> **節奏**：每天 5–7 題，每題 = 讀 code → 手寫一遍 → 口述思路 → 說出 kernel 關聯

---

### Day 1：Linked List I — Kernel 的根基 `list_head`

> 🎯 目標：所有 linked list 基本操作能閉眼手寫，pointer 操作零失誤

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [206](./206_Reverse%20Linked%20List/206_Reverse%20Linked%20List.c) | [Reverse Linked List](https://leetcode.com/problems/reverse-linked-list/) | 三指標迭代法 prev/curr/next | `list_head` 反轉操作 |
| 2 | [21](./21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c) | [Merge Two Sorted Lists](https://leetcode.com/problems/merge-two-sorted-lists/) | dummy head + 雙指標合併 | `list_sort()` 的 merge 階段 |
| 3 | [141](./141_Linked%20List%20Cycle/141_Linked%20List%20Cycle.c) | [Linked List Cycle](https://leetcode.com/problems/linked-list-cycle/) | Floyd 快慢指標，為何一定相遇 | lockdep 環偵測思維 |
| 4 | [876](./876_Middle%20of%20the%20Linked%20List/876_Middle%20of%20the%20Linked%20List.c) | [Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/) | 快慢指標找中點 | list 分割操作前置步驟 |
| 5 | [160](./160_Intersection%20of%20Two%20Linked%20Lists/160_Intersection%20of%20Two%20Linked%20Lists.c) | [Intersection of Two Linked Lists](https://leetcode.com/problems/intersection-of-two-linked-lists/) | 雙指標交叉走法 + 數學證明 | list splicing 交叉操作 |
| 6 | [19](./19_Remove%20Nth%20Node%20From%20End%20of%20List/19_Remove%20Nth%20Node%20From%20End%20of%20List.c) | [Remove Nth Node From End](https://leetcode.com/problems/remove-nth-node-from-end-of-list/) | 間距法 + dummy head | list 刪除的安全操作 |
| 7 | [234](./234-palindrome-linked-list/234_Palindrome%20Linked%20List.c) | [Palindrome Linked List](https://leetcode.com/problems/palindrome-linked-list/) | 綜合：快慢+反轉+比較+還原 | 完整 list 操作組合技 |

📋 **Kernel 重點摘要**：
- `list_head` 是 kernel 最頻繁使用的資料結構，`task_struct`、`wait_queue`、`workqueue` 全部用它串接
- 核心 API：`list_add()`, `list_del()`, `list_move()`, `list_splice()`, `list_for_each_safe()`
- 面試加分點：解釋 `container_of` macro 如何從 `list_head` 取得外部 struct

---

### Day 2：Linked List II + LRU Cache — 從操作到設計

> 🎯 目標：LRU Cache 能完整手寫 doubly-linked list + hash map 的 C 實作

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [328](./328-odd-even-linked-list/main.c) | [Odd Even Linked List](https://leetcode.com/problems/odd-even-linked-list/) | 奇偶拆分再合併 | list 重排操作 |
| 2 | [2095](./2095-delete-the-middle-node-of-a-linked-list/2095_Delete%20the%20Middle%20Node%20of%20a%20Linked%20List.c) | [Delete the Middle Node](https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/) | 快慢指標定位 + 刪除 | list 刪除 |
| 3 | [2130](./2130-maximum-twin-sum-of-a-linked-list/2130_Maximum%20Twin%20Sum%20of%20a%20Linked%20List.c) | [Maximum Twin Sum](https://leetcode.com/problems/maximum-twin-sum-of-a-linked-list/) | 反轉後半 + 雙指標求和 | list 操作組合 |
| 4 | [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c) | **[Merge k Sorted Lists](https://leetcode.com/problems/merge-k-sorted-lists/)** | ★ 分治合併 or min-heap | `list_sort()` bottom-up merge |
| 5 | **[146](./146_LRU%20Cache/146_LRU%20Cache.c)** | **[LRU Cache](https://leetcode.com/problems/lru-cache/)** | ★★★ O(1) get/put 完整實作 | page cache reclaim 核心機制 |

📋 **Kernel 重點摘要**：
- **LRU Cache = 今天重頭戲**。kernel 的 page reclaim 使用 active/inactive 雙 LRU list
- `mm/vmscan.c` 中的 `shrink_active_list()` / `shrink_inactive_list()` 就是 LRU 淘汰策略
- 面試加分點：解釋 kernel 為何用 LRU-2Q 而非純 LRU（避免一次性大量 page scan 污染 active list）
- `list_sort()` 使用 bottom-up merge sort 而非 top-down，因為不需要隨機存取（linked list 無法 O(1) 取中點的 array 方式）

---

### Day 3：Hash Table + Binary Search — Kernel 查找雙引擎

> 🎯 目標：能手寫 C 的 hash table（chaining），二分搜尋的邊界條件零失誤

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [1](./1_Two%20Sum/1_Two%20Sum.c) | [Two Sum](https://leetcode.com/problems/two-sum/) | hash map 一次遍歷 O(n) | pid hash, inode hash |
| 2 | [217](./217_Contains%20Duplicate/217_Contains%20Duplicate.c) | [Contains Duplicate](https://leetcode.com/problems/contains-duplicate/) | hash set 判重 | duplicate detection |
| 3 | [242](./242_Valid%20Anagram/Valid%20Anagram.c) | [Valid Anagram](https://leetcode.com/problems/valid-anagram/) | 頻率陣列替代 hash | 計數技巧 |
| 4 | [49](./49_Group%20Anagrams/Group%20Anagrams.c) | [Group Anagrams](https://leetcode.com/problems/group-anagrams/) | hash key 設計 | hash 函數設計 |
| 5 | [704](./704_Binary%20Search/704_Binary%20Search.c) | [Binary Search](https://leetcode.com/problems/binary-search/) | 標準左閉右閉模板 | `lib/bsearch.c` |
| 6 | [35](./35_Search%20Insert%20Position/Search%20Insert%20Position.c) | [Search Insert Position](https://leetcode.com/problems/search-insert-position/) | 左邊界二分 | page table lookup 思維 |
| 7 | [33](./33_Search%20in%20Rotated%20Sorted%20Array/Search%20in%20Rotated%20Sorted%20Array.c) | [Search in Rotated Sorted Array](https://leetcode.com/problems/search-in-rotated-sorted-array/) | 變形二分判斷邏輯 | — |

📋 **Kernel 重點摘要**：
- kernel 用 `hlist_head`/`hlist_node` 做 hash table，每個 bucket 是**單向**鏈結（節省一個指標的記憶體）
- `include/linux/hashtable.h` 提供 `hash_add()`, `hash_del()`, `hash_for_each_possible()`
- `lib/bsearch.c` 提供通用 `bsearch()` 函數，使用 function pointer 作比較器
- 面試加分點：解釋為何 `hlist` 是單向（bucket 數量極多時，每個省一個指標 = 可觀的記憶體節省）

---

### Day 4：Stack/Queue + 拓撲排序 — workqueue & module dependency

> 🎯 目標：拓撲排序能手寫 BFS(Kahn) 和 DFS 兩種版本

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [20](./20_Valid%20Parentheses/Valid%20Parentheses.c) | [Valid Parentheses](https://leetcode.com/problems/valid-parentheses/) | 堆疊匹配基礎 | — |
| 2 | [232](./232_Implement%20Queue%20using%20Stacks/232_Implement%20Queue%20using%20Stacks.c) | [Queue using Stacks](https://leetcode.com/problems/implement-queue-using-stacks/) | ★ amortized O(1) 分析 | kfifo, workqueue |
| 3 | [933](./933_Number%20of%20Recent%20Calls/933_Number%20of%20Recent%20Calls.c) | [Number of Recent Calls](https://leetcode.com/problems/number-of-recent-calls/) | 佇列 / ring buffer | `kfifo` 環形緩衝區 |
| 4 | [155](./155_Min%20Stack/Min%20Stack.c) | [Min Stack](https://leetcode.com/problems/min-stack/) | 輔助堆疊追蹤最小值 | — |
| 5 | [207](./207_Course%20Schedule/207_Course%20Schedule.c) | **[Course Schedule](https://leetcode.com/problems/course-schedule/)** | ★ DFS 三色環偵測 | module dependency 環偵測 |
| 6 | [210](./210_Course%20Schedule%20II/210_Course%20Schedule%20II.c) | **[Course Schedule II](https://leetcode.com/problems/course-schedule-ii/)** | ★ BFS Kahn's 拓撲排序 | depmod / modprobe 載入順序 |

📋 **Kernel 重點摘要**：
- **kfifo**：power-of-2 大小的環形緩衝區，`in` 和 `out` 用自然溢位實現無鎖單生產者/單消費者
- **module dependency**：`depmod` 掃描所有 `.ko` 建立依賴 DAG → 拓撲排序決定載入順序
- **lockdep**：在 runtime 建立 lock dependency graph，偵測 ABBA 型死鎖（= 有向圖環偵測）
- 面試加分點：能解釋 lockdep 的 lock class 概念（同一把鎖的不同實例共享同一個 class）

---

### Day 5：Sliding Window 全系列 — TCP 滑動視窗基礎

> 🎯 目標：掌握「擴右縮左」統一模板，固定/可變視窗都能寫

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [643](./643-maximum-average-subarray-I/643-maximum-average-subarray-I.c) | [Maximum Average Subarray I](https://leetcode.com/problems/maximum-average-subarray-i/) | 固定視窗入門 | — |
| 2 | [567](./567_Permutation%20in%20String/Permutation%20in%20String.c) | [Permutation in String](https://leetcode.com/problems/permutation-in-string/) | 固定視窗 + matches 計數 | — |
| 3 | [3](./3_Longest%20Substring%20Without%20Repeating%20Characters/3_Longest%20Substring%20Without%20Repeating%20Characters.c) | [Longest Substring w/o Repeating](https://leetcode.com/problems/longest-substring-without-repeating-characters/) | 可變視窗 + hash set | TCP receive window |
| 4 | [424](./424_Longest%20Repeating%20Character%20Replacement/Longest%20Repeating%20Character%20Replacement.c) | [Longest Repeating Char Replacement](https://leetcode.com/problems/longest-repeating-character-replacement/) | 可變視窗 + 頻率計數 | — |
| 5 | [76](./76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring.c) | **[Minimum Window Substring](https://leetcode.com/problems/minimum-window-substring/)** | ★ 可變視窗完整模板 | TCP congestion window |
| 6 | [239](./239_Sliding%20Window%20Maximum/239_Sliding%20Window%20Maximum.c) | **[Sliding Window Maximum](https://leetcode.com/problems/sliding-window-maximum/)** | ★ 單調 deque（Google 高頻） | scheduler 時間窗口 |

📋 **Kernel 重點摘要**：
- TCP 滑動視窗：`cwnd`（congestion window）控制發送速率，`rwnd`（receive window）控制接收端緩衝
- `net/ipv4/tcp_input.c` 中的 `tcp_ack()` 處理視窗滑動
- 239 的 monotonic deque 類似 scheduler 中維護一段時間內的「最優」任務選擇
- 面試加分點：能將 sliding window 演算法概念連結到 TCP 流量控制的實際機制

---

### Day 6：Heap / Priority Queue — 排程器思維

> 🎯 目標：能手寫 C 的 min-heap（sift_up / sift_down），Top-K 模式熟練

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [703](./703_Kth%20Largest%20Element%20in%20a%20Stream/Kth%20Largest%20Element%20in%20a%20Stream.c) | [Kth Largest in a Stream](https://leetcode.com/problems/kth-largest-element-in-a-stream/) | min-heap 大小 k 基礎 | — |
| 2 | [215](./215_Kth%20Largest%20Element%20in%20an%20Array/215_Kth%20Largest%20Element%20in%20an%20Array.c) | [Kth Largest Element](https://leetcode.com/problems/kth-largest-element-in-an-array/) | Quickselect vs Heap 取捨 | CFS priority 選取 |
| 3 | [347](./347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements.c) | [Top K Frequent Elements](https://leetcode.com/problems/top-k-frequent-elements/) | freq count + heap/bucket | — |
| 4 | [692](./692_Top%20K%20Frequent%20Words/692_Top%20K%20Frequent%20Words.c) | [Top K Frequent Words](https://leetcode.com/problems/top-k-frequent-words/) | heap + 自訂比較器 | — |
| 5 | [295](./295_Find%20Median%20from%20Data%20Stream/Find%20Median%20from%20Data%20Stream.c) | **[Find Median from Data Stream](https://leetcode.com/problems/find-median-from-data-stream/)** | ★ 雙堆平衡 | — |
| 6 | [480](./480_Sliding%20Window%20Median/480_Sliding%20Window%20Median.c) | **[Sliding Window Median](https://leetcode.com/problems/sliding-window-median/)** | ★ 雙堆 + 延遲刪除 | — |

📋 **Kernel 重點摘要**：
- CFS（Completely Fair Scheduler）用 rb-tree 管理 task，以 `vruntime` 為 key，最小值 = 下一個排程的 task
- 概念上是 priority queue，但 kernel 選擇 rb-tree 而非 binary heap（因為需要高效刪除任意節點）
- timer wheel (`kernel/time/timer.c`) 也是一種 priority 管理機制
- 面試加分點：解釋為何 CFS 用 rb-tree 而非 heap（O(log n) 任意刪除 vs heap 的 O(n) 搜尋 + O(log n) 刪除）

---

### Day 7：Graph + Tree — device tree & DFS/BFS

> 🎯 目標：Sprint 1 收官，圖的 DFS/BFS + 樹的基本操作全部到位

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | [200](./200_Number%20of%20Islands/Number%20of%20Islands.c) | [Number of Islands](https://leetcode.com/problems/number-of-islands/) | DFS 連通分量 | — |
| 2 | [133](./133_Clone%20Graph/Clone%20Graph.c) | [Clone Graph](https://leetcode.com/problems/clone-graph/) | DFS + visited hash map | — |
| 3 | [700](./700_Search%20in%20a%20Binary%20Search%20Tree/700_Search%20in%20a%20Binary%20Search%20Tree.c) | **[Search in a BST](https://leetcode.com/problems/search-in-a-binary-search-tree/)** | ★ BST 搜尋基本功 | rb-tree 操作前提 |
| 4 | [104](./104_Maximum%20Depth%20of%20Binary%20Tree/104_Maximum%20Depth%20of%20Binary%20Tree.c) | [Maximum Depth of Binary Tree](https://leetcode.com/problems/maximum-depth-of-binary-tree/) | 樹 DFS 遞迴 | 樹遍歷基本功 |
| 5 | [199](./199_Binary%20Tree%20Right%20Side%20View/199_Binary%20Tree%20Right%20Side%20View.c) | [Binary Tree Right Side View](https://leetcode.com/problems/binary-tree-right-side-view/) | 樹 BFS 層序 | — |
| 6 | [994](./994_Rotting%20Oranges/Rotting%20Oranges.c) | [Rotting Oranges](https://leetcode.com/problems/rotting-oranges/) | 多源 BFS | device probe propagation |
| 7 | [841](./841_Keys%20and%20Rooms/841_Keys%20and%20Rooms.c) | [Keys and Rooms](https://leetcode.com/problems/keys-and-rooms/) | DFS 可達性 | — |

📋 **Kernel 重點摘要**：
- rb-tree (`include/linux/rbtree.h`) 用於 CFS（`sched_entity`）、`vm_area_struct`（virtual memory）、`epoll`（fd 管理）
- device tree (DTS) 解析是一種樹走訪，`drivers/of/` 中有大量 DFS 式遍歷
- 面試加分點：能解釋 rb-tree 的 `rb_insert_color()` / `rb_erase()` 旋轉邏輯的高層概念

---

### 📊 Sprint 1 完成統計

| 指標 | 數量 |
|------|------|
| 總題數 | **45 題** |
| Tier 1 (Kernel 核心) | 9/9 ✅ |
| Tier 2 (Kernel 相關) | 10/11 |
| Tier 3 (Google 高頻) | 14/15 |
| Tier 4 (基礎題) | 12/38 |

---

## 四、Sprint 2（Day 8–14）：進階整合 + 模擬面試

> **目標**：完成剩餘全部題目 + 模式整合 + 計時模擬面試
> **節奏**：Day 8–12 每天 5–6 題，Day 13–14 模擬面試 + 總複習

---

### Day 8：Monotonic Stack + DP I — 進階面試題型

> 🎯 目標：Monotonic stack 模式內化，DP 的狀態轉移能秒寫

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | [739](./739_Daily%20Temperatures/Daily%20Temperatures.c) | [Daily Temperatures](https://leetcode.com/problems/daily-temperatures/) | 單調遞減堆疊入門 |
| 2 | [84](./84_Largest%20Rectangle%20in%20Histogram/Largest%20Rectangle%20in%20Histogram.c) | **[Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/)** | ★ 單調遞增堆疊（Google Hard 經典） |
| 3 | [2390](./2390_Removing%20Stars%20From%20a%20String/2390_Removing%20Stars%20From%20a%20String.c) | [Removing Stars From a String](https://leetcode.com/problems/removing-stars-from-a-string/) | 堆疊模擬操作 |
| 4 | [53](./53_Maximum%20Subarray/Maximum%20Subarray.c) | [Maximum Subarray](https://leetcode.com/problems/maximum-subarray/) | Kadane's algorithm |
| 5 | [152](./152_Maximum%20Product%20Subarray/Maximum%20Product%20Subarray.c) | [Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/) | 同時追蹤 max/min |
| 6 | [121](./121_Best%20Time%20to%20Buy%20and%20Sell%20Stock/Best%20Time%20to%20Buy%20and%20Sell%20Stock.c) | [Best Time to Buy/Sell Stock](https://leetcode.com/problems/best-time-to-buy-and-sell-stock/) | 單次掃描追蹤 min |

📋 **重點**：84 是 Google 面試經典 Hard，必須能在 20 分鐘內寫完。Kadane's 要能變體應用。

---

### Day 9：DP II + Two Pointers 基礎 — 掃描模式

> 🎯 目標：雙指標的四種模式（同向、對撞、快慢、間距）全部能分辨

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | [122](./122_Best%20Time%20to%20Buy%20and%20Sell%20Stock%20II/Best%20Time%20to%20Buy%20and%20Sell%20Stock%20II.c) | [Best Time to Buy/Sell Stock II](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/) | 貪心:每段上漲都吃 |
| 2 | [283](./283-move-zeroes/283-move-zeroes.c) | [Move Zeroes](https://leetcode.com/problems/move-zeroes/) | 雙指標原地分區 |
| 3 | [11](./11-container-with-most-water/main.c) | [Container With Most Water](https://leetcode.com/problems/container-with-most-water/) | 對撞雙指標 + 貪心證明 |
| 4 | [287](./287-find-the-duplicate-number/main.c) | [Find the Duplicate Number](https://leetcode.com/problems/find-the-duplicate-number/) | Floyd 環偵測用在陣列 |
| 5 | [238](./238-product-of-array-except-self/238.%20Product%20of%20Array%20Except%20Self.c) | [Product of Array Except Self](https://leetcode.com/problems/product-of-array-except-self/) | 前綴/後綴乘積不用除法 |
| 6 | [334](./334-increasing-triplet-subsequence/main.c) | [Increasing Triplet Subsequence](https://leetcode.com/problems/increasing-triplet-subsequence/) | 貪心雙變數追蹤 |

📋 **Kernel 重點**：283 Move Zeroes 的概念 = memory compaction（記憶體碎片整理），`mm/compaction.c` 就是把使用中的 page 往一端移動，空閒 page 往另一端集中。287 的 Floyd 環偵測要和 Day 1 的 141 串聯複習。

---

### Day 10：剩餘 Two Pointers + String — 基礎鞏固

> 🎯 目標：簡單題 5 分鐘內寫完，確保零失誤

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | [345](./345-reverse-vowels-of-a-string/main.c) | [Reverse Vowels of a String](https://leetcode.com/problems/reverse-vowels-of-a-string/) | 對撞雙指標 |
| 2 | [392](./392-is-subsequence/main.c) | [Is Subsequence](https://leetcode.com/problems/is-subsequence/) | 雙指標順序匹配 |
| 3 | [151](./151-reverse-words-in-a-string/main.c) | [Reverse Words in a String](https://leetcode.com/problems/reverse-words-in-a-string/) | 全反轉 + 逐字反轉 |
| 4 | [1768](./1768-merge-strings-alternately-c/1768-merge-strings-alternately.c) | [Merge Strings Alternately](https://leetcode.com/problems/merge-strings-alternately/) | 雙指標交替合併 |
| 5 | [1071](./1071-gcd-of-strings/main.c) | [GCD of Strings](https://leetcode.com/problems/greatest-common-divisor-of-strings/) | GCD 數學性質 |
| 6 | [219](./219_Contains%20Duplicate%20II/Contains%20Duplicate%20II.c) | [Contains Duplicate II](https://leetcode.com/problems/contains-duplicate-ii/) | 滑動視窗 + hash |

---

### Day 11：剩餘 Easy 題速刷 + Graph 補完

> 🎯 目標：所有 Easy 確認無死角，Graph 的 BFS 最短路徑收尾

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | [605](./605-can-place-flowers/main.c) | [Can Place Flowers](https://leetcode.com/problems/can-place-flowers/) | 貪心掃描 |
| 2 | [1431](./1431-kids-with-the-greatest-number-of-candies/main.c) | [Kids Greatest Candies](https://leetcode.com/problems/kids-with-the-greatest-number-of-candies/) | 線性掃描 |
| 3 | [1732](./1732-find-the-highest-altitude/1732-find-the-highest-altitude.c) | [Find the Highest Altitude](https://leetcode.com/problems/find-the-highest-altitude/) | 前綴和 |
| 4 | [724](./724-find-pivot-index/main.c) | [Find Pivot Index](https://leetcode.com/problems/find-pivot-index/) | 前綴和 |
| 5 | [2215](./2215_Find%20the%20Difference%20of%20Two%20Arrays/2215_Find%20the%20Difference%20of%20Two%20Arrays.c) | [Find Difference of Two Arrays](https://leetcode.com/problems/find-the-difference-of-two-arrays/) | hash set 差集 |
| 6 | [1926](./1926_Nearest%20Exit%20from%20Entrance%20in%20Maze/1926_Nearest%20Exit%20from%20Entrance%20in%20Maze.c) | [Nearest Exit in Maze](https://leetcode.com/problems/nearest-exit-from-entrance-in-maze/) | BFS 最短路徑 |
| 7 | [374](./374-guess-number-higher-or-lower/374_Guess%20Number%20Higher%20or%20Lower.c) | [Guess Number Higher or Lower](https://leetcode.com/problems/guess-number-higher-or-lower/) | 標準二分（API 呼叫型） |

---

### Day 12：剩餘 Medium + 補強弱項

> 🎯 目標：Binary Search 進階 + Backtracking + 查漏補缺

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | [162](./162_Find%20Peak%20Element/Find%20Peak%20Element.c) | [Find Peak Element](https://leetcode.com/problems/find-peak-element/) | 條件二分（非單調） |
| 2 | [17](./17_Letter%20Combinations%20of%20a%20Phone%20Number/17_Letter%20Combinations%20of%20a%20Phone%20Number.c) | [Letter Combinations of Phone Number](https://leetcode.com/problems/letter-combinations-of-a-phone-number/) | 回溯法模板 |
| 3 | 回顧 | **Tier 1 中自己最弱的 3 題** | 不看 code 重寫 |
| 4 | 回顧 | **Tier 3 中自己最弱的 2 題** | 計時重寫 |

📋 **重點**：今天是彈性日。如果前面有卡住的題目，今天全力補強。

---

### Day 13：模擬面試 I — Google Phone Screen

> 🎯 目標：45 分鐘內完成 1 Medium + 口述 + follow-up

| 時段 | 內容 |
|------|------|
| 上午 模擬 1 | 抽一題 Medium（推薦池：[146](./146_LRU%20Cache/146_LRU%20Cache.c), [207](./207_Course%20Schedule/207_Course%20Schedule.c), [3](./3_Longest%20Substring%20Without%20Repeating%20Characters/3_Longest%20Substring%20Without%20Repeating%20Characters.c), [347](./347_Top%20K%20Frequent%20Elements/347_Top%20K%20Frequent%20Elements.c), [239](./239_Sliding%20Window%20Maximum/239_Sliding%20Window%20Maximum.c)），45 分鐘計時手寫 |
| 上午 模擬 2 | 抽一題 Hard（推薦池：[76](./76_Minimum%20Window%20Substring/76_Minimum%20Window%20Substring.c), [84](./84_Largest%20Rectangle%20in%20Histogram/Largest%20Rectangle%20in%20Histogram.c), [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c), [295](./295_Find%20Median%20from%20Data%20Stream/Find%20Median%20from%20Data%20Stream.c)），45 分鐘計時 |
| 下午 | 重做 Tier 1 任意 5 題，每題 10 分鐘手寫（不看 code） |
| 晚上 | 整理所有 ★★★ 題的 kernel 關聯「一句話摘要」 |

📋 **模擬面試 SOP**：
1. 讀題 2 分鐘
2. 口述 brute force → 最優解思路 3 分鐘
3. 手寫 code 20 分鐘
4. 走一遍 test case 5 分鐘
5. 口述 time/space complexity + follow-up 5 分鐘
6. 解釋 kernel 關聯 3 分鐘

---

### Day 14：模擬面試 II + Kernel 總複習 — 最終衝刺

> 🎯 目標：面試狀態調整到最佳

| 時段 | 內容 |
|------|------|
| 上午 模擬 | 隨機抽 2 Medium + 1 Hard，模擬完整 Google On-site（3 × 45 min） |
| 下午 Kernel 總複習 | 逐一過一遍下方「Kernel 一句話速查表」，每個能說出 30 秒的口述 |
| 晚上 | 整理 behavioral questions + system design 思路（LRU Cache → page cache 設計） |

---

### 📊 Sprint 2 完成統計

| 指標 | 數量 |
|------|------|
| 總題數 | **28 題 + 模擬面試** |
| Tier 2 剩餘 | 1/1 ✅ |
| Tier 3 剩餘 | 1/1 ✅ |
| Tier 4 剩餘 | 26/26 ✅ |
| 模擬面試場次 | 5+ 場 |

---

## 五、Kernel 一句話速查表（面試前 30 分鐘快速過）

> 💡 題號直接開啟本地 C 實作檔案，題目名稱連 LeetCode

| 題目 | Kernel 一句話 |
|------|-------------|
| [146](./146_LRU%20Cache/146_LRU%20Cache.c) [LRU Cache](https://leetcode.com/problems/lru-cache/) | = `mm/vmscan.c` 的 page reclaim：active/inactive 雙 LRU list + `hlist` hash 查找 |
| [206](./206_Reverse%20Linked%20List/206_Reverse%20Linked%20List.c) [Reverse LL](https://leetcode.com/problems/reverse-linked-list/) | = `list_head` 反轉，kernel patch 中最常見的指標操作 |
| [21](./21_Merge%20Two%20Sorted%20Lists/21_Merge%20Two%20Sorted%20Lists.c) [Merge Sorted](https://leetcode.com/problems/merge-two-sorted-lists/) | = `lib/list_sort.c` 的 merge 階段，比較器用 function pointer |
| [141](./141_Linked%20List%20Cycle/141_Linked%20List%20Cycle.c) [Cycle Detect](https://leetcode.com/problems/linked-list-cycle/) | = lockdep 環偵測思維：有向圖中找 back edge |
| [23](./23_Merge%20k%20Sorted%20Lists/23_Merge%20k%20Sorted%20Lists.c) [Merge k Lists](https://leetcode.com/problems/merge-k-sorted-lists/) | = `list_sort()` bottom-up merge sort（不用遞迴，省 kernel stack） |
| [207](./207_Course%20Schedule/207_Course%20Schedule.c) [Course Sched](https://leetcode.com/problems/course-schedule/) | = `depmod` 建立 module dependency DAG + 環偵測 |
| [210](./210_Course%20Schedule%20II/210_Course%20Schedule%20II.c) [Course Sched II](https://leetcode.com/problems/course-schedule-ii/) | = `modprobe` 用拓撲排序決定 module 載入順序 |
| [232](./232_Implement%20Queue%20using%20Stacks/232_Implement%20Queue%20using%20Stacks.c) [Queue/Stacks](https://leetcode.com/problems/implement-queue-using-stacks/) | = `kfifo` 環形緩衝區的概念簡化版 |
| [700](./700_Search%20in%20a%20Binary%20Search%20Tree/700_Search%20in%20a%20Binary%20Search%20Tree.c) [BST Search](https://leetcode.com/problems/search-in-a-binary-search-tree/) | = `rb_tree` 搜尋的簡化版（無旋轉平衡） |
| [704](./704_Binary%20Search/704_Binary%20Search.c) [Binary Search](https://leetcode.com/problems/binary-search/) | = `lib/bsearch.c`，kernel 直接提供的 API |
| [239](./239_Sliding%20Window%20Maximum/239_Sliding%20Window%20Maximum.c) [Window Max](https://leetcode.com/problems/sliding-window-maximum/) | = TCP sliding window + scheduler 的時間窗口最優選取 |
| [1](./1_Two%20Sum/1_Two%20Sum.c) [Two Sum](https://leetcode.com/problems/two-sum/) | = `hlist_head` hash table 查找，類似 pid/inode hash |
| [933](./933_Number%20of%20Recent%20Calls/933_Number%20of%20Recent%20Calls.c) [Recent Calls](https://leetcode.com/problems/number-of-recent-calls/) | = `kfifo` ring buffer 的 enqueue/dequeue |
| [287](./287-find-the-duplicate-number/main.c) [Find Dup](https://leetcode.com/problems/find-the-duplicate-number/) | = Floyd 環偵測（同 141），lockdep 的數學基礎 |
| [283](./283-move-zeroes/283-move-zeroes.c) [Move Zeroes](https://leetcode.com/problems/move-zeroes/) | = `mm/compaction.c` 記憶體碎片整理的抽象版 |
| [994](./994_Rotting%20Oranges/Rotting%20Oranges.c) [Rotting Oranges](https://leetcode.com/problems/rotting-oranges/) | = device probe propagation 的 BFS 擴散模型 |
| [215](./215_Kth%20Largest%20Element%20in%20an%20Array/215_Kth%20Largest%20Element%20in%20an%20Array.c) [Kth Largest](https://leetcode.com/problems/kth-largest-element-in-an-array/) | = CFS scheduler 選取 vruntime 最小 task 的思維 |

---

## 六、每日 Checklist（每題完成後確認）

1. ✅ **能手寫**：不看 IDE，白板上完整寫出 C code
2. ✅ **能口述**：2 分鐘清楚說明思路 + 為什麼選這個方法 + 複雜度
3. ✅ **能延伸**：知道 follow-up 怎麼答（e.g. Two Sum → 3Sum → sorted input → streaming）
4. ✅ **能連結**：Tier 1/2 的題目能說出 kernel 中對應場景 + 原始碼位置

---

> **Sprint 節奏建議**：
> - Sprint 1 的 Day 1–4 是最重要的四天，kernel 核心全在這裡
> - Sprint 2 的 Day 13–14 是模擬面試，務必嚴格計時
> - 如果某天做不完，優先跳過 Tier 4 的 Easy 題，Tier 1/2 絕不能跳
