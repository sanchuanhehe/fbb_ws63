# Arithmetic

64 位整数除法运算。提供无符号和有符号 64 位数的除法接口（÷32 位或 64 位除数），返回商和余数。在 32 位处理器上 64 位除法无硬件指令支持，需软件模拟——OSAL (Operating System Abstraction Layer) 封装了不同平台下最高效的实现。

**头文件清单**
```c
#include "kernel/osal/include/math/osal_math.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_div_u64](#osal_div_u64) | 无符号64位整数除以无符号32位整数，返回商 |
| [osal_div_s64](#osal_div_s64) | 有符号64位整数除以有符号32位整数，返回商 |
| [osal_div64_u64](#osal_div64_u64) | 无符号64位整数除以无符号64位整数，返回商 |
| [osal_div64_s64](#osal_div64_s64) | 有符号64位整数除以有符号64位整数，返回商 |
| [osal_div_u64_rem](#osal_div_u64_rem) | 无符号64位整数除以无符号32位整数，返回余数 |
| [osal_div_s64_rem](#osal_div_s64_rem) | 有符号64位整数除以有符号32位整数，返回余数 |
| [osal_div64_u64_rem](#osal_div64_u64_rem) | 无符号64位整数除以无符号64位整数，返回余数 |
| [osal_get_random_int](#osal_get_random_int) | 生成随机数 |

## Functions

### osal_div_u64 <a id="osal_div_u64"></a>

```c
unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现无符号64位整数除以无符号32位整数的除法运算
- 返回被除数除以除数的商
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | unsigned long long | 无符号64位被除数 | 0 ~ 18446744073709551615 |
| divisor | unsigned int | 无符号32位除数 | 1 ~ 4294967295 |

**返回值**

- 返回类型：unsigned long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned long long | dividend / divisor 的商 | 除数非零时正常返回 |

### osal_div_s64 <a id="osal_div_s64"></a>

```c
long long osal_div_s64(long long dividend, int divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现有符号64位整数除以有符号32位整数的除法运算
- 返回被除数除以除数的商
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | long long | 有符号64位被除数 | -9223372036854775808 ~ 9223372036854775807 |
| divisor | int | 有符号32位除数 | -2147483648 ~ -1 或 1 ~ 2147483647 |

**返回值**

- 返回类型：long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| long long | dividend / divisor 的商 | 除数非零时正常返回 |

### osal_div64_u64 <a id="osal_div64_u64"></a>

```c
unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现无符号64位整数除以无符号64位整数的除法运算
- 返回被除数除以除数的商
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | unsigned long long | 无符号64位被除数 | 0 ~ 18446744073709551615 |
| divisor | unsigned long long | 无符号64位除数 | 1 ~ 18446744073709551615 |

**返回值**

- 返回类型：unsigned long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned long long | dividend / divisor 的商 | 除数非零时正常返回 |

### osal_div64_s64 <a id="osal_div64_s64"></a>

```c
long long osal_div64_s64(long long dividend, long long divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现有符号64位整数除以有符号64位整数的除法运算
- 返回被除数除以除数的商
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | long long | 有符号64位被除数 | -9223372036854775808 ~ 9223372036854775807 |
| divisor | long long | 有符号64位除数 | -9223372036854775808 ~ -1 或 1 ~ 9223372036854775807 |

**返回值**

- 返回类型：long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| long long | dividend / divisor 的商 | 除数非零时正常返回 |

### osal_div_u64_rem <a id="osal_div_u64_rem"></a>

```c
unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现无符号64位整数除以无符号32位整数的除法运算，返回余数
- 返回被除数除以除数的余数
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | unsigned long long | 无符号64位被除数 | 0 ~ 18446744073709551615 |
| divisor | unsigned int | 无符号32位除数 | 1 ~ 4294967295 |

**返回值**

- 返回类型：unsigned long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned long long | dividend / divisor 的余数 | 除数非零时正常返回 |

### osal_div_s64_rem <a id="osal_div_s64_rem"></a>

```c
long long osal_div_s64_rem(long long dividend, int divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现有符号64位整数除以有符号32位整数的除法运算，返回余数
- 返回被除数除以除数的余数
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | long long | 有符号64位被除数 | -9223372036854775808 ~ 9223372036854775807 |
| divisor | int | 有符号32位除数 | -2147483648 ~ -1 或 1 ~ 2147483647 |

**返回值**

- 返回类型：long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| long long | dividend / divisor 的余数 | 除数非零时正常返回 |

### osal_div64_u64_rem <a id="osal_div64_u64_rem"></a>

```c
unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 实现无符号64位整数除以无符号64位整数的除法运算，返回余数
- 返回被除数除以除数的余数
- 调用者须确保除数不为零，否则行为未定义

**前置条件**

- 除数 divisor 不为 0
- 支持 linux、liteos 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dividend | unsigned long long | 无符号64位被除数 | 0 ~ 18446744073709551615 |
| divisor | unsigned long long | 无符号64位除数 | 1 ~ 18446744073709551615 |

**返回值**

- 返回类型：unsigned long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned long long | dividend / divisor 的余数 | 除数非零时正常返回 |

### osal_get_random_int <a id="osal_get_random_int"></a>

```c
unsigned int osal_get_random_int(void)
```

**头文件清单**

```c
#include "kernel/osal/include/math/osal_math.h"
```

**功能说明**

- 生成无符号32位随机数
- 返回随机数值
- 支持 linux、liteos 系统

**前置条件**

- 支持 linux、liteos 系统

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| unsigned int | 随机数 | 正常调用 |

## Macros

### osal_max <a id="osal_max"></a>

```c
#define osal_max(x, y)                 \
    ({                                 \
        __typeof__(x)_max1 = (x);      \
        __typeof__(y)_max2 = (y);      \
        (void)(&_max1 == &_max2);      \
        _max1 > _max2 ? _max1 : _max2; \
    })
```

**使用说明**

用于取两个值中的较大值，x 与 y 须为同类型表达式

### osal_min <a id="osal_min"></a>

```c
#define osal_min(x, y)                 \
    ({                                 \
        __typeof__(x)_min1 = (x);      \
        __typeof__(y)_min2 = (y);      \
        (void)(&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; \
    })
```

**使用说明**

用于取两个值中的较小值，x 与 y 须为同类型表达式

### osal_abs <a id="osal_abs"></a>

```c
#define osal_abs(x)                           \
    ({                                        \
        long ret;                             \
        if (sizeof(x) == sizeof(long)) {      \
            long __x = (x);                   \
            ret = (__x < 0) ? (-__x) : (__x); \
        } else {                              \
            int __x = (x);                    \
            ret = (__x < 0) ? (-__x) : (__x); \
        }                                     \
        ret;                                  \
    })
```

**使用说明**

用于取绝对值，返回 long 类型结果


