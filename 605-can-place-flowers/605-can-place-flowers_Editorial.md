# 605. Can Place Flowers — Editorial

## Approach #1: Single Scan (Greedy)

### Idea

Traverse the entire flowerbed array once. For each position that is currently empty (`0`), check whether both its left and right neighbours are also empty (or out of bounds). If so, we can safely plant a flower there. Mark the position as planted (`1`) and increment a counter. After the full scan, compare the counter against `n`.

The boundary positions deserve special treatment: the first element has no left neighbour and the last element has no right neighbour, so those neighbours are treated as empty by default.

### Algorithm

1. Initialise `count = 0`.
2. For each index `i` from `0` to `flowerbedSize - 1`:
   - If `flowerbed[i] == 0`:
     - Let `emptyLeft  = (i == 0) || (flowerbed[i-1] == 0)`
     - Let `emptyRight = (i == flowerbedSize-1) || (flowerbed[i+1] == 0)`
     - If both are true, set `flowerbed[i] = 1` and increment `count`.
3. Return `count >= n`.

### Complexity

| | |
|---|---|
| **Time** | O(n) — single pass over the array |
| **Space** | O(1) — only a counter variable |

---

## Approach #2: Optimized (Early Return)

### Idea

The same greedy scan as Approach #1, but with an **early termination**: as soon as `count` reaches `n`, return `true` immediately. This avoids scanning the rest of the array when we have already found enough plantable positions.

If the loop finishes without `count` reaching `n`, return `count >= n` (which will be `false`).

### Algorithm

1. Initialise `count = 0`.
2. For each index `i` from `0` to `flowerbedSize - 1`:
   - Same adjacency check as Approach #1.
   - After incrementing `count`, check `if (count >= n) return true`.
3. Return `count >= n`.

### Complexity

| | |
|---|---|
| **Time** | O(n) worst case, but may terminate early |
| **Space** | O(1) |

---

## 解題分析（繁體中文）

### 方法一：單次掃描（貪心法）

#### 核心想法

從頭到尾掃描花圃陣列一次。對每一個目前為空（`0`）的位置，檢查它的左右鄰居是否也為空（或超出邊界）。若三者皆為空，就在此位置種花，將該位置標記為 `1` 並累加計數器。掃描結束後，比較計數器與 `n` 的大小。

邊界情況需要特別處理：第一個元素沒有左鄰居、最後一個元素沒有右鄰居，這些不存在的鄰居一律視為空。

#### 演算法步驟

1. 初始化 `count = 0`。
2. 對每個索引 `i`（從 `0` 到 `flowerbedSize - 1`）：
   - 若 `flowerbed[i] == 0`：
     - 判斷左邊是否為空：`emptyLeft = (i == 0) || (flowerbed[i-1] == 0)`
     - 判斷右邊是否為空：`emptyRight = (i == flowerbedSize-1) || (flowerbed[i+1] == 0)`
     - 若兩者皆為真，將 `flowerbed[i]` 設為 `1`，`count++`。
3. 回傳 `count >= n`。

#### 複雜度

- **時間複雜度**：O(n) — 只掃描陣列一次。
- **空間複雜度**：O(1) — 只使用一個計數器變數。

---

### 方法二：最佳化（提前返回）

#### 核心想法

與方法一相同的貪心掃描，但加上**提前終止**的優化：一旦 `count` 達到 `n`，立即回傳 `true`，不必繼續掃描剩餘的陣列。

若迴圈結束後 `count` 仍未達到 `n`，則回傳 `false`。

#### 演算法步驟

1. 初始化 `count = 0`。
2. 對每個索引 `i`（從 `0` 到 `flowerbedSize - 1`）：
   - 與方法一相同的鄰居檢查。
   - 累加 `count` 後，立即檢查 `if (count >= n) return true`。
3. 回傳 `count >= n`。

#### 複雜度

- **時間複雜度**：O(n) 最壞情況，但可能提前終止。
- **空間複雜度**：O(1)。

---

*Editorial content sourced from the live authenticated LeetCode session.*
