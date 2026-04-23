# 一週模擬面試題庫（Google LeetCode + Linux Kernel 雙維度）

> 本檔案是一個 **7 天輪班制的模擬面試題庫**。每一天都模擬 Google / Meta / Kernel 相關職缺的一場現場面試（45-60 分鐘），每一天圍繞一個 Linux kernel 子系統 + 2-3 題對應的 LeetCode 題目展開。
>
> 使用方式：
>
> 1. 選定一天。關掉其他頁面、開計時器。
> 2. 依順序執行：**熱身 → 演算法 Round → Code Review Round → Kernel Deep-Dive → 自評**。
> 3. 當天結束後翻到 **面試官備忘** 比對自己的回答。
> 4. 一週 7 天 × 2-3 輪 = 約 15-20 場模擬，夠應付 Google 的 on-site quota。
>
> 日程設計參考 `wiki/_index.md` 的 kernel subsystem 索引，並與 `wiki/interview/code-review-questions.md` 的 kernel 擴充章節**雙向呼應**。

---

## 總覽：本週 7 天節奏

| 星期 | 主題 | Kernel 子系統 | 代表 LeetCode 題 | 面試 Round 形式 |
|------|------|----------------|------------------|-----------------|
| Mon  | 鏈表與侵入式結構 | `list_head`, `list_sort` | 206, 141, 234 | Algorithm + Kernel Code Review |
| Tue  | 雜湊與 Open-addressing | `hlist`, `rhashtable` | 1, 49, 347 | Algorithm + Design-for-Kernel |
| Wed  | 平衡樹與排序查找 | `rbtree`, `struct vm_area_struct` | 700, 104, 199 | Algorithm + System Design Mini |
| Thu  | 排序 / Top-K / 合併 | `lib/sort.c`, `lib/list_sort.c` | 215, 23 | Algorithm + Complexity Defense |
| Fri  | 記憶體管理與 LRU | MGLRU, `mm/vmscan.c`, `slub` | 146, 460 (bonus) | Algorithm + Kernel Internals QA |
| Sat  | 環形緩衝與單調佇列 | `kfifo`, `printk ringbuf` | 232, 239, 933 | Algorithm + Embedded Bias |
| Sun  | 圖 / 環偵測 / 拓樸 | `lockdep`, `depmod` | 207, 210 | Algorithm + Debug Scenario |

**每天固定格式**：

- ⏱️ **10 min 熱身**：3 題 concept recall（口述就好）
- ⏱️ **45 min 演算法 Round**：1 題 LeetCode，白板 C 實作 + 口述複雜度
- ⏱️ **30 min Code Review Round**：面試官給一段 kernel-風格代碼，找出 4-6 個問題
- ⏱️ **20 min Kernel Deep-Dive**：追問 kernel 真實實作細節
- ⏱️ **5 min 自評**：對照檢查清單，標記弱項

---

# 📅 Monday — 鏈表與侵入式結構

**Kernel 子系統**：`list_head` （`include/linux/list.h`, `lib/list_sort.c`）

## Mon-1. 熱身（10 min）

用 30 秒回答每題，不要看筆記：

1. `struct list_head` 長什麼樣？為什麼環形？
2. `container_of(ptr, type, member)` 的宏展開原理？
3. 為什麼 kernel 幾乎不寫 LeetCode 那種 `struct ListNode { int val; ListNode *next; }`？

<details>
<summary>面試官備忘</summary>

1. 兩個指標：`next`, `prev`；環形讓頭本身就是哨兵，遍歷無需 NULL 檢查。
2. `((type *)((char *)ptr - offsetof(type, member)))`，編譯時算 offset，零執行時成本。
3. 侵入式讓同一物件可同時在多條鏈上（run queue、sibling list、children list），節省配置且 cache-friendly。
</details>

---

## Mon-2. 演算法 Round（45 min）

> **LeetCode 206. Reverse Linked List（Easy）**
>
> 面試官：「給你單向鏈表，回傳 reversed。寫 C。」

### 做法 A：迭代（首選）

```c
struct ListNode {
	int val;
	struct ListNode *next;
};

struct ListNode *reverseList(struct ListNode *head)
{
	struct ListNode *prev = NULL;
	struct ListNode *curr = head;

	while (curr) {
		struct ListNode *next = curr->next;  /* 暫存下一個 */
		curr->next = prev;                    /* 反轉指向 */
		prev = curr;                          /* 前移 prev */
		curr = next;                          /* 前移 curr */
	}
	return prev;
}
```

**複雜度**：時間 O(n)、空間 O(1)。

### 做法 B：pointer-to-pointer（展示 C 指標功力）

```c
struct ListNode *reverseList(struct ListNode *head)
{
	struct ListNode *prev = NULL, *curr = head, *next;

	while (curr) {
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}
	return prev;
}
```

### Follow-up（面試官會追問）

> 「如果我想反轉區間 `[m, n]` 呢？」

**策略**：dummy node + 找到 `pre = dummy (m-1 次)`；`curr = pre->next`，在 `[m, n]` 區間內做 `n - m` 次「頭插到 pre 之後」。

> 「如果這是 kernel `list_head`，怎麼反轉？」

**策略**：其實 kernel 不需要「反轉」鏈表本身 — 遍歷時用 `list_for_each_prev()` 就是反向。若真要物理反轉，交換每個節點的 `next` 與 `prev` 即可，一趟 O(n)。

---

## Mon-3. Code Review Round（30 min）

以下是面試官給你的 kernel 風格代碼 — **找出至少 5 個問題**：

```c
struct my_obj {
	int val;
	struct list_head node;
};

int add_object(struct list_head *head, int val)
{
	struct my_obj *obj = kmalloc(sizeof(struct my_obj), GFP_KERNEL);
	obj->val = val;
	list_add(&obj->node, head);
	return 0;
}

void delete_all(struct list_head *head)
{
	struct list_head *pos;
	struct my_obj *obj;

	list_for_each(pos, head) {
		obj = container_of(pos, struct my_obj, node);
		list_del(pos);
		kfree(obj);
	}
}

struct my_obj *find(struct list_head *head, int val)
{
	struct my_obj *obj;
	struct list_head *pos;

	spin_lock(&g_lock);
	list_for_each(pos, head) {
		obj = container_of(pos, struct my_obj, node);
		if (obj->val == val) {
			kmalloc(32, GFP_KERNEL);   // 複製一份 metadata
			spin_unlock(&g_lock);
			return obj;
		}
	}
	spin_unlock(&g_lock);
	return NULL;
}
```

<details>
<summary>面試官備忘（6 個問題）</summary>

1. **`add_object` 沒檢查 kmalloc 回傳** — `obj` 可能是 NULL，後面 `obj->val = val` 就 oops。改 `if (!obj) return -ENOMEM;`。
2. **`add_object` 呼叫上下文未寫在文件** — `GFP_KERNEL` 隱含「可睡」，所以不能在 spinlock 裡呼叫；caller 得知道。加註解或 `might_sleep()` 提示。
3. **`delete_all` 用 `list_for_each` 不安全** — 刪除 `pos` 後 `pos->next` 已無意義，會 UAF。改 `list_for_each_safe(pos, n, head)`。
4. **`find` 的 kmalloc 在持有 spinlock 內** — `GFP_KERNEL` 可睡，違反「spinlock 內不可睡」鐵律。Lockdep 會噴。要嘛移出 critical section，要嘛改 `GFP_ATOMIC`。
5. **`find` 回傳指標但不加 refcount** — caller 拿到指標後，若另一執行緒 `delete_all` 釋放，立刻 UAF。正確做法：`refcount_inc(&obj->ref)` 或整個 lookup 改 RCU + `refcount_inc_not_zero`。
6. **`NULL` 無法區分「找不到」與「分配失敗」** — 若要區分，改回 `ERR_PTR(-ENOENT)` / `ERR_PTR(-ENOMEM)`，caller 用 `IS_ERR`。
</details>

---

## Mon-4. Kernel Deep-Dive（20 min）

追問：

1. `lib/list_sort.c` 用什麼演算法？為什麼不用 `lib/sort.c`（Introsort）？
2. `list_sort` 是 bottom-up merge sort，為什麼選 bottom-up 而非 top-down？
3. 這個函式有沒有遞迴？如果沒有，為什麼？
4. `list_sort` 如何處理穩定性？

<details>
<summary>面試官備忘</summary>

1. **Bottom-up 歸併排序**。`lib/sort.c` 是陣列用的 Introsort（quicksort + heapsort 混合），但鏈表隨機訪問 O(n)，不適合 quicksort；歸併天生適合鏈表。
2. **避開 kernel 棧預算**。Kernel 棧通常只有 4K（x86）或 16K（arm64）。Top-down 遞迴深度 O(log n)，壓力不小；bottom-up 迭代，棧固定為配對陣列 `u32 *pending[64]`（夠 2^64 個節點），完全不爆棧。
3. **無遞迴**。純迭代。Merge step 也是迭代逐一穿針。
4. **穩定**。Merge 時當 `cmp(a, b) <= 0` 就取 a（即原順序優先），不 swap 同值節點。穩定性對鎖依賴圖、mount list 等語義很關鍵。
</details>

---

## Mon-5. 自評 Checklist

- [ ] 能白板寫出 iterative reverseList，不查資料
- [ ] 能解釋 `container_of` 零成本 offset 計算
- [ ] 能說明「為什麼 kernel 不用 LeetCode 風格鏈表」
- [ ] 能一眼抓出 spinlock 內 `GFP_KERNEL` 的地雷
- [ ] 能區分 `list_for_each` 與 `list_for_each_safe` 用法
- [ ] 能解釋 `list_sort` 為何 bottom-up

---

# 📅 Tuesday — 雜湊與 Open-addressing

**Kernel 子系統**：`hlist` （`include/linux/hashtable.h`）、`rhashtable` （`include/linux/rhashtable.h`, `lib/rhashtable.c`）

## Tue-1. 熱身（10 min）

1. `hlist` 相比雙向 `list_head`，在雜湊桶節省多少記憶體？
2. `GOLDEN_RATIO_32` 是什麼，為什麼要乘？
3. `rhashtable` 的 resize 是阻塞還是無鎖？

