# 如何有效抓出這類錯誤——測試方法論

以這題的五個 bug 為例，演示一套「**測試 + Sanitizer + 編譯器警告 + 靜態分析**」
四層夾擊的 C 程式驗證流程。本文件附帶可重複執行的工具
（[`tests.c`](./tests.c) / [`bug_demo.c`](./bug_demo.c) / [`Makefile`](./Makefile)），
所有「實際輸出」段落都是真的跑出來的結果。

---

## 一、回顧：這題的五個 bug

| #  | Bug 描述                                       | 類型             |
|----|-----------------------------------------------|------------------|
| 1  | `pop` 用 `values_[values_count_--]`，讀錯位置  | 差一錯誤 + 未初始化讀取 |
| 2  | `peek` 用 `values_[values_count_]`，讀錯位置   | 差一錯誤 + 未初始化讀取 |
| 3  | `peek` 成功路徑漏寫 `*err = 0`                | 邏輯漏寫         |
| 4  | `pop` 在空棧時 `return *err` (= -1)，與合法 -1 衝突 | API 設計缺陷 |
| 5  | `min()` 在空棧時回傳未初始化的 `v`             | UB（未初始化變數）|

---

## 二、四層偵測手段

### 第一層：精準的單元測試

寫測試的關鍵不是「有測試」，是**測試本身要夠精準才抓得到 bug**。
這題在 [`tests.c`](./tests.c) 我刻意設計的幾個重點：

```c
#define POISON 0x5A5A5A   /* 把 *err 預先投毒 */

static void test_push_then_peek(void) {
    int err = POISON;     /* 故意給垃圾值 */
    push(42);
    int v = peek(&err);
    EXPECT_EQ(v, 42, "peek value");
    EXPECT_EQ(err, 0, "peek must set *err = 0 on success");  /* Bug 3 */
}
```

**為什麼把 `err` 預先設成 POISON？**
如果寫成 `int err = 0;`，原版 `peek` 即使漏寫 `*err`，讀出來的還是 0，
測試會誤判通過。投毒法強迫被測函式「主動寫」，否則就會被抓到。

```c
static void test_push_minus_one_pop_returns_minus_one(void) {
    /* 原版把 -1 當錯誤碼，導致 push(-1) 後 pop 的值與「空棧錯誤」無法區分。 */
    push(-1);
    int err = POISON;
    int v = pop(&err);
    EXPECT_EQ(v, -1, "popped -1 must come back as -1");      /* Bug 4 */
    EXPECT_EQ(err, 0, "popping a real value sets *err = 0");
}
```

這個案例**單獨**就把 bug 4 的 API 設計問題打出來——靠把可能的「錯誤哨兵」當成
合法資料推進去看會發生什麼事。

```c
static void test_random_property(void) {
    /* 對拍：隨機 push/pop/peek/min 5000 次，與簡易 reference 比對 */
    int ref[2000]; int ref_top = 0; ...
    srand(42);
    for (int op = 0; op < 5000; op++) {
        int choice = (ref_top == 0) ? 0 : (rand() % 4);
        if (choice == 0) { push(v); ref[ref_top++] = v; }
        else if (choice == 1) {
            int v = pop(&err);
            EXPECT_EQ(v, ref[--ref_top], "rand pop value");
        }
        ...
    }
}
```

**Property-based testing**（屬性測試）補足人腦想不到的邊界——隨機操作配合
參考實作對拍，跑幾千次只要有一次不一致就抓到。

#### 測試的精準度檢核表

| 是否做到                                                   |
|-----------------------------------------------------------|
| ☑ 每個 `EXPECT_EQ` 比對精確值（不只判斷非零 / 非 NULL）        |
| ☑ Out-parameter (`*err`) 在呼叫前投毒，強迫被測函式主動寫入   |
| ☑ 邊界：空容器 / 容量上限 / 重複元素 / 推 N 個彈 N 個         |
| ☑ 把可能的錯誤哨兵當成合法資料推進去（如 -1, INT_MIN, 0）      |
| ☑ 隨機序列 + 參考實作對拍                                    |
| ☑ 每個測試前 reset 狀態，避免測試間互相污染                  |

