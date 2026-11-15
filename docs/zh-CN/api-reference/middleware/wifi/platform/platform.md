# Wi-Fi Platform

Wi-Fi Platform 是 Wi-Fi 中间件提供的平台设备控制接口集合，用于对平台级配置进行管理，包括设置平台低功耗开关以及复位单板等基础操作。

**头文件清单**

```c
#include "include/middleware/services/wifi/plat_device.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [plat_set_pm_mode](#plat_set_pm_mode) | 设置平台低功耗开关 |
| [plat_reset_board](#plat_reset_board) | 复位单板 |

## Functions

### plat_set_pm_mode <a id="plat_set_pm_mode"></a>

```c
errcode_t plat_set_pm_mode(int32_t pm_switch)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/plat_device.h"
```

**功能说明**

- 设置平台低功耗模式的开关状态
- 传入参数 1 开启低功耗模式，传入参数 0 关闭低功耗模式
- 调用后由底层执行低功耗功能使能/去使能操作

**前置条件**

- Wi-Fi 模块已完成初始化
- 需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pm_switch | int32_t | 低功耗开关参数 | 1: 开启低功耗模式; 0: 关闭低功耗模式 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 低功耗开关设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RST_SUPPORT | 特性宏 | 支持平台复位与低功耗管理功能 | n |

### plat_reset_board <a id="plat_reset_board"></a>

```c
errcode_t plat_reset_board(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/plat_device.h"
```

**功能说明**

- 复位单板，执行 Wi-Fi 去初始化、设备复位及平台重新初始化操作
- 调用后将依次执行 Wi-Fi 去初始化、板级电源复位、平台重新初始化、Wi-Fi 重新初始化
- 复位过程中若任一步骤失败则返回错误码，不继续执行后续步骤

**前置条件**

- Wi-Fi 模块已完成初始化
- 需在主线程调用，禁止在中断上下文中调用

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 单板复位全部流程执行成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RST_SUPPORT | 特性宏 | 支持平台复位与低功耗管理功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

plat_set_pm_mode 和 plat_reset_board 的返回值类型