<details>
<summary>面試官備忘</summary>

1. **50%**。桶頭只存一個 `first` 指標（而非 `list_head` 的 `next` + `prev`），因為雜湊表的入口點不需要反向遍歷。節點仍有 `next` + `pprev`（pointer-to-pointer）。
2. **Fibonacci hashing**。`key * GOLDEN_RATIO_32 >> shift`，散佈性比模運算好（尤其是 key 有低位偏差時），且免除慢的除法。
3. **無鎖並發 resize**（`lib/rhashtable.c`）。Reader 透過 RCU 同時看舊桶與新桶，writer 逐桶搬移；resize 進行中讀寫都不阻塞。
</details>

---

## Tue-2. 演算法 Round（45 min）

> **LeetCode 1. Two Sum（Easy）**
>
> 面試官：「給 `nums` 和 `target`，回傳兩個索引使其和為 `target`。進階：用 kernel-style 雜湊表實作。」

### 做法 A：LeetCode 風格 O(n)

```c
#define TABLE_SIZE 65537  /* 質數減少碰撞 */

struct entry {
	int val;
	int idx;
	struct entry *next;
};

int *twoSum(int *nums, int numsSize, int target, int *returnSize)
{
	struct entry *table[TABLE_SIZE] = {0};
	int *ans = malloc(2 * sizeof(int));
	*returnSize = 2;

	for (int i = 0; i < numsSize; i++) {
		int need = target - nums[i];
		unsigned h = ((unsigned)(need * 2654435769u)) % TABLE_SIZE;
		for (struct entry *e = table[h]; e; e = e->next) {
			if (e->val == need) {
				ans[0] = e->idx;
				ans[1] = i;
				/* 清理省略 — 面試要提 */
				return ans;
			}
		}
		/* 插入 nums[i] */
		h = ((unsigned)(nums[i] * 2654435769u)) % TABLE_SIZE;
		struct entry *ne = malloc(sizeof(*ne));
		ne->val = nums[i]; ne->idx = i; ne->next = table[h];
		table[h] = ne;
	}
	return ans;
}
```

**複雜度**：平均 O(n) 時間、O(n) 空間。

### Follow-up

> 「你這個雜湊很像 kernel 的 `DEFINE_HASHTABLE`。展示如何用 `hlist` 寫。」

```c
DEFINE_HASHTABLE(lookup, 16);   // 2^16 buckets

struct entry {
	int val;
	int idx;
	struct hlist_node hnode;
};

/* 插入 */
struct entry *e = kmalloc(sizeof(*e), GFP_KERNEL);
e->val = v; e->idx = i;
hash_add(lookup, &e->hnode, v);

/* 查找 */
struct entry *e;
hash_for_each_possible(lookup, e, hnode, need) {
	if (e->val == need)
		return e;
}
```

---

## Tue-3. Code Review Round（30 min）

```c
struct cache_entry {
	char key[32];
	int value;
	struct hlist_node hnode;
};

static DEFINE_HASHTABLE(cache, 10);
static DEFINE_SPINLOCK(cache_lock);

int cache_insert(const char *key, int val)
{
	struct cache_entry *e = kmalloc(sizeof(*e), GFP_ATOMIC);
	strcpy(e->key, key);
	e->value = val;

	spin_lock(&cache_lock);
	hash_add(cache, &e->hnode, jhash(key, strlen(key), 0));
	spin_unlock(&cache_lock);
	return 0;
}

int cache_get(const char *key, int *out)
{
	struct cache_entry *e;
	u32 h = jhash(key, strlen(key), 0);

	rcu_read_lock();
	hash_for_each_possible_rcu(cache, e, hnode, h) {
		if (strcmp(e->key, key) == 0) {
			*out = e->value;
			rcu_read_unlock();
			return 0;
		}
	}
	rcu_read_unlock();
	return -ENOENT;
}

void cache_remove(const char *key)
{
	struct cache_entry *e;
	u32 h = jhash(key, strlen(key), 0);

	spin_lock(&cache_lock);
	hash_for_each_possible(cache, e, hnode, h) {
		if (strcmp(e->key, key) == 0) {
			hash_del(&e->hnode);
			kfree(e);
		}
	}
	spin_unlock(&cache_lock);
}
```

<details>
<summary>面試官備忘（6 個問題）</summary>

1. **`cache_insert` 沒檢查 kmalloc** — `e` 可能 NULL，`strcpy` 會 oops。
2. **`strcpy` 沒邊界檢查** — 若 `key` > 31 字元，爆 `e->key`。改 `strscpy(e->key, key, sizeof(e->key))`。
3. **`GFP_ATOMIC` 沒必要** — 若 `cache_insert` 在 process context 被呼叫，應該用 `GFP_KERNEL`；`GFP_ATOMIC` 會用掉 emergency pool。先問 caller context。
4. **`cache_remove` 與 RCU reader 有 race** — `hash_del` 後立刻 `kfree`，此時其他 CPU 的 `cache_get` 可能還在持有 `e`。應該用 `hash_del_rcu` + `synchronize_rcu` 後再 free，或 `kfree_rcu(e, rcu)`。
5. **`cache_remove` 找到後不 break** — 雖然 key 唯一時不會真遇到，但 `for_each_possible` 是遍歷 hash chain，刪除後繼續走同鏈很危險；至少加 `break`，或改 `hash_for_each_possible_safe`。
6. **`cache_insert` 允許重複 key** — 不檢查 existing，兩次插入同 key 會留兩個 entry，記憶體永遠漏。應先 lookup 再 insert，或用 `hash_del_rcu(old); kfree_rcu(old)` + `hash_add_rcu(new)`。
</details>

---

## Tue-4. Kernel Deep-Dive（20 min）

1. `rhashtable` 如何在 resize 期間讓 reader 同時看舊桶與新桶？
2. `netfilter conntrack` 為什麼用 `rhashtable` 而不是 `hlist` + 全域鎖？
3. `inode cache` 在 kernel 裡是怎麼組織的？

<details>
<summary>面試官備忘</summary>

1. 每個 bucket head 有 `struct rhash_head __rcu *next`，resize 時新舊表**同時存在**。Reader 先讀 `ht->tbl` 的 RCU 指標，看到舊表就走舊表，看到新表就走新表。Writer 逐桶搬移並在遷移完成後把舊桶的 bucket head 設為「forwarding」狀態；reader 會 retry。詳見 `lib/rhashtable.c:rhashtable_walk_start`。
2. 連線數可達百萬，全域鎖 + rehash 會把整個 softirq 堵住；`rhashtable` 無鎖 resize + per-bucket spinlock，writer 只鎖單一 bucket。
3. `fs/inode.c:inode_hashtable`。用靜態 `hlist_head *` 陣列（大小從 `mem_init` 動態決定），每個 bucket `hlist_bl`（bit-locked hlist，頭指標的最低位當 bit lock）節省一個 spinlock_t 的空間。
</details>

---

## Tue-5. 自評 Checklist

- [ ] 能寫出 open-addressing + chaining 的雜湊表
- [ ] 能解釋 `hlist` 省 50% 記憶體的結構差異
- [ ] 能說明 `rhashtable` 無鎖 resize 的核心技巧
- [ ] 能正確搭配 `hash_del_rcu` + `kfree_rcu`
- [ ] 能判斷 `GFP_KERNEL` vs `GFP_ATOMIC` 依 context 選用

---

# 📅 Wednesday — 平衡樹與系統設計

**Kernel 子系統**：`rbtree` （`include/linux/rbtree.h`, `lib/rbtree.c`）；VMA tree (`mm/mmap.c`)；CFS scheduler (`kernel/sched/fair.c`)

## Wed-1. 熱身（10 min）

1. 為什麼 kernel 選 red-black tree 而不是 AVL？
2. `rb_node` 結構是什麼？「顏色」藏在哪？
3. `rb_leftmost` 如何做到 O(1) 取最小？

<details>
<summary>面試官備忘</summary>

1. RB tree 的 rotation 均攤次數較少（每次插入最多 2 次 rotation，刪除最多 3 次），旋轉是 cache-unfriendly 操作；AVL 更嚴格平衡但旋轉頻繁。Kernel 選 RB 取寫多效能。
2. `struct rb_node { unsigned long __rb_parent_color; struct rb_node *rb_right, *rb_left; };`。父指標與顏色**共用 8 bytes** — 用 `__rb_parent_color` 的最低位當 color bit（因 `rb_node` 至少 4-byte 對齊，最低 2 bits 永遠 0）。
3. `rb_root_cached` 存一個 `rb_leftmost` 指標，每次插入 / 刪除時順便維護。讀時 O(1)，寫時仍 O(log n) 但常數小。用於 CFS scheduler 的 run queue（需要 O(1) 取 vruntime 最小）。
</details>

---

## Wed-2. 演算法 Round（45 min）

> **LeetCode 700. Search in a Binary Search Tree（Easy）**
>
> 面試官：「給 BST root 和 val，找到節點。寫 C。然後告訴我 kernel 的 BST/RB 怎麼組織。」

```c
struct TreeNode {
	int val;
	struct TreeNode *left, *right;
};

struct TreeNode *searchBST(struct TreeNode *root, int val)
{
	while (root) {
		if (val == root->val)
			return root;
		root = (val < root->val) ? root->left : root->right;
	}
	return NULL;
}
```

**複雜度**：時間 O(log n) 平均、O(n) 最差；空間 O(1)。

### Follow-up

> 「為什麼選迭代而不是遞迴？」

**答**：kernel 風格 — 棧預算有限，degenerate tree 深度可能 = n。迭代免得爆棧。

> 「Kernel 的 `rbtree` 沒有通用 search？怎麼用？」

**答**：`rbtree` 不知道你的 key，所以 search 必須由 caller 自己寫。典型 pattern：

```c
struct my_node *my_search(struct rb_root *root, int key)
{
	struct rb_node *n = root->rb_node;

	while (n) {
		struct my_node *m = rb_entry(n, struct my_node, node);
		if (key < m->key)
			n = n->rb_left;
		else if (key > m->key)
			n = n->rb_right;
		else
			return m;
	}
	return NULL;
}

/* insert 同樣由 caller 自己找 parent + link，最後 rb_link_node + rb_insert_color */
```

