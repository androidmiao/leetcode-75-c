# 162. Find Peak Element — Linux Kernel 連結

## Linux 相關原始碼位置

| 檔案路徑 | 說明 |
|---------|------|
| `lib/bsearch.c` | 核心通用二分搜尋函式 `bsearch()`，供整個核心使用 |
| `include/linux/bsearch.h` | `bsearch()` 的宣告標頭檔 |
| `lib/test_bsearch.c` | `bsearch()` 的核心自測程式（KUnit 測試） |
| `mm/compaction.c` | 記憶體壓縮中使用二分搜尋尋找合適的遷移目標頁框 |
| `fs/btrfs/ctree.c` | Btrfs 檔案系統在 B-tree 節點中使用二分搜尋定位鍵值 |
| `kernel/sched/topology.c` | 排程器用二分搜尋在 CPU 拓撲結構中查找效能域 |

### `lib/bsearch.c` 概述

Linux kernel 的 `bsearch()` 是標準 C `bsearch()` 的核心內部實作，簽名為：

```c
void *bsearch(const void *key, const void *base, size_t num,
              size_t size, cmp_func_t cmp)
```

這是一個**精確匹配**搜尋：給定一個排序好的陣列，用二分法找到與 `key` 完全相等的元素。演算法骨架與 LeetCode 162 的二分搜尋非常相似——都是計算 `mid`、比較、縮小搜尋範圍——但搜尋條件和終止邏輯不同。

---

## Kernel vs. LeetCode 實作對照

| 面向 | Linux `bsearch()` | LeetCode 162 |
|------|-------------------|---------------|
| **搜尋目標** | 精確匹配特定 key | 尋找任意一個局部峰值 |
| **前提條件** | 陣列必須已排序 | 陣列不需排序；僅需相鄰不等 |
| **比較方式** | 透過 `cmp` 函式指標比較 key 與 `base[mid]` | 直接比較 `nums[mid]` 與 `nums[mid+1]` |
| **分支邏輯** | 三路分支：`< 0` 往左、`> 0` 往右、`== 0` 找到 | 二路分支：`>` 往左（含 mid）、`<` 往右（不含 mid） |
| **回傳值** | 找到的元素指標，或 `NULL` | 峰值索引（保證存在） |
| **泛型設計** | `void *` 指標 + `size` 步長 + 比較函式 | 硬編碼 `int *` 陣列 |
| **邊界處理** | 標準 `[0, num)` 開區間右端 | `[l, r]` 閉區間，利用 `-∞` 邊界保證 |
| **空間複雜度** | O(1) 迭代 | O(1) 迭代 |
| **時間複雜度** | O(log n) | O(log n) |

### 核心差異分析

1. **搜尋不變量不同**：`bsearch()` 的不變量是「目標 key 若存在，必在 `[low, high)` 中」；LeetCode 162 的不變量是「搜尋空間 `[l, r]` 必含至少一個峰值」。後者不依賴排序，而是利用邊界 `-∞` 和有限長度的數學性質。

2. **比較方向不同**：`bsearch()` 將 key 與當前元素比較；162 比較的是相鄰的兩個元素 `nums[mid]` 和 `nums[mid+1]`，根據「坡度方向」決定峰值在哪一側。

3. **泛型 vs. 特化**：核心的 `bsearch()` 使用 `void *` 和比較函式指標實現泛型，能搜尋任意型別的陣列。LeetCode 解法直接操作 `int *`，在面試場景中更清晰直觀。

---

## 主管 Code Review 角度考題

### Q1：為什麼不用三路比較（三分支 if-else if-else）？

**預期回答方向：** 本題相鄰元素保證不等（`nums[i] != nums[i+1]`），所以 `mid` 和 `mid+1` 的比較只有兩種結果，不需要等號分支。三路比較是精確搜尋（如 kernel bsearch）的需求，在峰值搜尋中是多餘的。

**追問：** 如果題目改為允許相鄰元素相等，你的演算法還正確嗎？
**考察重點：** 對不變量前提條件的理解深度。

### Q2：`mid = l + (r - l) / 2` 而非 `(l + r) / 2`，這在 kernel 程式碼中重要嗎？

**預期回答方向：** 防止 `l + r` 整數溢位。在 kernel 中，陣列大小可能非常大（例如頁框號陣列），溢位風險是真實的。kernel `bsearch()` 使用相同的防溢位寫法。

**追問：** 在 64 位元系統上 `size_t` 是 64 位元，溢位風險還存在嗎？
**考察重點：** 對資料型別寬度和防禦性程式設計的掌握。

### Q3：kernel 的 `bsearch()` 用 `void *` 實現泛型，你會怎麼把這個峰值搜尋泛化？

**預期回答方向：** 可以抽象為「給定一個判定函式 `predicate(mid)`，在滿足某個單調性條件的空間中找分界點」。這是二分搜尋的通用範式——kernel 的 `bsearch()` 是精確匹配的特例，而峰值搜尋是「方向判定」的特例。

**考察重點：** 抽象思維能力，能否看穿不同二分搜尋變體背後的共同結構。

### Q4：如果陣列非常大且不適合放入 L1 cache，這個演算法的實際效能如何？

**預期回答方向：** 二分搜尋的存取模式對快取不友好——每次跳到陣列的不同區段，幾乎每次都是 cache miss。但因為總共只需 O(log n) 次存取，即使每次都 miss，總代價仍然很小。kernel 的 `bsearch()` 在頁表搜尋等場景中接受這個 trade-off，因為 log n 次 cache miss 遠好於 n 次線性掃描。

**考察重點：** Cache locality 意識，理論複雜度 vs. 實際硬體效能的平衡。

### Q5：迭代 vs. 遞迴——kernel 為何偏好迭代？

**預期回答方向：** Kernel stack 預設只有 8 KB（某些架構甚至更小）。遞迴二分搜尋的 O(log n) 堆疊深度在 user-space 通常不是問題，但在 kernel context 中，深層遞迴可能導致 stack overflow。kernel 的 `bsearch()` 採用迭代實作正是為此。

**追問：** 如果遞迴深度只有 log₂(1000) ≈ 10 層，還有必要擔心嗎？
**考察重點：** 理解 kernel 開發的防禦性原則——即使個別函式安全，組合呼叫鏈可能很深。

---

## 面試加分總結

- 提及 kernel `lib/bsearch.c` 使用相同的迭代二分搜尋骨架，展示你了解這個模式在生產程式碼中的實際應用
- 解釋 kernel 選擇迭代而非遞迴的原因（8 KB kernel stack 限制），展現系統程式設計意識
- 指出 `mid = l + (r - l) / 2` 的防溢位寫法在 kernel 中同樣被採用，展示防禦性程式設計素養
- 區分「精確匹配二分搜尋」（kernel bsearch）和「方向判定二分搜尋」（峰值搜尋），展示對二分搜尋抽象結構的深度理解
- 討論 cache miss 在 O(log n) 存取下的可接受性，展現硬體感知的效能分析能力

---

## Sources

- [lib/bsearch.c](https://github.com/torvalds/linux/blob/master/lib/bsearch.c)
- [include/linux/bsearch.h](https://github.com/torvalds/linux/blob/master/include/linux/bsearch.h)
- [lib/test_bsearch.c](https://github.com/torvalds/linux/blob/master/lib/test_bsearch.c)
