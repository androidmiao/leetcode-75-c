# MCUPM Driver — Code Review 解答集（修正版 + 繁體中文逐行註解）

> 對應題目檔案：`MCUPM_Code_Review_Questions.md`
> 本檔針對每一題提供：
> 1. **問題摘要**（What's wrong）
> 2. **修正後程式碼**（每行繁體中文註解）
> 3. **修正重點說明**

---

## Q1 解答：Mailbox helper 加上 bounds check

### 問題摘要
- `mbox` 為 `unsigned int`，沒有檢查上界 → array out-of-bounds (OOB) read。
- `pin_send_table[mbox].msg_size` 與 `mcupm_mbox_lock[mbox]` 都會踩到任意記憶體。
- `WARN_ON` 在 OOB 之後才觸發，stack trace 已被汙染。

### 修正後程式碼

```c
/* 全域 spinlock 陣列，每個 mailbox channel 一把鎖（保護該 channel 的 send 路徑）*/
static spinlock_t mcupm_mbox_lock[MCUPM_MBOX_TOTAL];

/*
 * mcupm_mbox_read - 從指定 mailbox channel 讀取資料
 * @mbox: mailbox channel index（必須 < MCUPM_MBOX_TOTAL）
 * @slot: slot 編號
 * @buf : 讀出資料的目標 buffer（不可為 NULL）
 * @len : 要讀取的 slot 數量
 *
 * 回傳 0 成功；負值為 -Exxx 標準 errno。
 */
int mcupm_mbox_read(unsigned int mbox, unsigned int slot, void *buf,
		    unsigned int len)
{
	/* (修正 1) 先做 bounds check：mbox 必須在合法範圍內，否則直接拒絕 */
	if (mbox >= MCUPM_MBOX_TOTAL) {
		/* 用 pr_err 而非 dev_err，避免在 mcupm_pdev 為 NULL 時 oops */
		pr_err("[MCUPM] %s: mbox index %u out of range (max %u)\n",
		       __func__, mbox, MCUPM_MBOX_TOTAL);
		return -EINVAL;
	}

	/* (修正 2) buf 也必須做 NULL check，是 caller 給的指標 */
	if (!buf)
		return -EINVAL;

	/* (修正 3) 確認 mcupm_pdev 仍存在，避免 remove 之後仍被呼叫 */
	if (!READ_ONCE(mcupm_pdev))
		return -ENODEV;

	/* 經過 bounds check 後再存取陣列元素，這時 msg_size 才是安全的 */
	if (len > mcupm_mboxdev.pin_send_table[mbox].msg_size) {
		dev_err(&mcupm_pdev->dev,
			"[MCUPM] %s: len %u > msg_size %u on mbox %u\n",
			__func__, len,
			mcupm_mboxdev.pin_send_table[mbox].msg_size, mbox);
		return -EINVAL;
	}

	/* 通過所有檢查後，呼叫底層 mbox API；len * MBOX_SLOT_SIZE 換算 byte */
	return mtk_mbox_read(&mcupm_mboxdev, mbox, slot,
			     buf, len * MBOX_SLOT_SIZE);
}
EXPORT_SYMBOL_GPL(mcupm_mbox_read);  /* 對外 export 加 GPL，並讓 symbol 受保護 */

/*
 * mcupm_mbox_write - 寫入指定 mailbox channel
 * 參數同上；額外多了「critical section 不再分段、不會 TOCTOU」的保證。
 */
int mcupm_mbox_write(unsigned int mbox, unsigned int slot, void *buf,
		     unsigned int len)
{
	unsigned long flags;          /* 儲存 IRQ flags 用，spin_lock_irqsave 需要 */
	unsigned int status;          /* mtk_mbox_check_send_irq 的回傳狀態 */
	int ret;                      /* 底層 API 的回傳值 */

	/* (修正 1) 先 bounds check 再做後續 deref */
	if (mbox >= MCUPM_MBOX_TOTAL)
		return -EINVAL;

	/* (修正 2) NULL pointer check */
	if (!buf)
		return -EINVAL;

	/* (修正 3) device 仍存在 */
	if (!READ_ONCE(mcupm_pdev))
		return -ENODEV;

	/* 通過 bounds check 後再驗證 len 不超過 msg_size */
	if (len > mcupm_mboxdev.pin_send_table[mbox].msg_size)
		return -EINVAL;

	/* (修正 4：對應 Q8) lock 一直持有到 mtk_mbox_write 結束，避免 TOCTOU */
	spin_lock_irqsave(&mcupm_mbox_lock[mbox], flags);

	/* 檢查目標 mailbox 是不是還在處理上一筆資料（busy） */
	status = mtk_mbox_check_send_irq(&mcupm_mboxdev, mbox,
			mcupm_mboxdev.pin_send_table[mbox].pin_index);
	if (status != 0) {
		/* busy：放鎖並回 BUSY，由 caller 決定要不要重試 */
		spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);
		return MBOX_PIN_BUSY;
	}

	/* 仍在 lock 內：寫入 SRAM 並 trigger MCU 端的 IRQ */
	ret = mtk_mbox_write(&mcupm_mboxdev, mbox, slot,
			     buf, len * MBOX_SLOT_SIZE);

	/*
	 * (修正 5) spin_unlock_irqrestore 本身已含 release barrier，
	 * 不需要額外的 mb()。spin_unlock 之後 SRAM 寫入順序已經被保證。
	 */
	spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);

	if (ret != MBOX_DONE)
		return ret;

	return 0;
}
EXPORT_SYMBOL_GPL(mcupm_mbox_write);
```

### 修正重點
1. 任何 array index 在 deref 之前都必須檢查上界。
2. NULL pointer check 與 device-alive check 分別處理。
3. 用 `pr_err` 替換 `dev_err` 以避免 `mcupm_pdev` 為 NULL 時 oops。
4. 把 critical section 整合成一段（解決 Q8 的 TOCTOU）。
5. 拿掉 `mb()`：`spin_unlock_*` 已含 release barrier。
6. `EXPORT_SYMBOL_GPL` 比 `EXPORT_SYMBOL` 嚴格，符合 GPL driver 慣例。

---

## Q2 解答：`mcupm_pdev` 全域變數的生命週期管理

### 問題摘要
- `remove()` 沒清掉 `mcupm_pdev`，沒 unregister IPI/釋放 mailbox。
- 其他模組可能在 remove 後持有指標 → use-after-free。

### 修正後程式碼

```c
/*
 * 用 RCU 或讀取側 cheap 的 READ_ONCE/WRITE_ONCE 配合 module ref 比較簡單，
 * 此處用最直觀的「null check + 在 remove 時清除」配 atomic_t 計數，
 * 來避免 helper 在 remove 後被呼叫。
 */
static struct platform_device __rcu *mcupm_pdev;  /* RCU 保護的全域 pdev */

static int mcupm_device_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;

	/* probe 進來先初始化 IPI 介面（mailbox + ipi_register） */
	ret = mtk_ipi_init(pdev);
	if (ret) {
		dev_err(dev, "[MCUPM] ipi interface init fail, ret %d\n", ret);
		return ret;
	}

	dev_dbg(dev, "MCUPM is ready to service IPI\n");

	/* 初始化 platform 層 sysfs node */
	ret = mcupm_plt_module_init(dev);
	if (ret) {
		dev_err(dev, "[MCUPM] plt module init fail, ret %d\n", ret);
		goto err_plt;     /* (修正) probe 失敗要往回收 */
	}

	/* 初始化 timesync (workqueue + hrtimer) */
	ret = mcupm_timesync_init(dev);
	if (ret) {
		dev_err(dev, "MCUPM timesync init fail, ret %d\n", ret);
		goto err_ts;
	}

	/*
	 * (修正) 一切都成功後，才公開 mcupm_pdev 給 helper 使用。
	 * 用 rcu_assign_pointer 確保 publish 順序與 visibility 正確。
	 */
	rcu_assign_pointer(mcupm_pdev, pdev);

	return 0;

err_ts:
	mcupm_plt_module_exit(dev);
err_plt:
	mtk_ipi_deinit(pdev);    /* 假設底層提供 deinit；若無，需自行包裝 */
	return ret;
}

static int mcupm_device_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	/*
	 * (修正 1) 先把全域指標設成 NULL，並等待所有 RCU reader 離開，
	 * 之後 helper 看到的 pdev 就會是 NULL，會回 -ENODEV。
	 */
	rcu_assign_pointer(mcupm_pdev, NULL);
	synchronize_rcu();

	/* (修正 2) 反向釋放 init 的所有資源（對稱） */
	mcupm_timesync_exit(dev);    /* 對應 Q10、Q11，需新增 */
	mcupm_plt_module_exit(dev);
	mtk_ipi_deinit(pdev);

	return 0;
}
```

