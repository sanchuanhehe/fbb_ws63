/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: wlan util notifier
 * Create: 2022-05-20
 */
#include "frw_util_notifier.h"
#include "oal_mem_hcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct osal_list_head g_event_list_head[WLAN_UTIL_NOTIFIER_EVENT_MAX];
osal_u8 g_event_list_depth[WLAN_UTIL_NOTIFIER_EVENT_MAX];
osal_bool g_inited_flag = OSAL_FALSE;
osal_spinlock g_event_list_lock;

#define FRW_UTIL_NOTIFIER_MAX_EVENT_DEPTH 15

osal_u32 frw_util_notifier_notify(wlan_util_notifier_event_en event, osal_void *notify_data)
{
    struct osal_list_head *entry = OSAL_NULL;
    notifier_node_stru *notifier_node = OSAL_NULL;
    wlan_util_notifier_hook_func hook_func_list[FRW_UTIL_NOTIFIER_MAX_EVENT_DEPTH] = {0};
    osal_u8 index = 0;
    osal_u8 i;

    if (event < 0 || event >= WLAN_UTIL_NOTIFIER_EVENT_MAX) {
        return OSAL_FAILURE;
    }

    osal_spin_lock(&g_event_list_lock);
    osal_list_for_each(entry, &g_event_list_head[event])
    {
        notifier_node = osal_list_entry(entry, notifier_node_stru, list_entry);
        if (notifier_node->hook_func != OSAL_NULL) {
            hook_func_list[index] = notifier_node->hook_func;
            index++;
        }
    }
    osal_spin_unlock(&g_event_list_lock);

    for (i = 0; i < index; i++) {
        if (hook_func_list[i](notify_data) != OSAL_TRUE) {
            wifi_printf("frw_util_notifier_notify,failed[%d %08x]\r\n", event, hook_func_list[i]);
        }
    }

    return OSAL_SUCCESS;
}

osal_bool frw_util_notifier_register_with_priority(wlan_util_notifier_event_en event,
    wlan_util_notifier_hook_func hook_func, osal_u32 priority)
{
    notifier_node_stru *notifier_node_new = OSAL_NULL;
    struct osal_list_head *entry = OSAL_NULL;
    struct osal_list_head *entry_next = OSAL_NULL;
    notifier_node_stru *notifier_node = OSAL_NULL;

    if (event < 0 || event >= WLAN_UTIL_NOTIFIER_EVENT_MAX) {
        return OSAL_FALSE;
    }

    if (g_event_list_depth[event] >= FRW_UTIL_NOTIFIER_MAX_EVENT_DEPTH) {
        wifi_printf("frw_util_notifier_register_with_priority failed, hook max num[%d] reached, \
            pls change limit, event[%d]\r\n", FRW_UTIL_NOTIFIER_MAX_EVENT_DEPTH, event);
        return OSAL_FALSE;
    }

    notifier_node_new = osal_kmalloc(OAL_SIZEOF(notifier_node_stru), OSAL_GFP_KERNEL);
    if (notifier_node_new == NULL) {
        wifi_printf("frw_util_notifier_register_with_priority failed\r\n");
        return OSAL_FALSE;
    }

    notifier_node_new->hook_func = hook_func;
    notifier_node_new->priority = priority;

    osal_spin_lock(&g_event_list_lock);
    if (osal_list_empty(&g_event_list_head[event]) > 0) {
        osal_list_add_tail(&notifier_node_new->list_entry, &g_event_list_head[event]);
        g_event_list_depth[event]++;
        osal_spin_unlock(&g_event_list_lock);
        return OSAL_TRUE;
    }

    /* 在队列中找到了一个节点的优先级小于新节点，则插入到该节点前面即可 */
    osal_list_for_each_safe(entry, entry_next, &g_event_list_head[event])
    {
        notifier_node = osal_list_entry(entry, notifier_node_stru, list_entry);
        if (notifier_node_new->priority > notifier_node->priority) {
            osal_list_add_tail(&notifier_node_new->list_entry, &notifier_node->list_entry);
            g_event_list_depth[event]++;
            osal_spin_unlock(&g_event_list_lock);
            return OSAL_TRUE;
        }
    }

    /* 所有节点的优先级都大于新插入的，则将改节点插入队尾 */
    osal_list_add_tail(&notifier_node_new->list_entry, &g_event_list_head[event]);
    g_event_list_depth[event]++;
    osal_spin_unlock(&g_event_list_lock);
    return OSAL_TRUE;
}

osal_bool frw_util_notifier_register(wlan_util_notifier_event_en event, wlan_util_notifier_hook_func hook_func)
{
    return frw_util_notifier_register_with_priority(event, hook_func, WLAN_UTIL_NOTIFIER_PRIORITY_DEFAULT);
}

osal_bool frw_util_notifier_unregister_with_priority(wlan_util_notifier_event_en event,
    wlan_util_notifier_hook_func hook_func, osal_u32 priority)
{
    struct osal_list_head *entry = OSAL_NULL;
    struct osal_list_head *entry_next = OSAL_NULL;
    notifier_node_stru *notifier_node = OSAL_NULL;

    if (event < 0 || event >= WLAN_UTIL_NOTIFIER_EVENT_MAX) {
        return OSAL_FALSE;
    }

    osal_spin_lock(&g_event_list_lock);
    osal_list_for_each_safe(entry, entry_next, &g_event_list_head[event])
    {
        notifier_node = osal_list_entry(entry, notifier_node_stru, list_entry);
        if (notifier_node->hook_func == hook_func && notifier_node->priority == priority) {
            osal_list_del(entry);
            osal_kfree(notifier_node);
            g_event_list_depth[event]--;
            osal_spin_unlock(&g_event_list_lock);
            return OSAL_TRUE;
        }
    }
    osal_spin_unlock(&g_event_list_lock);

    return OSAL_FALSE;
}

osal_bool frw_util_notifier_unregister(wlan_util_notifier_event_en event, wlan_util_notifier_hook_func hook_func)
{
    return frw_util_notifier_unregister_with_priority(event, hook_func, WLAN_UTIL_NOTIFIER_PRIORITY_DEFAULT);
}

osal_void frw_util_notifier_init(osal_void)
{
    osal_u32 i = 0;

    if (g_inited_flag == OSAL_TRUE) {
        return;
    }

    osal_spin_lock_init(&g_event_list_lock);

    g_inited_flag = OSAL_TRUE;
    osal_spin_lock(&g_event_list_lock);
    for (i = 0; i < sizeof(g_event_list_head) / sizeof(g_event_list_head[0]); i++) {
        OSAL_INIT_LIST_HEAD(&g_event_list_head[i]);
        g_event_list_depth[i] = 0;
    }
    osal_spin_unlock(&g_event_list_lock);
}

osal_void frw_util_notifier_exit(osal_void)
{
    osal_u32 i = 0;
    struct osal_list_head *entry = OSAL_NULL;
    struct osal_list_head *entry_next = OSAL_NULL;
    notifier_node_stru *notifier_node = OSAL_NULL;

    if (g_inited_flag == OSAL_FALSE) {
        return;
    }

    g_inited_flag = OSAL_FALSE;
    osal_spin_lock(&g_event_list_lock);
    for (i = 0; i < sizeof(g_event_list_head) / sizeof(g_event_list_head[0]); i++) {
        osal_list_for_each_safe(entry, entry_next, &g_event_list_head[i]) {
            notifier_node = osal_list_entry(entry, notifier_node_stru, list_entry);
            osal_list_del(entry);
            osal_kfree(notifier_node);
            g_event_list_depth[i]--;
        }
    }
    osal_spin_unlock(&g_event_list_lock);

    osal_spin_lock_destroy(&g_event_list_lock);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
