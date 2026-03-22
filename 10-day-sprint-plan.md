# LeetCode 75 C — Linux Kernel 相關分析 & 十天衝刺計畫

> 以 Google 面試角度，針對 Linux Kernel 實際會用到的資料結構與演算法，整理出優先複習策略。

---

## 一、全部題目分類總覽

### 分類 A：Linked List（鏈結串列）— 共 10 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 206 | Reverse Linked List | Easy | 迭代/遞迴反轉 | ★★★ |
| 21 | Merge Two Sorted Lists | Easy | 雙指標合併 | ★★★ |
| 141 | Linked List Cycle | Easy | Floyd 快慢指標 | ★★★ |
| 876 | Middle of the Linked List | Easy | 快慢指標 | ★★ |
| 19 | Remove Nth Node From End of List | Med | 雙指標間距法 | ★★ |
| 160 | Intersection of Two Linked Lists | Easy | 雙指標交叉走 | ★★★ |
| 234 | Palindrome Linked List | Easy | 快慢 + 反轉 + 比較 | ★★ |
| 328 | Odd Even Linked List | Med | 奇偶重排 | ★★ |
| 2095 | Delete the Middle Node | Med | 快慢指標 | ★★ |
| 2130 | Maximum Twin Sum | Med | 快慢 + 反轉 | ★★ |

> **Kernel 關聯**：Linux kernel 大量使用 `list_head` 雙向環形鏈結串列（`<linux/list.h>`）。鏈結串列的遍歷、插入、刪除、合併、反轉都是 kernel 開發的日常操作。task_struct、wait queue、workqueue 全部用 linked list 串接。

---

### 分類 B：Hash Table（雜湊表）— 共 7 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 1 | Two Sum | Easy | Hash map 查找 | ★★ |
| 49 | Group Anagrams | Med | 排序鍵 + hash map | ★ |
| 217 | Contains Duplicate | Easy | Hash set | ★★ |
| 219 | Contains Duplicate II | Easy | 滑動視窗 + hash | ★★ |
| 242 | Valid Anagram | Easy | 頻率計數陣列 | ★ |
| 2215 | Find Difference of Two Arrays | Easy | Hash set 差集 | ★ |
| 1 | Two Sum（重複列出） | - | - | - |

> **Kernel 關聯**：Kernel 使用 `hlist_head` / `hlist_node` 實作 hash table（如 pid hash、inode hash、dentry cache）。理解 hash collision 處理（chaining）和 hash 函數設計對 kernel 很重要。

---

### 分類 C：Stack & Queue（堆疊與佇列）— 共 6 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 20 | Valid Parentheses | Easy | 堆疊匹配 | ★ |
| 155 | Min Stack | Med | 輔助堆疊 | ★ |
| 739 | Daily Temperatures | Med | 單調遞減堆疊 | ★ |
| 84 | Largest Rectangle in Histogram | Hard | 單調遞增堆疊 | ★ |
| 2390 | Removing Stars From a String | Med | 堆疊模擬 | ★ |
| 232 | Implement Queue using Stacks | Easy | 雙堆疊 | ★★★ |
| 933 | Number of Recent Calls | Easy | 佇列（環形緩衝區） | ★★★ |

> **Kernel 關聯**：Kernel 的 workqueue、kfifo（環形緩衝區）、中斷處理的 bottom-half 機制都基於 queue。`kfifo` 的 ring buffer 實作是經典考題。kernel stack 本身也極為關鍵（每個 thread 有固定大小的 kernel stack）。

---

### 分類 D：Binary Search（二分搜尋）— 共 4 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 704 | Binary Search | Easy | 標準二分 | ★★ |
| 35 | Search Insert Position | Easy | 左邊界二分 | ★★ |
| 162 | Find Peak Element | Med | 條件二分 | ★ |
| 33 | Search in Rotated Sorted Array | Med | 變形二分 | ★ |
| 374 | Guess Number Higher or Lower | Easy | 標準二分 | ★ |

