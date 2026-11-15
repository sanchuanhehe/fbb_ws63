# OSAL List

OSAL (Operating System Abstraction Layer) List 提供双向链表的初始化、节点插入/删除、替换、移动、合并、切割及状态查询等操作，用于内核与驱动模块中的链表数据结构管理。

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [OSAL_INIT_LIST_HEAD](#OSAL_INIT_LIST_HEAD) | 初始化双向链表头节点 |
| [osal___list_add](#osal___list_add) | 在两个已知连续节点之间插入新节点 |
| [osal_list_add](#osal_list_add) | 在指定链表头节点之后添加新节点 |
| [osal_list_add_tail](#osal_list_add_tail) | 在指定链表尾节点之前添加新节点 |
| [osal___list_del](#osal___list_del) | 通过使prev和next节点互相指向来删除中间节点 |
| [osal___list_del_entry](#osal___list_del_entry) | 删除链表中的指定节点 |
| [osal_list_del](#osal_list_del) | 从链表中删除节点并标记为毒值 |
| [osal_list_replace](#osal_list_replace) | 用新节点替换链表中的旧节点 |
| [osal_list_replace_init](#osal_list_replace_init) | 用新节点替换旧节点并重新初始化旧节点 |
| [osal_list_del_init](#osal_list_del_init) | 从链表中删除节点并重新初始化该节点 |
| [osal_list_move](#osal_list_move) | 将节点从一个链表删除并添加到另一个链表头部 |
| [osal_list_move_tail](#osal_list_move_tail) | 将节点从一个链表删除并添加到另一个链表尾部 |
| [osal_list_is_last](#osal_list_is_last) | 判断节点是否为链表的最后一个节点 |
| [osal_list_empty](#osal_list_empty) | 判断链表是否为空 |
| [osal_list_empty_careful](#osal_list_empty_careful) | 判断链表是否为空且未被其他CPU修改 |
| [osal_list_rotate_left](#osal_list_rotate_left) | 将链表向左旋转，首节点移到尾部 |
| [osal_list_is_singular](#osal_list_is_singular) | 判断链表中是否只有一个节点 |
| [osal___list_cut_position](#osal___list_cut_position) | 将链表从指定位置切割为两个链表 |
| [osal_list_cut_position](#osal_list_cut_position) | 将链表从指定位置切割为两个链表 |
| [osal___list_splice](#osal___list_splice) | 将一个链表合并到另一个链表的指定位置 |
| [osal_list_splice](#osal_list_splice) | 将链表合并到目标链表头部 |
| [osal_list_splice_tail](#osal_list_splice_tail) | 将链表合并到目标链表尾部 |
| [osal_list_splice_init](#osal_list_splice_init) | 将链表合并到目标链表头部并重新初始化源链表 |
| [osal_list_splice_tail_init](#osal_list_splice_tail_init) | 将链表合并到目标链表尾部并重新初始化源链表 |
| [INIT_OSAL_HLIST_NODE](#INIT_OSAL_HLIST_NODE) | 初始化哈希链表节点 |
| [osal_hlist_unhashed](#osal_hlist_unhashed) | 判断哈希链表节点是否未被哈希 |
| [osal_hlist_empty](#osal_hlist_empty) | 判断哈希链表是否为空 |
| [osal___hlist_del](#osal___hlist_del) | 删除哈希链表中的节点 |
| [osal_hlist_del](#osal_hlist_del) | 删除哈希链表节点并标记为毒值 |
| [osal_hlist_del_init](#osal_hlist_del_init) | 删除哈希链表节点并重新初始化该节点 |
| [osal_hlist_add_head](#osal_hlist_add_head) | 在哈希链表头部添加节点 |
| [osal_hlist_add_before](#osal_hlist_add_before) | 在指定哈希链表节点之前添加新节点 |
| [osal_hlist_add_after](#osal_hlist_add_after) | 在指定哈希链表节点之后添加新节点 |
| [osal_hlist_add_fake](#osal_hlist_add_fake) | 为哈希链表节点设置假pprev使其可被删除 |
| [osal_hlist_move_list](#osal_hlist_move_list) | 将哈希链表从一个头节点迁移到另一个头节点 |

## Functions

### OSAL_INIT_LIST_HEAD <a id="OSAL_INIT_LIST_HEAD"></a>

```c
void OSAL_INIT_LIST_HEAD(struct osal_list_head * list)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 初始化双向链表头节点，将next和prev指针指向自身
- 链表初始化后为空链表状态
- 入参为NULL时直接返回，不进行操作

**前置条件**

- 入参list不为NULL，且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待初始化的链表头节点指针 | 非NULL |

### osal___list_add <a id="osal___list_add"></a>

```c
void osal___list_add(struct osal_list_head * _new, struct osal_list_head * prev, struct osal_list_head * next)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在两个已知连续节点之间插入新节点
- 仅用于已知prev/next节点的内部链表操作
- 任意入参为NULL时直接返回，不进行操作

**前置条件**

- prev和next为链表中已知的连续节点
- 三个入参均不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| _new | [struct osal_list_head](#struct_osal_list_head) * | 待插入的新节点指针 | 非NULL |
| prev | [struct osal_list_head](#struct_osal_list_head) * | 新节点的前驱节点指针 | 非NULL |
| next | [struct osal_list_head](#struct_osal_list_head) * | 新节点的后继节点指针 | 非NULL |

### osal_list_add <a id="osal_list_add"></a>

```c
void osal_list_add(struct osal_list_head * cur, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在指定链表头节点之后添加新节点，适用于栈结构实现
- 内部调用osal___list_add在head和head->next之间插入节点
- 入参为NULL时由内部函数检查返回

**前置条件**

- head为已初始化的链表头节点
- cur不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cur | [struct osal_list_head](#struct_osal_list_head) * | 待添加的新节点指针 | 非NULL |
| head | [struct osal_list_head](#struct_osal_list_head) * | 链表头节点指针 | 非NULL，已初始化 |

### osal_list_add_tail <a id="osal_list_add_tail"></a>

```c
void osal_list_add_tail(struct osal_list_head * cur, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在指定链表尾节点之前添加新节点，适用于队列结构实现
- 内部调用osal___list_add在head->prev和head之间插入节点
- 入参为NULL时由内部函数检查返回

**前置条件**

- head为已初始化的链表头节点
- cur不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cur | [struct osal_list_head](#struct_osal_list_head) * | 待添加的新节点指针 | 非NULL |
| head | [struct osal_list_head](#struct_osal_list_head) * | 链表头节点指针 | 非NULL，已初始化 |

### osal___list_del <a id="osal___list_del"></a>

```c
void osal___list_del(struct osal_list_head * prev, struct osal_list_head * next)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 通过使prev和next节点互相指向来删除中间节点
- 仅用于已知prev/next节点的内部链表操作
- 任意入参为NULL时直接返回，不进行操作

**前置条件**

- prev和next为链表中已知的连续节点
- 两个入参均不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| prev | [struct osal_list_head](#struct_osal_list_head) * | 待删除节点的前驱节点指针 | 非NULL |
| next | [struct osal_list_head](#struct_osal_list_head) * | 待删除节点的后继节点指针 | 非NULL |

### osal___list_del_entry <a id="osal___list_del_entry"></a>

```c
void osal___list_del_entry(struct osal_list_head * entry)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 删除链表中的指定节点，通过entry的prev和next指针完成删除
- 入参为NULL时直接返回，不进行操作
- 删除后节点未标记毒值，与osal_list_del不同

**前置条件**

- entry不为NULL且在链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| entry | [struct osal_list_head](#struct_osal_list_head) * | 待删除的链表节点指针 | 非NULL，且在链表中 |

### osal_list_del <a id="osal_list_del"></a>

```c
void osal_list_del(struct osal_list_head * entry)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 从链表中删除节点，并将next和prev指针标记为毒值
- 入参为NULL时直接返回，不进行操作
- 删除后节点处于未定义状态，不可再通过list_empty判断

**前置条件**

- entry不为NULL且在链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| entry | [struct osal_list_head](#struct_osal_list_head) * | 待删除的链表节点指针 | 非NULL，且在链表中 |

### osal_list_replace <a id="osal_list_replace"></a>

```c
void osal_list_replace(struct osal_list_head * old, struct osal_list_head * _new)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 用新节点替换链表中的旧节点
- 新节点继承旧节点在链表中的位置
- 若旧节点为空链表头，则新节点覆盖旧节点

**前置条件**

- old不为NULL且在链表中
- _new不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| old | [struct osal_list_head](#struct_osal_list_head) * | 被替换的旧节点指针 | 非NULL，且在链表中 |
| _new | [struct osal_list_head](#struct_osal_list_head) * | 替换的新节点指针 | 非NULL |

### osal_list_replace_init <a id="osal_list_replace_init"></a>

```c
void osal_list_replace_init(struct osal_list_head * old, struct osal_list_head * _new)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 用新节点替换链表中的旧节点，并重新初始化旧节点
- 替换后旧节点成为独立的空链表头

**前置条件**

- old不为NULL且在链表中
- _new不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| old | [struct osal_list_head](#struct_osal_list_head) * | 被替换的旧节点指针 | 非NULL，且在链表中 |
| _new | [struct osal_list_head](#struct_osal_list_head) * | 替换的新节点指针 | 非NULL |

### osal_list_del_init <a id="osal_list_del_init"></a>

```c
void osal_list_del_init(struct osal_list_head * entry)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 从链表中删除节点并重新初始化该节点，使其成为独立的空链表头
- 删除后可安全调用list_empty判断
- 入参为NULL时由内部函数检查返回

**前置条件**

- entry不为NULL且在链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| entry | [struct osal_list_head](#struct_osal_list_head) * | 待删除并重新初始化的链表节点指针 | 非NULL，且在链表中 |

### osal_list_move <a id="osal_list_move"></a>

```c
void osal_list_move(struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将节点从原链表删除并添加到目标链表头部
- 入参为NULL时由内部函数检查返回

**前置条件**

- list不为NULL且在原链表中
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待移动的链表节点指针 | 非NULL，且在原链表中 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头节点指针 | 非NULL，已初始化 |

### osal_list_move_tail <a id="osal_list_move_tail"></a>

```c
void osal_list_move_tail(struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将节点从原链表删除并添加到目标链表尾部
- 入参为NULL时由内部函数检查返回

**前置条件**

- list不为NULL且在原链表中
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待移动的链表节点指针 | 非NULL，且在原链表中 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头节点指针 | 非NULL，已初始化 |

### osal_list_is_last <a id="osal_list_is_last"></a>

```c
int osal_list_is_last(const struct osal_list_head * list, const struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断节点是否为链表的最后一个节点
- 任意入参为NULL时返回-1
- 通过比较list->next是否等于head来判断

**前置条件**

- list不为NULL且在链表中
- head不为NULL且为链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) const * | 待判断的链表节点指针 | 非NULL |
| head | [struct osal_list_head](#struct_osal_list_head) const * | 链表头节点指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 是最后一个节点 | list->next == head |
| 0 | 不是最后一个节点 | list->next != head |
| -1 | 入参无效 | list或head为NULL |

### osal_list_empty <a id="osal_list_empty"></a>

```c
int osal_list_empty(const struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断链表是否为空
- 入参为NULL时返回-1
- 通过比较head->next是否等于head来判断

**前置条件**

- head不为NULL且为已初始化的链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| head | [struct osal_list_head](#struct_osal_list_head) const * | 链表头节点指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 链表为空 | head->next == head |
| 0 | 链表非空 | head->next != head |
| -1 | 入参无效 | head为NULL |

### osal_list_empty_careful <a id="osal_list_empty_careful"></a>

```c
int osal_list_empty_careful(const struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断链表是否为空且未被其他CPU修改
- 入参为NULL时返回-1
- 同时检查head->next == head和next == head->prev

**前置条件**

- head不为NULL且为已初始化的链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| head | [struct osal_list_head](#struct_osal_list_head) const * | 链表头节点指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 链表为空且未被修改 | head->next == head且next == head->prev |
| 0 | 链表非空或正在被修改 | 不满足上述条件 |
| -1 | 入参无效 | head为NULL |

### osal_list_rotate_left <a id="osal_list_rotate_left"></a>

```c
void osal_list_rotate_left(struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表向左旋转，首节点移到尾部
- 链表为空时不进行操作

**前置条件**

- head不为NULL且为已初始化的链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| head | [struct osal_list_head](#struct_osal_list_head) * | 链表头节点指针 | 非NULL，已初始化 |

### osal_list_is_singular <a id="osal_list_is_singular"></a>

```c
int osal_list_is_singular(const struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断链表中是否只有一个节点
- 同时检查链表非空且head->next == head->prev

**前置条件**

- head不为NULL且为已初始化的链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| head | [struct osal_list_head](#struct_osal_list_head) const * | 链表头节点指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 链表只有一个节点 | 链表非空且head->next == head->prev |
| 0 | 链表为空或有多于一个节点 | 不满足上述条件 |

### osal___list_cut_position <a id="osal___list_cut_position"></a>

```c
void osal___list_cut_position(struct osal_list_head * list, struct osal_list_head * head, struct osal_list_head * entry)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表从指定位置切割为两个链表，内部实现函数
- 将head链表从开始到entry的部分移动到list链表

**前置条件**

- head链表非空且不为单节点或entry位置合法
- list为可接收切割数据的目标链表

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 接收切割部分的新链表头指针 | 非NULL |
| head | [struct osal_list_head](#struct_osal_list_head) * | 原链表头指针 | 非NULL，已初始化 |
| entry | [struct osal_list_head](#struct_osal_list_head) * | 切割位置节点指针 | 非NULL，在head链表中 |

### osal_list_cut_position <a id="osal_list_cut_position"></a>

```c
void osal_list_cut_position(struct osal_list_head * list, struct osal_list_head * head, struct osal_list_head * entry)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表从指定位置切割为两个链表
- 将head链表从开始到entry的部分移动到list链表
- 链表为空时直接返回，单节点且entry不在链表中时直接返回

**前置条件**

- list不为NULL
- head为已初始化的链表头节点
- entry在head链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 接收切割部分的新链表头指针 | 非NULL |
| head | [struct osal_list_head](#struct_osal_list_head) * | 原链表头指针 | 非NULL，已初始化 |
| entry | [struct osal_list_head](#struct_osal_list_head) * | 切割位置节点指针 | 非NULL，在head链表中 |

### osal___list_splice <a id="osal___list_splice"></a>

```c
void osal___list_splice(const struct osal_list_head * list, struct osal_list_head * prev, struct osal_list_head * next)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将一个链表合并到另一个链表的指定位置，内部实现函数
- 将list链表的所有节点插入到prev和next之间

**前置条件**

- list链表非空
- prev和next为链表中已知的连续节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) const * | 待合并的源链表头指针 | 非NULL，非空链表 |
| prev | [struct osal_list_head](#struct_osal_list_head) * | 插入位置的前驱节点指针 | 非NULL |
| next | [struct osal_list_head](#struct_osal_list_head) * | 插入位置的后继节点指针 | 非NULL |

### osal_list_splice <a id="osal_list_splice"></a>

```c
void osal_list_splice(const struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表合并到目标链表头部，适用于栈结构
- 源链表为空时不进行操作

**前置条件**

- list为已初始化的源链表头节点
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) const * | 待合并的源链表头指针 | 非NULL，已初始化 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头指针 | 非NULL，已初始化 |

### osal_list_splice_tail <a id="osal_list_splice_tail"></a>

```c
void osal_list_splice_tail(struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表合并到目标链表尾部，适用于队列结构
- 源链表为空时不进行操作

**前置条件**

- list为已初始化的源链表头节点
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待合并的源链表头指针 | 非NULL，已初始化 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头指针 | 非NULL，已初始化 |

### osal_list_splice_init <a id="osal_list_splice_init"></a>

```c
void osal_list_splice_init(struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表合并到目标链表头部并重新初始化源链表
- 合并完成后源链表变为空链表

**前置条件**

- list为已初始化的源链表头节点
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待合并的源链表头指针 | 非NULL，已初始化 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头指针 | 非NULL，已初始化 |

### osal_list_splice_tail_init <a id="osal_list_splice_tail_init"></a>

```c
void osal_list_splice_tail_init(struct osal_list_head * list, struct osal_list_head * head)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将链表合并到目标链表尾部并重新初始化源链表
- 合并完成后源链表变为空链表

**前置条件**

- list为已初始化的源链表头节点
- head为已初始化的目标链表头节点

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| list | [struct osal_list_head](#struct_osal_list_head) * | 待合并的源链表头指针 | 非NULL，已初始化 |
| head | [struct osal_list_head](#struct_osal_list_head) * | 目标链表头指针 | 非NULL，已初始化 |

### INIT_OSAL_HLIST_NODE <a id="INIT_OSAL_HLIST_NODE"></a>

```c
void INIT_OSAL_HLIST_NODE(struct osal_hlist_node * h)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 初始化哈希链表节点，将next和pprev置为NULL
- 初始化后节点处于未链接状态

**前置条件**

- h不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| h | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待初始化的哈希链表节点指针 | 非NULL |

### osal_hlist_unhashed <a id="osal_hlist_unhashed"></a>

```c
int osal_hlist_unhashed(const struct osal_hlist_node * h)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断哈希链表节点是否未被哈希
- 通过检查pprev是否为NULL来判断

**前置条件**

- h不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| h | [struct osal_hlist_node](#struct_osal_hlist_node) const * | 待判断的哈希链表节点指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 节点未被哈希 | h->pprev == NULL |
| 0 | 节点已被哈希 | h->pprev != NULL |

### osal_hlist_empty <a id="osal_hlist_empty"></a>

```c
int osal_hlist_empty(const struct osal_hlist_head * h)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 判断哈希链表是否为空
- 通过检查first是否为NULL来判断

**前置条件**

- h不为NULL且为已初始化的哈希链表头

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| h | [struct osal_hlist_head](#struct_osal_hlist_head) const * | 哈希链表头指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 哈希链表为空 | h->first == NULL |
| 0 | 哈希链表非空 | h->first != NULL |

### osal___hlist_del <a id="osal___hlist_del"></a>

```c
void osal___hlist_del(struct osal_hlist_node * n)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 删除哈希链表中的节点，内部实现函数
- 将节点的前驱和后继直接连接

**前置条件**

- n不为NULL且在哈希链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待删除的哈希链表节点指针 | 非NULL，且在链表中 |

### osal_hlist_del <a id="osal_hlist_del"></a>

```c
void osal_hlist_del(struct osal_hlist_node * n)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 删除哈希链表节点并标记为毒值
- 删除后节点处于未定义状态

**前置条件**

- n不为NULL且在哈希链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待删除的哈希链表节点指针 | 非NULL，且在链表中 |

### osal_hlist_del_init <a id="osal_hlist_del_init"></a>

```c
void osal_hlist_del_init(struct osal_hlist_node * n)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 删除哈希链表节点并重新初始化该节点
- 仅在节点已被哈希时执行删除操作
- 删除后节点处于可重用状态

**前置条件**

- n不为NULL且在哈希链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待删除的哈希链表节点指针 | 非NULL，且在链表中 |

### osal_hlist_add_head <a id="osal_hlist_add_head"></a>

```c
void osal_hlist_add_head(struct osal_hlist_node * n, struct osal_hlist_head * h)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在哈希链表头部添加节点
- 新节点成为链表的第一个节点

**前置条件**

- n不为NULL且指向已分配的内存空间
- h不为NULL且为已初始化的哈希链表头

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待添加的新节点指针 | 非NULL |
| h | [struct osal_hlist_head](#struct_osal_hlist_head) * | 哈希链表头指针 | 非NULL，已初始化 |

### osal_hlist_add_before <a id="osal_hlist_add_before"></a>

```c
void osal_hlist_add_before(struct osal_hlist_node * n, struct osal_hlist_node * next)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在指定哈希链表节点之前添加新节点
- next节点必须不为NULL

**前置条件**

- n不为NULL且指向已分配的内存空间
- next不为NULL且在哈希链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待添加的新节点指针 | 非NULL |
| next | [struct osal_hlist_node](#struct_osal_hlist_node) * | 目标节点指针 | 非NULL，且在链表中 |

### osal_hlist_add_after <a id="osal_hlist_add_after"></a>

```c
void osal_hlist_add_after(struct osal_hlist_node * n, struct osal_hlist_node * next)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 在指定哈希链表节点之后添加新节点
- 内部调用后更新next节点的pprev指针

**前置条件**

- n不为NULL且指向已分配的内存空间
- next不为NULL且在哈希链表中

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 已在链表中的节点指针 | 非NULL，且在链表中 |
| next | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待添加的新节点指针 | 非NULL |

### osal_hlist_add_fake <a id="osal_hlist_add_fake"></a>

```c
void osal_hlist_add_fake(struct osal_hlist_node * n)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 为哈希链表节点设置假pprev指针，使其可被hlist_del删除
- 节点不在链表中但可安全调用删除操作

**前置条件**

- n不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | [struct osal_hlist_node](#struct_osal_hlist_node) * | 待设置假pprev的哈希链表节点指针 | 非NULL |

### osal_hlist_move_list <a id="osal_hlist_move_list"></a>

```c
void osal_hlist_move_list(struct osal_hlist_head * old, struct osal_hlist_head * cur)
```

**头文件清单**

```c
#include "kernel/osal/include/osal_list.h"
```

**功能说明**

- 将哈希链表从一个头节点迁移到另一个头节点
- 迁移后原头节点的first置为NULL
- 若链表非空则修正首节点的pprev指针指向新头节点

**前置条件**

- old不为NULL且为已初始化的哈希链表头
- cur不为NULL且为已初始化的哈希链表头

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| old | [struct osal_hlist_head](#struct_osal_hlist_head) * | 原哈希链表头指针 | 非NULL，已初始化 |
| cur | [struct osal_hlist_head](#struct_osal_hlist_head) * | 新哈希链表头指针 | 非NULL，已初始化 |

## Structures

<a id="struct_osal_list_head"></a>
struct osal_list_head

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| next | struct osal_list_head * | 指向下一个节点的指针 |
| prev | struct osal_list_head * | 指向前一个节点的指针 |

<a id="struct_osal_hlist_node"></a>
struct osal_hlist_node

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| next | struct osal_hlist_node * | 指向下一个节点的指针 |
| pprev | struct osal_hlist_node ** | 指向前一个节点的next指针的地址 |

<a id="struct_osal_hlist_head"></a>
struct osal_hlist_head

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| first | struct osal_hlist_node * | 指向哈希链表第一个节点的指针 |


