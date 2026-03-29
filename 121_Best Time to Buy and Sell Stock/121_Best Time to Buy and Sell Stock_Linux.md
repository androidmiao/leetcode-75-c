# LeetCode 121：股票買賣最佳時機 - Linux 核心連結分析

## 問題概述

LeetCode 121 要求在給定價格陣列中找到最大利潤。演算法採用**單遍掃描**策略，追蹤執行中的最小價格與最大差額。

**時間複雜度**: O(n)
**空間複雜度**: O(1)

```python
def maxProfit(prices):
    min_price = float('inf')
    max_profit = 0
    for price in prices:
        min_price = min(min_price, price)
        max_profit = max(max_profit, price - min_price)
    return max_profit
```

---

## 1. Linux 相關原始碼位置

此演算法在 Linux 核心中有多個對應的實作位置，反映出執行中追蹤最小值/最大值的普遍模式。

### 1.1 `kernel/sched/fair.c` - CFS 排程器的 min_vruntime 追蹤

**位置**: https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c

CFS（完全公平排程）排程器使用 `min_vruntime` 來追蹤紅黑樹中所有工作項的最小虛擬執行時間：

```c
// 簡化示例
static void update_min_vruntime(struct cfs_rq *cfs_rq)
{
    u64 vruntime = cfs_rq->min_vruntime;

    if (cfs_rq->curr)
        vruntime = min_vruntime(vruntime, cfs_rq->curr->vruntime);

    if (cfs_rq->rb_leftmost) {
        struct sched_entity *se = rb_entry(cfs_rq->rb_leftmost,
                                          struct sched_entity,
                                          run_node);
        vruntime = min_vruntime(vruntime, se->vruntime);
    }

    cfs_rq->min_vruntime = max_vruntime(cfs_rq->min_vruntime, vruntime);
}
```

**與 LeetCode 121 的相似性**：
- 都在單遍掃描中維護執行中的最小值
- `min_vruntime` 在調度決策中扮演類似 `min_price` 的角色
- 資料源不同（進程工作項 vs 價格陣列），但演算法邏輯相同

### 1.2 `net/ipv4/tcp_input.c` - TCP 最小 RTT 追蹤

**位置**: https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c

TCP 擁塞控制使用 Reno/CUBIC 演算法，在傳入封包時追蹤最小往返時間（RTT）：

```c
// 簡化的 TCP 最小 RTT 追蹤模式
struct tcp_sock {
    u32 rtt_min;  // 執行中最小 RTT
    // ... 其他欄位
};

static void tcp_update_rtt_min(struct tcp_sock *tp, u32 rtt)
{
    if (rtt > 0) {
        tp->rtt_min = min(tp->rtt_min, rtt);  // 類似 min_price = min(min_price, price)
    }
}
```

**與 LeetCode 121 的對照**：
- `min_price` <-> `rtt_min`：執行中的最小追蹤者
- `price - min_price` <-> `current_rtt - rtt_min`：差額計算
- 應用：網路性能優化 vs 股票利潤最大化

### 1.3 `mm/vmstat.c` - 記憶體統計的 min/max 追蹤

**位置**: https://github.com/torvalds/linux/blob/master/mm/vmstat.c

虛擬記憶體子系統單遍掃描統計陣列時會追蹤最小/最大值：

```c
// 概念性示例：統計記憶體狀態
for (i = 0; i < NR_VM_STAT_ITEMS; i++) {
    long val = atomic_long_read(&vm_stat[i]);
    min_val = min(min_val, val);
    max_val = max(max_val, val);
}
```

### 1.4 `include/linux/minmax.h` - min/max 宏定義

**位置**: https://github.com/torvalds/linux/blob/master/include/linux/minmax.h

核心定義了型別安全的 min/max 宏，在整個核心中廣泛使用：

```c
#define min(x, y) ({            \
    typeof(x) _min1 = (x);      \
    typeof(y) _min2 = (y);      \
    (void) (&_min1 == &_min2);  \
    _min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({            \
    typeof(x) _max1 = (x);      \
    typeof(y) _max2 = (y);      \
    (void) (&_max1 == &_max2);  \
    _max1 > _max2 ? _max1 : _max2; })
```

這些宏確保型別安全，正如 LeetCode 解答中的邏輯一樣穩健。

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 執行中最小值追蹤比較

