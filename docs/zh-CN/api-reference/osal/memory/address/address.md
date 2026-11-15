# Address

内核态动态内存分配。提供 kmalloc（不初始化）、kzalloc（零初始化）、kcalloc（数组零初始化）、对齐分配（kmalloc_align）和释放（kfree）。同时提供 vmalloc（虚拟连续大块内存）、ioremap（映射物理地址到内核虚拟地址空间）和用户态-内核态数据拷贝接口（copy_from_user/copy_to_user）。

分配标志（GFP_ZERO/GFP_ATOMIC/GFP_DMA/GFP_KERNEL）控制内存行为和来源。

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_kmalloc](#osal_kmalloc) | 动态内存分配，返回指定大小的内存块指针 |
| [osal_kzalloc](#osal_kzalloc) | 动态内存分配，返回指定大小的内存块指针并清零 |
| [osal_kmalloc_align](#osal_kmalloc_align) | 按指定边界对齐分配动态内存 |
| [osal_kzalloc_align](#osal_kzalloc_align) | 按指定边界对齐分配动态内存并清零 |
| [osal_kfree](#osal_kfree) | 释放由 osal_kmalloc/osal_kzalloc 分配的动态内存 |
| [osal_vmalloc](#osal_vmalloc) | 分配虚拟地址连续的内存空间 |
| [osal_vzalloc](#osal_vzalloc) | 分配虚拟地址连续的内存空间并清零 |
| [osal_vfree](#osal_vfree) | 释放由 osal_vmalloc 分配的虚拟内存 |
| [osal_pool_mem_init](#osal_pool_mem_init) | 初始化指定内存池的动态内存 |
| [osal_pool_mem_alloc](#osal_pool_mem_alloc) | 从指定内存池分配动态内存 |
| [osal_pool_mem_alloc_align](#osal_pool_mem_alloc_align) | 从指定内存池按指定边界对齐分配动态内存 |
| [osal_pool_mem_free](#osal_pool_mem_free) | 释放指定内存池中的动态内存 |
| [osal_pool_mem_deinit](#osal_pool_mem_deinit) | 反初始化指定内存池的动态内存 |
| [osal_blockmem_get_status](#osal_blockmem_get_status) | 获取块内存状态 |
| [osal_ioremap](#osal_ioremap) | 将设备总线地址映射为 CPU 虚拟地址（device memory，不使用 cache） |
| [osal_ioremap_nocache](#osal_ioremap_nocache) | 将设备总线地址映射为 CPU 虚拟地址（不缓存，与 osal_ioremap 功能一致） |
| [osal_ioremap_cached](#osal_ioremap_cached) | 将设备总线地址映射为 CPU 虚拟地址（normal memory，使用 cache） |
| [osal_iounmap](#osal_iounmap) | 释放 ioremap 映射的虚拟地址 |
| [osal_ioremap_wc](#osal_ioremap_wc) | 将设备总线地址映射为 CPU 虚拟地址（write combined） |
| [osal_phys_to_virt](#osal_phys_to_virt) | 物理地址转换为虚拟地址 |
| [osal_virt_to_phys](#osal_virt_to_phys) | 虚拟地址转换为物理地址 |
| [osal_blockmem_vmap](#osal_blockmem_vmap) | 将物理地址映射为连续内核虚拟空间 |
| [osal_blockmem_vunmap](#osal_blockmem_vunmap) | 释放由 osal_blockmem_vmap 映射的虚拟内存区域 |
| [osal_blockmem_free](#osal_blockmem_free) | 释放产品中定义的保留内存 |
| [osal_copy_from_user](#osal_copy_from_user) | 从用户空间拷贝数据到内核空间 |
| [osal_copy_to_user](#osal_copy_to_user) | 从内核空间拷贝数据到用户空间 |
| [osal_access_ok](#osal_access_ok) | 检查用户空间内存块是否可访问 |

## Functions

### osal_kmalloc <a id="osal_kmalloc"></a>

```c
void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

动态内存分配，返回指定大小的内存块指针。在 LiteOS (Huawei LiteOS) 和 FreeRTOS (Free Real-Time Operating System) 下，osal_gfp_flag 参数未使用；在 Linux 下，必须包含 OSAL_GFP_ATOMIC、OSAL_GFP_DMA (Direct Memory Access)、OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 按位或组合使用。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned long | 需要分配的内存字节数 |
| osal_gfp_flag | unsigned int | 分配标志，LiteOS/FreeRTOS 下未使用；Linux 下须包含 OSAL_GFP_ATOMIC/OSAL_GFP_DMA/OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 组合 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已分配内存块指针 |

```c
// src/application/samples/wifi/sta_sample/sta_sample.c:99
wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
// ...
osal_kfree(result);
```

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### osal_kzalloc <a id="osal_kzalloc"></a>

```c
void *osal_kzalloc(unsigned long size, unsigned int osal_gfp_flag)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

动态内存分配，返回指定大小的内存块指针并清零。在 LiteOS 和 FreeRTOS 下，osal_gfp_flag 参数未使用；在 Linux 下，必须包含 OSAL_GFP_ATOMIC、OSAL_GFP_DMA、OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 按位或组合使用。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned long | 需要分配的内存字节数 |
| osal_gfp_flag | unsigned int | 分配标志，LiteOS/FreeRTOS 下未使用；Linux 下须包含 OSAL_GFP_ATOMIC/OSAL_GFP_DMA/OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 组合 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已清零的内存块指针 |

### osal_kmalloc_align <a id="osal_kmalloc_align"></a>

```c
void *osal_kmalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

按指定边界对齐分配动态内存。对齐参数 boundary 必须为 2 的幂且最小值为 4。在 LiteOS 和 FreeRTOS 下，osal_gfp_flag 参数未使用；在 Linux 下，必须包含 OSAL_GFP_ATOMIC、OSAL_GFP_DMA、OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 按位或组合使用。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned int | 需要分配的内存字节数 |
| osal_gfp_flag | unsigned int | 分配标志，LiteOS/FreeRTOS 下未使用；Linux 下须包含 OSAL_GFP_ATOMIC/OSAL_GFP_DMA/OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 组合 |
| boundary | unsigned int | 内存对齐边界（单位：字节），必须为 2 的幂且最小值为 4 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已对齐的内存块指针 |

### osal_kzalloc_align <a id="osal_kzalloc_align"></a>

```c
void *osal_kzalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

按指定边界对齐分配动态内存并清零。对齐参数 boundary 必须为 2 的幂且最小值为 4。在 LiteOS 和 FreeRTOS 下，osal_gfp_flag 参数未使用；在 Linux 下，必须包含 OSAL_GFP_ATOMIC、OSAL_GFP_DMA、OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 按位或组合使用。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned int | 需要分配的内存字节数 |
| osal_gfp_flag | unsigned int | 分配标志，LiteOS/FreeRTOS 下未使用；Linux 下须包含 OSAL_GFP_ATOMIC/OSAL_GFP_DMA/OSAL_GFP_KERNEL 之一，可与 OSAL_GFP_ZERO 组合 |
| boundary | unsigned int | 内存对齐边界（单位：字节），必须为 2 的幂且最小值为 4 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已对齐且清零的内存块指针 |

### osal_kfree <a id="osal_kfree"></a>

```c
void osal_kfree(void *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放由 osal_kmalloc/osal_kzalloc 分配的动态内存。

**前置条件**

待释放内存由 osal_kmalloc 或 osal_kzalloc 分配。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void* | 待释放内存块的起始地址 |

**返回值**

- 返回类型：void

```c
// src/application/samples/wifi/sta_sample/sta_sample.c:99,106
wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
// ...
osal_kfree(result);
```

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_vmalloc <a id="osal_vmalloc"></a>

```c
void *osal_vmalloc(unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

分配虚拟地址连续的内存空间。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 LOS_MemAlloc，否则调用 LOS_VMalloc。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned long | 需要分配的内存字节数 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回虚拟内存指针 |

```c
// src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c:238
property.value = osal_vmalloc(sizeof(g_sle_property_value));
```

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 LOS_MemAlloc 或 LOS_VMalloc |

### osal_vzalloc <a id="osal_vzalloc"></a>

```c
void *osal_vzalloc(unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

分配虚拟地址连续的内存空间并清零。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 LOS_MemAlloc，否则调用 LOS_VMalloc；分配成功后清零。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned long | 需要分配的内存字节数 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已清零的虚拟内存指针 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 LOS_MemAlloc 或 LOS_VMalloc |

### osal_vfree <a id="osal_vfree"></a>

```c
void osal_vfree(void *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放由 osal_vmalloc 分配的虚拟内存。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 LOS_MemFree，否则调用 LOS_VFree。

**前置条件**

待释放内存由 osal_vmalloc 或 osal_vzalloc 分配。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void* | 待释放虚拟内存块的起始地址 |

**返回值**

- 返回类型：void

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 LOS_MemFree 或 LOS_VFree |

### osal_pool_mem_init <a id="osal_pool_mem_init"></a>

```c
int osal_pool_mem_init(void *pool, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

初始化指定内存池的动态内存。size 参数应满足：1) 小于或等于内存池大小；2) 大于各系统 min_pool_size。输入参数必须 4 或 8 字节对齐。初始化区域 [pool, pool + size] 不应与其他内存池冲突。LiteOS 下仅在 LOSCFG_MEM_MUL_MODULE 定义时生效；FreeRTOS 下仅在 XLTCFG_SUPPORT_MEMMNG 定义时生效。

**前置条件**

- LiteOS 下须定义 LOSCFG_MEM_MUL_MODULE
- FreeRTOS 下须定义 XLTCFG_SUPPORT_MEMMNG

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pool | void* | 内存起始地址 |
| size | unsigned int | 内存大小（单位：字节），须 4 或 8 字节对齐 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS | 内存池初始化成功 |
| OSAL_FAILURE | 内存池初始化失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_MEM_MUL_MODULE | LiteOS 多模块内存管理开关，控制本函数在 LiteOS 下是否编译 |
| XLTCFG_SUPPORT_MEMMNG | FreeRTOS 内存管理开关，控制本函数在 FreeRTOS 下是否编译 |

### osal_pool_mem_alloc <a id="osal_pool_mem_alloc"></a>

```c
void *osal_pool_mem_alloc(void *pool, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

从指定内存池分配动态内存。pool 参数必须已通过 osal_pool_mem_init 初始化。size 不得超过 osal_pool_mem_init 时指定的内存池大小，且须 4 字节对齐。LiteOS 下仅在 LOSCFG_MEM_MUL_MODULE 定义时生效；FreeRTOS 下仅在 XLTCFG_SUPPORT_MEMMNG 定义时生效。

**前置条件**

- pool 已通过 osal_pool_mem_init 初始化
- LiteOS 下须定义 LOSCFG_MEM_MUL_MODULE
- FreeRTOS 下须定义 XLTCFG_SUPPORT_MEMMNG

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pool | void* | 指向已初始化的内存池指针 |
| size | unsigned int | 需要分配的内存字节数，须 4 字节对齐 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已分配内存块指针 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_MEM_MUL_MODULE | LiteOS 多模块内存管理开关，控制本函数在 LiteOS 下是否编译 |
| XLTCFG_SUPPORT_MEMMNG | FreeRTOS 内存管理开关，控制本函数在 FreeRTOS 下是否编译 |

### osal_pool_mem_alloc_align <a id="osal_pool_mem_alloc_align"></a>

```c
void *osal_pool_mem_alloc_align(void *pool, unsigned int size, unsigned int boundary)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

从指定内存池按指定边界对齐分配动态内存。pool 参数必须已通过 osal_pool_mem_init 初始化。size 不得超过 osal_pool_mem_init 时指定的内存池大小。boundary 必须为 2 的幂且最小值为 4。LiteOS 下仅在 LOSCFG_MEM_MUL_MODULE 定义时生效；FreeRTOS 下仅在 XLTCFG_SUPPORT_MEMMNG 定义时生效。

**前置条件**

- pool 已通过 osal_pool_mem_init 初始化
- LiteOS 下须定义 LOSCFG_MEM_MUL_MODULE
- FreeRTOS 下须定义 XLTCFG_SUPPORT_MEMMNG

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pool | void* | 指向已初始化的内存池指针 |
| size | unsigned int | 需要分配的内存字节数 |
| boundary | unsigned int | 内存对齐边界（单位：字节），必须为 2 的幂且最小值为 4 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 内存分配失败 |
| void* | 内存分配成功，返回已对齐的内存块指针 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_MEM_MUL_MODULE | LiteOS 多模块内存管理开关，控制本函数在 LiteOS 下是否编译 |
| XLTCFG_SUPPORT_MEMMNG | FreeRTOS 内存管理开关，控制本函数在 FreeRTOS 下是否编译 |

### osal_pool_mem_free <a id="osal_pool_mem_free"></a>

```c
void osal_pool_mem_free(void *pool, const void *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放指定内存池中的动态内存。pool 参数必须已通过 osal_pool_mem_init 初始化。addr 参数必须由 osal_pool_mem_alloc 或 osal_pool_mem_alloc_align 分配。LiteOS 下仅在 LOSCFG_MEM_MUL_MODULE 定义时生效；FreeRTOS 下仅在 XLTCFG_SUPPORT_MEMMNG 定义时生效。

**前置条件**

- pool 已通过 osal_pool_mem_init 初始化
- addr 由 osal_pool_mem_alloc 或 osal_pool_mem_alloc_align 分配
- LiteOS 下须定义 LOSCFG_MEM_MUL_MODULE
- FreeRTOS 下须定义 XLTCFG_SUPPORT_MEMMNG

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pool | void* | 指向已初始化的内存池指针 |
| addr | const void* | 待释放内存块的起始地址 |

**返回值**

- 返回类型：void

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_MEM_MUL_MODULE | LiteOS 多模块内存管理开关，控制本函数在 LiteOS 下是否编译 |
| XLTCFG_SUPPORT_MEMMNG | FreeRTOS 内存管理开关，控制本函数在 FreeRTOS 下是否编译 |

### osal_pool_mem_deinit <a id="osal_pool_mem_deinit"></a>

```c
int osal_pool_mem_deinit(void *pool)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

反初始化指定内存池的动态内存。LiteOS 下仅在 LOSCFG_MEM_MUL_POOL 定义且未定义 TINY_KERNEL 时生效；FreeRTOS 下仅在 XLTCFG_SUPPORT_MEMMNG 和 XLTCFG_MEM_MUL_POOL 同时定义时生效。

**前置条件**

- LiteOS 下须定义 LOSCFG_MEM_MUL_POOL 且未定义 TINY_KERNEL
- FreeRTOS 下须同时定义 XLTCFG_SUPPORT_MEMMNG 和 XLTCFG_MEM_MUL_POOL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pool | void* | 内存起始地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS | 内存池反初始化成功 |
| OSAL_FAILURE | 内存池反初始化失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_MEM_MUL_POOL | LiteOS 多内存池开关，控制本函数在 LiteOS 下是否编译 |
| TINY_KERNEL | 微内核模式，定义时本函数直接返回 OSAL_SUCCESS |
| XLTCFG_SUPPORT_MEMMNG | FreeRTOS 内存管理开关 |
| XLTCFG_MEM_MUL_POOL | FreeRTOS 多内存池开关，控制本函数在 FreeRTOS 下是否编译 |

### osal_blockmem_get_status <a id="osal_blockmem_get_status"></a>

```c
osal_blockmem_status osal_blockmem_get_status(unsigned long phyaddr, unsigned int size);
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

获取块内存状态。根据物理地址和大小判断块内存的有效性。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phyaddr | unsigned long | 物理地址 |
| size | unsigned int | 待操作的地址大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_BLOCKMEM_VALID | 块内存有效 |
| OSAL_BLOCKMEM_INVALID_PHYADDR | 物理地址无效 |
| OSAL_BLOCKMEM_INVALID_SIZE | 大小无效 |

### osal_ioremap <a id="osal_ioremap"></a>

```c
void *osal_ioremap(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

将设备总线地址映射为 CPU 虚拟地址。映射的内存类型为 device memory，不使用 cache。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 ioremap_nocache，否则调用 ioremap。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 总线地址（物理地址） |
| size | unsigned long | 映射资源大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 映射失败 |
| void* | 映射成功，返回虚拟地址指针 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 ioremap_nocache 或 ioremap |

### osal_ioremap_nocache <a id="osal_ioremap_nocache"></a>

```c
void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

将设备总线地址映射为 CPU 虚拟地址。与 osal_ioremap 功能一致，不使用缓存，保留此接口用于向后兼容。映射后须使用 osal_iounmap 释放。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 总线地址（物理地址） |
| size | unsigned long | 映射资源大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 映射失败 |
| void* | 映射成功，返回虚拟地址指针 |

### osal_ioremap_cached <a id="osal_ioremap_cached"></a>

```c
void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

将设备总线地址映射为 CPU 虚拟地址。映射的内存类型为 normal memory，使用 cache，可加速内存访问、提升系统性能。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 总线地址（物理地址） |
| size | unsigned long | 映射资源大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 映射失败 |
| void* | 映射成功，返回虚拟地址指针 |

### osal_iounmap <a id="osal_iounmap"></a>

```c
void osal_iounmap(void *addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放 ioremap 映射的虚拟地址。调用者须确保同一指针只执行一次 unmapping。

**前置条件**

- addr 由 osal_ioremap/osal_ioremap_nocache/osal_ioremap_cached/osal_ioremap_wc 映射获得

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void* | ioremap 返回的虚拟地址 |
| size | unsigned long | 映射资源大小（单位：字节） |

**返回值**

- 返回类型：void

### osal_ioremap_wc <a id="osal_ioremap_wc"></a>

```c
void *osal_ioremap_wc(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

将设备总线地址映射为 CPU 虚拟地址，使用 write combined 模式。映射后须使用 osal_iounmap 释放。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 总线地址（物理地址） |
| size | unsigned long | 映射资源大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 映射失败 |
| void* | 映射成功，返回虚拟地址指针 |

### osal_phys_to_virt <a id="osal_phys_to_virt"></a>

```c
void *osal_phys_to_virt(unsigned long addr)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

物理地址转换为虚拟地址。在 LiteOS 下直接将物理地址强制类型转换为虚拟地址指针。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | unsigned long | 物理地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| void* | 转换后的虚拟地址指针 |

### osal_virt_to_phys <a id="osal_virt_to_phys"></a>

```c
unsigned long osal_virt_to_phys(const void *virt_addr);
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

虚拟地址转换为物理地址。在 LiteOS 下直接将虚拟地址强制类型转换为物理地址。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| virt_addr | const void* | 虚拟地址指针 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned long | 转换后的物理地址 |

### osal_blockmem_vmap <a id="osal_blockmem_vmap"></a>

```c
void *osal_blockmem_vmap(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

将物理地址映射为连续内核虚拟空间。仅支持 VM_MAP 与 PAGE_KERNEL 标志。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 物理地址 |
| size | unsigned long | 待操作的地址大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 映射失败 |
| void* | 映射成功，返回虚拟地址指针 |

### osal_blockmem_vunmap <a id="osal_blockmem_vunmap"></a>

```c
void osal_blockmem_vunmap(const void *virt_addr)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放由 osal_blockmem_vmap 映射的虚拟内存区域。不可在中断上下文中调用。

**前置条件**

- virt_addr 由 osal_blockmem_vmap 映射获得

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| virt_addr | const void* | 虚拟地址 |

**返回值**

- 返回类型：void

### osal_blockmem_free <a id="osal_blockmem_free"></a>

```c
void osal_blockmem_free(unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

释放产品中定义的保留内存。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| phys_addr | unsigned long | 物理地址 |
| size | unsigned long | 待操作的地址大小（单位：字节） |

**返回值**

- 返回类型：void

### osal_copy_from_user <a id="osal_copy_from_user"></a>

```c
unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n);
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

从用户空间拷贝数据到内核空间。当 to 或 from 为 NULL 时直接返回 n。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 memcpy_s，否则调用 LOS_CopyToKernel。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| to | void* | 内核空间目标地址（出参） |
| from | const void* | 用户空间源地址 |
| n | unsigned long | 拷贝数据长度（单位：字节） |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| to | void* | 拷贝完成后，内核空间中写入数据的起始地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 拷贝成功 |
| n | 拷贝失败，返回未拷贝的字节数 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 memcpy_s 或 LOS_CopyToKernel |

### osal_copy_to_user <a id="osal_copy_to_user"></a>

```c
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n);
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

从内核空间拷贝数据到用户空间。当 to 或 from 为 NULL 时直接返回 n。在 LiteOS 开源版本（HW_LITEOS_OPEN_VERSION_NUM 定义时）下调用 memcpy_s，否则调用 LOS_CopyFromKernel。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| to | void* | 用户空间目标地址（出参） |
| from | const void* | 内核空间源地址 |
| n | unsigned long | 拷贝数据长度（单位：字节） |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| to | void* | 拷贝完成后，用户空间中写入数据的起始地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 拷贝成功 |
| n | 拷贝失败，返回未拷贝的字节数 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| HW_LITEOS_OPEN_VERSION_NUM | LiteOS 开源版本标识，影响内部实现选择 memcpy_s 或 LOS_CopyFromKernel |

### osal_access_ok <a id="osal_access_ok"></a>

```c
int osal_access_ok(int type, const void *addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_addr.h"
```

**功能说明**

检查用户空间内存块是否可访问。在 LiteOS 下打印日志 "Do not support in liteos!" 并直接返回 1（true）。type 参数取值：OSAL_VERIFY_READ（0）表示读检查，OSAL_VERIFY_WRITE（1）表示写检查。

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| type | int | 访问类型，0：读（OSAL_VERIFY_READ），1：写（OSAL_VERIFY_WRITE） |
| addr | const void* | 用户空间待检查内存块的起始地址指针 |
| size | unsigned long | 待检查内存块大小（单位：字节） |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true（1） | 用户空间内存块可访问（LiteOS 下始终返回 true） |
| false（0） | 用户空间内存块不可访问 |

## Enumerations

### osal_blockmem_status <a id="enum_osal_blockmem_status"></a>

```c
typedef enum {
    OSAL_BLOCKMEM_VALID = 0,
    OSAL_BLOCKMEM_INVALID_PHYADDR = 1,
    OSAL_BLOCKMEM_INVALID_SIZE = 2,
    OSAL_BLOCKMEM_MAX,
} osal_blockmem_status;
```

**功能说明**

块内存状态枚举，用于 osal_blockmem_get_status 返回值。

| 枚举值 | 值 | 描述 |
| ------ | -- | ---- |
| OSAL_BLOCKMEM_VALID | 0 | 块内存有效 |
| OSAL_BLOCKMEM_INVALID_PHYADDR | 1 | 物理地址无效 |
| OSAL_BLOCKMEM_INVALID_SIZE | 2 | 大小无效 |
| OSAL_BLOCKMEM_MAX | 3 | 枚举边界值 |



