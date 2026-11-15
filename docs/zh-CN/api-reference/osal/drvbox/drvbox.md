# Drvbox

Drvbox 提供执行域切换机制——在 User Box（usrbox）和 Driver Box（drvbox）之间切换，切换时更新 PMP/MPU 配置实现硬件级隔离。类似轻量 TrustZone，将被隔离的用户代码与驱动代码分离。

仅在 seliteos 上可用，仅两个接口：切换到 drvbox（含权限校验）和切换回 usrbox。

**头文件清单**

```c
#include "kernel/osal/include/drvbox/osal_drvbox.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_drvmgr_switch_to_drvbox](#osal_drvmgr_switch_to_drvbox) | 将执行区域从用户态切换到驱动态，并变更PMP/MPU配置 |
| [osal_drvmgr_switch_to_usrbox](#osal_drvmgr_switch_to_usrbox) | 将执行区域从驱动态切换回用户态，并变更PMP/MPU配置 |

## Functions

### osal_drvmgr_switch_to_drvbox <a id="osal_drvmgr_switch_to_drvbox"></a>

```c
unsigned int osal_drvmgr_switch_to_drvbox(unsigned int drv_id)
```

**头文件清单**

```c
#include "kernel/osal/include/drvbox/osal_drvbox.h"
```

**功能说明**

- 将执行区域从用户态（usr box）切换到驱动态（driver box）
- 切换过程中同步变更PMP/MPU配置
- 仅可在common usr lib中调用

**前置条件**

- 调用上下文为common usr lib，禁止在驱动态或其他上下文中调用
- 当前系统为seliteos

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| drv_id | unsigned int | 用于校验驱动的调用权限 | 有效的驱动标识符 |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| DRV_BOX_INVALID_SWITCH | 无需执行切换流程 | 受限条件下无需切换 |
| DRV_BOX_SWITCH_FAIL | 权限被拒绝 | 驱动权限校验失败 |
| LOS_OK(0) | 区域切换成功 | 切换操作成功完成 |

### osal_drvmgr_switch_to_usrbox <a id="osal_drvmgr_switch_to_usrbox"></a>

```c
void osal_drvmgr_switch_to_usrbox(void)
```

**头文件清单**

```c
#include "kernel/osal/include/drvbox/osal_drvbox.h"
```

**功能说明**

- 将执行区域从驱动态（driver box）切换回用户态（usr box）
- 切换过程中同步变更PMP/MPU配置
- 仅可在common usr lib空间中调用

**前置条件**

- 调用上下文为common usr lib空间，禁止在非用户态上下文中调用
- 当前系统为seliteos


