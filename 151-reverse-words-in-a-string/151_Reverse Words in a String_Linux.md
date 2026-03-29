# LeetCode 151 - 反轉字符串中的單詞 (Reverse Words in a String)

## 演算法概述

此題採用**原地反轉策略**：先反轉整個字符串，再反轉每個單詞，同時用寫指針壓縮空格。時間複雜度 O(n)，空間複雜度 O(1)。

```c
void reverseWords(char* s, int sSize) {
    int n = strlen(s);

    // 第一步：反轉整個字符串
    short left = 0, right = n - 1;
    while (left < right) {
        char tmp = s[left];
        s[left] = s[right];
        s[right] = tmp;
        left++;
        right--;
    }

    // 第二步：反轉每個單詞，並壓縮空格
    int k = 0;  // 寫指針
    for (int i = 0; i < n; i++) {
        if (s[i] != ' ') {
            // 單詞間加入空格
            if (k > 0) s[k++] = ' ';

            // 複製單詞
            int start = i;
            while (i < n && s[i] != ' ') i++;

            // 反轉當前單詞
            left = k;
            right = k + (i - start) - 1;
            while (left < right) {
                char tmp = s[left];
                s[left] = s[right];
                s[right] = tmp;
                left++;
                right--;
            }
            k += i - start;
        }
    }

    s[k] = '\0';
}
```

---

## Linux 相關原始碼位置

### 1. **lib/string.c** — 核心字符串函數
- **位置**：`linux/lib/string.c`
- **相關函數**：
  - `strrev()` — 字符串反轉（若實作）
  - `memmove()` — 安全的記憶體移動，處理重疊複製
  - `memcpy()` — 非重疊複製

核心實作使用 `memmove` 而非手動迴圈，以提高可讀性和安全性：

```c
// kernel 典型做法（lib/string.c）
void *memmove(void *dest, const void *src, size_t count)
{
    char *d = dest;
    const char *s = src;

    // 處理重疊情況
    if ((unsigned long)dest < (unsigned long)src) {
        while (count--)
            *d++ = *s++;
    } else {
        d += count;
        s += count;
        while (count--)
            *--d = *--s;
    }
    return dest;
}
```

### 2. **lib/string_helpers.c** — 字符串輔助函數
- **位置**：`linux/lib/string_helpers.c`
- **功能**：字符串轉義、反轉、分割等操作
- **相關函數**：`string_unescape()` — 處理轉義序列

### 3. **fs/proc/cmdline.c** — 核心命令列解析
- **位置**：`linux/fs/proc/cmdline.c`
- **概念**：以空格分隔的 token 處理
- 與 LeetCode 151 類似的場景：解析空格分隔的單詞

### 4. **net/core/utils.c** — 網路地址字符串操作
- **位置**：`linux/net/core/utils.c`
- **特性**：
  - 原地字符操作（in-place transformation）
  - 空間最小化
  - 邊界檢查

### 5. **include/linux/string.h** — 字符串宣告
- 定義字符串工具函數的公共介面

### 6. **net/core/devlink.c 與 trace/** — 高級字符串處理
- 字符串反轉、篩選、格式化

---

## Kernel vs. LeetCode 實作對照

| 面向 | Linux Kernel 做法 | LeetCode 解法 | 為什麼不同 |
|------|---|---|---|
| **記憶體操作** | 使用 `memmove` 安全處理重疊 | 手動迴圈複製 | kernel 強調安全性和可靠性 |
| **交換變數** | `temp` 局部變數 | XOR 交換（`a ^= b ^= a ^= b`） | kernel 優先可讀性；LeetCode 測試微最佳化 |
| **防禦性程式設計** | 檢查 NULL 指針、大小邊界 | 假設合法輸入 | kernel 處理不信任的系統呼叫 |
| **資料型別** | `size_t` 無符號整數（64-bit）| `int` 簽名整數（32-bit） | kernel 支援任意大小；LeetCode 受限輸入 |
| **Tokenization** | `strsep()`, `strtok_r()` | 手動掃描找空格 | kernel API 提供通用介面 |
| **編譯最佳化** | 依賴編譯器內聯 | 明確手寫最佳化 | kernel 信任 gcc/clang；LeetCode 測試意圖 |

### 具體範例：kernel 的 strrev 概念

```c
// kernel 風格的字符串反轉（假設實作）
static inline void reverse_string(char *s, int len)
{
    char *start = s;
    char *end = s + len - 1;

    while (start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        start++;
        end--;
    }
}
```

相比 LeetCode 的 XOR 交換：
```c
// LeetCode 風格：微最佳化
while (left < right) {
    s[left] ^= s[right];
    s[right] ^= s[left];
    s[left] ^= s[right];
    left++;
    right--;
}
```

---

## 主管 Code Review 角度考題

### 1. **XOR 交換 vs. Temp 變數**
**問題**：為什麼使用 XOR 交換而非 `temp` 變數？

**回答框架**：
- XOR 交換在理論上節省一個變數註冊；但現代編譯器對 `temp` 也能最佳化
- XOR 交換在相同位址時失敗：`a ^= a` 清零
- Kernel 選擇 `temp` 因為可讀性更好（團隊維護成本）
- LeetCode 接受 XOR 以測試位元級別思考

**測試邊界情況**：
```c
// 若 left == right，XOR 會發生什麼？
int a = 5;
a ^= a; a ^= a; a ^= a;  // 結果：0（不符預期）
```

