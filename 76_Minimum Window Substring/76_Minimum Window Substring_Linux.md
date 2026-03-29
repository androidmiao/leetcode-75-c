# LeetCode 76: Minimum Window Substring - Linux 核心概念對照

## 1. Linux 相關原始碼位置

### TCP 滑動窗口實現
Linux 核心中的滑動窗口概念與本題最密切相關的實現：

| 檔案路徑 | 功能描述 |
|---------|---------|
| `net/ipv4/tcp_input.c` | TCP 接收窗口管理、序列號追蹤、流量控制。實現了類似本題的視窗擴展/收縮邏輯來追蹤已接收的位元組。 |
| `net/ipv4/tcp_output.c` | TCP 發送窗口管理、擁塞控制演算法。使用滑動窗口決定可以發送的資料量。 |
| `net/ipv4/tcp.c` | TCP 協議的核心實現，包含窗口縮放 (TCP window scaling) 的處理。 |

### 字串匹配與模式搜尋
用於高效搜尋和過濾的相關實現：

| 檔案路徑 | 功能描述 |
|---------|---------|
| `lib/string.c` | 基本字串操作（比較、複製、查找）。提供底層字串處理原始操作。 |
| `lib/ts_kmp.c` | Knuth-Morris-Pratt (KMP) 線性時間字串匹配演算法。用於核心內網路封包過濾和字串搜尋。 |
| `lib/ts_bm.c` | Boyer-Moore 字串匹配演算法。更高效的模式匹配，用於高效能字串搜尋和深度封包檢查 (DPI)。 |

**核心概念聯繫：** TCP 滑動窗口和本題的滑動窗口都是使用相同的範式——維護一個動態的區間 [left, right)，根據條件調整邊界以滿足特定目標。

---

## 2. Kernel vs. LeetCode 實作對照

### 概念相似性

#### TCP 滑動窗口 (Kernel)
```c
// tcp_input.c 中的窗口管理概念（簡化版）
struct tcp_sock {
    u32 rcv_wnd;        // 接收窗口大小
    u32 rcv_nxt;        // 下一個期望的序列號
    u32 rcv_wup;        // 發送上次窗口更新的位置
    u8 *sk_buff;        // 接收緩衝區
};

// 窗口擴展邏輯
if (seq >= rcv_nxt && seq < rcv_nxt + rcv_wnd) {
    // 資料在窗口內，可以接收
    add_to_buffer(seq, data);
    advance_window();
}
```

#### LeetCode 76 滑動窗口 (C)
```c
// 最小視窗子串
int minWindow(char* s, char* t, char* result) {
    int need[128] = {0}, have[128] = {0};
    int required = 0, formed = 0;
    int left = 0, min_len = INT_MAX, min_start = 0;

    // 計算目標字元需求
    for (char *p = t; *p; p++) {
        if (need[*p] == 0) required++;
        need[*p]++;
    }

    // 滑動視窗搜尋
    for (int right = 0; s[right]; right++) {
        have[s[right]]++;
        if (have[s[right]] == need[s[right]]) {
            formed++;
        }

        // 視窗收縮
        while (formed == required) {
            int window_len = right - left + 1;
            if (window_len < min_len) {
                min_len = window_len;
                min_start = left;
            }

            have[s[left]]--;
            if (have[s[left]] < need[s[left]]) {
                formed--;
            }
            left++;
        }
    }

    return min_len == INT_MAX ? 0 : min_len;
}
```

### 詳細對照表

| 面向 | TCP 滑動窗口 | LeetCode 76 |
|-----|------------|-----------|
| **目的** | 流量控制、可靠傳輸。控制發送方不超過接收方能處理的速度。 | 子串搜尋。找到包含目標所有字元的最小視窗。 |
| **視窗定義** | `[rcv_nxt, rcv_nxt + rcv_wnd)`，用序列號表示。 | `[left, right]`，用陣列索引表示。 |
| **視窗擴展** | 右邊界 (rcv_nxt + rcv_wnd) 根據接收緩衝可用空間變化。 | 右指標遍歷字串，直到視窗包含所有目標字元。 |
| **視窗收縮** | 左邊界 (rcv_nxt) 當資料被應用層讀取時推進。 | 左指標推進，移除左端字元直到視窗不再滿足條件。 |
| **資料結構** | 環形緩衝區、序列號追蹤、ACK 機制。 | 計數陣列 (need/have)、兩個指標。 |
| **時間複雜度** | O(n)，其中 n 是接收的位元組數。 | O(n + m)，其中 n = len(s), m = len(t)。 |
| **空間複雜度** | O(rcv_wnd) 用於接收緩衝。 | O(1)，固定 128 位大小陣列（ASCII 範圍）。 |

### 收縮策略的相似性

**Kernel TCP 視窗收縮：**
- 當應用層讀取資料時，左邊界推進
- 只有在新資料可用時才繼續接收
- 防止緩衝區溢出，維持流量平衡

**LeetCode 滑動視窗收縮：**
- 當視窗包含所有目標字元時，嘗試從左邊移除字元
- 記錄最小視窗，然後繼續調整
- 儘可能縮小視窗以找到最優解

---

## 3. 主管 Code Review 角度考題

### 問題 1：為什麼使用固定大小 128 陣列而不是哈希表？

**提問的層次：**
- 表面：效能最佳化
- 深層：對字元編碼的理解、權衡考量

**期望回答方向：**
- ASCII 只有 128 個字元，固定陣列的查詢/更新是 O(1) 且常數極小
- 對比哈希表：有雜湊函式開銷、碰撞解決、動態擴展
- 如果支援 Unicode（UTF-8），需要改用哈希表或預分配足夠大的陣列

**追問探針：**
1. "如果輸入包含中文字符怎麼辦？"
   - 正確答案：改用 `int need[256*256]` 或動態哈希表
   - 或使用 UTF-8 解碼後按碼點分類

2. "這個方案能否適用於很大的字元集？"
   - 正確答案：不能。超過 512 字元後，陣列變得低效，哈希表更合適

3. "能否舉出 Kernel 中類似的最佳化例子？"
   - 連接答案：TCP 協議使用序列號而非陣列索引，因為 32 位序列號空間足以處理所有可能的窗口大小

**測試維度：** 對字元編碼、時間空間權衡、資料結構選擇的理解

---

### 問題 2：視窗收縮的條件是什麼？為何需要 while 循環而不是 if？

**期望回答方向：**
```c
// while 而不是 if 的原因
while (formed == required) {  // 可能有多個位置滿足條件
    // 記錄當前最小視窗
    // 移除左端，嘗試找到更短的視窗
    left++;
}
```

- 因為可能有重複字元，多次移除仍然滿足條件
- 例如：s="ADOBECODEBANC", t="ABC"
  - 當到達位置 8（CODE**B**ANC） 時，視窗包含 A、B、C
  - 移除第一個 A 後（left=1），仍然包含所有字元
  - 持續收縮直到移除到某個必要的字元

**追問探針：**
1. "如果用 if 代替 while 會怎樣？"
   - 回答：只會檢查一次，可能錯過最小視窗

2. "最壞情況下 while 循環會執行幾次？"
   - 回答：每個字元最多進入和離開一次，總共 O(n) 次

3. "這與 TCP 中接收窗口推進的區別是什麼？"
   - 回答：TCP 視窗逐步推進；LeetCode 尋找所有滿足條件的視窗

**測試維度：** 對演算法邏輯的深度理解

---

### 問題 3：假如 t 極其龐大（超過 s 的長度）會如何？

**期望回答方向：**
- 立即返回 0（無有效視窗）
- `required > s.length()` 時不可能滿足

**改進建議：**
```c
int minWindow(char* s, char* t, char* result) {
    int slen = strlen(s), tlen = strlen(t);

    // 提早終止
    if (tlen > slen) {
        return 0;
    }

    // ... 其餘邏輯
}
```

**追問探針：**
1. "如何在初始化時檢測不可能的情況？"
2. "現實網路場景中如何對應？"
   - 答：TCP 中，如果要求的視窗大小超過可用記憶體，連接會失敗

**測試維度：** 邊界條件處理、防禦性編程

---

### 問題 4：描述視窗擴展與收縮的完整週期

**期望回答方向：**
```
初始化: left=0, right=0, formed=0
      ↓
擴展(右指針): right 遍歷字串，累積字元計數
      ↓
檢查: 是否 formed == required?
      ↓
不滿足 → 繼續擴展
      ↓
滿足 → 記錄視窗 → 收縮(左指針) → 重複直到不滿足
      ↓
右指針到達末尾 → 結束
```

**追問探針：**
1. "這個兩指針模式在其他問題中的應用？"
   - 盛水最多的容器、三數之和、無重複字元最長子串等

2. "與 TCP 接收窗口管理的相似性是什麼？"
   - TCP 的左邊界（rcv_nxt）代表已確認的資料起點
   - TCP 的右邊界（rcv_wnd）由接收緩衝動態決定
   - 兩者都需要有序推進

**測試維度：** 全局演算法理解、問題建模能力

---

### 問題 5：如何應對 Unicode / UTF-8 輸入？

**期望回答方向：**
```c
// 方案 A：擴大陣列（簡單但浪費）
int need[65536] = {0};  // 基本多文種平面 (BMP)

// 方案 B：使用哈希表（推薦）
struct HashMap {
    int capacity;
    struct Entry {
        char key;
        int value;
    } *entries;
};

// 方案 C：UTF-8 解碼（複雜但精確）
int get_utf8_codepoint(const char *p);
```

