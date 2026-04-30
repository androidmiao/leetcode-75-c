# Google Interview — Stack with Min（reference 教材）

> **原始素材**：[`interview/google_stack_with_min/`](../../interview/google_stack_with_min/)
> 8 個檔、1601 行的完整 mock-interview 教材，是 `[[../SKILL.md]]` 中明文指定的 **Interview review style 範例實作**。
>
> **適用情境**：
> - 你想看 SKILL.md 描述的 `original.c` / `solution.c` / `README.md` / `TRACE.md` / `TESTING.md` / `tests.c` / `bug_demo.c` / `Makefile` 八件套長什麼樣子。
> - 你準備一場 Google 風格 onsite「拿一段含 bug 的 C code，30–45 分鐘 review + fix + reason about」。
> - 你要 demo「沒有 MSan / Valgrind 的環境下，如何用 ASan + 投毒記憶體 + property test 把 UB bug 抓出來」。

---

## 為什麼這份教材值得單獨建索引

`[[../SKILL.md]]` 把 Interview review style 列為三種 folder layout 之一，並指明：

> A reference implementation of this layout is at `interview/google_stack_with_min/`.

也就是說——所有未來新增的 `interview/<problem_or_topic>/` 都要對齊這份結構。本檔的職責是把這份 reference 在 wiki 上正式登錄，方便：

1. 想「再寫一個類似教材」時快速對照八件套的長相。
2. 想看 Google L4/L5 風格 review 怎麼分「bug 嚴重度 / 設計缺陷 / hygiene」三層。
3. 想把同一套測試方法論（投毒、property test、Makefile target 矩陣）套用到別的題目。

---

## 題目情境

面試官給一份候選人寫的 C 整數堆疊，要求支援 `push / pop / peek / is_empty / min`。請：

1. 找出原始程式所有 bug。
2. 修正後跑序列 `push(2) → peek → push(8) → peek → push(1) → pop → min`。
3. 進階追問：`min()` 能不能做到 O(1)？

---

## 五個 bug 對照表

| # | bug | 類型 | 為何不容易抓到 |
|---|-----|------|----------------|
| 1 | `pop` 用 `values_[values_count_--]` | 差一 + 未初始化讀取 | ASan 不抓未初始化讀；MSan 才抓得到 |
| 2 | `peek` 用 `values_[values_count_]` | 差一 + 未初始化讀取 | 同上 |
| 3 | `peek` 成功路徑漏寫 `*err = 0` | 邏輯漏寫 | 測試若把 `int err = 0;` 預設會誤判通過 |
| 4 | `pop` 在空棧時 `return *err` (= -1)，與合法 -1 衝突 | API 設計缺陷 | 單純 round-trip 測試抓不到，要 push(-1) 才暴露 |
| 5 | `min()` 空棧回傳未初始化 `v` | UB（未初始化變數） | 每次跑可能值都不同 |

---

## 教材八件套

| 檔 | 行數 | 角色 |
|----|------|------|
| `original.c` | 71 | 候選人原始版本，**逐字保留**含全部 5 個 bug |
| `solution.c` | 125 | 修正版；用 `static` 限制連結範圍；`#ifndef` 包 main 讓 tests.c 能當 library 引用 |
| `README.md` | 301 | bug 分析 + 操作 trace 摘要 + 進階討論（O(1) `min` 用平行最小棧）|
| `TRACE.md` | 312 | 對序列 `push(2)→peek→push(8)→peek→push(1)→pop→min` 的逐步 ASCII state diagram |
| `TESTING.md` | 362 | **四層偵測方法論**：精準 unit test / sanitizer / 編譯器警告 / 靜態分析；含 per-bug 工具偵測矩陣 + 可重用 checklist |
| `tests.c` | 223 | unit + property tests 對 solution.c：投毒 `*err`、隨機序列 vs reference impl |
| `bug_demo.c` | 207 | 對 original.c 的 per-bug demo；`./bug_demo pop` / `peek` / `min` / `err` / `api` 五個入口 |
| `Makefile` | — | targets：`test` / `demo` / `warnings` / `analyze` / `all` / `clean` |

`solution.c` 與 `tests.c` 的銜接技巧：

```c
/* solution.c */
#ifndef GOOGLE_STACK_WITH_MIN_NO_MAIN
int main(void) { ... }
#endif
```

```makefile
# Makefile
tests: tests.c solution.c
	$(CC) -DGOOGLE_STACK_WITH_MIN_NO_MAIN ... -o $@ $^
```

—— 同一份 source 既能獨立執行又能被 link 進 test binary。

---

## 測試方法論（TESTING.md 摘要）

### 第一層：精準 unit test，要點是「投毒法」

```c
#define POISON 0x5A5A5A
int err = POISON;             /* 預先投毒 */
int v = peek(&err);
EXPECT_EQ(err, 0, "peek must set *err = 0 on success");   /* 抓 bug 3 */
```

