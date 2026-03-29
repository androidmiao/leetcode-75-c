# 217. Contains Duplicate — Linux Kernel 連結分析

## Linux 相關原始碼位置

| 檔案 | 角色 |
|------|------|
| `lib/sort.c` | Kernel 內建排序實作（heap sort），類似本題使用 `qsort` 排序後掃描的模式 |
| `include/linux/sort.h` | `sort()` 函式宣告，接受自訂比較函式（如同 `qsort` 的 comparator） |
| `lib/list_sort.c` | 鏈結串列用的 merge sort，另一種 kernel 排序場景 |
| `include/linux/hashtable.h` | Kernel 雜湊表巨集，對應本題的 Hash Set 方法 |
| `include/linux/rhashtable.h` | 可調整大小的雜湊表（resizable hash table），進階雜湊結構 |
| `lib/rhashtable.c` | `rhashtable` 實作：動態擴縮、併行安全 |
| `tools/testing/selftests/lib/` | 排序相關的自測試程式 |

## Kernel vs. LeetCode 實作對照

| 面向 | LeetCode 解法（本 repo） | Kernel `sort()` (`lib/sort.c`) |
|------|--------------------------|-------------------------------|
| 排序演算法 | `qsort`（通常為 introsort / quicksort 變體） | Heapsort（保證 O(n log n) 最差情況） |
| 選擇 heapsort 的原因 | N/A | 避免 quicksort 最差 O(n²)；不需額外記憶體；kernel context 中堆疊空間有限 |
| 比較函式 | `int compare(const void* a, const void* b)` | `int (*cmp)(const void *, const void *)` — 相同簽章 |
| 交換函式 | 隱含在 qsort 內部 | 額外提供 `swap_func` 參數，允許最佳化（4-byte / 8-byte 快速路徑） |
| 穩定性 | qsort 不保證穩定 | heapsort 不穩定；`list_sort` 是穩定的 merge sort |
| 記憶體配置 | 在使用者空間堆疊上 | 完全原地（in-place），零動態配置；kernel 中 `kmalloc` 可能失敗 |
| 重複偵測 | 排序後線性掃描相鄰元素 | Kernel 不做重複偵測；但 `rhashtable` 在插入時可偵測重複鍵值 |

### 為何 Kernel 選擇 Heapsort 而非 Quicksort？

1. **最差情況保證**：Quicksort 最差 O(n²)，在 kernel 中不可接受（可能被攻擊者利用特製輸入觸發 DoS）
2. **堆疊深度**：Quicksort 遞迴深度可達 O(n)；kernel 模式堆疊通常只有 8 KB（x86_64），heapsort 為 O(1) 額外空間
3. **無須動態記憶體**：Heapsort 完全原地操作
4. **可預測性**：中斷處理、鎖定持有期間需要可預測的執行時間

### Kernel 雜湊表對應

本題的 Hash Set 方法在 kernel 中對應 `include/linux/hashtable.h` 提供的鏈式雜湊表：

```c
/* Kernel 雜湊表使用範例 */
DEFINE_HASHTABLE(my_table, 10);  /* 2^10 = 1024 buckets */

struct my_entry {
    int key;
    struct hlist_node node;
};

/* 插入 */
hash_add(my_table, &entry->node, entry->key);

/* 查找 */
hash_for_each_possible(my_table, entry, node, key) {
    if (entry->key == target_key)
        /* 找到重複 */
}
```

與 LeetCode 的差異：kernel 雜湊表使用侵入式鏈結（`hlist_node` 嵌入結構體），避免額外記憶體配置。

## 主管 Code Review 角度考題

### Q1：為什麼你用排序法而不是雜湊表？

**預期回答方向**：在純 C 且無外部函式庫的 LeetCode 環境中，手寫雜湊表容易出錯。`qsort` 是 stdlib 標準函式，O(n log n) 對 n ≤ 10⁵ 足夠。若在生產環境中，應使用雜湊表以達到 O(n)。

**真正測試的能力**：工程判斷力——能否在時間限制與正確性之間做出務實的取捨。

**追問**：如果 `n` 到了 10⁸ 呢？你還會用排序嗎？

### Q2：你的 `compare` 函式有什麼潛在問題？

**預期回答方向**：`*(int*)a - *(int*)b` 在極端值（例如 `INT_MAX` 與 `-1`）時會溢位。本題限制條件保證安全，但通用解法應使用三元運算子：`return (a > b) - (a < b);`

**真正測試的能力**：整數溢位意識、防禦性程式設計。

**追問**：Linux kernel 的 `sort()` 是如何處理這個問題的？（答：由呼叫者提供的 comparator 負責，但 kernel 文件強調要避免溢位。）

### Q3：`qsort` 會修改原始陣列。在哪些場景下這是不可接受的？

**預期回答方向**：當呼叫者仍需要原始順序時（例如保留插入順序的日誌系統）。Kernel 的 `sort()` 同樣是原地排序。若需保留原始資料，應先複製。

**真正測試的能力**：API 副作用意識、不可變性思維。

### Q4：如果這個函式在中斷上下文中被呼叫，你會有什麼顧慮？

**預期回答方向**：`qsort` / heapsort 的執行時間為 O(n log n)，在中斷處理器中可能佔用過多時間導致延遲。Kernel 中的排序通常在 process context 下進行。此外，若使用雜湊表，`kmalloc` 在中斷中可能失敗（除非使用 `GFP_ATOMIC`）。

**真正測試的能力**：執行環境意識、kernel context 區分能力。

### Q5：如何將此解法擴展為支援串流資料（streaming）？

**預期回答方向**：排序法需要所有資料到齊才能開始，不適合串流。應改用雜湊集合，對應 kernel 中的 `rhashtable`（可動態擴縮）。每收到一個元素就查詢 + 插入，達到單元素 O(1) 攤銷。

**真正測試的能力**：系統設計思維、online vs. offline 演算法區分。

## 面試加分總結

- 提到 kernel 使用 heapsort 而非 quicksort 的原因（最差情況保證、堆疊安全、零配置），展現對系統層級限制的理解
- 比較 `qsort` 與 kernel `sort()` 的 API 設計差異（swap function 參數），展現對通用函式庫設計的關注
- 討論 `compare` 溢位問題並能給出安全寫法，展現防禦性程式設計素養
- 能從排序解法延伸到雜湊表解法，並對應 kernel 的 `hashtable.h` / `rhashtable`，展現跨層級的資料結構知識
- 能區分 process context vs. interrupt context 對演算法選擇的影響，展現 kernel 開發基本功

## Sources

- [lib/sort.c (Linux kernel source)](https://github.com/torvalds/linux/blob/master/lib/sort.c)
- [include/linux/sort.h](https://github.com/torvalds/linux/blob/master/include/linux/sort.h)
- [include/linux/hashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h)
- [include/linux/rhashtable.h](https://github.com/torvalds/linux/blob/master/include/linux/rhashtable.h)
- [lib/list_sort.c](https://github.com/torvalds/linux/blob/master/lib/list_sort.c)
