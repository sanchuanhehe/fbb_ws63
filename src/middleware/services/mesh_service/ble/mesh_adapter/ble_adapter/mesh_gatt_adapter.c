/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: gatt adapter source
 *
 */
#include "securec.h"
#include "mesh/glue.h"
#include "host/ble_gap.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "mesh/glue.h"
#include "mesh/porting.h"
#include "mesh/slist.h"
#include "osal_task.h"
#include "osal_wait.h"
#include "soc_errno.h"
#include "bts_def.h"
#include "mesh_common.h"
#include "os_sem.h"
#include "pb_gatt_srv.h"
#include "mesh_gap_adapter.h"
#include "mesh_gatt_adapter.h"

#define BLE_MESH_GATT_SERVICE_MIN_HANDLE 2

static const struct ble_gatt_svc_def *g_svc_defs = NULL;
static uint16_t g_svc_handle = 0xFFFF;
// Mesh 服务ID
static uint8_t g_mesh_server_id = 0;
// Proxy服务句柄，对应UUID BT_UUID_MESH_PROXY(1828)
static uint16_t g_proxy_svc_handle = 0;
// Proxy服务接收数据写入char句柄，对应UUID BT_UUID_MESH_PROXY_DATA_IN_VAL(0x2add)
static uint16_t g_proxy_data_in_h;
// Proxy服务接收数据通知句柄，对应UUID BT_UUID_MESH_PROXY_DATA_OUT_VAL(0x2ade)
static uint16_t g_proxy_data_out_h;
// Proxy服务CCCD句柄
static uint16_t g_proxy_config_h;

// Prov服务句柄，对应UUID BT_UUID_MESH_PROV(1827)
static uint16_t g_prov_svc_handle = 0;
// Prov服务接收数据写入char句柄，对应UUID BT_UUID_MESH_PROV_DATA_IN(0x2adb)
static uint16_t g_prov_data_in_h;
// Prov服务接收数据通知句柄，对应UUID BT_UUID_MESH_PROXY_DATA_OUT_VAL(0x2adc)
static uint16_t g_prov_data_out_h;
// Prov服务CCCD句柄
static uint16_t g_prov_config_h;
// 服务初始数据
static uint8_t g_char_value[] = {0x00, 0x00};
// CCCD初始值
static uint8_t g_ccc_val[] = {0x00, 0x00};

static ble_uuid16_t g_gatt_ccc                  = BLE_UUID16_INIT(0x2902);

static sys_slist_t bt_mesh_gatts_db;
#define GATTS_READ_BUF_LEN 100
#define BLE_MESH_GATT_CCC_NOTIFY 0x0001

// 定义osal task用于服务变更线性处理
#define SERVICE_CHANGE_TASK_STACK_SIZE 0x400
#define SERVICE_CHANGE_TASK_PRIORITY_NUM 9
#define SERVICE_QUEUE_MAX_SIZE 5
#define OSAL_INVALID_MSG_NUM 0xFFFFFFFF

// 信号量，用于start 和 stop service串行等待
struct ble_npl_sem g_change_service_sem = {0};
static osal_task *g_service_change_task_hdl = NULL;

// 服务变更结构
typedef struct {
    uint8_t  server_id;
    uint8_t  type;
    uint16_t handle;
} ble_mesh_service_event_t;
// 任务句柄
static const char *SERVICE_CHANGE_TASK_NAME = "mesh_srv_change";
// 任务队列ID
static unsigned long g_service_change_queue_id = OSAL_INVALID_MSG_NUM;
// 当前正在操作的服务句柄，用于收到服务变更回调后的信号释放
static uint16_t g_cur_handle = 0xFFFF;

// 服务注册标记
static bool g_mesh_server_registered = false;

#define BLE_MESH_DEFAULT_MTU 23
uint16_t g_mtu_value = BLE_MESH_DEFAULT_MTU;
static struct bt_mesh_conn_cb *bt_mesh_gatts_conn_cb = NULL;

