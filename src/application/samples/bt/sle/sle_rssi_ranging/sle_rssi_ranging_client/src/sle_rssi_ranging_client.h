/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging Client interface. \n
 * @else
 * @brief SLE RSSI 测距 Client 接口。 \n
 * @endif
 */
#ifndef SLE_RSSI_RANGING_CLIENT_H
#define SLE_RSSI_RANGING_CLIENT_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize Client discovery, connection, calibration and RSSI polling.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 初始化 Client 的扫描、连接、校准和 RSSI 轮询功能。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_client_init(void);

#endif
