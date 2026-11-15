# Proc

简化版 /proc 文件系统接口，用于在运行时创建可调试的伪文件条目。支持在 /proc 下创建目录和文件，每个文件可注册 open/read/write 回调，通过文件读写实现运行时信息查询和参数调整。

适用调试阶段暴露内核状态、运行时配置参数、性能计数器等场景。

**头文件清单**
```c
#include "kernel/osal/include/proc/osal_proc.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_proc_init](#osal_proc_init) | 在 /proc 目录下创建目录 |
| [osal_proc_exit](#osal_proc_exit) | 删除由 osal_proc_init() 创建的目录及其下所有文件 |
| [osal_remove_proc_entry](#osal_remove_proc_entry) | 删除由 osal_create_proc_entry() 创建的文件 |
| [osal_create_proc_entry](#osal_create_proc_entry) | 在由 osal_proc_init() 创建的目录下创建文件 |

## Functions

### osal_proc_init <a id="osal_proc_init"></a>

```c
void osal_proc_init(const char *name)
```

**头文件清单**

```c
#include "kernel/osal/include/proc/osal_proc.h"
```

**功能说明**

- 在 /proc 目录下创建指定名称的目录
- 该接口仅允许调用一次，即在 /proc 目录下同一时间只能存在一个由用户创建的目录
- 支持 Linux 和 LiteOS (Huawei LiteOS) 系统

**前置条件**

- 模块初始化状态：LOSCFG_FS_PROC 宏已开启，osal_proc.c 已参与编译
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 待创建的目录名称 | 非NULL，字符串长度不超过 OSAL_PROC_NAME_LENGTH(32) |

**返回值**

- 返回类型：void

### osal_proc_exit <a id="osal_proc_exit"></a>

```c
void osal_proc_exit(const char *name)
```

**头文件清单**

```c
#include "kernel/osal/include/proc/osal_proc.h"
```

**功能说明**

- 删除由 osal_proc_init() 创建的目录及其下所有文件
- 调用后将清理该目录下的全部 proc 文件条目
- 支持 Linux 和 LiteOS 系统

**前置条件**

- 模块初始化状态：已通过 osal_proc_init() 成功创建对应目录
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 待删除的目录名称，与 osal_proc_init() 传入的名称一致 | 非NULL，字符串长度不超过 OSAL_PROC_NAME_LENGTH(32) |

**返回值**

- 返回类型：void

### osal_remove_proc_entry <a id="osal_remove_proc_entry"></a>

```c
void osal_remove_proc_entry(const char *name, osal_proc_entry *parent)
```

**头文件清单**

```c
#include "kernel/osal/include/proc/osal_proc.h"
```

**功能说明**

- 删除由 osal_create_proc_entry() 创建的文件
- 删除后该文件条目不再存在于 /proc 目录结构中
- 支持 Linux 和 LiteOS 系统

**前置条件**

- 模块初始化状态：已通过 osal_proc_init() 成功创建目录，且已通过 osal_create_proc_entry() 在该目录下创建文件
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 待删除的文件名称，与 osal_create_proc_entry() 的第一个参数一致 | 非NULL |
| parent | [osal_proc_entry](#struct_osal_proc_dir_entry) * | 文件所在的父目录条目，由 osal_create_proc_entry() 返回 | 非NULL，须为有效的 osal_proc_entry 指针 |

**返回值**

- 返回类型：void

### osal_create_proc_entry <a id="osal_create_proc_entry"></a>

```c
osal_proc_entry *osal_create_proc_entry(const char *name, osal_proc_entry *parent)
```

**头文件清单**

```c
#include "kernel/osal/include/proc/osal_proc.h"
```

**功能说明**

- 在由 osal_proc_init() 创建的目录下创建文件
- 创建成功后返回 osal_proc_entry 指针，代表 "/proc/xxx" 父目录，其中 xxx 为 osal_proc_init() 的第一个参数
- 通过返回的 osal_proc_entry 可注册 open/read/write 回调及命令列表，实现文件交互操作
- 支持 Linux 和 LiteOS 系统

**前置条件**

- 模块初始化状态：已通过 osal_proc_init() 成功创建目录
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 待创建的文件名称 | 非NULL，字符串长度不超过 OSAL_PROC_NAME_LENGTH(32) |
| parent | [osal_proc_entry](#struct_osal_proc_dir_entry) * | 保留参数，当前未使用 | NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| 返回值 | [osal_proc_entry](#struct_osal_proc_dir_entry) * | 创建成功返回有效的 osal_proc_entry 指针；创建失败返回 NULL |

**返回值**

- 返回类型：osal_proc_entry *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 创建成功 | 文件条目创建成功 |
| NULL | 创建失败 | 文件条目创建失败 |

## Structures

### osal_proc_entry <a id="struct_osal_proc_dir_entry"></a>

```c
typedef struct osal_proc_dir_entry {
    char name[OSAL_PROC_NAME_LENGTH];
    unsigned int cmd_cnt;
    osal_proc_cmd *cmd_list;
    void *proc_dir_entry;
    int (*open)(struct osal_proc_dir_entry *entry);
    int (*read)(struct osal_proc_dir_entry *entry);
    int (*write)(struct osal_proc_dir_entry *entry, const char *buf, int count, long long *);
    void *private_data;
    void *seqfile;
    struct osal_list_head node;
} osal_proc_entry;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| name | char[OSAL_PROC_NAME_LENGTH] | proc 条目名称，最大长度 32 |
| cmd_cnt | unsigned int | 命令列表中命令的数量 |
| cmd_list | [osal_proc_cmd](#struct_osal_proc_cmd_) * | 命令列表指针 |
| proc_dir_entry | void * | 底层 proc 目录条目指针 |
| open | int (*)(struct osal_proc_dir_entry *) | 文件打开回调函数 |
| read | int (*)(struct osal_proc_dir_entry *) | 文件读取回调函数 |
| write | int (*)(struct osal_proc_dir_entry *, const char *, int, long long *) | 文件写入回调函数 |
| private_data | void * | 私有数据指针 |
| seqfile | void * | 序列文件指针 |
| node | struct osal_list_head | 链表节点 |

### osal_proc_cmd <a id="struct_osal_proc_cmd_"></a>

```c
typedef struct osal_proc_cmd_ {
    char name[OSAL_PROC_NAME_LENGTH];
    int (*handler)(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private_data);
} osal_proc_cmd;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| name | char[OSAL_PROC_NAME_LENGTH] | 命令名称，最大长度 32 |
| handler | int (*)(unsigned int, char (*)[PROC_CMD_SINGEL_LENGTH_MAX], void *) | 命令处理回调函数 |


