# Linux Kernel Memory Barriers 使用分析

> 掃描範圍:`/Users/alex.miao/android-kernel/common-android-mainline/common`
> 掃描方法:`grep -RIc --include='*.[ch]' --include='*.cpp' '\bXXX()'`(字面 macro 呼叫,不含變體)
> 分析日期:2026-04-30

---

## 0. TL;DR — 六個屏障的決策表

| 情境 | write side | read side |
|------|-----------|-----------|
| device MMIO / 強保證(legacy / non-coherent) | `wmb()` | `rmb()` |
| DMA descriptor ↔ device(coherent) | `dma_wmb()` | `dma_rmb()` |
| 跨 CPU 共享資料結構(SMP) | `smp_wmb()` | `smp_rmb()` |

實務選用順序:能用 `dma_*` 就用 `dma_*`(最便宜),需要跨 CPU 同步就用 `smp_*`,只有「真的不在一致性 domain 內或必須強制 UP 也生效」時才用無前綴版本。前綴版本在現代 ARM64 / RISC-V 上能編譯成更輕量的指令(例如 `dma_wmb` 在 ARM64 是 `dmb oshst`),效能差距明顯。

---

## 1. 整體統計總表

| Barrier | files | total occurrences |
|---------|-------|-------------------|
| `wmb()`     | 669 | 1802 |
| `smp_wmb()` | 433 |  753 |
| `dma_wmb()` | 110 |  304 |
| `rmb()`     | 211 |  378 |
| `smp_rmb()` | 334 |  535 |
| `dma_rmb()` | 123 |  213 |

讀側(rmb 系列)全面比寫側(wmb 系列)少,符合典型 producer–consumer 程式的不對稱性(produce 端通常較多,consume 端常以 load-acquire 取代 barrier)。

---

## 2. `wmb()` — 強制寫屏障(UP+SMP 都生效)

### 2.1 起源:`kernel/` 目錄內的使用

直接呼叫 `wmb()` 的程式碼在 `common/kernel/` 共出現在 **3 個檔案**,其中只有 1 處是真正功能性使用,另 2 處是 KCSAN 測試。

#### 真正功能性使用

**`kernel/debug/debug_core.c`**(kgdb / kdb 偵錯核心)

```c
1210  {
1211      atomic_inc(&kgdb_setting_breakpoint);
1212      wmb(); /* Sync point before breakpoint */
1213      arch_kgdb_breakpoint();
1214      wmb(); /* Sync point after breakpoint */
1215      atomic_dec(&kgdb_setting_breakpoint);
1216  }
```

確保 `kgdb_setting_breakpoint` flag 寫入順序對其他 CPU 可見,避免亂序執行造成偵錯狀態不一致。

#### 測試用途

- `kernel/kcsan/selftest.c` 行 171、200 — KCSAN 自我測試,驗證 barrier 偵測器能正確分類 `wmb()`
- `kernel/kcsan/kcsan_test.c` 行 572、617、662 — KCSAN 單元測試

### 2.2 全 tree 整體統計

```
總計:1802 次  /  669 個檔案
```

### 2.3 各 top-level 子系統分布

```
drivers   1410 次  /  547 檔   ← 約 78%
arch       278 次  /   80 檔   ← 約 15%
sound       66 次  /   12 檔
tools       24 次  /   16 檔
include     11 次  /    6 檔
kernel       7 次  /    3 檔
net          6 次  /    5 檔
mm           0
fs           0
block        0
lib          0
crypto       0
security/virt/ipc/init/io_uring   0
```

注意:**mm、fs、block、lib、crypto、security、virt、ipc、init、io_uring 全部是 0**。這是因為這些核心子系統幾乎都改用 `smp_wmb()`(只在 SMP 啟用),`wmb()` 主要保留給「真的需要對 device / DMA / MMIO 強制保證寫入順序」的情境,所以集中在 drivers 與 arch 的低階平台程式。

### 2.4 arch/* 第二層分布

```
arch/mips        181   ← 絕大多數來自 Au1x (Alchemy) SoC
arch/sparc        25
arch/x86          14
arch/powerpc      13
arch/arm          11
arch/alpha        10
arch/sh            9
arch/csky          6
arch/parisc        2
arch/loongarch     2
arch/arc           2
arch/xtensa        1
arch/um            1
arch/arm64         1
```

### 2.5 drivers/* 第二層分布(前 15)

