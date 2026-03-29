# LeetCode 933: Number of Recent Calls — Implementation & Analysis

## English Version

### Overview

Problem 933 asks us to track incoming ping calls and return the count of calls within a 3000ms sliding window. Both the provided `.c` solution and the LeetCode editorial describe the same core algorithm—a **sliding window using a queue**—but implement it with different data structures.

The key insight is that since ping times are strictly increasing, expired calls always appear at the front of the queue, so we can efficiently remove them with a simple front pointer instead of searching.

---

### Implementation Comparison

#### Array-Based Approach (`.c` solution)
- **Data structure:** Fixed-size array `data[10000]` with two index pointers
- **front:** Points to the first valid (non-expired) element
- **rear:** Points to the next available write position
- **Queue size:** Calculated as `rear - front`
- **Memory model:** Single pre-allocated block, no per-element allocation

```c
typedef struct {
    int data[10000];
    int front;
    int rear;
} RecentCounter;
```

**Operation flow:**
1. Enqueue: Insert at `data[rear++]`
2. Dequeue: Increment `front` pointer (logical deletion)
3. Count: Return `rear - front`

---

#### Linked-List Approach (Editorial / `_linked-list.c`)
- **Data structure:** Dynamically allocated nodes linked together
- **head:** Pointer to the first (oldest) valid call
- **tail:** Pointer to the last (newest) call
- **size:** Explicit counter for the number of valid elements
- **Memory model:** Per-element malloc/free on every ping

```c
typedef struct Node {
    int time;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} RecentCounter;
```

**Operation flow:**
1. Enqueue: Allocate new Node, append at tail
2. Dequeue: Free expired nodes from head, update pointers
3. Count: Return `size` (or traverse and count)

---

### Which Approach is Better?

| Factor | Array-Based | Linked-List-Based |
|--------|-------------|-------------------|
| **Time Complexity (ping)** | O(1) amortized | O(1) amortized |
| **Space Complexity** | O(1) / O(3000) constant | O(1) / O(3000) constant |
| **Cache Locality** | Excellent (sequential memory) | Poor (scattered pointers) |
| **Memory Overhead per Call** | None (pre-allocated) | malloc + Node overhead |
| **Malloc/Free Calls** | Single (create/destroy only) | Per ping (allocate new node, free expired) |
| **Code Simplicity** | Very simple (index arithmetic) | More complex (pointer chasing) |
| **CPU Performance** | Faster (better cache utilization) | Slower (cache misses, malloc overhead) |
| **Interview Suitability (C)** | **Better** — demonstrates data structure understanding | Good but heavier-weight |
| **Implementation Difficulty** | **Easier** in C | **Harder** in C (manual memory management) |

---

### Baseline vs. Recommended for C Interviews

**Baseline (from Editorial):**
- LinkedList/deque approach
- Conceptually cleaner (explicit enqueue/dequeue)
- Language-agnostic

**Recommended for C Interviews:**
- Fixed-size array with front/rear indices
- **Why?**
  1. **Simpler code in C** — avoids verbose malloc/free boilerplate
  2. **Dramatically faster** — memory is cache-friendly and avoids allocation overhead
  3. **Problem-appropriate** — window size is bounded (max 3000), so pre-allocation is justified
  4. **Better demonstrates systems thinking** — shows awareness of memory layout and performance trade-offs

In C, the linked-list approach requires careful malloc/free management on every ping operation, introducing non-trivial overhead and complexity. The array-based approach is not just more efficient; it's also **cleaner and more maintainable** in C.

---

### Key Algorithmic Insight

Both approaches rely on a critical problem constraint:
> "It is guaranteed that every call to ping uses a strictly larger value of t than before."

This means:
- Calls arrive in **strictly increasing order**
- Expired calls **only appear at the front** of the queue
- **No sorting or searching needed** — just advance the front pointer

Without this guarantee, neither approach would work, and we'd need binary search or a more complex data structure (e.g., balanced BST or segment tree).

---

### Time Complexity Details

**Single ping() call:**
- Worst case: O(3000) — when many old calls expire at once
- But per the editorial: O(3000) = O(1) because 3000 is a constant bound

**Amortized analysis:**
- Each call is added to the queue once via `rear++`
- Each call is removed once via `front++`
- Total operations across all n pings: O(n), so O(1) per ping

**Space Complexity:**
- Theoretical: O(n) — can store all previous calls
- Practical: O(1) — window bounded by 3000ms difference

---

### Performance Notes

**Array approach advantages:**
- One contiguous block of memory = better CPU cache utilization
- No allocation/deallocation overhead
- Faster in practice by orders of magnitude

**Linked-list approach advantages:**
- Philosophically aligns with "queue" abstraction
- Doesn't pre-commit to fixed size
- Language-portable (Python, Java naturally prefer deque)

For C, especially in interviews, the array approach is **strongly preferred**.

---

## 繁體中文版本

### 概覽

LeetCode 933 要求我們追蹤進來的 ping 呼叫，並回傳在 3000ms 滑動視窗內的呼叫計數。提供的 `.c` 解法與 LeetCode 官方編輯的解法描述同一個核心演算法——**使用佇列的滑動視窗**——但採用不同的資料結構實現。

