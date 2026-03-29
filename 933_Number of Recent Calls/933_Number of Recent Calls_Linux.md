# LeetCode 933: Number of Recent Calls — Linux Kernel 實作對照

## 問題概述

LeetCode 933 要求實作 `RecentCounter` 類別，使用 ping 時間戳記追蹤 3000 毫秒內的最近呼叫。這是典型的滑動視窗 (sliding window) 和佇列 (queue) 問題。

---

## 1. Linux 相關原始碼位置

### 1.1 Kernel FIFO (kfifo)
- **位置**: `include/linux/kfifo.h` / `lib/kfifo.c`
- **用途**: 無鎖、高效的生產者-消費者環形緩衝區
- **特點**: 支援單一生產者/單一消費者或多執行緒場景（使用自旋鎖）

### 1.2 Network Packet Queue (sk_buff)
- **位置**: `include/linux/skbuff.h` / `net/core/skbuff.c`
- **用途**: 網路封包接收和傳輸佇列
- **相似性**: 類似 ping 資料的時間戳記追蹤，需要快速入隊/出隊

### 1.3 Rate Limiting Module
- **位置**: `net/netfilter/xt_limit.c`
- **用途**: 使用 token bucket 算法實現速率限制
- **相似性**: 時間視窗概念（雖然使用 token bucket 而非滑動視窗）

### 1.4 Circular Buffer Macros
- **位置**: `include/linux/circ_buf.h`
- **提供**: `CIRC_SPACE()` / `CIRC_CNT()` 等巨集簡化環形緩衝區計算

---

## 2. Kernel vs. LeetCode 實作對照

### 2.1 基本資料結構

**LeetCode 典型實作 (Python/C++)**:
```python
class RecentCounter:
    def __init__(self):
        self.calls = []  # 簡單陣列或內建佇列

    def ping(self, t: int) -> int:
        self.calls.append(t)
        # 移除超出時間窗口的呼叫
        while self.calls and self.calls[0] < t - 3000:
            self.calls.pop(0)
        return len(self.calls)
```

**Linux kfifo 實作 (C)**:
```c
struct kfifo {
    unsigned int in;      // 寫入指標
    unsigned int out;     // 讀取指標
    unsigned int mask;    // 位元遮罩 = size - 1 (size 必為 2 的冪)
    unsigned char *data;  // 緩衝區
};

// 典型使用
struct kfifo fifo;
kfifo_alloc(&fifo, 256, GFP_KERNEL);  // 大小必為 2 的冪
kfifo_in(&fifo, &value, sizeof(value));
kfifo_out(&fifo, &value, sizeof(value));
```

### 2.2 核心差異

| 面向 | LeetCode | Linux kfifo |
|------|----------|------------|
| **大小** | 動態（根據時間窗口） | 固定且為 2 的冪 |
| **索引計算** | 模數運算 (%) | 位元遮罩 (&) — 更快 |
| **過期移除** | 顯式 `pop(0)` | 隱式環形覆蓋或顯式 `out` 指標遞進 |
| **執行緒安全** | 通常無（單一執行緒） | 支援無鎖 (spinlock-free) 或有鎖版本 |
| **使用場景** | 演算法面試 | 網路驅動、I/O 佇列 |

### 2.3 滑動視窗 vs. 環形覆蓋

**LeetCode 滑動視窗方式**:
```
時間線: [100] [200] [300] [3100] [3101] ...
時間窗口: [t-3000, t]
操作: 當新呼叫進入時，移除所有 < (t-3000) 的元素
特點: 陣列前端持續刪除、後端持續新增（不穩定）
```

**Linux kfifo 環形覆蓋方式**:
```
環形緩衝區: [0] [1] [2] [3] [4] ... (大小固定為 power-of-2)
in  指標: 總寫入次數 (mod size)
out 指標: 總讀出次數 (mod size)
計算空間: (in - out) & mask → 已用空間
特點: 固定記憶體、無需動態分配（高效）
```

### 2.4 時間複雜度比較

| 操作 | LeetCode 陣列 | Linux kfifo |
|------|-------------|------------|
| ping (入隊) | O(1) 均攤（pop 可能 O(n)） | O(1) 保證 |
| 移除過期 | O(k)，k = 過期元素數 | 取決於實作；滑動視窗場景無額外成本 |
| 空間 | O(n)，n ≤ 視窗內元素數 | O(1) 固定預分配 |

