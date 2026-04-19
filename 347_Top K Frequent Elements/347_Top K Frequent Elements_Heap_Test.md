# 347. Top K Frequent Elements — Heap 版本測試說明

This document describes how `347_Top K Frequent Elements_Heap.c` was verified locally before being considered ready for LeetCode submission.

本文件說明 `347_Top K Frequent Elements_Heap.c` 在提交到 LeetCode 之前，於本機使用哪些方式驗證正確性。

---

## English Version

### Goal of local testing

LeetCode's online judge is the final arbiter, but depending on it alone is a slow feedback loop. Before submitting, we want to catch:

- Compile errors and warnings (uninitialized variables, sign mismatch, unused parameters)
- Off-by-one bugs in the heap indexing or `returnSize` write-back
- Hash-collision bugs when negative numbers or boundary values (`±10000`) are used
- Memory-lifetime bugs (double free, use-after-free) visible only under sanitizers

A small C test harness covers all of these cheaply.

### Test harness design

The test driver file `test_347_heap.c` is kept **outside** the repository folder (in the scratch directory) so it never pollutes the repo. It pulls the solution in with a single `#include`:

```c
#include "/…/347 Top K Frequent Elements/347 Top K Frequent Elements_Heap.c"
```

Why `#include` a `.c` file instead of linking?

- The solution file has `static` helpers (`siftUp`, `siftDown`, `hashLookupOrInsert`). Linking would require exposing them via a header, which would force the repo's standalone-style `.c` to be split into `.c + .h`. That violates SKILL.md's "standalone-first" rule.
- A single-translation-unit include keeps the test completely self-contained.
- The final submission to LeetCode is the unmodified `.c` file; the test file is never shipped.

### Comparison helper: set equality, not sequence equality

LeetCode explicitly says *the answer may be returned in any order*. So the harness uses a set-comparison helper:

1. Copy `got[]` and `want[]` into scratch buffers.
2. `qsort` both buffers.
3. Compare element-by-element.

If the sizes differ the test fails immediately with a clear message; otherwise both sorted buffers must match.

### Compiler flags

```
gcc -std=c11 -Wall -Wextra -O2 test_347_heap.c -o test_347_heap
```

- `-std=c11` — matches LeetCode's C compiler standard.
- `-Wall -Wextra` — promotes the common classes of warnings that LeetCode judges silently tolerate but that usually indicate real bugs (unused variables, sign-compare, missing field initializers).
- `-O2` — forces the optimizer to exercise the code harder; this occasionally exposes strict-aliasing or undefined-behavior problems that `-O0` hides.

A clean build at these flags is a much stronger signal than "LeetCode Accepted".

### Test cases

Six scenarios are run. Each targets a distinct failure mode.

| # | Name | Input | k | Expected | Why it exists |
|---|------|-------|---|----------|---------------|
| 1 | Example 1 | `[1,1,1,2,2,3]` | 2 | `{1,2}` | LeetCode's first official sample — basic sanity. |
| 2 | Example 2 | `[1]` | 1 | `{1}` | Smallest legal input. Confirms `numsSize == 1`, `k == 1` edge. |
| 3 | Negatives + dups | `[-1,-1,2,2,2,3,3,3,3]` | 2 | `{2,3}` | Ensures the hash map treats negative keys correctly (Knuth multiplicative hash of a negative `int`). |
| 4 | k == #unique | `[4,4,5,6]` | 3 | `{4,5,6}` | All distinct values must end up in the heap; checks the "heap never rejects" path. |
| 5 | All identical | `[7,7,7,7]` | 1 | `{7}` | Only one unique key; checks that we don't accidentally double-insert or under-insert. |
| 6 | Boundary values | `[-10000,-10000,10000,10000,10000,0]` | 2 | `{-10000,10000}` | The problem's declared value range (`nums[i] ∈ [-10^4, 10^4]`). Confirms hashing/indexing works at the extremes and that a mid-frequency value (`10000` appearing 3 times) beats `-10000` (appearing 2 times), but both ultimately land in the top-2 because `0` only appears once. |

### Expected output

```
[PASS] Example 1 -> got {2,1}, want {1,2}
[PASS] Example 2 -> got {1}, want {1}
[PASS] Negatives + dups -> got {2,3}, want {2,3}
[PASS] k == #unique -> got {6,5,4}, want {4,5,6}
[PASS] All identical -> got {7}, want {7}
[PASS] Boundary values -> got {-10000,10000}, want {-10000,10000}

ALL TESTS PASSED
```

Note how "got" and "want" are printed in their raw order. The order differs between them in several cases (e.g. test 4 prints `{6,5,4}` vs. `{4,5,6}`), which is expected — correctness is defined by *set equality*.

### What the tests do NOT cover (future work)

The harness is intentionally lightweight and does not attempt:

- **AddressSanitizer / LeakSanitizer** — a single `-fsanitize=address,undefined` run would catch any malloc/free mismatches and UB. Worth adding if the solution is changed.
- **Stress test with random input** — the current 6 cases are hand-picked; a fuzz loop comparing against a brute-force `O(U log U)` reference would give much stronger coverage.
- **Timing under worst case** — a test with `numsSize == 10^5` and many distinct values to confirm O(n log k) behaviour empirically.

These are deliberately out of scope for the initial verification, but are the natural next steps if the file is refactored.

### Clean-up

The test driver and its binary are deleted after the run. The repo folder is left containing only the solution `.c` and the maintained markdown notes; nothing test-specific is committed.

---

## 中文翻譯

### 本機測試的目的

LeetCode 線上判題是最後的裁判，但只靠它的回饋循環太慢。在送出之前，我們希望先在本機抓到：

