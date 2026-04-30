# Tue-3. Code Review Round — 解答檔

> 來源：`weekly-mock-bank.md` 第 307–373 行的 **Tue-3. Code Review Round（30 min）**
>
> 本檔案：
> 1. 將原始 hash-table + spinlock + RCU 風格代碼 **逐行加上繁體中文註解**，清楚標示每一行的用途與「地雷點」。
> 2. 依照面試官備忘提出的 **6 個問題**，給出修正後的完整版本，同樣逐行加上繁體中文註解。
>
> 目標讀者：Android/Linux kernel 面試自我演練用。搭配 [[kernel/spinlock]]、[[kernel/hash-table]] 閱讀效果最好。

---

## 一、原始代碼（逐行繁體中文註解＋問題標註）

下列代碼每一行後方以 `//` 或獨立註解解說用途；其中有地雷的行會標上 **🔴 問題 N**，對應第二節的修正。

```c
/* ----------- 資料結構定義 ----------- */

struct cache_entry {                              // cache 條目：key → value
    char key[32];                                 // 固定大小字串 key（最多 31 字元 + '\0'）—— 之後 strcpy 會踩這個邊界
    int value;                                    // 對應的整數值
    struct hlist_node hnode;                      // kernel hash chain 節點（hlist 版本比 list_head 省 50% 記憶體）
    /* ❌ 少了 RCU callback head（如 struct rcu_head rcu;）—— Bug #4 無法用 kfree_rcu */
};

/* 全域 hash table 與鎖 */
static DEFINE_HASHTABLE(cache, 10);               // 1 << 10 = 1024 個 buckets；編譯期初始化
static DEFINE_SPINLOCK(cache_lock);               // 保護 writer（insert / remove）；reader 走 RCU 不需要鎖

/* ==============================================================
 *  cache_insert —— 新增一個 key → value 映射
 * ============================================================== */

int cache_insert(const char *key, int val)
{
    struct cache_entry *e = kmalloc(sizeof(*e),   // 分配新 entry
                                    GFP_ATOMIC);  // 🔴 問題 3：硬是用 GFP_ATOMIC 會吃 emergency pool；若 caller 在 process context 應該用 GFP_KERNEL
                                                  //          caller context 未寫在文件，拿不準就選 GFP_ATOMIC 是壞習慣
    /* 🔴 問題 1：沒檢查 kmalloc 回傳 NULL —— 下一行 strcpy 會 NULL deref → kernel oops */

    strcpy(e->key, key);                          // 🔴 問題 2：strcpy 不做邊界檢查；若 strlen(key) >= 32 會直接踩壞 e->value / e->hnode / 相鄰 slab 物件
    e->value = val;                               // 寫入值

    spin_lock(&cache_lock);                       // 進入 writer critical section（此處不可睡）
    /* 🔴 問題 6：沒先 lookup 是否已有相同 key；重複 insert 會在同 bucket 留兩個 entry → 永久記憶體洩漏（舊 entry 再也找不到） */
    hash_add(cache, &e->hnode, jhash(key, strlen(key), 0)); // 把 entry 掛進 bucket；hash_add 非 RCU 版本（publish 不用 rcu_assign_pointer）
    spin_unlock(&cache_lock);                     // 釋鎖
    return 0;                                     // 永遠回 0（連 -ENOMEM 都沒 —— 呼應 Bug #1）
}

/* ==============================================================
 *  cache_get —— 依 key 取出 value（RCU reader）
 * ============================================================== */

int cache_get(const char *key, int *out)
{
    struct cache_entry *e;                        // 走訪用
    u32 h = jhash(key, strlen(key), 0);           // 算 hash 值；jhash = Jenkins hash，kernel 內建

    rcu_read_lock();                              // 進入 RCU read-side critical section（禁止 schedule、但可睡的除外）
    hash_for_each_possible_rcu(cache, e, hnode, h) {  // RCU-safe 走訪同一 bucket；reader 可與 writer 並發
        if (strcmp(e->key, key) == 0) {           // 比對 key（完整字串比對）
            *out = e->value;                      // 命中：把 value 拷回 caller（注意 e 在 unlock 後就不能再用）
            rcu_read_unlock();                    // 退出 RCU read-side
            return 0;                             // 成功
        }
    }
    rcu_read_unlock();                            // 未命中也要退出 RCU read-side
    return -ENOENT;                               // 回傳標準錯誤碼
}

/* ==============================================================
 *  cache_remove —— 依 key 刪除 entry
 * ============================================================== */

void cache_remove(const char *key)
{
    struct cache_entry *e;                        // 走訪用
    u32 h = jhash(key, strlen(key), 0);           // 算 hash 值

    spin_lock(&cache_lock);                       // 進入 writer critical section
    hash_for_each_possible(cache, e, hnode, h) {  // 🔴 問題 5：非 *_safe 版本；刪了 e 之後 e->next 已被毒化，若再進下一輪 iteration 就 UAF（雖然 key 唯一時通常不會遇到，但語意危險）
        if (strcmp(e->key, key) == 0) {
            hash_del(&e->hnode);                  // 🔴 問題 4（之一）：hash_del 非 RCU 版本；publish/unpublish 語意錯誤，reader 可能看不到新鏈結狀態
            kfree(e);                             // 🔴 問題 4（之二）：立刻 kfree —— 此刻其他 CPU 的 cache_get 可能還持有 e 的指標（處於 rcu_read_lock 區段），立即 UAF
            /* 🔴 問題 5：沒 break；就算 key 唯一，同一條 chain 上繼續走訪並重新讀 e（已 free）也是地雷 */
        }
    }
    spin_unlock(&cache_lock);                     // 釋鎖
}
```

### 面試官備忘整理（對應修正清單）

| # | 位置 | 問題 | 修正方向 |
|---|------|------|---------|
| 1 | `cache_insert` | `kmalloc` 未檢查 NULL | `if (!e) return -ENOMEM;` |
| 2 | `cache_insert` | `strcpy` 沒邊界檢查 | 改 `strscpy(e->key, key, sizeof(e->key))`，並判斷回傳值 |
| 3 | `cache_insert` | `GFP_ATOMIC` 過度保守 | 依 caller context 選；可睡就用 `GFP_KERNEL`，並在 kernel-doc 寫清楚 |
| 4 | `cache_remove` | `hash_del` + 立即 `kfree` 與 RCU reader race | 用 `hash_del_rcu()` + `kfree_rcu(e, rcu)`（需在 struct 內加 `rcu_head`） |
| 5 | `cache_remove` | 找到後不 `break`、`hash_for_each_possible` 刪邊走 | 加 `break` 或改 `hash_for_each_possible_safe` |
| 6 | `cache_insert` | 允許重複 key | 先 lookup，有則以 RCU replace（`hlist_replace_rcu` + `kfree_rcu`）或直接拒絕 |

---

## 二、修正後代碼（逐行繁體中文註解）

```c
#include <linux/slab.h>            // kmalloc / kfree / kfree_rcu
#include <linux/hashtable.h>       // DEFINE_HASHTABLE / hash_add_rcu / hash_del_rcu / hash_for_each_possible_rcu
#include <linux/spinlock.h>        // spinlock_t / DEFINE_SPINLOCK
#include <linux/rcupdate.h>        // rcu_read_lock / rcu_read_unlock / rcu_assign_pointer / synchronize_rcu
#include <linux/jhash.h>           // jhash
#include <linux/string.h>          // strscpy / strcmp
#include <linux/err.h>             // -ENOMEM / -ENOENT / -EINVAL / -E2BIG

/* ----------- 資料結構定義（修正版） ----------- */

struct cache_entry {                              // cache 條目
    char key[32];                                 // 固定 key buffer；之後由 strscpy 保證 '\0' 終止
    int value;                                    // 對應 value
    struct hlist_node hnode;                      // hash chain 節點
    struct rcu_head rcu;                          // ✅ 修正 #4：RCU callback head，讓 kfree_rcu() 有地方掛 work
};

/* 全域表與鎖 */
static DEFINE_HASHTABLE(cache, 10);               // 1024 buckets
static DEFINE_SPINLOCK(cache_lock);               // writer 之間的互斥；reader 走 RCU 不需要鎖

/* ==============================================================
 *  __cache_find_locked —— 在持鎖狀態下查 key（writer 用）
 *
 *  僅供 writer 在持 cache_lock 時呼叫。
 *  回傳：命中則回 entry 指標；未命中回 NULL。
 * ============================================================== */

static struct cache_entry *
__cache_find_locked(const char *key, u32 h)
{
    struct cache_entry *e;                        // 走訪游標

    /* 注意：持鎖時用非 RCU 版本 hash_for_each_possible 也可以，
     * 但為了與 reader 一致、且不排斥 RCU reader，仍用 _rcu 變體最安全。 */
    hash_for_each_possible(cache, e, hnode, h) {  // 走訪同 bucket（持鎖期間鏈表穩定）
        if (strcmp(e->key, key) == 0)             // 精確比對
            return e;                             // 命中
    }
    return NULL;                                  // 未命中
}

/* ==============================================================
 *  cache_insert —— 新增或替換 key → value 映射（修正版）
 *
 *  Context: process context, may sleep (GFP_KERNEL).     // ✅ 修正 #3：文件化 context
 *  Return : 0 on success.
 *           -ENOMEM 若分配失敗。                          // ✅ 修正 #1
 *           -E2BIG  若 key 超過 31 字元。                 // ✅ 修正 #2
 * ============================================================== */

int cache_insert(const char *key, int val)
{
    struct cache_entry *e, *old;                  // e：新 entry；old：若已存在的同 key 舊 entry
    u32 h;                                        // hash 值
    ssize_t len;                                  // strscpy 回傳值

    might_sleep();                                // ✅ 修正 #3：明示可睡；lockdep / CONFIG_DEBUG_ATOMIC_SLEEP 會幫忙抓 context 錯用

    e = kmalloc(sizeof(*e), GFP_KERNEL);          // ✅ 修正 #3：process context 用 GFP_KERNEL，不吃 emergency pool
    if (!e)                                       // ✅ 修正 #1：必檢查 kmalloc 回傳
        return -ENOMEM;

    len = strscpy(e->key, key, sizeof(e->key));   // ✅ 修正 #2：有邊界檢查、保證 '\0' 終止；回傳複製長度或 -E2BIG
    if (len < 0) {                                // key 超長
        kfree(e);                                 // 記得回收剛配置的記憶體
        return -E2BIG;                            // 用 kernel 標準錯誤碼通知 caller
    }
    e->value = val;                               // 寫入值
    INIT_HLIST_NODE(&e->hnode);                   // 保險：初始化 hlist 節點指標（方便除錯 POISON 檢查）

    h = jhash(e->key, len, 0);                    // ✅ 用剛算好的 len，不必再 strlen 一次

    spin_lock(&cache_lock);                       // 進入 writer critical section；reader 仍可並行
    old = __cache_find_locked(e->key, h);         // ✅ 修正 #6：先看看是否已有相同 key
    if (old) {
        /* 已存在同 key：做 RCU-safe 替換，避免兩個同 key entry 並存 */
        hlist_replace_rcu(&old->hnode, &e->hnode); // 原子性地把 old 換成 e，reader 會看到其中之一
        spin_unlock(&cache_lock);                 // 先釋鎖再 kfree_rcu（kfree_rcu 本身可在鎖內，但縮短 critical section 較好）
        kfree_rcu(old, rcu);                      // ✅ 修正 #4：等所有 pre-existing RCU reader 結束才真正 free
        return 0;
    }

    hash_add_rcu(cache, &e->hnode, h);            // ✅ 新 entry：用 _rcu 版本 publish（內含 rcu_assign_pointer 語意）
    spin_unlock(&cache_lock);
    return 0;
}

/* ==============================================================
 *  cache_get —— 依 key 取 value（RCU reader，無鎖、無睡眠）
 *
 *  Context: any context（含 atomic、softirq、hard IRQ）。
 *           rcu_read_lock() 本身是 preempt-disable 等級。
 * ============================================================== */

int cache_get(const char *key, int *out)
{
    struct cache_entry *e;                        // 走訪游標
    u32 h;                                        // hash 值
    size_t klen;                                  // key 長度

    if (!key || !out)                             // 防呆：呼叫端傳 NULL
        return -EINVAL;

    klen = strnlen(key, sizeof_field(struct cache_entry, key)); // 只掃最多 32 bytes，避免 pathological input
    if (klen == sizeof_field(struct cache_entry, key))          // 若剛好 32 代表沒看到 '\0'
        return -E2BIG;

    h = jhash(key, klen, 0);                      // 計算 hash

    rcu_read_lock();                              // 進入 RCU read-side；此後不可 schedule
    hash_for_each_possible_rcu(cache, e, hnode, h) {  // RCU-safe 走訪同 bucket
        if (strcmp(e->key, key) == 0) {           // 命中
            *out = e->value;                      // 把 value 拷到 caller 空間（e 在 unlock 後即可能被回收）
            rcu_read_unlock();                    // 退出 RCU read-side
            return 0;
        }
    }
    rcu_read_unlock();                            // 未命中也要退出
    return -ENOENT;                               // 沒找到
}

/* ==============================================================
 *  cache_remove —— 依 key 刪除 entry（修正版）
 *
 *  Context: process context（kfree_rcu 本身非睡眠，但 spin_lock 已足夠保護 writer 互斥）。
 * ============================================================== */

int cache_remove(const char *key)                 // ✅ 讓回傳值有意義：0 / -ENOENT
{
    struct cache_entry *e;                        // 被找到的 entry
    u32 h;                                        // hash 值
    size_t klen;                                  // key 長度
    int ret = -ENOENT;                            // 預設：找不到

    if (!key)
        return -EINVAL;

    klen = strnlen(key, sizeof_field(struct cache_entry, key));
    if (klen == sizeof_field(struct cache_entry, key))
        return -E2BIG;

    h = jhash(key, klen, 0);                      // 算 hash

    spin_lock(&cache_lock);                       // writer 互斥
    e = __cache_find_locked(key, h);              // ✅ 修正 #5：抽出單次查找，找到就一次處理完，自然不會 race 下一輪 iteration
    if (e) {
        hash_del_rcu(&e->hnode);                  // ✅ 修正 #4（之一）：用 _rcu 版本 unpublish（rcu_assign_pointer 語意 + memory barrier）
        ret = 0;                                  // 標記成功
    }
    spin_unlock(&cache_lock);                     // 先釋鎖

    if (e)
        kfree_rcu(e, rcu);                        // ✅ 修正 #4（之二）：等所有 pre-existing RCU reader 結束才真正 free；鎖外呼叫更乾淨
    return ret;                                   // 0 或 -ENOENT
}

/* ==============================================================
 *  使用範例：呼叫端正確處理所有錯誤碼
 * ============================================================== */

static void caller_example(void)
{
    int v;
    int ret;

    ret = cache_insert("alice", 42);              // 可能：0 / -ENOMEM / -E2BIG / -EINVAL
    if (ret)
        pr_warn("insert failed: %d\n", ret);

    ret = cache_get("alice", &v);                 // RCU reader；any context 都可呼叫
    if (ret == 0)
        pr_info("alice = %d\n", v);
    else if (ret == -ENOENT)
        pr_debug("not found\n");

    ret = cache_remove("alice");                  // 0 / -ENOENT / -EINVAL / -E2BIG
    if (ret && ret != -ENOENT)
        pr_warn("remove failed: %d\n", ret);
}
```

