# Bit Manipulation 位元操作模式

## 簡介

位元操作把整數當作「位元向量」處理。在 C 這類低階語言裡特別自然 — 因為 C 的整數就是硬體上的 register，位元操作是 O(1) 且無記憶體額外開銷。面試常見場景：常數空間的集合表示、O(1) 狀態壓縮、XOR 找唯一元素、2 的冪判斷、快速乘除 2。

## 辨識信號

- 題目要求 **O(1) 空間** 或強調常數空間
- 有「找唯一 / 出現奇數次 / 出現 k 次」的元素
- 子集 / 狀態壓縮 DP（n ≤ 20 左右）
- 2 的冪 / 4 的冪 / 補碼 / 整數反轉
- 需要在 O(1) 時間判斷「前 i 位是否都為 0 / 1」
- 需要在整數集合做 union / intersection / difference

## 核心工具箱

| 操作 | 公式 | 含義 |
|------|------|------|
| 測試第 i 位 | `(x >> i) & 1` | 取得位元值 (0 或 1) |
| 設定第 i 位 | `x \| (1 << i)` | 把該位設為 1 |
| 清除第 i 位 | `x & ~(1 << i)` | 把該位設為 0 |
| 翻轉第 i 位 | `x ^ (1 << i)` | 0↔1 |
| 取最低位 1 | `x & -x` | lowbit，Fenwick tree 核心 |
| 清除最低位 1 | `x & (x - 1)` | popcount / 判 2 冪用 |
| 判 2 的冪 | `x > 0 && (x & (x-1)) == 0` | — |
| popcount（人口計數） | `__builtin_popcount(x)` | GCC/Clang 內建 |
| 前導零個數 | `__builtin_clz(x)` | 最高位起的 0 數 |
| 後導零個數 | `__builtin_ctz(x)` | 最低位起的 0 數 |
| 最高位（log2） | `31 - __builtin_clz(x)` | x>0 |
| XOR 自己 | `x ^ x = 0` | 找重複必用 |
| XOR 0 | `x ^ 0 = x` | identity |

## 變體分類

### 1. XOR 消去法

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 136 | Single Number | Easy | 一數出現 1 次其他 2 次 → XOR 全部 |
| 137 | Single Number II | Med | 一數 1 次其他 3 次 → 位元分別 mod 3 |
| 260 | Single Number III | Med | 兩數 1 次其他 2 次 → 用 lowbit 分組 |
| 268 | Missing Number | Easy | XOR `[0..n]` 與陣列 |
| 389 | Find the Difference | Easy | XOR 兩串所有字元 |

### 2. 集合 / 狀態壓縮

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 78 | Subsets | Med | 枚舉 0..(1<<n)-1，每個 bit 代表「選不選」 |
| 90 | Subsets II | Med | + 去重 |
| 1125 | Smallest Sufficient Team | Hard | bitmask DP，state = 已覆蓋的技能集 |
| 691 | Stickers to Spell Word | Hard | bitmask BFS |

### 3. 單純位操作

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 190 | Reverse Bits | Easy | 逐位翻轉或分治 swap |
| 191 | Number of 1 Bits | Easy | popcount 或 `x & (x-1)` 迴圈 |
| 231 | Power of Two | Easy | `x > 0 && (x & (x-1)) == 0` |
| 338 | Counting Bits | Easy | DP: `dp[i] = dp[i>>1] + (i & 1)` |
| 461 | Hamming Distance | Easy | `popcount(x ^ y)` |

### 4. 位元數學技巧

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 371 | Sum of Two Integers | Med | 不用 `+`，用 XOR + 進位 `(a & b) << 1` |
| 29 | Divide Two Integers | Med | 用 `<<` 做重複減法 |
| 201 | Bitwise AND of Range | Med | 找 [m, n] 共同前綴 |

## C 語言模板

### 枚舉子集

```c
// 所有 {0,1,...,n-1} 的子集
for (int mask = 0; mask < (1 << n); mask++) {
    for (int i = 0; i < n; i++) {
        if (mask & (1 << i)) {
            // i 在這個子集中
        }
    }
}
```

### 枚舉 mask 的所有子集（子集的子集）

```c
// SOS / DP on subsets
for (int sub = mask; sub > 0; sub = (sub - 1) & mask) {
    // sub 是 mask 的一個非空子集
}
// 注意迴圈會跳過空集 0，若需要請額外處理
```

### Single Number III — 兩個出現 1 次的數

```c
int* singleNumber(int* nums, int n, int* rs) {
    int xor_all = 0;
    for (int i = 0; i < n; i++) xor_all ^= nums[i];
    // xor_all = a ^ b，必有某位為 1 (即 a, b 在該位不同)
    int diff = xor_all & -xor_all;   // lowbit
    int a = 0, b = 0;
    for (int i = 0; i < n; i++) {
        if (nums[i] & diff) a ^= nums[i];
        else                b ^= nums[i];
    }
    int* out = malloc(2 * sizeof(int));
    out[0] = a; out[1] = b;
    *rs = 2;
    return out;
}
```

### 不用 `+` 做加法

```c
int getSum(int a, int b) {
    while (b != 0) {
        unsigned carry = ((unsigned)a & b) << 1;  // unsigned 避免左移 UB
        a = a ^ b;                                 // 無進位加法
        b = (int)carry;
    }
    return a;
}
```

