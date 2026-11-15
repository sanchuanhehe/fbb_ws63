# 分区管理

**头文件清单**
```c
#include "include/middleware/utils/partition.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_partition_init](#uapi_partition_init) | 初始化分区管理模块 |
| [uapi_partition_get_info](#uapi_partition_get_info) | 获取指定分区信息 |

## Functions

### uapi_partition_init <a id="uapi_partition_init"></a>

```c
errcode_t uapi_partition_init(void)
```

**头文件清单** ``include/middleware/utils/partition.h``

**功能说明**

- 初始化分区管理模块
- 完成分区表的加载和解析
- 在使用分区查询功能之前必须调用

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 初始化失败 |

### uapi_partition_get_info <a id="uapi_partition_get_info"></a>

```c
errcode_t uapi_partition_get_info(partition_ids_t partition_id, partition_information_t *info)
```

**头文件清单** ``include/middleware/utils/partition.h``

**功能说明**

- 获取指定分区ID的分区信息
- 分区信息包括存储位置类型、起始地址、字节长度或文件路径
- 支持内存地址和文件系统两种存储位置类型

**前置条件**

- 分区模块已通过uapi_partition_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| partition_id | [partition_ids_t](#enum_partition_ids_t) | 指定的分区ID | PARTITION_SSB(0x0) / PARTITION_FLASH_BOOT_IMAGE(0x1) / PARTITION_FLASH_BOOT_IMAGE_BACKUP(0x2) / PARTITION_FLASH_ROOT_PUBLIC_KEYS_AREA(0x3) / PARTITION_CUSTOMER_FACTORY(0x8) / PARTITION_NV_DATA_BACKUP(0x9) / PARTITION_NV_DATA(0x10) / PARTITION_CRASH_INFO(0x11) / PARTITION_APP_IMAGE(0x20) / PARTITION_FOTA_DATA(0x21) / PARTITION_RESERVE0(0x30) / PARTITION_HILINK_IMAGE(0x31) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| info | [partition_information_t](#struct_partition_information_t) * | 保存获取到的分区信息 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 获取失败 |

**参考案例**

- [sfc_perf_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc_perf/sfc_perf_demo.c)


## Associations

### partition_information_t.part_info <a id="assoc_partition_information_t_part_info"></a>
```c
union {
    struct {
        uint32_t addr;
        uint32_t size;
    } addr_info;
    char *file_path;
} part_info;
```
保存分区信息（地址或文件路径）的共同体。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr_info | struct | 分区的地址信息 | 当type为PARTITION_BY_ADDRESS时有效 |
| addr_info.addr | uint32_t | 分区的起始地址 | 有效的内存地址 |
| addr_info.size | uint32_t | 分区的字节长度 | 大于0 |
| file_path | char * | 分区所在文件路径 | 当type为PARTITION_BY_PATH时有效，非NULL |

**头文件清单** ``include/middleware/utils/partition.h``

## Enumerations

### partition_type_t <a id="enum_partition_type_t"></a>
```c
typedef enum partition_type {
    PARTITION_BY_ADDRESS,
    PARTITION_BY_PATH,
    PARTITION_TYPE_COUNT
} partition_type_t;
```
分区位置类型枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| PARTITION_BY_ADDRESS | 0 | 分区存储在内存地址上 |
| PARTITION_BY_PATH | 1 | 分区存储在文件系统上 |
| PARTITION_TYPE_COUNT | 2 | 类型数量，不作为有效类型使用 |

**头文件清单** ``include/middleware/utils/partition.h``

### partition_ids_t <a id="enum_partition_ids_t"></a>
```c
typedef enum {
    PARTITION_SSB = 0x0,
    PARTITION_FLASH_BOOT_IMAGE = 0x1,
    PARTITION_FLASH_BOOT_IMAGE_BACKUP = 0x2,
    PARTITION_FLASH_ROOT_PUBLIC_KEYS_AREA = 0x3,
    PARTITION_CUSTOMER_FACTORY = 0x8,
    PARTITION_NV_DATA_BACKUP = 0x9,
    PARTITION_NV_DATA = 0x10,
    PARTITION_CRASH_INFO = 0x11,
    PARTITION_APP_IMAGE = 0x20,
    PARTITION_FOTA_DATA = 0x21,
    PARTITION_RESERVE0 = 0x30,
    PARTITION_HILINK_IMAGE = 0x31,
    PARTITION_RESERVE2 = 0x32,
    PARTITION_RESERVE3 = 0x33,
    PARTITION_RESERVE4 = 0x34,
    PARTITION_RESERVE5 = 0x35,
    PARTITION_MAX_CNT = 16
} partition_ids_t;
```
分区ID枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| PARTITION_SSB | 0x0 | SSB (Secure Secondary Boot) 分区 |
| PARTITION_FLASH_BOOT_IMAGE | 0x1 | Flash Boot镜像分区 |
| PARTITION_FLASH_BOOT_IMAGE_BACKUP | 0x2 | Flash Boot镜像备份分区 |
| PARTITION_FLASH_ROOT_PUBLIC_KEYS_AREA | 0x3 | 根公钥区域分区 |
| PARTITION_CUSTOMER_FACTORY | 0x8 | 客户工厂分区 |
| PARTITION_NV_DATA_BACKUP | 0x9 | NV (Non-Volatile) 数据备份分区 |
| PARTITION_NV_DATA | 0x10 | NV数据分区 |
| PARTITION_CRASH_INFO | 0x11 | 崩溃信息分区 |
| PARTITION_APP_IMAGE | 0x20 | 应用镜像分区 |
| PARTITION_FOTA_DATA | 0x21 | FOTA (Firmware Over-The-Air) 数据分区 |
| PARTITION_RESERVE0 | 0x30 | 保留分区0 |
| PARTITION_HILINK_IMAGE | 0x31 | HiLink镜像分区 |
| PARTITION_RESERVE2 | 0x32 | 保留分区2 |
| PARTITION_RESERVE3 | 0x33 | 保留分区3 |
| PARTITION_RESERVE4 | 0x34 | 保留分区4 |
| PARTITION_RESERVE5 | 0x35 | 保留分区5 |
| PARTITION_MAX_CNT | 16 | 分区数量，不作为分区ID有效性的判断 |

**头文件清单** ``include/middleware/utils/partition_resource_id.h``

## Structures

### partition_information_t <a id="struct_partition_information_t"></a>
```c
typedef struct partition_information {
    partition_type_t type;
    union {
        struct {
            uint32_t addr;
            uint32_t size;
        } addr_info;
        char *file_path;
    } part_info;
} partition_information_t;
```
分区信息结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| type | [partition_type_t](#enum_partition_type_t) | 分区存储位置类型 | PARTITION_BY_ADDRESS或PARTITION_BY_PATH |
| part_info | union | 保存分区信息的联合体，参考 [partition_information_t.part_info](#assoc_partition_information_t_part_info) | 根据type选择对应成员 |

**头文件清单** ``include/middleware/utils/partition.h``