#### 實際輸出（對 `solution.c` 跑 `make test`）

```
=== Run tests against solution.c (ASan + UBSan) ===
Running tests...

  test_initial_is_empty                        PASS
  test_pop_empty_sets_err                      PASS
  test_peek_empty_sets_err                     PASS
  test_min_empty_sets_err                      PASS
  test_push_then_peek                          PASS
  test_push_then_pop                           PASS
  test_lifo_order                              PASS
  test_push_minus_one_pop_returns_minus_one    PASS
  test_min_basic                               PASS
  test_min_after_pop_top                       PASS
  test_min_with_duplicates                     PASS
  test_min_single_element                      PASS
  test_resize_beyond_initial                   PASS
  test_alternating_push_pop                    PASS
  test_peek_does_not_pop                       PASS
  test_random_property                         PASS

16 passed, 0 failed
```

---

### 第二層：Sanitizer

```bash
gcc -fsanitize=address,undefined -fno-omit-frame-pointer -g ...
```

| Sanitizer                       | 抓什麼                              | 對這題的效果         |
|---------------------------------|-------------------------------------|--------------------|
| **AddressSanitizer (ASan)**     | OOB 讀寫、UAF、heap overflow         | 不會 fire（off-by-one 仍在 realloc 配置範圍內）|
| **UndefinedBehaviorSanitizer**  | 整數溢位、NULL deref、shift 過大     | 不會 fire           |
| **MemorySanitizer (MSan)** ⭐    | 未初始化記憶體讀取                   | **直接抓 bug 1, 2, 5**（需 clang）|
| **Valgrind Memcheck**           | 同 MSan + heap 錯誤                | **直接抓 bug 1, 2, 5**|

#### 為什麼 ASan 抓不到差一錯誤？

原版 `pop` 讀 `values_[values_count_]` = `values_[1]`。`make_size()` 用
`realloc(NULL, 10 * sizeof(int))` 配置了 10 格，所以 `values_[1]` 是 **配置範圍內**
的位置——只是「邏輯上」沒寫過。

ASan 監控的是「這段記憶體有沒有被 malloc 給你」，不管「你有沒有寫過再讀」。
要抓「未初始化讀取」必須用 MSan 或 Valgrind。

#### 即使在沒有 MSan 的環境，sanitizer 仍有價值

ASan + UBSan 能抓的東西非常多——這次測試組跑了 5000 次隨機 push/pop/peek/min，
ASan 就在背景監控所有 realloc / 越界。**測試本身配合 sanitizer**，等於同一份
測試多得到一層保護。

如果這題裡有「彈出後忘記減 count，下次 push 寫到陣列外」之類的 bug，ASan
就會立刻 fire 而不是等到測試結果不對才發現。

#### 實際命令

```bash
# 開發日常
gcc -fsanitize=address,undefined -g tests.c solution.c -o tests

# 特別查未初始化（需 clang + Linux）
clang -fsanitize=memory -fno-omit-frame-pointer -g tests.c solution.c -o tests

# 沒裝 sanitizer 也能跑
valgrind --error-exitcode=1 --track-origins=yes ./tests
```

---

### 第三層：編譯器警告

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wuninitialized -O2 -c original.c
```

#### 實際輸出（`make warnings`）

```
=== Warnings on original.c (-Wall -Wextra -Wuninitialized -O2) ===
original.c: In function ‘min’:
original.c:70:10: warning: ‘v’ may be used uninitialized in this function [-Wmaybe-uninitialized]
   70 |   return v;
      |          ^

=== Warnings on solution.c (should be clean) ===
(no output above means no warnings)
```

**直接抓到 bug 5**，而且不需要任何測試或執行——光編譯就會告訴你。

#### 重點：`-O2` 是必要的

GCC 的 `-Wmaybe-uninitialized` 依賴資料流分析，這需要優化等級才會跑。
`-O0`（debug build）下這類警告經常不會出現。建議至少 `-O1`，CI 中跑
`-O2` 配合 `-Werror` 直接擋下 commit。

#### 把警告當錯誤

```bash
gcc -Werror -Wall -Wextra -Wuninitialized -O2 ...
```

`-Werror` 把所有警告升級為編譯錯誤——bug 5 那種等級的問題直接擋在
build 階段，根本進不到測試。

---

### 第四層：靜態分析

```bash
gcc -fanalyzer -c original.c
```

GCC 10+ 內建路徑感知靜態分析。在這題上實測：

```
=== gcc -fanalyzer on original.c ===
(no output)
```

對這個案例 `-fanalyzer` 沒抓到額外問題（因為 bug 5 已經被 `-Wuninitialized` 抓走，
其他 bug 屬於語意 / API 設計問題，不在 `-fanalyzer` 的覆蓋範圍）。

但靜態分析在更複雜的場景會贏：
- NULL 指標解參考（沿著條件分支追蹤）
- 雙重 free / 用後 free
- 資源洩漏（fopen 沒 fclose）
- 邏輯錯誤（條件永遠為真 / 為假）

#### 其他可加上的工具

| 工具          | 抓的東西                                |
|---------------|----------------------------------------|
| `clang --analyze` | 跟 gcc -fanalyzer 類似，覆蓋面略不同 |
| `cppcheck`    | 樣式 + 邏輯，誤報率較低                  |
| `scan-build`  | clang 的包裝，產 HTML 報告               |
| `splint`      | 老牌但仍堪用                             |

CI pipeline 推薦至少跑一個。

---

## 三、五個 bug × 偵測方式 對照表

`◎` = 容易抓到　`◯` = 有機會　`△` = 看運氣　`–` = 抓不到

| Bug                          | 精準測試 | ASan | MSan/Valgrind | UBSan | 編譯警告 | 靜態分析 |
|------------------------------|:------:|:----:|:------------:|:-----:|:--------:|:------:|
| 1 `pop` 差一                  |   ◎    |  –   |     ◎        |  –    |    –     |   △    |
| 2 `peek` 差一                 |   ◎    |  –   |     ◎        |  –    |    –     |   △    |
| 3 `peek` 漏寫 `*err`          |   ◎ †  |  –   |     ◯        |  –    |    –     |   △    |
| 4 `-1` 當錯誤碼（API）        |   ◎ ‡  |  –   |     –        |  –    |    –     |   –    |
| 5 `min` 空棧未初始化          |   ◎    |  –   |     ◎        |  –    |    ◎     |   ◎    |

> † 需要 `*err` 投毒；普通 `int err = 0;` 會漏抓。
> ‡ 需要 push 一個會撞到錯誤哨兵的合法值（如 -1）。

**結論：**
- **記憶體類 bug**（1、2、5）→ MSan / Valgrind 是大殺器；沒有的話靠精準測試 + 投毒。
- **API / 邏輯類 bug**（3、4）→ 工具難抓，**只有寫得夠嚴格的測試 + code review** 能找到。
- **未初始化變數**（5）→ 編譯器警告就抓住，幾乎免費。

---

## 四、實際示範：Bug demo 對 `original.c`

[`bug_demo.c`](./bug_demo.c) 對原版每個 bug 都做了個別示範，
用「投毒 + 比對哨兵值」讓 bug 在沒有 MSan 的環境下也能清楚可見。

執行 `make demo` 的實際輸出：

```
========== Bug 1: pop off-by-one ==========
expected: 2 (err=0)
actual:   -889275714 (err=0)  [hex: 0xCAFEBABE]
>>> BUG CONFIRMED: pop read uninitialized slot values_[count]

========== Bug 2: peek off-by-one ==========
expected: 2 (err=0)
actual:   -889275714 (err=0)  [hex: 0xCAFEBABE]
>>> BUG CONFIRMED: peek read uninitialized slot values_[count]

========== Bug 3: peek doesn't write *err on success ==========
expected: peek returns 2, *err = 0
actual:   v=-1094795586, err=0x5A5A5A
>>> BUG CONFIRMED: peek left *err untouched on success

========== Bug 4: -1 is ambiguous (error code vs legit value) ==========
Scenario A: pop on empty stack
  return = -1, err = -1   <-- err=-1 means error
Scenario B: push(-1) then pop
  return = -889275714, err = 0   <-- err=0 means success
>>> Lesson: don't conflate error code with payload.

========== Bug 5: min returns uninitialized on empty stack ==========
min() called 3 times on empty stack:
  call 1: 1885952960
  call 2: 1885952960
  call 3: 1885952960
>>> BUG CONFIRMED at compile time: see -Wuninitialized warning
```

每個 demo 的「投毒手法」見 `bug_demo.c` 內註解。

---

## 五、可重用的 checklist（套用到其他 LeetCode 題）

每寫完一題 C 都跑一次：

```bash
# 1. 編譯時擋下基本錯誤
gcc -std=c11 -Wall -Wextra -Wpedantic -Wuninitialized -Werror -O2 -c solution.c

# 2. 跑測試（有的話）配合 sanitizer
gcc -std=c11 -Wall -Wextra -fsanitize=address,undefined -g \
    tests.c solution.c -o tests && ./tests

# 3. 靜態分析（任選一個）
gcc -fanalyzer -c solution.c
# 或：clang --analyze solution.c
# 或：cppcheck --enable=all solution.c
```

#### 寫測試時自我檢查

| 提問                                                     | 沒做到 → bug 容易溜走 |
|---------------------------------------------------------|---------------------|
| 我有沒有比對「精確值」，還是只判斷「非零」？               | 投毒、UB 都會漏 |
| Out-parameter 我有沒有預先投毒？                          | 漏寫 `*err` 會漏 |
| 邊界（空、滿、單一元素、重複、負數、INT_MIN / INT_MAX）有沒有測？ | 邊界 bug 會漏 |
| 我有沒有把「可能的錯誤哨兵」當合法資料測？(`-1`、`0`、`NULL`) | API 設計 bug 會漏 |
| 我有沒有寫一個隨機序列對拍？                              | 沒想到的邊界會漏 |
| 我有沒有 reset 狀態？                                    | 測試間互相污染 |

---

## 六、回到原問題：「寫測試是不是抓 bug 最好的方式？」

**是，但只能是**起點。光寫測試會漏掉：
- 「測試剛好沒撞到 bug」的情況（例如垃圾值剛好是預期值）。
- API 設計類問題，需要設計**對抗性**的測試案例才看得出來。
- 記憶體類問題，需要 MSan / Valgrind 配合才會顯形。

最划算的組合是：

> **編譯警告擋住免費的 bug → 嚴格測試覆蓋邏輯 → Sanitizer 在背景巡邏記憶體 → 靜態分析查路徑邏輯**

這四層的成本都不高（除了寫測試之外，其他都只是改幾個 build flag），但每層
都有獨佔的覆蓋面，缺一個就會有一類 bug 從縫裡溜走。

---

## 七、如何重現本文件所有結果

```bash
cd interview/google_stack_with_min
make all
```

會依序跑：

1. `make test` — tests.c 對 solution.c，全部 16 個測試通過。
2. `make demo` — bug_demo.c 對 original.c，五個 bug 個別示範。
3. `make warnings` — gcc 警告對照（original 有警告、solution 乾淨）。
4. `make analyze` — gcc -fanalyzer 靜態分析。

`make clean` 清掉 `build/`。
