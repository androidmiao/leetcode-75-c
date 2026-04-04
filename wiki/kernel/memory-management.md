# Memory Management 記憶體管理

## 核心概念

Linux kernel 的記憶體管理系統管理數十 GB 甚至數 TB 的實體記憶體，涉及複雜的資料結構（LRU、頁表、slab 分配器）。LeetCode 146（LRU 快取）簡化了生產級別設計的複雜度。

## 頁面回收：mm/vmscan.c, mm/swap.c

### 傳統 2 級 LRU

```c
struct zone {
  struct lruvec {
    struct list_head lists[NR_LRU_LISTS];
  } lruvec;
};

// LRU 列表類型
#define LRU_INACTIVE_ANON 0
#define LRU_ACTIVE_ANON   1
#define LRU_INACTIVE_FILE 2
#define LRU_ACTIVE_FILE   3
#define NR_LRU_LISTS      4
```

**頁面生命週期**

```
新頁（冷頁）
  ↓
INACTIVE_FILE/ANON（尚未訪問，準備回收）
  ↓ [訪問]
ACTIVE_FILE/ANON（熱頁，保護）
  ↓ [持續未訪問]
INACTIVE_FILE/ANON（冷卻）
  ↓
回收（交換或丟棄）
```

### 掃描策略

```c
// mm/vmscan.c
static unsigned long shrink_lruvec(struct lruvec *lruvec,
                                   struct scan_control *sc)
{
  unsigned long nr[NR_LRU_LISTS] = {0};

  // 分開掃描 ANON 和 FILE
  while (nr_scanned < scan_target) {
    // 1. 掃描 INACTIVE list
    nr_scanned += shrink_list(LRU_INACTIVE_FILE, ...);

    // 2. 如果仍需更多頁面，冷卻 ACTIVE list
    if (nr_scanned < scan_target && rotations < threshold) {
      move_active_to_inactive(...);
    }

    // 3. 掃描 ANON（交換成本更高，保守）
    if (swap_available && need_swap_pressure) {
      nr_scanned += shrink_list(LRU_ANON, ...);
    }
  }
}

// 實際回收
static int shrink_page_list(struct list_head *page_list)
{
  struct page *page;

  list_for_each_entry(page, page_list, lru) {
    if (!writepage_ok(page))
      continue;

    if (PageAnon(page)) {
      swapout(page);  // 寫入交換分區
    } else {
      writeback(page);  // 寫入檔案系統
    }

    __delete_from_page_cache(page);
  }
}
```

### 複雜性來源

**ANON vs FILE**
```
FILE：如果在磁碟上有備份，直接丟棄
ANON：匿名頁（棧、堆），必須交換出去
```

**工作集維持**
```
活躍集：應保留在記憶體中
不活躍集：可回收
邊界：隨負載動態變化
```

**記憶體壓力級別**
```c
enum zone_watermark_idx {
  WMARK_MIN,    // 最小值，觸發回收
  WMARK_LOW,    // 低位
  WMARK_HIGH,   // 高位，停止回收
};

// 回收觸發
if (zone_page_state(zone, NR_FREE_PAGES) < zone->watermark[WMARK_MIN]) {
  kswapd_wakeup();  // 喚醒回收守護
}
```

## Multi-Gen LRU（MGLRU）

### 傳統 2 級 LRU 的瓶頸

```
缺點 1：頻繁列表操作
  // 每次訪問都需移動頁面
  page_accessed(page) {
    list_del(&page->lru);
    list_add_tail(&page->lru, &active_list);
  }
  // 高頻次導致鎖爭用

缺點 2：不精確的年齡信息
  // 只知道「活躍」或「不活躍」，不知道相對年齡
  // 無法區別：最近訪問 vs 很久前訪問

缺點 3：同步回收
  // 單一掃描動作，導致延遲尖峰
```

### MGLRU 解決方案（Linux 5.15+）

