/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging Server announcement interface. \n
 * @else
 * @brief SLE RSSI 测距 Server 广播接口。 \n
 * @endif
 */
#ifndef SLE_RSSI_RANGING_SERVER_ADV_H
#define SLE_RSSI_RANGING_SERVER_ADV_H

#include <stdint.h>
#include "errcode.h"

#define SLE_RSSI_RANGING_ADV_HANDLE 1

/**
 * @if Eng
 * @brief Common length-type-value field used in announcement data.
 * @else
 * @brief 广播数据使用的通用长度、类型和值字段。
 * @endif
 */
typedef struct {
    uint8_t length;  /*!< @if Eng Field length. @else 字段长度。 @endif */
    uint8_t type;    /*!< @if Eng Field type. @else 字段类型。 @endif */
    uint8_t value;   /*!< @if Eng Field value. @else 字段值。 @endif */
} sle_rssi_ranging_adv_common_value_t;

/**
 * @if Eng
 * @brief Register Server announcement callbacks.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 注册 Server 广播回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_announce_register_callbacks(void);

/**
 * @if Eng
 * @brief Configure announcement parameters and payload, then start announcing.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 配置广播参数和数据并启动广播。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_server_announce_start(void);

#endif
