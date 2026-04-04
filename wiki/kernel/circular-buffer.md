# Circular Buffer 環形緩衝區

## 核心概念

環形緩衝區是一個經典且高效的資料結構，在 kernel 中廣泛應用於日誌記錄、I/O 隊列和事件追蹤。核心實現位於 `include/linux/circ_buf.h` 和 `include/linux/kfifo.h`。

## 基礎設計：circ_buf.h

### 環形緩衝區的結構

```c
struct circ_buf {
  char *buf;      // 指向緩衝區
  int head;       // 寫入指標
  int tail;       // 讀取指標
};

// 宏定義
#define CIRC_CNT(head, tail, size) \
  (((head) - (tail)) & ((size) - 1))

#define CIRC_SPACE(head, tail, size) \
  CIRC_CNT((tail), ((head) + 1), (size))

#define CIRC_CNT_TO_END(head, tail, size) \
  ({int end = (size) - (tail) & ((size) - 1); \
    int n = ((head) + 1) & ((size) - 1); \
    n < end ? n : end;})

#define CIRC_SPACE_TO_END(head, tail, size) \
  ({int end = (size) - (head) - 1; \
    int n = (((tail) - (head) - 1) & ((size) - 1)); \
    n < end ? n : end;})
```

### 視覺化

```
緩衝區大小 = 8（2^3）
indices = [0, 1, 2, 3, 4, 5, 6, 7]

狀態 1：空
  head = 0, tail = 0
  [E, E, E, E, E, E, E, E]
  CIRC_CNT = 0

狀態 2：部分填滿
  head = 3, tail = 0
  [D, D, D, E, E, E, E, E]
  CIRC_CNT = 3

狀態 3：滿（繞回）
  head = 0, tail = 1
  [E, D, D, D, D, D, D, D]
  CIRC_CNT = 7

狀態 4：滿
  head = 5, tail = 6
  [E, E, D, D, D, D, E, E]
  CIRC_CNT = 7（即 5 - 6 mod 8 = -1 mod 8 = 7）
```

## 位運算優化

### 為什麼使用冪次 2？

```c
// 模運算在位運算中是自由的
int idx = (idx + 1) % size;    // 除法，昂貴

int idx = (idx + 1) & (size - 1);  // 位與，極快
```

**例**
```
size = 8 = 0b1000
size - 1 = 7 = 0b0111

(5 + 1) % 8:
  6 % 8 = 6

(5 + 1) & 7:
  0b0110 & 0b0111 = 0b0110 = 6 ✓

(7 + 1) % 8:
  8 % 8 = 0

(7 + 1) & 7:
  0b1000 & 0b0111 = 0b0000 = 0 ✓
```

**成本**
```
算術模：1-3 個時鐘週期（除法很貴）
位與：1 個時鐘週期
在高頻路徑上，節省可觀
```

## 計數宏詳解

### CIRC_CNT(head, tail, size)

計算緩衝區中的有效元素數。

```c
#define CIRC_CNT(head, tail, size) \
  (((head) - (tail)) & ((size) - 1))
```

**例**
```
head = 5, tail = 2, size = 8
(5 - 2) & 7 = 3 & 0b0111 = 3
[E, E, D, D, D, E, E, E]
    ^        ^
    tail     head
有 3 個元素
```

**環繞情況**
```
head = 1, tail = 6, size = 8
(1 - 6) & 7 = -5 & 0b0111
-5 = 0b...11111011（二補數）
0b...11111011 & 0b0111 = 0b0011 = 3
[D, E, E, E, E, E, D, D]
    ^              ^
   head          tail
實際有 3 個元素（6, 7, 0）
```

### CIRC_SPACE(head, tail, size)

計算剩餘空間。

```c
#define CIRC_SPACE(head, tail, size) \
  CIRC_CNT((tail), ((head) + 1), (size))
```

邏輯：從 tail 到 (head + 1) 的距離就是空間。

```
head = 2, tail = 5, size = 8
CIRC_SPACE = CIRC_CNT(5, (2 + 1), 8)
           = CIRC_CNT(5, 3, 8)
           = (5 - 3) & 7 = 2
[E, E, D, D, D, D, E, E]
        ^  ^
       head tail
可寫 2 個元素（位置 2, 1 不能用）
```

## 高級實現：kfifo.h

### 問題陳述

circ_buf 是最小實現。實際應用需要：

1. **通用類型支援**
2. **線程安全**
3. **分隔讀寫**
4. **效率優化**

### kfifo 設計

```c
struct kfifo {
  unsigned char *buffer;
  unsigned int size;
  unsigned int in;    // 生產者位置
  unsigned int out;   // 消費者位置
  spinlock_t *lock;
};

// 泛型宏
#define DECLARE_KFIFO(name, type, size) \
  struct { \
    unsigned int in; \
    unsigned int out; \
    unsigned int mask; \
    type *data; \
  } name = { \
    .mask = size - 1, \
  }
```

### 生產者/消費者分離

```c
// 生產者（寫）
int kfifo_in(struct kfifo *fifo, const void *from, unsigned int len) {
  unsigned int off = fifo->in & (fifo->size - 1);
  unsigned int l = min(len, fifo->size - (fifo->in - fifo->out));

  memcpy(fifo->buffer + off, from, l);
  fifo->in += l;

  return l;
}

// 消費者（讀）
int kfifo_out(struct kfifo *fifo, void *to, unsigned int len) {
  unsigned int off = fifo->out & (fifo->size - 1);
  unsigned int l = min(len, fifo->in - fifo->out);

  memcpy(to, fifo->buffer + off, l);
  fifo->out += l;

  return l;
}
```

