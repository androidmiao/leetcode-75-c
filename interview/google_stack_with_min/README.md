# Google Interview — Stack with Min

## 題目情境

面試官給你一份候選人寫的 C 程式（見 [`original.c`](./original.c)），要求實作一個整數堆疊，支援：

| 操作 | 行為 |
|------|------|
| `push(value)` | 將值推入棧頂 |
| `pop(&err)` | 彈出棧頂並回傳；空棧時透過 `*err` 回報錯誤 |
| `peek(&err)` | 查看棧頂；空棧時透過 `*err` 回報錯誤 |
| `is_empty()` | 回傳棧是否為空 |
| `min()` | 回傳棧中的最小元素 |

請：

1. 找出原始程式中所有的 bug。
2. 修正後跑下列序列，驗證輸出：
   `push(2) → peek → push(8) → peek → push(1) → pop → min`
3. 進階追問：`min()` 能不能做到 O(1)？

---

## 一、Bug 分析

### Bug 1：`pop` 的差一錯誤

```c
return values_[values_count_--];
```

`values_count_--` 是**後置遞減**——先用舊值當索引，再把計數減 1。
但棧頂元素的索引是 `values_count_ - 1`（`push` 用的是 `values_[count++] = v`，
所以寫入後 count 永遠比最後一個有效索引大 1）。

結果：讀到陣列裡尚未寫入的垃圾值，邏輯上的棧高度也只少 1 個位置（其實還是少對了，
只是讀錯了位置）。

**修正：**

```c
return values_[--values_count_];   /* 先減再讀 */
```

### Bug 2：`peek` 的差一錯誤

```c
return values_[values_count_];
```

同樣的問題——應該讀 `values_count_ - 1`。

```c
return values_[values_count_ - 1];
```

### Bug 3：`peek` 成功路徑沒設 `*err`

```c
int peek(int *err) {
  if (is_empty()) {
    *err = -1;
  }
  return values_[values_count_];
}
```

非空時 `*err` 完全沒被寫入，呼叫者看到的是傳入前殘留的舊值——可能是 0，也可能不是。
規範錯誤輸出時，**所有路徑都要寫 `*err`**。

### Bug 4：錯誤碼與回傳值混用

```c
int pop(int *err) {
  if (is_empty()) {
    *err = -1;
    return *err;     /* 回傳 -1 */
  }
  ...
}
```

呼叫者看到 `pop` 回傳 `-1` 時，無法判斷到底是「彈出的值就是 -1」還是「空棧錯誤」。
正確做法：**錯誤一律走 `*err`，回傳值只在 `*err == 0` 時有效**，這樣 -1 也可以是合法資料。

### Bug 5：`min()` 空棧回傳未初始化變數

```c
int min() {
  int first = 1;
  int v;                   /* 未初始化 */
  for (int i = 0; i < values_count_; i++) { ... }
  return v;                /* 空棧時直接回傳垃圾 */
}
```

空棧時迴圈不執行，`v` 從未被賦值就被回傳，是 undefined behavior。
應該加上空棧檢查並透過 `*err` 回報。

### 風格小問題

- `is_empty` 那個 `if (...) return 1; return 0;` 直接寫成 `return values_count_ == 0;` 即可。
- 全域變數加 `static` 限制連結範圍，避免汙染其他 translation unit。

完整修正版見 [`solution.c`](./solution.c)。

---

## 二、操作序列追蹤

下面的圖把 `values_` 畫成 10 格（首次 `make_size` 後配置的容量）。
`·` 表示未定義 / 邏輯上不存在的格子，**粗框**標示棧頂。

### 初始狀態

```
values_       = NULL
values_size_  = 0
values_count_ = 0
```

### ① `push(2)`

`make_size`：`values_size_ (0) <= values_count_ (0)`，配置 10 格。
`values_[values_count_++] = 2`：先寫 `values_[0] = 2`，count 從 0 變 1。

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [ ┃2┃  ·   ·   ·   ·   ·   ·   ·   ·   · ]
values_size_  = 10
values_count_ = 1                  ↑ 棧頂在 idx 0 (= count-1)
```

### ② `peek(&err)`

非空，`*err = 0`，回傳 `values_[1 - 1] = values_[0] = 2`。

```
回傳值: 2     err: 0
```

如果是舊版 `values_[values_count_]`，這裡會讀到 `values_[1]` 的垃圾值。

### ③ `push(8)`

`make_size`：`10 <= 1` 為 false，不擴容。
`values_[values_count_++] = 8`：寫 `values_[1] = 8`，count 從 1 變 2。

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2  ┃8┃  ·   ·   ·   ·   ·   ·   ·   · ]
values_count_ = 2                      ↑ 棧頂 idx 1
```

### ④ `peek(&err)`

非空，`*err = 0`，回傳 `values_[2 - 1] = values_[1] = 8`。

```
回傳值: 8     err: 0
```