```c
#define LRU_GENS 4  // 世代數

struct lru_gen_struct {
  unsigned long timestamp[LRU_GENS];
  struct list_head lists[LRU_GENS][NR_LRU_TYPES];
};

// 世代迴轉機制
void age_lru_gens(void) {
  for (int gen = LRU_GENS - 1; gen > 0; gen--) {
    lists[gen] = lists[gen - 1];  // 世代進階
  }
  lists[0] = empty;  // 新世代
}
```

**四世代模型**

```
Gen 0：最年輕（剛訪問）
Gen 1：次新
Gen 2：舊
Gen 3：最老（準備回收）

隨著時間進階：
時刻 T：G0(新) → G1 → G2 → G3(老) → 回收
時刻 T+Δ：新訪問進 G0，舊的 G0 升至 G1
```

### MGLRU 優勢

```c
// 優勢 1：減少列表操作
page_accessed(page) {
  // 僅更新 generation stamp，無列表移動
  page->gen = current_gen;
}

// 優勢 2：精確年齡感知
for (int gen = 3; gen >= 0; gen--) {
  if (gen != current_gen) {
    reclaim_pages(&lists[gen]);
  }
}

// 優勢 3：異步掃描
kthread_worker() {
  while (1) {
    sleep(AGE_INTERVAL);
    age_lru_gens();  // 定期推進世代
  }
}
```

### 複雜度對比

| 操作 | 2-LRU | MGLRU |
|------|-------|-------|
| 頁訪問 | O(1) 列表移動 | O(1) 戳記更新 |
| 鎖爭用 | 高（每訪問） | 低（批量推進） |
| 回收精度 | 低（2 級） | 高（4+ 級） |

## Slab 分配器：mm/slub.c

### 問題陳述

內核頻繁分配小物件（任務結構、inode、dentry），堆碎片化會浪費記憶體。

### Slab 概念

```
PAGE：4 KB（或更大）
  ↓
SLAB：PAGE 的集合，分割成定長對象
  ↓
OBJECT：實際分配給使用者的物件
```

### SLUB 實現（簡化版）

```c
struct kmem_cache {
  size_t object_size;
  size_t slab_size;
  struct list_head full_slabs;
  struct list_head partial_slabs;
  struct list_head empty_slabs;
  spinlock_t list_lock;
};

struct slab {
  struct page *page;
  void *freelist;  // 空閒物件棧
  unsigned long inuse;
};

void *kmem_cache_alloc(struct kmem_cache *s) {
  spinlock_lock(&s->list_lock);

  // 嘗試 partial slab
  if (!list_empty(&s->partial_slabs)) {
    slab = list_entry(s->partial_slabs.next, ...);
    obj = slab->freelist;
    slab->freelist = *(void **)obj;
    slab->inuse++;

    if (slab->inuse == slab->num_objects) {
      list_del(&slab->list);
      list_add(&slab->list, &s->full_slabs);
    }

    spinlock_unlock(&s->list_lock);
    return obj;
  }

  // 分配新 slab
  slab = alloc_slab(s);
  list_add(&slab->list, &s->partial_slabs);
  obj = slab->freelist;
  slab->freelist = *(void **)obj;
  slab->inuse = 1;

  spinlock_unlock(&s->list_lock);
  return obj;
}

void kmem_cache_free(struct kmem_cache *s, void *obj) {
  spinlock_lock(&s->list_lock);

  // 將物件放回空閒表
  slab = virt_to_slab(obj);
  *(void **)obj = slab->freelist;
  slab->freelist = obj;
  slab->inuse--;

  // 如果空了，可能銷毀
  if (slab->inuse == 0) {
    list_del(&slab->list);
    list_add(&slab->list, &s->empty_slabs);
    maybe_shrink();
  }

  spinlock_unlock(&s->list_lock);
}
```

### 空閒對象追蹤

```
SLAB 內記憶體佈局：
[對象 0][對象 1][對象 2][對象 3]
  ↓ 空閒時 ↓
  指向對象 2        指向對象 3
  ↓
  NULL

freelist 形成「空閒棧」，O(1) 分配和釋放
```

### 優勢

- **減少碎片**：定長物件避免外碎片
- **快速分配**：無複雜搜尋，O(1) 操作
- **快取感知**：同類物件聚集在 slab 中
- **資源管理**：可設定限制防止濫用

