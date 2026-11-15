# Flash 与 RAM

> WS63 芯片的 Flash 分区方案与 RAM (Random Access Memory) 内存分配

---

## Flash 分区布局

分区表定义在 `build/config/target_config/ws63/param_sector/param_sector.json`。Flash 基址 `0x200000`，分区表覆盖 4 MB（`0x200000` ~ `0x600000`）。以下地址均为片内 Flash 绝对地址，偏移值为相对 Flash 基址的偏移量。

```
Flash 基址: 0x200000, 分区表覆盖: 4 MB
注：以下地址均来源于 param_sector.json，ID 与偏移见右侧标注

绝对地址                分区 (ID)                  JSON 偏移
0x200000 ┌──────────────────────────┐
         │ Root public key   (0x03) │  2 KB       0x00000000
0x202000 ├──────────────────────────┤
         │ SSB               (0x00) │  24 KB      0x00002000
0x208000 ├──────────────────────────┤
         │ Customer factory  (0x08) │  16 KB      0x00008000
0x20C000 ├──────────────────────────┤
         │ NV Backup         (0x09) │  16 KB      0x0000C000
0x210000 ├──────────────────────────┤
         │ FlashBoot backup  (0x02) │  64 KB      0x00010000
0x220000 ├──────────────────────────┤
         │ FlashBoot         (0x01) │  64 KB      0x00020000
0x230000 ├──────────────────────────┤
         │                          │
         │ imageA (App)      (0x20) │  2.25 MB    0x00030000
         │                          │
0x470000 ├──────────────────────────┤
         │                          │
         │ fota data         (0x21) │  1.55 MB    0x00270000
         │                          │
0x5F3000 ├──────────────────────────┤
         │ rsv for customer  (0x30) │  32 KB      0x003F3000
0x5FB000 ├──────────────────────────┤
         │ Crash info        (0x11) │  4 KB       0x003FB000
0x5FC000 ├──────────────────────────┤
         │ NV DATA           (0x10) │  16 KB      0x003FC000
0x600000 └──────────────────────────┘
```

| 分区 ID | 分区名称 | 绝对地址 | 大小 | 说明 |
|---------|---------|---------|------|------|
| 0x03 | Root public key | `0x200000` | 2 KB | 安全启动根公钥 |
| 0x00 | SSB (Secure Secondary Boot) | `0x202000` | 24 KB | Secure Secondary Boot |
| 0x08 | Customer factory | `0x208000` | 16 KB | 客户出厂配置 |
| 0x09 | NV (Non-Volatile) Backup | `0x20C000` | 16 KB | NV 数据备份 |
| 0x02 | FlashBoot backup | `0x210000` | 64 KB | FlashBoot 备份区 |
| 0x01 | FlashBoot | `0x220000` | 64 KB | 引导程序 |
| 0x20 | imageA (App) | `0x230000` | 2.25 MB | 主应用程序固件 |
| 0x21 | fota data | `0x470000` | 1.55 MB | OTA (Over-The-Air) 固件下载区 |
| 0x30 | rsv for customer | `0x5F3000` | 32 KB | 客户预留区 |
| 0x11 | Crash info | `0x5FB000` | 4 KB | 死机信息保存区 |
| 0x10 | NV DATA | `0x5FC000` | 16 KB | 非易失性数据存储 |

> 双镜像（A/B）布局使用 `param_sector_double.json`，分区偏移和大小有所不同。

---

## RAM 内存分配

以下地址和大小取自当前 SDK 构建生成的 `ws63-liteos-app.elf` 和 `ws63-liteos-app.map`。表中的结束地址均包含在对应区域内。

### RAM 总体布局

