# LeetCode 1071: Greatest Common Divisor of Strings × Linux 核心原始碼對照

## 問題概述

**LeetCode 1071** 要求找到兩個字串的「最大公因數字串」——即最長的字串 `x`，使 `x` 能重複拼接成 `str1` 也能重複拼接成 `str2`。此問題的數學方法與歐幾里得演算法（GCD）密切相關。

---

## 1. Linux 相關原始碼位置

### 1.1 歐幾里得演算法（GCD）— `include/linux/math.h` / `lib/math/gcd.c`

Linux 核心在多處需要計算最大公因數，例如時脈頻率轉換、排程器時間片計算等。

```c
// include/linux/math.h
#define __gcd(a, b) \
({                                                  \
    typeof(a) __a = (a);                             \
    typeof(b) __b = (b);                             \
    while (__b) {                                    \
        typeof(__b) __t = __b;                       \
        __b = __a % __b;                             \
        __a = __t;                                   \
    }                                                \
    __a;                                             \
})
```

```c
// lib/math/gcd.c — 二進位 GCD（Stein's algorithm）
unsigned long gcd(unsigned long a, unsigned long b)
{
    unsigned long r = a | b;

    if (!a || !b)
        return r;

    /* 移除共同的因子 2 */
    b >>= __ffs(b) - 1;

    while (a) {
        a >>= __ffs(a) - 1;
        if (a < b)
            swap(a, b);
        a -= b;
    }

    return b << (__ffs(r) - 1);
}
```

**對標 LeetCode 1071**：
- LeetCode 的數學方法（方案 2）直接使用 `gcd(len(str1), len(str2))` 來決定 GCD 字串的長度
- 核心的 `gcd()` 函式與 LeetCode 所需的 GCD 運算完全相同
- 核心使用二進位 GCD（Stein's algorithm）避免昂貴的模運算，而 LeetCode 解法通常用歐幾里得演算法

### 1.2 字串比對 — `lib/string.c`

核心提供多種字串操作函式，與 LeetCode 1071 中的字串驗證邏輯相關。

```c
// lib/string.c
int strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;

    while (1) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
    }
    return 0;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
    unsigned char c1, c2;

    while (count) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
        count--;
    }
    return 0;
}
```

**對標 LeetCode 1071**：
- 倉庫的 `solution.c` 手動逐字元比對 `*(ret + i) != *(str1 + pass * len + i)`，相當於核心中 `strncmp()` 的手寫版本
- 官方方案 2 使用 `str1+str2 == str2+str1`，底層即為 `strcmp()` 的呼叫

### 1.3 週期性模式匹配 — `lib/glob.c`

核心的 glob 匹配在檔案系統路徑解析中使用，涉及重複模式的識別。

```c
// lib/glob.c — 萬用字元匹配
bool glob_match(char const *pat, char const *str)
{
    /* ... 涉及重複模式 '*' 的展開，
     * 類似於檢查字串是否由某個子模式重複組成 */
}
```

**對標 LeetCode 1071**：
- 「字串是否由基礎字串重複組成」可以視為一種特殊的模式匹配
- Glob 中的 `*` 展開在概念上類似於「base 重複 k 次」的驗證

### 1.4 時脈頻率計算 — `kernel/time/clocksource.c`

時脈頻率的最簡化使用 GCD 來約分。

```c
// kernel/time/clocksource.c（簡化）
void clocks_calc_mult_shift(u32 *mult, u32 *shift,
                            u32 from, u32 to, u32 maxsec)
{
    /* 利用 GCD 約分 from/to 比率，
     * 避免中間計算溢位 */
    u64 tmp;
    u32 sft, sftacc = 32;

    /* 計算 from 和 to 的 GCD 來約分 */
    tmp = gcd(from, to);
    from /= tmp;
    to /= tmp;
    /* ... */
}
```

**對標 LeetCode 1071**：
- 時脈約分需要 `gcd(from, to)`，如同 LeetCode 需要 `gcd(len1, len2)` 來找到 GCD 字串的長度
- 兩者都是將 GCD 的數學性質應用於實際問題：一個約分頻率比，一個找出字串的最小重複單元

---

## 2. 核心原始碼 vs. LeetCode 實作對照

| 面向 | LeetCode 1071 | Linux 核心 |
|------|--------------|----------|
| **GCD 用途** | 找出字串的最大公因數長度 | 時脈約分、記憶體對齊、排程計算 |
| **GCD 演算法** | 歐幾里得演算法（取模） | 二進位 GCD / Stein's algorithm（位元運算） |
| **字串比對** | 手動逐字元比對或 `strcmp` | `strcmp()` / `strncmp()` / `memcmp()` |
| **驗證策略** | 重複拼接候選並與原字串比較 | 位元遮罩驗證對齊、模運算驗證整除性 |
| **時間複雜度** | O(m+n)（方案 2） | O(log(min(a,b)))（GCD 本身） |
| **溢位考量** | 字串長度 ≤ 1000，無需擔心 | 32/64 位元溢位是核心考量 |
| **記憶體管理** | `malloc` / `realloc` / `free` | `kmalloc` / `kfree`，slab 分配器 |

### 2.1 為什麼 Linux 核心使用二進位 GCD 而非歐幾里得演算法？

**歐幾里得演算法**：
```c
unsigned long gcd_euclid(unsigned long a, unsigned long b) {
    while (b) {
        unsigned long t = b;
        b = a % b;  // 模運算：在某些 CPU 上非常昂貴
        a = t;
    }
    return a;
}
```

