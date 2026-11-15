# PBAP

PBAP（Phone Book Access Profile，电话簿访问配置文件）提供电话簿访问客户端（PCE）能力，本设备作为客户端访问对端电话簿服务端（PSE）。本模块支持与 PSE 建立连接、下载并浏览电话簿对象、设置当前文件夹以及查询 vCard 属性等接口。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [pbap_pce_connect](#pbap_pce_connect) | 建立与PSE (Phone Book Server Equipment) 的连接 |
| [pbap_pce_disconnect](#pbap_pce_disconnect) | 断开与PSE的连接 |
| [pbap_pce_get_device_connect_state](#pbap_pce_get_device_connect_state) | 查询PBAP客户端连接状态 |
| [pbap_pce_pull_phone_book](#pbap_pce_pull_phone_book) | 下载电话簿对象 |
| [pbap_pce_set_phone_book](#pbap_pce_set_phone_book) | 设置虚拟文件夹体系结构中的当前文件夹 |
| [pbap_pce_pull_vcard_listing](#pbap_pce_pull_vcard_listing) | 检索PSE的电话簿列表对象 |
| [pbap_pce_pull_vcard_entry](#pbap_pce_pull_vcard_entry) | 检索特定的vCard |
| [pbap_pce_stop_trans](#pbap_pce_stop_trans) | 停止传输 |
| [pbap_pce_register_callbacks](#pbap_pce_register_callbacks) | 注册PBAP回调函数 |

## Functions

### pbap_pce_connect <a id="pbap_pce_connect"></a>

```c
int pbap_pce_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 建立与远端PSE的PBAP (Phone Book Access Profile)连接
- 连接结果通过[state_changed_cb](#struct_pbap_pce_callbacks_t)回调函数上报连接状态变更
- 调用该接口发起连接请求，连接成功或失败均以异步回调方式通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 待连接的远端设备地址指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_disconnect <a id="pbap_pce_disconnect"></a>

```c
int pbap_pce_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 断开与远端PSE的PBAP连接
- 断开结果通过[state_changed_cb](#struct_pbap_pce_callbacks_t)回调函数上报连接状态变更
- 调用该接口发起断连请求，断连成功或失败均以异步回调方式通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向已连接的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 待断开连接的远端设备地址指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_get_device_connect_state <a id="pbap_pce_get_device_connect_state"></a>

```c
int pbap_pce_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 查询PBAP客户端与远端设备的连接状态
- 返回值为[profile_connect_state_t](#enum_profile_connect_state_t)枚举，包含正在连接、已连接、正在断连、已断连四种状态
- 调用该接口不会触发连接状态变更，仅做查询操作

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 待查询的远端设备地址指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立PBAP连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | PBAP连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开PBAP连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | PBAP连接已断开 |

### pbap_pce_pull_phone_book <a id="pbap_pce_pull_phone_book"></a>

```c
int pbap_pce_pull_phone_book(const bd_addr_t *bd_addr, const pbap_pce_pull_phone_book_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 从远端PSE下载电话簿对象
- 下载结果通过[receive_data_cb](#struct_pbap_pce_callbacks_t)回调函数上报接收到的数据，电话簿大小通过[phone_book_sized_cb](#struct_pbap_pce_callbacks_t)回调上报
- 传输完成后通过[trans_complete_cb](#struct_pbap_pce_callbacks_t)回调通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 与远端PSE已建立PBAP连接
- 入参bd_addr不为NULL，param不为NULL，且param中name指向有效的电话簿名称字符串

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址指针 | 不为NULL，设备已连接 |
| param | const [pbap_pce_pull_phone_book_param_t](#struct_pbap_pce_pull_phone_book_param_t) * | 电话簿参数指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 下载请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_set_phone_book <a id="pbap_pce_set_phone_book"></a>

```c
int pbap_pce_set_phone_book(const bd_addr_t *bd_addr, const unsigned char *name)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 设置远端PSE虚拟文件夹体系结构中的当前文件夹
- 支持在电话簿目录结构中导航，切换当前工作目录
- 设置完成后通过[trans_complete_cb](#struct_pbap_pce_callbacks_t)回调通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 与远端PSE已建立PBAP连接
- 入参bd_addr不为NULL，name不为NULL，且指向有效的文件夹名称字符串

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址指针 | 不为NULL，设备已连接 |
| name | const unsigned char * | 文件夹名称指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 设置请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_pull_vcard_listing <a id="pbap_pce_pull_vcard_listing"></a>

```c
int pbap_pce_pull_vcard_listing(const bd_addr_t *bd_addr, const pbap_pce_pull_vcard_listing_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 检索远端PSE的电话簿列表对象，返回vCard列表
- 支持按名称或号码搜索，搜索结果通过[receive_data_cb](#struct_pbap_pce_callbacks_t)回调函数上报
- 传输完成后通过[trans_complete_cb](#struct_pbap_pce_callbacks_t)回调通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 与远端PSE已建立PBAP连接
- 入参bd_addr不为NULL，param不为NULL，且param中name指向有效的电话簿名称字符串

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址指针 | 不为NULL，设备已连接 |
| param | const [pbap_pce_pull_vcard_listing_param_t](#struct_pbap_pce_pull_vcard_listing_param_t) * | 电话簿列表对象参数指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 检索请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_pull_vcard_entry <a id="pbap_pce_pull_vcard_entry"></a>

```c
int pbap_pce_pull_vcard_entry(const bd_addr_t *bd_addr, const pbap_pce_pull_vcard_entry_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 检索远端PSE中特定的vCard条目
- vCard数据通过[receive_data_cb](#struct_pbap_pce_callbacks_t)回调函数上报给上层应用
- 传输完成后通过[trans_complete_cb](#struct_pbap_pce_callbacks_t)回调通知上层应用

**前置条件**

- 蓝牙协议栈已初始化完成
- 与远端PSE已建立PBAP连接
- 入参bd_addr不为NULL，param不为NULL，且param中name指向有效的vCard名称字符串

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址指针 | 不为NULL，设备已连接 |
| param | const [pbap_pce_pull_vcard_entry_param_t](#struct_pbap_pce_pull_vcard_entry_param_t) * | vCard参数指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 检索请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_stop_trans <a id="pbap_pce_stop_trans"></a>

```c
int pbap_pce_stop_trans(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 停止当前正在进行的PBAP数据传输操作
- 传输停止后通过[trans_complete_cb](#struct_pbap_pce_callbacks_t)回调通知上层应用
- 调用该接口可中断pull_phone_book、pull_vcard_listing、pull_vcard_entry等数据传输操作

**前置条件**

- 蓝牙协议栈已初始化完成
- 与远端PSE已建立PBAP连接
- 当前存在正在进行的数据传输操作

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 停止传输请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pbap_pce_register_callbacks <a id="pbap_pce_register_callbacks"></a>

```c
int pbap_pce_register_callbacks(pbap_pce_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pbap.h"
```

**功能说明**

- 注册PBAP上层应用回调函数，用于接收连接状态变更、电话簿大小、传输完成、数据接收、错误码等事件通知
- 回调函数运行于bts线程，不能阻塞或长时间等待
- 回调中bd_addr等参数的内存由bts申请和释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参func不为NULL，且指向有效的回调函数结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [pbap_pce_callbacks_t](#struct_pbap_pce_callbacks_t) * | 指向回调函数接口定义的指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Enumerations

### pbap_format_vcard_t <a id="enum_pbap_format_vcard_t"></a>

```c
typedef enum {
    PBAP_FORMAT_VCARD21 = 0x00, /*!< PBAP vCard格式v2.1 */
    PBAP_FORMAT_VCARD30,        /*!< PBAP vCard格式v3.0 */
} pbap_format_vcard_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_FORMAT_VCARD21 | 0x00 | PBAP vCard格式v2.1 |
| PBAP_FORMAT_VCARD30 | 0x01 | PBAP vCard格式v3.0 |

### pbap_repositories_t <a id="enum_pbap_repositories_t"></a>

```c
typedef enum {
    PBAP_REPOSITORY_LOCAL = 0x01, /*!< PBAP电话簿存储位置在手机 */
    PBAP_REPOSITORY_SIM, /*!< PBAP电话簿存储位置在SIM卡 */
} pbap_repositories_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_REPOSITORY_LOCAL | 0x01 | PBAP电话簿存储位置在手机 |
| PBAP_REPOSITORY_SIM | 0x02 | PBAP电话簿存储位置在SIM (Subscriber Identity Module) 卡 |

### pbap_order_type_t <a id="enum_pbap_order_type_t"></a>

```c
typedef enum {
    PBAP_ORDER_INDEXED = 0x00, /*!< PBAP排序顺序为索引序 */
    PBAP_ORDER_ALPHANUMERIC,   /*!< PBAP排序顺序为字典序 */
    PBAP_ORDER_PHONETIC,       /*!< PBAP排序顺序为语音序 */
} pbap_order_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_ORDER_INDEXED | 0x00 | PBAP排序顺序为索引序 |
| PBAP_ORDER_ALPHANUMERIC | 0x01 | PBAP排序顺序为字典序 |
| PBAP_ORDER_PHONETIC | 0x02 | PBAP排序顺序为语音序 |

### pbap_search_property_t <a id="enum_pbap_search_property_t"></a>

```c
typedef enum {
    PBAP_SEARCH_PROPERTY_NAME = 0x00, /*!< PBAP搜索属性为名字 */
    PBAP_SEARCH_PROPERTY_NUMBER,      /*!< PBAP搜索属性为号码 */
    PBAP_SEARCH_PROPERTY_SOUND,       /*!< PBAP搜索属性为读音 */
} pbap_search_property_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_SEARCH_PROPERTY_NAME | 0x00 | PBAP搜索属性为名字 |
| PBAP_SEARCH_PROPERTY_NUMBER | 0x01 | PBAP搜索属性为号码 |
| PBAP_SEARCH_PROPERTY_SOUND | 0x02 | PBAP搜索属性为读音 |

### pbap_property_mask_t <a id="enum_pbap_property_mask_t"></a>

```c
typedef enum {
    PBAP_PROPERTY_MASK_VERSION = 0x00, /*!< PBAP属性 vCard版本 */
    PBAP_PROPERTY_MASK_FN,             /*!< PBAP属性 格式化名称 */
    PBAP_PROPERTY_MASK_N,              /*!< PBAP属性 名称的结构化表示 */
    PBAP_PROPERTY_MASK_PHOTO,          /*!< PBAP属性 相关图像或照片 */
    PBAP_PROPERTY_MASK_BDAY,           /*!< PBAP属性 生日 */
    PBAP_PROPERTY_MASK_ADR,            /*!< PBAP属性 收货地址 */
    PBAP_PROPERTY_MASK_LABEL,          /*!< PBAP属性 标签 */
    PBAP_PROPERTY_MASK_TEL,            /*!< PBAP属性 电话号码 */
    PBAP_PROPERTY_MASK_EMAIL,          /*!< PBAP属性 电子邮件地址 */
    PBAP_PROPERTY_MASK_MAILER,         /*!< PBAP属性 电子邮件 */
    PBAP_PROPERTY_MASK_RTY_MASK_TZ,    /*!< PBAP属性 时区 */
    PBAP_PROPERTY_MASK_GEO,            /*!< PBAP属性 地理位置 */
    PBAP_PROPERTY_MASK_TITLE,          /*!< PBAP属性 工作 */
    PBAP_PROPERTY_MASK_RTY_MASK_ROLE,  /*!< PBAP属性 组织内的角色 */
    PBAP_PROPERTY_MASK_LOGO,           /*!< PBAP属性 组织徽标 */
    PBAP_PROPERTY_MASK_RTY_MASK_AGENT, /*!< PBAP属性 代表人员的vCard */
    PBAP_PROPERTY_MASK_ORG,            /*!< PBAP属性 组织名称 */
    PBAP_PROPERTY_MASK_NOTE,           /*!< PBAP属性 注释 */
    PBAP_PROPERTY_MASK_REV,            /*!< PBAP属性 修订版本 */
    PBAP_PROPERTY_MASK_SOUND,          /*!< PBAP属性 姓名读音 */
    PBAP_PROPERTY_MASK_URL,            /*!< PBAP属性 统一资源定位器 */
    PBAP_PROPERTY_MASK_UID,            /*!< PBAP属性 唯一标识 */
    PBAP_PROPERTY_MASK_KEY,            /*!< PBAP属性 公共加密密钥 */
    PBAP_PROPERTY_MASK_NICKNAME,       /*!< PBAP属性 昵称 */
    PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_CATEGORIES,  /*!< PBAP属性 类别 */
    PBAP_PROPERTY_MASK_RTY_MASK_PROID,                /*!< PBAP属性 产品ID */
    PBAP_PROPERTY_MASK_CLASS,                         /*!< PBAP属性 CLASS信息 */
    PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_SORT_STRING, /*!< PBAP属性 用于排序操作的字符串 */
    PBAP_PROPERTY_MASK_X_IRMC_CALL_DATETIME, /*!< PBAP属性 时间戳 */
    PBAP_PROPERTY_MASK_RTY_MASK_X_BT_SPEEDDIALKEY, /*!< PBAP属性 快速拨号快捷方式 */
    PBAP_PROPERTY_MASK_RTY_MASK_X_BT_UCI, /*!< PBAP属性 统一主叫方标识符 */
    PBAP_PROPERTY_MASK_X_BT_UID, /*!< PBAP属性 蓝牙联系人唯一标识符 */
    PBAP_PROPERTY_MASK_FILTER = 0x27, /*!< PBAP属性 私有过滤器 */
    PBAP_PROPERTY_MASK_INVALID = 0xFF,
} pbap_property_mask_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_PROPERTY_MASK_VERSION | 0x00 | PBAP属性 vCard版本 |
| PBAP_PROPERTY_MASK_FN | 0x01 | PBAP属性 格式化名称 |
| PBAP_PROPERTY_MASK_N | 0x02 | PBAP属性 名称的结构化表示 |
| PBAP_PROPERTY_MASK_PHOTO | 0x03 | PBAP属性 相关图像或照片 |
| PBAP_PROPERTY_MASK_BDAY | 0x04 | PBAP属性 生日 |
| PBAP_PROPERTY_MASK_ADR | 0x05 | PBAP属性 收货地址 |
| PBAP_PROPERTY_MASK_LABEL | 0x06 | PBAP属性 标签 |
| PBAP_PROPERTY_MASK_TEL | 0x07 | PBAP属性 电话号码 |
| PBAP_PROPERTY_MASK_EMAIL | 0x08 | PBAP属性 电子邮件地址 |
| PBAP_PROPERTY_MASK_MAILER | 0x09 | PBAP属性 电子邮件 |
| PBAP_PROPERTY_MASK_RTY_MASK_TZ | 0x0A | PBAP属性 时区 |
| PBAP_PROPERTY_MASK_GEO | 0x0B | PBAP属性 地理位置 |
| PBAP_PROPERTY_MASK_TITLE | 0x0C | PBAP属性 工作 |
| PBAP_PROPERTY_MASK_RTY_MASK_ROLE | 0x0D | PBAP属性 组织内的角色 |
| PBAP_PROPERTY_MASK_LOGO | 0x0E | PBAP属性 组织徽标 |
| PBAP_PROPERTY_MASK_RTY_MASK_AGENT | 0x0F | PBAP属性 代表人员的vCard |
| PBAP_PROPERTY_MASK_ORG | 0x10 | PBAP属性 组织名称 |
| PBAP_PROPERTY_MASK_NOTE | 0x11 | PBAP属性 注释 |
| PBAP_PROPERTY_MASK_REV | 0x12 | PBAP属性 修订版本 |
| PBAP_PROPERTY_MASK_SOUND | 0x13 | PBAP属性 姓名读音 |
| PBAP_PROPERTY_MASK_URL | 0x14 | PBAP属性 统一资源定位器 |
| PBAP_PROPERTY_MASK_UID | 0x15 | PBAP属性 唯一标识 |
| PBAP_PROPERTY_MASK_KEY | 0x16 | PBAP属性 公共加密密钥 |
| PBAP_PROPERTY_MASK_NICKNAME | 0x17 | PBAP属性 昵称 |
| PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_CATEGORIES | 0x18 | PBAP属性 类别 |
| PBAP_PROPERTY_MASK_RTY_MASK_PROID | 0x19 | PBAP属性 产品ID |
| PBAP_PROPERTY_MASK_CLASS | 0x1A | PBAP属性 CLASS信息 |
| PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_SORT_STRING | 0x1B | PBAP属性 用于排序操作的字符串 |
| PBAP_PROPERTY_MASK_X_IRMC_CALL_DATETIME | 0x1C | PBAP属性 时间戳 |
| PBAP_PROPERTY_MASK_RTY_MASK_X_BT_SPEEDDIALKEY | 0x1D | PBAP属性 快速拨号快捷方式 |
| PBAP_PROPERTY_MASK_RTY_MASK_X_BT_UCI | 0x1E | PBAP属性 统一主叫方标识符 |
| PBAP_PROPERTY_MASK_X_BT_UID | 0x1F | PBAP属性 蓝牙联系人唯一标识符 |
| PBAP_PROPERTY_MASK_FILTER | 0x27 | PBAP属性 私有过滤器 |
| PBAP_PROPERTY_MASK_INVALID | 0xFF | 无效属性掩码 |

### pbap_error_codes_t <a id="enum_pbap_error_codes_t"></a>

```c
#define PBAP_ERROR_OFFSET 0x100
typedef enum {
    PBAP_ERROR_OK = PBAP_ERROR_OFFSET, /*!< PBAP错误码 正常 */
    PBAP_ERROR_BAD_REQUEST,            /*!< PBAP错误码 错误请求 */
    PBAP_ERROR_NOT_IMPLEMENTED,        /*!< PBAP错误码 未执行 */
    PBAP_ERROR_UNAUTHORIZED,           /*!< PBAP错误码 未授权 */
    PBAP_ERROR_PRECONDITION_FAILED, /*!< PBAP错误码 预置条件失败 */
    PBAP_ERROR_NOT_FOUND,           /*!< PBAP错误码 未找到 */
    PBAP_ERROR_NOT_ACCEPTABLE, /*!< PBAP错误码 不可接受 */
    PBAP_ERROR_SERVICE_UNAVAILABLE, /*!< PBAP错误码 服务不可用 */
    PBAP_ERROR_FORBIDDEN,           /*!< PBAP错误码 禁止 */
} pbap_error_codes_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PBAP_ERROR_OK | 0x100 | PBAP错误码 正常 |
| PBAP_ERROR_BAD_REQUEST | 0x101 | PBAP错误码 错误请求 |
| PBAP_ERROR_NOT_IMPLEMENTED | 0x102 | PBAP错误码 未执行 |
| PBAP_ERROR_UNAUTHORIZED | 0x103 | PBAP错误码 未授权 |
| PBAP_ERROR_PRECONDITION_FAILED | 0x104 | PBAP错误码 预置条件失败 |
| PBAP_ERROR_NOT_FOUND | 0x105 | PBAP错误码 未找到 |
| PBAP_ERROR_NOT_ACCEPTABLE | 0x106 | PBAP错误码 不可接受 |
| PBAP_ERROR_SERVICE_UNAVAILABLE | 0x107 | PBAP错误码 服务不可用 |
| PBAP_ERROR_FORBIDDEN | 0x108 | PBAP错误码 禁止 |

### profile_connect_state_t <a id="enum_profile_connect_state_t"></a>

```c
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,    /*!< 正在连接。 */
    PROFILE_STATE_CONNECTED = 0x02,     /*!< 已连接。 */
    PROFILE_STATE_DISCONNECTING = 0x03, /*!< 正在断连。 */
    PROFILE_STATE_DISCONNECTED = 0x04   /*!< 已断连。 */
} profile_connect_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PROFILE_STATE_CONNECTING | 0x01 | 正在连接 |
| PROFILE_STATE_CONNECTED | 0x02 | 已连接 |
| PROFILE_STATE_DISCONNECTING | 0x03 | 正在断连 |
| PROFILE_STATE_DISCONNECTED | 0x04 | 已断连 |

### bt_addr_type <a id="enum_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS          =  0x00, /*!< 蓝牙公有地址。 */
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS          =  0x01, /*!< 蓝牙随机地址。 */
} bt_addr_type;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS | 0x00 | 蓝牙公有地址 |
| BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS | 0x01 | 蓝牙随机地址 |

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
#define ERRCODE_BT_COMMON_BASE 0x80006000
#define ERRCODE_BT_COMMON_END  0x800067FF
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,                /*!< 执行成功错误码 */
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE, /*!< 执行失败错误码 */
    ERRCODE_BT_NOT_READY,                     /*!< 执行状态未就绪错误码 */
    ERRCODE_BT_MALLOC_FAIL,                   /*!< 内存不足错误码 */
    ERRCODE_BT_MEMCPY_FAIL,                   /*!< 内存拷贝错误码 */
    ERRCODE_BT_BUSY,                          /*!< 繁忙无法响应错误码 */
    ERRCODE_BT_DONE,                          /*!< 执行完成错误码 */
    ERRCODE_BT_UNSUPPORTED,                   /*!< 不支持错误码 */
    ERRCODE_BT_PARAM_ERR,                     /*!< 无效参数错误码 */
    ERRCODE_BT_STATE_ERR,                     /*!< 状态错误 */
    ERRCODE_BT_UNHANDLED,                     /*!< 未处理错误码 */
    ERRCODE_BT_AUTH_FAIL,                     /*!< 鉴权失败错误码 */
    ERRCODE_BT_RMT_DEV_DOWN,                  /*!< 远端设备关闭错误码 */
    ERRCODE_BT_AUTH_REJECTED,                 /*!< 鉴权被拒错误码 */
    ERRCODE_BT_MAX = ERRCODE_BT_COMMON_END    /*!< 蓝牙错误码最大值 */
} errcode_bt_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_BT_SUCCESS | 0x00 | 执行成功 |
| ERRCODE_BT_FAIL | 0x80006000 | 执行失败 |
| ERRCODE_BT_NOT_READY | 0x80006001 | 状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝失败 |
| ERRCODE_BT_BUSY | 0x80006004 | 繁忙无法响应 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒 |
| ERRCODE_BT_MAX | 0x800067FF | 错误码最大值 |

## Structures

### pbap_pce_pull_phone_book_param_t <a id="struct_pbap_pce_pull_phone_book_param_t"></a>

```c
#define PBAP_MASK_LEN 8
typedef struct {
    unsigned char property_selector[PBAP_MASK_LEN]; /*!< 属性选择器 */
    unsigned char vcard_selector[PBAP_MASK_LEN];    /*!< vCard选择器 */
    unsigned short max_list_count;                  /*!< 最大列表计数 */
    unsigned short list_offset;                     /*!< 列表起始偏移量 */
    unsigned char *name;                            /*!< 名称 */
    unsigned char format;                           /*!< 格式 */
    unsigned char reset_new_missed_calls;  /*!< 重置新的未接呼叫 */
    unsigned char vcard_selector_operator; /*!< vCard选择器运算符 */
} pbap_pce_pull_phone_book_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| property_selector | unsigned char[8] | 属性选择器，长度为PBAP_MASK_LEN(8)，每位对应[pbap_property_mask_t](#enum_pbap_property_mask_t)中的一个属性 |
| vcard_selector | unsigned char[8] | vCard选择器，长度为PBAP_MASK_LEN(8)，用于筛选vCard条目 |
| max_list_count | unsigned short | 最大列表计数，限制返回的列表条目数 |
| list_offset | unsigned short | 列表起始偏移量，指定从第几条记录开始返回 |
| name | unsigned char * | 电话簿名称指针 |
| format | unsigned char | 格式，取值参考[pbap_format_vcard_t](#enum_pbap_format_vcard_t) |
| reset_new_missed_calls | unsigned char | 重置新的未接呼叫 |
| vcard_selector_operator | unsigned char | vCard选择器运算符 |

### pbap_pce_pull_vcard_listing_param_t <a id="struct_pbap_pce_pull_vcard_listing_param_t"></a>

```c
typedef struct {
    unsigned char vcard_selector[PBAP_MASK_LEN]; /*!< vCard选择器 */
    unsigned short max_list_count;               /*!< 最大列表计数 */
    unsigned short list_offset;                  /*!< 列表起始偏移量 */
    unsigned char *name;                         /*!< 名称 */
    unsigned char *search_value;                 /*!< 搜索值 */
    unsigned char search_property;               /*!< 搜索属性 */
    unsigned char order;                         /*!< 顺序 */
    unsigned char reset_new_missed_calls;  /*!< 重置新的未接呼叫 */
    unsigned char vcard_selector_operator; /*!< vCard选择器运算符 */
} pbap_pce_pull_vcard_listing_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| vcard_selector | unsigned char[8] | vCard选择器，长度为PBAP_MASK_LEN(8)，用于筛选vCard条目 |
| max_list_count | unsigned short | 最大列表计数，限制返回的列表条目数 |
| list_offset | unsigned short | 列表起始偏移量，指定从第几条记录开始返回 |
| name | unsigned char * | 电话簿名称指针 |
| search_value | unsigned char * | 搜索值指针，用于按关键字搜索 |
| search_property | unsigned char | 搜索属性，取值参考[pbap_search_property_t](#enum_pbap_search_property_t) |
| order | unsigned char | 排序方式，取值参考[pbap_order_type_t](#enum_pbap_order_type_t) |
| reset_new_missed_calls | unsigned char | 重置新的未接呼叫 |
| vcard_selector_operator | unsigned char | vCard选择器运算符 |

### pbap_pce_pull_vcard_entry_param_t <a id="struct_pbap_pce_pull_vcard_entry_param_t"></a>

```c
typedef struct {
    unsigned char property_selector[PBAP_MASK_LEN]; /*!< 属性选择器 */
    unsigned char *name;                            /*!< 名称 */
    unsigned char format;                           /*!< 格式 */
} pbap_pce_pull_vcard_entry_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| property_selector | unsigned char[8] | 属性选择器，长度为PBAP_MASK_LEN(8)，每位对应[pbap_property_mask_t](#enum_pbap_property_mask_t)中的一个属性 |
| name | unsigned char * | vCard名称指针 |
| format | unsigned char | 格式，取值参考[pbap_format_vcard_t](#enum_pbap_format_vcard_t) |

### pbap_pce_callbacks_t <a id="struct_pbap_pce_callbacks_t"></a>

```c
typedef struct {
    pbap_pce_conn_state_changed_callback state_changed_cb;
    pbap_pce_phone_book_size_callback phone_book_sized_cb;
    pbap_pce_trans_complete_callback trans_complete_cb;
    pbap_pce_receive_data_callback receive_data_cb;
    pbap_pce_error_code_callback error_code_cb;
} pbap_pce_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| state_changed_cb | pbap_pce_conn_state_changed_callback | 连接状态变化回调函数指针，参数为远端设备地址和连接状态 |
| phone_book_sized_cb | pbap_pce_phone_book_size_callback | 电话簿大小回调函数指针，参数为远端设备地址和电话簿大小 |
| trans_complete_cb | pbap_pce_trans_complete_callback | 传输完成回调函数指针，参数为远端设备地址 |
| receive_data_cb | pbap_pce_receive_data_callback | 数据接收回调函数指针，参数为远端设备地址、数据指针和数据长度 |
| error_code_cb | pbap_pce_error_code_callback | 错误码回调函数指针，参数为远端设备地址和错误码 |

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
#define BD_ADDR_LEN 6
typedef struct {
    uint8_t addr[BD_ADDR_LEN];     /*!< 蓝牙地址。 */
    uint8_t type;                  /*!< 蓝牙地址类型，参考 bt_addr_type。 */
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙设备地址，长度为6字节 |
| type | uint8_t | 蓝牙地址类型，取值参考[bt_addr_type](#enum_bt_addr_type) |


