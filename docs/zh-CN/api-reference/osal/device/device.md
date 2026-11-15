# Device

Linux 风格的设备驱动模型抽象。定义设备结构体（`osal_dev`）、文件操作接口（open/read/write/ioctl/poll/mmap 等）和电源管理接口。

提供设备注册/注销、异步通知、poll/wait 等待机制和低功耗管理（冻结/恢复）功能。是构建字符设备驱动的基础框架。

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_pm_lowpower_enter](#osal_pm_lowpower_enter) | 调用所有设备的低功耗进入回调函数 |
| [osal_pm_lowpower_exit](#osal_pm_lowpower_exit) | 调用所有设备的低功耗退出回调函数 |
| [osal_dev_create](#osal_dev_create) | 创建设备并申请内存，返回设备指针 |
| [osal_dev_destroy](#osal_dev_destroy) | 释放由osal_dev_create创建的设备内存 |
| [osal_dev_register](#osal_dev_register) | 注册设备到系统 |
| [osal_dev_unregister](#osal_dev_unregister) | 从系统注销设备 |
| [osal_device_set_async](#osal_device_set_async) | 设置设备异步唤醒 |
| [osal_poll_wait](#osal_poll_wait) | 将当前进程添加到指定等待队列 |
| [osal_notify_poll](#osal_notify_poll) | 唤醒指定等待队列中的进程 |
| [osal_remap_pfn_range](#osal_remap_pfn_range) | 将内核物理内存页重映射到用户空间 |
| [osal_try_to_freeze](#osal_try_to_freeze) | 尝试冻结当前任务 |
| [osal_set_freezable](#osal_set_freezable) | 设置当前任务可冻结并尝试冻结 |
| [osal_kobject_uevent_env](#osal_kobject_uevent_env) | 发送携带环境数据的uevent事件 |
| [osal_fasync_helper](#osal_fasync_helper) | 设置字符设备驱动的fasync队列 |
| [osal_fasync_notify](#osal_fasync_notify) | 发送异步通知信号 |
| [osal_pgprot_noncached](#osal_pgprot_noncached) | 获取非缓存页保护属性 |
| [osal_pgprot_cached](#osal_pgprot_cached) | 获取缓存页保护属性 |
| [osal_pgprot_writecombine](#osal_pgprot_writecombine) | 获取写合并页保护属性 |
| [osal_smccc_smc](#osal_smccc_smc) | 执行SMC (Secure Monitor Call) 调用 |
| [osal_opendev](#osal_opendev) | 打开设备，返回设备文件描述符 |
| [osal_closedev](#osal_closedev) | 关闭设备文件描述符 |
| [osal_readdev](#osal_readdev) | 从设备读取数据 |
| [osal_writedev](#osal_writedev) | 向设备写入数据 |
| [osal_ioctldev](#osal_ioctldev) | 对设备执行IOCTL操作 |
| [osal_init](#osal_init) | OSAL (Operating System Abstraction Layer) 用户空间初始化 |
| [osal_exit](#osal_exit) | OSAL用户空间退出 |

## Functions

### osal_pm_lowpower_enter <a id="osal_pm_lowpower_enter"></a>

```c
void osal_pm_lowpower_enter(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 调用所有已注册设备的低功耗进入回调函数（pm_lowpower_enter）
- 用于系统进入低功耗模式前，通知各设备执行低功耗进入操作
- 仅支持 Linux 系统

**前置条件**

- 已通过 osal_dev_register 注册设备，且设备的 pmops 中已设置 pm_lowpower_enter 回调
- 系统即将进入低功耗模式

### osal_pm_lowpower_exit <a id="osal_pm_lowpower_exit"></a>

```c
void osal_pm_lowpower_exit(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 调用所有已注册设备的低功耗退出回调函数（pm_lowpower_exit）
- 用于系统退出低功耗模式后，通知各设备执行低功耗退出操作
- 仅支持 Linux 系统

**前置条件**

- 已通过 osal_dev_register 注册设备，且设备的 pmops 中已设置 pm_lowpower_exit 回调
- 系统已从低功耗模式退出

### osal_dev_create <a id="osal_dev_create"></a>

```c
osal_dev * osal_dev_create(const char *name)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 申请 osal_dev 类型内存并返回设备指针
- 创建的设备必须通过 osal_dev_destroy 释放
- 支持 Linux、LiteOS (Huawei LiteOS)、FreeRTOS (Free Real-Time Operating System) 系统

**前置条件**

- 入参 name 不为 NULL，且字符串长度不超过 OSAL_DEV_NAME_LEN(32)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 设备名称 | 非 NULL，长度 ≤ OSAL_DEV_NAME_LEN(32) |

**返回值**

- 返回类型：osal_dev *
- 返回申请到的 [osal_dev](#struct_osal_dev_) 设备指针；申请失败返回 NULL

### osal_dev_destroy <a id="osal_dev_destroy"></a>

```c
int osal_dev_destroy(osal_dev *dev)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 释放由 osal_dev_create 创建的设备内存
- 调用后需将 dev 指针置 NULL，避免悬空指针
- 支持 Linux、LiteOS、FreeRTOS 系统

**前置条件**

- 入参 dev 为 osal_dev_create 的返回值，不为 NULL
- 调用后调用方应将 dev 置 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev | osal_dev * | osal_dev_create 创建的设备指针 | 非 NULL，须为 osal_dev_create 返回值 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 释放成功 | 设备内存释放成功 |
| OSAL_FAILURE(-1) | 释放失败 | 设备内存释放失败 |

### osal_dev_register <a id="osal_dev_register"></a>

```c
int osal_dev_register(osal_dev *dev)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 将设备注册到系统，使设备可被用户态程序访问
- 注册前需通过 osal_dev_create 创建设备并设置 fops 和 pmops
- 支持 Linux、LiteOS、FreeRTOS 系统

**前置条件**

- 入参 dev 为 osal_dev_create 的返回值，不为 NULL
- dev 的 fops 或 pmops 已正确设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev | osal_dev * | osal_dev_create 创建的设备指针 | 非 NULL，须为 osal_dev_create 返回值 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 注册成功 | 设备注册到系统成功 |
| OSAL_FAILURE(-1) | 注册失败 | 设备注册到系统失败 |

### osal_dev_unregister <a id="osal_dev_unregister"></a>

```c
void osal_dev_unregister(osal_dev *dev)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 从系统注销已注册的设备
- 注销后设备不可再被用户态程序访问
- 支持 Linux、LiteOS、FreeRTOS 系统

**前置条件**

- 入参 dev 为 osal_dev_create 的返回值，且已通过 osal_dev_register 注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev | osal_dev * | osal_dev_create 创建的设备指针 | 非 NULL，须为已注册设备 |

### osal_device_set_async <a id="osal_device_set_async"></a>

```c
void osal_device_set_async(unsigned int minor)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 设置指定次设备号的设备为异步唤醒模式
- 设备异步唤醒后可通过信号通知用户态
- 仅支持 Linux 系统

**前置条件**

- 对应 minor 的设备已通过 osal_dev_register 注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| minor | unsigned int | 设备次设备号 | 已注册设备的有效次设备号 |

### osal_poll_wait <a id="osal_poll_wait"></a>

```c
void osal_poll_wait(osal_poll *table, osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 将当前进程添加到 wait 参数指定的等待队列中
- 用于设备驱动的 poll 接口实现，配合 osal_notify_poll 使用
- 支持 Linux、LiteOS 系统

**前置条件**

- table 不为 NULL，wait 不为 NULL
- 在设备 fops 的 poll 回调函数中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| table | [osal_poll](#typedef_osal_poll) * | poll 表指针 | 非 NULL |
| wait | [osal_wait](../schedule/wait/wait.md#struct_osal_wait) * | 等待队列指针 | 非 NULL |

### osal_notify_poll <a id="osal_notify_poll"></a>

```c
void osal_notify_poll(osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 唤醒 wait 参数指定的等待队列中的进程
- 与 osal_poll_wait 配对使用，在设备数据就绪时调用
- 仅支持 LiteOS 系统

**前置条件**

- wait 不为 NULL，且已通过 osal_poll_wait 添加等待队列
- 设备数据已就绪，需唤醒等待进程

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](../schedule/wait/wait.md#struct_osal_wait) * | 等待队列指针 | 非 NULL |

### osal_remap_pfn_range <a id="osal_remap_pfn_range"></a>

```c
int osal_remap_pfn_range(osal_vm *vm, unsigned long addr, unsigned long pfn, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 将内核物理内存页帧重映射到用户空间虚拟地址
- 用于设备驱动的 mmap 接口实现，使用户态可直接访问设备物理内存
- 支持 Linux、LiteOS 系统

**前置条件**

- vm 不为 NULL
- 在设备 fops 的 mmap 回调函数中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| vm | [osal_vm](#typedef_osal_vm) * | 虚拟内存管理结构指针 | 非 NULL |
| addr | unsigned long | 用户空间虚拟地址 | 页对齐地址 |
| pfn | unsigned long | 物理页帧号 | 有效物理页帧号 |
| size | unsigned long | 映射大小（字节） | > 0，页对齐 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 映射成功 | 内核物理内存页重映射到用户空间成功 |
| Other | 其他错误码，参考OSAL错误码 | 映射失败 |

### osal_try_to_freeze <a id="osal_try_to_freeze"></a>

```c
int osal_try_to_freeze(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 尝试冻结当前任务
- 用于系统休眠流程中，判断当前任务是否可被冻结
- 支持 Linux、LiteOS 系统

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true(非0) | 冻结成功 | 当前任务已成功冻结 |
| false(0) | 冻结失败 | 当前任务未冻结 |

### osal_set_freezable <a id="osal_set_freezable"></a>

```c
int osal_set_freezable(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 设置当前任务为可冻结状态，并尝试冻结当前任务
- 标记任务可冻结后，系统休眠时可冻结该任务
- 支持 Linux、LiteOS 系统

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true(非0) | 冻结成功 | 当前任务已设置为可冻结并成功冻结 |
| false(0) | 冻结失败 | 当前任务未冻结 |

### osal_kobject_uevent_env <a id="osal_kobject_uevent_env"></a>

```c
int osal_kobject_uevent_env(osal_dev *dev, osal_kobject_action action, char *envp[])
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 发送携带环境数据的 uevent 事件到用户空间
- 用于设备热插拔等场景通知用户态
- 支持 Linux、LiteOS 系统

**前置条件**

- dev 不为 NULL，且已通过 osal_dev_register 注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev | [osal_dev](#struct_osal_dev_) * | 设备指针 | 非 NULL，已注册设备 |
| action | [osal_kobject_action](#enum_osal_kobject_action_) | 事件动作类型 | [OSAL_KOBJ_ADD, OSAL_KOBJ_REMOVE, OSAL_KOBJ_CHANGE, OSAL_KOBJ_MOVE, OSAL_KOBJ_ONLINE, OSAL_KOBJ_OFFLINE, OSAL_KOBJ_BIND, OSAL_KOBJ_UNBIND] |
| envp | char *[] | 环境变量字符串数组 | 以 NULL 结尾的字符串数组 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 发送成功 | kobject_uevent_env 执行成功 |
| Other | 其他错误码 | kobject_uevent_env 执行失败 |

### osal_fasync_helper <a id="osal_fasync_helper"></a>

```c
int osal_fasync_helper(int fd, void *filp, int mode, void **fapp)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 设置字符设备驱动的 fasync 异步通知队列
- 在 fasync 回调中调用，用于管理异步通知的注册与注销
- 仅支持 Linux 系统

**前置条件**

- fapp 不为 NULL
- 在设备 fops 的 fasync 回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fd | int | 文件描述符 | 有效文件描述符 |
| filp | void * | 文件指针 | 非 NULL |
| mode | int | fasync 模式标志 | FASYNC 标志位 |
| fapp | void ** | fasync 队列指针地址 | 非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 负值 | 错误 | 操作失败 |
| 0 | 无变化 | 未增删条目 |
| 正值 | 成功 | 增加或删除了条目 |

### osal_fasync_notify <a id="osal_fasync_notify"></a>

```c
void osal_fasync_notify(void **fapp, int sig, int band)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 通过 fasync 队列向用户态发送异步通知信号（SIGIO (Signal I/O)）
- 用户态收到 SIGIO 信号后触发对应处理函数
- 仅支持 Linux 系统

**前置条件**

- fapp 不为 NULL，且已通过 osal_fasync_helper 初始化
- fasync 队列已注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fapp | void ** | fasync 队列指针地址 | 非 NULL，已初始化 |
| sig | int | 信号编号 | SIGIO 等有效信号 |
| band | int | 事件带宽 | POLL_IN, POLL_OUT 等 |

### osal_pgprot_noncached <a id="osal_pgprot_noncached"></a>

```c
void osal_pgprot_noncached(osal_vm *vm)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 获取非缓存（nocache）页保护属性
- 设置虚拟内存区域为非缓存映射，适用于设备寄存器映射
- 仅支持 Linux 系统

**前置条件**

- vm 不为 NULL
- 在设备 mmap 回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| vm | [osal_vm](#typedef_osal_vm) * | 虚拟内存管理结构指针 | 非 NULL |

### osal_pgprot_cached <a id="osal_pgprot_cached"></a>

```c
void osal_pgprot_cached(osal_vm *vm)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 获取缓存（cache）页保护属性
- 设置虚拟内存区域为缓存映射，适用于普通内存映射
- 仅支持 Linux 系统

**前置条件**

- vm 不为 NULL
- 在设备 mmap 回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| vm | [osal_vm](#typedef_osal_vm) * | 虚拟内存管理结构指针 | 非 NULL |

### osal_pgprot_writecombine <a id="osal_pgprot_writecombine"></a>

```c
void osal_pgprot_writecombine(osal_vm *vm)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 获取写合并（writecombine）页保护属性
- 设置虚拟内存区域为写合并映射，适用于帧缓冲等场景
- 仅支持 Linux 系统

**前置条件**

- vm 不为 NULL
- 在设备 mmap 回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| vm | [osal_vm](#typedef_osal_vm) * | 虚拟内存管理结构指针 | 非 NULL |

### osal_smccc_smc <a id="osal_smccc_smc"></a>

```c
void osal_smccc_smc(const osal_smccc_info *info, osal_smccc_res *res)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 按照 SMC Calling Convention 执行 SMC 调用
- 用于从非安全世界调用安全世界（TrustZone）的服务
- 仅支持 Linux 系统

**前置条件**

- info 不为 NULL，res 不为 NULL
- SMC 调用参数已正确设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| info | const [osal_smccc_info](#struct_osal_smccc_info) * | SMC 调用输入参数 | 非 NULL |
| res | [osal_smccc_res](#struct_osal_smccc_res) * | SMC 调用返回结果 | 非 NULL |

### osal_opendev <a id="osal_opendev"></a>

```c
int osal_opendev(const char *path, int flag, ...)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 打开设备，调用设备驱动的 open 回调函数
- 返回设备文件描述符，用于后续 read/write/ioctl 操作
- 仅支持 Linux 用户态

**前置条件**

- path 不为 NULL，指向有效的设备路径
- 设备已通过 osal_dev_register 注册到系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| path | const char * | 设备路径 | 非 NULL，有效设备路径 |
| flag | int | 打开标志 | O_RDONLY, O_WRONLY, O_RDWR 等 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 打开成功 | 设备文件描述符 |
| OSAL_FAILURE(-1) | 打开失败 | 设备打开失败 |

### osal_closedev <a id="osal_closedev"></a>

```c
int osal_closedev(int fd)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 关闭设备文件描述符，调用设备驱动的 release 回调函数
- 释放设备资源
- 仅支持 Linux 用户态

**前置条件**

- fd 为 osal_opendev 返回的有效文件描述符

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fd | int | 设备文件描述符 | osal_opendev 返回的有效 fd |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 关闭成功 | 设备关闭成功 |
| OSAL_FAILURE(-1) | 关闭失败 | 设备关闭失败 |

### osal_readdev <a id="osal_readdev"></a>

```c
int osal_readdev(int fd, void *buf, unsigned long count)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 从设备读取数据，调用设备驱动的 read 回调函数
- 返回实际读取的字节数
- 仅支持 Linux 用户态

**前置条件**

- fd 为 osal_opendev 返回的有效文件描述符
- buf 不为 NULL，且指向的内存空间不小于 count

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fd | int | 设备文件描述符 | osal_opendev 返回的有效 fd |
| buf | void * | 读取数据缓冲区 | 非 NULL，空间 ≥ count |
| count | unsigned long | 请求读取字节数 | > 0 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 正值 | 读取字节数 | 读取成功，返回实际读取字节数 |
| -1 | 读取失败 | fd 不正确或读取失败 |

### osal_writedev <a id="osal_writedev"></a>

```c
int osal_writedev(int fd, const void *buf, unsigned long count)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 向设备写入数据，调用设备驱动的 write 回调函数
- 返回实际写入的字节数
- 仅支持 Linux 用户态

**前置条件**

- fd 为 osal_opendev 返回的有效文件描述符
- buf 不为 NULL，且指向的内存空间不小于 count

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fd | int | 设备文件描述符 | osal_opendev 返回的有效 fd |
| buf | const void * | 写入数据缓冲区 | 非 NULL，空间 ≥ count |
| count | unsigned long | 请求写入字节数 | > 0 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 正值 | 写入字节数 | 写入成功，返回实际写入字节数 |
| -1 | 写入失败 | fd 不正确或写入失败 |

### osal_ioctldev <a id="osal_ioctldev"></a>

```c
int osal_ioctldev(int fd, unsigned int cmd, ...)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- 对设备执行 IOCTL 操作，调用设备驱动的 cmd_list 中对应 handler
- 用于设备特定控制命令
- 仅支持 Linux 用户态

**前置条件**

- fd 为 osal_opendev 返回的有效文件描述符
- cmd 为设备驱动支持的合法命令字

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fd | int | 设备文件描述符 | osal_opendev 返回的有效 fd |
| cmd | unsigned int | IOCTL 命令字 | 设备驱动支持的合法命令 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负值 | 操作成功 | handler 返回值 |
| -1 | 操作失败 | fd 不正确或 handler 执行失败 |

### osal_init <a id="osal_init"></a>

```c
int osal_init(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- OSAL Linux 用户态初始化
- 初始化用户态 OSAL 设备访问框架
- 仅支持 Linux 用户态

**前置条件**

- 尚未调用 osal_init 进行初始化
- 系统处于用户态运行环境

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | OSAL 用户态初始化成功 |
| OSAL_FAILURE(-1) | 初始化失败 | OSAL 用户态初始化失败 |

### osal_exit <a id="osal_exit"></a>

```c
void osal_exit(void)
```

**头文件清单**

```c
#include "kernel/osal/include/device/osal_device.h"
```

**功能说明**

- OSAL Linux 用户态退出，释放资源
- 与 osal_init 配对使用
- 仅支持 Linux 用户态

**前置条件**

- 已通过 osal_init 完成初始化

## Type definitions

### osal_poll <a id="typedef_osal_poll"></a>

```c
typedef struct osal_poll_ {
    void *poll_table;
    void *data;
} osal_poll;
```

**使用说明**

在 osal_poll_wait 接口中作为入参，传递 poll 表和私有数据

### osal_ioctl_cmd <a id="typedef_osal_ioctl_cmd"></a>

```c
typedef struct osal_ioctl_cmd_ {
    unsigned int cmd;
    int (*handler)(unsigned int cmd, void *arg, void *private_data);
} osal_ioctl_cmd;
```

**使用说明**

在 osal_dev 的 fops->cmd_list 中定义 IOCTL 命令与处理函数映射，由 osal_ioctldev 间接调用

### osal_vm <a id="typedef_osal_vm"></a>

```c
typedef struct osal_vm_ {
    void *vm;
} osal_vm;
```

**使用说明**

在 osal_remap_pfn_range、osal_pgprot_noncached、osal_pgprot_cached、osal_pgprot_writecombine 接口中作为入参，封装底层虚拟内存管理结构

### osal_fileops <a id="typedef_osal_fileops"></a>

```c
typedef struct osal_fileops_ {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    unsigned int (*poll)(osal_poll *osal_poll, void *private_data);
    int (*mmap)(osal_vm *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
    int (*fasync)(int fd, void *filp, int mode);
    osal_ioctl_cmd *cmd_list;
    unsigned int cmd_cnt;
} osal_fileops;
```

**使用说明**

在 osal_dev 的 fops 成员中设置设备文件操作回调，由 osal_opendev/closedev/readdev/writedev/ioctldev 间接调用

### osal_pmops <a id="typedef_osal_pmops"></a>

```c
typedef struct osal_pmops_ {
    int (*pm_suspend)(void *private_data);
    int (*pm_resume_early)(void *private_data);
    int (*pm_resume)(void *private_data);
    int (*pm_lowpower_enter)(void *private_data);
    int (*pm_lowpower_exit)(void *private_data);
    int (*pm_poweroff)(void *private_data);
    void *private_data;
} osal_pmops;
```

**使用说明**

在 osal_dev 的 pmops 成员中设置设备电源管理回调，由 osal_pm_lowpower_enter/osal_pm_lowpower_exit 间接调用

## Enumerations

### osal_kobject_action <a id="enum_osal_kobject_action_"></a>

```c
typedef enum osal_kobject_action_ {
    OSAL_KOBJ_ADD,
    OSAL_KOBJ_REMOVE,
    OSAL_KOBJ_CHANGE,
    OSAL_KOBJ_MOVE,
    OSAL_KOBJ_ONLINE,
    OSAL_KOBJ_OFFLINE,
    OSAL_KOBJ_BIND,
    OSAL_KOBJ_UNBIND,
    OSAL_KOBJ_MAX
} osal_kobject_action;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| OSAL_KOBJ_ADD | 0 | 设备添加事件 |
| OSAL_KOBJ_REMOVE | 1 | 设备移除事件 |
| OSAL_KOBJ_CHANGE | 2 | 设备变更事件 |
| OSAL_KOBJ_MOVE | 3 | 设备移动事件 |
| OSAL_KOBJ_ONLINE | 4 | 设备上线事件 |
| OSAL_KOBJ_OFFLINE | 5 | 设备下线事件 |
| OSAL_KOBJ_BIND | 6 | 设备绑定事件 |
| OSAL_KOBJ_UNBIND | 7 | 设备解绑事件 |
| OSAL_KOBJ_MAX | 8 | 枚举边界值 |

## Structures

### osal_dev <a id="struct_osal_dev_"></a>

```c
#define OSAL_DEV_NAME_LEN 32
typedef struct osal_dev_ {
    char name[OSAL_DEV_NAME_LEN];
    int minor;
    unsigned int parent_minor;
    osal_fileops *fops;
    osal_pmops *pmops;
    void *dev;
    void *owner;
} osal_dev;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| name | char[OSAL_DEV_NAME_LEN] | 设备名称，最大长度32 |
| minor | int | 设备次设备号 |
| parent_minor | unsigned int | 父设备次设备号 |
| fops | [osal_fileops](#typedef_osal_fileops) * | 设备文件操作回调指针 |
| pmops | [osal_pmops](#typedef_osal_pmops) * | 设备电源管理操作回调指针 |
| dev | void * | 底层设备指针 |
| owner | void * | 模块所有者指针 |

### osal_smccc_info <a id="struct_osal_smccc_info"></a>

```c
typedef struct {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
} osal_smccc_info;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| a0 | unsigned long | SMC 调用参数0 |
| a1 | unsigned long | SMC 调用参数1 |
| a2 | unsigned long | SMC 调用参数2 |
| a3 | unsigned long | SMC 调用参数3 |
| a4 | unsigned long | SMC 调用参数4 |
| a5 | unsigned long | SMC 调用参数5 |
| a6 | unsigned long | SMC 调用参数6 |
| a7 | unsigned long | SMC 调用参数7 |

### osal_smccc_res <a id="struct_osal_smccc_res"></a>

```c
typedef struct {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
} osal_smccc_res;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| a0 | unsigned long | SMC 调用返回值0 |
| a1 | unsigned long | SMC 调用返回值1 |
| a2 | unsigned long | SMC 调用返回值2 |
| a3 | unsigned long | SMC 调用返回值3 |


