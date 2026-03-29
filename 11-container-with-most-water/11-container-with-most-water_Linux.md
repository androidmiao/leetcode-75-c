# LeetCode 11: Container With Most Water × Linux 核心原始碼對照

## 問題概述

**LeetCode 11** 要求在一個數列中找到兩個元素，使其形成的容器能盛最多的水。

### 兩指標解法（Two-Pointer Pattern）

```python
def maxArea(height: List[int]) -> int:
    left, right = 0, len(height) - 1
    max_area = 0

    while left < right:
        # 計算當前容量
        current_area = min(height[left], height[right]) * (right - left)
        max_area = max(max_area, current_area)

        # 移動較短邊的指標
        if height[left] < height[right]:
            left += 1
        else:
            right -= 1

    return max_area
```

---

## 1. Linux 核心原始碼中的兩指標模式

### 1.1 Socket Buffer (SKB) 修剪 — `net/core/skbuff.c`

在 Linux 網路棧中，SKB（socket buffer）需要動態調整其有效資料的邊界。

```c
// net/core/skbuff.c - SKB trim 操作
void skb_trim(struct sk_buff *skb, unsigned int len)
{
    if (skb->len > len)
        __skb_trim(skb, len);
}

static inline void __skb_trim(struct sk_buff *skb, unsigned int len)
{
    if (unlikely(skb->data_len)) {
        skb->data_len -= (skb->len - len);
        // head 指標向前，tail 指標向後
    }
    skb->len = len;
}
```

**對標 LeetCode 11**：
- 左指標 `left` ⟷ SKB 的 `head` 指標（開始位置）
- 右指標 `right` ⟷ SKB 的 `tail` 指標（結束位置）
- 兩個指標同時向內縮小，維護 SKB 的有效範圍

### 1.2 頁面分配器 — `mm/page_alloc.c`

Linux 的伙伴分配器（Buddy Allocator）使用成對的指標/索引來管理記憶體區塊的合併與分割。

```c
// mm/page_alloc.c - buddy system simplification
static inline void __free_pages_ok(struct page *page,
                                    unsigned int order)
{
    unsigned long combined_pfn;
    unsigned long uninitialized_var(buddy_pfn);
    struct page *buddy;

    while (order < MAX_ORDER) {
        // 找尋 buddy block（使用位元運算的"兩指標"概念）
        buddy_pfn = __find_buddy_pfn(page_pfn, order);
        buddy = page + (buddy_pfn - page_pfn);

        if (!page_is_buddy(buddy, page, order))
            goto done_merging;

        // 合併兩個 buddy 區塊
        list_del(&buddy->lru);
        combined_pfn = buddy_pfn & page_pfn;
        page = page + (combined_pfn - page_pfn);
        order++;
    }
}
```

**對標 LeetCode 11**：
- 兩個指標代表相鄰的記憶體塊
- 向"中間"縮小（合併）時，維護不變量：結合的區塊必須相鄰
- 類似於兩指標策略中「移動較小邊」的貪心選擇

### 1.3 二元搜尋 — `lib/bsearch.c`

雖然二元搜尋不是嚴格的「兩指標」，但其左右邊界收縮思想一致。

```c
// lib/bsearch.c
void *bsearch(const void *key, const void *base, size_t num, size_t size,
              cmp_func_t cmp)
{
    size_t start = 0, end = num;
    int result;

    while (start < end) {
        size_t mid = start + (end - start) / 2;  // 避免整數溢位
        result = cmp(key, base + mid * size);

        if (result < 0)
            end = mid;  // 左邊界向中間縮小
        else if (result > 0)
            start = mid + 1;  // 右邊界向中間縮小
        else
            return (void *)base + mid * size;
    }
    return NULL;
}
```

**對標 LeetCode 11**：
- `start` / `end` 類似於 `left` / `right`
- 逐步縮小搜尋空間直到找到答案

### 1.4 TCP 滑動視窗 — `net/ipv4/tcp_input.c`