### popcount 手寫版（若不信任內建）

```c
int popcount(unsigned x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;   // SWAR：一次加四個 byte
}
```

## 常見陷阱

1. **左移超界是 UB** — `1 << 31` 對 signed int 是 undefined behavior。用 `1u << 31` 或 `1LL << 63`。
2. **負數右移 implementation-defined** — 對 signed 負數，`>>` 是邏輯還是算術由編譯器決定。要明確語意就先 cast 成 unsigned。
3. **`__builtin_clz(0)` 是 UB** — 輸入 0 時行為未定義。先檢查。
4. **XOR 不等於「相減」** — 直覺上 `x ^ x = 0` 像是相減，但 XOR 沒有「大小」概念，例如 `5 ^ 3 = 6`，不是 `5 - 3 = 2`。
5. **32-bit vs 64-bit 整數** — LeetCode 題有時用 `long long`，一定確認 mask 位數夠不夠。

## 面試講解

### 30 秒版

> 位元操作把整數當位元向量。核心工具包三件：`x & (x-1)` 清最低位 1、`x & -x` 取最低位 1、XOR 消去重複。適合用在「常數空間」、「找唯一元素」、「n ≤ 20 的狀態壓縮」三類題目。C 能直接寫這些操作因為整數就是硬體 register。

### 2 分鐘版

> 位元操作在面試有三個典型用法。
>
> 第一類是 XOR 消去。XOR 有三個性質：交換律、結合律、`x ^ x = 0`。所以要找陣列中唯一出現奇數次的元素，直接 XOR 全部就好。如果有兩個，先 XOR 出 `a ^ b`，再找任一位不同的位把原陣列分成兩組，各自 XOR。
>
> 第二類是狀態壓縮。當 n ≤ 20 左右，可以用一個 int 表示「哪些元素被選了」。子集枚舉是 `for mask = 0..(1<<n)-1`，檢查第 i 個是 `mask & (1 << i)`。DP on subsets 常見在旅行推銷員、集合覆蓋、技能組合。
>
> 第三類是整數性質題。判 2 的冪用 `x & (x-1) == 0`；popcount 直接 `__builtin_popcount`；兩數相加不用 `+` 就是 XOR 做無進位加法 + AND 左移做進位，迴圈直到進位為 0。
>
> 實務上 C 裡面要注意兩個地雷：左移越界是 undefined behavior，所以 `1 << 31` 要寫成 `1u << 31`；另一個是負數右移 implementation-defined，要明確就 cast unsigned。
>
> Kernel 裡面大量用位元操作，例如 `include/linux/bitops.h` 提供 `test_bit` / `set_bit` 的原子版本，頁面標記、CPU mask、interrupt 狀態都是 bit-based。

### 常見 follow-up

| 追問 | 回答要點 |
|------|----------|
| "為什麼 `x & (x-1)` 清最低位 1？" | x-1 會把最低位的 1 變 0，該位以下的 0 全變 1，AND 後下面整段歸零 |
| "popcount 在硬體怎麼做？" | x86 有 `popcnt` 指令（SSE4.2），ARM 有 `CNT`，`__builtin_popcount` 會 lower 到該指令 |
| "不用 popcount 怎麼數 1？" | SWAR：一次處理多個 byte；或 Brian Kernighan: `while (x) { cnt++; x &= x-1; }` |
| "為什麼 XOR 對找唯一有用？" | XOR 是 abelian group，作用於 GF(2)；成對的元素互相抵消 |
| "n=40 還能用 bitmask DP 嗎？" | 1<<40 太大，改 meet-in-the-middle：分兩半各 n/2，合併時用 hash |

## 和 Kernel 的關聯

Linux Kernel 是位元操作的重度使用者：

- **`include/linux/bitops.h`** — `test_bit / set_bit / clear_bit / test_and_set_bit` 的原子與非原子版本。大量用在頁面狀態、IRQ mask、CPU affinity。
- **`include/linux/bitmap.h` / `lib/bitmap.c`** — 大型位元陣列的批次操作（union / intersection / weight）。CPU mask、memory node mask、各種 `cpumask_t`。
- **`include/linux/hweight.h`** — popcount 的多種實作，依 CPU feature 選擇。
- **`lib/sort.c`** 裡的 heap sort 用 `1 << ilog2(n)` 找初始根節點層。
- **Buddy allocator (`mm/page_alloc.c`)** — 所有 free list 按 2 的冪組織，`__ffs` / `__fls` 找最低 / 最高位。
- **紅黑樹 `rbtree` 的顏色位** — 把顏色塞進 parent 指標的最低位（因為指標至少 4-byte 對齊），省一個欄位。見 `include/linux/rbtree.h` 的 `rb_parent_color`。

**面試延伸建議**：若談到位元操作，可以舉 `rbtree` 的顏色位壓縮當 kernel 實戰例子 — 這是經典的「記憶體優化 + 位元操作」結合。

## 參見

- [[dynamic-programming]] — bitmask DP 的完整說明
- [[../kernel/rbtree]] — 顏色位壓縮於父指標
- [[../c-idioms/macro-tricks]] — `BIT(n)`, `GENMASK(h,l)` 等位元巨集
- [[../interview/complexity-cheatsheet]] — 位元運算於複雜度的常見誤用
