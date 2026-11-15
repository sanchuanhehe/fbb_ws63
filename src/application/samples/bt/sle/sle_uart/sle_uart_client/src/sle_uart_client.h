/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Declares the public interface of the SLE UART client.
 * @else
 * @brief 声明 SLE UART 客户端的公共接口。
 * @endif
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_UART_CLIENT_H
#define SLE_UART_CLIENT_H

#include <stdint.h>
#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_uart_client_init.
 * @else
 * @brief 初始化 \c sle_uart_client_init 对应的功能。
 * @endif
 */
void sle_uart_client_init(ssapc_notification_callback notification_cb, ssapc_write_cfm_callback write_cfm_cb);

/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_uart_client_is_connected(void);

/**
 * @if Eng
 * @brief Starts scanning for the target SLE server.
 * @else
 * @brief 开始扫描目标 SLE 服务端。
 * @endif
 */
void sle_uart_client_start_scan(void);

/* Connection handle used by the forwarding task. / 透传任务使用的连接句柄。 */
extern uint16_t g_conn_id;
extern unsigned long g_sle_uart_client_msgq_id;
extern ssapc_write_param_t g_write_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
