/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE PHY/MCS dynamic switch Server interface. \n
 * @else
 * @brief SLE PHY/MCS 动态切换 Server 接口。 \n
 * @endif
 */
#ifndef SLE_PHY_MCS_SWITCH_SERVER_H
#define SLE_PHY_MCS_SWITCH_SERVER_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize Server announcement, RSSI polling and adaptive switching.
 * @else
 * @brief 初始化 Server 广播、RSSI 轮询和自适应切换。
 * @endif
 */
errcode_t sle_phy_mcs_switch_server_init(void);

#endif
