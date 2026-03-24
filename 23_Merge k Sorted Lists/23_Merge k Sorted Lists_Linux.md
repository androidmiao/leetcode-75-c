# 23. Merge k Sorted Lists — Linux Kernel `list_sort` 對照分析

## Linux Kernel 相關原始碼位置

| 檔案 | 用途 |
|------|------|
| `lib/list_sort.c` | 核心實作：bottom-up merge sort for `struct list_head` |
| `include/linux/list_sort.h` | 公開 API 宣告：`list_sort()` 函式原型 |
| `include/linux/list.h` | 雙向環形鏈結串列基礎設施（`struct list_head`, `list_add`, `list_del` 等） |
| `lib/test_list_sort.c` | 核心模組測試：驗證排序正確性與穩定性 |

### API 簽名

```c
/* include/linux/list_sort.h */
typedef int __attribute__((nonnull(2,3)))
(*list_cmp_func_t)(void *priv,
                   const struct list_head *a,
                   const struct list_head *b);

__attribute__((nonnull(2,3)))
void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);
```

重點：`list_sort` 是 **in-place** 排序，直接修改傳入的鏈結串列，不回傳新鏈結串列。比較函式透過 `priv` 攜帶不透明上下文。

---

## Kernel `list_sort` vs. LeetCode 23 實作對照

### 演算法層面

| 維度 | LeetCode 23 (本 repo `.c`) | Linux Kernel `list_sort` |
|------|---------------------------|--------------------------|
| 方向 | Top-down 分治（遞迴二分） | Bottom-up 歸併（迭代累積） |
| 資料結構 | 單向鏈結串列 `struct ListNode` | 雙向環形鏈結串列 `struct list_head` |
| 合併策略 | 遞迴拆半 → 兩兩合併 | 維護 pending list，按 2^k 規則觸發合併 |
| 穩定性 | 穩定（`<` 而非 `<=` 分支取 l2） | 穩定（cmp 回傳 0 時保持原序） |
| 時間複雜度 | O(N log k) | O(N log N) |
| 空間複雜度 | O(log k) 遞迴堆疊 | O(log N) pending 指標陣列 |
| 比較函式 | 硬編碼 `val` 比較 | 函式指標 + `priv` 參數（泛型） |
| 輸入型別 | `struct ListNode**` 陣列 | 單一 `struct list_head*` 鏈結串列 |

### 核心差異：Top-Down vs. Bottom-Up

**LeetCode 23 — Top-Down:**
```
           merge(0, k-1)
          /             \
   merge(0, k/2)    merge(k/2+1, k-1)
      /    \              /    \
   ...     ...         ...     ...
```
先遞迴拆分到單個串列，再逐層合併上來。需要知道 `k`（串列總數）。

**Kernel list_sort — Bottom-Up with 2^k merging:**
```
逐一取出元素 → pending[0], pending[1], ...
count=1: pending[0] (size 1)
count=2: merge pending[0]+pending[1] → pending[1] (size 2)
count=3: pending[2] (size 1)
count=4: merge pending[2]+pending[1] → pending[1] (size 4)
...
最後: final merge 把所有 pending 串列合併
```
不需要事先知道串列長度，一邊走訪一邊合併。count 的二進位表示控制何時觸發合併——當第 k 位從 0 翻到 1 時，合併兩個大小為 2^k 的子串列。

### Kernel 的 2^k 合併規則（精華）

核心思想：用 `count` 的二進位位元來決定合併時機。

```
count 的二進位    觸發的動作
─────────────    ──────────
0001 (1)         存入 pending
0010 (2)         合併 2 個 size-1 → size-2
0011 (3)         存入 pending
0100 (4)         合併 2 個 size-2 → size-4
0101 (5)         存入 pending
0110 (6)         合併 2 個 size-1 → size-2
0111 (7)         存入 pending
1000 (8)         合併 2 個 size-4 → size-8
```

規則：每次 count++，觀察哪個 bit 被設定——如果 bit k 從 0→1 且 bits 0..(k-1) 全歸零，就合併兩個 size-2^k 的串列。

這個設計的妙處在於：cache 友善度。合併總是發生在「剛好有足夠的新元素來配對」的時刻，使得工作集盡可能留在 L1/L2 cache 內。

### Kernel `merge()` 與 `merge_final()` 的分工

```
merge():        合併兩個 NULL-terminated 單向串列 (內部工作格式)
                ─ 只設定 ->next，不碰 ->prev
                ─ 回傳合併後的單向串列頭

merge_final():  最後一次合併，同時重建雙向環形結構
                ─ 設定 ->prev 指標
                ─ 接回 head，完成環形
                ─ 只在 list_sort() 最後呼叫一次
```

這個兩階段設計避免了在中間合併步驟浪費時間維護 `prev` 指標——反正下一次合併又會打亂它們。

---

## 主管 Code Review 角度可能的考題

以下模擬一位熟悉 Linux kernel 的主管，在 code review 時可能提出的問題與考察方向：

### 1. 「為什麼 Kernel 選 Bottom-Up 而不是你這種 Top-Down？」

**考察重點：** 對 cache locality 的理解

**期望回答方向：**

Top-down 遞迴需要先拆分到底再合併，遞迴深度 O(log k)。在 kernel 場景中，排序的鏈結串列可能非常長（數萬甚至數十萬個元素），top-down 需要：(a) 事先知道長度或用快慢指標找中點——前者不適用於 `list_head`，後者多一次完整走訪；(b) 遞迴堆疊在 kernel space 中很寶貴（kernel stack 通常只有 8KB-16KB）。

Bottom-up 迭代法用一個小的 pending 陣列（~64 個指標，log₂(N) 槽位）就能完成，不需要遞迴，也不需要預先知道長度。更重要的是，2^k 合併策略讓每次合併的工作集大小控制在 cache 容量內。

### 2. 「你的 `mergeTwoLists` 為什麼用 dummy node？Kernel 的 merge 怎麼處理的？」

**考察重點：** 指標操作功力與 indirect pointer 技巧

**期望回答方向：**

LeetCode 版本用 `struct ListNode dummy` 在 stack 上配置哨兵節點，避免頭節點的特殊處理。這在 userspace 完全 OK。

Kernel 版本使用 **indirect pointer**（指向指標的指標）技巧：

```c
struct list_head *merge(... struct list_head *a, struct list_head *b)
{
    struct list_head *head, **tail = &head;
    for (;;) {
        if (cmp(priv, a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) { *tail = b; break; }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) { *tail = a; break; }
        }
    }
    return head;
}
```

`**tail` 是 Linus Torvalds 強調的「理解指標本質」的經典技巧。不需要 dummy node，透過 `tail = &head` 初始化，然後 `tail = &node->next` 推進，每次 `*tail = winner` 就自動接上鏈結串列。

**面試追問：** 「能不能把你的 `mergeTwoLists` 改用 indirect pointer？省掉 dummy node？」

### 3. 「Kernel 的 cmp 回傳值語義和你的硬編碼比較有什麼差異？這對 API 設計有什麼影響？」

**考察重點：** 泛型設計思維、穩定排序

**期望回答方向：**

LeetCode 版本：`if (l1->val < l2->val)` — 硬編碼比較整數，只適用於特定型別。

Kernel 版本：`cmp(priv, a, b) <= 0` — 函式指標 + void* priv，完全泛型。任何可嵌入 `list_head` 的結構體都能排序，使用者透過 `container_of` 在 cmp 中取得外層結構。

穩定性保證：cmp 回傳 <= 0 時取 `a`（先出現的元素），確保相等元素不交換。LeetCode 版本的 `<`（取 l1）和 `else`（取 l2）剛好也是穩定的，但這是巧合而非有意設計。

### 4. 「Kernel 為什麼要分 `merge()` 和 `merge_final()` 兩個函式？」

**考察重點：** 效能敏感度、lazy evaluation 思維

**期望回答方向：**

`list_head` 是雙向環形鏈結串列，完整維護需要設定 `prev` 指標。但在中間合併步驟中，`prev` 指標會在下一次合併被覆寫。所以 `merge()` 只維護 `next`（當作單向串列操作），只在最後一步 `merge_final()` 才重建完整的雙向環形結構。

這省掉了 O(N log N) 次不必要的 `prev` 指標寫入——如果有 log N 層合併，每層 N 個節點都要多寫一次 `prev`。

**面試追問：** 「如果讓你設計一個支援雙向鏈結串列的 merge sort，你會怎麼安排 prev 的維護時機？」

### 5. 「Kernel stack 只有 8-16KB，你的遞迴方案放到 kernel 裡會怎樣？」

**考察重點：** Kernel programming 的限制意識

**期望回答方向：**

LeetCode 方案的遞迴深度是 O(log k)。若 k = 10^4，log₂(10000) ≈ 14，每個 stack frame 大約 32-48 bytes（兩個指標 + 區域變數 + return address），總共不到 1KB，在 kernel 中也 OK。

