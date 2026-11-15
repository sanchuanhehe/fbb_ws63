# I2C

I2C (Inter-Integrated Circuit) 是两线制串行总线（SCL (Serial Clock Line) 时钟 + SDA (Serial Data Line) 数据），采用主从模型，支持多设备挂载同一条总线。WS63 I2C 支持 Master 和 Slave 两种角色，提供中断和 DMA (Direct Memory Access) 两种传输模式。

Master 模式下写入/读取 Slave 设备数据；Slave 模式下响应 Master 的读写请求。适用 EEPROM (Electrically Erasable Programmable Read-Only Memory)、温湿度传感器、OLED (Organic Light-Emitting Diode) 显示屏等外设通信。

**头文件清单**

```c
#include "include/driver/i2c.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_i2c_master_init](#uapi_i2c_master_init) | 初始化I2C为主机模式 |
| [uapi_i2c_master_write](#uapi_i2c_master_write) | 主机向目标从机写入数据 |
| [uapi_i2c_master_read](#uapi_i2c_master_read) | 主机从目标从机读取数据 |
| [uapi_i2c_master_writeread](#uapi_i2c_master_writeread) | 主机向目标从机写入数据并读取数据 |
| [uapi_i2c_slave_init](#uapi_i2c_slave_init) | 初始化I2C为从机模式 |
| [uapi_i2c_slave_write](#uapi_i2c_slave_write) | 从机向主机写入数据 |
| [uapi_i2c_slave_read](#uapi_i2c_slave_read) | 从机从主机读取数据 |
| [uapi_i2c_set_irq_mode](#uapi_i2c_set_irq_mode) | 设置是否使用中断模式传输数据 |
| [uapi_i2c_register_irq_callback](#uapi_i2c_register_irq_callback) | 注册I2C中断事件回调函数 |
| [uapi_i2c_unregister_irq_callback](#uapi_i2c_unregister_irq_callback) | 取消注册I2C中断事件回调函数 |
| [uapi_i2c_set_dma_mode](#uapi_i2c_set_dma_mode) | 使能/去使能DMA模式下I2C传输 |
| [uapi_i2c_deinit](#uapi_i2c_deinit) | 去初始化I2C，支持主从机 |
| [uapi_i2c_set_baudrate](#uapi_i2c_set_baudrate) | 重置已初始化I2C的波特率 |
| [uapi_i2c_suspend](#uapi_i2c_suspend) | 挂起所有I2C通道 |
| [uapi_i2c_resume](#uapi_i2c_resume) | 恢复所有I2C通道 |

## Functions

### uapi_i2c_master_init <a id="uapi_i2c_master_init"></a>

```c
errcode_t uapi_i2c_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 根据指定的参数初始化I2C为主机模式
- 支持标准模式（100KHz）、快速模式（400KHz）、高速模式（3.4MHz）三种波特率配置
- 高速模式下需配置唯一主机码，用于高速仲裁

**前置条件**

- I2C总线未被初始化（未被其他模块占用）
- 对应I2C引脚已通过pinctrl配置为I2C功能模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| baudrate | uint32_t | I2C波特率 | 标准模式上限：100000; 快速模式上限：400000; 高速模式上限：3400000; 且不为0 |
| hscode | uint8_t | I2C高速模式主机码 | 0 ~ 7，仅在高速模式下需要配置 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [helloworld.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/helloworld_oled/helloworld.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_MASTER | 特性宏 | 支持I2C主机功能 | y |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |

### uapi_i2c_master_write <a id="uapi_i2c_master_write"></a>

```c
errcode_t uapi_i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 主机向目标从机写入数据
- 支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换），两种方式静态配置
- 自动切换模式下，数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式
- 手动切换模式的三种传输模式不能在同一总线中同时使用

**前置条件**

- I2C已通过uapi_i2c_master_init()初始化为主机模式
- data指针非空，且send_buf非空，send_len大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| dev_addr | uint16_t | 目标从机地址 | 7比特地址范围[0x8, 0x77]; 10比特地址范围[0x7800, 0x7BFF] |
| data | [i2c_data_t](#struct_i2c_data) * | 发送数据信息指针 | 非NULL，send_buf非NULL，send_len大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 写入成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ssd1306.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/helloworld_oled/ssd1306.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_MASTER | 特性宏 | 支持I2C主机功能 | y |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |
| CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |

### uapi_i2c_master_read <a id="uapi_i2c_master_read"></a>

```c
errcode_t uapi_i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 主机从目标从机读取数据
- 支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换），两种方式静态配置
- 自动切换模式下，数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式
- 手动切换模式的三种传输模式不能在同一总线中同时使用

**前置条件**

- I2C已通过uapi_i2c_master_init()初始化为主机模式
- data指针非空，且receive_buf非空，receive_len大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| dev_addr | uint16_t | 目标从机地址 | 7比特地址范围[0x8, 0x77]; 10比特地址范围[0x7800, 0x7BFF] |
| data | [i2c_data_t](#struct_i2c_data) * | 接收数据信息指针 | 非NULL，receive_buf非NULL，receive_len大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 读取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2c_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2c/i2c_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_MASTER | 特性宏 | 支持I2C主机功能 | y |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |
| CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |

### uapi_i2c_master_writeread <a id="uapi_i2c_master_writeread"></a>

```c
errcode_t uapi_i2c_master_writeread(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 主机向目标从机写入数据并接收来自此从机的数据，在一次传输中完成写操作和读操作
- 支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换），两种方式静态配置
- 自动切换模式下，数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式
- 手动切换模式的三种传输模式不能在同一总线中同时使用

**前置条件**

- I2C已通过uapi_i2c_master_init()初始化为主机模式
- data指针非空，且send_buf非空、send_len大于0，receive_buf非空、receive_len大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| dev_addr | uint16_t | 目标从机地址 | 7比特地址范围[0x8, 0x77]; 10比特地址范围[0x7800, 0x7BFF] |
| data | [i2c_data_t](#struct_i2c_data) * | 收发数据信息指针 | 非NULL，send_buf非NULL且send_len大于0，receive_buf非NULL且receive_len大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 写入并读取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_MASTER | 特性宏 | 支持I2C主机功能 | y |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |
| CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |

### uapi_i2c_slave_init <a id="uapi_i2c_slave_init"></a>

```c
errcode_t uapi_i2c_slave_init(i2c_bus_t bus, uint32_t baudrate, uint16_t addr)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 根据指定的参数初始化I2C为从机模式
- 波特率需与主机保持一致
- 支持7比特地址和10比特地址两种寻址模式

**前置条件**

- I2C总线未被初始化（未被其他模块占用）
- 对应I2C引脚已通过pinctrl配置为I2C功能模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| baudrate | uint32_t | I2C波特率，需与主机保持一致 | 标准模式上限：100000; 快速模式上限：400000; 高速模式上限：3400000; 且不为0 |
| addr | uint16_t | 从机地址 | 7比特地址范围[0x8, 0x77]; 10比特地址范围[0x7800, 0x7BFF] |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2c_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2c/i2c_slave_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_SLAVE | 特性宏 | 支持I2C从机功能 | y |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |

### uapi_i2c_slave_write <a id="uapi_i2c_slave_write"></a>

```c
errcode_t uapi_i2c_slave_write(i2c_bus_t bus, i2c_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 从机向主机写入数据
- 支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换），两种方式静态配置
- 自动切换模式下，数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式
- 手动切换模式的三种传输模式不能在同一总线中同时使用

**前置条件**

- I2C已通过uapi_i2c_slave_init()初始化为从机模式
- data指针非空，且send_buf非空，send_len大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| data | [i2c_data_t](#struct_i2c_data) * | 发送数据信息指针 | 非NULL，send_buf非NULL，send_len大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 写入成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2c_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2c/i2c_slave_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_SLAVE | 特性宏 | 支持I2C从机功能 | y |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |
| CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |

### uapi_i2c_slave_read <a id="uapi_i2c_slave_read"></a>

```c
errcode_t uapi_i2c_slave_read(i2c_bus_t bus, i2c_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 从机从主机读取数据
- 支持手动切换模式（轮询/DMA/中断）和自动切换模式（轮询与DMA自动切换），两种方式静态配置
- 自动切换模式下，数据长度小于等于阈值时使用轮询模式，大于阈值时自动切换为DMA模式
- 手动切换模式的三种传输模式不能在同一总线中同时使用

**前置条件**

- I2C已通过uapi_i2c_slave_init()初始化为从机模式
- data指针非空，且receive_buf非空，receive_len大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| data | [i2c_data_t](#struct_i2c_data) * | 接收数据信息指针 | 非NULL，receive_buf非NULL，receive_len大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 读取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2c_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2c/i2c_slave_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_SLAVE | 特性宏 | 支持I2C从机功能 | y |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |
| CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH | 特性宏 | 支持轮询与DMA自动切换特性 | n |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持中断模式传输特性 | n |

### uapi_i2c_set_irq_mode <a id="uapi_i2c_set_irq_mode"></a>

```c
errcode_t uapi_i2c_set_irq_mode(i2c_bus_t bus, bool irq_en)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 设置是否使用中断模式传输数据
- 使能中断模式后，I2C传输通过中断回调方式完成数据收发
- 去使能中断模式后，I2C传输切换为轮询模式

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| irq_en | bool | 是否使用中断模式 | true: 使能中断模式; false: 去使能中断模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2c_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2c/i2c_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持I2C中断模式传输特性 | n |

### uapi_i2c_register_irq_callback <a id="uapi_i2c_register_irq_callback"></a>

```c
errcode_t uapi_i2c_register_irq_callback(i2c_bus_t bus, i2c_irq_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 注册I2C中断事件回调函数
- 回调函数在中断上下文中执行，需注意执行时效性
- 回调函数接收总线编号和中断事件类型参数

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成
- 已通过uapi_i2c_set_irq_mode()使能中断模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| callback | [i2c_irq_callback_t](#typedef_i2c_irq_callback_t) | 中断事件回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持I2C中断模式传输特性 | n |

### uapi_i2c_unregister_irq_callback <a id="uapi_i2c_unregister_irq_callback"></a>

```c
errcode_t uapi_i2c_unregister_irq_callback(i2c_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 取消注册I2C中断事件回调函数
- 取消注册后，I2C中断事件不再触发用户回调

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成
- 已通过uapi_i2c_register_irq_callback()注册过回调函数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 取消注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_INT | 特性宏 | 支持I2C中断模式传输特性 | n |

### uapi_i2c_set_dma_mode <a id="uapi_i2c_set_dma_mode"></a>

```c
errcode_t uapi_i2c_set_dma_mode(i2c_bus_t bus, bool en)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 使能/去使能DMA模式下I2C传输
- 使能DMA模式后，I2C传输通过DMA方式完成数据搬移
- 去使能DMA模式后，I2C传输切换为轮询模式

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成
- DMA模块已通过uapi_dma_init()初始化并已打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| en | bool | 是否使能DMA传输 | true: 使能DMA模式; false: 去使能DMA模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持I2C DMA模式功能 | n |

### uapi_i2c_deinit <a id="uapi_i2c_deinit"></a>

```c
errcode_t uapi_i2c_deinit(i2c_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 去初始化I2C，支持主从机模式
- 释放I2C总线占用的硬件资源，注销中断，销毁互斥锁和信号量
- 去初始化后I2C总线不可使用，需重新初始化后方可使用

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 去初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_DMA | 特性宏 | 支持DMA传输特性 | n |

### uapi_i2c_set_baudrate <a id="uapi_i2c_set_baudrate"></a>

```c
errcode_t uapi_i2c_set_baudrate(i2c_bus_t bus, uint32_t baudrate)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 对已初始化的I2C重置波特率，支持主从机模式
- 主机模式下重新调用hal_i2c_master_init更新波特率，从机模式下重新调用hal_i2c_slave_init更新波特率
- 波特率不能超过IP (Internet Protocol) 上限

**前置条件**

- I2C已通过uapi_i2c_master_init()或uapi_i2c_slave_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [i2c_bus_t](#enum_i2c_bus_t) | I2C总线编号 | I2C_BUS_0, I2C_BUS_1 |
| baudrate | uint32_t | I2C波特率 | 标准模式上限：100000; 快速模式上限：400000; 高速模式上限：3400000; 且不为0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_i2c_suspend <a id="uapi_i2c_suspend"></a>

```c
errcode_t uapi_i2c_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 挂起所有I2C通道
- 用于低功耗模式场景，挂起I2C外设运行

**前置条件**

- I2C模块已初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起所需要的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 挂起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_LPM | 特性宏 | 支持I2C低功耗模式功能 | n |

### uapi_i2c_resume <a id="uapi_i2c_resume"></a>

```c
errcode_t uapi_i2c_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/i2c.h"
```

**功能说明**

- 恢复所有I2C通道
- 用于低功耗模式场景，恢复I2C外设运行

**前置条件**

- I2C已通过uapi_i2c_suspend()挂起

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复所需要的参数 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 恢复成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2C_SUPPORT_LPM | 特性宏 | 支持I2C低功耗模式功能 | n |

## Type definitions

### i2c_irq_callback_t <a id="typedef_i2c_irq_callback_t"></a>

```c
typedef void (*i2c_irq_callback_t)(i2c_bus_t bus, uint8_t event);
```

**使用说明**

I2C中断事件回调函数类型，通过uapi_i2c_register_irq_callback()注册到驱动中，在中断上下文中执行。bus为I2C总线编号，event为中断事件类型，参考[i2c_irq_event_t](#enum_i2c_irq_event_t)。

## Enumerations

### i2c_bus_t <a id="enum_i2c_bus_t"></a>

```c
typedef enum {
    I2C_BUS_0,               // !< I2C0
    I2C_BUS_1,               // !< I2C1
    I2C_BUS_NONE = I2C_BUS_MAX_NUMBER
} i2c_bus_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| I2C_BUS_0 | 0 | I2C总线0 |
| I2C_BUS_1 | 1 | I2C总线1 |
| I2C_BUS_NONE | I2C_BUS_MAX_NUMBER | 无效/未使用的I2C总线编号 |

### i2c_irq_event_t <a id="enum_i2c_irq_event_t"></a>

```c
typedef enum {
    I2C_IRQ_EVT_RX_DONE,
    I2C_IRQ_EVT_TX_DONE,
    I2C_IRQ_EVT_I2C_BUSY,
    I2C_IRQ_EVT_I2C_ERR,
}i2c_irq_event_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| I2C_IRQ_EVT_RX_DONE | 0 | 接收完成事件 |
| I2C_IRQ_EVT_TX_DONE | 1 | 发送完成事件 |
| I2C_IRQ_EVT_I2C_BUSY | 2 | I2C总线忙事件 |
| I2C_IRQ_EVT_I2C_ERR | 3 | I2C传输错误事件 |

## Structures

### i2c_data_t <a id="struct_i2c_data"></a>

```c
typedef struct i2c_data {
    uint8_t *send_buf;              /*!< 发送数据的buffer指针。  */
    uint32_t send_len;              /*!< 发送数据的buffer长度。  */
    uint8_t *receive_buf;           /*!< 接收数据的buffer指针。  */
    uint32_t receive_len;           /*!< 接收数据的buffer长度。  */
} i2c_data_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| send_buf | uint8_t * | 发送数据的buffer指针 |
| send_len | uint32_t | 发送数据的buffer长度 |
| receive_buf | uint8_t * | 接收数据的buffer指针 |
| receive_len | uint32_t | 接收数据的buffer长度 |



