/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE CHBA LINK MANAGER
 */
#include "sle_chba_server.h"
#include "securec.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"

sle_chba_user_link_t g_sle_chba_user_link_list[SLE_CHBA_LINK_NUM_MAX] = {0};

static void sle_chba_user_link_clean(sle_chba_user_link_t *link_info)
{
    (void)memset_s(link_info, sizeof(sle_chba_user_link_t), 0, sizeof(sle_chba_user_link_t));
    link_info->conn_id = CHBA_INVALID_CONN_ID;
    link_info->status = STATUS_IDLE;
    link_info->link_param.interval = 0xFFFF;
    link_info->link_param.phy = SLE_PHY_1M;
    link_info->link_param.mcs = 0xFF;
    link_info->link_ready = false;
}

sle_chba_user_link_t *sle_chba_user_get_new_linkinfo(void)
{
    for (uint8_t i = 0; i < SLE_CHBA_LINK_NUM_MAX; i++) {
        if (g_sle_chba_user_link_list[i].conn_id == CHBA_INVALID_CONN_ID) {
            sle_chba_user_link_clean(&g_sle_chba_user_link_list[i]);
            return &g_sle_chba_user_link_list[i];
        }
    }
    return NULL;
}

sle_chba_user_link_t *sle_chba_user_get_linkinfo_by_connid(uint16_t conn_id)
{
    if (conn_id == CHBA_INVALID_CONN_ID) {
        return NULL;
    }
    for (uint8_t i = 0; i < SLE_CHBA_LINK_NUM_MAX; i++) {
        if (g_sle_chba_user_link_list[i].conn_id == conn_id) {
            return &g_sle_chba_user_link_list[i];
        }
    }
    return NULL;
}

void sle_chba_user_del_linkinfo_by_connid(uint16_t conn_id)
{
    for (uint8_t i = 0; i < SLE_CHBA_LINK_NUM_MAX; i++) {
        if (g_sle_chba_user_link_list[i].conn_id == conn_id) {
            sle_chba_user_link_clean(&g_sle_chba_user_link_list[i]);
            return;
        }
    }
}

void sle_chba_user_links_clean(void)
{
    for (uint8_t i = 0; i < SLE_CHBA_LINK_NUM_MAX; i++) {
        sle_chba_user_link_clean(&g_sle_chba_user_link_list[i]);
    }
}