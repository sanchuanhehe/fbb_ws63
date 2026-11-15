# NV 存储

**头文件清单**
```c
#include "include/middleware/utils/nv.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_nv_init](#uapi_nv_init) | 初始化NV (Non-Volatile) 模块 |
| [uapi_nv_write](#uapi_nv_write) | 写入NV数据项 |
| [uapi_nv_write_with_attr](#uapi_nv_write_with_attr) | 写入NV数据项并配置属性及回调 |
| [uapi_nv_read](#uapi_nv_read) | 读取指定NV数据项的值 |
| [uapi_nv_read_with_attr](#uapi_nv_read_with_attr) | 读取NV数据项的值并获取属性 |
| [uapi_nv_get_store_status](#uapi_nv_get_store_status) | 获取NV存储空间使用情况 |
| [uapi_nv_backup](#uapi_nv_backup) | 执行NV备份 |
| [uapi_nv_set_restore_mode_all](#uapi_nv_set_restore_mode_all) | 设置NV全量恢复标志 |
| [uapi_nv_set_restore_mode_partitial](#uapi_nv_set_restore_mode_partitial) | 设置NV部分恢复标志 |
| [uapi_nv_flush](#uapi_nv_flush) | 将NV数据从RAM (Random Access Memory) 同步到Flash |
| [uapi_nv_register_change_notify_proc](#uapi_nv_register_change_notify_proc) | 注册NV键值变更通知回调 |

## Functions

### uapi_nv_init <a id="uapi_nv_init"></a>

```c
void uapi_nv_init(void)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 初始化NV模块，在使用NV读写功能之前必须调用
- 完成NV存储区域的初始化和恢复
- 初始化完成后NV模块才可正常工作

### uapi_nv_write <a id="uapi_nv_write"></a>

