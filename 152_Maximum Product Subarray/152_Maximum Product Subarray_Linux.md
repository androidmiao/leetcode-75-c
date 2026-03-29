# LeetCode 152: Maximum Product Subarray — Linux 核心程式碼對照

## 問題概述

**題目**：給定整數陣列，找出連續子陣列的最大乘積。

**關鍵演算法特性**：單次掃描，同時追蹤執行中的最大值和最小值（動態規劃狀態追蹤）。關鍵洞察在於負數乘以負數等於正數，因此必須同時追蹤兩種極值狀態。

**時間複雜度**：O(n)
**空間複雜度**：O(1)

---

## Linux 相關原始碼位置

### 1. `lib/minmax.c` 和 `include/linux/minmax.h`

Linux 核心的通用最小值、最大值宏和工具集。這些定義了 `min()`、`max()` 等基礎巨集，以及更進階的 `min_t()`、`max_t()` 型別安全版本。在許多核心子系統中被廣泛使用，尤其是需要追蹤資源約束和界限的地方。

**檔案位置**：
- 標頭檔：`include/linux/minmax.h`
- 實作：`lib/minmax.c`（核心實作細節）

### 2. `include/linux/minmax.h` — 滑動視窗最小值追蹤器

此標頭檔包含 `struct minmax_sample` 和相關函數，用於在滑動視窗內追蹤最小和最大值。此結構體在 TCP 壅塞控制演算法（如 TCP Westwood 和 BBR）中被使用，以追蹤往返時間（RTT）估計和頻寬計算。

**核心結構**：
```c
struct minmax_sample {
    u32 t;      /* time stamp */
    u32 v;      /* value */
};

struct minmax {
    struct minmax_sample s[3];  /* 最多三個樣本來追蹤最小/最大值 */
};
```

### 3. `net/ipv4/tcp_input.c` — 運行最小值/最大值追蹤

TCP 輸入處理邏輯，特別是在壅塞視窗和 RTT 估計中使用執行最小/最大追蹤。函數如 `tcp_update_rtt_min()` 和壅塞控制回呼使用類似於 LeetCode 152 的狀態追蹤模式。

**相關函數**：
- `tcp_rtt_estimator()`：追蹤執行 RTT 樣本的最小/最大值
- `tcp_update_rtt_min()`：更新執行最小 RTT
- BBR 壅塞控制（`net/ipv4/tcp_bbr.c`）：追蹤頻寬與 RTT 的最小/最大值

### 4. `net/ipv4/tcp_bbr.c` — BBR 壅塞控制演算法

BBR（Bottleneck Bandwidth and Round-trip time）演算法使用高度最佳化的最小值/最大值追蹤來估計網路瓶頸。它同時維護多個狀態變數來追蹤極值。

**相關程式碼模式**：
```c
/* BBR 追蹤執行的最小 RTT 和最大頻寬 */
struct bbr {
    u32 min_rtt_us;
    u32 min_rtt_stamp;
    u32 probe_rtt_done_stamp;
    u32 prior_cwnd;
    u32 bw_lo, bw_hi;  /* 頻寬的下界和上界 */
    /* ... 其他狀態變數 ... */
};
```

---

## Kernel vs. LeetCode 實作對照

### 演算法結構對比

在 LeetCode 152 中，我們掃描一次陣列並維持兩個狀態：

```
for i in range(len(nums)):
    current_max = max(nums[i], current_min * nums[i], current_max * nums[i])
    current_min = min(nums[i], current_min * nums[i], current_max * nums[i])
```

Linux 核心的滑動視窗最小值追蹤器採用類似的狀態維持策略，但用於時間序列資料而非陣列元素。在 TCP Westwood 中，演算法追蹤樣本時間戳記內的最小和最大值：

```c
/* TCP Westwood 中的概念性結構 */
void minmax_add(struct minmax *m, u32 t, u32 v) {
    /* 如果新樣本比現有的最小值更小，或時間已超過視窗 */
    if (v < m->min || is_beyond_window(t)) {
        /* 更新最小值狀態 */
        m->min = v;
    }
    if (v > m->max || is_beyond_window(t)) {
        /* 更新最大值狀態 */
        m->max = v;
    }
}
```

### 狀態追蹤與更新規則

**LeetCode 152 中的狀態追蹤**：

1. **狀態變數**：`current_max`（執行最大乘積）、`current_min`（執行最小乘積）
2. **更新規則**：每次迭代時，根據三個候選值更新：
   - 當前元素單獨（`nums[i]`）
   - 當前元素與執行最小值的乘積（可能翻轉為最大值）
   - 當前元素與執行最大值的乘積
3. **關鍵洞察**：負數乘以負數翻轉極值，所以必須同時追蹤兩種極值狀態

**Linux Kernel 中的狀態追蹤**（TCP Westwood/BBR）：

