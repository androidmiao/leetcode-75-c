# LeetCode 643: Maximum Average Subarray I - Linux 核心原始碼對照

## 問題概述

給定一個整數陣列和固定視窗大小 k，找出任何長度為 k 的連續子陣列的最大平均值。使用滑動視窗技術：視窗向右滑動一次，加入新元素並移除離開的元素，追蹤最大和。

---

## Linux 相關原始碼位置

### 1. EWMA (Exponentially Weighted Moving Average)
**檔案**: `include/linux/average.h`

Linux 核心定義了 `DECLARE_EWMA` 巨集，用於追蹤移動平均值。與 LeetCode 643 的固定視窗不同，EWMA 使用指數衰減權重：
- 舊的資料點的權重隨時間遞減
- 計算公式：`新平均 = α × 新值 + (1-α) × 舊平均`
- 在網路、電源管理等子系統中廣泛使用

```c
/* 例子：kernel 中的 EWMA 使用 */
#define DECLARE_EWMA(name, _precision, _weight_rcp)
struct ewma_##name {
    unsigned long internal;
};
```

### 2. TCP RTT 估計 (TCP Round Trip Time)
**檔案**: `net/ipv4/tcp_input.c`

TCP 使用平滑化 RTT (SRTT) 來估計往返時間，這是移動平均的變種：
- 使用公式：`SRTT = (7 × 舊SRTT + 新RTT) / 8`
- 相當於指數衰減平均，權重比例為 7:1
- 影響重傳超時計算，是 TCP 擁塞控制的關鍵

```c
/* tcp_input.c 中的簡化概念 */
tp->srtt_us = (tp->srtt_us * 7 + rtt_us) >> 3;
```

### 3. 負載平均計算
**檔案**: `kernel/sched/loadavg.c`

Linux 計算系統負載平均使用固定點算術的移動平均：
- 追蹤可執行任務數
- 使用 EXP_n 常數進行指數衰減
- 與 LeetCode 643 的想法相似，但使用固定點數學避免浮點溢出

```c
/* loadavg.c 的概念 */
DECLARE_EWMA(load, 10, 1024)
```

### 4. 網路子系統的往返延遲 (RTD)
**檔案**: `net/ipv4/tcp.c`

某些網路監測實用工具使用滑動視窗追蹤封包延遲的統計資訊。

---

## Kernel vs. LeetCode 實作對照

| 面向 | Linux Kernel (EWMA/SRTT) | LeetCode 643 (固定視窗) |
|------|--------------------------|------------------------|
| **演算法類型** | 指數衰減移動平均 | 固定大小滑動視窗 |
| **視窗性質** | 無限視窗，權重遞減 | 固定 k 個元素 |
| **更新策略** | `new_avg = α × val + (1-α) × old_avg` | `sum -= arr[i-k]; sum += arr[i];` |
| **數據結構** | 單一累積值 (內部表示為整數) | 陣列 + 指標 + sum 變數 |
| **時間複雜度** | O(1) 每次更新 | O(n) 掃過整個陣列，O(1) 每次更新 |
| **空間複雜度** | O(1) | O(1) (不計輸入陣列) |
| **溢位防護** | 固定點算術，預定義位元寬度 | 需要考慮 int/long 溢位 |
| **精度損失** | 有意舍入，適合長期趨勢 | 無舍入損失，精確值 |
| **API 設計** | 巨集驅動，靜態類型生成 | 函式介面，直接操作指標 |
| **使用情境** | 即時監測、擁塞控制 | 離線分析、子陣列問題 |

---

## 主管 Code Review 角度考題

### 1. **視窗邊界與溢位**
**問題**: LeetCode 643 的解決方案如何處理大陣列的和溢位？例如，k=50000，陣列元素都是 INT_MAX？

**預期回答方向**:
- 使用 `long` 或 `long long` 累積和，而不是 `int`
- 在計算前進行型別轉換
- 討論該問題的邊界條件測試

**追蹤探針**:
- 如果固定用 `int` 累積，n=50000, k=50000, 每個元素=1000 會發生什麼？
- 有無必要同時追蹤最小值以檢測溢位？

**測試內容**: 數值穩定性、型別轉換意識

---

### 2. **快取局部性 (Cache Locality)**
**問題**: 與 kernel 的 EWMA 只維護單一累積值相比，LeetCode 643 的滑動視窗方法對 CPU 快取有何影響？

**預期回答方向**:
- LeetCode 解法逐個存取陣列元素，具有優秀的空間局部性
- 連續陣列訪問通常會填滿 L1/L2 快取行
- EWMA 則只存取單一記憶體位置，快取開銷更低，但靈活性更差

**追蹤探針**:
- 在大陣列上（例如 1GB），滑動視窗與 EWMA 的實際執行時間差異？
- 如何利用 SIMD 或向量化來加速滑動視窗？

**測試內容**: 記憶體存取模式、效能分析能力

---

### 3. **Kernel 中的固定點算術**
**問題**: `kernel/sched/loadavg.c` 使用固定點表示負載平均（例如，10 位元小數）。為什麼不在 LeetCode 643 中也使用固定點而是浮點除法？