```c
errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 写入NV数据项，默认属性为Normal，无回调函数
- 通过key索引写入对应的数据值
- 写入操作将数据持久化到Flash存储

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key | uint16_t | NV项的key ID，用于索引 | 0x0001~0xFFFF |
| kvalue | const uint8_t * | 指向要写入的NV项的值的指针 | 非NULL |
| kvalue_length | uint16_t | 写入数据的长度（字节） | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考errcode_t | 写入失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### uapi_nv_write_with_attr <a id="uapi_nv_write_with_attr"></a>

```c
errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr, nv_storage_completed_callback func)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 写入NV数据项，并根据业务需求配置属性及回调函数
- 可配置NV项的存储属性（永久、加密、不可升级等）
- 写入Flash完成后调用回调函数通知
- NV的加密属性和永久属性不能修改，永久属性的kvalue不能修改

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key | uint16_t | NV项的key ID，用于索引 | 0x0001~0xFFFF |
| kvalue | const uint8_t * | 指向要写入的NV项的值的指针 | 非NULL |
| kvalue_length | uint16_t | 写入数据的长度（字节） | 大于0 |
| attr | [nv_key_attr_t](#struct_nv_key_attr_t) * | NV项的属性配置 | 非NULL |
| func | [nv_storage_completed_callback](#typedef_nv_storage_completed_callback) | 写入Flash后的回调函数 | 可为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 写入成功 |
| Other | 其他错误码，参考errcode_t | 写入失败 |

### uapi_nv_read <a id="uapi_nv_read"></a>

```c
errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 读取指定NV数据项的值，默认不获取NV属性值
- 通过key索引读取对应的数据值
- 读取时需指定缓冲区最大长度，实际读取长度通过出参返回

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key | uint16_t | NV项的key ID，用于索引 | 0x0001~0xFFFF |
| kvalue_max_length | uint16_t | 允许存储数据的最大长度（字节） | 大于0 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| kvalue_length | uint16_t * | 实际读取到的数据长度 |
| kvalue | uint8_t * | 指向保存读取数据的buffer指针 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考errcode_t | 读取失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### uapi_nv_read_with_attr <a id="uapi_nv_read_with_attr"></a>

```c
errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue, nv_key_attr_t *attr)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 读取指定NV数据项的值，同时获取key的属性值
- 通过key索引读取对应的数据值及属性
- 属性信息通过出参attr返回

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key | uint16_t | NV项的key ID，用于索引 | 0x0001~0xFFFF |
| kvalue_max_length | uint16_t | 允许存储数据的最大长度（字节） | 大于0 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| kvalue_length | uint16_t * | 实际读取到的数据长度 |
| kvalue | uint8_t * | 指向保存读取数据的buffer指针 |
| attr | [nv_key_attr_t](#struct_nv_key_attr_t) * | 获取到的NV项的属性 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 读取成功 |
| Other | 其他错误码，参考errcode_t | 读取失败 |

### uapi_nv_get_store_status <a id="uapi_nv_get_store_status"></a>

```c
errcode_t uapi_nv_get_store_status(nv_store_status_t *status)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 查询NV存储空间使用情况
- 返回总空间、已使用空间、可回收空间、损坏空间等信息
- 用于监控NV存储空间的健康状态

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | [nv_store_status_t](#struct_nv_store_status_t) * | 指向保存NV状态数据的指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 查询成功 |
| Other | 其他错误码，参考errcode_t | 查询失败 |

### uapi_nv_backup <a id="uapi_nv_backup"></a>

```c
errcode_t uapi_nv_backup(const nv_backup_mode_t *backup_mode)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 执行NV数据备份
- 可按区域标志配置选择需要备份的区域
- 备份区域配置中true代表要备份该区域

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| backup_mode | const [nv_backup_mode_t](#struct_nv_backup_mode_t) * | 指向NV备份区域选择的指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 备份成功 |
| Other | 其他错误码，参考errcode_t | 备份失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_NV_SUPPORT_BACKUP_RESTORE | 特性宏 | 支持NV备份恢复特性 | - |
| CONFIG_NV_SUPPORT_BACKUP_UPGRADE | 特性宏 | 支持NV备份升级特性 | - |

### uapi_nv_set_restore_mode_all <a id="uapi_nv_set_restore_mode_all"></a>

```c
errcode_t uapi_nv_set_restore_mode_all(void)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 设置NV全量恢复标志
- 恢复出厂设置时所有NV区域将被恢复
- 设置后下次恢复操作将恢复全部NV数据

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 设置失败 |

### uapi_nv_set_restore_mode_partitial <a id="uapi_nv_set_restore_mode_partitial"></a>

```c
errcode_t uapi_nv_set_restore_mode_partitial(const nv_restore_mode_t *restore_mode)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 设置NV部分恢复标志
- 可按区域配置选择需要恢复的NV区域
- 恢复区域配置中true代表要恢复该区域

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| restore_mode | const [nv_restore_mode_t](#struct_nv_restore_mode_t) * | 指向NV各区域恢复标志的指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 设置失败 |

### uapi_nv_flush <a id="uapi_nv_flush"></a>

```c
errcode_t uapi_nv_flush(void)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 确保NV数据从RAM同步到Flash
- 仅NV支持异步存储时调用有效
- 用于在关键操作前确保数据持久化

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 同步成功 |
| Other | 其他错误码，参考errcode_t | 同步失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE | 特性宏 | 支持NV异步存储特性 | - |

### uapi_nv_register_change_notify_proc <a id="uapi_nv_register_change_notify_proc"></a>

```c
errcode_t uapi_nv_register_change_notify_proc(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
```

**头文件清单** ``include/middleware/utils/nv.h``

**功能说明**

- 注册NV键值变更通知的回调函数
- 当指定key范围内的NV值发生变更时，回调函数被触发
- 通过min_key和max_key限定监听的key范围

**前置条件**

- NV模块已通过uapi_nv_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| min_key | uint16_t | 注册回调支持的最小key ID | 0x0001~0xFFFF |
| max_key | uint16_t | 注册回调支持的最大key ID | 大于等于min_key |
| func | [nv_changed_notify_func](#typedef_nv_changed_notify_func) | 键值变更通知回调函数 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 注册失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_NV_SUPPORT_CHANGE_NOTIFY | 特性宏 | 支持NV键值变更通知特性 | - |

## Type definitions

### nv_storage_completed_callback <a id="typedef_nv_storage_completed_callback"></a>
```c
typedef void (*nv_storage_completed_callback)(errcode_t result);
```
NV存储回调函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | errcode_t | NV写入flash的结果 | ERRCODE_SUCC或其他错误码 |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/nv.h``

### nv_changed_notify_func <a id="typedef_nv_changed_notify_func"></a>
```c
typedef void (*nv_changed_notify_func)(uint16_t key);
```
NV值更改回调函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| key | uint16_t | 发生变更的NV项的key ID | 有效的NV key ID |

**返回值**

- 无

**头文件清单** ``include/middleware/utils/nv.h``

## Enumerations

### nv_key_id_region_t <a id="enum_nv_key_id_region_t"></a>
```c
typedef enum {
    KEY_ID_REGION0,
    KEY_ID_REGION1,
    KEY_ID_REGION2,
    KEY_ID_REGION3,
    KEY_ID_REGION4,
    KEY_ID_REGION5,
    KEY_ID_REGION6,
    KEY_ID_REGION7,
    KEY_ID_REGION8,
    KEY_ID_REGION9,
    KEY_ID_REGION10,
    KEY_ID_REGION11,
    KEY_ID_REGION12,
    KEY_ID_REGION13,
    KEY_ID_REGION14,
    KEY_ID_REGION15,
    KEY_ID_REGION_MAX_NUM
} nv_key_id_region_t;
```
key_id取值区域枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| KEY_ID_REGION0 | 0 | key_id的取值区域0：[0x0001,0x1000) |
| KEY_ID_REGION1 | 1 | key_id的取值区域1：[0x1000,0x2000) |
| KEY_ID_REGION2 | 2 | key_id的取值区域2：[0x2000,0x3000) |
| KEY_ID_REGION3 | 3 | key_id的取值区域3：[0x3000,0x4000) |
| KEY_ID_REGION4 | 4 | key_id的取值区域4：[0x4000,0x5000) |
| KEY_ID_REGION5 | 5 | key_id的取值区域5：[0x5000,0x6000) |
| KEY_ID_REGION6 | 6 | key_id的取值区域6：[0x6000,0x7000) |
| KEY_ID_REGION7 | 7 | key_id的取值区域7：[0x7000,0x8000) |
| KEY_ID_REGION8 | 8 | key_id的取值区域8：[0x8000,0x9000) |
| KEY_ID_REGION9 | 9 | key_id的取值区域9：[0x9000,0xA000) |
| KEY_ID_REGION10 | 10 | key_id的取值区域10：[0xA000,0xB000) |
| KEY_ID_REGION11 | 11 | key_id的取值区域11：[0xB000,0xC000) |
| KEY_ID_REGION12 | 12 | key_id的取值区域12：[0xC000,0xD000) |
| KEY_ID_REGION13 | 13 | key_id的取值区域13：[0xD000,0xE000) |
| KEY_ID_REGION14 | 14 | key_id的取值区域14：[0xE000,0xF000) |
| KEY_ID_REGION15 | 15 | key_id的取值区域15：[0xF000,0xFFFF] |
| KEY_ID_REGION_MAX_NUM | 16 | key_id的取值区域数量 |

**头文件清单** ``include/middleware/utils/nv.h``

## Structures

### nv_key_attr_t <a id="struct_nv_key_attr_t"></a>
```c
typedef struct {
    bool permanent;
    bool encrypted;
    bool non_upgrade;
    uint8_t reserve;
} nv_key_attr_t;
```
NV属性结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| permanent | bool | 是否为永久NV | true：永久，false：非永久 |
| encrypted | bool | 是否为密文存储 | true：密文，false：明文 |
| non_upgrade | bool | 是否不可升级 | true：不可升级，false：可升级 |
| reserve | uint8_t | 保留字段 | 0 |

**头文件清单** ``include/middleware/utils/nv.h``

### nv_store_status_t <a id="struct_nv_store_status_t"></a>
```c
typedef struct {
    uint32_t total_space;
    uint32_t used_space;
    uint32_t reclaimable_space;
    uint32_t corrupted_space;
    uint32_t max_key_space;
} nv_store_status_t;
```
NV存储状态结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| total_space | uint32_t | 当前核的总NV空间 | 大于0 |
| used_space | uint32_t | 当前核已使用的NV空间 | 大于等于0 |
| reclaimable_space | uint32_t | 当前核的NV可回收空间，擦除后可复用 | 大于等于0 |
| corrupted_space | uint32_t | 当前核已损坏的NV空间，数据异常但有效，擦除后可复用 | 大于等于0 |
| max_key_space | uint32_t | 可存储的最大单NV项空间 | 大于0 |

**头文件清单** ``include/middleware/utils/nv.h``

### nv_restore_mode_t <a id="struct_nv_restore_mode_t"></a>
```c
typedef struct {
    bool region_mode[KEY_ID_REGION_MAX_NUM];
} nv_restore_mode_t;
```
恢复出厂区域配置结构，true代表要恢复。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| region_mode | bool[KEY_ID_REGION_MAX_NUM] | 恢复出厂区域标志配置，数组索引对应[nv_key_id_region_t](#enum_nv_key_id_region_t) | true：恢复该区域，false：不恢复 |

**头文件清单** ``include/middleware/utils/nv.h``

### nv_backup_mode_t <a id="struct_nv_backup_mode_t"></a>
```c
typedef struct {
    bool region_mode[KEY_ID_REGION_MAX_NUM];
} nv_backup_mode_t;
```
备份区域配置结构，true代表要备份。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| region_mode | bool[KEY_ID_REGION_MAX_NUM] | 备份区域标志配置，数组索引对应[nv_key_id_region_t](#enum_nv_key_id_region_t) | true：备份该区域，false：不备份 |

**头文件清单** ``include/middleware/utils/nv.h``



