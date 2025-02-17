/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE UART sample of client. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */
#ifndef SLE_UART_CLIENT_H
#define SLE_UART_CLIENT_H

#include "sle_ssap_client.h"
/* 任务相关*/
#define SLE_SERVER_TASK_PRIO 24
#define SLE_SERVER_STACK_SIZE 0x2000
/* 串口接收数据结构体 */
typedef struct {
    uint8_t *value;
    uint16_t value_len;
} msg_data_t;
/* 串口接收io*/
#define CONFIG_UART_TXD_PIN 17
#define CONFIG_UART_RXD_PIN 18
#define CONFIG_UART_PIN_MODE 1
#define CONFIG_UART_ID UART_BUS_0
void sle_client_init(void);

void sle_uart_start_scan(void);

void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);
void sle_uart_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);
void app_uart_init_config(void);
#endif