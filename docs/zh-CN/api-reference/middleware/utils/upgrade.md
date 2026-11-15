# 升级

**头文件清单**
```c
#include "include/middleware/utils/upg.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_upg_init](#uapi_upg_init) | 初始化升级模块 |
| [uapi_upg_start](#uapi_upg_start) | 开始本地升级 |
| [uapi_upg_register_progress_callback](#uapi_upg_register_progress_callback) | 注册升级进度通知回调函数 |
| [uapi_upg_get_result](#uapi_upg_get_result) | 获取升级结果 |
| [uapi_upg_get_status](#uapi_upg_get_status) | 获取升级状态 |
| [uapi_upg_prepare](#uapi_upg_prepare) | 准备本地存储器用于存放升级包 |
| [uapi_upg_reset_upgrade_flag](#uapi_upg_reset_upgrade_flag) | 重置升级标记 |
| [uapi_upg_write_package_async](#uapi_upg_write_package_async) | 异步写入升级包数据到本地存储器 |
| [uapi_upg_write_package_sync](#uapi_upg_write_package_sync) | 同步写入升级包数据到本地存储器 |
| [uapi_upg_read_package](#uapi_upg_read_package) | 从本地存储器读取升级包数据 |
| [uapi_upg_get_storage_size](#uapi_upg_get_storage_size) | 获取可存放升级包的空间大小 |
| [uapi_upg_request_upgrade](#uapi_upg_request_upgrade) | 申请开始进行本地升级 |
| [uapi_upg_verify_file_head](#uapi_upg_verify_file_head) | 校验升级包头结构 |
| [uapi_upg_verify_file_image](#uapi_upg_verify_file_image) | 校验升级包中的升级镜像 |
| [uapi_upg_verify_file](#uapi_upg_verify_file) | 校验整个升级包 |
| [uapi_upg_register_user_defined_verify_func](#uapi_upg_register_user_defined_verify_func) | 注册用户自定义字段校验函数 |

## Functions

### uapi_upg_init <a id="uapi_upg_init"></a>

```c
errcode_t uapi_upg_init(const upg_func_t *func_list)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 初始化升级模块
- 注册升级模块使用的函数列表，包括内存分配、释放和串口输出函数

**前置条件**

