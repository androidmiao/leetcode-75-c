# 操作序列追蹤：`push(2) → peek → push(8) → peek → push(1) → pop → min`

本文件針對下列序列，逐步追蹤修正版 [`solution.c`](./solution.c) 的內部狀態變化。

> 操作序列：
> `push(2)` → `peek()` → `push(8)` → `peek()` → `push(1)` → `pop()` → `min()`

---

## 圖示約定

- `values_` 畫成 10 格陣列（首次 `make_size` 呼叫後配置的容量）。
- `·` 表示**未定義 / 邏輯上不存在**的格子（記憶體上可能有值，但已超出 `values_count_`）。
- **粗框 `┃ ┃`** 標示目前的棧頂元素。
- 索引 `idx` 由左到右是 `0..9`，即「棧底 → 棧頂」的方向。

---

## 初始狀態

程式啟動時三個全域變數的初值：

```
values_       = NULL
values_size_  = 0
values_count_ = 0
```

此時棧為空，`is_empty()` 回傳 1。任何 `pop` / `peek` / `min` 都會把 `*err` 設成 -1。

---

## ① `push(2)`

### 程式碼路徑

```c
void push(int value) {
    make_size();
    values_[values_count_++] = value;
}
```

### 內部動作

1. 進入 `make_size()`：判斷 `values_size_ (0) <= values_count_ (0)` 為 true。
2. `values_size_ += 10` → `values_size_` 從 0 變 **10**。
3. `realloc(NULL, 10 * sizeof(int))`：等同 `malloc`，配置 10 個 int 的空間。
4. `assert(values_ != NULL)` 通過。
5. 回到 `push`，執行 `values_[values_count_++] = 2`：
   - **後置遞增**——先以 `values_count_` 的舊值 `0` 當索引，寫入 `values_[0] = 2`。
   - 接著 `values_count_` 從 0 變 **1**。

### 狀態快照

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [ ┃2┃  ·   ·   ·   ·   ·   ·   ·   ·   · ]
values_size_  = 10
values_count_ = 1                  ↑ 棧頂在 idx 0 (= count - 1)
```

### 重點

`push` 用後置遞增是**對的**——先寫到舊的 count 位置（恰好是新的棧頂），再讓 count 反映「目前堆疊裡有幾個元素」。
這跟後面 `pop` 的差一錯誤對比鮮明：那邊用後置遞減就會出問題。

---

## ② `peek(&err)`

### 程式碼路徑

```c
int peek(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    return values_[values_count_ - 1];
}
```

### 內部動作

1. `is_empty()`：`values_count_ == 0` 為 false（目前 count = 1），回傳 0。
2. 跳到成功路徑，`*err = 0`。
3. 計算 `values_count_ - 1` = `1 - 1` = `0`。
4. 讀 `values_[0]` = `2`，回傳。

### 結果

```
回傳值: 2
err:    0
```

### 與舊版對照

舊版寫成 `return values_[values_count_]`——這裡會讀 `values_[1]`，而 `values_[1]` 從未被寫入過，
是 `realloc` 配置出來的**未初始化**記憶體。回傳值會是任意垃圾，行為不可預測。

陣列內容沒變：

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [ ┃2┃  ·   ·   ·   ·   ·   ·   ·   ·   · ]
values_count_ = 1
```

---

## ③ `push(8)`

### 內部動作

1. `make_size()`：`values_size_ (10) <= values_count_ (1)` 為 false，**不擴容**。
2. `values_[values_count_++] = 8`：
   - 寫 `values_[1] = 8`。
   - `values_count_` 從 1 變 **2**。

### 狀態快照

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2  ┃8┃  ·   ·   ·   ·   ·   ·   ·   · ]
values_count_ = 2                      ↑ 棧頂 idx 1
```

---

## ④ `peek(&err)`

### 內部動作

1. `is_empty()` 回傳 0（count = 2）。
2. `*err = 0`。
3. `values_[2 - 1]` = `values_[1]` = `8`，回傳。

### 結果

```
回傳值: 8
err:    0
```

陣列內容不變。

---

## ⑤ `push(1)`

### 內部動作

1. `make_size()`：`10 <= 2` 為 false，不擴容。
2. `values_[2] = 1`，`values_count_` 從 2 變 **3**。

### 狀態快照

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2   8  ┃1┃  ·   ·   ·   ·   ·   ·   · ]
values_count_ = 3                          ↑ 棧頂 idx 2
```

---

## ⑥ `pop(&err)` ★ 關鍵步驟

### 程式碼路徑

```c
int pop(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    return values_[--values_count_];
}
```

### 內部動作

1. `is_empty()` 回傳 0（count = 3）。
2. `*err = 0`。
3. 計算 `values_[--values_count_]`——**前置遞減**，求值順序：
   1. `values_count_` 先從 3 變 **2**。
   2. 用新值 2 當索引，讀 `values_[2]` = `1`。
   3. 回傳 `1`。

