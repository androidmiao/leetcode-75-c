# Mon-3. Code Review Round — 解答檔

> 來源：`weekly-mock-bank.md` 第 120–178 行的 **Mon-3. Code Review Round（30 min）**
>
> 本檔案：
> 1. 將原始 kernel-style 代碼 **逐行加上繁體中文註解**，清楚標示每一行的用途與「地雷點」。
> 2. 依照面試官備忘提出的 **6 個問題**，給出修正後的完整版本，同樣逐行加上繁體中文註解。
>
> 目標讀者：Android/Linux kernel 面試自我演練用。

---

## 一、原始代碼（逐行繁體中文註解＋問題標註）

下列代碼每一行後方以 `//` 或獨立註解解說用途；其中有地雷的行會標上 **🔴 問題 N**，對應第二節的修正。

```c
/* ----------- 資料結構定義 ----------- */

struct my_obj {                           // 定義使用者自訂物件 my_obj
    int val;                              // 物件內含的整數資料欄位
    struct list_head node;                // kernel 內建雙向鏈表節點（嵌入式設計，非指標）
};                                        // 注意：沒有 refcount_t，這是後面 Bug#5 的根源

/* ==============================================================
 *  函式 1：add_object —— 分配一個新物件並掛進鏈表
 * ============================================================== */

int add_object(struct list_head *head, int val)                 // 傳入鏈表頭指標與要填入的整數值
{
    struct my_obj *obj = kmalloc(sizeof(struct my_obj),         // 在 kernel heap 上分配一份 my_obj
                                 GFP_KERNEL);                   // 🔴 問題 2：GFP_KERNEL 允許睡眠，呼叫端若持有 spinlock/atomic context 會出事；函式卻沒用註解或 might_sleep() 告知 caller
    obj->val = val;                                             // 🔴 問題 1：沒檢查 kmalloc 是否回傳 NULL，OOM 時會立即 NULL pointer dereference → kernel oops
    list_add(&obj->node, head);                                 // 將節點掛到鏈表 head 後方（O(1) 頭插）；如果 obj 為 NULL 這行也會爆炸
    return 0;                                                   // 永遠回傳 0 — 即使失敗也沒回傳 -ENOMEM，呼叫端無法辨識錯誤
}

/* ==============================================================
 *  函式 2：delete_all —— 走訪整條鏈表並釋放每個節點
 * ============================================================== */

void delete_all(struct list_head *head)                         // 傳入鏈表頭指標
{
    struct list_head *pos;                                      // 迭代游標
    struct my_obj *obj;                                         // 透過 container_of 反查出的使用者物件

    list_for_each(pos, head) {                                  // 🔴 問題 3：list_for_each 無法安全刪除！迴圈會以 pos = pos->next 前進，但 pos 已被 list_del + kfree，即 use-after-free
        obj = container_of(pos, struct my_obj, node);           // 由節點指標反推出 my_obj 起始位址（零成本的 offsetof 運算）
        list_del(pos);                                          // 從鏈表中移除 pos（會把 pos->next 毒化成 LIST_POISON1/2）
        kfree(obj);                                             // 釋放記憶體 — 此時 pos 已經是 dangling pointer
    }                                                           //  迴圈 header 接著讀 pos->next，觸發 UAF
}

/* ==============================================================
 *  函式 3：find —— 依照 val 尋找鏈表中的物件
 * ============================================================== */

struct my_obj *find(struct list_head *head, int val)            // 在鏈表中尋找 val 相符的第一個節點
{
    struct my_obj *obj;                                         // 迭代用的使用者物件指標
    struct list_head *pos;                                      // 迭代游標

    spin_lock(&g_lock);                                         // 取得全域 spinlock（此刻進入 atomic context，不可睡眠）
    list_for_each(pos, head) {                                  // 這裡用 list_for_each 可以（沒刪節點）；但與鎖一起看要小心睡眠
        obj = container_of(pos, struct my_obj, node);           // 還原出使用者物件
        if (obj->val == val) {                                  // 命中：值相等
            kmalloc(32, GFP_KERNEL);                            // 🔴 問題 4：持 spinlock 時呼叫 GFP_KERNEL 會睡眠，違反「spinlock 內不可睡」；且這次 malloc 回傳值沒存 → 純粹的記憶體洩漏
            spin_unlock(&g_lock);                               // 釋放 spinlock
            return obj;                                         // 🔴 問題 5：回傳裸指標，未 refcount_inc；另一條執行緒若同時 delete_all()，caller 拿到的就是 dangling → UAF
        }
    }
    spin_unlock(&g_lock);                                       // 未命中也要釋鎖
    return NULL;                                                // 🔴 問題 6：NULL 無法區分「查不到」與「分配失敗」；caller 無法正確處理錯誤
}
```

### 面試官備忘整理（對應修正清單）

| # | 位置 | 問題 | 修正方向 |
|---|------|------|---------|
| 1 | `add_object` | `kmalloc` 未檢查 NULL | `if (!obj) return -ENOMEM;` |
| 2 | `add_object` | 未標註可睡眠上下文 | 加 `might_sleep()` 或 kernel-doc 註解 |
| 3 | `delete_all` | `list_for_each` 對自己刪 UAF | 改 `list_for_each_safe(pos, n, head)` |
| 4 | `find` | 持 spinlock 時 `GFP_KERNEL` 會睡 | 改 `GFP_ATOMIC` 或移出 critical section；並接住回傳值 |
| 5 | `find` | 回傳裸指標缺 refcount | 加 `refcount_t` 與 `refcount_inc_not_zero()` |
| 6 | `find` | 用 `NULL` 混淆錯誤碼 | 改回 `ERR_PTR(-ENOENT)` / `ERR_PTR(-ENOMEM)`，caller 用 `IS_ERR()` |

---

## 二、修正後代碼（逐行繁體中文註解）

下列版本修掉全部 6 個問題；為了讓範例自洽，補上 refcount、release 函式、以及一個 safe-find 的 RCU 輕量替代方案註解。

```c
#include <linux/slab.h>          // kmalloc / kfree
#include <linux/list.h>          // list_head / list_add / list_for_each_safe
#include <linux/spinlock.h>      // spin_lock / spin_unlock
#include <linux/refcount.h>      // refcount_t / refcount_inc_not_zero
#include <linux/err.h>           // ERR_PTR / PTR_ERR / IS_ERR
#include <linux/errno.h>         // -ENOMEM / -ENOENT
#include <linux/kernel.h>        // might_sleep / container_of

/* ----------- 資料結構定義（修正版） ----------- */

struct my_obj {                                   // 使用者物件
    int val;                                      // 資料欄位
    refcount_t ref;                               // ✅ 修正 #5：加入 refcount 以安全共享指標
    struct list_head node;                        // 嵌入式鏈表節點
};

/* 全域鎖：保護鏈表串接 */
static DEFINE_SPINLOCK(g_lock);                   // 檔案內靜態 spinlock；編譯期就初始化好

/* ==============================================================
 *  my_obj_release —— refcount 降為 0 後真正釋放
 *  必須在「已確定無人持鎖、無人引用」時呼叫
 * ============================================================== */

static void my_obj_release(struct my_obj *obj)    // 釋放單一物件的 helper
{
    kfree(obj);                                   // 將配置時的記憶體還給 slab
}

/* put helper：API 使用者拿到指標後，用完呼叫這個 */
static inline void my_obj_put(struct my_obj *obj) // 減少引用計數；為 0 時釋放
{
    if (obj && refcount_dec_and_test(&obj->ref))  // 原子地 --ref，若剛好歸零才進入 if
        my_obj_release(obj);                      // 真正 kfree
}

/* ==============================================================
 *  add_object —— 分配一個新物件並掛進鏈表（修正版）
 *
 *  Context: process context, may sleep (GFP_KERNEL).            // ✅ 修正 #2：kernel-doc 風格文件標註可睡
 *  Return:  0 on success, -ENOMEM on allocation failure.
 * ============================================================== */

int add_object(struct list_head *head, int val)   // 傳入鏈表頭與要填的值
{
    struct my_obj *obj;                           // 先宣告，分配後再賦值，方便檢查
    unsigned long flags;                          // 用於 spin_lock_irqsave（避免與 IRQ 衝突）

    might_sleep();                                // ✅ 修正 #2：呼叫者若在 atomic context lockdep/CONFIG_DEBUG_ATOMIC_SLEEP 會直接抓出

    obj = kmalloc(sizeof(*obj), GFP_KERNEL);      // 分配物件（可睡，因為已明確宣告可睡上下文）
    if (!obj)                                     // ✅ 修正 #1：一定要檢查 kmalloc 回傳值
        return -ENOMEM;                           // 失敗回傳 kernel 標準錯誤碼，讓 caller 可辨識

    obj->val = val;                               // 填入資料欄位
    refcount_set(&obj->ref, 1);                   // ✅ 修正 #5：初始引用計數為 1（表示鏈表自己持有一份）
    INIT_LIST_HEAD(&obj->node);                   // 初始化節點（保險動作，便於除錯時 POISON 檢查）

    spin_lock_irqsave(&g_lock, flags);            // 進入 critical section；保存 IRQ 狀態
    list_add(&obj->node, head);                   // 頭插到鏈表（O(1)）
    spin_unlock_irqrestore(&g_lock, flags);       // 釋鎖並還原 IRQ 狀態

    return 0;                                     // 成功
}

/* ==============================================================
 *  delete_all —— 走訪整條鏈表並釋放每個節點（修正版）
 *
 *  Context: process context, may sleep.
 * ============================================================== */

void delete_all(struct list_head *head)           // 清空整條鏈表
{
    struct list_head *pos, *n;                    // ✅ 修正 #3：需要兩個游標給 *_safe 使用
    struct my_obj *obj;                           // 透過 container_of 反查
    unsigned long flags;

    might_sleep();                                // 明確標示可睡上下文

    spin_lock_irqsave(&g_lock, flags);            // 保護鏈表結構變更
    list_for_each_safe(pos, n, head) {            // ✅ 修正 #3：先把 pos->next 存到 n，再 kfree(pos) 就不會 UAF
        obj = container_of(pos, struct my_obj, node); // 反查使用者物件
        list_del(pos);                            // 從鏈表中摘下（pos 的 prev/next 會被毒化）
        spin_unlock_irqrestore(&g_lock, flags);   // 短暫釋鎖，讓 put 的 release path 不卡在鎖內
        my_obj_put(obj);                          // ✅ 修正 #5：放掉「鏈表自己持有」那份 refcount；若沒別人引用就立即釋放
        spin_lock_irqsave(&g_lock, flags);        // 重新上鎖繼續走訪（n 仍然有效，因為 n 不是被刪的那個）
    }
    spin_unlock_irqrestore(&g_lock, flags);       // 全部走完釋鎖
}

/* ==============================================================
 *  find —— 尋找 val 相符的物件（修正版）
 *
 *  Context: any context (spin_lock only, no sleep).
 *  Return:  valid pointer with a refcount held, or
 *           ERR_PTR(-ENOENT) if not found.
 *  Caller:  must call my_obj_put() when done.                   // ✅ 修正 #5：API 契約寫清楚
 * ============================================================== */

struct my_obj *find(struct list_head *head, int val)
{
    struct my_obj *obj;                           // 迭代用
    struct list_head *pos;                        // 游標
    struct my_obj *ret = ERR_PTR(-ENOENT);        // ✅ 修正 #6：用 ERR_PTR 代替 NULL，caller 用 IS_ERR 判斷
    unsigned long flags;                          // IRQ 安全

    spin_lock_irqsave(&g_lock, flags);            // 進入 critical section（atomic context，下面不可睡）
    list_for_each(pos, head) {                    // 單純走訪（沒刪節點，安全）
        obj = container_of(pos, struct my_obj, node); // 還原物件
        if (obj->val == val) {                    // 命中
            if (refcount_inc_not_zero(&obj->ref)) // ✅ 修正 #5：原子地 +1；若物件已在被釋放途中（ref==0）回 false
                ret = obj;                        // 成功，帶著新的一份 refcount 交給 caller
            else
                ret = ERR_PTR(-ENOENT);           // 競態中被刪掉，視為找不到
            break;                                // ✅ 修正 #4：整段 critical section 已不再呼叫 kmalloc，不會睡眠
        }
    }
    spin_unlock_irqrestore(&g_lock, flags);       // 釋鎖

    /*
     * 若原題「複製一份 metadata」確實有業務需求，放到這裡：
     * 此時已離開 spinlock，可安全使用 GFP_KERNEL；
     * 並且務必接住回傳值、失敗時 put 掉已加的 refcount。
     */
    if (!IS_ERR(ret)) {                           // 只有真的找到才走這段
        void *meta = kmalloc(32, GFP_KERNEL);     // ✅ 修正 #4：移出鎖外；允許睡眠
        if (!meta) {                              // ✅ 修正 #4：檢查回傳值，避免洩漏
            my_obj_put(ret);                      // 回吐前面加的 refcount
            return ERR_PTR(-ENOMEM);              // ✅ 修正 #6：用 ERR_PTR 明確區分於 -ENOENT
        }
        /* ... 對 meta 做事 ... */
        kfree(meta);                              // demo 目的：用完即釋放，實務上依需求保留
    }

    return ret;                                   // 可能是有效指標，或 ERR_PTR(-ENOENT)
}

/* ==============================================================
 *  使用範例：呼叫端正確處理三種結果
 * ============================================================== */

static void caller_example(struct list_head *head)
{
    struct my_obj *o = find(head, 42);            // 嘗試尋找值為 42 的物件
    if (IS_ERR(o)) {                              // ✅ 修正 #6：先判斷錯誤指標
        if (PTR_ERR(o) == -ENOENT)                // 找不到
            pr_debug("not found\n");
        else if (PTR_ERR(o) == -ENOMEM)           // 記憶體不足
            pr_warn("oom\n");
        return;                                   // 錯誤路徑：不需要 put
    }

    /* o 為有效指標，且我們持有 1 份 refcount */
    pr_info("got val=%d\n", o->val);              // 安全使用
    my_obj_put(o);                                // ✅ 修正 #5：用完一定要 put
}
```

