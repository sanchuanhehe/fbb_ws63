/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: host hcc spi driver os adapt.
 * Author: plat
 * Create: 2024-12-12
 */

#ifndef __HCC_ADAPT_SPI_H__
#define __HCC_ADAPT_SPI_H__

#include "td_base.h"
#include "td_type.h"
#include "hcc_bus.h"
#include "hcc_queue.h"

#define SPI_BUS_CLK                    80000000
#define SPI_FREQ_MHZ                   10

#define HANDSHAKE_CNT 2
#define HANDSHAKE_WAIT_TIME_US 5

#define HCC_SPI_DMA_DATA_POOL_ALIGN 32

#define SPI_MODULE_NAME "[SPI]"
#define oal_spi_log(loglevel, fmt, arg...) \
    do { \
        if (loglevel <= BUS_LOG_INFO) { \
            printf("%s%s" fmt"[%s:%d]\r\n", SPI_MODULE_NAME, g_loglevel[loglevel], ##arg, __FUNCTION__, __LINE__); \
        } \
    } while (0)

typedef enum {
    HCC_SPI_TYPE_DATA,
    HCC_SPI_TYPE_MSG,
    HCC_SPI_PKT_TYPE_MAX,
} hcc_spi_packet_type;

typedef struct spi_packet_head_t {
    uint8_t packet_start;
    struct {
        uint8_t type : 3;
        uint8_t queue_id : 5;
    } packet_info;
    uint8_t len_h;
    uint8_t len_l;
} spi_packet_head;

typedef struct hcc_spi_priv_data_t {
    spi_bus_t spi_id;
    pin_t spi_di;
    pin_t spi_do;
    pin_t spi_clk;
    pin_t spi_cs;
    pin_t handshake_pin;
    pin_t pin_d2h;
    osal_event rx_event;
    osal_event tx_event;
    osal_task *rx_task;
    uint8_t *tx_data_pool;
    uint8_t *rx_data_pool;
} hcc_spi_priv_data;

hcc_bus *hcc_adapt_spi_load(hcc_handler *hcc);
void hcc_adapt_spi_unload(void);
#endif