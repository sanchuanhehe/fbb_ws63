/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Mfg_ccpriv configuration command.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#include "wal_mfg_ccpriv.h"
#include "wifi_main.h"
#include "wal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static const wal_mfg_ccpriv_cmd_stru g_mfg_ccpriv_cmd[] = {
    {"set_xo_trim_coarse",      EXT_AT_MFG_SET_XO_TRIM_COARSE},
    {"set_xo_trim_fine",        EXT_AT_MFG_SET_XO_TRIM_FINE},
    {"get_cmu_xo_trim",         EXT_AT_MFG_GET_XO_TRIM},
    {"set_curve_factor",        EXT_AT_MFG_SET_FACTOR},
    {"get_curve_factor",        EXT_AT_MFG_GET_FACTOR},
    {"cali_power",              EXT_AT_MFG_SET_CALI_POWER},
    {"get_rssi_offset",         EXT_AT_MFG_GET_RSSI_OFFSET},
    {"set_curve_param",         EXT_AT_MFG_SET_CURVE_PARAM},
    {"get_curve_param",         EXT_AT_MFG_GET_CURVE_PARAM},
    {"set_low_curve_param",     EXT_AT_MFG_SET_LOW_CURVE_PARAM},
    {"get_low_curve_param",     EXT_AT_MFG_GET_LOW_CURVE_PARAM},
    {"set_rssi_offset",         EXT_AT_MFG_SET_RSSI_OFFSET},
    {"set_tar_power",           EXT_AT_MFG_SET_TAR_POWER},
    {"get_temp",                EXT_AT_MFG_GET_TEMP},
    {"set_efuse_rssi_offset",   EXT_AT_MFG_SET_EFUSE_RSSI_COMP},
    {"get_efuse_rssi_offset",   EXT_AT_MFG_GET_EFUSE_RSSI_COMP},
    {"efuse_write_power_info",  EXT_AT_MFG_SET_EFUSE_POWER},
    {"efuse_read_power_info",   EXT_AT_MFG_GET_EFUSE_POWER},
    {"efuse_write_cmu_xo_trim", EXT_AT_MFG_SET_EFUSE_CMU_XO_TRIM},
    {"efuse_read_cmu_xo_trim",  EXT_AT_MFG_GET_EFUSE_CMU_XO_TRIM},
    {"efuse_write_temp",        EXT_AT_MFG_SET_EFUSE_TEMP},
    {"efuse_read_temp",         EXT_AT_MFG_GET_EFUSE_TEMP},
    {"efuse_status",            EXT_AT_MFG_EFUSE_RCALDATA},
    {"efuse_remain",            EXT_AT_MFG_EFUSE_REMAIN},
};

static osal_u32 wal_mfg_get_cmd_name_by_type(osal_u32 cmd_type, osal_s8 *name, osal_u32 len)
{
    osal_u32 cmd_id;
    for (cmd_id = 0; cmd_id < uapi_array_size(g_mfg_ccpriv_cmd); cmd_id++) {
        if (cmd_type != g_mfg_ccpriv_cmd[cmd_id].type) {
            continue;
        }
        if (memcpy_s(name, len, g_mfg_ccpriv_cmd[cmd_id].cmd_name,
            osal_strlen(g_mfg_ccpriv_cmd[cmd_id].cmd_name)) != EOK) {
            oam_error_log0(0, 0, "{wal_mfg_get_cmd_name_by_type::mem safe function err!}");
            return OAL_FAIL;
        }
        return OAL_SUCC;
    }
    return OAL_FAIL;
}

static osal_u32 wal_mfg_ccpriv_process_cmd(oal_net_device_stru *netdev, osal_char *param,  osal_u32 cmd_type)
{
    osal_u32 ret;
    hmac_ccpriv_para info = {0};

    ret = wal_mfg_get_cmd_name_by_type(cmd_type, info.name, CCPRIV_CMD_NAME_MAX_LEN);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_mfg_ccpriv_process_cmd::get_cmd_name return err code[%d]!}", ret);
        return ret;
    }
    info.param = (const osal_s8 *)param;
    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CCPRIV_CMD,
        (osal_u8 *)&info, OAL_SIZEOF(info));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_mfg_ccpriv_process_cmd::parse cmd return err code[%d]!}", ret);
        return ret;
    }
    return ret;
}

osal_u32 uapi_wifi_mfg_ccpriv(osal_s32 argc, const osal_char *argv[], osal_u32 cmd_type)
{
    osal_u32 ret;
    osal_u32 param_len = 0;
    osal_char temp_buffer[3]; /* 3 存储buffer */
    osal_char *buffer = NULL;
    osal_char *buffer_temp = NULL;
    osal_char *buffer_index = NULL;
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_s32 index;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_wifi_mfg_ccpriv::netdev isn't exits.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (argc == 0) {
        temp_buffer[0] = '\0';
        buffer_temp = temp_buffer;
    } else {
        for (index = 0; index < argc; index++) {
            param_len += osal_strlen((osal_char *)argv[index]) + 1;
        }
        buffer = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (osal_u16)param_len, OAL_TRUE);
        if (buffer == NULL) {
            oam_error_log0(0, OAM_SF_ANY, "uapi_wifi_mfg_ccpriv: malloc failed!");
            return EXT_ERR_FAILURE;
        }
        buffer_index = buffer;
        for (index = 0; index < argc; index++) {
            osal_u32 len = osal_strlen((osal_char *)argv[index]);
            if (memcpy_s(buffer_index, len, argv[index], len) != EOK) {
                oam_error_log0(0, 0, "{uapi_wifi_mfg_ccpriv::mem safe function err!}");
                oal_mem_free(buffer, OAL_TRUE);
                return EXT_ERR_FAILURE;
            }
            buffer_index[len] = ' ';
            buffer_index += (len + 1);
        }
        buffer[param_len - 1] = '\0';
        buffer_temp = buffer;
    }
    ret = wal_mfg_ccpriv_process_cmd(netdev, buffer_temp, cmd_type);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_mfg_ccpriv::parse cmd return err code[%d]!}", ret);
    }
    oal_mem_free(buffer, OAL_TRUE);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif
