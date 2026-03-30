# LeetCode 605: Can Place Flowers — Linux Kernel 實作對照

## 問題概述

給定一個布林陣列表示花圃狀態（`1` = 已佔用，`0` = 空），判斷是否能放置 `n` 朵花。
限制條件：相鄰位置不能同時有花（左右都必須為 `0`）。

**貪心策略**：從左到右掃描，遇到合法位置立即放置。

---

## 1. Linux 相關原始碼位置

### `lib/bitmap.c` — Bitmap Allocation & Scanning
位置：`linux/lib/bitmap.c`
與 605 最直接相關。實現了通用位元圖掃描，查找符合間距限制的連續空閒區域。

**相關函數**：
- `bitmap_find_free_region()` — 掃描位元圖尋找連續 N 個空閒位元，考慮對齊限制
- `__bitmap_find_next_free_region()` — 更底層的掃描邏輯，檢查相鄰位元狀態

**核心概念**：檢查目標位置及其相鄰位是否符合約束，類似花圃中檢查左右相鄰位置。

---

### `arch/x86/kernel/cpuid.c` & CPU Affinity
位置：`linux/kernel/sched/cpumask.c`
CPU 親和性位元遮罩使用類似掃描邏輯——檢查連續 CPU 核心的可用性，考慮隔離約束。

**相關函數**：
- `cpumask_next()` — 掃描有效 CPU，跳過被隔離或不可用的核心
- `find_next_bit()` — 通用位元掃描

---

### `mm/page_alloc.c` — Buddy Allocator
位置：`linux/mm/page_alloc.c`
Free list 管理時需檢查相鄰頁框是否可合併（類似檢查相鄰花位是否佔用）。

**概念相似處**：
- 掃描頁框陣列檢查相鄰狀態
- 貪心的合併策略（有空閒頁就立即合併，類似有合法位置就立即放置）

---

## 2. Kernel vs. LeetCode 實作對照

### LeetCode 605 解法（貪心掃描）

```c
bool canPlaceFlowers(int* flowerbed, int flowerBedSize, int n) {
    int placed = 0;
    for (int i = 0; i < flowerBedSize && placed < n; i++) {
        if (flowerbed[i] == 0) {
            // 檢查左側：i == 0 或左側為 0
            bool emptyLeft = (i == 0) || (flowerbed[i-1] == 0);
            // 檢查右側：i 為最後位置或右側為 0
            bool emptyRight = (i == flowerBedSize - 1) || (flowerbed[i+1] == 0);

            if (emptyLeft && emptyRight) {
                flowerbed[i] = 1;  // 貪心：立即放置
                placed++;
            }
        }
    }
    return placed >= n;
}
```

**掃描邏輯**：
1. 線性迭代陣列
2. 檢查當前位置及相鄰位置狀態（邊界特殊處理）
3. 滿足約束則立即動作（原地修改）
4. 累計計數

---

### Kernel `bitmap_find_free_region()` 實作

位置：`linux/lib/bitmap.c` (簡化版概念)

```c
int bitmap_find_free_region(unsigned long *bitmap, int bits, int order) {
    int nbits = 1 << order;  // 需要的連續位元數
    int nlongs = BITS_TO_LONGS(bits);

    for (int pos = 0; pos <= bits - nbits; pos++) {
        bool region_free = true;
        // 掃描該位置的連續 nbits 位
        for (int i = pos; i < pos + nbits; i++) {
            if (test_bit(i, bitmap)) {  // 檢查位元是否被佔用
                region_free = false;
                break;
            }
        }

        if (region_free) {
            // 貪心：立即標記整個區域為佔用
            for (int i = pos; i < pos + nbits; i++) {
                set_bit(i, bitmap);
            }
            return pos;
        }
    }
    return -ENOMEM;
}
```

**對應關係**：
| LeetCode 605 | Kernel bitmap.c |
|---|---|
| `flowerbed[i] == 0` | `!test_bit(i, bitmap)` |
| 檢查 `flowerbed[i-1]` 和 `flowerbed[i+1]` | 掃描相鄰位元狀態 |
| `flowerbed[i] = 1` | `set_bit()` 標記已佔用 |
| 邊界檢查 (`i == 0` or `i == size-1`) | 與 `bitmap_find_free_region()` 中的邊界檢查相同 |

---

### 主要差異

| 方面 | LeetCode 605 | Kernel bitmap |
|---|---|---|
| **資料結構** | 簡單 `int` 陣列 | 優化的 `unsigned long` 位圖 |
| **粒度** | 檢查單一元素 + 相鄰 | 檢查連續區域（可配置大小） |
| **邊界** | 簡單單層檢查 | 複雜多層對齐 & 邊界優化 |
| **複雜度** | O(n) 掃描 | 最壞 O(n) 但經過優化 |

