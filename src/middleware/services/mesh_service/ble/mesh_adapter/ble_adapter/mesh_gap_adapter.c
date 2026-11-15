/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: gap adapter source
 *
 */
#include "osal_addr.h"
#include "bts_le_gap.h"
#include "host/ble_gap.h"  // 导入ble_gap_adv_params
#include "cfg.h"
#include "ble_hs_conn_priv.h"
#include "os_time.h"
#include "npl_timer.h"
#include "adv.h"
#include "remote_prov_server.h"
#include "mesh_gap_adapter.h"

#define BT_DATA_HEAD_LEN (2)

uint8_t adv_buf[BLE_MESH_GAP_ADV_LEN_MAX];
static ble_gap_event_fn *g_ble_gap_event_fn = NULL;
static bt_mesh_scan_cb_t *bt_mesh_scan_dev_found_cb = NULL;
static struct ble_npl_sem g_gap_sem = {0};
static struct bt_mesh_dev g_bt_mesh_dev;
static bd_addr_t g_bt_mesh_gatts_addr;
static struct ble_hs_conn g_ble_hs_conn;

extern errcode_t gap_ble_register_mesh_callbacks(gap_ble_mesh_callback_t *mesh_callback);

int ble_gap_event_listener_register(struct ble_gap_event_listener *listener, ble_gap_event_fn *fn, void *arg)
{
    g_ble_gap_event_fn = fn;
    return 0;
}

static void bt_mesh_connect_server_cb(int conn_id)
{
    BT_INFO("[debug] bt_mesh_connect_server_cb conn_id = %d\n", conn_id);
    struct ble_gap_event event;
    event.type = BLE_GAP_EVENT_CONNECT;
    event.connect.conn_handle = conn_id;
    event.connect.status = 0;
    g_ble_gap_event_fn(&event, NULL);
}

static void bt_mesh_disconnect_server_cb(int conn_id, int reason)
{
    BT_INFO("[debug] bt_mesh_disconnect_server_cb conn_id = %d\n", conn_id);
    struct ble_gap_event event;
    event.disconnect.conn.conn_handle = conn_id;
    event.disconnect.conn.role = BLE_GAP_ROLE_SLAVE;
    event.disconnect.reason = reason;
    event.type = BLE_GAP_EVENT_DISCONNECT;
    g_ble_gap_event_fn(&event, NULL);
}

static void ble_mesh_terminate_adv_cb(uint8_t adv_id, adv_status_t status)
{
    BT_DBG("adv terminate cb adv_id %d, status %d\n", adv_id, status);
    MESH_UNUSED(adv_id);
    MESH_UNUSED(status);
}

static errcode_t ble_mesh_scan_result_cb(gap_scan_result_data_t *scan_result_data)
{
    struct ble_gap_event event = {0};
    struct ble_gap_disc_desc disc = {0};
    uint8_t ex_scan_state = 0;
#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER == 1)
    ex_scan_state = bt_mesh_rmt_prov_ex_scan_state_get();
#endif
    if (scan_result_data == NULL || scan_result_data->adv_len < 2 ||         // 2: index
        (scan_result_data->adv_data[1] != BLE_HS_ADV_TYPE_MESH_MESSAGE &&    // 0x2a: Mesh Message.
        scan_result_data->adv_data[1] != BLE_HS_ADV_TYPE_MESH_BEACON &&      // 0x2b: mesh beacon.
        scan_result_data->adv_data[1] != BLE_HS_ADV_TYPE_MESH_PROV &&        // 0x29: PB-ADV.
        ex_scan_state == 0)) {                                               // 扩展扫描未启动
        return EINVAL;
    }

    if (!ble_get_adapter_init_status()) {
        return EINVAL;
    }

    disc.event_type = scan_result_data->event_type;
    uint8_t *data = (uint8_t *)osal_kmalloc(scan_result_data->adv_len * sizeof(uint8_t), 0);
    if (data == NULL) {
        return ENOMEM;
    }

    if (memcpy_s(data, scan_result_data->adv_len, scan_result_data->adv_data, scan_result_data->adv_len) != EOK) {
        BT_ERR("data memcpy_s failed");
        osal_kfree(data);
        return EFAULT;
    }
    disc.event_type = BLE_HCI_ADV_TYPE_ADV_NONCONN_IND;
    disc.length_data = scan_result_data->adv_len;
    disc.data = data;
    disc.rssi = scan_result_data->rssi;
    disc.addr.type = scan_result_data->addr.type;
    (void)memcpy_s(disc.addr.val, BD_ADDR_LEN, scan_result_data->addr.addr, BD_ADDR_LEN);
    event.type = BLE_GAP_EVENT_DISC;
    event.disc = disc;
    (void)memcpy_s(&event.disc, sizeof(struct ble_gap_disc_desc), &disc, sizeof(struct ble_gap_disc_desc));
    ble_adv_gap_mesh_cb(&event, NULL);
#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER == 1)
    if (ex_scan_state) {
        bt_mesh_rmt_prov_extend_unprovisioned_adv(
            scan_result_data->event_type, &disc.addr, disc.data, disc.length_data);
    }
#endif
    osal_kfree(data);

    return 0;
}

