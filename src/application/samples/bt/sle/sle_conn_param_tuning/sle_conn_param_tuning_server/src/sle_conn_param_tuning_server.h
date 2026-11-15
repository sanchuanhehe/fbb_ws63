/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE connection parameter tuning Server interface. \n
 * @else
 * @brief SLE 连接参数调优 Server 接口。 \n
 * @endif
 */
#ifndef SLE_CONN_PARAM_TUNING_SERVER_H
#define SLE_CONN_PARAM_TUNING_SERVER_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize SLE, callbacks and announcement for the tuning Server.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief 初始化调优 Server 的 SLE、回调和广播。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t sle_conn_param_tuning_server_init(void);

#endif
