/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE CHBA SERVER
 */
#include "sle_chba_server.h"
#include "sle_chba_manager.h"
#include "sle_chba_opt.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "stdio.h"
#include "common_def.h"
#include "securec.h"
#include "sle_device_discovery.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "nv.h"
#include "app_init.h"
#include "soc_osal.h"
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
#include "syschannel_api.h"
#endif
#include "sle_chba_netif_mng.h"
#include "sle_chba_manager.h"
#if CHBA_LWIP_SWITCH
#include "lwip/netifapi.h"
#endif
#include "mac_addr.h"

#define CHECK_RC(rc, func)                                                             \
    do {                                                                               \
        if ((rc) != 0) {                                                               \
            printf("ERROR: %s: call %s return 0x%x!\r\n", __FUNCTION__, func, rc);         \
        }                                                                              \
    } while (0)

chba_cfg_t g_chba_cfg = {0, 0};
uint8_t g_chba_addr[SLE_CHBA_LINK_NUM_MAX + 1][SLE_ADDR_LEN] = {
    {0x2, 0x2, 0x2, 0x2, 0x2, 0x2},
    {0x12, 0x12, 0x12, 0x12, 0x12, 0x12},
    {0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
    {0x32, 0x32, 0x32, 0x32, 0x32, 0x32},
    {0x42, 0x42, 0x42, 0x42, 0x42, 0x42}
};

#define CHBA_AP_IDX 0
#define CHBA_ADV_HANDLE 2
#define CHBA_ADV_DATA "SLE_CHBA"
#define CHBA_SLE_SCAN_INTERVAL 800  // slot
#define CHBA_SLE_SCAN_WINDOW 800    // slot
#define CHBA_SLE_ADV_INTERVAL 800    // slot
#define CHBA_SLE_UPD_CON_INTERVAL 20    // slot

#define SET_STATUS(link, sts)                                \
    do {                                                     \
        printf("%s: STATUS %d -> %s(%d)\r\n", \
            __FUNCTION__,                                    \
            ((sle_chba_user_link_t *)(link))->status,        \
            #sts,                                            \
            sts);                                            \
        ((sle_chba_user_link_t *)(link))->status = sts;      \
    } while (0)

#define GET_STATUS(link) sle_chba_get_status(__FUNCTION__, link)

static errcode_t sle_chba_user_start_scan(void);
static errcode_t sle_chba_user_config_adv(void);
static errcode_t sle_chba_user_create_connection(const sle_addr_t *remote_addr);
static errcode_t sle_chba_user_set_phy(sle_chba_user_link_t *link, uint8_t phy);

static enum sle_chba_status sle_chba_get_status(const char *func, sle_chba_user_link_t *link)
{
    printf("%s: STATUS %d\r\n", func, link->status);
    return link->status;
}

static errcode_t sle_chba_user_set_mcs(sle_chba_user_link_t *link, uint8_t mcs)
{
    errcode_t rc = sle_set_mcs(link->conn_id, mcs);
    CHECK_RC(rc, "sle_set_mcs");
    link->link_param.mcs = mcs;
    return rc;
}

static void sle_chba_user_set_phy_cb(uint16_t conn_id, errcode_t status, const sle_set_phy_t *param)
{
    printf("%s: handle:%d, status 0x%x, txphy %d, rxphy %d\r\n",
        __FUNCTION__,
        conn_id,
        status,
        param->tx_phy,
        param->rx_phy);
    sle_chba_user_link_t *link = sle_chba_user_get_linkinfo_by_connid(conn_id);
    if (link == NULL) {
        return;
    }
    link->link_param.phy = param->tx_phy;
    SET_STATUS(link, STATUS_CHBA_READY);
    errcode_t rc = sle_chba_user_set_mcs(link, SLE_CHBA_DEFAULT_MCS);
    CHECK_RC(rc, "sle_chba_user_set_mcs");
    sle_chba_netdev_add_link(conn_id, &link->remote_addr);
    link->link_ready = true;
}

static void sle_chba_user_conn_state_cb(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(pair_state);
    errcode_t rc;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        sle_chba_user_link_t *link = sle_chba_user_get_new_linkinfo();
        link->conn_id = conn_id;
        (void)memcpy_s(&link->remote_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t));
        SET_STATUS(link, STATUS_CHBA_READY);
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
        sle_stop_seek();
#endif
        // PHY update
        if (g_chba_cfg.role_idx == CHBA_AP_IDX) {
            rc = sle_pair_remote_device(addr);
            CHECK_RC(rc, "sle_pair_remote_device");
        }
        SET_STATUS(link, STATUS_ENCRYPTING);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        chba_log_print("discon reason:0x%x\r\n", disc_reason);
        sle_chba_user_link_t *link = sle_chba_user_get_linkinfo_by_connid(conn_id);
        if (link != NULL) {
            sle_chba_netdev_del_link(conn_id, addr);
            sle_chba_user_del_linkinfo_by_connid(link->conn_id);
        }
        if (g_chba_cfg.role_idx == CHBA_AP_IDX) {
            rc = sle_chba_user_start_scan();
            CHECK_RC(rc, "sle_chba_user_start_scan");
        } else {
            rc = sle_chba_user_config_adv();
            CHECK_RC(rc, "sle_chba_user_config_adv");
        }
    }
}