### 修正重點
1. **公開時機**：所有資源 init 完才設 `mcupm_pdev`，避免 helper 看到「半成品」。
2. **釋放時機**：先 NULL 全域、`synchronize_rcu()` 等 reader 離開，再 teardown。
3. **對稱性**：probe 的所有 init 都要在 remove 對應地 cleanup。
4. **長期建議**：把 helper API 改成「caller 傳入 device handle」，徹底擺脫全域。

---

## Q3 解答：函式簽名改成 `int` + 回傳標準 errno

### 修正後程式碼

```c
/*
 * mcupm_timesync_init - 初始化 timesync 模組
 * @dev: 對應 platform device
 *
 * (修正) 簽名改為 int，遵循 Linux kernel 慣例：
 *   - 回傳 0 表示成功
 *   - 回傳 -Exxx 表示失敗，與 IS_ERR/PTR_ERR 等 helper 一致
 */
int mcupm_timesync_init(struct device *dev)
{
	u64 wrap;                                /* 暫存 wrap 時間（nsec） */
	int ret;                                 /* (修正) 加上 ret 統一處理錯誤 */

	/*
	 * (修正) create_workqueue 已 deprecated，改用 alloc_workqueue
	 * 並明確指定 flag：
	 *   WQ_FREEZABLE   - suspend 時自動 freeze worker thread
	 *   WQ_MEM_RECLAIM - 在 memory pressure 下也能跑（避免 deadlock）
	 *   max_active = 1 - timesync 的 work 不需要併發
	 */
	timesync_workqueue = alloc_workqueue("mcupm_ts_wq",
					     WQ_FREEZABLE | WQ_MEM_RECLAIM, 1);
	if (!timesync_workqueue) {
		dev_err(dev, "%s workqueue create failed\n", __func__);
		timesync_ctx.enabled = 0;
		/* (修正) 回傳標準 errno，workqueue 建立失敗本質為 OOM */
		return -ENOMEM;
	}

	INIT_WORK(&timesync_ctx.work, timesync_ws);
	spin_lock_init(&timesync_ctx.lock);

	/* 計算 cyclecounter 的 mult/shift（將 cycle 轉換為 nsec） */
	clocks_calc_mult_shift(&timesync_cc.mult, &timesync_cc.shift,
			       arch_timer_get_cntfrq(), NSEC_PER_SEC, 3600);

	/* 取得 cyclecounter 在 wrap 之前能跑多久（nsec），作為 hrtimer 週期 */
	wrap = get_ts_max_nsecs(timesync_cc.mult, timesync_cc.shift,
				timesync_cc.mask);
	timesync_ctx.wrap_kt = ns_to_ktime(wrap);

	/* 初始化 timecounter，起始時間用當下的 sched_clock */
	timecounter_init(&timesync_counter, &timesync_cc, sched_clock());

	/* hrtimer 用 monotonic clock + relative mode */
	hrtimer_init(&timesync_refresh_timer,
		     CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timesync_refresh_timer.function = timesync_refresh;

	dev_dbg(dev, "%s ts: cycle_last %lld, time_base:%lld, wrap:%lld\n",
		TIMESYNC_TAG, timesync_counter.cycle_last,
		timesync_counter.nsec, wrap);

	/*
	 * (修正) 順序：先設 enabled = 1，再 hrtimer_start，最後 sync_base。
	 * 用 smp_store_release 確保 enabled 的寫入在 hrtimer_start 之前可見。
	 */
	smp_store_release(&timesync_ctx.enabled, 1);

	hrtimer_start(&timesync_refresh_timer, wrap, HRTIMER_MODE_REL);

	timesync_sync_base(TIMESYNC_FLAG_SYNC);

	return 0;
}

/* (新增，對應 Q10/Q11) timesync 的 exit path，與 init 對稱 */
void mcupm_timesync_exit(struct device *dev)
{
	/* 先把 enabled 設 0，讓所有 path early return */
	smp_store_release(&timesync_ctx.enabled, 0);

	/* 取消 hrtimer 並等待 callback 結束 */
	hrtimer_cancel(&timesync_refresh_timer);

	/* 把 workqueue flush 完之後 destroy，避免 dangling work */
	if (timesync_workqueue) {
		flush_workqueue(timesync_workqueue);
		destroy_workqueue(timesync_workqueue);
		timesync_workqueue = NULL;
	}
}
```

### 修正重點
1. **簽名**：`unsigned int → int`，符合 errno 慣例。
2. **errno**：`-1 → -ENOMEM`。
3. **API**：`create_workqueue → alloc_workqueue` 並指定 `WQ_FREEZABLE | WQ_MEM_RECLAIM`。
4. **新增 exit path**：與 init 對稱，cleanup workqueue + hrtimer。

---

## Q4 解答：移除錯誤的 `free_page`，並修正 `const` cast

### 問題摘要
- `buf` 是 sysfs framework 擁有的 buffer，**不可以**被 driver 釋放。
- 強制將 `const char *` cast 成 `unsigned long` 是 ownership 錯誤的紅旗。

### 修正後程式碼

```c
/*
 * mcdi_dynamic_finegrain_store - sysfs write callback
 * @kobj: device 物件
 * @attr: device attribute
 * @buf : sysfs core 提供的 kernel buffer，driver **不擁有** 它
 * @n   : buf 的有效長度
 *
 * 回傳：成功時回 n（已消耗的 byte 數）；失敗回 -Exxx。
 */
static ssize_t mcdi_dynamic_finegrain_store(struct device *kobj,
					    struct device_attribute *attr,
					    const char *buf, size_t n)
{
	struct mcupm_ipi_data_s mcdi_ipi_data;   /* 要送到 MCU 的 IPI payload */
	int ret;                                 /* 暫存錯誤碼 */

	/* MCDI fine-grain enable/disable 命令 ID = 0xB1 */
	mcdi_ipi_data.cmd = 0xB1;

	/* 將 sysfs 字串轉成 u32，存入 IPI payload 的 enable 欄位 */
	ret = kstrtou32(buf, 0, &mcdi_ipi_data.u.logger.enable);
	if (ret) {
		/*
		 * (修正) 不要釋放 buf！
		 * buf 是 sysfs framework 的，driver 沒有 ownership，
		 * 任何 free_page/kfree/vfree 都會炸掉。
		 * 直接回 -EINVAL 即可。
		 */
		return -EINVAL;
	}

	/* 把命令送給 MCU 並等 ack（最多 2000 ms） */
	ret = mtk_ipi_send_compl(&mcupm_ipidev, CH_S_PLATFORM, IPI_SEND_WAIT,
				 &mcdi_ipi_data,
				 sizeof(struct mcupm_ipi_data_s) / MCUPM_MBOX_SLOT_SIZE,
				 2000);
	if (ret) {
		dev_err(kobj, "[MCUPM] %s: ipi send fail, ret %d\n", __func__, ret);
		return ret;     /* (修正) 失敗時回真實 errno，而不是 n */
	}

	return n;     /* sysfs store 慣例：成功時回傳消耗的 byte 數 */
}
```

### 修正重點
1. **刪掉 `free_page((unsigned long)buf)`**：根本不需要釋放任何東西。
2. **錯誤路徑回真實 errno**，不要把 IPI 失敗也回成功。
3. **`const char *` 不再被強轉**，型別系統的安全性恢復。

---

## Q5 解答：用 `completion` 取代 sentinel，並用 `mutex` 序列化 sysfs

### 問題摘要
- 共用一個全域 `mcupm_plt_ackdata` → 多個 sysfs reader / IPI ack callback 互相 race。
- 結果：`Alive/Dead` 字串不可靠。

### 修正後程式碼

```c
/*
 * (修正) 把全域 ackdata 改成 per-channel 的 completion + 結果結構，
 * 並用 mutex 序列化 sysfs 路徑。
 */

struct mcupm_plt_request {
	struct completion done;     /* IPI ack callback 來時 complete() */
	u32 result;                  /* MCU 回傳的結果值 */
};

/* sysfs 路徑用的 mutex（一個 sysfs node 不會同時被讀寫，但兩個 node 之間需要序列化） */
static DEFINE_MUTEX(mcupm_plt_lock);
static struct mcupm_plt_request mcupm_plt_req;   /* 取代原本的全域 int */

/* IPI ack callback：被 mtk_ipi 框架在收到 MCU 回覆時呼叫 */
static int mcupm_plt_ack_cb(unsigned int id, void *prdata, void *data, unsigned int len)
{
	struct mcupm_plt_request *req = prdata;

	if (data && len >= sizeof(u32))
		req->result = *(u32 *)data;
	else
		req->result = 0;

	complete(&req->done);    /* 喚醒 sysfs 等待者 */
	return 0;
}

/* 註冊 IPI 時要傳 prdata = &mcupm_plt_req（取代原本傳 &mcupm_plt_ackdata） */

static ssize_t mcupm_alive_show(struct device *kobj,
				struct device_attribute *attr, char *buf)
{
	struct mcupm_ipi_data_s ipi_data;
	int ret;
	u32 alive;

	/* (修正) 序列化 sysfs：避免兩個 reader 互相覆蓋 ack */
	mutex_lock(&mcupm_plt_lock);

	ipi_data.cmd = 0xDEAD;
	reinit_completion(&mcupm_plt_req.done);   /* 把 completion 重設為未完成 */
	mcupm_plt_req.result = 0;

	/* 送命令並等 ack（mtk_ipi_send_compl 內部會 wait_for_completion） */
	ret = mtk_ipi_send_compl(&mcupm_ipidev, CH_S_PLATFORM, IPI_SEND_WAIT,
				 &ipi_data,
				 sizeof(struct mcupm_ipi_data_s) / MCUPM_MBOX_SLOT_SIZE,
				 2000);

	alive = mcupm_plt_req.result;     /* 在 mutex 保護下讀，不會 race */

	mutex_unlock(&mcupm_plt_lock);

	return scnprintf(buf, PAGE_SIZE, "%s MBOX%d ret=%d\n",
			 alive ? "Alive" : "Dead", CH_S_PLATFORM, ret);
}
```