1. **狀態變數**：`minmax` 結構體（包含多個樣本）、時間戳記、值
2. **更新規則**：每次接收新的時間序列樣本時：
   - 檢查時間戳記是否在視窗內
   - 如果值超過現有的最大值或低於現有的最小值，更新狀態
   - 如果時間已超過視窗，移除舊樣本
3. **關鍵洞察**：時間維度的滑動視窗約束條件類似於陣列遍歷的端點約束

### 使用情境差異

**LeetCode 152**：同步追蹤最大和最小的乘積狀態，以處理負數的符號翻轉行為。這在任何涉及乘法運算的動態規劃問題中都很常見。

**Linux Kernel**：在網路壅塞控制中，追蹤時間序列資料（RTT、頻寬）的極值，以便在動態環境中做出即時決策。RTT 的最小值幫助區分壅塞延遲和傳播延遲；頻寬的最大值估計網路容量。

### 快取行為與效能考量

在 LeetCode 環境中，演算法的所有狀態都適合 CPU 暫存器，因此快取壓力最小。Linux kernel 中的 `struct minmax` 需要適應 CPU 快取行大小（通常 64 字節），以便在高頻率操作（例如每個 TCP 確認）中快速存取。

```c
/* 為快取友善而設計的緊湊結構 */
struct minmax {
    struct minmax_sample s[3];  /* 通常適合 L1 快取 */
};
```

---

## 主管 Code Review 角度考題

### Question 1: 為什麼必須同時追蹤最大值和最小值？

**預期答案**：因為乘法運算的符號特性。負數乘以負數得正數。在掃描過程中，前一個迭代的「最小」（最負）值可能在乘以新的負數時變成「最大」值。如果我們只追蹤最大值，會錯過這些情況。

**進階提問**（測試深度理解）：
- 加法問題（例如 LeetCode 53 - 最大子陣列）為什麼不需要追蹤最小值？答案應該解釋加法的單調性——最小值永遠不會翻轉為最大值。
- 如果我們允許只處理非負數的陣列呢？答案應該表明我們只需要追蹤最大值。
- 在 Linux 核心的 BBR 演算法中，頻寬和 RTT 都是非負的，所以為什麼仍然需要追蹤最小值和最大值？答案應該指出，在這種情況下，追蹤極值是為了估計網路特性，而非符號翻轉。

### Question 2: 整數溢出如何影響此演算法？

**預期答案**：當執行最大值或最小值乘以陣列元素時，結果可能超過 32 位元整數的範圍。必須使用 64 位元整數或應用 `Math.abs()` 和符號處理。

**進階提問**（測試警覺性）：
- 在使用 64 位元類型後，演算法的空間複雜度是否改變？答案應該是否定的——仍然是 O(1)，因為狀態變數的數量不變。
- 在 Linux 核心中，如何處理不同 CPU 架構（32 位元 vs 64 位元）中的溢出？答案應該提及使用 `kernel/sys.c` 中的型別檢查巨集或平台特定的 `__u64` 類型。
- 如果要在嵌入式環境（例如 Linux ARM）中執行此演算法，但只有 32 位元的乘法器硬體，會發生什麼？答案應該討論使用軟體乘法或模數運算的必要性。

### Question 3: 時間與空間權衡

**預期答案**：此演算法以 O(n) 時間和 O(1) 空間解決問題。與之對比，純遞迴或未最佳化的 DP 可能使用 O(n) 空間。

**進階提問**（測試優化知識）：
- 在 Linux 核心中，為什麼 `struct minmax` 被限制為只有 3 個樣本，而不是無限的環形緩衝區？答案應該涉及快取局部性和操作延遲——3 個樣本是一個經過驗證的折衷。
- 如果要支援無限數量的元素流（例如網路資料包的連續流），是否需要改變空間複雜度？答案應該解釋滑動視窗概念，只需要儲存視窗內的值。

### Question 4: 狀態轉移的正確性

**預期答案**：在每個位置 `i`，新的 `current_max` 取決於三個候選值。必須在更新 `current_max` 之前計算新的 `current_min`，否則會使用已更新的值導致錯誤。

**進階提問**（測試實作細節認知）：
- 如果誤用已更新的 `current_max` 來計算 `current_min`，會發生什麼？答案應該透過具體例子展示算法失敗。
- 在 Linux 核心的 BBR 實作中，更新多個狀態變數（`min_rtt_us`、`bw_lo`、`bw_hi`）的順序是否重要？答案應該詳細說明依賴關係和原子操作需求。

### Question 5: 邊界條件與正確性

**預期答案**：演算法必須正確處理陣列長度為 1、所有負數、零混合等邊界情況。初始值的設定（通常為第一個元素）至關重要。

