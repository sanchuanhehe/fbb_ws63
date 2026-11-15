# Barrier

CPU 内存屏障（Memory Barrier），确保多核/DMA 并发场景下内存访问顺序的正确性。提供全屏障（mb，读写顺序保证）、读屏障（rmb，只保读序）、写屏障（wmb，只保写序）及其 SMP (Security Manager Protocol) 变体（smp_mb/smp_rmb/smp_wmb），以及编译器屏障（barrier）。

DMA (Direct Memory Access) 缓冲区操作必需——DMA 控制器看到的物理内存可能与 CPU Cache 内容不一致。

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_mb](#osal_mb) | 通用CPU内存屏障，确保屏障前的内存访问在屏障后的内存访问之前完成 |
| [osal_rmb](#osal_rmb) | 读内存屏障，确保屏障前的读操作在屏障后的读操作之前完成 |
| [osal_wmb](#osal_wmb) | 写内存屏障，确保屏障前的写操作在屏障后的写操作之前完成 |
| [osal_smp_mb](#osal_smp_mb) | SMP条件下的通用CPU内存屏障 |
| [osal_smp_rmb](#osal_smp_rmb) | SMP条件下的读内存屏障 |
| [osal_smp_wmb](#osal_smp_wmb) | SMP条件下的写内存屏障 |
| [osal_isb](#osal_isb) | 指令同步屏障，刷新处理器流水线 |
| [osal_dsb](#osal_dsb) | 数据同步屏障，确保所有显式内存访问和维护操作完成 |
| [osal_dmb](#osal_dmb) | 数据内存屏障，确保显式内存访问的观测顺序 |

## Functions

### osal_mb <a id="osal_mb"></a>

```c
void osal_mb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 通用CPU内存屏障，确保屏障之前出现的所有内存访问在屏障之后出现的任何内存访问之前完成
- 适用于需要严格内存访问顺序约束的场景，防止CPU或编译器对内存操作进行重排序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_rmb <a id="osal_rmb"></a>

```c
void osal_rmb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 读内存屏障，确保屏障之前出现的所有读操作在屏障之后出现的任何读操作之前完成
- 仅约束读操作的顺序，不限制写操作的顺序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_wmb <a id="osal_wmb"></a>

```c
void osal_wmb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 写内存屏障，确保屏障之前出现的所有写操作在屏障之后出现的任何写操作之前完成
- 仅约束写操作的顺序，不限制读操作的顺序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_smp_mb <a id="osal_smp_mb"></a>

```c
void osal_smp_mb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- SMP条件下的通用CPU内存屏障，对应osal_mb的SMP版本
- 在多核处理器环境下确保跨核心的内存访问顺序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_smp_rmb <a id="osal_smp_rmb"></a>

```c
void osal_smp_rmb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- SMP条件下的读内存屏障，对应osal_rmb的SMP版本
- 在多核处理器环境下确保跨核心的读操作顺序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_smp_wmb <a id="osal_smp_wmb"></a>

```c
void osal_smp_wmb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- SMP条件下的写内存屏障，对应osal_wmb的SMP版本
- 在多核处理器环境下确保跨核心的写操作顺序
- 仅在Linux系统下支持

**前置条件**

- 运行环境为Linux系统

### osal_isb <a id="osal_isb"></a>

```c
void osal_isb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 指令同步屏障，刷新处理器流水线，确保ISB (Instruction Synchronization Barrier) 之后的指令在ISB指令完成后从缓存或内存中重新获取
- 确保上下文修改操作（如ASID变更、TLB维护操作、分支预测维护操作、CP15寄存器变更）在ISB之前执行的效果对ISB之后获取的指令可见
- 确保ISB之后出现的分支总是使用ISB之后可见的上下文写入分支预测逻辑
- 在Linux和LiteOS (Huawei LiteOS) 系统下均支持

**前置条件**

- 运行环境为Linux或LiteOS系统

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | 特性宏 | 支持LiteOS开源版本指令同步屏障特性 | - |

### osal_dsb <a id="osal_dsb"></a>

```c
void osal_dsb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 数据同步屏障，作为一种特殊的内存屏障，确保在此指令之后程序顺序中的任何指令不会执行，直到此指令完成
- 此指令完成条件：此指令之前的所有显式内存访问完成；此指令之前的所有Cache、分支预测器和TLB维护操作完成
- 在Linux和LiteOS系统下均支持

**前置条件**

- 运行环境为Linux或LiteOS系统

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | 特性宏 | 支持LiteOS开源版本数据同步屏障特性 | - |

### osal_dmb <a id="osal_dmb"></a>

```c
void osal_dmb(void)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_barrier.h"
```

**功能说明**

- 数据内存屏障，确保在程序顺序中DMB指令之前出现的所有显式内存访问在DMB指令之后出现的任何显式内存访问之前被观测到
- 不影响处理器上执行的其他指令的顺序
- 在Linux和LiteOS系统下均支持

**前置条件**

- 运行环境为Linux或LiteOS系统

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | 特性宏 | 支持LiteOS开源版本数据内存屏障特性 | - |


