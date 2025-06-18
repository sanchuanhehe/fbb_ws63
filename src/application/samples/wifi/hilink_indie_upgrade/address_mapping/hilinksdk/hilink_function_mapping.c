/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "hilink_function_mapping.h"
#include "func_call.h"
#include "func_call_list.h"
#include "hilink.h"
#include "hilink_log_manage.h"
#include "ble_cfg_net_api.h"
#include "hilink_bt_function.h"
#include "hilink_custom.h"
#include "hilink_sle_api.h"
#include "hilink_quick_netcfg_api.h"
#include "hilink_quick_netcfg_adapter.h"
#include "hilink_bt_api.h"
#include "hilink_device.h"
#include "hilink_network_adapter.h"
#include "hilink_socket_adapter.h"

static const struct mapping_tbl g_hilink_call_tbl[HILINK_CALL_MAX] = {
    [HILINK_CALL_HILINK_REGISTER_BASE_CALLBACK]        = { HILINK_RegisterBaseCallback,       0xBEC62B3A },
    [HILINK_CALL_HILINK_MAIN]                          = { HILINK_Main,                       0x1B90717B },
    [HILINK_CALL_HILINK_RESET]                         = { HILINK_Reset,                      0xABD8C9AD },
    [HILINK_CALL_HILINK_SET_SDK_ATTR]                  = { HILINK_SetSdkAttr,                 0x15D3BE15 },
    [HILINK_CALL_HILINK_GET_SDK_ATTR]                  = { HILINK_GetSdkAttr,                 0x60977111 },
    [HILINK_CALL_HILINK_RESTORE_FACTORY_SETTINGS]      = { HILINK_RestoreFactorySettings,     0xA022C59C },
    [HILINK_CALL_HILINK_GET_DEV_STATUS]                = { HILINK_GetDevStatus,               0xEF399A97 },
    [HILINK_CALL_HILINK_GET_SDK_VERSION]               = { HILINK_GetSdkVersion,              0xE8B1D1D2 },
    [HILINK_CALL_HILINK_REPORT_CHAR_STATE]             = { HILINK_ReportCharState,            0xD981EE97 },
    [HILINK_CALL_HILINK_IS_REGISTER]                   = { HILINK_IsRegister,                 0x2F4C881D },
    [HILINK_CALL_HILINK_GET_NETWORKING_MODE]           = { HILINK_GetNetworkingMode,          0x0D3D30D8 },
    [HILINK_CALL_HILINK_GET_REGISTER_STATUS]           = { HILINK_GetRegisterStatus,          0x165FA46B },
    [HILINK_CALL_HILINK_SET_SCHEDULE_INTERVAL]         = { HILINK_SetScheduleInterval,        0xEB1F07B4 },
    [HILINK_CALL_HILINK_SET_MONITOR_SCHEDULE_INTERVAL] = { HILINK_SetMonitorScheduleInterval, 0xB8F1AECE },
    [HILINK_CALL_HILINK_SET_NET_CONFIG_MODE]           = { HILINK_SetNetConfigMode,           0xA4335DFC },
    [HILINK_CALL_HILINK_GET_NET_CONFIG_MODE]           = { HILINK_GetNetConfigMode,           0x7388E123 },
    [HILINK_CALL_HILINK_SET_NET_CONFIG_TIMEOUT]        = { HILINK_SetNetConfigTimeout,        0x4641EDD1 },
    [HILINK_CALL_HILINK_SET_OTA_BOOT_TIME]             = { HILINK_SetOtaBootTime,             0xAF31DFED },
    [HILINK_CALL_HILINK_ENABLE_KITFRAMEWORK]           = { HILINK_EnableKitframework,         0xB616EBF0 },
    [HILINK_CALL_HILINK_ENABLE_BATCH_CONTROL]          = { HILINK_EnableBatchControl,         0xA9900778 },
    [HILINK_CALL_HILINK_ENABLE_PROCESS_DEL_ERR_CODE]   = { HILINK_EnableProcessDelErrCode,    0xC03C4467 },
    [HILINK_CALL_HILINK_UNBIND_DEVICE]                 = { HILINK_UnbindDevice,               0xD3CBCD65 },
    [HILINK_CALL_HILINK_SET_DEVICE_INSTALL_TYPE]       = { HILINK_SetDeviceInstallType,       0x40F51D66 },
    [HILINK_CALL_HILINK_GET_DEV_SETUP_TYPE]            = { HILINK_GetDevSetupType,            0xCBE0FF99 },
    [HILINK_CALL_HILINK_ENABLE_DEV_ID_INHERIT]         = { HILINK_EnableDevIdInherit,         0x8506BA06 },
    [HILINK_CALL_HILINK_NOTIFY_NETWORK_AVAILABLE]      = { HILINK_NotifyNetworkAvailable,     0x448D8BEB },
    [HILINK_CALL_HILINK_SET_LOG_LEVEL]                 = { HILINK_SetLogLevel,                0x6CDA8E9E },
    [HILINK_CALL_HILINK_GET_LOG_LEVEL]                 = { HILINK_GetLogLevel,                0x79BE4C8B },
    [HILINK_CALL_HILINK_REGISTER_GET_AC_V2_FUNC]       = { HILINK_RegisterGetAcV2Func,        0xF15449A4 },
    [HILINK_CALL_BLE_CFG_NET_INIT]                     = { BLE_CfgNetInit,                    0xDEFEB22F },
    [HILINK_CALL_BLE_CFG_NET_DE_INIT]                  = { BLE_CfgNetDeInit,                  0x342B09F0 },
    [HILINK_CALL_BLE_CFG_NET_ADV_CTRL]                 = { BLE_CfgNetAdvCtrl,                 0x3683E17D },
    [HILINK_CALL_BLE_CFG_NET_ADV_UPDATE]               = { BLE_CfgNetAdvUpdate,               0x699DA475 },
    [HILINK_CALL_BLE_CFG_NET_DIS_CONNECT]              = { BLE_CfgNetDisConnect,              0xD08A2F07 },
    [HILINK_CALL_BLE_SEND_CUSTOM_DATA]                 = { BLE_SendCustomData,                0x9EDA59B9 },
    [HILINK_CALL_BLE_GET_ADV_TYPE]                     = { BLE_GetAdvType,                    0x9A96CD0E },
    [HILINK_CALL_BLE_SET_ADV_TYPE]                     = { BLE_SetAdvType,                    0x468B0892 },
    [HILINK_CALL_BLE_SET_ADV_NAME_MPP]                 = { BLE_SetAdvNameMpp,                 0x24120EE9 },
    [HILINK_CALL_BLE_NEAR_DISCOVERY_INIT]              = { BLE_NearDiscoveryInit,             0x6B86A8BB },
    [HILINK_CALL_BLE_NEAR_DISCOVERY_ENABLE]            = { BLE_NearDiscoveryEnable,           0x4B250B35 },
    [HILINK_CALL_HILINK_BT_GET_TASK_STACK_SIZE]        = { HILINK_BT_GetTaskStackSize,        0xE0E8B8A1 },
    [HILINK_CALL_HILINK_BT_SET_TASK_STACK_SIZE]        = { HILINK_BT_SetTaskStackSize,        0xE77FE8A1 },
    [HILINK_CALL_HILINK_BT_SET_SDK_EVENT_CALLBACK]     = { HILINK_BT_SetSdkEventCallback,     0x3F50CF8B },
    [HILINK_CALL_HILINK_BT_HARD_REVOKE]                = { HILINK_BT_HardRevoke,              0x018FA5A9 },
    [HILINK_CALL_HILINK_REG_WI_FI_RECOVERY_CALLBACK]   = { HILINK_RegWiFiRecoveryCallback,    0x1475DDA9 },
    [HILINK_CALL_HILINK_REGISTER_ERRNO_CALLBACK]       = { HILINK_RegisterErrnoCallback,      0x32141DAF },
    [HILINK_CALL_HILINK_SET_PROT_TYPE]                 = { HILINK_SetProtType,                0x1F5C4D9C },
    [HILINK_CALL_HILINK_ENABLE_PRESCAN]                = { HILINK_EnablePrescan,              0x5D7F8DC8 },
    [HILINK_CALL_HILINK_SET_NET_CONFIG_INFO]           = { HILINK_SetNetConfigInfo,           0xE3CB582D },
    [HILINK_CALL_HILINK_SET_SOFT_APMODE]               = { HILINK_SetSoftAPMode,              0x0DB0EA88 },
    [HILINK_CALL_HILINK_REQUEST_REG_INFO]              = { HILINK_RequestRegInfo,             0x23396BF6 },
    [HILINK_CALL_HILINK_DIAGNOSIS_INFO_RECORD]         = { HILINK_DiagnosisInfoRecord,        0xBC0A30D6 },
    [HILINK_CALL_HILINK_ENABLE_SLE]                    = { HILINK_EnableSle,                  0x8B968BEE },
    [HILINK_CALL_HILINK_SET_QUICK_CFG_COMMON_LOADER]   = { HILINK_SetQuickCfgCommonLoader,    0x77AB6014 },
    [HILINK_CALL_HILINK_START_QUICK_CFG]               = { HILINK_StartQuickCfg,              0x036385A6 },
    [HILINK_CALL_HILINK_FRAME_PARSE]                   = { HILINK_FrameParse,                 0x8BB35701 },
    [HILINK_CALL_HILINK_QUICK_CFG_CMD_PARSE]           = { HILINK_QuickCfgCmdParse,           0xD274404A },
    [HILINK_CALL_HILINK_SET_DEVICE_TYPE]               = { HILINK_SetDeviceType,              0x9BFA2434 },
    [HILINK_CALL_HILINK_SET_QUICK_CFG_WIFI_LOADER]     = { HILINK_SetQuickCfgWifiLoader,      0x8E7321F6 },
    [HILINK_CALL_HILINK_ENABLE_QUICK_NET_CFG]          = { HILINK_EnableQuickNetCfg,          0x3FEA98B7 },
};

const struct mapping_tbl *get_hilink_mapping_tbl(void)
{
    return g_hilink_call_tbl;
}

unsigned int get_hilink_mapping_tbl_size(void)
{
    return HILINK_CALL_MAX;
}
