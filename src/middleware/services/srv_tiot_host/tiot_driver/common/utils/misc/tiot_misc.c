/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: TIoT miscellaneous file. \n
 *
 * History: \n
 * 2024-06-20, Create file. \n
 */

#include "tiot_misc.h"

#define TIOT_HEX_RADIX           16
#define TIOT_CONVERT_BASIC_LEN   1
#define TIOT_CONVERT_STR_MIN_LEN 4

// For example, if 3200 is entered, 0xC80 is displayed.
int32_t tiot_num_to_hex(uint32_t num, char* str_buff, uint32_t str_buff_len)
{
    int right = 0;
    int str_num = 0;
    uint32_t number = num;
    int left = 2; // 跳过前两个字符"0x".
    const char map[] = "0123456789ABCDEF";

    if (str_buff == NULL || str_buff_len < TIOT_CONVERT_STR_MIN_LEN) {
        return -1;
    }

    str_buff[str_num++] = '0';
    str_buff[str_num++] = 'x';

    // Conversion part.
    do {
        str_buff[str_num++] = map[number % TIOT_HEX_RADIX];
        number /= TIOT_HEX_RADIX;

        // 确保有空间存放结束符'\0'.
        if ((uint32_t)(str_num + TIOT_CONVERT_BASIC_LEN) > str_buff_len) {
            str_buff[0] = '\0';
            return -1;
        }
    } while (number);

    // Inverting Data
    right = str_num - TIOT_CONVERT_BASIC_LEN;
    while (right > left) {
        char tmp = str_buff[left];
        str_buff[left] = str_buff[right];
        str_buff[right] = tmp;
        left++;
        right--;
    }
    str_buff[str_num++] = '\0';
    return ERRCODE_TIOT_SUCC;
}