static void ble_mesh_gap_ble_connect_state_changed_callback(uint16_t conn_id, bd_addr_t *addr,
    gap_ble_conn_state_t conn_state, gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_bt_mesh_gatts_addr.type = addr->type;
    memcpy_s(g_bt_mesh_gatts_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
    if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        bt_mesh_disconnect_server_cb((int)conn_id, disc_reason);
    } else if (conn_state == GAP_BLE_STATE_CONNECTED) {
        bt_mesh_connect_server_cb((int)conn_id);
    } else {
        BT_ERR("[ble_connect_state_changed] state err %d \r\n", conn_state);
    }

    MESH_UNUSED(pair_state);
}

static void ble_mesh_scan_param_set_cb(errcode_t status)
{
    BT_DBG("scan param set cb status %d\n", status);
    MESH_UNUSED(status);
}

static void ble_mesh_adv_data_set_cb(uint8_t adv_id, errcode_t status)
{
    BT_DBG("adv data set cb adv_id %d, status %d\n", adv_id, status);
    MESH_UNUSED(adv_id);
    MESH_UNUSED(status);
}

static void ble_mesh_adv_enable_cb(uint8_t adv_id, adv_status_t status)
{
    BT_DBG("adv enable cb adv_id %d, status %d\n", adv_id, status);
    ble_npl_sem_release(&g_gap_sem);
    MESH_UNUSED(adv_id);
    MESH_UNUSED(status);
}

static void ble_mesh_adv_disable_cb(uint8_t adv_id, adv_status_t status)
{
    BT_DBG("adv disable cb adv_id %d, status %d\n", adv_id, status);
    ble_npl_sem_release(&g_gap_sem);
    MESH_UNUSED(adv_id);
    MESH_UNUSED(status);
}

static int set_ad(const struct bt_data *data, size_t data_len, uint8_t *buf, uint8_t *buf_len)
{
    size_t i;
    uint8_t len = 0;

    for (i = 0; i < data_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (len + data[i].data_len + BT_DATA_HEAD_LEN > BLE_MESH_GAP_ADV_LEN_MAX) {
            return -1;
        }

        buf[len++] = data[i].data_len + 1;
        buf[len++] = data[i].type;

        if (memcpy_s(&buf[len], BLE_MESH_GAP_ADV_LEN_MAX - len, data[i].data, data[i].data_len) != EOK) {
            BT_ERR("memcpy_s failed");
            return -1;
        }
        len += data[i].data_len;
    }

    *buf_len = len;

    return 0;
}

static int ble_start_adv(int adv_id, gap_ble_adv_params_t *param)
{
    gap_ble_set_adv_param(adv_id, param);
    errcode_t ret = gap_ble_start_adv(adv_id);
    uint16_t timeout = param->duration + 20 > TIME_OUT_500_MS ? TIME_OUT_500_MS : param->duration + 20; // 20 时延
    BT_DBG("adv start adv id:%d ret = %d.\n", adv_id, ret);
    if (ret != 0) {
        return -1;
    } else {
        gap_ble_set_adv_random_delay(adv_id, 1);
        ble_npl_sem_pend_ms(&g_gap_sem, timeout);
    }
    return 0;
}