```
drivers/net        600   ← 主要是有線/無線網卡 driver
drivers/scsi       121
drivers/gpu        119
drivers/video       91   ← framebuffer (fbdev)
drivers/usb         69
drivers/infiniband  63
drivers/media       56
drivers/mmc         39
drivers/spi         36
drivers/dma         29
drivers/iommu       17
drivers/crypto      17
drivers/accel       16
drivers/ata         11
drivers/mtd         10
```

### 2.6 真正熱點的檔案(Top 25)

```
43  arch/mips/alchemy/common/usb.c
33  drivers/spi/spi-au1550.c
29  sound/soc/au1x/psc-ac97.c
29  drivers/video/fbdev/tgafb.c                      (DEC Alpha TGA fb)
27  drivers/net/hippi/rrunner.c
27  arch/mips/alchemy/common/irq.c
23  drivers/mmc/host/au1xmmc.c
19  sound/soc/au1x/psc-i2s.c
19  drivers/net/ethernet/jme.c
17  arch/mips/alchemy/common/dbdma.c
15  drivers/video/fbdev/pm2fb.c
14  arch/mips/alchemy/common/vss.c
13  drivers/video/fbdev/au1200fb.c
13  drivers/scsi/qla2xxx/qla_target.c
13  drivers/scsi/qla2xxx/qla_iocb.c
12  drivers/net/ethernet/amd/au1000_eth.c
12  drivers/net/ethernet/alteon/acenic.c
12  arch/sparc/kernel/setup_64.c
11  drivers/scsi/qla2xxx/qla_isr.c
11  drivers/scsi/ipr.c
11  drivers/net/wireless/intersil/p54/p54pci.c
11  drivers/infiniband/hw/mlx4/qp.c
11  arch/mips/cavium-octeon/octeon-irq.c
10  drivers/net/ethernet/cadence/macb_main.c
10  drivers/infiniband/hw/mthca/mthca_qp.c
```

### 2.7 觀察重點

- **Au1x(Alchemy MIPS SoC)生態系獨佔熱點**:Top 25 裡 9 個檔案是它的 USB/IRQ/DBDMA/SPI/MMC/AC97/I2S/Ethernet/fbdev driver,反映出這個平台的 MMIO/DMA 排序需求很嚴格
- **`drivers/net` 雖總量最大**(600),但分散在很多網卡 driver 裡,沒有特別集中的熱點檔
- **`drivers/scsi/qla2xxx`**(Qlogic FC HBA)、**`drivers/infiniband/hw/{mlx4,mthca}/qp.c`**、**`drivers/video/fbdev/tgafb.c`** 都是經典的「跟硬體 doorbell / ring buffer 嚴格同步」案例
- **核心的 `kernel/` 只有 7 次**,跟「核心 SMP 同步用 `smp_wmb()`」的設計慣例完全一致

---

## 3. `smp_wmb()` — SMP-only 寫屏障

### 3.1 整體統計

```
總計:753 次  /  433 個檔案
（對照 wmb():1802 次 / 669 個檔案)
```

### 3.2 各 top-level 子系統分布

```
drivers     297  /  173 檔
arch        149  /   84 檔
fs           72  /   44 檔   ← wmb()=0
kernel       64  /   39 檔   ← wmb()=7
net          62  /   36 檔   ← wmb()=6
include      33  /   16 檔
mm           25  /   11 檔   ← wmb()=0
tools        13  /    9 檔
lib          13  /    5 檔   ← wmb()=0
virt          8  /    4 檔
sound         5  /    4 檔
security      4  /    1 檔
block         4  /    4 檔   ← wmb()=0
io_uring      2  /    1 檔
ipc           1  /    1 檔
rust          1  /    1 檔
```

對比 `wmb()` 最關鍵的差異:`fs` (72)、`kernel` (64)、`mm` (25)、`lib` (13)、`block` (4)、`ipc` (1)、`io_uring` (2)、`virt` (8) 在 `wmb()` 那邊都是 0。這些是核心併發資料結構的所在地,標準慣例就是用 `smp_wmb()`。

### 3.3 arch/* 第二層分布

```
arch/powerpc   54   ← 對比 wmb() 才 13,smp_wmb 占多數
arch/x86       36   ← 多在 KVM
arch/arm       12
arch/mips      11   ← 對比 wmb() 是 181,完全反過來
arch/arm64      9
arch/riscv      7
arch/loongarch  7
arch/sparc      3
arch/xtensa     2
arch/um         2
arch/sh         2
arch/openrisc   2
arch/arc        2
```

### 3.4 drivers/* 第二層分布(前 15)