**進階提問**（測試全面性）：
- 給定陣列 `[−2]`，演算法應該返回什麼？答案應該是 `-2`（最小值仍然是最大乘積）。
- 在 Linux 核心中，當沒有樣本或視窗為空時，`minmax_add()` 如何初始化？答案應該參考核心程式碼中的初始化條件。
- 如果陣列包含零怎樣？答案應該解釋零如何充當「重置點」，因為任何乘以零的結果都是零。

---

## 主管 Code Review 角度考題——續

### 實作層級評估

**測試內容**：
1. **狀態追蹤理解** — 候選人是否能清楚解釋為什麼必須同時保留最大值和最小值，以及它們如何相互依賴
2. **邊界條件意識** — 是否考慮單元素陣列、全負數陣列、包含零的情況
3. **溢出警覺** — 是否認識到乘法溢出的風險，並選擇適當的資料型別
4. **演算法複雜度** — 是否能清楚表達 O(n) 時間和 O(1) 空間的優化，以及為什麼不需要額外的陣列或雜湊表
5. **核心知識遷移** — 在 Linux 核心背景下，是否能識別類似的最小值/最大值追蹤模式，並解釋區別

---

## 面試加分總結

- **狀態機設計** — 展示對多狀態追蹤的理解。能夠解釋為什麼某些問題需要追蹤多個「極值」狀態，而其他問題可以只用一個。在系統設計或信號處理面試中高度相關。

- **符號與代數特性** — 將演算法與數學基礎聯繫起來。解釋乘法的符號翻轉行為，以及它如何影響最佳化策略。在金融科技或科學計算角色中加分。

- **Linux 核心參考** — 提及 TCP BBR 中的類似設計模式，展示對系統軟體的深入瞭解。在涉及網路或作業系統最佳化的角色中印象深刻。

- **效能與快取** — 討論 O(1) 空間的含義以及為什麼沒有額外分配。在硬體親和性或低延遲系統中有幫助。

- **邊界與正確性** — 詳盡列舉邊界情況並逐一驗證。展示全面的測試思維，這在安全性關鍵系統或金融軟體中高度評價。

- **類比與遷移** — 能夠從 LeetCode 問題遷移到真實世界場景（Linux 核心中的 BBR）。這展示了深度的概念理解，而不僅僅是機械性地應用演算法。類比能力在架構和系統設計面試中非常有價值。

---

## 核心實作對照範例

### LeetCode 152 實作（Python 型）

```python
def maxProduct(nums):
    if not nums:
        return 0

    current_max = nums[0]
    current_min = nums[0]
    result = nums[0]

    for i in range(1, len(nums)):
        # 儲存舊的 current_max 以備 current_min 計算
        temp_max = current_max

        # 更新 current_max：考慮當前元素本身或前面的乘積
        current_max = max(nums[i],
                         current_min * nums[i],
                         temp_max * nums[i])

        # 更新 current_min
        current_min = min(nums[i],
                         current_min * nums[i],
                         temp_max * nums[i])

        # 追蹤全局最大值
        result = max(result, current_max)

    return result
```

### Linux 核心 BBR 中的狀態追蹤概念

```c
/* BBR 壅塞控制中的簡化狀態追蹤 */
struct bbr {
    u32 min_rtt_us;          /* 執行最小 RTT */
    u32 min_rtt_stamp;       /* 最後一次更新時間 */
    u32 probe_rtt_done_stamp;
    u32 bw_lo, bw_hi;        /* 頻寬下界和上界 */
};

/* 類似於 LeetCode 中追蹤 current_max/current_min 的更新 */
void bbr_update_min_rtt(struct tcp_sock *tp, const struct rate_sample *rs) {
    u32 filter_window = bbr_min_rtt_win_sec * HZ;

    /* 追蹤執行最小值 */
    if (!rs->rtt_us)
        return;  /* 無有效 RTT 樣本 */

    /* 類似於 current_min = min(current_min, new_value) */
    if (rs->rtt_us < bbr->min_rtt_us ||
        !after(tcp_time_stamp, bbr->min_rtt_stamp + filter_window)) {
        bbr->min_rtt_us = rs->rtt_us;
        bbr->min_rtt_stamp = tcp_time_stamp;
    }
}
```

---

## Sources

- [Linux Kernel - minmax.h on GitHub](https://github.com/torvalds/linux/blob/master/include/linux/minmax.h)
- [Linux Kernel - lib/minmax.c on GitHub](https://github.com/torvalds/linux/blob/master/lib/minmax.c)
- [Linux Kernel - tcp_input.c (TCP Input Processing) on GitHub](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c)
- [Linux Kernel - tcp_bbr.c (BBR Congestion Control) on GitHub](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_bbr.c)
- [Linux Kernel - tcp_westwood.c (TCP Westwood) on GitHub](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_westwood.c)
- [LeetCode 152: Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/)
- [BBR: Congestion-Based Congestion Control — IETF Draft](https://tools.ietf.org/html/draft-cardwell-iccrg-bbr-congestion-control)
