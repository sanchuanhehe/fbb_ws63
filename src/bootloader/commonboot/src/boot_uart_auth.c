/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: uart auth
 *
 * Create: 2023-01-09
 */

#include "boot_uart_auth.h"
#include "boot_crc.h"
#include "boot_delay.h"
#include "boot_debug.h"
#include "boot_watchdog.h"
#include "securec.h"
#include "boot_cmd_loop.h"


#define UART_RX_DELAY_MS   100 /* 100ms */
#define US_PER_MS          1000
#define UTM_DISABLE        1
#define SDC_DISABLE        0
#define UART_ABNORMAL_MAX  32
#define RANDOM_RETRY_MAX   100
#define CHECKSUM_SIZE      2
#define UART_CFG_LEN       8
#define INTER_DATA_LEN     0x12
#define ACK_LEN            0x0C
#define ACK_SUCCESS        0x5A
#define ACK_FAILURE        0xA5

typedef enum {
    RX_STATUS_WAIT_START_0,
    RX_STATUS_WAIT_START_1,
    RX_STATUS_WAIT_START_2,
    RX_STATUS_WAIT_START_3,
    RX_STATUS_WAIT_SIZE_0,
    RX_STATUS_WAIT_SIZE_1,
    RX_STATUS_WAIT_TYPE,
    RX_STATUS_WAIT_PAD,
    RX_STATUS_WAIT_DATA,
    RX_STATUS_WAIT_CS_0,
    RX_STATUS_WAIT_CS_1,
} rx_status;

uart_param_stru g_uart_param = { 0 };
static uint32_t g_uart_int_type = UART_TYPE_ROMBOOT_HANDSHAKE;
static uint8_t g_encrypt_flag = 0;

static uint32_t uart_frame_rx(uart_ctx *ctx, uint32_t first_byte_timeout_ms);
static uint32_t uart_frame_head_rx(uart_ctx *ctx, uint32_t first_byte_timeout_ms);
static uint32_t uart_frame_data_rx(uart_ctx *ctx);
static uint32_t uart_wait_usr_interrupt(uart_ctx *ctx, uint32_t interrupt_timeout_ms);
static uint32_t uart_usr_interrupt_verify(uart_ctx *ctx);