TCP 連線管理使用左右邊界指標來追蹤「已確認」和「已傳送」的資料範圍。

```c
// net/ipv4/tcp_input.c - simplified ACK processing
static void tcp_process_loss(struct sock *sk, struct sk_buff *skb, int flag)
{
    struct tcp_sock *tp = tcp_sk(sk);
    bool recovered = false;

    // snd_una 和 snd_nxt 形成 [left, right) 的滑動視窗
    // snd_una = 最後確認的序列號（左邊界）
    // snd_nxt = 下一個待發送的序列號（右邊界）

    if (after(tp->snd_una, tp->high_seq)) {
        // 左邊界已超過高序列號，說明所有封包已確認
        recovered = true;
    }
}

// tcp_input.c - ACK 確認時移動左邊界
static int tcp_ack(struct sock *sk, const struct sk_buff *skb, int flag)
{
    struct tcp_sock *tp = tcp_sk(sk);
    u32 prior_snd_una = tp->snd_una;

    if (after(ack, tp->snd_una)) {
        tp->snd_una = ack;  // 左指標向右移動（確認了更多資料）
    }

    // ... 其他邏輯 ...
}
```

**對標 LeetCode 11**：
- `snd_una`（左邊界）：已確認的序列號，類似於左指標
- `snd_nxt`（右邊界）：待發送的序列號，類似於右指標
- TCP 動態調整兩個邊界，LeetCode 則計算最大面積

---

## 2. 核心原始碼 vs. LeetCode 實作對照

| 面向 | LeetCode 11 | Linux 核心 |
|------|-----------|----------|
| **指標用途** | 尋找最大面積 | 管理緩衝區、記憶體或連線狀態 |
| **收縮策略** | 移動較短邊（貪心） | 移動已完成的邊界或尋找 buddy 塊 |
| **不變量** | `left < right`；計算面積 | `head <= tail`；緩衝區有效性 |
| **時間複雜度** | O(n) 單趟 | O(log n) ~ O(n) 依操作而異 |
| **空間複雜度** | O(1) | O(1) ~ O(log n) 依結構而異 |
| **整數溢位風險** | `height[i] * (right - left)` | 位址運算：`buddy_pfn`、`combined_pfn` |

### 2.1 貪心正確性的核心原因

**為什麼在 LeetCode 11 中移動較短邊是正確的？**

假設當前 `height[left] < height[right]`，若移動 `right`：
- 新面積 = `min(height[left], height[right-1]) × (right-1-left)`
- 寬度減小 1
- 高度可能不變或減小（因為 `height[left]` 仍是瓶頸）
- 面積必然不會增大

因此，只有移動 `left` 才有可能增大面積（尋找更高的左邊界）。

**在 Linux 核心中的類比**：
- TCP 中，只移動已確認的左邊界（`snd_una`），不丟棄未確認的資料
- SKB 修剪中，移動 `head` 時確保不丟失有效資料
- 伙伴分配器中，只合併相鄰的同級 buddy 塊，不能任意合併

---

## 3. 主管代碼審查角度的考題

### Q1: 為什麼移動較短邊？

**答案框架**：
```
貪心證明：若左邊較短，右邊寬度只會減小，且高度由左邊界決定，
所以新面積 ≤ 舊面積。只有移動左邊才有機會找到更高的值。
```

**核心思想**：單調性與搜尋空間縮小

### Q2: 如何與 TCP 視窗管理相關聯？

**答案框架**：
```
TCP 的 [snd_una, snd_nxt) 滑動視窗和 LeetCode 的 [left, right] 範圍
都遵循"邊界管理"原則。TCP 根據確認號移動左邊界，而 LeetCode 根據
高度比較移動指標。兩者都尋求在特定條件下的最優值。
```

### Q3: 整數溢位問題

**高風險代碼**：
```python
# 危險！可能溢位
area = height[left] * (right - left)
```

