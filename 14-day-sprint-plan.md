# LeetCode 75 C — 雙 Sprint 衝刺計畫（14 天）

> Google 面試 × Linux Kernel 工程師角度，兩個 Sprint 複習完全部 73 題。
> Sprint 1：打穩 Kernel 核心 + Google 高頻基礎（Day 1–7）
> Sprint 2：進階題型 + 模式整合 + 模擬面試（Day 8–14）

---

## 一、全部題目分類 & Linux Kernel 相關性總覽

### 🔴 Tier 1：Kernel 核心機制直接對應（9 題）— 必須滾瓜爛熟

| # | 題目 | 難度 | 分類 | Kernel 對應 | 為什麼是 Tier 1 |
|---|------|:----:|------|------------|----------------|
| 146 | LRU Cache | Med | Cache | page cache reclaim, dentry LRU, slab reclaim | kernel 記憶體管理的靈魂，面試幾乎必考 |
| 206 | Reverse Linked List | Easy | Linked List | `list_head` 操作 | kernel 最基本的資料結構操作 |
| 21 | Merge Two Sorted Lists | Easy | Linked List | `list_sort()` merge 階段 | `lib/list_sort.c` 直接使用 |
| 141 | Linked List Cycle | Easy | Linked List | lockdep 死鎖偵測 | Floyd 環偵測 = 依賴圖環檢查的核心思維 |
| 23 | Merge k Sorted Lists | Hard | Heap + LL | `list_sort()` bottom-up merge | kernel 的 list_sort 就是 merge sort 變體 |
| 207 | Course Schedule | Med | Graph | module dependency 環偵測 | `depmod` / lockdep 的基礎 |
| 210 | Course Schedule II | Med | Graph | `depmod` 拓撲排序 | systemd 服務啟動順序、module 載入順序 |
| 232 | Queue using Stacks | Easy | Stack/Queue | kfifo, workqueue 概念 | 理解 kernel 的工作佇列機制 |
| 700 | Search in a BST | Easy | Tree | rb-tree 操作前提 | `rbtree.h` 是 kernel 最常用平衡樹 |

### 🟡 Tier 2：Kernel 概念高度相關（11 題）— 優先準備

| # | 題目 | 難度 | 分類 | Kernel 對應 |
|---|------|:----:|------|------------|
| 160 | Intersection of Two Linked Lists | Easy | Linked List | list splicing / 合併操作 |
| 876 | Middle of the Linked List | Easy | Linked List | 快慢指標是 list 操作基本功 |
| 704 | Binary Search | Easy | Binary Search | `bsearch()` 直接存在於 `<linux/bsearch.h>` |
| 35 | Search Insert Position | Easy | Binary Search | 左邊界搜尋，page table lookup 思維 |
| 1 | Two Sum | Easy | Hash Table | pid hash, inode hash, dentry cache |
| 217 | Contains Duplicate | Easy | Hash Table | hash set 概念，重複偵測 |
| 239 | Sliding Window Maximum | Hard | Sliding Window | TCP window, scheduler 時間窗口 |
| 933 | Number of Recent Calls | Easy | Queue | ring buffer / `kfifo` 概念 |
| 215 | Kth Largest Element | Med | Heap | CFS 排程器 priority 選取 |
| 994 | Rotting Oranges | Med | Graph/BFS | device probe propagation |
| 287 | Find the Duplicate Number | Med | Two Pointers | Floyd 環偵測（同 141） |

### 🟢 Tier 3：Google 面試高頻但 Kernel 關聯較弱（15 題）

| # | 題目 | 難度 | 分類 | Google 面試重要度 |
|---|------|:----:|------|:-----------------:|
| 3 | Longest Substring Without Repeating | Med | Sliding Window | ★★★ |
| 76 | Minimum Window Substring | Hard | Sliding Window | ★★★ |
| 84 | Largest Rectangle in Histogram | Hard | Monotonic Stack | ★★★ |
| 49 | Group Anagrams | Med | Hash Table | ★★★ |
| 200 | Number of Islands | Med | Graph/DFS | ★★★ |
| 295 | Find Median from Data Stream | Hard | Heap | ★★★ |
| 347 | Top K Frequent Elements | Med | Heap | ★★★ |
| 155 | Min Stack | Med | Stack | ★★ |
| 739 | Daily Temperatures | Med | Monotonic Stack | ★★ |
| 53 | Maximum Subarray | Med | DP | ★★ |
| 152 | Maximum Product Subarray | Med | DP | ★★ |
| 11 | Container With Most Water | Med | Two Pointers | ★★ |
| 238 | Product of Array Except Self | Med | Array/Prefix | ★★ |
| 133 | Clone Graph | Med | Graph/DFS | ★★ |
| 33 | Search in Rotated Sorted Array | Med | Binary Search | ★★ |