---

## 三、重點整理（背誦版）

1. **kmalloc 必檢查 NULL**；失敗回 `-ENOMEM`；配置失敗後若已有其他分配，記得 `kfree` 再 return。
2. **kernel 字串拷貝用 `strscpy`**（不用 `strcpy` / `strncpy`）；`strscpy` 會回傳複製長度或 `-E2BIG`，又保證 `'\0'` 終止。
3. **`GFP_KERNEL` vs `GFP_ATOMIC`** 依 context 選：process context + 可睡 → `GFP_KERNEL`；atomic context / IRQ / spinlock 內 → `GFP_ATOMIC`（但 pool 有限、失敗率高）。kernel-doc 標註 context 並用 `might_sleep()` 讓 lockdep 幫忙抓。
4. **hash-table + RCU** 的寫法鐵律：
   - 讀：`rcu_read_lock()` + `hash_for_each_possible_rcu()`，unlock 後不可再碰指標。
   - 寫：`spin_lock()` 互斥 → `hash_add_rcu()` / `hash_del_rcu()` / `hlist_replace_rcu()` → 釋鎖 → `kfree_rcu(e, rcu)`。
   - 所有 RCU-protected 物件的 struct 內都應帶 `struct rcu_head`。
5. **絕對不要 `hash_del` + 立即 `kfree`**；一定要走 `hash_del_rcu` + `kfree_rcu` 或 `synchronize_rcu()` 後 `kfree`。
6. **邊走邊刪用 `*_safe`**；但更好的作法是先 lookup 找到單一目標，再單點處理，避免 iterator 語意問題。
7. **寫入端先查再插**：不檢查重複 key 等於留下永久記憶體洩漏；用 `hlist_replace_rcu` 做原子替換。

> 小技巧：review hash-table 代碼時，腦中跑三個問題：
> 1. **寫者互斥**夠不夠？（spin_lock 還是 mutex、有沒有遺漏 critical section）
> 2. **讀者並發**安全嗎？（是否用 `_rcu` 走訪、free 是否走 `kfree_rcu`）
> 3. **entry 生命週期**封閉嗎？（alloc 失敗、重複 key、找不到、超長 key 都要處理，且每個錯誤路徑都 free 該 free 的東西）

---

## 四、相關 wiki

- `[[interview/Mon-3-code-review-solution]]` — list_head + spinlock 版本的 code review 解答（refcount / ERR_PTR / list_for_each_safe）
- `[[kernel/spinlock]]` — 本解答所用 spinlock 變體與地雷清單
- `[[kernel/hash-table]]` — `hlist` / `hashtable.h` / `rhashtable` 的內部設計
- `[[interview/code-review-questions]]` — 主管視角 code review 清單（RCU、GFP、kfree_rcu 相關章節）
- `[[interview/weekly-mock-bank]]` — 本題出處（Tue-3）