static uint32_t get_perm(const struct ble_gatt_chr_def *chr_def)
{
    uint32_t perm = 0;
    if ((chr_def->flags & BLE_GATT_CHR_F_WRITE_NO_RSP) || (chr_def->flags & BLE_GATT_CHR_F_WRITE)) {
        perm |= GATT_ATTRIBUTE_PERMISSION_WRITE;
    }
    return perm;
}

uint16_t ble_mesh_mtu(void)
{
    return g_mtu_value;
}

static void bt_mesh_mtu_change_cb(int conn_id, int mtu)
{
    BT_DBG("conn_id %d, mtu:%d", conn_id, mtu);
    if (bt_mesh_gatts_conn_cb != NULL && bt_mesh_gatts_conn_cb->gatt_mtu_change != NULL) {
        (bt_mesh_gatts_conn_cb->gatt_mtu_change)(conn_id, mtu);
    }
}

static void ble_mesh_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    bt_mesh_mtu_change_cb(conn_id, mtu_size);
    g_mtu_value = mtu_size;
    MESH_UNUSED(server_id);
    MESH_UNUSED(status);
}

static void ble_mesh_service_add_cb(int status, int server_id, bt_uuid_t *uuid, int srvc_handle)
{
    if (uuid != NULL) {
        BT_INFO("uuid len %d uuid[0][1]:0x%x%x", uuid->uuid_len, uuid->uuid[0], uuid->uuid[1]);
    }
    BT_INFO("status %d server_id %d srvc_handle %d", status, server_id, srvc_handle);
    return;
}

static void ble_mesh_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    BT_DBG("[debug] ble_mesh_server_service_add_cbk\n");
    ble_mesh_service_add_cb((int)status, (int)server_id, uuid, (int)handle);
    g_svc_handle = handle;
}

static void ble_mesh_nimuuid_2_btuuid(bt_uuid_t *out, const ble_uuid_t *uuid)
{
    if (uuid->type == BLE_UUID_TYPE_16) {
        out->uuid_len = LEN_UUID_16;
        (void)memcpy_s(out->uuid, LEN_UUID_16, &(BLE_UUID16(uuid)->value), LEN_UUID_16);
    } else if (uuid->type == BLE_UUID_TYPE_32) {
        out->uuid_len = LEN_UUID_32;
        (void)memcpy_s(out->uuid, LEN_UUID_32, &(BLE_UUID32(uuid)->value), LEN_UUID_32);
    } else if (uuid->type == BLE_UUID_TYPE_128) {
        out->uuid_len = LEN_UUID_128;
        (void)memcpy_s(out->uuid, LEN_UUID_128, (BLE_UUID128(uuid)->value), LEN_UUID_128);
    }
}

static void ble_mesh_char_add_cb(int status, int server_id, bt_uuid_t *uuid, int srvc_handle, int characteristic_handle,
    gatts_add_character_result_t *result)
{
    BT_INFO(
        "status %d, server_id %d srvc_handle 0x%x char_handle 0x%x handle 0x%x value_handle 0x%x",
        status,
        server_id,
        srvc_handle,
        characteristic_handle,
        result->handle,
        result->value_handle);
    if (uuid != NULL) {
        BT_INFO("uuid len %d uuid[0][1]:0x%x%x", uuid->uuid_len, uuid->uuid[0], uuid->uuid[1]);
    }
}

static void ble_mesh_server_characteristic_add_cbk(
    uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle, gatts_add_character_result_t *result, errcode_t status)
{
    ble_mesh_char_add_cb(status, server_id, uuid, service_handle, 0, result);
}

static int ble_mesh_gatts_add_characteristic(
    uint8_t server_id, uint16_t service_handle, gatts_add_chara_info_t *character, gatts_add_character_result_t *result)
{
    uint8_t tmp = character->chara_uuid.uuid[0];
    character->chara_uuid.uuid[0] = character->chara_uuid.uuid[1];
    character->chara_uuid.uuid[1] = tmp;
    errcode_t ret = gatts_add_characteristic_sync(server_id, service_handle, character, result);
    if (character->chara_uuid.uuid[1] == 0xDD) {
        g_proxy_data_in_h = result->value_handle;
    } else if (character->chara_uuid.uuid[1] == 0xDB) {
        g_prov_data_in_h = result->value_handle;
    } else if (character->chara_uuid.uuid[1] == 0xDE) {
        g_proxy_data_out_h = result->value_handle;
    } else if (character->chara_uuid.uuid[1] == 0xDC) {
        g_prov_data_out_h = result->value_handle;
    }

    if (ret != 0) {
        BT_ERR("[ERROR] err gatts_add_characteristic err ret = 0x%x", ret);
        return -1;
    }
    BT_INFO("character->chara_uuid.uuid[0][1] 0x%x%x result->value_handle = 0x%x",
        character->chara_uuid.uuid[0], character->chara_uuid.uuid[1], result->value_handle);
    return 0;
}

static void bt_mesh_descriptor_add_cb(
    int status, int server_id, bt_uuid_t *uuid, int srvc_handle, int descriptor_handle)
{
    BT_INFO(
        "status %d server_id %d, srvc_handle 0x%x, des_handle 0x%x", status, server_id, srvc_handle, descriptor_handle);
    if (uuid != NULL) {
        BT_INFO("uuid len %d uuid[0][1]:0x%x%x", uuid->uuid_len, uuid->uuid[0], uuid->uuid[1]);
    }
}

static void ble_mesh_server_descriptor_add_cbk(
    uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle, uint16_t handle, errcode_t status)
{
    bt_mesh_descriptor_add_cb(status, server_id, uuid, service_handle, handle);
}

static int ble_mesh_gatts_add_descriptor(uint8_t server_id, uint16_t service_handle, gatts_add_desc_info_t *descriptor)
{
    uint8_t tmp = descriptor->desc_uuid.uuid[0];
    descriptor->desc_uuid.uuid[0] = descriptor->desc_uuid.uuid[1];
    descriptor->desc_uuid.uuid[1] = tmp;
    uint16_t handle = 0;
    errcode_t ret = gatts_add_descriptor_sync(server_id, service_handle, descriptor, &handle);

    if (service_handle == g_proxy_svc_handle) {
        g_proxy_config_h = handle;
        BT_INFO("g_proxy_config_h 0x%x", g_proxy_config_h);
    } else if (service_handle == g_prov_svc_handle) {
        g_prov_config_h = handle;
        BT_INFO("g_prov_config_h 0x%x", g_prov_config_h);
    }
    if (ret != 0) {
        BT_ERR("[ERROR] err gatts_add_descriptor err ret = %x\n", ret);
        return -1;
    }
    return 0;
}

static void bt_mesh_service_change_release_sem(uint16_t handle)
{
    if (handle == g_cur_handle) {
        ble_npl_sem_release(&g_change_service_sem);
        BT_INFO("handle recv:0x%x release sem:0x%x", handle, &g_change_service_sem);
    }
}

static void ble_mesh_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    BT_INFO("[start service]status %d server_id %d hdl 0x%x", status, server_id, handle);
    bt_mesh_service_change_release_sem(handle);
}

static void ble_mesh_server_service_stop_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    BT_INFO("[stop service] status %d server_id %d hdl 0x%x", status, server_id, handle);
    bt_mesh_service_change_release_sem(handle);
}

static void ble_mesh_receive_read_req_cbk(
    uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    BT_INFO("gatts read, server_id %d handle %d", server_id, read_cb_para->handle);  // 当前app写的数据
    MESH_UNUSED(conn_id);
    MESH_UNUSED(status);
}

