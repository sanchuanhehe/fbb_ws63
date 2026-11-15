/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements the BLE UART bridge entry, bounded buffering, and transfer worker.
 * @else
 * @brief 实现 BLE UART 透传入口、有界缓冲和传输任务。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#include "app_init.h"
#include "bts_def.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "uart.h"
#include "ble_uart_bridge.h"

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_SERVER_SAMPLE)
#include "ble_uart_bridge_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_CLIENT_SAMPLE)
#include "ble_uart_bridge_client.h"
#endif

/* Worker task configuration. / 工作任务配置。 */
#define BLE_UART_BRIDGE_TASK_PRIO 26
#define BLE_UART_BRIDGE_TASK_STACK_SIZE 0x2000

/* UART1 hardware mapping and frame format. / UART1 硬件映射与帧格式。 */
#define BLE_UART_BRIDGE_UART_BUS UART_BUS_1
#define BLE_UART_BRIDGE_UART_TX_PIN S_MGPIO15
#define BLE_UART_BRIDGE_UART_RX_PIN S_MGPIO16
#define BLE_UART_BRIDGE_UART_TX_MODE PIN_MODE_1
#define BLE_UART_BRIDGE_UART_RX_MODE PIN_MODE_1
#define BLE_UART_BRIDGE_UART_BAUDRATE 115200

/*
 * Each 1024-byte ring keeps one slot empty, so each direction can queue at most 1023 bytes.
 * 每个 1024 字节环形队列固定保留一个空槽，因此每个方向最多排队 1023 字节。
 */
#define BLE_UART_BRIDGE_UART_DRIVER_BUFFER_SIZE BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN
#define BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE 1024U
#define BLE_UART_BRIDGE_UART_QUEUE_CAPACITY (BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE - 1U)
#define BLE_UART_BRIDGE_BLE_RETRY_DELAY_MS 20
#define BLE_UART_BRIDGE_UART_RETRY_DELAY_MS 5

/* UART driver storage, two bounded queues, and cross-context state. / UART 驱动存储、双向有界队列及跨上下文状态。 */
static uint8_t g_uart_driver_buffer[BLE_UART_BRIDGE_UART_DRIVER_BUFFER_SIZE];
static uint8_t g_uart_rx_queue[BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE];
static volatile uint16_t g_uart_rx_queue_head;
static volatile uint16_t g_uart_rx_queue_tail;
static volatile uint32_t g_uart_rx_dropped_frames;
static volatile uint32_t g_uart_rx_dropped_bytes;
static uint8_t g_uart_tx_queue[BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE];
static uint8_t g_uart_tx_staging_buffer[BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN];
static volatile uint16_t g_uart_tx_queue_head;
static volatile uint16_t g_uart_tx_queue_tail;
static volatile uint32_t g_uart_tx_dropped_frames;
static volatile uint32_t g_uart_tx_dropped_bytes;
static volatile bool g_ble_send_pending;
static volatile bool g_ble_retry_required;
static volatile uint16_t g_ble_send_pending_length;
static osal_semaphore g_worker_sem;
static volatile bool g_worker_event_pending;
static volatile bool g_worker_sem_ready;

/**
 * @if Eng
 * @brief Returns the number of bytes currently stored in either UART bridge queue.
 * @param [in] head Queue producer index.
 * @param [in] tail Queue consumer index.
 * @return Number of queued bytes.
 * @else
 * @brief 返回任一 UART 透传队列中当前保存的字节数。
 * @param [in] head 队列生产者索引。
 * @param [in] tail 队列消费者索引。
 * @return 已排队字节数。
 * @endif
 */
static uint16_t ble_uart_bridge_uart_queue_count(uint16_t head, uint16_t tail)
{
    if (head >= tail) {
        return (uint16_t)(head - tail);
    }
    return (uint16_t)(BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE - tail + head);
}

/**
 * @if Eng
 * @brief Advances a UART bridge queue index inside the fixed-size storage.
 * @param [in] index Current queue index.
 * @param [in] length Number of bytes to advance.
 * @return Wrapped queue index.
 * @else
 * @brief 在固定大小存储区内移动 UART 透传队列索引。
 * @param [in] index 当前队列索引。
 * @param [in] length 需要移动的字节数。
 * @return 回绕后的队列索引。
 * @endif
 */
static uint16_t ble_uart_bridge_uart_queue_advance(uint16_t index, uint16_t length)
{
    return (uint16_t)((index + length) % BLE_UART_BRIDGE_UART_QUEUE_STORAGE_SIZE);
}

/**
 * @if Eng
 * @brief Wakes the bridge worker once without accumulating redundant semaphore tokens.
 * @else
 * @brief 唤醒一次透传任务，并避免累积无意义的信号量计数。
 * @endif
 */
static void ble_uart_bridge_worker_wake(void)
{
    if (!g_worker_sem_ready || g_worker_event_pending) {
        return;
    }
    g_worker_event_pending = true;
    osal_sem_up(&g_worker_sem);
}

/**
 * @if Eng
 * @brief Copies queued UART bytes without consuming them before BLE completion.
 * @param [out] data Destination buffer.
 * @param [in] capacity Destination capacity in bytes.
 * @return Number of copied bytes.
 * @else
 * @brief 在 BLE 完成前预读 UART 队列数据，不提前消费。
 * @param [out] data 目标缓冲区。
 * @param [in] capacity 目标缓冲区容量，单位为字节。
 * @return 已复制字节数。
 * @endif
 */
static uint16_t ble_uart_bridge_uart_queue_peek(uint8_t *data, uint16_t capacity)
{
    uint16_t head = g_uart_rx_queue_head;
    uint16_t index = g_uart_rx_queue_tail;
    uint16_t length = 0;

    while (index != head && length < capacity) {
        data[length++] = g_uart_rx_queue[index];
        index = ble_uart_bridge_uart_queue_advance(index, 1);
    }
    return length;
}

/**
 * @if Eng
 * @brief Reports both bounded queue overflow counters from task context.
 * @else
 * @brief 在任务上下文中上报双向有界队列的溢出统计。
 * @endif
 */
static void ble_uart_bridge_report_uart_overflow(void)
{
    static uint32_t reported_rx_frames;
    static uint32_t reported_rx_bytes;
    static uint32_t reported_tx_frames;
    static uint32_t reported_tx_bytes;
    uint32_t dropped_rx_frames = g_uart_rx_dropped_frames;
    uint32_t dropped_rx_bytes = g_uart_rx_dropped_bytes;
    uint32_t dropped_tx_frames = g_uart_tx_dropped_frames;
    uint32_t dropped_tx_bytes = g_uart_tx_dropped_bytes;

    if (dropped_rx_frames != reported_rx_frames || dropped_rx_bytes != reported_rx_bytes) {
        reported_rx_frames = dropped_rx_frames;
        reported_rx_bytes = dropped_rx_bytes;
        osal_printk("[ble uart bridge] UART RX queue overflow: frames=%u, bytes=%u, capacity=%u\r\n",
                    dropped_rx_frames, dropped_rx_bytes, BLE_UART_BRIDGE_UART_QUEUE_CAPACITY);
    }
    if (dropped_tx_frames != reported_tx_frames || dropped_tx_bytes != reported_tx_bytes) {
        reported_tx_frames = dropped_tx_frames;
        reported_tx_bytes = dropped_tx_bytes;
        osal_printk("[ble uart bridge] UART TX queue overflow: frames=%u, bytes=%u, capacity=%u\r\n",
                    dropped_tx_frames, dropped_tx_bytes, BLE_UART_BRIDGE_UART_QUEUE_CAPACITY);
    }
}

/**
 * @if Eng
 * @brief Queues one complete UART RX callback fragment for the bridge worker.
 * @param [in] buffer UART driver receive buffer.
 * @param [in] length Number of received bytes.
 * @param [in] error Whether the UART driver reported an error.
 * @else
 * @brief 将一段完整 UART 接收回调数据加入队列，等待透传任务处理。
 * @param [in] buffer UART 驱动接收缓冲区。
 * @param [in] length 接收字节数。
 * @param [in] error UART 驱动是否上报错误。
 * @endif
 */
static void ble_uart_bridge_uart_rx_cb(const void *buffer, uint16_t length, bool error)
{
    const uint8_t *source = (const uint8_t *)buffer;
    uint16_t free_length;
    uint16_t head;
    uint16_t index;

    if (error || source == NULL || length == 0) {
        return;
    }

    head = g_uart_rx_queue_head;
    free_length = (uint16_t)(BLE_UART_BRIDGE_UART_QUEUE_CAPACITY -
                             ble_uart_bridge_uart_queue_count(head, g_uart_rx_queue_tail));
    /* Reject the whole callback fragment when the bounded queue cannot preserve it. / 空间不足时整段丢弃，避免破坏字节顺序。 */
    if (length > free_length) {
        g_uart_rx_dropped_frames++;
        g_uart_rx_dropped_bytes += length;
        ble_uart_bridge_worker_wake();
        return;
    }

    for (index = 0; index < length; index++) {
        g_uart_rx_queue[head] = source[index];
        head = ble_uart_bridge_uart_queue_advance(head, 1);
    }
    /* Publish head only after the complete fragment is stored. / 完整写入后再发布头指针，任务不会读取半成品。 */
    g_uart_rx_queue_head = head;
    ble_uart_bridge_worker_wake();
}

/**
 * @if Eng
 * @brief Queues one complete BLE fragment for task-context UART1 transmission.
 * @param [in] data BLE payload.
 * @param [in] length Payload length.
 * @return ERRCODE_BT_SUCCESS on complete enqueue, ERRCODE_BT_BUSY when the bounded queue is full,
 *         otherwise ERRCODE_BT_FAIL.
 * @else
 * @brief 将一段完整 BLE 数据加入有界队列，等待任务上下文写入 UART1。
 * @param [in] data BLE 数据。
 * @param [in] length 数据长度。
 * @return 完整入队返回 ERRCODE_BT_SUCCESS，队列空间不足返回 ERRCODE_BT_BUSY，其他情况返回 ERRCODE_BT_FAIL。
 * @endif
 */
errcode_t ble_uart_bridge_uart_enqueue(const uint8_t *data, uint16_t length)
{
    uint16_t free_length;
    uint16_t head;
    uint16_t index;

    if (data == NULL || length == 0 || length > BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN) {
        return ERRCODE_BT_FAIL;
    }

    head = g_uart_tx_queue_head;
    free_length = (uint16_t)(BLE_UART_BRIDGE_UART_QUEUE_CAPACITY -
                             ble_uart_bridge_uart_queue_count(head, g_uart_tx_queue_tail));
    /* Reject the whole BLE fragment when the bounded queue cannot preserve its byte order. */
    if (length > free_length) {
        g_uart_tx_dropped_frames++;
        g_uart_tx_dropped_bytes += length;
        ble_uart_bridge_worker_wake();
        return ERRCODE_BT_BUSY;
    }

    for (index = 0; index < length; index++) {
        g_uart_tx_queue[head] = data[index];
        head = ble_uart_bridge_uart_queue_advance(head, 1);
    }
    /* Publish the producer index only after the complete BLE fragment has been copied. */
    g_uart_tx_queue_head = head;
    ble_uart_bridge_worker_wake();
    return ERRCODE_BT_SUCCESS;
}

/**
 * @if Eng
 * @brief Writes one queued BLE fragment to UART1 and consumes only bytes accepted by the driver.
 * @return true when bytes were consumed, otherwise false.
 * @else
 * @brief 将一段已排队 BLE 数据写入 UART1，并且只消费驱动实际接收的字节。
 * @return 有字节被消费返回 true，否则返回 false。
 * @endif
 */
static bool ble_uart_bridge_process_uart_tx(void)
{
    uint16_t head = g_uart_tx_queue_head;
    uint16_t index = g_uart_tx_queue_tail;
    uint16_t length = 0;
    int32_t written;

    /* Copy a task-owned fragment so a ring wrap never changes UART byte order. */
    while (index != head && length < (uint16_t)sizeof(g_uart_tx_staging_buffer)) {
        g_uart_tx_staging_buffer[length++] = g_uart_tx_queue[index];
        index = ble_uart_bridge_uart_queue_advance(index, 1);
    }
    if (length == 0) {
        return false;
    }

    written = uapi_uart_write(BLE_UART_BRIDGE_UART_BUS, g_uart_tx_staging_buffer, length, 0);
    if (written <= 0) {
        /* Back off only after a UART rejection; normal traffic has no fixed polling delay. */
        osal_msleep(BLE_UART_BRIDGE_UART_RETRY_DELAY_MS);
        return false;
    }
    if (written > (int32_t)length) {
        written = (int32_t)length;
    }
    g_uart_tx_queue_tail = ble_uart_bridge_uart_queue_advance(g_uart_tx_queue_tail, (uint16_t)written);
    return true;
}

/**
 * @if Eng
 * @brief Completes the current UART-to-BLE fragment and releases it only on success.
 * @param [in] status BLE operation result.
 * @else
 * @brief 完成当前 UART 到 BLE 分片，并且仅在成功时释放数据。
 * @param [in] status BLE 操作结果。
 * @endif
 */
void ble_uart_bridge_ble_send_complete(errcode_t status)
{
    uint16_t length;

    if (!g_ble_send_pending) {
        return;
    }

    length = g_ble_send_pending_length;
    if (status == ERRCODE_BT_SUCCESS) {
        /* The peer accepted the fragment, so its bytes can now leave the queue. / 对端成功接收后才消费队列数据。 */
        g_uart_rx_queue_tail = ble_uart_bridge_uart_queue_advance(g_uart_rx_queue_tail, length);
        g_ble_retry_required = false;
    } else {
        /* Keep failed bytes at the queue tail and retry after a short error backoff. / 失败数据保留在队首，退避后重试。 */
        g_ble_retry_required = true;
    }
    g_ble_send_pending_length = 0;
    g_ble_send_pending = false;
    ble_uart_bridge_worker_wake();
}

/**
 * @if Eng
 * @brief Configures UART1 with the callback-based receive path and bounded queue.
 * @return ERRCODE_SUCC on success, otherwise the driver error code.
 * @else
 * @brief 使用回调接收方式和有界队列配置 UART1。
 * @return 成功返回 ERRCODE_SUCC，否则返回驱动错误码。
 * @endif
 */