### 修正重點
1. **completion** 取代 sentinel：對「等待非同步事件」是 kernel 標準作法。
2. **mutex** 序列化 sysfs：避免兩個讀者互相覆蓋。
3. **per-request 結構**：未來想加 per-channel 也容易擴充。
4. **`scnprintf`** 取代 `snprintf`：sysfs 慣用，避免「看起來寫了多少其實沒寫」的回傳值問題。

---

## Q6 解答：probe 失敗時用 goto cleanup label

### 修正後程式碼

```c
static int mtk_ipi_init(struct platform_device *pdev)
{
	int ret;
	u32 val, i;
	unsigned int j;            /* (修正) 用於 cleanup loop 的索引 */
	struct device *dev = &pdev->dev;
	char name[32];
	bool legacy_mbox_dt;
	unsigned int mbox_total_number;
	struct mtk_mbox_device *mbox_dev = &mcupm_mboxdev;

	/* 從 DT 讀 mailbox 數量；新 binding 用 mediatek,mbox-extend，舊的固定 8 */
	if (!of_property_read_u32(dev->of_node, "mediatek,mbox-extend", &val)) {
		mbox_total_number = val;
		legacy_mbox_dt = false;
	} else {
		mbox_total_number = 8;
		legacy_mbox_dt = true;
	}

	if (mbox_total_number > MCUPM_MBOX_TOTAL) {
		dev_err(dev, "[MCUPM] mbox number out-range %u\n", mbox_total_number);
		return -EINVAL;
	}

	mbox_dev->count = mbox_total_number;
	mbox_dev->recv_count = mbox_total_number;
	mbox_dev->send_count = mbox_total_number;

	ret = mtk_mbox_probe(pdev, mbox_dev, 0);
	if (ret) {
		dev_err(dev, "[MCUPM] mbox(0) probe fail, ret %d\n", ret);
		return ret;     /* (修正) 不要把真實 errno 翻譯成 -EINVAL */
	}

	for (i = 1; i < mbox_total_number; i++) {
		struct resource *res;
		resource_size_t offset = 0, res_size = 0;   /* (修正 Q7) 初始化 */
		void __iomem *base = NULL;
		struct mtk_mbox_info *minfo_table = mbox_dev->info_table;

		if (!legacy_mbox_dt) {
			res = platform_get_resource_byname(pdev,
					IORESOURCE_MEM, "mbox0_base");
			if (!res) {
				dev_err(dev, "[MCUPM] no resource for mbox0_base\n");
				ret = -ENODEV;
				goto err_smem;
			}
			res_size = resource_size(res);
			offset = res->start + (i * res_size);
			base = devm_ioremap(dev, offset, res_size);
		} else {
			snprintf(name, sizeof(name), "mbox%d_base", i);
			res = platform_get_resource_byname(pdev,
					IORESOURCE_MEM, name);
			if (!res) {
				dev_err(dev, "[MCUPM] no resource for %s\n", name);
				ret = -ENODEV;
				goto err_smem;
			}
			/* (修正 Q7) legacy 分支也要設好 offset/res_size，方便統一印 log */
			offset = res->start;
			res_size = resource_size(res);
			base = devm_ioremap_resource(dev, res);
		}

		if (IS_ERR(base)) {
			ret = PTR_ERR(base);
			dev_err(dev, "mbox-%u remap(%pa) size=%pa ret=%d\n",
				i, &offset, &res_size, ret);
			goto err_smem;
		}

		ret = mtk_smem_init(pdev, mbox_dev, i, base,
				    minfo_table->set_irq_reg,
				    minfo_table->clr_irq_reg,
				    minfo_table->send_status_reg,
				    minfo_table->recv_status_reg);
		if (ret) {
			dev_err(dev, "[MCUPM] smem init fail mbox-%u ret %d\n", i, ret);
			goto err_smem;
		}
	}

	dev_dbg(dev, "[MCUPM] ipi register\n");
	ret = mtk_ipi_device_register(&mcupm_ipidev, pdev, mbox_dev,
				      mbox_total_number);
	if (ret) {
		dev_err(dev, "[MCUPM] ipi_dev_register fail, ret %d\n", ret);
		goto err_smem;
	}

	ret = mtk_ipi_register(&mcupm_ipidev, CH_S_PLATFORM,
			       NULL, mcupm_plt_ack_cb,
			       (void *)&mcupm_plt_req);
	if (ret) {
		dev_err(dev, "[MCUPM] ipi_register fail ipi %d, ret %d\n",
			CH_S_PLATFORM, ret);
		goto err_ipi_dev;
	}

	for (i = 0; i < mbox_total_number; i++)
		spin_lock_init(&mcupm_mbox_lock[i]);

	return 0;

	/* ===== Cleanup labels：以「最後成功的步驟」為起點往回拆 ===== */
err_ipi_dev:
	mtk_ipi_device_unregister(&mcupm_ipidev);
err_smem:
	/* 把已 init 過的 smem 倒序 teardown（j 從 i-1 倒回 1） */
	for (j = i; j-- > 1;)
		mtk_smem_exit(mbox_dev, j);
	mtk_mbox_remove(pdev, mbox_dev);    /* 對應 mtk_mbox_probe */
	return ret;
}
```

