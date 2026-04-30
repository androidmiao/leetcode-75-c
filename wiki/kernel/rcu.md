# RCU 讀-複製-更新

## 核心概念

RCU (Read-Copy-Update) 是 Linux kernel 中**讀者完全零成本**的同步機制——讀者不取鎖、不寫共享記憶體、不被寫者阻塞；寫者用「複製→修改→原子替換指標→延後釋放舊版本」的方式更新。代價是寫端比較貴、釋放被延後到所有讀者離開後才能進行。

### 一句話判斷

> 讀遠多於寫、結構由指標串起來、讀路徑要極快且不能睡眠 → 選 RCU。其他情況優先 spinlock / mutex。

---

## 為什麼 spinlock / rwlock 不夠

| 場景 | spinlock | rwlock | RCU |
|------|----------|--------|-----|
| 讀者開銷 | 取鎖 + cache line bouncing | 同左（atomic inc reader count） | **零**（只是普通 deref） |
| 讀者間延展性 | 寫者進來會阻塞所有讀者 | 寫者飢餓 / 讀者飢餓擇一 | 讀者完全不阻塞 |
| 寫者開銷 | 中 | 中 | 高（要複製 + 等 grace period） |
| 寫者頻率假設 | 任意 | 任意 | **必須遠少於讀** |

幾乎所有 kernel 子系統的「查表」路徑（dcache、路由表、SELinux AVC、tracepoint list、module list、network namespaces）都從 rwlock 改用 RCU 之後，throughput 與 tail latency 都顯著改善。

---

## 三層核心 API

### 1. Read-side：完全零鎖

```c
rcu_read_lock();                                  // 標記進入 RCU read-side critical section
                                                  //   實作：preempt_disable()（classic RCU）
                                                  //   或 only marker（preemptible RCU）
                                                  //   不會阻塞寫者
struct foo *p = rcu_dereference(global_p);        // 讀取受 RCU 保護的指標
                                                  //   = READ_ONCE + memory_order_consume
if (p)
    use(p->field);                                // 在 critical section 內 p 保證有效
rcu_read_unlock();                                // 離開 critical section
                                                  //   不可在這之前 schedule()
```

`rcu_read_lock()` / `rcu_read_unlock()` 配對的區段內**不可睡眠**，所以禁止 `kmalloc(GFP_KERNEL)`、`mutex_lock`、`copy_from_user` 等任何可能 schedule 的呼叫。需要可睡眠的 RCU 請看下方 SRCU。

### 2. Update-side：複製 + 替換 + 排隊釋放

```c
mutex_lock(&update_lock);                         // 寫者間互斥（RCU 不負責 writer-writer 同步）

struct foo *old = rcu_dereference_protected(global_p, lockdep_is_held(&update_lock));
struct foo *new = kmalloc(sizeof(*new), GFP_KERNEL);
*new = *old;                                      // copy
new->field = updated_value;                       // modify

rcu_assign_pointer(global_p, new);                // 原子發佈新版（含 release barrier）

mutex_unlock(&update_lock);

call_rcu(&old->rcu, foo_free_cb);                 // 排隊「等所有目前 read-side 都離開後再釋放 old」
                                                  //   或 synchronize_rcu() 同步等（會睡眠，禁止在 atomic）
```

### 3. Grace period：何時可以真的釋放

`call_rcu()` 註冊的 callback 不是立刻執行，而是等到一個 **grace period** 經過——也就是「所有目前正在跑的 read-side critical section 都已經離開」之後。

實作方式（classic RCU）：grace period 結束 ⇔ 系統中每顆 CPU 都至少做過一次 context switch 或 idle 過。因為 read-side 內 preempt 是關掉的，只要 CPU 換過 context，就代表那顆 CPU 已經不在任何 read-side 裡。

---

## list / hlist 的 RCU 變體

```c
struct foo {
    struct list_head node;
    int               key;
    char              data[64];
};

/* Reader */
rcu_read_lock();
list_for_each_entry_rcu(p, &my_list, node) {       // _rcu 版會用 rcu_dereference 讀 next
    if (p->key == target) { use(p); break; }
}
rcu_read_unlock();

/* Writer (mutex 已經拿好) */
list_add_rcu(&new->node, &my_list);                 // 含 release barrier 的 publish
list_del_rcu(&old->node);                           // unlink，但不釋放
synchronize_rcu();                                  // 等讀者離開
kfree(old);                                         // 才能真的 free
```

`list_replace_rcu` / `hlist_replace_rcu` 還能做到「讀者要嘛看到完整舊版，要嘛看到完整新版」，永遠不會看到半新半舊。這是 dcache rename、路由表 update 的核心慣例。

---

## SRCU / Tasks RCU / Tree RCU 變體

| 變體 | 讀者可睡 | 寫端 grace period 成本 | 場景 |
|------|---------|----------------------|------|
| **classic / Tree RCU** | 否 | 中 | 大宗：list、hash、route table |
| **SRCU** (sleepable) | **是** | 比 classic 高 | KVM MMU notifier、tracepoint probe、`copy_from_user` 在 read-side |
| **Tasks RCU** | 是（user mode 也算） | 高 | trampoline 卸除、ftrace |
| **RCU-bh** | 否 | 比 classic 短 | 已棄用（被 unification 吸收）|
| **RCU-sched** | 否 | — | 同上，已 unify |

選擇規則：**能用 classic RCU 就用 classic**，只在 read-side 真的需要睡眠才升級到 SRCU。

---

## 三個 idiomatic 用法

### 用法 1：發佈一個新物件

```c
new = kmalloc(...);
fill(new);
rcu_assign_pointer(global, new);                  // 完成發佈
```