---

## Wed-3. 系統設計 Mini（30 min）

> 面試官：「設計一個 VMA (virtual memory area) 管理器。要求：給 addr 找到所在的 VMA；插入新 VMA；合併相鄰 VMA。說明資料結構選擇。」

### 預期討論路徑

1. **為什麼不是 hash table？** — VMA 要按**範圍查詢**（給 addr 找包含它的 VMA），雜湊表無法。
2. **為什麼不是 array + binary search？** — 插入 / 刪除是 O(n)，而 VMA 數可達數千（大型 Java app、遊戲引擎），每次 `mmap` 都要搬移陣列不實際。
3. **為什麼 rbtree？** — 範圍查詢 + 插入 / 刪除都 O(log n)；VMA 不太可能極度不平衡；每 `task_struct->mm->mm_mt` 現今 Linux 已改用 **maple tree** (自 6.1)，但 rbtree 是歷史預設。
4. **相鄰合併？** — 需要「鄰居」概念，rbtree 的 inorder 前驅 / 後繼即鄰居；若 `prev->end == curr->start && prev->prot == curr->prot`，可合併。

### 核心 API

```c
struct vma {
	unsigned long start, end;
	unsigned long flags;
	struct rb_node rb;
};

struct vma *find_vma(struct rb_root *root, unsigned long addr)
{
	struct rb_node *n = root->rb_node;
	struct vma *best = NULL;

	while (n) {
		struct vma *v = rb_entry(n, struct vma, rb);
		if (addr < v->end) {
			best = v;
			n = n->rb_left;
		} else {
			n = n->rb_right;
		}
	}
	/* best 是第一個 end > addr 的 VMA；再確認 addr >= best->start */
	return (best && addr >= best->start) ? best : NULL;
}
```

> 加分：提到 **maple tree** 是取代 rbtree 的新資料結構（by Liam Howlett），B-tree 變體，快取更友善，range operations 是一等公民。

---

## Wed-4. Kernel Deep-Dive（20 min）

1. CFS scheduler 為何用 rbtree？key 是什麼？
2. `ext4` 的 extent tree 為什麼不用 rbtree，改用 B+ tree？
3. 比較 rbtree / hash table / maple tree 在 VMA 管理上的取捨。

<details>
<summary>面試官備忘</summary>

1. CFS 要選 **vruntime 最小** 的 task 執行。rbtree 按 vruntime 排序，`rb_leftmost` 取最小 O(1)；tick 中減 vruntime 後重新插入 O(log n)。
2. **磁碟 I/O 成本 >> 計算成本**。Extent tree 節點很大（每 block 4K），B+ tree fanout 幾百，樹高度 2-3 就能索引幾百萬 block。rbtree fanout = 2，樹高太高，每次查詢多次 I/O。
3. Hash：O(1) lookup 但無範圍；rbtree：O(log n) 全能但 cache-unfriendly；maple tree：類 B-tree，範圍操作更好、cache locality 佳、但實作複雜度高。Kernel 主線從 VMA 切 rbtree → maple tree 是 2022 的大事件。
</details>

---

## Wed-5. 自評 Checklist

- [ ] 能白板寫 BST iterative search
- [ ] 能說出 kernel rbtree 沒有通用 search 的設計意圖
- [ ] 能解釋 `rb_leftmost` O(1) 的代價（插入維護成本）
- [ ] 能為 VMA 管理選正確資料結構
- [ ] 知道 maple tree 正在取代 rbtree

---

# 📅 Thursday — 排序、Top-K 與合併

**Kernel 子系統**：`lib/sort.c` (Introsort)、`lib/list_sort.c` (bottom-up merge)、`lib/bsearch.c`

## Thu-1. 熱身（10 min）

1. Kernel 的 `sort()` 用什麼演算法？為什麼不是 qsort？
2. `list_sort()` 與 `sort()` 何時選哪個？
3. Kernel `bsearch()` 有什麼溢位陷阱？

<details>
<summary>面試官備忘</summary>

1. **Heapsort 為主**（`lib/sort.c` 採 heapsort — 舊版是 Introsort，新版單純 heapsort 以保證最差 O(n log n) 且不用遞迴）。選擇原因：
   - Kernel 棧小（4-16K），遞迴 quicksort 會爆棧。
   - 最差 O(n log n)，攻擊者可構造輸入讓 quicksort 退化。
2. 陣列用 `sort()`，鏈表用 `list_sort()`。後者不移動節點只改指標，O(1) 額外記憶體。
3. 計算中點：`(lo + hi) / 2` 會溢位。kernel 用 `lo + (hi - lo) / 2`。
</details>

---

## Thu-2. 演算法 Round（45 min）

> **LeetCode 215. Kth Largest Element in an Array（Medium）**
>
> 面試官：「找陣列第 k 大的數。寫最好的 C 實作。」

### 做法 A：Size-k Min-Heap（Google interview 首選）

```c
/* 陣列實作 min-heap，容量 k */
static void sift_down(int *h, int n, int i)
{
	while (1) {
		int l = 2 * i + 1, r = 2 * i + 2, s = i;
		if (l < n && h[l] < h[s]) s = l;
		if (r < n && h[r] < h[s]) s = r;
		if (s == i) break;
		int t = h[i]; h[i] = h[s]; h[s] = t;
		i = s;
	}
}

int findKthLargest(int *nums, int numsSize, int k)
{
	int *h = malloc(k * sizeof(int));
	for (int i = 0; i < k; i++) h[i] = nums[i];
	for (int i = k / 2 - 1; i >= 0; i--) sift_down(h, k, i);

	for (int i = k; i < numsSize; i++) {
		if (nums[i] > h[0]) {
			h[0] = nums[i];
			sift_down(h, k, 0);
		}
	}

	int ans = h[0];
	free(h);
	return ans;
}
```

**複雜度**：O(n log k) 時間、O(k) 空間。

### 做法 B：Quickselect（面試官追問「更快？」時提）

**期望 O(n)**，最差 O(n²)。Kernel 不會用（最差太糟），但面試要知道取捨。

### Follow-up

> 「如果是 data stream 呢？」 → 維持 size-k heap（即 LeetCode 703）。
> 「如果 n 很大放不進記憶體？」 → external sort / reservoir sampling / MapReduce。

---

## Thu-3. Code Review Round（30 min）

```c
int compare(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

int *sort_copy(int *arr, int n)
{
	int *copy = kmalloc(n * sizeof(int), GFP_KERNEL);
	memcpy(copy, arr, n * sizeof(int));
	sort(copy, n, sizeof(int), compare, NULL);
	return copy;
}

int binary_search(int *arr, int n, int target)
{
	int lo = 0, hi = n;
	while (lo < hi) {
		int mid = (lo + hi) / 2;
		if (arr[mid] == target) return mid;
		if (arr[mid] < target) lo = mid + 1;
		else hi = mid;
	}
	return -1;
}
```

<details>
<summary>面試官備忘（5 個問題）</summary>

1. **`compare` 整數溢位**：`INT_MIN - INT_MAX` 折返。改 `(x > y) - (x < y)` 或 `if/else` 三分支。
2. **`sort_copy` 乘法溢位**：`n * sizeof(int)`，若 n 是 user-controlled 可能折返。改 `kmalloc_array(n, sizeof(int), GFP_KERNEL)`。
3. **未檢查 kmalloc 失敗**。
4. **`binary_search` 的 `(lo + hi) / 2` 整數溢位**：若 `lo, hi` 接近 `INT_MAX`。改 `lo + (hi - lo) / 2`。
5. **回傳值 `int` 表示索引與錯誤碼混用**：`-1` 既是「找不到」也容易與「錯誤碼」混淆；kernel 慣例分開用 `ERR_PTR` 或 `out` 參數，或直接用 `ssize_t`。
</details>

---

## Thu-4. Kernel Deep-Dive（20 min）

1. 為何 `lib/sort.c` 是 heapsort？它以前曾經是 Introsort — 為何改回？
2. `list_sort` 的 bottom-up merge 用哪種 pairing 策略？為何 cache-friendly？
3. 何時會在 kernel 裡用 `bsearch()`？

<details>
<summary>面試官備忘</summary>

1. **Heapsort 有保證的 O(n log n) 最差上界且無遞迴**，符合 kernel 棧預算；quicksort 最差 O(n²)，且遞迴對 kernel 棧有風險。單純 heapsort 簡潔、好 audit、好 merge。
2. **pending list 的 Fibonacci-like pairing** — 保持各子鏈大小接近 `2^k`（更精確是 `fib_k * k/ln(2)`），merge 時永遠在**大小相近**的兩條鏈做合併，確保 cache 熱區小（每次 merge 只觸碰兩個小範圍的記憶體）。詳見 `lib/list_sort.c` 的 head comment。
3. Sorted static array lookup — 例如 system call table、驅動的 register map；不需動態修改但需快速 O(log n) 查找。
</details>

---

## Thu-5. 自評 Checklist

- [ ] 能白板寫 size-k heap Top-K
- [ ] 能講 Quickselect 的最差 case + 為何 kernel 不用
- [ ] 能抓出 `(lo + hi) / 2` 溢位
- [ ] 能 defend 「heapsort 勝 qsort」在 kernel 的原因
- [ ] 知道 `list_sort` 的 Fibonacci pairing 理由

---

# 📅 Friday — 記憶體管理與 LRU Cache

**Kernel 子系統**：MGLRU (`mm/vmscan.c`)、slub allocator (`mm/slub.c`)、page cache

## Fri-1. 熱身（10 min）

1. LRU 的課本答案是什麼資料結構？kernel 的 LRU 一樣嗎？
2. 為什麼 kernel 有 MGLRU (Multi-Generational LRU)？
3. `slub` 為什麼能比 `buddy` 減少碎片？

<details>
<summary>面試官備忘</summary>