### 2. **`short` vs. `int` 資料型別**
**問題**：為什麼使用 `short` 作為索引？若字符串長度 > 32767 怎辦？

**風險分析**：
- `short` 是 16-bit 簽名整數，範圍 [-32768, 32767]
- 輸入字符串超過 32767 字符時溢位
- 安全做法：使用 `int` 或 `size_t`

**改進**：
```c
// 不安全
short left = 0, right = n - 1;  // n 若 > 32767 則溢位

// 安全做法
int left = 0, right = n - 1;        // 32-bit
// 或
size_t left = 0, right = n - 1;     // 無符號，支援大型緩衝區
```

### 3. **UTF-8 多位元組字符處理**
**問題**：如何處理 UTF-8 編碼的多位元組字符？目前演算法會破壞它們。

**挑戰**：
- UTF-8 字符可能佔 1-4 位元組
- 位元組級別反轉會破壞字符邊界
- 需要逐字符（不是位元組）反轉

**改進方案**：
```c
// 使用 UTF-8 庫（例如 utf8proc）
// 1. 解碼為 Unicode code points
// 2. 反轉 code point 序列
// 3. 重新編碼為 UTF-8
```

**Kernel 参考**：
- `fs/nls/` — 國際化字符集支援
- `lib/utf8.c` — UTF-8 編碼操作

### 4. **memmove vs. 手動迴圈**
**問題**：能否用 `memmove` 替代手動複製迴圈？

**答案**：可以，但需小心重疊。

```c
// 手動迴圈（當前做法）
while (i < n && s[i] != ' ') i++;

// memmove 替代（kernel 風格）
int word_len = i - start;
memmove(s + k, s + start, word_len);
k += word_len;
```

**優缺點**：
| 方法 | 優點 | 缺點 |
|------|------|------|
| 手動迴圈 | 明確意圖，易除錯 | 代碼冗長 |
| memmove | 標準庫，安全 | 函數呼叫開銷 |

### 5. **空格壓縮邏輯**
**問題**：為什麼在 `if (k > 0)` 時才加入空格？

**邏輯**：
- `k = 0` 表示還沒複製任何字符，加空格會產生前導空格
- `k > 0` 確保只在單詞間加入單一空格
- 末尾自動截斷（`s[k] = '\0'`）

**邊界測試**：
```c
// 輸入："  hello  world  "
// 預期："hello world"
// 檢查：無前導、無末尾、無重複空格
```

---

## 面試加分總結

### 系統層知識應用

1. **原地操作意識**
   - 認識 O(1) 空間優化的價值
   - 類比 kernel 中的緩衝區最小化（disk I/O、網路封包）

2. **Kernel String.h 參考**
   - 提及 `strsep()`, `strtok_r()` 等標準工具
   - 展示對 POSIX/GNU 字符串 API 的了解
   - 說明為何不用 `strtok()`（非線程安全）

3. **安全性與可靠性**
   - 解釋 `memmove` vs. `memcpy` 的重疊問題
   - 討論 NULL 檢查和邊界檢查（kernel 做法）
   - 提及 UTF-8、locale-aware 字符處理

4. **位元級最佳化的合理性**
   - XOR 交換、short 索引都是「可以做但不應該做」的決策
   - 展示理解微最佳化的成本（可讀性、錯誤風險）
   - 準備說明：何時才真正需要這些最佳化

5. **測試驅動開發**
   - 列舉邊界情況：空字符串、單詞、多空格、UTF-8
   - 參考 kernel 的單元測試風格（`lib/test_string.c`）
   - 提議加入 assertions 確保不變式（invariants）

### 回答範本

> "我會用 `temp` 變數而非 XOR，因為 kernel 代碼強調可讀性和維護性。kernel 開發者信任編譯器會最佳化，而不是手寫微最佳化。但我理解 LeetCode 場景可能有不同的評判標準。"

> "資料型別選擇應該是 `int` 或 `size_t`，而非 `short`。Kernel 使用 `size_t` 以支援任意大型緩衝區。選 `short` 會在字符串長度 > 32KB 時溢位——這在現代應用中不少見。"

> "UTF-8 支援需要字符級別的操作而非位元組級。我會查閱 `lib/utf8.c` 和 `fs/nls/` 看 kernel 如何處理多位元組編碼的反轉。"

---

## Sources

### Linux Kernel Repository
- **lib/string.c** — Core string functions
  https://github.com/torvalds/linux/blob/master/lib/string.c

- **lib/string_helpers.c** — String manipulation helpers
  https://github.com/torvalds/linux/blob/master/lib/string_helpers.c

- **fs/proc/cmdline.c** — Command-line parsing
  https://github.com/torvalds/linux/blob/master/fs/proc/cmdline.c

- **net/core/utils.c** — Network string utilities
  https://github.com/torvalds/linux/blob/master/net/core/utils.c

- **include/linux/string.h** — String declarations
  https://github.com/torvalds/linux/blob/master/include/linux/string.h

- **lib/test_string.c** — String unit tests
  https://github.com/torvalds/linux/blob/master/lib/test_string.c

- **fs/nls/** — National Language Support (multi-byte encoding)
  https://github.com/torvalds/linux/tree/master/fs/nls

- **lib/utf8.c** — UTF-8 encoding utilities
  https://github.com/torvalds/linux/blob/master/lib/utf8.c

### Related Kernel Concepts
- **strsep()** — POSIX string tokenization (safer than strtok)
- **memmove()** — Memory operations with overlap detection
- **XOR bitwise operations** — Low-level optimization patterns