> **Kernel 關聯**：`bsearch()` 在 kernel 中有直接提供（`<linux/bsearch.h>`）。page table lookup、memory region 查找（vm_area_struct 的 rb-tree 也含二分思維）。

---

### 分類 E：Sliding Window（滑動視窗）— 共 6 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 3 | Longest Substring Without Repeating | Med | Hash + 滑動視窗 | ★ |
| 567 | Permutation in String | Med | 固定大小視窗 | ★ |
| 424 | Longest Repeating Char Replacement | Med | 可變視窗 | ★ |
| 76 | Minimum Window Substring | Hard | 可變視窗 + 計數 | ★ |
| 239 | Sliding Window Maximum | Hard | 單調遞減 deque | ★★ |
| 480 | Sliding Window Median | Hard | 雙堆 | ★ |
| 643 | Maximum Average Subarray I | Easy | 固定視窗 | ★ |

> **Kernel 關聯**：TCP 的滑動視窗機制（congestion window、receive window）直接對應此概念。network packet 的流量控制就是 sliding window 的實戰應用。

---

### 分類 F：Heap / Priority Queue（堆積）— 共 5 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 215 | Kth Largest Element | Med | Quickselect / Heap | ★★ |
| 703 | Kth Largest in a Stream | Easy | Min-heap 大小 k | ★★ |
| 347 | Top K Frequent Elements | Med | Bucket sort / Heap | ★★ |
| 692 | Top K Frequent Words | Med | Min-heap + 比較器 | ★★ |
| 23 | Merge k Sorted Lists | Hard | Min-heap 合併 | ★★★ |
| 295 | Find Median from Data Stream | Hard | 雙堆 | ★★ |

> **Kernel 關聯**：CFS 排程器使用 rb-tree（本質上也是 priority queue 的思維）。timer wheel、I/O 排程器（deadline scheduler）都需要高效的優先級管理。kernel 本身雖沒用 binary heap，但 priority queue 的概念無處不在。

---

### 分類 G：Tree（樹）— 共 3 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 104 | Maximum Depth of Binary Tree | Easy | DFS 遞迴 | ★★ |
| 199 | Binary Tree Right Side View | Med | BFS 層序 | ★ |
| 700 | Search in a BST | Easy | BST 搜尋 | ★★★ |

> **Kernel 關聯**：rb-tree（紅黑樹）是 kernel 最常用的平衡樹（`<linux/rbtree.h>`），用於 CFS、vm_area_struct、epoll。radix tree 用於 page cache。BST 的基本操作是理解這些的前提。

---

### 分類 H：Graph（圖）— 共 6 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 200 | Number of Islands | Med | DFS 連通分量 | ★ |
| 133 | Clone Graph | Med | DFS + hash map | ★ |
| 207 | Course Schedule | Med | 拓撲排序 / 環偵測 | ★★★ |
| 210 | Course Schedule II | Med | 拓撲排序 | ★★★ |
| 994 | Rotting Oranges | Med | 多源 BFS | ★★ |
| 841 | Keys and Rooms | Med | DFS 可達性 | ★ |
| 1926 | Nearest Exit from Entrance in Maze | Med | BFS 最短路徑 | ★ |

> **Kernel 關聯**：Module dependency 就是 DAG 拓撲排序。`depmod` / `modprobe` 處理 kernel module 相依性時要偵測環和決定載入順序。device tree 的解析也涉及圖走訪。lock dependency（lockdep）用圖做死鎖偵測。

---

### 分類 I：Dynamic Programming（動態規劃）— 共 4 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 53 | Maximum Subarray | Med | Kadane's | ★ |
| 121 | Best Time to Buy/Sell Stock | Easy | 單次掃描 | ★ |
| 122 | Best Time to Buy/Sell Stock II | Med | 貪心 | ★ |
| 152 | Maximum Product Subarray | Med | 追蹤 max/min | ★ |

> **Kernel 關聯**：DP 概念在 kernel 不多見，但「保持局部最優解」的貪心思維在 memory allocator（buddy system）和 scheduler 中有應用。