static void sle_chba_user_conn_update_cb(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    chba_log_print("conn_update_cb handle:%d, state:0x%x, interval %d\r\n",
        conn_id,
        status,
        param->interval);
    sle_chba_user_link_t *link = sle_chba_user_get_linkinfo_by_connid(conn_id);
    if (link == NULL) {
        return;
    }
    link->link_param.interval = param->interval;
    SET_STATUS(link, STATUS_CHBA_READY);
}

static void sle_chba_user_adv_enable_cb(uint32_t announce_id, errcode_t status)
{
    chba_info_log("announce_id %d status 0x%x\r\n", announce_id, status);
}

static void sle_chba_user_scan_enable_cb(errcode_t status)
{
    chba_info_log("scan enable, status 0x%x\r\n", status);
}

static void sle_chba_user_scan_disable_cb(errcode_t status)
{
    chba_info_log("scan disable, status 0x%x\r\n", status);
}

static void sle_chba_user_scan_report_cb(sle_seek_result_info_t *report)
{
    errcode_t rc = ERRCODE_SUCC;
    if (report == NULL) {
        return;
    }
    // 扫描到chba支持 停止scan 开始建立连接
    if ((report->data_length == sizeof(CHBA_ADV_DATA)) &&
        (strncmp(CHBA_ADV_DATA, (char *)report->data, report->data_length) == 0)) {
        chba_info_log("Find CHBA support, addr: %02x:**:**:**:%02x:%02x!\r\n",
            report->addr.addr[CHBA_INDEX_0],
            report->addr.addr[CHBA_INDEX_4],
            report->addr.addr[CHBA_INDEX_5]);
        rc = sle_chba_user_create_connection(&report->addr);
        CHECK_RC(rc, "sle_chba_user_create_connection");
    }
}

static void sle_chba_user_pair_complete_cb(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    chba_info_log("pair complete! conn_id: %d, addr: %02x:**:**:**:%02x:%02x, status: 0x%x!\r\n",
        conn_id,
        addr->addr[CHBA_INDEX_0],
        addr->addr[CHBA_INDEX_4],
        addr->addr[CHBA_INDEX_5],
        status);
    if (status != ERRCODE_SUCC) {
        // 配对失败，删除配对信息
        sle_remove_paired_remote_device(addr);
        return;
    }
    sle_chba_user_link_t *link = sle_chba_user_get_linkinfo_by_connid(conn_id);
    if (link == NULL) {
        chba_err_log("invalid conn_id(%d)", conn_id);
        return;
    }
    SET_STATUS(link, STATUS_CHBA_READY);
    if (g_chba_cfg.role_idx != CHBA_AP_IDX) {
        errcode_t rc = sle_chba_user_set_phy(link, SLE_CHBA_DEFAULT_PHY);
        CHECK_RC(rc, "sle_chba_user_set_phy");
    }
}

static errcode_t sle_chba_user_register_cbks(void)
{
    errcode_t rc = ERRCODE_SUCC;

    sle_connection_callbacks_t cm_cbks = { .connect_state_changed_cb = sle_chba_user_conn_state_cb,
                                           .connect_param_update_cb = sle_chba_user_conn_update_cb,
                                           .set_phy_cb = sle_chba_user_set_phy_cb,
                                           .pair_complete_cb = sle_chba_user_pair_complete_cb,
                                        };
    rc = sle_connection_register_callbacks(&cm_cbks);

    CHECK_RC(rc, "sle_connection_register_callbacks");

    sle_announce_seek_callbacks_t scan_adv_cbks = { .seek_enable_cb = sle_chba_user_scan_enable_cb,
                                                    .seek_disable_cb = sle_chba_user_scan_disable_cb,
                                                    .seek_result_cb = sle_chba_user_scan_report_cb,
                                                    .announce_enable_cb = sle_chba_user_adv_enable_cb,
                                                    };
    rc = sle_announce_seek_register_callbacks(&scan_adv_cbks);
    CHECK_RC(rc, "sle_announce_seek_register_callbacks");

    return rc;
}

errcode_t sle_chba_user_release_connection(const sle_addr_t *remote_addr);
errcode_t sle_chba_user_release_connection(const sle_addr_t *remote_addr)
{
    errcode_t rc = ERRCODE_SUCC;
    rc = sle_disconnect_remote_device(remote_addr);
    CHECK_RC(rc, "sle_disconnect_remote_device");
    return rc;
}