**共通點**：
- 都是貪心左-右線性掃描
- 都檢查相鄰/相關狀態以驗證約束
- 都進行原地修改或標記

---

## 3. 主管 Code Review 角度考題

### Q1: 為何此解法能保證全局最優？
**預期方向**：
貪心正確性証明。學生應解釋：放置一朵花後，它不會阻擋之前位置的任何合法位置（已掃過），且對之後位置的影響是確定的（右側立即失效，但更右側受益可能性不變）。

**考查重點**：
- 能否証明貪心選擇不會導致後續選擇變差
- 理解「局部最優導致全局最優」的成立條件

---

### Q2: 與 `lib/bitmap.c` 中的 `bitmap_find_free_region()` 相比，如何優化到生產環境等級？
**預期方向**：
- 使用位元圖而非整數陣列（記憶體效率）
- 採用機器字（`unsigned long`）批量檢查，而非逐位迭代
- 考慮快取局部性：掃描應連續訪問記憶體
- 邊界情況預計算或手工展開迴圈

**考查重點**：
- 對實際硬體性能的理解
- 能否在簡單演算法與優化實現間取捨

---

### Q3: 多執行緒環境下如何保證安全？
**預期方向**：
- LeetCode 版本無同步機制，需加鎖（互斥體或原子操作）
- Kernel `bitmap.c` 使用原子操作（`atomic_set_bit()`）
- 討論鎖粒度：全陣列鎖 vs. 位元粒度鎖的權衡

**考查重點**：
- 並發程式設計認知
- 性能與安全的平衡

---

### Q4: 邊界條件處理的完整性檢查
**預期方向**：
應列舉：
- `i == 0`（無左鄰）：只需檢查右側
- `i == n-1`（無右鄰）：只需檢查左側
- 陣列長度為 1 的邊界情況
- `n == 0`（無需放置任何花）

**考查重點**：
- 細心度與系統思考能力
- 能否以測試用例驗證邊界

---

### Q5: 時間複雜度能否低於 O(n)？為什麼或為什麼不能？
**預期方向**：
不能。必須掃描整個陣列以確定有多少個合法位置。除非使用預計算結構（如線段樹），但對此問題無益。
Kernel 優化的重點不在漸近複雜度，而在常數因子和實際執行時間。

**考查重點**：
- 對演算法極限的理解
- 區分理論複雜度與工程優化

---

## 4. 面試加分總結

### 核心競爭力

1. **貪心演算法証明**
   能清晰説明為何「掃描時遇到合法位置立即放置」不會錯過最優方案。這展現邏輯嚴謹性。

2. **Kernel 記憶體管理知識**
   提及 `lib/bitmap.c` 與 `mm/page_alloc.c` 的相似掃描模式，說明自己理解核心層最佳實踐。

3. **性能優化意識**
   - 討論位元圖 vs. 整數陣列的記憶體效率
   - 批量檢查（機器字級）的 CPU 效率
   - 快取友好性

4. **並發安全考量**
   主動提及多執行緒場景的原子操作或鎖機制，展現系統層思考。

5. **完整邊界處理**
   細緻地列舉所有邊界情況及測試方法，說明工程嚴謹性。

### 加分話術

- *"This problem is conceptually similar to `bitmap_find_free_region()` in the kernel, which scans for contiguous free regions while respecting alignment constraints."*
- *"The greedy choice is optimal because we're scanning left-to-right, and placing a flower as early as possible never blocks a valid earlier position."*
- *"In production, I'd use bitfields and word-level operations for better cache locality and memory efficiency, similar to how the kernel bitmap allocator works."*
- *"For concurrency, atomic bit operations would replace simple assignment, ensuring thread-safe updates."*

---

## 5. Sources

- **Linux Kernel Source**:
  - [`lib/bitmap.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/bitmap.c) — Bitmap scanning and allocation
  - [`mm/page_alloc.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/mm/page_alloc.c) — Buddy allocator and page frame management
  - [`kernel/sched/cpumask.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/sched/cpumask.c) — CPU affinity and masking

- **LeetCode Problem**:
  - [Problem 605: Can Place Flowers](https://leetcode.com/problems/can-place-flowers/)

- **Reference Articles**:
  - "Understanding Linux Kernel Memory Management" — Memory allocation strategies
  - "Greedy Algorithms and Correctness Proofs" — Theoretical foundations

---

**最後提醒**：在面試中，深度理解一個問題比廣泛涉獵多個淺嘗更有說服力。以此題為入口，展現對貪心、記憶體管理、並發與效能最佳實踐的綜合掌握。