1. 課本：**雙向鏈表 + hash map**。Kernel 也用鏈表（`active_list`, `inactive_list`），但每個 zone / memcg 多條，且有 `lru_lock` 保護。
2. 單條 LRU 的 `lru_lock` 在大量 page fault 下成為瓶頸；MGLRU 用**多世代** + per-CPU batching，減少鎖爭用，並對 working set 的識別更精確。
3. slub 是 **slab allocator**，為小物件設計。同 size 的物件打包到同 slab page；分配 / 釋放走 per-CPU freelist，O(1) 且不碰 buddy。Buddy 只能配 2^n 頁，對 kmalloc(96) 這種小物件會浪費頁內大多空間。
</details>

---

## Fri-2. 演算法 Round（45 min）

> **LeetCode 146. LRU Cache（Medium）**
>
> 面試官：「實作 `LRUCache(capacity)`, `get(key)`, `put(key, value)` 均 O(1)。」

### 核心資料結構：hash map + 雙向鏈表

```c
struct node {
	int key, val;
	struct node *prev, *next;
};

typedef struct {
	int capacity;
	int size;
	struct node **table;   /* hash bucket head */
	int mask;
	struct node head;      /* dummy head；最新在 head->next */
	struct node tail;      /* dummy tail；最舊在 tail->prev */
} LRUCache;

LRUCache *lRUCacheCreate(int capacity)
{
	LRUCache *c = calloc(1, sizeof(*c));
	c->capacity = capacity;
	int sz = 1;
	while (sz < capacity * 2) sz <<= 1;
	c->mask = sz - 1;
	c->table = calloc(sz, sizeof(struct node *));
	c->head.next = &c->tail;
	c->tail.prev = &c->head;
	return c;
}

static void dlist_remove(struct node *n)
{
	n->prev->next = n->next;
	n->next->prev = n->prev;
}

static void dlist_push_front(LRUCache *c, struct node *n)
{
	n->next = c->head.next;
	n->prev = &c->head;
	c->head.next->prev = n;
	c->head.next = n;
}

/* bucket chain 簡化：與 dlist 共用 node 不實際，這裡用另一條 hash chain */
/* 為了簡潔省略 hash chain 的實作；面試時口述即可 */

int lRUCacheGet(LRUCache *c, int key)
{
	struct node *n = hash_lookup(c, key);
	if (!n) return -1;
	dlist_remove(n);
	dlist_push_front(c, n);
	return n->val;
}

void lRUCachePut(LRUCache *c, int key, int value)
{
	struct node *n = hash_lookup(c, key);
	if (n) {
		n->val = value;
		dlist_remove(n);
		dlist_push_front(c, n);
		return;
	}
	if (c->size == c->capacity) {
		struct node *old = c->tail.prev;
		dlist_remove(old);
		hash_remove(c, old->key);
		free(old);
		c->size--;
	}
	n = malloc(sizeof(*n));
	n->key = key; n->val = value;
	hash_insert(c, n);
	dlist_push_front(c, n);
	c->size++;
}
```

**複雜度**：`get` / `put` 均 O(1)。

### Follow-up

> 「如果要 LFU + LRU 混合（tie-breaker）呢？」 → 每個頻率 bucket 一條 LRU 鏈。LeetCode 460。
> 「多執行緒版本？」 → 用 `spinlock_t` 或改 read-mostly + RCU。Kernel 的 page LRU 就是 `lru_lock` 保護的雙向鏈表，大 critical section 成為瓶頸 → MGLRU。

---

## Fri-3. Kernel Internals QA（30 min）

> 面試官連續問：

1. Kernel 的 page LRU 為什麼不只用一條？
2. `struct folio` 代替 `struct page` 是什麼進展？
3. `folio_batch` 為何要 batch？
4. `slub` 的 per-CPU freelist 在 `cpu_slab` 裡怎麼組織？

<details>
<summary>面試官備忘</summary>

1. 每個 `mem_cgroup_per_node` 有獨立 LRU（active/inactive × anon/file 共 4 條）。因為 reclaim 要分類決策：anon 要寫 swap，file 可直接 discard；active 剛用過不該換，inactive 才是候選。單鏈表 mixture 做決策會很糟。
2. `struct folio` 是「明確的複合頁面 head」—— 解決以前 `struct page` 對 compound page 語義混淆的問題（tail page 是否能傳入 API、refcount 在哪個 subpage）。Folio 強制「指向 head page」，介面清晰可靜態分析。
3. LRU 移動 / mark_accessed 是熱路徑，每次都拿 `lru_lock` 代價高。`folio_batch` 在 per-CPU buffer 收集 15 個 folio，滿了再批次拿鎖移動。典型的**鎖聚合**（lock coalescing）技巧。
4. `struct kmem_cache_cpu` 有 `freelist` + `page`，`alloc` 走 lockless CAS：取 `freelist` 頭。Freelist 是鏈表，存在 free 物件的前 8 bytes（inline，省記憶體）。當 per-CPU slab 滿時，push 到 per-node partial list。
</details>

---

## Fri-4. Code Review Round（20 min）

```c
int *lru_get_inactive_pages(int cpu)
{
	spin_lock(&lru_lock);
	int *pages = kmalloc(1024 * sizeof(int), GFP_KERNEL);
	if (!pages) {
		spin_unlock(&lru_lock);
		return NULL;
	}
	int i = 0;
	struct page *p;
	list_for_each_entry(p, &inactive_list[cpu], lru) {
		pages[i++] = page_to_pfn(p);
		if (i >= 1024) break;
	}
	spin_unlock(&lru_lock);
	return pages;
}
```

<details>
<summary>面試官備忘（4 個問題）</summary>

1. **spinlock 內 GFP_KERNEL 可睡** — Lockdep 會噴。Alloc 移到 lock 外。
2. **`1024 * sizeof(int)` 乘法溢位風險低但仍應用 `kmalloc_array`**。
3. **硬編碼 1024** — 為什麼？寫成 `#define LRU_SNAPSHOT_MAX 1024` 或做參數。
4. **`inactive_list[cpu]` 的 per-CPU 設計是否正確？** — 若它是 per-NUMA node 而非 per-CPU，這裡索引錯；而且要加 `get_cpu()` / `put_cpu()` 避免搶佔切換 CPU。
</details>

---

## Fri-5. 自評 Checklist

- [ ] 能白板寫 O(1) LRU Cache
- [ ] 能解釋 kernel LRU 與課本 LRU 的差異（多條、多世代）
- [ ] 能說明 `folio` 取代 `page` 的動機
- [ ] 能說明 slub per-CPU freelist 的 lockless 路徑
- [ ] 能抓 spinlock 內 GFP_KERNEL 地雷

---

# 📅 Saturday — 環形緩衝與單調佇列

**Kernel 子系統**：`kfifo` (`include/linux/kfifo.h`, `lib/kfifo.c`)、printk ringbuf (`kernel/printk/printk_ringbuffer.c`)

## Sat-1. 熱身（10 min）

1. 為什麼 `kfifo` 要求大小是 2 的冪次？
2. `kfifo->in` 和 `kfifo->out` 為什麼永不 reset（即永遠 monotonic increasing）？
3. `CIRC_CNT(head, tail, size)` 怎麼計算？

<details>
<summary>面試官備忘</summary>

1. **用位運算代替取模**。`i % size` 變 `i & (size - 1)`，避免 divide。現代 CPU `div` 要幾十 cycle，`and` 是 1 cycle。
2. **避免 ABA**。若 `in` reset，producer 剛 wrap 時 consumer 可能以為 fifo 是空的（`in == out`）。讓 `in`, `out` 一直加，用 `(in - out) & (size - 1)` 算目前位置、用 `in - out` 算可讀量。只要 `in - out` 永遠 ≤ size，數字再大也無妨（unsigned wrap 定義行為）。
3. `(head - tail) & (size - 1)` — 取得環形距離；size 必須是 2 冪次。
</details>

---

## Sat-2. 演算法 Round（45 min）

> **LeetCode 232. Implement Queue using Stacks（Easy）**
>
> 熱身後立刻升到：**LeetCode 239. Sliding Window Maximum（Hard）**
>
> 面試官：「給陣列和窗口大小 k，回傳每個窗口的最大值。O(n)。」

### 做法：單調遞減佇列（deque）

```c
/* 用 indices 在 deque 中，保持 nums[deque[i]] 遞減 */
int *maxSlidingWindow(int *nums, int n, int k, int *returnSize)
{
	int *ans = malloc((n - k + 1) * sizeof(int));
	int *dq = malloc(n * sizeof(int));
	int head = 0, tail = 0;  /* [head, tail) */

	*returnSize = 0;

	for (int i = 0; i < n; i++) {
		/* pop back while smaller — 保持遞減 */
		while (tail > head && nums[dq[tail - 1]] < nums[i])
			tail--;
		dq[tail++] = i;

		/* pop front if out of window */
		if (dq[head] <= i - k)
			head++;

		if (i >= k - 1)
			ans[(*returnSize)++] = nums[dq[head]];
	}

	free(dq);
	return ans;
}
```

**複雜度**：時間 O(n)（每 index 最多進 / 出 deque 各一次），空間 O(k)。

### Follow-up

> 「如果是 data stream（streaming）？」 → 無法知道未來，每個新元素來就打 `push`，deque front 就是答案；若窗口滿了才 pop front。本質一樣。
> 「能不能用 kernel 風格 `kfifo` 實作 queue 部分？」 → `kfifo` 是環形緩衝，寫入 O(1)、讀出 O(1)；但它不提供「pop back」，所以要保留 deque 語義得自己維護 head/tail，或者用 `list_head` 寫雙向 deque。
> 「多執行緒生產者 / 消費者？」 → Kernel 的 `kfifo` 設計為 **single-producer single-consumer lockless**（靠 memory barrier）；多對多要加鎖或另用 `ring_buffer_rcu`。

---

## Sat-3. Code Review Round（30 min）

```c
#define BUF_SIZE 1000

struct my_kfifo {
	char buf[BUF_SIZE];
	int in;
	int out;
};

int kfifo_put_byte(struct my_kfifo *k, char c)
{
	if ((k->in + 1) % BUF_SIZE == k->out)
		return -1;   /* full */
	k->buf[k->in] = c;
	k->in = (k->in + 1) % BUF_SIZE;
	return 0;
}

int kfifo_get_byte(struct my_kfifo *k, char *out)
{
	if (k->in == k->out)
		return -1;  /* empty */
	*out = k->buf[k->out];
	k->out = (k->out + 1) % BUF_SIZE;
	return 0;
}
```