static errcode_t sle_chba_user_create_connection(const sle_addr_t *remote_addr)
{
    errcode_t rc = ERRCODE_SUCC;
    sle_default_connect_param_t private_connect_param = {
        .enable_filter_policy = 0, // 过滤功能：0 关闭，1 打开
        .initiate_phys = 1, // 通信带宽：  1：1M，2：2M
        .gt_negotiate = 1, // 是否进行G和T交互： 0：否，1：是
        .scan_interval = CHBA_SLE_SCAN_INTERVAL,
        .scan_window = CHBA_SLE_SCAN_WINDOW,
        .min_interval = SLE_CHBA_DEFAULT_INTERVAL,
        .max_interval = SLE_CHBA_DEFAULT_INTERVAL,
        .timeout = SLE_CHBA_LINK_TIMEOUT
    };
    rc = sle_default_connection_param_set(&private_connect_param);
    CHECK_RC(rc, "sle_private_connection_param_set");

    rc = sle_connect_remote_device(remote_addr);
    CHECK_RC(rc, "sle_connect_remote_device");
    return rc;
}

static errcode_t sle_chba_user_config_adv(void)
{
    errcode_t rc = ERRCODE_SUCC;
    sle_addr_t local_addr = {0};
    rc = sle_get_local_addr(&local_addr);
    CHECK_RC(rc, "sle_get_local_addr");

    sle_announce_param_t adv_params = {
        .announce_handle = CHBA_ADV_HANDLE,
        .announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE,
        .announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO,
        .announce_level = SLE_ANNOUNCE_LEVEL_NORMAL,
        .announce_interval_min = CHBA_SLE_ADV_INTERVAL,
        .announce_interval_max = CHBA_SLE_ADV_INTERVAL,
        .announce_channel_map = 0x7,
        .announce_tx_power = 0x7f, // 不设定特定发射功率

        .conn_interval_min = SLE_CHBA_DEFAULT_INTERVAL,
        .conn_interval_max = SLE_CHBA_DEFAULT_INTERVAL,
        .conn_max_latency = 0,
        .conn_supervision_timeout = SLE_CHBA_LINK_TIMEOUT,
        .ext_param = NULL,
    };
    memcpy_s(&adv_params.own_addr, sizeof(sle_addr_t), &local_addr, sizeof(sle_addr_t));

    rc = sle_set_announce_param(CHBA_ADV_HANDLE, &adv_params);
    CHECK_RC(rc, "sle_set_announce_param");

    sle_announce_data_t adv_data = {
        .announce_data_len = sizeof(CHBA_ADV_DATA),
        .seek_rsp_data_len = sizeof(CHBA_ADV_DATA),
        .announce_data = (uint8_t *)&CHBA_ADV_DATA,
        .seek_rsp_data = (uint8_t *)&CHBA_ADV_DATA,
    };

    rc = sle_set_announce_data(CHBA_ADV_HANDLE, &adv_data);
    CHECK_RC(rc, "sle_set_announce_data");
    rc = sle_start_announce(CHBA_ADV_HANDLE);
    CHECK_RC(rc, "sle_start_announce en");
    return rc;
}

static errcode_t sle_chba_user_start_scan(void)
{
    sle_seek_param_t scan_params = {
        .own_addr_type = SLE_ADDRESS_TYPE_PUBLIC,
        .filter_duplicates = 1,
        .seek_filter_policy = SLE_SEEK_FILTER_ALLOW_ALL,
        .seek_phys = 1,
        .seek_type[0] = SLE_SEEK_ACTIVE,
        .seek_interval[0] = CHBA_SLE_SCAN_INTERVAL,
        .seek_window[0] = CHBA_SLE_SCAN_WINDOW,
    };
    errcode_t rc = sle_set_seek_param(&scan_params);
    CHECK_RC(rc, "sle_set_seek_param");

    rc = sle_start_seek();
    CHECK_RC(rc, "sle_start_seek");
    return rc;
}

static errcode_t sle_chba_user_set_phy(sle_chba_user_link_t *link, uint8_t phy)
{
    if (GET_STATUS(link) != STATUS_CHBA_READY) {
        return ERRCODE_SUCC;
    }
    uint8_t pilot_density = ((link->link_param.mcs == SLE_MCS_08) || (link->link_param.mcs == SLE_MCS_12)) ?
        SLE_PHY_PILOT_DENSITY_NO : SLE_PHY_PILOT_DENSITY_16_TO_1;
    sle_set_phy_t phy_param = {
        .tx_format = SLE_RADIO_FRAME_2,
        .rx_format = SLE_RADIO_FRAME_2,
        .tx_phy = phy,
        .rx_phy = phy,
        .tx_pilot_density = pilot_density,
        .rx_pilot_density = pilot_density,
        .g_feedback = 0,
        .t_feedback = 0
    };
    errcode_t rc = sle_set_phy_param(link->conn_id, &phy_param);
    CHECK_RC(rc, "sle_set_phy_param");
    SET_STATUS(link, STATUS_PHY_UPDATING);
    return rc;
}

