/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#ifndef BTH_GLE_SERVICE_SERVICE_ACCESS_PROTOCOL_CLIENT_H
#define BTH_GLE_SERVICE_SERVICE_ACCESS_PROTOCOL_CLIENT_H

#include "sdk_list.h"

#define btsrv_send_sle_sspac_msg(id, p0, p1, p2)          \
    btsrv_send_simple_msg(BTSRV_MSG_ID_SLE_SSAPC_MANAGER, \
                          (uintptr_t)(id), (uintptr_t)(p0), (uintptr_t)(p1), (uintptr_t)(p2))

/* 本地服务管理客户端模块消息定义 */
enum {
    SLE_SSAPC_MSG_ID_FIND_STRUCTURE,
    SLE_SSAPC_MSG_ID_FIND_PROPERTY,
    SLE_SSAPC_MSG_ID_FIND_STRUCTURE_COMPLETE,
    SLE_SSAPC_MSG_ID_READ_BY_UUID_COMPLETE,
    SLE_SSAPC_MSG_ID_READ_CFM,
    SLE_SSAPC_MSG_ID_WRITE_CFM,
    SLE_SSAPC_MSG_ID_EXCHANGE_INFO,
    SLE_SSAPC_MSG_ID_NOTIFICATION,
    SLE_SSAPC_MSG_ID_INDICATION
};

void ssapc_client_init(void);
#endif