# 345. Reverse Vowels of a String — Linux Kernel 連結分析

## Linux 相關原始碼位置

本題的核心模式是「雙指標從兩端向中間收斂、依條件選擇性交換」。此模式在 Linux kernel 中多處出現：

| 檔案 | 角色說明 |
|---|---|
| `lib/sort.c` | 通用排序函式，partition 階段使用雙指標從兩端掃描並交換 |
| `lib/string.c` | `strrev()`（部分架構實作）使用雙指標反轉字串 |
| `net/core/filter.c` | BPF/packet filter 中的 `sk_run_filter` 會做雙指標式的資料掃描 |
| `fs/binfmt_elf.c` | ELF loader 解析段表時使用前後掃描匹配模式 |
| `include/linux/swap.h` | `swap()` 巨集定義——kernel 中通用的安全交換機制 |

### 最直接的對應：`lib/sort.c` 的 partition

kernel 的 `sort()` 函式（基於 heapsort，但早期版本與 qsort 的 partition 類似）中，partition 步驟會：
1. 左指標從低端往高端掃描，直到找到應該在右半的元素
2. 右指標從高端往低端掃描，直到找到應該在左半的元素
3. 交換兩者
4. 重複直到指標交叉

這與本題「左指標找母音、右指標找母音、交換」的邏輯完全同構。

## Kernel vs. LeetCode 實作對照

| 面向 | Kernel `lib/sort.c` | LeetCode `solution.c` |
|---|---|---|
| 掃描方向 | 雙向（左→右、右→左） | 雙向（左→右、右→左） |
| 掃描停止條件 | 找到不符合 partition 條件的元素 | 找到母音字元 |
| 交換機制 | `swap()` 巨集（使用暫存變數，支援任意型別） | `swap_xor()`（XOR 位元運算） |
| 型別泛用性 | 高——`swap()` 透過 `size_t` 參數支援任意大小 | 低——僅限 `char` |
| 邊界保護 | 嚴格——防止越界存取 | 不完整——內迴圈缺少邊界檢查 |
| 記憶體模型 | 原地操作（in-place） | malloc 新字串（非原地） |

### Kernel 的 `swap()` 巨集

```c
/* include/linux/swap.h 風格 */
#define swap(a, b) \
    do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)
```

Kernel 明確**不使用** XOR swap，原因：
1. XOR swap 對同一位址操作會歸零（`a ^ a == 0`）
2. 暫存變數版本在現代 CPU 上通常更快（compiler 可最佳化為暫存器操作）
3. 可讀性更好，code review 時不易出錯

## 主管 Code Review 角度考題

### Q1：為什麼 kernel 的 `swap()` 不用 XOR 交換？
**期待回答方向：** XOR swap 有同位址歸零的陷阱，且暫存變數版本讓 compiler 更容易最佳化。在 kernel 中可靠性和可讀性優先於「省一個暫存變數」的技巧。
**真正考什麼：** 是否理解 micro-optimization 的陷阱和 production code 的取捨。

### Q2：你的內迴圈 `while (!isVowel(*s_rptr))` 如果字串中完全沒有第二個母音會怎樣？
**期待回答方向：** `s_rptr` 會一路遞減到字串開頭之前，造成未定義行為。應加入 `s_lptr < s_rptr` 作為內迴圈的防護條件。
**追問：** kernel 中對 buffer boundary 的處理原則是什麼？
**真正考什麼：** 防禦式程式設計意識、buffer overflow 敏感度。

### Q3：為什麼要 malloc 一份新字串而不是原地修改？
**期待回答方向：** LeetCode C 介面的慣例是回傳新配置的字串。但在面試中應說明原地版本的空間優勢。Kernel 中大量使用原地操作以避免不必要的 heap 配置。
**追問：** kernel 中的 `kmalloc` 在中斷上下文中安全嗎？什麼時候用 `GFP_ATOMIC` vs `GFP_KERNEL`？
**真正考什麼：** 記憶體配置意識、是否了解 heap allocation 的成本。

### Q4：`isVowel()` 用 switch 和用查表（lookup table）哪個較快？
**期待回答方向：** Compiler 通常會將 switch 最佳化為跳表或查表。但若手動使用 256-byte 的 `bool vowel_table[256]`，可保證 O(1) 查表且 branch-free。Kernel 的 `lib/ctype.c` 就是用查表法分類字元。
**追問：** 查表法對 cache 的影響是什麼？
**真正考什麼：** cache 局部性意識、資料驅動 vs 控制流驅動的選擇。

### Q5：雙指標的外迴圈是 `while (s_lptr < s_rptr)`，為什麼不是 `<=`？
**期待回答方向：** 當 `s_lptr == s_rptr` 時，指向同一個字元，不需要交換（自己和自己交換無意義）。但若用 XOR swap 且未加 `a == b` 的防護，還會導致歸零 bug。
**真正考什麼：** 邊界條件推理能力、off-by-one 錯誤的意識。

## 面試加分總結

- 提及 kernel 的 `swap()` 巨集使用暫存變數而非 XOR，展示對 production code 品質的理解
- 指出 `lib/ctype.c` 的查表法字元分類，展示對 branch-free programming 的認識
- 主動說明 malloc vs in-place 的 trade-off，帶出 kernel 的記憶體配置策略
- 討論內迴圈邊界保護的必要性，展示防禦式程式設計能力
- 理解雙指標技巧在 partition / quickselect 中的廣泛應用，能舉一反三

## Sources

- [lib/sort.c (Linux kernel)](https://github.com/torvalds/linux/blob/master/lib/sort.c)
- [include/linux/minmax.h — swap macro](https://github.com/torvalds/linux/blob/master/include/linux/minmax.h)
- [lib/ctype.c — character classification](https://github.com/torvalds/linux/blob/master/lib/ctype.c)
- [lib/string.c — string utilities](https://github.com/torvalds/linux/blob/master/lib/string.c)