---

### 分類 J：Two Pointers & Array（雙指標與陣列）— 共 10 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 283 | Move Zeroes | Easy | 雙指標原地 | ★★ |
| 11 | Container With Most Water | Med | 對撞雙指標 | ★ |
| 238 | Product of Array Except Self | Med | 前綴/後綴乘積 | ★ |
| 334 | Increasing Triplet Subsequence | Med | 貪心 + 雙變數 | ★ |
| 345 | Reverse Vowels of a String | Easy | 對撞雙指標 | ★ |
| 605 | Can Place Flowers | Easy | 貪心掃描 | ★ |
| 392 | Is Subsequence | Easy | 雙指標 | ★ |
| 1431 | Kids Greatest Candies | Easy | 線性掃描 | ★ |
| 1732 | Find the Highest Altitude | Easy | 前綴和 | ★ |
| 724 | Find Pivot Index | Easy | 前綴和 | ★ |
| 287 | Find the Duplicate Number | Med | Floyd 環偵測 | ★★ |
| 151 | Reverse Words in a String | Med | 雙指標反轉 | ★ |
| 1768 | Merge Strings Alternately | Easy | 雙指標 | ★ |
| 1071 | GCD of Strings | Easy | GCD 數學 | ★ |

> **Kernel 關聯**：陣列原地操作在 kernel buffer management 常見。compaction（記憶體整理）概念類似 move zeroes。

---

### 分類 K：Cache & 特殊資料結構 — 共 2 題

| # | 題目 | 難度 | 核心技巧 | Kernel 相關 |
|---|------|------|----------|:-----------:|
| 146 | LRU Cache | Med | 雙向鏈結 + hash map | ★★★ |
| 17 | Letter Combinations of a Phone Number | Med | 回溯法 | ★ |

> **Kernel 關聯**：LRU 是 kernel 的核心機制——page cache 的回收策略、slab allocator 的 reclaim、dentry cache 的淘汰全部基於 LRU 或其變體（如 LRU-2Q）。這是 kernel 面試必考題。

---

## 二、Linux Kernel 高度相關題目（按重要性排序）

以下是從 Google 面試 + Linux Kernel 工程師角度，最應該優先準備的題目：

### Tier 1：Kernel 核心機制直接對應（必練）

| 優先 | # | 題目 | 為什麼重要 |
|:----:|---|------|-----------|
| 1 | 146 | **LRU Cache** | page cache reclaim、dentry LRU、slab reclaim 的核心。面試幾乎必考 |
| 2 | 206 | **Reverse Linked List** | `list_head` 操作基本功，kernel patch review 常見 |
| 3 | 21 | **Merge Two Sorted Lists** | 合併有序結構（merge sort 在 kernel list sort 中使用） |
| 4 | 141 | **Linked List Cycle** | lockdep 死鎖偵測思維、kernel debug 常用 |
| 5 | 207 | **Course Schedule** | module dependency DAG、死鎖偵測的環偵測 |
| 6 | 210 | **Course Schedule II** | `depmod` 拓撲排序、systemd 服務啟動順序 |
| 7 | 23 | **Merge k Sorted Lists** | kernel 的 list_sort 就是 merge sort 變體 |
| 8 | 232 | **Implement Queue using Stacks** | 理解 kernel 的 kfifo、workqueue 基本概念 |
| 9 | 700 | **Search in a BST** | 理解 rb-tree 操作的前提 |

### Tier 2：Kernel 概念高度相關

