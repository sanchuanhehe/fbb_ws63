/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os list source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include "os_list.h"

void *liteos_adapter_list_node_new(uint32_t len)
{
    void *node = NULL;
    node = (void *)osal_kmalloc(len, 0);
    if (node == NULL) {
        return NULL;
    }
    if (memset_s(node, len, 0, len) != EOK) {
        osal_kfree(node);
        return NULL;
    }
    return node;
}
void *liteos_adapter_list_node_create(uint32_t len)
{
    uint32_t len_cpy = len;
    void *node = NULL;
    len_cpy += (uint32_t)(sizeof(void *) << 1);
    node = liteos_adapter_list_node_new(len_cpy);
    if (node == NULL) {
        return NULL;
    }
    return lget(node);
}

static void *liteos_adapter_list_node_check(struct liteos_list_stru *list, const void *p)
{
    void *node = list->head;
    while (node && (node != p)) {
        node = lnext(node);
    }
    return node;
}

void *liteos_adapter_list_remove(struct liteos_list_stru *list, void *node)
{
    void *next = lnext(node);
    void *prev = NULL;

    if (liteos_adapter_list_node_check(list, node) == NULL) {
        return NULL;
    }

    if ((prev = lprev(node)) != NULL) {
        lnext(prev) = next;
    } else {
        list->head = next;
    }
    if (next != NULL) {
        lprev(next) = prev;
    } else {
        list->tail = prev;
    }

    lprev(node) = NULL;
    lnext(node) = NULL;
    return next;
}

void liteos_adapter_list_add_tail(struct liteos_list_stru *list, void *node)
{
    if (list->tail == NULL) {
        list->head = node;
    } else {
        lnext(list->tail) = node;
        lprev(node) = list->tail;
        lnext(node) = NULL;
    }
    list->tail = node;
}
void *liteos_adapter_list_remove_head(struct liteos_list_stru *list)
{
    void *node = NULL;
    if ((node = list->head) != NULL) {
        if ((list->head = lnext(node)) != NULL) {
            lprev(list->head) = NULL;
        } else {
            list->tail = NULL;
        }
    }

    if (node != NULL) {
        lnext(node) = NULL;
    }
    return node;
}

void liteos_adapter_list_insert_before(struct liteos_list_stru *list, void *next, void *node)
{
    if (next == NULL) {
        liteos_adapter_list_add_tail(list, node);
    } else {
        if (list->head == next) {
            list->head = node;
        } else {
            lnext(lprev(next)) = node;
        }
        lnext(node) = next;
        lprev(node) = lprev(next);
        lprev(next) = node;
    }
}

uint32_t liteos_adapter_list_node_count(struct liteos_list_stru *list)
{
    void *node = list->head;
    uint32_t count = 0;

    while (node != NULL) {
        count++; /* Assert could not overflow */
        node = lnext(node);
    }

    return count;
}