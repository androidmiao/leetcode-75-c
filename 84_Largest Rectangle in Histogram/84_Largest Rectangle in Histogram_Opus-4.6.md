# LeetCode 84: Largest Rectangle in Histogram — Implementation & Analysis

## English Version

### Overview

Problem 84 asks us to find the largest rectangular area in a histogram where each bar has width 1. The `.c` solution in this repo implements **Approach 5 (Monotonic Increasing Stack with Sentinels)** from the editorial — the optimal O(n) approach.

---

### Implementation Comparison

#### Repo `.c` Solution — Stack with Sentinel Array

The implementation creates a new array `new_heights` with 0-height sentinels at both ends, then runs a single pass with a monotonic increasing stack.

**Distinctive features:**
- Allocates a `new_heights` array of size n+2 and copies the original data in, rather than modifying the input or handling boundaries with conditionals
- Left sentinel (index 0, height 0) ensures the stack is never empty during pop
- Right sentinel (index n+1, height 0) forces all remaining bars to be popped at the end
- Eliminates the need for a separate cleanup loop after the main iteration

#### Editorial Approach 5 — Stack with Boundary Sentinel

The editorial pushes −1 as a sentinel onto the stack instead of modifying the heights array. After the main loop, a separate while-loop pops remaining elements using `heights.length` as the right boundary.

**Distinctive features:**
- No extra array allocation — works directly on the original heights array
- Uses −1 in the stack as a left boundary marker
- Requires a second loop to handle bars remaining on the stack after traversal

---

### Which Approach Is Better?

| Dimension | Repo (Array Sentinels) | Editorial (Stack Sentinel) |
|-----------|----------------------|---------------------------|
| Time complexity | O(n) | O(n) |
| Space complexity | O(n) — extra array + stack | O(n) — stack only |
| Memory allocation | Two malloc calls (stack + new_heights) | One allocation (stack only) |
| Code simplicity | Single loop, no special cases | Two loops (main + cleanup) |
| Interview clarity | Slightly easier to explain — unified loop | Slightly more memory efficient |
| C implementation | Straightforward — array copy + single loop | Need to handle empty-stack edge cases |

**Baseline approach:** Approach 2 (Better Brute Force) — O(n²), O(1). Easy to code, easy to explain, but too slow for n = 10⁵.

**Recommended approach:** Approach 5 (Stack-based). Both the repo's sentinel-array variant and the editorial's stack-sentinel variant are optimal. For a Google-style interview, the editorial version saves one allocation but requires explaining two loops. The repo's version is cleaner in C because the single-loop structure is easier to get right under pressure.

**Verdict:** The repo's implementation and the editorial's Approach 5 are the same algorithm (monotonic increasing stack). The repo uses a slightly different sentinel strategy that trades O(n) extra memory for cleaner control flow. Both are excellent interview answers.

---

---

# LeetCode 84：柱狀圖中最大的矩形 — 實作分析

## 繁體中文版

### 概述

第 84 題要求在柱狀圖中找出最大矩形面積，每根柱子寬度為 1。本 repo 的 `.c` 解法實作了題解中的**方法五（單調遞增棧 + 哨兵）**— 最佳的 O(n) 解法。

---

### 實作對照

#### Repo `.c` 解法 — 哨兵陣列 + 堆疊

建立新陣列 `new_heights`，首尾各放一個高度為 0 的哨兵柱子，然後用單調遞增棧做一趟掃描。

**特色：**
- 配置大小 n+2 的 `new_heights` 陣列，將原始資料複製進去，不修改輸入也不需要條件判斷處理邊界
- 左哨兵（索引 0，高度 0）確保 pop 時棧永遠不為空
- 右哨兵（索引 n+1，高度 0）強制所有殘留柱子被彈出
- 不需要主迴圈後的額外清理迴圈

#### 題解方法五 — 棧中哨兵

題解將 −1 推入棧中作為哨兵，而非修改高度陣列。主迴圈結束後，另一個 while 迴圈以 `heights.length` 為右邊界彈出剩餘元素。

**特色：**
- 不需額外陣列配置 — 直接操作原始高度陣列
- 以 −1 在棧中作為左邊界標記
- 需要第二個迴圈處理遍歷後仍留在棧中的柱子

---

### 哪種方法較好？

| 比較維度 | Repo（陣列哨兵） | 題解（棧哨兵） |
|---------|----------------|--------------|
| 時間複雜度 | O(n) | O(n) |
| 空間複雜度 | O(n) — 額外陣列 + 棧 | O(n) — 僅棧 |
| 記憶體配置 | 兩次 malloc（棧 + new_heights） | 一次配置（僅棧） |
| 程式簡潔度 | 單迴圈，無特殊情況 | 雙迴圈（主迴圈 + 清理） |
| 面試清晰度 | 稍易解釋 — 統一迴圈結構 | 稍省記憶體 |
| C 實作難度 | 簡單 — 陣列複製 + 單迴圈 | 需處理空棧邊界條件 |

**基線方法：** 方法二（改良暴力法）— O(n²)，O(1)。容易撰寫和解釋，但對 n = 10⁵ 太慢。

**推薦方法：** 方法五（堆疊法）。本 repo 的哨兵陣列變體和題解的棧哨兵變體都是最佳解。在 Google 風格面試中，題解版本省一次配置但需解釋兩個迴圈。Repo 版本在 C 中更乾淨，因為單迴圈結構在壓力下更容易寫對。

**結論：** Repo 實作與題解方法五是相同演算法（單調遞增棧）。Repo 使用稍微不同的哨兵策略，以 O(n) 額外記憶體換取更清晰的控制流程。兩者都是優秀的面試答案。
