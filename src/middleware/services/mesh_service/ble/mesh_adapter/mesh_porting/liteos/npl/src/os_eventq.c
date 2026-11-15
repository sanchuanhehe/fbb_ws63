/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os event source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include <stdint.h>
#include <string.h>

#include "nimble/nimble_npl.h"
#include "systick.h"
#include "os_list.h"
#include "soc_osal.h"
#include "osal_semaphore.h"
#include "osal_mutex.h"
#include "mesh/glue.h"
static osal_mutex g_mutex;
static bool g_inited = false;
static osal_semaphore g_semaphore;

static bool ble_eventq_init_sem(void)
{
    if (osal_mutex_init(&g_mutex) != OSAL_SUCCESS) {
        return false;
    }
    if (osal_sem_init(&g_semaphore, 0) != OSAL_SUCCESS) {
        BT_ERR("[err] g_semaphore_inited fail");
        osal_mutex_destroy(&g_mutex);
        return false;
    }
    g_inited = true;
    return true;
}

uint32_t ble_eventq_enter_critical(void)
{
    if (!g_inited) {
        BT_ERR("[err] g_mutex uninited");
        if (!ble_eventq_init_sem()) {
            BT_ERR("[err] g_semaphore_inited fail");
            return OSAL_FAILURE;
        }
    }

    return osal_mutex_lock(&g_mutex);
}

uint32_t ble_eventq_enter_critical_timeout(uint32_t timeout)
{
    if (!g_inited) {
        BT_ERR("[err] g_mutex uninited");
        if (!ble_eventq_init_sem()) {
            BT_ERR("[err] g_semaphore_inited fail");
            return OSAL_FAILURE;
        }
    }
    return osal_mutex_lock_timeout(&g_mutex, timeout);
}
void ble_eventq_exit_critical(uint32_t ctx)
{
    osal_mutex_unlock(&g_mutex);
}

struct ble_npl_event *ble_npl_new_list_node(void)
{
    struct ble_npl_event *ev = (struct ble_npl_event *)osal_kmalloc(sizeof(struct ble_npl_event), 0);
    if (ev == NULL) {
        BT_ERR("[ERROR]ble npl new list node fail");
        return NULL;
    }
    return ev;
}

void ble_npl_free_list_node(struct ble_npl_event *ev)
{
    osal_kfree(ev);
}

void ble_npl_list_put_tail(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    BT_DBG("ble_npl_list_put_tail");
    if (evq == NULL) {
        BT_ERR("[err] ble_npl_list_put_tail err, evq == NULL");
        return;
    }
    if (ev == NULL) {
        BT_ERR("[err] ble_npl_list_put_tail err, ev == NULL");
        return;
    }
    uint32_t ctx = 0;
    ble_eventq_enter_critical();
    osal_list_add_tail(&ev->entry, &evq->event_queue);
    ev->ev_queued = 1;
    evq->size++;
    ble_eventq_exit_critical(ctx);
    osal_sem_up(&g_semaphore);
}

void ble_npl_list_put_head(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    if (evq == NULL) {
        BT_ERR("[err] ble_npl_list_put_head err, evq == NULL");
        return;
    }
    if (ev == NULL) {
        BT_ERR("[err] ble_npl_list_put_head err, ev == NULL");
        return;
    }
    uint32_t ctx = 0;
    ble_eventq_enter_critical();
    osal_list_add(&ev->entry, &evq->event_queue);
    ev->ev_queued = 1;
    evq->size++;
    ble_eventq_exit_critical(ctx);
    osal_sem_up(&g_semaphore);
}

void ble_npl_eventq_deinit(struct ble_npl_eventq *evq)
{
    uint32_t ctx = 0;
    ble_eventq_enter_critical();
    struct osal_list_head *list_entry = NULL;
    struct osal_list_head *list_entry_tmp = NULL;
    struct ble_npl_event *ev = NULL;
    struct os_mbuf *buf = NULL;
    // 遍历删除所有节点
    osal_list_for_each_safe(list_entry, list_entry_tmp, &evq->event_queue) {
        ev = (struct ble_npl_event *)osal_list_entry(list_entry, struct ble_npl_event, entry);
        if (ev != NULL) {
            buf = ble_npl_event_get_arg(ev);
            if (buf) {
                net_buf_unref(buf);
                ble_npl_event_set_arg(ev, NULL);
            }
        }
        osal_list_del(&(ev->entry));
        evq->size--;
        BT_DBG("ble_npl_list_delete_tail evq->size = %x addr %x ev_addr %x entry:%x", evq->size, evq, ev, &(ev->entry));
    }
    BT_DBG("ble_npl_list_delete empty:%d", osal_list_empty(&evq->event_queue));
    evq->size = 0;
    ble_eventq_exit_critical(ctx);
}

void ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    if (evq == NULL) {
        return;
    }
    BT_INFO("[DEBUG] ble_npl_eventq_init start.");
    evq->size = 0;
    OSAL_INIT_LIST_HEAD(&evq->event_queue);
    evq->inited = 1;

    if (!g_inited) {
        if (!ble_eventq_init_sem()) {
            BT_ERR("[err] g_semaphore_inited fail");
            return;
        }
    }
    BT_DBG("[DEBUG] ble_npl_eventq_init ed.");
}

bool ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    if (evq == NULL) {
        BT_ERR("[err] ble_npl_eventq_is_empty err, evq == NULL");
        return true;
    }
    uint32_t q_size = evq->size;

    if (q_size > 0) {
        return 1;
    }
    return 0;
}

int ble_npl_eventq_inited(const struct ble_npl_eventq *evq)
{
    if (evq && evq->inited) {
        return 1;
    }
    return 0;
}

void ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    if (ev && ev->ev_queued) {
        return;
    }

    ev->ev_queued = 1;  // 标识事件结点已经放进队列中了
    ble_npl_list_put_tail(evq, ev);
}

static struct ble_npl_event *ble_npl_list_dequeue(struct ble_npl_eventq *evq)
{
    struct osal_list_head *head = &evq->event_queue;
    struct osal_list_head *node = evq->event_queue.next;
    if (node == head) {
        return NULL;
    }
    osal_list_del(node);

    return osal_list_entry(node, struct ble_npl_event, entry);
}

struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    struct ble_npl_event *ev = NULL;
    if (evq == NULL) {
        BT_ERR("evq is NULL");
        return ev;
    }
    ble_eventq_enter_critical();
    if (tmo) {
        while (evq->size == 0) {
            ble_eventq_exit_critical(0);
            osal_sem_down(&g_semaphore);
            ble_eventq_enter_critical();
        }
    }
    if (evq->size != 0) {
        ev = ble_npl_list_dequeue(evq);
        evq->size--;
    }
    if (ev) {
        ev->ev_queued = 0;
    }
    
    ble_eventq_exit_critical(0);
    return ev;
}

void ble_npl_eventq_run(struct ble_npl_eventq *evq)
{
    struct ble_npl_event *ev;

    ev = ble_npl_eventq_get(evq, BLE_NPL_TIME_FOREVER);
    if (ev == NULL) {
        BT_ERR("evq is NULL");
        return;
    }
    ble_npl_event_run(ev);
}

// ========================================================================
//                         Event Implementation
// ========================================================================

void ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn, void *arg)
{
    if (ev == NULL) {
        return;
    }
    memset_s(ev, sizeof(*ev), 0, sizeof(*ev));
    ev->ev_cb = fn;
    ev->ev_arg = arg;
}

bool ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return ev != NULL && ev->ev_queued;
}

void *ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return ev ? ev->ev_arg : NULL;
}

void ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    if (ev) {
        ev->ev_arg = arg;
    }
}

void ble_npl_event_run(struct ble_npl_event *ev)
{
    if (ev && ev->ev_cb) {
        ev->ev_cb(ev);
    }
}

void ble_npl_list_remove_node(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    uint32_t ctx = 0;
    ble_eventq_enter_critical();
    struct osal_list_head *list_entry = NULL;
    struct osal_list_head *list_entry_tmp = NULL;
    struct ble_npl_event *ev_tmp = NULL;

    // 遍历删除所有节点
    osal_list_for_each_safe(list_entry, list_entry_tmp, &evq->event_queue) {
        ev_tmp = (struct ble_npl_event *)osal_list_entry(list_entry, struct ble_npl_event, entry);
        if (ev_tmp == ev) {
            osal_list_del(&(ev->entry));
            evq->size--;
            BT_DBG("ble_npl_list_remove_node evq->size %d. evq %x", evq->size, evq);
            break;
        }
    }

    ble_eventq_exit_critical(ctx);
}

void ble_npl_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    if (!ev->ev_queued) {
        return;
    }
    ev->ev_queued = 0;
    ble_npl_list_remove_node(evq, ev);
}