/* 手机写数据后的回调 */
static void bt_mesh_request_write_cb(uint16_t conn_id, gatts_req_write_cb_t *write_cb_para)
{
    BT_INFO("gatts write, handle %d, len %d, data %s",
        write_cb_para->handle,
        write_cb_para->length,
        bt_hex(write_cb_para->value, write_cb_para->length));  // 当前app写的数据
    
    struct ble_gatt_access_ctxt ctxt = {0};
    struct os_mbuf *om = ble_hs_mbuf_from_flat(write_cb_para->value, write_cb_para->length); // 解析当前app发送过来的数据
    ctxt.om = om;
    
    BT_DBG(" in %d %d", g_proxy_data_in_h, g_prov_data_in_h);
    BT_DBG(" out %d %d", g_proxy_data_out_h, g_prov_data_out_h);
    BT_DBG(" config %d %d", g_proxy_config_h, g_prov_config_h);
    
    // 配网流程数据
    if (write_cb_para->handle == g_prov_data_in_h) {
        BT_INFO("prov access_cb in %x", g_svc_defs[1].characteristics[0].access_cb);
        g_svc_defs[1].characteristics[0].access_cb(conn_id, write_cb_para->handle, &ctxt, NULL);
        os_mbuf_free_chain(om);
        return;
    }

    // 入网后配置数据，绑定秘钥等
    if (write_cb_para->handle == g_proxy_data_in_h) {
        BT_INFO("proxy access_cb in %x", g_svc_defs[0].characteristics[0].access_cb);
        g_svc_defs[0].characteristics[0].access_cb(conn_id, write_cb_para->handle, &ctxt, NULL);
        os_mbuf_free_chain(om);
        return;
    }
    os_mbuf_free_chain(om);

    // 使能CCCD，订阅数据通知
    uint16_t write_value = write_cb_para->value[0] | write_cb_para->value[1] << 8;
    if (write_value != BLE_MESH_GATT_CCC_NOTIFY) {
        BT_INFO("Client wrote 0x%04x instead enabling notify", write_value);
        return;
    }

    // 注册客户端
    struct ble_gap_event event;
    event.subscribe.conn_handle = conn_id;
    event.subscribe.attr_handle =
        (write_cb_para->handle == g_prov_config_h) ? g_prov_data_out_h : g_proxy_data_out_h;

    event.type = BLE_GAP_EVENT_SUBSCRIBE;
    ble_gap_event_fn *gap_cb = ble_mesh_get_gap_event();
    if (gap_cb != NULL) {
        gap_cb(&event, NULL);
    } else {
        BT_ERR("gap cb is null");
    }
    BT_INFO("[debug] bt_mesh_request_write_cb end\n");
}

static void ble_mesh_receive_write_req_cbk(
    uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    bt_mesh_request_write_cb(conn_id, write_cb_para);
    MESH_UNUSED(server_id);
    MESH_UNUSED(status);
}

static int ble_mesh_gatts_add_service(uint8_t serverId, bt_uuid_t *srvcUuid, bool isPrimary, uint16_t *svc_handle)
{
    uint8_t tmp = srvcUuid->uuid[0];
    srvcUuid->uuid[0] = srvcUuid->uuid[1];
    srvcUuid->uuid[1] = tmp;
    errcode_t ret = gatts_add_service_sync(serverId, srvcUuid, isPrimary, svc_handle);

    if (srvcUuid->uuid[1] == 0x28) {
        g_proxy_svc_handle = *svc_handle;
    } else if (srvcUuid->uuid[1] == 0x27) {
        g_prov_svc_handle = *svc_handle;
    }
    BT_INFO("g_proxy_svc_handle = 0x%x, g_prov_svc_handle = 0x%x", g_proxy_svc_handle, g_prov_svc_handle);
    if (ret != 0) {
        BT_ERR("[ERROR] err gatts_add_service ret = %x", ret);
        return -1;
    }
    return 0;
}

static void ble_mesh_set_proxy_svc_handle(uint16_t handle)
{
    g_proxy_svc_handle = handle;
    if (handle == 0) {
        g_proxy_data_in_h = 0;
        g_proxy_data_out_h = 0;
        g_proxy_config_h = 0;
    }
}

static void ble_mesh_set_prov_svc_handle(uint16_t handle)
{
    g_prov_svc_handle = handle;
    if (handle == 0) {
        g_prov_config_h = 0;
        g_prov_data_in_h = 0;
        g_prov_data_out_h = 0;
    }
}

static int32_t bt_mesh_service_change_ev_push(uint8_t srv_id, uint8_t type, uint16_t handle)
{
    ble_mesh_service_event_t ev = {0};
    int32_t ret = ERRCODE_SUCC;

    ev.server_id = srv_id;
    ev.type = type;
    ev.handle = handle;

    ret = osal_msg_queue_write_copy(g_service_change_queue_id, &ev, sizeof(ble_mesh_service_event_t), 0);
    if (ret != EOK) {
        BT_ERR("queue full");
        return ERRCODE_FAIL;
    }

    return ret;
}

