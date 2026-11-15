/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV - key_id.
 */

#ifndef __KEY_ID_H__
#define __KEY_ID_H__

#define REGION_KEY_NUMS 0x1000 /* 每个区域容纳key_id的个数 */

/* Please use the format of NV_ID_XXX */

/* system factory key_id [0x0001,0x00C0) */
#define NV_ID_SYSTEM_FACTORY_AREA_START    0x0001 /* Not a real key_id , just used for region determination. */
#define NV_ID_SYSTEM_FACTORY_MAC           0x0005 /* Not a real key_id , just used for region determination. */
#define NV_ID_SYSTEM_XO_TRIM_TEMP_SW       0x0006 /* Temp comp switch for xo trim. */
#define NV_ID_SYSTEM_XO_TRIM_TEMP_PARAM    0x0007 /* Temp comp fine code for xo trim. */
#define NV_ID_SYSTEM_FACTORY_SLE_MAC       0x0008 /* SLE MAC NV KEY ID. */

/* BTH key_id [0x00C0,0x00FF) */
#define NV_ID_BTH_PRODUCT_INFORMATION      0x00C0
#define NV_ID_GLE_SEC                      0x00C1
#define NV_ID_BTH_SMP_KEYS                 0x00C2
#define NV_ID_BTH_HIDINFO                  0x00C3
#define NV_ID_BTH_RESERVED                 0x00C4

#define NV_ID_BTH_BLE_MESH_RPL             0x00D0 /* ble mesh replay list */
#define NV_ID_BTH_BLE_MESH_NET_KEY         0x00D1 /* ble mesh net key */
#define NV_ID_BTH_BLE_MESH_APP_KEY         0x00D2 /* ble mesh app key */
#define NV_ID_BTH_BLE_MESH_NET_ADDR        0x00D3 /* ble mesh net addr */
#define NV_ID_BTH_BLE_MESH_IV              0x00D4 /* ble mesh iv */
#define NV_ID_BTH_BLE_MESH_SEQ             0x00D5 /* ble mesh sequence */
#define NV_ID_BTH_BLE_MESH_HB_PUB          0x00D6 /* ble mesh heart beat */
#define NV_ID_BTH_BLE_MESH_CFG             0x00D7 /* ble mesh cfg */
#define NV_ID_BTH_BLE_MESH_VA              0x00D8 /* ble mesh virtual addr */
#define NV_ID_BTH_BLE_MESH_OTA_FLAG        0x00D9 /* ble mesh upg flag */
#define NV_ID_BTH_BLE_MESH_MOD             0x00E0 /* ble mesh model nv */
#define NV_ID_BTH_BLE_MESH_TUPLE           0x00E1 /* ble mesh tuple nv */
#define NV_ID_COMPOSITION_DATA_PAGE_128    0x00E2 /* NV nppi composition data page 128 id */

/* user factory key_id [0x1000,0x2000) */
#define NV_ID_USER_FACTORY_AREA_START      0x1000 /* Not a real key_id , just used for region determination. */

/* system normal key_id [0x2000,0x3000) */
#define NV_ID_SYSTEM_NORMAL_AREA_START     0x2000 /* Not a real key_id , just used for region determination. */
#define NV_ID_SECURE_CONN                  0x2001 /* secure connect */
#define NV_ID_OFFLINELOG_ENBALE_FLAG       0x2002 /* Offline log enable flag */
#define NV_ID_COUNTRY_CODE                 0x2003 /* Country code */
#define NV_ID_DATA_SAMPLE                  0x2004 /* Enable data sample */
#define NV_ID_ROAM_ENABLE                  0x2005 /* Enable roam */
#define NV_ID_ROAM_OVER_DS_ENABLE          0x2006 /* Enable roam */
#define NV_ID_SU_BFEE                      0x2007 /* Enable su bfee */
#define NV_ID_TX_LDPC                      0x2008 /* Enable tx ldpc */
#define NV_ID_RX_STBC                      0x2009 /* Enable rx stbc */
#define NV_ID_ER_SU_DISABLE                0x200A /* Enable er su disable */
#define NV_ID_DCM_CONSTELLATION_TX         0x200B /* Enable dcm constellation tx */
#define NV_ID_BANDWIDTH_EXTENDED_RANGE     0x200C /* Enable bandwidth extended range */
#define NV_ID_AMSDU_TX_NUM                 0x200D /* amsdu tx max num */
#define NV_ID_AMSDU_TX_ON                  0x200E /* Enable amsdu tx */
#define NV_ID_AMPDU_AMSDU_TX_ON            0x200F /* Enable ampdu amsdu tx */
#define NV_ID_AMPDU_TX_MAX_MPDU_NUM        0x2010 /* ampdu tx max mpdu num */
#define NV_ID_AMPDU_RX_MAX_MPDU_NUM        0x2011 /* ampdu rx max mpdu num */
#define NV_ID_AMPDU_TX_BAW_SIZE            0x2012 /* ampdu tx baw size */
#define NV_ID_MAX_USER_NUM                 0x2013 /* max user num */
#define NV_ID_PA_AL_ON_ENABLE              0x2014 /* pa always on */
#define NV_ID_CLR_IP_DISABLE               0x2015 /* Disable report lwip clr ip when disconn */
#define NV_ID_LINKLOSS_DISABLE             0x2016 /* Disable linkloss */
#define NV_ID_SOFTAP_FIX_DSPS              0x2017 /* Set Softap DSPS */
#define NV_ID_ALG_CROSS_PROT_ENABLE        0x2018 /* alg cross protocol enable */
#define NV_ID_ER_SU_FAIL_CNT_TH            0x2019 /* er su fail cnt threshold */
#define NV_ID_POWER_LEVEL                  0x201B /* POWER LEVEL VAL */

