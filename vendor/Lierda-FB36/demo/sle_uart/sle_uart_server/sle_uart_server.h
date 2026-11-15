/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE UUID Server module.
 */

/**
 * @defgroup SLE UUID SERVER API
 * @ingroup
 * @{
 */
#ifndef SLE_UUID_SERVER_H
#define SLE_UUID_SERVER_H

#include "sle_ssap_server.h"

/* sle define */
/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0xABCD

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x1122

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

#define OCTET_BIT_LEN   8
#define UUID_LEN_2      2
#define BT_INDEX_4      4
#define BT_INDEX_5      5
#define BT_INDEX_0      0
#define BT_INDEX_14     14

#define UART_DEFAULT_KTHREAD_SIZE 0x2000
#define UART_DEFAULT_KTHREAD_PROI 26
#define DEFAULT_SLE_UART_DATA_LEN 1500
#define DEFAULT_SLE_UART_MTU_SIZE 1500
#define UART_DEFAULT_CONN_INTERVAL 0x64
#define UART_DEFAULT_TIMEOUT_MULTIPLIER 0x1f4
#define UART_DURATION_MS                2000

/* uart define */
#define CONFIG_SLE_UART_BUS 0
#define CONFIG_UART_TXD_PIN 17
#define CONFIG_UART_RXD_PIN 18
#define SLE_UART_BAUDRATE   115200
#define SLE_UART_TRANSFER_SIZE   256

void sle_enable_server_cbk(void);
#endif
