# Security Init

Security Init 提供安全驱动模块的硬件初始化接口，完成安全子系统（Cipher、TRNG (True Random Number Generator)、KM (Key Management)、PKE (Public Key Engine) 等）的底层硬件资源初始化与反初始化。

**头文件清单**

```c
#include "include/driver/security_unified/security_init.h"
#include "include/driver/security_unified/mbedtls_harden_adapt.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_drv_cipher_env_init](#uapi_drv_cipher_env_init) | 安全驱动模块初始化 |
| [uapi_drv_cipher_env_deinit](#uapi_drv_cipher_env_deinit) | 安全驱动模块去初始化 |
| [uapi_drv_cipher_env_resume](#uapi_drv_cipher_env_resume) | 安全驱动模块深睡唤醒恢复初始化 |
| [uapi_drv_cipher_env_suspend](#uapi_drv_cipher_env_suspend) | 安全驱动模块深睡前去初始化 |
| [uapi_drv_cipher_wait_func_disable_all](#uapi_drv_cipher_wait_func_disable_all) | 安全驱动模块关闭中断模式 |
| [uapi_drv_cipher_wait_func_enable_all](#uapi_drv_cipher_wait_func_enable_all) | 安全驱动模块打开中断模式 |
| [mbedtls_adapt_register_func](#mbedtls_adapt_register_func) | 注册mbedtls硬件加速适配函数 |

## Functions

### uapi_drv_cipher_env_init <a id="uapi_drv_cipher_env_init"></a>

```c
void uapi_drv_cipher_env_init(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块初始化接口，由系统初始化流程调用
- 初始化安全驱动模块内部状态，为后续安全业务（加密、解密、哈希等）提供运行环境
- 不需要用户主动调用，由系统启动流程自动调用

**前置条件**

- 系统启动流程已进入外设驱动初始化阶段
- 安全驱动模块硬件时钟已使能

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |

### uapi_drv_cipher_env_deinit <a id="uapi_drv_cipher_env_deinit"></a>

```c
void uapi_drv_cipher_env_deinit(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块去初始化接口，由系统去初始化流程调用
- 释放安全驱动模块内部资源，关闭安全驱动运行环境
- 不需要用户主动调用

**前置条件**

- 安全驱动模块已通过 `uapi_drv_cipher_env_init()` 初始化完成
- 当前无进行中的安全业务操作

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |

### uapi_drv_cipher_env_resume <a id="uapi_drv_cipher_env_resume"></a>

```c
void uapi_drv_cipher_env_resume(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块深睡唤醒恢复初始化接口，由系统深睡唤醒流程调用
- 系统深睡唤醒后恢复安全驱动模块的运行环境，重新初始化安全硬件资源
- 不需要用户主动调用，由系统深睡唤醒流程自动调用

**前置条件**

- 系统已从深睡状态唤醒
- 安全驱动模块已通过 `uapi_drv_cipher_env_suspend()` 完成深睡前去初始化

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |
| CONFIG_SECURITY_UNIFIED_SUPPORT_DEEP_SLEEP | 特性宏 | 支持安全驱动深睡特性 | n |

### uapi_drv_cipher_env_suspend <a id="uapi_drv_cipher_env_suspend"></a>

```c
void uapi_drv_cipher_env_suspend(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块深睡前去初始化接口，由系统深睡流程调用
- 系统进入深睡前去初始化安全驱动模块，释放安全硬件资源以降低功耗
- 不需要用户主动调用，由系统深睡流程自动调用

**前置条件**

- 安全驱动模块已通过 `uapi_drv_cipher_env_init()` 初始化完成
- 当前无进行中的安全业务操作

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |
| CONFIG_SECURITY_UNIFIED_SUPPORT_DEEP_SLEEP | 特性宏 | 支持安全驱动深睡特性 | n |

### uapi_drv_cipher_wait_func_disable_all <a id="uapi_drv_cipher_wait_func_disable_all"></a>

```c
void uapi_drv_cipher_wait_func_disable_all(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块关闭中断模式，在NMI (Non-Maskable Interrupt)（不可屏蔽中断）中需关闭安全驱动的中断
- 关闭后安全驱动采用轮询模式等待操作完成
- 不需要用户主动调用，仅在NMI上下文中需调用

**前置条件**

- 安全驱动模块已通过 `uapi_drv_cipher_env_init()` 初始化完成

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |

### uapi_drv_cipher_wait_func_enable_all <a id="uapi_drv_cipher_wait_func_enable_all"></a>

```c
void uapi_drv_cipher_wait_func_enable_all(void)
```

**头文件清单** ``include/driver/security_unified/security_init.h``

**功能说明**

- 安全驱动模块打开中断模式，中断模式默认为打开状态
- 打开后安全驱动采用中断模式等待操作完成，提升CPU利用率
- 不需要用户主动调用，仅在NMI处理完毕后恢复中断模式时使用

**前置条件**

- 安全驱动模块已通过 `uapi_drv_cipher_env_init()` 初始化完成

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |

### mbedtls_adapt_register_func <a id="mbedtls_adapt_register_func"></a>

```c
int32_t mbedtls_adapt_register_func(void)
```

**头文件清单** ``include/driver/security_unified/mbedtls_harden_adapt.h``

**功能说明**

- 注册mbedtls硬件加速适配函数，将安全驱动模块的硬件加速函数注册到mbedtls框架
- 若开启第三方对接宏 `MBEDTLS_HARDEN_OPEN`，该接口由对接适配层调用
- 注册完成后，mbedtls的哈希、对称加解密、PKE等算法可使用硬件加速实现

**前置条件**

- 安全驱动模块已通过 `uapi_drv_cipher_env_init()` 初始化完成
- 已开启 `MBEDTLS_HARDEN_OPEN` 宏

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 注册成功 | 适配函数注册成功 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持安全驱动接口功能 | y |
| CONFIG_SECURITY_UNIFIED_SUPPORT_PKE | 特性宏 | 支持PKE (Public Key Engine) 功能特性 | y |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 特性宏 | 支持PKE大数运算特性 | y |
| CONFIG_PKE_SUPPORT_ECC_GEN_KEY | 特性宏 | 支持ECC (Elliptic Curve Cryptography) 密钥生成特性 | y |
| CONFIG_PKE_SUPPORT_ECC_SIGN | 特性宏 | 支持ECC签名特性 | y |
| CONFIG_PKE_SUPPORT_ECC_VERIFY | 特性宏 | 支持ECC验签特性 | y |
| CONFIG_PKE_SUPPORT_EDWARD | 特性宏 | 支持Edward曲线特性 | y |
| CONFIG_PKE_SUPPORT_ECC_ECDH | 特性宏 | 支持ECDH (Elliptic Curve Diffie-Hellman) 密钥协商特性 | y |
| CONFIG_PKE_SUPPORT_ECC_CAL | 特性宏 | 支持ECC大数计算特性 | y |
| CONFIG_PKE_SUPPORT_RSA | 特性宏 | 支持RSA (Rivest-Shamir-Adleman) 算法特性 | y |

