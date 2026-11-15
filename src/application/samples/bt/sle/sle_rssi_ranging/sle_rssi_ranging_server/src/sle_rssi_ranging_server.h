/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging Server interface. \n
 * @else
 * @brief SLE RSSI 测距 Server 接口。 \n
 * @endif
 */
#ifndef SLE_RSSI_RANGING_SERVER_H
#define SLE_RSSI_RANGING_SERVER_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize SLE, callbacks and connectable announcement for the Server.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 初始化 Server 的 SLE、回调和可连接广播。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_server_init(void);

#endif
