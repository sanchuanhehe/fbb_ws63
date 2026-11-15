 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: led control config. \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
 
#ifndef BLE_LED_SERVICE_H
#define BLE_LED_SERVICE_H

#include <stdint.h>
#include "errcode.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// 以下为设置灯颜色的4个级别, 每个级别对应不同的颜色
#define MAX_LEVEL0    (32767 / 4)       // 8191
#define MAX_LEVEL1    (32767 / 2)       // 16383
#define MAX_LEVEL2    (32767 / 4 * 3)   // 24575
#define MAX_LEVEL3    (32767)           // 32767
#define MAX_LEVEL4    ((65535 / 8) * 5) // 40955
#define MAX_LEVEL5    ((65535 / 8) * 6) // 49146
#define MAX_LEVEL6    ((65535 / 8) * 7) // 57337

typedef enum {
    MESH_LED_ON,
    MESH_LED_OFF
} mesh_led_bool_t;

void set_led_state(mesh_led_bool_t on);
void set_led_light(uint16_t valve);
void set_led_gpio(char* color, uint8_t gpio_num);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif