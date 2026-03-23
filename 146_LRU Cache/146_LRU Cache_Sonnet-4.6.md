# 146. LRU Cache — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The `.c` solution in this folder implements **the same algorithm** as Editorial Approach 1 (Doubly Linked List + Hash Map). Both use:

- A doubly linked list with sentinel `head` and `tail` nodes to maintain access order.
- A hash map for O(1) key → node lookup.
- `removeNode` / `insertBeforeTail` helpers that correspond exactly to the editorial's `remove` / `add`.
- The same `get` and `put` logic: look up in hash map, move-to-tail on access, evict `head->next` when over capacity.

**Key difference from the original stub:** The initial `.c` file used a flat array hash table (`Node *table[HASH_SIZE]`) with direct-mapped slots and **no collision handling**. Two different keys hashing to the same bucket would silently overwrite each other, causing incorrect evictions and wrong return values. The updated implementation uses **separate chaining** (a `HashNode` singly linked list per bucket) to correctly handle hash collisions.

## Which Approach Is Better

| Criterion | Approach 1 (Manual DLL + Hash Map) | Approach 2 (Built-in Data Structures) |
|---|---|---|
| Time complexity | O(1) get/put | O(1) get/put |
| Space complexity | O(capacity) | O(capacity) |
| Interview suitability | **Best** — demonstrates understanding of data structures | Acceptable in Python/Java, but may be seen as "too easy" |
| C implementation difficulty | Moderate — must manage pointers and memory manually | **Not applicable** — C has no built-in ordered map |

## Baseline vs. Recommended

- **Baseline approach:** Approach 2 (built-in OrderedDict / LinkedHashMap). Quick to code in Python/Java, but not available in C and doesn't demonstrate deep understanding.
- **Recommended approach:** Approach 1 (manual doubly linked list + hash map). This is the standard interview solution. It directly demonstrates mastery of pointer manipulation, sentinel-node technique, and hash map design — all critical skills for a systems-level interview. In C specifically, this is the **only viable approach** since there are no built-in ordered containers.

For interview discussion, the recommended talking points are: (1) why a doubly linked list is needed (O(1) removal given a node pointer) vs. a singly linked list (which would need O(n) to find the predecessor), (2) why sentinel nodes simplify edge cases, and (3) hash collision handling strategy (separate chaining vs. open addressing).

---

# 146. LRU 快取 — 實作 vs. 官方題解比較

## 實作與題解比較

本資料夾中的 `.c` 解法實作的是與官方題解方法一（雙向鏈結串列 + 雜湊表）**完全相同的演算法**。兩者都使用：

- 帶有哨兵 `head` 和 `tail` 節點的雙向鏈結串列來維護存取順序。
- 雜湊表提供 O(1) 的 key → 節點查找。
- `removeNode` / `insertBeforeTail` 輔助函式，分別對應題解中的 `remove` / `add`。
- 相同的 `get` 和 `put` 邏輯：在雜湊表中查找、存取時移至尾端、超出容量時淘汰 `head->next`。

**與原始版本的關鍵差異：** 原始的 `.c` 檔案使用平坦陣列雜湊表（`Node *table[HASH_SIZE]`），以直接映射方式存取且**沒有碰撞處理**。兩個不同的 key 若雜湊到同一個 bucket，會靜默地互相覆蓋，導致錯誤的淘汰和回傳值。更新後的實作使用**鏈式雜湊（separate chaining）**（每個 bucket 一條 `HashNode` 單向鏈結串列）來正確處理雜湊碰撞。

## 哪種方法更好

| 比較項目 | 方法一（手動 DLL + 雜湊表） | 方法二（語言內建資料結構） |
|---|---|---|
| 時間複雜度 | O(1) get/put | O(1) get/put |
| 空間複雜度 | O(capacity) | O(capacity) |
| 面試適用性 | **最佳** — 展示對資料結構的深入理解 | Python/Java 中可接受，但可能被認為「太簡單」 |
| C 語言實作難度 | 中等 — 需手動管理指標和記憶體 | **不適用** — C 語言沒有內建的有序 map |

## 基準方法 vs. 推薦方法

- **基準方法：** 方法二（內建 OrderedDict / LinkedHashMap）。在 Python/Java 中快速編碼，但 C 中不可用，且無法展示深層理解。
- **推薦方法：** 方法一（手動雙向鏈結串列 + 雜湊表）。這是標準的面試解法。它直接展示了指標操作、哨兵節點技巧和雜湊表設計的掌握能力 — 這些都是系統級面試的關鍵技能。在 C 語言中，這是**唯一可行的方法**，因為沒有內建的有序容器。

面試討論的建議重點：(1) 為什麼需要雙向鏈結串列（給定節點指標可 O(1) 移除）而非單向鏈結串列（需要 O(n) 找前驅節點），(2) 哨兵節點如何簡化邊界情況，(3) 雜湊碰撞處理策略（鏈式雜湊 vs. 開放定址法）。




@https://leetcode.com/problems/lru-cache/description/?envType=problem-list-v2&envId=wl94y6ih https://leetcode.com/problems/lru-cache/editorial/?envType=problem-list-v2&envId=wl94y6ih 依據 SKILL.md 來初始化 146_LRU Cache

Let me fetch both LeetCode pages and check the repo structure.The folder exists but the markdown files are stubs. I need to fetch the LeetCode pages and fill everything in properly. Let me navigate to the Description page first.Good, I can see the Description page. Let me scroll down to get constraints and follow-up.I have all the Description content. Now let me navigate to the Editorial page.The page is still loading. Let me wait and try again.The Editorial page is loaded and accessible. Let me extract the full text and continue scrolling to capture all the editorial content.I've captured all the editorial content. The editorial has two approaches: (1) Doubly Linked List + Hash Map (manual implementation), and (2) Using Built-in Data Structures (like LinkedHashMap in Java, std::list in C++, OrderedDict in Python). Now I have enough information to create all the files. Let me also note a **critical bug** in the existing `.c` file — the hash table has no collision handling, which will fail on LeetCode.

Let me now create all the files properly.The existing `.c` has a critical bug: the hash table uses direct mapping (`table[hash(key)] = node`) with no collision handling. Two different keys that hash to the same index will silently overwrite each other. I need to fix this with chaining. Let me rewrite the `.c` file with proper hash collision handling.Now let me create the Description markdown and Editorial markdown in parallel.Now let me verify all files are complete.146_LRU Cache 初始化完成。以下是最終報告：

## 檔案清單

| 檔案 | 行數 | 說明 |
|---|---|---|
| `146_LRU Cache.c` | 234 | 完整 LeetCode 提交解法，含中文行內註解 + ASCII 示意圖 |
| `146_LRU Cache_Description.md` | 77 | 題目描述（英文 + 中文） |
| `146_LRU Cache_Editorial.md` | 165 | 官方題解兩種方法（英文 + 中文） |
| `146_LRU Cache_Sonnet-4.6.md` | 59 | 實作 vs. 題解比較 + 面試推薦（英文 + 中文） |

## 重要修正

原始 `.c` 檔有一個**嚴重 bug**：雜湊表使用直接映射（`Node *table[hash(key)]`），**沒有碰撞處理**。兩個不同 key 若 hash 值相同會互相覆蓋。已改為 **separate chaining**（每個 bucket 用 `HashNode` 單向鏈結串列處理碰撞）。

## 內容來源

Description 和 Editorial 內容均來自 **live authenticated LeetCode session**（瀏覽器已登入狀態直接讀取）。




@146_LRU Cache.c 使用Example 1, 畫「記憶體位址/陣列索引版」的詳細說明圖

Let me first read the C file and the project's SKILL.md to understand the implementation.Let me quickly verify the key transitions are correct by re-checking the pointer logic against the code:[View your diagram](computer:///Users/alex.miao/Documents/GitHub/leetcode-75-c/146_LRU%20Cache/146_LRU_Cache_Memory_Diagram.html)

圖中追蹤了 Example 1 全部 10 步操作，每一步都顯示：

- **假設記憶體位址**（0xB000, 0xC000, 0xD000...）標示每個 Node 和 HashNode 的位置
- **Hash Table 陣列索引**（index 0~4）和每個 bucket 指向的 HashNode chain
- **雙向鏈結串列**完整的 prev/next 指標值，包括 HEAD/TAIL 哨兵
- **指標修改細節**（removeNode 改了哪 2 個指標、insertBeforeTail 改了哪 4 個指標）
- **淘汰過程**（free 哪個 Node、移除哪個 HashNode）
- 最後附有**記憶體分配/釋放追蹤總表**和**指標修改次數統計**



@LRU 是 kernel 的核心機制——page cache 的回收策略、slab allocator 的 reclaim、dentry cache 的淘汰全部基於 LRU 或其變體（如 LRU-2Q）, 請列出Linux相關的code在哪裡？若以主管Code Review的角度, 可能會怎麼考?


@請把這個回覆記錄到146_LRU Cache_Linux.md
