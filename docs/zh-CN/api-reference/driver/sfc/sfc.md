# SFC

SFC (Serial Flash Controller) 用于读写外部 NOR Flash，支持标准 SPI (Serial Peripheral Interface) 和 QSPI (Quad Serial Peripheral Interface) 多线模式（单线/双线/四线）。提供寄存器模式（单次读写擦除，扇区对齐）和 DMA (Direct Memory Access) 模式（批量传输）。

NOR Flash 遵循"先擦除后写入"原则——bit 只能 1→0，擦除将整块恢复为全 1。擦除粒度分为扇区/块/全片。适用固件存储、文件系统、字库等外部存储场景。

**头文件清单**

```c
#include "include/driver/sfc.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_sfc_init](#uapi_sfc_init) | 初始化并配置SFC，未识别的Flash默认采用单线读写512KB |
| [uapi_sfc_init_rom](#uapi_sfc_init_rom) | 初始化并配置SFC，ROM (Read-Only Memory) 默认采用单线读写512KB |
| [uapi_sfc_deinit](#uapi_sfc_deinit) | 去初始化SFC |
| [uapi_sfc_reg_read](#uapi_sfc_reg_read) | 寄存器模式读取Flash数据 |
| [uapi_sfc_reg_write](#uapi_sfc_reg_write) | 寄存器模式写入Flash数据 |
| [uapi_sfc_reg_erase](#uapi_sfc_reg_erase) | 寄存器模式擦除Flash指定区域 |
| [uapi_sfc_reg_erase_chip](#uapi_sfc_reg_erase_chip) | 寄存器模式擦除整片Flash |
| [uapi_sfc_reg_other_flash_opt](#uapi_sfc_reg_other_flash_opt) | 寄存器模式读写Flash属性 |
| [uapi_sfc_dma_read](#uapi_sfc_dma_read) | DMA模式读取Flash数据 |
| [uapi_sfc_dma_write](#uapi_sfc_dma_write) | DMA模式写入Flash数据 |
| [uapi_sfc_suspend](#uapi_sfc_suspend) | 挂起SFC |
| [uapi_sfc_resume](#uapi_sfc_resume) | 恢复SFC |

## Functions

### uapi_sfc_init <a id="uapi_sfc_init"></a>

```c
errcode_t uapi_sfc_init(sfc_flash_config_t *config)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 初始化并配置SFC，未识别的Flash默认采用单线读写512KB配置
- 读取Flash ID，根据配置的读写类型构建SPI命令集，完成Flash映射地址和映射大小的校验
- 重复调用时返回ERRCODE_SUCC，不重复初始化

**前置条件**

