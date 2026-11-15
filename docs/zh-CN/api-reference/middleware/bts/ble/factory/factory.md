# BTS FACTORY

BTS (Bluetooth Subsystem) FACTORY 提供 BLE (Bluetooth Low Energy) 产测与射频校准能力。管理射频长发/长收、收发结束与复位、单音模式、射频 NV (Non-Volatile) 校准，以及产线频偏（XO）和功率校准及其 EFUSE (Electronic Fuse) 读写。

用于芯片产线测试与射频指标校准，支持通过 HCI (Host Controller Interface) vendor 命令完成频偏/温度/功率校准值的写入、读取与 EFUSE 持久化，配合回调函数返回执行结果。

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [ble_factory_register_callbacks](#ble_factory_register_callbacks) | 注册BLE factory管理回调函数 |
| [ble_factory_rf_tx_start](#ble_factory_rf_tx_start) | 开启BLE射频长发 |
| [ble_factory_rf_rx_start](#ble_factory_rf_rx_start) | 开启BLE射频长收 |
| [ble_factory_rf_trx_end](#ble_factory_rf_trx_end) | 关闭BLE射频收发 |
| [ble_factory_rf_reset](#ble_factory_rf_reset) | 重置BLE射频收发 |
| [ble_factory_rf_cali_nv](#ble_factory_rf_cali_nv) | 校准BLE射频NV  |
| [ble_factory_rf_single_tone](#ble_factory_rf_single_tone) | 设置BLE射频单音模式 |
| [ble_factory_vendor_productline_cmd](#ble_factory_vendor_productline_cmd) | 设置BLE产线校准命令 |

## Functions

### ble_factory_register_callbacks <a id="ble_factory_register_callbacks"></a>

```c
errcode_t ble_factory_register_callbacks(ble_factory_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 注册BLE factory管理回调函数，用于接收射频长发、长收、收发结束、收发复位、NV校准、单音模式、产线校准等事件通知
- 回调函数在BLE service上下文中被调用，不应阻塞或长时间等待
- 回调函数中指针由BLE service申请和释放，回调中不应释放

**前置条件**

- BLE协议栈已初始化完成
- 入参func不为NULL，且指向的ble_factory_callbacks_t结构体已正确填充

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [ble_factory_callbacks_t](#struct_ble_factory_callbacks_t) * | 回调函数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_tx_start <a id="ble_factory_rf_tx_start"></a>

```c
errcode_t ble_factory_rf_tx_start(ble_rf_tx_start_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 开启BLE射频长发测试，用于产线射频发射测试场景
- 发射结果通过ble_factory_rf_tx_start_callback回调异步返回
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_tx_start_cb已设置
- 入参param不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [ble_rf_tx_start_t](#struct_ble_rf_tx_start_t) * | 射频长发参数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_tx_start_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_rx_start <a id="ble_factory_rf_rx_start"></a>

```c
errcode_t ble_factory_rf_rx_start(ble_rf_rx_start_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 开启BLE射频长收测试，用于产线射频接收测试场景
- 接收结果通过ble_factory_rf_rx_start_callback回调异步返回
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_rx_start_cb已设置
- 入参param不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [ble_rf_rx_start_t](#struct_ble_rf_rx_start_t) * | 射频长收参数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_rx_start_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_trx_end <a id="ble_factory_rf_trx_end"></a>

```c
errcode_t ble_factory_rf_trx_end(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 关闭BLE射频收发，停止当前正在进行的射频长发或长收测试
- 收发结束结果通过ble_factory_rf_trx_end_callback回调异步返回，包含接收数据包数
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_trx_end_cb已设置
- 射频长发或长收已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_trx_end_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_reset <a id="ble_factory_rf_reset"></a>

```c
errcode_t ble_factory_rf_reset(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 重置BLE射频收发，将射频测试状态复位
- 复位结果通过ble_factory_rf_reset_callback回调异步返回
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_reset_cb已设置

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_reset_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_cali_nv <a id="ble_factory_rf_cali_nv"></a>

```c
errcode_t ble_factory_rf_cali_nv(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 校准BLE射频NV，执行射频校准并将校准数据写入NV存储区
- 校准结果通过ble_factory_rf_cali_nv_callback回调异步返回
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_cali_nv_cb已设置

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_cali_nv_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_rf_single_tone <a id="ble_factory_rf_single_tone"></a>

```c
errcode_t ble_factory_rf_single_tone(ble_rf_single_tone_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 设置BLE射频单音模式，用于产线射频单音测试场景，可开启或关闭单音
- 单音设置结果通过ble_factory_rf_single_tone_callback回调异步返回
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_rf_single_tone_cb已设置
- 入参param不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [ble_rf_single_tone_t](#struct_ble_rf_single_tone_t) * | 射频单音参数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_rf_single_tone_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

### ble_factory_vendor_productline_cmd <a id="ble_factory_vendor_productline_cmd"></a>

```c
errcode_t ble_factory_vendor_productline_cmd(ble_vendor_productline_cmd_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_factory.h"
```

**功能说明**

- 设置BLE产线校准命令，用于产线频偏校准、功率校准及其补偿值EFUSE读写
- 校准结果通过ble_factory_vendor_pdl_cmd_callback回调异步返回，携带ble_hci_vendor_productline_complete_t事件
- 依赖bts_def.h

**前置条件**

- BLE协议栈已初始化完成
- 已通过ble_factory_register_callbacks注册回调函数，且ble_vendor_pdl_cmd_cb已设置
- 入参param不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [ble_vendor_productline_cmd_t](#struct_ble_vendor_productline_cmd_t) * | 产线校准命令参数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功，读取结果将在ble_factory_vendor_pdl_cmd_callback中返回 | 请求成功发起 |
| Other | 其他错误码，参考errcode.h | 执行失败 |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为BLE Factory所有接口的返回值类型，用于表示接口调用的执行结果

## Enumerations

### ble_rf_phy_t <a id="enum_ble_rf_phy_t"></a>

```c
typedef enum {
    BLE_RF_PHY_1M = 0x01,      /*!< radio frequency physical: 1M */
    BLE_RF_PHY_2M = 0x02,      /*!< radio frequency physical: 2M */
    BLE_RF_PHY_S8 = 0x03,      /*!< radio frequency physical: coded PHY with S=8 data coding */
    BLE_RF_PHY_S2 = 0x04,      /*!< radio frequency physical: coded PHY with S=2 data coding */
} ble_rf_phy_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BLE_RF_PHY_1M | 0x01 | 射频物理层1M |
| BLE_RF_PHY_2M | 0x02 | 射频物理层2M |
| BLE_RF_PHY_S8 | 0x03 | 射频物理层 S=8数据编码的编码PHY (Physical Layer) |
| BLE_RF_PHY_S2 | 0x04 | 射频物理层 S=2数据编码的编码PHY |

### ble_rf_payload_type_t <a id="enum_ble_rf_payload_type_t"></a>

```c
typedef enum {
    BLE_RF_PATLOAD_TYPE_0 = 0x00,    /*!< radio frequency payload type:PRBS9 */
    BLE_RF_PATLOAD_TYPE_1 = 0x01,    /*!< radio frequency payload type:11110000 */
    BLE_RF_PATLOAD_TYPE_2 = 0x02,    /*!< radio frequency payload type:10101010 */
    BLE_RF_PATLOAD_TYPE_3 = 0x03,    /*!< radio frequency payload type:PRBS15 */
    BLE_RF_PATLOAD_TYPE_4 = 0x04,    /*!< radio frequency payload type:11111111 */
    BLE_RF_PATLOAD_TYPE_5 = 0x05,    /*!< radio frequency payload type:00000000 */
    BLE_RF_PATLOAD_TYPE_6 = 0x06,    /*!< radio frequency payload type:00001111 */
    BLE_RF_PATLOAD_TYPE_7 = 0x07,    /*!< radio frequency payload type:01010101 */
} ble_rf_payload_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BLE_RF_PATLOAD_TYPE_0 | 0x00 | 射频信息体类型:PRBS9 |
| BLE_RF_PATLOAD_TYPE_1 | 0x01 | 射频信息体类型:11110000 |
| BLE_RF_PATLOAD_TYPE_2 | 0x02 | 射频信息体类型:10101010 |
| BLE_RF_PATLOAD_TYPE_3 | 0x03 | 射频信息体类型:PRBS15 |
| BLE_RF_PATLOAD_TYPE_4 | 0x04 | 射频信息体类型:11111111 |
| BLE_RF_PATLOAD_TYPE_5 | 0x05 | 射频信息体类型:00000000 |
| BLE_RF_PATLOAD_TYPE_6 | 0x06 | 射频信息体类型:00001111 |
| BLE_RF_PATLOAD_TYPE_7 | 0x07 | 射频信息体类型:01010101 |

### ble_pdl_sub_opcode_t <a id="enum_ble_pdl_sub_opcode_t"></a>

```c
typedef enum {
    BTH_PRODUCTLINE_XO_TRIM = 0x01,                    // 设置频偏校准值，寄存器控
    BTH_PRODUCTLINE_XO_TRIM_RD_VAL = 0x02,             // 读取频偏校准寄存器值
    BTH_PRODUCTLINE_GET_TSENSOR_TEMPERATURE = 0x03,    // 获取芯片温度
    BTH_PRODUCTLINE_EFUSE_WRITE_XO_TRIM = 0x04,        // 将频偏校准值写入EFUSE
    BTH_PRODUCTLINE_EFUSE_READ_XO_TRIM = 0x05,         // 从EFUSE读取频偏校准值
    BTH_PRODUCTLINE_EFUSE_WRITE_TEMPERATURE = 0x06,    // 将产测时的芯片温度写入EFUSE
    BTH_PRODUCTLINE_EFUSE_READ_TEMPERATURE = 0x07,     // 从EFUSE读取产测温度
    BTH_PRODUCTLINE_PWR_CALI_SET_MEASSURED_PWR = 0x08, // 将实测发送功率发送给驱动
    BTH_PRODUCTLINE_PWR_CALI_GET_COMP_RESULT = 0x09,   // 获取功率校准结果
    BTH_BTH_PRODUCTLINE_PWR_CALI_APPLY_COMP = 0x0A,    // 应用功率校准结果
    BTH_PRODUCTLINE_EFUSE_WRITE_PWR_COMP = 0x0B,       // 将功率校准结果写入EFUSE
    BTH_PRODUCTLINE_EFUSE_READ_PWR_COMP = 0x0C,        // 从EFUSE读取功率校准结果
} ble_pdl_sub_opcode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BTH_PRODUCTLINE_XO_TRIM | 0x01 | 设置频偏校准值，寄存器控 |
| BTH_PRODUCTLINE_XO_TRIM_RD_VAL | 0x02 | 读取频偏校准寄存器值 |
| BTH_PRODUCTLINE_GET_TSENSOR_TEMPERATURE | 0x03 | 获取芯片温度 |
| BTH_PRODUCTLINE_EFUSE_WRITE_XO_TRIM | 0x04 | 将频偏校准值写入EFUSE |
| BTH_PRODUCTLINE_EFUSE_READ_XO_TRIM | 0x05 | 从EFUSE读取频偏校准值 |
| BTH_PRODUCTLINE_EFUSE_WRITE_TEMPERATURE | 0x06 | 将产测时的芯片温度写入EFUSE |
| BTH_PRODUCTLINE_EFUSE_READ_TEMPERATURE | 0x07 | 从EFUSE读取产测温度 |
| BTH_PRODUCTLINE_PWR_CALI_SET_MEASSURED_PWR | 0x08 | 将实测发送功率发送给驱动 |
| BTH_PRODUCTLINE_PWR_CALI_GET_COMP_RESULT | 0x09 | 获取功率校准结果 |
| BTH_BTH_PRODUCTLINE_PWR_CALI_APPLY_COMP | 0x0A | 应用功率校准结果 |
| BTH_PRODUCTLINE_EFUSE_WRITE_PWR_COMP | 0x0B | 将功率校准结果写入EFUSE |
| BTH_PRODUCTLINE_EFUSE_READ_PWR_COMP | 0x0C | 从EFUSE读取功率校准结果 |

## Associations

### ble_vendor_productline_cmd_t匿名联合体 <a id="union_ble_vendor_productline_cmd_t_unnamed"></a>

```c
union {
    pdl_xo_trim_t xo_trim;
    pdl_efuse_temp_wr_t efuse_wr_temp;
    pdl_pwr_cali_set_pwr_t pwr_info;
};
```

| 成员名称 | 类型 | 描述 | 接口使用逻辑 |
| ------- | ---- | ---- | ----------- |
| xo_trim | [pdl_xo_trim_t](#struct_pdl_xo_trim_t) | 频偏校准值 | ble_factory_vendor_productline_cmd入参载体，sub_opcode为BTH_PRODUCTLINE_XO_TRIM时使用 |
| efuse_wr_temp | [pdl_efuse_temp_wr_t](#struct_pdl_efuse_temp_wr_t) | EFUSE温度写入数据 | ble_factory_vendor_productline_cmd入参载体，sub_opcode为BTH_PRODUCTLINE_EFUSE_WRITE_TEMPERATURE时使用 |
| pwr_info | [pdl_pwr_cali_set_pwr_t](#struct_pdl_pwr_cali_set_pwr_t) | 功率校准数据 | ble_factory_vendor_productline_cmd入参载体，sub_opcode为BTH_PRODUCTLINE_PWR_CALI_SET_MEASSURED_PWR时使用 |

### ble_hci_vendor_productline_complete_t匿名联合体 <a id="union_ble_hci_vendor_productline_complete_t_unnamed"></a>

```c
union {
    ble_pdl_rd_xo_trim_t xo_trim_reg_val;
    ble_pdl_get_temp_t temp;
    ble_pdl_efuse_rd_xo_trim_t xo_trim;
    ble_pdl_efuse_rd_tmp_t temp_lvl;
    ble_pdl_pwr_cali_get_result_t pwr_result;
    ble_pdl_efuse_rd_pwr_result_t efuse_pwr_result;
};
```

| 成员名称 | 类型 | 描述 | 接口使用逻辑 |
| ------- | ---- | ---- | ----------- |
| xo_trim_reg_val | [ble_pdl_rd_xo_trim_t](#struct_ble_pdl_rd_xo_trim_t) | 频偏校准寄存器读取值 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_XO_TRIM_RD_VAL时使用 |
| temp | [ble_pdl_get_temp_t](#struct_ble_pdl_get_temp_t) | 芯片温度 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_GET_TSENSOR_TEMPERATURE时使用 |
| xo_trim | [ble_pdl_efuse_rd_xo_trim_t](#struct_ble_pdl_efuse_rd_xo_trim_t) | EFUSE频偏校准值 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_EFUSE_READ_XO_TRIM时使用 |
| temp_lvl | [ble_pdl_efuse_rd_tmp_t](#struct_ble_pdl_efuse_rd_tmp_t) | EFUSE产测温度等级 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_EFUSE_READ_TEMPERATURE时使用 |
| pwr_result | [ble_pdl_pwr_cali_get_result_t](#struct_ble_pdl_pwr_cali_get_result_t) | 功率校准结果 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_PWR_CALI_GET_COMP_RESULT时使用 |
| efuse_pwr_result | [ble_pdl_efuse_rd_pwr_result_t](#struct_ble_pdl_efuse_rd_pwr_result_t) | EFUSE功率校准结果 | ble_factory_vendor_pdl_cmd_callback出参载体，subcode为BTH_PRODUCTLINE_EFUSE_READ_PWR_COMP时使用 |

## Structures

### ble_rf_tx_start_t <a id="struct_ble_rf_tx_start_t"></a>

```c
typedef struct {
    uint8_t tx_freq;            /*!< tx frequency,Scope:0x00~0x27,2402+x*2 */
    uint16_t test_data_len;     /*!< tx test data len,Scope:0x00~0xFB */
    uint8_t payload_type;       /*!< palyload type { @ref ble_rf_payload_type_t } */
    uint8_t tx_phy;             /*!< radio frequency physical { @ref ble_rf_phy_t } */
} ble_rf_tx_start_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| tx_freq | uint8_t | 发送频率，范围:0x00~0x27，计算公式:2402+x*2 |
| test_data_len | uint16_t | 发射测试数据长度，范围:0x00~0xFB |
| payload_type | uint8_t | 发射测试数据包类型，取值参考[ble_rf_payload_type_t](#enum_ble_rf_payload_type_t) |
| tx_phy | uint8_t | 射频物理层，取值参考[ble_rf_phy_t](#enum_ble_rf_phy_t) |

### ble_rf_rx_start_t <a id="struct_ble_rf_rx_start_t"></a>

```c
typedef struct {
    uint8_t rx_freq;            /*!< tx frequency,Scope:0x00~0x27,2402+x*2 */
    uint8_t rx_phy;             /*!< radio frequency physical { @ref ble_rf_phy_t } */
    uint8_t modulation_index;   /*!< modulation index,0:standard,1:stable */
} ble_rf_rx_start_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rx_freq | uint8_t | 接收频率，范围:0x00~0x27，计算公式:2402+x*2 |
| rx_phy | uint8_t | 射频物理层，取值参考[ble_rf_phy_t](#enum_ble_rf_phy_t) |
| modulation_index | uint8_t | 调制指数，0:标准，1:稳定 |

### ble_rf_single_tone_t <a id="struct_ble_rf_single_tone_t"></a>

```c
typedef struct {
    uint8_t rf_freq;            /*!< tx frequency,Scope:0x00~0x4E,2402+x*2 */
    uint8_t rf_mode;            /*!< single tone mode,00:start,FF:stop */
} ble_rf_single_tone_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rf_freq | uint8_t | 发送频率，范围:0x00~0x4E，计算公式:2402+x*2 |
| rf_mode | uint8_t | 单音模式，0x00:开启，0xFF:关闭 |

### pdl_xo_trim_t <a id="struct_pdl_xo_trim_t"></a>

```c
typedef struct {
    uint8_t coarse_val;
    uint8_t fine_val;
} pdl_xo_trim_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| coarse_val | uint8_t | 频偏校准粗调值 |
| fine_val | uint8_t | 频偏校准细调值 |

### pdl_efuse_temp_wr_t <a id="struct_pdl_efuse_temp_wr_t"></a>

```c
typedef struct {
    int16_t temp;
} pdl_efuse_temp_wr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| temp | int16_t | 产测时芯片温度值 |

### pdl_pwr_cali_set_pwr_t <a id="struct_pdl_pwr_cali_set_pwr_t"></a>

```c
typedef struct {
    int16_t target_pwr;
    int16_t pwr;
} pdl_pwr_cali_set_pwr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| target_pwr | int16_t | 目标发射功率 |
| pwr | int16_t | 实测发送功率 |

### ble_vendor_productline_cmd_t <a id="struct_ble_vendor_productline_cmd_t"></a>

```c
typedef struct {
    uint8_t sub_opcode; // subcode
    union {
        pdl_xo_trim_t xo_trim;
        pdl_efuse_temp_wr_t efuse_wr_temp;
        pdl_pwr_cali_set_pwr_t pwr_info;
    };
} ble_vendor_productline_cmd_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| sub_opcode | uint8_t | 产线校准命令子操作码，取值参考[ble_pdl_sub_opcode_t](#enum_ble_pdl_sub_opcode_t) |
| xo_trim | [pdl_xo_trim_t](#struct_pdl_xo_trim_t) | 频偏校准值，sub_opcode为BTH_PRODUCTLINE_XO_TRIM时使用 |
| efuse_wr_temp | [pdl_efuse_temp_wr_t](#struct_pdl_efuse_temp_wr_t) | EFUSE温度写入数据，sub_opcode为BTH_PRODUCTLINE_EFUSE_WRITE_TEMPERATURE时使用 |
| pwr_info | [pdl_pwr_cali_set_pwr_t](#struct_pdl_pwr_cali_set_pwr_t) | 功率校准数据，sub_opcode为BTH_PRODUCTLINE_PWR_CALI_SET_MEASSURED_PWR时使用 |

### ble_pdl_rd_xo_trim_t <a id="struct_ble_pdl_rd_xo_trim_t"></a>

```c
#pragma pack(1)
typedef struct {
    uint8_t sel;
    uint8_t coarse_val;
    uint8_t fine_val;
} ble_pdl_rd_xo_trim_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| sel | uint8_t | 频偏校准寄存器选择值 |
| coarse_val | uint8_t | 频偏校准粗调值 |
| fine_val | uint8_t | 频偏校准细调值 |

### ble_pdl_get_temp_t <a id="struct_ble_pdl_get_temp_t"></a>

```c
typedef struct {
    int16_t temp;
} ble_pdl_get_temp_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| temp | int16_t | 芯片温度值 |

### ble_pdl_efuse_rd_xo_trim_t <a id="struct_ble_pdl_efuse_rd_xo_trim_t"></a>

```c
typedef struct {
    uint8_t coarse_val;
    uint8_t fine_val;
} ble_pdl_efuse_rd_xo_trim_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| coarse_val | uint8_t | EFUSE频偏校准粗调值 |
| fine_val | uint8_t | EFUSE频偏校准细调值 |

### ble_pdl_efuse_rd_tmp_t <a id="struct_ble_pdl_efuse_rd_tmp_t"></a>

```c
typedef struct {
    uint8_t temp_lvl;
} ble_pdl_efuse_rd_tmp_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| temp_lvl | uint8_t | EFUSE产测温度等级 |

### ble_pdl_pwr_cali_get_result_t <a id="struct_ble_pdl_pwr_cali_get_result_t"></a>

```c
typedef struct {
    int16_t curve_c_offset;
} ble_pdl_pwr_cali_get_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| curve_c_offset | int16_t | 功率校准曲线C偏移值 |

### ble_pdl_efuse_rd_pwr_result_t <a id="struct_ble_pdl_efuse_rd_pwr_result_t"></a>

```c
typedef struct {
    int16_t curve_c_offset;
} ble_pdl_efuse_rd_pwr_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| curve_c_offset | int16_t | EFUSE功率校准曲线C偏移值 |

### ble_hci_vendor_productline_complete_t <a id="struct_ble_hci_vendor_productline_complete_t"></a>

```c
#pragma pack(1)
typedef struct {
    uint8_t nb;
    uint16_t opcode;
    uint8_t status;
    uint8_t subcode;
    union {
        ble_pdl_rd_xo_trim_t xo_trim_reg_val;
        ble_pdl_get_temp_t temp;
        ble_pdl_efuse_rd_xo_trim_t xo_trim;
        ble_pdl_efuse_rd_tmp_t temp_lvl;
        ble_pdl_pwr_cali_get_result_t pwr_result;
        ble_pdl_efuse_rd_pwr_result_t efuse_pwr_result;
    };
} ble_hci_vendor_productline_complete_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| nb | uint8_t | 事件参数数量 |
| opcode | uint16_t | HCI操作码 |
| status | uint8_t | 执行状态 |
| subcode | uint8_t | 子操作码，取值参考[ble_pdl_sub_opcode_t](#enum_ble_pdl_sub_opcode_t) |
| xo_trim_reg_val | [ble_pdl_rd_xo_trim_t](#struct_ble_pdl_rd_xo_trim_t) | 频偏校准寄存器读取值，subcode为BTH_PRODUCTLINE_XO_TRIM_RD_VAL时使用 |
| temp | [ble_pdl_get_temp_t](#struct_ble_pdl_get_temp_t) | 芯片温度，subcode为BTH_PRODUCTLINE_GET_TSENSOR_TEMPERATURE时使用 |
| xo_trim | [ble_pdl_efuse_rd_xo_trim_t](#struct_ble_pdl_efuse_rd_xo_trim_t) | EFUSE频偏校准值，subcode为BTH_PRODUCTLINE_EFUSE_READ_XO_TRIM时使用 |
| temp_lvl | [ble_pdl_efuse_rd_tmp_t](#struct_ble_pdl_efuse_rd_tmp_t) | EFUSE产测温度等级，subcode为BTH_PRODUCTLINE_EFUSE_READ_TEMPERATURE时使用 |
| pwr_result | [ble_pdl_pwr_cali_get_result_t](#struct_ble_pdl_pwr_cali_get_result_t) | 功率校准结果，subcode为BTH_PRODUCTLINE_PWR_CALI_GET_COMP_RESULT时使用 |
| efuse_pwr_result | [ble_pdl_efuse_rd_pwr_result_t](#struct_ble_pdl_efuse_rd_pwr_result_t) | EFUSE功率校准结果，subcode为BTH_PRODUCTLINE_EFUSE_READ_PWR_COMP时使用 |

### ble_factory_callbacks_t <a id="struct_ble_factory_callbacks_t"></a>

```c
typedef struct {
    ble_factory_rf_tx_start_callback ble_rf_tx_start_cb;         /*!< radio frequency tx start callback. */
    ble_factory_rf_rx_start_callback ble_rf_rx_start_cb;         /*!< radio frequency rx start callback. */
    ble_factory_rf_trx_end_callback ble_rf_trx_end_cb;           /*!< radio frequency tx and rx end callback. */
    ble_factory_rf_reset_callback ble_rf_reset_cb;               /*!< radio frequency tx and rx reset callback. */
    ble_factory_rf_cali_nv_callback ble_rf_cali_nv_cb;           /*!< radio frequency cali nv callback. */
    ble_factory_rf_single_tone_callback ble_rf_single_tone_cb;   /*!< radio frequency single tone mode callback. */
    ble_factory_vendor_pdl_cmd_callback ble_vendor_pdl_cmd_cb;   /*!< productline calibration vendor cmd callback. */
} ble_factory_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ble_rf_tx_start_cb | ble_factory_rf_tx_start_callback | 射频长发回调函数 |
| ble_rf_rx_start_cb | ble_factory_rf_rx_start_callback | 射频长收回调函数 |
| ble_rf_trx_end_cb | ble_factory_rf_trx_end_callback | 射频收发结束回调函数 |
| ble_rf_reset_cb | ble_factory_rf_reset_callback | 射频收发复位回调函数 |
| ble_rf_cali_nv_cb | ble_factory_rf_cali_nv_callback | 校准射频NV回调函数 |
| ble_rf_single_tone_cb | ble_factory_rf_single_tone_callback | 设置射频单音模式回调函数 |
| ble_vendor_pdl_cmd_cb | ble_factory_vendor_pdl_cmd_callback | 产线校准命令回调函数 |


