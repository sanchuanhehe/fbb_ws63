/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: shell source
 *
 * History:
 * 2025-6-12, Create file.
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
#include "model_common.h"
#include "device_property.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_hex.h"
#if defined(CONFIG_BLE_MESH_PROVISIONER_ADV) && defined(CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
#include "ble_mesh_common.h"
#include "ble_mesh_sample.h"
#endif
#include "ble_mesh_prov.h"
#include "ble_mesh_hex.h"
#include "ble_mesh_nv_op.h"
#include "remote_prov_client.h"
#include "remote_prov_server.h"

#define EXPECTED_UUID_ARGC_LEN 32
// 远程配网
#if MYNEWT_VAL(BLE_MESH_REMOTE_PROVISION)

#define EXPECTED_SCAN_CAPABILITIES_ARGC_SIZE 9
#define EXPECTED_SCAN_GET_ARGC_SIZE 9
#define EXPECTED_SCAN_START_ARGC_SIZE 15
#define EXPECTED_EX_SCAN_START_ARGC_SIZE 17
#define EXPECTED_SCAN_STOP_ARGC_SIZE 9
#define EXPECTED_LINK_GET_ARGC_SIZE 9
#define EXPECTED_LINK_CLOSE_ARGC_SIZE 9
#define EXPECTED_LINK_OPEN_ARGC_SIZE 15

// AT+REMOTEPROV=scan_capabilities_get,app,0,net,0,dst,2,ttl,7
int cmd_cfg_remote_prov_cli_scan_capabilities_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    if (argc != EXPECTED_SCAN_CAPABILITIES_ARGC_SIZE) {  // 11:size
        BT_ERR("cmd_cfg_remote_prov_cli_scan_capabilities_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0)) {                                   // 7:ttl
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl

    struct bt_mesh_rmt_prov_abilities_sts_t rsp = {0};

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    // 获取远程配网server扫描能力
    int ret = bt_mesh_rmt_prov_scan_abilities_get((const struct bt_mesh_model *)common.model, &srv_node, &rsp);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Remote Provisioning scan get Capabilities error!");
        return -EINVAL;
    }

    BT_INFO_SHELL("Remote Provisioning scan on 0x%04x:", common.ctx.addr);
    BT_INFO_SHELL("\tmax_scanned_items:         %u", rsp.max_scanned_items);
    if (rsp.active_scan) {
        BT_INFO_SHELL("\tactive scan is supported:");
    } else {
        BT_INFO_SHELL("\tactive scan is not supported:");
    }

    return ret;
}

// AT+REMOTEPROV=scan_get,app,0,net,0,dst,2,ttl,7
int cmd_cfg_remote_prov_cli_scan_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    if (argc != EXPECTED_SCAN_GET_ARGC_SIZE) {  // 9:size
        BT_ERR("cmd_cfg_remote_prov_cli_scan_capabilities_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0)) {                                   // 7:ttl
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl

    struct bt_mesh_rmt_prov_scan_sts_t rsp = {0};

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    // 获取远程配网server扫描能力
    int ret = bt_mesh_rmt_prov_scan_get((const struct bt_mesh_model *)common.model, &srv_node, &rsp);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Remote Provisioning scan get error!");
        return -EINVAL;
    }

    BT_INFO_SHELL("Remote Provisioning scan on 0x%04x:", common.ctx.addr);
    BT_INFO_SHELL("\tStatus:         %u", rsp.req_sts);
    BT_INFO_SHELL("\tScan state:      %u", rsp.scan_sts);
    BT_INFO_SHELL("\tMax devices:    %u", rsp.max_report_dev);
    BT_INFO_SHELL("\tRemaining time: %u", rsp.timeout);
    return ret;
}

__attribute__((weak)) int ble_mesh_sample_rmt_prov_scan_start(
    uint16_t net_idx, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t items_limit, uint8_t timeout)
{
    BT_INFO_SHELL("rmt scan start not implement");
    return ERRCODE_BLE_MESH_SUCCESS;
}

// AT+REMOTEPROV=scan_start,app,0,net,0,dst,2,ttl,7,timeout,xxx,uuid,xxx,maxScanItems,xxx
static int cmd_cfg_remote_prov_cli_scan_start(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // 远程配网server扫描能力
    struct bt_mesh_rmt_prov_scan_sts_t rsp = {0};
    uint8_t timeout;
    uint8_t uuid[16] = {0};
    uint8_t max_scan_items;

    if (argc != EXPECTED_SCAN_START_ARGC_SIZE) {  // 15:size
        BT_ERR("cmd_cfg_remote_prov_cli_scan_start:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||        // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0) ||                                         // 7:ttl
        (strcmp(argv[9], "timeout") != 0) || (strcmp(argv[11], "uuid") != 0) ||  // 9:timeout 11:uuid
        (strcmp(argv[13], "scan_items") != 0)) {                               // 13:scan items
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl
    SHELL_PARAM_UINT(timeout, argv[10]);             // 10:timeout
    SHELL_PARAM_UINT(max_scan_items, argv[14]);      // 14:max_scan_items

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    // client scan start
    int ret = 0;
    if (strlen(argv[12]) == EXPECTED_UUID_ARGC_LEN) {          // 12:uuid
        if (ble_mesh_hex2bin(argv[12], uuid, BLE_MESH_UUID_LEN) < 0) {  // 12:uuid
            BT_INFO_SHELL("wrong params");
            return ERRCODE_BLE_MESH_FAIL;
        }

        ret = ble_mesh_sample_rmt_prov_scan_start(common.ctx.net_idx, common.ctx.addr, uuid, max_scan_items, timeout);
    } else {
        ret = ble_mesh_sample_rmt_prov_scan_start(common.ctx.net_idx, common.ctx.addr, NULL, max_scan_items, timeout);
    }
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Remote Provisioning Scan start failed!");
        return -EINVAL;
    }

    return ret;
}

__attribute__((weak)) int ble_mesh_sample_rmt_prov_ex_scan_start(
    uint16_t net_idx, uint16_t addr, const uint8_t *uuid, uint8_t timeout, void *ad_filter)
{
    BT_INFO_SHELL("rmt ex scan start not implement");
    return ERRCODE_BLE_MESH_SUCCESS;
}

// AT+REMOTEPROV=ex_scan_start,app,0,net,0,dst,2,ttl,7,timeout,xxx,uuid,xxx,ad_count,xxx,ad_type,xxx
int cmd_cfg_remote_prov_cli_ex_scan_start(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // 远程配网server扫描能力
    ble_mesh_rmt_prov_ex_scan_t ad_filter = {0};
    uint8_t timeout;
    uint8_t *uuid = NULL;
    uint8_t uuid_tmp[BLE_MESH_UUID_LEN] = {0};

    if (argc != EXPECTED_EX_SCAN_START_ARGC_SIZE) {
        BT_ERR("cmd_cfg_remote_prov_cli_ex_scan_start:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||             // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0) ||                                              // 7:ttl
        (strcmp(argv[9], "timeout") != 0) || (strcmp(argv[11], "uuid") != 0) ||       // 9:timeout 11:uuid
        (strcmp(argv[13], "ad_count") != 0) || (strcmp(argv[15], "ad_type") != 0)) {  // 13:ad count 15:ad type
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);             // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);                // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);            // 8:ttl
    SHELL_PARAM_UINT(timeout, argv[10]);                       // 10:timeout
    SHELL_PARAM_UINT(ad_filter.ad_count, argv[14]);            // 14:ad count
    if (ble_mesh_hex2bin(argv[16], ad_filter.ad_type, ad_filter.ad_count) < 0) {  // 16:ad type
        BT_INFO_SHELL("wrong params");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    // client scan start
    int ret = 0;
    if (strlen(argv[12]) == EXPECTED_UUID_ARGC_LEN) {                       // 12:uuid
        if (ble_mesh_hex2bin(argv[12], uuid_tmp, BLE_MESH_UUID_LEN) < 0) {  // 12:uuid
            BT_INFO_SHELL("wrong params");
            return ERRCODE_BLE_MESH_FAIL;
        }
        uuid = uuid_tmp;
    }
    ret = ble_mesh_sample_rmt_prov_ex_scan_start(common.ctx.net_idx, common.ctx.addr, uuid, timeout, &ad_filter);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Remote Provisioning Scan start failed!");
        return -EINVAL;
    }

    return ret;
}

// AT+REMOTEPROV=scan_stop,app,0,net,0,dst,2,ttl,7
static int cmd_cfg_remote_prov_cli_scan_stop(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // 远程配网server扫描能力
    struct bt_mesh_rmt_prov_scan_sts_t rsp = {0};

    if (argc != EXPECTED_SCAN_STOP_ARGC_SIZE) {  // 9:size
        BT_ERR("cmd_cfg_remote_prov_cli_scan_stop:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0)) {                                   // 7:ttl
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    // client scan stop
    int ret = bt_mesh_rmt_prov_scan_stop((const struct bt_mesh_model *)common.model, &srv_node, &rsp);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Scan stop failed: %d", ret);
        return -EINVAL;
    }

    BT_INFO_SHELL("Remote Provisioning scan on 0x%04x stopped.", common.ctx.addr);
    BT_INFO_SHELL("Remote Provisioning stop response: %d", rsp.req_sts);
    return ret;
}

// AT+REMOTEPROV=link_get,app,0,net,0,dst,2,ttl,7
int cmd_cfg_remote_prov_cli_link_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // link get
    struct bt_mesh_rmt_prov_link_t rsp = {0};

    if (argc != EXPECTED_LINK_GET_ARGC_SIZE) {  // 9:size
        BT_ERR("cmd_cfg_remote_prov_cli_link_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0)) {                                   // 7:ttl
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    int ret = bt_mesh_rmt_prov_link_get(common.model, &srv_node, &rsp);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Link get failed: %d", ret);
        return -EINVAL;
    }

    BT_INFO_SHELL("Remote Provisioning Link on 0x%04x:", common.ctx.addr);
    BT_INFO_SHELL("\tStatus: %u", rsp.sts);
    BT_INFO_SHELL("\tState:  %u", rsp.state);

    return ret;
}

// AT+REMOTEPROV=link_close,app,0,net,0,dst,2,ttl,7
int cmd_cfg_remote_prov_cli_link_close(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // link close
    struct bt_mesh_rmt_prov_link_t rsp = {0};

    if (argc != EXPECTED_LINK_CLOSE_ARGC_SIZE) {  // 9:size
        BT_ERR("cmd_cfg_remote_prov_cli_link_close:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||  // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0)) {                                   // 7:ttl
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl

    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = common.ctx.addr, .net_idx = common.ctx.net_idx, .ttl = common.ctx.send_ttl};

    int ret = bt_mesh_rmt_prov_link_close(common.model, &srv_node, &rsp);
    if (ret != BLE_MESH_SUCC) {
        BT_ERR("Remote Provisioning link close failed: %d", ret);
        return -EINVAL;
    }

    BT_INFO_SHELL("Remote Provisioning Link close on 0x%04x:", common.ctx.addr);
    BT_INFO_SHELL("\tStatus: %u", rsp.sts);
    BT_INFO_SHELL("\tState:  %u", rsp.state);
    return ret;
}

__attribute__((weak)) int ble_mesh_sample_rmt_prov_link_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t rmt_addr, uint8_t nppi)
{
    BT_INFO_SHELL("rmt link open not implement");
    return ERRCODE_BLE_MESH_SUCCESS;
}

// AT+REMOTEPROV=link_open,app,0,net,0,dst,2,ttl,7,uuid,xxxx,timeout,xxxx,nodeRefeshType,0
int cmd_cfg_remote_prov_cli_link_open(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_rmt_prov_cli_model()  // 配网client model
    };

    // link open
    uint8_t uuid[16] = {0};
    uint8_t timeout;
    uint8_t nppi;

    if (argc != EXPECTED_LINK_OPEN_ARGC_SIZE) {  // 15:size
        BT_ERR("cmd_cfg_remote_prov_cli_link_open:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[3], "net") != 0) || (strcmp(argv[5], "dst") != 0) ||        // 3:net 5:dst
        (strcmp(argv[7], "ttl") != 0) ||                                         // 7:ttl
        (strcmp(argv[9], "uuid") != 0) || (strcmp(argv[11], "timeout") != 0) ||  // 9:uuid 11:timeout
        (strcmp(argv[13], "node_refesh_type") != 0)) {                             // 13:nppi
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(common.ctx.net_idx, argv[4]);   // 4:net_id
    SHELL_PARAM_UINT(common.ctx.addr, argv[6]);      // 6:addr
    SHELL_PARAM_UINT(common.ctx.send_ttl, argv[8]);  // 8:ttl
    SHELL_PARAM_UINT(timeout, argv[12]);             // 12:timeout
    SHELL_PARAM_UINT(nppi, argv[14]);                // 14:nppi

    int ret = 1;
    if (strlen(argv[10]) == EXPECTED_UUID_ARGC_LEN) {          // 10:uuid
        if (ble_mesh_hex2bin(argv[10], uuid, BLE_MESH_UUID_LEN) < 0) {  // 10:uuid
            BT_INFO_SHELL("wrong params");
            return ERRCODE_BLE_MESH_FAIL;
        }
        ret = ble_mesh_sample_rmt_prov_link_open(uuid, common.ctx.net_idx, common.ctx.addr, nppi);
    } else {
        ret = ble_mesh_sample_rmt_prov_link_open(NULL, common.ctx.net_idx, common.ctx.addr, nppi);
    }
    if (ret != 0) {
        BT_INFO_SHELL("Link open failed: %d", ret);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}
#else
// AT+REMOTEPROV=srv_scan_state_get
static int cmd_cfg_remote_prov_srv_scan_state_get(int argc, const char *argv[])
{
    if (argc != 1) {  // 1:size
        BT_ERR("cmd_cfg_remote_prov_srv_scan_state_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[0], "srv_scan_state_get") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_rmt_prov_scan_state_get();
    return ERRCODE_BLE_MESH_SUCCESS;
}

// AT+REMOTEPROV=srv_link_state_get
static int cmd_cfg_remote_prov_srv_link_state_get(int argc, const char *argv[])
{
    if (argc != 1) {  // 1:size
        BT_ERR("cmd_cfg_remote_prov_srv_link_state_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[0], "srv_link_state_get") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_rmt_prov_link_state_get();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int bt_mesh_comp_set_128(void)
{
    struct os_mbuf *buf = NET_BUF_SIMPLE(BT_MESH_TX_SDU_MAX);
    if (!buf) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_comp_get_page_0(buf);
    buf->om_data[4] = 0x7d;  // 4:vid
    buf->om_data[5] = 0x02;  // 5:vid
    ble_mesh_data_write(BTH_BLE_MESH_COMP_DATA_128, buf->om_data, buf->om_len);
    os_mbuf_free_chain(buf);

    return SUCCESS;
}

// AT+REMOTEPROV=composition_refresh
static int cmd_cfg_remote_prov_composition_refresh(int argc, const char *argv[])
{
    int ret = 0;
    if (argc != 1) {  // 1:size
        BT_ERR("cmd_cfg_remote_prov_srv_link_state_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[0], "composition_refresh") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    atomic_set_bit(bt_mesh.flags, BT_MESH_COMP_DIRTY);
    ret = bt_mesh_comp_set_128();
    if (ret != 0) {
        BT_INFO_SHELL("prov composition refresh set failed: %d", ret);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

// AT+REMOTEPROV=srv_scan_start,net,0,addr,2,timeout,xxx,uuid,xxx,scan_items,xxx
static uint32_t cmd_cfg_remote_prov_srv_scan_start(int argc, const char *argv[])
{
    uint16_t addr = 0;
    uint16_t net = 0;
    // 远程配网server扫描能力
    uint8_t timeout;
    uint8_t uuid[16] = {0};
    uint8_t max_scan_items;
    errcode_t ret = 0;
    if (argc != 11) {  // 11: para num
        BT_ERR("cmd_cfg_remote_prov_cli_scan_start:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net") != 0) || (strcmp(argv[3], "addr") != 0) ||        // 1:net 3:dst para index
        (strcmp(argv[5], "timeout") != 0) || (strcmp(argv[7], "uuid") != 0) ||  // 5:timeout 7:uuid
        (strcmp(argv[9], "scan_items") != 0)) {                               // 9:scan items
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(net, argv[2]);                 // 2:net_id
    SHELL_PARAM_UINT(addr, argv[4]);                // 4:addr
    SHELL_PARAM_UINT(timeout, argv[6]);             // 6:timeout
    SHELL_PARAM_UINT(max_scan_items, argv[10]);      // 10:max_scan_items

    if (strlen(argv[8]) == EXPECTED_UUID_ARGC_LEN) {          // 8:uuid
        if (ble_mesh_hex2bin(argv[8], uuid, BLE_MESH_UUID_LEN) < 0) {  // 8:uuid
            BT_INFO_SHELL("wrong params");
            return ERRCODE_BLE_MESH_FAIL;
        }
    }
    ret = ble_mesh_pb_start_rmt_scan_local(net, addr, uuid, max_scan_items, timeout);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Remote Provisioning Scan start failed!");
        return ret;
    }
    BT_INFO_SHELL("Remote Provisioning scan start success.");
    return ret;
}

// AT+REMOTEPROV=srv_scan_stop
static uint32_t cmd_cfg_remote_prov_srv_scan_stop(int argc, const char *argv[])
{
    if (argc != 1) {  // 1:size
        BT_ERR("cmd_cfg_remote_prov_cli_scan_stop:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    // client scan stop
    errcode_t ret = ble_mesh_pb_stop_rmt_scan_local();
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Scan stop failed: %d", ret);
        return ret;
    }

    BT_INFO_SHELL("Remote Provisioning scan stopped success.");
    return ret;
}
#endif

uint32_t cmd_ble_mesh_remote_provision_op_func(int argc, const char *argv[])
{
    if (argc < 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    static at_cmd cfg_cmd_remote_prov_tbl[] = {
#if MYNEWT_VAL(BLE_MESH_REMOTE_PROVISION)
        {"scan_capabilities_get", cmd_cfg_remote_prov_cli_scan_capabilities_get},
        {"scan_get", cmd_cfg_remote_prov_cli_scan_get},
        {"scan_start", cmd_cfg_remote_prov_cli_scan_start},
        {"ex_scan_start", cmd_cfg_remote_prov_cli_ex_scan_start},
        {"scan_stop", cmd_cfg_remote_prov_cli_scan_stop},
        {"link_get", cmd_cfg_remote_prov_cli_link_get},
        {"link_close", cmd_cfg_remote_prov_cli_link_close},
        {"link_open", cmd_cfg_remote_prov_cli_link_open},
#else
        {"srv_scan_state_get", cmd_cfg_remote_prov_srv_scan_state_get},
        {"srv_link_state_get", cmd_cfg_remote_prov_srv_link_state_get},
        {"srv_scan_start", cmd_cfg_remote_prov_srv_scan_start},
        {"srv_scan_stop", cmd_cfg_remote_prov_srv_scan_stop},
#ifdef CONFIG_BLE_MESH_PTS_MODE
        {"composition_refresh", cmd_cfg_remote_prov_composition_refresh},
#endif    // CONFIG_BLE_MESH_PTS_MODE
#endif
    };

    int cfg_cmd_tbl_num = (sizeof(cfg_cmd_remote_prov_tbl) / sizeof(at_cmd));
    for (int i = 0; i < cfg_cmd_tbl_num; i++) {
        if (strcmp(argv[0], cfg_cmd_remote_prov_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return cfg_cmd_remote_prov_tbl[i].func(argc, argv);
        }
    }

    BT_INFO_SHELL("invalid cmd:%s", argv[0]);

    return ERRCODE_BLE_MESH_FAIL;
}

// 远程配网
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
} ble_mesh_remote_provision_t;

at_ret_t at_bt_mesh_remote_provision(const ble_mesh_remote_provision_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_remote_prov args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[22] = {NULL};
    int argc = 0;
    // 进行参数处理
    ASSIGN_PARA(argv, args->para1, argc);
    ASSIGN_PARA(argv, args->para2, argc);
    ASSIGN_PARA(argv, args->para3, argc);
    ASSIGN_PARA(argv, args->para4, argc);
    ASSIGN_PARA(argv, args->para5, argc);
    ASSIGN_PARA(argv, args->para6, argc);
    ASSIGN_PARA(argv, args->para7, argc);
    ASSIGN_PARA(argv, args->para8, argc);
    ASSIGN_PARA(argv, args->para9, argc);
    ASSIGN_PARA(argv, args->para10, argc);
    ASSIGN_PARA(argv, args->para11, argc);
    ASSIGN_PARA(argv, args->para12, argc);
    ASSIGN_PARA(argv, args->para13, argc);
    ASSIGN_PARA(argv, args->para14, argc);
    ASSIGN_PARA(argv, args->para15, argc);
    ASSIGN_PARA(argv, args->para16, argc);
    ASSIGN_PARA(argv, args->para17, argc);
    ASSIGN_PARA(argv, args->para18, argc);
    ASSIGN_PARA(argv, args->para19, argc);
    ASSIGN_PARA(argv, args->para20, argc);
    uint32_t ret = cmd_ble_mesh_remote_provision_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_remote_provision_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_remote_provision_params[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_remote_provision_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_remote_provision_t, para20)}};

const at_cmd_entry_t at_bt_mesh_rmt_prov_table[] = {{
    "REMOTEPROV",  // 远程配网
    0,
    0,
    ble_mesh_remote_provision_params,            // 参数类型
    NULL,                                        // 无参数时调用的函数
    (at_set_func_t)at_bt_mesh_remote_provision,  // 有参数时调用的函数
    NULL,
    NULL,
}};

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_rmt_prov_table) / sizeof(at_bt_mesh_rmt_prov_table[0]))

void mesh_at_rmt_prov_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_rmt_prov_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("at_bt_mesh_rmt_prov_table ret = %d", ret);
}
