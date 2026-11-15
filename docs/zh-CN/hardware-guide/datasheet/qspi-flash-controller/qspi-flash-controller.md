<a id="3"></a>
# QSPI Flash 控制器

<a id="3.1"></a>
## 概述

SFC是一个SPI Flash控制器。业务侧提供一个AHB（Advanced High Performance Bus）Slave接口，主要完成AHB通道对SPI Flash的访问控制功能；提供一个AHB Master接口，用于DMA方式读写Flash。

图3-1 SFC应用框图

<div style="text-align:left; margin:10px 0;"><img src="images/图3-1 SFC 应用框图.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

注：IF（Interface）。

<a id="3.2"></a>
## 功能描述

### AHB Slave接口

AHB Slave接口具有以下特点：

- 提供一个AHB Slave接口，可以根据不同的选择信号访问内部配置寄存器或直接访问SPI Flash Memory。
- 支持AMBA2.0协议。
- 仅支持小端（Little-Endian）。

### AHB Master接口

AHB Master接口具有以下特点：

- 提供一个AHB Master接口，用于DMA方式在内存和Flash之间搬运数据。
- 支持AMBA2.0协议。
- 只支持小端。
- 支持Single、INCR、INCR4、INCR8、INCR16传输类型。
- 不支持Early Termination。
- 支持总线Lock传输。

### 存储器接口

存储器接口具有以下特点：

- 片选的存储空间最大支持到64Mbit（3Byte地址模式）。片选映射基地址可配置。只支持片选1，不支持片选0。
- 支持Standard SPI、Dual-Output/Dual-Input SPI、Quad-Output/Quad-Input SPI、Dual-I/O SPI、Quad-I/O SPI五种接口类型。上电后默认支持Standard SPI接口类型，可通过寄存器配置切换接口类型。
- 支持XIP（Executed In Place）。
- SPI Flash读写操作支持总线直接读写、寄存器编程读写、DMA读写三种方式。
- 支持多种写保护操作。
- SFC模块支持SPI Mode0和Mode3，按协议要求，支持SPI Mode0和Mode3的SPI Flash器件在时钟的上升沿采样数据，在时钟的下降沿输出数据。
- XIP实现地址remap，为了软件只编译一个XIP镜像，升级镜像时主镜像和备份镜像使用相同地址。

### Flash数据在线解密

Flash数据在线解密具有以下特点：

- 解密算法为AES-128-CTR，仅支持1个IV，秘钥来源于KM派生。
- 支持4个解密区域，配置粒度为256Byte；每个区域支持单独的IV解密起始地址可配，配置粒度同为256Byte，4个解密区域配置不能存在交叉地址。

支持AES在线解密，解密时读取数据量没有16Byte的倍数与对齐的约束。

<a id="3.3"></a>
## 工作方式

<a id="3.3.1"></a>
### 读写Flash

有三种方式读写Flash：

- 通过寄存器配置方式发送SPI Flash Program、Read等命令来读写Flash。例如：对寄存器<a href="#reg-3.5-cmd_config">CMD_CONFIG</a>写0x0000_7F8B，对寄存器<a href="#reg-3.5-cmd_ins">CMD_INS</a>写0x03，表示通过Standard SPI方式发起读64Byte Flash数据的操作。

此方式直接控制需要发送的Flash命令。

- 通过AHB Slave接口以类似读写普通Memory的方式读写Flash。

SFC模块会自动将AHB总线的读写操作时序映射为SPI Flash读写命令。

- 通过DMA方式在Flash和外部Memory之间搬移数据。

<a id="3.3.2"></a>
### 其他操作

对Flash的其他操作如Erase、进入Deep Power Down、读Device ID等必须通过寄存器访问来实现。需要配置<a href="#reg-3.5-cmd_ins">CMD_INS</a>[REG_INS]为相应的命令，具体请参见Flash器件手册。

例如：对寄存器<a href="#reg-3.5-cmd_config">CMD_CONFIG</a>写0x0000_0583，对寄存器<a href="#reg-3.5-cmd_ins">CMD_INS</a>写0x0000_009F，表示读器件ID的操作。

<a id="3.3.3"></a>
### 初始化流程

注意以下初始化流程仅做参考，请根据器件差异进行调整。

初始化流程如下：

步骤 1 （如果需要调整Timing参数）配置<a href="#reg-3.5-timing">TIMING</a>寄存器。

步骤 2 配置总线操作方式寄存器。