關鍵洞見是：既然 ping 時間嚴格遞增，過期的呼叫一定只出現在佇列的前端，所以我們可以用簡單的前端指標有效地移除它們，無需搜尋。

---

### 實現比較

#### 陣列型方法（`.c` 解法）
- **資料結構：** 固定大小陣列 `data[10000]` 搭配兩個索引指標
- **front：** 指向第一個有效（未過期）的元素
- **rear：** 指向下一個可寫入的位置
- **佇列大小：** 計算為 `rear - front`
- **記憶體模型：** 單一預先配置的記憶體塊，無逐元素配置

```c
typedef struct {
    int data[10000];
    int front;
    int rear;
} RecentCounter;
```

**操作流程：**
1. 入隊：在 `data[rear++]` 插入
2. 出隊：遞增 `front` 指標（邏輯刪除）
3. 計數：回傳 `rear - front`

---

#### 鏈結串列型方法（編輯社方法 / `_linked-list.c`）
- **資料結構：** 動態配置的節點相互鏈結
- **head：** 指向第一個（最舊）有效呼叫的指標
- **tail：** 指向最後一個（最新）呼叫的指標
- **size：** 有效元素數量的明確計數器
- **記憶體模型：** 每次 ping 都執行逐元素的 malloc/free

```c
typedef struct Node {
    int time;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} RecentCounter;
```

**操作流程：**
1. 入隊：配置新節點，在 tail 後方附加
2. 出隊：釋放 head 的過期節點，更新指標
3. 計數：回傳 `size`（或遍歷並計數）

---

### 哪種方法更好？

| 因素 | 陣列型 | 鏈結串列型 |
|------|--------|------------|
| **時間複雜度 (ping)** | O(1) 攤銷 | O(1) 攤銷 |
| **空間複雜度** | O(1) / O(3000) 常數 | O(1) / O(3000) 常數 |
| **快取親和性** | 優秀（連續記憶體） | 差（分散的指標） |
| **每次呼叫的記憶體開銷** | 無（預先配置） | malloc + 節點開銷 |
| **Malloc/Free 呼叫** | 單次（僅建立/銷毀） | 每次 ping（配置新節點、釋放過期節點） |
| **程式碼簡潔度** | 非常簡單（索引算術） | 更複雜（指標追蹤） |
| **CPU 效能** | 更快（更好的快取使用） | 較慢（快取遺漏、malloc 開銷） |
| **面試適用性 (C)** | **更佳** — 展示資料結構理解 | 好但更重量級 |
| **實現難度** | **更簡單** 在 C 中 | **更困難** 在 C 中（手動記憶體管理） |

---

### 基準 vs. C 面試推薦方案

**基準（來自編輯社方法）：**
- 鏈結串列/deque 方法
- 概念上更乾淨（明確的入隊/出隊）
- 與語言無關

**C 面試推薦：**
- 搭配 front/rear 索引的固定大小陣列
- **為什麼？**
  1. **在 C 中程式碼更簡單** — 避免冗長的 malloc/free 樣板
  2. **效能大幅提升** — 記憶體對快取友善，避免配置開銷
  3. **適合問題特性** — 視窗大小有界（最多 3000），預先配置合理
  4. **更好地展示系統思維** — 展現對記憶體配置和效能權衡的認識

在 C 中，鏈結串列方法在每次 ping 操作都需要謹慎的 malloc/free 管理，引入非平凡的開銷和複雜性。陣列方法不僅更有效率，在 C 中也**更乾淨、更易維護**。

---

### 關鍵演算法洞見

兩種方法都依賴於一個關鍵的問題保證：
> "It is guaranteed that every call to ping uses a strictly larger value of t than before."

這代表：
- 呼叫按**嚴格遞增順序**到達
- 過期呼叫**僅出現在佇列前端**
- **無需排序或搜尋** — 只需推進前端指標

沒有此保證，兩種方法都不適用，我們需要二分搜尋或更複雜的資料結構（如平衡二叉樹或線段樹）。

---

### 時間複雜度細節

**單次 ping() 呼叫：**
- 最差情況：O(3000) — 當許多舊呼叫同時過期
- 但根據編輯社方法：O(3000) = O(1)，因為 3000 是常數上界

**攤銷分析：**
- 每個呼叫透過 `rear++` 加入佇列一次
- 每個呼叫透過 `front++` 移除一次
- 所有 n 次 ping 的總操作：O(n)，所以每次 ping 平均 O(1)

**空間複雜度：**
- 理論：O(n) — 可儲存所有先前呼叫
- 實際：O(1) — 視窗受 3000ms 時間差限制

---

### 效能注記

**陣列方法優勢：**
- 一個連續記憶體塊 = 更好的 CPU 快取利用
- 無配置/解除配置開銷
- 實際上快數個數量級

**鏈結串列方法優勢：**
- 在哲學上符合「佇列」抽象
- 無需預先承諾固定大小
- 語言可移植（Python、Java 天然偏好 deque）

對 C 而言，特別是在面試中，陣列方法**強烈推薦**。