| 地址范围 | 分配大小 | 当前使用情况 | 区域内容 |
|----------|----------|--------------|----------|
| `0x0014C000`～`0x0014FFFF` | 16 KiB | 已使用 12,992 B，剩余 3,392 B | ITCM (Instruction Tightly Coupled Memory) ：RISC-V Patch、ROM (Read-Only Memory) 回调代码和需要在 TCM 中执行的热点代码 |
| `0x00180000`～`0x00183FFF` | 16 KiB | 已使用 14,916 B，剩余 1,468 B | DTCM (Data Tightly Coupled Memory) ：ROM 运行数据、TCM 数据以及 Wi-Fi、BLE (Bluetooth Low Energy) 等模块的 TCM BSS |
| `0x00A00000`～`0x00A85EFF` | 535.75 KiB | 链接时固定占用 189,664 B；系统堆 358,944 B | App SRAM (Static Random Access Memory) ：Packet RAM、RAM 代码、静态数据、固定异常栈和系统堆 |
| `0x00A85F00`～`0x00A85FFF` | 256 B | `.preserve` 实际使用 252 B | 跨指定复位或异常流程保留的诊断数据 |
| `0x00A86000`～`0x00A87FFF` | 8 KiB | 整段预留 | Radar RX (Receive) 接收缓冲区 |

ITCM 当前从 `0x0014C000` 连续使用到 `0x0014F2BF`；DTCM 当前从 `0x00180000` 连续使用到 `0x00183A43`。两者末尾的剩余空间没有分配给普通堆。

### App SRAM 详细分配

| 地址范围 | 大小 | 链接段或区域 | 说明 |
|----------|------|--------------|------|
| `0x00A00000`～`0x00A0BFFF` | 48 KiB | `.wifi_pkt_ram` | Wi-Fi Packet RAM 预留区；启动汇编使用其结束地址 `0x00A0C000` 作为初始栈顶 |
| `0x00A0C000`～`0x00A0F85B` | 14,428 B | `.text_sram` | 需要在 SRAM 中执行的代码和只读数据 |
| `0x00A0F85C`～`0x00A0F85F` | 4 B | 对齐填充 | 将 `.data` 对齐到 8 字节边界 |
| `0x00A0F860`～`0x00A1217B` | 10,524 B | `.data` | 有初始值的全局变量、静态变量及组件运行数据，启动时从 Flash 装载 |
| `0x00A1217C`～`0x00A2D0D3` | 110,424 B | TLS (Transport Layer Security) 、各组件 BSS、`.bss` | 未初始化或零初始化的全局变量、静态对象和静态缓冲区；大小包含段间对齐 |
| `0x00A2D0D4`～`0x00A2D0DF` | 12 B | 对齐填充 | 将固定栈区域对齐到 16 字节边界 |
| `0x00A2D0E0`～`0x00A2D8DF` | 2 KiB | IRQ (Interrupt Request) 栈 | 中断处理使用，栈顶符号为 `__irq_stack_top` |
| `0x00A2D8E0`～`0x00A2E0DF` | 2 KiB | Exception 栈 | 异常处理使用，栈顶符号为 `__exc_stack_top` |
| `0x00A2E0E0`～`0x00A2E4DF` | 1 KiB | NMI (Non-Maskable Interrupt) 栈 | NMI 处理使用，栈顶符号为 `__nmi_stack_top` |
| `0x00A2E4E0`～`0x00A85EFF` | 358,944 B（约 350.53 KiB） | LiteOS (Huawei LiteOS) 系统堆 | 运行时任务栈、OSAL (Operating System Abstraction Layer) 动态内存、队列、控制块及中间件动态缓冲区从这里申请 |

链接器报告的 SRAM 固定占用为 189,664 B，即从 `0x00A00000` 到固定栈末尾 `0x00A2E4DF`。系统堆紧随其后，初始范围为 `0x00A2E4E0`～`0x00A85EFF`。

### `.bss` 与静态缓冲区

“通用缓冲区”不是一个独立链接分区。定义为全局或静态数组的缓冲区会进入对应组件的 BSS；运行时申请的缓冲区则进入系统堆。当前 ELF (Executable and Linkable Format) 中的零初始化段如下：

| 输出段 | 地址 | 段大小 | 主要归属 |
|--------|------|--------|----------|
| `.tbss.*` | `0x00A1217C` | 8 B | C/C++ 线程局部零初始化数据 |
| `.plat_bss_sram` | `0x00A12180` | 1,169 B | LiteOS、OSAL、中断及平台基础模块 |
| `.plat_bss_flash` | `0x00A12618` | 12,956 B | 平台驱动、文件系统、日志、NV 等模块 |
| `.wifi_bss_flash` | `0x00A158B8` | 38,048 B | Wi-Fi、LWIP (Lightweight IP) 和 WPA (Wi-Fi Protected Access) 相关静态状态与缓冲区 |
| `.radar_bss_flash` | `0x00A1ED58` | 38,156 B | Radar 驱动和感知模块静态数据 |
| `.radar_ai_bss_flash` | `0x00A28268` | 6,133 B | Radar AI (Artificial Intelligence) 静态数据 |
| `.btc_bss_flash` | `0x00A29A60` | 3,272 B | BLE/SLE Controller 静态数据 |
| `.bth_bss_flash` | `0x00A2A728` | 3,320 B | 蓝牙 Host、SDK、示例及测试模块静态数据 |
| `.bss` | `0x00A2B420` | 7,348 B | 未被前述组件专用规则收集的普通 BSS/SBSS/COMMON 数据 |

各输出段之间存在地址对齐，TLS 段也具有特殊的地址表示方式，因此应以“App SRAM 详细分配”中的完整地址范围计算占用，不应简单累加本表得到区域终点。

### 任务栈与动态缓冲区

IRQ、Exception 和 NMI 使用上表中固定的 5 KiB 栈。普通 LiteOS 任务栈没有固定链接地址：`LOS_TaskCreate()` 最终通过 `LOS_MemAllocAlign()` 从 LiteOS 系统堆动态分配，因此只能在链接时确定申请大小，实际地址和堆剩余量要在设备运行后查看。

`g_app_tasks[]` 中平台任务当前配置的栈申请大小如下；任务是否存在取决于对应编译条件。

| 任务 | 栈申请大小 | 编译条件 |
|------|------------|----------|
| `app` | 2 KiB（`0x800`） | 非 `CHIP_EDA` 构建 |
| `cmd_loop` | 4 KiB（`0x1000`） | `TEST_SUITE` |
| `log` | 2 KiB（`0x800`） | `HSO_SUPPORT` |
| `bt` | 3.5 KiB（`0xE00`） | `BGLE_TASK_EXIST` |
| `bt_sdk` | 2 KiB（`0x800`） | `BTH_TASK_EXIST` |
| `bth_sdk` | 512 B（`0x200`） | `BTH_TASK_EXIST` |
| `bt_service` | 4 KiB（`0x1000`） | `BTH_TASK_EXIST` |
| `at` | 8 KiB（`0x2000`） | `AT_COMMAND` |
| `wifi` | 8 KiB（`0x2000`） | `WIFI_TASK_EXIST` |
| `radar_driver` | 2 KiB（`0x800`） | `CONFIG_RADAR_SERVICE` 且未定义 `DISABLE_RADAR` |
| `radar_feature` | 9.5 KiB（`0x2600`） | `CONFIG_RADAR_SERVICE` 且未定义 `DISABLE_RADAR` |
| `hilink` | 8 KiB（`0x2000`） | `CONFIG_SUPPORT_HILINK` |
| `matter` | 8 KiB（`0x2000`） | `CONFIG_SUPPORT_MATTER` |

LiteOS 内核任务、工作队列以及其他中间件自行创建的任务也会占用系统堆，任务控制块和内存分配器元数据还会产生额外开销。因此，358,944 B 是调度器启动前的堆区域容量，不是业务可以全部占用的“空闲 RAM”。动态缓冲区同样没有预先划定的固定地址或固定总量，应通过运行时内存统计确认实际使用量。

## 参考

- [构建系统](../build-output/index.md) — 各镜像文件与 Flash 分区的对应关系
- [启动流程](../boot-flow/index.md) — 各启动阶段对内存的使用