static int bt_mesh_start_adv(int adv_id, const struct ble_gap_adv_params *param, const struct bt_data *sd)
{
    gap_ble_adv_params_t adv_param = {0};
    uint8_t adv_type = 0U;
    if (param->conn_mode == BLE_GAP_CONN_MODE_UND) {
        adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    } else if (param->conn_mode == BLE_GAP_CONN_MODE_DIR) {
        adv_type = GAP_BLE_ADV_NONCONN_SCAN_UNDIR;
    } else if (sd != NULL) {
        adv_type = GAP_BLE_ADV_NONCONN_SCAN_UNDIR;
    } else {
        adv_type = GAP_BLE_ADV_NONCONN_NONSCAN_UNDIR;
    }
    adv_param.adv_type = adv_type;
    adv_param.adv_filter_policy = param->filter_policy;
    adv_param.channel_map = BLE_MESH_ADV_CHNL_37 | BLE_MESH_ADV_CHNL_38 | BLE_MESH_ADV_CHNL_39;
    adv_param.duration = param->duration;
    adv_param.max_interval = (uint32_t)(param->itvl_max);
    adv_param.min_interval = (uint32_t)(param->itvl_min);
    if (bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED) {
        // 私有beacon使能后使用random addr
        adv_param.own_addr.type = BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS;
        bt_rand(adv_param.own_addr.addr + 3, 3);    // 3: index
        adv_param.own_addr.addr[5] = ((adv_param.own_addr.addr[5] & 0x3f) | 0x40);  // 5: index, 0x3f, 0x40: 随机地址
        memset_s(adv_param.peer_addr.addr, BLE_MESH_ADDR_LEN, 0, BLE_MESH_ADDR_LEN);
    } else {
        adv_param.own_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;
        adv_param.peer_addr.type = 0;
        memset_s(adv_param.own_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        memset_s(adv_param.peer_addr.addr, BLE_MESH_ADDR_LEN, 0, BLE_MESH_ADDR_LEN);
    }
    adv_param.tx_power = 0x7F;  // 不设置特定发送功率
    adv_param.max_events = param->max_events;
    // 开始广播
    int ret = ble_start_adv(adv_id, &adv_param);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("ble_start_adv failed. err %d", ret);
        return ret;
    }
    return 0;
}

static int bt_mesh_set_adv_data(
    const struct bt_data *ad, size_t ad_len, const struct bt_data *sd, size_t sd_len, int *adv_id)
{
    uint8_t *buf = adv_buf;
    uint8_t buf_len = 0;
    int ret;
    gap_ble_config_adv_data_t ble_adv_data;

    if (ad == NULL || ad_len == 0) {
        MESH_UNUSED(ble_adv_data);
        return 0;
    }

    ret = set_ad(ad, ad_len, buf, &buf_len);
    if (ret != 0) {
        MESH_UNUSED(ble_adv_data);
        return -1;
    }

    ble_adv_data.adv_data = (uint8_t *)buf;
    ble_adv_data.adv_length = (uint16_t)buf_len;

    BT_DBG("data %u: %s", ble_adv_data.adv_length, bt_hex(ble_adv_data.adv_data, ble_adv_data.adv_length));

    if (sd != NULL && sd_len != 0) {
        uint8_t buf_sd[BLE_MESH_GAP_ADV_LEN_MAX];
        buf_len = 0;
        ret = set_ad(sd, sd_len, buf_sd, &buf_len);
        if (ret != 0) {
            MESH_UNUSED(ble_adv_data);
            return -1;
        }

        ble_adv_data.scan_rsp_data = (uint8_t *)buf_sd;
        ble_adv_data.scan_rsp_length = (uint16_t)buf_len;
    } else {
        ble_adv_data.scan_rsp_data = NULL;
        ble_adv_data.scan_rsp_length = 0;
    }

    BT_DBG("[DEBUG]gap_ble_set_adv_data. adv_length=%u scan_rsp_length = %u \r\n",
        ble_adv_data.adv_length,
        ble_adv_data.scan_rsp_length);

    if (ble_adv_data.adv_length > BLE_MESH_LEGACY_ADV_LEN_MAX) {
        *adv_id = 1;
        ret = gap_ble_set_adv_data(1, &ble_adv_data);
    } else {
        *adv_id = 0;
        ret = gap_ble_set_adv_data(0, &ble_adv_data);
    }
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("BleSetAdvData failed. err %d", ret);
        MESH_UNUSED(ble_adv_data);
        return -1;
    }

    MESH_UNUSED(ble_adv_data);
    return 0;
}

static int ble_stop_adv(uint8_t adv_id)
{
    errcode_t ret = gap_ble_stop_adv(adv_id);
    if (ret != 0) {
        BT_ERR("[ERROR] err gap_ble_stop_adv err code = %x.\n", ret);
        return -1;
    } else {
        ble_npl_sem_pend_ms(&g_gap_sem, TIME_OUT_100_MS);
    }
    return 0;
}

static int ble_set_scan_parameters(gap_ble_scan_params_t *param)
{
    errcode_t ret = gap_ble_set_scan_parameters(param);
    if (ret != 0) {
        BT_ERR("[ERROR] ble_set_scan_parameters err code = %x.\n", ret);
        return -1;
    }
    return 0;
}

