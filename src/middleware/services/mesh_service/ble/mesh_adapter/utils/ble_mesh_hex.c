/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh hex convert.
 *
 * History:
 * 2025-8-2, Create file.
 */
#include <stdbool.h>
#include <stdint.h>

#include "ble_mesh_hex.h"

static uint8_t ble_mesh_hex2val(char c)
{
    if (c >= '0' && c <= '9') {  // 10:size
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;  // 10:size
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;  // 10:size
    } else {
        return 0;
    }
}

static bool ble_mesh_isxdigit_str(const char *s)
{
    while (isspace(*s)) {
        s++;
    }

    if (*s == '+' || *s == '-') {
        return false;
    }

    if (*s == '\0') {
        return false;
    }

    while (*s != '\0') {
        if (!isxdigit((unsigned char)*s)) {
            return false;
        }
        s++;
    }

    return true;
}

int32_t ble_mesh_hex2bin(const char *hex, uint8_t *bin, uint32_t bin_len)
{
    uint32_t len = 0;
    if (!ble_mesh_isxdigit_str(hex)) {
        return -1;  // -1:error
    }
    
    while (*hex && len < bin_len) {
        bin[len] = ble_mesh_hex2val(*hex++) << 4;  // 4:size

        if (!*hex) {
            len++;
            break;
        }

        bin[len++] |= ble_mesh_hex2val(*hex++);
    }

    return len;
}

static int ble_mesh_hex2char(uint8_t x, char *c)
{
    if (x <= 9) { // 9:digit to ascii
        *c = x + 48;  // 48:ASCII value of '0'
    } else if (x <= 15) { // 15:digit to ascii
        *c = x - 10 + 97; // 10:digit to ascii, 97:ASCII value of 'a'
    } else {
        return -1;
    }

    return 0;
}

uint32_t ble_mesh_bin2hex(const uint8_t *buf, uint32_t buflen, char *hex, uint32_t hexlen)
{
    if (hexlen < ((buflen * 2U) + 1U)) { // 2U:len
        return 0;
    }

    for (uint32_t i = 0; i < buflen; i++) {
        if (ble_mesh_hex2char(buf[i] >> 4, &hex[2U * i]) < 0) { // 4:size
            return 0;
        }
        if (ble_mesh_hex2char(buf[i] & 0xf, &hex[2U * i + 1U]) < 0) {
            return 0;
        }
    }

    hex[2U * buflen] = '\0';
    return 2U * buflen;
}