| 維度 | LeetCode 121 | Linux TCP (min RTT) | Linux CFS (min_vruntime) |
|------|-------------|-------------------|------------------------|
| **資料來源** | 價格陣列（靜態） | 網路封包（動態流） | 進程工作項隊列（動態） |
| **更新策略** | 迭代陣列，單遍掃描 | 中斷驅動，非同期更新 | 排程時脈，準同期更新 |
| **初始值** | `float('inf')` | 預設上界（e.g., `~0U`） | 0 或上次快照 |
| **溢位處理** | 受約束（price <= 10^4） | 型別轉換（u32） | 邏輯位移（vruntime 溢位） |
| **窗口類型** | 全域最小值 | 滑動時間窗口（RTT 歷史） | 樹結構中的最小值 |
| **時間複雜度** | O(n) 單遍 | O(1) 或 O(log n)（取決於資料結構） | O(log k)，k = 隊列大小 |

### 2.2 關鍵差異

**LeetCode 121**：
- 目標明確：找到全域最大利潤
- 儲存空間最小：僅記住一個 min_price
- 離線問題：整個陣列預先給定

**Linux Kernel 最小 RTT 追蹤**：
- 目標：持續監控網路延遲，驅動擁塞控制決策
- 需要考慮時間衰減（舊的 RTT 樣本權重應更低）
- 線上問題：新封包不斷到達

**Linux CFS min_vruntime**：
- 目標：確保排程公平性，防止饑餓
- 需要支援動態插入/刪除（進程啟動/終止）
- 複雜資料結構：紅黑樹而非陣列

---

## 3. 主管 Code Review 角度考題

### Q1. 為何不排序再掃描？

**問題**：如果我們對價格進行排序，然後進行掃描，是否能得到更好的效能？

**答案**：
```
不可行。排序為 O(n log n)，而單遍掃描為 O(n)，違反時間複雜度目標。
另外，排序會破壞原始的時間序列關係——我們必須在某一天買入，
之後某一天賣出。排序會改變這個約束。

例如：prices = [3, 2, 6, 5, 0, 3]
排序後：[0, 2, 3, 3, 5, 6]
但我們不能在價格 0 後買入並在它之前的價格 6 賣出。
```

**核心相關**：TCP 最小 RTT 也不能排序，因為時間順序很重要（擁塞控制必須根據實時事件做出決定）。

### Q2. 大價格時的整數溢位？

**問題**：如果 `price` 是 32 位整數，差額計算會溢位嗎？

**答案**：
```
根據約束：prices[i] <= 10^4
最大差額：10^4 - 0 = 10^4，遠小於 2^31 - 1
因此 32 位整數不會溢位。

但如果約束放寬到 prices[i] <= 10^9，我們需要使用 64 位。
```

**核心相關**：Linux 在 `include/linux/minmax.h` 中使用 `typeof()` 確保型別安全，避免隱藏的溢位錯誤。

### Q3. 擴展到 k 筆交易？

**問題**：如果允許最多 k 筆買賣交易，演算法如何改進？

**答案**：
```
使用動態規劃。定義：
dp[i][j][0] = 使用 i 個元素，完成 j 筆交易，目前未持股的最大利潤
dp[i][j][1] = 使用 i 個元素，完成 j 筆交易，目前持股的最大利潤

轉移方程：
dp[i][j][0] = max(dp[i-1][j][0], dp[i-1][j][1] + price[i])
dp[i][j][1] = max(dp[i-1][j][1], dp[i-1][j-1][0] - price[i])

時間複雜度：O(n * k)
空間複雜度：O(k)（使用滾動陣列優化）
```

**核心相關**：核心排程器使用類似的狀態機模式（進程狀態轉移）。

### Q4. 與 Kadane 演算法的關係？

**問題**：這與最大子陣列問題有關嗎？

**答案**：
```
是的。我們可以將問題轉換為：
找到價格差陣列 diff[i] = prices[i] - prices[i-1] 的最大子陣列和。

diff = [prices[1]-prices[0], prices[2]-prices[1], ..., prices[n-1]-prices[n-2]]

然後應用 Kadane 演算法找最大子陣列和。

例如：prices = [7, 1, 5, 3, 6, 4]
diff = [-6, 4, -2, 3, -2]
最大子陣列：[4, -2, 3] = 5（對應日期 1 買入，日期 4 賣出）
```

**核心相關**：Linux 記憶體管理中使用類似的差異追蹤來計算頁面異常率。

### Q5. 核心如何處理類似的串流 min/max 問題？

**答案**：

1. **滑動視窗 min/max**（使用雙端隊列）：
   - 核心網路棧在 `net/core/` 中使用此技術
   - 適用於固定大小的視窗（e.g., TCP 時間戳記窗口）

