/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell source
 *
 * History:
 * 2025-5-26, Create file.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mesh/mesh.h"
#include "at.h"
#include "ble_mesh_errcode.h"

#include "shell_utils.h"

bool shell_check_str(const char *s)
{
    while (isspace(*s)) {
        s++;
    }
    if (*s == '\0') {
        return false;
    }
    if (*s == '-' || *s == '+') {
        s++;
    }
    for (; *s != '\0'; s++) {
        if (isspace(*s)) {
            return false;
        }
        if (*s > '9' || *s < '0') {
            return false;
        }
    }
    return true;
}

int atou(const char *v, size_t size)
{
    uint32_t max = 0;

    if (size == 1) { // 1:OCTA
        max = BIT_MASK(8); // 8:BIT
    } else if (size == 2) { // 2:OCTA
        max = BIT_MASK(16); // 16:BIT
    } else if (size == 4) { // 4:OCTA
        max = 0xffffffff; // 32:BIT
    }

    int vv = atoi(v);
    if (vv < 0 || vv > max) {
        BT_INFO("input = %s is out of range [0, %u] (%d bytes unsigned integer)", v, max, size);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return vv;
}

int check_dtt(uint8_t dtt)
{
    uint8_t step = dtt & 0x3F;
    if (step == 0x3F) {
        BT_ERR("tt can not set to %u", dtt);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

void char_to_digit_array(uint8_t *out, const char *in, int max_len)
{
    for (int i = 0; i < max_len && in[i] != '\0'; i++) {
        if (isdigit(in[i])) {
            out[i] = (uint8_t)(in[i] - '0');  // 字符 '0'-'9' → 数字 0-9
        } else {
            out[i] = 0;  // 非数字字符处理（可选错误处理）
        }
    }
}

uint8_t bt_mesh_at_str_to_data(const char *str, uint16_t data_len, uint8_t *data)
{
    const char *str_cpy = str;
    uint16_t len = (uint16_t)strlen(str_cpy);
    uint8_t num_h;
    uint8_t num_l;

    if (len != data_len * BLE_MESH_NUM_2) {
        if (len != data_len * BLE_MESH_NUM_2 + BLE_MESH_NUM_2) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (!(str_cpy[0] == '0' && (str_cpy[1] == 'x' || str_cpy[1] == 'X'))) {
            return AT_RET_SYNTAX_ERROR;
        }
        str_cpy += BLE_MESH_NUM_2;
        len -= BLE_MESH_NUM_2; /* 2 0x or 0X length */
    }
    for (uint16_t i = 0; i < len - 1; i += BLE_MESH_NUM_2) { /* 2 Array subscript change value */
        if ((bt_sdk_at_char_to_num_16(str_cpy[i], &num_h) == 1) ||
            (bt_sdk_at_char_to_num_16(str_cpy[i + 1], &num_l) == 1)) {
            return AT_RET_SYNTAX_ERROR;
        }
        data[i / BLE_MESH_NUM_2] = (num_h << BLE_MESH_NUM_4) | num_l;
    }
    return AT_RET_OK;
}

static uint8_t char2uint(char c)
{
    uint8_t val = 0;
    if (isdigit(c)) {
        val = (c - '0');
    } else if (isupper(c)) {
        val = (c - 'A' + 10);  // 10:size
    } else {
        val = (c - 'a' + 10);  // 10:size
    }
    return val;
}

void arg_str_to_key(const char *str, uint8_t *key, int key_len)
{
    int temp = 0;
    for (int i = 0, j = 0, k = 0; i < 32 && k < key_len; i++) {  // 32：size
        temp = char2uint(str[i]);
        if (j == 0) {
            key[k] = temp * 16;  // 16:size
            j = 1;
            continue;
        }
        if (j == 1) {
            key[k] += temp;
            j = 0;
            k++;
        }
    }
}

int cmd_bt_mesh_ctx_info_set(struct bt_mesh_msg_ctx *ctx, const char *argv[])
{
    if ((strcmp(argv[1], "app") != 0) || (strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3,5:index
        (strcmp(argv[7], "ttl") != 0)) {                                                                    // 7:size
        BT_ERR("cmd err, %s, %s, %s, %s", argv[1], argv[3], argv[5], argv[7]);  // 3,5,7:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(ctx->app_idx, argv[2]);   // 2:index
    SHELL_PARAM_UINT(ctx->net_idx, argv[4]);   // 4:index
    SHELL_PARAM_UINT(ctx->addr, argv[6]);      // 6:index
    SHELL_PARAM_UINT(ctx->send_ttl, argv[8]);  // 8:index

    return ERRCODE_BLE_MESH_SUCCESS;
}
