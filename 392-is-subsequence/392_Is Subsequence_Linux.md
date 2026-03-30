# LeetCode 392: Is Subsequence — Linux 核心程式碼實踐

## 問題概述

**LeetCode 392**: 給定兩個字串 `s` 和 `t`，判斷 `s` 是否為 `t` 的**子序列**（subsequence）。子序列是指在原字串中刪除某些字元後，不改變剩餘字元相對位置而得到的序列。

**輸入例**:
- `s = "abc"`, `t = "ahbgdc"` → `true` （`s` 是 `t` 的子序列）
- `s = "axc"`, `t = "ahbgdc"` → `false` （無法按順序找到 'x'）

**核心演算法**: 兩指標掃描（Two-Pointer Greedy Scan）
- 時間複雜度: O(|t|)
- 空間複雜度: O(1)

---

## Linux 相關原始碼位置

### 1. **lib/string.c** — 字串掃描與匹配

**位置**: [`linux/lib/string.c`](https://github.com/torvalds/linux/blob/master/lib/string.c)

**相關函式**: `strstr()`、`str_has_prefix()`

```c
// 簡化版本的 strstr 邏輯（核心思想）
char *strstr(const char *s1, const char *s2)
{
    size_t l1, l2;
    l2 = strlen(s2);
    if (!l2)
        return (char *)s1;
    l1 = strlen(s1);
    while (l1 >= l2) {
        l1--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }
    return NULL;
}
```

**連結**: 與 subsequence 檢查相似的**逐字元掃描**模式。`strstr()` 在 haystack 中尋找完整的 needle，但子序列檢查允許間隙（gap）。兩者都避免重新掃描已匹配的字元。

---

### 2. **lib/glob.c** — 貪心字元掃描模式

**位置**: [`linux/lib/glob.c`](https://github.com/torvalds/linux/blob/master/lib/glob.c)

**相關函式**: `glob_match()`

```c
// 簡化的 glob_match 核心邏輯
static bool __pure glob_match(const char *pat, const char *str)
{
    // 處理 '*' 通配符：貪心掃描
    // '*' 可匹配零個或多個任意字元（類似 subsequence 中的間隙）
    while (*pat == '*') {
        pat++;
        if (glob_match(pat, str))
            return true;
        if (*str == '\0')
            return false;
        str++;
    }
    // 逐字元掃描，直到不匹配
    while (*str != '\0' && *pat != '\0') {
        if (*pat == '?') {
            pat++;
            str++;
        } else if (*pat != *str) {
            return false;
        } else {
            pat++;
            str++;
        }
    }
    return *pat == '\0' && *str == '\0';
}
```

**連結**: `glob_match()` 的**貪心雙指標掃描**是子序列檢查的直接類比：
- `*` 在 glob 中允許跳過任意字元（對應子序列中的跳過）
- 當字元匹配時，兩個指標同時前進
- 時間複雜度同樣取決於被掃描字串的長度

---

### 3. **fs/seq_file.c** — 序列化掃描介面

**位置**: [`linux/fs/seq_file.c`](https://github.com/torvalds/linux/blob/master/fs/seq_file.c)

**相關函式**: `seq_file` 迭代器介面（`start()`、`next()`、`stop()`、`show()`）

```c
// seq_file 迭代器介面的概念範例
struct seq_operations {
    void * (*start) (struct seq_file *m, loff_t *pos);
    void  (*stop)  (struct seq_file *m, void *v);
    void * (*next) (struct seq_file *m, void *v, loff_t *pos);
    int   (*show)  (struct seq_file *m, void *v);
};
```

**連結**: 子序列檢查的**迭代前進邏輯**與 seq_file 的順序掃描相似：
- `start()` 初始化位置（對應兩個指標初始化）
- `next()` 前進到下一個匹配元素（對應貪心的指標推進）
- `stop()` 清理資源（對應演算法終止）
- 核心概念：**單向遍歷**，狀態由遊標（cursor）維護

---

### 4. **lib/ts_kmp.c** — Knuth-Morris-Pratt 文字搜尋

**位置**: [`linux/lib/ts_kmp.c`](https://github.com/intel/linux-intel-lts/blob/master/lib/ts_kmp.c)

**演算法**: Knuth-Morris-Pratt (KMP) 模式匹配

- **時間複雜度**: O(|pattern| + |text|)
- **應用**: 核心用於網路封包檢查（`xt_string` 模組）

```c
// KMP 中的前綴函式概念（部分）
// 當不匹配時，利用前綴資訊跳過已掃描的字元
// 避免從頭重新掃描
```

**連結**: 雖然 KMP 比簡單的子序列檢查複雜得多，但**核心思想相同**：
- 避免重複掃描已檢查的字元
- 利用已知的匹配狀態進行智慧跳躍
- 適用於大規模文字搜尋場景

---

### 5. **net/netfilter/xt_string.c** — 網路封包字串匹配

**位置**: [`linux/net/netfilter/xt_string.c`](https://github.com/torvalds/linux/blob/master/net/netfilter/xt_string.c)

**功能**: 在 IP 封包資料中搜尋特定字串模式

```c
// 概念化的 netfilter 字串匹配邏輯
// 在偏移範圍 [from_offset, to_offset] 內搜尋模式
static bool string_mt(const struct sk_buff *skb,
                      struct xt_action_param *par,
                      const char *pattern,
                      size_t from_offset,
                      size_t to_offset)
{
    // 使用 skb_find_text() 掃描封包資料
    // 類似於 subsequence 檢查：在約束範圍內逐位元組掃描
}
```

**連結**: 網路資料檢查的**約束範圍掃描**：
- 在指定的位元組範圍內搜尋
- 支援 Boyer-Moore 和 KMP 演算法優化
- 實務應用：檢查 HTTP 請求頭、DNS 查詢等

---

## Kernel vs. LeetCode 實作對照

### 簡單子序列檢查（LeetCode 392）

```c
// LeetCode 392 標準解答（C 版本）
bool isSubsequence(char *s, char *t)
{
    int i = 0;  // s 的指標
    int j = 0;  // t 的指標

    while (s[i] != '\0' && t[j] != '\0') {
        if (s[i] == t[j]) {
            i++;  // 匹配，s 指標前進
        }
        j++;    // 無論是否匹配，t 指標總是前進
    }

    // 若 s 全部匹配，i 應等於 s 的長度
    return s[i] == '\0';
}
```

### Kernel 的 strstr() 實現

```c
// linux/lib/string.c 中的 strstr()
// 核心邏輯：尋找**完整的**連續子字串
char *strstr(const char *s1, const char *s2)
{
    // ...字串長度計算...
    while (l1 >= l2) {
        if (!memcmp(s1, s2, l2))  // 完整匹配整個 s2
            return (char *)s1;
        s1++;  // 滑動視窗
    }
    return NULL;
}
```

### 對照表

| 特性 | LeetCode 392 (子序列) | lib/string.c (strstr) | lib/glob.c (glob_match) |
|------|---------------------|----------------------|------------------------|
| **目標** | 檢查非連續子序列 | 尋找連續子字串 | 模式匹配（含萬用字元） |
| **掃描策略** | 貪心（單向掃描） | 滑動視窗 | 貪心 + 遞迴 / 回溯 |
| **時間複雜度** | O(\|t\|) | O(\|s1\| × \|s2\|) | O(\|str\| × \|pat\|) |
| **指標數量** | 2 個 | 1 個（加視窗） | 2 個（帶回溯） |
| **間隙處理** | 允許字元間隙 | 不允許間隙 | 允許（透過 `*`) |
| **應用場景** | 子序列驗證 | 字串查找 | 裝置黑名單、萬用字元 |

---

## 主管 Code Review 角度考題

### 1. **時間複雜度驗證**
> **問**: 為什麼 LeetCode 392 的兩指標方案是 O(|t|) 而非 O(|s| × |t|)？
>
> **答**: 因為 `t` 的指標 `j` 只會單調前進（never backtrack），而不是針對 `s` 的每個字元重新掃描 `t`。每個 `t` 中的字元最多被檢查一次。

### 2. **與 strstr() 的區別**
> **問**: `strstr()` 和子序列檢查都涉及字串掃描。為何 `strstr()` 需要回溯（backtrack）但子序列檢查不需要？
>
> **答**: `strstr()` 需要尋找**連續**的完整子字串，所以當匹配失敗時，必須回溯到下一個可能的起點。而子序列允許間隙，所以 `t` 的指標永遠向前，不需回溯。

### 3. **Kernel 中的應用**
> **問**: Linux kernel 在哪些場景下需要類似「子序列檢查」的邏輯，而不是完整的字串搜尋？
>
> **答**:
> - **glob 模式匹配**（`lib/glob.c`）：`*` 萬用字元允許跳過任意字元，語意上接近子序列
> - **seq_file 迭代**（`fs/seq_file.c`）：單向掃描資料，狀態由遊標維護，不回溯
> - **网络过滤**（`xt_string`）：在約束的位元組範圍內尋找，類似「受限的子序列」

### 4. **邊界條件處理**
> **問**: 在 kernel 代碼中，如何安全地處理空字串或 NULL 指標？
>
> **答**:
> ```c
> // 子序列檢查的安全版本
> bool isSubsequence(const char *s, const char *t)
> {
>     if (!s || !t) return false;  // NULL 檢查
>     if (*s == '\0') return true;  // 空 s 是任何字串的子序列
>
>     for (; *t != '\0'; t++) {
>         if (*s == *t) s++;
>     }
>     return *s == '\0';
> }
> ```

### 5. **大規模資料優化**
> **問**: 如果 `t` 非常大（數 GB），而需要檢查多個子序列，LeetCode 方案足夠嗎？
>
> **答**: 不足。應考慮：
> - **KMP 預處理**（如 `lib/ts_kmp.c`）：若 `s` 是常數，預計算失敗函式可加速
> - **Trie / 後綴陣列**：若檢查多個子序列，預構建資料結構
> - **Bloom filter / 位元向量**：用於快速排除不可能的子序列

### 6. **並行化潛力**
> **問**: 這個演算法能否並行化（如多線程）？
>
> **答**: 難。因為兩個指標的推進取決於前一步的結果（資料相依性）。但可以：
> - 使用 SIMD（Single Instruction Multiple Data）進行位元組級並行
> - 將多個子序列檢查任務分配給不同線程（tasklet 或 workqueue）

---

## 主要實作模式比較

### 模式 A: 簡單子序列（LeetCode 392）
**優點**: 簡潔、時間最優
**缺點**: 不適用於有重複掃描需求的場景

```c
bool isSubsequence(char *s, char *t)
{
    while (*s && *t) {
        if (*s == *t) s++;
        t++;
    }
    return !*s;
}
```

### 模式 B: Kernel 的 strstr()（連續子字串）
**優點**: 完整性保證、應用廣泛
**缺點**: 複雜度較高，需要處理回溯

### 模式 C: Kernel 的 glob_match()（貪心模式匹配）
**優點**: 支援萬用字元、語義豐富
**缺點**: 可能有回溯，最壞情況下二次方複雜度

### 模式 D: KMP / Boyer-Moore（高效搜尋）
**優點**: 線性時間、適用大規模文字
**缺點**: 預處理複雜、實作細節繁瑣

---

## 面試加分總結

### 演算法深度
- **理解本質**: 子序列檢查是「在允許間隙的前提下，依序匹配」
- **時間複雜度**: 單向掃描 → O(|t|) 是最優的（因為每字元最多檢查一次）
- **與 KMP 的聯繫**: 雖然 KMP 更複雜，但核心思想相同——避免重複掃描

### Linux Kernel 知識點
1. **lib/string.c**: 提供基礎字串操作，但用於連續子字串搜尋
2. **lib/glob.c**: 展示如何用貪心策略進行模式匹配（`*` 類似於子序列的「跳過」）
3. **fs/seq_file.c**: 展示序列化掃描的設計模式（start/next/stop/show）
4. **lib/ts_kmp.c**: 用於高性能文字搜尋，Netfilter 的 `xt_string` 模組依賴它
5. **net/netfilter/xt_string.c**: 實務應用——在網路封包中進行約束範圍的字串匹配

### 面試回答框架
> **"在 LeetCode 392 的基礎上，我理解了貪心單向掃描的核心。在 Linux kernel 中，我看到類似的模式應用於不同場景："**
> 1. **glob_match()** 用 `*` 萬用字元實現「允許跳過」，語義上類似子序列
> 2. **strstr()** 用滑動視窗處理連續子字串，需要回溯
> 3. **KMP 算法** 為高效搜尋預處理失敗函式，複雜度線性
> 4. **seq_file** 展示如何用遊標（cursor）維護掃描狀態
> 5. **xt_string** 在實際網路過濾中應用字串匹配，有偏移範圍約束

### 進階話題
- **SIMD 優化**: 使用 `memcmp()` / SSE 進行批量位元組比較（kernel 也這樣做）
- **Bloom Filter**: 快速檢驗「字元是否可能出現」
- **預計算失敗函式**: 若子序列 `s` 固定，多次檢查時用 KMP 預處理
- **記憶體局部性**: 順序掃描能充分利用 CPU 快取，見 kernel 中對 cache-friendly 代碼的強調

---

## Sources

- [Linux Kernel: lib/string.c](https://github.com/torvalds/linux/blob/master/lib/string.c) — 字串掃描與 strstr() 實現
- [Linux Kernel: lib/glob.c](https://github.com/torvalds/linux/blob/master/lib/glob.c) — glob_match() 貪心模式匹配
- [Linux Kernel: fs/seq_file.c](https://github.com/torvalds/linux/blob/master/fs/seq_file.c) — 序列檔案迭代介面
- [Linux Kernel: lib/ts_kmp.c](https://github.com/intel/linux-intel-lts/blob/master/lib/ts_kmp.c) — Knuth-Morris-Pratt 文字搜尋
- [Linux Kernel: net/netfilter/xt_string.c](https://github.com/torvalds/linux/blob/master/net/netfilter/xt_string.c) — 網路封包字串匹配模組
- [The seq_file Interface — Linux Kernel Documentation](https://docs.kernel.org/filesystems/seq_file.html) — 序列檔案 API 文檔
- [LWN.net: Knuth-Morris-Pratt textsearch algorithm](https://lwn.net/Articles/141162/) — KMP 在核心中的應用說明
- [LWN.net: Driver porting: The seq_file interface](https://lwn.net/Articles/22355/) — seq_file 設計與最佳實踐

---

**文件建立日期**: 2026-03-30
**目標受眾**: 準備系統程式設計面試、對 Linux kernel 實踐有興趣的工程師
**知識深度**: 中級至進階（假設讀者已熟悉基本的資料結構和演算法）