static int ble_mesh_gatts_add_svcs(uint8_t i)
{
    if (!g_svc_defs) {
        return -1;
    }
    const struct ble_gatt_svc_def *svcs = g_svc_defs;
    const struct ble_gatt_svc_def *svc_def = &svcs[i];
    if (svc_def->type == BLE_GATT_SVC_TYPE_PRIMARY) {
        bt_uuid_t svcuuid = {0};
        ble_mesh_nimuuid_2_btuuid(&svcuuid, svc_def->uuid);
        ble_mesh_gatts_add_service(g_mesh_server_id, &svcuuid, true, &g_svc_handle);
        uint16_t svc_handle = g_svc_handle;
        for (int j = 0; j < BLE_GATT_SVC_COUNT; j++) {
            const struct ble_gatt_chr_def *chr_def = &(svc_def->characteristics[j]);
            bt_uuid_t chruuid = {0};
            gatts_add_character_result_t result = {0};
            ble_mesh_nimuuid_2_btuuid(&chruuid, chr_def->uuid);
            uint32_t perm = get_perm(chr_def);
            gatts_add_chara_info_t character;
            character.chara_uuid = chruuid;
            character.permissions = perm;
            character.properties = chr_def->flags;
            character.value_len = sizeof(g_char_value);
            character.value = g_char_value;
            BT_DBG("[debug] start i = %d.\n", i);
            ble_mesh_gatts_add_characteristic(g_mesh_server_id, svc_handle, &character, &result);
        }
        bt_uuid_t descuuid = {0};
        ble_mesh_nimuuid_2_btuuid(&descuuid, &g_gatt_ccc.u);
        gatts_add_desc_info_t descriptor;
        descriptor.desc_uuid = descuuid;
        descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
        descriptor.value_len = sizeof(g_ccc_val);
        descriptor.value = g_ccc_val;
        ble_mesh_gatts_add_descriptor(g_mesh_server_id, g_svc_handle, &descriptor);
        errcode_t ret = gatts_start_service(g_mesh_server_id, g_svc_handle);
        if (ret != 0) {
            BT_ERR("err start service:%x", ret);
        }
    }
    return g_svc_handle;
}

static int ble_mesh_register_gatt_cb(void)
{
    gatts_callbacks_t cb = {0};
    cb.add_service_cb = ble_mesh_server_service_add_cbk;
    cb.add_characteristic_cb = ble_mesh_server_characteristic_add_cbk;
    cb.add_descriptor_cb = ble_mesh_server_descriptor_add_cbk;
    cb.start_service_cb = ble_mesh_server_service_start_cbk;
    cb.read_request_cb = ble_mesh_receive_read_req_cbk;
    cb.write_request_cb = ble_mesh_receive_write_req_cbk;
    cb.mtu_changed_cb = ble_mesh_mtu_changed_cbk;
    cb.stop_service_cb = ble_mesh_server_service_stop_cbk;

    return gatts_register_callbacks(&cb);
}

static int bt_mesh_service_change_task_body(void *data)
{
    MESH_UNUSED(data);
    ble_mesh_service_event_t ev = {0};
    uint32_t msg_data_size = sizeof(ble_mesh_service_event_t);
    int ret;

    while (true) {
        if (osal_kthread_should_stop() != 0) {
            break;
        }
        ble_npl_sem_pend_ms(&g_change_service_sem, TIME_OUT_1000_MS);
        (void)memset_s(&ev, sizeof(ble_mesh_service_event_t), 0, msg_data_size);
        ret = osal_msg_queue_read_copy(g_service_change_queue_id, &ev, &msg_data_size, OSAL_WAIT_FOREVER);
        if (ret != ERRCODE_SUCC) {
            continue;
        }

        if (ev.type == 0) {
            g_cur_handle = ev.handle;

            gatts_stop_service(ev.server_id, ev.handle);
            BT_INFO("[DEBUG] ble_gatts_svc_set_visibility stop handle = 0x%x", g_cur_handle);
            if (g_proxy_svc_handle == ev.handle) {
                ble_mesh_set_proxy_svc_handle(0);
            }
            if (g_prov_svc_handle == ev.handle) {
                ble_mesh_set_prov_svc_handle(0);
            }
        } else {
            g_cur_handle = ble_mesh_gatts_add_svcs(ev.handle);
            BT_INFO("[DEBUG] ble_gatts_svc_set_visibility start handle = 0x%x", g_cur_handle);
        }
        // 更新服务句柄
        resolve_svc_handles();
    }

    return EXT_ERR_SUCCESS;
}