```
drivers/net          107
drivers/infiniband    20
drivers/gpu           20
drivers/md            15
drivers/mmc           14
drivers/crypto        10
drivers/vhost          9
drivers/firmware       8
drivers/dma            8
drivers/bus            8
drivers/vdpa           7
drivers/input          6
drivers/firewire       6
drivers/video          5
drivers/xen            4
```

### 3.5 kernel/* 第二層分布(全列)

```
kernel/trace                18   ← ring buffer / tracing
kernel/events                9   ← perf events
kernel/sched                 7
kernel/kcsan                 5
kernel/livepatch             4
kernel/smp.c                 3
kernel/rcu                   3
kernel/watchdog_buddy.c      2
kernel/user_namespace.c      2
kernel/signal.c              2
kernel/locking               2
kernel/audit_tree.c          2
kernel/workqueue.c           1
kernel/stop_machine.c        1
kernel/kcov.c                1
kernel/cred.c                1
kernel/bpf                   1
```

### 3.6 真正熱點的檔案(Top 30)

```
9  drivers/vhost/vhost.c                              ← virtio host (vring 同步)
9  arch/powerpc/kvm/book3s_hv.c                       ← KVM HV
8  net/packet/af_packet.c                             ← AF_PACKET ring buffer
8  drivers/mmc/host/atmel-mci.c
7  include/linux/seqlock.h                            ← seqlock 寫入端
7  arch/x86/kvm/x86.c
6  net/xdp/xsk.c                                      ← AF_XDP ring buffer
6  drivers/mmc/host/dw_mmc.c
6  drivers/dma/qcom/gpi.c
5  virt/kvm/kvm_main.c
5  mm/memory.c
5  kernel/trace/ring_buffer.c                         ← ftrace ring buffer
5  fs/overlayfs/util.c
5  arch/x86/kvm/xen.c
4  security/selinux/status.c
4  lib/kfifo.c
4  lib/assoc_array.c
4  kernel/events/core.c                               ← perf
4  include/vdso/helpers.h
4  fs/ubifs/gc.c
4  drivers/vdpa/vdpa_sim/vdpa_sim_net.c
4  drivers/net/ethernet/netronome/nfp/ccm_mbox.c
4  drivers/net/ethernet/3com/typhoon.c
4  drivers/input/misc/cm109.c
4  drivers/firmware/arm_scmi/notify.c
4  drivers/crypto/marvell/octeontx2/otx2_cptpf_mbox.c
4  arch/x86/kvm/vmx/tdx.c
4  arch/x86/kvm/mmu/mmu.c
4  arch/powerpc/platforms/pseries/dtl.c
4  arch/mips/kernel/rtlx.c
```

---

## 4. `dma_wmb()` — DMA 寫屏障(Inner Shareable domain)

### 4.1 整體統計

```
總計:304 次  /  110 個檔案
```

### 4.2 各 top-level 子系統分布

```
drivers   287  /  101 檔   ← 占 94%,幾乎只在 driver 出現
arch        5  /    3 檔
kernel      5  /    2 檔
include     3  /    2 檔
tools       4  /    2 檔
（mm / fs / block / lib / crypto / sound / net / virt 全部為 0）
```

跟前兩個對比鮮明:`wmb()` 還會散到 sound、tools、include;`smp_wmb()` 大量出現在 fs / kernel / mm / lib;`dma_wmb()` 幾乎完全集中在 drivers,而且是有 hardware DMA ring 的 driver。

### 4.3 arch/* 第二層分布

```
arch/parisc   2
arch/arm      2
arch/arm64    1
```

只有 5 次,且全是 arch 自己的 barrier infrastructure 相關檔案,實際使用面是 0。

### 4.4 drivers/* 第二層分布

```
drivers/net          158   ← 主要是有 DMA ring 的 NIC
drivers/infiniband    73   ← 主要來自 Intel irdma
drivers/soc           16   ← 多為 NXP DPAA2 (qbman)
drivers/iommu         11
drivers/scsi           7
drivers/crypto         5
drivers/mmc            4
drivers/misc           4
drivers/usb            3
drivers/vfio           1
drivers/vdpa           1
drivers/spi            1
drivers/media          1
drivers/dma            1
drivers/bus            1
```

`drivers/net + drivers/infiniband` 占了 231 / 287 ≈ 80%。完全符合 `dma_wmb()` 的典型語意:driver 把 descriptor 寫進 DMA ring 之後,在敲 doorbell 之前用 `dma_wmb()` 確保 device 從 memory 讀到正確內容。

