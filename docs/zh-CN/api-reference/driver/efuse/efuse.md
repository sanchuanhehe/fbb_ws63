# eFuse

eFuse (Electronic Fuse) 是芯片内一次性可编程（OTP (One-Time Programmable)）存储单元。每个 bit 初始值为 0，编程后不可逆地熔断为 1，用于永久存储设备身份标识（Die-ID/Chip-ID/SoC-ID）、出厂校准参数等不可更改的信息。

支持按 bit 或按字节读写，提供写保护标志位防止误操作。

**头文件清单**

```c
#include "include/driver/efuse.h"
#include "include/driver/efuse_user.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_efuse_init](#uapi_efuse_init) | 初始化eFuse模块 |
| [uapi_efuse_deinit](#uapi_efuse_deinit) | 去初始化eFuse模块 |
| [uapi_efuse_read_bit](#uapi_efuse_read_bit) | 从eFuse中读取指定位 |
| [uapi_efuse_read_buffer](#uapi_efuse_read_buffer) | 从eFuse中读取多个字节 |
| [uapi_efuse_write_bit](#uapi_efuse_write_bit) | 向eFuse写入一位 |
| [uapi_efuse_write_bit_with_flag](#uapi_efuse_write_bit_with_flag) | 在保护标志正确的情况下向eFuse写入一位 |
| [uapi_efuse_write_buffer](#uapi_efuse_write_buffer) | 从缓冲区向eFuse写入多个字节 |
| [uapi_efuse_write_buffer_with_flag](#uapi_efuse_write_buffer_with_flag) | 在保护标志正确的情况下从缓冲区向eFuse写入多个字节 |
| [uapi_efuse_get_die_id](#uapi_efuse_get_die_id) | 获取eFuse的Die-ID |
| [uapi_efuse_get_chip_id](#uapi_efuse_get_chip_id) | 获取eFuse的Chip-ID |
| [uapi_efuse_calc_crc](#uapi_efuse_calc_crc) | 计算eFuse零计数CRC (Cyclic Redundancy Check) |
| [uapi_soc_read_id](#uapi_soc_read_id) | 获取SoC-ID |
| [uapi_efuse_user_read_buffer](#uapi_efuse_user_read_buffer) | 从用户预留的eFuse空间中读取多个字节到缓冲区 |
| [uapi_efuse_user_write_buffer](#uapi_efuse_user_write_buffer) | 从缓冲区向用户预留的eFuse空间写入多个字节 |
| [uapi_efuse_user_write_bit](#uapi_efuse_user_write_bit) | 向用户预留eFuse空间中的对应bit写1 |
| [uapi_efuse_user_read_bit](#uapi_efuse_user_read_bit) | 从用户预留的eFuse空间中读取一位 |

## Functions

### uapi_efuse_init <a id="uapi_efuse_init"></a>

```c
errcode_t uapi_efuse_init(void)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 初始化eFuse模块，注册HAL (Hardware Abstraction Layer) 层函数并完成底层硬件初始化
- 调用前需确保eFuse模块未被初始化，不可重复调用
- 初始化完成后方可调用其他eFuse读写接口

**前置条件**

- eFuse模块未初始化，调用本接口前不可调用其他eFuse接口
- 硬件eFuse外设时钟已使能

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_deinit <a id="uapi_efuse_deinit"></a>

```c
errcode_t uapi_efuse_deinit(void)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 去初始化eFuse模块，注销HAL层函数并释放底层资源
- 调用后不可再调用其他eFuse读写接口，需重新初始化
- 去初始化始终返回成功

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去初始化成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_read_bit <a id="uapi_efuse_read_bit"></a>

```c
errcode_t uapi_efuse_read_bit(uint8_t *value, uint32_t byte_number, uint8_t bit_pos)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 从eFuse指定字节地址的指定位位置读取单个位值
- 读取操作在关中断上下文中执行，保证原子性
- 仅在EFUSE_BIT_OPERATION宏定义启用时可用

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- byte_number小于EFUSE_MAX_BYTES(256)，bit_pos小于EFUSE_MAX_BIT_POS(8)
- value指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| value | uint8_t * | 保存读取的位值（0或1） | 指针不为NULL |
| byte_number | uint32_t | 要读取位的源eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| bit_pos | uint8_t | 位的位置 | 0 ~ 7（EFUSE_MAX_BIT_POS - 1） |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| value | uint8_t * | 读取的位值，0或1 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |
| EFUSE_BIT_OPERATION | 特性宏 | 支持eFuse位操作接口功能 | 由构建目标决定 |

### uapi_efuse_read_buffer <a id="uapi_efuse_read_buffer"></a>

