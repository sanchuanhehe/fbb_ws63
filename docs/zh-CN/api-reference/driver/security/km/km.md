# KM

KM (Key Management) 提供密钥管理接口，支持对称密钥与非对称密钥的生成、写入、读取、删除及派生等操作，为加解密运算提供统一的密钥生命周期管理能力。

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_drv_km_init](#uapi_drv_km_init) | KM模块初始化 |
| [uapi_drv_km_deinit](#uapi_drv_km_deinit) | KM模块去初始化 |
| [uapi_drv_keyslot_create](#uapi_drv_keyslot_create) | 创建keyslot句柄 |
| [uapi_drv_keyslot_destroy](#uapi_drv_keyslot_destroy) | 销毁keyslot句柄 |
| [uapi_drv_klad_create](#uapi_drv_klad_create) | 创建keyladder通道句柄 |
| [uapi_drv_klad_destroy](#uapi_drv_klad_destroy) | 销毁keyladder通道句柄 |
| [uapi_drv_klad_attach](#uapi_drv_klad_attach) | 将keyslot句柄与klad句柄关联 |
| [uapi_drv_klad_detach](#uapi_drv_klad_detach) | 将keyslot句柄与klad句柄解关联 |
| [uapi_drv_klad_set_attr](#uapi_drv_klad_set_attr) | 设置keyladder属性 |
| [uapi_drv_klad_get_attr](#uapi_drv_klad_get_attr) | 获取keyladder属性 |
| [uapi_drv_klad_set_effective_key](#uapi_drv_klad_set_effective_key) | 设置effective key（硬件派生密钥） |
| [uapi_drv_klad_set_clear_key](#uapi_drv_klad_set_clear_key) | 设置clear key（明文密钥） |
| [uapi_drv_kdf_update](#uapi_drv_kdf_update) | 更新根密钥 |

## Functions

### uapi_drv_km_init <a id="uapi_drv_km_init"></a>

```c
errcode_t uapi_drv_km_init(void)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 提供KM模块初始化能力，完成密钥管理模块内部环境准备
- 初始化过程中完成模块内部互斥锁、keyslot上下文、klad上下文等资源的初始化
- 在使用任何KM模块的其他接口前，必须先调用此接口完成初始化

**前置条件**

- 无特殊前置条件

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_km_deinit <a id="uapi_drv_km_deinit"></a>

```c
errcode_t uapi_drv_km_deinit(void)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 提供KM模块去初始化能力，释放密钥管理模块内部资源
- 去初始化过程中销毁互斥锁、清零keyslot上下文列表与klad上下文列表
- 调用此接口后，KM模块所有接口不可再使用

**前置条件**

- KM模块已通过uapi_drv_km_init()初始化完成

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去初始化成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_keyslot_create <a id="uapi_drv_keyslot_create"></a>

```c
errcode_t uapi_drv_keyslot_create(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 创建keyslot句柄，用于保存密钥的通道句柄
- 根据keyslot_type分配对应类型的keyslot通道，MCIPHER类型最多支持8个通道，HMAC (Hash-based Message Authentication Code) 类型最多支持2个通道
- 创建成功后添加睡眠否决，防止系统在密钥操作期间进入睡眠

**前置条件**

- KM模块已通过uapi_drv_km_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| keyslot_type | [uapi_drv_keyslot_type_t](#enum_uapi_drv_keyslot_type_t) | key的用途类型 | UAPI_DRV_KEYSLOT_TYPE_MCIPHER(0), UAPI_DRV_KEYSLOT_TYPE_HMAC(1), UAPI_DRV_KEYSLOT_TYPE_FLASH(2) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| keyslot_handle | uint32_t* | 用于保存key的通道句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | keyslot创建成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_keyslot_destroy <a id="uapi_drv_keyslot_destroy"></a>

```c
errcode_t uapi_drv_keyslot_destroy(uint32_t keyslot_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 销毁keyslot句柄，释放对应密钥通道资源
- 销毁成功后移除睡眠否决，允许系统进入睡眠
- 仅销毁与当前进程相同的keyslot句柄，不允许跨进程操作

**前置条件**

- KM模块已通过uapi_drv_km_init()初始化完成
- keyslot已通过uapi_drv_keyslot_create()创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| keyslot_handle | uint32_t | 要销毁的key的通道句柄 | 有效的keyslot句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | keyslot销毁成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_create <a id="uapi_drv_klad_create"></a>

```c
errcode_t uapi_drv_klad_create(uint32_t *klad_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 创建keyladder通道句柄，用于密钥阶梯派生操作
- 最多支持4个klad虚拟通道同时创建
- 创建成功后klad通道处于已打开但未关联、未设置属性的状态

**前置条件**

- KM模块已通过uapi_drv_km_init()初始化完成

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| klad_handle | uint32_t* | keyladder通道句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | klad通道创建成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_destroy <a id="uapi_drv_klad_destroy"></a>

```c
errcode_t uapi_drv_klad_destroy(uint32_t klad_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 销毁keyladder通道句柄，释放klad通道资源
- 仅销毁与当前进程相同的klad句柄，不允许跨进程操作
- 销毁后清零该通道上下文，包括关联状态、属性设置状态等

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | 要销毁的keyladder通道句柄 | 有效的klad句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | klad通道销毁成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_attach <a id="uapi_drv_klad_attach"></a>

```c
errcode_t uapi_drv_klad_attach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 将keyslot句柄与klad句柄关联，建立密钥通道与密钥阶梯的绑定关系
- 关联后klad通道可对目标模块进行密钥派生与分发操作
- 当klad_type为NPU时，keyslot_handle直接作为NPU模块的密钥句柄使用

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- 当klad_type非NPU时，keyslot已通过uapi_drv_keyslot_create()创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | 要关联的keyladder通道句柄 | 有效的klad句柄 |
| klad_type | [uapi_drv_klad_dest_t](#enum_uapi_drv_klad_dest_t) | klad目标模块 | UAPI_DRV_KLAD_DEST_MCIPHER(0), UAPI_DRV_KLAD_DEST_HMAC(1), UAPI_DRV_KLAD_DEST_FLASH(2), UAPI_DRV_KLAD_DEST_NPU(3), UAPI_DRV_KLAD_DEST_AIDSP(4) |
| keyslot_handle | uint32_t | 要关联的keyslot通道句柄 | 有效的keyslot句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 关联成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_detach <a id="uapi_drv_klad_detach"></a>

```c
errcode_t uapi_drv_klad_detach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 将keyslot句柄与klad句柄解关联，断开密钥通道与密钥阶梯的绑定关系
- 解关联时校验klad_type与keyslot_handle与关联时一致，防止解关联错误的通道
- 解关联后klad通道恢复为未关联状态

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- klad通道已通过uapi_drv_klad_attach()与keyslot关联成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | 要解关联的keyladder通道句柄 | 有效的klad句柄 |
| klad_type | [uapi_drv_klad_dest_t](#enum_uapi_drv_klad_dest_t) | klad目标模块 | UAPI_DRV_KLAD_DEST_MCIPHER(0), UAPI_DRV_KLAD_DEST_HMAC(1), UAPI_DRV_KLAD_DEST_FLASH(2), UAPI_DRV_KLAD_DEST_NPU(3), UAPI_DRV_KLAD_DEST_AIDSP(4) |
| keyslot_handle | uint32_t | 要解关联的keyslot通道句柄 | 与关联时相同的keyslot句柄 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 解关联成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_set_attr <a id="uapi_drv_klad_set_attr"></a>

```c
errcode_t uapi_drv_klad_set_attr(uint32_t klad_handle, const uapi_drv_klad_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 设置keyladder的属性，包括根密钥类型、工作密钥算法引擎、加解密支持及安全属性
- 属性设置后，后续密钥派生与分发操作将按此属性执行
- 设置属性时对attr参数进行空指针校验

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- attr指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | Keyladder通道句柄 | 有效的klad句柄 |
| attr | const [uapi_drv_klad_attr_t](#struct_uapi_drv_klad_attr_t)* | Keyladder配置属性 | 非NULL |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 属性设置成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | attr为NULL |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_get_attr <a id="uapi_drv_klad_get_attr"></a>

```c
errcode_t uapi_drv_klad_get_attr(uint32_t klad_handle, uapi_drv_klad_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 获取keyladder的属性，包括根密钥类型、工作密钥算法引擎、加解密支持及安全属性
- 获取前校验attr参数空指针，以及klad通道是否已设置属性
- 仅在已调用uapi_drv_klad_set_attr()后才能获取到有效属性

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- klad通道已通过uapi_drv_klad_set_attr()设置属性
- attr指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | Keyladder通道句柄 | 有效的klad句柄 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| attr | [uapi_drv_klad_attr_t](#struct_uapi_drv_klad_attr_t)* | Keyladder配置属性 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 属性获取成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | attr为NULL |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_set_effective_key <a id="uapi_drv_klad_set_effective_key"></a>

```c
errcode_t uapi_drv_klad_set_effective_key(uint32_t klad_handle, const uapi_drv_klad_effective_key_t *key)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 设置effective key，通过硬件KDF算法派生密钥并分发到目标模块
- 使用盐值作为用户输入材料参与密钥派生，不同盐值产生不同的工作密钥
- 支持oneway模式，设置后即使使用相同派生材料也无法派生出相同密钥
- 操作期间添加睡眠否决，防止系统在密钥派生期间进入睡眠

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- klad通道已通过uapi_drv_klad_attach()关联keyslot
- klad通道已通过uapi_drv_klad_set_attr()设置属性
- key指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | Keyladder通道句柄 | 有效的klad句柄 |
| key | const [uapi_drv_klad_effective_key_t](#struct_uapi_drv_klad_effective_key_t)* | Effective key配置属性 | 非NULL，salt_length为28，key_size为128/192/256 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | effective key设置成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | key为NULL或参数不合法 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_klad_set_clear_key <a id="uapi_drv_klad_set_clear_key"></a>

```c
errcode_t uapi_drv_klad_set_clear_key(uint32_t klad_handle, const uapi_drv_klad_clear_key_t *key)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 设置clear key，将明文密钥分发到目标模块
- 明文密钥长度约束：对称算法为16/24/32字节；HMAC-SHA1/SHA224/SHA256/SM3不超过64字节；HMAC-SHA384/SHA512不超过128字节
- 操作期间添加睡眠否决，防止系统在密钥分发期间进入睡眠

**前置条件**

- klad通道已通过uapi_drv_klad_create()创建成功
- klad通道已通过uapi_drv_klad_attach()关联keyslot
- klad通道已通过uapi_drv_klad_set_attr()设置属性
- key指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| klad_handle | uint32_t | Keyladder通道句柄 | 有效的klad句柄 |
| key | const [uapi_drv_klad_clear_key_t](#struct_uapi_drv_klad_clear_key_t)* | Clear key配置属性 | 非NULL，key_length与算法类型匹配 |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | clear key设置成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | key为NULL |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

### uapi_drv_kdf_update <a id="uapi_drv_kdf_update"></a>

```c
errcode_t uapi_drv_kdf_update(uapi_drv_kdf_otp_key_t otp_key, uapi_drv_kdf_update_alg_t alg)
```

**头文件清单**

```c
#include "include/driver/security_unified/km.h"
```

**功能说明**

- 更新根密钥，通过指定OTP (One-Time Programmable) 密钥类型和算法类型完成密钥更新
- 支持MRK1、USK、RUSK三种OTP密钥类型和AES (Advanced Encryption Standard)、SM4 (SM4 Block Cipher)两种对称算法
- 操作期间添加睡眠否决，防止系统在密钥更新期间进入睡眠

**前置条件**

- KM模块已通过uapi_drv_km_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| otp_key | [uapi_drv_kdf_otp_key_t](#enum_uapi_drv_kdf_otp_key_t) | 要更新的密钥类型 | UAPI_DRV_KDF_OTP_KEY_MRK1(0), UAPI_DRV_KDF_OTP_KEY_USK(1), UAPI_DRV_KDF_OTP_KEY_RUSK(2) |
| alg | [uapi_drv_kdf_update_alg_t](#enum_uapi_drv_kdf_update_alg_t) | 更新时的算法类型 | UAPI_DRV_KDF_UPDATE_ALG_AES(0), UAPI_DRV_KDF_UPDATE_ALG_SM4(1) |

**返回值**
&emsp;&emsp;返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 根密钥更新成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SECURITY_UNIFIED_SUPPORT_KM | 功能宏 | 支持KM接口功能 | n |
| CONFIG_DRIVER_SUPPORT_SECURITY_UNIFIED | 功能宏 | 支持SECURITY_UNIFIED功能 | y |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为KM模块所有接口的返回值类型，表示接口执行结果

## Enumerations

### uapi_drv_keyslot_type_t <a id="enum_uapi_drv_keyslot_type_t"></a>

```c
typedef enum {
    UAPI_DRV_KEYSLOT_TYPE_MCIPHER = 0,
    UAPI_DRV_KEYSLOT_TYPE_HMAC,
    UAPI_DRV_KEYSLOT_TYPE_FLASH,
} uapi_drv_keyslot_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KEYSLOT_TYPE_MCIPHER | 0 | 对称加密密钥槽 |
| UAPI_DRV_KEYSLOT_TYPE_HMAC | 1 | HMAC密钥槽 |
| UAPI_DRV_KEYSLOT_TYPE_FLASH | 2 | Flash在线解密密钥槽 |

### uapi_drv_kdf_otp_key_t <a id="enum_uapi_drv_kdf_otp_key_t"></a>

```c
typedef enum {
    UAPI_DRV_KDF_OTP_KEY_MRK1 = 0,
    UAPI_DRV_KDF_OTP_KEY_USK,
    UAPI_DRV_KDF_OTP_KEY_RUSK
} uapi_drv_kdf_otp_key_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KDF_OTP_KEY_MRK1 | 0 | 主根密钥1 |
| UAPI_DRV_KDF_OTP_KEY_USK | 1 | 用户安全密钥 |
| UAPI_DRV_KDF_OTP_KEY_RUSK | 2 | 根用户安全密钥 |

### uapi_drv_kdf_update_alg_t <a id="enum_uapi_drv_kdf_update_alg_t"></a>

```c
typedef enum {
    UAPI_DRV_KDF_UPDATE_ALG_AES = 0,
    UAPI_DRV_KDF_UPDATE_ALG_SM4
} uapi_drv_kdf_update_alg_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KDF_UPDATE_ALG_AES | 0 | AES算法 |
| UAPI_DRV_KDF_UPDATE_ALG_SM4 | 1 | SM4算法 |

### uapi_drv_kdf_hard_key_type_t <a id="enum_uapi_drv_kdf_hard_key_type_t"></a>

```c
typedef enum {
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK0  = 0x03000000,
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_DRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_DRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_PSK,
    UAPI_DRV_KDF_HARD_KEY_TYPE_FDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_OARK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK3,

    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_REE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_TEE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_REE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_TEE,
} uapi_drv_kdf_hard_key_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK0 | 0x03000000 | 静态引导根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK1 | 0x03000001 | 静态引导根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK2 | 0x03000002 | 静态引导根密钥2 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK0 | 0x03000003 | 辅助引导根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK1 | 0x03000004 | 辅助引导根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK2 | 0x03000005 | 辅助引导根密钥2 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_DRK0 | 0x03000006 | 设备根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_DRK1 | 0x03000007 | 设备根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK0 | 0x03000008 | 远程设备根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK1 | 0x03000009 | 远程设备根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_PSK | 0x0300000A | 预共享密钥 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_FDRK0 | 0x0300000B | 固件设备根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK0 | 0x0300000C | ODR根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK1 | 0x0300000D | ODR根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_OARK0 | 0x0300000E | OAR根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK0 | 0x0300000F | MDR根密钥0 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK1 | 0x03000010 | MDR根密钥1 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK2 | 0x03000011 | MDR根密钥2 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK3 | 0x03000012 | MDR根密钥3 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_REE | 0x03000013 | REE (Rich Execution Environment) 侧辅助引导根密钥 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_TEE | 0x03000014 | TEE (Trusted Execution Environment) 侧辅助引导根密钥 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_REE | 0x03000015 | REE侧远程设备根密钥 |
| UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_TEE | 0x03000016 | TEE侧远程设备根密钥 |

### uapi_drv_kdf_hard_alg_t <a id="enum_uapi_drv_kdf_hard_alg_t"></a>

```c
typedef enum {
    UAPI_DRV_KDF_HARD_ALG_SHA256 = 0,
    UAPI_DRV_KDF_HARD_ALG_SM3,
    UAPI_DRV_KDF_HARD_ALG_MAX
} uapi_drv_kdf_hard_alg_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KDF_HARD_ALG_SHA256 | 0 | SHA256 (Secure Hash Algorithm 256-bit) 算法 |
| UAPI_DRV_KDF_HARD_ALG_SM3 | 1 | SM3 (SM3 Cryptographic Hash Algorithm) 算法 |
| UAPI_DRV_KDF_HARD_ALG_MAX | 2 | 算法类型上限值 |

### uapi_drv_klad_engine_t <a id="enum_uapi_drv_klad_engine_t"></a>

```c
typedef enum {
    UAPI_DRV_KLAD_ENGINE_AES = 0x20,
    UAPI_DRV_KLAD_ENGINE_LAE = 0x40,
    UAPI_DRV_KLAD_ENGINE_SM4 = 0x50,
    UAPI_DRV_KLAD_ENGINE_TDES = 0x70,
    UAPI_DRV_KLAD_ENGINE_SHA1_HMAC = 0xA0,
    UAPI_DRV_KLAD_ENGINE_SHA2_HMAC = 0xA1,
    UAPI_DRV_KLAD_ENGINE_SM3_HMAC = 0xA2,
    UAPI_DRV_KLAD_ENGINE_MAX
} uapi_drv_klad_engine_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KLAD_ENGINE_AES | 0x20 | AES算法引擎 |
| UAPI_DRV_KLAD_ENGINE_LAE | 0x40 | LAE算法引擎 |
| UAPI_DRV_KLAD_ENGINE_SM4 | 0x50 | SM4算法引擎 |
| UAPI_DRV_KLAD_ENGINE_TDES | 0x70 | TDES (Triple Data Encryption Standard) 算法引擎 |
| UAPI_DRV_KLAD_ENGINE_SHA1_HMAC | 0xA0 | SHA1 (Secure Hash Algorithm 1) HMAC引擎 |
| UAPI_DRV_KLAD_ENGINE_SHA2_HMAC | 0xA1 | SHA2 HMAC引擎 |
| UAPI_DRV_KLAD_ENGINE_SM3_HMAC | 0xA2 | SM3 HMAC引擎 |
| UAPI_DRV_KLAD_ENGINE_MAX | 0xA3 | 引擎类型上限值 |

### uapi_drv_klad_dest_t <a id="enum_uapi_drv_klad_dest_t"></a>

```c
typedef enum {
    UAPI_DRV_KLAD_DEST_MCIPHER = 0,
    UAPI_DRV_KLAD_DEST_HMAC,
    UAPI_DRV_KLAD_DEST_FLASH,
    UAPI_DRV_KLAD_DEST_NPU,
    UAPI_DRV_KLAD_DEST_AIDSP,
    UAPI_DRV_KLAD_DEST_MAX,
} uapi_drv_klad_dest_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KLAD_DEST_MCIPHER | 0 | 对称加密目标模块 |
| UAPI_DRV_KLAD_DEST_HMAC | 1 | HMAC目标模块 |
| UAPI_DRV_KLAD_DEST_FLASH | 2 | Flash在线解密目标模块 |
| UAPI_DRV_KLAD_DEST_NPU | 3 | NPU目标模块 |
| UAPI_DRV_KLAD_DEST_AIDSP | 4 | AIDSP目标模块 |
| UAPI_DRV_KLAD_DEST_MAX | 5 | 目标模块上限值 |

### uapi_drv_klad_flash_key_type_t <a id="enum_uapi_drv_klad_flash_key_type_t"></a>

```c
typedef enum {
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_INVALID,
} uapi_drv_klad_flash_key_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KLAD_FLASH_KEY_TYPE_REE_DEC | 0x00 | REE Flash在线解密密钥 |
| UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_DEC | 0x01 | TEE Flash在线解密密钥 |
| UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_AUT | 0x02 | TEE Flash在线认证密钥 |
| UAPI_DRV_KLAD_FLASH_KEY_TYPE_INVALID | 0x03 | 无效密钥类型 |

### uapi_drv_klad_key_size_t <a id="enum_uapi_drv_klad_key_size_t"></a>

```c
typedef enum {
    UAPI_DRV_KLAD_KEY_SIZE_128BIT,
    UAPI_DRV_KLAD_KEY_SIZE_192BIT,
    UAPI_DRV_KLAD_KEY_SIZE_256BIT,
    UAPI_DRV_KLAD_KEY_SIZE_INVALID = 0xffffffff
} uapi_drv_klad_key_size_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KLAD_KEY_SIZE_128BIT | 0 | 128位密钥长度 |
| UAPI_DRV_KLAD_KEY_SIZE_192BIT | 1 | 192位密钥长度 |
| UAPI_DRV_KLAD_KEY_SIZE_256BIT | 2 | 256位密钥长度 |
| UAPI_DRV_KLAD_KEY_SIZE_INVALID | 0xFFFFFFFF | 无效密钥长度 |

### uapi_drv_klad_hmac_type_t <a id="enum_uapi_drv_klad_hmac_type_t"></a>

```c
typedef enum {
    UAPI_DRV_KLAD_HMAC_TYPE_SHA1 = 0x20,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA224,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA256,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA384,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA512,
    UAPI_DRV_KLAD_HMAC_TYPE_SM3 = 0x30,
    UAPI_DRV_KLAD_HMAC_TYPE_MAX,
    UAPI_DRV_KLAD_HMAC_TYPE_INVALID = 0xffffffff,
} uapi_drv_klad_hmac_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_KLAD_HMAC_TYPE_SHA1 | 0x20 | HMAC-SHA1算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_SHA224 | 0x21 | HMAC-SHA224算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_SHA256 | 0x22 | HMAC-SHA256算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_SHA384 | 0x23 | HMAC-SHA384算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_SHA512 | 0x24 | HMAC-SHA512算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_SM3 | 0x30 | HMAC-SM3算法 |
| UAPI_DRV_KLAD_HMAC_TYPE_MAX | 0x31 | 算法类型上限值 |
| UAPI_DRV_KLAD_HMAC_TYPE_INVALID | 0xFFFFFFFF | 无效算法类型 |

## Structures

### uapi_drv_klad_clear_key_t <a id="struct_uapi_drv_klad_clear_key_t"></a>

```c
typedef struct {
    uint8_t *key;     /*!< 明文key内容。 */
    uint32_t key_length;  /*!< 明文key长度，单位为字节。对于对称算法，只能是16/24/32；
                               对于HMAC-SH1/SHA224/SHA256/SM3，长度不超过64；
                               对于HMAC-SHA384/SHA512，长度不超过128。 */
    bool key_parity; /*!< key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 */
    uapi_drv_klad_hmac_type_t hmac_type; /*!< hmac 算法。当目标为HMAC算法引擎时生效。 */
} uapi_drv_klad_clear_key_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| key | uint8_t* | 明文key内容 |
| key_length | uint32_t | 明文key长度，单位为字节。对于对称算法只能是16/24/32；对于HMAC-SHA1/SHA224/SHA256/SM3不超过64；对于HMAC-SHA384/SHA512不超过128 |
| key_parity | bool | key的奇偶属性。当目标为对称算法引擎且key_length为16时生效 |
| hmac_type | [uapi_drv_klad_hmac_type_t](#enum_uapi_drv_klad_hmac_type_t) | HMAC算法。当目标为HMAC算法引擎时生效 |

### uapi_drv_klad_config_t <a id="struct_uapi_drv_klad_config_t"></a>

```c
typedef struct {
    uapi_drv_kdf_hard_key_type_t rootkey_type;     /*!< 要生成的根密钥的类型。 */
} uapi_drv_klad_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rootkey_type | [uapi_drv_kdf_hard_key_type_t](#enum_uapi_drv_kdf_hard_key_type_t) | 要生成的根密钥的类型 |

### uapi_drv_klad_key_config_t <a id="struct_uapi_drv_klad_key_config_t"></a>

```c
typedef struct {
    uapi_drv_klad_engine_t engine;  /*!< 工作密钥可用于加密引擎的哪种算法。 */
    bool decrypt_support;    /*!< 工作密钥可用于解密。 */
    bool encrypt_support;    /*!< 工作密钥可用于加密。 */
} uapi_drv_klad_key_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| engine | [uapi_drv_klad_engine_t](#enum_uapi_drv_klad_engine_t) | 工作密钥可用于加密引擎的哪种算法 |
| decrypt_support | bool | 工作密钥可用于解密 |
| encrypt_support | bool | 工作密钥可用于加密 |

### uapi_drv_klad_key_secure_config_t <a id="struct_uapi_drv_klad_key_secure_config_t"></a>

```c
typedef struct {
    bool key_sec;    /*!< 安全密钥只能由TEE CPU和AIDSP锁定的对称通道或哈希通道使用。 */
    bool master_only_enable; /*!< 只有与Keylader相同的CPU锁定的密码或哈希通道才能使用此密钥，
                                  当TEE CPU或AIDSP时生效。 */
    bool dest_buf_sec_support;   /*!< 目标引擎的目标缓冲区可以是安全的。 */
    bool dest_buf_non_sec_support; /*!< 目标引擎的目标缓冲区可以是非安全的。 */
    bool src_buf_sec_support;      /*!< 目标引擎的源缓冲区可以是安全的。 */
    bool src_buf_non_sec_support;  /*!< 目标引擎的源缓冲区可以是非安全的。 */
} uapi_drv_klad_key_secure_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| key_sec | bool | 安全密钥只能由TEE CPU和AIDSP锁定的对称通道或哈希通道使用 |
| master_only_enable | bool | 只有与Keyladder相同的CPU锁定的密码或哈希通道才能使用此密钥，当TEE CPU或AIDSP时生效 |
| dest_buf_sec_support | bool | 目标引擎的目标缓冲区可以是安全的 |
| dest_buf_non_sec_support | bool | 目标引擎的目标缓冲区可以是非安全的 |
| src_buf_sec_support | bool | 目标引擎的源缓冲区可以是安全的 |
| src_buf_non_sec_support | bool | 目标引擎的源缓冲区可以是非安全的 |

### uapi_drv_klad_attr_t <a id="struct_uapi_drv_klad_attr_t"></a>

```c
typedef struct {
    uapi_drv_klad_config_t klad_cfg;    /*!< KeyLader配置，对硬件密钥有效。 */
    uapi_drv_klad_key_config_t key_cfg; /*!< 工作密钥配置。 */
    uapi_drv_klad_key_secure_config_t key_sec_cfg;  /*!< 工作密钥安全配置。 */
    uint32_t rkp_sw_cfg;                /*!< NPU模块使用，其他模块不需要配置。 */
} uapi_drv_klad_attr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| klad_cfg | [uapi_drv_klad_config_t](#struct_uapi_drv_klad_config_t) | KeyLadder配置，对硬件密钥有效 |
| key_cfg | [uapi_drv_klad_key_config_t](#struct_uapi_drv_klad_key_config_t) | 工作密钥配置 |
| key_sec_cfg | [uapi_drv_klad_key_secure_config_t](#struct_uapi_drv_klad_key_secure_config_t) | 工作密钥安全配置 |
| rkp_sw_cfg | uint32_t | NPU模块使用，其他模块不需要配置 |

### uapi_drv_klad_effective_key_t <a id="struct_uapi_drv_klad_effective_key_t"></a>

```c
typedef struct {
    uapi_drv_kdf_hard_alg_t kdf_hard_alg;   /*!< key派生时使用的hmac算法。 */
    bool key_parity; /*!< key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 */
    uapi_drv_klad_key_size_t key_size;  /*!< 需要派生的key的长度。 */
    uint8_t *salt;    /*!< 盐值内容。作为用户输入材料参与密钥派生，盐值不同，最终的工作密钥也不同。 */
    uint32_t salt_length; /*!< 盐值长度，单位是字节。只能为28。 */
    bool oneway; /*!< 密钥派生的单一性，默认为0。如果设置为1，即使使用相同的密钥派生材料也无法派生出相同的密钥。 */
} uapi_drv_klad_effective_key_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| kdf_hard_alg | [uapi_drv_kdf_hard_alg_t](#enum_uapi_drv_kdf_hard_alg_t) | key派生时使用的HMAC算法 |
| key_parity | bool | key的奇偶属性。当目标为对称算法引擎且key_length为16时生效 |
| key_size | [uapi_drv_klad_key_size_t](#enum_uapi_drv_klad_key_size_t) | 需要派生的key的长度 |
| salt | uint8_t* | 盐值内容。作为用户输入材料参与密钥派生，盐值不同，最终的工作密钥也不同 |
| salt_length | uint32_t | 盐值长度，单位是字节。只能为28 |
| oneway | bool | 密钥派生的单一性，默认为0。如果设置为1，即使使用相同的密钥派生材料也无法派生出相同的密钥 |