static void gap_ble_scan_result_cbk(const bt_mesh_addr_t *addr, int8_t rssi, uint8_t adv_type, struct os_mbuf *buf)
{
    BT_INFO("ScanResultCallback: gap ext adv report. \
        addr_type:0x%x addr:0x%02x:**:**:**:**:%02x data_len:0x%02x\r\n",
        adv_type, addr->val[0], addr->val[BT_INDEX_5], BLE_MESH_ADDR_LEN);
}

static int ble_mesh_gap_register_cbk(void)
{
    gap_ble_callbacks_t cb_gap = {0};
    cb_gap.set_adv_data_cb = ble_mesh_adv_data_set_cb;
    cb_gap.set_scan_param_cb = ble_mesh_scan_param_set_cb;
    cb_gap.start_adv_cb = ble_mesh_adv_enable_cb;
    cb_gap.stop_adv_cb = ble_mesh_adv_disable_cb;
    cb_gap.scan_result_cb = ble_mesh_scan_result_cb;
    cb_gap.conn_state_change_cb = ble_mesh_gap_ble_connect_state_changed_callback;
    cb_gap.terminate_adv_cb = ble_mesh_terminate_adv_cb;
    
    gap_ble_mesh_callback_t cb_mesh_gap = {0};
    cb_mesh_gap.mesh_scan_func = ble_mesh_scan_result_cb;
    int ret = gap_ble_register_mesh_callbacks(&cb_mesh_gap);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gap_ble_register_mesh_callbacks failed. err 0x%x", ret);
        return -1;
    }

    ret = gap_ble_register_callbacks(&cb_gap);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gap_ble_register_mesh_callbacks failed. err 0x%x", ret);
        return -1;
    }
    return 0;
}

int ble_gap_adv_rsp_set_data(const uint8_t *data, int data_len)
{
    MESH_UNUSED(data);
    MESH_UNUSED(data_len);
    return 0;
}

// nimble调用该接口开始设置广播参数然后起广播
int bt_le_adv_start(const struct ble_gap_adv_params *param, const struct bt_data *ad,
    size_t ad_len, const struct bt_data *sd, size_t sd_len, int *adv_id)
{
    int ret = 0;
#if BLE_MESH_DEV
    if (atomic_set_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        BT_ERR("[ERROR] err atomic_set_bit err.\n");
        return -EALREADY;
    }
#endif

    if (param->conn_mode & BLE_GAP_CONN_MODE_UND) {
        ret = bt_mesh_set_adv_data(ad, ad_len, sd, sd_len, adv_id);
    } else {
        ret = bt_mesh_set_adv_data(ad, ad_len, NULL, 0, adv_id);
    }

    if (ret != 0) {
        BT_ERR("bt_mesh_set_adv_data err %d", ret);
        return ret;
    }

    ret = bt_mesh_start_adv(*adv_id, param, sd);
    if (ret != 0) {
        BT_ERR("bt_mesh_start_adv err %d", ret);
        return ret;
    }

#if BLE_MESH_DEV
    atomic_set_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);

    if (!(param->options & BLE_MESH_ADV_OPT_ONE_TIME)) {
        atomic_set_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    }
#endif
    return 0;
}

int ble_gap_adv_stop(int adv_id)
{
    return ble_stop_adv(adv_id);
}

int ble_gap_ext_disc(uint8_t own_addr_type, uint16_t duration, uint16_t period,
    uint8_t filter_duplicates, uint8_t filter_policy,
    uint8_t limited,
    const struct ble_gap_ext_disc_params *uncoded_params,
    const struct ble_gap_ext_disc_params *coded_params,
    ble_gap_event_fn *cb, void *cb_arg)
{
    int ret = 0;
    gap_ble_scan_params_t scan_params;

    if (atomic_test_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        BT_INFO("Scan is already started");
        return -EALREADY;
    }

    scan_params.scan_interval = uncoded_params->itvl;
    scan_params.scan_window = uncoded_params->window;
    scan_params.scan_type = uncoded_params->passive;
    scan_params.scan_phy = BLE_MESH_SCAN_PHY_1M;
    scan_params.scan_filter_policy = filter_policy;

    BT_INFO("interval %u, Window %u, scanType %u, scanPhy %u, scanFilterPolicy %u\n",
        scan_params.scan_interval,
        scan_params.scan_window,
        scan_params.scan_type,
        scan_params.scan_phy,
        scan_params.scan_filter_policy);

    ret = ble_set_scan_parameters(&scan_params);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("ble_set_scan_parameters faild! retCode: %d", ret);
        return -1;
    }

    gap_ble_extern_scan_params_t ext_param = {0};
    ext_param.filter_duplicate = GAP_BLE_FILTER_DUPLICATES_DISABLE;
    BT_ERR(" ble_gap_ext_disc gap_ble_set_scan_extern_parameters enter \r\n");
    ret = gap_ble_set_scan_extern_parameters(&ext_param);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR(" ble_gap_ext_disc ble_set_scan_extern_parameters faild! retCode: %d", ret);
        return -1;
    }

    ret = gap_ble_start_scan();
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gap_ble_start_scan faild! retCode: %d", ret);
        return -1;
    }

    atomic_set_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    if (cb == NULL) {
        // 添加scan cbk
        bt_mesh_scan_dev_found_cb = (bt_mesh_scan_cb_t *)gap_ble_scan_result_cbk;
    } else {
        bt_mesh_scan_dev_found_cb = (bt_mesh_scan_cb_t *)cb;
    }
    MESH_UNUSED(own_addr_type);
    MESH_UNUSED(cb_arg);
    return 0;
}

