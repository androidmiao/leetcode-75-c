# 242. Valid Anagram — Linux Kernel 連結分析

## Linux 相關原始碼位置

頻率計數（字元直方圖）是一種基礎技巧，在 Linux kernel 中並沒有一個專門的「異位詞檢查」函式，但**字元/位元組頻率統計**和**固定大小計數陣列**的模式在多處出現：

| 檔案 / 模組 | 說明 |
|-------------|------|
| `lib/string.c` | 核心字串操作函式（`strlen`, `strcmp`, `memcmp` 等）。我們的解法依賴 `strlen` 來做長度前置檢查，kernel 的 `strlen` 是高度優化的逐字元或 SIMD 版本。 |
| `lib/sort.c` | kernel 內建的堆積排序（heapsort）實作。題解方法一（排序法）在 kernel 中對應的就是此處的通用排序。kernel 選擇 heapsort 而非 quicksort，以確保 O(n log n) 最壞情況且不需額外堆疊空間。 |
| `fs/ext4/hash.c` | ext4 檔案系統的目錄雜湊函式。使用位元組層級的統計/轉換來計算雜湊值，概念上與我們用字元索引做頻率計數相似。 |
| `crypto/` 子系統 | 密碼學模組中的頻率分析相關工具。例如 `crypto/testmgr.c` 中驗證加密輸出的隨機性時，會統計位元組出現次數——本質上就是 256-bucket 的頻率計數。 |
| `include/linux/bitmap.h` | bitmap 操作巨集。當我們只需要「有/無」而非「多少次」時，bitmap 是 kernel 中的標準做法（例如 CPU mask、IRQ mask）。概念上是頻率計數退化為布林計數的版本。 |
| `lib/textsearch.c` | 文字搜尋框架（Boyer-Moore、Knuth-Morris-Pratt 等）。Boyer-Moore 演算法在建立 bad-character table 時，使用的正是「字元 → 索引」的固定大小查表，與 `freq[c - 'a']` 同一模式。 |

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 242 實作 | Linux Kernel 類比 |
|------|-------------------|-------------------|
| **資料結構** | `int freq[26]` — 固定 26 個小寫字母 | `lib/textsearch_bm.c` 的 bad-char table：`unsigned int bad_shift[ASIZE]`（ASIZE=256），對全部 byte 值建表 |
| **索引方式** | `c - 'a'` 映射到 0–25 | 直接以 byte 值 0–255 為索引，無需偏移 |
| **空間選擇** | O(1)，26 個 int | O(1)，256 個 unsigned int（固定大小） |
| **排序替代** | 題解方法一用 `qsort` | kernel 用 `lib/sort.c`（heapsort），避免 quicksort 的 O(n²) 最壞情況 |
| **穩定性考量** | 不需要（只比較頻率） | `lib/sort.c` 不保證穩定；需要穩定排序時用 `list_sort`（merge sort） |
| **記憶體模型** | 使用者空間堆疊配置 | kernel 中固定大小陣列通常放堆疊，但需注意 kernel 堆疊大小限制（通常 8KB–16KB） |

## 主管 Code Review 角度考題

### 1. 為什麼不用 `memcmp` 取代最後的迴圈檢查？

**期望回答方向**：可以先用 `memset` 建立一個全零陣列再 `memcmp`，但這引入額外的 `memset` 成本且可讀性未必更好。直接迴圈 26 次是常數時間，compiler 通常會向量化或展開。在 kernel 中 `memcmp` 的實作（`lib/string.c`）是逐 byte 比較，對 26×4=104 bytes 的比較反而可能更慢（因為 `memcmp` 不知道是比較全零）。

**真正在考什麼**：理解底層函式的實作成本，不盲目使用抽象。

### 2. 如果字元集從 26 擴展到 Unicode（>1M 碼位），你會怎麼改？

**期望回答方向**：改用雜湊表（hash map）。在 kernel 中對應的是 `include/linux/hashtable.h` 提供的 bucket-chain hash table。Key 是字元碼位，value 是計數。

**追問**：kernel 的 hash table 和 user-space 的 `uthash` / `glib` 有什麼差異？（答：kernel 用 `hlist_head` + `hlist_node` 侵入式鏈結，避免額外的記憶體配置。）

**真正在考什麼**：能否從固定大小陣列平滑過渡到動態資料結構，並理解 kernel 中侵入式鏈結的設計哲學。

### 3. `freq[s[i] - 'a']++` 有沒有安全問題？

**期望回答方向**：如果 `s[i]` 不在 `'a'`–`'z'` 範圍（例如大寫字母或特殊字元），`s[i] - 'a'` 可能產生負數或超出 25 的索引，造成越界存取。在 kernel 中這類存取會觸發 KASAN（Kernel Address Sanitizer）報警。應加入範圍檢查或使用 `unsigned char` 確保索引非負。

**追問**：LeetCode 的 constraint 保證只有小寫字母，但 production code 該怎麼做防禦性程式設計？

**真正在考什麼**：防禦性程式設計意識、對越界存取（OOB）的敏感度。

### 4. 這段程式碼的 cache 行為如何？

**期望回答方向**：`freq[26]` 只有 104 bytes（26 × 4），完全放入 L1 cache line（通常 64 bytes × 2 = 2 條 cache line）。字串 `s` 和 `t` 是連續記憶體的線性掃描，具有極佳的空間局部性。整體 cache miss rate 極低。排序法的 cache 行為較差（`qsort` 的隨機存取模式）。

**真正在考什麼**：cache 局部性（cache locality）意識，理解為什麼線性掃描 + 小陣列查表在實務中比理論複雜度暗示的更快。

### 5. kernel 的 `lib/sort.c` 為什麼選 heapsort 而不是 quicksort？

**期望回答方向**：(1) heapsort 保證 O(n log n) 最壞情況，quicksort 最壞 O(n²)；(2) heapsort 是原地排序，不需額外堆疊空間（quicksort 遞迴深度可達 O(n)）；(3) kernel 堆疊空間有限（8KB–16KB），深遞迴會造成 stack overflow。

**真正在考什麼**：對 kernel 堆疊限制的認知，理解排序演算法在受限環境中的取捨。

## 面試加分總結

- **字元索引查表** 是 kernel 中反覆出現的模式（Boyer-Moore bad-char table、bitmap、crypto byte histogram）。熟悉此模式可展示「從 LeetCode 到系統程式設計」的遷移能力。
- **提及 `lib/sort.c` 選擇 heapsort 的原因**（堆疊限制、最壞情況保證）展示對 kernel 約束環境的理解。
- **防禦性程式設計**（索引越界檢查）是 kernel code review 的必考題，在 LeetCode 解法中主動提及會留下好印象。
- **cache 分析**（26-int 陣列 vs. 排序的隨機存取）顯示你不只看 Big-O，還關注實際硬體效能。
- **Unicode 擴展 → hash table → kernel 侵入式鏈結**的思路展示從具體問題推廣到系統設計的能力。

## Sources

- [lib/sort.c — Linux kernel heapsort](https://github.com/torvalds/linux/blob/master/lib/sort.c)
- [lib/textsearch_bm.c — Boyer-Moore text search](https://github.com/torvalds/linux/blob/master/lib/textsearch_bm.c)
- [lib/string.c — core string functions](https://github.com/torvalds/linux/blob/master/lib/string.c)
- [include/linux/hashtable.h — kernel hash table](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h)
- [include/linux/bitmap.h — bitmap operations](https://github.com/torvalds/linux/blob/master/include/linux/bitmap.h)