**Linux 核心的做法**：
```c
// lib/bsearch.c - 避免溢位的中點計算
size_t mid = start + (end - start) / 2;  // 而不是 (start + end) / 2

// net/ipv4 - 使用 after() 巨集比較序列號
#define after(a, b) ((int)((b)-(a)) < 0)
```

**改進方案**：
```python
# Python 沒有整數溢位，但在 C/C++ 中：
# 檢查 right - left 是否可能導致溢位
if height[left] > INT_MAX // (right - left):
    # 可能溢位的情況
    pass
```

### Q4: 快取局部性（Cache Locality）

**優勢**：
```
左右指標順序或逆序遍歷陣列，CPU 快取效率高（sequential access pattern）。
相比隨機訪問，命中率可提升 10-100 倍。
```

**Linux 核心最佳實踐**：
```c
// 好：順序訪問，快取友善
for (int i = 0; i < n; i++)
    process(data[i]);

// 差：隨機訪問，快取不友善
for (int i = 0; i < n; i++)
    process(data[random_indices[i]]);
```

### Q5: SIMD 優化

**思考**：
```
是否能用 SIMD（Single Instruction Multiple Data）來加速？
- 可以向量化計算多對高度的面積
- 但兩指標的依賴性（需知道前次結果）阻礙完全向量化
- 實務上：預計算所有可能的面積，然後用 SIMD max reduction
```

---

## 4. 面試加分總結

### 4.1 展示核心知識

**說法示例**：
> 「兩指標模式在 LeetCode 中用於尋找最優配對，在 Linux 核心中則用於管理
> 邊界。比如 TCP 的滑動視窗使用 `snd_una` 和 `snd_nxt` 兩個序列號來追蹤
> 已確認和待發送的範圍，原理與本題類似——都是通過貪心或區間管理來達成目標。」

### 4.2 展示工程思維

**說法示例**：
> 「在 Linux 核心中，這類指標操作需要考慮整數溢位（如伙伴分配器的
> `combined_pfn` 計算）、邊界條件（如 `lib/bsearch.c` 中避免 `(start+end)/2` 溢位）
> 和快取效率（順序訪問 vs 隨機訪問）。在實作 LeetCode 方案時，也應檢查
> `height[i] * width` 是否可能溢位，以及陣列訪問的模式。」

### 4.3 展示系統性思維

**說法示例**：
> 「時間複雜度 O(n) 是最優的，因為每個元素最多被訪問一次。空間複雜度 O(1)
> 則是因為只維護兩個指標。與二元搜尋（O(log n)）相比，本題無法進一步優化，
> 除非使用預計算或離線方案，但那會破壞輸入的通用性。」

---

## Sources / 參考資源

- [Linux Kernel Source - net/core/skbuff.c](https://github.com/torvalds/linux/blob/master/net/core/skbuff.c)
  SKB（Socket Buffer）的實作，展示網路堆疊中的指標管理

- [Linux Kernel Source - mm/page_alloc.c](https://github.com/torvalds/linux/blob/master/mm/page_alloc.c)
  伙伴分配器，展示記憶體管理中的兩指標/配對概念

- [Linux Kernel Source - lib/bsearch.c](https://github.com/torvalds/linux/blob/master/lib/bsearch.c)
  二元搜尋實作，展示邊界縮小與整數溢位預防

- [Linux Kernel Source - net/ipv4/tcp_input.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c)
  TCP 輸入處理，展示滑動視窗與序列號管理

- [LeetCode 11 - Container With Most Water](https://leetcode.com/problems/container-with-most-water/)
  題目與討論

- [CPU Cache and Memory Access Patterns](https://gallery.techexcellence.com/cache-locality/)
  快取局部性與效能影響

---

## 結語

「Container With Most Water」這道題雖然看似簡單，但其兩指標的貪心策略在 Linux 核心中的應用極為廣泛：
從 TCP 滑動視窗、SKB 緩衝管理，到伙伴分配器的記憶體合併，都遵循類似的邊界管理原則。
掌握這道題的核心思想，不僅能解題，更能理解現代作業系統如何高效地管理資源。