### 4.5 真正熱點的檔案(Top 25)

```
48  drivers/infiniband/hw/irdma/ctrl.c              ← Intel iRDMA control
14  drivers/infiniband/hw/irdma/uk.c                ← Intel iRDMA user-kernel
10  drivers/soc/fsl/dpio/qbman-portal.c             ← NXP DPAA2 QBMan
 8  drivers/net/ethernet/sun/sunhme.c
 7  drivers/scsi/ibmvscsi_tgt/ibmvscsi_tgt.c
 7  drivers/net/ethernet/sun/sungem.c
 7  drivers/net/ethernet/mellanox/mlx4/en_tx.c       ← Mellanox mlx4 TX
 7  drivers/net/ethernet/chelsio/cxgb3/sge.c        ← Chelsio T3 SGE
 6  drivers/net/ethernet/engleder/tsnep_main.c
 5  drivers/net/wireless/quantenna/qtnfmac/pcie/topaz_pcie.c
 5  drivers/net/ethernet/renesas/ravb_main.c
 5  drivers/infiniband/hw/irdma/puda.c
 4  drivers/net/ethernet/synopsys/dwc-xlgmac-hw.c
 4  drivers/net/ethernet/socionext/netsec.c
 4  drivers/net/ethernet/neterion/s2io.c
 4  drivers/net/ethernet/amd/xgbe/xgbe-dev.c
 4  drivers/iommu/arm/arm-smmu-v3/arm-smmu-v3.c     ← ARM SMMUv3 command queue
 3  kernel/kcsan/kcsan_test.c                       ← 測試而已
 3  drivers/usb/gadget/udc/cdns2/cdns2-gadget.c
 3  drivers/net/ethernet/stmicro/stmmac/stmmac_main.c
 3  drivers/net/ethernet/renesas/rswitch_main.c
 3  drivers/net/ethernet/realtek/rtase/rtase_main.c
 3  drivers/net/ethernet/meta/fbnic/fbnic_txrx.c
 3  drivers/net/ethernet/marvell/pxa168_eth.c
 3  drivers/net/ethernet/intel/e1000/e1000_main.c
```

---

## 5. `rmb()` — 強制讀屏障

### 5.1 整體統計

```
總計:378 次  /  211 個檔案
```

### 5.2 各 top-level 子系統分布

```
drivers   276  /  156 檔
arch       54  /   27 檔
tools      28  /   19 檔
kernel      7  /    4 檔
include     5  /    2 檔
net         4  /    1 檔
sound       4  /    2 檔
（mm / fs / block / lib / crypto / virt / ipc / io_uring / security 全為 0)
```

### 5.3 arch/* 第二層分布

```
arch/x86         14
arch/sparc        9
arch/powerpc      7
arch/sh           5
arch/mips         5     ← 對比 wmb() 是 181,讀側完全不集中在 MIPS
arch/arm          5
arch/csky         3
arch/parisc       2
arch/arc          2
arch/loongarch    1
```

### 5.4 drivers/* 第二層分布(前 10)

```
drivers/net          141
drivers/scsi          42
drivers/usb           13
drivers/gpu           13
drivers/infiniband     9
drivers/dma            9
drivers/media          8
drivers/video          7
drivers/platform       6
drivers/firewire       5
```

### 5.5 Top 15 熱點檔案

```
9  drivers/scsi/megaraid/megaraid_mbox.c
7  drivers/net/ethernet/cadence/macb_main.c
7  arch/x86/kernel/cpu/resctrl/pseudo_lock.c        ← Cache pseudo-lock
7  arch/sparc/kernel/smp_64.c
6  drivers/net/ethernet/cisco/enic/enic_rq.c
5  drivers/platform/raspberrypi/vchiq-interface/vchiq_core.c
5  drivers/net/ethernet/silan/sc92031.c
5  drivers/net/ethernet/brocade/bna/bnad.c
5  drivers/net/ethernet/broadcom/bnx2x/bnx2x_cmn.c
5  drivers/firewire/ohci.c
4  net/sunrpc/svcsock.c
4  include/asm-generic/barrier.h
4  drivers/scsi/stex.c
4  drivers/net/ethernet/broadcom/tg3.c
4  drivers/net/ethernet/broadcom/cnic.c
```

---

## 6. `smp_rmb()` — SMP-only 讀屏障

### 6.1 整體統計

```
總計:535 次  /  334 個檔案
```

