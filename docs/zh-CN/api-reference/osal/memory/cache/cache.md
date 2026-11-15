# Cache

CPU 数据缓存（DCache）维护操作，解决 DMA (Direct Memory Access) 与 CPU 之间的缓存一致性问题。DMA 写入内存前需执行 DCache clean（刷新到物理内存），DMA 写入后需执行 DCache invalidate（使缓存失效，CPU 读到最新物理内存内容）。

提供按虚拟地址或物理地址范围的 writeback/invalidate/clean 操作。

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_dcache_region_wb](#osal_dcache_region_wb) | 按地址回写DCache，将指定内存区域的DCache数据写回主存 |
| [osal_dcache_region_inv](#osal_dcache_region_inv) | 按地址无效DCache，使指定内存区域的DCache缓存行失效 |
| [osal_dcache_region_clean](#osal_dcache_region_clean) | 按地址清除DCache，将指定内存区域的DCache数据写回主存并使缓存行无效 |
| [osal_arch_dcache_flush_by_va](#osal_arch_dcache_flush_by_va) | 按虚拟地址刷写DCache，将指定内存区域的DCache数据写回主存 |
| [osal_arch_dcahce_inv_by_va](#osal_arch_dcahce_inv_by_va) | 按虚拟地址无效DCache，使指定内存区域的DCache缓存行失效 |

## Functions

### osal_dcache_region_wb <a id="osal_dcache_region_wb"></a>

```c
void osal_dcache_region_wb(void *kvirt, unsigned long phys_addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

**功能说明**

- 将指定内存区域的DCache数据写回主存（write-back）
- 起始地址若未按CACHE_LINE_SIZE(32Bytes)对齐，将自动向下对齐到CACHE_LINE_SIZE边界
- 当MMU (Memory Management Unit) 不存在时，需确保phys_addr有效，此时用户通过kvirt参数设置地址

**前置条件**

- 无MMU场景下，phys_addr参数必须为有效的物理地址，或通过kvirt传入有效地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| kvirt | void * | 内核虚拟地址指针 | 非NULL指针，指向有效内存区域 |
| phys_addr | unsigned long | 需要回写的起始物理地址 | 有效物理地址；当MMU不存在时需确保有效，若为0则使用kvirt的地址值 |
| size | unsigned long | 需要回写的内存大小（字节） | 大于0 |

### osal_dcache_region_inv <a id="osal_dcache_region_inv"></a>

```c
void osal_dcache_region_inv(void *addr, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

**功能说明**

- 使指定内存区域的DCache缓存行失效（invalidate）
- 起始地址若未按CACHE_LINE_SIZE(32Bytes)对齐，将自动向下对齐到CACHE_LINE_SIZE边界
- 用于DMA读取前使CPU缓存失效，确保后续读取来自主存而非缓存

**前置条件**

- addr指向的内存区域必须有效且可访问

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void * | 需要无效的起始地址 | 非NULL指针，指向有效内存区域 |
| size | unsigned long | 需要无效的内存大小（字节） | 大于0 |

### osal_dcache_region_clean <a id="osal_dcache_region_clean"></a>

```c
void osal_dcache_region_clean(void *addr, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

**功能说明**

- 将指定内存区域的DCache数据写回主存（clean）
- 根据起始地址和大小清除DCache，将脏缓存行写回主存
- 用于DMA写入前确保CPU已写入的数据已同步到主存

**前置条件**

- addr指向的内存区域必须有效且可访问

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void * | 需要清除的起始地址 | 非NULL指针，指向有效内存区域 |
| size | unsigned int | 需要清除的内存大小（字节） | 大于0 |

### osal_arch_dcache_flush_by_va <a id="osal_arch_dcache_flush_by_va"></a>

```c
void osal_arch_dcache_flush_by_va(void *base_addr, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

**功能说明**

- 按虚拟地址刷写DCache，将指定内存区域的DCache数据写回主存
- 用于seLiteOS系统下的DCache刷写操作
- 刷写操作包含写回并无效缓存行

**前置条件**

- base_addr指向的内存区域必须有效且可访问

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| base_addr | void * | 需要刷写的起始虚拟地址 | 非NULL指针，指向有效内存区域 |
| size | unsigned int | 需要刷写的内存大小（字节） | 大于0 |

### osal_arch_dcahce_inv_by_va <a id="osal_arch_dcahce_inv_by_va"></a>

```c
void osal_arch_dcahce_inv_by_va(void *base_addr, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/memory/osal_cache.h"
```

**功能说明**

- 按虚拟地址无效DCache，使指定内存区域的DCache缓存行失效
- 用于seLiteOS系统下的DCache无效操作
- 无效操作使缓存行失效，后续读取将从主存获取数据

**前置条件**

- base_addr指向的内存区域必须有效且可访问

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| base_addr | void * | 需要无效的起始虚拟地址 | 非NULL指针，指向有效内存区域 |
| size | unsigned int | 需要无效的内存大小（字节） | 大于0 |