### 狀態快照

```
            idx:  0   1   2   3   4   5   6   7   8   9
values_     = [  2  ┃8┃ (1)  ·   ·   ·   ·   ·   ·   · ]
                              ↑ idx 2 仍有 1，但已在 count 之外
values_count_ = 2                      ↑ 新棧頂 idx 1

回傳值: 1
err:    0
```

### 重點：為什麼必須是前置遞減

對比兩種寫法：

| 寫法                              | count 變化 | 讀取索引 | 行為 |
|----------------------------------|-----------|----------|------|
| `values_[--values_count_]` ✅    | 3 → 2     | 2        | 讀到棧頂的 1，count 正確少 1 |
| `values_[values_count_--]` ❌    | 3 → 2     | 3        | 讀到 idx 3 的垃圾值（從未寫入），count 雖然也少 1 但讀錯位置 |

舊版的雙重災難：呼叫者拿到亂數，邏輯上以為棧只剩 `[2, 8]`（這部份其實是對的），但回傳值卻是無意義的垃圾。

### 殘留資料的觀察

`values_[2]` 仍然儲存 `1`——**沒有被擦掉**，只是邏輯上不存在了。下次 `push` 會直接覆寫該格。
這是設計選擇：每次 pop 都歸零會浪費效能；下游若有安全考量（例如儲存敏感資料），才需要主動清除。

---

## ⑦ `min(&err)`

### 程式碼路徑

```c
int min(int* err) {
    if (is_empty()) {
        *err = -1;
        return 0;
    }
    *err = 0;
    int v = values_[0];
    for (int i = 1; i < values_count_; i++) {
        if (values_[i] < v) v = values_[i];
    }
    return v;
}
```

### 內部動作

1. `is_empty()` 回傳 0（count = 2）。
2. `*err = 0`。
3. 初始化 `v = values_[0]` = `2`。
4. 進入迴圈：
   - `i = 1`：`values_[1]` = `8`，判斷 `8 < 2` 為 false，`v` 維持 `2`。
   - `i = 2`：條件 `i < values_count_` 即 `2 < 2` 為 false，**跳出迴圈**。
5. 回傳 `v` = `2`。

### 結果

```
回傳值: 2
err:    0
```

### 重點：`min` 不會看到 pop 後殘留的 1

雖然 `values_[2]` 還儲存著 `1`，但迴圈邊界是 `i < values_count_`（= 2），**不會**讀到 idx 2。
所有正確使用 `values_count_` 當邊界的函式都自動忽略「邏輯外」的記憶體。

---

## 最終總覽

| 步驟 | 操作       | 回傳 | err | values_count_ | 邏輯內容（底→頂） |
|----:|-----------|----:|---:|--------------:|------------------|
| ①   | push(2)   |   – |  – | 1             | [2]              |
| ②   | peek      |   2 |  0 | 1             | [2]              |
| ③   | push(8)   |   – |  – | 2             | [2, 8]           |
| ④   | peek      |   8 |  0 | 2             | [2, 8]           |
| ⑤   | push(1)   |   – |  – | 3             | [2, 8, 1]        |
| ⑥   | pop       |   1 |  0 | 2             | [2, 8]           |
| ⑦   | min       |   2 |  0 | 2             | [2, 8]           |

整個流程 `*err` 從未變成 -1，因為每次讀取操作前棧都不是空的。
若在 ① 之前就呼叫 `peek` / `pop` / `min`，就會看到 `*err = -1`、回傳值該被忽略的情況。

---

## 用 driver 驗證

[`solution.c`](./solution.c) 的 `main` 就是這個序列。編譯執行：

```bash
cc -std=c11 -Wall -Wextra solution.c -o stack_demo
./stack_demo
```

實際輸出（與本文件追蹤結果一致）：

```
push(2)
peek -> 2 (err=0)
push(8)
peek -> 8 (err=0)
push(1)
pop  -> 1 (err=0)
min  -> 2 (err=0)
```

---

## 學到的核心觀念

1. **後置 vs 前置 ++/--**：在「count 同時當索引又當大小」的容器設計裡，`push` 用後置遞增、`pop` 用前置遞減才會對齊到正確位置。
2. **`*err` 必須在所有路徑寫入**：成功路徑漏寫會讓呼叫者讀到舊值。
3. **錯誤碼別塞回傳值**：用 out-parameter 區分「這次呼叫成功了嗎」與「資料是什麼」，回傳值就能涵蓋整個 int 範圍。
4. **`values_count_` 是邏輯邊界，不是物理邊界**：陣列實體上可能還有殘留資料，但只要所有讀寫都以 `values_count_` 為界，就不會洩漏。
5. **未初始化變數的回傳是 UB**：原版 `min` 在空棧時就踩到這個地雷。