- 根据实际Flash大小配置BUS_FLASH_SIZE[flash_size_cs1]（直接获知器件大小或可通过发Read ID命令给Flash查询获得）。
- 有些器件要求进入非Standard SPI读写时序，需要预先以特殊命令配置Flash。根据器件需要，对寄存器<a href="#reg-3.5-cmd_ins">CMD_INS</a>进行写操作，发特定命令配置Flash。
- 通过<a href="#reg-3.5-bus_config1">BUS_CONFIG1</a>/<a href="#reg-3.5-bus_config2">BUS_CONFIG2</a>配置总线读写操作指令和参数。

例如：对寄存器<a href="#reg-3.5-bus_config1">BUS_CONFIG1</a>写0xCC85_EB1E表示配置的参数为写指令32h，写方式为Quad-Input SPI，读指令EBh、读方式为Quad I/O SPI。

- 如果需要开启总线写操作，配置<a href="#reg-3.5-bus_config1">BUS_CONFIG1</a>[wr_enable]为1，使能总线写。默认关闭总线写功能。

----结束

<a id="3.3.4"></a>
### 通过寄存器方式读Flash操作流程

通过寄存器读取Flash的操作流程（查询方式），如图3-2所示。

图3-2 通过寄存器读取Flash的操作流程（查询方式）

<div style="text-align:left; margin:10px 0;"><img src="images/图3-2 通过寄存器读取 Flash 的操作流程（查询方式）.jpg" alt="" style="max-width:100%; max-height:560px; width:auto; height:auto;"></div>

<a id="3.3.5"></a>
### 通过寄存器方式写Flash操作流程

- 通过寄存器方式写Flash数据时，总线和DMA不得访问Flash。
- 单次写Flash不能跨越Page边界（寄存器写方式没有跨越Page边界保护，需要软件保证，如果跨越256Byte边界，将会Wrap到该Page的起始地址，覆盖原来的内容）。

通过寄存器写Flash的操作流程（中断方式），如图3-3所示。

图3-3 通过寄存器写Flash的操作流程（中断方式）

<div style="text-align:left; margin:10px 0;"><img src="images/图3-3 通过寄存器写 Flash 的操作流程（中断方式）.jpg" alt="" style="max-width:100%; max-height:560px; width:auto; height:auto;"></div>
注：WREN（Write Read Enable）， PP（Page Program）， RDSR（Read Status Register）。

<a id="3.3.6"></a>
### 通过寄存器方式其他操作流程

通过寄存器方式其他操作流程如图3-4所示。

图3-4 通过寄存器方式其他操作流程

<div style="text-align:left; margin:10px 0;"><img src="images/图3-4 通过寄存器方式其他操作流程.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

SFC控制器不支持发出 "OPCODE（1byte）+ DUMMY（3byte全0）" 组合SPI时序，某些Flash指令需要这种组合时序时，可以采用 "OPCODE（1byte）+ADDR（3byte全0）" 组合代替。

<a id="3.3.7"></a>
### 通过AHB Slave直接读写Flash操作流程

上电复位后，默认配置为Standard SPI时序模式。不需要额外配置，可直接读Flash。

默认通过AHB Slave写Flash是禁止的。需要配置<a href="#reg-3.5-bus_config1">BUS_CONFIG1</a>[wr_enable] 为1，使能总线写操作。

如果需要调整默认配置，请参见"3.3.3 初始化流程"。

<a id="3.3.8"></a>
### 通过DMA方式读写Flash操作流程

DMA操作流程如下：

步骤 1 如需调整总线操作方式时序配置，请参见 "3.3.3 初始化流程"。

步骤 2 写<a href="#reg-3.5-bus_dma_mem_saddr">BUS_DMA_MEM_SADDR</a>，配置DMA操作的内存端起始地址；写<a href="#reg-3.5-bus_dma_flash_saddr">BUS_DMA_FLASH_SADDR</a>，配置Flash端起始地址（Flash偏移地址）；写BUS_DMA_LENBUS_DMA_LEN，配置数据长度。

步骤 3 写<a href="#reg-3.5-bus_dma_ctrl">BUS_DMA_CTRL</a>，配置读写方向，选择Flash片选1。

步骤 4 写<a href="#reg-3.5-bus_dma_ctrl">BUS_DMA_CTRL</a>[start]为1，使能DMA操作。