static bool bt_mesh_service_change_task_deinit(void)
{
    if (g_service_change_task_hdl != NULL) {
        osal_kthread_destroy(g_service_change_task_hdl, 0);
        g_service_change_task_hdl = NULL;
    }

    if (g_service_change_queue_id != OSAL_INVALID_MSG_NUM) {
        osal_msg_queue_delete(g_service_change_queue_id);
        g_service_change_queue_id = OSAL_INVALID_MSG_NUM;
    }

    ble_npl_sem_deinit(&g_change_service_sem);
    return true;
}

static bool bt_mesh_service_change_task_init(void)
{
    if (g_service_change_queue_id != OSAL_INVALID_MSG_NUM) {
        return true;
    }
    ble_npl_sem_init(&g_change_service_sem, 1);

    BT_INFO("");
    int32_t ret = ERRCODE_SUCC;

    ret = (uint32_t)osal_msg_queue_create(
        "service_change_msg", SERVICE_QUEUE_MAX_SIZE, &g_service_change_queue_id, 0, sizeof(ble_mesh_service_event_t));
    if (ret != ERRCODE_SUCC) {
        BT_ERR("queue init fail, ret: 0x%x", ret);
        return false;
    }

    g_service_change_task_hdl =
        osal_kthread_create(bt_mesh_service_change_task_body, NULL,
                            SERVICE_CHANGE_TASK_NAME, SERVICE_CHANGE_TASK_STACK_SIZE);
    if (g_service_change_task_hdl == NULL) {
        BT_ERR("create service change task fail");
        osal_msg_queue_delete(g_service_change_queue_id);
        g_service_change_queue_id = OSAL_INVALID_MSG_NUM;
        return false;
    }
    osal_kthread_set_priority(g_service_change_task_hdl, SERVICE_CHANGE_TASK_PRIORITY_NUM);
    return true;
}

int ble_gatts_svc_set_visibility(uint16_t handle, int visible)
{
    if (visible) {
        bt_mesh_service_change_ev_push(g_mesh_server_id, 1, handle);
    } else {
        if (handle < BLE_MESH_GATT_SERVICE_MIN_HANDLE) {
            return 0;
        }

        bt_mesh_service_change_ev_push(g_mesh_server_id, 0, handle);
    }

    return 0;
}

int ble_gatts_find_chr(
    const ble_uuid_t *svc_uuid, const ble_uuid_t *chr_uuid, uint16_t *out_def_handle, uint16_t *out_val_handle)
{
    bt_uuid_t svcuuid = {0};
    bt_uuid_t chruuid = {0};
    ble_mesh_nimuuid_2_btuuid(&svcuuid, svc_uuid);
    ble_mesh_nimuuid_2_btuuid(&chruuid, chr_uuid);
    if (chruuid.uuid[0] == 0xDD) {
        *out_val_handle = g_proxy_data_in_h;
    } else if (chruuid.uuid[0] == 0xDB) {
        *out_val_handle = g_prov_data_in_h;
    } else if (chruuid.uuid[0] == 0xDE) {
        *out_val_handle = g_proxy_data_out_h;
    } else if (chruuid.uuid[0] == 0xDC) {
        *out_val_handle = g_prov_data_out_h;
    }
    BT_DBG("chruuid.uuid[0]:0x%x,chruuid.uuid[1]:0x%x *out_val_handle:%d\n", chruuid.uuid[0], chruuid.uuid[1],
        *out_val_handle);
    BT_DBG("g_proxy_data_in_h:%d,g_proxy_data_out_h:%d,g_prov_data_in_h:%d,g_prov_data_out_h:%d\n",
        g_proxy_data_in_h, g_proxy_data_out_h, g_prov_data_in_h, g_prov_data_out_h);
    MESH_UNUSED(out_def_handle);
    MESH_UNUSED(out_val_handle);
    return 0;
}