<details>
<summary>面試官備忘（5 個問題）</summary>

1. **`BUF_SIZE = 1000` 不是 2 冪次** — 取模不能替換成 AND，熱路徑慢。改 `BUF_SIZE = 1024` + `((x) & (BUF_SIZE - 1))`。
2. **(in + 1) % SIZE == out 判斷滿**，浪費一個 slot — 這是課本做法，可接受；但可改用 `in - out` 無符號算距離，物盡其用所有 slot。
3. **缺記憶體屏障** — 若是 SPSC 跨 CPU，producer 寫 `buf[in]` 後要 `smp_wmb()` 才能更新 `in`，consumer 讀 `in` 要 `smp_rmb()` 才讀 `buf[out]`。無屏障 reorder 會讀到 garbage。
4. **非原子的 in/out 更新** — 若非單一 producer / consumer，要 atomic 或鎖。
5. **`in`, `out` 為 `int` 可能是負值** — kernel `kfifo` 用 `unsigned int`，wraparound 是 defined behavior。signed overflow 是 UB。
</details>

---

## Sat-4. Kernel Deep-Dive（20 min）

1. printk 的 ringbuf 有什麼特殊需求？
2. `kfifo` 的 `out_rcu` 變種用來做什麼？
3. Per-CPU kfifo 為何減少鎖爭用但增加記憶體？

<details>
<summary>面試官備忘</summary>

1. Printk 必須在**任何** context 下運作 — NMI、中斷關閉、panic 途中、kgdb。因此 ringbuf 不能用 spinlock（會死鎖），採用 **lockless append + sequence-based read**，多 producer 用 atomic CAS 搶 slot，consumer 用序列號重試。詳見 `kernel/printk/printk_ringbuffer.c`。
2. `kfifo_out_rcu` 對底層 data 加 RCU 保護 — 適用於 kfifo 儲存的是指標且目標物件可能被其他路徑釋放的場景，讀者拿出指標後透過 RCU grace period 延遲釋放。
3. Per-CPU kfifo 每 CPU 一份緩衝區，producer 寫自己的 CPU 區，無跨 CPU 原子；但記憶體用量 = `N_CPU × BUF_SIZE`，在 128 核心機上很可觀。trace_pipe、ftrace 就是 per-CPU ring buffer。
</details>

---

## Sat-5. 自評 Checklist

- [ ] 能白板寫單調 deque 求 sliding window max
- [ ] 能解釋 `kfifo` 大小 2 冪次的動機
- [ ] 能解釋 in/out 不 reset 的動機（ABA 防護）
- [ ] 能識別 SPSC 中缺失的 `smp_wmb` / `smp_rmb`
- [ ] 能解釋 printk ringbuf 為何 lockless

---

# 📅 Sunday — 圖、環偵測與拓樸排序

**Kernel 子系統**：`lockdep`（AB-BA 偵測）、`depmod`（模組依賴）、Kahn 拓樸

## Sun-1. 熱身（10 min）

1. Lockdep 偵測環的演算法是 DFS 還是 BFS？為什麼？
2. `depmod` 要解決什麼問題？
3. Kahn 拓樸算法的核心思想？

<details>
<summary>面試官備忘</summary>

1. **BFS**。Kernel 偏好 BFS 因為：棧使用可預測（不遞迴）、延遲上界確定、適合中斷 context。
2. Linux 模組之間有 symbol 依賴（A 用 B 的 exported symbol 就依賴 B）。`depmod` 建立 DAG，決定 `modprobe a` 時要先載 `b`。
3. 反覆挑「入度 0」節點輸出並移除其出邊，直到圖空；若圖有環，必剩下節點都入度 > 0，無法輸出。
</details>

---

## Sun-2. 演算法 Round（45 min）

> **LeetCode 207. Course Schedule（Medium）** + **210. Course Schedule II（Medium）**
>
> 面試官：「給課程依賴 `prerequisites`，能否全部修完？若能，回傳一個有效順序。」

### 做法：Kahn（BFS 拓樸）

```c
bool canFinish(int n, int **prereq, int prereqSize, int *col)
{
	int *indeg = calloc(n, sizeof(int));
	int **adj = malloc(n * sizeof(int *));
	int *cap = calloc(n, sizeof(int));
	int *sz  = calloc(n, sizeof(int));

	for (int i = 0; i < prereqSize; i++) {
		int a = prereq[i][0], b = prereq[i][1];   /* b → a */
		if (sz[b] == cap[b]) {
			cap[b] = cap[b] ? cap[b] * 2 : 4;
			adj[b] = realloc(adj[b], cap[b] * sizeof(int));
		}
		adj[b][sz[b]++] = a;
		indeg[a]++;
	}

	int *q = malloc(n * sizeof(int));
	int head = 0, tail = 0;
	for (int i = 0; i < n; i++)
		if (indeg[i] == 0)
			q[tail++] = i;

	int done = 0;
	while (head < tail) {
		int u = q[head++];
		done++;
		for (int j = 0; j < sz[u]; j++) {
			int v = adj[u][j];
			if (--indeg[v] == 0)
				q[tail++] = v;
		}
	}

	/* cleanup 省略 */
	return done == n;
}
```

**複雜度**：O(V + E)。

### Follow-up

> 「若要回傳順序 (210)，修改處？」 → `q` 本身就是拓樸順序，`done == n` 時 `q` 即答案。
> 「若圖是動態的（邊會增刪）？」 → Kahn 重跑 O(V + E) 每次；或用 **incremental topological sort**（論文題材）。
> 「Kernel lockdep 如何用這個技巧？」 → Lockdep 不做拓樸，它做**環偵測**，但資料結構類似：每把鎖一個 node，每次 `A held while acquiring B` 加一條邊 A → B。BFS 檢查「加新邊會否形成 B → ... → A 的反向路徑」。

---

## Sun-3. Debug Scenario（30 min）

> 面試官：「假設你送了一個 patch，CI 報錯：
>
> ```
> ======================================================
> WARNING: possible circular locking dependency detected
> 6.8.0 #1 Not tainted
> ------------------------------------------------------
> kworker/0:1/8 is trying to acquire lock:
>   (&inode->i_mutex_dir_key#3){+.+.}, at: vfs_rmdir+0x5c/0x1f0
> but task is already holding lock:
>   (&sb->s_type->i_mutex_key#10){+.+.}, at: do_unlinkat+0xa1/0x2e0
> which lock already depends on the new lock.
> ```
>
> 解釋這個 report 在說什麼，你下一步會做什麼。」

### 預期回答

1. **Lockdep 抓到潛在 AB-BA**。當前 context 已持 `s_type->i_mutex_key#10`，正要取 `inode->i_mutex_dir_key#3`；lockdep 從歷史記錄中找到另一個 code path 順序相反。
2. **`{+.+.}` 含義**：第 1 位 `+` 表 irq-on 中曾取過；第 2 位 `.` 表 irq-off 中無；第 3 位 `+` 表 softirq-on；第 4 位 `.`。四個一組對應 hardirq-R / hardirq-W / softirq-R / softirq-W。
3. **下一步**：
   - 在 `/proc/lockdep` 看完整 chain。
   - 閱讀 lockdep report 的「Chain exists of:」段，找到對立 chain。
   - 檢查 VFS 的鎖序文件 `Documentation/filesystems/directory-locking.rst`，確認哪個順序是正確的。
   - 若我的 patch 顛倒順序，改回；否則給鎖分 nesting subclass (`mutex_lock_nested`) 告訴 lockdep「這兩個看起來同類的鎖，我有明確順序」。

---

## Sun-4. Kernel Deep-Dive（20 min）

1. Lockdep 的 `struct lock_class` 多大？為何要限制 classes 數？
2. `lockdep` 抓到 false positive 怎麼關？
3. `depmod -a` 做什麼？`modules.dep` 的格式？

<details>
<summary>面試官備忘</summary>

1. 數十 bytes × 最多 8192 class（`MAX_LOCKDEP_KEYS`）。每次 lockdep_init_map 就分一個 class；若動態建大量鎖（例如每個 inode 一個鎖類別）會耗盡，`lockdep turning off` 警告。解法：用 key + subclass，同一 key 不同 subclass 共享 class。
2. `lockdep_set_class()` 強制指定 class；`lockdep_off()` 暫時關閉；確定是 design 決定的 false positive 可用 `lockdep_set_nocheck()`。
3. 掃描 `/lib/modules/$(uname -r)/` 下所有 `.ko`，讀每個 ELF 的 `__ksymtab` 和 `__versions` 建依賴圖；輸出 `modules.dep`（格式：`module: dep1 dep2 dep3`）。`modprobe` 讀這檔決定載入順序。
</details>

---

## Sun-5. 自評 Checklist

- [ ] 能白板寫 Kahn BFS 拓樸排序
- [ ] 能解釋 lockdep 為何用 BFS 而非 DFS
- [ ] 能讀懂 `possible circular locking dependency` 報告
- [ ] 能說出 `{+.+.}` 四個位元的含義
- [ ] 能連結「拓樸排序」與「`depmod` / module loader」的關係

---

# 週末回顧與二週循環

## 一週結束檢查表（Sunday 晚上）

| 維度 | 自我檢查 |
|------|----------|
| 演算法熟練度 | 7 題是否都能在 25 分鐘內寫完 C？ |
| 指標與記憶體 | 各題的 kernel 版 code review 是否抓到 ≥ 70% bug？ |
| Kernel 知識點 | 能口述每天的 Deep-Dive 答案？ |
| 表達能力 | 能用「模式辨識 → C 實作 → Kernel 延伸」三層結構講每題？ |

**若某天低於 70% 通過**，下週把那天重做，並把當天的 `kernel/*.md` 對應檔案再讀一次。

## 第 2 週建議輪替主題

保留星期節奏不變，但把題目換成「上週同主題但難度更高」：