---

## 3. 主管 Code Review 角度考題

### 3.1 為什麼 kfifo 要求大小為 2 的冪？

**回答**:
- 使用位元遮罩 `(index & (size-1))` 取代模數 `(index % size)`
- 位元遮罩: 1 CPU 週期；模數: 10+ CPU 週期
- 2 的冪: `size-1` 的所有低位元都是 1（如 `0b111` for size=8）
- 範例:
  ```
  size = 8 (0b1000)
  mask = 7 (0b0111)
  index = 15 (0b1111)
  index & mask = 15 & 7 = 0b1111 & 0b0111 = 0b0111 = 7 ✓
  ```

### 3.2 kfifo 如何實現無鎖操作？

**回答**:
- 前提條件: **單一生產者，單一消費者**
- 機制:
  1. 生產者只更新 `in` 指標，消費者只更新 `out` 指標（無競爭）
  2. 使用 **記憶體屏障** (`smp_wmb()`, `smp_rmb()`) 確保順序
  3. 讀取 `in`/`out` 時使用 `volatile` 或 barrier 防止編譯器優化
- 如果需要多生產者/消費者，使用自旋鎖保護 `in`/`out` 的更新

```c
// Linux 無鎖讀取
unsigned int kfifo_in(struct kfifo *fifo, const void *buf, unsigned int len) {
    unsigned int off = fifo->in & fifo->mask;  // 位元遮罩
    smp_wmb();  // 記憶體屏障：確保寫入可見
    fifo->in += len;
    return len;
}
```

### 3.3 你的 LeetCode 陣列實作滿出時怎麼辦？

**回答場景**:
```
問題: 一般陣列若預先分配大小，ping 呼叫超過預設容量時會掉資料
原方案: Python list 自動擴展，但浪費記憶體；C++ vector 也會重新分配

改進方案（學習 kfifo）:
1. 預分配足夠大的環形緩衝區（2 的冪）
2. 使用 in/out 指標追蹤邊界，允許覆蓋舊資料
3. 無需持續 pop_front，直接遞進 out 指標
```

### 3.4 與環形緩衝區的權衡

**你的方案 vs. 環形緩衝區**:

| 評比面 | 你的方案（簡單陣列） | 環形緩衝區 |
|------|---------------|---------|
| **實現複雜度** | 低；使用 `pop_front()` | 高；需手動管理 in/out 指標 |
| **時間複雜度** | O(1) 均攤，最壞 O(n) | O(1) 保證 |
| **空間效率** | 高；只儲存有效元素 | 低；預分配固定大小，可能浪費 |
| **快取局部性** | 差；持續移除和重分配 | 優；連續記憶體訪問 |
| **生產場景** | 適合面試、演算法 | 適合高效能系統（網路、驅動） |

**對談範例**:
> 面試官: "在高頻交易系統中，你會選哪個？"
> 你: "環形緩衝區。因為時間延遲敏感，O(1) 保證和快取友善性更重要。"

### 3.5 如何改進為多執行緒速率限制器？

**設計思路**:
```c
struct rate_limiter {
    spinlock_t lock;           // 保護 in/out 指標
    unsigned int in;           // 寫入次數
    unsigned int out;          // 可讀出次數（基於時間視窗）
    unsigned int window_ms;    // 3000 ms
};

int rate_limit_ping(struct rate_limiter *lim, unsigned int t) {
    spin_lock_irqsave(&lim->lock);

    // 模擬：計算有多少呼叫在 [t-3000, t] 內
    unsigned int expired = /* t - 3000 之前的 out */ ;
    lim->out = expired;

    lim->in++;  // 新呼叫
    int count = lim->in - lim->out;

    spin_unlock_irqrestore(&lim->lock);
    return count;
}
```

**進階: 無鎖多生產者**:
- 使用原子操作 `atomic_inc()` / `atomic_read()` 取代自旋鎖
- 時間戳記追蹤改用 per-CPU 佇列（避免競爭）

---

## 4. 面試加分總結

### 4.1 展現深度認識的關鍵句

1. **提到 kfifo**:
   > "這題本質是滑動視窗和佇列。在 Linux 核心中，類似的需求用 kfifo 實現——一個無鎖、固定大小的環形緩衝區，用位元遮罩而非模數以提升性能。"

2. **談論性能瓶頸**:
   > "我的 pop_front() 方案是 O(n) 最壞情況。更好的方法是使用環形緩衝區，如 kfifo，維護 in/out 指標，避免每次都移除陣列元素。"

3. **涉及多執行緒時**:
   > "在多執行緒環境下，我會考慮使用自旋鎖或原子操作保護 in/out 指標的更新，類似 Linux 的 kfifo 多生產者支援。"

4. **談論工程權衡**:
   > "簡單方案適合面試演示，但生產環境（如網路驅動）會選環形緩衝區，因為時間複雜度保證和快取效率更重要。"

### 4.2 回答追問時的深度

**追問: "如果 ping 呼叫間隔不規則怎麼辦？"**
> "這時滑動視窗仍然有效，因為每個 ping 獨立驗證。但如果要追蹤**事件密度**（如每秒最多 N 個），token bucket 演算法會更合適——就像 Linux 的 `xt_limit` 模組。"

**追問: "能否避免線性掃描移除過期元素？"**
> "是的。如果維護 in/out 指標和時間戳記陣列，可以直接遞進 out 指標而無需掃描。這就是環形緩衝區的設計。"

---

## 5. Sources

### Linux Kernel Source (GitHub)

1. **kfifo 核心實作**
   - https://github.com/torvalds/linux/blob/master/include/linux/kfifo.h
   - https://github.com/torvalds/linux/blob/master/lib/kfifo.c

2. **sk_buff 網路佇列**
   - https://github.com/torvalds/linux/blob/master/include/linux/skbuff.h
   - https://github.com/torvalds/linux/blob/master/net/core/skbuff.c

3. **速率限制模組**
   - https://github.com/torvalds/linux/blob/master/net/netfilter/xt_limit.c

4. **環形緩衝區巨集**
   - https://github.com/torvalds/linux/blob/master/include/linux/circ_buf.h

### 相關文獻

- Linux Kernel Documentation: "Circular Buffers"
- "Linux Device Drivers" 3rd Edition — Chapter on Queuing and FIFO
- "The Art of Linux Kernel Programming" — Performance-critical data structures

---

## 附錄: 完整對照程式碼示例

### LeetCode 933 標準解法（Python）
```python
from collections import deque

class RecentCounter:
    def __init__(self):
        self.calls = deque()

    def ping(self, t: int) -> int:
        self.calls.append(t)
        while self.calls and self.calls[0] < t - 3000:
            self.calls.popleft()  # 移除過期
        return len(self.calls)
```

### 改進: 環形緩衝區概念（C++ 示例）
```cpp
class RecentCounterRingBuffer {
private:
    vector<int> buffer;
    int in_idx = 0, out_idx = 0;
    static constexpr int SIZE = 256;  // 2 的冪
    static constexpr int MASK = SIZE - 1;

public:
    RecentCounterRingBuffer() : buffer(SIZE, -1) {}

    int ping(int t) {
        buffer[in_idx & MASK] = t;
        in_idx++;

        // 移除過期的（簡化: 線性掃描，實際應使用時間戳記陣列）
        while (out_idx < in_idx && buffer[out_idx & MASK] < t - 3000) {
            out_idx++;
        }

        return in_idx - out_idx;
    }
};
```

### Linux kfifo 實際使用（核心代碼片段）
```c
#include <linux/kfifo.h>

struct kfifo fifo;
int ping_buffer[256];  // 固定大小環形緩衝區

void init_ping_queue(void) {
    kfifo_alloc(&fifo, 256, GFP_KERNEL);
}

int process_ping(int t) {
    kfifo_in(&fifo, &t, sizeof(t));

    // 簡化的過期移除（實際實作更複雜）
    int oldest = t - 3000;
    int temp;
    while (kfifo_peek(&fifo, &temp, sizeof(temp)) && temp < oldest) {
        kfifo_out(&fifo, &temp, sizeof(temp));
    }

    return kfifo_len(&fifo) / sizeof(int);
}
```

---

**最後提醒**: 在面試中引用 Linux 核心知識時，**先確保基本解法正確無誤**。深度知識是加分項，不是必需項。重點是**展現理解問題本質和系統設計思維**。
