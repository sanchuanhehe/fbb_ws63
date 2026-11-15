/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell source
 *
 * History:
 * 2025-5-26, Create file.
 */


#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "mesh/main.h"
#include "mesh/access.h"
#include "bts_def.h"
#include "model_opcode.h"
#include "client_common.h"
#include "generic_client.h"
#include "priv_beacon_cli.h"
#include "device_property.h"
#include "mesh/cfg_cli.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_BEACON_CONFIG)
/* AT+BM_CT=set_beacon,beacon,0 */
int cmd_cfg_set_beacon(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "beacon") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 1); // 1 2: index, 0 1:range

    bool beacon = atoi(argv[2]); // 2 : index
    bt_mesh_beacon_set(beacon);
    BT_INFO_SHELL("set beacon state: %u", beacon);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_beacon */
int cmd_cfg_get_beacon(int argc, const char *argv[])
{
    BT_INFO_SHELL("cmd_cfg_get_beacon begin");
    bool beacon = bt_mesh_beacon_enabled();
    BT_INFO_SHELL("get beacon state: %u", beacon);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=set_priv_beacon,priv_beacon,0 */
int cmd_cfg_set_priv_beacon(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "priv_beacon") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 1); // 1 2: index, 0 1:range

    bool priv_beacon = atoi(argv[2]); // 2 : index
    bt_mesh_priv_beacon_set(priv_beacon);
    BT_INFO_SHELL("set priv beacon state: %u", priv_beacon);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_priv_beacon */
int cmd_cfg_get_priv_beacon(int argc, const char *argv[])
{
    BT_INFO_SHELL("cmd_cfg_get_beacon begin");
    enum bt_mesh_feat_state priv_beacon = bt_mesh_priv_beacon_get();
    BT_INFO_SHELL("get beacon state: %u", priv_beacon);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=set_priv_beacon_interval,interval,60 */
int cmd_cfg_set_priv_beacon_interval(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "interval") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 255); // 1 2: index, 0 255:range

    uint8_t interval = 0;
    SHELL_PARAM_UINT(interval, argv[2]);  // 2:index
    bt_mesh_priv_beacon_update_interval_set(interval);
    BT_INFO_SHELL("set priv beacon interval: %u", interval);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_priv_beacon_interval */
int cmd_cfg_get_priv_beacon_interval(int argc, const char *argv[])
{
    BT_INFO_SHELL("cmd_cfg_get_beacon begin");
    uint8_t interval = bt_mesh_priv_beacon_update_interval_get();
    BT_INFO_SHELL("get beacon interval: %u", interval);

    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif
