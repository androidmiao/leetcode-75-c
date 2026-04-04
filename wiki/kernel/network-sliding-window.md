# Network Sliding Window 網路滑動窗口

## 核心概念

滑動窗口是網路協議（特別是 TCP）中的基礎概念，也是 LeetCode 中常見的面試主題。Linux kernel TCP 實現在 `net/ipv4/tcp_input.c` 和 `net/ipv4/tcp_output.c`。Kernel 實現必須處理序列號環繞、擁塞控制和流量控制。

## TCP 滑動窗口基礎

### 發送者視角

```c
struct tcp_sock {
  u32 snd_una;      // 未確認的最舊序列號
  u32 snd_nxt;      // 下一個要發送的序列號
  u32 snd_wnd;      // 發送窗口大小（接收者通告）
  u32 snd_wl1, snd_wl2;  // 窗口更新追蹤
};

// 發送窗口範圍
// [snd_una, snd_una + snd_wnd) 可以發送的序列號範圍
```

### 接收者視角

```c
struct tcp_sock {
  u32 rcv_nxt;      // 期望的下一個序列號
  u32 rcv_wnd;      // 接收窗口大小
  struct sk_buff_head *receive_queue;
};

// 接收窗口範圍
// [rcv_nxt, rcv_nxt + rcv_wnd) 可以接受的序列號範圍
```

### 視覺化

```
發送者：
  0   10  20  30  40  50  60
  |---|---|---|---|---|---|
  ↑   ↑           ↑
  0   snd_una     snd_nxt=35

      [已確認]   [傳輸中]  [未發送]

接收窗口=30
snd_wnd = snd_nxt - snd_una + window
       = 35 - 10 + 30 = 55

發送者可以發送到序列號 10 + 30 = 40
```

## 序列號環繞處理

### 32-bit 序列號空間

```c
// TCP 使用 32-bit 序列號
u32 seq = 0x80000000;  // ~2.1 GB
u32 next_seq = seq + 1;  // 0x80000001

// 4GB 後環繞
seq = 0xFFFFFFFF;
next_seq = seq + 1;  // 0x00000000（溢出）
```

### 序列號比較

```c
// 錯誤：無符號比較不適用環繞
if (seq > snd_una)
  ok = true;  // 當 seq 接近 0, snd_una 接近 0xFFFFFFFF 時失敗

// 正確：相對順序比較
static inline int before(u32 seq1, u32 seq2) {
  return (__s32)(seq1 - seq2) < 0;
}

static inline int after(u32 seq1, u32 seq2) {
  return (__s32)(seq2 - seq1) < 0;
}

// 使用帶符號整數的二補數算術
// seq1 - seq2 > 0 means seq1 後於 seq2（相對）
```

### 例：環繞點

```
snd_una = 0xFFFFFFF0
snd_nxt = 0x00000010

差異 = 0x00000010 - 0xFFFFFFF0 = 0x20
before(0x00000010, 0xFFFFFFF0) = (__s32)(0x20) < 0 = false ✓
after(0x00000010, 0xFFFFFFF0) = (__s32)(0xFFFFFFF0) < 0 = true ✓

相對順序正確！
```

## 流量控制：接收窗口管理

### 窗口縮小問題

```c
// 接收者應避免縮小窗口（RFC 793）
void tcp_update_window(struct tcp_sock *tp,
                       u32 seg_seq, u32 seg_len,
                       u32 seg_ack, u32 seg_wnd) {
  if (after(seg_ack, tp->snd_wl1) ||
      (seg_ack == tp->snd_wl1 && seg_wnd > tp->snd_wnd)) {
    tp->snd_wnd = seg_wnd;
    tp->snd_wl1 = seg_ack;
    tp->snd_wl2 = seg_ack;
  }

  // 重要：不允許縮小窗口
  // 這防止發送者重新發送已確認的資料
}
```

### 零窗口

```c
// 接收者通告窗口 = 0（緩衝區滿）
struct tcp_sock {
  u32 rcv_wnd = 0;  // 不能再接收
};

// 發送者進入零窗口探測
tcp_retransmit_timer() {
  if (tp->snd_wnd == 0 && !in_zero_window_probe) {
    // 定期發送 1 字節探測
    tcp_write_xmit_probe();
    in_zero_window_probe = true;
    backoff_timer();
  }
}
```

## 擁塞控制：發送窗口管理

### 有效窗口

```c
// 發送者的有效發送窗口
u32 tcp_snd_test(struct tcp_sock *tp, ...) {
  u32 window = tp->snd_wnd;  // 流量控制窗口

  // 擁塞控制窗口進一步限制
  u32 cwnd = tp->snd_cwnd;   // 擁塞窗口

  u32 effective_window = min(window, cwnd);

  // 已發送但未確認的位元組
  u32 in_flight = tp->snd_nxt - tp->snd_una;

  // 可以發送的位元組
  u32 available = effective_window - in_flight;

  return available;
}
```

### 擁塞窗口狀態機

```
開始：cwnd = 1 MSS

slow start:
  每 RTT 翻倍（指數增長）
  cwnd = 2 * cwnd

congestion avoidance:
  每 RTT 增加 1 MSS（線性增長）
  cwnd = cwnd + 1

loss detected:
  cwnd = cwnd / 2（乘法減少）
  ssthresh = cwnd
```

### 實現細節