### 🔵 Tier 4：基礎練習 & 暖手題（38 題）

| # | 題目 | 難度 | 分類 |
|---|------|:----:|------|
| 19 | Remove Nth Node From End | Med | Linked List |
| 234 | Palindrome Linked List | Easy | Linked List |
| 328 | Odd Even Linked List | Med | Linked List |
| 2095 | Delete the Middle Node | Med | Linked List |
| 2130 | Maximum Twin Sum | Med | Linked List |
| 219 | Contains Duplicate II | Easy | Hash Table |
| 242 | Valid Anagram | Easy | Hash Table |
| 2215 | Find Difference of Two Arrays | Easy | Hash Table |
| 20 | Valid Parentheses | Easy | Stack |
| 2390 | Removing Stars From a String | Med | Stack |
| 162 | Find Peak Element | Med | Binary Search |
| 374 | Guess Number Higher or Lower | Easy | Binary Search |
| 567 | Permutation in String | Med | Sliding Window |
| 424 | Longest Repeating Char Replacement | Med | Sliding Window |
| 480 | Sliding Window Median | Hard | Sliding Window |
| 643 | Maximum Average Subarray I | Easy | Sliding Window |
| 703 | Kth Largest in a Stream | Easy | Heap |
| 692 | Top K Frequent Words | Med | Heap |
| 104 | Maximum Depth of Binary Tree | Easy | Tree |
| 199 | Binary Tree Right Side View | Med | Tree |
| 841 | Keys and Rooms | Med | Graph/DFS |
| 1926 | Nearest Exit in Maze | Med | Graph/BFS |
| 121 | Best Time to Buy/Sell Stock | Easy | DP |
| 122 | Best Time to Buy/Sell Stock II | Med | DP |
| 283 | Move Zeroes | Easy | Two Pointers |
| 334 | Increasing Triplet Subsequence | Med | Greedy |
| 345 | Reverse Vowels | Easy | Two Pointers |
| 392 | Is Subsequence | Easy | Two Pointers |
| 605 | Can Place Flowers | Easy | Greedy |
| 151 | Reverse Words in a String | Med | Two Pointers |
| 1768 | Merge Strings Alternately | Easy | Two Pointers |
| 1071 | GCD of Strings | Easy | Math |
| 1431 | Kids Greatest Candies | Easy | Array |
| 1732 | Find the Highest Altitude | Easy | Prefix Sum |
| 724 | Find Pivot Index | Easy | Prefix Sum |
| 17 | Letter Combinations of Phone Number | Med | Backtracking |

---

## 二、Kernel 資料結構 ↔ LeetCode 完整映射表

| Kernel 結構/機制 | 原始碼位置 | 對應 LeetCode 題 | 面試延伸話題 |
|-----------------|-----------|:----------------:|-------------|
| **`list_head`** 雙向環形鏈結串列 | `include/linux/list.h` | 206, 21, 141, 160, 234, 328, 876, 23, 146 | container_of macro、list_for_each_safe、為何用環形？ |
| **`hlist_head`** hash table chaining | `include/linux/hashtable.h` | 1, 217, 219, 146 | 為何 hlist 只有單向？節省 bucket 記憶體 |
| **`rb_root`/`rb_node`** 紅黑樹 | `include/linux/rbtree.h` | 700, 215, 295 | CFS 的 vruntime、vm_area_struct 查找 |
| **`kfifo`** 環形緩衝區 | `include/linux/kfifo.h` | 232, 933 | power-of-2 大小、無鎖單生產者/消費者 |
| **module dependency** DAG | `kernel/module/` | 207, 210 | depmod 如何建 DAG、modprobe 載入順序 |
| **lockdep** 鎖依賴圖 | `kernel/locking/lockdep.c` | 141, 207 | 偵測 ABBA 死鎖、lock class 概念 |
| **page LRU lists** 頁面回收 | `mm/vmscan.c` | 146 | active/inactive list、LRU-2Q、refault distance |
| **`bsearch()`** 二分搜尋 | `lib/bsearch.c` | 704, 35 | generic comparator 設計 |
| **TCP sliding window** | `net/ipv4/tcp_input.c` | 239, 3, 76, 643 | cwnd, rwnd, 流量控制 |
| **`list_sort()`** merge sort | `lib/list_sort.c` | 21, 23 | bottom-up 為何優於 top-down（cache + 無遞迴） |
| **buddy system** 夥伴系統 | `mm/page_alloc.c` | 283 (compaction 思維) | 記憶體碎片整理 = move zeroes 概念 |
| **workqueue** 工作佇列 | `kernel/workqueue.c` | 232, 933 | CMWQ、延遲工作、priority ordered |
| **epoll rb-tree** | `fs/eventpoll.c` | 700, 215 | epoll 如何用 rb-tree 管理 fd |
| **radix tree / XArray** | `include/linux/xarray.h` | 704 (搜尋思維) | page cache 的 index 查找 |