- 升级模块尚未初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func_list | const [upg_func_t](#struct_upg_func_t) * | 升级模块使用的注册函数列表 | 非NULL，malloc和free为必选函数 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_start <a id="uapi_upg_start"></a>

```c
errcode_t uapi_upg_start(void)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 开始本地升级

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

- 无

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_register_progress_callback <a id="uapi_upg_register_progress_callback"></a>

```c
errcode_t uapi_upg_register_progress_callback(uapi_upg_progress_cb func)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 注册升级进度通知回调函数
- 升级过程中通过回调函数通知上层升级进度百分比

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [uapi_upg_progress_cb](#typedef_uapi_upg_progress_cb) | 进度通知的回调函数 | 非NULL |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_get_result <a id="uapi_upg_get_result"></a>

```c
errcode_t uapi_upg_get_result(upg_result_t *result, uint32_t *last_image_index)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 获取升级结果
- 获取升级的最终结果和最后一个处理的升级镜像序号

**前置条件**

- 升级流程已结束

**入参**

- 无

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [upg_result_t](#enum_upg_result_t) * | 保存获取的升级结果 | 非NULL |
| last_image_index | uint32_t * | 保存获取的最后一个处理的升级镜像的序号 | 非NULL |

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_get_status <a id="uapi_upg_get_status"></a>

```c
upg_status_t uapi_upg_get_status(void)
```

**头文件清单**

```c
#include "include/middleware/utils/upg.h"
```

**功能说明**

- 获取升级状态

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

- 无

**出参**

- 无

**返回值**

- 返回类型：[upg_status_t](#enum_upg_status_t)
- UPG_STATUS_SUCC：升级成功
- UPG_STATUS_FAIL：升级失败
- UPG_STATUS_UPDATING：正在升级
- UPG_STATUS_NONE：非升级状态

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_prepare <a id="uapi_upg_prepare"></a>

```c
errcode_t uapi_upg_prepare(upg_prepare_info_t *prepare_info)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 用来保存升级包的本地存储器的准备工作
- 该函数阻塞等待执行完返回

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| prepare_info | [upg_prepare_info_t](#struct_upg_prepare_info_t) * | 准备信息的指针 | 非NULL，package_len需大于0 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**参考案例**

- [hilink_open_ota_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_open_ota_adapter.c)

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_reset_upgrade_flag <a id="uapi_upg_reset_upgrade_flag"></a>

```c
errcode_t uapi_upg_reset_upgrade_flag(void)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 重置升级标记
- 该函数阻塞等待执行完返回

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

- 无

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_write_package_async <a id="uapi_upg_write_package_async"></a>

```c
errcode_t uapi_upg_write_package_async(uint32_t offset, const uint8_t *buff, uint16_t len, uapi_upg_write_done_cb callback)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 将升级包数据写入本地存储器（异步方式）
- 该函数异步执行立即返回，实际操作完成后调用回调函数
- 当前接口的回调函数被调用且返回写成功才能继续写下一个数据包，在此之前禁止再次调用该接口写下一个数据包

**前置条件**

- 已调用uapi_upg_prepare完成存储器准备

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | uint32_t | 相对升级包开头的偏移 | 大于等于0 |
| buff | const uint8_t * | 存放升级包数据的buffer | 非NULL |
| len | uint16_t | 升级包数据buffer的长度 | 大于0 |
| callback | [uapi_upg_write_done_cb](#typedef_uapi_upg_write_done_cb) | 写入完成的回调函数 | 非NULL |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_write_package_sync <a id="uapi_upg_write_package_sync"></a>

```c
errcode_t uapi_upg_write_package_sync(uint32_t offset, const uint8_t *buff, uint16_t len)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 将升级包数据写入本地存储器（同步方式）
- 该函数同步执行

**前置条件**

- 已调用uapi_upg_prepare完成存储器准备

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | uint32_t | 相对升级包开头的偏移 | 大于等于0 |
| buff | const uint8_t * | 存放升级包数据的buffer | 非NULL |
| len | uint16_t | 升级包数据buffer的长度 | 大于0 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_read_package <a id="uapi_upg_read_package"></a>

```c
errcode_t uapi_upg_read_package(uint32_t offset, uint8_t *buff, uint32_t len)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 从本地存储器读取升级包数据

**前置条件**

- 已调用uapi_upg_prepare完成存储器准备
- 升级包数据已写入本地存储器

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | uint32_t | 相对升级包开头的偏移 | 大于等于0 |
| buff | uint8_t * | 存放升级包数据的buffer | 非NULL |
| len | uint32_t | 升级包数据的长度 | 大于0 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_get_storage_size <a id="uapi_upg_get_storage_size"></a>

```c
uint32_t uapi_upg_get_storage_size(void)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 获取可存放升级包的空间大小

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

- 无

**出参**

- 无

**返回值**

- 返回类型：uint32_t
- 0：失败
- 其他值：成功返回空间大小

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_request_upgrade <a id="uapi_upg_request_upgrade"></a>

```c
errcode_t uapi_upg_request_upgrade(bool reset)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 申请开始进行本地升级

**前置条件**

- 已完成升级包写入和校验

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| reset | bool | 申请流程结束后是否重启系统 | true：重启系统，false：不重启 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**参考案例**

- [hilink_open_ota_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_open_ota_adapter.c)

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_verify_file_head <a id="uapi_upg_verify_file_head"></a>

```c
errcode_t uapi_upg_verify_file_head(const upg_package_header_t *pkg_header)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 校验升级包头结构

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化
- 升级包头数据已读取

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pkg_header | const [upg_package_header_t](#struct_upg_package_header_t) * | 指向升级包头结构的指针 | 非NULL |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_verify_file_image <a id="uapi_upg_verify_file_image"></a>

```c
errcode_t uapi_upg_verify_file_image(const upg_image_header_t *img_header, const uint8_t *hash, uint32_t hash_len, bool verify_old)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 校验升级包中的升级镜像
- verify_old一般适用于差分升级场景下，用于判断当前使用的旧镜像与做差分镜像时的旧镜像是否一致

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化
- 升级镜像头数据已读取

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| img_header | const [upg_image_header_t](#struct_upg_image_header_t) * | 指向升级包中升级镜像头结构的指针 | 非NULL |
| hash | const uint8_t * | 升级镜像的HASH值 | 非NULL |
| hash_len | uint32_t | HASH的长度（单位：字节） | 大于0 |
| verify_old | bool | 是否校验旧镜像 | true：校验旧镜像，false：不校验 |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_verify_file <a id="uapi_upg_verify_file"></a>

```c
errcode_t uapi_upg_verify_file(const upg_package_header_t *pkg_header)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 校验整个升级包

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化
- 升级包数据已写入本地存储器

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pkg_header | const [upg_package_header_t](#struct_upg_package_header_t) * | 指向升级包头结构的指针 | 非NULL |

**出参**

- 无

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败返回ERRCODE_FAIL或其他错误码

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

### uapi_upg_register_user_defined_verify_func <a id="uapi_upg_register_user_defined_verify_func"></a>

```c
void uapi_upg_register_user_defined_verify_func(uapi_upg_user_defined_check func, uintptr_t param)
```

**头文件清单** ``include/middleware/utils/upg.h``

**功能说明**

- 注册用户自定义字段的校验函数
- 注册后，调用uapi_upg_verify_file_head和uapi_upg_verify_file函数时，校验函数会被调用到

**前置条件**

- 已调用uapi_upg_init完成升级模块初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [uapi_upg_user_defined_check](#typedef_uapi_upg_user_defined_check) | 用于校验用户自定义字段的校验函数 | 非NULL |
| param | uintptr_t | 注册参数 | 无特殊约束 |

**出参**

- 无

**返回值**

- 无

**Kconfig配置**

- CONFIG_MIDDLEWARE_SUPPORT_UPG：升级功能总开关，默认y

## Type definitions

### uapi_upg_write_done_cb <a id="typedef_uapi_upg_write_done_cb"></a>
```c
typedef void (*uapi_upg_write_done_cb)(errcode_t result);
```
升级包写入完成后的回调函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | errcode_t | 写入操作的结果 | ERRCODE_SUCC或其他错误码 |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/upg.h``

### uapi_upg_progress_cb <a id="typedef_uapi_upg_progress_cb"></a>
```c
typedef void (*uapi_upg_progress_cb)(uint32_t percent);
```
进度通知回调函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| percent | uint32_t | 升级进度百分比 | 0~100 |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/upg.h``

### uapi_upg_user_defined_check <a id="typedef_uapi_upg_user_defined_check"></a>
```c
typedef errcode_t (*uapi_upg_user_defined_check)(uint8_t *user_info, uint32_t info_len, uintptr_t param);
```
自定义字段类型校验函数。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| user_info | uint8_t * | 用户自定义信息 | 非NULL |
| info_len | uint32_t | 用户自定义信息的长度 | 大于0 |
| param | uintptr_t | 注册参数 | 无特殊约束 |

**返回值**

- ERRCODE_SUCC：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/upg.h``

### upg_func_malloc <a id="typedef_upg_func_malloc"></a>
```c
typedef void *(*upg_func_malloc)(const uint32_t size);
```
升级使用内存分配函数。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | const uint32_t | 需要分配的内存大小 | 大于0 |

**返回值**

- 返回类型：void *
- 分配的内存指针，失败返回NULL

**头文件清单** ``include/middleware/utils/upg.h``

### upg_func_free <a id="typedef_upg_func_free"></a>
```c
typedef void (*upg_func_free)(void *ptr);
```
升级释放内存函数。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ptr | void * | 需要释放的内存指针 | 非NULL |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/upg.h``

### upg_func_serial_putc <a id="typedef_upg_func_serial_putc"></a>
```c
typedef void (*upg_func_serial_putc)(const char c);
```
UPG (Upgrade) 串口输出函数。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| c | const char | 需要输出的字符 | 无特殊约束 |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/upg.h``

## Enumerations

### upg_result_t <a id="enum_upg_result_t"></a>
```c
typedef enum upg_result {
    UPG_RESULT_UPDATE_SUCCESS,
    UPG_RESULT_VERIFY_HEAD_FAILED,
    UPG_RESULT_VERIFY_HASH_TABLE_FAILED,
    UPG_RESULT_VERIFY_IMAGE_FAILED,
    UPG_RESULT_VERIFY_OLD_IMAGE_FAILED,
    UPG_RESULT_DECOMPRESS_IMAGE_FAILED,
    UPG_RESULT_DECRYPT_IMAGE_FAILED,
    UPG_RESULT_RECRYPT_IMAGE_FAILED,
    UPG_RESULT_DIFF_IMAGE_FAILED,
    UPG_RESULT_UPDATE_IMAGE_FAILED,
    UPG_RESULT_PROCESS_NV_FAILED,
    UPG_RESULT_VERIFY_VERSION_FAILED,
    UPG_RESULT_IMAGE_ID_FAILED,
    UPG_RESULT_RETRY_ALL_FAILED,
    UPG_RESULT_MAX
} upg_result_t;
```
升级结果枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| UPG_RESULT_UPDATE_SUCCESS | 0 | 升级成功 |
| UPG_RESULT_VERIFY_HEAD_FAILED | 1 | 校验升级包头失败 |
| UPG_RESULT_VERIFY_HASH_TABLE_FAILED | 2 | 校验Hash Table失败 |
| UPG_RESULT_VERIFY_IMAGE_FAILED | 3 | 校验镜像失败 |
| UPG_RESULT_VERIFY_OLD_IMAGE_FAILED | 4 | 校验旧镜像失败 |
| UPG_RESULT_DECOMPRESS_IMAGE_FAILED | 5 | 解压缩失败 |
| UPG_RESULT_DECRYPT_IMAGE_FAILED | 6 | 解密失败 |
| UPG_RESULT_RECRYPT_IMAGE_FAILED | 7 | 重加密失败 |
| UPG_RESULT_DIFF_IMAGE_FAILED | 8 | 差分恢复失败 |
| UPG_RESULT_UPDATE_IMAGE_FAILED | 9 | 更新镜像到Flash失败 |
| UPG_RESULT_PROCESS_NV_FAILED | 10 | 处理NV (Non-Volatile) 镜像失败 |
| UPG_RESULT_VERIFY_VERSION_FAILED | 11 | 防回滚校验失败 |
| UPG_RESULT_IMAGE_ID_FAILED | 12 | 镜像ID校验失败 |
| UPG_RESULT_RETRY_ALL_FAILED | 13 | 所有升级尝试均失败 |
| UPG_RESULT_MAX | 14 | 升级结果最大值 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_status_t <a id="enum_upg_status_t"></a>
```c
typedef enum upg_status {
    UPG_STATUS_SUCC,
    UPG_STATUS_FAIL,
    UPG_STATUS_UPDATING,
    UPG_STATUS_NONE
} upg_status_t;
```
升级状态枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| UPG_STATUS_SUCC | 0 | 升级成功 |
| UPG_STATUS_FAIL | 1 | 升级失败 |
| UPG_STATUS_UPDATING | 2 | 正在进行升级 |
| UPG_STATUS_NONE | 3 | 非升级状态 |

**头文件清单** ``include/middleware/utils/upg.h``

## Structures

### upg_key_area_data_t <a id="struct_upg_key_area_data_t"></a>
```c
typedef struct upg_key_area_data {
    uint32_t image_id;
    uint32_t struct_version;
    uint32_t struct_length;
    uint32_t signature_length;
    uint32_t key_owner_id;
    uint32_t key_id;
    uint32_t key_alg;
    uint32_t ecc_curve_type;
    uint32_t key_length;
    uint32_t fota_key_version_ext;
    uint32_t mask_fota_key_version_ext;
    uint32_t msid_ext;
    uint32_t mask_msid_ext;
    uint32_t maintenance_mode;
    uint8_t die_id[DIE_ID_LEN];
    uint32_t fota_info_addr;
    uint8_t reserved[KEY_AREA_RESERVED_LEN];
    uint8_t fota_external_public_key[PUBLIC_KEY_LEN];
    uint8_t sig_fota_key_area[SIG_LEN];
} upg_key_area_data_t;
```
升级key区域数据类型。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| image_id | uint32_t | FOTA (Firmware Over-The-Air) key区域的标识 | 有效的image ID |
| struct_version | uint32_t | FOTA key区域结构版本 | 大于0 |
| struct_length | uint32_t | 结构长度 | 大于0 |
| signature_length | uint32_t | 签名长度 | 大于0 |
| key_owner_id | uint32_t | FOTA二级公钥的所有者ID | 有效的owner ID |
| key_id | uint32_t | FOTA二级公共密钥的密钥ID | 有效的key ID |
| key_alg | uint32_t | 二级公钥算法 | 有效的算法类型 |
| ecc_curve_type | uint32_t | ECC (Elliptic Curve Cryptography) 曲线类型 | 有效的曲线类型 |
| key_length | uint32_t | FOTA二级公共密钥的长度 | 大于0 |
| fota_key_version_ext | uint32_t | FOTA二级公共密钥的版本 | 大于0 |
| mask_fota_key_version_ext | uint32_t | FOTA密钥版本扩展的掩码 | 有效的掩码值 |
| msid_ext | uint32_t | 细分市场ID | 有效的MSID (Module/Manufacturer Specific ID) |
| mask_msid_ext | uint32_t | MSID掩码 | 有效的掩码值 |
| maintenance_mode | uint32_t | 维护模式 | 0：禁用，非0：启用 |
| die_id | uint8_t[DIE_ID_LEN] | 芯片组芯片ID，在启用维护模式时有效 | 16字节 |
| fota_info_addr | uint32_t | FOTA info区域的偏移地址 | 有效的偏移地址 |
| reserved | uint8_t[KEY_AREA_RESERVED_LEN] | 为字节对齐而保留的字段 | 无特殊约束 |
| fota_external_public_key | uint8_t[PUBLIC_KEY_LEN] | FOTA二级公钥 | 有效的公钥数据 |
| sig_fota_key_area | uint8_t[SIG_LEN] | FOTA key区域签名 | 有效的签名数据 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_fota_info_data_t <a id="struct_upg_fota_info_data_t"></a>
```c
typedef struct upg_fota_info_data {
    uint32_t image_id;
    uint32_t struct_version;
    uint32_t struct_length;
    uint32_t signature_length;
    uint32_t fota_version_ext;
    uint32_t mask_fota_version_ext;
    uint32_t msid_ext;
    uint32_t mask_msid_ext;
    uint32_t image_hash_table_addr;
    uint32_t image_hash_table_length;
    uint8_t image_hash_table_hash[SHA_256_LENGTH];
    uint32_t image_num;
    uint32_t hardware_id;
    uint8_t user_defined[INFO_AREA_USER_LEN];
    uint8_t sign_fota_info[SIG_LEN];
} upg_fota_info_data_t;
```
升级fota信息数据类型。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| image_id | uint32_t | FOTA信息区标识 | 有效的image ID |
| struct_version | uint32_t | FOTA key区域结构版本 | 大于0 |
| struct_length | uint32_t | 结构长度 | 大于0 |
| signature_length | uint32_t | 签名长度 | 大于0 |
| fota_version_ext | uint32_t | FOTA信息区的版本 | 大于0 |
| mask_fota_version_ext | uint32_t | FOTA二级公共密钥的版本 | 有效的掩码值 |
| msid_ext | uint32_t | 细分市场ID | 有效的MSID |
| mask_msid_ext | uint32_t | MSID掩码 | 有效的掩码值 |
| image_hash_table_addr | uint32_t | FOTA包中镜像哈希表的地址 | 有效的偏移地址 |
| image_hash_table_length | uint32_t | 镜像哈希表的长度 | 大于0 |
| image_hash_table_hash | uint8_t[SHA_256_LENGTH] | 镜像哈希表的哈希 | 32字节 |
| image_num | uint32_t | FOTA镜像的总数 | 大于0 |
| hardware_id | uint32_t | 硬件ID | 有效的硬件ID |
| user_defined | uint8_t[INFO_AREA_USER_LEN] | 预留字节供用户自定义使用 | 无特殊约束 |
| sign_fota_info | uint8_t[SIG_LEN] | FOTA信息签名 | 有效的签名数据 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_image_hash_node_t <a id="struct_upg_image_hash_node_t"></a>
```c
typedef struct upg_image_hash_node {
    uint32_t image_id;
    uint32_t image_addr;
    uint32_t image_length;
    uint8_t image_hash[SHA_256_LENGTH];
} upg_image_hash_node_t;
```
升级镜像哈希节点类型。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| image_id | uint32_t | FOTA key区域的标识 | 有效的image ID |
| image_addr | uint32_t | 镜像头偏移量 | 有效的偏移地址 |
| image_length | uint32_t | 镜像长度 | 大于0 |
| image_hash | uint8_t[SHA_256_LENGTH] | 镜像头的哈希 | 32字节 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_package_header_t <a id="struct_upg_package_header_t"></a>
```c
typedef struct upg_package_header {
    upg_key_area_data_t  key_area;
    upg_fota_info_data_t info_area;
} upg_package_header_t;
```
升级包头类型。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key_area | [upg_key_area_data_t](#struct_upg_key_area_data_t) | 升级密钥区域数据 | 有效的key区域数据 |
| info_area | [upg_fota_info_data_t](#struct_upg_fota_info_data_t) | FOTA info区数据 | 有效的info区域数据 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_image_header_t <a id="struct_upg_image_header_t"></a>
```c
typedef struct upg_image_header {
    uint32_t header_magic;
    uint32_t image_id;
    uint32_t image_offset;
    uint32_t image_len;
    uint8_t image_hash[SHA_256_LENGTH];
    uint32_t old_image_len;
    uint8_t old_image_hash[SHA_256_LENGTH];
    uint32_t new_image_len;
    uint32_t version_ext;
    uint32_t version_mask;
    uint32_t decompress_flag;
    uint32_t re_enc_flag;
    uint32_t root_key_type;
    uint8_t enc_pk_l1[PROTECT_KEY_LEN];
    uint8_t enc_pk_l2[PROTECT_KEY_LEN];
    uint8_t iv[IV_LEN];
    uint8_t padding[4];
} upg_image_header_t;
```
升级包镜像头类型。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| header_magic | uint32_t | 镜像头魔术字 | 有效的魔术字 |
| image_id | uint32_t | FOTA key区域的标识 | 有效的image ID |
| image_offset | uint32_t | 要更新的镜像数据的偏移地址 | 有效的偏移地址 |
| image_len | uint32_t | 更新的镜像数据的长度（实际数据长度，不包括填充字段） | 大于0 |
| image_hash | uint8_t[SHA_256_LENGTH] | 更新镜像数据的哈希 | 32字节 |
| old_image_len | uint32_t | 旧镜像长度 | 大于0 |
| old_image_hash | uint8_t[SHA_256_LENGTH] | 旧镜像的哈希值 | 32字节 |
| new_image_len | uint32_t | 新镜像长度 | 大于0 |
| version_ext | uint32_t | 新镜像版本 | 大于0 |
| version_mask | uint32_t | 版本掩码 | 有效的掩码值 |
| decompress_flag | uint32_t | 解压标志 | 0：不压缩，非0：压缩 |
| re_enc_flag | uint32_t | 重新加密标志 | 0：不重加密，非0：重加密 |
| root_key_type | uint32_t | 用于加密镜像的密钥 | 有效的密钥类型 |
| enc_pk_l1 | uint8_t[PROTECT_KEY_LEN] | 用于解密更新镜像的一级加密保护密钥 | 16字节 |
| enc_pk_l2 | uint8_t[PROTECT_KEY_LEN] | 用于解密更新镜像的二级加密保护密钥 | 16字节 |
| iv | uint8_t[IV_LEN] | 用于解密升级镜像的IV (Initialization Vector) | 16字节 |
| padding | uint8_t[4] | 保留字段，保证整个结构16字节对齐 | 无特殊约束 |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_func_t <a id="struct_upg_func_t"></a>
```c
typedef struct upg_func {
    upg_func_malloc malloc;
    upg_func_free free;
    upg_func_serial_putc serial_putc;
} upg_func_t;
```
可选函数结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| malloc | [upg_func_malloc](#typedef_upg_func_malloc) | 升级使用内存分配函数（必选函数） | 非NULL |
| free | [upg_func_free](#typedef_upg_func_free) | 升级释放内存函数（必选函数） | 非NULL |
| serial_putc | [upg_func_serial_putc](#typedef_upg_func_serial_putc) | UPG串口输出函数（可选函数） | 可为NULL |

**头文件清单** ``include/middleware/utils/upg.h``

### upg_prepare_info_t <a id="struct_upg_prepare_info_t"></a>
```c
typedef struct upg_prepare_info {
    uint32_t package_len;
} upg_prepare_info_t;
```
升级准备信息结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| package_len | uint32_t | 升级包的大小 | 大于0 |

**头文件清单** ``include/middleware/utils/upg.h``