```c
errcode_t uapi_efuse_read_buffer(uint8_t *buffer, uint32_t byte_number, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 从eFuse指定字节地址起连续读取多个字节到缓冲区
- 逐字节读取，读取操作在关中断上下文中执行，保证原子性
- 读取范围不得超过eFuse最大字节地址空间

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- buffer指针不为NULL，length不为0
- byte_number + length不超过EFUSE_MAX_BYTES(256)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buffer | uint8_t * | 保存读取数据的缓冲区 | 指针不为NULL，长度不小于length |
| byte_number | uint32_t | 要读取数据的初始源eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| length | uint16_t | 数据的长度，以字节为单位 | 1 ~ 256，且byte_number + length ≤ 256 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer | uint8_t * | 读取到的eFuse数据 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_write_bit <a id="uapi_efuse_write_bit"></a>

```c
errcode_t uapi_efuse_write_bit(uint32_t byte_number, uint8_t bit_pos)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 向eFuse指定字节地址的指定位位置写入一位（仅可将位从0写为1，eFuse不可逆）
- 若目标位已为1，则返回ERRCODE_INVALID_PARAM
- 写入操作在关中断上下文中执行，保证原子性
- 仅在EFUSE_BIT_OPERATION宏定义启用时可用

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- byte_number小于EFUSE_MAX_BYTES(256)，bit_pos小于EFUSE_MAX_BIT_POS(8)
- 目标位当前值为0（eFuse只能从0写为1）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_number | uint32_t | 要写入位的目标eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| bit_pos | uint8_t | 要写入的位位置（0~7分别对应第0~7位），接口将所选位置对应的eFuse位由0写为1 | 0 ~ 7（EFUSE_MAX_BIT_POS - 1） |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |
| EFUSE_BIT_OPERATION | 特性宏 | 支持eFuse位操作接口功能 | 由构建目标决定 |

### uapi_efuse_write_bit_with_flag <a id="uapi_efuse_write_bit_with_flag"></a>

