# OSAL 堆内存分配

> 堆内存 — 运行时分配 — OSAL（Operating System Abstraction Layer，操作系统抽象层）动态内存接口

> 应用统一使用 OSAL 内存接口，不直接依赖 LiteOS (Huawei LiteOS) 私有的 `LOS_MemBox`。需要固定块内存池时，应在产品公共组件中封装并评估，避免业务代码绑定具体内核。

> 前置阅读：[OSAL 任务调度](../task/task-concurrency.md)

## 学习目标

- 理解堆内存的适用条件与典型场景
- 掌握 OSAL 提供的六种内存分配接口及其释放方式
- 学会检查 `NULL`，安全处理内存分配失败

## 使用场景

固定大小、长期存在的数据应优先使用全局变量、静态变量或栈内存。当内存大小或生命周期只能在程序运行时确定时，可以使用堆内存，例如：

- 接收长度不固定的协议数据
- 根据设备数量创建动态列表
- 临时申请文件、升级包或数据处理缓冲区
- 为硬件数据结构申请指定对齐的内存
- 某项功能启用时才创建对应对象

## 堆内存概念

堆内存是由内存管理器统一管理的一块运行时内存区域。程序可以按需申请指定大小的空间，并决定其使用周期。与函数退出时自动回收的栈内存不同，堆内存使用完成后必须主动释放。

堆空间容量有限，反复申请和释放还可能产生内存碎片，因此每次申请都必须检查返回值，并保证成功申请的内存最终得到释放。

### 生命周期

堆内存由程序在运行时申请，并在使用完成后主动释放。完整流程如下：

```mermaid
flowchart LR
    A[申请内存] --> B{返回 NULL}
    B -->|是| C[执行失败处理]
    B -->|否| D[读写内存]
    D --> E[调用配对接口释放]
    E --> F[指针置为 NULL]
```

堆空间有限，分配接口可能返回 `NULL`。返回 `NULL` 后不能访问该指针，应根据业务选择跳过操作、降低缓冲区大小、分块处理或返回错误。

`malloc` 类接口返回的内存未初始化，必须先写后读；`zalloc` 类接口会将内存清零。对齐分配接口用于满足缓存行或特定硬件数据结构的地址对齐要求。

## 涉及 API

| API | 谁调用 | 用途 |
| --- | --- | --- |
| `osal_kmalloc()` | 案例任务 | 申请普通堆内存 |
| `osal_kzalloc()` | 案例任务 | 申请并清零普通堆内存 |
| `osal_kmalloc_align()` | 案例任务 | 申请指定地址对齐的堆内存 |
| `osal_kzalloc_align()` | 案例任务 | 申请指定地址对齐并清零的堆内存 |
| `osal_vmalloc()` | 案例任务 | 申请较大的缓冲区 |
| `osal_vzalloc()` | 案例任务 | 申请并清零较大的缓冲区 |
| `osal_kfree()` | 案例任务 | 释放 `kmalloc`、`kzalloc` 及对齐分配的内存 |
| `osal_vfree()` | 案例任务 | 释放 `vmalloc`、`vzalloc` 分配的内存 |

分配接口和释放接口必须配对，不要混用 `osal_kfree()` 与 `osal_vfree()`。所有分配接口都可能返回 `NULL`。

WS63 当前 LiteOS 实现会忽略 `osal_gfp_flag` 参数，本案例统一传入 `OSAL_GFP_KERNEL`。

## 案例说明

### 案例简介

- 普通堆内存申请：`osal_kmalloc()`、`osal_kzalloc()`
- 对齐堆内存申请：`osal_kmalloc_align()`、`osal_kzalloc_align()`
- 较大缓冲区申请：`osal_vmalloc()`、`osal_vzalloc()`
- 分配失败处理：申请 1 MiB 并检查返回值

### 功能规格

| 测试项 | 分配大小 | 验证内容 |
| --- | ---: | --- |
| `osal_kmalloc()` | 256 字节 | 数据写入、回读和释放 |
| `osal_kzalloc()` | 256 字节 | 初始内容全部为 0 |
| `osal_kmalloc_align()` | 256 字节 | 地址满足 32 字节对齐 |
| `osal_kzalloc_align()` | 256 字节 | 地址对齐且内容全部为 0 |
| `osal_vmalloc()` | 8192 字节 | 较大缓冲区的数据读写和释放 |
| `osal_vzalloc()` | 8192 字节 | 较大缓冲区的清零和释放 |
| 预期失败 | 1 MiB | 返回 `NULL` 后程序安全继续运行 |

### 源码目录

```text
application/samples/os/
├── CMakeLists.txt
├── Kconfig
└── memory/
    └── osal_kmalloc/
        ├── CMakeLists.txt
        └── osal_kmalloc.c
```

## 案例操作指导

### 第一步：启用并编译

```bash
fbb config set CONFIG_SAMPLE_ENABLE=y --target ws63-liteos-app
fbb config set CONFIG_ENABLE_OS_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_OSAL_KMALLOC=y --target ws63-liteos-app
fbb build --clean ws63-liteos-app
```

### 第二步：烧录

```bash
fbb flash ws63-liteos-app
```

### 第三步：验证

复位开发板，串口输出如下：

```text
[osal memory] sample start
[osal memory] kmalloc size=256 checksum=0x7f80 PASS
[osal memory] kzalloc size=256 zero_check PASS
[osal memory] kmalloc_align size=256 align=32 checksum=0x7f80 PASS
[osal memory] kzalloc_align size=256 align=32 zero_check PASS
[osal memory] vmalloc size=8192 checksum=0xff000 PASS
[osal memory] vzalloc size=8192 zero_check PASS
[osal memory] expected_failure size=1048576 return=NULL handled PASS
[osal memory] summary passed=7 failed=0 alloc=6 free=6 leak=0
[osal memory] ALL TESTS PASS
```

## 关键配置

| 配置项 | 当前值 | 说明 |
| --- | ---: | --- |
| `OSAL_MEMORY_NORMAL_SIZE` | 256 字节 | 用于普通、清零和对齐分配验证 |
| `OSAL_MEMORY_LARGE_SIZE` | 8192 字节 | 用于较大缓冲区验证，调大会增加堆占用 |
| `OSAL_MEMORY_ALIGNMENT` | 32 字节 | 对齐值必须满足接口要求 |
| `OSAL_MEMORY_FAILURE_SIZE` | 1 MiB | 用于触发本案例目标配置下的失败分支 |

## 代码详解

### 分配、判空和释放

这段代码展示普通堆内存的完整生命周期。只有分配成功后才能访问和释放内存。

```c
uint8_t *buffer = (uint8_t *)osal_kmalloc(OSAL_MEMORY_NORMAL_SIZE, OSAL_GFP_KERNEL);
if (buffer == NULL) {
    return osal_memory_handle_alloc_failure("kmalloc", OSAL_MEMORY_NORMAL_SIZE);
}
stats->allocated++;

bool passed = osal_memory_fill_and_verify(buffer, OSAL_MEMORY_NORMAL_SIZE, &checksum);
osal_kfree(buffer);
buffer = NULL;
stats->freed++;
return passed;
```

`osal_kzalloc()` 和两个对齐分配接口使用相同的判空与释放流程；`osal_vmalloc()`、`osal_vzalloc()` 则使用 `osal_vfree()` 释放。

### 分配失败判断

分配接口返回 `NULL` 表示申请失败，程序不能继续访问该指针。本案例申请 1 MiB 后，如果返回 `NULL`，说明预期失败分支执行正确；如果返回非空指针，则立即释放该内存并将测试标记为失败。
