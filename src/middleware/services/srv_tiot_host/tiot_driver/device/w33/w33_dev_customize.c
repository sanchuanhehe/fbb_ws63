/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: W33 device customize info. \n
 *
 * History: \n
 * 2024-06-06, Create file. \n
 */

#include "w33_service.h"
#include "tiot_board_log.h"
#include "tiot_autoconfig.h"
#include "w33_dev_customize.h"

static tiot_dev_customize w33_dev_cus[CONFIG_W33_DEV_NUM] = { 0 };

static const uint32_t w33_dev_customize[DEV_CUS_MAX_INDEX] = {
    [DEV_CUS_TCXO_CLK_FREQ] = CONFIG_W33_TCXO_FREQ,
    [DEV_CUS_GUART_BAUD_RATE] = CONFIG_W33_UART_BAUDRATE,
    [DEV_CUS_CLKPLL_CLK_SEL] = CONFIG_W33_CLKPLL_CLK_SEL,
};

const tiot_dev_customize *w33_dev_customize_get(uint8_t index)
{
    return &w33_dev_cus[index];
}

void w33_dev_customize_init(void)
{
    int i;
    for (i = 0; i < CONFIG_W33_DEV_NUM; i++) {
        w33_dev_cus[i].start_data = (uintptr_t)w33_dev_customize;
        w33_dev_cus[i].curr_data = (uintptr_t)w33_dev_customize;
        w33_dev_cus[i].end_data = (uintptr_t)(w33_dev_customize + (DEV_CUS_MAX_INDEX - 1));
        w33_dev_cus[i].unit_data_len = sizeof(uint32_t);
    }
}