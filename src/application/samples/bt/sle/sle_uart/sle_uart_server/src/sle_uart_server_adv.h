/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Declares advertising types and interfaces for the SLE UART server.
 * @else
 * @brief 声明 SLE UART 服务端的广播类型与接口。
 * @endif
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_UART_SERVER_ADV_H
#define SLE_UART_SERVER_ADV_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief Defines a data type used by this sample.
 * @else
 * @brief 定义本案例使用的数据类型。
 * @endif
 */
typedef struct sle_adv_common_value {
    uint8_t length;
    uint8_t type;
    uint8_t value;
} le_adv_common_t;

/**
 * @if Eng
 * @brief Defines a data type used by this sample.
 * @else
 * @brief 定义本案例使用的数据类型。
 * @endif
 */
typedef enum sle_adv_channel {
    SLE_ADV_CHANNEL_MAP_77                 = 0x01,
    SLE_ADV_CHANNEL_MAP_78                 = 0x02,
    SLE_ADV_CHANNEL_MAP_79                 = 0x04,
    SLE_ADV_CHANNEL_MAP_DEFAULT            = 0x07
} sle_adv_channel_map_t;

/**
 * @if Eng
 * @brief Defines a data type used by this sample.
 * @else
 * @brief 定义本案例使用的数据类型。
 * @endif
 */
typedef enum sle_adv_data {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL                              = 0x01,
    SLE_ADV_DATA_TYPE_ACCESS_MODE                                  = 0x02,
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID                      = 0x03,
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID                     = 0x04,
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS         = 0x05,
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS        = 0x06,
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS       = 0x07,
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS      = 0x08,
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE                 = 0x09,
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME                         = 0x0A,
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME                          = 0x0B,
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL                               = 0x0C,
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN                     = 0x0D,
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID                    = 0x0E,
    SLE_ADV_DATA_TYPE_EXTENDED                                     = 0xFE,
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA                   = 0xFF
} sle_adv_data_type;

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_uart_server_announce_register_cbks.
 * @else
 * @brief 注册 \c sle_uart_server_announce_register_cbks 所需的回调函数。
 * @endif
 */
errcode_t sle_uart_server_announce_register_cbks(void);
/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_uart_server_adv_init.
 * @else
 * @brief 初始化 \c sle_uart_server_adv_init 对应的功能。
 * @endif
 */
errcode_t sle_uart_server_adv_init(void);
/**
 * @if Eng
 * @brief Implements the internal workflow of \c sle_start_announce.
 * @else
 * @brief 实现 \c sle_start_announce 对应的内部处理流程。
 * @endif
 */
errcode_t sle_start_announce(uint8_t announce_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