### 6.2 各 top-level 子系統分布

```
drivers   190  /  128 檔
arch       85  /   50 檔
kernel     62  /   31 檔
fs         52  /   36 檔
net        50  /   31 檔
include    36  /   19 檔
tools      16  /   10 檔
mm         11  /    8 檔
lib        10  /    7 檔
sound      10  /    7 檔
virt        6  /    2 檔
block       4  /    3 檔
io_uring    2  /    1 檔
rust        1  /    1 檔
```

跟 `smp_wmb()` 一樣,fs / kernel / mm / lib 都有可觀數量,代表這是核心併發資料結構的標準工具。

### 6.3 arch/* 第二層分布(前 10)

```
arch/powerpc    27
arch/x86        22
arch/arm64       9
arch/mips        6
arch/arm         4
arch/xtensa      3
arch/sparc       3
arch/alpha       3
arch/um          2
arch/sh          2
```

### 6.4 drivers/* 第二層分布(前 10)

```
drivers/net          66
drivers/gpu          24
drivers/md           17
drivers/firewire     12
drivers/firmware      7
drivers/xen           6
drivers/scsi          6
drivers/media         6
drivers/infiniband    6
drivers/char          6
```

### 6.5 Top 15 熱點檔案

```
8  drivers/firewire/sbp2.c
7  kernel/trace/ring_buffer.c                       ← ftrace ring buffer 消費端
7  kernel/sched/core.c
6  kernel/printk/printk_ringbuffer.c                ← printk lockless ringbuf
6  include/linux/seqlock.h                          ← seqlock 讀取端
5  drivers/xen/pvcalls-front.c
5  arch/x86/kvm/mmu/mmu.c
4  virt/kvm/kvm_main.c
4  net/xdp/xsk.c                                    ← AF_XDP ring 消費端
4  kernel/user_namespace.c
4  include/linux/pgtable.h
4  fs/overlayfs/util.c
4  drivers/net/ethernet/via/via-rhine.c
4  drivers/net/ethernet/broadcom/tg3.c
4  drivers/md/dm-raid.c
```

幾個 `smp_wmb()` 的熱點檔在 `smp_rmb()` 也出現:`ring_buffer.c`、`printk_ringbuffer.c`、`seqlock.h`、`xsk.c`、`fs/overlayfs/util.c`、`virt/kvm/kvm_main.c`、`arch/x86/kvm/mmu/mmu.c`。這就是典型的 paired 用法。

---

## 7. `dma_rmb()` — DMA 讀屏障

### 7.1 整體統計

```
總計:213 次  /  123 個檔案
```

### 7.2 各 top-level 子系統分布

```
drivers   195  /  113 檔   ← 92%
arch        6  /    4 檔
kernel      5  /    2 檔   ← 都是 kcsan 測試
include     3  /    2 檔
tools       4  /    2 檔
（其他全為 0)
```

### 7.3 arch/* 第二層分布

```
arch/parisc   2
arch/arm      2
arch/x86      1
arch/arm64    1
```

### 7.4 drivers/* 第二層分布

```
drivers/net          146   ← 75%
drivers/infiniband    26
drivers/scsi          12
drivers/nvme           2
drivers/misc           2
drivers/crypto         2
drivers/accel          2
drivers/soc            1
drivers/mmc            1
drivers/bluetooth      1
```

### 7.5 Top 15 熱點檔案

```
12  drivers/scsi/ibmvscsi_tgt/ibmvscsi_tgt.c
 6  drivers/net/ethernet/actions/owl-emac.c
 5  drivers/net/ethernet/freescale/enetc/enetc.c
 5  drivers/net/ethernet/broadcom/bnxt/bnxt.c       ← Broadcom NetXtreme RX
 5  drivers/infiniband/hw/bnxt_re/qplib_fp.c
 4  drivers/net/wan/fsl_ucc_hdlc.c
 4  drivers/net/ethernet/intel/e1000e/netdev.c
 4  drivers/net/ethernet/ibm/ibmvnic.c
 3  kernel/kcsan/kcsan_test.c
 3  drivers/net/ethernet/renesas/rswitch_main.c
 3  drivers/net/ethernet/renesas/ravb_main.c
 3  drivers/net/ethernet/mellanox/mlx5/core/eq.c    ← mlx5 EQ 消費
 3  drivers/net/ethernet/intel/e1000/e1000_main.c
 3  drivers/net/ethernet/engleder/tsnep_main.c
 3  drivers/net/ethernet/chelsio/cxgb3/sge.c
```