| 星期 | Week 1 | Week 2（進階） |
|------|--------|------------------|
| Mon | 206 反轉 | 92 區間反轉 + 25 k-group 反轉 |
| Tue | 1 Two Sum | 49 Group Anagrams + 347 Top K Frequent |
| Wed | 700 BST Search | 104 Depth + 199 Right View |
| Thu | 215 Kth Largest | 23 Merge K Sorted Lists |
| Fri | 146 LRU | 460 LFU Cache |
| Sat | 239 Sliding Window Max | 480 Sliding Window Median |
| Sun | 207 Course Schedule | 210 + 269 Alien Dictionary |

## 第 3-4 週：System Design 融合

3 週累積後，把每天的 Kernel Deep-Dive 升級為 **30 分鐘 system design**：

- Mon: 設計進程 run queue（list_head / CFS 的 rbtree）
- Tue: 設計 routing cache（rhashtable）
- Wed: 設計 VMA / page cache（rbtree / maple tree / radix tree）
- Thu: 設計 filesystem write buffer（sort + merge）
- Fri: 設計 OOM killer 策略（page scoring + LRU）
- Sat: 設計 kernel log（ringbuf lockless）
- Sun: 設計 lock ordering checker（lockdep 輕量版）

---

# 附錄 A：每題面試對話腳本模板

以下是一個可直接套用的**三段式面試回答結構**（對應 `overview.md` 強調的三維度整合）：

```
【1. 辨識模式】（30 秒）
「這題是 XXX 模式，辨識信號是 YYY。標準解法 O(n log n)，空間 O(n)，用 ZZZ 資料結構。」

【2. C 實作考量】（2-3 分鐘）
「我會注意三個 C 特有點：
 a) 指標安全：___
 b) 記憶體配對：___
 c) 整數溢位：___
 實作骨架是這樣......」

【3. Kernel 延伸】（1-2 分鐘）
「這個問題在 Linux Kernel 裡對應 XXX 子系統，檔案在 YYY；kernel 做了 ZZZ 不同的選擇，因為 WWW（棧預算 / 上下文 / 鎖爭用 / cache locality）。若這段代碼要進 kernel，我會改成......」
```

---

# 附錄 B：面試官可能追問的萬用 follow-up

無論哪題，這些 follow-up 都適用。練到每個都能答出主體回覆：

1. 「如果輸入是無限流 (stream)？」
2. 「如果輸入放不進單台機器記憶體？」
3. 「如果有 N 個 CPU 同時呼叫這個函式？」
4. 「這段在 kernel 裡會走 atomic context 嗎？怎麼設計才能？」
5. 「如果要這段進 upstream，你會加哪些 annotation（`__must_check` / `likely` / `__read_mostly` / `DEFINE_PER_CPU`）？」
6. 「Lockdep / KASAN / KCSAN 會抓到什麼問題？」
7. 「這個資料結構的 worst case 是什麼？攻擊者能不能惡意構造？」
8. 「如果 memory 只有 4 MB（嵌入式）你會怎麼改？」
9. 「32-bit vs 64-bit 差別？」
10. 「big-endian vs little-endian 會影響答案嗎？」

---

# 附錄 C：對應的 wiki 閱讀路徑

每天模擬完後，回對應 wiki 檔案加深：

| 星期 | 主題 | 對應 wiki 檔案 |
|------|------|-----------------|
| Mon | list_head | `[[kernel/list-head]]` + `[[patterns/linked-list]]` + `[[c-idioms/pointer-manipulation]]` |
| Tue | hash | `[[kernel/hash-table]]` + `[[patterns/hash-table]]` |
| Wed | rbtree | `[[kernel/rbtree]]` + `[[patterns/tree]]` |
| Thu | sort / top-k | `[[kernel/sort-search]]` + `[[patterns/heap-priority-queue]]` |
| Fri | mm / LRU | `[[kernel/memory-management]]` + `[[patterns/hash-table]]` + `[[patterns/linked-list]]` |
| Sat | kfifo / sliding | `[[kernel/circular-buffer]]` + `[[kernel/network-sliding-window]]` + `[[patterns/sliding-window]]` + `[[patterns/monotonic-stack]]` |
| Sun | graph / lockdep | `[[kernel/graph-cycle]]` + `[[patterns/bfs-dfs-graph]]` + `[[patterns/topological-sort]]` |

外加每天都讀的 cheat sheet：

- `[[interview/code-review-questions]]` — 基礎 + Kernel 擴充章節（§6-§13）
- `[[interview/complexity-cheatsheet]]` — 複雜度查表
- `[[interview/edge-cases]]` — 邊界案例庫
- `[[interview/follow-up-patterns]]` — 面試官追問模板

---

# 附錄 D：每週 retrospective 範本

週日晚上在 `logs/YYYY-MM-DD_weekly_mock.md` 紀錄：

```markdown
# Weekly Mock — YYYY-MM-DD

## 完成狀況
- Mon: [ ] Algo [ ] Review [ ] Deep-Dive    通過率：__%
- Tue: [ ] Algo [ ] Review [ ] Deep-Dive    通過率：__%
- Wed: ...
- Sun: ...

## 今週學到的 3 個新知識點
1.
2.
3.

## 今週最弱的環節（下週重做）
- 弱點：___
- 對應 wiki：___
- 下週行動：___

## 預計下週主題
（換進階題組 / 換 system design 模式）
```

---

## 維護者備註

- 本檔 2026-04-20 建立，對應 wiki v1.1（新增 Linux Kernel 擴充章節）。
- 2026-04-20 下午補丁：新增 **附錄 E — Firmware / AP↔SCP 模擬日**（ARM SCMI vs Google ACPM）作為選修第 8 日，配合 `code-review-questions.md` §14-§17 的韌體子系統擴充。
- 下次更新時機：新增 kernel 子系統 wiki、或用戶回報某題在 on-site 被問到的新 follow-up。
- 補題規則：若 raw `.c` 檔新增某題且可對應到本表某天，直接加到附錄 A 的題目列。
- 不要把本檔換成「題目列表」— 它的價值在於**節奏**（熱身 → algo → review → deep-dive → self-eval），不是題量。

---

# 附錄 E：Firmware / AP↔SCP 模擬日（ARM SCMI vs Google ACPM）

> **適用職缺**：Google Android Platform / Pixel Silicon / Samsung Soc / Qualcomm BSP / ARM Systems / 任何涉及 `drivers/firmware/` 的職缺。
>
> **為何選修而非排進週日程**：Firmware 子系統並非每場 on-site 必出；它是**加分項**，用來展示你不只寫 userspace 演算法，而是真的讀得懂 AP↔SCP 的契約。若面試排隊的 JD 明確提到 Pixel、Tensor、Exynos、Snapdragon BSP、或 ARM SCMI，建議排進 Week 2 的週三或週六當天做。
>
> **預習**：做此日**之前**必讀：
>
> - `wiki/interview/code-review-questions.md` §14-§17（ARM SCMI + Google ACPM + 三方比較）
> - `/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/analyses/scmi-vs-google-acpm.md`（三方對照完整分析）
> - `/sessions/amazing-peaceful-wozniak/mnt/common-android-mainline/wiki/subsystems/firmware.md`（Firmware 子系統總覽）

## FW-1. 熱身（10 min）

用 30 秒回答每題，不要看筆記：

1. ARM SCMI 定義了幾個 protocol？10 個大類是什麼？
2. Google Tensor GS101 的 `firmware { }` DT node 的 `compatible` string 是什麼？它走 SCMI 嗎？
3. Qualcomm 主流 Snapdragon（SM8xxx）用什麼協定做 CPU DVFS？延遲和 SCMI perf protocol 相比哪個快？

<details>
<summary>面試官備忘</summary>

1. **10 個**：Base (0x10)、Power Domain (0x11)、System Power (0x12)、Performance (0x13)、Clock (0x14)、Sensor (0x15)、Reset (0x16)、Voltage (0x17)、Powercap (0x18)、Pinctrl (0x19)。4 種 transport：mailbox / SMC / VirtIO / optee。
2. `compatible = "google,gs101-acpm-ipc"`（在 `arch/arm64/boot/dts/exynos/google/gs101.dtsi:486`）。**不是 SCMI** — 走 Samsung Exynos 派的私有 ACPM 協定。對整個 `arch/arm64/boot/dts/exynos/google/` 做 `grep -i scmi` 沒有 match。
3. **OSM/EPSS 硬體直寫**，`qcom-cpufreq-hw` driver 寫暫存器，CPU hardware FSM 自己跑 DVFS 狀態切換 — **連 mailbox IPC 都沒有**。比 SCMI perf（mailbox doorbell + SCP 回覆）快一個量級。這也是 Qualcomm 主流路線堅持 RPMh 不轉 SCMI 的主要技術原因。
</details>

---

## FW-2. 演算法 Round（45 min）

> **Design 題：Bounded Token Allocator（SCMI xfer token 模型）**
>
> 面試官：「設計一個 token 分配器。用途：在有限 in-flight RPC 系統中，為每筆請求分配唯一 ID；當回覆到達時根據 ID 找回 xfer context。
>
> 約束：
>
> 1. Token 空間 $N = 1024$（10-bit，模擬 SCMI 實際 spec）。
> 2. 支援 `alloc(xfer) -> token` 和 `release(token)` 均 **O(1) 均攤**。
> 3. 支援 `lookup(token) -> xfer` O(1)。
> 4. 不能把剛 `release` 的 token 立刻再分配給新 alloc（要等一個 epoch，避免 late reply 誤認）。
> 5. 多生產者 / 多消費者都要正確。
>
> 請用 C 實作，並說明複雜度與同步策略。」

### 解答骨架

核心資料結構：

```c
#include <linux/types.h>   /* 為 u16 等型別 */

#define TOKEN_N       1024          /* 必須 2^k */
#define TOKEN_MASK    (TOKEN_N - 1)

struct xfer;   /* 前向宣告 */

struct token_alloc {
	/* 已分配的 token bitmap（1 = 使用中） */
	unsigned long       bitmap[TOKEN_N / BITS_PER_LONG];

	/* token → xfer 的 O(1) 查找表 */
	struct xfer        *table[TOKEN_N];

	/* 下次 alloc 從哪一格 scan 起（次檢索搜尋起點） */
	u16                 next_hint;

	/* epoch counter：release 時 token 打 epoch tag，alloc 必須看到
	 * epoch 已滾過才能複用。用下面的 released_at[] 實作。 */
	u16                 epoch;
	u16                 released_at[TOKEN_N];

	/* 同步：單鎖版本簡單清楚，熱路徑可改 per-bucket lock 或 idr */
	spinlock_t          lock;
};

void token_init(struct token_alloc *ta)
{
	memset(ta, 0, sizeof(*ta));
	ta->next_hint = 0;
	ta->epoch = 1;
	spin_lock_init(&ta->lock);
}

/* O(1) 均攤 — 用 find_next_zero_bit 找下一個空 slot；
 * 最壞 O(N) 但多數時候 next_hint 就是答案 */
int token_alloc(struct token_alloc *ta, struct xfer *x, u16 *out)
{
	u16 pos;
	unsigned long flags;

	spin_lock_irqsave(&ta->lock, flags);

	/* 從 hint 找空位；繞一圈後若都滿就 -ENOMEM */
	pos = find_next_zero_bit(ta->bitmap, TOKEN_N, ta->next_hint);
	if (pos >= TOKEN_N) {
		pos = find_next_zero_bit(ta->bitmap, TOKEN_N, 0);
		if (pos >= TOKEN_N) {
			spin_unlock_irqrestore(&ta->lock, flags);
			return -EBUSY;   /* 池子滿 */
		}
	}

	/* Epoch check：若這個 token 最近才 release，且 epoch 未滾，跳過 */
	if (ta->released_at[pos] == ta->epoch) {
		/* 近期釋放，等 epoch 滾過再用 */
		spin_unlock_irqrestore(&ta->lock, flags);
		return -EAGAIN;  /* caller 可 retry 或用其他 token */
	}

	set_bit(pos, ta->bitmap);
	ta->table[pos] = x;
	ta->next_hint = (pos + 1) & TOKEN_MASK;

	*out = pos;
	spin_unlock_irqrestore(&ta->lock, flags);
	return 0;
}

struct xfer *token_lookup(struct token_alloc *ta, u16 token)
{
	struct xfer *x;
	unsigned long flags;

	if (token >= TOKEN_N)
		return NULL;

	spin_lock_irqsave(&ta->lock, flags);
	x = test_bit(token, ta->bitmap) ? ta->table[token] : NULL;
	spin_unlock_irqrestore(&ta->lock, flags);
	return x;
}

void token_release(struct token_alloc *ta, u16 token)
{
	unsigned long flags;

	if (token >= TOKEN_N)
		return;

	spin_lock_irqsave(&ta->lock, flags);

	if (!test_and_clear_bit(token, ta->bitmap))
		goto out;  /* double release，忽略（或 WARN） */

	ta->table[token] = NULL;
	ta->released_at[token] = ta->epoch;

	/* 每釋放 N/4 個 token 就滾一次 epoch */
	if (++ta->epoch == 0)   /* u16 wraparound safe */
		ta->epoch = 1;

out:
	spin_unlock_irqrestore(&ta->lock, flags);
}
```

**複雜度**：

- `alloc`：均攤 O(1)，最壞 O(N/64)（bitmap scan 一圈）。
- `release`：O(1)。
- `lookup`：O(1)。

### Follow-up（面試官會追問）

> 「Kernel 裡有現成工具做這件事嗎？」

**答**：有兩個 —

1. **`idr` / `ida`**（`lib/idr.c`、`include/linux/idr.h`）：radix tree 實作、O(log N) alloc/release、id-to-pointer lookup。SCMI 實際就用類似結構（`scmi_xfer_token_set` + `hash_add`）。
2. **`xarray`**（`lib/xarray.c`）：現代版本取代 idr，lockless reader、RCU 友善。新寫的子系統優先 xarray。

> 「為什麼 epoch/defer-reuse 很重要？」

**答**：SCMI 實際碰到的 bug：AP 送 xfer token=7，等待 timeout（30 ms），SCP 過晚回覆 token=7。若此時 token=7 已被新 xfer 複用，SCP 的舊回覆會打到新 xfer 的 completion，造成**用戶看到錯誤的 response payload**。Epoch / defer-reuse 或 kernel SCMI 的 `scmi_abort_match_transfer` 都是為了解決這個。

> 「多 producer 擴展性？」

**答**：單一 `spinlock_t` 在 16 核以上會爆；方案：

- **Per-CPU bitmap 預取**：每個 CPU 保留一小段 token 範圍，alloc 先走 per-CPU cache，滿了才去全域拿一段。參考 `lib/percpu_ida.c`。
- **idr with RCU lookup**：lookup 不拿鎖，alloc/release 拿鎖；SCMI 實際路徑。

> 「這個題目怎麼映射到 SCMI 和 ACPM？」

**答**：

- **SCMI**：`drivers/firmware/arm_scmi/driver.c` 的 `scmi_xfer_token_set()` 用 `IDR + hash_add`，token 是 10-bit 欄位（`msg_hdr` bits 18..27）。池大小由 `scmi.max_msg = 20` 配置但 token 空間是 1024。timeout 時 `scmi_xfer_state_update` 把 state 標為 `SCMI_XFER_DRESP_OK/TIMED_OUT`，late reply 由 token matching 時檢查 state 丟棄 — 等價於 epoch 保護。
- **ACPM**：slot 數量很小（4-8），token 空間與 slot 空間幾乎相同，無分離的「token pool」概念；因此 ACPM 是 per-slot lock + blocking send，沒有 timeout/late reply race（因為沒有真正 concurrent in-flight）。

---

## FW-3. Code Review Round（30 min）

以下是面試官給你的 SCMI vendor protocol driver 骨架 — **找出至少 6 個問題**：

```c
/* vendors/google/scmi_pixel_accel.c — 假想的 Google vendor SCMI protocol */

#include <linux/module.h>
#include <linux/scmi_protocol.h>

#define PIXEL_ACCEL_PROTO  0x81

struct pixel_accel_priv {
	void *ph;                      /* protocol handle */
	struct mutex lock;
	u32 cached_state;
};

static int pixel_accel_set_state(const struct scmi_protocol_handle *ph,
				  u32 state)
{
	struct scmi_xfer *t;
	int ret;

	ret = ph->xops->xfer_get_init(ph, 0x1, sizeof(state), 0, &t);
	memcpy(t->tx.buf, &state, sizeof(state));

	mutex_lock(&((struct pixel_accel_priv *)ph->priv)->lock);
	ret = ph->xops->do_xfer(ph, t);
	mutex_unlock(&((struct pixel_accel_priv *)ph->priv)->lock);

	ph->xops->xfer_put(ph, t);
	return ret;
}

static int pixel_accel_notify_cb(struct notifier_block *nb,
				  unsigned long action, void *data)
{
	struct pixel_event *ev;
	ev = kmalloc(sizeof(*ev), GFP_KERNEL);
	memcpy(ev, data, sizeof(*ev));
	schedule_work(&ev->work);
	return NOTIFY_OK;
}

static int pixel_accel_probe(struct scmi_device *sdev)
{
	struct pixel_accel_priv *priv;
	struct scmi_protocol_handle *ph;

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) return -ENOMEM;

	ph = sdev->handle->protocol_get(sdev, PIXEL_ACCEL_PROTO, NULL);
	if (!ph) return -ENODEV;

	priv->ph = ph;
	sdev->priv = priv;
	return 0;
}

static const struct scmi_device_id pixel_accel_id_table[] = {
	{ PIXEL_ACCEL_PROTO, "pixel_accel" },
	{},
};

static struct scmi_driver pixel_accel_drv = {
	.name       = "scmi-pixel-accel",
	.probe      = pixel_accel_probe,
	.id_table   = pixel_accel_id_table,
};

module_scmi_driver(pixel_accel_drv);
MODULE_LICENSE("GPL v2");
```

<details>
<summary>面試官備忘（至少 8 個問題）</summary>

1. **`xfer_get_init` 的回傳值未檢查** — 第一行 `ret = ...` 之後直接 `memcpy`，若 alloc 失敗 `t` 是 uninitialized，立刻 oops。
2. **`xfer_get_init` 的 tx/rx size 參數順序可能寫反** — 規格是 `(ph, msg_id, tx_size, rx_size, *xfer)`；`sizeof(state), 0` 意思是「tx 4 bytes、rx 0 bytes」；但 `set_state` 通常 SCP 需要回 status，rx=0 是 bug（至少 `sizeof(__le32)` 接 completion code）。
3. **`memcpy(t->tx.buf, &state, sizeof(state))` 沒處理 endian** — 應該 `put_unaligned_le32(state, t->tx.buf)`；spec 要求 little-endian payload。
4. **`ph->priv` 並不存在** — `struct scmi_protocol_handle` 沒有 `priv` 欄位；priv 要存在 `dev_set_drvdata(&sdev->dev, priv)`，取出時 `dev_get_drvdata(&sdev->dev)`。編譯都過不了。
5. **`sdev->priv = priv` 同上** — `struct scmi_device` 也無 `priv`。用 `dev_set_drvdata`。
6. **`protocol_get(...)` 應改用 `devm_scmi_protocol_acquire`**，並用 `IS_ERR`/`PTR_ERR` 而非 `NULL` 檢查（這邊 `return -ENODEV` 吞掉了 `-EPROBE_DEFER` / `-ENOMEM` 的區別）。
7. **`kmalloc` → `devm_kzalloc`**，避免 probe 半途 leak；`kmalloc` 沒清 0 後續用了未初始化的 `cached_state`。
8. **notify_cb 用 `GFP_KERNEL`** — SCMI RX callback 可能在 softirq/threaded IRQ context，要 `GFP_ATOMIC` 或 `GFP_NOWAIT`；`kmalloc` 返回值沒檢查 NULL；`schedule_work` 對一個 uninit 的 `ev->work` 是 UB（沒 `INIT_WORK`）。
9. **mutex + SCMI xfer 的鎖順序** — `do_xfer` 內部已有 SCMI 自己的鎖；在它外面包 mutex 若別處也反過來鎖（先 SCMI 後 mutex），lockdep 會抓 AB-BA。
10. **`PIXEL_ACCEL_PROTO = 0x81` 未對齊 SCMI spec vendor 範圍** — SCMI spec 定義 vendor protocol ID 從 0x80 起，0x81 可用，但要確認沒撞 i.MX vendor 已用的。且這種 vendor protocol 按慣例應該放 `drivers/firmware/arm_scmi/vendors/google/`，而不是自建 path。
11. **沒 `MODULE_DEVICE_TABLE(scmi, ...)`** — modprobe 不會自動載入。
12. **缺 `SPDX-License-Identifier` 開頭的 header**（kernel 2020 後強制）。