#define NV_ID_FE_RX_INSERT_LOSS            0x2050 /* RF RX insert loss */
#define NV_ID_FE_CALI_MASK                 0x2051 /* RF cali mask */
#define NV_ID_FE_CALI_DATA_MASK            0x2052 /* RF cali data mask */
#define NV_ID_FE_TX_POWER_FCC              0x2053 /* RF TX power for FCC */
#define NV_ID_FE_TX_POWER_ETSI             0x2054 /* RF TX power for ETSI */
#define NV_ID_FE_TX_POWER_JAPAN            0x2055 /* RF TX power for JAPAN */
#define NV_ID_FE_TX_POWER_COMMON           0x2056 /* RF TX power for COMMON */
#define NV_ID_FE_RX_RSSI_COMP              0x2057 /* RF RX rssi comp */
#define NV_ID_FE_TX_REF_POWER              0x2058 /* RF TX reference power */
#define NV_ID_FE_TX_POWER_CURVE_HIGH       0x2059 /* RF TX power curve for high level */
#define NV_ID_FE_TX_POWER_CURVE_LOW        0x205A /* RF TX power curve for low level */
#define NV_ID_FE_TX_CURVE_FACTOR           0x205B /* RF TX curve factor */
#define NV_ID_FE_CALI_DATA                 0x205C /* RF cali data */
#define NV_ID_FE_CTA_COEF_SWITCH           0x205D /* RF cta special coef switch */

#define NV_ID_BTC_TXPOWER_CFG              0x20A0 /* btc tx power cfg */
#define NV_ID_BTC_GOLDEN_PER_SWITCH        0x20A1 /* bt_golden_per_switch */
#define NV_ID_BTC_TX_TARGET_POWER_CFG      0x20A4 /* btc tx target power cfg */
#define NV_ID_BTC_SRRC_SWITCH              0x20A5 /* SRRC switch on */
#define NV_ID_BTC_SRRC_SET_CHANNEL         0x20A6 /* SRRC set channel */
#define NV_ID_BTC_SRRC_SET_POWER           0x20A7 /* SRRC set power */
#define NV_ID_BTC_SLE_CONN_DURATION        0x20A8 /* sle connection duration [8, 16], default value is 8 */
#define NV_ID_BTC_DEMOD_DIFF               0x20A9 /* bsle 1m/2m demod is diff */
#define NV_ID_BTC_SCHED_PRE_ARG_SWITCH     0x20AA /* Scheduling pre-arrangement switch */
#define NV_ID_BTC_CHANNEL_SCAN_SWITCH      0x20AB /* channel scan switch */
#define NV_ID_BTC_BLE_SCAN_CHANNEL_MAP     0x20AC /* ble scan Channel_Map bit[0-2] */
#define NV_ID_BTC_BLE_CALI_SWITCH          0x20AD /* btc_ble_poweron_cali_switch */
#define NV_ID_BTC_SLE_CFRAME_STATS         0x20AE /* btc sle cframe statistic */

#define NV_ID_RADAR_1M_HOME_STA_PLATSIC    0x2100
#define NV_ID_RADAR_1M_HALL_STA_PLATSIC    0x2101
#define NV_ID_RADAR_1M_HOME_AP_PLATSIC     0x2102
#define NV_ID_RADAR_1M_HALL_AP_PLATSIC     0x2103
#define NV_ID_RADAR_2M_HOME_STA_PLATSIC    0x2104
#define NV_ID_RADAR_2M_HALL_STA_PLATSIC    0x2105
#define NV_ID_RADAR_2M_HOME_AP_PLATSIC     0x2106
#define NV_ID_RADAR_2M_HALL_AP_PLATSIC     0x2107
#define NV_ID_RADAR_3M_HOME_STA_PLATSIC    0x2108
#define NV_ID_RADAR_3M_HALL_STA_PLATSIC    0x2109
#define NV_ID_RADAR_3M_HOME_AP_PLATSIC     0x210A
#define NV_ID_RADAR_3M_HALL_AP_PLATSIC     0x210B
#define NV_ID_RADAR_1M_HOME_STA_PCB        0x210C
#define NV_ID_RADAR_1M_HALL_STA_PCB        0x210D
#define NV_ID_RADAR_1M_HOME_AP_PCB         0x210E
#define NV_ID_RADAR_1M_HALL_AP_PCB         0x210F
#define NV_ID_RADAR_2M_HOME_STA_PCB        0x2110
#define NV_ID_RADAR_2M_HALL_STA_PCB        0x2111
#define NV_ID_RADAR_2M_HOME_AP_PCB         0x2112
#define NV_ID_RADAR_2M_HALL_AP_PCB         0x2113
#define NV_ID_RADAR_3M_HOME_STA_PCB        0x2114
#define NV_ID_RADAR_3M_HALL_STA_PCB        0x2115
#define NV_ID_RADAR_3M_HOME_AP_PCB         0x2116
#define NV_ID_RADAR_3M_HALL_AP_PCB         0x2117
#define NV_ID_RADAR_RCV1                   0x2118
#define NV_ID_RADAR_RCV2                   0x2119
#define NV_ID_RADAR_RCV3                   0x211A
#define NV_ID_RADAR_RCV4                   0x211B
#define NV_ID_RADAR_RCV5                   0x211C
#define NV_ID_RADAR_RCV6                   0x211D
#define NV_ID_RADAR_RCV7                   0x211E
#define NV_ID_RADAR_RCV8                   0x211F
#define NV_ID_RADAR_MWO_MODE               0x2120
#define NV_ID_RADAR_THRU_WALL_MODE         0x2124
#define NV_ID_RADAR_CHN_SWITCH             0x2130
#define NV_ID_RADAR_SLP_BASIC_PARA         0x2135
#define NV_ID_RADAR_SLP_AI_PARA            0x2136
#define NV_ID_RADAR_SLP_SENSITIVITY_PARA   0x2137
#define NV_ID_RADAR_SLP_MFG_PARA           0x2138
#define NV_ID_RADAR_SLP_MFG_STATUS         0x2139
#define NV_ID_RADAR_CTRL                   0x2140

#define NV_ID_HILINK_ENTRY_MODE            0x2150 /* hilink entry mode */
#define NV_ID_HILINK_CONNECT_INFO          0x2151 /* hilink connect info */
#define NV_ID_CHBA_MODE_CFG                0x2160 /* chba mode cfg */
/* system stable key_id [0x3000,0x4000) */
#define NV_ID_SYSTEM_STABLE_AREA_START     0x3000 /* Not a real key_id , just used for region determination. */
#define NV_ID_DBG_UART_BUS_ID              0x3001 /* dbg uart bus id */
#define NV_ID_AT_UART_BUS_ID               0x3002 /* at uart bus id */
#define NV_ID_HSO_UART_BUS_ID              0x3003 /* hso uart bus id */

/* user stable key_id [0x4000,0x5000) */
#define NV_ID_USER_STABLE_AREA_START       0x4000 /* Not a real key_id , just used for region determination. */

/* user normal key_id [0x5000,0xFFFF) */
#define NV_ID_USER_NORMAL_AREA_START      0x5000 /* Not a real key_id , just used for region determination. */

/* Matter key_id [0x6000,0x7000) */
#define NV_ID_MATTER_AREA_START           0x6000 /* NV restore matter cert region flag */
#define NV_ID_RESTORE_ENABLE              0xA000 /* NV restore factory region flag */

#endif /* __KEY_ID_H__ */