**討論內容：**
- ASCII 假設會導致 Unicode 字串失敗
- UTF-8 多位元組編碼需要正確解析
- Kernel 中的字串處理如何處理多位元組字元？

**測試維度：** 國際化意識、實用問題解決

---

### 問題 6：如何與 TCP 流量控制的視窗概念關聯？

**期望回答方向：**

| 觀點 | TCP 流量控制 | LeetCode 視窗搜尋 |
|-----|-----------|-----------------|
| 視窗代表 | 可以發送的資料範圍 | 可能包含目標的子串範圍 |
| 擴展時機 | 接收方有更多緩衝空間 | 新字元可能完成目標匹配 |
| 收縮時機 | 接收方緩衝滿 | 可以移除最左邊仍滿足條件 |
| 最佳化 | 最大化吞吐量 | 最小化視窗大小 |

**深入思考：**
```c
// TCP window scaling factor
u8 snd_wscale;  // 發送視窗縮放
u8 rcv_wscale;  // 接收視窗縮放

// 類似的概念：如何縮放我們的搜尋速度？
// 透過提早終止、預先計算等
```

**追問探針：**
1. "TCP 視窗為何需要縮放？"
   - 高延遲高頻寬網路需要很大的視窗
   - 標準 TCP 視窗只有 16 位（最大 64KB），不夠用

2. "這對我們的演算法有什麼啟示？"
   - 考慮資料規模，調整策略（固定陣列 vs 哈希表）

**測試維度：** 系統設計思維、知識遷移能力

---

## 4. 面試加分總結

### 深度知識亮點

1. **系統層面的理解**
   - 不僅知道演算法本身，還能關聯到核心概念
   - 展示對 TCP/IP 協議棧的認識
   - 能夠用網路術語解釋資料結構選擇

2. **工程最佳實踐**
   - 討論權衡（固定陣列 vs 哈希表、迴圈 vs 條件）
   - 提出邊界條件檢測（t 超過 s）
   - 考慮國際化支援（Unicode 處理）

3. **Linux Kernel 知識遷移**
   - 舉例說明 `tcp_input.c` 中的視窗推進邏輯
   - 引用 `lib/ts_kmp.c` 等字串匹配實現
   - 展示對記憶體管理、複雜度分析的理解

4. **追問駕馭能力**
   - 對每個追問都能給出技術深度回答
   - 能主動提出相關延伸（Unicode、效能最佳化）
   - 展現對類似問題（三數之和、盛水最多）的通解能力

### 回答框架

當被問到滑動窗口問題時：

```
1. 提出問題的核心：
   "這題的關鍵是維持一個動態視窗，類似 TCP 協議中的流量控制"

2. 解釋資料結構選擇：
   "使用固定 128 陣列因為 ASCII 有界，但需要考慮 Unicode 場景"

3. 描述演算法邏輯：
   "兩指針遍歷，右指針擴展視窗，當滿足條件時用 while 循環收縮"

4. 關聯到系統層面：
   "與 kernel 中 tcp_input.c 的視窗管理原理一致"

5. 提出最佳化方向：
   "可以提前終止、調整資料結構、支援 Unicode 等"
```

---

## 5. Sources

### Linux Kernel 官方倉庫
- [net/ipv4/tcp_input.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_input.c) - TCP 接收窗口、流量控制實現
- [net/ipv4/tcp_output.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp_output.c) - TCP 發送窗口、擁塞控制實現
- [net/ipv4/tcp.c](https://github.com/torvalds/linux/blob/master/net/ipv4/tcp.c) - TCP 協議核心實現

### 字串匹配演算法
- [lib/ts_kmp.c](https://github.com/torvalds/linux/blob/master/lib/ts_kmp.c) - Knuth-Morris-Pratt 線性字串匹配
- [lib/ts_bm.c](https://github.com/torvalds/linux/blob/master/lib/ts_bm.c) - Boyer-Moore 高效字串匹配
- [lib/string.c](https://github.com/torvalds/linux/blob/master/lib/string.c) - 基本字串操作

### 相關技術文章
- [Cloudflare: When the window is not fully open](https://blog.cloudflare.com/when-the-window-is-not-fully-open-your-tcp-stack-is-doing-more-than-you-think/) - TCP 接收窗口管理深度分析
- [Linux TCP Manual Page](https://man7.org/linux/man-pages/man7/tcp.7.html) - TCP 協議參考文檔

---

**筆記：** 本文檔旨在為 LeetCode 問題 76 與 Linux kernel 實現之間建立概念橋樑。核心洞察是：現代作業系統核心中的許多演算法與資料結構（如 TCP 滑動窗口）在本質上反映了相同的計算思想，只是應用場景不同。掌握這些聯繫將大幅提升系統層面的工程思維。