static uint32_t uart_frame_rx(uart_ctx *ctx, uint32_t first_byte_timeout_ms)
{
    packet_data_info *packet = &ctx->packet;
    packet_data_head *head = &packet->head;
    uint32_t ret = ERRCODE_FAIL;

    /* 复位接收状态 */
    ctx->status = RX_STATUS_WAIT_START_0;
    ctx->received = 0;
    (void)memset_s(packet, sizeof(packet_data_info), 0, sizeof(packet_data_info));

    ret = uart_frame_head_rx(ctx, first_byte_timeout_ms);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = uart_frame_data_rx(ctx);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    uint32_t cs = crc16_ccitt(0, (uint8_t *)packet, head->packet_size - CHECKSUM_SIZE);
    if (cs == packet->check_sum) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

static uint32_t uart_frame_head_rx(uart_ctx *ctx, uint32_t first_byte_timeout_ms)
{
    uint8_t ch;
    bool reset_flag = false;
    uint16_t rcv = 0;
    uint32_t ret = ERRCODE_FAIL;

    packet_data_head *head = &ctx->packet.head;
    uint8_t *payload = (uint8_t *)head;

    while (rcv <= UART_ABNORMAL_MAX) {
        if (ctx->status == RX_STATUS_WAIT_START_0) {
            ret = serial_getc_timeout(first_byte_timeout_ms * US_PER_MS, &ch);
        } else {
            ret = serial_getc_timeout(UART_RX_DELAY_MS * US_PER_MS, &ch);
        }
        if (ret != ERRCODE_SUCC) {
            return (ctx->status == RX_STATUS_WAIT_START_0) ? UART_FIRST_CHAR_TIMEOUT : ERRCODE_FAIL;
        }
        rcv++;
        if (reset_flag == true) {
            reset_flag = false;
            head->start_flag = 0;
            ctx->status = RX_STATUS_WAIT_START_0;
        }
        if (ctx->status <= RX_STATUS_WAIT_START_3) {
            uint32_t start_flag = UART_PACKET_START_FLAG;
            uint8_t *start_byte = (uint8_t *)&start_flag;
            if (ch == start_byte[ctx->status]) {
                payload[ctx->status] = ch;
                ctx->status++;
                continue;
            } else if (ch == 0xEF) {
                payload[RX_STATUS_WAIT_START_0] = ch;
                ctx->status = RX_STATUS_WAIT_START_1;
                continue;
            }
            reset_flag = true;
            continue;
        } else {
            payload[ctx->status] = ch;
            if (ctx->status >= RX_STATUS_WAIT_SIZE_1 && (head->packet_size > UART_PACKET_PAYLOAD_MAX)) {
                reset_flag = true;
                continue;
            }
            ctx->status++;
            if (ctx->status >= RX_STATUS_WAIT_DATA) {
                return ERRCODE_SUCC;
            }
        }
        boot_wdt_kick();
    }
    return ERRCODE_FAIL;
}

static uint32_t uart_frame_data_rx(uart_ctx *ctx)
{
    uint8_t ch;
    ctx->received = 0;
    uint32_t ret = ERRCODE_FAIL;
    packet_data_head *head = &ctx->packet.head;
    uint8_t *payload = ctx->packet.payload;

    while (ctx->received < (head->packet_size - sizeof(packet_data_head))) {
        ret = serial_getc_timeout(UART_RX_DELAY_MS * US_PER_MS, &ch);
        if (ret == ERRCODE_SUCC) {
            payload[ctx->received++] = ch;
            continue;
        }
        return ERRCODE_FAIL;
    }
    ctx->packet.check_sum = (payload[head->packet_size - 9] << 8) | payload[head->packet_size - 10]; /* 8,9,10: sub */
    payload[head->packet_size - 9] = 0;  /* 9: sub 9 */
    payload[head->packet_size - 10] = 0; /* 10: sub 10 */

    if (ctx->received == (head->packet_size - sizeof(packet_data_head))) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

static void uart_ack(uart_ctx *ctx)
{
    packet_data_head *head = &ctx->packet.head;

    head->start_flag = UART_PACKET_START_FLAG;
    head->type = UART_TYPE_ACK;
    head->pad = (uint8_t)(~(UART_TYPE_ACK));
    head->packet_size = ACK_LEN;
    ctx->packet.payload[0] = ACK_SUCCESS;
    ctx->packet.payload[1] = g_encrypt_flag;
    ctx->packet.check_sum = crc16_ccitt(0, (uint8_t *)&(ctx->packet), head->packet_size - CHECKSUM_SIZE);

    serial_put_buf ((const char *)&(ctx->packet), (int)(head->packet_size - CHECKSUM_SIZE));
    serial_put_buf ((const char *)&(ctx->packet.check_sum), CHECKSUM_SIZE);
}

uint32_t uart_process(uint32_t interrupt_timeout_ms)
{
    uart_ctx uart_ctx = { 0 };
    set_boot_status(HI_BOOT_WAIT_UART_INTERRUPT);
    if (uart_wait_usr_interrupt(&uart_ctx, interrupt_timeout_ms) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    uart_ack(&uart_ctx);
    uart_ack(&uart_ctx);
    uart_ack(&uart_ctx);

    /* wait transmit ok */
    mdelay(5); /* 5: wait 5 ms */

    return ERRCODE_SUCC;
}

static uint32_t uart_wait_usr_interrupt(uart_ctx *ctx, uint32_t interrupt_timeout_ms)
{
    volatile uint32_t ret = ERRCODE_FAIL;
    ret = uart_frame_rx(ctx, interrupt_timeout_ms);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return uart_usr_interrupt_verify(ctx);
}

static uint32_t uart_usr_interrupt_verify(uart_ctx *ctx)
{
    packet_data_head *head = &ctx->packet.head;
    if ((head->packet_size == INTER_DATA_LEN) && (g_uart_int_type == head->type)) {
        if (head->type == UART_TYPE_ROMBOOT_HANDSHAKE) {
            (void)memcpy_s((void *)&g_uart_param, sizeof(uart_param_stru),
                (void *)(ctx->packet.payload), UART_CFG_LEN);
        }
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
uart_param_stru* uart_baudrate_rcv(const uart_ctx *ctx)
{
    uart_ctx uart = { 0 };
    packet_data_head *head = &(((uart_ctx*)ctx)->packet.head);

    if ((head->packet_size != INTER_DATA_LEN) || (head->type != CMD_SET_BUADRATE)) {
        return NULL;
    }

    (void)memcpy_s((void *)&g_uart_param, sizeof(uart_param_stru), (void *)(ctx->packet.payload), UART_CFG_LEN);
    uart_ack(&uart);
    mdelay(5); /* 5: wait 5 ms for transmit ok */
    return &g_uart_param;
}
#endif