`rcu_assign_pointer` 內含 `smp_store_release`，保證 `fill()` 的所有 store 在 publish 之前完成、其他 CPU 不會看到「指標已換、欄位還沒填」的中間狀態。

### 用法 2：替換鏈表中一個節點

```c
new = kmalloc(...);
*new = *old;                                      // copy
new->v = updated;                                  // modify
list_replace_rcu(&old->node, &new->node);         // atomic swap
synchronize_rcu();                                 // wait readers
kfree(old);
```

### 用法 3：lock-free lookup + 寫端護欄

```c
/* lookup hot path */
rcu_read_lock();
hash_for_each_possible_rcu(table, p, hnode, key) {
    if (p->key == key) { found = p; break; }
}
rcu_read_unlock();

/* update cold path */
spin_lock(&update_lock);
hlist_add_head_rcu(&new->hnode, &table[hash(key)]);
spin_unlock(&update_lock);
```

讀端不取任何鎖，寫端用普通 spinlock 互斥彼此。Linux 的網路 socket hash、ARP cache、connection track 都用這個模式。

---

## 三大陷阱

### 陷阱 1：read-side 內睡眠

```c
rcu_read_lock();
p = rcu_dereference(global);
copy_to_user(buf, p->data, p->len);                // ❌ 可能 schedule，破壞 grace period 假設
rcu_read_unlock();
```

修正：抓出需要的欄位後 unlock，再做 user 拷貝；或改用 SRCU。

### 陷阱 2：忘了 `rcu_assign_pointer` / `rcu_dereference`

直接 `global = new;` 會少 release barrier，弱記憶體模型 CPU（ARM、POWER）會把欄位 store 重排到指標 publish 之後，讀者看到 garbage。直接 `p = global;` 會少 dependency hint，編譯器可能 reorder load。**永遠用 paired API**。

### 陷阱 3：把 RCU 當 mutex

RCU 不保證 writer-writer 互斥，多個寫者必須自己另外 lock（典型用 mutex 或 spinlock）。RCU 也不適合用在「寫多讀少」或「需要 transactional 行為」的場景。

---

## 與其他同步原語的對比

| 原語 | 讀者 | 寫者 | 場景 |
|------|------|------|------|
| `spinlock_t` | 取鎖（busy-wait） | 取鎖 | atomic context、短臨界區 |
| `rwlock_t` | 取讀鎖（仍 atomic op） | 取寫鎖 | 已大半被 RCU 取代 |
| `rw_semaphore` | 取讀信號量（可睡） | 同 | 長 critical section、讀多寫少（如 mmap_lock）|
| `seqlock` | 讀+retry，零鎖 | spinlock | 寫罕見、讀允許 retry（jiffies）|
| **RCU** | **零鎖** | mutex / spinlock + grace period | 讀極多、結構由指標串起 |
| `srcu` | 零鎖且**可睡** | 同上 | KVM、tracepoint probe |

---

## 實戰：在 kernel 源碼中的定位

### 典型用例

- **`fs/dcache.c`** — dentry lookup 走 RCU-walk fast path，失敗才退回 ref-walk + spinlock。
- **`net/ipv4/route.c`** — FIB / route cache 完全 RCU 化，packet forwarding hot path 沒有任何鎖。
- **`security/selinux/avc.c`** — AVC (Access Vector Cache) 用 RCU 達到 lockless lookup。
- **`kernel/module.c`** — module list traversal 用 RCU；`modprobe` 罕見、`__symbol_get` 高頻。
- **`include/linux/rcupdate.h`** — 所有 API 入口；`include/linux/rculist.h` 是 list 版本。

### Lockdep / RCU debugging

開啟 `CONFIG_PROVE_RCU` + `CONFIG_DEBUG_OBJECTS_RCU_HEAD` 後：
- 在 read-side 外做 `rcu_dereference` 會 splat。
- `rcu_dereference_protected(p, cond)` 可以宣告「我握著 cond 這把寫鎖，所以可以不在 read-side」。
- `synchronize_rcu()` 在 atomic context 呼叫會 splat。
- `call_rcu` 的 head 被重複註冊也會抓出。

---

## 相關 wiki

- `[[kernel/spinlock]]` — RCU 的寫端通常需搭配 spinlock；對比參數見「與其他同步原語」一節
- `[[kernel/list-head]]` — `list_for_each_entry_rcu` / `list_add_rcu` / `list_replace_rcu` 是 RCU 的 list API
- `[[kernel/hash-table]]` — `hash_for_each_possible_rcu` 與 RCU-hashtable 模式
- `[[kernel/memory-management]]` — `call_rcu` 釋放路徑、`kfree_rcu()` 短路寫法
- `[[interview/google_code_review_semaphore_prep_linux_primitives]]` — 5 大原語選擇邏輯總圖中 RCU 的定位
- `[[interview/google_code_review_semaphore_prep_linux]]` — code review 題目中 reader-heavy 場景改 RCU 的建議
- `[[interview/code-review-questions]]` — 主管視角 review checklist 中 RCU 章節
- `[[interview/mcupm-code-review]]` — MCUPM driver Q5 (`mcupm_plt_ackdata` race) 的修法選項之一可改 SRCU

---

## 面試速記（背誦版）

1. RCU = **讀者零鎖** + 寫者 copy-modify-publish + 延後釋放。
2. read-side 不能睡，要睡用 SRCU。
3. 永遠用 `rcu_assign_pointer` / `rcu_dereference` 配對，不要赤裸 `=`。
4. 寫者間互斥要自己另外加 mutex / spinlock。
5. `synchronize_rcu()` 會睡（process context 才能用）；`call_rcu()` / `kfree_rcu()` 不會。
6. 適用：路由表、dcache、SELinux AVC、module list、tracepoint。
7. 不適用：寫多讀少、transactional、單變數（→ `atomic_t` / `refcount_t`）。