- SFC模块尚未初始化（首次调用）
- 入参config不为NULL，且指向的sfc_flash_config_t结构体成员已正确赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | [sfc_flash_config_t](#struct_sfc_flash_config) * | SFC初始化配置参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_open_ota_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_open_ota_adapter.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_ALREADY_INIT | 特性宏 | 支持SFC已初始化跳过寄存器配置特性 | n |

### uapi_sfc_init_rom <a id="uapi_sfc_init_rom"></a>

```c
errcode_t uapi_sfc_init_rom(sfc_flash_config_t *config)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- ROM模式下初始化并配置SFC，默认采用单线读写512KB配置
- 使用未识别Flash的默认配置构建SPI命令集，完成Flash映射地址和映射大小的校验
- 重复调用时返回ERRCODE_SUCC，不重复初始化

**前置条件**

- SFC模块尚未初始化（首次调用）
- 入参config不为NULL，且指向的sfc_flash_config_t结构体成员已正确赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | [sfc_flash_config_t](#struct_sfc_flash_config) * | SFC初始化配置参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_sfc_deinit <a id="uapi_sfc_deinit"></a>

```c
void uapi_sfc_deinit(void)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 去初始化SFC，释放已初始化的资源
- 若SFC尚未初始化则直接返回，不执行操作

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_ALREADY_INIT | 特性宏 | 支持SFC已初始化跳过寄存器去初始化特性 | n |

### uapi_sfc_reg_read <a id="uapi_sfc_reg_read"></a>

```c
errcode_t uapi_sfc_reg_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 提供寄存器模式读功能，读取的数据按字节存入read_buffer
- 支持非4字节对齐地址和长度的读取，内部进行对齐处理
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 入参read_buffer不为NULL，且指向内存空间已申请成功，长度不小于read_size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flash_addr | uint32_t | 数据所在的Flash首地址 | 0 ~ chip_size - 1 |
| read_buffer | uint8_t * | 用于接收数据的buffer | 非NULL，长度不小于read_size |
| read_size | uint32_t | 读取的字节数 | flash_addr + read_size ≤ chip_size |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| read_buffer | uint8_t * | 读取的Flash数据按字节存入该缓冲区 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [sfc_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc/sfc_demo.c)


### uapi_sfc_reg_write <a id="uapi_sfc_reg_write"></a>

```c
errcode_t uapi_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 提供寄存器模式写功能，预计写入的数据按字节存入write_data
- 支持非4字节对齐地址和长度的写入，内部进行对齐处理（先读后写）
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 入参write_data不为NULL，且指向内存空间已申请成功，长度不小于write_size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flash_addr | uint32_t | 目标Flash首地址 | 0 ~ chip_size - 1 |
| write_data | uint8_t * | 预计写入的数据 | 非NULL，长度不小于write_size |
| write_size | uint32_t | 写入数据的字节数 | flash_addr + write_size ≤ chip_size |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [sfc_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc/sfc_demo.c)


### uapi_sfc_reg_erase <a id="uapi_sfc_reg_erase"></a>

```c
errcode_t uapi_sfc_reg_erase(uint32_t flash_addr, uint32_t erase_size)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 使用寄存器模式对Flash进行擦除，采用贪心算法选择最优擦除命令
- 未使能写回时强制要求地址和大小按扇区（4KB）对齐
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 未使能CONFIG_SFC_ALLOW_ERASE_WRITEBACK时，flash_addr和erase_size必须按4KB对齐

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flash_addr | uint32_t | 擦除的首地址 | 0 ~ chip_size - 1，未使能写回时需4KB对齐 |
| erase_size | uint32_t | 擦除的Flash空间大小 | flash_addr + erase_size ≤ chip_size，未使能写回时需4KB对齐 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 擦除成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [sfc_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc/sfc_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_ALLOW_ERASE_WRITEBACK | 特性宏 | 支持任意地址擦除写回特性 | n |

### uapi_sfc_reg_erase_chip <a id="uapi_sfc_reg_erase_chip"></a>

```c
errcode_t uapi_sfc_reg_erase_chip(void)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 使用寄存器模式对整片Flash进行擦除
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 整片擦除成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_sfc_reg_other_flash_opt <a id="uapi_sfc_reg_other_flash_opt"></a>

```c
errcode_t uapi_sfc_reg_other_flash_opt(sfc_flash_op_t cmd_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 使用寄存器模式对Flash属性进行读写操作
- 支持通过SPI指令读取或配置Flash状态寄存器
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 入参buffer不为NULL，入参length不超过18

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_type | [sfc_flash_op_t](#enum_sfc_flash_op) | 设置指令的读写类型 | READ_TYPE(0x0) / WRITE_TYPE(0x1) |
| cmd | uint8_t | SPI指令 | Flash支持的指令编码 |
| buffer | uint8_t * | 数据缓冲区 | 非NULL |
| length | uint32_t | 需要读/写的数据长度 | 0 ~ 18 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer | uint8_t * | 当cmd_type为READ_TYPE时，读取的数据存入该缓冲区 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 操作成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_sfc_dma_read <a id="uapi_sfc_dma_read"></a>

```c
errcode_t uapi_sfc_dma_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 提供DMA模式读功能，读取的数据按字节存入read_buffer
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 已使能CONFIG_SFC_SUPPORT_DMA配置
- 入参read_buffer不为NULL，且指向内存空间已申请成功，长度不小于read_size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flash_addr | uint32_t | 数据的Flash首地址 | 0 ~ chip_size - 1 |
| read_buffer | uint8_t * | 用于接收数据的buffer | 非NULL，长度不小于read_size |
| read_size | uint32_t | 读取的字节数 | flash_addr + read_size ≤ chip_size |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| read_buffer | uint8_t * | 读取的Flash数据按字节存入该缓冲区 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA读取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_SUPPORT_DMA | 特性宏 | 支持SFC DMA读写功能 | y |

### uapi_sfc_dma_write <a id="uapi_sfc_dma_write"></a>

```c
errcode_t uapi_sfc_dma_write(uint32_t flash_addr, uint8_t *write_buffer, uint32_t write_size)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 提供DMA模式写功能，预计写入的数据按字节存入write_buffer
- 不允许在中断中调用

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 已使能CONFIG_SFC_SUPPORT_DMA配置
- 入参write_buffer不为NULL，且指向内存空间已申请成功，长度不小于write_size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flash_addr | uint32_t | 目标Flash首地址 | 0 ~ chip_size - 1 |
| write_buffer | uint8_t * | 预计写入的数据 | 非NULL，长度不小于write_size |
| write_size | uint32_t | 写入数据的字节数 | flash_addr + write_size ≤ chip_size |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA写入成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_SUPPORT_DMA | 特性宏 | 支持SFC DMA读写功能 | y |

### uapi_sfc_suspend <a id="uapi_sfc_suspend"></a>

```c
errcode_t uapi_sfc_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 挂起SFC，进入低功耗状态

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 已使能CONFIG_SFC_SUPPORT_LPM (Low Power Management)配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起所需要的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 挂起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_SUPPORT_LPM | 特性宏 | 支持SFC低功耗挂起/恢复特性 | 由构建目标决定 |

### uapi_sfc_resume <a id="uapi_sfc_resume"></a>

```c
errcode_t uapi_sfc_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/sfc.h"
```

**功能说明**

- 恢复SFC，从低功耗状态恢复

**前置条件**

- SFC模块已通过uapi_sfc_init()或uapi_sfc_init_rom()初始化完成
- 已使能CONFIG_SFC_SUPPORT_LPM配置
- SFC已通过uapi_sfc_suspend()挂起

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复所需要的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 恢复成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SFC_SUPPORT_LPM | 特性宏 | 支持SFC低功耗挂起/恢复特性 | 由构建目标决定 |

## Enumerations

### sfc_read_if_t <a id="enum_sfc_read_if"></a>

```c
typedef enum sfc_read_if {
    STANDARD_READ = 0x0,               /*!< 以标准SPI模式执行读操作(指令一般为0x03)。 */
    FAST_READ = 0x1,                   /*!< 以标准SPI模式执行快速读操作(指令一般为0x0B)。 */
    FAST_READ_DUAL_OUTPUT = 0x2,       /*!< 以双线Out SPI模式执行读操作(指令一般为0x3B)。 */
    FAST_READ_DUAL_IO = 0x3,           /*!< 以双线In/Out SPI模式执行读操作(指令一般为0xBB)。 */
    FAST_READ_QUAD_OUTPUT = 0x4,       /*!< 以四线Out SPI模式执行读操作(指令一般为0x6B)。 */
    FAST_READ_QUAD_IO = 0x5            /*!< 以四线In/Out SPI模式执行读操作(指令一般为0xEB)。 */
} sfc_read_if_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| STANDARD_READ | 0 | 以标准SPI模式执行读操作(指令一般为0x03) |
| FAST_READ | 1 | 以标准SPI模式执行快速读操作(指令一般为0x0B) |
| FAST_READ_DUAL_OUTPUT | 2 | 以双线Out SPI模式执行读操作(指令一般为0x3B) |
| FAST_READ_DUAL_IO | 3 | 以双线In/Out SPI模式执行读操作(指令一般为0xBB) |
| FAST_READ_QUAD_OUTPUT | 4 | 以四线Out SPI模式执行读操作(指令一般为0x6B) |
| FAST_READ_QUAD_IO | 5 | 以四线In/Out SPI模式执行读操作(指令一般为0xEB) |

### sfc_write_if_t <a id="enum_sfc_write_if"></a>

```c
typedef enum sfc_write_if {
    WRITE_DISABLE = 0x0,               /*!< 写操作禁止。 */
    PAGE_PROGRAM = 0x1,                /*!< 以标准SPI模式执行写操作。 */
    DUAL_INPUT_PAGE_PROGRAM = 0x2,     /*!< 以双线In SPI模式执行写操作。 */
    DUAL_IO_PAGE_PROGRAM = 0x3,        /*!< 以双线I/O SPI模式执行写操作。 */
    QUAD_INPUT_PAGE_PROGRAM = 0x4,     /*!< 以四线In SPI模式执行写操作。 */
    QUAD_IO_PAGE_PROGRAM = 0x5         /*!< 以四线I/O SPI模式执行写操作。 */
} sfc_write_if_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| WRITE_DISABLE | 0 | 写操作禁止 |
| PAGE_PROGRAM | 1 | 以标准SPI模式执行写操作 |
| DUAL_INPUT_PAGE_PROGRAM | 2 | 以双线In SPI模式执行写操作 |
| DUAL_IO_PAGE_PROGRAM | 3 | 以双线I/O SPI模式执行写操作 |
| QUAD_INPUT_PAGE_PROGRAM | 4 | 以四线In SPI模式执行写操作 |
| QUAD_IO_PAGE_PROGRAM | 5 | 以四线I/O SPI模式执行写操作 |

### sfc_flash_op_t <a id="enum_sfc_flash_op"></a>

```c
typedef enum sfc_flash_op {
    READ_TYPE = 0x0,                   /*!< 读取Flash相关信息。 */
    WRITE_TYPE = 0x1                   /*!< 配置Flash状态。 */
} sfc_flash_op_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| READ_TYPE | 0 | 读取Flash相关信息 |
| WRITE_TYPE | 1 | 配置Flash状态 |

## Structures

### sfc_flash_config_t <a id="struct_sfc_flash_config"></a>

```c
typedef struct sfc_flash_config {
    sfc_read_if_t read_type;           /*!< 读操作SPI类型。 */
    sfc_write_if_t write_type;         /*!< 写操作SPI类型。 */
    uint32_t mapping_addr;             /*!< Flash映射基地址，对应为Flash的0地址。
                                        *   需要映射在SFC内部总线空间，仅高16位有效。 */
    uint32_t mapping_size;             /*!< Flash映射的大小，可自定义映射空间。
                                        *   参数形式必须为64KB * 2^n，其中n >= 0。
                                        *   尾地址需要在SFC内部总线空间。
                                        *   超出了表单配置的Flash大小时会将大小置为表单中配置的大小。 */
} sfc_flash_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| read_type | [sfc_read_if_t](#enum_sfc_read_if) | 读操作SPI类型 |
| write_type | [sfc_write_if_t](#enum_sfc_write_if) | 写操作SPI类型 |
| mapping_addr | uint32_t | Flash映射基地址，对应为Flash的0地址，需映射在SFC内部总线空间，仅高16位有效 |
| mapping_size | uint32_t | Flash映射大小，参数形式必须为64KB * 2^n（n >= 0），最小64KB，尾地址需在SFC内部总线空间内 |