int ble_gap_disc(uint8_t own_addr_type, int32_t duration_ms, const struct ble_gap_disc_params *disc_params,
    ble_gap_event_fn *cb, void *cb_arg)
{
    int ret = 0;
    gap_ble_scan_params_t scan_params;

    if (atomic_test_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        BT_INFO("Scan is already started");
        return -EALREADY;
    }
    scan_params.scan_interval = disc_params->itvl;
    scan_params.scan_window = disc_params->window;
    scan_params.scan_type = disc_params->passive;
    scan_params.scan_phy = GAP_BLE_PHY_1M;
    scan_params.scan_filter_policy = disc_params->filter_policy;

    BT_INFO("interval %u, Window %u, scanType %u, scanPhy %u, scanFilterPolicy %u\n",
        scan_params.scan_interval,
        scan_params.scan_window,
        scan_params.scan_type,
        scan_params.scan_phy,
        scan_params.scan_filter_policy);

    ret = ble_set_scan_parameters(&scan_params);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("ble_set_scan_parameters faild! retCode: %d", ret);
        return -1;
    }

    gap_ble_extern_scan_params_t ext_param = {0};
    ext_param.filter_duplicate = GAP_BLE_FILTER_DUPLICATES_DISABLE;
    ret = gap_ble_set_scan_extern_parameters(&ext_param);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR(" ble_gap_disc ble_set_scan_extern_parameters faild! retCode: %d", ret);
        return -1;
    }

    ret = gap_ble_start_scan();
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gap_ble_start_scan faild! retCode: %d", ret);
        return -1;
    }

    atomic_set_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    if (cb == NULL) {
        // 添加scan cbk
        bt_mesh_scan_dev_found_cb = (bt_mesh_scan_cb_t *)gap_ble_scan_result_cbk;
    } else {
        bt_mesh_scan_dev_found_cb = (bt_mesh_scan_cb_t *)cb;
    }
    MESH_UNUSED(own_addr_type);
    MESH_UNUSED(duration_ms);
    MESH_UNUSED(cb_arg);
    return 0;
}

int ble_gap_disc_cancel()
{
    if (!atomic_test_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        BT_INFO("Scan is already stopped");
        return -EALREADY;
    }

    int ret = gap_ble_stop_scan();
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("gap_ble_stop_scan failed. err %d", ret);
        return -1;
    }

    atomic_clear_bit(g_bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    bt_mesh_scan_dev_found_cb = NULL;

    return 0;
}

int ble_gap_terminate(uint16_t conn_handle, uint8_t hci_reason)
{
    MESH_UNUSED(conn_handle);
    MESH_UNUSED(hci_reason);
    return gap_ble_disconnect_remote_device(&g_bt_mesh_gatts_addr);
}

bool ble_mesh_gap_adapter_init(void)
{
    if (ble_mesh_gap_register_cbk() != 0) {
        return false;
    }
    ble_npl_sem_init(&g_gap_sem, 0);
    return true;
}

bool ble_mesh_gap_adapter_deinit(void)
{
    ble_npl_sem_deinit(&g_gap_sem);
    return true;
}

struct ble_hs_conn *ble_hs_conn_find(uint16_t conn_handle)
{
    // 只有一路连接
    MESH_UNUSED(conn_handle);
    return &g_ble_hs_conn;
}

// 空实现，不影响功能
void ble_hs_conn_addrs(const struct ble_hs_conn *conn, struct ble_hs_conn_addrs *addrs)
{
    MESH_UNUSED(conn);
    MESH_UNUSED(addrs);
}

ble_gap_event_fn *ble_mesh_get_gap_event(void)
{
    return g_ble_gap_event_fn;
};