---

## 三、Sprint 1（Day 1–7）：Kernel 核心 + Google 基礎

> **目標**：完成所有 Tier 1 + Tier 2 + 大部分 Tier 3 題目
> **節奏**：每天 5–7 題，每題 = 讀 code → 手寫一遍 → 口述思路 → 說出 kernel 關聯

---

### Day 1：Linked List I — Kernel 的根基 `list_head`

> 🎯 目標：所有 linked list 基本操作能閉眼手寫，pointer 操作零失誤

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | 206 | Reverse Linked List | 三指標迭代法 prev/curr/next | `list_head` 反轉操作 |
| 2 | 21 | Merge Two Sorted Lists | dummy head + 雙指標合併 | `list_sort()` 的 merge 階段 |
| 3 | 141 | Linked List Cycle | Floyd 快慢指標，為何一定相遇 | lockdep 環偵測思維 |
| 4 | 876 | Middle of the Linked List | 快慢指標找中點 | list 分割操作前置步驟 |
| 5 | 160 | Intersection of Two Linked Lists | 雙指標交叉走法 + 數學證明 | list splicing 交叉操作 |
| 6 | 19 | Remove Nth Node From End | 間距法 + dummy head | list 刪除的安全操作 |
| 7 | 234 | Palindrome Linked List | 綜合：快慢+反轉+比較+還原 | 完整 list 操作組合技 |

📋 **Kernel 重點摘要**：
- `list_head` 是 kernel 最頻繁使用的資料結構，`task_struct`、`wait_queue`、`workqueue` 全部用它串接
- 核心 API：`list_add()`, `list_del()`, `list_move()`, `list_splice()`, `list_for_each_safe()`
- 面試加分點：解釋 `container_of` macro 如何從 `list_head` 取得外部 struct

---

### Day 2：Linked List II + LRU Cache — 從操作到設計

> 🎯 目標：LRU Cache 能完整手寫 doubly-linked list + hash map 的 C 實作

