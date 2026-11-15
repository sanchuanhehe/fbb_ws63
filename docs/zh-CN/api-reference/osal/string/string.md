# String

内核字符串与内存操作工具集。提供比较（strcmp/strncmp/memcmp）、查找（strchr/strstr/memchr）、长度计算（strlen/strnlen）、复制（strcpy/strncpy/memcpy/memmove）、格式化输出（snprintf/sscanf）等标准 C 库函数的 OSAL (Operating System Abstraction Layer) 封装。

确保在不同 OS (Operating System) 内核下行为一致，避免直接调用平台相关 C 库实现。

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_memncmp](#osal_memncmp) | 比较两块内存区域的前size字节 |
| [osal_strcmp](#osal_strcmp) | 比较两个字符串 |
| [osal_strncmp](#osal_strncmp) | 比较两个字符串的前size字节 |
| [osal_strcasecmp](#osal_strcasecmp) | 忽略大小写比较两个字符串 |
| [osal_strncasecmp](#osal_strncasecmp) | 忽略大小写比较两个字符串的前size字节 |
| [osal_strchr](#osal_strchr) | 在字符串中查找指定字符的首次出现位置 |
| [osal_strnchr](#osal_strnchr) | 在限长字符串中查找指定字符的首次出现位置 |
| [osal_strrchr](#osal_strrchr) | 在字符串中查找指定字符的最后一次出现位置 |
| [osal_strstr](#osal_strstr) | 在字符串中查找子串的首次出现位置 |
| [osal_strnstr](#osal_strnstr) | 在限长字符串中查找子串的首次出现位置 |
| [osal_strlen](#osal_strlen) | 计算字符串长度 |
| [osal_strnlen](#osal_strnlen) | 计算限长字符串长度 |
| [osal_strpbrk](#osal_strpbrk) | 在字符串中搜索字符集中任意字符的首次出现位置 |
| [osal_strsep](#osal_strsep) | 从字符串中提取分隔标记 |
| [osal_strspn](#osal_strspn) | 计算字符串前缀中仅包含指定字符集的长度 |
| [osal_strcspn](#osal_strcspn) | 计算字符串前缀中不包含指定字符集的长度 |
| [osal_memscan](#osal_memscan) | 在内存区域中查找指定字节 |
| [osal_memcmp](#osal_memcmp) | 比较两块内存区域的前count字节 |
| [osal_memchr](#osal_memchr) | 在内存区域中查找指定字节的首次出现位置 |
| [osal_memchr_inv](#osal_memchr_inv) | 在内存区域中查找非指定字节的首次出现位置 |
| [osal_strtoull](#osal_strtoull) | 将字符串转换为unsigned long long整型 |
| [osal_strtoul](#osal_strtoul) | 将字符串转换为unsigned long整型 |
| [osal_strtol](#osal_strtol) | 将字符串转换为long整型 |
| [osal_strtoll](#osal_strtoll) | 将字符串转换为long long整型 |

## Functions

### osal_memncmp <a id="osal_memncmp"></a>

```c
int osal_memncmp(const void *buf1, const void *buf2, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 比较两块内存区域buf1和buf2的前size字节内容
- 实际调用libc的memcmp函数完成比较
- 支持系统：linux liteos

**前置条件**

- 参数buf1和buf2指向有效内存区域，且长度不小于size
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buf1 | const void * | 待比较的内存区域指针 | 非NULL，指向有效内存且长度不小于size |
| buf2 | const void * | 待比较的内存区域指针 | 非NULL，指向有效内存且长度不小于size |
| size | unsigned long | 比较的字节数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | buf1小于buf2 | buf1前size字节内容小于buf2 |
| 0 | buf1等于buf2 | buf1前size字节内容与buf2完全一致 |
| > 0 | buf1大于buf2 | buf1前size字节内容大于buf2 |

### osal_strcmp <a id="osal_strcmp"></a>

```c
int osal_strcmp(const char *s1, const char *s2)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 比较两个字符串s1和s2
- 实际调用libc的strcmp函数完成比较
- 支持系统：linux liteos freertos

**前置条件**

- 参数s1和s2指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | s1小于s2 | s1字典序小于s2 |
| 0 | s1等于s2 | s1与s2完全一致 |
| > 0 | s1大于s2 | s1字典序大于s2 |

### osal_strncmp <a id="osal_strncmp"></a>

```c
int osal_strncmp(const char *s1, const char *s2, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 比较两个字符串s1和s2的前size字节
- 实际调用libc的strncmp函数完成比较
- 支持系统：linux liteos

**前置条件**

- 参数s1和s2指向有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| size | unsigned long | 比较的字节数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | s1小于s2 | s1前size字节字典序小于s2 |
| 0 | s1等于s2 | s1前size字节与s2完全一致 |
| > 0 | s1大于s2 | s1前size字节字典序大于s2 |

### osal_strcasecmp <a id="osal_strcasecmp"></a>

```c
int osal_strcasecmp(const char *s1, const char *s2)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 忽略大小写逐字节比较字符串s1和s2
- 实际调用libc的strcasecmp函数完成比较
- 支持系统：linux liteos freertos

**前置条件**

- 参数s1和s2指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | s1小于s2 | 忽略大小写后s1字典序小于s2 |
| 0 | s1等于s2 | 忽略大小写后s1与s2完全一致 |
| > 0 | s1大于s2 | 忽略大小写后s1字典序大于s2 |

### osal_strncasecmp <a id="osal_strncasecmp"></a>

```c
int osal_strncasecmp(const char *s1, const char *s2, unsigned long size)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 忽略大小写逐字节比较字符串s1和s2的前size字节
- 实际调用libc的strncasecmp函数完成比较
- 支持系统：linux liteos

**前置条件**

- 参数s1和s2指向有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待比较的字符串指针 | 非NULL，以'\0'结尾 |
| size | unsigned long | 比较的字节数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | s1小于s2 | 忽略大小写后s1前size字节字典序小于s2 |
| 0 | s1等于s2 | 忽略大小写后s1前size字节与s2完全一致 |
| > 0 | s1大于s2 | 忽略大小写后s1前size字节字典序大于s2 |

### osal_strchr <a id="osal_strchr"></a>

```c
char *osal_strchr(const char *s, int n)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s中查找字符n的首次出现位置
- 实际调用libc的strchr函数完成查找
- 支持系统：linux liteos

**前置条件**

- 参数s指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| n | int | 待查找的字符 | - |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向字符n首次出现的位置 | 在字符串s中找到字符n |
| NULL | 未找到指定字符 | 字符串s中不存在字符n |

### osal_strnchr <a id="osal_strnchr"></a>

```c
char *osal_strnchr(const char *s, int count, int c)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s的前count个字符范围内查找字符c的首次出现位置
- 当s为NULL或count小于等于0时直接返回NULL
- 支持系统：linux liteos

**前置条件**

- 参数s指向以'\0'结尾的有效字符串
- count大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| count | int | 搜索的字符数 | > 0 |
| c | int | 待查找的字符 | - |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向字符c首次出现的位置 | 在字符串s前count个字符中找到字符c |
| NULL | 未找到指定字符 | 字符串s前count个字符中不存在字符c，或s为NULL，或count小于等于0 |

### osal_strrchr <a id="osal_strrchr"></a>

```c
char *osal_strrchr(const char *s, int c)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s中查找字符c的最后一次出现位置
- 实际调用libc的strrchr函数完成查找
- 支持系统：linux liteos

**前置条件**

- 参数s指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| c | int | 待查找的字符 | - |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向字符c最后一次出现的位置 | 在字符串s中找到字符c |
| NULL | 未找到指定字符 | 字符串s中不存在字符c |

### osal_strstr <a id="osal_strstr"></a>

```c
char *osal_strstr(const char *s1, const char *s2)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s1中查找子串s2的首次出现位置
- 实际调用libc的strstr函数完成查找
- 支持系统：linux liteos

**前置条件**

- 参数s1和s2指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待查找的子串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向子串s2在s1中首次出现的位置 | 在字符串s1中找到子串s2 |
| NULL | 未找到指定子串 | 字符串s1中不存在子串s2 |

### osal_strnstr <a id="osal_strnstr"></a>

```c
char *osal_strnstr(const char *s1, const char *s2, int n)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s1的前n个字符范围内查找子串s2的首次出现位置
- 当s1或s2为NULL时直接返回NULL
- 支持系统：linux liteos

**前置条件**

- 参数s1和s2指向以'\0'结尾的有效字符串
- n不小于s2的长度

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 待查找的子串指针 | 非NULL，以'\0'结尾 |
| n | int | 搜索的字符串长度 | - |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向子串s2在s1前n个字符中首次出现的位置 | 在s1前n个字符中找到子串s2 |
| NULL | 未找到指定子串 | s1前n个字符中不存在子串s2，或s1为NULL，或s2为NULL，或n小于s2长度 |

### osal_strlen <a id="osal_strlen"></a>

```c
unsigned int osal_strlen(const char *s)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 计算字符串s的长度，不包括终止符'\0'
- 实际调用libc的strlen函数完成计算
- 支持系统：linux liteos freertos

**前置条件**

- 参数s指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待计算长度的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 字符串长度 | 字符串s中字符的数量（不含'\0'） | 字符串s有效 |

### osal_strnlen <a id="osal_strnlen"></a>

```c
unsigned int osal_strnlen(const char *s, unsigned int size)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 计算字符串s的长度，最多检查前size个字符，不包括终止符'\0'
- 实际调用libc的strnlen函数完成计算
- 支持系统：linux liteos

**前置条件**

- 参数s指向以'\0'结尾的有效字符串或至少有size个字符的内存
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待计算长度的字符串指针 | 非NULL |
| size | unsigned int | 最大检查字符数 | - |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 字符串长度 | 字符串s中字符的数量（不含'\0'），最多为size | 字符串s有效 |

### osal_strpbrk <a id="osal_strpbrk"></a>

```c
char *osal_strpbrk(const char *s1, const char *s2)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在字符串s1中搜索字符集s2中任意字符的首次出现位置
- 实际调用libc的strpbrk函数完成查找
- 支持系统：linux liteos

**前置条件**

- 参数s1和s2指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s1 | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| s2 | const char * | 包含待搜索字符集的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向s1中匹配s2字符集任意字符的首次出现位置 | 在s1中找到s2字符集中的字符 |
| NULL | 未找到匹配字符 | s1中不存在s2字符集中的任何字符 |

### osal_strsep <a id="osal_strsep"></a>

```c
char *osal_strsep(char **s, const char *ct)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 从字符串s中提取以ct中字符为分隔符的标记
- 实际调用libc的strsep函数完成提取
- 支持系统：linux liteos

**前置条件**

- 参数s指向有效的字符串指针
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | char ** | 待搜索的字符串指针的指针 | 非NULL |
| ct | const char * | 分隔字符集字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向提取的标记起始位置 | 成功提取到标记 |
| NULL | 无更多标记可提取 | 字符串已完全解析 |

### osal_strspn <a id="osal_strspn"></a>

```c
unsigned int osal_strspn(const char *s, const char *accept)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 计算字符串s的前缀中仅包含accept字符集字符的长度
- 实际调用libc的strspn函数完成计算
- 支持系统：linux liteos

**前置条件**

- 参数s和accept指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| accept | const char * | 包含允许字符集的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 前缀长度 | 字符串s前缀中仅由accept字符集组成的字节数 | 字符串s有效 |

### osal_strcspn <a id="osal_strcspn"></a>

```c
unsigned int osal_strcspn(const char *s, const char *reject)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 计算字符串s的前缀中不包含reject字符集字符的长度
- 实际调用libc的strcspn函数完成计算
- 支持系统：linux liteos

**前置条件**

- 参数s和reject指向以'\0'结尾的有效字符串
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const char * | 待搜索的字符串指针 | 非NULL，以'\0'结尾 |
| reject | const char * | 包含排除字符集的字符串指针 | 非NULL，以'\0'结尾 |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 前缀长度 | 字符串s前缀中不包含reject字符集字符的字节数 | 字符串s有效 |

### osal_memscan <a id="osal_memscan"></a>

```c
void *osal_memscan(void *addr, int c, int size)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在内存区域addr中查找字节c
- 当前实现直接返回NULL，未提供实际扫描功能
- 支持系统：linux liteos

**前置条件**

- 参数addr指向有效的内存区域

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | void * | 待搜索的内存区域指针 | 非NULL |
| c | int | 待查找的字节值 | - |
| size | int | 内存区域大小 | - |

**返回值**

- 返回类型：void *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 当前实现未提供实际扫描功能 | 任何调用 |

### osal_memcmp <a id="osal_memcmp"></a>

```c
int osal_memcmp(const void *cs, const void *ct, int count)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 比较两块内存区域cs和ct的前count字节
- 实际调用libc的memcmp函数完成比较
- 支持系统：linux liteos freertos

**前置条件**

- 参数cs和ct指向有效内存区域，且长度不小于count
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cs | const void * | 待比较的内存区域指针 | 非NULL，长度不小于count |
| ct | const void * | 待比较的内存区域指针 | 非NULL，长度不小于count |
| count | int | 比较的字节数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| < 0 | cs小于ct | cs前count字节内容小于ct |
| 0 | cs等于ct | cs前count字节内容与ct完全一致 |
| > 0 | cs大于ct | cs前count字节内容大于ct |

### osal_memchr <a id="osal_memchr"></a>

```c
void *osal_memchr(const void *s, int c, int n)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在内存区域s的前n个字节中查找字节c的首次出现位置
- 实际调用libc的memchr函数完成查找
- 支持系统：linux liteos

**前置条件**

- 参数s指向有效内存区域，且长度不小于n
- 参数应满足libc接口调用的合法性要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const void * | 待搜索的内存区域指针 | 非NULL，长度不小于n |
| c | int | 待查找的字节值 | - |
| n | int | 搜索的字节数 | - |

**返回值**

- 返回类型：void *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 指向字节c首次出现的位置 | 在内存区域s前n个字节中找到字节c |
| NULL | 未找到指定字节 | 内存区域s前n个字节中不存在字节c |

### osal_memchr_inv <a id="osal_memchr_inv"></a>

```c
void *osal_memchr_inv(const void *s, int c, int n)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 在内存区域s的前n个字节中查找非字节c的首次出现位置
- 当前实现直接返回NULL，未提供实际查找功能
- 支持系统：linux liteos

**前置条件**

- 参数s指向有效内存区域

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s | const void * | 待搜索的内存区域指针 | 非NULL |
| c | int | 对比排除的字节值 | - |
| n | int | 搜索的字节数 | - |

**返回值**

- 返回类型：void *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NULL | 当前实现未提供实际查找功能 | 任何调用 |

### osal_strtoull <a id="osal_strtoull"></a>

```c
unsigned long long osal_strtoull(const char *cp, char **endp, unsigned int base)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 将字符串cp按照指定进制base转换为unsigned long long整型值
- 当前实现直接返回0，未提供实际转换功能
- 支持系统：linux

**前置条件**

- 参数cp指向有效字符串
- base取值范围为2至36或特殊值0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cp | const char * | 待转换的字符串指针 | 非NULL |
| endp | char ** | 存储第一个无效字符地址的指针 | 可为NULL |
| base | unsigned int | 转换进制 | 2~36或0（0表示自动推断） |

**返回值**

- 返回类型：unsigned long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 当前实现固定返回0 | 任何调用 |

### osal_strtoul <a id="osal_strtoul"></a>

```c
unsigned long osal_strtoul(const char *cp, char **endp, unsigned int base)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 将字符串cp按照指定进制base转换为unsigned long整型值
- 实际调用libc的strtoul函数完成转换
- 支持系统：linux liteos

**前置条件**

- 参数cp指向有效字符串
- 参数应满足libc接口调用的合法性要求
- base取值范围为2至36或特殊值0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cp | const char * | 待转换的字符串指针 | 非NULL |
| endp | char ** | 存储第一个无效字符地址的指针 | 可为NULL |
| base | unsigned int | 转换进制 | 2~36或0（0表示自动推断） |

**返回值**

- 返回类型：unsigned long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 转换结果 | 字符串转换后的无符号长整型值 | 转换成功 |
| ULONG_MAX | 转换溢出 | 转换值超出unsigned long范围，errno设置为ERANGE |
| 0 | 无有效数字可转换 | 字符串中无有效数字 |

### osal_strtol <a id="osal_strtol"></a>

```c
long osal_strtol(const char *cp, char **endp, unsigned int base)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 将字符串cp按照指定进制base转换为long整型值
- 实际调用内核的simple_strtol函数完成转换
- 支持系统：linux liteos

**前置条件**

- 参数cp指向有效字符串
- base取值范围为2至36或特殊值0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cp | const char * | 待转换的字符串指针 | 非NULL |
| endp | char ** | 存储第一个无效字符地址的指针 | 可为NULL |
| base | unsigned int | 转换进制 | 2~36或0（0表示自动推断） |

**返回值**

- 返回类型：long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 转换结果 | 字符串转换后的长整型值 | 转换成功 |
| LONG_MAX | 转换正溢出 | 转换值超出long范围正上限 |
| LONG_MIN | 转换负溢出 | 转换值超出long范围负下限 |
| 0 | 无有效数字可转换 | 字符串中无有效数字 |

### osal_strtoll <a id="osal_strtoll"></a>

```c
long long osal_strtoll(const char *cp, char **endp, unsigned int base)
```

**头文件清单**

```c
#include "kernel/osal/include/string/osal_string.h"
```

**功能说明**

- 将字符串cp按照指定进制base转换为long long整型值
- 当前实现直接返回0，未提供实际转换功能
- 支持系统：linux

**前置条件**

- 参数cp指向有效字符串
- base取值范围为2至36或特殊值0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cp | const char * | 待转换的字符串指针 | 非NULL |
| endp | char ** | 存储第一个无效字符地址的指针 | 可为NULL |
| base | unsigned int | 转换进制 | 2~36或0（0表示自动推断） |

**返回值**

- 返回类型：long long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 当前实现固定返回0 | 任何调用 |

## Type definitions

### OSAL_BASE_DEC <a id="macro_OSAL_BASE_DEC"></a>

```c
#define OSAL_BASE_DEC 10
```

**使用说明**

用于osal_strtoul、osal_strtol、osal_strtoull、osal_strtoll接口的base参数，表示十进制转换

### OSAL_BASE_HEX <a id="macro_OSAL_BASE_HEX"></a>

```c
#define OSAL_BASE_HEX 16
```

**使用说明**

用于osal_strtoul、osal_strtol、osal_strtoull、osal_strtoll接口的base参数，表示十六进制转换