步骤 5 等待dma_done中断触发（中断方式）或轮询DMA操作完成 （<a href="#reg-3.5-bus_dma_ctrl">BUS_DMA_CTRL</a>[start] 变为0）。

- DMA操作时可以同时Flash寄存器读命令操作。
- DMA操作时可以同时通过AHB Slave直接访问Flash，但需保证中间不修改总线操作相关配置。
- DMA操作时需要保证首地址4Byte对齐。

----结束

<a id="3.4"></a>
## 寄存器概览

SFC寄存器概览如表3-1所示。

表3-1 SFC寄存器概览（基址是0x4800_0000）

| 偏移地址 | 名称 | 描述 |
| --- | --- | --- |
| 0x0100 | <a href="#reg-3.5-global_config">GLOBAL_CONFIG</a> | 全局配置寄存器。 |
| 0x0110 | <a href="#reg-3.5-timing">TIMING</a> | Timing配置寄存器。 |
| 0x0120 | <a href="#reg-3.5-int_raw_status">INT_RAW_STATUS</a> | 中断原始状态寄存器。 |
| 0x0124 | <a href="#reg-3.5-int_status">INT_STATUS</a> | 经过屏蔽处理的中断状态寄存器。 |
| 0x0128 | <a href="#reg-3.5-int_mask">INT_MASK</a> | 中断屏蔽寄存器。 |
| 0x012C | <a href="#reg-3.5-int_clear">INT_CLEAR</a> | 中断清除寄存器。 |
| 0x0130 | <a href="#reg-3.5-soft_rst_mask">SOFT_RST_MASK</a> | 软复位寄存器屏蔽位。 |
| 0x0200 | <a href="#reg-3.5-bus_config1">BUS_CONFIG1</a> | 总线操作方式配置1寄存器。 |
| 0x0204 | <a href="#reg-3.5-bus_config2">BUS_CONFIG2</a> | 总线操作方式配置2寄存器。 |
| 0x0240 | <a href="#reg-3.5-bus_dma_ctrl">BUS_DMA_CTRL</a> | DMA操作控制寄存器。 |
| 0x0244 | <a href="#reg-3.5-bus_dma_mem_saddr">BUS_DMA_MEM_SADDR</a> | DMA操作DDR起始地址寄存器。 |
| 0x0248 | <a href="#reg-3.5-bus_dma_flash_saddr">BUS_DMA_FLASH_SADDR</a> | DMA操作Flash起始地址寄存器。 |
| 0x024C | <a href="#reg-3.5-bus_dma_len">BUS_DMA_LEN</a> | DMA操作搬运数据长度寄存器。 |
| 0x0250 | <a href="#reg-3.5-bus_dma_ahb_ctrl">BUS_DMA_AHB_CTRL</a> | DMA操作AHB时burst操作方式选择控制寄存器。 |
| 0x0300 | <a href="#reg-3.5-cmd_config">CMD_CONFIG</a> | 命令操作方式配置寄存器。 |
| 0x0308 | <a href="#reg-3.5-cmd_ins">CMD_INS</a> | 命令操作方式指令寄存器。 |
| 0x030C | <a href="#reg-3.5-cmd_addr">CMD_ADDR</a> | 命令操作方式地址寄存器。 |
| 0x0400＋4×n | <a href="#reg-3.5-cmd_databuf_n">CMD_DATABUF_N</a> | 命令操作方式数据Buffer寄存器。 |
| 0x1000＋4×n | <a href="#reg-3.5-apc_cfg_start_addr">APC_CFG_START_ADDR</a> | FAPC鉴权。 |
| 0x1040＋4×n | <a href="#reg-3.5-apc_cfg_end_addr">APC_CFG_END_ADDR</a> | FAPC鉴权。 |
| 0x1180 | <a href="#reg-3.5-sfc_fapc_dec_auth_cfg">SFC_FAPC_DEC_AUTH_CFG</a> | FAPC鉴权。 |
| 0x1200 | <a href="#reg-3.5-sfc_fapc_saddr_status">SFC_FAPC_SADDR_STATUS</a> | FAPC鉴权。 |
| 0x1204 | <a href="#reg-3.5-sfc_fapc_eaddr_status">SFC_FAPC_EADDR_STATUS</a> | FAPC鉴权。 |
| 0x1208 | <a href="#reg-3.5-sfc_apc_err_int">SFC_APC_ERR_INT</a> | FAPC鉴权。 |
| 0x120C | <a href="#reg-3.5-sfc_apc_clr">SFC_APC_CLR</a> | FAPC鉴权。 |
| 0x1220 | <a href="#reg-3.5-fapc_one_way_lock">FAPC_ONE_WAY_LOCK</a> | FAPC鉴权锁定寄存器。 |
| 0x1300 | <a href="#reg-3.5-lea_lp_en">LEA_LP_EN</a> | LEA控制。 |
| 0x1304 | <a href="#reg-3.5-lea_dfx_info">LEA_DFX_INFO</a> | LEA DFX。 |
| 0x1600 | <a href="#reg-3.5-lea_iv_vld">LEA_IV_VLD</a> | LEA控制。 |
| 0x1640 | <a href="#reg-3.5-lea_iv_acpu_start_addr_0">LEA_IV_ACPU_START_ADDR_0</a> | LEA IV解密起始地址寄存器0。 |
| 0x1644 | <a href="#reg-3.5-lea_iv_acpu_start_addr_1">LEA_IV_ACPU_START_ADDR_1</a> | LEA IV解密起始地址寄存器1。 |
| 0x1648 | <a href="#reg-3.5-lea_iv_acpu_start_addr_2">LEA_IV_ACPU_START_ADDR_2</a> | LEA IV解密起始地址寄存器2。 |
| 0x164C | <a href="#reg-3.5-lea_iv_acpu_start_addr_3">LEA_IV_ACPU_START_ADDR_3</a> | LEA IV解密起始地址寄存器3。 |