int ble_gatts_add_svcs(const struct ble_gatt_svc_def *svcs)
{
    BT_INFO("[debug] ble_gatts_add_svcs start.\n");
    g_svc_defs = svcs;

    return 0;
}

int ble_gatts_notify_custom(uint16_t conn_handle, uint16_t chr_val_handle, struct os_mbuf *txom)
{
    int rc = 0;
    gatts_ntf_ind_t ind_param;
    if (txom == NULL) {
        BT_ERR("txom is NULL");
        return ERRCODE_BT_FAIL;
    }
    ind_param.attr_handle = chr_val_handle;
    ind_param.value_len = txom->om_len;
    ind_param.value = txom->om_data;
    rc = gatts_notify_indicate(g_mesh_server_id, conn_handle, &ind_param);
    os_mbuf_free_chain(txom);
    return rc;
}

void ble_svc_gatt_changed(uint16_t start_handle, uint16_t end_handle)
{
    // bluez协议栈属性更新会自动通知
    MESH_UNUSED(start_handle);
    MESH_UNUSED(end_handle);
}

int ble_gatts_find_svc(const ble_uuid_t *uuid, uint16_t *out_handle)
{
    bt_uuid_t svc_uuid = {0};
    ble_mesh_nimuuid_2_btuuid(&svc_uuid, uuid);
    BT_DBG("[debug] ble_gatts_find_svc uuid:0x%x%x", svc_uuid.uuid[0], svc_uuid.uuid[1]);
    if (svc_uuid.uuid[0] == 0x28) {
        if (g_proxy_svc_handle != 0) {
            *out_handle = g_proxy_svc_handle;
        } else {
            *out_handle = 0;
        }
    }
    if (svc_uuid.uuid[0] == 0x27) {
        if (g_prov_svc_handle != 0) {
            *out_handle = g_prov_svc_handle;
        } else {
            *out_handle = 1;
        }
    }
    return 0;
}

int ble_mesh_gatts_register(void)
{
    bt_uuid_t gatts_app_uuid = {BLE_UUID_MAX_LEN, {0}};

    if (g_mesh_server_registered) {
        BT_INFO("g_mesh_server_registered true. g_mesh_server_id = %d\n", g_mesh_server_id);
        return 0;
    }

    if (memset_s(gatts_app_uuid.uuid, BLE_UUID_MAX_LEN, BLE_MESH_GATTS_APP_UUID_BYTE, BLE_UUID_MAX_LEN) != EOK) {
        BT_ERR("memset_s failed");
        return -1;
    }
    BT_DBG("ble_mesh_gatts_register. g_mesh_server_id = %d\n", g_mesh_server_id);
    int ret = gatts_register_server(&gatts_app_uuid, &g_mesh_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gatts_register_server failed. err %d", ret);
        return -1;
    }

    g_mesh_server_registered = true;
    return 0;
}

int ble_mesh_gatts_unregister(void)
{
    int ret = gatts_unregister_server(g_mesh_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gatts_unregister_server failed. err %d", ret);
        return -1;
    }
    g_mesh_server_registered = false;
    return 0;
}

/* 校验gatt 配置 */
int ble_gatts_count_cfg(const struct ble_gatt_svc_def *defs)
{
    MESH_UNUSED(defs);
    return EOK;
}

uint16_t ble_att_mtu(uint16_t conn_handle)
{
    MESH_UNUSED(conn_handle);
    return ble_mesh_mtu();
}

bool ble_mesh_gatt_adapter_init(void)
{
    int ret = ble_mesh_register_gatt_cb();
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gatts_register_callbacks failed. err %d", ret);
        return false;
    }

    if (!bt_mesh_service_change_task_init()) {
        BT_ERR("service change task init failed.");
        return false;
    }
    return true;
}

bool ble_mesh_gatt_adapter_deinit(void)
{
    if (!bt_mesh_service_change_task_deinit()) {
        BT_ERR("service change task deinit failed.");
        return false;
    }
    return true;
}