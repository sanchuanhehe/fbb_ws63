/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: WAL layer external API interface implementation.
 */

#include "oal_plat_type.h"
#include "oam_ext_if.h"
#include "wlan_spec.h"
#include "mac_vap_ext.h"
#include "oal_main.h"
#include "oal_mem_hcm.h"
#include "dmac_ext_if_hcm.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_ioctl.h"
#include "wal_ccpriv.h"
#include "common_dft.h"
#include "frw_hmac_hcc_adapt.h"
#include "dmac_common_inc_rom.h"
#include "soc_customize_wifi.h"
#include "msg_smooth_phase_rom.h"
#include "soc_wifi_driver_api.h"
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
#include "nv.h"
#include "nv_common_cfg.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_CUSTOMIZE_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST


#define DEFAULT_HOST2DEV_SCATT_MAX 64

/*
 * 2 Global Variable Definition
 */
osal_s32 g_al_host_init_params_etc[WLAN_CFG_INIT_BUTT] = {0};      /* ini定制化参数数组 */
osal_s8  g_ac_country_code_etc[COUNTRY_CODE_LEN] = "CN";
wlan_customize_private_stru g_al_priv_cust_params[WLAN_CFG_PRIV_BUTT] = {{0, 0}};  /* 私有定制化参数数组 */
wlan_cust_country_code_ingore_flag_stru g_st_cust_country_code_ignore_flag = {0}; /* 定制化国家码配置 */
/* 标识当前用哪一套大区功率表 */
osal_u16 g_nvram_tag = 0;
/* 默认定制功率 */
const wlan_cust_rf_fe_power_params g_default_cust_tx_power[REGDOMAIN_COUNT] = {
    {
        .chip_max_power_2g = {230},
        .target_power_2g = {
            // 1M 2M 5.5M 11M 11b
            0x2E, 0x2E, 0x2E, 0x2B,
            // 6M 9M 12M 18M 24M 36M 48M 54M 11g
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x26,
            // MCS0~MCS9 11n(MCS7)/11ax
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E,
            // MCS0~MCS9 11n(MCS7)/11ax MCS32 11n
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E, 0x16},
        .limit_power_2g = {
            // ch1-3
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch4-6
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch7-9
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch10-12
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch13-14
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C},
        .sar_power_2g = { 60, 60, 60},  // level 0,1,2
        .special_cta_coef_switch = { 0 },
        .rsv = {0}
    },
    {
        .chip_max_power_2g = {230},
        .target_power_2g = {
            // 1M 2M 5.5M 11M 11b
            0x2E, 0x2E, 0x2E, 0x2B,
            // 6M 9M 12M 18M 24M 36M 48M 54M 11g
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x26,
            // MCS0~MCS9 11n(MCS7)/11ax
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E,
            // MCS0~MCS9 11n(MCS7)/11ax MCS32 11n
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E, 0x16},
        .limit_power_2g = {
            // ch1-3
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch4-6
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch7-9
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch10-12
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch13-14
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C},
        .sar_power_2g = { 60, 60, 60},  // level 0,1,2
        .rsv = {0}
    },
    {
        .chip_max_power_2g = {230},
        .target_power_2g = {
            // 1M 2M 5.5M 11M 11b
            0x2E, 0x2E, 0x2E, 0x2B,
            // 6M 9M 12M 18M 24M 36M 48M 54M 11g
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x26,
            // MCS0~MCS9 11n(MCS7)/11ax
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E,
            // MCS0~MCS9 11n(MCS7)/11ax MCS32 11n
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E, 0x16},
        .limit_power_2g = {
            // ch1-3
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch4-6
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch7-9
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch10-12
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch13-14
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C},
        .sar_power_2g = { 60, 60, 60},  // level 0,1,2
        .rsv = {0}
    },
    {
        .chip_max_power_2g = {230},
        .target_power_2g = {
            // 1M 2M 5.5M 11M 11b
            0x2E, 0x2E, 0x2E, 0x2B,
            // 6M 9M 12M 18M 24M 36M 48M 54M 11g
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x26,
            // MCS0~MCS9 11n(MCS7)/11ax
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E,
            // MCS0~MCS9 11n(MCS7)/11ax MCS32 11n
            0x28, 0x28, 0x28, 0x25, 0x25, 0x25, 0x25, 0x24, 0x21, 0x1E, 0x16},
        .limit_power_2g = {
            // ch1-3
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch4-6
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch7-9
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch10-12
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
            // ch13-14
            0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C},
        .sar_power_2g = { 60, 60, 60},  // level 0,1,2
        .rsv = {0}
    }
};

osal_u16 g_hmac_max_ap_num = 0;
uint32_t g_self_healing_enable = 0; /* 异常自愈开关，默认不打开 */
uint32_t g_self_healing_period = 5000; /* 异常自愈检测周期，默认5000ms */
uint32_t g_self_healing_cnt = 10; /* 异常自愈上，表示上报100次，上报频率减少10倍 */
osal_u8 g_amsdu_max_num = 4; /* 最大amsdu聚合个数 */
osal_u8 g_amsdu_tx_active = 1; /* amsdu_tx_on开关，默认开启 */
osal_u8 g_ampdu_amsdu_tx_active = 1; /* ampdu_amsdu_tx_on开关，默认开启 */
osal_u8 g_ampdu_tx_max_num = 16; /* TX ampdu最大聚合个数，默认值16 */
osal_u8 g_ampdu_rx_max_num = 32; /* RX ampdu最大聚合个数，默认值32（最大接收32聚合度的报文） */
osal_u8 g_ampdu_tx_baw_size = 32; /* TX聚合窗口大小，默认值32（发送报文聚合度需不超过窗口1/2） */
osal_u32 g_wow_event = 0xf; /* wow唤醒源配置 默认全部支持 */
osal_u8 g_wow_enable = 1; /* wow_offload模块开关 默认打开 */
uint8_t g_data_sample = 0;
uint8_t g_apf_enable = 1;
uint8_t g_smooth_phase_en = 0;
uint8_t g_compatibility_er_su_th = 5; /* ERSU兼容性处理，使用ERSU发送失败次数阈值，默认值5 */
uint8_t g_ext_coex_en = 0;
osal_u16 g_cus_low_current_boot_mode = 0x0;   /* 低电流启动控制开关 管理 cali_data_mask */
wlan_cust_rf_fe_params g_cust_rf_fe_params = {0};
/*
 * 定制化结构体
 * default values as follows:
 * ampdu_tx_max_num:            WLAN_AMPDU_TX_MAX_NUM               = 64
 * switch:                      ON                                  = 1
 * scan_orthogonal:             ROAM_SCAN_CHANNEL_ORG_BUTT          = 4
 */
wlan_customize_stru g_wlan_customize_etc = {
    64,             /* addba_buffer_size */
    1,              /* roam switch */
    4,              /* roam scan org */
    -78,            /* roam trigger 2G */
    -128,            /* roam trigger 5G */
    12,             /* roam delta 2G */
    12,             /* roam delta 5G */
    0, 0, 0, 0, 0, 0,
    /* random_mac_addr_scan disable_capab_2ght40 lte_gpio_check_switch ism_priority lte_rx lte_tx */
    0, 0, 0, 0, 0, 0,
    /* lte_inact ism_rx_act bant_pri bant_status want_pri want_status */
    0, 0, 0, 0, 0, 0,
};

/**
 *  regdomain <-> country code map table
 *  max support country num: MAX_COUNTRY_COUNT
 *
 **/
OAL_STATIC const countryinfo_stru g_ast_country_info_table[] = {
    /* Note:too few initializers for unsigned char [3] */
    {REGDOMAIN_COMMON, {'0', '0'}}, // WORLD DOMAIN
    {REGDOMAIN_FCC, {'A', 'D'}}, // ANDORRA
    {REGDOMAIN_ETSI, {'A', 'N'}}, // NETHERLANDS ANTILLES
    {REGDOMAIN_FCC, {'A', 'R'}}, // ARGENTINA
    {REGDOMAIN_FCC, {'A', 'S'}}, // AMERICAN SOMOA
    {REGDOMAIN_FCC, {'B', 'B'}}, // BARBADOS
    {REGDOMAIN_ETSI, {'A', 'T'}}, // AUSTRIA
    {REGDOMAIN_ETSI, {'A', 'W'}}, // ARUBA
    {REGDOMAIN_ETSI, {'A', 'E'}}, // UAE
    {REGDOMAIN_ETSI, {'A', 'L'}}, // ALBANIA
    {REGDOMAIN_ETSI, {'A', 'M'}}, // ARMENIA
    {REGDOMAIN_FCC, {'A', 'U'}}, // AUSTRALIA
    {REGDOMAIN_ETSI,  {'A', 'Z'}}, // AZERBAIJAN
    {REGDOMAIN_ETSI, {'B', 'A'}}, // BOSNIA AND HERZEGOVINA
    {REGDOMAIN_ETSI, {'B', 'D'}}, // BANGLADESH
    {REGDOMAIN_ETSI, { 'B', 'E'}}, // BELGIUM
    {REGDOMAIN_ETSI, {'B', 'G'}}, // BULGARIA
    {REGDOMAIN_ETSI, {'B', 'H'}}, // BAHRAIN
    {REGDOMAIN_ETSI, {'B', 'L'}}, //
    {REGDOMAIN_FCC, {'B', 'M'}}, // BERMUDA
    {REGDOMAIN_ETSI, {'B', 'N'}}, // BRUNEI DARUSSALAM
    {REGDOMAIN_ETSI, {'B', 'O'}}, // BOLIVIA
    {REGDOMAIN_ETSI, {'B', 'R'}}, // BRAZIL
    {REGDOMAIN_FCC, {'B', 'S'}}, // BAHAMAS
    {REGDOMAIN_ETSI, {'B', 'Y'}}, // BELARUS
    {REGDOMAIN_ETSI, {'B', 'Z'}}, // BELIZE
    {REGDOMAIN_FCC, {'C', 'A'}}, // CANADA
    {REGDOMAIN_ETSI, {'C', 'H'}}, // SWITZERLAND
    {REGDOMAIN_ETSI, {'C', 'L'}}, // CHILE
    {REGDOMAIN_COMMON, {'C', 'N'}}, // CHINA
    {REGDOMAIN_FCC, {'C', 'O'}}, // COLOMBIA
    {REGDOMAIN_ETSI, {'C', 'R'}}, // COSTA RICA
    {REGDOMAIN_ETSI, {'C', 'S'}},
    {REGDOMAIN_ETSI, {'C', 'Y'}}, // CYPRUS
    {REGDOMAIN_ETSI, {'C', 'Z'}}, // CZECH REPUBLIC
    {REGDOMAIN_ETSI, {'D', 'E'}}, // GERMANY
    {REGDOMAIN_ETSI, {'D', 'K'}}, // DENMARK
    {REGDOMAIN_FCC, {'D', 'O'}}, // DOMINICAN REPUBLIC
    {REGDOMAIN_ETSI, {'D', 'Z'}}, // ALGERIA
    {REGDOMAIN_ETSI, {'E', 'C'}}, // ECUADOR
    {REGDOMAIN_ETSI, {'E', 'E'}}, // ESTONIA
    {REGDOMAIN_ETSI, {'E', 'G'}}, // EGYPT
    {REGDOMAIN_ETSI, {'E', 'S'}}, // SPAIN
    {REGDOMAIN_ETSI, {'F', 'I'}}, // FINLAND
    {REGDOMAIN_ETSI, {'F', 'R'}}, // FRANCE
    {REGDOMAIN_ETSI, {'G', 'B'}}, // UNITED KINGDOM
    {REGDOMAIN_FCC, {'G', 'D'}},  // GRENADA
    {REGDOMAIN_ETSI, {'G', 'E'}}, // GEORGIA
    {REGDOMAIN_ETSI, {'G', 'F'}}, // FRENCH GUIANA
    {REGDOMAIN_ETSI, {'G', 'L'}}, // GREENLAND
    {REGDOMAIN_ETSI, {'G', 'P'}}, // GUADELOUPE
    {REGDOMAIN_ETSI, {'G', 'R'}}, // GREECE
    {REGDOMAIN_FCC, {'G', 'T'}},  // GUATEMALA
    {REGDOMAIN_FCC, {'G', 'U'}},  // GUAM
    {REGDOMAIN_ETSI, {'H', 'U'}}, // HUNGARY
    {REGDOMAIN_FCC, {'I', 'D'}},  // INDONESIA
    {REGDOMAIN_ETSI, {'I', 'E'}}, // IRELAND
    {REGDOMAIN_ETSI, {'I', 'L'}}, // ISRAEL
    {REGDOMAIN_ETSI, {'I', 'N'}}, // INDIA
    {REGDOMAIN_ETSI, {'I', 'R'}}, // IRAN, ISLAMIC REPUBLIC OF
    {REGDOMAIN_ETSI, {'I', 'S'}}, // ICELNAD
    {REGDOMAIN_ETSI, {'I', 'T'}}, // ITALY
    {REGDOMAIN_FCC, {'J', 'M'}},  // JAMAICA
    {REGDOMAIN_JAPAN, {'J', 'P'}}, // JAPAN
    {REGDOMAIN_ETSI, {'J', 'O'}}, // JORDAN
    {REGDOMAIN_ETSI, {'K', 'E'}}, // KENYA
    {REGDOMAIN_ETSI, {'K', 'H'}}, // CAMBODIA
    {REGDOMAIN_ETSI, {'K', 'P'}}, // KOREA, DEMOCRATIC PEOPLE's REPUBLIC OF
    {REGDOMAIN_ETSI, {'K', 'R'}}, // KOREA, REPUBLIC OF
    {REGDOMAIN_ETSI, {'K', 'W'}}, // KUWAIT
    {REGDOMAIN_ETSI, {'K', 'Z'}}, // KAZAKHSTAN
    {REGDOMAIN_ETSI, {'L', 'B'}}, // LEBANON
    {REGDOMAIN_ETSI, {'L', 'I'}}, // LIECHTENSTEIN
    {REGDOMAIN_ETSI, {'L', 'K'}}, // SRI-LANKA
    {REGDOMAIN_ETSI, {'L', 'T'}}, // LITHUANIA
    {REGDOMAIN_ETSI, {'L', 'U'}}, // LUXEMBOURG
    {REGDOMAIN_ETSI, {'L', 'V'}},  // LATVIA
    {REGDOMAIN_ETSI, {'M', 'A'}}, // MOROCCO
    {REGDOMAIN_ETSI, {'M', 'C'}}, // MONACO
    {REGDOMAIN_ETSI, {'M', 'K'}}, // MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF
    {REGDOMAIN_FCC, {'M', 'N'}}, // MONGOLIA
    {REGDOMAIN_FCC, {'M', 'O'}}, // MACAO
    {REGDOMAIN_FCC, {'M', 'P'}}, // NORTHERN MARIANA ISLANDS
    {REGDOMAIN_ETSI, {'M', 'Q'}}, // MARTINIQUE
    {REGDOMAIN_FCC, {'M', 'T'}}, // MALTA
    {REGDOMAIN_ETSI, {'M', 'U'}}, // MAURITIUS
    {REGDOMAIN_ETSI, {'M', 'W'}}, // MALAWI
    {REGDOMAIN_FCC, {'M', 'X'}}, // MEXICO
    {REGDOMAIN_ETSI, {'M', 'Y'}}, // MALAYSIA
    {REGDOMAIN_ETSI, {'N', 'G'}}, // NIGERIA
    {REGDOMAIN_FCC, {'N', 'I'}}, // NICARAGUA
    {REGDOMAIN_ETSI, {'N', 'L'}}, // NETHERLANDS
    {REGDOMAIN_ETSI, {'N', 'O'}}, // NORWAY
    {REGDOMAIN_ETSI, {'N', 'P'}}, // NEPAL
    {REGDOMAIN_FCC, {'N', 'Z'}}, // NEW-ZEALAND
    {REGDOMAIN_FCC, {'O', 'M'}}, // OMAN
    {REGDOMAIN_FCC, {'P', 'A'}}, // PANAMA
    {REGDOMAIN_ETSI, {'P', 'E'}}, // PERU
    {REGDOMAIN_ETSI, {'P', 'F'}}, // FRENCH POLYNESIA
    {REGDOMAIN_ETSI, {'P', 'G'}}, // PAPUA NEW GUINEA
    {REGDOMAIN_FCC, {'P', 'H'}}, // PHILIPPINES
    {REGDOMAIN_ETSI, {'P', 'K'}}, // PAKISTAN
    {REGDOMAIN_ETSI, {'P', 'L'}}, // POLAND
    {REGDOMAIN_FCC, {'P', 'R'}}, // PUERTO RICO
    {REGDOMAIN_FCC, {'P', 'S'}}, // PALESTINIAN TERRITORY, OCCUPIED
    {REGDOMAIN_ETSI, {'P', 'T'}}, // PORTUGAL
    {REGDOMAIN_FCC, {'P', 'Y'}}, // PARAGUAY
    {REGDOMAIN_ETSI, {'Q', 'A'}}, // QATAR
    {REGDOMAIN_ETSI, {'R', 'E'}}, // REUNION
    {REGDOMAIN_ETSI, {'R', 'O'}}, // ROMAINIA
    {REGDOMAIN_ETSI, {'R', 'S'}}, // SERBIA
    {REGDOMAIN_ETSI, {'R', 'U'}}, // RUSSIA
    {REGDOMAIN_FCC, {'R', 'W'}}, // RWANDA
    {REGDOMAIN_ETSI, {'S', 'A'}}, // SAUDI ARABIA
    {REGDOMAIN_ETSI, {'S', 'E'}}, // SWEDEN
    {REGDOMAIN_ETSI, {'S', 'G'}}, // SINGAPORE
    {REGDOMAIN_ETSI, {'S', 'I'}}, // SLOVENNIA
    {REGDOMAIN_ETSI, {'S', 'K'}}, // SLOVAKIA
    {REGDOMAIN_ETSI, {'S', 'V'}}, // EL SALVADOR
    {REGDOMAIN_ETSI, {'S', 'Y'}}, // SYRIAN ARAB REPUBLIC
    {REGDOMAIN_ETSI, {'T', 'H'}}, // THAILAND
    {REGDOMAIN_ETSI, {'T', 'N'}}, // TUNISIA
    {REGDOMAIN_ETSI, {'T', 'R'}}, // TURKEY
    {REGDOMAIN_ETSI, {'T', 'T'}}, // TRINIDAD AND TOBAGO
    {REGDOMAIN_FCC, {'T', 'W'}}, // TAIWAN, PRIVINCE OF CHINA
    {REGDOMAIN_FCC, {'T', 'Z'}}, // TANZANIA, UNITED REPUBLIC OF
    {REGDOMAIN_ETSI, {'U', 'A'}}, // UKRAINE
    {REGDOMAIN_ETSI, {'U', 'G'}}, // UGANDA
    {REGDOMAIN_FCC, {'U', 'S'}}, // USA
    {REGDOMAIN_ETSI, {'U', 'Y'}}, // URUGUAY
    {REGDOMAIN_FCC, {'U', 'Z'}}, // UZBEKISTAN
    {REGDOMAIN_ETSI, {'V', 'E'}}, // VENEZUELA
    {REGDOMAIN_FCC, {'V', 'I'}}, // VIRGIN ISLANDS, US
    {REGDOMAIN_ETSI, {'V', 'N'}}, // VIETNAM
    {REGDOMAIN_ETSI, {'Y', 'E'}}, // YEMEN
    {REGDOMAIN_ETSI, {'Y', 'T'}}, // MAYOTTE
    {REGDOMAIN_ETSI, {'Z', 'A'}}, // SOUTH AFRICA
    {REGDOMAIN_ETSI, {'Z', 'W'}}, // ZIMBABWE
    {REGDOMAIN_COUNT, {'9', '9'}}
};

/*****************************************************************************
 函 数 名  : host_params_init_first
 功能描述  : 给定制化参数全局数组 g_al_host_init_params_etc 附初值 ini文件读取失败时用初值
*****************************************************************************/
OAL_STATIC osal_void host_params_init_first(osal_void)
{
    /* 性能 */
    g_al_host_init_params_etc[WLAN_CFG_INIT_USED_MEM_FOR_START] = 45; /* used mem for start set to 45 */
    g_al_host_init_params_etc[WLAN_CFG_INIT_USED_MEM_FOR_STOP] = 25;  /* used mem for stop set to 25 */

    g_al_host_init_params_etc[WLAN_CFG_INIT_BUS_D2H_SCHED_COUNT] = 8; /* 8:hcc d2h aggregation num */
    g_al_host_init_params_etc[WLAN_CFG_INIT_BUS_H2D_SCHED_COUNT] = 8; /* 8:hcc h2d aggregation num */

    g_al_host_init_params_etc[WLAN_CFG_INIT_RX_RESTORE_THRES] = 0;

    /* 2G RF前端 */
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND1] = 0xF4F4;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND2] = 0xF4F4;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND3] = 0xF4F4;
    /* 5G RF前端 */
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND1] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND2] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND3] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND4] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND5] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND6] = 0xF8F8;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND7] = 0xF8F8;
    /* 用于定制化计算PWR RF值的偏差 */
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C0_MULT4] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C1_MULT4] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C0_MULT4] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C1_MULT4] = 0;

    g_al_host_init_params_etc[WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_2G] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_2G] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_5G] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_5G] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_JUNCTION_TEMP_DIFF] = 0;
}

/*****************************************************************************
 函 数 名  : hwifi_get_regdomain_from_country_code
 功能描述  : 根据国家码找到对应的regdomain
*****************************************************************************/
regdomain_enum hwifi_get_regdomain_from_country_code(const unsigned char *country_code)
{
    regdomain_enum regdomain = REGDOMAIN_COMMON;
    osal_s32 table_idx = 0;

    while (g_ast_country_info_table[table_idx].regdomain != REGDOMAIN_COUNT) {
        if (osal_memcmp(country_code, g_ast_country_info_table[table_idx].country_code, COUNTRY_CODE_LEN) == 0) {
            regdomain = g_ast_country_info_table[table_idx].regdomain;
            break;
        }
        ++table_idx;
    }

    return regdomain;
}