**預期回答方向**:
- Kernel 避免浮點運算（效能、context switch 開銷）
- LeetCode 允許浮點，且 k 通常很小（整數乘除足夠）
- 對於嵌入式/實時系統，固定點可預測且無舍入誤差累積

**追蹤探針**:
- 如果 k 很大（例如 k=10^9），浮點除法會失去精度嗎？
- 如何用整數算術實現 `(double) sum / k` 同時避免溢位？

**測試內容**: 數值精度、kernel 與使用者空間的取捨

---

### 4. **演算法選擇的妥協**
**問題**: Kernel 選擇指數衰減平均而非固定視窗。這在 RTT 估計中意味著什麼？如果我們改用固定視窗會如何？

**預期回答方向**:
- 指數衰減給予最近的值更高的權重，適合動態變化的網路
- 固定視窗（例如最後 32 個 RTT 樣本）會對舊的高延遲反應較慢
- 取捨：EWMA 更平滑但反應快；固定視窗更清晰但可能抖動

**追蹤探針**:
- 在 TCP 擁塞控制中，如果用固定視窗而非 EWMA，會如何影響重傳邏輯？
- 如何在程式碼中驗證 EWMA vs 固定視窗的行為差異？

**測試內容**: 演算法選擇的系統考量、權衡分析

---

### 5. **Kernel 巨集與型別安全性**
**問題**: Linux 使用 `DECLARE_EWMA` 巨集進行靜態型別生成。相比於 LeetCode 643 的簡單函式，這有何優點和缺點？

**預期回答方向**:
- 巨集優點：零執行時開銷、編譯時型別檢查、內聯友好
- 缺點：除錯困難、巨集展開可能很冗長、可讀性降低
- LeetCode 函式方法更簡潔，但無型別參數化

**追蹤探針**:
- 如何為 LeetCode 643 設計一個通用的 C 巨集版本，支援不同的元素型別？
- 巨集與模板（C++）相比有何優缺點？

**測試內容**: C 語言深度、 generic 程式設計、編譯時優化

---

### 6. **邊界條件與錯誤處理**
**問題**: LeetCode 643 假設 `1 <= k <= n`。Kernel 程式碼在面對無效輸入時如何保護自己？

**預期回答方向**:
- Kernel 進行完整的輸入驗證 (`BUG_ON`, `WARN_ON`)
- 早期返回，明確的錯誤代碼
- LeetCode 通常不需要，但正式程式碼應該

**追蹤探針**:
- 如果 k > n 或 k <= 0 傳入，函式應如何行動？
- 如何在不添加額外空間的情況下檢測這些錯誤？

**測試內容**: 防禦程式設計、邊界測試

---

## 面試加分總結

### 如何在面試中利用 Kernel 知識：

1. **展示深厚的系統理解**
   - 提及 Linux 在 TCP、負載平均中使用移動平均的實例
   - 討論為何選擇指數衰減而非固定視窗的 tradeoff
   - 觀眾會印象深刻於跨層的知識

2. **數值穩定性與效能意識**
   - 討論溢位防護 (使用 `long`)
   - 提到快取局部性優化
   - 參考 kernel 的固定點算術來討論浮點精度

3. **從 Kernel 的眼光改進解法**
   - 提議一個更 kernel-友好的版本（例如固定點平均）
   - 討論如何用 SIMD 加速
   - 考慮邊界條件和錯誤處理

4. **算法取捨的思考**
   - 解釋為什麼 LeetCode 643 用固定視窗而不是指數衰減
   - 說明在不同場景下何時選擇哪種方法
   - 顯示對實際應用的深入理解

5. **提出明智的問題**
   - 「此演算法在實時嵌入式系統中如何表現？」
   - 「能否無浮點運算實現相同功能？」
   - 「大規模資料流中的記憶體局部性如何改進？」

6. **程式碼品質與 Kernel 慣例**
   - 應用 kernel 的輸入驗證和錯誤處理
   - 使用巨集和內聯優化性能
   - 撰寫符合 kernel 編碼風格的簡潔程式碼

---

## Sources

- **Linux Kernel Source**:
  - [include/linux/average.h](https://github.com/torvalds/linux/blob/master/include/linux/average.h) — EWMA 巨集定義
  - [kernel/sched/loadavg.c](https://github.com/torvalds/linux/blob/master/kernel/sched/loadavg.c) — 負載平均計算
  - [net/ipv4/tcp_input.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c) — TCP RTT 估計

- **相關文件與論文**:
  - Linux Kernel Documentation on [scheduler load average](https://www.kernel.org/doc/html/latest/filesystems/proc.html)
  - LWN: [Understanding the Linux kernel's load average](https://lwn.net/Articles/676540/)

- **LeetCode 相關**:
  - LeetCode Problem #643: Maximum Average Subarray I
  - Sliding Window 技術概述

---

**建立日期**: 2026-03-30
**主題**: 滑動視窗、移動平均、Kernel 對照
**難度**: 中等（LeetCode）/ 高等（Kernel 深度）