SFC寄存器偏移地址中变量的取值范围和含义如表3-2所示。

表3-2 SFC寄存器偏移地址变量表

| 变量名称 | 取值范围 | 描述 |
| --- | --- | --- |
| n | 0～3 | FLASH解密地址的区间个数。 |

<a id="3.5"></a>
## 寄存器描述

<a id="reg-3.5-global_config"></a>
### GLOBAL_CONFIG

GLOBAL_CONFIG为全局配置寄存器。

Offset Address： 0x0100 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:6] | - | reserved | 保留。 | 0x000000 |
| [5:3] | RW | rd_delay | SPI读出数据延迟周期个数。000： 0.5～1个时钟周期（默认值）；001： 1～1.5个时钟周期；010： 1.5～2个时钟周期；011： 2～2.5个时钟周期；100： 2.5～3个时钟周期；101： 3～3.5个时钟周期；110： 3.5～4个时钟周期；111： 不支持，（按照"110"含义处理）。 | 0x0 |
| [2] | RW | flash_addr_mode | SPI地址模式。0： 3byte寻址模式（默认值）；1： 4byte寻址模式。注意：<a href="#reg-3.5-cmd_config">CMD_CONFIG</a>[start]为1时写无效。 | 0x0 |
| [1] | RW | wp_en | 硬件写保护使能（写保护管脚）。0： 禁止；1： 使能。 | 0x0 |
| [0] | RW | mode | SPI模式设置。0： 支持Mode0；1： 支持Mode3。 | 0x0 |

<a id="reg-3.5-timing"></a>
### TIMING

TIMING为Timing配置寄存器。

Offset Address： 0x0110 Total Reset Value： 0x0000_660F

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:15] | - | reserved | 保留。 | 0x00000 |
| [14:12] | RW | tcsh | 片选保持时间。0x0～0x7： （n＋1）个时钟周期。例如： 0x6表示7个时钟周期。 | 0x6 |
| [11] | - | reserved | 保留。 | 0x0 |
| [10:8] | RW | tcss | 片选建立时间。0x0～0x7：（n＋1）个时钟周期。例如：0x6表示7个时钟周期。 | 0x6 |
| [7:4] | - | reserved | 保留。 | 0x0 |
| [3:0] | RW | tshsl | 设置2次Flash操作之间的时间间隔。0x0～0xF：（n＋2）个时钟周期。例如：0xF表示17个时钟周期。 | 0xF |

<a id="reg-3.5-int_raw_status"></a>
### INT_RAW_STATUS

INT_RAW_STATUS为中断原始状态寄存器。

Offset Address： 0x0120 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:2] | - | reserved | 保留。 | 0x00000000 |
| [1] | RO | dma_done_int_raw_status | DMA操作完成中断原始状态（未经过屏蔽）。0： 未完成；1： 已完成。 | 0x0 |
| [0] | RO | cmd_op_end_raw_status | 指令操作结束原始中断状态（未经过屏蔽）。0： 未完成；1： 已完成。 | 0x0 |

