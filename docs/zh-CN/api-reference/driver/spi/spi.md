# SPI

SPI (Serial Peripheral Interface) 是一种高速、全双工、同步的串行通信总线，主要用于主控芯片与外设之间的短距离通信。WS63 SPI驱动支持Master/Slave模式、DMA (Direct Memory Access) 传输、中断模式、CRC (Cyclic Redundancy Check) 校验以及QSPI (Quad Serial Peripheral Interface) 高级配置，适用于Flash存储、显示屏、传感器等外设的高速数据交互。

**头文件清单**

```c
#include "include/driver/spi.h"
```

## 接口清单

| 接口名称                                                              | 功能简述                  |
| ----------------------------------------------------------------- | --------------------- |
| [uapi_spi_init](#uapi_spi_init)                                 | 初始化SPI总线              |
| [uapi_spi_deinit](#uapi_spi_deinit)                             | 去初始化SPI总线             |
| [uapi_spi_set_tmod](#uapi_spi_set_tmod)                        | 设置SPI传输模式             |
| [uapi_spi_set_attr](#uapi_spi_set_attr)                        | 设置SPI基础配置参数           |
| [uapi_spi_get_attr](#uapi_spi_get_attr)                        | 读取SPI基础配置参数           |
| [uapi_spi_set_interrupt_mask](#uapi_spi_set_interrupt_mask)   | 设置SPI中断屏蔽值            |
| [uapi_spi_clear_all_interrupt](#uapi_spi_clear_all_interrupt) | 清除SPI所有中断             |
| [uapi_spi_set_extra_attr](#uapi_spi_set_extra_attr)           | 设置SPI高级配置参数           |
| [uapi_spi_select_slave](#uapi_spi_select_slave)                | Master模式下选择对通的Slave设备 |
| [uapi_spi_master_write](#uapi_spi_master_write)                | Master模式下向Slave写入数据   |
| [uapi_spi_master_read](#uapi_spi_master_read)                  | Master模式下从Slave读取数据   |
| [uapi_spi_master_writeread](#uapi_spi_master_writeread)        | Master模式下写入和读取数据      |
| [uapi_spi_get_extra_attr](#uapi_spi_get_extra_attr)           | 读取SPI高级配置参数           |
| [uapi_spi_slave_write](#uapi_spi_slave_write)                  | Slave模式下向Master写入数据   |
| [uapi_spi_slave_read](#uapi_spi_slave_read)                    | Slave模式下从Master读取数据   |
| [uapi_spi_slave_writeread](#uapi_spi_slave_writeread)          | Slave模式下写入和读取数据       |
| [uapi_spi_set_dma_mode](#uapi_spi_set_dma_mode)               | 使能/去使能DMA模式下SPI传输     |
| [uapi_spi_set_irq_mode](#uapi_spi_set_irq_mode)               | 设置是否使用中断模式传输数据        |
| [uapi_spi_set_loop_back_mode](#uapi_spi_set_loop_back_mode)  | 设置环回测试模式              |
| [uapi_spi_set_crc_mode](#uapi_spi_set_crc_mode)               | 设置SPI发送和接收CRC模式       |
| [uapi_spi_suspend](#uapi_spi_suspend)                           | 挂起所有SPI通道             |
| [uapi_spi_resume](#uapi_spi_resume)                             | 恢复所有SPI通道             |

## Functions

### uapi_spi_init <a id="uapi_spi_init"></a>

```c
errcode_t uapi_spi_init(spi_bus_t bus, spi_attr_t *attr, spi_extra_attr_t *extra_attr)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 初始化指定SPI总线，配置基础属性和高级属性
- 根据配置初始化DMA信号量、中断模式、互斥锁等内部资源
- 调用HAL (Hardware Abstraction Layer) 层完成SPI硬件初始化，初始化成功后标记总线已初始化状态

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_MASTER或CONFIG_SPI_SUPPORT_SLAVE）
- 入参attr不为NULL，且指向的内存空间已申请成功

**入参**

| 名称          | 参数类型                                               | 详细说明       | 约束取值范围                           |
| ----------- | -------------------------------------------------- | ---------- | -------------------------------- |
| bus         | [spi_bus_t](#enum_spi_bus_t)                     | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| attr        | [spi_attr_t](#struct_spi_attr_t) *              | SPI的基础配置参数 | 不为NULL                           |
| extra_attr | [spi_extra_attr_t](#struct_spi_extra_attr_t) * | SPI的高级配置参数 | -                                |

**返回值**

| 返回值                 | 文字含义               | 触发场景  |
| ------------------- | ------------------ | ----- |
| ERRCODE_SUCC(0x0) | 成功                 | 初始化成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败  |

**参考案例**

- [spi_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_master_demo.c)

**Kconfig配置**

| 配置项                             | 宏类型 | 说明               | 默认值 |
| ------------------------------- | --- | ---------------- | --- |
| CONFIG_SPI_SUPPORT_QUAD_SPI | 特性宏 | 支持SPI QSPI高级配置特性 | 由构建目标决定 |

### uapi_spi_deinit <a id="uapi_spi_deinit"></a>

```c
errcode_t uapi_spi_deinit(spi_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 去初始化指定SPI总线，释放硬件资源
- 调用HAL层去初始化SPI硬件，若使能DMA则销毁DMA信号量，若使能中断则注销中断，若使能LPC则关闭时钟
- 释放互斥锁，标记总线为未初始化状态
- 若指定SPI总线未初始化，直接返回ERRCODE_SUCC

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成

**入参**

| 名称  | 参数类型                           | 详细说明     | 约束取值范围                           |
| --- | ------------------------------ | -------- | -------------------------------- |
| bus | [spi_bus_t](#enum_spi_bus_t) | 指定的SPI接口 | SPI_BUS_0(0) ~ SPI_BUS_1(1) |

**返回值**

| 返回值                                 | 文字含义               | 触发场景       |
| ----------------------------------- | ------------------ | ---------- |
| ERRCODE_SUCC(0x0)                 | 成功                 | 去初始化成功     |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效               | bus超出范围    |
| Other                               | 其他错误码，参考errcode_t | HAL层去初始化失败 |

**Kconfig配置**

| 配置项                             | 宏类型 | 说明          | 默认值 |
| ------------------------------- | --- | ----------- | --- |
| CONFIG_SPI_SUPPORT_DMA       | 特性宏 | 支持DMA传输特性   | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT | 特性宏 | 支持中断传输特性    | n   |
| CONFIG_SPI_SUPPORT_LPC       | 特性宏 | 支持低功耗时钟控制特性 | n   |

### uapi_spi_set_tmod <a id="uapi_spi_set_tmod"></a>

```c
errcode_t uapi_spi_set_tmod(spi_bus_t bus, hal_spi_trans_mode_t tmod, uint8_t data_frame_num)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置SPI的传输模式，同时设置接收数据帧数
- 通过HAL层控制接口下发传输模式配置

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成

**入参**

| 名称               | 参数类型                                                   | 详细说明        | 约束取值范围                                                             |
| ---------------- | ------------------------------------------------------ | ----------- | ------------------------------------------------------------------ |
| bus              | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口    | SPI_BUS_0(0) ~ SPI_BUS_1(1)                                   |
| tmod             | [hal_spi_trans_mode_t](#enum_hal_spi_trans_mode_t) | SPI传输模式     | HAL_SPI_TRANS_MODE_TXRX(0) ~ HAL_SPI_TRANS_MODE_EEPROM(3) |
| data_frame_num | uint8_t                                               | SPI接收数据帧的数量 | -                                                                  |

**返回值**

| 返回值                                 | 文字含义               | 触发场景             |
| ----------------------------------- | ------------------ | ---------------- |
| ERRCODE_SUCC(0x0)                 | 成功                 | 设置成功             |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效               | bus超出范围或tmod超出范围 |
| Other                               | 其他错误码，参考errcode_t | HAL层设置失败         |

### uapi_spi_set_attr <a id="uapi_spi_set_attr"></a>

```c
errcode_t uapi_spi_set_attr(spi_bus_t bus, spi_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置SPI的基础配置参数，包括工作模式、时钟极性/相位、帧格式、传输模式等
- 通过HAL层控制接口下发基础属性配置

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成
- 入参attr不为NULL

**入参**

| 名称   | 参数类型                                  | 详细说明       | 约束取值范围                           |
| ---- | ------------------------------------- | ---------- | -------------------------------- |
| bus  | [spi_bus_t](#enum_spi_bus_t)        | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| attr | [spi_attr_t](#struct_spi_attr_t) * | SPI的基础配置参数 | 不为NULL                           |

**返回值**

| 返回值                                 | 文字含义               | 触发场景              |
| ----------------------------------- | ------------------ | ----------------- |
| ERRCODE_SUCC(0x0)                 | 成功                 | 设置成功              |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效               | bus超出范围或attr为NULL |
| Other                               | 其他错误码，参考errcode_t | HAL层设置失败          |

### uapi_spi_get_attr <a id="uapi_spi_get_attr"></a>

```c
errcode_t uapi_spi_get_attr(spi_bus_t bus, spi_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 读取SPI的基础配置参数，包括工作模式、时钟极性/相位、帧格式、传输模式等
- 通过HAL层控制接口获取当前基础属性

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成
- 入参attr不为NULL

**入参**

| 名称   | 参数类型                                  | 详细说明       | 约束取值范围                           |
| ---- | ------------------------------------- | ---------- | -------------------------------- |
| bus  | [spi_bus_t](#enum_spi_bus_t)        | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| attr | [spi_attr_t](#struct_spi_attr_t) * | SPI的基础配置参数 | 不为NULL                           |

**出参**

| 名称   | 数据类型                                  | 输出说明          |
| ---- | ------------------------------------- | ------------- |
| attr | [spi_attr_t](#struct_spi_attr_t) * | 返回当前SPI基础配置参数 |

**返回值**

| 返回值                                 | 文字含义               | 触发场景              |
| ----------------------------------- | ------------------ | ----------------- |
| ERRCODE_SUCC(0x0)                 | 成功                 | 读取成功              |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效               | bus超出范围或attr为NULL |
| Other                               | 其他错误码，参考errcode_t | HAL层获取失败          |

### uapi_spi_set_interrupt_mask <a id="uapi_spi_set_interrupt_mask"></a>

```c
void uapi_spi_set_interrupt_mask(spi_bus_t bus, uint32_t mask_val)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置SPI中断屏蔽值，通过HAL层控制接口下发中断屏蔽配置
- 支持的中断类型包括：RX_FULL、RX_OVERFLOW、RX_UNDERFLOW、TX_EMPTY、TX_OVERFLOW

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成

**入参**

| 名称        | 参数类型                           | 详细说明     | 约束取值范围                                                                                                                                            |
| --------- | ------------------------------ | -------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| bus       | [spi_bus_t](#enum_spi_bus_t) | 指定的SPI接口 | SPI_BUS_0(0) ~ SPI_BUS_1(1)                                                                                                                  |
| mask_val | uint32_t                      | 中断屏蔽值    | SPI_RX_FULL_INT(0x1); SPI_RX_OVERFLOW_INT(0x4); SPI_RX_UNDERFLOW_INT(0x8); SPI_TX_EMPTY_INT(0x10); SPI_TX_OVERFLOW_INT(0x20)，可组合使用 |

**返回值**

无返回值（void）

### uapi_spi_clear_all_interrupt <a id="uapi_spi_clear_all_interrupt"></a>

```c
void uapi_spi_clear_all_interrupt(spi_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 清除SPI所有中断状态，通过HAL层控制接口清除所有中断

**前置条件**

- 指定SPI总线已通过uapi_spi_init()初始化完成

**入参**

| 名称  | 参数类型                           | 详细说明     | 约束取值范围                           |
| --- | ------------------------------ | -------- | -------------------------------- |
| bus | [spi_bus_t](#enum_spi_bus_t) | 指定的SPI接口 | SPI_BUS_0(0) ~ SPI_BUS_1(1) |

**返回值**

无返回值（void）

### uapi_spi_set_extra_attr <a id="uapi_spi_set_extra_attr"></a>

```c
errcode_t uapi_spi_set_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置SPI的高级配置参数，包括DMA收发使能、QSPI参数、SSPI参数
- 通过HAL层控制接口下发高级属性配置

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_QUAD_SPI，当前无Kconfig入口）
- 指定SPI总线已通过uapi_spi_init()初始化完成
- 入参extra_attr不为NULL

**入参**

| 名称          | 参数类型                                               | 详细说明       | 约束取值范围                           |
| ----------- | -------------------------------------------------- | ---------- | -------------------------------- |
| bus         | [spi_bus_t](#enum_spi_bus_t)                     | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| extra_attr | [spi_extra_attr_t](#struct_spi_extra_attr_t) * | SPI的高级配置参数 | 不为NULL                           |

**返回值**

| 返回值                                 | 文字含义               | 触发场景                     |
| ----------------------------------- | ------------------ | ------------------------ |
| ERRCODE_SUCC(0x0)                 | 成功                 | 设置成功                     |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效               | bus超出范围或extra_attr为NULL |
| Other                               | 其他错误码，参考errcode_t | HAL层设置失败                 |

**Kconfig配置**

| 配置项                             | 宏类型 | 说明               | 默认值 |
| ------------------------------- | --- | ---------------- | --- |
| CONFIG_SPI_SUPPORT_QUAD_SPI | 特性宏 | 支持SPI QSPI高级配置特性 | 由构建目标决定 |

### uapi_spi_select_slave <a id="uapi_spi_select_slave"></a>

```c
errcode_t uapi_spi_select_slave(spi_bus_t bus, spi_slave_t cs)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Master模式下选择需要对通的Slave设备
- 通过HAL层控制接口下发Slave选择配置

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_MASTER）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Master模式

**入参**

| 名称  | 参数类型                               | 详细说明        | 约束取值范围                           |
| --- | ---------------------------------- | ----------- | -------------------------------- |
| bus | [spi_bus_t](#enum_spi_bus_t)     | 指定的SPI接口    | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| cs  | [spi_slave_t](#enum_spi_slave_t) | 被选中的Slave设备 | SPI_SLAVE0(0) ~ SPI_SLAVE1(1) |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 选择成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                          | 宏类型 | 说明             | 默认值 |
| ---------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_MASTER | 特性宏 | 支持SPI Master功能 | y   |

### uapi_spi_master_write <a id="uapi_spi_master_write"></a>

```c
errcode_t uapi_spi_master_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Master模式下向Slave写入数据，支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式、中断模式，三种模式不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_MASTER）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Master模式
- 入参data不为NULL

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                           |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | -------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL                           |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT   |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 写入成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [spi_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_master_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明             | 默认值 |
| -------------------------------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_MASTER                       | 特性宏 | 支持SPI Master功能 | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性      | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性       | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性       | n   |

### uapi_spi_master_read <a id="uapi_spi_master_read"></a>

```c
errcode_t uapi_spi_master_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Master模式下从Slave读取数据，支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式、中断模式，三种模式不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_MASTER）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Master模式
- 入参data不为NULL，data->rx_buff不为NULL，data->rx_bytes不为0

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                             |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | ---------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1)   |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL，rx_buff不为NULL，rx_bytes不为0 |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT     |

**出参**

| 名称   | 数据类型                                                   | 输出说明              |
| ---- | ------------------------------------------------------ | ----------------- |
| data | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | rx_buff中存储接收到的数据 |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 读取成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [spi_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_master_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明             | 默认值 |
| -------------------------------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_MASTER                       | 特性宏 | 支持SPI Master功能 | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性      | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性       | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性       | n   |

### uapi_spi_master_writeread <a id="uapi_spi_master_writeread"></a>

```c
errcode_t uapi_spi_master_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Master模式下写入和读取数据，支持手动切换模式（轮询/DMA）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式，不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_MASTER）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Master模式
- 入参data不为NULL，data->rx_buff不为NULL，data->rx_bytes不为0

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                             |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | ---------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1)   |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL，rx_buff不为NULL，rx_bytes不为0 |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT     |

**出参**

| 名称   | 数据类型                                                   | 输出说明              |
| ---- | ------------------------------------------------------ | ----------------- |
| data | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | rx_buff中存储接收到的数据 |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 收发成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明             | 默认值 |
| -------------------------------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_MASTER                       | 特性宏 | 支持SPI Master功能 | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性      | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性       | n   |

### uapi_spi_get_extra_attr <a id="uapi_spi_get_extra_attr"></a>

```c
errcode_t uapi_spi_get_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 读取SPI的高级配置参数，包括DMA收发使能、QSPI参数、SSPI参数
- 通过HAL层控制接口获取当前高级属性

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_QUAD_SPI，当前无Kconfig入口）
- 指定SPI总线已通过uapi_spi_init()初始化完成
- 入参extra_attr不为NULL

**入参**

| 名称          | 参数类型                                               | 详细说明       | 约束取值范围                           |
| ----------- | -------------------------------------------------- | ---------- | -------------------------------- |
| bus         | [spi_bus_t](#enum_spi_bus_t)                     | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| extra_attr | [spi_extra_attr_t](#struct_spi_extra_attr_t) * | SPI的高级配置参数 | 不为NULL                           |

**出参**

| 名称          | 数据类型                                               | 输出说明          |
| ----------- | -------------------------------------------------- | ------------- |
| extra_attr | [spi_extra_attr_t](#struct_spi_extra_attr_t) * | 返回当前SPI高级配置参数 |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 读取成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                             | 宏类型 | 说明               | 默认值 |
| ------------------------------- | --- | ---------------- | --- |
| CONFIG_SPI_SUPPORT_QUAD_SPI | 特性宏 | 支持SPI QSPI高级配置特性 | 由构建目标决定 |

### uapi_spi_slave_write <a id="uapi_spi_slave_write"></a>

```c
errcode_t uapi_spi_slave_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Slave模式下向Master写入数据，支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式、中断模式，三种模式不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_SLAVE）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Slave模式
- 入参data不为NULL，data->tx_buff不为NULL，data->tx_bytes不为0

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                             |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | ---------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1)   |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL，tx_buff不为NULL，tx_bytes不为0 |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT     |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 写入成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [spi_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_slave_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明                    | 默认值 |
| -------------------------------------------------- | --- | --------------------- | --- |
| CONFIG_SPI_SUPPORT_SLAVE                        | 特性宏 | 支持SPI Slave功能         | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性        | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性             | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性              | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性              | n   |
| CONFIG_SPI_SLAVE_SUPPORT_NOTIFY                | 特性宏 | 支持Slave通知Master发送时钟特性 | n   |

### uapi_spi_slave_read <a id="uapi_spi_slave_read"></a>

```c
errcode_t uapi_spi_slave_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Slave模式下从Master读取数据，支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式、中断模式，三种模式不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_SLAVE）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Slave模式
- 入参data不为NULL，data->rx_buff不为NULL，data->rx_bytes不为0

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                             |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | ---------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1)   |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL，rx_buff不为NULL，rx_bytes不为0 |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT     |

**出参**

| 名称   | 数据类型                                                   | 输出说明              |
| ---- | ------------------------------------------------------ | ----------------- |
| data | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | rx_buff中存储接收到的数据 |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 读取成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [spi_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_slave_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明                    | 默认值 |
| -------------------------------------------------- | --- | --------------------- | --- |
| CONFIG_SPI_SUPPORT_SLAVE                        | 特性宏 | 支持SPI Slave功能         | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性        | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性             | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性              | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性              | n   |
| CONFIG_SPI_SLAVE_SUPPORT_NOTIFY                | 特性宏 | 支持Slave通知Master发送时钟特性 | n   |

### uapi_spi_slave_writeread <a id="uapi_spi_slave_writeread"></a>

```c
errcode_t uapi_spi_slave_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- Slave模式下写入和读取数据，支持手动切换模式（轮询/DMA）和自动切换模式（轮询与DMA自动切换）
- 手动切换模式：轮询模式、DMA模式，不能在同一bus中同时使用
- 自动切换模式：当传输数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_SLAVE）
- 指定SPI总线已通过uapi_spi_init()初始化完成，且工作在Slave模式
- 入参data不为NULL，data->rx_buff不为NULL，data->rx_bytes不为0

**入参**

| 名称      | 参数类型                                                   | 详细说明                                             | 约束取值范围                             |
| ------- | ------------------------------------------------------ | ------------------------------------------------ | ---------------------------------- |
| bus     | [spi_bus_t](#enum_spi_bus_t)                         | 指定的SPI接口                                         | SPI_BUS_0(0) ~ SPI_BUS_1(1)   |
| data    | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | 数据传输指针                                           | 不为NULL，rx_buff不为NULL，rx_bytes不为0 |
| timeout | uint32_t                                              | 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效） | 0表示使用CONFIG_SPI_MAX_TIMEOUT     |

**出参**

| 名称   | 数据类型                                                   | 输出说明              |
| ---- | ------------------------------------------------------ | ----------------- |
| data | const [spi_xfer_data_t](#struct_spi_xfer_data_t) * | rx_buff中存储接收到的数据 |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 收发成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明             | 默认值 |
| -------------------------------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_SLAVE                        | 特性宏 | 支持SPI Slave功能  | y   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性      | n   |
| CONFIG_SPI_SUPPORT_CONCURRENCY                  | 特性宏 | 支持并发互斥特性       | n   |

### uapi_spi_set_dma_mode <a id="uapi_spi_set_dma_mode"></a>

```c
errcode_t uapi_spi_set_dma_mode(spi_bus_t bus, bool en, const spi_dma_config_t *dma_cfg)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 使能/去使能DMA模式下SPI传输
- 使能DMA时，配置DMA参数（源端/目的端数据宽度、burst长度、通道优先级），并通过HAL层设置DMA传输使能和数据level
- 去使能DMA时，dma_cfg参数配置为NULL，DMA传输数据level设为0
- DMA模式与中断模式互斥，不能在同一bus中同时使用

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_DMA）
- 指定SPI总线已通过uapi_spi_init()初始化完成
- 未使能中断模式（CONFIG_SPI_SUPPORT_INTERRUPT使能时，DMA与中断模式互斥）
- 未使能轮询与DMA自动切换模式（CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH未使能时此接口可用）

**入参**

| 名称       | 参数类型                                                     | 详细说明       | 约束取值范围                            |
| -------- | -------------------------------------------------------- | ---------- | --------------------------------- |
| bus      | [spi_bus_t](#enum_spi_bus_t)                           | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1)  |
| en       | bool                                                     | 是否使能DMA传输  | true: 使能; false: 去使能              |
| dma_cfg | const [spi_dma_config_t](#struct_spi_dma_config_t) * | DMA配置结构体指针 | en为true时，不为NULL；en为false时，配置为NULL |

**返回值**

| 返回值                                             | 文字含义               | 触发场景          |
| ----------------------------------------------- | ------------------ | ------------- |
| ERRCODE_SUCC(0x0)                             | 成功                 | 设置成功          |
| ERRCODE_INVALID_PARAM(0x80000001)             | 参数无效               | bus超出范围       |
| ERRCODE_SPI_DMA_IRQ_MODE_MUTEX(0x8000133C) | DMA与中断模式互斥         | 中断模式已使能时使能DMA |
| Other                                           | 其他错误码，参考errcode_t | 执行失败          |

**参考案例**

- [spi_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_master_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明                        | 默认值 |
| -------------------------------------------------- | --- | ------------------------- | --- |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持SPI DMA传输特性             | n   |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性（与DMA互斥）          | n   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性（使能后此接口不可用） | n   |

### uapi_spi_set_irq_mode <a id="uapi_spi_set_irq_mode"></a>

```c
errcode_t uapi_spi_set_irq_mode(spi_bus_t bus, bool irq_en, spi_rx_callback_t rx_callback, spi_tx_callback_t tx_callback)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置是否使用中断模式传输数据
- 使能中断模式时，注册接收和发送回调函数
- 去使能中断模式时，将接收和发送回调函数置为NULL
- 中断模式与DMA模式互斥，不能在同一bus中同时使用

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_INTERRUPT）
- 指定SPI总线已通过uapi_spi_init()初始化完成
- 未使能DMA模式（CONFIG_SPI_SUPPORT_DMA使能时，DMA与中断模式互斥）
- 未使能轮询与DMA自动切换模式（CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH未使能时此接口可用）

**入参**

| 名称           | 参数类型                                       | 详细说明         | 约束取值范围                           |
| ------------ | ------------------------------------------ | ------------ | -------------------------------- |
| bus          | [spi_bus_t](#enum_spi_bus_t)             | 指定的SPI接口     | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| irq_en      | bool                                       | 是否使用中断模式     | true: 使能; false: 去使能             |
| rx_callback | [spi_rx_callback_t](#spi_rx_callback_t) | 接收数据完成时的回调函数 | irq_en为true时，不为NULL             |
| tx_callback | [spi_tx_callback_t](#spi_tx_callback_t) | 数据发送完成时的回调函数 | irq_en为true时，不为NULL             |

**返回值**

| 返回值                                             | 文字含义               | 触发场景          |
| ----------------------------------------------- | ------------------ | ------------- |
| ERRCODE_SUCC(0x0)                             | 成功                 | 设置成功          |
| ERRCODE_INVALID_PARAM(0x80000001)             | 参数无效               | bus超出范围       |
| ERRCODE_SPI_DMA_IRQ_MODE_MUTEX(0x8000133C) | DMA与中断模式互斥         | DMA模式已使能时使能中断 |
| Other                                           | 其他错误码，参考errcode_t | 执行失败          |

**参考案例**

- [spi_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/spi/spi_master_demo.c)

**Kconfig配置**

| 配置项                                                | 宏类型 | 说明                        | 默认值 |
| -------------------------------------------------- | --- | ------------------------- | --- |
| CONFIG_SPI_SUPPORT_INTERRUPT                    | 特性宏 | 支持中断传输特性               | n   |
| CONFIG_SPI_SUPPORT_DMA                          | 特性宏 | 支持DMA传输特性（与中断互斥）          | n   |
| CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性（使能后此接口不可用） | n   |

### uapi_spi_set_loop_back_mode <a id="uapi_spi_set_loop_back_mode"></a>

```c
errcode_t uapi_spi_set_loop_back_mode(spi_bus_t bus, bool loopback_en)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置环回测试模式，使能/去使能环回测试
- 当前为预留接口，内部直接返回ERRCODE_SUCC

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_LOOPBACK）
- 指定SPI总线已通过uapi_spi_init()初始化完成

**入参**

| 名称           | 参数类型                           | 详细说明       | 约束取值范围                           |
| ------------ | ------------------------------ | ---------- | -------------------------------- |
| bus          | [spi_bus_t](#enum_spi_bus_t) | 指定的SPI接口   | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| loopback_en | bool                           | 环回模式使能/去使能 | true: 使能; false: 去使能             |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 设置成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                            | 宏类型 | 说明          | 默认值 |
| ------------------------------ | --- | ----------- | --- |
| CONFIG_SPI_SUPPORT_LOOPBACK | 特性宏 | 支持SPI环回测试功能 | n   |

### uapi_spi_set_crc_mode <a id="uapi_spi_set_crc_mode"></a>

```c
errcode_t uapi_spi_set_crc_mode(spi_bus_t bus, const spi_crc_config_t *crc_config, spi_crc_err_callback_t cb)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 设置SPI发送和接收CRC模式
- 配置CRC参数包括：发送/接收CRC长度、初始值、多项式、异或输出值、输入/输出值翻转
- 注册CRC校验错误回调函数
- 当前为预留接口，内部直接返回ERRCODE_SUCC

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_CRC）
- 指定SPI总线已通过uapi_spi_init()初始化完成
- 入参crc_config不为NULL

**入参**

| 名称          | 参数类型                                                     | 详细说明        | 约束取值范围                           |
| ----------- | -------------------------------------------------------- | ----------- | -------------------------------- |
| bus         | [spi_bus_t](#enum_spi_bus_t)                           | 指定的SPI接口    | SPI_BUS_0(0) ~ SPI_BUS_1(1) |
| crc_config | const [spi_crc_config_t](#struct_spi_crc_config_t) * | 配置SPI的CRC参数 | 不为NULL                           |
| cb          | [spi_crc_err_callback_t](#spi_crc_err_callback_t)    | CRC校验错误回调函数 | -                                |

**返回值**

| 返回值                 | 文字含义               | 触发场景 |
| ------------------- | ------------------ | ---- |
| ERRCODE_SUCC(0x0) | 成功                 | 设置成功 |
| Other               | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项                       | 宏类型 | 说明            | 默认值 |
| ------------------------- | --- | ------------- | --- |
| CONFIG_SPI_SUPPORT_CRC | 特性宏 | 支持SPI CRC校验功能 | n   |

### uapi_spi_suspend <a id="uapi_spi_suspend"></a>

```c
errcode_t uapi_spi_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 挂起所有SPI通道
- 调用HAL层挂起指定SPI通道，若使能LPC则关闭该通道时钟，若使能DMA则挂起DMA
- 若指定SPI通道未初始化，直接返回ERRCODE_SUCC

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_LPM (Low Power Management)）
- 指定SPI总线已通过uapi_spi_init()初始化完成（未初始化时直接返回成功）

**入参**

| 名称  | 参数类型       | 详细说明              | 约束取值范围                    |
| --- | ---------- | ----------------- | ------------------------- |
| arg | uintptr_t | 挂起所需要的参数，即SPI总线编号 | 0 ~ SPI_BUS_MAX_NUM-1 |

**返回值**

| 返回值                       | 文字含义               | 触发场景       |
| ------------------------- | ------------------ | ---------- |
| ERRCODE_SUCC(0x0)       | 成功                 | 挂起成功       |
| ERRCODE_FAIL(0xFFFFFFFF) | 失败                 | HAL层挂起操作失败 |
| Other                     | 其他错误码，参考errcode_t | 执行失败       |

**Kconfig配置**

| 配置项                       | 宏类型 | 说明             | 默认值 |
| ------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_LPM | 特性宏 | 支持SPI低功耗管理功能   | n   |
| CONFIG_SPI_SUPPORT_LPC | 特性宏 | 支持SPI低功耗时钟控制特性 | n   |
| CONFIG_SPI_SUPPORT_DMA | 特性宏 | 支持DMA传输特性      | n   |

### uapi_spi_resume <a id="uapi_spi_resume"></a>

```c
errcode_t uapi_spi_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/spi.h"
```

**功能说明**

- 恢复所有SPI通道
- 若使能LPC则先打开该通道时钟，再调用HAL层恢复指定SPI通道，若使能DMA则恢复DMA
- 若指定SPI通道未初始化，直接返回ERRCODE_SUCC

**前置条件**

- SPI模块已通过Kconfig配置使能（CONFIG_SPI_SUPPORT_LPM）
- 指定SPI总线已通过uapi_spi_suspend()挂起完成

**入参**

| 名称  | 参数类型       | 详细说明              | 约束取值范围                    |
| --- | ---------- | ----------------- | ------------------------- |
| arg | uintptr_t | 恢复所需要的参数，即SPI总线编号 | 0 ~ SPI_BUS_MAX_NUM-1 |

**返回值**

| 返回值                       | 文字含义               | 触发场景       |
| ------------------------- | ------------------ | ---------- |
| ERRCODE_SUCC(0x0)       | 成功                 | 恢复成功       |
| ERRCODE_FAIL(0xFFFFFFFF) | 失败                 | HAL层恢复操作失败 |
| Other                     | 其他错误码，参考errcode_t | 执行失败       |

**Kconfig配置**

| 配置项                       | 宏类型 | 说明             | 默认值 |
| ------------------------- | --- | -------------- | --- |
| CONFIG_SPI_SUPPORT_LPM | 特性宏 | 支持SPI低功耗管理功能   | n   |
| CONFIG_SPI_SUPPORT_LPC | 特性宏 | 支持SPI低功耗时钟控制特性 | n   |
| CONFIG_SPI_SUPPORT_DMA | 特性宏 | 支持DMA传输特性      | n   |

## Type definitions

### spi_attr_t <a id="typedef_spi_attr_t"></a>

```c
typedef hal_spi_attr_t spi_attr_t;
```

**使用说明**

SPI基础配置参数

**头文件清单**

```c
#include "include/driver/spi.h"
```

### spi_extra_attr_t <a id="typedef_spi_extra_attr_t"></a>

```c
typedef hal_spi_extra_attr_t spi_extra_attr_t;
```

**使用说明**

SPI高级配置参数

**头文件清单**

```c
#include "include/driver/spi.h"
```

### spi_xfer_data_t <a id="typedef_spi_xfer_data_t"></a>

```c
typedef hal_spi_xfer_data_t spi_xfer_data_t;
```

**使用说明**

SPI传输数据结构

**头文件清单**

```c
#include "include/driver/spi.h"
```

### spi_rx_callback_t <a id="spi_rx_callback_t"></a>

```c
typedef void (*spi_rx_callback_t)(const void *buffer, uint32_t length, bool error);
```

**使用说明**

SPI接收数据回调函数

**头文件清单**

```c
#include "include/driver/spi.h"
```

### spi_tx_callback_t <a id="spi_tx_callback_t"></a>

```c
typedef void (*spi_tx_callback_t)(const void *buffer, uint32_t length);
```

**使用说明**

SPI发送数据回调函数

**头文件清单**

```c
#include "include/driver/spi.h"
```

### spi_crc_err_callback_t <a id="spi_crc_err_callback_t"></a>

```c
typedef void (*spi_crc_err_callback_t)(spi_bus_t bus);
```

**使用说明**

SPI CRC校验错误回调函数

**头文件清单**

```c
#include "include/driver/spi.h"
```

## Enumerations

### spi_bus_t <a id="enum_spi_bus_t"></a>

```c
typedef enum spi_bus_t {
    SPI_BUS_0 = 0,  /*!< SPI总线0 */
    SPI_BUS_1 = 1,  /*!< SPI总线1 */
    SPI_BUS_2 = 2,  /*!< SPI总线2 */
    SPI_BUS_3 = 3,  /*!< SPI总线3 */
    SPI_BUS_4 = 4,  /*!< SPI总线4 */
    SPI_BUS_5 = 5,  /*!< SPI总线5 */
    SPI_BUS_6 = 6,  /*!< SPI总线6 */
    SPI_BUS_NONE = SPI_BUS_MAX_NUMBER,  /*!< 无效SPI总线 */
} spi_bus_t;
```

| 枚举成员           | 取值                       | 描述      |
| -------------- | ------------------------ | ------- |
| SPI_BUS_0    | 0                        | SPI总线0  |
| SPI_BUS_1    | 1                        | SPI总线1  |
| SPI_BUS_2    | 2                        | SPI总线2  |
| SPI_BUS_3    | 3                        | SPI总线3  |
| SPI_BUS_4    | 4                        | SPI总线4  |
| SPI_BUS_5    | 5                        | SPI总线5  |
| SPI_BUS_6    | 6                        | SPI总线6  |
| SPI_BUS_NONE | SPI_BUS_MAX_NUMBER(2) | 无效SPI总线 |

### spi_mode_t <a id="enum_spi_mode_t"></a>

```c
typedef enum spi_mode_t {
    SPI_MODE_SLAVE = 0,  /*!< SPI Slave模式 */
    SPI_MODE_MASTER = 1,  /*!< SPI Master模式 */
    SPI_MODE_MAX_NUM,
    SPI_MODE_NONE = SPI_MODE_MAX_NUM,
} spi_mode_t;
```

| 枚举成员              | 取值 | 描述           |
| ----------------- | -- | ------------ |
| SPI_MODE_SLAVE  | 0  | SPI Slave模式  |
| SPI_MODE_MASTER | 1  | SPI Master模式 |
| SPI_MODE_MAX_NUM | 2  | 模式枚举上限 |
| SPI_MODE_NONE | 2  | 无效模式 |

### spi_slave_t <a id="enum_spi_slave_t"></a>

```c
typedef enum spi_slave_t {
    SPI_SLAVE0 = 0,  /*!< SPI从机索引0 */
    SPI_SLAVE1 = 1,  /*!< SPI从机索引1 */
    SPI_SLAVE_MAX_NUM,
    SPI_SLAVE_NONE = SPI_SLAVE_MAX_NUM,
} spi_slave_t;
```

| 枚举成员        | 取值 | 描述       |
| ----------- | -- | -------- |
| SPI_SLAVE0 | 0  | SPI从机索引0 |
| SPI_SLAVE1 | 1  | SPI从机索引1 |
| SPI_SLAVE_MAX_NUM | 2  | 从机枚举上限 |
| SPI_SLAVE_NONE | 2  | 无效从机 |

### hal_spi_trans_mode_t <a id="enum_hal_spi_trans_mode_t"></a>

```c
typedef enum hal_spi_trans_mode_t {
    HAL_SPI_TRANS_MODE_TXRX = 0,  /*!< 收发模式 */
    HAL_SPI_TRANS_MODE_TX = 1,  /*!< 发送模式 */
    HAL_SPI_TRANS_MODE_RX = 2,  /*!< 接收模式 */
    HAL_SPI_TRANS_MODE_EEPROM = 3,  /*!< EEPROM模式 */
    HAL_SPI_TRANS_MODE_MAX,
} hal_spi_trans_mode_t;
```

| 枚举成员                          | 取值 | 描述       |
| ----------------------------- | -- | -------- |
| HAL_SPI_TRANS_MODE_TXRX   | 0  | 收发模式     |
| HAL_SPI_TRANS_MODE_TX     | 1  | 发送模式     |
| HAL_SPI_TRANS_MODE_RX     | 2  | 接收模式     |
| HAL_SPI_TRANS_MODE_EEPROM | 3  | EEPROM (Electrically Erasable Programmable Read-Only Memory) 模式 |
| HAL_SPI_TRANS_MODE_MAX | 4  | 传输模式枚举上限 |

### hal_spi_cfg_clk_cpol_t <a id="enum_hal_spi_cfg_clk_cpol_t"></a>

```c
typedef enum hal_spi_cfg_clk_cpol_t {
    SPI_CFG_CLK_CPOL_0 = 0,  /*!< SPI非激活状态为低电平 */
    SPI_CFG_CLK_CPOL_1 = 1,  /*!< SPI非激活状态为高电平 */
    SPI_CFG_CLK_CPOL_MAX,
} hal_spi_cfg_clk_cpol_t;
```

| 枚举成员                   | 取值 | 描述           |
| ---------------------- | -- | ------------ |
| SPI_CFG_CLK_CPOL_0 | 0  | SPI非激活状态为低电平 |
| SPI_CFG_CLK_CPOL_1 | 1  | SPI非激活状态为高电平 |
| SPI_CFG_CLK_CPOL_MAX | 2  | 时钟极性枚举上限 |

### hal_spi_cfg_clk_cpha_t <a id="enum_hal_spi_cfg_clk_cpha_t"></a>

```c
typedef enum hal_spi_cfg_clk_cpha_t {
    SPI_CFG_CLK_CPHA_0 = 0,  /*!< SPI时钟在第一个数据位中间切换 */
    SPI_CFG_CLK_CPHA_1 = 1,  /*!< SPI时钟在第一个数据位开始时切换 */
    SPI_CFG_CLK_CPHA_MAX,
} hal_spi_cfg_clk_cpha_t;
```

| 枚举成员                   | 取值 | 描述                |
| ---------------------- | -- | ----------------- |
| SPI_CFG_CLK_CPHA_0 | 0  | SPI时钟在第一个数据位中间切换  |
| SPI_CFG_CLK_CPHA_1 | 1  | SPI时钟在第一个数据位开始时切换 |
| SPI_CFG_CLK_CPHA_MAX | 2  | 时钟相位枚举上限 |

### hal_spi_cfg_frame_format_t <a id="enum_hal_spi_cfg_frame_format_t"></a>

```c
typedef enum hal_spi_cfg_frame_format_t {
    SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI = 0,  /*!< 摩托罗拉SPI帧格式 */
    SPI_CFG_FRAME_FORMAT_TEXAS_SSP = 1,  /*!< 德州仪器SSP帧格式 */
    SPI_CFG_FRAME_FORMAT_NS_MICROWIRE = 2,  /*!< 国家微线帧格式 */
    SPI_CFG_FRAME_FORMAT_MAX,
} hal_spi_cfg_frame_format_t;
```

| 枚举成员                                   | 取值 | 描述         |
| -------------------------------------- | -- | ---------- |
| SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI | 0  | 摩托罗拉SPI帧格式 |
| SPI_CFG_FRAME_FORMAT_TEXAS_SSP    | 1  | 德州仪器SSP帧格式 |
| SPI_CFG_FRAME_FORMAT_NS_MICROWIRE | 2  | 国家微线帧格式    |
| SPI_CFG_FRAME_FORMAT_MAX | 3  | 帧格式枚举上限 |

### hal_spi_cfg_sste_t <a id="enum_hal_spi_cfg_sste_t"></a>

```c
typedef enum hal_spi_cfg_sste_t {
    SPI_CFG_SSTE_DISABLE = 0,  /*!< SPI从机选择切换不使能。不使能时，主机从从机读取数据需一次性读完，否则数据丢失 */
    SPI_CFG_SSTE_ENABLE = 1,  /*!< SPI从机选择切换使能 */
    SPI_CFG_SSTE_MAX,
} hal_spi_cfg_sste_t;
```

| 枚举成员                    | 取值 | 描述                                       |
| ----------------------- | -- | ---------------------------------------- |
| SPI_CFG_SSTE_DISABLE | 0  | SPI从机选择切换不使能。不使能时，主机从从机读取数据需一次性读完，否则数据丢失 |
| SPI_CFG_SSTE_ENABLE  | 1  | SPI从机选择切换使能                              |
| SPI_CFG_SSTE_MAX | 2  | 从机选择切换枚举上限 |

### hal_spi_frame_size_t <a id="enum_hal_spi_frame_size_t"></a>

```c
typedef enum hal_spi_frame_size_t {
    HAL_SPI_FRAME_SIZE_8 = 0x07,  /*!< 8位串行数据传输 */
    HAL_SPI_FRAME_SIZE_16 = 0x0F,  /*!< 16位串行数据传输（暂不支持） */
    HAL_SPI_FRAME_SIZE_24 = 0x17,  /*!< 24位串行数据传输（暂不支持） */
    HAL_SPI_FRAME_SIZE_32 = 0x1F,  /*!< 32位串行数据传输 */
} hal_spi_frame_size_t;
```

| 枚举成员                      | 取值   | 描述              |
| ------------------------- | ---- | --------------- |
| HAL_SPI_FRAME_SIZE_8  | 0x07 | 8位串行数据传输        |
| HAL_SPI_FRAME_SIZE_16 | 0x0F | 16位串行数据传输（暂不支持） |
| HAL_SPI_FRAME_SIZE_24 | 0x17 | 24位串行数据传输（暂不支持） |
| HAL_SPI_FRAME_SIZE_32 | 0x1F | 32位串行数据传输       |

### hal_spi_frame_format_t <a id="enum_hal_spi_frame_format_t"></a>

```c
typedef enum hal_spi_frame_format_t {
    HAL_SPI_FRAME_FORMAT_STANDARD = 0,  /*!< 标准单线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_DUAL = 1,  /*!< 双线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_QUAD = 2,  /*!< 4线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_OCTAL = 3,  /*!< 8线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_DOUBLE_OCTAL = 4,  /*!< 16线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_SIXT = 5,  /*!< 32线SPI帧格式 */
    HAL_SPI_FRAME_FORMAT_MAX_NUM,
    HAL_SPI_FRAME_FORMAT_NONE = HAL_SPI_FRAME_FORMAT_MAX_NUM,
} hal_spi_frame_format_t;
```

| 枚举成员                                   | 取值 | 描述         |
| -------------------------------------- | -- | ---------- |
| HAL_SPI_FRAME_FORMAT_STANDARD      | 0  | 标准单线SPI帧格式 |
| HAL_SPI_FRAME_FORMAT_DUAL          | 1  | 双线SPI帧格式   |
| HAL_SPI_FRAME_FORMAT_QUAD          | 2  | 4线SPI帧格式   |
| HAL_SPI_FRAME_FORMAT_OCTAL         | 3  | 8线SPI帧格式   |
| HAL_SPI_FRAME_FORMAT_DOUBLE_OCTAL | 4  | 16线SPI帧格式  |
| HAL_SPI_FRAME_FORMAT_SIXT | 5  | 32线SPI帧格式  |
| HAL_SPI_FRAME_FORMAT_MAX_NUM | 6  | 帧格式枚举上限 |
| HAL_SPI_FRAME_FORMAT_NONE | 6  | 无效帧格式 |

### hal_spi_trans_type_t <a id="enum_hal_spi_trans_type_t"></a>

```c
typedef enum hal_spi_trans_type_t {
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_S = 0,  /*!< 指令和地址使用单线SPI传输 */
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q = 1,  /*!< 指令使用单线SPI传输，地址按帧格式寄存器配置传输 */
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q = 2,  /*!< 指令和地址都按帧格式寄存器配置传输 */
    HAL_SPI_TRANS_TYPE_MAX = HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
} hal_spi_trans_type_t;
```

| 枚举成员                                    | 取值 | 描述                        |
| --------------------------------------- | -- | ------------------------- |
| HAL_SPI_TRANS_TYPE_INST_S_ADDR_S | 0  | 指令和地址使用单线SPI传输            |
| HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q | 1  | 指令使用单线SPI传输，地址按帧格式寄存器配置传输 |
| HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q | 2  | 指令和地址都按帧格式寄存器配置传输         |
| HAL_SPI_TRANS_TYPE_MAX | 2  | 传输类型枚举上限 |

### hal_spi_inst_len_t <a id="enum_hal_spi_inst_len_t"></a>

```c
typedef enum hal_spi_inst_len_t {
    HAL_SPI_INST_LEN_0 = 0,  /*!< 不携带指令 */
    HAL_SPI_INST_LEN_4 = 1,  /*!< 4位指令 */
    HAL_SPI_INST_LEN_8 = 2,  /*!< 8位指令 */
    HAL_SPI_INST_LEN_16 = 3,  /*!< 16位指令 */
    HAL_SPI_INST_LEN_MAX = HAL_SPI_INST_LEN_16,
} hal_spi_inst_len_t;
```

| 枚举成员                    | 取值 | 描述    |
| ----------------------- | -- | ----- |
| HAL_SPI_INST_LEN_0  | 0  | 不携带指令 |
| HAL_SPI_INST_LEN_4  | 1  | 4位指令  |
| HAL_SPI_INST_LEN_8  | 2  | 8位指令  |
| HAL_SPI_INST_LEN_16 | 3  | 16位指令 |
| HAL_SPI_INST_LEN_MAX | 3  | 指令长度枚举上限 |

### hal_spi_addr_len_t <a id="enum_hal_spi_addr_len_t"></a>

```c
typedef enum hal_spi_addr_len_t {
    HAL_SPI_ADDR_LEN_0 = 0,  /*!< 0位地址长度 */
    HAL_SPI_ADDR_LEN_4 = 1,  /*!< 4位地址长度 */
    HAL_SPI_ADDR_LEN_8 = 2,  /*!< 8位地址长度 */
    HAL_SPI_ADDR_LEN_12 = 3,  /*!< 12位地址长度 */
    HAL_SPI_ADDR_LEN_16 = 4,  /*!< 16位地址长度 */
    HAL_SPI_ADDR_LEN_20 = 5,  /*!< 20位地址长度 */
    HAL_SPI_ADDR_LEN_24 = 6,  /*!< 24位地址长度 */
    HAL_SPI_ADDR_LEN_28 = 7,  /*!< 28位地址长度 */
    HAL_SPI_ADDR_LEN_32 = 8,  /*!< 32位地址长度 */
    HAL_SPI_ADDR_LEN_36 = 9,  /*!< 36位地址长度 */
    HAL_SPI_ADDR_LEN_40 = 10,  /*!< 40位地址长度 */
    HAL_SPI_ADDR_LEN_44 = 11,  /*!< 44位地址长度 */
    HAL_SPI_ADDR_LEN_48 = 12,  /*!< 48位地址长度 */
    HAL_SPI_ADDR_LEN_52 = 13,  /*!< 52位地址长度 */
    HAL_SPI_ADDR_LEN_56 = 14,  /*!< 56位地址长度 */
    HAL_SPI_ADDR_LEN_60 = 15,  /*!< 60位地址长度 */
    HAL_SPI_ADDR_LEN_MAX = HAL_SPI_ADDR_LEN_60,
} hal_spi_addr_len_t;
```

| 枚举成员                    | 取值 | 描述      |
| ----------------------- | -- | ------- |
| HAL_SPI_ADDR_LEN_0  | 0  | 0位地址长度  |
| HAL_SPI_ADDR_LEN_4  | 1  | 4位地址长度  |
| HAL_SPI_ADDR_LEN_8  | 2  | 8位地址长度  |
| HAL_SPI_ADDR_LEN_12 | 3  | 12位地址长度 |
| HAL_SPI_ADDR_LEN_16 | 4  | 16位地址长度 |
| HAL_SPI_ADDR_LEN_20 | 5  | 20位地址长度 |
| HAL_SPI_ADDR_LEN_24 | 6  | 24位地址长度 |
| HAL_SPI_ADDR_LEN_28 | 7  | 28位地址长度 |
| HAL_SPI_ADDR_LEN_32 | 8  | 32位地址长度 |
| HAL_SPI_ADDR_LEN_36 | 9  | 36位地址长度 |
| HAL_SPI_ADDR_LEN_40 | 10 | 40位地址长度 |
| HAL_SPI_ADDR_LEN_44 | 11 | 44位地址长度 |
| HAL_SPI_ADDR_LEN_48 | 12 | 48位地址长度 |
| HAL_SPI_ADDR_LEN_52 | 13 | 52位地址长度 |
| HAL_SPI_ADDR_LEN_56 | 14 | 56位地址长度 |
| HAL_SPI_ADDR_LEN_60 | 15 | 60位地址长度 |
| HAL_SPI_ADDR_LEN_MAX | 15 | 地址长度枚举上限 |

## Structures

### spi_dma_config_t <a id="struct_spi_dma_config_t"></a>

```c
typedef struct spi_dma_config {
    uint8_t src_width;          /*!< 源端传输数据宽度。0: 1字节; 1: 2字节; 2: 4字节 */
    uint8_t dest_width;         /*!< 目的端传输数据宽度。0: 1字节; 1: 2字节; 2: 4字节 */
    uint8_t burst_length;       /*!< 每次目的burst请求写入的数据量。0: 1; 1: 4; 2: 8; 3: 16 */
    uint8_t priority;           /*!< 传输通道优先级，最小为0，最大为3 */
} spi_dma_config_t;
```

| 成员名称          | 数据类型     | 描述                                        |
| ------------- | -------- | ----------------------------------------- |
| src_width    | uint8_t | 源端传输数据宽度。0: 1字节; 1: 2字节; 2: 4字节           |
| dest_width   | uint8_t | 目的端传输数据宽度。0: 1字节; 1: 2字节; 2: 4字节          |
| burst_length | uint8_t | 每次目的burst请求写入的数据量。0: 1; 1: 4; 2: 8; 3: 16 |
| priority      | uint8_t | 传输通道优先级，最小为0，最大为3                         |

### spi_crc_config_t <a id="struct_spi_crc_config_t"></a>

```c
typedef struct spi_crc_config {
    uint32_t  tx_crc_len;       /*!< 逻辑在此长度之前发送CRC验证数据 */
    uint32_t  rx_crc_len;       /*!< 逻辑在此长度之前接收CRC验证数据 */
    uint32_t  tx_crc_ini;       /*!< 发送初始化值配置 */
    uint32_t  rx_crc_ini;       /*!< 接收初始化值配置 */
    uint32_t  tx_crc_poly;      /*!< 发送多项式配置 */
    uint32_t  rx_crc_poly;      /*!< 接收多项式配置 */
    uint32_t  tx_crc_xor_out;   /*!< 发送结果异或配置 */
    uint32_t  rx_crc_xor_out;   /*!< 接收结果异或配置 */
    bool      tx_crc_refin;     /*!< 发送输入值翻转配置 */
    bool      tx_crc_refout;    /*!< 发送输出值翻转配置 */
    bool      rx_crc_refin;     /*!< 接收输入值翻转配置 */
    bool      rx_crc_refout;    /*!< 接收输出值翻转配置 */
} spi_crc_config_t;
```

| 成员名称              | 数据类型      | 描述                |
| ----------------- | --------- | ----------------- |
| tx_crc_len      | uint32_t | 逻辑在此长度之前发送CRC验证数据 |
| rx_crc_len      | uint32_t | 逻辑在此长度之前接收CRC验证数据 |
| tx_crc_ini      | uint32_t | 发送初始化值配置          |
| rx_crc_ini      | uint32_t | 接收初始化值配置          |
| tx_crc_poly     | uint32_t | 发送多项式配置           |
| rx_crc_poly     | uint32_t | 接收多项式配置           |
| tx_crc_xor_out | uint32_t | 发送结果异或配置          |
| rx_crc_xor_out | uint32_t | 接收结果异或配置          |
| tx_crc_refin    | bool      | 发送输入值翻转配置         |
| tx_crc_refout   | bool      | 发送输出值翻转配置         |
| rx_crc_refin    | bool      | 接收输入值翻转配置         |
| rx_crc_refout   | bool      | 接收输出值翻转配置         |

### spi_attr_t <a id="struct_spi_attr_t"></a>

```c
typedef struct spi_attr_t {
    bool is_slave;                      /*!< SPI工作模式。false: Master模式; true: Slave模式 */
    uint32_t slave_num;                 /*!< 选择从机时的索引。0: 不选择; 1: 从机索引0; 2: 从机索引1 */
    uint32_t bus_clk;                   /*!< 用于计算SPI的时钟分频系数 */
    uint32_t freq_mhz;                  /*!< SPI的工作频率（单位：MHz） */
    uint32_t clk_polarity;              /*!< SPI的时钟极性，参考 [hal_spi_cfg_clk_cpol_t](#enum_hal_spi_cfg_clk_cpol_t) */
    uint32_t clk_phase;                 /*!< SPI的时钟相位，参考 [hal_spi_cfg_clk_cpha_t](#enum_hal_spi_cfg_clk_cpha_t) */
    uint32_t frame_format;              /*!< 选择串行传输的协议，参考 [hal_spi_cfg_frame_format_t](#enum_hal_spi_cfg_frame_format_t) */
    uint32_t spi_frame_format;          /*!< SPI的帧格式，参考 [hal_spi_frame_format_t](#enum_hal_spi_frame_format_t) */
    uint32_t frame_size;                /*!< SPI的帧长度，参考 [hal_spi_frame_size_t](#enum_hal_spi_frame_size_t) */
    uint32_t tmod;                      /*!< SPI的传输模式，参考 [hal_spi_trans_mode_t](#enum_hal_spi_trans_mode_t) */
    uint32_t ndf;                       /*!< SPI的数据帧数 */
    uint32_t sste;                      /*!< SPI从机选择切换使能。参考 [hal_spi_cfg_sste_t](#enum_hal_spi_cfg_sste_t) */
} spi_attr_t;
```

| 成员名称               | 数据类型      | 描述                                                                               |
| ------------------ | --------- | -------------------------------------------------------------------------------- |
| is_slave          | bool      | SPI工作模式。false: Master模式; true: Slave模式                                           |
| slave_num         | uint32_t | 选择从机时的索引。0: 不选择; 1: 从机索引0; 2: 从机索引1                                              |
| bus_clk           | uint32_t | 用于计算SPI的时钟分频系数                                                                   |
| freq_mhz          | uint32_t | SPI的工作频率（单位：MHz）                                                                 |
| clk_polarity      | uint32_t | SPI的时钟极性，参考 [hal_spi_cfg_clk_cpol_t](#enum_hal_spi_cfg_clk_cpol_t)          |
| clk_phase         | uint32_t | SPI的时钟相位，参考 [hal_spi_cfg_clk_cpha_t](#enum_hal_spi_cfg_clk_cpha_t)          |
| frame_format      | uint32_t | 选择串行传输的协议，参考 [hal_spi_cfg_frame_format_t](#enum_hal_spi_cfg_frame_format_t) |
| spi_frame_format | uint32_t | SPI的帧格式，参考 [hal_spi_frame_format_t](#enum_hal_spi_frame_format_t)            |
| frame_size        | uint32_t | SPI的帧长度，参考 [hal_spi_frame_size_t](#enum_hal_spi_frame_size_t)                |
| tmod               | uint32_t | SPI的传输模式，参考 [hal_spi_trans_mode_t](#enum_hal_spi_trans_mode_t)               |
| ndf                | uint32_t | SPI的数据帧数                                                                         |
| sste               | uint32_t | SPI从机选择切换使能。参考 [hal_spi_cfg_sste_t](#enum_hal_spi_cfg_sste_t)                |

### spi_extra_attr_t <a id="struct_spi_extra_attr_t"></a>

```c
typedef struct spi_extra_attr_t {
    bool tx_use_dma;                        /*!< SPI是否使用DMA发送数据 */
    bool rx_use_dma;                        /*!< SPI是否使用DMA接收数据 */
    [hal_spi_xfer_qspi_param_t](#struct_hal_spi_xfer_qspi_param_t) qspi_param;  /*!< QSPI参数 */
    [hal_spi_xfer_sspi_param_t](#struct_hal_spi_xfer_sspi_param_t) sspi_param;  /*!< Single SPI参数 */
} spi_extra_attr_t;
```

| 成员名称         | 数据类型                                                                | 描述             |
| ------------ | ------------------------------------------------------------------- | -------------- |
| tx_use_dma | bool                                                                | SPI是否使用DMA发送数据 |
| rx_use_dma | bool                                                                | SPI是否使用DMA接收数据 |
| qspi_param  | [hal_spi_xfer_qspi_param_t](#struct_hal_spi_xfer_qspi_param_t) | QSPI参数         |
| sspi_param  | [hal_spi_xfer_sspi_param_t](#struct_hal_spi_xfer_sspi_param_t) | Single SPI参数   |

### spi_xfer_data_t <a id="struct_spi_xfer_data_t"></a>

```c
typedef struct spi_xfer_data_t {
    uint8_t * tx_buff;  /*!< 通过TX FIFO发送数据的缓冲区 */
    uint32_t tx_bytes;  /*!< 发送数据的字节数。需根据frame_size设定为对应倍数 */
    uint8_t * rx_buff;  /*!< 通过RX FIFO接收数据的缓冲区 */
    uint32_t rx_bytes;  /*!< 接收数据的字节数。需根据frame_size设定为对应倍数 */
    uint8_t cmd;  /*!< QSPI模式下的命令 */
    uint8_t reserved[3];  /*!< 保留 */
#if defined(CONFIG_SPI_SLAVE_SUPPORT_NOTIFY)
    slave_notify_callback notify_callback;  /*!< Slave通知Master发送时钟或数据 */
    void * privdata;  /*!< 回调私有数据 */
#endif
    uint32_t addr;  /*!< QSPI模式下的地址 */
} spi_xfer_data_t;
```

| 成员名称             | 数据类型                    | 描述                                                         |
| ---------------- | ----------------------- | ---------------------------------------------------------- |
| tx_buff         | uint8_t *             | 通过TX (Transmit) FIFO (First-In First-Out) 发送数据的缓冲区                                          |
| tx_bytes        | uint32_t               | 发送数据的字节数。需根据frame_size设定为对应倍数                             |
| rx_buff         | uint8_t *             | 通过RX (Receive) FIFO接收数据的缓冲区                                          |
| rx_bytes        | uint32_t               | 接收数据的字节数。需根据frame_size设定为对应倍数                             |
| cmd              | uint8_t                | QSPI模式下的命令                                                 |
| reserved         | uint8_t[3]            | 保留                                                         |
| notify_callback | slave_notify_callback | Slave通知Master发送时钟或数据（需CONFIG_SPI_SLAVE_SUPPORT_NOTIFY） |
| privdata         | void *                 | 回调私有数据（需CONFIG_SPI_SLAVE_SUPPORT_NOTIFY）               |
| addr             | uint32_t               | QSPI模式下的地址                                                 |

### hal_spi_xfer_qspi_param_t <a id="struct_hal_spi_xfer_qspi_param_t"></a>

```c
typedef struct hal_spi_xfer_qspi_param_t {
    [hal_spi_trans_type_t](#enum_hal_spi_trans_type_t) trans_type;  /*!< 传输类型，用于指定指令和地址的长度 */
    [hal_spi_inst_len_t](#enum_hal_spi_inst_len_t) inst_len;  /*!< 指令长度，支持0、4、8、16位 */
    [hal_spi_addr_len_t](#enum_hal_spi_addr_len_t) addr_len;  /*!< 地址长度，支持0、8、16、24、32位 */
    uint32_t wait_cycles;  /*!< 等待的周期数 */
} hal_spi_xfer_qspi_param_t;
```

| 成员名称         | 数据类型                                                   | 描述                   |
| ------------ | ------------------------------------------------------ | -------------------- |
| trans_type  | [hal_spi_trans_type_t](#enum_hal_spi_trans_type_t) | 传输类型，用于指定指令和地址的长度    |
| inst_len    | [hal_spi_inst_len_t](#enum_hal_spi_inst_len_t)     | 指令长度，支持0、4、8、16位     |
| addr_len    | [hal_spi_addr_len_t](#enum_hal_spi_addr_len_t)     | 地址长度，支持0、8、16、24、32位 |
| wait_cycles | uint32_t                                              | 等待的周期数               |

### hal_spi_xfer_sspi_param_t <a id="struct_hal_spi_xfer_sspi_param_t"></a>

```c
typedef struct hal_spi_xfer_sspi_param {
    uint32_t             wait_cycles; /*!< @if Eng Indicates the wait cycles.
                                           @else   等待的周期数。 @endif */
} hal_spi_xfer_sspi_param_t;
```

| 成员名称         | 数据类型      | 描述     |
| ------------ | --------- | ------ |
| wait_cycles | uint32_t | 等待的周期数 |


