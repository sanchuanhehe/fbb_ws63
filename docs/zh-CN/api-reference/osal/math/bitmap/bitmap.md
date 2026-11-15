# Bitmap

位图（Bit Array）操作。提供位设置、清除、翻转、测试和查找（find_first_set/zero, find_next_set/zero）功能，配合 `osal_for_each_set_bit/clear_bit` 迭代宏遍历位图中所有置位/清零位。

适用内存管理中的页框追踪、中断号分配、设备 ID 分配等需要高效位标记的场景。仅在 Linux 上可用。

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_bitmap_set_bit](#osal_bitmap_set_bit) | 设置位图中指定位为1 |
| [osal_bitmap_clear_bit](#osal_bitmap_clear_bit) | 清除位图中指定位为0 |
| [osal_bitmap_change_bit](#osal_bitmap_change_bit) | 翻转位图中指定位的值 |
| [osal_bitmap_test_bit](#osal_bitmap_test_bit) | 测试位图中指定位是否被设置 |
| [osal_bitmap_test_and_set_bit](#osal_bitmap_test_and_set_bit) | 设置指定位为1并返回旧值 |
| [osal_bitmap_test_and_clear_bit](#osal_bitmap_test_and_clear_bit) | 清除指定位为0并返回旧值 |
| [osal_bitmap_test_and_change_bit](#osal_bitmap_test_and_change_bit) | 翻转指定位并返回旧值 |
| [osal_bitmap_find_first_zero_bit](#osal_bitmap_find_first_zero_bit) | 查找位图中第一个为0的位 |
| [osal_bitmap_find_first_bit](#osal_bitmap_find_first_bit) | 查找位图中第一个为1的位 |
| [osal_bitmap_find_next_zero_bit](#osal_bitmap_find_next_zero_bit) | 从指定偏移起查找下一个为0的位 |
| [osal_bitmap_find_next_bit](#osal_bitmap_find_next_bit) | 从指定偏移起查找下一个为1的位 |

## Functions

### osal_bitmap_set_bit <a id="osal_bitmap_set_bit"></a>

```c
void osal_bitmap_set_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 设置位图（unsigned long数组）中指定位nr为1
- 用于对位图进行置位操作，适用于资源位分配、状态标记等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

当前接口无返回值（返回类型为void）

### osal_bitmap_clear_bit <a id="osal_bitmap_clear_bit"></a>

```c
void osal_bitmap_clear_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 清除位图（unsigned long数组）中指定位nr为0
- 用于对位图进行复位操作，适用于资源位释放、状态清除等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

当前接口无返回值（返回类型为void）

### osal_bitmap_change_bit <a id="osal_bitmap_change_bit"></a>

```c
void osal_bitmap_change_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 翻转位图（unsigned long数组）中指定位nr的值（0变1，1变0）
- 用于对位图进行取反操作，适用于状态切换等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

当前接口无返回值（返回类型为void）

### osal_bitmap_test_bit <a id="osal_bitmap_test_bit"></a>

```c
int osal_bitmap_test_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 测试位图（unsigned long数组）中指定位nr是否被设置为1
- 用于查询位图中某一位的状态，适用于资源占用判断、状态检查等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非零值(true) | 指定位已设置 | 位图中nr对应位为1 |
| 0(false) | 指定位未设置 | 位图中nr对应位为0 |

### osal_bitmap_test_and_set_bit <a id="osal_bitmap_test_and_set_bit"></a>

```c
int osal_bitmap_test_and_set_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 设置位图（unsigned long数组）中指定位nr为1，并返回该位的旧值
- 用于原子性置位操作，适用于资源独占分配、并发位标记等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非零值(true) | 指定位原值为1 | 操作前该位已被设置 |
| 0(false) | 指定位原值为0 | 操作前该位未被设置 |

### osal_bitmap_test_and_clear_bit <a id="osal_bitmap_test_and_clear_bit"></a>

```c
int osal_bitmap_test_and_clear_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 清除位图（unsigned long数组）中指定位nr为0，并返回该位的旧值
- 用于原子性复位操作，适用于资源释放、并发位清除等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非零值(true) | 指定位原值为1 | 操作前该位已被设置 |
| 0(false) | 指定位原值为0 | 操作前该位未被设置 |

### osal_bitmap_test_and_change_bit <a id="osal_bitmap_test_and_change_bit"></a>

```c
int osal_bitmap_test_and_change_bit(int nr, unsigned long *addr)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 翻转位图（unsigned long数组）中指定位nr的值，并返回该位的旧值
- 用于原子性取反操作，适用于状态切换与旧值获取等场景
- 支持Linux系统

**前置条件**

- 参数addr不为NULL，且指向的内存空间已申请成功
- 参数nr应小于位图的总位数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| nr | int | 位图中的位编号 | 非负整数，须小于位图总位数 |
| addr | unsigned long * | 位图数组首地址 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非零值(true) | 指定位原值为1 | 操作前该位已被设置 |
| 0(false) | 指定位原值为0 | 操作前该位未被设置 |

### osal_bitmap_find_first_zero_bit <a id="osal_bitmap_find_first_zero_bit"></a>

```c
int osal_bitmap_find_first_zero_bit(const unsigned long *name, unsigned size)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 在位图（unsigned long数组）中查找第一个为0的位的位置
- 用于空闲资源位查找，适用于资源分配、空闲位搜索等场景
- 支持Linux系统

**前置条件**

- 参数name不为NULL，且指向的内存空间已申请成功
- 参数size应与位图实际分配的位数一致

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const unsigned long * | 位图数组首地址 | 非NULL |
| size | unsigned | 位图的总位数 | 正整数，与位图实际分配位数一致 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0~size-1 | 第一个为0的位的位置 | 位图中存在为0的位 |
| >=size | 位图中所有位均为1 | 位图中无空闲位 |

### osal_bitmap_find_first_bit <a id="osal_bitmap_find_first_bit"></a>

```c
int osal_bitmap_find_first_bit(const unsigned long *name, unsigned size)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 在位图（unsigned long数组）中查找第一个为1的位的位置
- 用于已占用资源位查找，适用于状态遍历、已设置位搜索等场景
- 支持Linux系统

**前置条件**

- 参数name不为NULL，且指向的内存空间已申请成功
- 参数size应与位图实际分配的位数一致

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const unsigned long * | 位图数组首地址 | 非NULL |
| size | unsigned | 位图的总位数 | 正整数，与位图实际分配位数一致 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0~size-1 | 第一个为1的位的位置 | 位图中存在为1的位 |
| >=size | 位图中所有位均为0 | 位图中无已设置位 |

### osal_bitmap_find_next_zero_bit <a id="osal_bitmap_find_next_zero_bit"></a>

```c
int osal_bitmap_find_next_zero_bit(const unsigned long *name, int size, int offset)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 从位图（unsigned long数组）中指定偏移位置起，查找下一个为0的位的位置
- 用于从指定位置开始的空闲资源位查找，适用于增量式空闲位搜索等场景
- 支持Linux系统

**前置条件**

- 参数name不为NULL，且指向的内存空间已申请成功
- 参数size应与位图实际分配的位数一致
- 参数offset应小于size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const unsigned long * | 位图数组首地址 | 非NULL |
| size | int | 位图的总位数 | 正整数，与位图实际分配位数一致 |
| offset | int | 起始搜索位置的位编号 | 非负整数，须小于size |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| offset~size-1 | 从offset起下一个为0的位的位置 | 位图中存在为0的位 |
| >=size | 从offset起所有位均为1 | 位图中无空闲位 |

### osal_bitmap_find_next_bit <a id="osal_bitmap_find_next_bit"></a>

```c
int osal_bitmap_find_next_bit(const unsigned long *name, unsigned size, int offset)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_bitmap.h"
```

**功能说明**

- 从位图（unsigned long数组）中指定偏移位置起，查找下一个为1的位的位置
- 用于从指定位置开始的已占用资源位查找，适用于增量式状态遍历等场景
- 支持Linux系统

**前置条件**

- 参数name不为NULL，且指向的内存空间已申请成功
- 参数size应与位图实际分配的位数一致
- 参数offset应小于size

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const unsigned long * | 位图数组首地址 | 非NULL |
| size | unsigned | 位图的总位数 | 正整数，与位图实际分配位数一致 |
| offset | int | 起始搜索位置的位编号 | 非负整数，须小于size |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| offset~size-1 | 从offset起下一个为1的位的位置 | 位图中存在为1的位 |
| >=size | 从offset起所有位均为0 | 位图中无已设置位 |

## Macros

### OSAL_BITS_PER_BYTE <a id="macro_osal_bits_per_byte"></a>

```c
#define OSAL_BITS_PER_BYTE 8
```

**使用说明**

表示每字节包含的位数，在OSAL_BITS_PER_LONG、OSAL_BIT_MASK、OSAL_BIT_WORD、OSAL_BITS_TO_LONGS等宏计算中使用

### OSAL_BITS_PER_LONG <a id="macro_osal_bits_per_long"></a>

```c
#define OSAL_BITS_PER_LONG (OSAL_BITS_PER_BYTE * sizeof(unsigned long))
```

**使用说明**

表示每个unsigned long包含的位数，在OSAL_BIT_MASK、OSAL_BIT_WORD、OSAL_BITS_TO_LONGS等宏计算中使用

### OSAL_BIT_MASK(nr) <a id="macro_osal_bit_mask"></a>

```c
#define OSAL_BIT_MASK(nr) (1UL << ((nr) % OSAL_BITS_PER_LONG))
```

**使用说明**

计算指定位nr在unsigned long内的掩码值，在位图操作函数内部计算中使用

### OSAL_BIT_WORD(nr) <a id="macro_osal_bit_word"></a>

```c
#define OSAL_BIT_WORD(nr) ((nr) / OSAL_BITS_PER_LONG)
```

**使用说明**

计算指定位nr所在的unsigned long数组索引，在位图操作函数内部计算中使用

### OSAL_BITS_TO_LONGS(nr) <a id="macro_osal_bits_to_longs"></a>

```c
#define OSAL_BITS_TO_LONGS(nr) (((nr) + (OSAL_BITS_PER_LONG)-1) / (OSAL_BITS_PER_LONG))
```

**使用说明**

计算容纳指定位数所需的unsigned long数组长度，在OSAL_DECLARE_BITMAP宏中使用

### OSAL_DECLARE_BITMAP(name, bits) <a id="macro_osal_declare_bitmap"></a>

```c
#define OSAL_DECLARE_BITMAP(name, bits) unsigned long name[OSAL_BITS_TO_LONGS(bits)]
```

**使用说明**

声明一个指定位数的位图数组，作为位图操作函数的addr/name参数使用

### osal_for_each_set_bit(bit, addr, size) <a id="macro_osal_for_each_set_bit"></a>

```c
#define osal_for_each_set_bit(bit, addr, size)                               \
    for ((bit) = osal_bitmap_find_first_bit((addr), (size)); (bit) < (size); \
        (bit) = osal_bitmap_find_next_bit((addr), (size), (bit) + 1))
```

**使用说明**

遍历位图中所有为1的位，内部调用osal_bitmap_find_first_bit和osal_bitmap_find_next_bit

### osal_for_each_clear_bit(bit, addr, size) <a id="macro_osal_for_each_clear_bit"></a>

```c
#define osal_for_each_clear_bit(bit, addr, size)                                  \
    for ((bit) = osal_bitmap_find_first_zero_bit((addr), (size)); (bit) < (size); \
        (bit) = osal_bitmap_find_next_zero_bit((addr), (size), (bit) + 1))
```

**使用说明**

遍历位图中所有为0的位，内部调用osal_bitmap_find_first_zero_bit和osal_bitmap_find_next_zero_bit