<a id="reg-3.5-int_status"></a>
### INT_STATUS

INT_STATUS为经过屏蔽处理的中断状态寄存器。

Offset Address： 0x0124 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:2] | - | reserved | 保留。 | 0x00000000 |
| [1] | RO | dma_done_int_status | DMA操作完成中断原始状态（经过屏蔽）。0：未完成；1：已完成。 | 0x0 |
| [0] | RO | cmd_op_end_status | 指令操作结束中断状态（经过屏蔽）。0：未完成；1：已完成。 | 0x0 |

<a id="reg-3.5-int_mask"></a>
### INT_MASK

INT_MASK为中断屏蔽寄存器。

Offset Address： 0x0128 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:2] | - | reserved | 保留。 | 0x00000000 |
| [1] | RW | dma_done_int_mask | DMA操作完成中断屏蔽位。0： 屏蔽；1： 不屏蔽。 | 0x0 |
| [0] | RW | cmd_op_end_int_mask | 指令操作结束中断屏蔽位。0： 屏蔽；1： 不屏蔽。 | 0x0 |

<a id="reg-3.5-int_clear"></a>
### INT_CLEAR

INT_CLEAR为中断清除寄存器。

Offset Address： 0x012C Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:2] | - | reserved | 保留。 | 0x00000000 |
| [1] | WO | dma_done_int_clr | DMA操作完成中断清除位，向该位写1将清除<a href="#reg-3.5-int_status">INT_STATUS</a>[dma_done_int_status]和<a href="#reg-3.5-int_raw_status">INT_RAW_STATUS</a>[dma_done_int_raw_status]。0：不清除；1：清除。注意：清除操作完成后该位自动返回0。 | 0x0 |
| [0] | WO | cmd_op_end_int_clr | 指令操作结束中断清除位，向该位写1将清除<a href="#reg-3.5-int_status">INT_STATUS</a>[cmd_op_end_status]和<a href="#reg-3.5-int_raw_status">INT_RAW_STATUS</a>[cmd_op_end_raw_status]。0：不清除；1：清除。注意：清除操作完成后该位自动返回0。 | 0x0 |

<a id="reg-3.5-soft_rst_mask"></a>
### SOFT_RST_MASK

SOFT_RST_MASK为软复位寄存器屏蔽位

Offset Address： 0x0130 Total Reset Value： 0x0000_0001

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:1] | - | reserved | 保留。 | 0x00000000 |
| [0] | RW | sfc_bus_soft_rst_mask | SFC总线时钟域数字逻辑软复位屏蔽位：0： 软复位可以正常生效；1： 软复位被屏蔽，不会生效。 | 0x1 |

<a id="reg-3.5-bus_config1"></a>
### BUS_CONFIG1

BUS_CONFIG1为总线操作方式配置1寄存器。

Offset Address： 0x0200 Total Reset Value： 0x8080_0300

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31] | RW | rd_enable | 总线读使能。0：禁止；1：使能。 | 0x1 |
| [30] | RW | wr_enable | 总线写使能。0：禁止；1：使能。 | 0x0 |
| [29:22] | RW | wr_ins | 写指令。 | 0x02 |
| [21:19] | RW | wr_dummy_bytes | 总线写操作DummyByte。000：没有DummyByte；001：1Byte；010：2Byte；......111：7Byte。 | 0x0 |
| [18:16] | RW | wr_mem_if_type | 总线写操作指定连接的SPI FLASH接口类型。000：Standard SPI接口类型；001： Dual-Input/Dual-Output SPI；010： Dual-I/O SPI；011： Full DIO SPI；100： 保留；101： Quad-Input/Dual-Output SPI；110： Quad-I/O SPI；111： Full QIO SPI。 | 0x0 |
| [15:8] | RW | rd_ins | 读指令。 | 0x03 |
| [7:6] | RW | rd_prefetch_cnt | 总线访问Flash方式（非定长读）预取周期。00： 不预取（默认值）；01： 预取1个时钟周期数据；10： 预取2个时钟周期数据；11： 预取3个时钟周期数据。 | 0x0 |
| [5:3] | RW | rd_dummy_bytes | 总线读操作DummyByte。00： 没有DummyByte；001： 1Byte010： 2Byte；......111： 7Byte。 | 0x0 |
| [2:0] | RW | rd_mem_if_type | 总线读操作指定连接的SPI FLASH接口类型。000： Standard SPI接口类型；001： Dual-Input/Dual-OutputSPI；010： Dual-I/O SPI；101： Quad-Input/Dual-Output SPI；110： Quad-I/O SPI；其他： 保留。 | 0x0 |

