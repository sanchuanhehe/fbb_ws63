# Fileops

fileops 提供内核级文件 I/O 操作的 OSAL (Operating System Abstraction Layer) 抽象接口，支持 POSIX (Portable Operating System Interface) 风格的文件打开、关闭、读写、同步、定位、删除与截断操作。接口通过 void * 文件指针管理文件生命周期，定义了标准文件打开标志与寻址基准模式。

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_klib_fopen](#osal_klib_fopen) | 打开文件并返回文件指针 |
| [osal_klib_fclose](#osal_klib_fclose) | 关闭文件 |
| [osal_klib_fwrite](#osal_klib_fwrite) | 向文件写入数据 |
| [osal_klib_fread](#osal_klib_fread) | 从文件读取数据 |
| [osal_klib_fsync](#osal_klib_fsync) | 将文件数据同步到存储设备 |
| [osal_klib_fseek](#osal_klib_fseek) | 设置文件读写位置 |
| [osal_klib_unlink](#osal_klib_unlink) | 删除文件或空目录 |
| [osal_klib_ftruncate](#osal_klib_ftruncate) | 将文件截断到指定长度 |

## Functions

### osal_klib_fopen <a id="osal_klib_fopen"></a>

```c
void *osal_klib_fopen(const char *file, int flags, int mode)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 打开指定路径的文件，根据标志位和模式参数控制文件的打开方式
- 返回文件指针用于后续文件操作接口的入参
- 文件指针由内核动态分配内存管理，调用者需通过 osal_klib_fclose 释放

**前置条件**

- 文件系统已初始化且可访问
- 入参 file 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| file | const char * | 文件路径字符串 | 非 NULL |
| flags | int | 文件操作标志，控制打开方式；访问模式（RDONLY / WRONLY / RDWR）互斥且必选其一，其余标志可组合 | [OSAL_O_RDONLY](#OSAL_O_RDONLY)(00000000) / [OSAL_O_WRONLY](#OSAL_O_WRONLY)(00000001) / [OSAL_O_RDWR](#OSAL_O_RDWR)(00000002) / [OSAL_O_CREAT](#OSAL_O_CREAT)(00000100) / [OSAL_O_EXCL](#OSAL_O_EXCL)(00000200) / [OSAL_O_TRUNC](#OSAL_O_TRUNC)(00001000) / [OSAL_O_APPEND](#OSAL_O_APPEND)(00002000) / [OSAL_O_CLOEXEC](#OSAL_O_CLOEXEC)(02000000) |
| mode | int | 文件创建权限位，当 flags 含 OSAL_O_CREAT 时生效 | 权限位组合（如 0666 / 0644 / 0600） |

**返回值**

- 返回类型：void *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非 NULL 指针 | 文件打开成功 | 文件路径合法且文件系统可用 |
| NULL | 文件打开失败 | file 为 NULL、文件不存在且未指定 OSAL_O_CREAT、存储空间不足 |

### osal_klib_fclose <a id="osal_klib_fclose"></a>

```c
void osal_klib_fclose(void *filp)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 关闭由 osal_klib_fopen 打开的文件
- 释放文件指针占用的内核内存资源
- 关闭后 filp 指针不可再用于其他文件操作接口

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |

### osal_klib_fwrite <a id="osal_klib_fwrite"></a>

```c
int osal_klib_fwrite(const char *buf, unsigned long size, void *filp)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 将缓冲区数据写入文件
- 从当前文件读写位置开始写入，写入完成后读写位置自动后移
- 返回实际写入的字节数

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针
- 文件以写模式（OSAL_O_WRONLY 或 OSAL_O_RDWR）打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| buf | const char * | 待写入数据缓冲区 | 非 NULL |
| size | unsigned long | 待写入数据字节数 | > 0 |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| > 0 | 实际写入的字节数 | 写入成功 |
| -1 | 写入失败 | filp 或 buf 为 NULL |

### osal_klib_fread <a id="osal_klib_fread"></a>

```c
int osal_klib_fread(char *buf, unsigned long size, void *filp)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 从文件读取数据到缓冲区
- 从当前文件读写位置开始读取，读取完成后读写位置自动后移
- 返回实际读取的字节数

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针
- 文件以读模式（OSAL_O_RDONLY 或 OSAL_O_RDWR）打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| size | unsigned long | 待读取数据字节数 | > 0 |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buf | char * | 读取的文件数据，由调用方分配缓冲区且长度不小于 size，函数填充 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| > 0 | 实际读取的字节数 | 读取成功 |
| -1 | 读取失败 | filp 或 buf 为 NULL |

### osal_klib_fsync <a id="osal_klib_fsync"></a>

```c
void osal_klib_fsync(void *filp)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 将文件缓存数据同步到存储设备
- 确保数据持久化写入底层存储介质
- 仅支持 linux、liteos 系统

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针
- 文件已写入数据且需持久化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |

### osal_klib_fseek <a id="osal_klib_fseek"></a>

```c
int osal_klib_fseek(long long offset, int whence, void *filp)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 设置文件读写位置偏移量
- 根据 whence 参数确定偏移基准位置
- offset 值超过 INT32_MAX 时返回 OSAL_EOVERFLOW

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| offset | long long | 偏移量 | ≤ INT32_MAX |
| whence | int | 偏移基准位置 | [OSAL_SEEK_SET](#OSAL_SEEK_SET)(0) / [OSAL_SEEK_CUR](#OSAL_SEEK_CUR)(1) / [OSAL_SEEK_END](#OSAL_SEEK_END)(2) |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >= 0 | 新的文件读写位置 | 设置成功 |
| -1 | 设置失败 | filp 为 NULL 或 offset 超过 INT32_MAX |
| -75 | 偏移结果溢出 | lseek 返回值超过 int 表达范围 |

### osal_klib_unlink <a id="osal_klib_unlink"></a>

```c
int osal_klib_unlink(const char *path)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 删除指定路径的文件或目录
- 删除目录时目录必须为空，否则操作失败
- 仅支持 seliteos 系统

**前置条件**

- 文件系统已挂载且可访问
- 入参 path 不为 NULL
- 若删除目录，目录必须为空

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| path | const char * | 文件或目录路径字符串 | 非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 删除成功 | 文件或空目录删除成功 |
| 其他值 | 删除失败 | 路径不存在、目录非空、权限不足 |

### osal_klib_ftruncate <a id="osal_klib_ftruncate"></a>

```c
int osal_klib_ftruncate(void *filp, unsigned long len)
```

**头文件清单**

```c
#include "kernel/osal/include/fileops/osal_fileops.h"
```

**功能说明**

- 将文件大小截断到指定长度
- 若指定长度小于当前文件大小，超出部分数据被丢弃
- 若指定长度大于当前文件大小，文件扩展并以零填充
- 仅支持 seliteos 系统

**前置条件**

- filp 为 osal_klib_fopen 返回的有效文件指针
- 文件以写模式打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| filp | void * | 文件指针 | osal_klib_fopen 返回的有效指针，非 NULL |
| len | unsigned long | 目标文件长度 | > 0 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 截断成功 | 文件大小调整成功 |
| 其他值 | 截断失败 | filp 无效、文件系统错误 |

## Macros

### OSAL_O_RDONLY <a id="OSAL_O_RDONLY"></a>

```c
#define OSAL_O_RDONLY 00000000
```

### OSAL_O_WRONLY <a id="OSAL_O_WRONLY"></a>

```c
#define OSAL_O_WRONLY 00000001
```

### OSAL_O_RDWR <a id="OSAL_O_RDWR"></a>

```c
#define OSAL_O_RDWR 00000002
```

### OSAL_O_ACCMODE <a id="OSAL_O_ACCMODE"></a>

```c
#define OSAL_O_ACCMODE 00000003
```

### OSAL_O_CREAT <a id="OSAL_O_CREAT"></a>

```c
#define OSAL_O_CREAT 00000100
```

### OSAL_O_EXCL <a id="OSAL_O_EXCL"></a>

```c
#define OSAL_O_EXCL 00000200
```

### OSAL_O_TRUNC <a id="OSAL_O_TRUNC"></a>

```c
#define OSAL_O_TRUNC 00001000
```

### OSAL_O_APPEND <a id="OSAL_O_APPEND"></a>

```c
#define OSAL_O_APPEND 00002000
```

### OSAL_O_CLOEXEC <a id="OSAL_O_CLOEXEC"></a>

```c
#define OSAL_O_CLOEXEC 02000000
```

### OSAL_SEEK_SET <a id="OSAL_SEEK_SET"></a>

```c
#define OSAL_SEEK_SET 0
```

### OSAL_SEEK_CUR <a id="OSAL_SEEK_CUR"></a>

```c
#define OSAL_SEEK_CUR 1
```

### OSAL_SEEK_END <a id="OSAL_SEEK_END"></a>

```c
#define OSAL_SEEK_END 2
```

