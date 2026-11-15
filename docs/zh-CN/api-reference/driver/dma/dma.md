# DMA

DMA (Direct Memory Access) 在无需 CPU 逐字节参与的情况下完成数据传输，释放 CPU 去处理其他任务。WS63 DMA 支持内存到内存、内存到外设、外设到内存三种传输方向，支持单次传输和 LLI (Linked List Item) 链式传输模式。

LLI 链式传输将多个传输节点串联成链表，由硬件自动逐节点执行，适合不连续地址的多段数据搬运。适用大块数据拷贝、UART (Universal Asynchronous Receiver/Transmitter)/SPI (Serial Peripheral Interface)数据收发缓冲等场景。

**头文件清单**

```c
#include "include/driver/dma.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_dma_init](#uapi_dma_init) | 初始化DMA模块 |
| [uapi_dma_deinit](#uapi_dma_deinit) | 去初始化DMA模块 |
| [uapi_dma_open](#uapi_dma_open) | 开启DMA模块 |
| [uapi_dma_close](#uapi_dma_close) | 关闭DMA模块 |
| [uapi_dma_start_transfer](#uapi_dma_start_transfer) | 启动指定通道的DMA传输 |
| [uapi_dma_end_transfer](#uapi_dma_end_transfer) | 停止指定通道的DMA传输 |
| [uapi_dma_get_block_ts](#uapi_dma_get_block_ts) | 获取DMA已传输的数据量 |
| [uapi_dma_transfer_memory_single](#uapi_dma_transfer_memory_single) | 以单块模式传输内存到内存的数据 |
| [uapi_dma_configure_peripheral_transfer_single](#uapi_dma_configure_peripheral_transfer_single) | 以单块模式配置内存到外设或外设到内存的传输 |
| [uapi_dma_get_lli_channel](#uapi_dma_get_lli_channel) | 获取DMA链表传输通道 |
| [uapi_dma_transfer_memory_lli](#uapi_dma_transfer_memory_lli) | 以链表模式传输内存到内存的数据 |
| [uapi_dma_configure_peripheral_transfer_lli](#uapi_dma_configure_peripheral_transfer_lli) | 以链表模式配置内存到外设或外设到内存的传输 |
| [uapi_dma_enable_lli](#uapi_dma_enable_lli) | 启用DMA链表传输 |
| [uapi_dma_resume](#uapi_dma_resume) | 恢复DMA模块 |
| [uapi_dma_suspend](#uapi_dma_suspend) | 挂起DMA模块 |

## Functions

### uapi_dma_init <a id="uapi_dma_init"></a>

```c
errcode_t uapi_dma_init(void)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 初始化DMA模块，完成DMA控制器底层硬件初始化
- 重复调用时返回成功，内部维护初始化状态标志
- 需在使用其他DMA接口前调用

**前置条件**

- 无硬件资源依赖前置条件

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA模块初始化成功 |
| Other | 其他错误码，参考errcode_t | 初始化失败 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_deinit <a id="uapi_dma_deinit"></a>

```c
void uapi_dma_deinit(void)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 去初始化DMA模块，释放DMA控制器资源
- 若模块未初始化则直接返回，不执行去初始化操作
- 调用后DMA模块不可用，需重新调用uapi_dma_init初始化

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)


### uapi_dma_open <a id="uapi_dma_open"></a>

```c
errcode_t uapi_dma_open(void)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 开启DMA模块，注册DMA中断处理函数
- 需在uapi_dma_init之后调用
- 调用后DMA通道可进行传输操作

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA模块开启成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_close <a id="uapi_dma_close"></a>

```c
void uapi_dma_close(void)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 关闭DMA模块，注销DMA中断处理函数
- 需在uapi_dma_init之后调用
- 调用后DMA通道停止传输操作

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成

### uapi_dma_start_transfer <a id="uapi_dma_start_transfer"></a>

```c
errcode_t uapi_dma_start_transfer(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 启动指定通道的DMA传输
- 调用前需已完成通道配置（单块传输或链表传输）
- 通道编号需在有效范围内

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA通道启动传输成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | 通道编号超出范围 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_end_transfer <a id="uapi_dma_end_transfer"></a>

```c
errcode_t uapi_dma_end_transfer(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 停止指定通道的DMA传输
- 通道编号需在有效范围内
- 停止传输后通道可重新配置

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA通道停止传输成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | 通道编号超出范围 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_get_block_ts <a id="uapi_dma_get_block_ts"></a>

```c
uint32_t uapi_dma_get_block_ts(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 获取DMA已传输的数据量（以传输宽度为单位）
- 通道编号需在有效范围内
- 未初始化时返回ERRCODE_DMA_NOT_INIT

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | 模块未调用uapi_dma_init初始化 |
| 0 | 通道编号无效 | 通道编号超出有效范围（≥ DMA_CHANNEL_MAX_NUM） |
| 其他正整数 | DMA已传输的数据量 | 通道编号有效且模块已初始化 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_transfer_memory_single <a id="uapi_dma_transfer_memory_single"></a>

```c
errcode_t uapi_dma_transfer_memory_single(const dma_ch_user_memory_config_t *user_cfg, dma_transfer_cb_t callback, uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 以单块模式传输内存到内存的数据
- 自动获取空闲DMA通道，配置传输参数后立即启动传输
- 传输完成后触发回调函数通知调用者
- 16位或32位宽度传输时，源地址和目的地址需满足对齐要求

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启
- user_cfg不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| user_cfg | const [dma_ch_user_memory_config_t](#struct_dma_ch_user_memory_config) * | 用户的DMA通道传输配置 | 不为NULL |
| callback | [dma_transfer_cb_t](#typedef_dma_transfer_cb_t) | 通道传输完成/错误回调函数 | - |
| arg | uintptr_t | 传递给回调函数的私有参数指针 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 内存传输启动成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | user_cfg为NULL或priority/width超出范围 |
| ERRCODE_DMA_RET_NO_AVAIL_CH(0x80001103) | 无可用通道 | 所有DMA通道均被占用 |
| ERRCODE_DMA_RET_ERROR_ADDRESS_ALIGN(0x80001108) | 地址未对齐 | 16位传输地址未2字节对齐，或32位传输地址未4字节对齐 |
| ERRCODE_DMA_RET_ERROR_CONFIG(0x80001104) | 配置错误 | DMA配置失败 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_configure_peripheral_transfer_single <a id="uapi_dma_configure_peripheral_transfer_single"></a>

```c
errcode_t uapi_dma_configure_peripheral_transfer_single(const dma_ch_user_peripheral_config_t *user_cfg, uint8_t *channel, dma_transfer_cb_t callback, uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 以单块模式配置内存到外设或外设到内存的DMA传输
- 根据传输方向自动获取空闲DMA通道，通过出参channel返回
- 配置完成后需调用uapi_dma_start_transfer启动传输
- 传输完成后触发回调函数通知调用者

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启
- user_cfg不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| user_cfg | const [dma_ch_user_peripheral_config_t](#struct_dma_ch_user_peripheral_config) * | 用户的DMA通道传输配置 | 不为NULL |
| callback | [dma_transfer_cb_t](#typedef_dma_transfer_cb_t) | 通道传输完成/错误回调函数 | - |
| arg | uintptr_t | 传递给回调函数的私有参数指针 | - |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| channel | uint8_t * | 获取被选择的DMA通道编号 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 外设传输配置成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | user_cfg参数校验不通过 |
| ERRCODE_DMA_RET_NO_AVAIL_CH(0x80001103) | 无可用通道 | 所有DMA通道均被占用 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_get_lli_channel <a id="uapi_dma_get_lli_channel"></a>

```c
uint8_t uapi_dma_get_lli_channel(uint8_t burst_length, uint8_t handshaking)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 获取DMA链表传输通道
- 根据burst长度和握手号获取空闲通道
- 获取的通道用于后续链表传输配置

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| burst_length | uint8_t | DMA的burst传输长度 | 0~7 |
| handshaking | uint8_t | DMA传输外设种类 | 0~45 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0~3 | DMA通道编号 | 成功获取空闲通道 |
| DMA_CHANNEL_NONE(4) | 无可用通道 | 参数无效或模块未初始化或无空闲通道 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LLI | 特性宏 | 支持DMA链表传输功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_transfer_memory_lli <a id="uapi_dma_transfer_memory_lli"></a>

```c
errcode_t uapi_dma_transfer_memory_lli(uint8_t channel, const dma_ch_user_memory_config_t *user_cfg, dma_transfer_cb_t callback)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 以链表模式配置内存到内存的DMA传输
- 将传输配置添加到链表节点，不立即启动传输
- 需配合uapi_dma_enable_lli启动链表传输
- 16位或32位宽度传输时，源地址和目的地址需满足对齐要求

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启
- user_cfg不为NULL，且指向内存空间已申请成功
- channel已通过uapi_dma_get_lli_channel获取

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |
| user_cfg | const [dma_ch_user_memory_config_t](#struct_dma_ch_user_memory_config) * | 用户的DMA通道传输配置 | 不为NULL |
| callback | [dma_transfer_cb_t](#typedef_dma_transfer_cb_t) | 通道传输完成/错误回调函数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 链表内存传输配置成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | channel超出范围或user_cfg为NULL或priority/width超出范围 |
| ERRCODE_DMA_RET_ERROR_ADDRESS_ALIGN(0x80001108) | 地址未对齐 | 16位传输地址未2字节对齐，或32位传输地址未4字节对齐 |
| ERRCODE_DMA_RET_ERROR_CONFIG(0x80001104) | 配置错误 | DMA配置失败 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LLI | 特性宏 | 支持DMA链表传输功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_configure_peripheral_transfer_lli <a id="uapi_dma_configure_peripheral_transfer_lli"></a>

```c
errcode_t uapi_dma_configure_peripheral_transfer_lli(uint8_t channel, const dma_ch_user_peripheral_config_t *user_cfg, dma_transfer_cb_t callback)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 以链表模式配置内存到外设或外设到内存的DMA传输
- 将传输配置添加到链表节点，不立即启动传输
- 需配合uapi_dma_enable_lli启动链表传输

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启
- user_cfg不为NULL，且指向内存空间已申请成功
- channel已通过uapi_dma_get_lli_channel获取

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |
| user_cfg | const [dma_ch_user_peripheral_config_t](#struct_dma_ch_user_peripheral_config) * | 用户的DMA通道传输配置 | 不为NULL |
| callback | [dma_transfer_cb_t](#typedef_dma_transfer_cb_t) | 通道传输完成/错误回调函数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 链表外设传输配置成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | channel超出范围或user_cfg参数校验不通过 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2s_dma_lli_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_slave_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LLI | 特性宏 | 支持DMA链表传输功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_enable_lli <a id="uapi_dma_enable_lli"></a>

```c
errcode_t uapi_dma_enable_lli(uint8_t channel, dma_transfer_cb_t callback, uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 启用DMA链表传输，启动已配置链表节点的DMA通道
- 通道必须已配置链表传输节点
- 通道不可处于已使能状态

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态
- DMA模块已通过uapi_dma_open开启
- 已通过uapi_dma_transfer_memory_lli或uapi_dma_configure_peripheral_transfer_lli配置链表节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | DMA通道编号 | 0~3 |
| callback | [dma_transfer_cb_t](#typedef_dma_transfer_cb_t) | 通道传输完成/错误回调函数 | - |
| arg | uintptr_t | 传递给回调函数的私有参数指针 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 链表传输启动成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| ERRCODE_DMA_INVALID_PARAMETER(0x80001102) | 参数无效 | 通道编号超出范围 |
| ERRCODE_DMA_CH_BUSY(0x80001107) | 通道忙 | 通道已处于使能状态 |

**参考案例**

- [dma_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/dma/dma_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LLI | 特性宏 | 支持DMA链表传输功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_resume <a id="uapi_dma_resume"></a>

```c
errcode_t uapi_dma_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 恢复DMA模块，从挂起状态恢复DMA通道运行
- 需在uapi_dma_suspend之后调用
- 恢复DMA控制器及所有通道状态

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复操作所需的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA模块恢复成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LPM | 特性宏 | 支持DMA低功耗模式功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

### uapi_dma_suspend <a id="uapi_dma_suspend"></a>

```c
errcode_t uapi_dma_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/dma.h"
```

**功能说明**

- 挂起DMA模块，暂停所有DMA通道运行
- 挂起后DMA通道停止传输，需调用uapi_dma_resume恢复
- 用于系统低功耗场景

**前置条件**

- DMA模块已通过uapi_dma_init初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起操作所需的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DMA模块挂起成功 |
| ERRCODE_DMA_NOT_INIT(0x80001100) | DMA模块未初始化 | DMA模块未初始化时调用 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DMA_SUPPORT_LPM | 特性宏 | 支持DMA低功耗模式功能 | y |
| CONFIG_DRIVER_SUPPORT_DMA | 特性宏 | 支持DMA接口功能 | n |

## Type definitions

### dma_transfer_cb_t <a id="typedef_dma_transfer_cb_t"></a>

```c
typedef void (*dma_transfer_cb_t)(uint8_t intr, uint8_t channel, uintptr_t arg);
```

**功能说明**

DMA传输回调函数类型

**入参**

| 名称 | 参数类型 | 详细说明 |
| ---- | ---- | ---- |
| intr | uint8_t | DMA中断类型 |
| channel | uint8_t | DMA通道编号 |
| arg | uintptr_t | 传递给回调函数的私有参数指针 |

**使用说明**

在[uapi_dma_transfer_memory_single](#uapi_dma_transfer_memory_single)、[uapi_dma_configure_peripheral_transfer_single](#uapi_dma_configure_peripheral_transfer_single)、[uapi_dma_transfer_memory_lli](#uapi_dma_transfer_memory_lli)、[uapi_dma_configure_peripheral_transfer_lli](#uapi_dma_configure_peripheral_transfer_lli)、[uapi_dma_enable_lli](#uapi_dma_enable_lli)接口中作为入参，用于DMA通道传输完成或错误时触发的回调。回调参数 intr 为 DMA 中断类型，channel 为 DMA 通道，arg 为调用 DMA 传输时传递的私有参数指针。

## Structures

### dma_ch_user_memory_config_t <a id="struct_dma_ch_user_memory_config"></a>

```c
typedef struct dma_ch_user_memory_config {
    uint32_t src;
    uint32_t dest;
    uint16_t transfer_num;
    uint8_t  priority;
    uint8_t  width;
} dma_ch_user_memory_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| src | uint32_t | 传输源地址 |
| dest | uint32_t | 传输目的地址 |
| transfer_num | uint16_t | 传输数据量 |
| priority | uint8_t | 传输通道优先级（最低为0，最高为3） |
| width | uint8_t | 传输数据宽度：0-1字节，1-2字节，2-4字节 |

### dma_ch_user_peripheral_config_t <a id="struct_dma_ch_user_peripheral_config"></a>

```c
typedef struct dma_ch_user_peripheral_config {
    uint32_t src;
    uint32_t dest;
    uint16_t transfer_num;
    uint16_t src_handshaking;
    uint16_t dest_handshaking;
    uint8_t  trans_type;
    uint8_t  trans_dir;
    uint8_t  priority;
    uint8_t  src_width;
    uint8_t  dest_width;
    uint8_t  burst_length;
    uint8_t  src_increment;
    uint8_t  dest_increment;
    uint8_t  protection;
} dma_ch_user_peripheral_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| src | uint32_t | 传输源地址 |
| dest | uint32_t | 传输目的地址 |
| transfer_num | uint16_t | 传输数据量 |
| src_handshaking | uint16_t | 源端硬件握手号，参考hal_dma_handshaking_source_t |
| dest_handshaking | uint16_t | 目的端硬件握手号，参考hal_dma_handshaking_source_t |
| trans_type | uint8_t | 传输类型：0-内存到内存(DMA流控)，1-内存到外设(DMA流控)，2-外设到内存(DMA流控)，3-外设到外设(DMA流控)，4-外设到内存(外设流控)，5-外设到外设(源端外设流控)，6-内存到外设(外设流控)，7-外设到外设(目的端外设流控) |
| trans_dir | uint8_t | 传输方向：0-内存到外设，1-外设到内存，2-外设到外设 |
| priority | uint8_t | 传输通道优先级（最低为0，最高为3） |
| src_width | uint8_t | 源端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| dest_width | uint8_t | 目的端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| burst_length | uint8_t | 传输burst长度：0-burst长度1，1-burst长度4，2-burst长度8，3-burst长度16 |
| src_increment | uint8_t | 源端地址增量模式：0-递增，1-递减，2-不变 |
| dest_increment | uint8_t | 目的端地址增量模式：0-递增，1-递减，2-不变 |
| protection | uint8_t | 保护控制位，用于驱动AHB HPROT[3:1]总线：0-HPROT[1]，1-HPROT[2]，2-HPROT[3] |



