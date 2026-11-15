# TRNG

TRNG (True Random Number Generator) 提供真随机数生成接口，基于物理噪声源产生高质量随机数，用于密钥生成、Nonce/挑战值构造等安全场景。

**头文件清单**

```c
#include "include/driver/security_unified/trng.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_drv_cipher_trng_get_random](#uapi_drv_cipher_trng_get_random) | 获取硬件随机数（uint32_t类型） |
| [uapi_drv_cipher_trng_get_random_bytes](#uapi_drv_cipher_trng_get_random_bytes) | 获取指定大小的硬件随机数 |

## Functions

### uapi_drv_cipher_trng_get_random <a id="uapi_drv_cipher_trng_get_random"></a>

```c
errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum)
```

**头文件清单**

```c
#include "include/driver/security_unified/trng.h"
```

**功能说明**

- 获取硬件随机数，生成uint32_t类型大小的随机数
- 生成其他大小的随机数，需要循环调用该接口
- 接口调用期间会添加睡眠否决，防止系统在TRNG操作期间进入睡眠状态

**前置条件**

- 安全模块已初始化完成（uapi_drv_cipher_env_init()已调用且返回成功）
- 入参randnum不为NULL，且指向的内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| randnum | uint32_t * | 指向存储生成的随机数的缓冲区的指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 硬件随机数生成成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_TRNG | 功能宏 | 支持 TRNG 接口功能 | n |
| CONFIG_TRNG_RING_ENABLE | 特性宏 | 支持 TRNG Ring 振荡器特性 | n |
| CONFIG_ENTROPY_SOURCES_FROM_FRO | 特性宏 | 支持 FRO 熵源特性 | n |

### uapi_drv_cipher_trng_get_random_bytes <a id="uapi_drv_cipher_trng_get_random_bytes"></a>

```c
errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size)
```

**头文件清单**

```c
#include "include/driver/security_unified/trng.h"
```

**功能说明**

- 获取指定大小的硬件随机数
- 可一次性生成指定字节数的随机数，无需循环调用
- 接口调用期间会添加睡眠否决，防止系统在TRNG操作期间进入睡眠状态

**前置条件**

- 安全模块已初始化完成（uapi_drv_cipher_env_init()已调用且返回成功）
- 入参randnum不为NULL，且指向的内存空间已申请成功，长度不小于size指定的值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| randnum | uint8_t * | 指向存储生成的随机数的缓冲区的指针 | 非NULL |
| size | uint32_t | 生成随机数的大小（字节数） | > 0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 硬件随机数生成成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_TRNG | 功能宏 | 支持 TRNG 接口功能 | n |
| CONFIG_TRNG_RING_ENABLE | 特性宏 | 支持 TRNG Ring 振荡器特性 | n |
| CONFIG_ENTROPY_SOURCES_FROM_FRO | 特性宏 | 支持 FRO 熵源特性 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为uapi_drv_cipher_trng_get_random和uapi_drv_cipher_trng_get_random_bytes接口的返回值类型