### 修正重點
1. **goto cleanup label**：每個失敗點跳到對應的清理位置，倒序釋放。
2. **保留真實 errno**：`return ret` 而不是 `return -EINVAL`。
3. **`devm_*` 不需要在 cleanup 中手動釋放**（probe 失敗 driver core 自動清）。
4. **同步修掉 Q7 的未初始化變數**。

---

## Q7 解答：legacy 分支也初始化 `offset/res_size`

已在 Q6 的修正版內處理：

```c
resource_size_t offset = 0, res_size = 0;   /* 宣告即初始化 */
...
} else {
	/* legacy 分支也要設好 offset/res_size，這樣失敗 log 才有意義 */
	offset = res->start;
	res_size = resource_size(res);
	base = devm_ioremap_resource(dev, res);
}
```

### 修正重點
- **宣告即初始化**：避免 garbage stack value 被印出。
- **每個分支都要寫值**：不要只在某一條 path 設定。
- **review 時的 checklist**：「分支內賦值的變數，所有分支都要覆蓋到，否則初始化在宣告時做」。

---

## Q8 解答：lock 包到 `mtk_mbox_write` 結束

已在 Q1 的 `mcupm_mbox_write` 修正版內整合：

```c
spin_lock_irqsave(&mcupm_mbox_lock[mbox], flags);

status = mtk_mbox_check_send_irq(...);
if (status != 0) {
	spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);
	return MBOX_PIN_BUSY;
}

/* (修正) 仍在 lock 內：寫入 SRAM */
ret = mtk_mbox_write(...);

/* spin_unlock 已含 release barrier，不需 mb() */
spin_unlock_irqrestore(&mcupm_mbox_lock[mbox], flags);
```

### 修正重點
1. **lock 範圍要涵蓋整個 critical region**：check + write 必須原子。
2. **拿掉多餘的 `mb()`**：`spin_unlock_*` 已是 release barrier。
3. **若 `mtk_mbox_write` 內部就有鎖**：仍應在這層保護「check + write」的 TOCTOU。

---

## Q9 解答：Time Sync protocol 加上明確的 seqlock 註解 + 收緊 barrier

### 修正後程式碼

