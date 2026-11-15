/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE connection parameter tuning Client interface. \n
 * @else
 * @brief SLE 连接参数调优 Client 接口。 \n
 * @endif
 */
#ifndef SLE_CONN_PARAM_TUNING_CLIENT_H
#define SLE_CONN_PARAM_TUNING_CLIENT_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initialize Client discovery, connection and parameter-update callbacks.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief 初始化 Client 的扫描、连接及参数更新回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t sle_conn_param_tuning_client_init(void);

#endif
