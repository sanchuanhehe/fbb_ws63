/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: validate_chip
 */
#ifdef CONFIG_LOADERBOOT
#define CONFIG_BOOT_SUPPORT_ECC_VERIFY
#include "secure_verify_boot.h"
#include "boot_serial.h"
#include "watchdog.h"
#include "efuse.h"
#include "efuse_porting.h"
#include "validate_chip.h"

#define LOADER_BOOT_START         0xA20000
#define ROOT_PUBLIC_KEY_LEN       0x80
#define CHIP_TYPE_WS63E           0x63e
#define CHIP_TYPE_WS63            0x63
#define EFUSE_DIE_ID_LEN          20
#define EFUSE_TIME_OFFSET         8
#define WHITELIST_YEAR            24
#define WHITELIST_MONTH           1
#define WHITELIST_DAY             12

typedef union {
    uint16_t data;
    struct {
        uint16_t year : 6;
        uint16_t month : 4;
        uint16_t day : 5;
        uint16_t rsv : 1;
    } time;
} efuse_time_t;

uint32_t get_efuse_chip_type(uint32_t *type)
{
    uint8_t efuse_type_id = 0;
    // 1. 读取efuse type_id，判断芯片类型
    uint32_t ret = efuse_read_item(EFUSE_TYPE_ID, &efuse_type_id, 1);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("read efuse type_id err! ret = ", ret);
        return ERRCODE_FAIL;
    }
    // 2. 若bit[1:0]任一为1且bit[3:2]均为0 表示63E（ATE烧写成0x3）
    if ((efuse_type_id & 0x03) != 0 && (efuse_type_id & 0x0C) == 0) {
        *type = CHIP_TYPE_WS63E;
        return ERRCODE_SUCC;
    }

    efuse_time_t efuse_time;
    uint8_t efuse_die_id[EFUSE_DIE_ID_LEN] = { 0 };
    ret = efuse_read_item(EFUSE_DIE_ID, efuse_die_id, sizeof(efuse_die_id));
    if (ret != ERRCODE_SUCC) {
        boot_msg1("read efuse die_id err! ret = ", ret);
        return ERRCODE_FAIL;
    }

    efuse_time.data = makeu16(efuse_die_id[EFUSE_TIME_OFFSET], efuse_die_id[EFUSE_TIME_OFFSET + 1]);
    if (efuse_time.time.year == WHITELIST_YEAR && efuse_time.time.month == WHITELIST_MONTH &&
        efuse_time.time.day == WHITELIST_DAY) {
        *type = CHIP_TYPE_WS63E;
        return ERRCODE_SUCC;
    }
    *type = CHIP_TYPE_WS63;
    return ERRCODE_SUCC;
}

uint32_t check_chip_type(void)
{
    image_key_area_t *key_area = (image_key_area_t *)(uintptr_t)(LOADER_BOOT_START + ROOT_PUBLIC_KEY_LEN);
    uint32_t pkt_chip_type = key_area->key_version_ext; // loadboot中key_version_ext字段复用为chip_type
    uint32_t efuse_chip_type = 0;
    // 未配置不做校验
    if (pkt_chip_type == 0) {
        return ERRCODE_SUCC;
    }
    if (get_efuse_chip_type(&efuse_chip_type) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (efuse_chip_type != pkt_chip_type) {
        boot_msg2("check_chip_type err [efuse:pkt]:", efuse_chip_type, pkt_chip_type);
        uapi_watchdog_disable();
        while (1) {};
    }
    return ERRCODE_SUCC;
}
#endif