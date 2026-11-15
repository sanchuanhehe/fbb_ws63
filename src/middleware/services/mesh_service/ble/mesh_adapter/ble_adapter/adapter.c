/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: adapter source
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "pthread.h"
#include "os_time.h"
#include "npl_timer.h"
#include "adapter.h"

ble_npl_error_t ble_npl_callout_reset_periodic(struct ble_npl_callout *c, ble_npl_time_t ticks)
{
    struct itimerspec its;
    ble_npl_time_t remaining_ticks = ticks;
    if (remaining_ticks < 0) {
        return BLE_NPL_EINVAL;
    }

    if (remaining_ticks == 0) {
        remaining_ticks = 1;
    }

    c->c_ticks = ble_npl_time_get() + remaining_ticks;
    its.it_value.tv_sec = (ble_npl_time_ticks_to_ms32(remaining_ticks) / MS_PER_SEC);
    its.it_value.tv_nsec = (ble_npl_time_ticks_to_ms32(remaining_ticks) % MS_PER_SEC) * MS_PER_SEC * MS_PER_SEC;
    its.it_value.tv_nsec %= (MS_PER_SEC * MS_PER_SEC * MS_PER_SEC);
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;  // periodic
    c->c_active = true;
    npl_timer_settime(c->c_timer, 0, &its, NULL);

    return BLE_NPL_OK;
}

void k_work_schedule_periodic(struct k_work_delayable *w, uint32_t ms)
{
    uint32_t ticks;

    if (ble_npl_time_ms_to_ticks(ms, &ticks) != 0) {
        BT_ERR("get ticks error!");
    }
    ble_npl_callout_reset_periodic(&w->work, ticks);
}

struct k_work_delayable *k_work_delayable_from_work(struct ble_npl_event *work)
{
    return CONTAINER_OF(work, struct k_work_delayable, work);
}
 
struct os_mbuf *ble_hs_mbuf_from_flat(const void *buf, uint16_t len)
{
    int err;
    struct os_mbuf *om = os_msys_get_pkthdr(len, 0);
    if (!om) {
        return NULL;
    }
    err = os_mbuf_append(om, buf, len);
    if (err) {
        return NULL;
    }
    return om;
}

void *ble_npl_get_current_task_id(void)
{
    return (void *)pthread_self();
}

int ble_mesh_adapter_deinit(void)
{
    if (adapter_deinit() != 0) {
        BT_ERR("adapter_deinit err !");
        return -1;
    }
    mesh_deinitialized();

    return 0;
}


int ble_mesh_adapter_init(void)
{
    if (adapter_init() != 0) {
        BT_ERR("adapter_init err !");
        return -1;
    }
    mesh_initialized();

    return 0;
}

uint8_t ble_hs_hci_get_hci_version(void)
{
    return BLE_HCI_VER_BCS_5_0;
}

void ble_transport_hs_init(void)
{
}