不投毒、預設 `err = 0` 會讓「漏寫 *err」誤判通過。**強迫被測函式主動寫**，才能抓到「該寫沒寫」的 bug。

### 第二層：Sanitizer

- **ASan** (`-fsanitize=address`) — OOB read/write、UAF、double free。
- **UBSan** (`-fsanitize=undefined`) — signed overflow、shift over width、NULL deref。
- **MSan** (`-fsanitize=memory`) — 未初始化讀取（bug 1 / 2 / 5 的真正剋星）。

但 MSan 在很多沙盒沒有，所以本教材的 `bug_demo.c` 改用「投毒記憶體 + 哨兵值」的軟體模擬，達到類似效果。

### 第三層：編譯器警告

`-Wall -Wextra -Wuninitialized -Wmaybe-uninitialized -Wreturn-type` 在 -O2 編譯下能直接抓到 bug 5 的「return uninitialized variable」。

### 第四層：靜態分析

`gcc -fanalyzer` / `clang --analyze` / `cppcheck` 各有覆蓋。教材列出 per-bug 偵測工具矩陣，方便「拿到一段 code 後，直接知道用哪個工具最可能命中」。

---

## 進階追問：O(1) `min`

題目本身的 `min()` 是 O(n) 線性掃描；面試常見追問是「能不能做 O(1)」。

**標準答案**：用平行的「最小值棧」`min_stack[]`，每次 `push(v)` 時：

```
if (min_stack 為空 || v <= min_stack.top())
    min_stack.push(v);
```

`pop` 時若彈出值等於 `min_stack.top()` 也彈最小棧。`min()` = `min_stack.top()`，O(1)。

這正是 `[[patterns/stack-queue]]` 與 `[[patterns/monotonic-stack]]` 的共同 idiom——LeetCode 155 Min Stack 的解法直接套用。

---

## 與其他 wiki 的交叉

### 與 patterns

- **`[[patterns/stack-queue]]`** — push/pop/peek 基本 stack；O(1) min 是補一個輔助棧。
- **`[[patterns/monotonic-stack]]`** — 「最小值棧」也算 monotonic stack 的退化形。

### 與 c-idioms

- **`[[c-idioms/manual-memory]]`** — `realloc` 擴容策略（×2 vs +10）、初值 `NULL` + size 0 的 idiom。
- **`[[c-idioms/comparator-patterns]]`** — error 用 `*err` out-parameter 而非把 sentinel 塞進回傳值，是「signed/unsigned 函式 return -1」的對偶設計。

### 與 interview 同類教材

- **`[[interview/Mon-3-code-review-solution]]`** / **`[[interview/Tue-3-code-review-solution]]`** / **`[[interview/Thu-3-code-review-solution]]`** / **`[[interview/Fri-4-code-review-solution]]`** — 4 份 weekly mock 解答，配合本教材八件套結構閱讀。
- **`[[interview/code-review-questions]]`** — 主管視角 review checklist；本教材的 5 個 bug 正好示範各條 checklist 怎麼觸發。
- **`[[interview/edge-cases]]`** — UB / 未初始化 / 邊界 / API 設計四大類在本教材都有 1–2 個範例。
- **`[[interview/mcupm-code-review]]`** — driver 級 code review；先做本檔（單檔 stack）熱身，再上 MCUPM 三檔交織。

### 與 LeetCode 題

- **155. Min Stack** — 本題 O(1) `min` 追問的直接對應。
- **232. Implement Queue using Stacks** — 同樣是「實作型」介面，差異在用兩個 stack 實作 queue；測試方法論可直接套。

---

## 學習建議

### 第一遍（45 分鐘 timer）

1. 只讀 `original.c`，不看 README / TESTING。
2. 寫下你看到的所有 bug + 嚴重度分類（NACK / Major / Nit）。
3. 結束後對照 README 的「Bug 分析」校驗。

### 第二遍（測試實作）

1. 讀 `tests.c` + `bug_demo.c` + `Makefile`。
2. `make test`、`make demo`、`make warnings`、`make analyze` 各跑一次，看實際輸出。
3. 注意「投毒法」與「sanitizer 配對」如何分工。

### 第三遍（teaching artifact 化）

挑另一題（建議 232 Implement Queue using Stacks），照八件套結構自己寫一份。重點：

- 一定要先有 `original.c`（包含至少 3 個 bug），不要先寫 solution。
- `TRACE.md` 用 ASCII 而非文字描述。
- `TESTING.md` 一定要包 per-bug 工具偵測矩陣。

---

## Sources

- `interview/google_stack_with_min/README.md`
- `interview/google_stack_with_min/TRACE.md`
- `interview/google_stack_with_min/TESTING.md`
- `interview/google_stack_with_min/solution.c`
- `interview/google_stack_with_min/original.c`
- `interview/google_stack_with_min/tests.c`
- `interview/google_stack_with_min/bug_demo.c`
- `interview/google_stack_with_min/Makefile`
- `SKILL.md`（Interview review style 規範）