**二進位 GCD（Stein's algorithm）**：
```c
// 用位移和減法取代模運算
// 在缺少硬體除法器的嵌入式平台上快 2-5 倍
```

核心選擇二進位 GCD 的原因：
- 模運算（`%`）在 ARM 等嵌入式平台上可能需要 20-40 個時脈週期
- 位移（`>>`）和減法（`-`）只需 1 個時脈週期
- 核心需要在所有架構上保持高效，包括無硬體除法器的平台

---

## 3. 主管代碼審查角度的考題

### Q1: 為什麼 `str1 + str2 == str2 + str1` 能證明存在公因數字串？

**答案框架**：
```
若兩個字串都由相同的基礎片段 base 重複組成，
例如 str1 = base^p, str2 = base^q，
則 str1 + str2 = base^(p+q) = str2 + str1。
反之，若串接順序不同則結果不同，表示不存在這樣的 base。
```

**考察重點**：數學歸納法、字串週期性的理解

### Q2: 為什麼 GCD 字串的長度一定是 gcd(m, n)？

**答案框架**：
```
假設 base 的長度為 L，則 L | m 且 L | n（L 整除 m 和 n）。
GCD 字串要求 L 最大，而滿足 L | m 且 L | n 的最大 L
根據定義就是 gcd(m, n)。
可透過反證法排除更長或更短的候選。
```

**考察重點**：數論基礎、證明能力

### Q3: 倉庫的 solution.c 中使用 `goto` 是否恰當？

**答案框架**：
```
在 C 語言中，goto 用於錯誤處理和多層迴圈跳出是可接受的模式
（Linux 核心大量使用此模式）。但此處的 goto 用於
控制流程中的「提前失敗」，可考慮重構為 return 語句
或使用旗標變數來提高可讀性。
```

**考察重點**：C 語言慣例、Linux 核心編碼風格、程式碼可維護性

### Q4: 在核心中 `gcd()` 被呼叫時需要注意什麼？

**答案框架**：
```
1. 輸入為 0 的邊界情況：gcd(0, n) = n
2. 溢位風險：中間計算不應超出型別範圍
3. 效能：在熱路徑上避免歐幾里得演算法的模運算
4. 鎖的考量：GCD 計算本身是純計算，不需鎖
```

**考察重點**：防禦性編程、效能意識

### Q5: `realloc` 縮小記憶體的行為是否保證安全？

**答案框架**：
```
C 標準規定 realloc 縮小時通常不會失敗（但不保證）。
在 Linux 核心中，等效操作是 krealloc()，
其行為依賴 slab 分配器——若新大小仍在同一 slab class 內，
則直接返回原指標。在用戶空間 glibc 中，
realloc 縮小幾乎總是成功且原地執行。
```

**考察重點**：記憶體分配器內部機制、C 標準規範

---

## 4. 面試加分總結

### 4.1 展示數學與系統的橋接能力

**說法示例**：
> 「LeetCode 1071 的數學解法直接使用歐幾里得演算法來計算字串長度的 GCD。
> 在 Linux 核心中，同樣的 GCD 運算被用於時脈頻率約分
> （`clocks_calc_mult_shift`）和記憶體對齊計算。核心選擇使用
> 二進位 GCD（Stein's algorithm）而非歐幾里得演算法，是因為
> 位移和減法在嵌入式平台上比模運算更高效。」

### 4.2 展示字串處理的工程經驗

**說法示例**：
> 「在驗證字串是否由基礎字串重複組成時，核心的 `strncmp` 提供了
> 高效的前綴比對。而 `str1+str2 == str2+str1` 這個巧妙的存在性檢查
> 在概念上類似於核心中的環形緩衝區驗證——透過首尾相接來確認
> 資料的一致性。」

### 4.3 展示記憶體管理意識

**說法示例**：
> 「倉庫的解法使用 `malloc` + `realloc` 管理候選字串的記憶體。
> 在核心中對應的是 `kmalloc` + `krealloc`，需要額外考慮
> GFP flags、slab 分配器的行為、以及中斷上下文中的分配限制。
> 面試中可以討論 `realloc` 縮小時的行為差異。」

---

## Sources / 參考資源

- [Linux Kernel Source - include/linux/math.h](https://github.com/torvalds/linux/blob/master/include/linux/math.h)
  核心的 `__gcd` 巨集定義

- [Linux Kernel Source - lib/math/gcd.c](https://github.com/torvalds/linux/blob/master/lib/math/gcd.c)
  二進位 GCD（Stein's algorithm）實作

- [Linux Kernel Source - lib/string.c](https://github.com/torvalds/linux/blob/master/lib/string.c)
  核心字串操作函式（`strcmp`、`strncmp`、`memcmp`）

- [Linux Kernel Source - kernel/time/clocksource.c](https://github.com/torvalds/linux/blob/master/kernel/time/clocksource.c)
  時脈頻率轉換中的 GCD 應用

- [Linux Kernel Source - lib/glob.c](https://github.com/torvalds/linux/blob/master/lib/glob.c)
  萬用字元模式匹配

- [LeetCode 1071 - Greatest Common Divisor of Strings](https://leetcode.com/problems/greatest-common-divisor-of-strings/)
  題目與討論

---

## 結語

「字串的最大公因數」這道題直接對應數學中的歐幾里得演算法，而此演算法在 Linux 核心中
無處不在：從時脈頻率約分、記憶體頁面對齊到排程器的時間片計算。核心甚至為了效能
特地採用二進位 GCD 來替代傳統的歐幾里得演算法。掌握 GCD 的數學性質和工程應用，
不僅能優雅地解決 LeetCode 問題，更能理解作業系統底層的數值運算設計。