<a id="reg-3.5-bus_config2"></a>
### BUS_CONFIG2

BUS_CONFIG2为总线操作方式配置2寄存器。

Offset Address： 0x0204 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:3] | - | reserved | 保留。 | 0x00000000 |
| [2:0] | RW | wip_locate | WIP（Write In Progress） 位于Flash状态寄存器的位置。000： WIP位于Flash状态寄存器的bit[0]（默认值）；001： WIP位于Flash状态寄存器的bit[1]；010： WIP位于Flash状态寄存器的bit[2]；011： WIP位于Flash状态寄存器的bit[3]；100： WIP位于Flash状态寄存器的bit[4]；101： WIP位于Flash状态寄存器的bit[5]；110： WIP位于Flash状态寄存器的bit[6]；111： WIP位于Flash状态寄存器的bit[7]。 | 0x0 |

<a id="reg-3.5-bus_dma_ctrl"></a>
### BUS_DMA_CTRL

BUS_DMA_CTRL为DMA操作控制寄存器。

Offset Address： 0x0240 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:5] | - | reserved | 保留。 | 0x0000000 |
| [4] | RW | dma_sel_cs | DMA操作指定片选。0： 片选0；1： 片选1。 | 0x0 |
| [3:2] | - | reserved | 保留。 | 0x0 |
| [1] | RW | dma_rw | DMA读写指示。0： 写操作；1： 读操作。 | 0x0 |
| [0] | RW | dma_start | DMA传输使能控制。0： 无操作；1： 开始DMA操作。注意： DMA传输完成自动回0。 | 0x0 |

<a id="reg-3.5-bus_dma_mem_saddr"></a>
### BUS_DMA_MEM_SADDR

BUS_DMA_MEM_SADDR为DMA操作芯片内存起始地址寄存器。

Offset Address： 0x0244 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RW | dma_mem_saddr | DMA操作memory起始地址。WS63配置值应在0x0010_0000～0x00BF_FFFF之间。 | 0x00000000 |

<a id="reg-3.5-bus_dma_flash_saddr"></a>
### BUS_DMA_FLASH_SADDR

BUS_DMA_FLASH_SADDR为DMA操作Flash起始地址寄存器。

Offset Address： 0x0248 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RW | dma_flash_saddr | DMA操作Flash起始地址。 | 0x00000000 |

<a id="reg-3.5-bus_dma_len"></a>
### BUS_DMA_LEN

BUS_DMA_LEN为DMA操作搬运数据长度寄存器。

Offset Address： 0x024C Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:30] | - | reserved | 保留。 | 0x0 |
| [29:0] | RW | dma_len | DMA操作数据搬运长度（n＋1），单位：byte。例如：6表示长度为7byte。 | 0x00000000 |

<a id="reg-3.5-bus_dma_ahb_ctrl"></a>
### BUS_DMA_AHB_CTRL

BUS_DMA_AHB_CTRL为DMA操作AHB时burst操作方式选择控制寄存器。

Offset Address： 0x0250 Total Reset Value： 0x0000_0007

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:3] | - | reserved | 保留。 | 0x00000000 |
| [2] | RW | incr16_en | INC16 burst类型使能。0：禁止；1：使能。 | 0x1 |
| [1] | RW | incr8_en | INC8 burst类型使能。0：禁止；1：使能。 | 0x1 |
| [0] | RW | incr4_en | INC4 burst类型使能。0：禁止；1：使能。 | 0x1 |

<a id="reg-3.5-cmd_config"></a>
### CMD_CONFIG

CMD_CONFIG为命令操作方式配置寄存器。

