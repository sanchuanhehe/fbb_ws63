# PMP

PMP (Physical Memory Protection) 是 RISC-V ISA 定义的硬件级内存保护机制。通过配置 PMP 条目为指定物理内存区域设置读/写/执行权限，当访问违反权限规则时硬件立即触发异常。

相比软件内存保护更高效、更安全。典型应用：保护固件代码段、安全密钥等关键内存区域不被意外篡改。

**头文件清单**

```c
#include "include/driver/drv_pmp.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_pmp_config](#uapi_pmp_config) | 初始化并配置PMP模块 |

## Functions

### uapi_pmp_config <a id="uapi_pmp_config"></a>

```c
errcode_t uapi_pmp_config(const pmp_conf_t *config, uint32_t length)
```

**头文件清单**

```c
#include "include/driver/drv_pmp.h"
```

**功能说明**

- 初始化并配置PMP模块，批量设置一组PMP域的地址、大小及访问权限
- 根据配置参数中的地址匹配模式（TOR/NA4/NAPOT），将用户配置转换为硬件寄存器格式并写入PMP寄存器
- 配置完成后执行数据同步屏障（dsb），确保PMP配置生效

**前置条件**

- 调用前PMP模块已通过Kconfig使能（CONFIG_DRIVER_SUPPORT_PMP已开启）
- 入参config指向的内存空间已申请成功，且长度不小于length个pmp_conf_t元素

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | const [pmp_conf_t](#struct_pmp_conf) * | 一组PMP域配置参数，数组首地址 | 不为NULL，指向内存空间已申请成功 |
| length | uint32_t | PMP配置的个数，即config数组元素个数 | ≥1 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 所有PMP域配置均成功写入 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 任一PMP域配置写入失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PMP | 特性宏 | 支持PMP接口功能 | y |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

PMP模块接口的返回值类型，表示接口执行结果

## Enumerations

### pmp_attr_t <a id="enum_pmp_attr"></a>

```c
typedef enum pmp_attr {
    PMP_ATTR_DEVICE_NO_BUFFERABLE = 0,
    PMP_ATTR_DEVICE_BUFFERABLE = 1,
    PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE = 2,
    PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE = 3,
    PMP_ATTR_WRITETHROUGH_NO_ALLOCATE = 4,
    PMP_ATTR_WRITETHROUGH_RALLOCATE = 6,
    PMP_ATTR_WRITEBACK_RALLOCATE = 7,
    PMP_ATTR_WRITEBACK_NO_ALLOCATE = 8,
    PMP_ATTR_WRITETHROUGH_RWALLOCATE = 14,
    PMP_ATTR_WRITEBACK_RWALLOCATE = 15
} pmp_attr_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PMP_ATTR_DEVICE_NO_BUFFERABLE | 0 | 设备属性，不可缓冲 |
| PMP_ATTR_DEVICE_BUFFERABLE | 1 | 设备属性，可缓冲 |
| PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE | 2 | 不可缓冲且不可缓存 |
| PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE | 3 | 不可缓存但可缓冲 |
| PMP_ATTR_WRITETHROUGH_NO_ALLOCATE | 4 | 写穿透，不分配 |
| PMP_ATTR_WRITETHROUGH_RALLOCATE | 6 | 写穿透，读分配 |
| PMP_ATTR_WRITEBACK_RALLOCATE | 7 | 写回，读分配 |
| PMP_ATTR_WRITEBACK_NO_ALLOCATE | 8 | 写回，不分配 |
| PMP_ATTR_WRITETHROUGH_RWALLOCATE | 14 | 写穿透，读写分配 |
| PMP_ATTR_WRITEBACK_RWALLOCATE | 15 | 写回，读写分配 |

### addr_match_t <a id="enum_addr_match"></a>

```c
typedef enum addr_match {
    PMPCFG_ADDR_MATCH_OFF = 0x0,
    PMPCFG_ADDR_MATCH_TOR = 0x1,
    PMPCFG_ADDR_MATCH_NA4 = 0x2,
    PMPCFG_ADDR_MATCH_NAPOT = 0x3
} addr_match_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PMPCFG_ADDR_MATCH_OFF | 0x0 | 地址匹配关闭，该PMP域不生效 |
| PMPCFG_ADDR_MATCH_TOR | 0x1 | Top of Range模式，地址为尾地址 |
| PMPCFG_ADDR_MATCH_NA4 | 0x2 | Naturally Aligned 4-byte模式 |
| PMPCFG_ADDR_MATCH_NAPOT | 0x3 | Naturally Aligned Power-of-Two模式，地址为首地址 |

### rwx_permission_t <a id="enum_rwx_permission"></a>

```c
typedef enum rwx_permission {
    PMPCFG_NO_ACCESS = 0x0,
    PMPCFG_READ_ONLY_NEXECUTE = 0x1,
    PMPCFG_RW_NEXECUTE = 0x3,
    PMPCFG_READ_ONLY_EXECUTE = 0x5,
    PMPCFG_RW_EXECUTE = 0x7
} rwx_permission_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PMPCFG_NO_ACCESS | 0x0 | 无访问权限 |
| PMPCFG_READ_ONLY_NEXECUTE | 0x1 | 只读，不可执行 |
| PMPCFG_RW_NEXECUTE | 0x3 | 可读写，不可执行 |
| PMPCFG_READ_ONLY_EXECUTE | 0x5 | 只读，可执行 |
| PMPCFG_RW_EXECUTE | 0x7 | 可读写，可执行 |

## Structures

### pmpx_config_t <a id="struct_pmpx_config"></a>

```c
typedef struct pmpx_config {
    rwx_permission_t rwx_permission;  /*!< Read-Write-Execute permission. */
    addr_match_t addr_match;          /*!< Address matching method. */
    bool lock;                        /*!< Lock bit configuration. */
    pmp_attr_t pmp_attr;              /*!< Read-Write mode configuration. */
} pmpx_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rwx_permission | [rwx_permission_t](#enum_rwx_permission) | 读写执行权限 |
| addr_match | [addr_match_t](#enum_addr_match) | 地址匹配方式 |
| lock | bool | Lock位配置，置位后该PMP域配置不可修改直至复位 |
| pmp_attr | [pmp_attr_t](#enum_pmp_attr) | 读写方式配置 |

### pmp_conf_t <a id="struct_pmp_conf"></a>

```c
typedef struct pmp_conf {
    uint32_t idx;                    /*!< Region number. */
    uint32_t addr;                   /*!< Base address, First Address in NAPOT mode and End Address in TOR mode. */
    uint32_t size;                   /*!< Used in NAPOT mode. Size of memory unit. */
    pmpx_config_t conf;              /*!< PMP region configuration parameters. */
} pmp_conf_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| idx | uint32_t | 域编号 |
| addr | uint32_t | 基地址，NAPOT/NA4模式为首地址，TOR模式为尾地址 |
| size | uint32_t | 在NAPOT (Naturally Aligned Power-Of-Two) 模式使用，内存单元的大小 |
| conf | [pmpx_config_t](#struct_pmpx_config) | PMP域配置参数 |