/*****************************************************************************
 函 数 名  : hwifi_is_regdomain_changed_etc
 功能描述  : 国家码改变后，对应的regdomain是否有变化
*****************************************************************************/
osal_s32 hwifi_is_regdomain_changed_etc(const osal_u8 *old_country_code, const osal_u8 *new_country_code)
{
    return hwifi_get_regdomain_from_country_code(old_country_code) !=
        hwifi_get_regdomain_from_country_code(new_country_code);
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_ini_device_perf_param
 功能描述  : 性能device定制化参数初始化
*****************************************************************************/
OAL_STATIC osal_void hwifi_custom_adapt_device_ini_perf_param(osal_u8 *data, osal_u32 *data_len)
{
    hmac_to_dmac_cfg_custom_data_stru syn_msg = {0};
    bus_cus_config_stru cus_config = {0};

    if (data == NULL) {
        oam_error_log1(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_perf_param data is NULL last data_len[%d].}",
            *data_len);
        return;
    }

    syn_msg.syn_id = CUSTOM_CFGID_INI_PERF_ID;

    cus_config.bus_d2h_sched_count = (osal_u8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_BUS_D2H_SCHED_COUNT);
    cus_config.bus_h2d_sched_count = (osal_u8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_BUS_H2D_SCHED_COUNT);

    if (cus_config.bus_h2d_sched_count < 1 || cus_config.bus_h2d_sched_count > HISDIO_HOST2DEV_SCATT_MAX) {
        oam_error_log2(0, OAM_SF_ANY,
            "{hwifi_custom_adapt_device_ini_perf_param::bus_h2d_sched_count[%d] out of range(0,%d]}",
            cus_config.bus_h2d_sched_count, HISDIO_HOST2DEV_SCATT_MAX);
    } else {
        hcc_hmac_config_bus_ini((osal_u8 *)&cus_config);
    }

    if (cus_config.bus_d2h_sched_count < 1 || cus_config.bus_d2h_sched_count > HISDIO_DEV2HOST_SCATT_MAX) {
        oam_error_log2(0, OAM_SF_ANY,
            "{hwifi_custom_adapt_device_ini_perf_param::sdio_assem_d2h[%d] out of range(0,%d]}",
            cus_config.bus_d2h_sched_count, HISDIO_DEV2HOST_SCATT_MAX);
    }

    syn_msg.len = OAL_SIZEOF(cus_config);

    if (memcpy_s(data, CUSTOM_MSG_DATA_HDR_LEN, &syn_msg, CUSTOM_MSG_DATA_HDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_perf_param::memcpy_s syn_msg error}");
    }
    if (memcpy_s(data + CUSTOM_MSG_DATA_HDR_LEN, OAL_SIZEOF(cus_config), &cus_config, OAL_SIZEOF(cus_config)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_perf_param::memcpy_s device_perf error}");
    }

    *data_len += (OAL_SIZEOF(cus_config) + CUSTOM_MSG_DATA_HDR_LEN);

    oam_warning_log1(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_perf_param::da_len[%d].}", *data_len);
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_device_ini_end_param
 功能描述  : 配置定制化参数结束标志
*****************************************************************************/
OAL_STATIC osal_void hwifi_custom_adapt_device_ini_end_param(osal_u8 *data, osal_u32 *pul_data_len)
{
    hmac_to_dmac_cfg_custom_data_stru syn_msg;

    if (data == NULL) {
        oam_error_log1(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_end_param data::NULL data_len[%d].}",
            *pul_data_len);
        return;
    }

    syn_msg.syn_id = CUSTOM_CFGID_INI_ENDING_ID;
    syn_msg.len = 0;

    if (memcpy_s(data, OAL_SIZEOF(syn_msg), &syn_msg, OAL_SIZEOF(syn_msg)) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_end_param::memcpy_s error}");
    }

    *pul_data_len += OAL_SIZEOF(syn_msg);

    oam_warning_log1(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_end_param::da_len[%d].}", *pul_data_len);
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_priv_ini_param
 功能描述  : 下发私有开机device配置定制化项到device
*****************************************************************************/
OAL_STATIC osal_s32 hwifi_custom_adapt_priv_ini_param(wlan_cfg_priv_id_uint8 cfg_id, osal_u8 *data,
    osal_u32 *pul_len)
{
    osal_s32 l_ret;
    hmac_to_dmac_cfg_custom_data_stru syn_msg;
    osal_s32 l_priv_val = 0;
    osal_u8 priv_cfg_value;
    static osal_u8 wlan_open_cnt = 0;

    if (data == NULL) {
        oam_error_log1(0, OAM_SF_CFG, "{hwifi_custom_adapt_mac_device_priv_ini_param::data is NULL data_len[%d].}",
            *pul_len);
        return OAL_FAIL;
    }

    l_ret = hwifi_get_init_priv_value(cfg_id, &l_priv_val);
    if (l_ret != OAL_SUCC) {
        return OAL_FAIL;
    }

    priv_cfg_value = (osal_u8)l_priv_val;

    switch (cfg_id) {
        case WLAN_CFG_PRIV_BW_MAX_WITH:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_BW_MAX_WITH_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::max_bw[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_SU_BFER:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_SU_BFER_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::su bfer[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_SU_BFEE:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_SU_BFEE_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::su bfee[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_MU_BFER:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_MU_BFER_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::mu bfer[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_MU_BFEE:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_MU_BFEE_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::mu bfee[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_LDPC:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_LDPC_ID;
            wifi_printf("hwifi_custom_adapt_mac_device_priv_ini_param::ldpc[%d].\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_CALI_DATA_MASK:
            /* 开机默认打开校准数据上传下发 */
            wlan_open_cnt++;
            priv_cfg_value = (osal_u8)hwifi_get_low_current_boot_mode_cali_data_mask((osal_u8)priv_cfg_value,
                uapi_get_custom_cali_done_etc());
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_CALI_DATA_MASK_ID;
            wifi_printf(
                "hwifi_custom_adapt_mac_device_priv_ini_param::wlan_open_cnt[%d]priv_cali_data_up_down[0x%x].\r\n",
                wlan_open_cnt, priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_CALI_AUTOCALI_MASK:
            /* 开机默认不打开开机校准 */
            priv_cfg_value = (uapi_get_custom_cali_done_etc() == OAL_FALSE) ? OAL_FALSE : priv_cfg_value;
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_AUTOCALI_MASK_ID;
            wifi_printf(
                "hwifi_custom_adapt_mac_device_priv_ini_param::g_uc_custom_cali_done_etc[%d]auto_cali_mask[0x%x].\r\n",
                uapi_get_custom_cali_done_etc(), priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_EXT_COEX_EN:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_EXT_COEX_EN_ID;
            wifi_printf("hwifi_custom_adapt_priv_ini_param::ext coex en[%d]\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_FRONT_SWITCH:
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_FRONT_SWITCH_ID;
            wifi_printf("hwifi_custom_adapt_priv_ini_param::front switch[%d]\r\n", priv_cfg_value);
            break;
        case WLAN_CFG_PRIV_CUSTOM_DATA_END:
            /* 开机定制化参数结束 */
            syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_CUSTOM_DATA_END;
            wifi_printf("hwifi_custom_adapt_priv_ini_param::custom data END\r\n");
            break;
        default:
            return OAL_FAIL;
    }

    syn_msg.len = OAL_SIZEOF(priv_cfg_value);
    if (memcpy_s(data, CUSTOM_MSG_DATA_HDR_LEN, &syn_msg, CUSTOM_MSG_DATA_HDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_custom_adapt_priv_ini_param::memcpy_s syn_msg error}");
    }
    if (memcpy_s(data + CUSTOM_MSG_DATA_HDR_LEN, OAL_SIZEOF(priv_cfg_value),
        &priv_cfg_value, OAL_SIZEOF(priv_cfg_value)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_custom_adapt_priv_ini_param::memcpy_s cfg error}");
    }

    *pul_len += (OAL_SIZEOF(priv_cfg_value) + CUSTOM_MSG_DATA_HDR_LEN);
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_device_priv_ini_cali_mask_param
 功能描述  : 下发私有开机校准配置定制化项到device
*****************************************************************************/
OAL_STATIC osal_s32 hwifi_custom_adapt_device_priv_ini_cali_mask_param(osal_u8 *data, osal_u32 *pul_data_len)
{
    osal_s32 l_ret;
    hmac_to_dmac_cfg_custom_data_stru syn_msg;
    osal_s32 l_priv_val = 0;
    osal_u16 cali_mask;

    if (data == NULL) {
        oam_error_log1(0, OAM_SF_CFG,
            "{hwifi_custom_adapt_device_priv_ini_cali_mask_param::data is NULL data_len[%d].}", *pul_data_len);
        return OAL_FAIL;
    }

    l_ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_CALI_MASK, &l_priv_val);
    if (l_ret == OAL_SUCC) {
        cali_mask = (osal_u16)(osal_u32)l_priv_val;
        wifi_printf("hwifi_custom_adapt_device_priv_ini_cali_mask_param::read cali_mask[%d]l_ret[%d]\r\n",
            cali_mask, l_ret);
    } else {
        return OAL_FAIL;
    }

    syn_msg.syn_id = CUSTOM_CFGID_PRIV_INI_CALI_MASK_ID;
    syn_msg.len = OAL_SIZEOF(cali_mask);

    if (memcpy_s(data, CUSTOM_MSG_DATA_HDR_LEN, &syn_msg, CUSTOM_MSG_DATA_HDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_custom_adapt_device_priv_ini_cali_mask_param::memcpy_s msg error}");
    }
    if (memcpy_s(data + CUSTOM_MSG_DATA_HDR_LEN, OAL_SIZEOF(cali_mask), &cali_mask, OAL_SIZEOF(cali_mask)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_custom_adapt_device_priv_ini_cali_mask_param::memcpy_s cali error}");
    }

    *pul_data_len += (OAL_SIZEOF(cali_mask) + CUSTOM_MSG_DATA_HDR_LEN);
    oam_warning_log2(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_priv_ini_cali_mask_param::da_len[%d] cali_mask[0x%x].}",
        *pul_data_len, cali_mask);

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_device_ini_param
 功能描述  : ini device侧上电前定制化参数适配
*****************************************************************************/
OAL_STATIC osal_u32 hwifi_custom_adapt_device_ini_param(osal_u8 *data)
{
    osal_u32 data_length = 0;

    if (data == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_ini_param::data is NULL.}");
        return INI_FAILED;
    }

    /* 发送消息的格式如下:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | CFGID0    |DATA0 Length| DATA0 Value | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 4 Bytes   |4 Byte      | DATA  Length| ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* 性能 */
    hwifi_custom_adapt_device_ini_perf_param(data + data_length, &data_length);

    /* 结束 */
    hwifi_custom_adapt_device_ini_end_param(data + data_length, &data_length);

    return data_length;
}

/*****************************************************************************
 函 数 名  : hwifi_custom_adapt_device_priv_ini_param
 功能描述  : ini device侧上电前定制化参数适配
*****************************************************************************/
OAL_STATIC osal_u32 hwifi_custom_adapt_device_priv_ini_param(osal_u8 *data)
{
    osal_u32 data_length = 0;

    if (data == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hwifi_custom_adapt_device_priv_ini_param::data is NULL.}");
        return INI_FAILED;
    }

    /* 发送消息的格式如下:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | CFGID0    |DATA0 Length| DATA0 Value | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 4 Bytes   |4 Byte      | DATA  Length| ......................... | */
    /* +-------------------------------------------------------------------+ */

    /* 私有定制化 */
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_LDPC, data + data_length, &data_length);
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_FRONT_SWITCH, data + data_length, &data_length);
    hwifi_custom_adapt_device_priv_ini_cali_mask_param(data + data_length, &data_length);
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_CALI_DATA_MASK, data + data_length, &data_length);
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_CALI_AUTOCALI_MASK, data + data_length, &data_length);
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_EXT_COEX_EN, data + data_length, &data_length);
    hwifi_custom_adapt_priv_ini_param(WLAN_CFG_PRIV_CUSTOM_DATA_END, data + data_length, &data_length);

    wifi_printf("hwifi_custom_adapt_device_priv_ini_param::data_length[%d]\r\n", data_length);

    return data_length;
}

/*****************************************************************************
 函 数 名  : hwifi_hcc_custom_ini_data_buf
 功能描述  : 下发定制化配置命令
*****************************************************************************/
OAL_STATIC osal_s32 hwifi_hcc_custom_ini_data_buf(osal_u16 syn_id)
{
    osal_u32 data_len = 0;
    osal_s32 ret;
    osal_u8 *tx_buf;
    osal_u8 *data_buffer;
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    hcc_transfer_param hcc_transfer_param = { 0 };
#endif

    tx_buf = frw_alloc_hcc_cfg_data(WLAN_LARGE_NETBUF_SIZE);
    if (tx_buf == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "hwifi_hcc_custom_ini_data_buf::alloc tx_buf fail.");
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }
    memset_s(tx_buf, WLAN_LARGE_NETBUF_SIZE, 0, WLAN_LARGE_NETBUF_SIZE);
    data_buffer = tx_buf + hcc_get_head_len();

    if (syn_id == CUSTOM_CFGID_INI_ID) {
        /* INI hmac to dmac 配置项 */
        data_len = hwifi_custom_adapt_device_ini_param(data_buffer);
    } else if (syn_id == CUSTOM_CFGID_PRIV_INI_ID) {
        /* 私有定制化配置项 */
        data_len = hwifi_custom_adapt_device_priv_ini_param(data_buffer);
    } else {
        oam_error_log1(0, OAM_SF_CFG, "hwifi_hcc_custom_ini_data_buf::unknown syn_id[%d]", syn_id);
    }

    if ((data_len > (osal_u32)(WLAN_LARGE_NETBUF_SIZE - hcc_get_head_len())) || (data_len == 0)) {
        oam_error_log1(0, OAM_SF_CFG, "hwifi_hcc_custom_ini_data_buf::got wrong data_len[%d]", data_len);
        frw_free_hcc_cfg_data(tx_buf);
        return OAL_FAIL;
    }

    wifi_printf("***hwifi_hcc_custom_ini_data_buf:%d ***********\r\n", data_len);

#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    hcc_transfer_param.service_type = HCC_ACTION_TYPE_CUSTOMIZE;
    hcc_transfer_param.sub_type = syn_id;
    hcc_transfer_param.queue_id = CTRL_QUEUE;
    hcc_transfer_param.fc_flag = 0;
    hcc_transfer_param.user_param = NULL;

    ret = (osal_u32)hcc_tx_data(HCC_CHANNEL_AP, tx_buf, WLAN_LARGE_NETBUF_SIZE, &hcc_transfer_param);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log2(0, OAM_SF_CFG, "hwifi_hcc_custom_ini_data_buf fail ret[%d] buf[%p]", ret, data_buffer);
        frw_free_hcc_cfg_data(tx_buf);
    }
#else
    ret = (osal_s32)frw_rx_custom_post_data_function(0, (osal_u8)syn_id, tx_buf, data_len + hcc_get_head_len(), NULL);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log0(0, OAM_SF_CFG, "frw_rx_custom_post_data_function fail");
        /* frw_rx_custom_post_data_function返回成功和失败都会释放内存 */
    }