### ⑤ `push(1)`

`values_[2] = 1`，count 從 2 變 3。

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2   8  ┃1┃  ·   ·   ·   ·   ·   ·   · ]
values_count_ = 3                          ↑ 棧頂 idx 2
```

### ⑥ `pop(&err)`

非空，執行 `return values_[--values_count_]`：

1. 前置遞減：`values_count_` 從 3 變 2。
2. 讀 `values_[2] = 1`。
3. 回傳 `1`。

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2  ┃8┃ (1)  ·   ·   ·   ·   ·   ·   · ]
                              ↑ idx 2 仍有 1，但已在 count 之外
values_count_ = 2                      ↑ 新棧頂 idx 1

回傳值: 1     err: 0
```

那個 `1` 沒被擦掉，只是邏輯上不存在了——下次 `push` 會直接覆蓋。

### ⑦ `min(&err)`

非空，`*err = 0`，線性掃描：

- `v = values_[0] = 2`
- `i = 1`：`values_[1] = 8`，`8 < 2` 為 false，`v` 不變。
- 迴圈結束（`i < values_count_ = 2`）。

```
回傳值: 2     err: 0
```

注意 `min` 用 `values_count_` 當邊界，**不會**讀到 idx 2 那個 pop 後殘留的 `1`。

### 最終總覽

| 步驟 | 操作       | 回傳 | err | values_count_ | 邏輯內容（底→頂） |
|----:|-----------|----:|---:|--------------:|------------------|
| ①   | push(2)   |   – |  – | 1             | [2]              |
| ②   | peek      |   2 |  0 | 1             | [2]              |
| ③   | push(8)   |   – |  – | 2             | [2, 8]           |
| ④   | peek      |   8 |  0 | 2             | [2, 8]           |
| ⑤   | push(1)   |   – |  – | 3             | [2, 8, 1]        |
| ⑥   | pop       |   1 |  0 | 2             | [2, 8]           |
| ⑦   | min       |   2 |  0 | 2             | [2, 8]           |

---

## 三、進階追問：`min()` 做到 O(1)

目前 `min()` 是 O(n)。經典面試技巧：再維護一個**平行的最小值棧** `min_stack_`，
讓 `min_stack_[i]` 永遠等於「主棧高度為 i+1 時的全域最小值」。

### push(v) 時

```
若主棧為空:
    min_stack_[0] = v
否則:
    min_stack_[count] = (v < min_stack_[count-1]) ? v : min_stack_[count-1]
```

### pop / min

```
pop:  count--（兩條棧同步）
min:  return min_stack_[count - 1]   // O(1)
```

### 為什麼正確？

棧是 LIFO——元素 v 在棧裡的期間，它下方的所有元素都不會改變。
所以「v 還在棧頂時的全域最小值」可以在 push 時就算好、永遠正確，pop 時不必修。

這個解法對應 LeetCode 155 (Min Stack)，倉庫裡 [`../../155_Min Stack/Min Stack.c`](../../155_Min%20Stack/Min%20Stack.c)
有完整實作可以參考。

### 取捨

- **時間：** `min` 從 O(n) 降到 O(1)。
- **空間：** 多用 O(n) 的輔助棧（最壞情況下與主棧一樣大）。
- 若 `min` 呼叫頻率低，O(n) 線性掃描可能反而省記憶體；面試中通常假設要優化 `min`。

---

## 四、編譯與執行

直接跑 demo：

```bash
cc -std=c11 -Wall -Wextra solution.c -o stack_demo
./stack_demo
```

預期輸出：

```
push(2)
peek -> 2 (err=0)
push(8)
peek -> 8 (err=0)
push(1)
pop  -> 1 (err=0)
min  -> 2 (err=0)
```

或用 Makefile 跑完整驗證流程：

```bash
make test       # 對 solution.c 跑測試（16 cases，配合 ASan + UBSan）
make demo       # 對 original.c 示範每個 bug
make warnings   # 編譯器警告對照
make analyze    # gcc -fanalyzer 靜態分析
make all        # 全部跑一遍
```

完整方法論見 [`TESTING.md`](./TESTING.md)，逐步追蹤見 [`TRACE.md`](./TRACE.md)。

---

## 五、面試官會看的點

| 觀察項目              | 重點 |
|----------------------|------|
| 是否能定位差一錯誤       | `pop` / `peek` 的索引、前置 vs 後置遞減 |
| 錯誤處理 API 設計       | 不要把錯誤碼塞進回傳值；所有路徑都要寫 `*err` |
| 邊界條件               | 空棧 `pop` / `peek` / `min`、`min` 未初始化變數 |
| 記憶體管理              | `realloc` 失敗檢查（這裡用 `assert` 是 OK 的，但生產環境應回報） |
| 進階優化能否想到 O(1) min | 平行棧、為何在 LIFO 下成立 |
| 程式碼風格              | 全域變數 `static`、簡潔的 `is_empty` |