errcode_t sle_chba_user_connection_update(sle_chba_user_link_t *link);
errcode_t sle_chba_user_connection_update(sle_chba_user_link_t *link)
{
    if (GET_STATUS(link) != STATUS_CHBA_READY) {
        return ERRCODE_SUCC;
    }
    sle_connection_param_update_t con_update = {
        .conn_id = link->conn_id,
        .interval_max = CHBA_SLE_UPD_CON_INTERVAL,
        .interval_min = CHBA_SLE_UPD_CON_INTERVAL,
        .max_latency = 0,
        .supervision_timeout = SLE_CHBA_LINK_TIMEOUT,
    };
    errcode_t rc = sle_update_connect_param(&con_update);
    CHECK_RC(rc, "sle_update_connect_param");
    SET_STATUS(link, STATUS_CONN_UPDATING);
    return ERRCODE_SUCC;
}

void sle_chba_sample_user_init_start_discovery(chba_cfg_t chba_cfg)
{
    if (chba_cfg.role_idx == CHBA_AP_IDX) {
        sle_chba_user_start_scan();
        chba_log_print("sle_chba_user_start_scan\r\n");
    } else {
        sle_chba_user_config_adv();
        chba_log_print("sle_chba_user_config_adv\r\n");
    }
}

void sle_chba_set_netdev_addr(void)
{
    sle_addr_t addr = { 0 };
    if (get_dev_addr(addr.addr, SLE_ADDR_LEN, IFTYPE_SLE) != ERRCODE_SUCC) {
        (void)memcpy_s(addr.addr, SLE_ADDR_LEN, g_chba_addr[g_chba_cfg.role_idx], SLE_ADDR_LEN);
    }
    errcode_t rc = sle_set_local_addr(&addr);
    CHECK_RC(rc, "sle_set_local_addr");
    struct netif *chba_net_dev = sle_chba_netdev_get();
    if (chba_net_dev != NULL) {
        (void)memcpy_s(chba_net_dev->hwaddr, SLE_ADDR_LEN, addr.addr, SLE_ADDR_LEN);
        chba_net_dev->hwaddr[0] = chba_net_dev->hwaddr[0] & 0xFE;
    }
}

int sle_chba_sample_init(void)
{
    uint16_t data_len = sizeof(chba_cfg_t);
    errcode_t rc = uapi_nv_read(NV_ID_CHBA_MODE_CFG, sizeof(chba_cfg_t), &data_len, (uint8_t *)&g_chba_cfg);
    CHECK_RC(rc, "uapi_nv_read");
    if (g_chba_cfg.role_idx > SLE_CHBA_LINK_NUM_MAX) {
        return 0;
    }
#ifndef ACHBA_SUPPORT
    g_chba_cfg.chba_mode = 0;
#endif
    sle_chba_user_links_clean();
    if (enable_sle() != ERRCODE_SUCC) {
        chba_err_log("enable_sle fail:0x%x\r\n", rc);
    }
#if CHBA_LWIP_SWITCH
    rc = chba_adapter_netdev_init();
    CHECK_RC(rc, "chba_adapter_netdev_init");
    sle_chba_set_netdev_addr();
#endif
    uint8_t chba_role = CHBA_ROLE_STA;
    if (g_chba_cfg.role_idx == CHBA_AP_IDX) {
        chba_role = CHBA_ROLE_AP;
    }
    rc = sle_chba_netdev_create(chba_role, g_chba_cfg.chba_mode);
    CHECK_RC(rc, "sle_chba_netdev_create");
    rc = sle_chba_user_register_cbks();
    CHECK_RC(rc, "sle_chba_user_register_cbks");
#if CHBA_LWIP_SWITCH
    rc = sle_chba_sample_netdev_register_callbacks();
    CHECK_RC(rc, "sle_chba_sample_netdev_register_callbacks");
#endif
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
    uapi_syschannel_vlwip_netif_init("sle0");
#endif
#ifdef ACHBA_SUPPORT
    if (g_chba_cfg.chba_mode == 1) {
        return 0;
    }
#endif
    sle_chba_sample_user_init_start_discovery(g_chba_cfg);
    sle_chba_sample_recv_pkt_thread();
    return 0;
}

#define SLE_CHBA_SPEED_TASK_PRIO 26
#define SLE_CHBA_SPEED_STACK_SIZE 0x800
static void chba_speed_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_chba_sample_init, 0,
        "chbaTask", SLE_CHBA_SPEED_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_CHBA_SPEED_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(chba_speed_entry);