```c
void tcp_cong_avoid(struct tcp_sock *tp) {
  if (tp->snd_cwnd <= tp->snd_ssthresh) {
    // Slow start
    if (tp->snd_cwnd < tp->snd_ssthresh)
      tp->snd_cwnd += tp->mss_cache;
    else
      tp->snd_cwnd += tp->mss_cache / tp->snd_cwnd;
  } else {
    // Congestion avoidance
    tp->snd_cwnd += tp->mss_cache / tp->snd_cwnd;
  }
}

void tcp_enter_cwr(struct tcp_sock *tp) {
  // 擁塞發生（超時或 3 個 ACK 重複）
  tp->snd_ssthresh = tp->snd_cwnd / 2;
  tp->snd_cwnd = tp->snd_ssthresh;
}
```

## RED（隨機早期檢測）

### 隊列管理問題

```
傳統 FIFO：
  - 隊列滿 → 丟包
  → 發送者無響應，RTT 增加
  → 更多發送者的包到達
  → 隊列更滿，更多丟包
  → 全局同步（多流同時退避）

結果：吞吐量下降，延遲增加
```

### RED 算法

```c
// net/core/red.c (qdisc_red)
struct red_parms {
  u32 qth_min;      // 最小閾值
  u32 qth_max;      // 最大閾值
  u32 limit;        // 隊列大小限制
  u32 Pmax;         // 最大丟包概率
};

int red_enqueue(struct sk_buff *skb, struct red_sched_data *q) {
  struct red_parms *p = &q->parms;

  // 計算平均隊列長度（指數加權）
  u32 avg_len = ((1 - weight) * avg_len + weight * current_len);

  if (avg_len < p->qth_min) {
    // 隊列短，接受所有包
    queue.enqueue(skb);
    return NET_XMIT_SUCCESS;
  }

  if (avg_len > p->qth_max) {
    // 隊列長，丟棄所有包
    return NET_XMIT_DROP;
  }

  // 中間區間：隨機丟棄
  u32 prob = (avg_len - p->qth_min) * p->Pmax / (p->qth_max - p->qth_min);
  if (random() < prob) {
    return NET_XMIT_DROP;  // 丟棄
  }

  queue.enqueue(skb);
  return NET_XMIT_SUCCESS;
}
```

**優勢**
- 早期信號：隊列還未滿時就開始丟包
- 避免全局同步：隨機丟棄，不同流丟包時間不同
- 動態適應：權重平均對流量變化反應靈敏

## 與 LeetCode 問題的對應

### 相關問題

- **76 最小覆蓋子字符串**：滑動窗口，擴大→縮小模式
- **239 滑動窗口最大值**：固定大小窗口，雙端隊列
- **3 無重複字符的最長子字符串**：變長窗口
- **424 用至多 k 次操作替換字符**：字符替換窗口
- **567 字符串排列**：固定大小窗口掃描
- **643 子陣列最大平均數 I**：固定窗口平均
- **480 滑動窗口中位數**：窗口統計

### 面試轉換

| LeetCode | TCP | 差異 |
|----------|-----|------|
| 簡單跟蹤 | 複雜狀態機 | 環繞、RTT、擁塞 |
| 線性緩衝區 | 環形序列號 | 無符號溢出 |
| 單流視角 | 多流交互 | 擁塞、RED、公平性 |

## 時間複雜度

| 操作 | 時間 | 備註 |
|------|------|------|
| 窗口更新 | O(1) | 簡單變數更新 |
| 序列號比較 | O(1) | 帶符號算術 |
| 擁塞控制 | O(1) | 每個 ACK 一次 |
| RED 決策 | O(1) | 概率計算 |

## 面試考點

### 問題 1：為什麼需要相對序列號比較？

**預期答案**
- TCP 使用 32-bit 序列號
- ~2.1 GB 後自動溢出環繞
- 無符號比較在環繞點失敗
- 帶符號差異自動處理環繞

```c
before(0x00000010, 0xFFFFFFF0)
= (__s32)(0x20) < 0 = false  // 0x00000010 後於 0xFFFFFFF0
```

### 問題 2：滑動窗口和擁塞窗口的關係？

**預期答案**
- 流量控制窗口（snd_wnd）：接收者通告，防止接收緩衝溢出
- 擁塞窗口（snd_cwnd）：發送者計算，防止網路過載
- 有效窗口：min(snd_wnd, snd_cwnd)
- 發送者受兩者限制

### 問題 3：RED 為何使用隨機丟棄？

**預期答案**
- 確定性丟棄（隊列滿時）導致全局同步
- 多個流同時退避，導致吞吐量尖峰下降
- 隨機丟棄：不同流統計上丟包時間分散
- 早期丟棄信號：隊列未滿時已提醒，流提前退避

### 問題 4：為什麼零窗口時需要探測？

**預期答案**
- 接收者：rcv_wnd = 0，窗口打開時發送更新
- 發送者：無法發送更多資料，陷入死鎖
- 解決：定期發送 1 字節探測
- 接收者響應新的窗口大小，恢復通信

## 參考資源

- **net/ipv4/tcp.c**：TCP 主邏輯
- **net/ipv4/tcp_input.c**：ACK 和窗口更新處理
- **net/ipv4/tcp_output.c**：發送實現
- **net/ipv4/tcp_timer.c**：超時和重傳
- **net/core/red.c**：RED 隊列規則
- **include/net/tcp.h**：TCP 結構和宏
- **RFC 793**：TCP 規範（序列號、窗口）
- **RFC 5681**：TCP 擁塞控制
