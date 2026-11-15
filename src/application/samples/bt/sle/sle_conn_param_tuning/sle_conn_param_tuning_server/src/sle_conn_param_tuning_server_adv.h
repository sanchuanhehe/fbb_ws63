/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE connection parameter tuning Server announcement interface. \n
 * @else
 * @brief SLE 连接参数调优 Server 广播接口。 \n
 * @endif
 */
#ifndef SLE_CONN_PARAM_TUNING_SERVER_ADV_H
#define SLE_CONN_PARAM_TUNING_SERVER_ADV_H

#include <stdint.h>
#include "errcode.h"

#define SLE_CONN_PARAM_ADV_HANDLE 1

typedef struct {
    uint8_t length;  /*!< @if Eng Field length. @else 字段长度。 @endif */
    uint8_t type;    /*!< @if Eng Field type. @else 字段类型。 @endif */
    uint8_t value;   /*!< @if Eng Field value. @else 字段值。 @endif */
} sle_conn_param_adv_common_value_t;

/**
 * @if Eng
 * @brief Register Server announcement callbacks.
 * @else
 * @brief 注册 Server 广播回调。
 * @endif
 */
errcode_t sle_conn_param_tuning_announce_register_callbacks(void);

/**
 * @if Eng
 * @brief Configure and start connectable Server announcement.
 * @else
 * @brief 配置并启动 Server 可连接广播。
 * @endif
 */
errcode_t sle_conn_param_tuning_server_announce_start(void);

#endif
