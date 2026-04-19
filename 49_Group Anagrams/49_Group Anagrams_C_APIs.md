# LeetCode 49 - Group Anagrams 所用 C 標準函式庫 API 筆記

本筆記整理 `Group Anagrams.c` 中用到的 C 標準函式庫 API，涵蓋函數簽章、正確用法、常見陷阱，以及本題程式碼對應片段。

---

## 1. `qsort` — 通用排序（`<stdlib.h>`）

### 函數簽章

```c
void qsort(void *base,
           size_t nmemb,
           size_t size,
           int (*compar)(const void *, const void *));
```

| 參數 | 說明 |
|------|------|
| `base` | 陣列起始位址 |
| `nmemb` | 元素個數（不是位元組數） |
| `size` | **單一元素**的位元組數（`sizeof(element)`） |
| `compar` | 比較函數指標，回傳 `< 0`、`0`、`> 0` |

回傳值：無。`qsort` 就地排序 `base[0 .. nmemb-1]`。

### 本題中的兩次使用

**(a) 字元排序（canonical key 建立）**

```c
qsort(sorted_strs[i], str_len, sizeof(char), compare_char);
```
- `nmemb = str_len`（字串長度，不含 `'\0'`）
- `size = sizeof(char)` = 1
- 效果：把 `"eat"` 原地排序成 `"aet"`

**(b) 配對陣列排序**

```c
qsort(pairs, strsSize, sizeof(char**), compare_pairs);
```
- 每個元素是 `char**`（指向 `char*[2]`）
- 交換時 `qsort` 搬動的是「指標」，不是字串本身 → O(1) 搬動成本

### 比較函數的撰寫要點

```c
int compare_char(const void* a, const void* b) {
    return *(char*)a - *(char*)b;
}
```

**陷阱 1：整數溢位**

對於 `int` 或 `long` 的比較，**不可以**直接寫 `return *(int*)a - *(int*)b;`。當兩個值一個很大正數、一個很小負數時，減法會溢位，行為未定義。

正確寫法：

```c
int compare_int(const void* a, const void* b) {
    int x = *(int*)a, y = *(int*)b;
    return (x > y) - (x < y);   /* 回傳 -1 / 0 / 1，永不溢位 */
}
```

本題的 `compare_char` 因為 `char` 差值最多 ±127，塞得進 `int`，所以安全；但寫成 `int` 陣列時務必改用上式。

**陷阱 2：參數型別**

`qsort` 傳進比較函數的是「指向元素的指標」。所以當元素本身就是指標（如 `char*`），你拿到的是 `char**`。本題 `compare_pairs` 裡 `pair_a` 的型別是 `const char**`（指向 `char*[2]` 的指標），解參考後才得到兩個 `char*`。

**陷阱 3：不保證穩定**

`qsort` 不保證穩定排序。若需要穩定性，應在比較函數中加入**次要鍵**作為 tie-breaker——這正是 `compare_pairs` 先比 sorted key、再比 original 的原因。

---

## 2. `strcmp` — 字串比較（`<string.h>`）

### 函數簽章

```c
int strcmp(const char *s1, const char *s2);
```

逐位元比對至第一個不同的字元或 `'\0'`；回傳值語意如下：

| 回傳值 | 含意 |
|--------|------|
| `< 0` | `s1` 字典序小於 `s2` |
| `= 0` | 兩字串完全相同 |
| `> 0` | `s1` 字典序大於 `s2` |

### 本題使用

```c
int cmp = strcmp(pair_a[0], pair_b[0]);  /* compare_pairs 主要鍵 */
if (current_sorted == NULL || strcmp(sorted, current_sorted) != 0) {
    /* 進入新群組 */
}
```

### 常見陷阱

**陷阱 1：不要假設回傳值是 -1 / 0 / 1**

C 標準只要求**正負號**正確，實際值可能是任意整數。例如 `strcmp("aet","abt")` 在某些實作回傳 3（'e'-'b'），在別的實作可能回傳 1。所以判斷時請寫：

```c
if (strcmp(a, b) < 0)      /* 正確 */
if (strcmp(a, b) == -1)    /* 錯誤：相依於實作 */
```

**陷阱 2：NULL 指標未定義**

傳入 `NULL` 是未定義行為，必須在呼叫前檢查。本題 `current_sorted == NULL` 的短路判斷正是為此。

**陷阱 3：需要限制長度時用 `strncmp`**

```c
int strncmp(const char *s1, const char *s2, size_t n);
```
只比前 `n` 個字元。適合用於「只想知道某字串是否以特定前綴開頭」等場景。

---

## 3. `strlen` — 字串長度（`<string.h>`）

```c
size_t strlen(const char *s);
```

回傳 `s` 從起點到第一個 `'\0'` **之前**的字元數（不含結尾 `'\0'`）。

### 本題使用

```c
int str_len = strlen(strs[i]);
sorted_strs[i] = (char*)malloc((str_len + 1) * sizeof(char));
```

**注意那個 `+ 1`**：`malloc` 要多配一個 byte 放 `'\0'`，否則 `strcpy` 寫入結尾時會溢位。這是最常見的 off-by-one bug。

### 陷阱

- `strlen` 是 **O(n)** 不是 O(1)。若在迴圈裡反覆呼叫 `strlen(s)`，請先存到變數。
- 回傳值型別是 `size_t`（無號），與 `int` 比較要小心（如 `strlen(s) - 1` 在空字串會底溢成天文數字）。
- 傳入非 `'\0'` 結尾的字元陣列是未定義行為。

---

## 4. `strcpy` — 字串複製（`<string.h>`）

```c
char *strcpy(char *dest, const char *src);
```