`dma_rmb()` 跟 `dma_wmb()` 一樣集中在 NIC 與 RDMA driver,但兩者用在 ring 的不同端:RX 端先讀 descriptor 的 ownership bit、用 `dma_rmb()` 確保不會在 ownership 還沒翻給 CPU 之前就讀後面的 payload 欄位;TX 端則是寫好整個 descriptor、用 `dma_wmb()` 確保翻 ownership bit 給 device 之前所有欄位都已 visible。

---

## 8. 三大屏障家族的「分工總結」

### 8.1 寫側分工對照表

```
                wmb()         smp_wmb()        dma_wmb()
總次數          1802            753              304
總檔案數         669            433              110
drivers 占比    78%             39%              94%
fs/mm/lib       0               110              0
熱點性質        legacy MMIO     KVM/vring/      modern DMA
                MIPS Au1x       ringbuffer       ring (NIC/RDMA)
                                seqlock/RCU
```

### 8.2 讀側分工對照表

```
                rmb()         smp_rmb()        dma_rmb()
總次數          378             535              213
總檔案數         211            334              123
drivers 占比    73%             36%              92%
fs/mm/lib       0                73              0
熱點性質        legacy device   ring buffer      modern DMA
                read           consumer / KVM    descriptor RX
                                seqlock reader
```

### 8.3 口語版的「該用哪個」

`dma_wmb()` / `dma_rmb()` 是「我要跟有 cache coherence 的 DMA 裝置同步 descriptor / doorbell」這類情境的最佳選擇。比 `wmb()` / `rmb()` 弱(只在 Inner Shareable domain 排序,例如 ARM64 上會編譯成 `dmb oshst` / `dmb oshld` 而不是 `dsb sy`),效能更好,但對「CPU↔可一致 DMA 裝置」的排序保證足夠。

`smp_wmb()` / `smp_rmb()` 是「我跟另一顆 CPU 共享一個資料結構(ring buffer / vring / seqlock),要保證它看到我寫好之後的內容」。UP 編譯時退化為 `barrier()`,效能極佳。

`wmb()` / `rmb()` 是上面兩個都不夠強的時候才用——通常代表這個硬體不在一致性 domain 內,或者是非常古老的平台/legacy driver。Au1x SoC 跟舊式 framebuffer 是典型代表。

### 8.4 典型 paired 用法案例

| 配對 | 寫端用 | 讀端用 | 場景 |
|------|--------|--------|------|
| Producer–Consumer ringbuf(SMP) | `smp_wmb()` | `smp_rmb()` | ftrace ring_buffer / printk_ringbuffer / AF_XDP xsk / virtio vring |
| seqlock | `smp_wmb()`(寫端) | `smp_rmb()`(讀端) | `include/linux/seqlock.h` |
| KVM shared structure | `smp_wmb()` | `smp_rmb()` | `arch/x86/kvm/mmu/mmu.c`、`virt/kvm/kvm_main.c` |
| NIC TX/RX descriptor | `dma_wmb()`(TX) | `dma_rmb()`(RX) | `mlx4/en_tx.c`、`bnxt.c`、`mlx5 eq.c` |
| Doorbell + MMIO(legacy) | `wmb()` | `rmb()` | Au1x SoC、tgafb |

---

## 9. 推薦進一步閱讀的「教科書級」檔案

下列檔案是各 barrier 的代表性使用範例,適合對照原始碼學習:

- `Documentation/memory-barriers.txt` — 官方權威說明
- `kernel/trace/ring_buffer.c` — `smp_wmb()` / `smp_rmb()` paired 經典
- `kernel/printk/printk_ringbuffer.c` — lockless ring buffer
- `include/linux/seqlock.h` — seqlock 的 paired barrier
- `drivers/vhost/vhost.c` — virtio host vring 同步
- `drivers/net/ethernet/mellanox/mlx4/en_tx.c` — `dma_wmb()` NIC TX 經典
- `drivers/net/ethernet/broadcom/bnxt/bnxt.c` — `dma_rmb()` NIC RX 經典
- `drivers/soc/fsl/dpio/qbman-portal.c` — NXP DPAA2 message-passing
- `drivers/iommu/arm/arm-smmu-v3/arm-smmu-v3.c` — SMMUv3 command queue
- `arch/mips/alchemy/common/usb.c` — `wmb()` legacy MMIO 最大宗
- `kernel/debug/debug_core.c` — `wmb()` 在 kernel core 的少數正當用例