| 順序 | # | 題目 | 重點複習 | Kernel 連結 |
|:----:|---|------|---------|------------|
| 1 | 328 | Odd Even Linked List | 奇偶拆分再合併 | list 重排操作 |
| 2 | 2095 | Delete the Middle Node | 快慢指標定位 + 刪除 | list 刪除 |
| 3 | 2130 | Maximum Twin Sum | 反轉後半 + 雙指標求和 | list 操作組合 |
| 4 | 23 | **Merge k Sorted Lists** | ★ 分治合併 or min-heap | `list_sort()` bottom-up merge |
| 5 | **146** | **LRU Cache** | ★★★ O(1) get/put 完整實作 | page cache reclaim 核心機制 |

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
| 1 | 1 | Two Sum | hash map 一次遍歷 O(n) | pid hash, inode hash |
| 2 | 217 | Contains Duplicate | hash set 判重 | duplicate detection |
| 3 | 242 | Valid Anagram | 頻率陣列替代 hash | 計數技巧 |
| 4 | 49 | Group Anagrams | hash key 設計 | hash 函數設計 |
| 5 | 704 | Binary Search | 標準左閉右閉模板 | `lib/bsearch.c` |
| 6 | 35 | Search Insert Position | 左邊界二分 | page table lookup 思維 |
| 7 | 33 | Search in Rotated Sorted Array | 變形二分判斷邏輯 | — |

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
| 1 | 20 | Valid Parentheses | 堆疊匹配基礎 | — |
| 2 | 232 | Queue using Stacks | ★ amortized O(1) 分析 | kfifo, workqueue |
| 3 | 933 | Number of Recent Calls | 佇列 / ring buffer | `kfifo` 環形緩衝區 |
| 4 | 155 | Min Stack | 輔助堆疊追蹤最小值 | — |
| 5 | 207 | **Course Schedule** | ★ DFS 三色環偵測 | module dependency 環偵測 |
| 6 | 210 | **Course Schedule II** | ★ BFS Kahn's 拓撲排序 | depmod / modprobe 載入順序 |

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
| 1 | 643 | Maximum Average Subarray I | 固定視窗入門 | — |
| 2 | 567 | Permutation in String | 固定視窗 + matches 計數 | — |
| 3 | 3 | Longest Substring w/o Repeating | 可變視窗 + hash set | TCP receive window |
| 4 | 424 | Longest Repeating Char Replacement | 可變視窗 + 頻率計數 | — |
| 5 | 76 | **Minimum Window Substring** | ★ 可變視窗完整模板 | TCP congestion window |
| 6 | 239 | **Sliding Window Maximum** | ★ 單調 deque（Google 高頻） | scheduler 時間窗口 |

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
| 1 | 703 | Kth Largest in a Stream | min-heap 大小 k 基礎 | — |
| 2 | 215 | Kth Largest Element | Quickselect vs Heap 取捨 | CFS priority 選取 |
| 3 | 347 | Top K Frequent Elements | freq count + heap/bucket | — |
| 4 | 692 | Top K Frequent Words | heap + 自訂比較器 | — |
| 5 | 295 | **Find Median from Data Stream** | ★ 雙堆平衡 | — |
| 6 | 480 | **Sliding Window Median** | ★ 雙堆 + 延遲刪除 | — |

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
| 1 | 200 | Number of Islands | DFS 連通分量 | — |
| 2 | 133 | Clone Graph | DFS + visited hash map | — |
| 3 | 700 | **Search in a BST** | ★ BST 搜尋基本功 | rb-tree 操作前提 |
| 4 | 104 | Maximum Depth of Binary Tree | 樹 DFS 遞迴 | 樹遍歷基本功 |
| 5 | 199 | Binary Tree Right Side View | 樹 BFS 層序 | — |
| 6 | 994 | Rotting Oranges | 多源 BFS | device probe propagation |
| 7 | 841 | Keys and Rooms | DFS 可達性 | — |

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
| 1 | 739 | Daily Temperatures | 單調遞減堆疊入門 |
| 2 | 84 | **Largest Rectangle in Histogram** | ★ 單調遞增堆疊（Google Hard 經典） |
| 3 | 2390 | Removing Stars From a String | 堆疊模擬操作 |
| 4 | 53 | Maximum Subarray | Kadane's algorithm |
| 5 | 152 | Maximum Product Subarray | 同時追蹤 max/min |
| 6 | 121 | Best Time to Buy/Sell Stock | 單次掃描追蹤 min |

📋 **重點**：84 是 Google 面試經典 Hard，必須能在 20 分鐘內寫完。Kadane's 要能變體應用。

---

### Day 9：DP II + Two Pointers 基礎 — 掃描模式

> 🎯 目標：雙指標的四種模式（同向、對撞、快慢、間距）全部能分辨

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | 122 | Best Time to Buy/Sell Stock II | 貪心：每段上漲都吃 |
| 2 | 283 | Move Zeroes | 雙指標原地分區 |
| 3 | 11 | Container With Most Water | 對撞雙指標 + 貪心證明 |
| 4 | 287 | Find the Duplicate Number | Floyd 環偵測用在陣列 |
| 5 | 238 | Product of Array Except Self | 前綴/後綴乘積不用除法 |
| 6 | 334 | Increasing Triplet Subsequence | 貪心雙變數追蹤 |

📋 **Kernel 重點**：283 Move Zeroes 的概念 = memory compaction（記憶體碎片整理），`mm/compaction.c` 就是把使用中的 page 往一端移動，空閒 page 往另一端集中。287 的 Floyd 環偵測要和 Day 1 的 141 串聯複習。