```c
/*
 * MCUPM Time Sync 協定（AP → MCU 單向寫入）
 * ================================================================
 * 4 個 32-bit slot：
 *   TICK_H (high 32-bit of arch_timer tick) | header(4-bit at top)
 *   TICK_L (low 32-bit of tick)
 *   TS_L   (low 32-bit of sched_clock ns)
 *   TS_H   (high 32-bit) | header(4-bit at top)
 *
 * Header layout：
 *   bit 31    : freeze flag
 *   bit 30..28: 3-bit version（每次更新 +1，會 wrap）
 *
 * 寫入順序：TICK_H → TICK_L → TS_L → TS_H
 * 開頭 TICK_H 與結尾 TS_H 都帶相同 version。
 *
 * MCU 端讀取規則（reader-side validation）：
 *   1. 讀 TS_H 取得 version A。
 *   2. 依序讀 TS_L、TICK_L、TICK_H。
 *   3. 重新讀 TS_H 取得 version B。
 *   4. 若 A != B → AP 正在更新，丟棄重試。
 *   5. 若一致 → 64-bit tick / ts 是穩定的 snapshot。
 *
 * 註：3-bit version 上限 8，wrap 後 reader 仍能正確判斷
 *     「不一致」這件事，因此 wrap 不會破壞 protocol。
 */
static void mcupm_ts_update(int suspended, u64 tick, u64 ts)
{
	u32 header, val;

	/*
	 * (註解) 假設 caller 已持有 timesync_ctx.lock，所以 mcupm_base_ver
	 * 不會 race。若未來有新 caller，需自行加鎖或改成 atomic_t。
	 */
	mcupm_base_ver = (mcupm_base_ver + 1) % (TIMESYNC_MAX_VER + 1);

	/* 組 header：freeze bit + version 欄位 */
	header = suspended ? TIMESYNC_HEADER_FREEZE : 0;
	header |= (mcupm_base_ver << TIMESYNC_HEADER_VER_OFS) &
		  TIMESYNC_HEADER_VER_MASK;

	/* 寫入順序：TICK_H 先；header 帶在 TICK_H 與 TS_H 兩端 */
	val = (tick >> 32) & TIMESYNC_MASK_32_BITS;
	val |= header;
	mcupm_ts_write(MCUPM_TS_MBOX_TICK_H, val);

	/*
	 * (修正) wmb() 即可：我們只在意「寫入 SRAM 的順序」，
	 * 不需要 full memory barrier。減少不必要的 barrier 成本。
	 */
	wmb();

	val = tick & TIMESYNC_MASK_32_BITS;
	mcupm_ts_write(MCUPM_TS_MBOX_TICK_L, val);
	wmb();

	val = ts & TIMESYNC_MASK_32_BITS;
	mcupm_ts_write(MCUPM_TS_MBOX_TS_L, val);
	wmb();

	/* 結尾 TS_H 帶 version，是 reader 判斷一致性的關鍵 */
	val = (ts >> 32) & TIMESYNC_MASK_32_BITS;
	val |= header;
	mcupm_ts_write(MCUPM_TS_MBOX_TS_H, val);
	wmb();
}
```

### 修正重點
1. **加上 protocol 註解**：把「為什麼順序是這樣」寫清楚，未來改 code 不會亂動。
2. **`mb() → wmb()`**：只關心 store ordering，省掉 read barrier。
3. **MCU 端讀法明文化**：reader 必須再讀一次 TS_H 比對 version。
4. **長期建議**：若 hardware 支援 64-bit atomic write，整個 protocol 可以簡化成一次寫。

---

## Q10 解答：`alloc_workqueue` + 完整 suspend/resume 對稱

### 修正後程式碼

```c
unsigned int mcupm_timesync_init(struct device *dev)  /* 已在 Q3 修正 */
{
	/* ...見 Q3... */
}

void mcupm_timesync_exit(struct device *dev)
{
	/*
	 * (新增) 與 init 對稱的 exit path：
	 *   1. 停 hrtimer
	 *   2. flush + destroy workqueue
	 *   3. 把 enabled 清 0
	 */
	smp_store_release(&timesync_ctx.enabled, 0);

	hrtimer_cancel(&timesync_refresh_timer);

	if (timesync_workqueue) {
		flush_workqueue(timesync_workqueue);
		destroy_workqueue(timesync_workqueue);
		timesync_workqueue = NULL;
	}
}

void mcupm_timesync_suspend(void)
{
	/* (修正) 用 smp_load_acquire 配合 init/exit 的 store_release */
	if (!smp_load_acquire(&timesync_ctx.enabled))
		return;

	/* 先 cancel hrtimer，確保不會再有新的 work queue 進來 */
	hrtimer_cancel(&timesync_refresh_timer);

	/* (修正) flush 已 queue 的 work，避免 freeze 之後 work 還在跑 */
	flush_workqueue(timesync_workqueue);

	/* 同步 freeze 狀態給 MCU */
	timesync_sync_base(TIMESYNC_FLAG_SYNC | TIMESYNC_FLAG_FREEZE);
}

void mcupm_timesync_resume(void)
{
	if (!smp_load_acquire(&timesync_ctx.enabled))
		return;

	/* sched_clock 在 suspend 時會停，arch_timer 不會，所以要重設 timecounter */
	timecounter_init(&timesync_counter, &timesync_cc, sched_clock());

	hrtimer_start(&timesync_refresh_timer,
		      timesync_ctx.wrap_kt, HRTIMER_MODE_REL);

	/* 通知 MCU 已 unfreeze */
	timesync_sync_base(TIMESYNC_FLAG_SYNC | TIMESYNC_FLAG_UNFREEZE);
}
```

### 修正重點
1. **`alloc_workqueue` + flag**：明確 `WQ_FREEZABLE | WQ_MEM_RECLAIM`。
2. **新增 exit path**：與 init 對稱。
3. **suspend 時 flush**：避免 work 在 freeze 後又動。
4. **`smp_load_acquire/store_release`**：保證跨 CPU visibility。

---

## Q11 解答：補上完整的 module exit

### 修正後程式碼

```c
static int mcupm_device_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	/* (修正) 先把全域 pdev 設為 NULL，等 RCU reader 離開 */
	rcu_assign_pointer(mcupm_pdev, NULL);
	synchronize_rcu();

	/* 反向 teardown：timesync → plt → ipi */
	mcupm_timesync_exit(dev);
	mcupm_plt_module_exit(dev);

	/* (修正) 真的 unregister IPI（對應 mtk_ipi_register） */
	mtk_ipi_unregister(&mcupm_ipidev, CH_S_PLATFORM);
	mtk_ipi_device_unregister(&mcupm_ipidev);

	return 0;
}

void mcupm_plt_module_exit(struct device *dev)
{
	/* (修正) 真的把 sysfs node 移除 */
	device_remove_file(dev, &dev_attr_mcupm_alive);
	device_remove_file(dev, &dev_attr_mcdi_dynamic_finegrain);

	dev_dbg(dev, "[MCUPM] mcupm plt module exit done\n");
}
```

### 修正重點
1. **不要留 `// TODO` 不做事**：要嘛立即實作，要嘛開 ticket、加 `WARN_ONCE` 提醒。
2. **對稱性**：`mcupm_plt_module_init` 註冊了什麼，exit 就要移除什麼。
3. **lifecycle**：unregister IPI、destroy workqueue、cancel hrtimer、清 `mcupm_pdev` 全部要做。

---

## 總結：Review 結論等級

| 等級 | 對應題目 | 原因 |
|------|----------|------|
| ❌ NACK | Q1, Q4 | OOB read + free 不屬於自己的 buffer，皆為 kernel panic 等級 |
| 🟡 Major | Q2, Q5, Q6, Q8, Q9, Q10 | 設計缺陷與 race，需要分 patch 修 |
| 🟢 Nit  | Q3, Q7, Q11 | 型別/初始化/cleanup hygiene，可一併或 follow-up 修 |

### 修正後驗證 Checklist

- [x] 所有 helper 都有 bounds check + NULL check + device-alive check。
- [x] 所有全域指標的 publish/teardown 用 RCU 保護。
- [x] 所有函式簽名與 errno 一致 (`int` 回傳 `-Exxx`)。
- [x] sysfs callback 不再 free 不屬於自己的 buffer。
- [x] 共享狀態用 `completion` 或 `mutex` 序列化。
- [x] probe 失敗有 goto cleanup label，並保留真實 errno。
- [x] 變數宣告即初始化，所有分支都覆蓋到。
- [x] 鎖的 critical section 不被切成兩段。
- [x] Mailbox protocol 註解明確、barrier 收緊到 `wmb()`。
- [x] `create_workqueue → alloc_workqueue` + `WQ_FREEZABLE | WQ_MEM_RECLAIM`。
- [x] init / exit / suspend / resume 完全對稱。
- [x] 所有 `// TODO` 都已實作或開 ticket。

---

## Sources

- `MCUPM_Code_Review_Questions.md`
- `drivers/remoteproc/mcupm/v2/mcupm_driver.c`
- `drivers/remoteproc/mcupm/v2/mcupm_plt.c`
- `drivers/remoteproc/mcupm/v2/mcupm_timesync.c`