但如果是排序單一很長的鏈結串列（N = 10^6），top-down 需要先找中點（O(N) 走訪），遞迴深度 O(log N) ≈ 20。每層的 stack frame 更大因為需要保存分割後的兩半指標。雖然 20 層通常不會爆 stack，但 kernel 開發者傾向避免不必要的遞迴。

Kernel 的 bottom-up 方案完全是迭代的，pending 陣列放在 stack 上只佔 ~64 個指標（512 bytes on 64-bit），非常安全。

### 6. 「`list_sort` 的 `__attribute__((nonnull))` 有什麼用？如果拿掉會怎樣？」

**考察重點：** Defensive programming、compiler 輔助

**期望回答方向：**

`nonnull(2,3)` 告訴編譯器第 2、3 個參數不可為 NULL。效果有二：(a) 編譯器可以優化掉內部的 NULL 檢查；(b) 如果呼叫端傳入已知為 NULL 的值，編譯器會發出 warning。這是 kernel 常用的防禦性程式設計手法，讓錯誤在編譯期而非執行期被發現。

### 7. 「你的方案和 Kernel 方案，哪個比較次數更少？」

**考察重點：** 精確的演算法分析能力

**期望回答方向：**

兩者的漸進複雜度都是 O(N log k) / O(N log N)，但常數因子不同。

Kernel 的 2^k 方案經過精確分析，比傳統 bottom-up merge sort 少了約 0.2N 次比較。原因是傳統方案在最後一步可能合併兩個嚴重不等長的串列（比如 size N-1 和 size 1），浪費比較次數。2^k 方案讓合併更平衡，但不完全嚴格——它犧牲了一點平衡性來換取更好的 cache 行為。

LeetCode 23 的 top-down 分治天然是平衡的（左右各 k/2 個串列），但它的比較次數取決於各串列長度的分佈，而非一個固定的排序序列。

**面試追問：** 「在什麼情況下 top-down 會比 bottom-up 做更多比較？」

### 8. 「如果要在 Kernel 中做 merge k sorted lists（不是排序單一串列），你會用 `list_sort` 嗎？」

**考察重點：** 系統設計、工具選擇判斷力

**期望回答方向：**

看情況。如果 k 個有序串列可以先用 `list_splice` 拼成一個串列再排 `list_sort`，那就是 O(N log N)。但如果 k 遠小於 N（比如 k=4, N=10^6），專門寫一個分治 merge-k 或用 min-heap 可以達到 O(N log k)，比 O(N log N) 更好。

在 kernel 實務中，k 通常很小（merge 幾個 per-CPU 的 sorted list），所以寫一個 ad-hoc k-way merge 比呼叫泛型 `list_sort` 更高效也更常見。

---

## 補充：Kernel 中其他使用 `list_sort` 的場景

| 子系統 | 用途 |
|--------|------|
| **fs/xfs** | 排序 inode cluster buffers、extent records |
| **mm/slab** | 排序 partial slab lists |
| **drivers/gpu/drm** | 排序 GPU command buffers、fences |
| **fs/ubifs** | 排序 journal entries |
| **net/netfilter** | 排序 connection tracking entries |

這些場景的共同特徵：資料量可大可小、需要穩定排序、排序鍵值透過 `container_of` 取得、不能 kmalloc 額外記憶體（可能在 atomic context）。

---

## 總結：面試中如何用 Linux Kernel 加分

1. **提到你知道 kernel 用 bottom-up 而非 top-down**，展示對不同 merge sort 變體的理解。
2. **解釋 indirect pointer (`**tail`) 技巧**，展示指標操作功力。
3. **說明 `merge()` vs. `merge_final()` 的 lazy prev 維護**，展示效能敏感度。
4. **討論 cache locality 對合併策略的影響**，展示系統層面思維。
5. **能判斷何時用 `list_sort` vs. ad-hoc k-way merge**，展示工程取捨能力。

---

# 23. 合併 K 個升序鏈結串列 — Linux Kernel `list_sort` 對照分析（繁體中文）

（以上內容已全程使用繁體中文撰寫，此處不再重複翻譯。）

---

## Sources

- [Linux kernel lib/list_sort.c (GitHub)](https://github.com/torvalds/linux/blob/master/lib/list_sort.c)
- [Linux kernel include/linux/list_sort.h (GitHub)](https://github.com/torvalds/linux/blob/master/include/linux/list_sort.h)
- [LWN.net: Introduce generic list_sort function](https://lwn.net/Articles/368648/)
- [LKML: George Spelvin — lib/sort bottom-up heapsort discussion](https://www.lkml.org/lkml/2019/3/13/1066)