---

### Day 10：剩餘 Two Pointers + String — 基礎鞏固

> 🎯 目標：簡單題 5 分鐘內寫完，確保零失誤

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | 345 | Reverse Vowels of a String | 對撞雙指標 |
| 2 | 392 | Is Subsequence | 雙指標順序匹配 |
| 3 | 151 | Reverse Words in a String | 全反轉 + 逐字反轉 |
| 4 | 1768 | Merge Strings Alternately | 雙指標交替合併 |
| 5 | 1071 | GCD of Strings | GCD 數學性質 |
| 6 | 219 | Contains Duplicate II | 滑動視窗 + hash |

---

### Day 11：剩餘 Easy 題速刷 + Graph 補完

> 🎯 目標：所有 Easy 確認無死角，Graph 的 BFS 最短路徑收尾

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | 605 | Can Place Flowers | 貪心掃描 |
| 2 | 1431 | Kids Greatest Candies | 線性掃描 |
| 3 | 1732 | Find the Highest Altitude | 前綴和 |
| 4 | 724 | Find Pivot Index | 前綴和 |
| 5 | 2215 | Find Difference of Two Arrays | hash set 差集 |
| 6 | 1926 | Nearest Exit in Maze | BFS 最短路徑 |
| 7 | 374 | Guess Number Higher or Lower | 標準二分（API 呼叫型） |

---

### Day 12：剩餘 Medium + 補強弱項

> 🎯 目標：Binary Search 進階 + Backtracking + 查漏補缺

| 順序 | # | 題目 | 重點複習 |
|:----:|---|------|---------|
| 1 | 162 | Find Peak Element | 條件二分（非單調） |
| 2 | 17 | Letter Combinations of Phone Number | 回溯法模板 |
| 3 | 回顧 | **Tier 1 中自己最弱的 3 題** | 不看 code 重寫 |
| 4 | 回顧 | **Tier 3 中自己最弱的 2 題** | 計時重寫 |

📋 **重點**：今天是彈性日。如果前面有卡住的題目，今天全力補強。

---

### Day 13：模擬面試 I — Google Phone Screen

> 🎯 目標：45 分鐘內完成 1 Medium + 口述 + follow-up

| 時段 | 內容 |
|------|------|
| 上午 模擬 1 | 抽一題 Medium（推薦池：146, 207, 3, 347, 239），45 分鐘計時手寫 |
| 上午 模擬 2 | 抽一題 Hard（推薦池：76, 84, 23, 295），45 分鐘計時 |
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

| 題目 | Kernel 一句話 |
|------|-------------|
| 146 LRU Cache | = `mm/vmscan.c` 的 page reclaim：active/inactive 雙 LRU list + `hlist` hash 查找 |
| 206 Reverse LL | = `list_head` 反轉，kernel patch 中最常見的指標操作 |
| 21 Merge Sorted | = `lib/list_sort.c` 的 merge 階段，比較器用 function pointer |
| 141 Cycle Detect | = lockdep 環偵測思維：有向圖中找 back edge |
| 23 Merge k Lists | = `list_sort()` bottom-up merge sort（不用遞迴，省 kernel stack） |
| 207 Course Sched | = `depmod` 建立 module dependency DAG + 環偵測 |
| 210 Course Sched II | = `modprobe` 用拓撲排序決定 module 載入順序 |
| 232 Queue/Stacks | = `kfifo` 環形緩衝區的概念簡化版 |
| 700 BST Search | = `rb_tree` 搜尋的簡化版（無旋轉平衡） |
| 704 Binary Search | = `lib/bsearch.c`，kernel 直接提供的 API |
| 239 Window Max | = TCP sliding window + scheduler 的時間窗口最優選取 |
| 1 Two Sum | = `hlist_head` hash table 查找，類似 pid/inode hash |
| 933 Recent Calls | = `kfifo` ring buffer 的 enqueue/dequeue |
| 287 Find Dup | = Floyd 環偵測（同 141），lockdep 的數學基礎 |
| 283 Move Zeroes | = `mm/compaction.c` 記憶體碎片整理的抽象版 |
| 994 Rotting Oranges | = device probe propagation 的 BFS 擴散模型 |
| 215 Kth Largest | = CFS scheduler 選取 vruntime 最小 task 的思維 |

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
