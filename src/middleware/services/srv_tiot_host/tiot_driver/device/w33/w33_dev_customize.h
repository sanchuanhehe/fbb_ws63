/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: W33 device sustomize header. \n
 *
 * History: \n
 * 2024-06-06, Create file. \n
 */
#ifndef W33_DEVICE_CUSTOMIZE_H
#define W33_DEVICE_CUSTOMIZE_H

#include "tiot_device_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup w33_device_customize  W33 Device Customize
 * @ingroup  device_w33
 * @{
 */

/**
 * @brief device customize index.
 */
enum {
    DEV_CUS_TCXO_CLK_FREQ,      /*!< TCXO clock frequency. */
    DEV_CUS_GUART_BAUD_RATE,    /*!< Baud rate of GUART. */
    DEV_CUS_CLKPLL_CLK_SEL,     /*!< Clock input selection. 0: Single-ended clock input 1: Differential clock input. */
    DEV_CUS_MAX_INDEX
};

enum {
    DEV_CUS_RET_OK,
    DEV_CUS_RET_ERR,
    DEV_CUS_RET_NO_CUSTOMIZ,
};

/**
 * @brief  w33 device customize get, same for all w33 devices.
 * @return w33 device customize.
 */
const tiot_dev_customize *w33_dev_customize_get(uint8_t index);

/**
 * @brief  w33 device customize init.
 */
void w33_dev_customize_init(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif