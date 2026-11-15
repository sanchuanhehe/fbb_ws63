/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At plt header \n
 */

#ifndef AT_PLT_H
#define AT_PLT_H

#include "at.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef MAC_LEN
#define MAC_LEN 6
#endif

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#define SIZE_2_BITS 2
#define SIZE_5_BITS 5
#define SIZE_8_BITS 8
#define SIZE_10_BITS 10
#define EFUSE_MAC_GROUP_NUM 4
#define EFUSE_GROUP_NUM 3
#define EFUSE_BSLE_POWER_LOCK_NUM 2
#define BIT_TO_BYTE 8
#define WIFI_MAC_1_PG19 314
#define WIFI_MAC_2_PG20 315
#define WIFI_MAC_3_PG21 316
#define WIFI_MAC_4_PG22 317
#define BLSE_POWER_1_PG11 306
#define BLSE_POWER_2_PG12 307
#define EFUSE_MFG_FLAG_1_BIT 1295
#define EFUSE_MFG_FLAG_2_BIT 1439
#define EFUSE_MFG_FLAG_3_BIT 1583
typedef struct {
    td_u16 xo_trim;
    td_u8 xo_temp;
    td_u8 resv;
} efuse_xo_trim_offset_stru;

typedef struct {
    td_u16 dsss_11b[2];
    td_u16 ofdm_20m[2];
    td_u16 ofdm_40m[2];
} efuse_wifi_pwroff_stru;

typedef struct {
    td_u8 mac_addr[MAC_LEN];
    td_u8 resv[2];
} efuse_mac_stru;

typedef struct {
    td_u8 *data;
    td_u8 len;
} efuse_mfg_cali_data_status;

typedef struct {
    efuse_xo_trim_offset_stru xo_trim[EFUSE_GROUP_NUM]; /* 频偏3组efuse */
    efuse_wifi_pwroff_stru wifi_pwr_offset[EFUSE_GROUP_NUM]; /* 功率校准3组efuse */
    td_u16 wifi_rssi_offset[EFUSE_GROUP_NUM];      /* rssi校准3组efuse */
    td_u16 bsle_c_offset[EFUSE_GROUP_NUM]; /* bsle功率3组efuse */
    efuse_mac_stru wifi_mac[EFUSE_MAC_GROUP_NUM];      /* wifi mac 4组efuse */
    efuse_mac_stru sle_mac;
} efuse_mfg_cali_data_stru;

typedef struct {
    uint16_t id_start_bit; /* 起始 bit位 */
    uint8_t id_size;      /* 以bit为单位 */
    uint8_t resv;
} efuse_data_stru;

typedef struct {
    uint8_t xo_trim_cnt;
    uint8_t wifi_mac_cnt;
    uint8_t bsle_power_cnt;
    uint8_t resv;
} efuse_times_left_stru;
#endif /* _PRE_WLAN_FEATURE_MFG_TEST */

void los_at_plt_cmd_register(void);


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_plt.h */