Offset Address： 0x0300 Total Reset Value： 0x0000_7E00

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:20] | - | reserved | 保留。 | 0x000 |
| [19:17] | RW | mem_if_type | 指定寄存器命令操作方式连接的SPI FLASH接口类型。000： Standard SPI接口类型；001： Dual-Input/Dual-Output SPI；010： Dual-I/O SPI；101： Quad-Input/Dual-Output SPI；110： Quad-I/O SPI；其他： 保留。 | 0x0 |
| [16:15] | - | reserved | 保留。 | 0x0 |
| [14:9] | RW | data_cnt | 读写数据长度（单位：Byte）。0x00～0x3F：（n＋1）Byte。例如：0x3F表示64Byte。 | 0x3F |
| [8] | RW | rw | 标识此次操作数据读写，需[data_en]为1。0：写，有发送数据；1：读，有返回数据。 | 0x0 |
| [7] | RW | data_en | 标识此次操作是否有数据。0：无数据；1：有数据。 | 0x0 |
| [6:4] | RW | dummy_byte_cnt | 寄存器命令操作方式DummyByte。000：没有DummyByte；001：1Byte；010：2Byte；......111：7Byte。 | 0x0 |
| [3] | RW | addr_en | 此次操作是否有地址。0：无地址；1：有地址。 | 0x0 |
| [2] | - | reserved | 保留。 | 0x0 |
| [1] | RW | sel_cs | 片选选择操作。0：选择片选0进行操作；1：选择片选1进行操作。 | 0x0 |
| [0] | RW | start | 标识指令操作开始。0：结束；1：开始。注意：此次操作完成后该位自动回0。 | 0x0 |

<a id="reg-3.5-cmd_ins"></a>
### CMD_INS

CMD_INS为命令操作方式指令寄存器。

Offset Address： 0x0308 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | - | reserved | 保留。 | 0x000000 |
| [7:0] | RW | reg_ins | 寄存器访问Flash方式下的指令码。 | 0x00 |

<a id="reg-3.5-cmd_addr"></a>
### CMD_ADDR

CMD_ADDR为命令操作方式地址寄存器。

Offset Address： 0x030C Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:30] | - | reserved | 保留。 | 0x0 |
| [29:0] | RW | cmd_addr | 寄存器访问Flash方式下的操作地址。 | 0x00000000 |

<a id="reg-3.5-cmd_databuf_n"></a>
### CMD_DATABUF_N

CMD_DATABUF_N为命令操作方式数据Buffer寄存器。

Offset Address： 0x0400＋4×n Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RW | cmd_databuf_n | 寄存器访问Flash方式下第n数据Buffer（n: 0～15）。 | 0x00000000 |

<a id="reg-3.5-apc_cfg_start_addr"></a>
### APC_CFG_START_ADDR

APC_CFG_START_ADDR为FAPC鉴权。

Offset Address： 0x1000＋4×n Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | apc_cfg_start_addr_n | flash存储区域划分4段地址区间，每段起始和截止地址位宽32bit，该寄存器表示第n段起始地址的高24bit (n: 0～3)。注意：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |

<a id="reg-3.5-apc_cfg_end_addr"></a>
### APC_CFG_END_ADDR

APC_CFG_END_ADDR为FAPC鉴权。

Offset Address： 0x1040＋4×n Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | apc_cfg_end_addr_n | flash存储区域划分4段地址区间，每段起始和截止地址位宽32bit，该寄存器表示第n段截止地址的高24bit（n: 0～3）注：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |

<a id="reg-3.5-sfc_fapc_dec_auth_cfg"></a>
### SFC_FAPC_DEC_AUTH_CFG

SFC_FAPC_DEC_AUTH_CFG为FAPC鉴权。

Offset Address： 0x1180 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RW | sfc_fapc_dec_auth_cfg | flash存储区域划分4段区间，该寄存器从低位开始，每2bit（n: 0～3）表示一段区间的数据处理方式：0b00：透传；0b10：解密；其他：保留。 | 0x00000000 |

<a id="reg-3.5-sfc_fapc_saddr_status"></a>
### SFC_FAPC_SADDR_STATUS

SFC_FAPC_SADDR_STATUS为FAPC鉴权。

Offset Address： 0x1200 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RO | sfc_fapc_saddr_status | APC状态检测寄存器：基于burst起始地址检测[31:31]：是否出现过鉴权不通过历史态；[30:28]：当前master对应MID的低3bit；[27:27]：1表示当前是写操作，0表示读；[26:0]：当前访问flash地址。 | 0x00000000 |

<a id="reg-3.5-sfc_fapc_eaddr_status"></a>
### SFC_FAPC_EADDR_STATUS

SFC_FAPC_EADDR_STATUS为FAPC鉴权。

Offset Address： 0x1204 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RO | sfc_fapc_eaddr_status | APC状态检测寄存器：基于burst结束地址检测[31:31]：是否出现过鉴权不通过历史态；[30:28]：当前master对应MID的低3bit；[27:27]： 1表示当前是写操作，0表示读；[26:0]： 当前访问flash地址。 | 0x00000000 |

<a id="reg-3.5-sfc_apc_err_int"></a>
### SFC_APC_ERR_INT

SFC_APC_ERR_INT为FAPC鉴权

Offset Address： 0x1208 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:1] | - | reserved | 保留。 | 0x00000000 |
| [0] | RO | sfc_apc_err_int | 鉴权错误中断状态：0： 鉴权全部通过；1： 出现鉴权不通过，上报中断。 | 0x0 |

<a id="reg-3.5-sfc_apc_clr"></a>
### SFC_APC_CLR

SFC_APC_CLR为FAPC鉴权

Offset Address： 0x120C Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:1] | - | reserved | 保留。 | 0x00000000 |
| [0] | W1_PULSE | sfc_apc_clr | 鉴权错误中断状态清除：0：无效；1：清除sfc_apc_err_int中断。 | 0x0 |

<a id="reg-3.5-fapc_one_way_lock"></a>
### FAPC_ONE_WAY_LOCK

FAPC_ONE_WAY_LOCK为FAPC鉴权锁定寄存器

Offset Address： 0x1220 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:16] | - | reserved | 保留。 | 0x0000 |
| [15:0] | RW | sfc_fpac_one way_lock | 鉴权寄存器锁定位，bit0～3分别表示16段地址区间对应的鉴权寄存器锁定位。0：对应鉴权寄存器可读可写；1：对应鉴权寄存器不可改写，只能读。注意：默认值是0，一旦写1，只有整harden复位才能归0。 | 0x0000 |

<a id="reg-3.5-lea_lp_en"></a>
### LEA_LP_EN

LEA_LP_EN为LEA控制

Offset Address： 0x1300 Total Reset Value： 0x0000_0001

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:1] | - | reserved | 保留。 | 0x00000000 |
| [0] | RW | lea_lp_en | AES模块低功耗模式配置：0： 关闭LEA低功耗配置；1： 使能LEA低功耗。 | 0x1 |

<a id="reg-3.5-lea_dfx_info"></a>
### LEA_DFX_INFO

LEA_DFX_INFO is LEA DFX

Offset Address： 0x1304 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:0] | RO | lea_dfx_info | AES模块DFX信息观测寄存器。 | 0x00000000 |

<a id="reg-3.5-lea_iv_vld"></a>
### LEA_IV_VLD

LEA_IV_VLD为LEA控制。

Offset Address： 0x1600 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:1] | - | reserved | 保留。 | 0x00000000 |
| [0] | RW | lea_iv_vld | AES IV值有效寄存器。配置AES_IV之后，需要将该寄存器写1，是配置同步生效，做完时钟同步之后，该信号自动归0。 | 0x0 |

<a id="reg-3.5-lea_iv_acpu_start_addr_0"></a>
### LEA_IV_ACPU_START_ADDR_0

LEA_IV_ACPU_START_ADDR_0为LEA IV解密起始地址寄存器0。

Offset Address： 0x1640 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | iv_start_addr_0 | ACPU IV解密区域0的起始地址，该寄存器表示起始地址的高24bit。注意：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |

<a id="reg-3.5-lea_iv_acpu_start_addr_1"></a>
### LEA_IV_ACPU_START_ADDR_1

LEA_IV_ACPU_START_ADDR_1为LEA IV解密起始地址寄存器1。

Offset Address： 0x1644 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | iv_start_addr_1 | ACPU IV解密区域1的起始地址，该寄存器表示起始地址的高24bit。注意：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |

<a id="reg-3.5-lea_iv_acpu_start_addr_2"></a>
### LEA_IV_ACPU_START_ADDR_2

LEA_IV_ACPU_START_ADDR_2为LEA IV解密起始地址寄存器2。

Offset Address： 0x1648 Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | iv_start_addr_2 | ACPU IV解密区域2的起始地址，该寄存器表示起始地址的高24bit。注意：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |

<a id="reg-3.5-lea_iv_acpu_start_addr_3"></a>
### LEA_IV_ACPU_START_ADDR_3

LEA_IV_ACPU_START_ADDR_3为LEA IV解密起始地址寄存器3。

Offset Address： 0x164C Total Reset Value： 0x0000_0000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| [31:8] | RW | iv_start_addr_3 | ACPU IV解密区域3的起始地址，该寄存器表示起始地址的高24bit。注意：该处为绝对地址，包含总线基地址。 | 0x000000 |
| [7:0] | - | reserved | 保留。 | 0x00 |