### 為何 in/out 不重置？

```c
// 永不重置的好處
unsigned int in = 0xFFFFFFF8;   // 接近 32-bit 上限
unsigned int out = 0xFFFFFF00;

int available = in - out;       // 0xFFFFFF8 - 0xFFFFFF00
                                // = 0xF8 = 248（正確！）

if (in + 1 == out)  // 會在 32-bit 溢出時正確環繞
  fifo_full();
```

**優勢**
- 避免重置開銷
- 無需檢查邊界條件
- 溢出時自動環繞（模算術）

## 線程安全：鎖定策略

### 簡單版本：整體鎖定

```c
int kfifo_in_locked(struct kfifo *fifo, const void *from,
                     unsigned int len, spinlock_t *lock) {
  unsigned long flags;
  int ret;

  spin_lock_irqsave(lock, flags);
  ret = kfifo_in(fifo, from, len);
  spin_unlock_irqrestore(lock, flags);

  return ret;
}
```

### 高級版本：RCU 讀

```c
// 讀可能無需鎖
int kfifo_out_rcu(struct kfifo *fifo, void *to, unsigned int len) {
  rcu_read_lock();
  int ret = kfifo_out_peek(fifo, to, len);
  rcu_read_unlock();
  return ret;
}
```

## 應用場景

### 1. 日誌環形緩衝區

```c
// kernel/printk/printk.c
struct printk_ringbuffer {
  struct kfifo fifo;
};

// 高頻日誌寫入
printk_log(const char *fmt, ...) {
  char buf[256];
  int len = vsnprintf(buf, sizeof(buf), fmt, args);
  kfifo_in(&log_fifo, buf, len);
}

// 低頻讀取（當需要時）
syslog_read(char *buf, int len) {
  return kfifo_out(&log_fifo, buf, len);
}
```

**優勢**
- 寫入無阻塞，O(1)
- 無記憶體碎片
- 旋轉後自動丟棄舊日誌

### 2. I/O 隊列（UART）

```c
// drivers/tty/serial/8250/8250_core.c
struct uart_8250_port {
  struct kfifo tx_fifo;  // 發送隊列
};

// 中斷：高頻寫
uart_interrupt() {
  kfifo_in(&port->tx_fifo, data, len);
  schedule_tx_irq();
}

// 處理器：定期讀
uart_tx_tasklet() {
  len = kfifo_out(&port->tx_fifo, buf, PAGE_SIZE);
  send_to_hardware(buf, len);
}
```

### 3. 追蹤環

```c
// kernel/trace/ring_buffer.c
struct ring_buffer {
  struct kfifo *buffers[NR_CPUS];  // 每 CPU 環
};

// 追蹤事件
trace_event(struct trace_event_data *data) {
  int cpu = smp_processor_id();
  kfifo_in(&ring_buffer->buffers[cpu], data, sizeof(*data));
}
```

## 與 LeetCode 問題的對應

### 相關問題

- **232 用棧實現隊列**：環形緩衝區是隊列的底層實現
- **239 滑動窗口最大值**：環形隊列存儲候選值
- **933 最近的請求次數**：環形時間戳隊列
- **642 搜索自動完成系統**：循環遍歷字典尋找前綴

### 面試價值

LeetCode 問題通常實現簡單 FIFO。Kernel 環形緩衝區展示：
- 原地實現（無額外分配）
- 高性能位運算
- 無邊界檢查
- 線程安全

## 時間複雜度

| 操作 | 時間 | 空間 |
|------|------|------|
| 入隊（in） | O(1) | O(size) |
| 出隊（out） | O(1) | 固定 |
| 查詢長度 | O(1) | 無 |
| 查詢空間 | O(1) | 無 |

## 面試考點

### 問題 1：為什麼環形緩衝區需要冪次大小？

**預期答案**
- 模運算可用位與替換：O(1) 而非 O(log size)
- (idx + 1) & (size - 1) 自動環繞，無分支
- 高頻操作中每個週期都計算，位運算成本巨大影響

### 問題 2：CIRC_CNT 宏如何處理環繞？

**預期答案**
```c
// head = 1, tail = 6, size = 8
(1 - 6) & 7
= -5 & 0b0111
= 0b...11111011 & 0b0111  (二補數)
= 0b0011 = 3
// 實際元素在位置 6, 7, 0
```

### 問題 3：為什麼 kfifo 的 in/out 永不重置？

**預期答案**
- 重置需鎖定，頻繁重置開銷高
- 無符號溢出後自動環繞（模 2^32）
- 減法 (in - out) 自動產生正確計數
- 無邊界檢查，簡化代碼

### 問題 4：環形緩衝區如何實現無阻塞讀寫？

**預期答案**
- 讀和寫不相交的區域（讀拖尾，寫移頭）
- 原子操作 in++, out++
- 檢查 CIRC_SPACE 和 CIRC_CNT 無鎖
- 只有同步點（修改 in/out）需鎖定

## 參考資源

- **include/linux/circ_buf.h**：基礎環形緩衝宏
- **include/linux/kfifo.h**：通用 FIFO 實現
- **lib/kfifo.c**：核心邏輯
- **kernel/printk/printk_ringbuffer.c**：日誌環實現
- **drivers/tty/serial/8250/8250_core.c**：UART FIFO 應用
- **kernel/trace/ring_buffer.c**：追蹤環實現
