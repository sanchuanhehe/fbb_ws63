/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE PHY/MCS dynamic switch Server announcement interface. \n
 * @else
 * @brief SLE PHY/MCS 动态切换 Server 广播接口。 \n
 * @endif
 */
#ifndef SLE_PHY_MCS_SWITCH_SERVER_ADV_H
#define SLE_PHY_MCS_SWITCH_SERVER_ADV_H

#include <stdint.h>
#include "errcode.h"

#define SLE_PHY_MCS_ADV_HANDLE 1

typedef struct {
    uint8_t length;  /*!< @if Eng Field length. @else 字段长度。 @endif */
    uint8_t type;    /*!< @if Eng Field type. @else 字段类型。 @endif */
    uint8_t value;   /*!< @if Eng Field value. @else 字段值。 @endif */
} sle_phy_mcs_adv_common_value_t;

/**
 * @if Eng
 * @brief Register Server announcement callbacks.
 * @else
 * @brief 注册 Server 广播回调。
 * @endif
 */
errcode_t sle_phy_mcs_switch_announce_register_callbacks(void);

/**
 * @if Eng
 * @brief Configure and start connectable Server announcement.
 * @else
 * @brief 配置并启动 Server 可连接广播。
 * @endif
 */
errcode_t sle_phy_mcs_switch_server_announce_start(void);

#endif
