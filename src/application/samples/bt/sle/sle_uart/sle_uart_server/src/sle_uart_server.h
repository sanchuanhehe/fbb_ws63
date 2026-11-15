/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Declares the public interface of the SLE UART server.
 * @else
 * @brief 声明 SLE UART 服务端的公共接口。
 * @endif
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_UART_SERVER_H
#define SLE_UART_SERVER_H

#include <stdint.h>
#include "sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID. / 服务 UUID。 */
#define SLE_UART_SERVER_SERVICE 0x2222

/* Property UUID. / 属性 UUID。 */
#define SLE_UART_SERVER_NTF_REPORT 0x2323

/* Property permissions: read and write. / 属性权限：读和写。 */
#define SLE_UART_SRV_PROPERTIES (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Supported operations: read, write, and notify. / 支持的操作：读、写和通知。 */
#define SLE_UART_SRV_OPERATION \
    (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE | SSAP_OPERATE_INDICATION_BIT_NOTIFY)

/* Descriptor permissions. / 描述符权限。 */
#define SLE_UART_SRV_DESCRIPTOR (SSAP_PERMISSION_READ)

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_uart_server_init.
 * @else
 * @brief 初始化 \c sle_uart_server_init 对应的功能。
 * @endif
 */
errcode_t sle_uart_server_init(ssaps_read_request_callback read_cb, ssaps_write_request_callback write_cb);

/**
 * @if Eng
 * @brief Sends UART data to the peer through an SLE notification.
 * @else
 * @brief 通过 SLE 通知向对端发送串口数据。
 * @endif
 */
errcode_t sle_uart_server_send_notification(const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_uart_server_is_connected(void);

extern unsigned long g_sle_uart_server_msgq_id;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