---

## 三、重點整理（背誦版）

1. **kmalloc 必檢查 NULL**；失敗回 `-ENOMEM`。
2. **可睡上下文** (`GFP_KERNEL`) 要用 `might_sleep()` 或 kernel-doc 明示 → lockdep 會幫你抓。
3. **`list_for_each` 不能邊走邊刪**；刪除必用 `list_for_each_safe` (+ 額外游標 `n`)。
4. **spinlock 內禁止睡眠**；若真的要分配，用 `GFP_ATOMIC` 或 **先釋鎖再分配**。任何 malloc 都要接住回傳值，否則洩漏。
5. **分享出去的指標一定要配 refcount**；`refcount_inc_not_zero` 搭配 RCU / 鎖，確保 put/free 的競態安全。
6. **錯誤回傳用 `ERR_PTR` 家族**，讓 caller 可用 `IS_ERR` / `PTR_ERR` 區分錯誤種類，不要把 `NULL` 當萬用訊號。

> 小技巧：code review 現場，先**從記憶體生命週期**（alloc/free/ref）掃一輪，再從 **鎖與上下文**（sleep/IRQ）掃第二輪，最後從 **API 契約**（回傳值、錯誤碼、文件）掃第三輪。三輪下來通常能把 90% kernel-style bug 撈乾淨。