| 優先 | # | 題目 | 為什麼重要 |
|:----:|---|------|-----------|
| 10 | 160 | **Intersection of Two Linked Lists** | list 交叉操作，kernel 中 list splicing |
| 11 | 704 | **Binary Search** | `bsearch()` 直接存在於 kernel API |
| 12 | 239 | **Sliding Window Maximum** | 類似 TCP window、scheduler 的時間窗口管理 |
| 13 | 1 | **Two Sum** | hash table 是 kernel 基礎（pid hash、inode hash） |
| 14 | 215 | **Kth Largest Element** | 排程器的 priority 選取思維 |
| 15 | 217 | **Contains Duplicate** | hash set 概念，duplicate detection |
| 16 | 994 | **Rotting Oranges** | BFS 在 kernel 的 device probe propagation |
| 17 | 287 | **Find the Duplicate Number** | Floyd 環偵測，同 141 |
| 18 | 876 | **Middle of the Linked List** | 快慢指標基本功 |
| 19 | 933 | **Number of Recent Calls** | ring buffer / kfifo 概念 |
| 20 | 104 | **Maximum Depth of Binary Tree** | 樹遍歷基本功 |

### Tier 3：Google 面試常考但 Kernel 關聯較弱

| # | 題目 | Google 面試重要度 |
|---|------|:----------------:|
| 3 | Longest Substring Without Repeating | ★★★ |
| 76 | Minimum Window Substring | ★★★ |
| 84 | Largest Rectangle in Histogram | ★★★ |
| 49 | Group Anagrams | ★★★ |
| 200 | Number of Islands | ★★★ |
| 295 | Find Median from Data Stream | ★★★ |
| 347 | Top K Frequent Elements | ★★★ |
| 155 | Min Stack | ★★ |
| 739 | Daily Temperatures | ★★ |
| 53 | Maximum Subarray | ★★ |

---

## 三、十天衝刺複習計畫

> 策略：前 5 天打穩 Kernel 核心 + Google 高頻，後 5 天攻破進階題型 + 模擬面試。
> 每天約 4～6 小時，7 題左右。每題包含：讀 code → 手寫一遍 → 說出思路（模擬白板）。

### Day 1：Linked List 基本功（Kernel 重中之重）
**主題**：`list_head` 操作的 LeetCode 版

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 206 | Reverse Linked List | 三指標迭代法，能閉眼寫出 |
| 2 | 21 | Merge Two Sorted Lists | dummy head 技巧 |
| 3 | 141 | Linked List Cycle | Floyd 快慢指標，解釋為何一定相遇 |
| 4 | 876 | Middle of the Linked List | 快慢指標變體 |
| 5 | 160 | Intersection of Two Linked Lists | 雙指標交叉走的數學證明 |
| 6 | 19 | Remove Nth Node From End | 間距法 + dummy head |
| 7 | 234 | Palindrome Linked List | 綜合：快慢 + 反轉 + 比較 + 還原 |

**複習要點**：每題用 C 手寫，注意 pointer 操作不要 segfault。能在白板上畫出每一步的指標變化圖。

---

### Day 2：Linked List 進階 + LRU Cache
**主題**：從基本操作到 Kernel 核心資料結構

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 328 | Odd Even Linked List | 奇偶拆分再合併 |
| 2 | 2095 | Delete the Middle Node | 快慢指標刪除 |
| 3 | 2130 | Maximum Twin Sum | 反轉後半 + 雙指標求和 |
| 4 | 23 | Merge k Sorted Lists | ★ 分治法合併（kernel list_sort） |
| 5 | **146** | **LRU Cache** | ★★★ 雙向鏈結 + hash map，O(1) get/put |

**複習要點**：LRU Cache 是今天的重頭戲。必須能完整手寫 doubly-linked list + hash map 的 C 實作，並解釋 kernel page cache 如何使用類似機制。

---

### Day 3：Hash Table + Binary Search
**主題**：Kernel 常用查找結構

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 1 | Two Sum | hash map 一次遍歷 |
| 2 | 217 | Contains Duplicate | hash set |
| 3 | 242 | Valid Anagram | 頻率陣列替代 hash |
| 4 | 49 | Group Anagrams | hash key 設計 |
| 5 | 704 | Binary Search | 標準模板（左閉右閉 vs 左閉右開） |
| 6 | 35 | Search Insert Position | 左邊界二分 |
| 7 | 33 | Search in Rotated Sorted Array | 變形二分的判斷邏輯 |