</details>

---

## FW-4. Kernel Deep-Dive（20 min）

面試官連問：

1. SCMI 的 `scmi_xfer` 生命週期？誰 alloc 誰 free？失敗路徑怎麼回收？
2. SCMI fastchannel 是什麼？為什麼要有？它 bypass 了什麼？
3. Google Tensor GS101 既然不走 SCMI，那它的 CPU DVFS 怎麼完成？走哪幾層軟體？
4. 若 Qualcomm X Elite 把 SCMI 從 CPUCP perf 擴展到整個 SoC，技術上會卡在哪？
5. 若未來 Google 要 Tensor 轉 SCMI，最小可行的遷移步驟？

<details>
<summary>面試官備忘</summary>

**1. scmi_xfer 生命週期**

- AP 端：`xfer_get_init()` 從 per-protocol xfer pool 拿一個 slot（IDR + refcount），分配 token，準備 tx buffer。
- `do_xfer()`：寫 shmem / VirtIO queue → ring doorbell → `wait_for_completion_timeout(&xfer->done)`。
- SCP 回覆：transport layer IRQ → `scmi_rx_callback` 找 token → `complete(&xfer->done)` 喚醒 waiter。
- `xfer_put()`：refcount 減一，歸還 pool。
- 失敗路徑：timeout → `scmi_xfer_state_update` 標 ABORTED；late reply 到時 match 不到 wake，SCMI 會丟棄；xfer 仍走 `xfer_put` 歸還。
- 參考源碼：`drivers/firmware/arm_scmi/driver.c` 的 `__scmi_xfer_put()`, `scmi_xfer_done_no_timeout()`。

**2. Fastchannel**

- SCMI spec 1.1 加入的 **bypass mailbox** 機制：protocol 在 shared memory 暴露一組 offset 讓 AP 直寫 level/freq，不需送 mailbox command。
- 動機：cpufreq governor 每 10 ms / 1 ms call set_level，走 mailbox 的 latency（AP→SCP→AP 大約 100+ μs）是熱路徑殺手。Fastchannel 把寫入縮到 ≈ 1 μs（single MMIO write + 可選 doorbell）。
- Bypass 了：mailbox doorbell、SCP response、xfer allocation。
- 副作用：SCP 必須 polling shmem 或硬體自動觸發；不是所有 SoC 都支援。
- 實作：`drivers/firmware/arm_scmi/perf.c::scmi_perf_fc_ring_db()`。

**3. GS101 CPU DVFS 完整路徑**

- `cpufreq-dt` 驅動讀 `operating-points-v2` → 選定 OPP → `clk_set_rate(cpu_clk, target)`。
- `cpu_clk` 在 DT 是 `<&acpm_ipc GS101_CLK_ACPM_DVFS_CPUCL0>`，由 `exynos-acpm.c` 的 clock provider 實作。
- ACPM clk `set_rate` → 組 IPC 訊息 → 寫 APM shmem channel → ring ap2apm mailbox doorbell。
- APM 小核（通常 Cortex-M3 / RISC-V）在 alive island 內收 doorbell → 解 IPC → 拉 PMIC（S2MPG10）電壓 → 切 CMU dividers → 回寫 shmem 狀態。
- AP 可選地 poll shmem 等 state=DONE，或走 async（純 fire-and-forget）。
- 整條路徑比 SCMI 多了**無 fastchannel**、**每次都走 mailbox**、**與 PMIC I2C 交織**的延遲。

**4. X Elite 擴 SCMI 的卡點**

- **BCM voter 無對應 SCMI protocol**：NoC 聚合的兩層投票（多 master → BCM 本地加總 → RPMh）SCMI spec 沒有。
- **OSM/EPSS 硬體 FSM**：CPU DVFS 在 SM8xxx 已是無 IPC 方案，換 SCMI perf 反而變慢。X Elite 新 CPUCP 是**為了**上 SCMI 才新設計，不是原 OSM 轉 SCMI。
- **TCS 硬體批次寫**：RPMh 的 sleep/wake vote 不用 CPU SW 執行，硬體自動 fire；SCMI 做不到這件事（mailbox 必定需 CPU send）。
- **QSEECOM / SCM**：安全世界呼叫非 SCMI 範疇，不可能遷。
- **結論**：技術上 SCMI 只能覆蓋「需要 SW policy 的 resource」（CPU perf、某些 hint），硬體旁路的 RPMh path 無法取代。

**5. Google Tensor 遷 SCMI 最小路徑**

- **Step 1**：APM 韌體新增 `SCMI_PROTOCOL_BASE` 的 dialect，同時保留 ACPM legacy 路徑。Shmem 擴增一段 SCMI-layout channel。
- **Step 2**：DT 加 `firmware { scmi { } }` 並列節點（不移除既有 `acpm_ipc`），`compatible = "arm,scmi"`、`mboxes` 指到同一 ap2apm mbox 的新 channel。
- **Step 3**：建立 `drivers/firmware/arm_scmi/vendors/google/` 目錄，加 Google 專屬 vendor protocol（如 thermal mitigation、accelerator hint）。
- **Step 4**：CPU DVFS 先遷：`<&acpm_ipc GS101_CLK_...>` → `<&scmi_perf CPU_CLUSTER_0>`。其餘 clock 保留 ACPM。
- **Step 5**：監測產品穩定性、cpufreq QoS；證實無 regression 後逐步遷 PMIC、power domain。
- **阻力**：Samsung 母平台（Exynos 手機 / modem / Automotive）也要同步改，否則雙源維護成本爆炸。

</details>

---

## FW-5. 自評 Checklist

- [ ] 能口述 SCMI 10 個 protocol 及 4 種 transport
- [ ] 能寫出 `devm_scmi_protocol_acquire` + `ERR_PTR` 的標準 probe 骨架
- [ ] 能解釋 SCMI RX callback 的 GFP 選擇
- [ ] 能設計 bounded token allocator（含 epoch / late-reply 保護）
- [ ] 能區分 SCMI / ACPM / RPMh 三者的 compatible / binding / 上下文
- [ ] 能說出 fastchannel 的動機與 bypass 的內容
- [ ] 能完整追 GS101 的 CPU DVFS 路徑（cpufreq → clk → ACPM → APM 小核 → PMIC）
- [ ] 能解釋為何 Qualcomm 主流不轉 SCMI（硬體旁路 + BCM + OSM）
- [ ] 能指出 SCMI vendor protocol 應放在 `drivers/firmware/arm_scmi/vendors/<vendor>/`
- [ ] 能找出上面 Code Review Round 中 ≥ 6 個問題

---

## FW-6. 對應的 wiki 閱讀路徑

做完這天後，回對應 wiki 檔案加深：

| 主題 | 對應 wiki 檔案 |
|------|-----------------|
| Firmware 子系統總覽 | [[../../../common-android-mainline/wiki/subsystems/firmware.md]] |
| 三方對照（SCMI / ACPM / RPMh）| [[../../../common-android-mainline/wiki/analyses/scmi-vs-google-acpm.md]] |
| ARM SCMI entity | [[../../../common-android-mainline/wiki/entities/arm-scmi.md]] |
| Google ACPM entity | [[../../../common-android-mainline/wiki/entities/google-acpm.md]] |
| Qualcomm firmware stack entity | [[../../../common-android-mainline/wiki/entities/qualcomm-firmware-stack.md]] |
| Code review SCMI 視角 | [[code-review-questions]] §14（本週新增）|
| Code review ACPM 視角 | [[code-review-questions]] §15（本週新增）|
| 三方比較速查 | [[code-review-questions]] §16 |

Firmware 專題補充 Sources：

- [Linux Kernel — `drivers/firmware/arm_scmi/driver.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/firmware/arm_scmi/driver.c)
- [Linux Kernel — `drivers/firmware/arm_scmi/perf.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/firmware/arm_scmi/perf.c)
- [Linux Kernel — `drivers/firmware/samsung/exynos-acpm.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/firmware/samsung/exynos-acpm.c)
- [Linux Kernel — `arch/arm64/boot/dts/exynos/google/gs101.dtsi`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm64/boot/dts/exynos/google/gs101.dtsi)
- [Linux Kernel — `arch/arm64/boot/dts/qcom/hamoa.dtsi` (X Elite SCMI 參考)](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm64/boot/dts/qcom/hamoa.dtsi)
- [Linux Kernel — `Documentation/devicetree/bindings/firmware/arm,scmi.yaml`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/devicetree/bindings/firmware/arm,scmi.yaml)
- [Linux Kernel — `Documentation/devicetree/bindings/firmware/google,gs101-acpm-ipc.yaml`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/devicetree/bindings/firmware/google,gs101-acpm-ipc.yaml)
- [ARM SCMI Specification DEN0056](https://developer.arm.com/documentation/den0056/)
- [LWN — ARM SCMI overview](https://lwn.net/Articles/812588/)

## Sources

- [Linux Kernel — `include/linux/list.h`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/list.h)
- [Linux Kernel — `lib/list_sort.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/list_sort.c)
- [Linux Kernel — `lib/rhashtable.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/rhashtable.c)
- [Linux Kernel — `lib/rbtree.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/rbtree.c)
- [Linux Kernel — `lib/sort.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/lib/sort.c)
- [Linux Kernel — `mm/vmscan.c` (MGLRU)](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/mm/vmscan.c)
- [Linux Kernel — `kernel/locking/lockdep.c`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/locking/lockdep.c)
- [Linux Kernel — `include/linux/kfifo.h`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/kfifo.h)
- [Linux Kernel — `Documentation/process/coding-style.rst`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/process/coding-style.rst)
- [LWN — The maple tree](https://lwn.net/Articles/845507/)
- [LWN — Multi-generational LRU](https://lwn.net/Articles/851184/)