static errcode_t ble_uart_bridge_uart_init(void)
{
    uart_attr_t attr = {
        .baud_rate = BLE_UART_BRIDGE_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_pin_config_t pins = {
        .tx_pin = BLE_UART_BRIDGE_UART_TX_PIN,
        .rx_pin = BLE_UART_BRIDGE_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uart_buffer_config_t buffer = {
        .rx_buffer = g_uart_driver_buffer,
        .rx_buffer_size = sizeof(g_uart_driver_buffer)
    };
    errcode_t ret;

    ret = uapi_pin_set_mode(BLE_UART_BRIDGE_UART_TX_PIN, BLE_UART_BRIDGE_UART_TX_MODE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_pin_set_mode(BLE_UART_BRIDGE_UART_RX_PIN, BLE_UART_BRIDGE_UART_RX_MODE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* No DMA configuration is supplied; the sample uses the UART driver's basic mode. */
    (void)uapi_uart_deinit(BLE_UART_BRIDGE_UART_BUS);
    ret = uapi_uart_init(BLE_UART_BRIDGE_UART_BUS, &pins, &attr, NULL, &buffer);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_uart_register_rx_callback(BLE_UART_BRIDGE_UART_BUS,
                                         UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                         1,
                                         ble_uart_bridge_uart_rx_cb);
    if (ret == ERRCODE_SUCC) {
        osal_printk("[ble uart bridge] UART1 ready: IRQ mode, RX queue=%u, TX queue=%u, no DMA, "
                    "TX=GPIO15 RX=GPIO16 115200 8N1\r\n",
                    BLE_UART_BRIDGE_UART_QUEUE_CAPACITY, BLE_UART_BRIDGE_UART_QUEUE_CAPACITY);
    }
    return ret;
}

/**
 * @if Eng
 * @brief Blocks the worker until UART input or BLE completion requires processing.
 * @else
 * @brief 阻塞工作任务，直到 UART 输入或 BLE 完成事件需要处理。
 * @endif
 */
static void ble_uart_bridge_worker_wait(void)
{
    if (osal_sem_down(&g_worker_sem) != OSAL_SUCCESS) {
        osal_printk("[ble uart bridge] worker semaphore wait failed\r\n");
    }
    /* Allow the next producer or completion callback to publish one new wake event. / 允许下一个生产或完成回调发布新的唤醒事件。 */
    g_worker_event_pending = false;
}

/**
 * @if Eng
 * @brief Initializes the selected BLE role and forwards UART fragments.
 * @param [in] arg Unused task argument.
 * @return Task exit status.
 * @else
 * @brief 初始化所选 BLE 角色并转发 UART 数据段。
 * @param [in] arg 未使用的任务参数。
 * @return 任务退出状态。
 * @endif
 */
static errcode_t ble_uart_bridge_role_init(void)
{
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_SERVER_SAMPLE)
    return ble_uart_bridge_server_init();
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_CLIENT_SAMPLE)
    return ble_uart_bridge_client_init();
#else
    return ERRCODE_FAIL;
#endif
}

static void ble_uart_bridge_process_ble_tx(void)
{
    uint8_t data[BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN];
    uint16_t length;
    errcode_t ret;
    if (g_ble_send_pending) {
        return;
    }
    length = ble_uart_bridge_uart_queue_peek(data, (uint16_t)sizeof(data));
    if (length == 0) {
        return;
    }
    g_ble_send_pending_length = length;
    g_ble_send_pending = true;
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_SERVER_SAMPLE)
    ret = ble_uart_bridge_server_send_notification(data, length);
#else
    ret = ble_uart_bridge_client_send_write(data, length);
#endif
    if (ret != ERRCODE_BT_SUCCESS && g_ble_send_pending) {
        ble_uart_bridge_ble_send_complete(ERRCODE_BT_FAIL);
    }
}

static bool ble_uart_bridge_work_pending(void)
{
    return g_uart_tx_queue_head != g_uart_tx_queue_tail ||
           (!g_ble_send_pending && g_uart_rx_queue_head != g_uart_rx_queue_tail);
}

static int ble_uart_bridge_task(const char *arg)
{
    errcode_t ret;

    (void)arg;
    if (osal_sem_init(&g_worker_sem, 0) != OSAL_SUCCESS) {
        osal_printk("[ble uart bridge] worker semaphore init failed\r\n");
        return (int)ERRCODE_FAIL;
    }
    /* Initialize the wake path before registering the interrupt-context UART callback. / 注册 UART 中断回调前先启用任务唤醒通道。 */
    g_worker_sem_ready = true;
    ret = ble_uart_bridge_uart_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("[ble uart bridge] UART init failed: 0x%x\r\n", ret);
        g_worker_sem_ready = false;
        osal_sem_destroy(&g_worker_sem);
        return (int)ret;
    }
    ret = ble_uart_bridge_role_init();
    if (ret != ERRCODE_SUCC) {
        (void)uapi_uart_deinit(BLE_UART_BRIDGE_UART_BUS);
        g_worker_sem_ready = false;
        osal_sem_destroy(&g_worker_sem);
        return (int)ret;
    }

    while (1) {
        ble_uart_bridge_report_uart_overflow();
        /* Drain peer data in task context even while a UART-to-BLE fragment awaits confirmation. */
        (void)ble_uart_bridge_process_uart_tx();

        if (!g_ble_send_pending && g_ble_retry_required) {
            /*
             * Back off only after a BLE rejection; normal traffic has no fixed polling delay.
             * 仅 BLE 拒绝后退避，正常流量无固定轮询延时。
             */
            g_ble_retry_required = false;
            osal_msleep(BLE_UART_BRIDGE_BLE_RETRY_DELAY_MS);
        }

        ble_uart_bridge_process_ble_tx();

        /* Continue immediately while either bounded queue still contains processable data. */
        if (ble_uart_bridge_work_pending()) {
            continue;
        }
        ble_uart_bridge_worker_wait();
    }
}

/**
 * @if Eng
 * @brief Creates the BLE UART bridge task.
 * @else
 * @brief 创建 BLE UART 透传任务。
 * @endif
 */
static void ble_uart_bridge_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ble_uart_bridge_task, NULL, "ble_uart_bridge",
                                      BLE_UART_BRIDGE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_UART_BRIDGE_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(ble_uart_bridge_entry);
