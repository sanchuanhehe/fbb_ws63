# Hash

hash 提供基于 SHA256 (Secure Hash Algorithm 256-bit) 算法的哈希计算能力，属于安全子系统（security）的统一接口层。支持两种使用模式：增量模式（创建通道、分多次输入数据、获取摘要并销毁通道）与一次性模式（单次调用完成完整哈希计算）。

**头文件清单**

```c
#include "include/driver/security_unified/security_sha256.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_drv_cipher_sha256_start](#uapi_drv_cipher_sha256_start) | 创建SHA256通道，获取句柄 |
| [uapi_drv_cipher_sha256_update](#uapi_drv_cipher_sha256_update) | 向SHA256通道输入数据进行计算 |
| [uapi_drv_cipher_sha256_finish](#uapi_drv_cipher_sha256_finish) | 获取SHA256摘要信息并销毁句柄 |
| [uapi_drv_cipher_sha256](#uapi_drv_cipher_sha256) | 一次性完成SHA256计算 |

## Functions

### uapi_drv_cipher_sha256_start <a id="uapi_drv_cipher_sha256_start"></a>

```c
errcode_t uapi_drv_cipher_sha256_start(uint32_t *hash_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/security_sha256.h"
```

**功能说明**

- 创建SHA256通道，返回通道句柄
- 调用成功后，可使用返回的句柄调用 `uapi_drv_cipher_sha256_update` 进行数据输入
- 需配合 `uapi_drv_cipher_sha256_finish` 完成计算并释放资源

**前置条件**

- 安全模块已初始化完成（`uapi_drv_cipher_env_init()` 已调用）
- 传入的 `hash_handle` 指针不为 NULL，且指向的内存空间已申请

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hash_handle | uint32_t * | 指向创建的SHA256通道句柄的指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| hash_handle | uint32_t * | 创建成功后输出SHA256通道句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SHA256通道创建成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_drv_cipher_sha256_update <a id="uapi_drv_cipher_sha256_update"></a>

```c
errcode_t uapi_drv_cipher_sha256_update(uint32_t hash_handle, const uint8_t *buf, uint32_t len)
```

**头文件清单**

```c
#include "include/driver/security_unified/security_sha256.h"
```

**功能说明**

- 向已创建的SHA256通道输入数据，进行SHA256计算
- 可单次调用，也可将数据拆分为多段多次调用，两种方式计算结果相同
- 已调用 `uapi_drv_cipher_sha256_finish` 获取摘要后，不能再调用此接口进行计算

**前置条件**

- 已通过 `uapi_drv_cipher_sha256_start` 创建SHA256句柄
- `buf` 指针不为 NULL，且指向的内存空间长度不小于 `len`

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hash_handle | uint32_t | 已创建的SHA256通道句柄 | 由 `uapi_drv_cipher_sha256_start` 输出的有效句柄 |
| buf | const uint8_t * | 源数据缓冲区指针 | 不为NULL |
| len | uint32_t | 缓冲区大小 | > 0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SHA256计算数据输入成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_drv_cipher_sha256_finish <a id="uapi_drv_cipher_sha256_finish"></a>

```c
errcode_t uapi_drv_cipher_sha256_finish(uint32_t hash_handle, uint8_t *out, uint32_t *out_len)
```

**头文件清单**

```c
#include "include/driver/security_unified/security_sha256.h"
```

**功能说明**

- 获取SHA256摘要信息，计算成功时销毁SHA256句柄
- 必须已创建SHA256句柄后才能调用
- 输入为缓冲区长度，输出为实际摘要长度

**前置条件**

- 已通过 `uapi_drv_cipher_sha256_start` 创建SHA256句柄
- `out` 指针不为 NULL，且指向的内存空间不小于 32 字节
- `out_len` 指针不为 NULL，输入值不小于 32

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hash_handle | uint32_t | 已创建的SHA256通道句柄 | 由 `uapi_drv_cipher_sha256_start` 输出的有效句柄 |
| out | uint8_t * | 存储摘要信息的缓冲区地址指针 | 不为NULL，缓冲区长度 ≥ 32 |
| out_len | uint32_t * | 存储摘要信息的缓冲区大小指针 | 不为NULL，输入值 ≥ 32 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| out | uint8_t * | 输出SHA256摘要数据，长度为32字节 |
| out_len | uint32_t * | 输出实际摘要长度 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SHA256摘要获取成功，句柄已销毁 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_drv_cipher_sha256 <a id="uapi_drv_cipher_sha256"></a>

```c
errcode_t uapi_drv_cipher_sha256(const uint8_t *buf, uint32_t len, uint8_t *out, uint32_t out_len)
```

**头文件清单**

```c
#include "include/driver/security_unified/security_sha256.h"
```

**功能说明**

- 一次性完成SHA256计算，内部自动完成通道创建、数据输入、摘要获取、句柄销毁
- 适用于单次计算完整数据摘要的场景，无需手动管理句柄
- 输出缓冲区长度必须为 32 字节（[SHA256_HASH_SIZE](#SHA256_HASH_SIZE)），否则返回 ERRCODE_INVALID_PARAM

**前置条件**

- 安全模块已初始化完成
- `buf` 指针不为 NULL，且指向的内存空间长度不小于 `len`
- `out` 指针不为 NULL，且指向的内存空间长度为 32 字节
- `out_len` 必须等于 32

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buf | const uint8_t * | 源数据缓冲区指针 | 不为NULL |
| len | uint32_t | 缓冲区大小 | > 0 |
| out | uint8_t * | 存储摘要信息的缓冲区地址指针 | 不为NULL，缓冲区长度 ≥ 32 |
| out_len | uint32_t | 存储摘要信息的缓冲区大小 | = 32 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| out | uint8_t * | 输出SHA256摘要数据，长度为32字节 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SHA256计算完成 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | out_len 不等于 32 |
| Other | 其他错误码，参考errcode_t | 其他执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持 SECURITY_UNIFIED 接口功能 | y |
| CONFIG_SECURITY_UNIFIED_SUPPORT_HASH | 功能宏 | 支持 SHA256 Hash 功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为所有 hash 模块接口的返回值类型使用

## Macros

### SHA256_HASH_SIZE <a id="SHA256_HASH_SIZE"></a>

```c
#define SHA256_HASH_SIZE    32
```