```c
errcode_t uapi_efuse_write_bit_with_flag(uint32_t byte_number, uint8_t bit_pos, uint32_t flag)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 在保护标志正确的情况下向eFuse写入一位，flag必须等于EFUSE_WRITE_PROTECT_FLAG(0x5A5A5A5A)
- 若flag不匹配，直接返回ERRCODE_EFUSE_INVALID_PARAM，防止误写
- 若目标位已为1，则返回ERRCODE_INVALID_PARAM
- 写入操作在关中断上下文中执行，保证原子性
- 仅在EFUSE_BIT_OPERATION宏定义启用时可用

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- byte_number小于EFUSE_MAX_BYTES(256)，bit_pos小于EFUSE_MAX_BIT_POS(8)
- 目标位当前值为0（eFuse只能从0写为1）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_number | uint32_t | 要写入位的目标eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| bit_pos | uint8_t | 要写入的位位置（0~7分别对应第0~7位），接口将所选位置对应的eFuse位由0写为1 | 0 ~ 7（EFUSE_MAX_BIT_POS - 1） |
| flag | uint32_t | 保护标志 | 必须为0x5A5A5A5A（EFUSE_WRITE_PROTECT_FLAG） |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |
| EFUSE_BIT_OPERATION | 特性宏 | 支持eFuse位操作接口功能 | 由构建目标决定 |

### uapi_efuse_write_buffer <a id="uapi_efuse_write_buffer"></a>

```c
errcode_t uapi_efuse_write_buffer(uint32_t byte_number, const uint8_t *buffer, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 从缓冲区向eFuse指定字节地址起连续写入多个字节
- eFuse为一次性可编程存储器，写入不可逆，仅可将位从0写为1
- 写入操作在关中断上下文中执行，保证原子性
- 写入范围不得超过eFuse最大字节地址空间

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- buffer指针不为NULL，length不为0
- byte_number + length不超过EFUSE_MAX_BYTES(256)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_number | uint32_t | 要写入数据的初始目的eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| buffer | const uint8_t * | 包含要写入的数据的缓冲区 | 指针不为NULL，长度不小于length |
| length | uint16_t | 数据的长度，以字节为单位 | 1 ~ 256，且byte_number + length ≤ 256 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_write_buffer_with_flag <a id="uapi_efuse_write_buffer_with_flag"></a>

```c
errcode_t uapi_efuse_write_buffer_with_flag(uint32_t byte_number, const uint8_t *buffer, uint16_t length, uint32_t flag)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 在保护标志正确的情况下从缓冲区向eFuse写入多个字节，flag必须等于EFUSE_WRITE_PROTECT_FLAG(0x5A5A5A5A)
- 若flag不匹配，直接返回ERRCODE_EFUSE_INVALID_PARAM，防止误写
- eFuse为一次性可编程存储器，写入不可逆
- 写入操作在关中断上下文中执行，保证原子性

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- buffer指针不为NULL，length不为0
- byte_number + length不超过EFUSE_MAX_BYTES(256)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_number | uint32_t | 要写入数据的初始目的eFuse字节地址 | 0 ~ 255（EFUSE_MAX_BYTES - 1） |
| buffer | const uint8_t * | 包含要写入的数据的缓冲区 | 指针不为NULL，长度不小于length |
| length | uint16_t | 数据的长度，以字节为单位 | 1 ~ 256，且byte_number + length ≤ 256 |
| flag | uint32_t | 保护标志 | 必须为0x5A5A5A5A（EFUSE_WRITE_PROTECT_FLAG） |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_get_die_id <a id="uapi_efuse_get_die_id"></a>

```c
errcode_t uapi_efuse_get_die_id(uint8_t *buffer, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 获取eFuse的Die-ID，Die-ID为芯片晶粒唯一标识
- 通过HAL层读取Die-ID对应的eFuse区域数据
- Die-ID在eFuse中的起始位地址为8，长度为160位（20字节）

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- buffer指针不为NULL，length不为0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buffer | uint8_t * | 保存读取数据的缓冲区 | 指针不为NULL，长度不小于length |
| length | uint16_t | 缓冲区容量，以字节为单位（实际写入固定20字节Die-ID，Die-ID占160位即20字节） | ≥ 20 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer | uint8_t * | 读取到的Die-ID数据 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_get_chip_id <a id="uapi_efuse_get_chip_id"></a>

```c
errcode_t uapi_efuse_get_chip_id(uint8_t *buffer, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 获取eFuse的Chip-ID，Chip-ID为芯片版本唯一标识
- 通过HAL层读取Chip-ID对应的eFuse区域数据
- Chip-ID在eFuse中的起始位地址为0，长度为8位（1字节）

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- buffer指针不为NULL，length不为0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buffer | uint8_t * | 保存读取数据的缓冲区 | 指针不为NULL，长度不小于length |
| length | uint16_t | Chip-ID数据的长度，以字节为单位 | ≥ 1（Chip-ID占8位即1字节） |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer | uint8_t * | 读取到的Chip-ID数据 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_calc_crc <a id="uapi_efuse_calc_crc"></a>

```c
errcode_t uapi_efuse_calc_crc(const uint8_t *buffer, uint8_t length, uint8_t *crc)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 计算eFuse零计数CRC，统计缓冲区数据中每字节0位的个数之和
- 最大支持32字节（256位）数据的CRC计算
- CRC值为所有字节中0位个数的累加和

**前置条件**

- buffer指针不为NULL，crc指针不为NULL
- length不超过32（EFUSE_CALC_CRC_MAX_LEN）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buffer | const uint8_t * | 保存读取数据的缓冲区 | 指针不为NULL |
| length | uint8_t | 数据长度，以字节为单位 | 1 ~ 32（EFUSE_CALC_CRC_MAX_LEN） |
| crc | uint8_t * | 保存CRC值的缓冲区 | 指针不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| crc | uint8_t * | 计算得到的零计数CRC值 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 计算成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_soc_read_id <a id="uapi_soc_read_id"></a>

```c
errcode_t uapi_soc_read_id(uint8_t *id, uint16_t id_length)
```

**头文件清单**

```c
#include "include/driver/efuse.h"
```

**功能说明**

- 获取SoC-ID，内部调用uapi_efuse_get_die_id实现
- SoC-ID为固定20字节数组，本接口固定写入20字节SoC-ID数据到id缓冲区
- SoC-ID与Die-ID内容相同，提供兼容性接口

**前置条件**

- eFuse模块已通过uapi_efuse_init()初始化完成
- id指针不为NULL，id_length不小于20字节（缓冲区容量须≥20字节以容纳完整的SoC-ID）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| id | uint8_t * | 用于存储返回的SoC-ID，SoC-ID为固定20字节数组 | 指针不为NULL，缓冲区容量≥ 20字节 |
| id_length | uint16_t | 缓冲区容量，以字节为单位（实际写入固定20字节SoC-ID） | ≥ 20 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| id | uint8_t * | 读取到的SoC-ID数据，长度为20字节 |

**返回值**

- 返回类型：[errcode_t](#typedef_errcode_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_user_read_buffer <a id="uapi_efuse_user_read_buffer"></a>

```c
errcode_t uapi_efuse_user_read_buffer(uint32_t offset, uint8_t *buffer, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse_user.h"
```

**功能说明**

- 从用户预留的eFuse空间中读取多个字节，进入提供的缓冲区
- 读取范围为用户预留区域，偏移地址相对于用户预留区域起始位置计算
- 调用前需确保eFuse模块已初始化，且offset与length不超过用户预留空间大小

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_efuse_init() 初始化完成，返回初始化成功状态（返回值为ERRCODE_SUCC）
- 依赖资源状态：offset与length之和不超过用户预留eFuse空间大小（CUSTOMER_RSVD_EFUSE_BIT_LEN / 8 = 16字节）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | uint32_t | 待读取的eFuse空间在用户预留区域中的起始偏移地址，以字节为单位 | [0, 15] |
| buffer | uint8_t * | 保存读取数据的缓冲区 | 不为NULL，且指向内存空间已申请成功，长度不小于length规定值 |
| length | uint16_t | 数据的长度，以字节为单位 | [1, 16]，且 offset + length ≤ 16 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer | uint8_t * | 读取到的eFuse用户预留空间数据，长度为length字节 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_user_write_buffer <a id="uapi_efuse_user_write_buffer"></a>

```c
errcode_t uapi_efuse_user_write_buffer(uint32_t offset, const uint8_t *buffer, uint16_t length)
```

**头文件清单**

```c
#include "include/driver/efuse_user.h"
```

**功能说明**

- 从提供的缓冲区向用户预留的eFuse空间写入多个字节
- 写入范围为用户预留区域，偏移地址相对于用户预留区域起始位置计算
- eFuse为一次性可编程存储器，已写入的位无法从1改回0，重复写入仅可将0位写为1

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_efuse_init() 初始化完成，返回初始化成功状态（返回值为ERRCODE_SUCC）
- 依赖资源状态：offset与length之和不超过用户预留eFuse空间大小（CUSTOMER_RSVD_EFUSE_BIT_LEN / 8 = 16字节）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | uint32_t | 待写入的eFuse空间在用户预留区域中的起始偏移地址，以字节为单位 | [0, 15] |
| buffer | const uint8_t * | 包含要写入的数据的缓冲区 | 不为NULL，且指向内存空间已申请成功，长度不小于length规定值 |
| length | uint16_t | 数据的长度，以字节为单位 | [1, 16]，且 offset + length ≤ 16 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |

### uapi_efuse_user_write_bit <a id="uapi_efuse_user_write_bit"></a>

```c
errcode_t uapi_efuse_user_write_bit(uint32_t byte_offset, uint8_t bit_pos)
```

**头文件清单**

```c
#include "include/driver/efuse_user.h"
```

**功能说明**

- 向用户预留eFuse空间中的对应bit写1
- 写入位置由字节偏移地址和位位置共同确定
- eFuse为一次性可编程存储器，已写入的位无法从1改回0

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_efuse_init() 初始化完成，返回初始化成功状态（返回值为ERRCODE_SUCC）
- 依赖资源状态：byte_offset与bit_pos对应的位地址不超过用户预留eFuse空间大小（CUSTOMER_RSVD_EFUSE_BIT_LEN = 128位）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_offset | uint32_t | 待写入位在用户预留空间中的字节偏移地址 | [0, 15]，且 byte_offset × 8 + bit_pos ≤ 128 |
| bit_pos | uint8_t | 待写入位在用户预留空间中对应字节中的bit位置 | [0, 7] |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |
| EFUSE_BIT_OPERATION | 特性宏 | 支持eFuse位操作接口功能 | 由构建目标决定 |

### uapi_efuse_user_read_bit <a id="uapi_efuse_user_read_bit"></a>

```c
errcode_t uapi_efuse_user_read_bit(uint32_t byte_offset, uint8_t bit_pos, uint8_t *value)
```

**头文件清单**

```c
#include "include/driver/efuse_user.h"
```

**功能说明**

- 从用户预留的eFuse空间中读取一位
- 读取位置由字节偏移地址和位位置共同确定
- 读取的位值通过输出参数value返回

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_efuse_init() 初始化完成，返回初始化成功状态（返回值为ERRCODE_SUCC）
- 依赖资源状态：byte_offset与bit_pos对应的位地址不超过用户预留eFuse空间大小（CUSTOMER_RSVD_EFUSE_BIT_LEN = 128位）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| byte_offset | uint32_t | 待读取位在用户预留空间中的字节偏移地址 | [0, 15]，且 byte_offset × 8 + bit_pos ≤ 128 |
| bit_pos | uint8_t | 待读取位在用户预留空间中对应字节中的bit位置 | [0, 7] |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| value | uint8_t * | 读取的位值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_EFUSE | 特性宏 | 支持eFuse接口功能 | n |
| EFUSE_BIT_OPERATION | 特性宏 | 支持eFuse位操作接口功能 | 由构建目标决定 |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

所有eFuse接口的返回值类型，表示接口执行结果