**複習要點**：C 語言手寫 hash table（open addressing 或 chaining）。二分搜尋的邊界條件是面試最常出錯的地方。

---

### Day 4：Stack & Queue + 拓撲排序
**主題**：Kernel 的 workqueue 與 module dependency

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 20 | Valid Parentheses | 堆疊基本應用 |
| 2 | 232 | Implement Queue using Stacks | amortized O(1) 分析 |
| 3 | 155 | Min Stack | 輔助堆疊設計 |
| 4 | 933 | Number of Recent Calls | 佇列 / ring buffer |
| 5 | 207 | Course Schedule | ★ DFS 三色環偵測 |
| 6 | 210 | Course Schedule II | ★ 拓撲排序輸出順序 |

**複習要點**：207/210 是 kernel module 相依性的直接映射。能解釋 `depmod` 為什麼需要拓撲排序，以及 lockdep 如何用有向圖偵測死鎖。

---

### Day 5：Sliding Window（全系列）
**主題**：TCP 滑動視窗的演算法基礎

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 643 | Maximum Average Subarray I | 固定大小視窗入門 |
| 2 | 567 | Permutation in String | 固定視窗 + matches 計數 |
| 3 | 3 | Longest Substring Without Repeating | 可變視窗 + hash set |
| 4 | 424 | Longest Repeating Char Replacement | 可變視窗 + 頻率計數 |
| 5 | 76 | Minimum Window Substring | ★ 可變視窗的完整模板 |
| 6 | 239 | Sliding Window Maximum | ★ 單調 deque |

**複習要點**：掌握「擴右縮左」的統一模板。239 的 monotonic deque 是 Google 面試高頻題。

---

### Day 6：Heap / Priority Queue + Top-K
**主題**：排程器思維 + 面試高頻

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 703 | Kth Largest in a Stream | min-heap 大小 k |
| 2 | 215 | Kth Largest Element in an Array | Quickselect vs Heap 取捨 |
| 3 | 347 | Top K Frequent Elements | freq count + heap 或 bucket sort |
| 4 | 692 | Top K Frequent Words | heap + 自訂比較器 |
| 5 | 295 | Find Median from Data Stream | ★ 雙堆平衡 |
| 6 | 480 | Sliding Window Median | ★ 雙堆 + 延遲刪除 |

**複習要點**：C 語言手寫 min-heap（sift_up / sift_down）。Google 面試 heap 題非常多，必須熟練。

---

### Day 7：Graph + BFS/DFS
**主題**：Kernel 的 device tree / 連通性

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 200 | Number of Islands | DFS 連通分量 |
| 2 | 133 | Clone Graph | DFS + visited hash map |
| 3 | 994 | Rotting Oranges | 多源 BFS |
| 4 | 841 | Keys and Rooms | DFS 可達性 |
| 5 | 1926 | Nearest Exit from Entrance in Maze | BFS 最短路徑 |
| 6 | 104 | Maximum Depth of Binary Tree | 樹 DFS |
| 7 | 199 | Binary Tree Right Side View | 樹 BFS |
| 8 | 700 | Search in a BST | BST 搜尋 |

**複習要點**：圖的 adjacency list 在 C 中的實作。BFS 用 queue + visited、DFS 用 recursion 或 explicit stack。

---

### Day 8：Monotonic Stack + DP
**主題**：進階面試題型

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 739 | Daily Temperatures | 單調遞減堆疊 |
| 2 | 84 | Largest Rectangle in Histogram | ★ 單調遞增堆疊，Hard 經典 |
| 3 | 2390 | Removing Stars From a String | 堆疊模擬 |
| 4 | 53 | Maximum Subarray | Kadane's algorithm |
| 5 | 121 | Best Time to Buy and Sell Stock | 單次掃描追蹤 min |
| 6 | 122 | Best Time to Buy and Sell Stock II | 貪心：每段上漲都吃 |
| 7 | 152 | Maximum Product Subarray | 同時追蹤 max/min |

