/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os list config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef OS_LIST_H
#define OS_LIST_H

#include "adapter.h"

struct liteos_list_stru {
    void *head;
    void *tail;
};

#define lget(n)            ((void **)(n) + (1 * 2))
#define lstart(n)          ((void **)(n) - (1 * 2))
#define lnext(n)           (*((void **)(n) - (1 * 2)))
#define lprev(n)           (*((void **)(n) - 1))

void *liteos_adapter_list_node_new(uint32_t len);
void *liteos_adapter_list_node_create(uint32_t len);
void *liteos_adapter_list_remove(struct liteos_list_stru *list, void *node);
void liteos_adapter_list_add_tail(struct liteos_list_stru *list, void *node);
void *liteos_adapter_list_remove_head(struct liteos_list_stru *list);
void liteos_adapter_list_insert_before(struct liteos_list_stru *list, void *next, void *node);
uint32_t liteos_adapter_list_node_count(struct liteos_list_stru *list);

#define liteos_list_node_free(n) (bt_os_free(lstart(n)))
#define liteos_list_node_new(size) (liteos_adapter_list_node_create(size))
#endif
