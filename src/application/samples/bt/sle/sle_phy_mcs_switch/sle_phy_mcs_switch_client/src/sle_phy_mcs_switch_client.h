/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE PHY/MCS dynamic switch Client interface. \n
 * @else
 * @brief SLE PHY/MCS 动态切换 Client 接口。 \n
 * @endif
 */
#ifndef SLE_PHY_MCS_SWITCH_CLIENT_H
#define SLE_PHY_MCS_SWITCH_CLIENT_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize Client discovery, connection and PHY-update callbacks.
 * @else
 * @brief 初始化 Client 的扫描、连接和 PHY 更新回调。
 * @endif
 */
errcode_t sle_phy_mcs_switch_client_init(void);

#endif