#endif
    return ret;
}

/* ****************************************************************************
 函 数 名  : wal_customize_init_cali
 功能描述  : 定制化参数::ini::校准
**************************************************************************** */
static td_u32 wal_customize_init_cali(oal_net_device_stru *netdev)
{
    td_u32 ret;
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_SET_CUS_DTS_CALI,
        OAL_PTR_NULL, 0);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        wifi_printf("{wal_customize_init_cali::wal_sync_send2device_no_rsp failed, error no[%u]!}\r\n", ret);
        return ret;
    }
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 国家码定制化
*****************************************************************************/
static void hwifi_config_init_ini_country(oal_net_device_stru *cfg_net_dev)
{
    osal_s32 ret;
    osal_char *country = (osal_char *)hwifi_get_country_code_etc();
    /* 如果NV有预制国家码值,则覆盖代码中初值 */
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u8 nv_country[COUNTRY_CODE_LEN - 1];
    osal_u16 nv_country_length = 0;
    osal_u32 nv_ret;
    (osal_void)memset_s(nv_country, sizeof(nv_country), 0, sizeof(nv_country));

    nv_ret = uapi_nv_read(NV_ID_COUNTRY_CODE, COUNTRY_CODE_LEN, &nv_country_length, nv_country);
    if (nv_ret == OAL_SUCC) {
        if (memcpy_s(country, COUNTRY_CODE_LEN - 1, nv_country, sizeof(nv_country)) != EOK) {
            oam_warning_log0(0, OAM_SF_ANY, "hwifi_config_init_ini_country::nv data memcpy fail");
        }
        wifi_printf("[%s][%d]nv read country[%s]\r\n", __func__, __LINE__, country);
    } else {
        oam_warning_log1(0, OAM_SF_ANY, "hwifi_config_init_ini_country::nv read country fail ret[%d]", nv_ret);
    }
#endif
    ret = (osal_s32)uapi_ccpriv_setcountry(cfg_net_dev, country);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY,
            "hwifi_config_init_ini_country::wal_send_cfg_event_etc return err code [%d]!", ret);
    }

    /* 开关wifi定制化配置国家码 */
    g_st_cust_country_code_ignore_flag.country_code_ingore_ccpriv_flag = OAL_FALSE;
}

/*****************************************************************************
 功能描述  : 日志相关定制化
*****************************************************************************/
static void hwifi_config_init_ini_log(oal_net_device_stru *cfg_net_dev)
{
    osal_s32 ret;
    td_s32 l_loglevel;

    /* log_level */
    l_loglevel = OAM_LOG_LEVEL_WARNING;
    if ((l_loglevel < OAM_LOG_LEVEL_ERROR) || (l_loglevel > OAM_LOG_LEVEL_INFO)) {
        oam_error_log3(0, OAM_SF_ANY,
            "{hwifi_config_init_ini_clock::loglevel[%d] out of range[%d,%d], check value in ini file!}\r\n", l_loglevel,
            OAM_LOG_LEVEL_ERROR, OAM_LOG_LEVEL_INFO);
        return;
    }
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_CFG_SET_ALL_LOG_LEVEL,
        (osal_u8 *)&l_loglevel, OAL_SIZEOF(osal_s32));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY, "{hwifi_config_init_ini_log::return err code[%d]!}\r\n", ret);
    }
}

#if defined(_PRE_PRODUCT_ID_HOST)
/*****************************************************************************
 函 数 名  : hwifi_check_pwr_ref_delta
 功能描述  : 检查定制化中的delta_rssi值是否超过阈值
*****************************************************************************/
OAL_STATIC td_s8 hwifi_check_pwr_ref_delta(td_s8 c_pwr_ref_delta)
{
    td_s8 c_ret = 0;
    if (c_pwr_ref_delta > WAL_CCPRIV_PWR_REF_DELTA_HI) {
        c_ret = WAL_CCPRIV_PWR_REF_DELTA_HI;
    } else if (c_pwr_ref_delta < WAL_CCPRIV_PWR_REF_DELTA_LO) {
        c_ret = WAL_CCPRIV_PWR_REF_DELTA_LO;
    } else {
        c_ret = c_pwr_ref_delta;
    }

    return c_ret;
}

/*****************************************************************************
 函 数 名  : hwifi_cfg_front_end
 功能描述  : hw 2g 5g 前端
 修改内容  : 适配双RF定制化内容
*****************************************************************************/
OAL_STATIC osal_void hwifi_cfg_pwr_ref_delta(mac_cfg_customize_rf *customize_rf)
{
    td_u8 rf_idx;
    wlan_cfg_init cfg_id;
    td_s32 l_pwr_ref_delta;

    /* 2G 20M/40M */
    for (rf_idx = 0; rf_idx < WLAN_RF_CHANNEL_NUMS; rf_idx++) {
        cfg_id = (rf_idx == WLAN_RF_CHANNEL_ZERO) ?
            WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C0_MULT4 : WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C1_MULT4;
        l_pwr_ref_delta = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_2g[0] =
            hwifi_check_pwr_ref_delta((osal_u32)l_pwr_ref_delta & 0xff);
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_2g[1] =
            hwifi_check_pwr_ref_delta(((osal_u32)l_pwr_ref_delta >> 8) & 0xff);  /* 偏移8位 */
    }
#ifdef _PRE_WLAN_SUPPORT_5G
    /* 5G 20M/40M/80M/160M */
    for (rf_idx = 0; rf_idx < WLAN_RF_CHANNEL_NUMS; rf_idx++) {
        cfg_id = (rf_idx == WLAN_RF_CHANNEL_ZERO) ?
            WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C0_MULT4 : WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C1_MULT4;
        l_pwr_ref_delta = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_5g[0] =
            hwifi_check_pwr_ref_delta((osal_u32)l_pwr_ref_delta & 0xff);
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_5g[1] =
            hwifi_check_pwr_ref_delta(((osal_u32)l_pwr_ref_delta >> 8) & 0xff);  /* 偏移8位 */
        /* 2表示80M */
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_5g[2] =
            hwifi_check_pwr_ref_delta(((osal_u32)l_pwr_ref_delta >> 16) & 0xff);  /* 偏移16位 */
        /* 3表示160M */
        customize_rf->delta_pwr_ref_cfg[rf_idx].cfg_delta_pwr_ref_rssi_5g[3] =
            hwifi_check_pwr_ref_delta(((osal_u32)l_pwr_ref_delta >> 24) & 0xff);  /* 偏移24位 */
    }
#endif
}

static osal_u32 hwifi_cfg_set_2g_rf(mac_cfg_customize_rf *customize_rf)
{
    osal_u8 idx;
    osal_u8 rf_idx;
    osal_s32 mult4;
    osal_s8 mult4_rf;
    osal_u8 offset = 8;

    for (rf_idx = 0; rf_idx < WLAN_RF_CHANNEL_NUMS; ++rf_idx) {
        for (idx = 0; idx < MAC_NUM_2G_BAND; ++idx) {
            /* 获取各2p4g 各band 0.25db及0.1db精度的线损值 */
            mult4 = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND_START + idx);
            mult4_rf = (osal_s8)(((osal_u32)mult4 >> (osal_u32)(rf_idx * offset)) & 0xFF);

            if (mult4_rf >= RF_LINE_TXRX_GAIN_DB_2G_MIN && mult4_rf <= RF_LINE_TXRX_GAIN_DB_MAX) {
                customize_rf->rf_gain_db_rf[rf_idx].ac_gain_db_2g[idx].rf_gain_db_mult4 = mult4_rf;
            } else {
                /* 值超出有效范围 */
                oam_error_log2(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 2g mult4[0x%0x}!}",
                    WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND_START + idx, mult4);
                return OAL_FAIL;
            }
        }
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_SUPPORT_5G
static osal_u32 hwifi_cfg_set_5g_rf(mac_cfg_customize_rf *customize_rf)
{
    osal_u8 idx;
    osal_u8 rf_idx;
    osal_s32 mult4;
    osal_s8 mult4_rf;
    osal_u8 offset = 8;

    for (rf_idx = 0; rf_idx < WLAN_RF_CHANNEL_NUMS; ++rf_idx) {
        for (idx = 0; idx < MAC_NUM_5G_BAND; ++idx) {
            /* 获取各5g 各band 0.25db及0.1db精度的线损值 */
            mult4 = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND_START + idx);
            mult4_rf = (osal_s8)(((osal_u32)mult4 >> (osal_u32)(rf_idx * offset)) & 0xFF);

            if (mult4_rf <= RF_LINE_TXRX_GAIN_DB_MAX) {
                customize_rf->rf_gain_db_rf[rf_idx].ac_gain_db_5g[idx].rf_gain_db_mult4 = mult4_rf;
            } else {
                /* 值超出有效范围 */
                oam_error_log2(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 5g mult4[0x%0x}!}",
                    WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND_START + idx, mult4);
                return OAL_FAIL;
            }
        }
    }

    return OAL_SUCC;
}
#endif

/* 判断CCA能量门限调整值是否超出范围, 最大调整幅度:DELTA_CCA_ED_HIGH_TH_RANGE */
static inline osal_u8 cus_delta_cca_ed_high_th_out_of_range(osal_s8 value)
{
    osal_s8 val = value;

    val = val > 0 ? val : -(val);
    return val > DELTA_CCA_ED_HIGH_TH_RANGE ? OAL_TRUE : OAL_FALSE;
}

static osal_void hwifi_cfg_cca_thresh(mac_cfg_customize_rf *customize_rf)
{
    osal_s8 delta_cca_ed_high_20th_2g =
        (osal_s8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_2G);
    osal_s8 delta_cca_ed_high_40th_2g =
        (osal_s8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_2G);
    osal_s8 delta_cca_ed_high_20th_5g =
        (osal_s8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_5G);
    osal_s8 delta_cca_ed_high_40th_5g =
        (osal_s8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_5G);
    /* 检查每一项的调整幅度是否超出最大限制 */
    if (cus_delta_cca_ed_high_th_out_of_range(delta_cca_ed_high_20th_2g) == OAL_TRUE ||
        cus_delta_cca_ed_high_th_out_of_range(delta_cca_ed_high_40th_2g) == OAL_TRUE ||
        cus_delta_cca_ed_high_th_out_of_range(delta_cca_ed_high_20th_5g) == OAL_TRUE ||
        cus_delta_cca_ed_high_th_out_of_range(delta_cca_ed_high_40th_5g) == OAL_TRUE) {
        oam_error_log4(0, OAM_SF_ANY, "{hwifi_cfg_front_end::one or more delta cca ed high threshold out of range \
            [delta_20th_2g=%d, delta_40th_2g=%d, delta_20th_5g=%d, delta_40th_5g=%d], please check the value!}",
            delta_cca_ed_high_20th_2g, delta_cca_ed_high_40th_2g, delta_cca_ed_high_20th_5g, delta_cca_ed_high_40th_5g);
        /* set 0 */
        customize_rf->delta_cca_ed_high_20th_5g = 0;
        customize_rf->delta_cca_ed_high_40th_5g = 0;
        customize_rf->delta_cca_ed_high_20th_2g = 0;
        customize_rf->delta_cca_ed_high_40th_2g = 0;
    } else {
        customize_rf->delta_cca_ed_high_20th_2g = delta_cca_ed_high_20th_2g;
        customize_rf->delta_cca_ed_high_40th_2g = delta_cca_ed_high_40th_2g;
        customize_rf->delta_cca_ed_high_20th_5g = delta_cca_ed_high_20th_5g;
        customize_rf->delta_cca_ed_high_40th_5g = delta_cca_ed_high_40th_5g;
    }
}

static osal_void hwifi_cfg_junction_temp(mac_cfg_customize_rf *customize_rf)
{
    const osal_s16 temp_diff_thr = 20;
    osal_s8 temp_diff = (osal_s8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_JUNCTION_TEMP_DIFF);
    /* 检查每一项的调整幅度是否超出最大限制 */
    if (temp_diff > temp_diff_thr || temp_diff < -temp_diff_thr) {
        oam_error_log2(0, OAM_SF_ANY, "hwifi_cfg_junction_temp::temp diff:%d/%d out of range!",
            temp_diff, temp_diff_thr);
        /* set 0 */
        temp_diff = 0;
    }
    customize_rf->junction_temp_diff = temp_diff;
}
static td_u32 hwifi_cfg_front_end(td_u8 *param)
{
    mac_cfg_customize_rf *customize;

    customize = (mac_cfg_customize_rf *)param;
    /* 配置: 2g rf */
    if (hwifi_cfg_set_2g_rf(customize) != OAL_SUCC) {
        return OAL_FAIL;
    }

    hwifi_cfg_pwr_ref_delta(customize);

    /* 通道radio cap */
    customize->chn_radio_cap = 0XF;
#ifdef _PRE_WLAN_SUPPORT_5G
    if (mac_device_check_5g_enable_per_chip()) {
        /* 配置: 5g rf */
        /* 配置: fem口到天线口的负增益 */
        if (hwifi_cfg_set_5g_rf(customize) != OAL_SUCC) {
            return OAL_FAIL;
        }
    }
#endif
    customize->far_dist_pow_gain_switch = OAL_TRUE;
    customize->far_dist_dsss_scale_promote_switch = OAL_TRUE;

    /* 配置: cca能量门限调整值 */
    hwifi_cfg_cca_thresh(customize);
    /* 配置 结温偏差补偿值 */
    hwifi_cfg_junction_temp(customize);
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : hwifi_config_init_ini_rf
 功能描述  : hw 2g 5g 前端定制化
*****************************************************************************/
static void hwifi_config_init_ini_rf(oal_net_device_stru *cfg_net_dev)
{
    td_u32 ul_ret;
    mac_cfg_customize_rf customize_rf = { 0 };

    ul_ret = hwifi_cfg_front_end((td_u8 *)&customize_rf);
    if (ul_ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_config_init_ini_rf::front end rf wrong value, not send cfg!}\r\n");
        return;
    }
    ul_ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_C_CFG_SET_CUS_RF,
        (osal_u8 *)&customize_rf, sizeof(customize_rf));
    if (OAL_UNLIKELY(ul_ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY,
            "{hwifi_config_init_ini_rf::EVENT[wal_send_cfg_event_etc] failed, return err code [%d]!}\r\n", ul_ret);
    }
}
wlan_cust_rf_fe_params *hwifi_get_rf_fe_custom_ini(osal_void)
{
    return &g_cust_rf_fe_params;
}
OAL_STATIC osal_void hwifi_config_rf_fe_custom_ini_tx_power(oal_net_device_stru *cfg_net_dev)
{
    wlan_cust_rf_fe_params *rf_fe_ini = hwifi_get_rf_fe_custom_ini();
    osal_s32 ret;

    if ((rf_fe_ini->ini_ret & (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER)) != 0) {
        ret = wal_async_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_CFG_SET_RF_FE_POWER,
            (osal_u8 *)&rf_fe_ini->rf_power, sizeof(rf_fe_ini->rf_power), FRW_POST_PRI_LOW);
        if (OAL_UNLIKELY(ret != OAL_SUCC)) {
            oam_error_log2(0, OAM_SF_ANY, "{hwifi_config_rf_fe_custom_ini_tx_power::send msg:%d failed, return[%d]!}",
                WLAN_MSG_W2H_CFG_SET_RF_FE_POWER, ret);
        }
        rf_fe_ini->ini_ret &= (~(1 << WLAN_CFG_INI_RF_FE_TYPE_POWER));
    }
}
OAL_STATIC osal_void hwifi_config_rf_fe_custom_ini(oal_net_device_stru *cfg_net_dev)
{
    wlan_cust_rf_fe_params *rf_fe_ini = hwifi_get_rf_fe_custom_ini();
    wlan_cust_rf_fe_rssi_params rssi = { 0 };
    osal_s32 ret;

    if ((rf_fe_ini->ini_ret & (1 << WLAN_CFG_INI_RF_FE_TYPE_PARAMS)) != 0) {
        ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_CFG_SET_RF_FE_RX_INSERT_LOSS,
            (osal_u8 *)&rf_fe_ini->rf_rx_loss, sizeof(rf_fe_ini->rf_rx_loss));
        if (OAL_UNLIKELY(ret != OAL_SUCC)) {
            oam_error_log2(0, OAM_SF_ANY, "{hwifi_config_rf_fe_custom_ini::send msg:%d failed, return[%d]!}",
                WLAN_MSG_W2H_CFG_SET_RF_FE_RX_INSERT_LOSS, ret);
        }
        rf_fe_ini->ini_ret &= (~(1 << WLAN_CFG_INI_RF_FE_TYPE_PARAMS));
    }

    if ((rf_fe_ini->ini_ret & (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER_CALI)) != 0) {
        ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_CFG_SET_RF_FE_POWER_CALI,
            (osal_u8 *)&rf_fe_ini->rf_power_cali, sizeof(rf_fe_ini->rf_power_cali));
        if (OAL_UNLIKELY(ret != OAL_SUCC)) {
            oam_error_log2(0, OAM_SF_ANY, "{hwifi_config_rf_fe_custom_ini::send msg:%d failed, return[%d]!}",
                WLAN_MSG_W2H_CFG_SET_RF_FE_POWER_CALI, ret);
        }
        rf_fe_ini->ini_ret &= (~(1 << WLAN_CFG_INI_RF_FE_TYPE_POWER_CALI));
    }

    if ((rf_fe_ini->ini_ret & (1 << WLAN_CFG_INI_RF_FE_TYPE_RSSI)) != 0) {
        memcpy_s(&rssi, sizeof(rssi), &rf_fe_ini->rf_rssi, sizeof(wlan_cust_rf_fe_rssi_params));
        rssi.rssi_sel_bits = 0x7; /* bit0~2代表subband0~2写入 */
        ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2H_CFG_SET_RF_FE_RSSI,
            (osal_u8 *)&rssi, sizeof(rssi));
        if (OAL_UNLIKELY(ret != OAL_SUCC)) {
            oam_error_log2(0, OAM_SF_ANY, "{hwifi_config_rf_fe_custom_ini::send msg:%d failed, return[%d]!}",
                WLAN_MSG_W2H_CFG_SET_RF_FE_RSSI, ret);
        }
        rf_fe_ini->ini_ret &= (~(1 << WLAN_CFG_INI_RF_FE_TYPE_RSSI));
    }
    // 功率相关参数解析 下发
    hwifi_config_rf_fe_custom_ini_tx_power(cfg_net_dev);
}
/*****************************************************************************
 函 数 名  : hwifi_config_init_ini_main
 功能描述  : 配置vap定制化，不涉及wlan p2p网络设备的，均由配置vap配置host参数或下发至device
*****************************************************************************/
static void hwifi_config_init_ini_main(oal_net_device_stru *cfg_net_dev)
{
    /* 国家码 */
    hwifi_config_init_ini_country(cfg_net_dev);

    /* 可维可测 */
    hwifi_config_init_ini_log(cfg_net_dev);
    /* RF */
    hwifi_config_init_ini_rf(cfg_net_dev);
    hwifi_config_rf_fe_custom_ini(cfg_net_dev);
}

#ifdef _PRE_WLAN_SMOOTH_PHASE
OAL_STATIC osal_u32 wal_set_smooth_phase_en(oal_net_device_stru *net_dev)
{
    smooth_phase_stru  smooth_phase_info;
    osal_u32    ret;

    smooth_phase_info.enable = 1;
    smooth_phase_info.extra_value = 0;
    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SMOOTH_PHASE,
        (osal_u8 *)&smooth_phase_info, OAL_SIZEOF(smooth_phase_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_PWR, "{wal_set_smooth_phase_en::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC osal_u32 wal_set_compatibility_er_su_th(oal_net_device_stru *net_dev)
{
    osal_u8 er_su_th = hwifi_get_compatibility_er_su_th();
    osal_u32 ret;

    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_ER_SU_FAIL_CNT_TH,
        (osal_u8 *)&er_su_th, OAL_SIZEOF(er_su_th));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_compatibility_er_su_th::return err code [%d]!}", ret);
    }
    return ret;
}

OAL_STATIC osal_u32 wal_set_compatibility_auto_send_null_data_switch(oal_net_device_stru *net_dev)
{
    osal_bool auto_send_null_data_switch = CUS_AUTO_SEND_NULL_DATA;
    osal_u32 ret;
    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev),
        WLAN_MSG_W2D_C_CFG_ENABLE_AUTO_NULLDATA_SWITCH,
        (osal_bool *)&auto_send_null_data_switch, OAL_SIZEOF(auto_send_null_data_switch));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_compatibility_auto_send_null_data_switch:return err[%d]!}", ret);
    }
    return ret;
}

#ifndef _PRE_WLAN_FEATURE_WS73
#ifdef _PRE_WLAN_FEATURE_AUTOAGGR
OAL_STATIC osal_u32 wal_set_ampdu_max_aggr_num(oal_net_device_stru *net_dev)
{
    osal_u8 ampdu_max_mpdu_num = hwifi_get_ampdu_tx_max_num();
    osal_u32 ret;

    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_AMPDU_MAX_AGGR_NUM,
        (osal_u8 *)&ampdu_max_mpdu_num, OAL_SIZEOF(ampdu_max_mpdu_num));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_ampdu_max_aggr_num::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}
#endif
#endif

/*****************************************************************************
 函 数 名  : wal_customize_set_config
 功能描述  : 配置定制化参数入口
*****************************************************************************/
osal_u32 wal_customize_set_config(td_void)
{
    osal_u32 ret = OAL_SUCC;
    oal_net_device_stru *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        wifi_printf("wifi_set_country Featureid0 device not fonud.\r\n");
        return OAL_FAIL;
    }

    hwifi_config_init_ini_main(netdev);
    /* 校准数据下发 */
    wal_send_cali_data_etc(netdev);
    /* 校准 */
    if (wal_customize_init_cali(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }

#ifdef _PRE_WLAN_SMOOTH_PHASE
    if (hwifi_get_smooth_phase_en() == OSAL_TRUE) {
        /* smooth使能下发消息 */
        ret = wal_set_smooth_phase_en(netdev);
        if (ret != OAL_SUCC) {
            return ret;
        }
    }
#endif

#ifndef _PRE_WLAN_FEATURE_WS73
#ifdef _PRE_WLAN_FEATURE_AUTOAGGR
    if (wal_set_ampdu_max_aggr_num(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }
#endif
#endif

    if (wal_set_compatibility_er_su_th(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }

    if (wal_set_compatibility_auto_send_null_data_switch(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : custom_host_read_cfg_init
 功能描述  : 首次读取定制化配置文件总入口
*****************************************************************************/
osal_s32 wal_customize_init(void)
{
    /* 先获取私有定制化项 */
    hwifi_config_init_etc(CUS_TAG_PRIV_INI);

    hwifi_config_init_etc(CUS_TAG_INI);
    /* 射频前端定制化项解析 */
    hwifi_config_init_etc(CUS_TAG_RF_FE);
    /* 射频前端定制化项解析 功率部分 */
    hwifi_config_init_etc(CUS_TAG_RF_FE_TX_POWER);
    /* 启动完成后，输出打印 */
    wifi_printf("wal_customize_init finish!\r\n");

    return OAL_SUCC;
}

osal_s32 wal_customize_exit(void)
{
    /* 启动完成后，输出打印 */
    wifi_printf("wal_customize_exit finish!\r\n");

    return OAL_SUCC;
}

int hwifi_hcc_h2d_priv_customize(void)
{
    osal_s32 l_ret;
    /* wifi上电时重读定制化配置 */
    l_ret = wal_customize_init();
    if (l_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "hwifi_hcc_customize_h2d_data_cfg data ret[%d]", l_ret);
    }

    // 如果不成功，返回失败
    l_ret = hwifi_hcc_custom_ini_data_buf(CUSTOM_CFGID_PRIV_INI_ID);
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_CFG, "hwifi_hcc_customize_h2d_data_cfg priv data fail, ret[%d]", l_ret);
        return OAL_FAIL;
    }
    return INI_SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_hcc_customize_h2d_data_cfg
 功能描述  : 协议栈初始化前定制化配置入口
*****************************************************************************/
osal_s32 hwifi_hcc_customize_h2d_data_cfg(void)
{
    osal_s32 l_ret;

    /* wifi上电时重读定制化配置 */
    l_ret = wal_customize_init();
    if (l_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "hwifi_hcc_customize_h2d_data_cfg data ret[%d]", l_ret);
    }

    l_ret = hwifi_hcc_custom_ini_data_buf(CUSTOM_CFGID_INI_ID);
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_CFG, "hwifi_hcc_customize_h2d_data_cfg ini data fail, ret[%d]", l_ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_config_init_ini_custom
 功能描述  : 初始化私有定制全局变量数组
*****************************************************************************/
OAL_STATIC osal_s32 hwifi_config_init_ini_custom(void)
{
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND1] = 0xF4F4;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND2] = 0xF4F4;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND3] = 0xF4F4;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C0_MULT4] = 0;
    g_al_host_init_params_etc[WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C1_MULT4] = 0;
    oam_info_log0(0, OAM_SF_CFG, "hwifi_config_init_ini_custom success!");

    return INI_SUCC;
}

#ifdef CUSTOMIZE_NV_WS92
static osal_u16 hwifi_config_get_rf_cali_mask(osal_void)
{
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u16 cali_mask = 0;
    osal_u16 nv_cali_mask_length = 0;
    osal_u32 nv_ret;

    nv_ret = uapi_nv_read(NV_ID_FE_CALI_MASK, sizeof(cali_mask), &nv_cali_mask_length, (osal_u8 *)&cali_mask);
    if (nv_ret == OAL_SUCC) {
        return cali_mask;
    } else {
        wifi_printf("hwifi_config_get_rf_cali_mask::nv read cali mask fail ret[%d]\r\n", nv_ret);
    }
#endif
#ifdef _PRE_WLAN_FEATURE_WS53
    return 0x1F33;
#else
    return 0x1F22;   /* 校准开关:0x1F02 开启IQ DC POWER RC */
#endif
}
static osal_u16 hwifi_config_get_rf_cali_data_mask(osal_void)
{
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u16 cali_data_mask = 0;
    osal_u16 nv_cali_data_mask_length = 0;
    osal_u32 nv_ret;

    nv_ret = uapi_nv_read(NV_ID_FE_CALI_DATA_MASK, sizeof(cali_data_mask),
        &nv_cali_data_mask_length, (osal_u8 *)&cali_data_mask);
    if (nv_ret == OAL_SUCC) {
        return cali_data_mask;
    } else {
        wifi_printf("hwifi_config_get_rf_cali_mask::nv read cali mask fail ret[%d]\r\n", nv_ret);
    }
#endif
    return 0x14;
}

static osal_u8 hwifi_get_user_num_nv(void)
{
    osal_u8 user_num = WLAN_ASSOC_USER_MAX_NUM;
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u8 nv_user_num;
    osal_u16 nv_user_num_len;
    osal_u32 ret;

    ret = uapi_nv_read(NV_ID_MAX_USER_NUM, sizeof(user_num), &nv_user_num_len, &nv_user_num);
    if (ret != OAL_SUCC) {
        user_num = WLAN_ASSOC_USER_MAX_NUM;
        wifi_printf("hwifi_get_user_num_nv fail ,use default %d\r\n", user_num);
    } else {
        user_num = OAL_MIN(nv_user_num, WLAN_ASSOC_USER_MAX_NUM);
    }
#endif
    return user_num;
}

static void hwifi_config_init_private_custom_val(void)
{
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_MASK].l_val = hwifi_config_get_rf_cali_mask();
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_DATA_MASK].l_val = hwifi_config_get_rf_cali_data_mask();
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_AUTOCALI_MASK].l_val = 0;  /* 自动化校准开关:0 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_BW_MAX_WITH].l_val = 0; /* 最大带宽能力:0 20M */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SU_BFER].l_val = 0; /* su_bfer:0不支持 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SU_BFEE].l_val = 1; /* su_bfee:1支持 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_MU_BFER].l_val = 0; /* mu_bfer:0不支持 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_MU_BFEE].l_val = 0; /* mu_bfee:0不支持 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_USER_NUM].l_val = hwifi_get_user_num_nv();    /* 支持用户数 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_HMAC_MAX_AP_NUM].l_val = 64; /* hmac扫描ap结果最大限制:32->64 去掉wpa扫描链表 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SCAN_PROBE_REQ_ALL_IE].l_val = 1; /* scan probe req 默认携带能力IE */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SCAN_PROBE_REQ_DEL_WPS_IE].l_val = 1; /* sta scan probe req 默认删除WPS IE */
    g_al_priv_cust_params[WLAN_CFG_PRIV_LDPC].l_val = 1;    /* ldpc:1支持 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_BA_32BIT_BITMAP].l_val = 0; /* ba_32bitmap:0 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_MTID_AGGR_RX].l_val = 0;    /* mtid_aggr_rx:0 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_ENABLE].l_val = 0; /* DFR设置: 异常自愈功能开关 关闭 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_PERIOD].l_val = 5000;  /* DFR设置: 异常自愈检测周期 5000ms */
    /* DFR设置：异常自愈，上报周期衰减，例子, cnt为10，表示上报100次后，上报的频率减小10倍 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_CNT].l_val = 10;
    /* dbac共存gc vap占用空口的比例，默认50(取值范围20-80) */
    g_al_priv_cust_params[WLAN_CFG_PRIV_DBAC_STA_GC_SLOT_RATIO].l_val = 50;
    /* dbac共存go vap占用空口的比例，默认30(取值范围20-80) */
    g_al_priv_cust_params[WLAN_CFG_PRIV_DBAC_STA_GO_SLOT_RATIO].l_val = 30;
    /* 小包amsdu聚合(<128bytes)的最大聚合个数，建议使用值4 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMSDU_NUM].l_val = 4;
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMSDU_TX_ON].l_val = 1; /* amsdu开关:1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMPDU_AMSDU_TX_ON].l_val = 1;   /* ampdu+amsdu联合聚合开关:1开启 */
    /* 最大聚合度，取值范围1-16，推荐值16 */
    g_al_priv_cust_params[WLAN_CFG_INIT_AMPDU_TX_MAX_NUM].l_val = 16;
    /* 接收方向ampdu窗口值，取值范围1-32，推荐值32 */
    g_al_priv_cust_params[WLAN_CFG_INIT_AMPDU_RX_MAX_NUM].l_val = 32;
    /* 发送方向ampdu窗口值，取值范围2-32，推荐值32（最大聚合度需小于窗口值的一半） */
    g_al_priv_cust_params[WLAN_CFG_INIT_AMPDU_TX_BAW_SIZE].l_val = 32;
    g_al_priv_cust_params[WLAN_CFG_PRIV_DATA_SAMPLE].l_val = 0; /* 数采开关:0关闭 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_ER_SU_DISABLE].l_val = 0;   /* 禁用er_su(1表示禁用,0表示使用) */
    g_al_priv_cust_params[WLAN_CFG_PRIV_DCM_CONSTELLATION_TX].l_val = 3; /* 0:不支持DCM,1:BPSK,2:QPSK,3:16-QAM */
    /* bandwidth extended range能力配置(0:不支持106-tone,1:支持106-tone) */
    g_al_priv_cust_params[WLAN_CFG_PRIV_BANDWIDTH_EXTENDED_RANGE].l_val = 1;
    g_al_priv_cust_params[WLAN_CFG_PRIV_APF_ENABLE].l_val = 1;  /* apf过滤报文开关:1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_WOW_EVENT].l_val = 0x0f;    /* wow唤醒源配置:0x0f */
    g_al_priv_cust_params[WLAN_CFG_PRIV_SMOOTH_PHASE_EN].l_val = 1; /* smooth平滑开关:1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_RX_STBC].l_val = 1; /* 控制stbc接收开关:1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_WOW_ENABLE].l_val = 1;  /* wow特性开关:1开启 */

    /* 漫游相关 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_ROAM_TRIGGER_RSSI_2G].l_val = -78;  /* roam_trigger_rssi_2g=-78 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_ROAM_DELTA_RSSI_2G].l_val = 12; /* roam_delta_rssi_2g=12 */

    g_al_priv_cust_params[WLAN_CFG_PRIV_OVER_DS_EN].l_val = 1;  /* 11r:over_ds使能开关 1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_EXT_COEX_EN].l_val = 0; /* 1:外部蓝牙共存 0:内部共存 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_TCP_ACK_FILTER_EN].l_val = 0; /* 1: 默认开 0:默认关 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_TCP_ACK_MAX_NUM_START_PROCESS].l_val = 2; /* 2:触发tcp ack处理启动的最大个数 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_FRONT_SWITCH].l_val = 0; /* 0:进入低功耗前端不关闭，1：进入低功耗前端关闭 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_TXDATA_QUE_LIMIT].l_val = 2000; /* 2000：txdata que最大长度限制 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_CROSS_PROT_ENABLE].l_val = 1; /* 跨协议探测开关：1开启 */
    g_al_priv_cust_params[WLAN_CFG_PRIV_COMPATIBILITY_ER_SU_TH].l_val = 5; /* ERSU兼容性处理发送失败次数阈值 默认5 */
}

static void hwifi_config_init_private_custom_state(void)
{
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_MASK].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_DATA_MASK].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_CALI_AUTOCALI_MASK].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_BW_MAX_WITH].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SU_BFER].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SU_BFEE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_MU_BFER].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_MU_BFEE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_USER_NUM].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_HMAC_MAX_AP_NUM].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SCAN_PROBE_REQ_ALL_IE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SCAN_PROBE_REQ_DEL_WPS_IE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_LDPC].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_BA_32BIT_BITMAP].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_MTID_AGGR_RX].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_ENABLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_PERIOD].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SELF_HEALING_CNT].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_DBAC_STA_GC_SLOT_RATIO].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_DBAC_STA_GO_SLOT_RATIO].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_TPC_FAR_RSSI].value_state = OAL_FALSE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMSDU_NUM].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMSDU_TX_ON].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_AMPDU_AMSDU_TX_ON].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_INIT_AMPDU_TX_MAX_NUM].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_INIT_AMPDU_RX_MAX_NUM].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_DATA_SAMPLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_ER_SU_DISABLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_FRONT_SWITCH].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_DCM_CONSTELLATION_TX].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_BANDWIDTH_EXTENDED_RANGE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_APF_ENABLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_WOW_EVENT].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_SMOOTH_PHASE_EN].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_RX_STBC].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_WOW_ENABLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_ROAM_TRIGGER_RSSI_2G].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_ROAM_DELTA_RSSI_2G].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_OVER_DS_EN].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_EXT_COEX_EN].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_TCP_ACK_FILTER_EN].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_TCP_ACK_MAX_NUM_START_PROCESS].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_CUSTOM_DATA_END].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_TXDATA_QUE_LIMIT].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_CROSS_PROT_ENABLE].value_state = OAL_TRUE;
    g_al_priv_cust_params[WLAN_CFG_PRIV_COMPATIBILITY_ER_SU_TH].value_state = OAL_TRUE;
}
#endif

/*****************************************************************************
 函 数 名  : hwifi_config_init_private_custom
 功能描述  : 初始化私有定制全局变量数组
*****************************************************************************/
OAL_STATIC osal_s32 hwifi_config_init_private_custom(void)
{
    hwifi_config_init_private_custom_val();
    hwifi_config_init_private_custom_state();

    oam_info_log0(0, OAM_SF_CFG, "hwifi_config_init_private_custom success!");
    return INI_SUCC;
}

OAL_STATIC osal_u32 hwifi_config_rf_fe_rx_insert_loss_custom(osal_void)
{
    wlan_cust_rf_fe_rx_insert_loss_params default_cust_rf_rx_loss = {
        .rx_insert_loss_2g = { 0, 0, 0 },
        .rsv = { 0 }
    };
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    wlan_cust_rf_fe_rx_insert_loss_params nv_rx_loss = {0};
    osal_u16 nv_rx_loss_length = 0;
    osal_u32 nv_ret;

    nv_ret = uapi_nv_read(NV_ID_FE_RX_INSERT_LOSS, SOC_CUSTOM_RF_FE_RX_LOSS_NUM,
        &nv_rx_loss_length, (osal_u8 *)&nv_rx_loss);
    if (nv_ret == OAL_SUCC) {
        default_cust_rf_rx_loss = nv_rx_loss;
    } else {
        wifi_printf("rf_fe_rx_insert_loss_custom::nv read rx loss fail ret[%d]\r\n", nv_ret);
    }
#endif
    g_cust_rf_fe_params.rf_rx_loss = default_cust_rf_rx_loss;
    g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_PARAMS);

    return INI_SUCC;
}
OAL_STATIC osal_u32 hwifi_config_rf_fe_cta_coef_switch(osal_void)
{
    osal_u8 default_cta_coef_switch = 0;
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u8 cta_coef_switch = 0;
    osal_u16 cta_coef_switch_len = 0;
    osal_u32 nv_ret;
    nv_ret = uapi_nv_read(NV_ID_FE_CTA_COEF_SWITCH, sizeof(osal_u8), &cta_coef_switch_len, &cta_coef_switch);
    if (nv_ret == OAL_SUCC) {
        g_cust_rf_fe_params.rf_power.special_cta_coef_switch[0] = cta_coef_switch;
        return INI_SUCC;
    } else {
        wifi_printf("hwifi_config_rf_fe_cta_coef_switch::nv read cta coef fail ret[%d]\r\n", nv_ret);
    }
#endif
    g_cust_rf_fe_params.rf_power.special_cta_coef_switch[0] = default_cta_coef_switch;
    return INI_SUCC;
}
// 根据域id取功率定制化参数
OAL_STATIC osal_u32 hwifi_config_rf_fe_power_custom(osal_void)
{
    osal_s32 tag_index = g_nvram_tag; // 按照国家码选择对应的tag index 如INI_MODU_FCC_NVRAM
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    wlan_cust_rf_fe_power_params tx_power = {0};
    osal_u16 nv_tx_power_length = 0;
    osal_u32 nv_ret;
    osal_u16 max_len = WLAN_RF_FE_MAX_POWER_NUM + WLAN_RF_FE_TARGET_POWER_NUM + WLAN_RF_FE_LIMIT_POWER_NUM +
        WLAN_RF_FE_SAR_POWER_NUM;
#endif
    // 域id有效则使用对应id的数据 否则使用默认
    if (tag_index >= REGDOMAIN_COUNT) {
        tag_index = REGDOMAIN_COMMON;
    }
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    nv_ret = uapi_nv_read(NV_ID_FE_TX_POWER_FCC + tag_index, max_len, &nv_tx_power_length, (osal_u8 *)&tx_power);
    if (nv_ret == OAL_SUCC) {
        g_cust_rf_fe_params.rf_power = tx_power;
        hwifi_config_rf_fe_cta_coef_switch();
        g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER);
        return INI_SUCC;
    } else {
        wifi_printf("hwifi_config_get_rf_cali_mask::nv read tx power fail ret[%d]\r\n", nv_ret);
    }
#endif
    g_cust_rf_fe_params.rf_power = g_default_cust_tx_power[tag_index];
    hwifi_config_rf_fe_cta_coef_switch();
    g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER);
    return INI_SUCC;
}

OAL_STATIC osal_u32 hwifi_config_rf_fe_power_cali_custom(osal_void)
{
#ifdef BOARD_ASIC_WIFI
    const wlan_cust_rf_fe_power_cali_params default_cust_rf_power_cali = {
        .ref_power_2g = { 200, 200, 200, 50, 50, 50 },
        .power_cure_2g = { { 0, 1, -198, 0, 1, -197, 0, 1, -197 },
            { 0, 1, -311,  0, 1, -323,  0, 1, -323 } },
        .curve_factor_2g = { 0, 0, 0,  0, 0, 0 }
    };
#else
    const wlan_cust_rf_fe_power_cali_params default_cust_rf_power_cali = {
        .ref_power_2g = { 150, 150, 150, 120, 120, 120 },
        .power_cure_2g = { { 0, 341, 0, 0, 341, 0, 0, 341, 0 },
            { 0, 341, 0, 0, 341, 0, 0, 341, 0 } },
        .curve_factor_2g = { 16, 10, 0, 16, 10, 0 }
    };
#endif
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    wlan_cust_rf_fe_power_cali_params nv_rf_power_cali = {0};
    osal_u16 nv_ref_power_2g_length = 0;
    osal_u16 nv_power_curve_2g_length = 0;
    osal_u16 nv_curve_factor_2g_length = 0;
    osal_u16 nv_id_power_curve[] = {NV_ID_FE_TX_POWER_CURVE_HIGH, NV_ID_FE_TX_POWER_CURVE_LOW};
    osal_u32 nv_ret = INI_SUCC;
    osal_u8 index = 0;

    nv_ret |= uapi_nv_read(NV_ID_FE_TX_REF_POWER, sizeof(nv_rf_power_cali.ref_power_2g),
        &nv_ref_power_2g_length, (osal_u8 *)nv_rf_power_cali.ref_power_2g);
    for (index = 0; index < SOC_CUSTOM_RF_FE_CURVE_NUM_LVL; index++) {
        nv_ret |= uapi_nv_read(nv_id_power_curve[index], sizeof(nv_rf_power_cali.power_cure_2g[index]),
            &nv_power_curve_2g_length, (osal_u8 *)nv_rf_power_cali.power_cure_2g[index]);
    }
    nv_ret |= uapi_nv_read(NV_ID_FE_TX_CURVE_FACTOR, sizeof(nv_rf_power_cali.curve_factor_2g),
        &nv_curve_factor_2g_length, (osal_u8 *)nv_rf_power_cali.curve_factor_2g);
    if (nv_ret == INI_SUCC) {
        g_cust_rf_fe_params.rf_power_cali = nv_rf_power_cali;
        g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER_CALI);
        return INI_SUCC;
    } else {
        wifi_printf("hwifi_config_rf_fe_power_cali_custom::nv read fail ret[%d]\r\n", nv_ret);
    }
#endif
    g_cust_rf_fe_params.rf_power_cali = default_cust_rf_power_cali;
    g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER_CALI);

    return INI_SUCC;
}

// 单独处理功率部分 国家码变更时刷新 其他ini配置项不需要刷新
OAL_STATIC osal_u32 hwifi_config_rf_fe_custom_tx_power(osal_void)
{
    // 状态更新 将功率置0 解析后成功刷1
    g_cust_rf_fe_params.ini_ret &= (~(1 << WLAN_CFG_INI_RF_FE_TYPE_POWER));
    hwifi_config_rf_fe_power_custom();
    return INI_SUCC;
}

OAL_STATIC osal_u32 hwifi_config_rf_fe_rssi_custom(osal_void)
{
    const wlan_cust_rf_fe_rssi_params default_cust_rf_rssi = {
        .rssi_comp = { 0, 0, 0 },
        .rssi_sel_bits = 0
    };
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    wlan_cust_rf_fe_rssi_params nv_rssi = {0};
    osal_u16 nv_rssi_comp_length = 0;
    osal_u32 nv_ret;

    nv_ret = uapi_nv_read(NV_ID_FE_RX_RSSI_COMP, sizeof(nv_rssi.rssi_comp),
        &nv_rssi_comp_length, (osal_u8 *)nv_rssi.rssi_comp);
    if (nv_ret == OAL_SUCC) {
        g_cust_rf_fe_params.rf_rssi = nv_rssi;
        g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_RSSI);
        return INI_SUCC;
    } else {
        wifi_printf("hwifi_config_rf_fe_rssi_custom::nv read rssi comp fail ret[%d]\r\n", nv_ret);
    }
#endif
    g_cust_rf_fe_params.rf_rssi = default_cust_rf_rssi;
    g_cust_rf_fe_params.ini_ret |= (1 << WLAN_CFG_INI_RF_FE_TYPE_RSSI);
    return INI_SUCC;
}

OAL_STATIC osal_s32 hwifi_config_rf_fe_custom(osal_void)
{
    // 状态更新 除功率外都置0 解析后成功的刷1
    g_cust_rf_fe_params.ini_ret &= (1 << WLAN_CFG_INI_RF_FE_TYPE_POWER);
    hwifi_config_rf_fe_rx_insert_loss_custom();
    hwifi_config_rf_fe_power_cali_custom();
    hwifi_config_rf_fe_rssi_custom();
    return INI_SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_config_init_etc
 功能描述  : netdev open 调用的定制化总入口，读取ini文件，更新 g_al_host_init_params_etc 全局数组
 修改内容  : 增加tag用于判断ini和dts
*****************************************************************************/
osal_s32 hwifi_config_init_etc(osal_s32 cus_tag)
{
    switch (cus_tag) {
        case CUS_TAG_INI:
            host_params_init_first();
            return hwifi_config_init_ini_custom();
        case CUS_TAG_PRIV_INI:
            return hwifi_config_init_private_custom();
        case CUS_TAG_RF_FE_TX_POWER:
            return (osal_s32)hwifi_config_rf_fe_custom_tx_power();
        case CUS_TAG_RF_FE:
            return hwifi_config_rf_fe_custom();
        default:
            oam_error_log1(0, OAM_SF_CUSTOM, "{hwifi_config_init_etc tag number[0x%x] not correct!}", cus_tag);
            return INI_FAILED;
    }

    return INI_SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_get_mac_addr_etc
 功能描述  : 从nvram中获取mac地址，如果获取失败，则随机一个mac地址
*****************************************************************************/
osal_s32 hwifi_get_mac_addr_etc(osal_u8 *buf)
{
    buf[1] = 0x11;  /* 1表示第一位mac地址 */
    buf[2] = 0x03;  /* 2表示第二位mac地址 */

    return INI_SUCC;
}
/*****************************************************************************
 函 数 名  : hwifi_get_init_value_etc
 修改内容   : 增加tag用于判断ini和dts
*****************************************************************************/
osal_s32 hwifi_get_init_value_etc(osal_s32 cus_tag, osal_s32 cfg_id)
{
    osal_s32 *pgal_params = OAL_PTR_NULL;
    osal_s32 l_wlan_cfg_butt;

    if (cus_tag == CUS_TAG_INI) {
        pgal_params = &g_al_host_init_params_etc[0];
        l_wlan_cfg_butt = WLAN_CFG_INIT_BUTT;
    } else {
        oam_error_log1(0, OAM_SF_ANY, "hwifi_get_init_value_etc tag number[0x%2x] not correct!", cus_tag);
        return INI_FAILED;
    }

    if ((cfg_id < 0) || (l_wlan_cfg_butt <= cfg_id)) {
        oam_error_log2(0, OAM_SF_ANY, "hwifi_get_init_value_etc cfg id[%d] out of range, max cfg id is:%d", cfg_id,
            (l_wlan_cfg_butt - 1));
        return INI_FAILED;
    }

    return pgal_params[cfg_id];
}

/*****************************************************************************
 函 数 名  : hwifi_get_init_priv_value
*****************************************************************************/
osal_s32 hwifi_get_init_priv_value(osal_s32 l_cfg_id, osal_s32 *pl_priv_value)
{
    if ((l_cfg_id < 0) || (l_cfg_id >= WLAN_CFG_PRIV_BUTT)) {
        oam_error_log2(0, OAM_SF_ANY, "hwifi_get_init_priv_value cfg id[%d] out of range, max[%d]", l_cfg_id,
            WLAN_CFG_PRIV_BUTT - 1);
        return OAL_FAIL;
    }

    if (g_al_priv_cust_params[l_cfg_id].value_state == OAL_FALSE) {
        return OAL_FAIL;
    }

    *pl_priv_value = g_al_priv_cust_params[l_cfg_id].l_val;

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : hwifi_get_country_code_etc
*****************************************************************************/
osal_s8 *hwifi_get_country_code_etc(void)
{
    return g_ac_country_code_etc;
}

/*****************************************************************************
 函 数 名  : hwifi_set_country_code_etc
*****************************************************************************/
osal_void hwifi_set_country_code_etc(osal_s8 *country_code, const osal_u32 len)
{
    if ((country_code == OAL_PTR_NULL) || (len != COUNTRY_CODE_LEN)) {
        oam_error_log0(0, OAM_SF_ANY, "hwifi_get_country_code_etc ptr null or illegal len!");
        return;
    }

    if (memcpy_s(g_ac_country_code_etc, sizeof(g_ac_country_code_etc), country_code, COUNTRY_CODE_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwifi_set_country_code_etc::memcpy_s error}");
    }
    g_ac_country_code_etc[2] = '\0';  /* 2表示结束位 */
}

/*****************************************************************************
 函 数 名  : hwifi_get_hmac_max_ap_num_etc
 功能描述      : 获取hmac侧保存ap数量的阈值
*****************************************************************************/
osal_u16 hwifi_get_hmac_max_ap_num_etc(void)
{
    return g_hmac_max_ap_num;
}

void *hwifi_wlan_customize_etc(void)
{
    return &g_wlan_customize_etc;
}

/*****************************************************************************
 函 数 名  : hwifi_set_hmac_max_ap_num_etc
 功能描述      : 初始化g_hmac_ap_list_info
 输入参数      : osal_u16 max_ap_num hmac侧保存ap数量的阈值
*****************************************************************************/
osal_void hwifi_set_hmac_max_ap_num_etc(osal_u16 max_ap_num)
{
    g_hmac_max_ap_num = max_ap_num;
#ifndef _PRE_WLAN_FEATURE_WS73
    if (g_hmac_max_ap_num > 0x40) {
        g_hmac_max_ap_num = 0x40;
    }
#endif
}

uint32_t hwifi_get_self_healing_enable_etc(void)
{
    return g_self_healing_enable;
}

void hwifi_set_self_healing_enable_etc(uint32_t self_healing_enable)
{
    g_self_healing_enable = self_healing_enable;
}

uint32_t hwifi_get_self_healing_period_etc(void)
{
    return g_self_healing_period;
}

void hwifi_set_self_healing_period_etc(uint32_t self_healing_period)
{
    g_self_healing_period = self_healing_period;
}

uint32_t hwifi_get_self_healing_cnt_etc(void)
{
    return g_self_healing_cnt;
}

void hwifi_set_self_healing_cnt_etc(uint32_t self_healing_cnt)
{
    g_self_healing_cnt = self_healing_cnt;
}

osal_u8 hwifi_get_amsdu_num(osal_void)
{
    return g_amsdu_max_num;
}

osal_void hwifi_set_amsdu_num(osal_u8 amsdu_num)
{
    g_amsdu_max_num = amsdu_num;
}

osal_u8 hwifi_get_amsdu_tx_active(osal_void)
{
    return g_amsdu_tx_active;
}

osal_void hwifi_set_amsdu_tx_active(osal_u8 active)
{
    g_amsdu_tx_active = active;
}

osal_u8 hwifi_get_ampdu_amsdu_tx_active(osal_void)
{
    return g_ampdu_amsdu_tx_active;
}

osal_void hwifi_set_ampdu_amsdu_tx_active(osal_u8 active)
{
    g_ampdu_amsdu_tx_active = active;
}

osal_u8 hwifi_get_ampdu_tx_max_num(osal_void)
{
    return g_ampdu_tx_max_num;
}

osal_void hwifi_set_ampdu_tx_max_num(osal_u8 num)
{
    g_ampdu_tx_max_num = num;
}

osal_u8 hwifi_get_ampdu_rx_max_num(osal_void)
{
    return g_ampdu_rx_max_num;
}

osal_void hwifi_set_ampdu_rx_max_num(osal_u8 num)
{
    g_ampdu_rx_max_num = num;
}

osal_void hwifi_set_ampdu_tx_baw_size(osal_u8 num)
{
    g_ampdu_tx_baw_size = num;
}

osal_u8 hwifi_get_ampdu_tx_baw_size(osal_void)
{
    return g_ampdu_tx_baw_size;
}

uint8_t hwifi_get_data_sample(void)
{
    return g_data_sample;
}

osal_void hwifi_set_data_sample(uint8_t data_sample)
{
    g_data_sample = data_sample;
}

void hwifi_set_apf_enable(uint8_t apf_enable)
{
    g_apf_enable = apf_enable;
}

uint8_t hwifi_get_apf_enable(void)
{
    return g_apf_enable;
}

/*****************************************************************************
 函 数 名  : hwifi_set_wow_enable
 功能描述      : 从ini文件读取wow开关，并设置，只在hmac初始化时调用
 输入参数      : osal_u8 wow_enable
****************************************************************************/
osal_void hwifi_set_wow_enable(osal_u8 wow_enable)
{
    g_wow_enable = wow_enable;
}

/*****************************************************************************
 函 数 名  : hwifi_set_wow_event
 功能描述      : 从ini文件读取wow 唤醒源，并设置，只在hmac初始化时调用
 输入参数      : osal_u32 wow_event
****************************************************************************/
osal_void hwifi_set_wow_event(osal_u32 wow_event)
{
    g_wow_event = wow_event;
}

/*****************************************************************************
 函 数 名  : hwifi_get_wow_enable
 功能描述  : 获取ini中设置的wow开关
****************************************************************************/
osal_u8 hwifi_get_wow_enable(osal_void)
{
    return g_wow_enable;
}

/*****************************************************************************
 函 数 名  : hwifi_get_wow_event
 功能描述  : 获取ini中设置的wow 唤醒源
****************************************************************************/
osal_u32 hwifi_get_wow_event(osal_void)
{
    return g_wow_event;
}

uint8_t hwifi_get_smooth_phase_en(void)
{
    return g_smooth_phase_en;
}

void hwifi_set_smooth_phase_en(uint8_t smooth_phase_en)
{
    g_smooth_phase_en = smooth_phase_en;
}

uint8_t hwifi_get_compatibility_er_su_th(void)
{
    return g_compatibility_er_su_th;
}

void hwifi_set_compatibility_er_su_th(uint8_t er_su_th)
{
    g_compatibility_er_su_th = er_su_th;
}

#ifdef _PRE_WLAN_FEATURE_BTCOEX
uint8_t hwifi_get_ext_coex_en(void)
{
    return g_ext_coex_en;
}

void hwifi_set_ext_coex_en(uint8_t ext_coex_en)
{
    g_ext_coex_en = ext_coex_en;
}
#endif

// 配置低电流启动模式
osal_void hwifi_set_low_current_boot_mode(osal_u16 flag)
{
    g_cus_low_current_boot_mode = flag;
}
osal_u16 hwifi_get_low_current_boot_mode_cali_data_mask(osal_u16 cali_data_mask, osal_u8 first_cali_done_flag)
{
    osal_u16 mask_bit2 = 0x4;  // cali data mask:cali
    osal_u16 mask_bit3 = 0x8;   // cali data mask:close pwr cali

    switch (g_cus_low_current_boot_mode) {
        case LOW_CURRENT_BOOT_MODE_DEFAULT:
            return ((cali_data_mask | mask_bit2) & (~mask_bit3));
        case LOW_CURRENT_BOOT_MODE_WITHOUT_PWR_CALI:
            return (cali_data_mask | mask_bit3);
        case LOW_CURRENT_BOOT_MODE_WITH_ONCE_PWR_CALI:
            // 经过首次校准后关闭功率校准
            if (first_cali_done_flag != 0) {
                return (cali_data_mask | mask_bit3);
            } else {
                return cali_data_mask;
            }
        case LOW_CURRENT_BOOT_MODE_WITHOUT_CALI:
            return (cali_data_mask & (~mask_bit2));
        default:
            return cali_data_mask;
    }
}

/*
    读取国家码所在大区index, 查询失败, 返回-1
*/
osal_s8 hwifi_get_region(const osal_s8 *country_code)
{
    osal_u8 index;
    /* 大区国家映射, 共4个大区, 每个大区对应多个国家 */
    const osal_s8 region_country_map[REGDOMAIN_COUNT][REGION_BUF_LEN] = {
        "US,CA,KH,NA",
        "RU,AU,MY,ID,TR,PL,FR,PT,IT,DE,ES,AR,ZA,MA,PH,TH,GB,CO,MX,EC,PE,CL,SA,EG,AE,EU",
        "JP",
        "CN,WW"
    };

    /* 在配置文件中依次查询四个大区的内容 */
    for (index = 0; index < REGDOMAIN_COUNT; index++) {
        if (strstr((osal_char *)region_country_map[index], (osal_char *)country_code) != OSAL_NULL) {
            wifi_printf("hwifi_get_region find %s in %d\r\n", country_code, index);
            return (osal_s8)index;
        }
    }
    wifi_printf("hwifi_get_region %s: region not found\r\n", country_code);
    return INI_FAILED;
}
/* 根据大区index设置当前大区功率tag */
osal_void hwifi_set_nvram_tag_by_region_index(osal_s8 region_idx)
{
    g_nvram_tag = osal_max(region_idx, 0);
}
osal_u32 hwifi_force_refresh_rf_params(oal_net_device_stru *net_dev)
{
    /* 刷新射频前端定制化项功率参数 跟随国家码区域变更 */
    hwifi_config_init_etc(CUS_TAG_RF_FE_TX_POWER);
    hwifi_config_rf_fe_custom_ini_tx_power(net_dev);
    return OAL_SUCC;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
