/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: host spi driver.
 * Author: plat
 * Create: 2024-12-12
 */

#include "td_base.h"
#include "td_type.h"
#include "securec.h"
#include "hcc_if.h"
#include "hcc_list.h"
#include "hcc_bus_types.h"
#include "hcc.h"
#include "hcc_comm.h"
#include "hcc_adapt.h"
#include "hcc_queue.h"
#include "hcc_flow_ctrl.h"
#include "hcc_cfg_comm.h"
#include "hcc_cfg.h"

#include "tcxo.h"
#include "oal_hcc_bus_os_adapt.h"
#include "gpio.h"
#include "pinctrl_porting.h"
#include "spi.h"

#include "hcc_adapt_spi.h"

#define PACKET_START_SIGNAL 0x7e
#define PACKET_END_SIGNAL   0xe7

#define SPI_BUF_MAX_2K 2048
#define SPI_WAIT_TIME_OUT 0xFFFFFFFF
#define SPI_WAIT_EVENT_TIME 500
#define BYTE_BIT_LEN        8
#define SPI_MSG_LEN        8
#define SPI_MASTER_PIN_MODE 3

#define SPI_SLAVE_NUM                   1
#define SPI_CLK_POLARITY                0
#define SPI_CLK_PHASE                   0
#define SPI_FRAME_FORMAT                0
#define SPI_FRAME_FORMAT_STANDARD       0
#define SPI_FRAME_SIZE_8                0x7
#define SPI_TMOD                        0
#define SPI_WAIT_CYCLES                 0x10
#define SPI_DMA_WIDTH                   0

#define HCC_SPI_DMA_BUFFADDR_ALIGN   4
#define HCC_SPI_DMA_DATA_ALIGN   1

#define RX_THREAD_START_EVENT 1
#define TX_THREAD_START_EVENT 1

#define HCC_SPI_RX_ISR_PRIO 5

hcc_bus *g_spi_bus = NULL;

osal_atomic g_tx_state;
#define SPI_STATE_TX 1
#define SPI_STATE_RX 2
#define SPI_STATE_TX_OVER 3
#define SPI_STATE_RX_OVER 4

static int32_t hcc_spi_reinit(hcc_bus *bus)
{
    uapi_unused(bus);
    return EXT_ERR_SUCCESS;
}

static void hcc_spi_packet_data(uint8_t *buf, uint16_t len, uint8_t type, uint8_t queue_id)
{
    spi_packet_head *spi_head = (spi_packet_head *)buf;
    spi_head->packet_start = PACKET_START_SIGNAL;
    spi_head->len_h = (len >> BYTE_BIT_LEN) & 0xFF;
    spi_head->len_l = len & 0xFF;
    spi_head->packet_info.queue_id = queue_id;
    spi_head->packet_info.type = type;
}

static errcode_t hcc_handshake_for_tx(hcc_spi_priv_data *spi_priv)
{
    int cnt = 0;
    unsigned int irq_lock = osal_irq_lock();
    uapi_gpio_set_dir(spi_priv->handshake_pin, GPIO_DIRECTION_INPUT);
    uapi_tcxo_delay_us(1);
    if (uapi_gpio_get_dir(spi_priv->handshake_pin) != GPIO_DIRECTION_INPUT) {
        uapi_gpio_set_dir(spi_priv->handshake_pin, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(spi_priv->handshake_pin, GPIO_LEVEL_LOW);
        osal_irq_restore(irq_lock);
        return EXT_ERR_FAILURE;
    }
    while (cnt < HANDSHAKE_CNT) {
        if (uapi_gpio_get_val(spi_priv->handshake_pin) == GPIO_LEVEL_HIGH) {
            uapi_gpio_set_dir(spi_priv->handshake_pin, GPIO_DIRECTION_OUTPUT);
            uapi_gpio_set_val(spi_priv->handshake_pin, GPIO_LEVEL_LOW);
            osal_irq_restore(irq_lock);
            return EXT_ERR_FAILURE;
        }
        if (cnt == 0) {
            uapi_tcxo_delay_us(HANDSHAKE_WAIT_TIME_US);
        }
        cnt++;
    }

    osal_irq_restore(irq_lock);
    return EXT_ERR_SUCCESS;
}

static ext_errno hcc_spi_send_msg(TD_CONST hcc_bus *bus, uint32_t msg_id)
{
    hcc_handler *hcc = (hcc_handler *)(bus->hcc);
    td_u16 len = sizeof(hcc_header) + sizeof(msg_id);
    uint8_t *buf = osal_kzalloc(sizeof(hcc_header) + sizeof(msg_id), OSAL_GFP_KERNEL);
    if (buf == NULL) {
        return EXT_ERR_MALLOC_FAILURE;
    }
    uint32_t *msg_buf = (uint32_t *)(buf + sizeof(hcc_header));
    *msg_buf = msg_id;
    hcc_transfer_param param = {
        .fc_flag = HCC_FC_WAIT,
        .queue_id = SYSCH_MSG_QUEUE,
        .service_type = SYSCHANNEL_SERVICE_TYPE_MSG,
        .sub_type = HCC_SPI_TYPE_MSG,
        .user_param = NULL,
    };

    return hcc_tx_data(hcc->channel_id, buf, len, &param);
}

static errcode_t hcc_spi_tx_data(hcc_handler *hcc, hcc_spi_priv_data *spi_priv,
    hcc_trans_queue *queue, uint16_t *remain_pkt_nums)
{
    errcode_t ret;
    hcc_unc_struc *unc_buf;
    uint8_t spi_head[SPI_MSG_LEN] = {0};
    spi_xfer_data_t data;
    uint32_t i;

    for (i = 0; i < *remain_pkt_nums; i++) {
        memset_s(spi_priv->tx_data_pool, SPI_BUF_MAX_2K, 0, SPI_BUF_MAX_2K);
        unc_buf = hcc_list_dequeue(&queue->queue_info);
        memcpy_s(spi_priv->tx_data_pool, unc_buf->length, unc_buf->buf, unc_buf->length);
        data.tx_buff = spi_head;
        data.tx_bytes = SPI_MSG_LEN;
        hcc_spi_packet_data(data.tx_buff, unc_buf->length, HCC_SPI_TYPE_DATA, queue->queue_id);
        ret = uapi_spi_master_write(spi_priv->spi_id, &data, SPI_WAIT_TIME_OUT);
        if (ret != EXT_ERR_SUCCESS) {
            oal_spi_log(BUS_LOG_ERR, "hcc_spi_send_data spi head fail, ret = 0x%x\r\n", ret);
            hcc_list_add_head(&queue->queue_info, unc_buf);
            break;
        }

        if (osal_event_read(&spi_priv->tx_event, TX_THREAD_START_EVENT, SPI_WAIT_EVENT_TIME,
            OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR) == 0) {
            hcc_adapt_mem_free(hcc, unc_buf);
            oal_spi_log(BUS_LOG_ERR, "read tx_event timeout\r\n");
            break;
        }

        data.tx_buff = spi_priv->tx_data_pool;
        data.tx_bytes = unc_buf->length;
        ret = uapi_spi_master_write(spi_priv->spi_id, &data, SPI_WAIT_TIME_OUT);
        if (ret != EXT_ERR_SUCCESS) {
            oal_spi_log(BUS_LOG_ERR, "hcc_spi_send_data Q[%d] fail, ret = 0x%x\r\n", queue->queue_id, ret);
            hcc_list_add_head(&queue->queue_info, unc_buf);
            break;
        }
        hcc_adapt_mem_free(hcc, unc_buf);
    }

    if (i == 0) {
        return EXT_ERR_FAILURE;
    }

    *remain_pkt_nums -= i;
    return EXT_ERR_SUCCESS;
}

static uint32_t hcc_spi_tx_proc(hcc_bus *bus, hcc_trans_queue *queue, uint16_t *remain_pkt_nums)
{
    hcc_handler *hcc = (hcc_handler *)(bus->hcc);
    hcc_spi_priv_data *spi_priv = (hcc_spi_priv_data *)bus->data;
    errcode_t ret;

    if (osal_atomic_read(&g_tx_state) == SPI_STATE_RX) {
        return EXT_ERR_FAILURE;
    }
    ret = hcc_handshake_for_tx(spi_priv);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    osal_atomic_set(&g_tx_state, SPI_STATE_TX);
    if (hcc == NULL) {
        return EXT_ERR_FAILURE;
    }

    if (OAL_UNLIKELY(hcc->hcc_state != HCC_ON)) {
        return EXT_ERR_FAILURE;
    }

    uapi_spi_set_tmod(spi_priv->spi_id, HAL_SPI_TRANS_MODE_TX, 0);
    ret = hcc_spi_tx_data(hcc, spi_priv, queue, remain_pkt_nums);
    osal_atomic_set(&g_tx_state, SPI_STATE_TX_OVER);
    uapi_gpio_set_dir(spi_priv->handshake_pin, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(spi_priv->handshake_pin, GPIO_LEVEL_LOW);

    return ret;
}

static bus_dev_ops g_spi_opt_ops = {
    .tx_proc = hcc_spi_tx_proc,
    .send_and_clear_msg = hcc_spi_send_msg,
    .reinit = hcc_spi_reinit,
};

static void spi_data_process(spi_packet_head *spi_head, hcc_spi_priv_data *spi_priv)
{
    uint16_t data_len = spi_head->len_l | (spi_head->len_h << BYTE_BIT_LEN);
    hcc_handler *hcc = (hcc_handler *)g_spi_bus->hcc;
    spi_xfer_data_t data = {0};
    hcc_unc_struc *unc_buf = NULL;
    if (spi_head->packet_info.queue_id >= hcc->que_max_cnt) {
        return;
    }

    memset_s(spi_priv->rx_data_pool, SPI_BUF_MAX_2K, 0, SPI_BUF_MAX_2K);
    data.rx_buff = spi_priv->rx_data_pool;
    data.rx_bytes = data_len;

    if (uapi_spi_master_read(spi_priv->spi_id, &data, SPI_WAIT_TIME_OUT) != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_spi_master_read err");
        return;
    }
    osal_atomic_set(&g_tx_state, SPI_STATE_RX_OVER);
    hcc_header *head = (hcc_header *)data.rx_buff;
    if (head->service_type == SYSCHANNEL_SERVICE_TYPE_MSG && head->sub_type == HCC_SPI_TYPE_MSG) {
        uint32_t msg_id = *(uint32_t *)(data.rx_buff + sizeof(hcc_header));
        if (msg_id == D2H_MSG_BSP_READY) {
            uapi_pin_set_pull(spi_priv->handshake_pin, PIN_PULL_TYPE_UP);
            osal_printk("=====bsp ready up===\r\n");
        }
        hcc_bus_call_rx_message(g_spi_bus, msg_id);
        return;
    }

    unc_buf = hcc_adapt_alloc_unc_buf(hcc, data_len, spi_head->packet_info.queue_id);
    if (unc_buf == NULL) {
        oal_spi_log(BUS_LOG_ERR, "hcc_adapt_alloc_unc_buf err");
        return;
    }
    if (memcpy_s(unc_buf->buf, data_len, data.rx_buff, data.rx_bytes) == EOK) {
        hcc_unc_buf_enqueue(hcc, spi_head->packet_info.queue_id, unc_buf);
        hcc_sched_transfer(hcc);
    }
}

static void spi_rx_process(hcc_spi_priv_data *spi_priv)
{
    uint8_t rx_data[SPI_MSG_LEN] = { 0 };
    spi_packet_head *spi_head = (spi_packet_head *)rx_data;
    spi_xfer_data_t data = {
        .rx_buff = rx_data,
        .rx_bytes = SPI_MSG_LEN,
    };

    uapi_spi_set_tmod(spi_priv->spi_id, HAL_SPI_TRANS_MODE_RX, 0);
    if (uapi_spi_master_read(spi_priv->spi_id, &data, SPI_WAIT_TIME_OUT) != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_spi_master_read err");
        return;
    }

    if (spi_head->packet_start != PACKET_START_SIGNAL) {
        oal_spi_log(BUS_LOG_ERR, "rx format start err start 0x%x", spi_head->packet_start);
        return;
    }

    if (spi_head->packet_info.type == HCC_SPI_TYPE_DATA) {
        spi_data_process(spi_head, spi_priv);
    }
}

static int32_t hcc_spi_rx_thread(osal_void *data)
{
    hcc_spi_priv_data *spi_priv = (hcc_spi_priv_data *)data;

    while (osal_kthread_should_stop() == 0) {
        osal_event_read(&spi_priv->rx_event, RX_THREAD_START_EVENT, OSAL_WAIT_FOREVER,
            OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR);
        /* start to read GPIO interrupt */
        spi_rx_process(spi_priv);
    }

    return EXT_ERR_SUCCESS;
}

static int32_t spi_start_rx_thread(hcc_spi_priv_data *spi_priv)
{
    osal_event_init(&spi_priv->rx_event);
    spi_priv->rx_task = osal_kthread_create(hcc_spi_rx_thread, spi_priv,
        "hcc_spi_rx_thread", 0x1000);
    if (spi_priv->rx_task == NULL) {
        return EXT_ERR_FAILURE;
    }
    osal_kthread_set_priority(spi_priv->rx_task, HCC_SPI_RX_ISR_PRIO);
    return EXT_ERR_SUCCESS;
}

static void spi_stop_rx_thread(hcc_spi_priv_data *spi_priv)
{
    osal_event_clear(&spi_priv->rx_event, RX_THREAD_START_EVENT);
    osal_kthread_destroy(spi_priv->rx_task, OSAL_TRUE);
    osal_event_destroy(&spi_priv->rx_event);
}

static void spi_gpio_isr(pin_t pin, uintptr_t param)
{
    hcc_spi_priv_data *spi_priv = (hcc_spi_priv_data *)g_spi_bus->data;
    if (spi_priv == NULL) {
        return;
    }
    int send_state = osal_atomic_read(&g_tx_state);
    unused(pin);
    unused(param);
    /* 唤醒RX线程 */
    if (send_state == SPI_STATE_TX) {
        osal_event_write(&spi_priv->tx_event, TX_THREAD_START_EVENT);
        return;
    }

    osal_atomic_set(&g_tx_state, SPI_STATE_RX);
    osal_event_write(&spi_priv->rx_event, RX_THREAD_START_EVENT);
}

static errcode_t spi_hw_init(hcc_spi_priv_data *spi_priv)
{
    errcode_t ret = ERRCODE_SUCC;
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };
    spi_dma_config_t dma_cfg = {
        .src_width = SPI_DMA_WIDTH,
        .dest_width = SPI_DMA_WIDTH,
        .burst_length = 0,
        .priority = 0
    };

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_BUS_CLK;
    config.freq_mhz = SPI_FREQ_MHZ;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 0;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;

    spi_priv->spi_id = SPI_BUS_0;
    spi_priv->spi_clk = GPIO_07;
    spi_priv->spi_cs = GPIO_10;
    spi_priv->spi_di = GPIO_11;
    spi_priv->spi_do = GPIO_09;
    spi_priv->handshake_pin = GPIO_08;
    spi_priv->pin_d2h = GPIO_12;
    ret |= uapi_pin_set_mode(spi_priv->spi_di, SPI_MASTER_PIN_MODE);
    ret |= uapi_pin_set_mode(spi_priv->spi_do, SPI_MASTER_PIN_MODE);
    ret |= uapi_pin_set_mode(spi_priv->spi_clk, SPI_MASTER_PIN_MODE);
    ret |= uapi_pin_set_mode(spi_priv->spi_cs, SPI_MASTER_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_pin_set_mode err, ret = 0x%x", ret);
        return ret;
    }

    ret = uapi_spi_init(spi_priv->spi_id, &config, &ext_config);
    if (ret != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_spi_init err, ret = 0x%x", ret);
        return ret;
    }
    ret |= uapi_dma_init();
    ret |= uapi_dma_open();
    ret |= uapi_spi_set_dma_mode(spi_priv->spi_id, true, &dma_cfg);
    return ret;
}

static void spi_hw_deinit(hcc_spi_priv_data *spi_priv)
{
    uapi_dma_close();
    uapi_dma_deinit();
    uapi_spi_deinit(spi_priv->spi_id);
}

static errcode_t spi_interrupt_register_gpio(hcc_spi_priv_data *spi_priv)
{
    errcode_t ret = ERRCODE_SUCC;
    ret |= uapi_gpio_set_dir(spi_priv->handshake_pin, GPIO_DIRECTION_OUTPUT);
    ret |= uapi_gpio_set_dir(spi_priv->pin_d2h, GPIO_DIRECTION_INPUT);
    if (ret != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_gpio_set_dir err, ret = 0x%x", ret);
        return ret;
    }

    ret = uapi_gpio_register_isr_func(spi_priv->pin_d2h, GPIO_INTERRUPT_RISING_EDGE, spi_gpio_isr);
    if (ret != ERRCODE_SUCC) {
        oal_spi_log(BUS_LOG_ERR, "uapi_gpio_register_isr_func err, ret = 0x%x", ret);
        return ret;
    }

    return ERRCODE_SUCC;
}

static void spi_interrupt_unregister_gpio(hcc_spi_priv_data *spi_priv)
{
    uapi_gpio_unregister_isr_func(spi_priv->pin_d2h);
}

static void hcc_spi_bus_init(hcc_bus *spi_bus)
{
    spi_bus->bus_type = HCC_BUS_SPI;
    spi_bus->max_trans_size =  SPI_BUF_MAX_2K;
    spi_bus->addr_align = HCC_SPI_DMA_BUFFADDR_ALIGN;
    spi_bus->len_align = HCC_SPI_DMA_DATA_ALIGN;
    spi_bus->max_assemble_cnt = 0;
    spi_bus->descr_align_bit = 0;
    spi_bus->state = 1;
}

static errcode_t hcc_spi_alloc_data_pool(hcc_spi_priv_data *spi_priv, td_u32 max_trans_size)
{
    spi_priv->tx_data_pool = osal_kzalloc_align(max_trans_size, OSAL_GFP_KERNEL, HCC_SPI_DMA_DATA_POOL_ALIGN);
    if (spi_priv->tx_data_pool == NULL) {
        return ERRCODE_MALLOC;
    }

    // 分配接收数据池
    spi_priv->rx_data_pool = osal_kzalloc_align(max_trans_size, OSAL_GFP_KERNEL, HCC_SPI_DMA_DATA_POOL_ALIGN);
    if (spi_priv->rx_data_pool == NULL) {
        osal_kfree(spi_priv->tx_data_pool);
        return ERRCODE_MALLOC;
    }

    return ERRCODE_SUCC;
}

static void hcc_spi_free_data_pool(hcc_spi_priv_data *spi_priv)
{
    if (spi_priv->tx_data_pool != NULL) {
        osal_kfree(spi_priv->tx_data_pool);
    }
    if (spi_priv->rx_data_pool != NULL) {
        osal_kfree(spi_priv->rx_data_pool);
    }
}

hcc_bus *hcc_adapt_spi_load(hcc_handler *hcc)
{
    errcode_t ret;
    hcc_bus *spi_bus = NULL;

    // 分配私有数据结构
    hcc_spi_priv_data *spi_priv = (hcc_spi_priv_data *)osal_kzalloc(sizeof(hcc_spi_priv_data), OSAL_GFP_KERNEL);
    if (spi_priv == NULL) {
        return NULL;
    }

    // 分配总线结构
    spi_bus = hcc_alloc_bus();
    if (spi_bus == NULL) {
        goto alloc_spi_bus_fail;
    }

    // 初始化SPI总线
    hcc_spi_bus_init(spi_bus);
    hcc_set_bus_ops(spi_bus, &g_spi_opt_ops);

    // 初始化硬件
    ret = spi_hw_init(spi_priv);
    if (ret != ERRCODE_SUCC) {
        goto spi_hw_init_fail;
    }

    // 初始化传输状态
    osal_atomic_set(&g_tx_state, 0);

    // 分配数据池
    ret = hcc_spi_alloc_data_pool(spi_priv, spi_bus->max_trans_size);
    if (ret != ERRCODE_SUCC) {
        goto alloc_data_pool_fail;
    }

    // 启动接收线程
    spi_start_rx_thread(spi_priv);
    osal_event_init(&spi_priv->tx_event);

    // 注册中断
    ret = spi_interrupt_register_gpio(spi_priv);
    if (ret != ERRCODE_SUCC) {
        goto interrupt_register_fail;
    }

    // 设置总线数据指针
    spi_bus->data = (void *)spi_priv;
    g_spi_bus = spi_bus;

    return g_spi_bus;

interrupt_register_fail:
    osal_event_clear(&spi_priv->tx_event, TX_THREAD_START_EVENT);
    osal_event_destroy(&spi_priv->tx_event);
    spi_stop_rx_thread(spi_priv);
    hcc_spi_free_data_pool(spi_priv);
alloc_data_pool_fail:
    spi_hw_deinit(spi_priv);
spi_hw_init_fail:
    hcc_free_bus(spi_bus);
alloc_spi_bus_fail:
    osal_kfree(spi_priv);

    return NULL;
}

void hcc_adapt_spi_unload(void)
{
    hcc_spi_priv_data *spi_priv = (hcc_spi_priv_data *)g_spi_bus->data;
    spi_interrupt_unregister_gpio(spi_priv);

    osal_event_clear(&spi_priv->tx_event, TX_THREAD_START_EVENT);
    osal_event_destroy(&spi_priv->tx_event);

    spi_stop_rx_thread(spi_priv);
    hcc_spi_free_data_pool(spi_priv);
    spi_hw_deinit(spi_priv);
    osal_kfree(spi_priv);
    hcc_free_bus(g_spi_bus);
    g_spi_bus = NULL;
}