- 編譯錯誤與警告（未初始化變數、sign mismatch、未使用參數）
- heap 索引或 `returnSize` 寫回值的 off-by-one
- 負數或邊界值（`±10000`）造成的 hash 碰撞問題
- 只有 sanitizer 才看得到的記憶體生命週期問題（double free、use-after-free）

一個小型的 C 測試 harness 就能以很低的成本涵蓋以上情境。

### 測試 harness 的設計

測試檔 `test_347_heap.c` 特意放在 **repo 資料夾之外**（暫存目錄），才不會污染 repo。它只用一個 `#include` 把解法拉進來：

```c
#include "/…/347 Top K Frequent Elements/347 Top K Frequent Elements_Heap.c"
```

為什麼要 `#include` 一個 `.c` 檔而不是正常的 link？

- 解法檔裡有很多 `static` 的輔助函式（`siftUp`、`siftDown`、`hashLookupOrInsert`）。如果要 link，就必須把它們放進 header 才能被測試看到，這樣就會把 repo 裡的 standalone-style `.c` 拆成 `.c + .h`，違反 SKILL.md 的「standalone-first」慣例。
- 單一 translation unit 的 include 讓測試完全自包含。
- 真正送上 LeetCode 的就是那個沒有被動過的 `.c`；測試檔從不會出現在 repo 裡。

### 比對工具：集合相等，不是序列相等

LeetCode 明確說明 *答案可以以任意順序回傳*。所以 harness 用的是集合比較工具：

1. 把 `got[]` 與 `want[]` 各自複製到暫存陣列。
2. 用 `qsort` 分別排序。
3. 一格一格比對。

若大小就不同則立即失敗並印出清楚訊息；若大小相同，排序後兩個陣列必須完全一致。

### 編譯器選項

```
gcc -std=c11 -Wall -Wextra -O2 test_347_heap.c -o test_347_heap
```

- `-std=c11` — 對齊 LeetCode 的 C 編譯器標準。
- `-Wall -Wextra` — 打開常見的警告類別，這些 LeetCode 判題通常會默默忽略，但幾乎都是真正的 bug（unused 變數、sign-compare、missing field initializer）。
- `-O2` — 強迫優化器更認真地處理程式碼；偶爾能觸發 `-O0` 下看不見的 strict-aliasing 或 undefined behavior 問題。

在這組選項下乾淨編譯過，是比「LeetCode Accepted」更強的信號。

### 測試案例

共跑 6 組情境，每一組針對一個獨立的失敗模式。

| # | 名稱 | 輸入 | k | 期望 | 存在理由 |
|---|------|------|---|------|----------|
| 1 | Example 1 | `[1,1,1,2,2,3]` | 2 | `{1,2}` | LeetCode 官方第一個範例——基本健全性。 |
| 2 | Example 2 | `[1]` | 1 | `{1}` | 最小合法輸入，確認 `numsSize == 1`、`k == 1` 的邊界。 |
| 3 | 負數 + 重複 | `[-1,-1,2,2,2,3,3,3,3]` | 2 | `{2,3}` | 確保 hash map 正確處理負 key（對負 `int` 做 Knuth multiplicative hash）。 |
| 4 | k == 不同元素個數 | `[4,4,5,6]` | 3 | `{4,5,6}` | 所有不同值都必須留在 heap 裡；測試「heap 永不拒絕」路徑。 |
| 5 | 全部相同 | `[7,7,7,7]` | 1 | `{7}` | 只有一個 unique key；確認我們不會誤插入兩次或少插入。 |
| 6 | 邊界值 | `[-10000,-10000,10000,10000,10000,0]` | 2 | `{-10000,10000}` | 題目宣告值域（`nums[i] ∈ [-10^4, 10^4]`）。驗證在兩端極值下 hashing / 索引都正常，且中等頻率的 `10000`（出現 3 次）擊敗 `-10000`（出現 2 次），但兩者最後都進 top-2，因為 `0` 只出現一次。 |

### 預期輸出

```
[PASS] Example 1 -> got {2,1}, want {1,2}
[PASS] Example 2 -> got {1}, want {1}
[PASS] Negatives + dups -> got {2,3}, want {2,3}
[PASS] k == #unique -> got {6,5,4}, want {4,5,6}
[PASS] All identical -> got {7}, want {7}
[PASS] Boundary values -> got {-10000,10000}, want {-10000,10000}

ALL TESTS PASSED
```

注意到 `got` 與 `want` 都是以原始順序印出，其中多筆測資（例如第 4 筆印出 `{6,5,4}` vs. `{4,5,6}`）兩邊順序不同，這是預期的——正確性是由*集合相等*定義。

### 測試目前**沒有**涵蓋的部分（後續可做）

Harness 刻意保持輕量，以下並未包含：

- **AddressSanitizer / LeakSanitizer** — 只要一次 `-fsanitize=address,undefined` 就能抓到 malloc/free 不匹配與 UB。若未來解法有改動，很值得補上。
- **隨機輸入 stress test** — 目前 6 筆都是手選；若再加一個 fuzz 迴圈，與一份暴力的 `O(U log U)` 參考實作做比較，覆蓋度會高很多。
- **最壞情況計時** — 用 `numsSize == 10^5`、且有大量不同元素的輸入，實測確認 O(n log k) 的行為。

這些在初次驗證時刻意排除，但若日後 refactor 這個檔案，是很自然的下一步。

### 收尾

測試 driver 與其二進位檔在跑完後會被刪掉。repo 資料夾內只留下解法 `.c` 與維護中的 markdown 筆記，測試相關的檔案不會被 commit。