把 `src`（含結尾 `'\0'`）逐位元組複製到 `dest`，回傳 `dest`。

### 本題使用

```c
sorted_strs[i] = (char*)malloc((str_len + 1) * sizeof(char));
strcpy(sorted_strs[i], strs[i]);  /* 複製原始字串 */
qsort(sorted_strs[i], str_len, sizeof(char), compare_char);  /* 再原地排序 */
```

**為什麼先複製再排序？** 因為 `qsort` 會修改 `sorted_strs[i]` 的內容，直接排序 `strs[i]` 會破壞原始輸入。

### 陷阱

**緩衝區溢位（Buffer Overflow）**

`strcpy` **完全不檢查 `dest` 的容量**。若 `dest` 長度不足，會寫超出範圍，造成堆疊/堆積毀損，是典型安全漏洞。正式程式碼通常應改用：

```c
char *strncpy(char *dest, const char *src, size_t n);    /* C89, 有自己的坑 */
size_t strlcpy(char *dest, const char *src, size_t size);/* BSD/Linux, 較安全 */
snprintf(dest, size, "%s", src);                         /* C99 可攜性最好 */
```

`strncpy` 的坑：若 `src` 長度 ≥ `n`，**不會**寫入 `'\0'`，需手動補上。

---

## 5. `malloc` / `free` — 動態記憶體（`<stdlib.h>`）

### 函數簽章

```c
void *malloc(size_t size);   /* 配置 size 個 bytes，內容未初始化 */
void  free(void *ptr);       /* 釋放先前 malloc 取得的指標 */
```

相關：
```c
void *calloc(size_t nmemb, size_t size);  /* 配置並清零 */
void *realloc(void *ptr, size_t size);    /* 調整大小 */
```

### 本題使用（典型的多層配置）

```c
/* 二維字串陣列：char** */
char** sorted_strs = (char**)malloc(strsSize * sizeof(char*));

/* 每個 pair 是 char*[2] */
pairs[i] = (char**)malloc(2 * sizeof(char*));

/* 三維結果陣列：char*** */
char*** result = (char***)malloc(strsSize * sizeof(char**));
```

對應的釋放：

```c
for (int i = 0; i < strsSize; i++) {
    free(sorted_strs[i]);  /* 先釋放內層 */
    free(pairs[i]);
}
free(sorted_strs);         /* 再釋放外層 */
free(original_strs);
free(pairs);
```

### 陷阱

**陷阱 1：`malloc` 回傳 `NULL`**

記憶體不足時 `malloc` 回傳 `NULL`，解參考就是段錯誤。嚴謹程式碼應檢查：

```c
char* p = malloc(n);
if (!p) { /* 處理失敗 */ }
```

LeetCode 環境通常省略，但正式程式碼不應省略。

**陷阱 2：配置與釋放要對稱**

多層指標結構要由內往外 `free`，否則內層記憶體洩漏。本題已正確處理 `sorted_strs[i]` → `sorted_strs`。

**陷阱 3：`sizeof` 搞錯型別**

```c
char** p = malloc(n * sizeof(char));   /* ✗ 應為 sizeof(char*) */
char** p = malloc(n * sizeof(char*));  /* ✓ */
```

記法：**配置什麼型別的陣列，`sizeof` 就寫該型別元素**。

**陷阱 4：`malloc(0)`**

C 標準允許 `malloc(0)` 回傳 `NULL` 或一個可被 `free` 的指標，屬於實作定義行為。本題 `malloc(0)` 用於空輸入的邊界處理，可攜性不高，可改為回傳 `NULL`。

**陷阱 5：double free / use-after-free**

同一指標 `free` 兩次、或 `free` 後繼續存取，都是 undefined behavior。習慣在 `free` 後把指標設為 `NULL` 可減少 bug。

---

## 6. 快速對照表

| API | 標頭檔 | 時間複雜度 | 是否就地 | 常見陷阱 |
|-----|--------|-----------|---------|---------|
| `qsort` | `<stdlib.h>` | O(n log n) 平均 | 是 | 比較函數減法溢位、非穩定排序 |
| `strcmp` | `<string.h>` | O(min(m,n)) | 否 | 不保證回傳 ±1、NULL 未定義 |
| `strncmp` | `<string.h>` | O(n) | 否 | 限制長度可避免越界 |
| `strlen` | `<string.h>` | O(n) | 否 | 不含 `'\0'`、回傳 `size_t` |
| `strcpy` | `<string.h>` | O(n) | 是（`dest`） | 無邊界檢查，改用 `snprintf`/`strlcpy` |
| `malloc` | `<stdlib.h>` | ~O(1) | — | 可能回 `NULL`、`sizeof` 型別要對 |
| `free` | `<stdlib.h>` | ~O(1) | — | double free、配對要對稱 |

---

## 7. 本題學到的通用技巧

1. **比較函數要考慮 tie-breaker**：`qsort` 非穩定，所以 `compare_pairs` 主鍵相同時改比次鍵，避免結果隨實作變動。
2. **排序指標而非資料**：`pairs` 存的是指標對，交換成本恆為常數；若直接交換字串內容，成本為 O(k)。
3. **先複製再改動**：需要排序輸入字串時，**永遠**先 `strcpy` 再 `qsort`，保留原始資料。
4. **malloc 多 1 byte 放 `'\0'`**：`strlen + 1` 是字串配置的慣用寫法。
5. **以 canonical form 當鍵**：排序後字串作為異位詞的規範形式（canonical key），是無法使用 hashmap 時的經典替代策略。

---

**文件版本**：1.0（2026-04-19）
**對應檔案**：`Group Anagrams.c`