## 批處理：folio_batch

### 鎖爭用瓶頸

```c
// 低效：高頻鎖獲取釋放
for (int i = 0; i < n; i++) {
  spinlock_lock(&zone_lock);
  reclaim_page(page[i]);
  spinlock_unlock(&zone_lock);  // 高開銷！
}
```

### 解決方案：批處理

```c
struct folio_batch {
  struct folio *folios[FOLIO_BATCH_SIZE];
  unsigned int nr;
};

void reclaim_folios_batch(struct folio_batch *batch) {
  spinlock_lock(&zone_lock);  // 單次獲取

  for (int i = 0; i < batch->nr; i++) {
    __reclaim_folio(batch->folios[i]);
  }

  spinlock_unlock(&zone_lock);  // 單次釋放
}
```

**成本對比**

| 方式 | 鎖操作 | CPU |
|------|--------|-----|
| 逐個 | 2n | n × (獲取 + 釋放) |
| 批量 | 2 | (獲取 + 釋放) × n/BATCH |

BATCH_SIZE = 15 時，減少 97% 鎖操作。

## 與 LeetCode 146 (LRU 快取) 的對比

### LeetCode 設計

```python
class LRUCache:
  def __init__(self, capacity):
    self.cache = {}
    self.lru = collections.OrderedDict()  # 有序，快速移動

  def get(self, key):
    if key in self.cache:
      self.lru.move_to_end(key)  # O(1) 移至末尾
      return self.cache[key]
    return -1

  def put(self, key, value):
    if key in self.cache:
      del self.lru[key]
    self.cache[key] = value
    self.lru[key] = None
    if len(self.lru) > self.capacity:
      oldest_key, _ = self.lru.popitem(last=False)
      del self.cache[oldest_key]
```

### Kernel 實現差異

```
簡化假設         →  生產事實
單執行緒          →  多執行緒，複雜鎖定
均勻訪問          →  熱資料集 vs 冷資料集
已知容量          →  動態水位標記
同步測試          →  異步回收守護
```

## 時間複雜度

| 操作 | 時間 | 備註 |
|------|------|------|
| 頁訪問 | O(1) | 戳記更新（MGLRU） |
| 頁回收 | O(k) | 掃描 k 個頁面 |
| Slab 分配 | O(1) | 從空閒表棧出 |
| Slab 釋放 | O(1) | 推入空閒表棧 |

## 面試考點

### 問題 1：為什麼 kernel 需要比 LeetCode LRU 複雜得多的設計？

**預期答案**
- **資料量**：數 GB 甚至 TB，無法全部掃描
- **異步性**：回收必須非同步進行，不能阻塞分配
- **工作集變化**：從秒級到分鐘級，需適應
- **多層級**：FILE vs ANON，不同成本
- **鎖爭用**：多執行緒高頻分配，鎖爭用致命

### 問題 2：2-LRU 的瓶頸和 MGLRU 的改進？

**預期答案**
- **瓶頸**：每次訪問需列表移動，頻繁且昂貴
- **MGLRU**：用戳記記錄世代，批量推進
- **結果**：減少鎖爭用、提高精度、支援異步掃描

### 問題 3：Slab 分配相比普通 malloc 的優勢？

**預期答案**
- **碎片**：定長物件無外碎片
- **速度**：無搜尋，O(1) 空閒表操作
- **局部性**：同類物件聚集，快取友善
- **可追蹤**：每個 slab 的利用率易於監控

### 問題 4：為什麼需要批處理？

**預期答案**
- 自旋鎖獲取/釋放成本高
- 逐個操作：2n 次鎖操作
- 批處理：2 + (k-1) ≈ O(1) 攤銷
- 在高頻場景（回收）中關鍵

## 參考資源

- **mm/vmscan.c**：頁面回收算法
- **mm/swap.c**：交換管理
- **mm/slub.c**：Slab 分配器
- **mm/memcontrol.c**：記憶體控制組
- **mm/page_alloc.c**：頁面分配器
- **include/linux/mm.h**：核心結構定義
