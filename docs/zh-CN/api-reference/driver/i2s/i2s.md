# I2S

I2S (Inter-IC Sound) 是数字音频传输接口，三线制：BCLK (Bit Clock)（位时钟）、LRCK (Left/Right Clock)（左右通道选择）、SDATA (Serial Data)（音频数据）。WS63 I2S 支持 Master 模式，提供轮询、中断和 DMA (Direct Memory Access) 三种传输方式。

DMA 模式下支持 LLI (Linked List Item) 链式传输，实现无间断音频数据推送，适合音频播放、语音采集等场景。支持环回自测模式用于调试。

**头文件清单**

```c
#include "include/driver/i2s.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_i2s_init](#uapi_i2s_init) | 初始化I2S设备 |
| [uapi_i2s_deinit](#uapi_i2s_deinit) | 去初始化I2S设备 |
| [uapi_i2s_set_config](#uapi_i2s_set_config) | 设置I2S设备配置信息 |
| [uapi_i2s_set_sample_rate](#uapi_i2s_set_sample_rate) | 设置I2S设备采样率 |
| [uapi_i2s_get_config](#uapi_i2s_get_config) | 获取I2S设备配置信息 |
| [uapi_i2s_write_data](#uapi_i2s_write_data) | 轮询模式下写入数据 |
| [uapi_i2s_read_start](#uapi_i2s_read_start) | 中断模式下启动读取数据 |
| [uapi_i2s_set_crg_clock_enable](#uapi_i2s_set_crg_clock_enable) | I2S时钟使能控制 |
| [uapi_i2s_loop_trans](#uapi_i2s_loop_trans) | I2S回路自测传输 |
| [uapi_i2s_get_data](#uapi_i2s_get_data) | 获取中断模式下I2S设备接收的数据 |
| [uapi_i2s_loopback](#uapi_i2s_loopback) | 开启或关闭回环模式 |
| [uapi_i2s_dma_config](#uapi_i2s_dma_config) | 配置I2S DMA传输参数 |
| [uapi_i2s_merge_write_by_dma](#uapi_i2s_merge_write_by_dma) | merge模式下通过DMA写数据 |
| [uapi_i2s_merge_read_by_dma](#uapi_i2s_merge_read_by_dma) | merge模式下通过DMA读数据 |

## Functions

### uapi_i2s_init <a id="uapi_i2s_init"></a>

```c
errcode_t uapi_i2s_init(sio_bus_t bus, i2s_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

初始化I2S设备，注册回调函数并使能时钟。

**前置条件**

- 该接口须在所有其他I2S接口之前调用
- 同一总线不可重复初始化，重复调用直接返回成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO (Serial I/O) 接口，参考 [sio_bus_t](#sio_bus_t) |
| callback | i2s_callback_t | I2S设备的回调函数，参考 [i2s_callback_t](#i2s_callback_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_DMA | 特性宏 | 使能DMA传输支持，初始化时额外初始化DMA信号量 | - |

### uapi_i2s_deinit <a id="uapi_i2s_deinit"></a>

```c
errcode_t uapi_i2s_deinit(sio_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

去初始化I2S设备，关闭接收、注销回调、关闭时钟。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)


### uapi_i2s_set_config <a id="uapi_i2s_set_config"></a>

```c
errcode_t uapi_i2s_set_config(sio_bus_t bus, const i2s_config_t *config)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

设置I2S设备的配置信息，包括工作模式、传输模式、数据宽度、通道数、时序模式、时钟边沿和分频系数。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| config | const i2s_config_t * | I2S设备的配置信息，参考 [i2s_config_t](#i2s_config_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效或config为NULL |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)


### uapi_i2s_set_sample_rate <a id="uapi_i2s_set_sample_rate"></a>

```c
errcode_t uapi_i2s_set_sample_rate(sio_bus_t bus, i2s_sample_rate_t sample_rate)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

设置I2S设备的采样率。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 采样率须为标准采样率，非标准采样率返回参数无效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| sample_rate | i2s_sample_rate_t | I2S设备的采样率，参考 [i2s_sample_rate_t](#i2s_sample_rate_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效或sample_rate非标准采样率 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_DYNAMIC_SAMPLE_RATE | 特性宏 | 使能动态采样率设置，控制本接口是否存在 | - |

### uapi_i2s_get_config <a id="uapi_i2s_get_config"></a>

```c
errcode_t uapi_i2s_get_config(sio_bus_t bus, i2s_config_t *config)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

获取I2S设备配置信息。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| config | i2s_config_t * | I2S设备的配置信息，参考 [i2s_config_t](#i2s_config_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效或config为NULL |
| Other | 失败，参考 [errcode_t](#errcode_t) |

### uapi_i2s_write_data <a id="uapi_i2s_write_data"></a>

```c
errcode_t uapi_i2s_write_data(sio_bus_t bus, i2s_tx_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

轮询模式下写入数据到I2S设备。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_set_config](#uapi_i2s_set_config) 完成配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| data | i2s_tx_data_t * | 数据传输指针，参考 [i2s_tx_data_t](#i2s_tx_data_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效、data为NULL或data成员指针为NULL |
| Other | 失败，参考 [errcode_t](#errcode_t) |

### uapi_i2s_read_start <a id="uapi_i2s_read_start"></a>

```c
errcode_t uapi_i2s_read_start(sio_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

中断模式下启动读取数据。若当前配置为主模式，会自动使能时钟。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_set_config](#uapi_i2s_set_config) 完成配置
- 已注册回调函数用于接收数据

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

### uapi_i2s_set_crg_clock_enable <a id="uapi_i2s_set_crg_clock_enable"></a>

```c
void uapi_i2s_set_crg_clock_enable(sio_bus_t bus, bool enable)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

I2S时钟使能控制，用于打开或关闭位时钟(BCLK)和采样时钟(WS)。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| enable | bool | true：使能时钟；false：关闭时钟 |

### uapi_i2s_loop_trans <a id="uapi_i2s_loop_trans"></a>

```c
errcode_t uapi_i2s_loop_trans(sio_bus_t bus, i2s_tx_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

I2S回路自测传输，将数据通过内部回路发送并接收。

> **注意**：本接口需启用 `CONFIG_I2S_SUPPORT_LOOPBACK` 配置，否则HAL函数指针未注册，调用将导致空指针异常。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_set_config](#uapi_i2s_set_config) 完成配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| data | i2s_tx_data_t * | 数据传输指针，参考 [i2s_tx_data_t](#i2s_tx_data_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效、data为NULL或data成员指针为NULL |
| Other | 失败，参考 [errcode_t](#errcode_t) |

### uapi_i2s_get_data <a id="uapi_i2s_get_data"></a>

```c
errcode_t uapi_i2s_get_data(sio_bus_t bus, i2s_rx_data_t *data)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

获取中断模式下I2S设备接收的数据。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_read_start](#uapi_i2s_read_start) 启动读取

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| data | i2s_rx_data_t * | 数据接收指针，参考 [i2s_rx_data_t](#i2s_rx_data_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效或data为NULL |
| Other | 失败，参考 [errcode_t](#errcode_t) |

### uapi_i2s_loopback <a id="uapi_i2s_loopback"></a>

```c
errcode_t uapi_i2s_loopback(sio_bus_t bus, bool en)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

开启或关闭I2S回环模式，用于设备自测。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| en | bool | true：开启回环模式；false：关闭回环模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_LOOPBACK | 特性宏 | 使能回环模式支持，控制本接口是否存在，默认值为1 | - |

### uapi_i2s_dma_config <a id="uapi_i2s_dma_config"></a>

```c
int32_t uapi_i2s_dma_config(sio_bus_t bus, i2s_dma_attr_t *i2s_dma_cfg)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

配置I2S DMA传输参数，设置DMA使能及中断水线。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| i2s_dma_cfg | i2s_dma_attr_t * | I2S DMA传输时I2S的配置参数，参考 [i2s_dma_attr_t](#i2s_dma_attr_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_DMA | 特性宏 | 使能I2S DMA传输支持，控制本接口及相关类型定义是否存在 | - |

### uapi_i2s_merge_write_by_dma <a id="uapi_i2s_merge_write_by_dma"></a>

```c
int32_t uapi_i2s_merge_write_by_dma(sio_bus_t bus, const void *buffer, uint32_t length, i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

merge模式下通过DMA写数据到I2S设备。支持阻塞和非阻塞传输方式。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_dma_config](#uapi_i2s_dma_config) 配置DMA参数
- 已调用 [uapi_i2s_set_config](#uapi_i2s_set_config) 完成配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| buffer | const void * | 写数据缓存区 |
| length | uint32_t | 需要传输的数据长度 |
| dma_cfg | i2s_dma_config_t * | I2S DMA传输时DMA的配置参数，参考 [i2s_dma_config_t](#i2s_dma_config_t) |
| arg | uintptr_t | 自定义参数指针，可被传递到中断处理函数 |
| block | bool | true：阻塞传输；false：非阻塞传输 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功（非阻塞模式或阻塞传输完成） |
| >0 | 阻塞模式下实际传输的数据块大小 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效、dma_cfg为NULL、buffer为NULL或length为0 |
| ERRCODE_NOT_SUPPORT (0x80000002) | DMA握手信号不支持 |
| ERRCODE_FAIL (0xFFFFFFFF) | DMA配置或启动失败，或阻塞等待信号量失败，或传输未成功 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_DMA | 特性宏 | 使能I2S DMA传输支持，控制本接口是否存在 | - |

### uapi_i2s_merge_read_by_dma <a id="uapi_i2s_merge_read_by_dma"></a>

```c
int32_t uapi_i2s_merge_read_by_dma(sio_bus_t bus, const void *buffer, uint32_t length, i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block)
```

**头文件清单**

```c
#include "include/driver/i2s.h"
```

**功能说明**

merge模式下通过DMA从I2S设备读数据。支持阻塞和非阻塞传输方式。

**前置条件**

- 已调用 [uapi_i2s_init](#uapi_i2s_init) 完成初始化
- 已调用 [uapi_i2s_dma_config](#uapi_i2s_dma_config) 配置DMA参数
- 已调用 [uapi_i2s_set_config](#uapi_i2s_set_config) 完成配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | sio_bus_t | 指定的SIO接口，参考 [sio_bus_t](#sio_bus_t) |
| buffer | const void * | 读数据缓存区 |
| length | uint32_t | 需要读的数据长度 |
| dma_cfg | i2s_dma_config_t * | I2S DMA传输时DMA的配置参数，参考 [i2s_dma_config_t](#i2s_dma_config_t) |
| arg | uintptr_t | 自定义参数指针，可被传递到中断处理函数 |
| block | bool | true：阻塞传输；false：非阻塞传输 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功（非阻塞模式或阻塞传输完成） |
| >0 | 阻塞模式下实际传输的数据块大小 |
| ERRCODE_I2S_NOT_INIT (0x80001410) | I2S未初始化 |
| ERRCODE_INVALID_PARAM (0x80000001) | bus参数无效、dma_cfg为NULL、buffer为NULL或length为0 |
| ERRCODE_NOT_SUPPORT (0x80000002) | DMA握手信号不支持 |
| ERRCODE_FAIL (0xFFFFFFFF) | DMA配置或启动失败，或阻塞等待信号量失败，或传输未成功 |
| Other | 失败，参考 [errcode_t](#errcode_t) |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_I2S_SUPPORT_DMA | 特性宏 | 使能I2S DMA传输支持，控制本接口是否存在 | - |

## Type definitions

### i2s_callback_t <a id="i2s_callback_t"></a>

I2S设备回调函数类型定义。

```c
typedef void (*i2s_callback_t)(uint32_t *left_buff, uint32_t *right_buff, uint32_t length);
```

| 参数名 | 方向 | 类型 | 描述 |
| ---- | ---- | ---- | ---- |
| left_buff | [in] | uint32_t * | 左声道接收的数据 |
| right_buff | [in] | uint32_t * | 右声道接收的数据 |
| length | [in] | uint32_t | 数据的长度 |

### errcode_t <a id="errcode_t"></a>

I2S接口统一返回值类型，定义于 `errcode.h`。

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为本模块所有接口的返回值类型使用。

## Enumerations

### sio_bus_t <a id="sio_bus_t"></a>

SIO总线编号枚举，定义于 `platform_core.h`。

```c
typedef enum {
    SIO_BUS_0,
    SIO_NONE = I2S_MAX_NUMBER
} sio_bus_t;
```

| 枚举值 | 值 | 描述 |
| ------ | -- | ---- |
| SIO_BUS_0 | 0 | SIO总线0 |
| SIO_NONE | 1 | 无效SIO总线（等于I2S_MAX_NUMBER） |

关联常量：

| 常量名 | 值 | 描述 |
| ------ | -- | ---- |
| I2S_MAX_NUMBER | 1 | I2S总线最大数量 |
| CONFIG_I2S_BUS_MAX_NUM | 1 | I2S总线配置最大数量 |

### i2s_sample_rate_t <a id="i2s_sample_rate_t"></a>

```c
typedef enum {
    I2S_SAMPLE_RATE_8K = 0,
    I2S_SAMPLE_RATE_11K = 1,
    I2S_SAMPLE_RATE_12K = 2,
    I2S_SAMPLE_RATE_16K = 3,
    I2S_SAMPLE_RATE_22K = 4,
    I2S_SAMPLE_RATE_24K = 5,
    I2S_SAMPLE_RATE_32K = 6,
    I2S_SAMPLE_RATE_44K = 7,
    I2S_SAMPLE_RATE_48K = 8,
    I2S_SAMPLE_RATE_88K = 9,
    I2S_SAMPLE_RATE_96K = 10,
    I2S_SAMPLE_RATE_176K = 11,
    I2S_SAMPLE_RATE_192K = 12,
    I2S_SAMPLE_RATE_MAX = 13,
} i2s_sample_rate_t;
```

I2S采样率枚举，定义于 `sio_porting.h`。

| 枚举值 | 值 | 描述 |
| ------ | -- | ---- |
| I2S_SAMPLE_RATE_8K | 0 | 8K采样率 |
| I2S_SAMPLE_RATE_11K | 1 | 11K采样率 |
| I2S_SAMPLE_RATE_12K | 2 | 12K采样率 |
| I2S_SAMPLE_RATE_16K | 3 | 16K采样率 |
| I2S_SAMPLE_RATE_22K | 4 | 22K采样率 |
| I2S_SAMPLE_RATE_24K | 5 | 24K采样率 |
| I2S_SAMPLE_RATE_32K | 6 | 32K采样率 |
| I2S_SAMPLE_RATE_44K | 7 | 44K采样率 |
| I2S_SAMPLE_RATE_48K | 8 | 48K采样率 |
| I2S_SAMPLE_RATE_88K | 9 | 88K采样率 |
| I2S_SAMPLE_RATE_96K | 10 | 96K采样率 |
| I2S_SAMPLE_RATE_176K | 11 | 176K采样率 |
| I2S_SAMPLE_RATE_192K | 12 | 192K采样率 |
| I2S_SAMPLE_RATE_MAX | 13 | 采样率上限值 |

## Structures

### i2s_config_t <a id="i2s_config_t"></a>

```c
typedef struct i2s_config {
    uint8_t drive_mode;  /*!< I2S设备模式：0-从模式(SLAVE)，1-主模式(MASTER) */
    uint8_t transfer_mode;  /*!< I2S传输路径模式：0-标准模式(Standard)，1-多路模式(Multichannel) */
    uint8_t data_width;  /*!< I2S数据宽度：0-保留，1-16位，2-18位，3-20位，4-24位，5-32位 */
    uint8_t channels_num;  /*!< I2S传输通道数：0-2通道，1-4通道，2-8通道，3-16通道 */
    uint8_t timing;  /*!< I2S时序模式：0-标准时序模式，1-自定义时序模式 */
    uint8_t clk_edge;  /*!< I2S时钟边沿模式：0-下降沿，1-上升沿 */
    uint8_t div_number;  /*!< 分频系数，参考data_width */
    uint8_t number_of_channels;  /*!< 通道数，参考channels_num */
} i2s_config_t;
```

I2S配置结构体。

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| drive_mode | uint8_t | I2S设备模式：0-从模式(SLAVE)，1-主模式(MASTER) |
| transfer_mode | uint8_t | I2S传输路径模式：0-标准模式(Standard)，1-多路模式(Multichannel) |
| data_width | uint8_t | I2S数据宽度：0-保留，1-16位，2-18位，3-20位，4-24位，5-32位 |
| channels_num | uint8_t | I2S传输通道数：0-2通道，1-4通道，2-8通道，3-16通道 |
| timing | uint8_t | I2S时序模式：0-标准时序模式，1-自定义时序模式 |
| clk_edge | uint8_t | I2S时钟边沿模式：0-下降沿，1-上升沿 |
| div_number | uint8_t | 分频系数，参考data_width |
| number_of_channels | uint8_t | 通道数，参考channels_num |

### i2s_tx_data_t <a id="i2s_tx_data_t"></a>

```c
typedef struct i2s_tx_data {
    uint32_t *left_buff;  /*!< 通过TX左FIFO发送的数据 */
    uint32_t *right_buff;  /*!< 通过TX右FIFO发送的数据 */
    uint32_t length;  /*!< 发送数据的个数 */
} i2s_tx_data_t;
```

I2S发送数据结构体。

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| left_buff | uint32_t * | 通过TX (Transmit) 左FIFO (First-In First-Out) 发送的数据 |
| right_buff | uint32_t * | 通过TX右FIFO发送的数据 |
| length | uint32_t | 发送数据的个数 |

### i2s_rx_data_t <a id="i2s_rx_data_t"></a>

```c
typedef struct i2s_rx_data {
    uint32_t left_buff[CONFIG_DATA_LEN_MAX];  /*!< 左声道数据，CONFIG_DATA_LEN_MAX=128 */
    uint32_t right_buff[CONFIG_DATA_LEN_MAX];  /*!< 右声道数据，CONFIG_DATA_LEN_MAX=128 */
    uint32_t length;  /*!< 数据长度 */
} i2s_rx_data_t;
```

I2S接收数据结构体。

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| left_buff | uint32_t[CONFIG_DATA_LEN_MAX] | 左声道数据，CONFIG_DATA_LEN_MAX=128 |
| right_buff | uint32_t[CONFIG_DATA_LEN_MAX] | 右声道数据，CONFIG_DATA_LEN_MAX=128 |
| length | uint32_t | 数据长度 |

### i2s_dma_config_t <a id="i2s_dma_config_t"></a>

```c
typedef struct i2s_dma_config {
    uint8_t src_width;  /*!< 源端传输数据宽度：0-1字节，1-2字节，2-4字节 */
    uint8_t dest_width;  /*!< 目的端传输数据宽度：0-1字节，1-2字节，2-4字节 */
    uint8_t burst_length;  /*!< 每次burst请求写入目的端数据量：0-1，1-4，2-8，3-16 */
    uint8_t priority;  /*!< 传输通道优先级（最小为0，最大为3） */
} i2s_dma_config_t;
```

I2S DMA配置结构体，受 `CONFIG_I2S_SUPPORT_DMA` 宏控制。

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| src_width | uint8_t | 源端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| dest_width | uint8_t | 目的端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| burst_length | uint8_t | 每次burst请求写入目的端数据量：0-1，1-4，2-8，3-16 |
| priority | uint8_t | 传输通道优先级（最小为0，最大为3） |

### i2s_dma_attr_t <a id="i2s_dma_attr_t"></a>

```c
typedef struct i2s_dma_attr {
    bool tx_dma_enable;  /*!< TX DMA使能：false-不使用DMA，true-使用DMA */
    uint8_t tx_int_threshold;  /*!< 触发中断的TX FIFO水线 */
    bool rx_dma_enable;  /*!< RX DMA使能：false-不使用DMA，true-使用DMA */
    uint8_t rx_int_threshold;  /*!< 触发中断的RX FIFO水线 */
} i2s_dma_attr_t;
```

I2S DMA属性结构体，受 `CONFIG_I2S_SUPPORT_DMA` 宏控制。

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- |--------------------------------------------|
| tx_dma_enable | bool | TX DMA使能：false-不使用DMA，true-使用DMA |
| tx_int_threshold | uint8_t | 触发中断的TX FIFO水线 |
| rx_dma_enable | bool | RX (Receive) DMA使能：false-不使用DMA，true-使用DMA |
| rx_int_threshold | uint8_t | 触发中断的RX FIFO水线 |


