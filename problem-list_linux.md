
206. Reverse Linked List
80.4%
Easy

21. Merge Two Sorted Lists
68.0%
Easy

141. Linked List Cycle
54.0%
Easy

704. Binary Search
60.6%
Easy

215. Kth Largest Element in an Array
68.8%
Med.

3. Longest Substring Without Repeating Characters
38.7%
Med.

76. Minimum Window Substring
47.1%
Hard

1. Two Sum
57.2%
Easy

238. Product of Array Except Self
68.7%
Med.

239. Sliding Window Maximum
48.5%
Hard

480. Sliding Window Median
39.0%
Hard

347. Top K Frequent Elements
66.1%
Med.

692. Top K Frequent Words
60.0%
Med.

19. Remove Nth Node From End of List
51.2%
Med.

876. Middle of the Linked List
81.7%
Easy

234. Palindrome Linked List
57.6%
Easy

160. Intersection of Two Linked Lists
63.4%
Easy

35. Search Insert Position
50.9%
Easy

162. Find Peak Element
46.8%
Med.

33. Search in Rotated Sorted Array
44.3%
Med.

567. Permutation in String
48.6%
Med.

424. Longest Repeating Character Replacement
59.2%
Med.

49. Group Anagrams
72.3%
Med.

217. Contains Duplicate
64.2%
Easy

219. Contains Duplicate II
51.0%
Easy

242. Valid Anagram
67.9%
Easy

703. Kth Largest Element in a Stream
60.8%
Easy

23. Merge k Sorted Lists
59.1%
Hard

295. Find Median from Data Stream
54.3%
Hard

53. Maximum Subarray
53.1%
Med.

121. Best Time to Buy and Sell Stock
56.5%
Easy

122. Best Time to Buy and Sell Stock II
70.8%
Med.

152. Maximum Product Subarray
36.1%
Med.

20. Valid Parentheses
43.9%
Easy

739. Daily Temperatures
68.5%
Med.

155. Min Stack
57.9%
Med.

84. Largest Rectangle in Histogram
49.5%
Hard

200. Number of Islands
64.0%
Med.

133. Clone Graph
64.9%
Med.

207. Course Schedule
51.0%
Med.

210. Course Schedule II
55.1%
Med.

994. Rotting Oranges
58.3%
Med.

146. LRU Cache
47.0%
Med.

232. Implement Queue using Stacks
69.5%
Easy




@@@
@leecode-75-c 請分析整理並分類所有題目, 哪些是linux kernel中實際會遇到的？以Google面試角度,我會優先準備這些, 並請整理出十天複習完的衝刺計劃


Now let me build the comprehensive analysis and sprint plan.完成了！我把你 repo 裡全部 71 道題目做了完整的分析、分類、Kernel 關聯標記，以及十天衝刺計畫。

核心結論：

**Kernel 最直接相關的前 9 題**（Tier 1 必練）：146 LRU Cache、206 Reverse Linked List、21 Merge Two Sorted Lists、141 Linked List Cycle、207/210 Course Schedule I & II、23 Merge k Sorted Lists、232 Queue using Stacks、700 Search in a BST。這些題目每一道都能直接對應到 kernel 中的 `list_head`、`hlist_head`、`rb_tree`、`kfifo`、module dependency 等核心機制。

**十天計畫的設計邏輯**：前兩天全力打 Linked List（kernel 的根基），Day 3-4 攻 Hash Table + 拓撲排序（kernel 查找 & module 相依性），Day 5-6 是 Sliding Window + Heap（Google 高頻），Day 7-8 是 Graph + Monotonic Stack + DP，Day 9 掃尾，Day 10 模擬面試。

[查看完整計畫](computer:///Users/alex.miao/Documents/GitHub/leetcode-75-c/10-day-sprint-plan.md)