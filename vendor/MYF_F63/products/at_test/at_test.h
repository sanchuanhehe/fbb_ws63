/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: AT_TEST Sample Source. \n
 *
 * History: \n
 * 2023-07-06, Create file. \n
 */

#ifndef AT_TEST_H
#define AT_TEST_H

#include "pinctrl.h"
#include "soc_osal.h"
#include "app_init.h"
#include "at.h"
#include "at_cmd_register.h"
#include "sle_ssap_client.h"
#include "sle_uuid_client.h"
#include "sle_uuid_server.h"
#include "sle_server_adv.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "uart_porting.h"

#define AT_TEST_TASK_PRIO 24
#define AT_TEST_TASK_STACK_SIZE 0x1000
#define AT_MY_FUNC_NUM (sizeof(at_myf_parse_table) / sizeof(at_myf_parse_table[0]))
#define MY_EXT_AT_MACSTR "%02x%02x%02x%02x%02x%02x"
#define MAC_HEX_LEN 12
#define MAC_LEN 6

typedef struct {
    uint32_t para_map;
    uint8_t *para1;
} AtSleConnByMac_t;

typedef struct {
    uint32_t para_map;
    int para1;
    int para2;
    uint8_t *para3;
} AtSlesendT;

typedef struct {
    uint32_t para_map;
    uint8_t para1;
} AtSleadvT;

/* AT+SLECONNBYMAC */
at_ret_t SLECONNBYMAC_my_set(AtSleConnByMac_t *args);

/* AT+SLESEND */
at_ret_t SLESEND_my_set(AtSlesendT *args);

/* AT+SLEADV */
at_ret_t SLEADV_my_set(AtSleadvT *args);

#endif