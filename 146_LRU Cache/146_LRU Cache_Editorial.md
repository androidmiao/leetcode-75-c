# 146. LRU Cache - Editorial

## Solution

### Overview

The key challenge is implementing a data structure that stores key-value pairs like a hash map, but is limited to a fixed `capacity`. When full, we must evict the least recently used key — and to do that efficiently, we need to track the usage order of all keys.

A queue naturally maintains usage order: the front holds the least recently used key and the back holds the most recently used. When we access or insert a key, we move it to the back. When eviction is needed, we remove from the front.

Using an array or single linked list for the queue makes `get` and `put` cost O(n) because relocating an element requires a linear scan. The solution is to combine a **doubly linked list** with a **hash map** so that every operation is O(1).

---

### Approach 1: Doubly Linked List + Hash Map

#### Core Idea

Each node in the doubly linked list stores a key-value pair. The hash map maps each key to its corresponding node, giving O(1) lookup. Two sentinel (dummy) nodes — `head` and `tail` — eliminate edge-case handling for empty lists.

#### Data Structure

- `capacity` — maximum number of key-value pairs.
- `dic` (hash map) — maps keys to their linked-list nodes.
- `head` — sentinel node; `head.next` is the least recently used real node.
- `tail` — sentinel node; `tail.prev` is the most recently used real node.

#### Helper Methods

**`add(node)`** — Insert a node just before `tail` (mark it as most recently used):

1. Let `previousEnd = tail.prev`.
2. Set `previousEnd.next = node`, `node.prev = previousEnd`.
3. Set `node.next = tail`, `tail.prev = node`.

**`remove(node)`** — Unlink a node from the list without freeing it:

1. Set `node.prev.next = node.next`.
2. Set `node.next.prev = node.prev`.

#### `get(key)` Algorithm

1. If `key` is not in the hash map, return `-1`.
2. Otherwise, retrieve `node = dic[key]`.
3. Call `remove(node)` then `add(node)` to move it to the most-recently-used position.
4. Return `node.val`.

#### `put(key, value)` Algorithm

1. If `key` already exists in the hash map, call `remove` on the old node (we will re-insert a fresh node).
2. Create a new `node` with `(key, value)`.
3. Set `dic[key] = node` and call `add(node)`.
4. If `dic.size > capacity`:
   - Let `nodeToDelete = head.next` (the LRU node).
   - Call `remove(nodeToDelete)`.
   - Delete `nodeToDelete.key` from the hash map.

#### Complexity Analysis

- **Time complexity:** O(1) for both `get` and `put`.
  - `get`: hash map lookup O(1), `remove` O(1), `add` O(1).
  - `put`: hash map lookup/insert/delete O(1), `remove` O(1), `add` O(1).
- **Space complexity:** O(capacity). The hash map and linked list each hold at most `capacity` entries.

---

### Approach 2: Using Built-in Data Structures

#### Core Idea

Many languages provide ordered hash maps that maintain insertion order and support O(1) re-ordering:

- **Java:** `LinkedHashMap` — a hash map that also maintains a doubly linked list internally.
- **C++:** `std::list` + `std::unordered_map` — `std::list` is a doubly linked list; store iterators in the map for O(1) splice/erase.
- **Python:** `collections.OrderedDict` — a dict subclass that remembers insertion order and supports `move_to_end`.

The algorithms are identical to Approach 1; the only difference is that the linked-list bookkeeping is handled by the language's built-in data structure rather than manually.

#### Complexity Analysis

- **Time complexity:** O(1) for both `get` and `put` (same reasoning as Approach 1).
- **Space complexity:** O(capacity).

---

# 146. LRU 快取 - 官方題解

## 解法

### 概述

核心挑戰在於：需要一個類似雜湊表的結構來儲存 key-value 對，但容量固定為 `capacity`。當快取滿了時，必須淘汰最久未使用的 key，因此需要追蹤所有 key 的使用順序。

佇列天然適合維護使用順序：前端是最久未用的 key，後端是最近使用的。存取或插入 key 時將其移到後端，需要淘汰時從前端移除。

用陣列或單向鏈結串列實作佇列會使操作為 O(n)，因為重新定位元素需要線性掃描。解法是結合**雙向鏈結串列**與**雜湊表**，讓所有操作都是 O(1)。

---

### 方法一：雙向鏈結串列 + 雜湊表

#### 核心想法

雙向鏈結串列的每個節點儲存一組 key-value。雜湊表將每個 key 映射到對應節點，提供 O(1) 查找。兩個哨兵（dummy）節點 `head` 和 `tail` 消除了空串列的邊界判斷。

#### 資料結構成員

- `capacity` — key-value 對的最大數量。
- `dic`（雜湊表）— key → 鏈結串列節點的映射。
- `head` — 哨兵頭節點；`head.next` 是最舊的真實節點。
- `tail` — 哨兵尾節點；`tail.prev` 是最新的真實節點。

#### 輔助方法

**`add(node)`** — 將節點插入到 `tail` 之前（標記為最近使用）：

1. 令 `previousEnd = tail.prev`。
2. 設定 `previousEnd.next = node`，`node.prev = previousEnd`。
3. 設定 `node.next = tail`，`tail.prev = node`。

**`remove(node)`** — 從串列中摘除節點（不釋放記憶體）：

1. 設定 `node.prev.next = node.next`。
2. 設定 `node.next.prev = node.prev`。

#### `get(key)` 演算法

1. 若 `key` 不在雜湊表中，回傳 `-1`。
2. 取得 `node = dic[key]`。
3. 呼叫 `remove(node)` 再 `add(node)`，將其移至最近使用位置。
4. 回傳 `node.val`。

#### `put(key, value)` 演算法

1. 若 `key` 已存在於雜湊表，對舊節點呼叫 `remove`（稍後會重新插入新節點）。
2. 建立新的 `node`，內容為 `(key, value)`。
3. 設定 `dic[key] = node` 並呼叫 `add(node)`。
4. 若 `dic.size > capacity`：
   - 令 `nodeToDelete = head.next`（最久未用節點）。
   - 呼叫 `remove(nodeToDelete)`。
   - 從雜湊表中刪除 `nodeToDelete.key`。

#### 複雜度分析

- **時間複雜度：** `get` 和 `put` 均為 O(1)。
- **空間複雜度：** O(capacity)。雜湊表和鏈結串列各最多持有 `capacity` 個條目。

---

### 方法二：使用語言內建資料結構

#### 核心想法

許多語言提供有序雜湊表，維護插入順序並支援 O(1) 重新排序：

- **Java：** `LinkedHashMap` — 內部同時維護雜湊表和雙向鏈結串列。
- **C++：** `std::list` + `std::unordered_map` — `std::list` 是雙向鏈結串列，在 map 中存放迭代器以實現 O(1) 的 splice/erase。
- **Python：** `collections.OrderedDict` — 記住插入順序的字典子類別，支援 `move_to_end`。

演算法與方法一完全相同，差別僅在於鏈結串列的管理由語言內建結構代勞。

#### 複雜度分析

- **時間複雜度：** `get` 和 `put` 均為 O(1)（與方法一相同）。
- **空間複雜度：** O(capacity)。