**複習要點**：84 是 Google 面試經典 Hard 題，必須能在 20 分鐘內寫完。Kadane's 要能變體（最大乘積、環形等）。

---

### Day 9：Two Pointers & Array（掃尾 + 查漏補缺）
**主題**：基礎操作確認 + 剩餘題目

| 順序 | # | 題目 | 重點 |
|:----:|---|------|------|
| 1 | 283 | Move Zeroes | 雙指標原地分區 |
| 2 | 11 | Container With Most Water | 對撞雙指標 + 貪心 |
| 3 | 238 | Product of Array Except Self | 前綴/後綴不用除法 |
| 4 | 287 | Find the Duplicate Number | Floyd 環偵測用在陣列 |
| 5 | 334 | Increasing Triplet Subsequence | 貪心雙變數 |
| 6 | 345 | Reverse Vowels | 對撞雙指標 |
| 7 | 219 | Contains Duplicate II | 滑動視窗 + hash |
| 8 | 17 | Letter Combinations of Phone Number | 回溯法 |

**複習要點**：這天以鞏固為主，確保簡單題能在 5 分鐘內寫完。287 的 Floyd 環偵測要和 141 串聯複習。

---

### Day 10：模擬面試日 + 剩餘 Easy 收尾
**主題**：計時模擬 + 口述練習

| 時段 | 內容 |
|------|------|
| 上午 | 速刷剩餘 Easy 題：605, 392, 1431, 1732, 724, 1768, 1071, 151, 374, 162, 2215 |
| 下午（模擬 1） | 隨機抽 1 Medium + 1 Hard，45 分鐘內完成 + 口述複雜度分析 |
| 下午（模擬 2） | 重做 Tier 1 中任意 3 題，計時 + 手寫（不看 code） |
| 晚上 | 回顧所有 ★★★ 題的 kernel 關聯，準備 behavioral + system design 的回答框架 |

**模擬面試推薦抽題池**：
- Medium：146 LRU Cache、207 Course Schedule、3 Longest Substring、347 Top K
- Hard：76 Minimum Window Substring、84 Largest Rectangle、23 Merge k Sorted Lists、239 Sliding Window Maximum、295 Find Median

---

## 四、每日複習 Checklist

每題完成後，確認以下四點：

1. **能手寫**：不看 IDE，在白紙或白板上完整寫出 C code
2. **能口述**：用 2 分鐘清楚說明思路、為什麼選這個方法、複雜度
3. **能延伸**：知道 follow-up 怎麼答（例如 Two Sum → 3Sum → 4Sum）
4. **能連結 Kernel**：對 Tier 1/2 的題目，能說出 kernel 中對應的使用場景

---

## 五、速查：Kernel 資料結構 ↔ LeetCode 映射

| Kernel 結構 | 對應 LeetCode | 說明 |
|------------|--------------|------|
| `list_head` (doubly-linked circular list) | 206, 21, 141, 146, 23 | kernel 最常用的資料結構 |
| `hlist_head` (hash table chaining) | 1, 217, 146 | pid hash, inode hash, dentry cache |
| `rb_root` / `rb_node` (red-black tree) | 700, 215 | CFS scheduler, vm_area_struct |
| `kfifo` (ring buffer / queue) | 232, 933 | workqueue, 中斷 bottom-half |
| `wait_queue_head` | 232 | process scheduling |
| module dependency graph | 207, 210 | depmod, modprobe |
| lockdep (lock dependency graph) | 141, 207 | 死鎖偵測 |
| page LRU lists | 146 | memory reclaim |
| `bsearch()` | 704, 35 | kernel 內建二分搜尋 |
| TCP sliding window | 239, 3, 76 | 網路子系統 |

---

> **最後提醒**：Google 面試不只考演算法。對 Kernel 工程師角色，面試官會期待你能把 LeetCode 解法連結到實際 kernel 場景（例如「這個 LRU 和 kernel 的 page reclaim 有什麼差別？」）。準備時，每做完一題，花 1 分鐘想想它在 kernel 的哪裡出現過。
