/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:
 *
 * History:
 * 2025-06-04, Create file.
 */

#ifndef __AT_BT_UTILS_H__
#define __AT_BT_UTILS_H__

#include "mesh/mesh.h"
#include "ble_mesh_errcode.h"

#define ASSIGN_PARA(argv, para, argc) \
    ({                                \
        if ((para) != NULL) {         \
            (argv)[(argc)++] = para;  \
        }                             \
    })

#define BLE_MESH_NUM_2 2
#define BLE_MESH_NUM_4 4

#define BT_INFO_SHELL(fmt, args...) osal_printk("%s: " fmt "\n", __func__, ##args)

#define SHELL_PARAM_RANGE_CHECK_IN(name, v, min, max)                          \
    do {                                                                       \
        if (!shell_check_str(v)) {                                             \
            BT_ERR("%s is an illegal value!", v);                              \
            return ERRCODE_BLE_MESH_FAIL;                                              \
        }                                                                      \
        int __v = atoi(v);                                                     \
        if (__v < (min) || __v > (max)) {                                      \
            BT_ERR("can't set %s to %s, range is [%d,%d]", name, v, min, max); \
            return ERRCODE_BLE_MESH_FAIL;                                              \
        }                                                                      \
    } while (0)

#define SHELL_PARAM_RANGE_CHECK_OUT(name, v, min, max)          \
    do {                                                        \
        if (!shell_check_str(v)) {                              \
            BT_ERR("%s is an illegal value!", v);               \
            return ERRCODE_BLE_MESH_FAIL;                               \
        }                                                       \
        int __v = atoi(v);                                      \
        if (__v >= (min) && __v <= (max)) {                     \
            BT_ERR("can't set %s to %s, Prohibited!", name, v); \
            return ERRCODE_BLE_MESH_FAIL;                               \
        }                                                       \
    } while (0)

#define SHELL_PARAM_UINT(param, v)                                         \
    do {                                                                   \
        if (!shell_check_str(v)) {                                         \
            BT_ERR("can't set %s to %s, incorrect data type!", #param, v); \
            return ERRCODE_BLE_MESH_FAIL;                                          \
        }                                                                  \
        int __v = atou(v, sizeof(param));                                  \
        if (__v < 0) {                                                     \
            BT_ERR("can't set %s to %s, out of range!", #param, v);        \
            return ERRCODE_BLE_MESH_FAIL;                                          \
        }                                                                  \
        (param) = __v;                                                     \
    } while (0)

#define SHELL_PARAM_INT(param, v)                                          \
    do {                                                                   \
        if (!shell_check_str(v)) {                                         \
            BT_ERR("can't set %s to %s, incorrect data type!", #param, v); \
            return ERRCODE_BLE_MESH_FAIL;                                          \
        }                                                                  \
        int __v = atoi(v);                                                 \
        (param) = __v;                                                     \
    } while (0)

bool shell_check_str(const char *s);
int atou(const char *v, size_t size);
int check_dtt(uint8_t dtt);
void char_to_digit_array(uint8_t *out, const char *in, int max_len);
uint8_t bt_mesh_at_str_to_data(const char  *str, uint16_t data_len, uint8_t *data);
void arg_str_to_key(const char *str, uint8_t *key, int key_len);
int cmd_bt_mesh_ctx_info_set(struct bt_mesh_msg_ctx *ctx, const char *argv[]);
#endif