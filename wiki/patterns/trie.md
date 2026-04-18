# Trie 字典樹模式

## 簡介

Trie（發音 "try" 或 "tree"，又稱 prefix tree / radix tree 的簡化版）是一種以 **字元為邊** 的樹狀資料結構。每個節點代表一個字首（prefix），從 root 走到節點的路徑就是字串本身。主要應用：字首搜尋、自動補全、字典儲存、字串排序、拼字檢查。

時間複雜度：插入 / 查詢 / 前綴搜尋皆為 O(L)，L 為字串長度（與已存入字串總數 **無關**）。相較於 hash map 的 O(L) 平均、O(n·L) 最壞（碰撞），Trie 沒有碰撞。

## 辨識信號

- 題目有 **大量字串**，且需要查「以某字首開頭的」
- 需要自動補全 / 拼字檢查 / 字典管理
- 字串集合有大量共用字首，想省記憶體
- 需要字串字典序遍歷
- XOR 最大 / 最小問題（特殊的二進位 Trie）

## 變體分類

### 1. 字元 Trie（最常見）

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 208 | Implement Trie | Med | 基本 insert / search / startsWith |
| 211 | Add and Search Word | Med | 支援 `.` 萬用字元，DFS 搜尋 |
| 212 | Word Search II | Hard | Trie + Board DFS，剪枝 |
| 648 | Replace Words | Med | 用字首替換完整單字 |

### 2. 自動補全

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 642 | Design Search Autocomplete System | Hard | Trie + priority queue on top-k |
| 1268 | Search Suggestions System | Med | 字典序 top-3 補全 |

### 3. 二進位 Trie（XOR）

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 421 | Maximum XOR of Two Numbers | Med | 30-bit Trie + 貪心走反向位元 |
| 1707 | Maximum XOR With an Element | Hard | 離線處理 + 持久化 Trie |

### 4. 字典管理 / 去重

| 題號 | 題目 | 難度 | 關鍵點 |
|------|------|------|--------|
| 820 | Short Encoding of Words | Med | 把字串反轉後建 Trie，保留葉節點 |
| 676 | Magic Dictionary | Med | 準確差一字元的搜尋 |

## C 語言模板

### 字元 Trie (小寫英文 26 個)

```c
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CHILD_NUM 26

typedef struct TrieNode {
    struct TrieNode* children[CHILD_NUM];
    bool isEnd;     // 標記：從 root 到此節點是否構成完整字串
} TrieNode;

TrieNode* trie_create_node(void) {
    TrieNode* node = calloc(1, sizeof(TrieNode));  // 所有 children 自動為 NULL
    return node;
}

void trie_insert(TrieNode* root, const char* word) {
    TrieNode* cur = root;
    for (int i = 0; word[i]; i++) {
        int idx = word[i] - 'a';
        if (!cur->children[idx])
            cur->children[idx] = trie_create_node();
        cur = cur->children[idx];
    }
    cur->isEnd = true;
}

bool trie_search(TrieNode* root, const char* word) {
    TrieNode* cur = root;
    for (int i = 0; word[i]; i++) {
        int idx = word[i] - 'a';
        if (!cur->children[idx]) return false;
        cur = cur->children[idx];
    }
    return cur->isEnd;
}

bool trie_starts_with(TrieNode* root, const char* prefix) {
    TrieNode* cur = root;
    for (int i = 0; prefix[i]; i++) {
        int idx = prefix[i] - 'a';
        if (!cur->children[idx]) return false;
        cur = cur->children[idx];
    }
    return true;  // 不需要 isEnd，能走到就算 prefix match
}

void trie_free(TrieNode* root) {
    if (!root) return;
    for (int i = 0; i < CHILD_NUM; i++)
        trie_free(root->children[i]);
    free(root);
}
```

### 二進位 Trie（32-bit XOR 最大）

```c
#define BITS 30  // 視題目決定，32-bit int 通常取 30 避開符號位

typedef struct BinTrie {
    struct BinTrie* child[2];
} BinTrie;

void bintrie_insert(BinTrie* root, int num) {
    BinTrie* cur = root;
    for (int i = BITS; i >= 0; i--) {
        int b = (num >> i) & 1;
        if (!cur->child[b]) cur->child[b] = calloc(1, sizeof(BinTrie));
        cur = cur->child[b];
    }
}

// 貪心：走與 num 當前位相反的分支以最大化 XOR
int bintrie_max_xor(BinTrie* root, int num) {
    BinTrie* cur = root;
    int x = 0;
    for (int i = BITS; i >= 0; i--) {
        int b = (num >> i) & 1;
        int want = 1 - b;
        if (cur->child[want]) {
            x |= (1 << i);
            cur = cur->child[want];
        } else {
            cur = cur->child[b];
        }
    }
    return x;
}
```

## 常見陷阱

1. **記憶體爆炸** — 每個節點含 26 個指標 = 208 bytes（64-bit 系統）。10000 個 15 字母字串就可能要 30 MB。若題目輸入大，改用 `hash map<char, TrieNode*>` 動態 children，或用 **壓縮 Trie（radix tree / patricia tree）**。
2. **沒有 free** — LeetCode 的 C 題 grader 會跑多輪測試，沒 free 容易記憶體累積。務必寫 `trie_free()`。
3. **isEnd vs null** — `trie_search` 必須檢查 `isEnd`，不能只看「走得到」。`trie_starts_with` 則反過來，不需檢查 `isEnd`。
4. **字元集擴展** — 若包含大小寫或 ASCII 完整 128 字元，把 `CHILD_NUM` 改 128 並用 `word[i]` 當 index（別忘了 unsigned cast）。
5. **DFS 萬用字元** — LeetCode 211 的 `.` 要 DFS 所有 children，不能單純走一條路。

## 面試講解

### 30 秒版

> Trie 是字元作為邊的樹狀結構，從 root 走到節點的路徑就是一個字首。適合大量字串 + 字首查詢的場景。插入和查詢都是 O(L)，L 是字串長度，沒有 hash 碰撞問題。代價是記憶體：每個節點要存字元集大小的 children 陣列。

### 2 分鐘版

> Trie 解決的核心問題是「字首查詢」。如果你有 10 萬個字串要查哪些以 "pre" 開頭，用 hash map 要 O(n·L)，用 Trie 只要 O(3)。
>
> 每個節點是一個結構，包含一個 children 陣列（小寫英文就是 26 個指標）和一個 isEnd 標記。insert 就是沿著字元一路下去，缺了就建。search 走完所有字元後要檢查 isEnd 是否為真。startsWith 則不檢查 isEnd，走得到就算。
>
> 變體一是 `.` 萬用字元，那一層就要 DFS 所有 children；變體二是二進位 Trie，用在 XOR 類題目，對每個位元從高位到低位貪心選反向分支。
>
> 實務注意兩件事：一是記憶體，26 個指標很貴，大量節點就得改用 hash map 動態 children，或壓縮成 radix tree；二是記得 free，避免 grader 多輪測試累積記憶體。
>
> Kernel 裡面最相關的是 `lib/radix-tree.c` 和後來的 `lib/xarray.c`，雖然它們是對 integer key 做索引不是字串，但底層思維相同 — 把 key 拆成多層 chunk，每層走一個 children 陣列。

### 常見 follow-up

| 追問 | 回答要點 |
|------|----------|
| "記憶體太大怎麼辦？" | (1) hash map children (2) radix tree / patricia tree 合併單支路徑 (3) double-array trie 編碼 |
| "如何刪除字串？" | 不真的刪節點：把 isEnd 設 false；若整棵子樹都沒字串，再遞迴刪 |
| "如何做拼字修正？" | DP on Trie：對每個位置維護 edit-distance 陣列，剪枝超過閾值的分支 |
| "相比 hash map 的優勢？" | 字首查詢、字典序遍歷、無碰撞、共用字首節省空間 |
| "Trie vs Aho-Corasick 的差別？" | AC 自動機是 Trie + KMP failure link，適合多模式串匹配 |

## 和 Kernel 的關聯

Kernel 中最接近 Trie 思維的實作是 **radix tree / XArray**：

- `lib/radix-tree.c` (已於 6.x 被 `xarray` 取代) — 把 unsigned long key 切成 6-bit chunk，每層 64 個 children。**概念**：以 key 的分段作為 index 走下去，本質就是一棵分支度 64 的 Trie。
- `lib/xarray.c` — radix tree 的現代替代，支援 RCU、內建 spinlock、值可用 internal marker。
- **應用**：page cache（`address_space->i_pages`）、IDR 分配器、filesystem inode 索引。

網路子系統也有相關實作：
- **LPM (Longest Prefix Match) routing** — IP 路由表用 trie 做最長字首匹配。實作在 `net/ipv4/fib_trie.c`。這是字元 Trie 最接近的 kernel 應用。

面試時若談 Trie，可自然延伸到「`fib_trie` 為何用 compressed trie 而非 standard trie？」答案是記憶體：32-bit IP 做標準 trie 要 32 層、分支度 2，節點數爆炸。壓縮後每個節點表示一段固定 prefix，大幅減少節點數。

## 參見

- [[hash-table]] — 字串查詢的另一種選擇（不需字首時更省）
- [[string-manipulation]] — 多模式字串處理的基礎
- [[../kernel/hash-table]] — kernel rhashtable 的設計取捨（對比 radix tree）
