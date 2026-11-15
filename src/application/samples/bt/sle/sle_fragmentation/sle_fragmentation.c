/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE fragmentation sample entry.
 */

#include "common_def.h"
#include <stddef.h>
#include "securec.h"
#include "soc_osal.h"
#include "app_init.h"
#include "sle_fragmentation_protocol.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_SERVER_SAMPLE)
#include "sle_fragmentation_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
#include "sle_fragmentation_client.h"
#endif

#define SLE_FRAGMENTATION_TASK_PRIO 28
#define SLE_FRAGMENTATION_TASK_STACK_SIZE 0x1000

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
static uint8_t g_reassembly_buffer[SLE_FRAGMENTATION_DATA_SIZE];
static uint16_t g_reassembly_length;
static uint16_t g_next_fragment;

static uint32_t sle_fragmentation_checksum(const uint8_t *data, uint16_t length)
{
    uint32_t checksum = 0;
    for (uint16_t index = 0; index < length; index++) {
        checksum += data[index];
    }
    return checksum;
}

static void sle_fragmentation_notification_cb(uint8_t client_id,
                                              uint16_t conn_id,
                                              ssapc_handle_value_t *data,
                                              errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    uint16_t header_len = (uint16_t)offsetof(sle_fragmentation_packet_t, payload);
    if ((status != ERRCODE_SUCC) || (data->data_len < header_len)) {
        osal_printk("[sle fragmentation client] invalid packet status=0x%x, length=%u\r\n", status, data->data_len);
        return;
    }
    const sle_fragmentation_packet_t *packet = (const sle_fragmentation_packet_t *)data->data;
    if ((packet->magic != SLE_FRAGMENTATION_MAGIC) || (packet->transfer_id != SLE_FRAGMENTATION_TRANSFER_ID) ||
        (packet->total != SLE_FRAGMENTATION_TOTAL_FRAGMENTS) ||
        (packet->payload_len > SLE_FRAGMENTATION_PAYLOAD_SIZE) ||
        (data->data_len != (uint16_t)(header_len + packet->payload_len))) {
        osal_printk("[sle fragmentation client] packet header rejected\r\n");
        return;
    }
    if (packet->index == 0) {
        g_reassembly_length = 0;
        g_next_fragment = 0;
    }
    if ((packet->index != g_next_fragment) ||
        ((uint32_t)g_reassembly_length + packet->payload_len > sizeof(g_reassembly_buffer))) {
        osal_printk("[sle fragmentation client] sequence error: expected=%u, received=%u\r\n", g_next_fragment,
                    packet->index);
        return;
    }
    if (memcpy_s(&g_reassembly_buffer[g_reassembly_length], sizeof(g_reassembly_buffer) - g_reassembly_length,
                 packet->payload, packet->payload_len) != EOK) {
        osal_printk("[sle fragmentation client] payload copy failed\r\n");
        return;
    }
    g_reassembly_length += packet->payload_len;
    g_next_fragment++;
    osal_printk("[sle fragmentation client] fragment received: %u/%u, total_bytes=%u\r\n", packet->index + 1,
                packet->total, g_reassembly_length);
    if (g_next_fragment == packet->total) {
        uint32_t checksum = sle_fragmentation_checksum(g_reassembly_buffer, g_reassembly_length);
        if ((g_reassembly_length == SLE_FRAGMENTATION_DATA_SIZE) && (checksum == packet->checksum)) {
            osal_printk("[sle fragmentation client] reassembly complete: bytes=%u, checksum=%u\r\n",
                        g_reassembly_length, checksum);
            osal_printk("[sle fragmentation client] test passed\r\n");
        } else {
            osal_printk("[sle fragmentation client] checksum failed: bytes=%u, actual=%u, expected=%u\r\n",
                        g_reassembly_length, checksum, packet->checksum);
        }
    }
}

static void sle_fragmentation_indication_cb(uint8_t client_id,
                                            uint16_t conn_id,
                                            ssapc_handle_value_t *data,
                                            errcode_t status)
{
    sle_fragmentation_notification_cb(client_id, conn_id, data, status);
}

static void sle_fragmentation_read_cfm_cb(uint8_t client_id,
                                          uint16_t conn_id,
                                          ssapc_handle_value_t *read_data,
                                          errcode_t status)
{
    unused(client_id);
    unused(read_data);
    osal_printk("[sle fragmentation client] read cfm status=0x%x, request transfer\r\n", status);
    if (status == ERRCODE_SUCC) {
        sle_fragmentation_client_send_write_req(conn_id);
    }
}

static void sle_fragmentation_write_cfm_cb(uint8_t client_id,
                                           uint16_t conn_id,
                                           ssapc_write_result_t *write_result,
                                           errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    if (status == ERRCODE_SUCC) {
        osal_printk("[sle fragmentation client] transfer request accepted, handle=0x%02x\r\n", write_result->handle);
    } else {
        osal_printk("[sle fragmentation client] transfer request failed, status=0x%x\r\n", status);
    }
}
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_SERVER_SAMPLE)
static void *sle_fragmentation_server_task(const char *arg)
{
    unused(arg);
    osal_printk("[sle fragmentation server] task start\r\n");
    sle_fragmentation_server_init();
    osal_printk("[sle fragmentation server] waiting for connection\r\n");
    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
static void *sle_fragmentation_client_task(const char *arg)
{
    unused(arg);
    osal_printk("[sle fragmentation client] task start\r\n");
    sle_fragmentation_client_init(sle_fragmentation_notification_cb, sle_fragmentation_indication_cb,
                                  sle_fragmentation_read_cfm_cb, sle_fragmentation_write_cfm_cb);
    osal_printk("[sle fragmentation client] waiting for data\r\n");
    return NULL;
}
#endif

static void sle_fragmentation_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_SERVER_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_fragmentation_server_task, 0, "SLEFragmentServer",
                                      SLE_FRAGMENTATION_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_fragmentation_client_task, 0, "SLEFragmentClient",
                                      SLE_FRAGMENTATION_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_FRAGMENTATION_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(sle_fragmentation_entry);
