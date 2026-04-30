# 146. LRU Cache — Implementation vs. Editorial Comparison

## Implementation vs. Editorial Comparison

The `.c` solution in this folder implements **the same algorithm** as Editorial Approach 1 (Doubly Linked List + Hash Map). Both use:

- A doubly linked list with sentinel `head` and `tail` nodes to maintain access order, where `head->next` is the LRU end and `tail->prev` is the MRU end.
- A hash map for O(1) `key → node` lookup, so any node can be located without scanning the list.
- `removeNode` / `insertBeforeTail` helpers that correspond directly to the editorial's `remove` / `add`.
- Identical `get` / `put` semantics: hash-map lookup, move-to-tail on access/update, evict `head->next` when over capacity.

The only meaningful divergence from the editorial pseudocode is in the hash map itself. The editorial assumes a language-provided dictionary; in C we hand-roll a hash table with **separate chaining** (`HashNode` singly linked list per bucket). Direct-indexed `Node *table[HASH_SIZE]` without chaining would be wrong: two distinct keys hashing to the same bucket would silently overwrite each other, producing incorrect evictions and wrong return values.

Approach 2 in the editorial (built-in ordered maps such as `LinkedHashMap` / `OrderedDict` / `std::list + std::unordered_map`) is not realistic in standalone C, since the standard library provides no ordered map. It is mentioned only for completeness.

## Which Approach Is Better

| Criterion | Approach 1 (Manual DLL + Hash Map) | Approach 2 (Built-in Ordered Map) |
|---|---|---|
| Time complexity | O(1) get / put | O(1) get / put |
| Space complexity | O(capacity) | O(capacity) |
| Interview suitability | **Best** — demonstrates pointer mastery and structural design | Acceptable in Python/Java, but often viewed as "too easy" |
| C implementation difficulty | Moderate — manual pointer + hash management | **Not applicable** — C has no built-in ordered map |
| Debuggability | Explicit pointers; easy to draw on whiteboard | Hidden inside library implementation |

## Baseline vs. Recommended

- **Baseline approach:** Approach 2 (built-in `OrderedDict` / `LinkedHashMap`). Fastest to write in Python/Java for a quick first pass, but unavailable in C and weak as a signal of data-structure understanding.
- **Recommended approach:** Approach 1 (manual doubly linked list + hash map with separate chaining). This is the canonical interview answer at Google / Amazon / Meta. It directly demonstrates:
  1. Why a **doubly** linked list is required — given a node pointer, removal is O(1); a singly linked list would need O(n) to find the predecessor.
  2. Why **sentinel** `head` / `tail` nodes simplify edge cases (no `if (list_empty)` branches).
  3. How **hash collisions** are handled (separate chaining vs. open addressing trade-offs).
  4. Why the hash map must store **node pointers**, not values — eviction needs O(1) lookup of the LRU node's key to delete its hash entry.

For interview discussion, the recommended talking points are: (1) the two-data-structure invariant ("hash map for O(1) lookup, list for O(1) ordering"), (2) why the LRU end must be `head->next` rather than `head` itself (sentinels are not real entries), and (3) memory-management correctness — `lRUCacheFree` must free both the list nodes and every `HashNode` chain entry to avoid leaks.

---

# 146. LRU 快取 — 實作 vs. 官方題解比較

## 實作與題解比較

本資料夾中的 `.c` 解法實作的是與官方題解方法一（雙向鏈結串列 + 雜湊表）**完全相同的演算法**。兩者都使用：

- 帶有哨兵 `head` 和 `tail` 節點的雙向鏈結串列來維護存取順序；`head->next` 是 LRU 端，`tail->prev` 是 MRU 端。
- 雜湊表提供 O(1) 的 `key → 節點` 查找，使得任意節點都不需要掃描串列即可定位。
- `removeNode` / `insertBeforeTail` 輔助函式，分別對應題解中的 `remove` / `add`。
- 相同的 `get` / `put` 語意：雜湊表查找、存取/更新時移到 tail 端、超出容量時淘汰 `head->next`。

唯一與題解有意義的差異在雜湊表本身。官方題解假設語言內建字典；C 語言中我們手寫**鏈式雜湊（separate chaining）**（每個 bucket 一條 `HashNode` 單向鏈結串列）。若直接用 `Node *table[HASH_SIZE]` 無碰撞處理會出錯：兩個不同 key 雜湊到同一 bucket 會靜默互相覆蓋，造成錯誤淘汰與錯誤回傳值。

題解的方法二（內建有序 map，如 `LinkedHashMap` / `OrderedDict` / `std::list + std::unordered_map`）在 standalone C 中不適用，因為 C 標準函式庫沒有有序 map，僅作完整性提及。

## 哪種方法更好

| 比較項目 | 方法一（手動 DLL + 雜湊表） | 方法二（內建有序 map） |
|---|---|---|
| 時間複雜度 | O(1) get / put | O(1) get / put |
| 空間複雜度 | O(capacity) | O(capacity) |
| 面試適用性 | **最佳** — 展示指標掌握與結構設計能力 | Python/Java 可接受，但常被認為「太簡單」 |
| C 語言實作難度 | 中等 — 需手動管理指標與雜湊 | **不適用** — C 沒有內建有序 map |
| 可除錯性 | 指標清楚，方便白板畫圖 | 隱藏在函式庫內部 |

## 基準方法 vs. 推薦方法

- **基準方法：** 方法二（內建 `OrderedDict` / `LinkedHashMap`）。Python/Java 中是最快的第一版寫法，但 C 中不可用，且作為資料結構理解的訊號偏弱。
- **推薦方法：** 方法一（手動雙向鏈結串列 + 雜湊表 + 鏈式雜湊）。這是 Google / Amazon / Meta 的標準面試解法，直接展示：
  1. 為何需要**雙向**鏈結串列 — 給定節點指標可 O(1) 移除；單向鏈結串列需 O(n) 找前驅。
  2. 為何**哨兵** `head` / `tail` 節點能簡化邊界情況（不必判斷 `list_empty`）。
  3. **雜湊碰撞**處理（鏈式雜湊 vs. 開放定址法）的取捨。
  4. 為何雜湊表必須存**節點指標**而非值 — 淘汰時需要 O(1) 取得 LRU 節點的 key 才能刪除其雜湊條目。

面試討論的建議重點：(1) 雙資料結構不變條件（「雜湊表負責 O(1) 查找、串列負責 O(1) 排序」），(2) 為何 LRU 端是 `head->next` 而非 `head` 本身（哨兵不是真實條目），以及 (3) 記憶體管理正確性 — `lRUCacheFree` 必須同時釋放串列節點與每條 `HashNode` 鏈，以避免記憶體洩漏。