2. **指數加權移動平均**（EWMA）：
   - 核心使用 `include/linux/average.h` 中的 EWMA 巨集
   - 為較新的樣本賦予更高權重
   - 應用：網路延遲平滑化、負載平衡

3. **統計資料結構**：
   - `kernel/trace/` 中的直方圖和百分位數計算
   - 不需存儲所有樣本，只追蹤統計資訊

---

## 4. 面試加分總結

### 為何提及 Linux 核心知識？

1. **展現深度理解**：
   - 不僅知道演算法如何工作，還能認識到它在系統級別的應用
   - 分類思維：從「股票利潤」抽象到「執行中極值追蹤」

2. **效能意識**：
   - 了解 O(n) vs O(n log n) 的實際成本（尤其在高頻交易系統中）
   - 懂得權衡：時間 vs 空間 vs 代碼複雜度

3. **可靠性考量**：
   - 溢位、邊界情況、資料型別安全
   - 核心程式碼必須考慮這些，你的面試答案也應該

4. **系統思維**：
   - 連接數據結構、演算法、系統設計
   - 展示跨領域應用的能力（不只是解 LeetCode）

### 推薦論述框架

```
我的解法採用執行中最小值追蹤，這在計算機系統中很常見。
例如，Linux CFS 排程器使用相同的模式維護 min_vruntime，
TCP 擁塞控制追蹤最小 RTT。這告訴我演算法的核心優勢是：

1. 時間複雜度 O(n)，不需排序
2. 空間複雜度 O(1)，無額外資料結構
3. 快取友好（單遍線性掃描）
4. 易於擴展（支援流式處理、動態更新）

如果要擴展到 k 筆交易，我會用動態規劃...
如果要即時更新價格，我會使用...（線段樹/堆）
```

---

## 5. 核心原始碼參考

### 關鍵檔案位置

| 檔案 | 主題 | GitHub 連結 |
|------|------|-----------|
| `kernel/sched/fair.c` | CFS min_vruntime 追蹤 | https://github.com/torvalds/linux/blob/master/kernel/sched/fair.c |
| `net/ipv4/tcp_input.c` | TCP RTT 追蹤與擁塞控制 | https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c |
| `mm/vmstat.c` | 記憶體統計單遍掃描 | https://github.com/torvalds/linux/blob/master/mm/vmstat.c |
| `include/linux/minmax.h` | min/max 宏定義 | https://github.com/torvalds/linux/blob/master/include/linux/minmax.h |
| `include/linux/average.h` | 指數加權移動平均 | https://github.com/torvalds/linux/blob/master/include/linux/average.h |
| `net/core/pktgen.c` | 滑動視窗最小值實作 | https://github.com/torvalds/linux/blob/master/net/core/pktgen.c |

### 進階閱讀

1. **CFS 排程理論**
   - Runnable Average : Linux 核心中的負載平衡
   - 參考：`Documentation/scheduler/sched-design-CFS.rst`

2. **TCP 擁塞控制**
   - Reno、CUBIC、BBR 演算法的實作對比
   - 參考：`Documentation/networking/ip-sysctl.rst`

3. **記憶體統計**
   - Per-CPU 統計與 lockless 更新
   - 參考：`Documentation/vm/page-reclaim.rst`

---

## 6. 時間複雜度驗證（Linux 核心視角）

| 操作 | 複雜度 | 核心範例 |
|------|-------|--------|
| 陣列單遍掃描 | O(n) | 記憶體統計掃描 |
| 動態規劃（k 筆交易） | O(n*k) | 排程器 DP（狀態機） |
| 紅黑樹查詢 min | O(log n) | CFS 紅黑樹左端節點 |
| 堆 insert/delete | O(log n) | 定時器堆（核心時鐘） |

---

## 7. 面試時的說法示例

> 我解這題用的是執行中最小值追蹤，這種模式在 Linux 核心裡很常見。
> 比如 CFS 排程器的 `min_vruntime` 就是用相同邏輯——每次掃描時更新一個執行中的最小值，
> 用來確保排程公平性。TCP 擁塞控制也追蹤最小 RTT 來優化網路性能。
>
> 這個演算法之所以優於排序方案（O(n log n) vs O(n)），是因為我們不需要改變時間順序。
> 如果允許多筆交易，我會轉向 DP，類似於核心中的狀態機設計...

---

**文件編寫日期**: 2026-03-29
**涵蓋 Linux 版本**: 5.15+ (最新 stable)
**難度等級**: 中高級工程師向

